/****************************************************************************
 * arch/loongarch64/src/common/la64_exception.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <nuttx/debug.h>
#include <syscall.h>

#include <arch/irq.h>
#include <nuttx/arch.h>
#include <nuttx/sched.h>
#include <arch/csr.h>
#include <arch/barriers.h>

#include "sched/sched.h"
#include "signal/signal.h"
#include "la64_internal.h"
#include "la64_percpu.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

extern void handle_ade(void);
extern void handle_ale(void);
extern void handle_sys(void);
extern void handle_bp(void);
extern void handle_ri(void);
extern void handle_fpu(void);
extern void handle_fpe(void);
extern void handle_reserved(void);
extern void handle_watch(void);
extern void handle_vint(void);
extern void handle_tlb_refill(void);
extern char except_vec_cex;

uintptr_t g_exception_table[EXCCODE_INT_START] =
{
  [0 ... EXCCODE_INT_START - 1] = (uintptr_t)handle_reserved,

  [EXCCODE_ADE]  = (uintptr_t)handle_ade,
  [EXCCODE_ALE]  = (uintptr_t)handle_ale,
  [EXCCODE_SYS]  = (uintptr_t)handle_sys,
  [EXCCODE_BP]   = (uintptr_t)handle_bp,
  [EXCCODE_INE]  = (uintptr_t)handle_ri,
  [EXCCODE_FPDIS]= (uintptr_t)handle_fpu,
  [EXCCODE_FPE]  = (uintptr_t)handle_fpe,
  [EXCCODE_WATCH]= (uintptr_t)handle_watch,
};

#define ECFG_VS_128B        (7 << CSR_ECFG_VS_SHIFT)
#define VECSIZE             0x200
#define SZ_4K               0x1000
#define SZ_16K              0x4000
#define SZ_64K              0x10000

unsigned long eentry;
unsigned long tlbrentry;
long exception_handlers[VECSIZE * 128 / sizeof(long)] aligned_data(SZ_64K);

static void configure_exception_vector(void)
{
  eentry    = (unsigned long)exception_handlers;
  tlbrentry = (unsigned long)exception_handlers + 80*VECSIZE;

  csr_write64(eentry, LA_CSR_EENTRY);
  csr_write64(eentry, LA_CSR_MERRENTRY);
  csr_write64(tlbrentry, LA_CSR_TLBRENTRY);
}

void set_handler(unsigned long offset, void *addr, unsigned long size)
{
  memcpy((void *)(eentry + offset), addr, size);
  UP_DSB();
  UP_ISB();
}

void set_merr_handler(unsigned long offset, void *addr, unsigned long size)
{
  unsigned long uncac_eentry = TO_UNCAC(eentry);

  memcpy((void *)(uncac_eentry + offset), addr, size);
}

void la64_exception_attach(void)
{
  irq_attach(LA_IRQ_SWI0, la64_swint, NULL);
  irq_attach(LA_IRQ_SWI1, la64_swint, NULL);
}

/****************************************************************************
 * Name: trap_init
 *
 * Description:
 *
 ****************************************************************************/

void trap_init(void)
{
  uint32_t ecfg, i;
  static int run_once;

  ecfg = csr_read32(LA_CSR_ECFG);
  ecfg &= ~CSR_ECFG_VS;
  ecfg |= ECFG_VS_128B;
  csr_write32(ecfg, LA_CSR_ECFG);

#if 0
  csr_write64(0x9000000000000000, LA_CSR_MERRENTRY);
  csr_write64(0x9000000000001000, LA_CSR_TLBRENTRY);
  csr_write64(0x9000000000002000, LA_CSR_EENTRY);
#else
  configure_exception_vector();
#endif

  for (i = 0; i < 64; i++)
    set_handler(i*VECSIZE, handle_reserved, VECSIZE);

  if (!run_once)
  {
    memcpy((void *)tlbrentry, handle_tlb_refill, 0x80);

    for (i = EXCCODE_TLBL; i <= EXCCODE_TLBPE; i++)
      set_handler(i * VECSIZE, g_exception_table[i], VECSIZE);

    run_once++;
  }

  for (i = EXCCODE_INT_START; i < EXCCODE_INT_END; i++)
    set_handler(i * VECSIZE, handle_vint, VECSIZE);

  for (i = EXCCODE_ADE; i < EXCCODE_BTDIS; i++)
    set_handler(i * VECSIZE, g_exception_table[i], VECSIZE);

  set_merr_handler(0x0, &except_vec_cex, 0x80);

  UP_DSB();
  UP_ISB();
}

typedef uintptr_t (*syscall_t)(unsigned int, ...);

/****************************************************************************
 * Name: la64_syscall_dispatch
 *
 * Description:
 *   Call the stub function corresponding to the system call.
 *   NOTE the non-standard parameter passing:
 *     A0 = SYS_ call number
 *     A1 = param0
 *     A2 = param1
 *     A3 = param2
 *     A4 = param3
 *     A5 = param4
 *     A6 = param5
 *     A7 = SYS_ call number
 *     A7 = context (aka SP)
 *
 ****************************************************************************/

#if 0
//uintptr_t la64_syscall_dispatch(unsigned int nbr, uint64_t arg0, uint64_t arg1,
//uintptr_t do_syscall(uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3,
 //   uint64_t arg4, uint64_t arg5, uint64_t *context, unsigned int nbr)
uintptr_t do_syscall(void *context)
{
    struct tcb_s *rtcb = this_task();
#if 0
    register long a7 asm("a7") = (long)(nbr);
    register long a0 asm("a0") = (long)(arg0);
    register long a1 asm("a1") = (long)(arg1);
    register long a2 asm("a2") = (long)(arg2);
    register long a3 asm("a3") = (long)(arg3);
    register long a4 asm("a4") = (long)(arg4);
    register long a5 asm("a5") = (long)(arg5);
#else
    uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
#endif
    syscall_t syscall_fn;
    uintptr_t ret;
    uintptr_t *regs = (uintptr_t *)context;

    a0 = regs[REG_A0];
    a1 = regs[REG_A1];
    a2 = regs[REG_A2];
    a3 = regs[REG_A3];
    a4 = regs[REG_A4];
    a5 = regs[REG_A5];
    a6 = regs[REG_A6];
    a7 = regs[REG_A7];

  syslog(LOG_INFO, "TCB(%p) ERA=0x%16lx, SP=0x%16lx\n", rtcb, rtcb->xcp.regs[REG_ERA], rtcb->xcp.regs[REG_SP]);
  syslog(LOG_EMERG, "regs(%p) ERA = 0x%016lx, SP = 0x%016lx,\n", regs, regs[REG_ERA], regs[REG_SP]);
  syslog(LOG_EMERG, "a0 = 0x%016lx, a1 = 0x%016lx, a2 = 0x%016lx\n", a0, a1, a2);
  syslog(LOG_EMERG, "a3 = 0x%016lx, a4 = 0x%016lx, a5 = 0x%016lx\n", a3, a4, a5);
  syslog(LOG_EMERG, "a6 = 0x%016lx, a7 = 0x%016lx,\n", a6, a7);

    //syslog(LOG_EMERG, "SYSCALL! ESTAT = 0x%x, nbr = 0x%x\n", csr_read32(LA_CSR_ESTAT), nbr);

    /* Valid system call ? */
    if (a7 > SYS_maxsyscall)
    {
        /* Nope, get out */
        return -ENOSYS;
    }

    syslog(LOG_EMERG, "1\n");

    regs[REG_ERA] += 4;
    syslog(LOG_EMERG, "2\n");

    /* Set the user register context to TCB */
    rtcb->xcp.sregs = context;
    /* Indicate that we are in a syscall handler */
    rtcb->flags |= TCB_FLAG_SYSCALL;
    /* Offset a0 to account for the reserved syscall */
    a7 -= CONFIG_SYS_RESERVED;
    //nbr -= CONFIG_SYS_RESERVED;
    /* Find the system call from the lookup table */
    syscall_fn = (syscall_t)g_stublookup[a7];
    /* Run the system call, save return value locally */
    syslog(LOG_EMERG, "3\n");
    ret = syscall_fn(a0, a1, a2, a3, a4, a5);
    syslog(LOG_EMERG, "4\n");

    /* System call is now done */
    rtcb->flags &= ~TCB_FLAG_SYSCALL;

    /* Update percpu_s regs */
    //la64_set_current_regs(cpu, NULL);

    /* Unmask any pending signals now */
    nxsig_unmask_pendingsignal();

    return ret;
}
#else
uintptr_t do_syscall(void *context)
{
    struct tcb_s *rtcb = this_task();
    uintptr_t *regs = (uintptr_t *)context;
    uint64_t a0, a1, a2, a3, a4, a5, a6, a7;

    a0 = regs[REG_A0];
    a1 = regs[REG_A1];
    a2 = regs[REG_A2];
    a3 = regs[REG_A3];
    a4 = regs[REG_A4];
    a5 = regs[REG_A5];
    a6 = regs[REG_A6];
    a7 = regs[REG_A7];

    /* 1. 检查是否为上下文切换系统调用 (SYS_switchcontext) */
    if (a7 == SYS_switch_context)
    {
        uint64_t **saveregs = (uint64_t **)a0;
        struct tcb_s *wtcb = (struct tcb_s *)a1;

        /* 保存当前任务现场指针 */
        if (saveregs != NULL)
        {
            *saveregs = regs;
        }

        /* 更新当前执行 TCB */
        /* 返回新任务 (AppBringUp) 的 xcp.regs 给汇编层的 RESTORE 宏 */
        if (wtcb != NULL && wtcb->xcp.regs != NULL)
        {
            return (uintptr_t)wtcb->xcp.regs;
        }
        return (uintptr_t)a1;
    }

    /* 2. 检查是否为恢复上下文系统调用 (SYS_restore_context) */
    if (a7 == SYS_restore_context)
    {
        struct tcb_s *wtcb = (struct tcb_s *)a0;
        if (wtcb != NULL && wtcb->xcp.regs != NULL)
        {
            return (uintptr_t)wtcb->xcp.regs;
        }
        return (uintptr_t)a0;
    }

    /* 3. 普通系统调用继续 PC 步进 (+4) */
    regs[REG_ERA] += 4;

    if (a7 > SYS_maxsyscall)
    {
        return -ENOSYS;
    }

    rtcb->xcp.sregs = context;
    rtcb->flags |= TCB_FLAG_SYSCALL;

    a7 -= CONFIG_SYS_RESERVED;
    syscall_t syscall_fn = (syscall_t)g_stublookup[a7];

    uintptr_t ret = syscall_fn(a0, a1, a2, a3, a4, a5);

    rtcb->flags &= ~TCB_FLAG_SYSCALL;

    /* 普通 syscall 将返回值写入 a0 */
    regs[REG_A0] = (uint64_t)ret;

    /* 返回原 context 指针继续恢复执行旧任务 */
    return (uintptr_t)regs;
}
#endif

/****************************************************************************
 * Name: la64_regs_dump
 *
 * Description:
 *
 ****************************************************************************/

static void la64_regs_dump(uint64_t *regs)
{
  int i;

  _alert("========== NuttX Core Register Dump ==========\n");
  for (i = 0; i < 32; i+= 4)
    _alert("R%d-R%d: 0x%016lx 0x%016lx 0x%016lx 0x%016lx\n",
        i, i+3, regs[i], regs[i+1], regs[i+2], regs[i+3]);

  _alert("ERA  : 0x%016lx\n", regs[REG_ERA]);
  _alert("CRMD : 0x%016lx\n", regs[REG_CRMD]);
  _alert("PRMD : 0x%016lx\n", regs[REG_PRMD]);
  _alert("ESTAT: 0x%016lx\n", regs[REG_ESTAT]);
  _alert("BADV : 0x%016lx\n", regs[REG_BADV]);
}

void do_ade(uint64_t *regs, bool user)
{
  _err("ADE\n");

  la64_regs_dump(regs);

  PANIC();
}

void do_ale(uint64_t *regs, bool user)
{
  _err("ALE\n");
  la64_regs_dump(regs);
  PANIC();
}

void do_bp(uint64_t *regs, bool user)
{
  _err("BP: Breakpoint at ERA=%p\n", regs[REG_ERA]);
  regs[REG_ERA] += 4;
}

void do_ri(uint64_t *regs, bool user)
{
  _err("INE: Illegal instruction at ERA=%p\n", regs[REG_ERA]);

  if (!user)
    {
      PANIC();
    }
  else
    {
      //nxsched_abort_task(this_task());
    PANIC();
    }
}

void do_reserved(uint64_t *regs)
{
  _err("Reserved exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}

void cache_parity_error(void)
{
  uint64_t merrera, merrctl;

  __asm__ __volatile__("csrrd %0, 0x30" : "=r"(merrera));
  __asm__ __volatile__("csrrd %0, 0x31" : "=r"(merrctl));

  _err("Cache error: MERRERA=%016llx MERRCTL=%016llx\n", merrera, merrctl);
  PANIC();
}

void do_fpe(uint64_t *regs)
{
  _err("FPE exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}

void do_fpu(uint64_t *regs)
{
  _err("FPU exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}
void do_lsx(uint64_t *regs)
{
  _err("LSX exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}
void do_lasx(uint64_t *regs)
{
  _err("LASX exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}
void do_lbt(uint64_t *regs)
{
  _err("LBT exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}
void do_watch(uint64_t *regs)
{
  _err("WATCH exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}

/****************************************************************************
 * Name: la64_exception_dispatch
 *
 * Description: IS[0]
 *
 ****************************************************************************/

uint64_t *la64_exception_handler(uint64_t *regs)
{
#if 1
  uint32_t estat = csr_read32(LA_CSR_ESTAT);
  uint32_t crmd = csr_read32(LA_CSR_CRMD);
  uint32_t prmd = csr_read32(LA_CSR_PRMD);
  uint32_t ecode = (estat & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;
#else
  uint32_t estat,crmd,prmd,ecode;
  estat = crmd = prmd = ecode =0x0;
#endif

  syslog(LOG_EMERG, "Exception! ESTAT = 0x%lx, ECODE = 0x%lx, CRMD = 0x%lx, PRMD = 0x%lx\n", estat, ecode, crmd, prmd);

  if (ecode == 0xb)
  {
    unsigned int nbr = (unsigned int)regs[REG_A7];

    if (nbr == SYS_switch_context)
    {
      uint64_t **saveregs = (uint64_t **)regs[REG_A0];
      uintptr_t arg1 = (uintptr_t)regs[REG_A1];
      uint64_t *next_regs = NULL;

      if (saveregs != NULL)
      {
        *saveregs = regs;
      }

      struct tcb_s *wtcb  = (struct tcb_s *)arg1;
      if (wtcb != NULL && wtcb->xcp.regs != NULL)
      {
        next_regs = (uint64_t *)wtcb->xcp.regs;
      }
      else
      {
        next_regs = (uint64_t *)arg1;
      }

      syslog(LOG_EMERG, "Switch Context: current regs=%p, next regs=%p\r\n",
          regs, next_regs);

      DEBUGASSERT(next_regs != NULL);

      return next_regs;
    }
    else if (nbr == SYS_restore_context)
    {
      struct tcb_s *wtcb = (struct tcb_s *)regs[REG_A0];
      if (wtcb != NULL && wtcb->xcp.regs !=NULL)
      {
        return (uint64_t *)wtcb->xcp.regs;
      }
      return (uint64_t *)regs[REG_A0];
    }

    regs[REG_ERA] += 4;

    uintptr_t ret = la64_syscall_dispatch(nbr,
        regs[REG_A0], regs[REG_A1], regs[REG_A2], regs[REG_A3],
        regs[REG_A4], regs[REG_A5], regs);

    regs[REG_A0] = (uint64_t)ret;
    return regs;
  }

  syslog(LOG_EMERG, "Unhandled Exception! ECODE = 0x%x, ERA = 0x%16lx\n",
      ecode, regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();

  return regs;
}

/****************************************************************************
 * Name: do_vint
 *
 * Description: IS[1~13]
 *
 ****************************************************************************/
uint64_t do_vint(uint64_t *regs)
//uint64_t *la64_vint_handler(uint64_t *regs)
{
  struct la64_percpu_s *percpu = la64_my_percpu();
  percpu->cur_regs = (uintptr_t)regs;

  uint32_t estat = csr_read32(LA_CSR_ESTAT);
  uint32_t irq = (estat & 0x3fff);

  up_set_interrupt_context(true);

  syslog(LOG_EMERG, "VINT! ESTAT = 0x%x, irq = 0x%x\n", estat, irq);

  if ((estat & CSR_ESTAT_IS) == 0) {
    up_set_interrupt_context(false);
    percpu->cur_regs = 0;
    return regs;
  }

  if (irq & (1 << 11)) {
    csr_write32(0x1, LA_CSR_TINTCLR);
    irq = LA_LOC_IRQ_BASE + INT_TI; /* TI */
  } else if (irq & 0x3fc) {
    int hwi = __builtin_ctz(irq & 0x3fc) - 2;
    irq = LA_LOC_IRQ_BASE + hwi;
  } else if (irq & 0x3) {
    int swi = __builtin_ctz(irq & 0x3);
    csr_write32(1 << swi, LA_CSR_ESTAT);
    irq = LA_LOC_IRQ_BASE + swi;
  } else {
    up_set_interrupt_context(false);
    percpu->cur_regs = 0;
    return regs;
  }

  regs = la64_doirq(irq, regs);

  percpu->cur_regs = 0;

  up_set_interrupt_context(false);

  syslog(LOG_EMERG, "VINT END! crmd = 0x%x, ecfg = 0x\n", csr_read32(LA_CSR_CRMD), csr_read32(LA_CSR_ECFG));

  return regs;
}

#ifdef CONFIG_ARCH_FPU
/****************************************************************************
 * Name: la64_fpu_lazy_load
 *
 * Description:
 *
 ****************************************************************************/
void la64_fpu_disable_dispatch(uint64_t *regs)
{
  sderr("========== NuttX Core Register Dump ==========\n");
#if 0
    int cpu = up_cpu_index();
    struct tcb_s *curr_tcb  = (struct tcb_s *)g_percpu_ctx[cpu].current_tcb;
    struct tcb_s *owner_tcb = (struct tcb_s *)g_percpu_ctx[cpu].current_fpu_owner;
    uint32_t euen;

    /* The task that triggers the FPU exception must be the current tcb */
    DEBUGASSERT(curr_tcb != NULL);

    euen = read_csr_euen();
    if (!(euen & 1))
        write_csr_euen(euen | 1);

    if (owner_tcb != curr_tcb)
    {
        /* Physical FPU is being consumed by other tasks (the previous task) */
        if (owner_tcb != NULL)
        {
            /* FPU of previous task is tripped and writen to its tcb backup area */
            la64_fpu_save_to_tcb(&owner_tcb->xcp.fpu);

            /* To re-trigger the lazy flow next run, clear the FPU flag of previous task */
            if (owner_tcb->xcp.regs != NULL)
            {
                owner_tcb->xcp.regs[REG_EUEN] &= ~1;
            }
        }

        la64_fpu_restore_from_tcb(&curr_tcb->xcp.fpu);

        g_percpu_ctx[cpu].current_fpu_owner = (uint64_t)curr_tcb;
    }

    /* The EUEN value of stack is forcibly set to 1. When the assembly layer subsequently executes
     * restore_all and ertn, the hardware allows the FPU to run.
     */
    regs[REG_EUEN] |= 1;
#endif
}
#endif

#if 1
/****************************************************************************
 * Name: la64_fatal_handler
 *
 * Description:
 *
 ****************************************************************************/

void la64_fatal_handler(uint64_t *regs)
{
  FAR struct la64_percpu_s *percpu = la64_my_percpu();

  uint64_t estat = regs[REG_ESTAT];
  uint64_t era   = regs[REG_ERA];
  uint64_t badv  = csr_read64(LA_CSR_BADV);
  uint64_t ecode = (estat & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;

  percpu->cur_regs = (uintptr_t)regs;

  syslog(LOG_EMERG, "\n==================================================\n");
  syslog(LOG_EMERG, "FATAL HARDWARE EXCEPTION: Machine Check / Bus Error\n");
  syslog(LOG_EMERG, "==================================================\n");
#if CONFIG_SMP_NCPUS > 1
  syslog(LOG_EMERG, "  CPU:    %" PRIu32 "\n", percpu->cpu);
#endif
  syslog(LOG_EMERG, "  ESTAT:  0x%016" PRIx64 " [Ecode: 0x%02" PRIx32 "]\n", estat, ecode);
  syslog(LOG_EMERG, "  ERA:    0x%016" PRIx64 " (Fault PC)\n", era);
  syslog(LOG_EMERG, "  BADV:   0x%016" PRIx64 " (Fault Address)\n", badv);
  syslog(LOG_EMERG, "--------------------------------------------------\n");

  up_dump_register(regs);

  PANIC_WITH_REGS("Fatal Machine Check / Bus Error!", regs);

  for (;;);
}
#endif

#if 0
/****************************************************************************
 * Name: up_interrupt_context
 *
 * Description:
 *   Return true is we are currently executing in the interrupt handler cotext.
 *
 ****************************************************************************/

bool up_interrupt_context(void)
{
#ifdef CONFIG_SMP
    int cpu = up_cpu_index();
#else
    int cpu = 0;
#endif

    return g_percpu_ctx[cpu].current_regs != 0;
}

#endif

