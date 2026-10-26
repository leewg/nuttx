/****************************************************************************
 * arch/loongarch/src/common/la64_initialstate.c
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
#include <nuttx/debug.h>

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include <nuttx/arch.h>
#include <nuttx/tls.h>
#include <nuttx/kmalloc.h>
#include <arch/irq.h>

#include "la64_internal.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_initial_state
 *
 * Description:
 *   A new thread is being started and a new TCB
 *   has been created. This function is called to initialize
 *   the processor specific portions of the new TCB.
 *
 *   This function must setup the initial architecture registers
 *   and/or  stack so that execution will begin at tcb->start
 *   on the next context switch.
 *
 ****************************************************************************/

void up_initial_state(struct tcb_s *tcb)
{
    struct xcptcontext *xcp = &tcb->xcp;
    uintptr_t topstack;
#ifdef CONFIG_ARCH_KERNEL_STACK
    uintptr_t *kstack = xcp->kstack;
#endif

  /* Initialize the initial exception register context structure */

  memset(xcp, 0, sizeof(struct xcptcontext));

  /* Initialize the idle thread stack */

  if (tcb->pid == IDLE_PROCESS_ID)
  {
      tcb->stack_alloc_ptr = (void *)g_cpux_idlestack(this_cpu());
      tcb->stack_base_ptr  = tcb->stack_alloc_ptr;
      tcb->adj_stack_size  = SMP_STACK_SIZE;

#ifdef CONFIG_STACK_COLORATION
      /* If stack debug is enabled, then fill the stack with a
       * recognizable value that we can use later to test for high
       * water marks.
       */

      la64_stack_color(tcb->stack_alloc_ptr, 0);
#endif /* CONFIG_STACK_COLORATION */

    return;
  }

  topstack = (uint64_t)tcb->stack_base_ptr + tcb->adj_stack_size;
  topstack &= ~((uint64_t)15);

#ifdef CONFIG_ARCH_KERNEL_STACK
  /* Use the process kernel stack to store context for user processes */

  if (kstack)
    {
      xcp->kstack  = kstack;
      xcp->ustkptr = (uintptr_t *)topstack;
      topstack     = (uintptr_t)kstack + ARCH_KERNEL_STACKSIZE;
      xcp->ktopstk = (uintptr_t *)topstack;
      xcp->kstkptr = xcp->ktopstk;
    }
#endif

  xcp->regs = (uint64_t *)(topstack - XCPTCONTEXT_SIZE);
  memset(xcp->regs, 0, XCPTCONTEXT_SIZE);

  /* Save the initial stack pointer.  Hmmm.. the stack is set to the very
   * beginning of the stack region.  Some functions may want to store data on
   * the caller's stack and it might be good to reserve some space.  However,
   * only the start function would do that and we have control over that one
   */

  xcp->regs[REG_SP]      = topstack;

#if 0
  /* DEBUG: task entry initialization */
  syslog(LOG_EMERG, "up_initial_state: tcb=%p pid=%d name=%s start=0x%lx stack_base=0x%lx sp=0x%lx tp=0x%lx\n",
      tcb, tcb->pid, tcb->name ? tcb->name : "(null)",
      (uintptr_t)tcb->start, (uintptr_t)tcb->stack_base_ptr,
      (uintptr_t)(tcb->stack_base_ptr + tcb->adj_stack_size),
      (uintptr_t)tcb->stack_alloc_ptr);
#endif

  /* Save the task entry point */

  xcp->regs[REG_ERA]     = (uintptr_t)tcb->start;

  syslog(LOG_EMERG, "up_initial_state: wrote ERA=0x%lx for tcb=%p pid=%d name=%s\n",
      (uintptr_t)tcb->start, tcb, tcb->pid, tcb->name ? tcb->name : "(null)");

  /* Setup thread local storage pointer */

#ifdef CONFIG_SCHED_THREAD_LOCAL
  xcp->regs[REG_TP] = (uintptr_t)tcb->stack_alloc_ptr + sizeof(struct tls_info_s);
#endif

  xcp->regs[REG_CRMD] = __csrrd(LA_CSR_CRMD);
  xcp->regs[REG_PRMD] = CSR_PRMD_PIE;

#ifndef CONFIG_BUILD_FLAT
  tcb->xcp.initregs = tcb->xcp.regs;
#endif

#if 0
  sinfo("Task %s initial ERA = 0x%016lx, SP = 0x%016lx, TP = 0x%016lx\n",
      tcb->name, xcp->regs[REG_ERA], xcp->regs[REG_SP], xcp->regs[REG_TP]);
#endif
}
