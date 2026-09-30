/****************************************************************************
 * arch/loongarch/src/common/la64_swint.c
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

#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <nuttx/debug.h>

#include <arch/irq.h>
#include <nuttx/addrenv.h>
#include <nuttx/sched.h>
#include <nuttx/userspace.h>

#ifdef CONFIG_LIB_SYSCALL
#  include <syscall.h>
#endif

#include "sched/sched.h"
#include "signal/signal.h"
#include "la64_internal.h"
//#include "addrenv.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/
typedef uintptr_t (*syscall_t)(unsigned int, ...);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: syscall_dispatch
 *
 * Description:
 *   Call the stub function corresponding to the system call.
 *   NOTE the non-standard parameter passing:
 *     A0 = param0
 *     A1 = param1
 *     A2 = param2
 *     A3 = param3
 *     A4 = param4
 *     A5 = param5
 *     A6 = context (aka SP)
 *     A7 = SYS_ call number
 *
 ****************************************************************************/

uintptr_t dispatch_syscall(uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3,
    uint64_t arg4, uint64_t arg5, uint64_t nbr, uint64_t *context)
{
    struct tcb_s *rtcb = this_task();
    register long a7 asm("a7") = (long)(nbr);
    register long a0 asm("a0") = (long)(arg0);
    register long a1 asm("a1") = (long)(arg1);
    register long a2 asm("a2") = (long)(arg2);
    register long a3 asm("a3") = (long)(arg3);
    register long a4 asm("a4") = (long)(arg4);
    register long a5 asm("a5") = (long)(arg5);
    syscall_t syscall_fn;
    uintptr_t ret;
    uintptr_t *regs = (uintptr_t *)context;

  syslog(LOG_INFO, "TCB(%p) ERA=0x%16lx, SP=0x%16lx\n", rtcb, rtcb->xcp.regs[REG_ERA], rtcb->xcp.regs[REG_SP]);
  syslog(LOG_EMERG, "regs(%p) ERA = 0x%016lx, SP = 0x%016lx,\n", regs, regs[REG_ERA], regs[REG_SP]);
  syslog(LOG_EMERG, "a0 = 0x%016lx, a1 = 0x%016lx, a2 = 0x%016lx\n", a0, a1, a2);
  syslog(LOG_EMERG, "a3 = 0x%016lx, a4 = 0x%016lx, a5 = 0x%016lx\n", a3, a4, a5);
  syslog(LOG_EMERG, "a6 = 0x%016lx, a7 = 0x%016lx,\n", context, a7);

    //syslog(LOG_EMERG, "SYSCALL! ESTAT = 0x%x, nbr = 0x%x\n", csr_read32(LA_CSR_ESTAT), nbr);

    /* Valid system call ? */
    if (a7 > SYS_maxsyscall)
    {
        /* Nope, get out */
        return -ENOSYS;
    }

    /* Set the user register context to TCB */
    rtcb->xcp.sregs = context;
    /* Indicate that we are in a syscall handler */
    rtcb->flags |= TCB_FLAG_SYSCALL;
    /* Offset a0 to account for the reserved syscall */
    a7 -= CONFIG_SYS_RESERVED;
    /* Find the system call from the lookup table */
    syscall_fn = (syscall_t)g_stublookup[a7];
    /* Run the system call, save return value locally */
    ret = syscall_fn(a7, a0, a1, a2, a3, a4, a5);

    /* System call is now done */
    rtcb->flags &= ~TCB_FLAG_SYSCALL;

    /* Update percpu_s regs */
    //la64_set_current_regs(cpu, NULL);

    /* Unmask any pending signals now */
    nxsig_unmask_pendingsignal();

    return ret;
}

/****************************************************************************
 * Name: la64_syscall
 *
 * Description:
 *   task switch syscall
 *
 ****************************************************************************/

uint64_t *la64_syscall(uint64_t *regs)
{
  int cpu = this_cpu();
  struct tcb_s **running_task = &g_running_tasks[cpu];
  struct tcb_s *tcb = this_task();
  uint64_t nbr = regs[REG_A7];

  DEBUGASSERT(regs);

  up_set_interrupt_context(true);

#ifdef CONFIG_DEBUG_SYSCALL_INFO
  svcinfo("Entry: regs: %p cmd: %d\n", regs, regs[REG_A7]);
#endif

  if (nbr != SYS_restore_context)
    (*running_task)->xcp.regs = regs;

  switch (nbr)
    {
      case SYS_restore_context:
        /* Restore the cpu lock */
        restore_critical_section(tcb, cpu);
#ifdef CONFIG_ARCH_ADDRENV
        addrenv_switch(tcb);
        tcb = this_task();
        *running_task = tcb;
#endif
        break;

      case SYS_switch_context:

#ifdef CONFIG_ARCH_ADDRENV
        addrenv_switch(tcb);
        tcb = this_task();
#endif
        /* Updata scheduler parameters */
        nxsched_switch_context(*running_task, tcb);
        *running_task = tcb;
        restore_critical_section(tcb, cpu);
        break;

      default:
        DEBUGPANIC();
        break;
    }

  regs = tcb->xcp.regs;
  DEBUGASSERT(regs != NULL);

  (*running_task)->xcp.regs = NULL;

  svcinfo("Restoring task entry ERA = 0x%" PRIx64 ", SP = 0x%" PRIx64 "\n", regs[REG_ERA], regs[REG_R3]);
  /* Report what happened.  That might difficult in the case of a context
   * switch
   */

#ifdef CONFIG_DEBUG_SYSCALL_INFO
  if (regs[REG_A7] <= SYS_switch_context)
    {
      svcinfo("SWInt Return: Context switch!, regs = %p\n", regs);
    }
  else
    {
      svcinfo("SWInt Return: %" PRIxPTR "\n", regs[REG_A7]);
    }
#endif
  up_set_interrupt_context(false);

  return regs;
}
