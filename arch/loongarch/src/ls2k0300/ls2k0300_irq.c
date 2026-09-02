/****************************************************************************
 * arch/loongarch/src/ls2k0300/ls2k0300_irq.c
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
#include <stdio.h>
#include <assert.h>
#include <nuttx/debug.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>
#include <arch/csr.h>

#include <arch/irq.h>
#include <arch/ls2k0300/chip.h>

#include "la64_internal.h"
#include "chip.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void la64_exception_attach(void)
{
  ;
}

/****************************************************************************
 * Name: up_irqinitialize
 ****************************************************************************/

void up_irqinitialize(void)
{
  uint64_t val;

  /* enable extioi mode */
  putreg32(getreg32(CHIP_CTRL0_BASE)|EXTIOI_ENABLE, CHIP_CTRL0_BASE);

  /* mask all INT */
  val = read_csr_ecfg();
  val &= ~0x1fff;
  write_csr_ecfg(val);

  /* disable all extern interrupt */
  putreg32(0x0, INTC_BASE + LS2K0300_EXTIOI_IEN0);
  putreg32(0x0, INTC_BASE + LS2K0300_EXTIOI_IEN1);
  putreg32(0x0, INTC_BASE + LS2K0300_EXTIOI_IEN2);
  putreg32(0x0, INTC_BASE + LS2K0300_EXTIOI_IEN3);

  /* all ext int map to INT2 */
  putreg32(0x04040404, INTC_BASE + LS2K0300_EXTIOI_MAP);

  /* level trigger */
  putreg32(0x0, INTC_BASE + LS2K0300_EXTIOI_POL0);
  putreg32(0x0, INTC_BASE + LS2K0300_EXTIOI_POL1);
  putreg32(0x0, INTC_BASE + LS2K0300_EXTIOI_POL2);
  putreg32(0x0, INTC_BASE + LS2K0300_EXTIOI_POL3);

  /* clear all isr */
  putreg32(0x1, INTC_BASE + LS2K0300_EXTIOI_ISR0);
  putreg32(0x1, INTC_BASE + LS2K0300_EXTIOI_ISR1);
  putreg32(0x1, INTC_BASE + LS2K0300_EXTIOI_ISR2);
  putreg32(0x1, INTC_BASE + LS2K0300_EXTIOI_ISR3);

  /* clear all map to CORE isr */
  putreg32(0x1, INTC_BASE + LS2K0300_EXTIOI_CORE_ISR0);
  putreg32(0x1, INTC_BASE + LS2K0300_EXTIOI_CORE_ISR1);
  putreg32(0x1, INTC_BASE + LS2K0300_EXTIOI_CORE_ISR2);
  putreg32(0x1, INTC_BASE + LS2K0300_EXTIOI_CORE_ISR3);

  /* Attach the common interrupt handler */
  la64_exception_attach();

  /* enable all INT */
  val = read_csr_ecfg();
  val |= 0x1fff;
  write_csr_ecfg(val);

  /* Colorize the interrupt stack for debug purposes */
#if defined(CONFIG_STACK_COLORATION) && CONFIG_ARCH_INTERRUPTSTACK > 15
  size_t intstack_size = (CONFIG_ARCH_INTERRUPTSTACK & ~15);
  la64_stack_color(g_intstackalloc, intstack_size);
#endif

#ifndef CONFIG_SUPPRESS_INTERRUPTS

  /* And finally, enable interrupts */
  putreg32(0xf, INTC_BASE + LS2K0300_EXTIOI_IEN0);
  //putreg32(0xe, INTC_BASE + LS2K0300_EXTIOI_IEN1);
  //putreg32(0xb000000e, INTC_BASE + LS2K0300_EXTIOI_IEN1);
  //putreg32(0x3f, INTC_BASE + LS2K0300_EXTIOI_IEN2);

  //la64_color_intstack();
  //up_irq_enable();
#endif

}

/****************************************************************************
 * Name: up_disable_irq
 *
 * Description:
 *   Disable the IRQ specified by 'irq'
 *
 ****************************************************************************/

void up_disable_irq(int irq)
{
  uint32_t extirq = 0, base = 0, offs = 0;
  uint32_t val;

  if (irq < LA_EXT_IRQ_BASE)
  {
    val = read_csr_ecfg();
    val &= ~(0x1 << irq);
    write_csr_ecfg(val);
  } else {
    extirq = irq - LA_EXT_IRQ_BASE;
    base = (extirq/32) * 4;
    offs = extirq%32;

    val = getreg32(LS2K0300_EXTIOI_IEN0 + base);
    val &= ~(0x1 << offs);
    putreg32(val, LS2K0300_EXTIOI_IEN0 + base);
  }
}

/****************************************************************************
 * Name: up_enable_irq
 *
 * Description:
 *   Enable the IRQ specified by 'irq'
 *
 ****************************************************************************/

void up_enable_irq(int irq)
{
  uint32_t extirq = 0, base = 0, offs = 0;
  uint32_t val, bit;

  if (irq > LA_LOC_IRQ_BASE && irq < LA_LOC_IRQ_BASE + 16)
  {
    bit = irq - LA_LOC_IRQ_BASE;
    val = read_csr_ecfg();
    val |= (0x1 << irq);
    write_csr_ecfg(val);
  } else {
    extirq = irq - LA_EXT_IRQ_BASE;
    base = (extirq/32) * 4;
    offs = extirq%32;

    val = getreg32(LS2K0300_EXTIOI_IEN0 + base);
    val |= (0x1 << offs);
    putreg32(val, LS2K0300_EXTIOI_IEN0 + base);
  }
}
