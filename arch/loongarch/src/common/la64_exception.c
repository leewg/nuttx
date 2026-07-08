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
#include "ilog2.h"

extern void handle_exception_generic(void);
extern void handle_fp_disabled(void);
extern void la64_fpu_disabled_dispatch(uint64_t *regs);
extern void cache_parity_error(void);

extern void handle_sys(void);
extern void handle_vint(void);
extern void handle_reserved(void);

#define VECSIZE 0x200
#define SZ_64K  0x00010000

unsigned long eentry;
unsigned long tlbrentry;
long exception_handlers[VECSIZE * 128 / sizeof(long)] aligned_data(SZ_64K);

void *exception_table[EXCCODE_INT_START] = {
  [0 ... EXCCODE_INT_START - 1] = handle_reserved,

#if 0
  [EXCCODE_TLBI]		= handle_tlb_load,
  [EXCCODE_TLBL]		= handle_tlb_load,
  [EXCCODE_TLBS]		= handle_tlb_store,
  [EXCCODE_TLBM]		= handle_tlb_modify,
  [EXCCODE_TLBNR]		= handle_tlb_protect,
  [EXCCODE_TLBNX]		= handle_tlb_protect,
  [EXCCODE_TLBPE]		= handle_tlb_protect,
  [EXCCODE_ADE]		= handle_ade,
  [EXCCODE_ALE]		= handle_ale,
  [EXCCODE_SYS]		= handle_sys,
  [EXCCODE_BP]		= handle_bp,
  [EXCCODE_INE]		= handle_ri,
  [EXCCODE_IPE]		= handle_ri,
  [EXCCODE_FPDIS]		= handle_fpu,
  [EXCCODE_LSXDIS]	= handle_lsx,
  [EXCCODE_LASXDIS]	= handle_lasx,
  [EXCCODE_FPE]		= handle_fpe,
  [EXCCODE_BTDIS]		= handle_lbt,
  [EXCCODE_WATCH]		= handle_watch,
#endif
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

typedef uintptr_t (*syscall_t)(unsigned int, ...);

static inline void setup_vint_size(unsigned int size)
{
  unsigned int vs;

  vs = ilog2(size/4);

  if (vs == 0 || vs > 7)
    syslog(LOG_INFO, "vs is error: %x\n", vs);

  csr_xchg32(vs<<CSR_ECFG_VS_SHIFT, CSR_ECFG_VS, LOONGARCH_CSR_ECFG);
}

static void configure_exception_vector(void)
{
  eentry = (unsigned long)exception_handlers;
  tlbrentry = (unsigned long)exception_handlers + 80*VECSIZE;

  csr_write64(eentry, LOONGARCH_CSR_EENTRY);
  csr_write64(tlbrentry, LOONGARCH_CSR_TLBRENTRY);
  csr_write64(eentry, LOONGARCH_CSR_MERRENTRY);
}

void set_handler(unsigned long offset, void *addr, unsigned long size)
{
  memcpy((void *)(eentry + offset), addr, size);

  UP_ISB();
}

/****************************************************************************
 * Name: trap_init
 *
 * Description:
 *
 ****************************************************************************/

void trap_init(void)
{
  int i;
  unsigned long ecfg;

  setup_vint_size(VECSIZE);
  configure_exception_vector();

  for (i = EXCCODE_INT_START; i < EXCCODE_INT_END; i++)
  {
    set_handler(i*VECSIZE, handle_vint, VECSIZE);
  }

  for (i = 0; i < 64; i++)
  {
    set_handler(i*VECSIZE, handle_reserved, VECSIZE);
  }

  set_handler(11*VECSIZE, handle_sys, VECSIZE);

#ifdef CONFIG_ARCH_FPU
  set_handler(31*VECSIZE, handle_fp_disabled, VECSIZE);
#endif

  UP_ISB();

}

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
 *     A7 = context (aka SP)
 *
 ****************************************************************************/

void la64_syscall_dispatch(unsigned int nbr, uint64_t arg0, uint64_t arg1,
                           uint64_t arg2, uint64_t arg3, uint64_t arg4,
                           uint64_t arg5, uint64_t *regs)
{
    struct tcb_s *rtcb = this_task();
    register long a0 asm("a0") = (long)(nbr);
    register long a1 asm("a1") = (long)(arg0);
    register long a2 asm("a2") = (long)(arg1);
    register long a3 asm("a3") = (long)(arg2);
    register long a4 asm("a4") = (long)(arg3);
    register long a5 asm("a5") = (long)(arg4);
    register long a6 asm("a6") = (long)(arg5);
    syscall_t do_syscall;
    uintptr_t ret;
    int cpu = up_cpu_index();

    /* Valid system call ? */
    if (a0 > SYS_maxsyscall)
    {
        /* Nope, get out */
        return -ENOSYS;
    }

    /* Set the user register context to TCB */
    //rtcb->xcp.sregs = regs;
    /* Indicate that we are in a syscall handler */
    rtcb->flags |= TCB_FLAG_SYSCALL;
    /* Offset a0 to account for the reserved syscall */
    a0 -= CONFIG_SYS_RESERVED;
    /* Find the system call from the lookup table */
    do_syscall = (syscall_t)g_stublookup[a0];
    /* Run the system call, save return value locally */
    ret = do_syscall(a0, a1, a2, a3, a4, a5, a6);

    /* System call is now done */
    rtcb->flags &= ~TCB_FLAG_SYSCALL;

    /* Update percpu_s regs */
    //la64_set_current_regs(cpu, NULL);

    /* Unmask any pending signals now */
    nxsig_unmask_pendingsignal();

    return ret;
}

/****************************************************************************
 * Name: la64_regs_dump
 *
 * Description:
 *
 ****************************************************************************/

static void la64_regs_dump(uint64_t *regs)
{
  _alert("========== NuttX Core Register Dump ==========\n");
  _alert("RA (r1)  = 0x%016lx  TP (r2)  = 0x%016lx\n", regs[REG_RA], regs[REG_TP]);
  _alert("SP (r3)  = 0x%016lx  FP (r22) = 0x%016lx\n", regs[REG_SP], regs[REG_FP]);
  _alert("A0 (r4)  = 0x%016lx  A1 (r5)  = 0x%016lx\n", regs[REG_A0], regs[REG_A1]);
  _alert("A2 (r6)  = 0x%016lx  A3 (r7)  = 0x%016lx\n", regs[REG_A2], regs[REG_A3]);
  _alert("A4 (r8)  = 0x%016lx  A5 (r9)  = 0x%016lx\n", regs[REG_A4], regs[REG_A5]);
  _alert("A6 (r10) = 0x%016lx  A7 (r11) = 0x%016lx\n", regs[REG_A6], regs[REG_A7]);
  _alert("T0 (r12) = 0x%016lx  T1 (r13) = 0x%016lx\n", regs[REG_T0], regs[REG_T1]);
  _alert("T2 (r14) = 0x%016lx  T3 (r15) = 0x%016lx\n", regs[REG_T2], regs[REG_T3]);

  _alert("---------------- CSR ----------------\n");
  _alert("CSR_ERA  = 0x%016lx  CSR_CRMD = 0x%016lx\n", regs[REG_ERA], regs[REG_CRMD]);
  _alert("CSR_PRMD = 0x%016lx  CSR_BADV = 0x%016lx\n", regs[REG_PRMD], regs[REG_BADV]);
  _alert("==============================================\n");
}

void do_reserved(uint64_t *regs)
{
  up_putc('E');
  up_putc('X');
  up_putc('C');
  up_putc('E');
  up_putc('P');
  up_putc('T');
  up_putc('\n');
}

/****************************************************************************
 * Name: la64_exception_dispatch
 *
 * Description:
 *
 ****************************************************************************/

void la64_exception_dispatch(uint64_t *regs)
{
  syslog(LOG_INFO, "la64_exceptino_dispatch\n");

  la64_regs_dump(regs);
  PANIC();
}

void cache_parity_error(void)
{
  /* For the moment, report the problem and hang. */
  syslog(LOG_INFO, "Cache error exception:\n");
  PANIC();
}

/****************************************************************************
 * Name: do_vint
 *
 * Description:
 *
 ****************************************************************************/
void do_vint(uint64_t *regs)
{
    uint64_t estat = regs[REG_ESTAT];
    uint32_t irq_mask = estat &CSR_ESTAT_IS;
    int irq_bit;

    up_putc('V');
    up_putc('I');
    up_putc('N');
    up_putc('T');
    up_putc('\n');

    /* Loongarch interrupt distrubition:
     * bit12   : IPP
     * bit11   : TI
     * bit10   : PMI
     * bit9..2 : HWI7 ~ HWI0
     * bit1..0 : SWI1 ~ SWI0
     */
    while (irq_mask != 0)
    {
#ifdef __GNUC__
        irq_bit = __builtin_ctz(irq_mask);
#else
        for (irq_bit = LA_LOC_IRQ_BASE; irq_bit < 13; irq_bit++)
        {
            if ((irq_mask & (1 << irq_bit)) != 0)
                break;
        }
#endif
        regs = la64_doirq(LA_EXT_IRQ_BASE + irq_bit, regs);

        irq_mask &= ~(1 << irq_bit);
    }
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

void la64_fatal_handler(uint64_t estat, uint64_t era, uint64_t badv, FAR uintptr_t *regs)
{
  FAR struct la64_percpu_s *percpu = la64_my_percpu();
  uint32_t ecode = (estat & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;

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

