/****************************************************************************
 * boards/loongarch/ls2k0300/pai_evb/src/board.c
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
#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <debug.h>
#include <stdint.h>
#include <stdbool.h>

#include <arch/board/board.h>
#include <arch/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define REG8(addr)      (*(volatile uint8_t *)(addr))
#define REG32(addr)     (*(volatile uint32_t *)(addr))

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pai_evb_uart_putc
 *
 * Description:
 *   Output a single character to UART0 (used for early debug output)
 *
 ****************************************************************************/

static void pai_evb_uart_putc(char ch)
{
    /* Wait for transmitter holding register to be empty */
    while ((REG8(PAI_EVB_UART0_BASE + UART_LSR_OFFSET) & UART_LSR_THRE) == 0);

    /* Send character */
    REG8(PAI_EVB_UART0_BASE + UART_DATA_OFFSET) = ch;

    /* Wait for transmission complete */
    while ((REG8(PAI_EVB_UART0_BASE + UART_LSR_OFFSET) & UART_LSR_TEMT) == 0);
}

/****************************************************************************
 * Name: pai_evb_uart_initialize
 *
 * Description:
 *   Initialize UART0 for early debug output at 115200 baud
 *
 *   LS2K0300 UART Baudrate Formula:
 *   Baudrate = (Ref_Clk / (16 * Divisor))
 *   For 100MHz Ref_Clk and 115200 baud:
 *   Divisor = 100000000 / (16 * 115200) ≈ 54 (0x36)
 *
 *   Note: The actual reference clock for UART is typically CLOCK_FREQ/4
 *   For 800MHz CPU: 800M / 4 = 200MHz
 *   Divisor = 200000000 / (16 * 115200) ≈ 108 (0x6C)
 *
 ****************************************************************************/
static void pai_evb_uart_initialize(void)
{
    uint16_t divisor;
    uint8_t lcr;

    /* Disable all interrupts */
    REG8(PAI_EVB_UART0_BASE + UART_IER_OFFSET) = 0x00;

    /* Set DLAB bit to access divisor latches */
    lcr = UART_LCR_8BITS | UART_LCR_STOPBITS_1;
    REG8(PAI_EVB_UART0_BASE + UART_LCR_OFFSET) = lcr | UART_LCR_DLAB;

    /* Calculate and set baud rate divisor */
    divisor = (BOARD_TIMER_FREQ / (16 * BOARD_UART_BAUDRATE));
    REG8(PAI_EVB_UART0_BASE + 0x00) = (divisor & 0xFF);        /* DLL */
    REG8(PAI_EVB_UART0_BASE + 0x01) = ((divisor >> 8) & 0xFF); /* DLM */

    /* Clear DLAB bit - normal operation */
    REG8(PAI_EVB_UART0_BASE + UART_LCR_OFFSET) = lcr;

    /* Enable FIFO if available, set trigger level to 14 bytes */
    REG8(PAI_EVB_UART0_BASE + UART_ISR_OFFSET) = 0xC7;

    /* Set Modem Control Register - DTR and RTS high */
    REG8(PAI_EVB_UART0_BASE + UART_MCR_OFFSET) = 0x03;
}

/****************************************************************************
 * Name: pai_evb_led_initialize
 *
 * Description:
 *   Initialize LED GPIO pins (Port 0, Pin 0) as output
 *
 ****************************************************************************/

#ifdef CONFIG_PAI_EVB_HAVE_LED
static void pai_evb_led_initialize(void)
{
    uint32_t oe_val;

    /* Configure LED GPIO as output */
    oe_val = REG32(PAI_EVB_GPIO_BASE + GPIO_OE_OFFSET);
    oe_val |= (1 << BOARD_LED_PIN);  /* Set bit 0 for output */
    REG32(PAI_EVB_GPIO_BASE + GPIO_OE_OFFSET) = oe_val;

    /* Set LED initially off (low) */
    uint32_t out_val = REG32(PAI_EVB_GPIO_BASE + GPIO_OUT_OFFSET);
    out_val &= ~(1 << BOARD_LED_PIN);
    REG32(PAI_EVB_GPIO_BASE + GPIO_OUT_OFFSET) = out_val;
}
#endif

/****************************************************************************
 * Name: pai_evb_button_initialize
 *
 * Description:
 *   Initialize button GPIO pins (Port 0, Pin 1) as input
 *
 ****************************************************************************/

#ifdef CONFIG_PAI_EVB_HAVE_BUTTON
static void pai_evb_button_initialize(void)
{
    uint32_t oe_val;

    /* Configure button GPIO as input */
    oe_val = REG32(PAI_EVB_GPIO_BASE + GPIO_OE_OFFSET);
    oe_val &= ~(1 << BOARD_BUTTON_PIN);  /* Clear bit 1 for input */
    REG32(PAI_EVB_GPIO_BASE + GPIO_OE_OFFSET) = oe_val;
}
#endif

/****************************************************************************
 * Name: pai_evb_i2c_initialize
 *
 * Description:
 *   Initialize I2C controller (I2C base at 0x1fe20000)
 *
 *   I2C Register Map:
 *   0x00: Control Register
 *   0x01: Slave Address Register
 *   0x02: Data Register
 *   0x03: Status Register
 *
 ****************************************************************************/

#ifdef CONFIG_PAI_EVB_HAVE_I2C
static void pai_evb_i2c_initialize(void)
{
    /* I2C initialization code */
    /* Set clock frequency: 100kHz */
    /* Configure I2C controller registers */

    uint32_t i2c_ctrl;

    /* Reset I2C controller */
    REG8(PAI_EVB_I2C_BASE + 0x00) = 0x00;

    /* Configure I2C clock: Ref_Clk / (2 * n) where n = divisor */
    /* For 200MHz ref and 100kHz I2C: n = 1000 */
    REG8(PAI_EVB_I2C_BASE + 0x01) = 0x3E8; /* Divisor low byte */

    /* Enable I2C */
    REG8(PAI_EVB_I2C_BASE + 0x00) = 0x80;
}
#endif

/****************************************************************************
 * Name: pai_evb_spi_initialize
 *
 * Description:
 *   Initialize SPI controller (SPI base at 0x1fee0000)
 *
 *   SPI Register Map:
 *   0x00: Control Register
 *   0x01: Clock Register
 *   0x02: Data Register
 *   0x03: Status Register
 *
 ****************************************************************************/

#ifdef CONFIG_PAI_EVB_HAVE_SPI
static void pai_evb_spi_initialize(void)
{
    /* SPI initialization code */
    /* Set clock frequency: 25MHz */
    /* Configure SPI mode and control */

    /* Reset SPI controller */
    REG32(PAI_EVB_SPI_BASE + 0x00) = 0x00;

    /* Configure SPI clock: 25MHz */
    /* SPI clock = Ref_Clk / (2^(clk_div+1)) */
    /* For 200MHz ref and 25MHz SPI: clk_div = 2 (200/2^3 = 25) */
    REG32(PAI_EVB_SPI_BASE + 0x04) = 2;

    /* Configure SPI mode: Mode 0 (CPOL=0, CPHA=0) */
    /* Enable SPI */
    REG32(PAI_EVB_SPI_BASE + 0x00) = 0x01;
}
#endif

/****************************************************************************
 * Name: pai_evb_nand_initialize
 *
 * Description:
 *   Initialize NAND Flash controller (NAND base at 0x1ffb0000)
 *
 *   NAND Register Map:
 *   0x00: Control Register
 *   0x01: Command Register
 *   0x02: Address Register
 *   0x03: Data Register
 *
 ****************************************************************************/

#ifdef CONFIG_PAI_EVB_HAVE_NAND
static void pai_evb_nand_initialize(void)
{
    /* NAND Flash initialization code */
    /* Detect NAND device and configure controller */

    /* Reset NAND controller */
    REG32(PAI_EVB_NAND_BASE + 0x00) = 0x00;

    /* Configure NAND timing */
    /* Timing parameters depend on NAND device specifications */
    REG32(PAI_EVB_NAND_BASE + 0x04) = 0x00;

    /* Enable NAND controller */
    REG32(PAI_EVB_NAND_BASE + 0x00) = 0x01;

    /* Reset NAND device */
    REG32(PAI_EVB_NAND_BASE + 0x01) = 0xFF;
}
#endif

/****************************************************************************
 * Name: pai_evb_ethernet_initialize
 *
 * Description:
 *   Initialize Ethernet MAC controller
 *
 *   Ethernet Controller Base Address: typically 0x1fe10000 + offset
 *
 ****************************************************************************/

#ifdef CONFIG_PAI_EVB_HAVE_ETHERNET
static void pai_evb_ethernet_initialize(void)
{
    /* Ethernet MAC initialization code */
    /* Configure Ethernet MAC and PHY */

    /* Reset Ethernet MAC */
    /* Configure MAC address */
    /* Initialize PHY */
    /* Configure DMA descriptors */
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_initialize
 *
 * Description:
 *   All architectures must provide the following entry point. This entry
 *   point is called early in the initialization phase -- after all
 *   memory has been configured and mapped but before any devices have
 *   been initialized.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void board_initialize(void)
{
  stringserial("PAI-EVB Board\r\n");
    /* Initialize UART for debug output first */
#ifdef CONFIG_PAI_EVB_HAVE_SERIAL
    pai_evb_uart_initialize();
    pai_evb_uart_putc('\
            ');
    pai_evb_uart_putc('P');
    pai_evb_uart_putc('A');
    pai_evb_uart_putc('I');
    pai_evb_uart_putc('-');
    pai_evb_uart_putc('E');
    pai_evb_uart_putc('V');
    pai_evb_uart_putc('B');
    pai_evb_uart_putc(' ');
    pai_evb_uart_putc('B');
    pai_evb_uart_putc('o');
    pai_evb_uart_putc('a');
    pai_evb_uart_putc('r');
    pai_evb_uart_putc('d');
    pai_evb_uart_putc('\\r');
    pai_evb_uart_putc('\
            ');
#endif

    /* Initialize LED GPIO */
#ifdef CONFIG_PAI_EVB_HAVE_LED
    pai_evb_led_initialize();
#endif

    /* Initialize Button GPIO */
#ifdef CONFIG_PAI_EVB_HAVE_BUTTON
    pai_evb_button_initialize();
#endif

    /* Initialize I2C interface */
#ifdef CONFIG_PAI_EVB_HAVE_I2C
    pai_evb_i2c_initialize();
#endif

    /* Initialize SPI interface */
#ifdef CONFIG_PAI_EVB_HAVE_SPI
    pai_evb_spi_initialize();
#endif
}

/****************************************************************************
 * Name: board_late_initialize
 *
 * Description:
 *   If CONFIG_BOARD_LATE_INITIALIZE is selected, then additional
 *   initialization will be performed in the boot-up sequence. This
 *   function will be called after up_initialize() has been called and
 *   after all of the initial configured devices have been brought up.
 *   New, non-critical I/O devices should be initialized here.
 *
 ****************************************************************************/

#ifdef CONFIG_BOARD_LATE_INITIALIZE
void board_late_initialize(void)
{
    /* Initialize NAND Flash (optional) */
#ifdef CONFIG_PAI_EVB_HAVE_NAND
    pai_evb_nand_initialize();
#endif

    /* Initialize Ethernet (optional) */
#ifdef CONFIG_PAI_EVB_HAVE_ETHERNET
    pai_evb_ethernet_initialize();
#endif
}
#endif

/****************************************************************************
 * Name: board_led_on
 *
 * Description:
 *   Set the LED on (output high)
 *
 * Input Parameters:
 *   led - LED number (0 for single LED)
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void board_led_on(int led)
{
#ifdef CONFIG_PAI_EVB_HAVE_LED
    uint32_t val = REG32(PAI_EVB_GPIO_BASE + GPIO_OUT_OFFSET);
    val |= (1 << BOARD_LED_PIN);
    REG32(PAI_EVB_GPIO_BASE + GPIO_OUT_OFFSET) = val;
#endif
}

/****************************************************************************
 * Name: board_led_off
 *
 * Description:
 *   Set the LED off (output low)
 *
 * Input Parameters:
 *   led - LED number (0 for single LED)
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void board_led_off(int led)
{
#ifdef CONFIG_PAI_EVB_HAVE_LED
    uint32_t val = REG32(PAI_EVB_GPIO_BASE + GPIO_OUT_OFFSET);
    val &= ~(1 << BOARD_LED_PIN);
    REG32(PAI_EVB_GPIO_BASE + GPIO_OUT_OFFSET) = val;
#endif
}

/****************************************************************************
 * Name: board_getbutton
 *
 * Description:
 *   Get the current state of a button. If a button is pressed, this
 *   function should return 1; if not pressed, it should return 0.
 *
 * Input Parameters:
 *   button - Button number (0 for single button)
 *
 * Returned Value:
 *   1 if button pressed, 0 if not pressed
 *
 ****************************************************************************/

int board_getbutton(int button)
{
#ifdef CONFIG_PAI_EVB_HAVE_BUTTON
    uint32_t val = REG32(PAI_EVB_GPIO_BASE + GPIO_IN_OFFSET);
    return ((val >> BOARD_BUTTON_PIN) & 1) ? 1 : 0;
#else
    return 0;
#endif
}
