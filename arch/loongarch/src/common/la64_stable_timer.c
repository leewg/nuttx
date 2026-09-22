/****************************************************************************
 * arch/loongarch/src/common/la64_stable_timer.c
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
#include <nuttx/clock.h>
#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include <assert.h>
#include <stdint.h>
#include <time.h>

#include <arch/board/board.h>
#include <arch/irq.h>
#include <arch/csr.h>

#include "la64_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef BOARD_TIMER_FREQUENCY
#  define BOARD_TIMER_FREQUENCY 100000000UL
#endif
#define TICKS_PER_SECOND 1000

/****************************************************************************
 * Private Data
 ****************************************************************************/

static unsigned int hda_freq;
static unsigned int hda_1us_cnt = 30;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: systimer_irq_handler
 *
 * Description:
 *   Handler to be executed by the Systimer ISR.
 *
 * Input Parameters:
 *   irq           - IRQ associated to that interrupt.
 *   context       - Interrupt register state save info.
 *   arg           - A pointer to the argument provided when the interrupt
 *                   was registered.
 *
 * Returned Value:
 *   Zero on success; a negated errno value on failure.
 *
 ****************************************************************************/

static int la64_timer_interrupt(int irq, void *context, void *arg)
{
  /* Process timer interrupt */
  up_putc('+');

  nxsched_process_timer();

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_timer_initialize
 *
 * Description:
 *   This function is called during start-up to initialize the timer
 *   interrupt.
 *
 * Input Parameters:
 *   None.
 *
 * Returned Value:
 *   None.
 *
 ****************************************************************************/

void up_timer_initialize(void)
{
  uint64_t tcfg;

  irq_attach(LA_IRQ_TIMER, la64_timer_interrupt, NULL);

  csr_write64(0, LA_CSR_TVAL);
  csr_write64(0, LA_CSR_CNTC);

  {
    unsigned long mcsr2;
    unsigned int mul, div;

    mcsr2 = csr_read64(LA_CSR_MCSR2);
    hda_freq = (unsigned int)(mcsr2 & MCSR2_CCFREQ);
    mul = (mcsr2 >> MCSR2_CCMUL_SHIFT) & 0xFFFF;
    div = (mcsr2 >> MCSR2_CCDIV_SHIFT) & 0xFFFF;
    if (div && mul)
    {
      hda_freq = hda_freq * mul /div;
    }
    hda_freq >>= 2;

  }

  tcfg = hda_freq / TICKS_PER_SECOND;
  tcfg <<= CSR_TCFG_VAL_SHIFT;
  tcfg |= CSR_TCFG_EN | CSR_TCFG_PERIOD;

  csr_write64(tcfg, LA_CSR_TCFG);

  /* Enable the allocated CPU interrupt. */
  up_enable_irq(LA_IRQ_TIMER);
}

/****************************************************************************
 * Name: up_timer_gettime
 *
 * Description:
 *
 ****************************************************************************/
#ifdef CONFIG_CLOCK_TIMEKEEPING
int up_timer_gettime(FAR struct timespec *tp)
{
  int rID = 0;
  uint64_t count = 0;
  uint64_t nsec;

  __asm__ __volatile__(
    "rdtime.d %0, %1"
    : "=r"(count), "=r"(rID)
    :
    : "memory"
  );

  nsec = (count * NSEC_PER_SEC) / BOARD_TIMER_FREQUENCY;

  tp->tv_sec  = nsec / NSEC_PER_SEC;
  tp->tv_nsec = nsec % NSEC_PER_SEC;

  return OK;
}
#endif
