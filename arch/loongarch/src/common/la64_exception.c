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

typedef uintptr_t (*syscall_t)(unsigned int, ...);

#define ECFG_VS_128B        (7 << CSR_ECFG_VS_SHIFT)


/****************************************************************************
 * Name: trap_init
 *
 * Description:
 *
 ****************************************************************************/

void trap_init(void)
{
  uint32_t ecfg;

  ecfg = csr_read32(LA_CSR_ECFG);
  ecfg &= ~CSR_ECFG_VS;
  ecfg |= ECFG_VS_128B;
  csr_write32(ecfg, LA_CSR_ECFG);

  csr_write64(0x9000000000000000, LA_CSR_MERRENTRY);
  csr_write64(0x9000000000001000, LA_CSR_TLBRENTRY);
  csr_write64(0x9000000000002000, LA_CSR_EENTRY);

  UP_DSB();
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

uintptr_t la64_syscall_dispatch(unsigned int nbr, uint64_t arg0, uint64_t arg1,
                           uint64_t arg2, uint64_t arg3, uint64_t arg4,
                           uint64_t arg5, uint64_t *regs)
{
    struct tcb_s *rtcb = this_task();
    syscall_t do_syscall;
    uintptr_t ret;

    syslog(LOG_EMERG, "SYSCALL! ESTAT = 0x%x, nbr = 0x%x\n",
        csr_read32(LA_CSR_ESTAT), nbr);

    /* Valid system call ? */
    if (nbr > SYS_maxsyscall)
    {
        /* Nope, get out */
        return -ENOSYS;
    }

    /* Set the user register context to TCB */
    //rtcb->xcp.sregs = regs;
    /* Indicate that we are in a syscall handler */
    rtcb->flags |= TCB_FLAG_SYSCALL;
    /* Offset a0 to account for the reserved syscall */
    nbr -= CONFIG_SYS_RESERVED;
    /* Find the system call from the lookup table */
    do_syscall = (syscall_t)g_stublookup[nbr];
    /* Run the system call, save return value locally */
    ret = do_syscall(nbr, arg0, arg1, arg2, arg3, arg4, arg5);

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

/****************************************************************************
 * Name: la64_exception_dispatch
 *
 * Description: IS[0]
 *
 ****************************************************************************/

uint64_t *la64_exception_handler(uint64_t *regs)
{
  uint32_t estat = csr_read32(LA_CSR_ESTAT);
  uint32_t ecode = (estat & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;

  syslog(LOG_EMERG, "Exception! ESTAT = 0x%x, ECODE = 0x%x\n", estat, ecode);

  if (ecode == 0xb)
  {
    regs[REG_ERA] += 4;
    uintptr_t ret = la64_syscall_dispatch((unsigned int)regs[REG_A0],
        regs[REG_A1], regs[REG_A2], regs[REG_A3], regs[REG_A4],
        regs[REG_A5], regs[REG_A6], regs);

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
uint64_t *la64_vint_handler(uint64_t *regs)
{
  struct la64_percpu_s *percpu = la64_my_percpu();
  percpu->cur_regs = (uintptr_t)regs;

  uint32_t estat = csr_read32(LA_CSR_ESTAT);
  uint32_t irq = (estat & 0x3fff);

  up_set_interrupt_context(true);

  syslog(LOG_EMERG, "VINT! ESTAT = 0x%x, irq = 0x%x\n", estat, irq);

  if ((estat & CSR_ESTAT_IS) == 0) {
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
    return regs;
  }

  regs = la64_doirq(irq, regs);

  percpu->cur_regs = 0;

  up_set_interrupt_context(false);

  syslog(LOG_EMERG, "VINT END! crmd = 0x%x, ecfg = 0x\n",
      csr_read32(LA_CSR_CRMD), csr_read32(LA_CSR_ECFG));

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

