/****************************************************************************
 * arch/loongarch64/src/common/la64_doirq.c
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
#include <assert.h>

#include <nuttx/irq.h>
#include <nuttx/addrenv.h>
#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <arch/board/board.h>
#include <sched/sched.h>

#include "la64_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

uintreg_t *la64_doirq(int irq, uintreg_t *regs)
{
  struct tcb_s *tcb = this_task();

  /* Current regs non-zero indicates that we are processing an interrupt;
   * current_regs is also used to manage interrupt level context switches.
   *
   * Nested interrupts are not supported. But an exception may occur while
   * processing an interrupt. In this case, current_regs will be non-NULL.
   */

  DEBUGASSERT(!up_interrupt_context());

  tcb->xcp.regs = regs;

  /* Deliver the IRQ */

  irq_dispatch(irq, regs);
  tcb = this_task();

  /* Check for a context switch. */

  if (regs != tcb->xcp.regs)
  {
    struct tcb_s **running_task = &g_running_tasks[this_cpu()];

#ifdef CONFIG_ARCH_ADDRENV
      /* Make sure that the address environment for the previously
       * running task is closed down gracefully (data caches dump,
       * MMU flushed) and set up the address environment for the new
       * thread at the head of the ready-to-run list.
       */

      addrenv_switch(tcb);
      tcb = this_task();
#endif

      /* Update scheduler parameters */

      nxsched_switch_context(*running_task, tcb);

      /* Record the new "running" task when context switch occurred.
       * g_running_tasks[] is only used by assertion logic for reporting
       * crashes.
       */

      *running_task = tcb;
      regs = tcb->xcp.regs;
  }

  /* Set irq flag */

  up_set_interrupt_context(false);

  /* (*running_task)->xcp.regs is about to become invalid
   * and will be marked as NULL to avoid misusage.
   */

  tcb->xcp.regs = NULL;

  return regs;
}
