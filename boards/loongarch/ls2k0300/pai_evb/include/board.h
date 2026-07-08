/****************************************************************************
 * boards/loongarch/ls2k0300/pai_evb/include/board.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * PAI-EVB Board Header - Hardware Definitions
 * Loongson LS2K0300 Single-core 64-bit Processor @ 800MHz
 *
 ****************************************************************************/

#ifndef __BOARDS_LOONGARCH_LS2K0300_PAI_EVB_INCLUDE_BOARD_H
#define __BOARDS_LOONGARCH_LS2K0300_PAI_EVB_INCLUDE_BOARD_H

#include <nuttx/config.h>

/* Board Identity */
#define BOARD_NAME               "PAI-EVB"
#define BOARD_PROCESSOR          "Loongson LS2K0300"
#define BOARD_CLOCK_FREQ         800000000   /* 800MHz */
#define BOARD_TIMER_FREQ         (BOARD_CLOCK_FREQ / 4)

/* LED Configuration */
#define BOARD_LED_PORT           0
#define BOARD_LED_PIN            0
#define BOARD_LED_ON             1
#define BOARD_LED_OFF            0

/* Button Configuration */
#define BOARD_BUTTON_PORT        0
#define BOARD_BUTTON_PIN         1

/* UART Configuration */
#define BOARD_UART_BAUDRATE      115200
#define BOARD_UART_BITS          8
#define BOARD_UART_PARITY        0          /* No parity */
#define BOARD_UART_STOPBITS      1
#define BOARD_HAVE_SERIAL_CONSOLE 1

/* Memory Map */
#define BOARD_RAM_START          0x80000000
#define BOARD_RAM_END            0x90000000 /* 256MB */
#define BOARD_RAM_SIZE           0x10000000

#define BOARD_HEAP_BASE          0x80300000
#define BOARD_HEAP_SIZE          0x04000000 /* 64MB */

#define BOARD_STACK_BASE         0x84300000
#define BOARD_STACK_SIZE         0x01000000 /* 16MB */

#define BOARD_FLASH_BASE         0x1c000000
#define BOARD_FLASH_SIZE         0x04000000 /* 64MB */

/* PAI-EVB Hardware Base Addresses */
#define PAI_EVB_GPIO_BASE        0x1fe10000
#define PAI_EVB_UART_BASE        0x1fe00000
#define PAI_EVB_UART0_BASE       (PAI_EVB_UART_BASE + 0x00000)
#define PAI_EVB_UART1_BASE       (PAI_EVB_UART_BASE + 0x01000)
#define PAI_EVB_UART2_BASE       (PAI_EVB_UART_BASE + 0x02000)
#define PAI_EVB_UART3_BASE       (PAI_EVB_UART_BASE + 0x03000)
#define PAI_EVB_TIMER_BASE       0x1fe00100
#define PAI_EVB_WATCHDOG_BASE    0x1fe00140
#define PAI_EVB_I2C_BASE         0x1fe20000
#define PAI_EVB_SPI_BASE         0x1fee0000
#define PAI_EVB_NAND_BASE        0x1ffb0000

/* UART Register Offsets */
#define UART_DATA_OFFSET         0x00
#define UART_IER_OFFSET          0x01
#define UART_ISR_OFFSET          0x02
#define UART_LCR_OFFSET          0x03
#define UART_MCR_OFFSET          0x04
#define UART_LSR_OFFSET          0x05
#define UART_MSR_OFFSET          0x06
#define UART_SCR_OFFSET          0x07

/* UART Line Control Register Bits */
#define UART_LCR_8BITS           0x03
#define UART_LCR_STOPBITS_1      0x00
#define UART_LCR_PARITY_ENABLE   0x08
#define UART_LCR_DLAB            0x80

/* UART Line Status Register Bits */
#define UART_LSR_DR              0x01  /* Data Ready */
#define UART_LSR_THRE            0x20  /* Transmitter Holding Register Empty */
#define UART_LSR_TEMT            0x40  /* Transmitter Empty */

/* GPIO Register Offsets */
#define GPIO_OUT_OFFSET          0x00
#define GPIO_IN_OFFSET           0x04
#define GPIO_OE_OFFSET           0x08

/* Interface Support */
#define BOARD_HAVE_I2C           1
#define BOARD_I2C_CLOCK_FREQ     100000
#define BOARD_HAVE_SPI           1
#define BOARD_SPI_CLOCK_FREQ     25000000
#define BOARD_HAVE_ETHERNET      1
#define BOARD_HAVE_NAND          1

/* Public Functions */
#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

    void board_initialize(void);
#ifdef CONFIG_BOARD_LATE_INITIALIZE
    void board_late_initialize(void);
#endif
    void board_led_on(int led);
    void board_led_off(int led);
    int board_getbutton(int button);

#ifdef __cplusplus
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* __BOARDS_LOONGARCH_LS2K0300_PAI_EVB_INCLUDE_BOARD_H */
