/****************************************************************************
 * libs/libc/machine/loongarch/arch_memset.S
 *
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 * SPDX-FileCopyrightText: 2026  Loongson Inc. All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the FreeBSD License.   This program is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
 * including the implied warranties of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  A copy of this license is available at
 * http://www.opensource.org/licenses.
 *
 ****************************************************************************/

#include "libc.h"

#ifdef LIBC_BUILD_MEMSET

//#include "asm.h"
#include <arch/irq.h>

.text
.global ARCH_LIBCFUN(memset)
.type	ARCH_LIBCFUN(memset), @function
.align	3

/************************************************************************************
 * Name: memset
 *
 * void *memset(void *s, int c, size_t n)
 *
 * Optimized with 32xSZREG unrolled loop and .option norvc.
 ************************************************************************************/
ARCH_LIBCFUN(memset):
  move a3, a0
  beqz  a2, .Ldone

  bstrpick.d  a1, a1, 7, 0
  slli.d  t0, a1, 8
  or      a1, a1, t0
  slli.d  t0, a1, 16
  or      a1, a1, t0
  slli.d  t0, a1, 32
  or      t0, a1, t0

  sltui   t1, a2, 8
  bnez    t1, .Lbyte_loop

  andi    t1, a3, 7
  beqz    t1, .Laligned_entry

  li.d    t2, 8
  sub.d   t1, t2, t1

.Lalign_head_loop:
  st.b    a1, a3, 0
  addi.d  a3, a3, 1
  addi.d  a2, a2, -1
  addi.d  t1, t1, -1
  bnez    t1, .Lalign_head_loop

.Laligned_entry:
  srli.d  t1, a2, 3
  beqz    t1, .Lbyte_loop

.Lword_loop:
  st.d    t0, a3, 0
  addi.d  a3, a3, 8
  addi.d  a2, a2, -8
  addi.d  t1, t1, -1
  bnez    t1, .Lword_loop

.Lbyte_loop:
  beqz    a2, .Ldone
.Lbyte_single:
  st.b    a1, a3, 0
  addi.d  a3, a3, 1
  addi.d  a2, a2, -1
  bnez    a2, .Lbyte_single

.Ldone:
  jirl    zero, ra, 0

  .size ARCH_LIBCFUN(memset), . - ARCH_LIBCFUN(memset)
#endif
