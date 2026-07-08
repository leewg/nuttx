/****************************************************************************
 * arch/loongarch/include/ls2k300/irq.h
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

#ifndef __ARCH_LOONGARCH_INCLUDE_LS2K300_IRQ_H
#define __ARCH_LOONGARCH_INCLUDE_LS2K300_IRQ_H

#include <arch/irq.h>

/****************************************************************************
 * Included Files
 ****************************************************************************/

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Legacy IO Interrupt */
#define LS2K0300_CORE_INTISR0   0x40    // map to core's low32 isr
#define LS2K0300_INTISR0        0x44    // low32 isr
#define LS2K0300_CORE_INTISR1   0x48    // map to core's hig32 isr
#define LS2K0300_INTISR1        0x4C    // hig32 isr

#define LS2K0300_ENTRY0_0       0x400   // route reg for 0~7
#define LS2K0300_ENTRY8_0       0x408   // route reg for 8~15
#define LS2K0300_ENTRY16_0      0x410   // route reg for 16~23
#define LS2K0300_ENTRY24_0      0x418   // route reg for 24~31

#define LS2K0300_INTISR_0       0x420   // low32, ISR
#define LS2K0300_INTIEN_0       0x424   // low32, enable status
#define LS2K0300_INTSET_0       0x428   // low32, enable
#define LS2K0300_INTCLR_0       0x42c   // low32, clear
#define LS2K0300_INTSET_0       0x430   // low32, POL
#define LS2K0300_INTSET_0       0x434   // low32, EDGE

#define LS2K0300_ENTRY0_1       0x440   // route reg for 32~39
#define LS2K0300_ENTRY8_1       0x448   // route reg for 40~47
#define LS2K0300_ENTRY16_1      0x450   // route reg for 48~55
#define LS2K0300_ENTRY24_1      0x458   // route reg for 56~63

#define LS2K0300_INTISR_1       0x460   // low32, ISR
#define LS2K0300_INTIEN_1       0x464   // low32, enable status
#define LS2K0300_INTSET_1       0x468   // low32, enable
#define LS2K0300_INTCLR_1       0x46c   // low32, clear
#define LS2K0300_INTSET_1       0x470   // low32, POL
#define LS2K0300_INTSET_1       0x474   // low32, EDGE

#define LS2K0300_THSENS_INT_CTRL_HI0    0x500
#define LS2K0300_THSENS_INT_CTRL_HI1    0x504
#define LS2K0300_THSENS_INT_CTRL_LO0    0x508
#define LS2K0300_THSENS_INT_CTRL_LO1    0x50c
#define LS2K0300_THSENS_INT_CTRL_ISR    0x510

/* Extern IO Interrupt */
#define EXTIOI_ENABLE   (1 << 19)

#define LS2K0300_EXTIOI_ACK     0x148
#define LS2K0300_EXTIOI_MAP     0x4c0
#define LS2K0300_EXTIOI_IEN0    0x600
#define LS2K0300_EXTIOI_IEN1    0x604
#define LS2K0300_EXTIOI_IEN2    0x608
#define LS2K0300_EXTIOI_IEN3    0x60c
#define LS2K0300_EXTIOI_POL0    0x640
#define LS2K0300_EXTIOI_POL1    0x644
#define LS2K0300_EXTIOI_POL2    0x648
#define LS2K0300_EXTIOI_POL3    0x64c
#define LS2K0300_EXTIOI_ISR0    0x700
#define LS2K0300_EXTIOI_ISR1    0x704
#define LS2K0300_EXTIOI_ISR2    0x708
#define LS2K0300_EXTIOI_ISR3    0x70c
#define LS2K0300_EXTIOI_CORE_ISR0    0x800
#define LS2K0300_EXTIOI_CORE_ISR1    0x804
#define LS2K0300_EXTIOI_CORE_ISR2    0x808
#define LS2K0300_EXTIOI_CORE_ISR3    0x80c

/* Map LoongArch64 exception code to NuttX IRQ */

#define LS2K0300_IRQ_UART0   (LA_EXT_IRQ_BASE + 0)
#define LS2K0300_IRQ_HPET0   (LA_EXT_IRQ_BASE + 33)

/* Total number of IRQs */
#define NR_IRQS 128

#endif /* __ARCH_LOONGARCH_INCLUDE_LS2K300_IRQ_H */
