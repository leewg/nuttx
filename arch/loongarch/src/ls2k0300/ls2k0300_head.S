/****************************************************************************
 * arch/loongarch/src/ls2k0300/loongarch64_start.S
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#include <nuttx/config.h>
#include <arch/arch.h>
#include <arch/csr.h>
#include <arch/irq.h>
#include <arch/ls2k0300/chip.h>
#include "la64_percpu.h"

#define TTYDBG(x)  \
  .section .rodata;98: .asciz x; .text; la a0, 98b; bl stringserial

#define PRINTSTR TTYDBG

  .text
  .global __start
  .global _start
  .global tgt_putc
  .global initserial
  .global stringserial
  .global hexserial
  .global hexserial64
  .type __start, @function
  .type tgt_putc, @function
  .type initserial, @function
  .type stringserial, @function
  .type hexserial, @function
  .type hexserial64, @function

tgt_putc:
  li.d  a1, TO_UNCAC(UART0_BASE)
1:
  ld.b  a2, a1, 0x5
  andi  a2, a2, 0x20
  beqz  a2, 1b

  st.b  a0, a1, 0

  jirl  zero, ra, 0
  .size tgt_putc, . - tgt_putc

stringserial:
  or  a4, ra, zero
  sub.d a3, a0, s0
  ld.bu a0, a3, 0
1:
  beqz  a0, 2f
  bl  tgt_putc

  addi.d  a3, a3, 1
  ld.bu a0, a3, 0
  b 1b

2:
  ori ra, a4, 0
  jirl  zero, ra, 0
  .size stringserial, . - stringserial

hexserial:
  ori a4, ra, 0
  ori a3, a0, 0
  li.d  a5, 8
1:
  rotri.w a0, a3, 28
  or  a3, a0, zero
  andi  a0, a0, 0xf

  la  a1, hexchar
  sub.d a1, a1, s0

  add.d a1, a1, a0
  ld.bu a0, a1, 0

  bl  tgt_putc

  addi.d  a5, a5, -1
  bnez  a5, 1b

  ori ra, a4, 0
  jirl  zero, ra, 0
  .size hexserial, . - hexserial

hexserial64:
  ori   a4, ra, 0
  ori   a3, a0, 0
  li.d  a5, 16
1:
  rotri.d a0, a3, 60
  or      a3, a0, zero
  andi    a0, a0, 0xf

  la    a1, hexchar
  sub.d a1, a1, s0

  add.d a1, a1, a0
  ld.bu a0, a1, 0

  bl  tgt_putc

  addi.d  a5, a5, -1
  bnez  a5, 1b

  ori ra, a4, 0
  jirl  zero, ra, 0
  .size hexserial64, . - hexserial64

  .section  .radata
hexchar:
  .ascii  "0123456789abcdef"
  .text
  .align 5

#define PRINT_CSR(offset) \
  PRINTSTR("\r\ncsr 0x"); \
  li.w  a0, offset; \
  bl  hexserial;  \
  PRINTSTR(" ->0x");  \
  csrrd a0, offset; \
  bl  hexserial64;  \
  PRINTSTR("\r\n");

initserial:
  li.d  a0, TO_UNCAC(UART0_BASE)
  li.w  a1, 0x80
  st.b  a1, a0, 3

  li.w  a1, UART_DIV_HI
  st.b  a1, a0, 1
  li.w  a1, UART_DIV_LO
  st.b  a1, a0, 0
  li.w  a1, 3
  st.b  a1, a0, 3

  li.w  a1, 71
  st.b  a1, a0, 2
  jirl  zero, ra, 0

  .size initserial, . - initserial

/****************************************************************************
 * Entry Point
 *
 * Bootloader (PMON or U-Boot) enters here with:
 * - a0: pmon magic number (0x0
 * - a1: start of exception handlers
 * - a2: start of environment strings
 * - a3: start of monitor routines
 * - sp: stack pointer already set
 *
 ****************************************************************************/

__start:
_start:

  /* Disable global interrupt */
  //li.d  t0, CSR_CRMD_IE
  //csrxchg zero, t0, LA_CSR_CRMD
  li.w  t0, 0xb0    # DATM=0 1,, DATF=01, PG=1, DA=0, IE=0, PLV=00
  csrwr t0, LA_CSR_CRMD

  /* set interrupt mask 0x1fff */
  csrrd   t0, LA_CSR_ECFG
  li.w    t1, CSR_ECFG_IM
  or      t0, t0, t1
  csrwr   t0, LA_CSR_ECFG

  /* disable tick interrupt */
  csrwr zero, LA_CSR_TCFG

  /* disable all device interrupts */
  li.d  t5, TO_UNCAC(0x16001400)
  li.w  t6, 0xffffffff
  st.w  t6, t5, 0x2c
  st.w  t6, t5, 0x6c
  li.d  t5, TO_UNCAC(0x16001600)
  st.w  zero, t5, 0x0
  st.w  zero, t5, 0x4
  st.w  zero, t5, 0x8
  st.w  zero, t5, 0xc

  /* Initialize BSS section */
  la.global  t0, _sbss
  la.global  t1, _ebss
1:
  bgeu  t0, t1, 2f
  st.d  zero, t0, 0
  addi.d  t0, t0, 8
  b     1b
2:
  /* Initialize stack pointer (already done by bootloader, but ensure it) */
  la.abs sp, g_idle_topstack

  /* GPIO aux to UART0 */
  li.d  t0, TO_UNCAC(0x16000498)
  ld.w  t1, t0, 0x0
  li.w  t2, 0xf0000
  or    t1, t1, t2
  st.w  t1, t0, 0x0

  li.d  a0, TO_UNCAC(0x16100000)
  bl  initserial

  PRINTSTR("PMOM -> Nuttx: Jump to ls2k0300_start\r\n")

  bl  __ls2k0300_start

9:
  b   9b

  .end __start
  .size __start, . - __start
