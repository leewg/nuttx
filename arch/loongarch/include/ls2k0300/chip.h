/****************************************************************************
 * arch/loongarch/include/ls2k0300/chip.h
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

#ifndef __ARCH_LOONGARCH_INCLUDE_LS2K0300_CHIP_H
#define __ARCH_LOONGARCH_INCLUDE_LS2K0300_CHIP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <arch/ls2k0300/irq.h>
#include <arch/arch.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Physical Memory Map
 *
 * Loongson 2K300 uses a unified address space with DDR mapped at 0x0
 * and MMIO region starting from 0x1FE0_0000 (typical for 2K series).
 */
#define CHIP_CTRL0_BASE 0x16000100
#define INTC_BASE       0x16001000
#define HPET_BASE       0x16120000
#define UART0_BASE      0x16100000
#define THSENS_BASE     0x16001500


/* HPET Configuration */
#define HPET_GEN_CAP        0x00
#define HPET_GEN_CFG        0x10
#define HPET_GEN_ISR        0x20
#define HPET_MAIN_CNT       0xf0

/* Timer N Offset */
#define HPET_TN_OFF(n)  (0x100 + (n) * 0x20)
#define HPET_TN_CFG(n)  (HPET_TN_OFF(n) + 0x00)
#define HPET_TN_CMP(n)  (HPET_TN_OFF(n) + 0x08)

#define ENABLE_CNF      (0x1 << 0)

/* Timer N configuration bit */
#define TN_INT_TYPE_CNF (1 << 1)     /* INT TYPE：1=level，0=edge */
#define TN_INT_ENB_CNF  (1 << 2)     /* INT Enable */
#define TN_TYPE_CNF     (1 << 3)     /* Timer type：1=cycle，0=signle */
#define TN_PER_INT_CAP  (1 << 4)     /* RO, cycle int cap */
#define TN_SIZE_CAP     (1 << 5)     /* RO, 1=64b，0=32b */
#define TN_VAL_SET_CNF  (1 << 6)     /* reset value */
#define TN_32MODE_CNF   (1 << 8)     /* RO, 32b mode */

#define HPET0_IRQ       (LOONGARCH_IRQ_BASE + 19)

/* UART (NS16550 Compatible) */


/* Clock and Timer Definitions */

#define CHIP_CPU_FREQ           (1000000000UL)     /* 1000MHz */
#define CHIP_HPET_FREQ          (10000000UL)        /* 10MHz */

/* Thsens Configuration */
#define HSENS_VAL         0x14
#define THSENS_CFG        0x18
#define THSENS_SCALE_HI0  0x20
#define THSENS_SCALE_HI1  0x24

/* UART Configuration */

#define UART_REF_CLK  100000000
#define UART_DIV_HI   (((UART_REF_CLK + (115200*8))/(115200*16)) >> 8)
#define UART_DIV_LO   (((UART_REF_CLK + (115200*8))/(115200*16)) & 0xff)

/**
 * 8250/16550 UART Register Offsets (Compatible)
 */

#define UART_THR    0x00    /* Transmitter Holding Register */
#define UART_RBR    0x00    /* Receiver Buffer Register */
#define UART_DLL    0x00    /* Divisor Latch Low (DLAB=1) */
#define UART_DLH    0x01    /* Divisor Latch High (DLAB=1) */
#define UART_IER    0x01    /* Interrupt Enable Register */
#define UART_IIR    0x02    /* Interrupt Identification Register */
#define UART_FCR    0x02    /* FIFO Control Register */
#define UART_DLD    0x02    /* Divisor Latch Decimals(DLAB=1) */
#define UART_LCR    0x03    /* Line Control Register */
#define UART_MCR    0x04    /* Modem Control Register */
#define UART_LSR    0x05    /* Line Status Register */
#define UART_MSR    0x06    /* Modem Status Register */

/* IER bits */
#define UART_IER_IRxE   (1 << 0)
#define UART_IER_ITxE   (1 << 1)
#define UART_IER_ILE    (1 << 2)
#define UART_IER_IME    (1 << 3)
#define UART_IER_RXDE   (1 << 4)
#define UART_IER_TXDE   (1 << 5)
#define UART_IER_ARTSE  (1 << 6)
#define UART_IER_ACTSE  (1 << 7)
#define UART_IER_ALL    (UART_IER_IRxE | UART_IER_ITxE | UART_IER_ILE | \
                         UART_IER_IME | UART_IER_RXDE | UART_IER_TXDE | \
                         UART_IER_ARTSE | UART_IER_ACTSE)

/* IIR bits */
#define UART_IIR_RxSR         (0x6)
#define UART_IIR_RxTRIG       (0x4)
#define UART_IIR_TxTIMEOUT    (0xc)
#define UART_IIR_TxEMPTY      (0x2)
#define UART_IIR_MSR          (0x0)
#define UART_IIR_INTp         (1 << 0)

/**
 * UART Line Status Register (LSR) bits
 */

#define UART_LSR_DR     (1 << 0)    /* Data Ready */
#define UART_LSR_OE     (1 << 1)    /* Overrun Error */
#define UART_LSR_PE     (1 << 2)    /* Parity Error */
#define UART_LSR_FE     (1 << 3)    /* Framing Error */
#define UART_LSR_BI     (1 << 4)    /* Break Interrupt */
#define UART_LSR_TFE    (1 << 5)    /* TX FIFO Empty */
#define UART_LSR_TE     (1 << 6)    /* TX transmit Empty */
#define UART_LSR_ERR    (1 << 7)    /* FIFO Error */

/**
 * UART Line Control Register (LCR) bits
 */

#define UART_LCR_WLS_MSK  0x03  /* char length select mask */
#define UART_LCR_WLS_5    0x00  /* 5 bit char length */
#define UART_LCR_WLS_6    0x01  /* 6 bit char length */
#define UART_LCR_WLS_7    0x02  /* 7 bit char length */
#define UART_LCR_WLS_8    0x03  /* 8 bit char length */
#define UART_LCR_STB      0x04  /* stop Bits */
#define UART_LCR_PEN      0x08  /* Parity enable */
#define UART_LCR_EPS      0x10  /* Even Parity select */
#define UART_LCR_STKP     0x20  /* Stick Parity */
#define UART_LCR_SBRK     0x20  /* Set Break */
#define UART_LCR_BKSE     0x80  /* Bank select enable */
#define UART_LCR_DLAB     0x80  /* Divisor latch access bit */

#define UART_FCR_TL_1B (0x0 << 6)
#define UART_FCR_TL_2B (0x1 << 6)
#define UART_FCR_TL_3B (0x2 << 6)
#define UART_FCR_TL_4B (0x3 << 6)
#define UART_FCR_TXSET (0x1 << 2)
#define UART_FCR_RXSET (0x1 << 1)

#define UART_BAUD          115200
#define UART_BITS          8
#define UART_PARITY        0
#define UART_2STOP         0
#define UART_LCR_8N1      0x03

#endif /* __ARCH_LOONGARCH_INCLUDE_LS2K0300_CHIP_H */
