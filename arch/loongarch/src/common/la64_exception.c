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
extern dispatch_syscall(uint64_t arg0, uint64_t arg1, uint64_t arg2,
    uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t nbr, uint64_t *cxt);

void *g_exception_table[EXCCODE_INT_START] =
{
  [0 ... EXCCODE_INT_START - 1] = handle_reserved,

  [EXCCODE_ADE]  = handle_ade,
  [EXCCODE_ALE]  = handle_ale,
  [EXCCODE_SYS]  = handle_sys,
  [EXCCODE_BP]   = handle_bp,
  [EXCCODE_INE]  = handle_ri,
  [EXCCODE_FPDIS]= handle_fpu,
  [EXCCODE_FPE]  = handle_fpe,
  [EXCCODE_WATCH]= handle_watch,
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

  syslog(LOG_EMERG, "eentry = 0x%016lx, tlbrentry = 0x%016lx\n", eentry, tlbrentry);

  csr_write64(eentry, LA_CSR_EENTRY);
  csr_write64(eentry, LA_CSR_MERRENTRY);
  csr_write64(tlbrentry, LA_CSR_TLBRENTRY);
}

void set_handler(unsigned long offset, void *addr, unsigned long size)
{
  memcpy((void *)(eentry + offset), addr, size);
  UP_DSB();
  UP_ISB();
if(offset > (64*VECSIZE))
  syslog(LOG_EMERG, "eentry +offset = 0x%016lx\n", (eentry + offset));
}

void set_merr_handler(unsigned long offset, void *addr, unsigned long size)
{
  unsigned long uncac_eentry = TO_UNCAC(eentry);
  memcpy((void *)(uncac_eentry + offset), addr, size);

  UP_DSB();
  UP_ISB();
}

void la64_exception_attach(void)
{
  irq_attach(LA_IRQ_SWI0, dispatch_syscall, NULL);
  //irq_attach(LA_IRQ_SWI1, la64_swint, NULL);
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

  set_handler(80*VECSIZE, handle_tlb_refill, VECSIZE);

  for (i = EXCCODE_TLBL; i <= EXCCODE_TLBPE; i++) {
    set_handler(i * VECSIZE, g_exception_table[i], VECSIZE);
  }

  for (i = EXCCODE_INT_START; i < EXCCODE_INT_END; i++)
    set_handler(i * VECSIZE, handle_vint, VECSIZE);

  for (i = EXCCODE_ADE; i < EXCCODE_BTDIS; i++)
    set_handler(i * VECSIZE, g_exception_table[i], VECSIZE);

  set_merr_handler(0x0, &except_vec_cex, VECSIZE);
}

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

#if 0
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
#endif

/****************************************************************************
 * Name: do_vint
 *
 * Description: IS[1~13]
 *
 ****************************************************************************/
uint64_t do_vint(uint64_t *regs)
{
  struct la64_percpu_s *percpu = la64_my_percpu();
  percpu->cur_regs = (uintptr_t)regs;

  uint32_t estat = csr_read32(LA_CSR_ESTAT);
  uint32_t irq = (estat & 0x3fff);

  syslog(LOG_EMERG, "VINT! ESTAT = 0x%x, irq = 0x%x, regs = 0x%p\n", estat, irq, regs);

  if ((estat & CSR_ESTAT_IS) == 0) {
    percpu->cur_regs = 0;
    return regs;
  }

  if (irq & (1 << 11)) {
    irq = LA_LOC_IRQ_BASE + INT_TI; /* TI */
  } else if (irq & 0x3fc) {
    int hwi = __builtin_ctz(irq & 0x3fc) - 2;
    irq = LA_LOC_IRQ_BASE + hwi;
  } else if (irq & 0x3) {
    int swi = __builtin_ctz(irq & 0x3);
    csr_write32(1 << swi, LA_CSR_ESTAT);
    irq = LA_LOC_IRQ_BASE + swi;
  } else {
    percpu->cur_regs = 0;
    return regs;
  }

  regs = la64_doirq(irq, regs);

  percpu->cur_regs = 0;

  syslog(LOG_EMERG, "VINT_END! ESTAT = 0x%x, regs = 0x%x\n", csr_read32(LA_CSR_ESTAT), regs);
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

