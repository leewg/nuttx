/****************************************************************************
 * arch/loongarch/src/common/la64_vectors.S
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

#include <arch/syscall.h>
#include <arch/irq.h>
#include <arch/csr.h>
#include <arch/arch.h>

#include "la64_internal.h"
#include "la64_macros.h"

/****************************************************************************
 * Public Symbols
 ****************************************************************************/

  .file "la64_vectors.S"

  .global machine_error_entry
  .global exception_common_entry
  .global tlbrefill_error_entry
  .extern la64_vint_handler
  .extern la64_exception_handler

  /* MERR vector entry */
  .section .vectors.merr, "ax"
machine_error_entry:
  csrwr   t0, LA_CSR_MERRSAVE
  la.abs  t0, real_machine_error_entry
  jirl  zero, t0, 0

  /* TLB Refill vector entry */
  .section .vectors.tlb, "ax"
tlbrefill_error_entry:
  csrwr   t0, LA_CSR_TLBRSAVE
  la.abs  t0, real_tlbrefill_error_entry
  jirl  zero, t0, 0

  /* Common Exception vector entry (Vector 0) */
  .section .vectors.eentry, "ax"
  .balign 128
exception_common_entry:
  BACKUP_T0T1
  la.abs  t1, real_exception_entry
  jirl  zero, t1, 0

  /* Interrupt Vector entry (Vector1~13) */
.macro BUILD_INT_VECTOR num
  .balign 128
vector_\num:
  BACKUP_T0T1
  la.abs  t1, common_int_handler
  jirl  zero, t1, 0
.endm

/* 1 ~ 13 entry */
BUILD_INT_VECTOR 1   /* EENTRY + 0x080: SWI0 */
BUILD_INT_VECTOR 2   /* EENTRY + 0x100: SWI1 */
BUILD_INT_VECTOR 3   /* EENTRY + 0x180: HWI0 */
BUILD_INT_VECTOR 4   /* EENTRY + 0x200: HWI1 */
BUILD_INT_VECTOR 5   /* EENTRY + 0x280: HWI2 */
BUILD_INT_VECTOR 6   /* EENTRY + 0x300: HWI3 */
BUILD_INT_VECTOR 7   /* EENTRY + 0x380: HWI4 */
BUILD_INT_VECTOR 8   /* EENTRY + 0x400: HWI5 */
BUILD_INT_VECTOR 9   /* EENTRY + 0x480: HWI6 */
BUILD_INT_VECTOR 10  /* EENTRY + 0x500: HWI7 */
BUILD_INT_VECTOR 11  /* EENTRY + 0x580: TI   */
BUILD_INT_VECTOR 12  /* EENTRY + 0x600: IPI  */
BUILD_INT_VECTOR 13  /* EENTRY + 0x680: PMC  */

  .text
SYM_FUNC_START(common_int_handler)
  SAVE_ALL
  move  a0, sp
  la.pcrel t0, la64_vint_handler
  jirl  ra, t0, 0

  move  sp, a0
  RESTORE_ALL_AND_RET
SYM_FUNC_END(common_int_handler)

SYM_FUNC_START(real_exception_entry)
  SAVE_ALL
  move  a0, sp
  la.pcrel t0, la64_exception_handler
  jirl  ra, t0, 0

  move  sp, a0
  RESTORE_ALL_AND_RET
SYM_FUNC_END(real_exception_entry)

SYM_FUNC_START(real_machine_error_entry)
  csrrd t1, LA_CSR_MERRCTL
  andi  t1, t1, 0x1
  beqz  t1, lbl_merr_return

  csrrd t1, LA_CSR_MERRSAVE
  SAVE_ALL

  move  a0, sp
  la.abs t1, la64_fatal_handler
  jirl  ra, t1, 0

lbl_merr_return:
  csrrd t8, LA_CSR_MERRSAVE
  ertn
SYM_FUNC_END(real_machine_error_entry)

SYM_FUNC_START(real_tlbrefill_error_entry)
  csrrd t8, LA_CSR_TLBRSAVE
  SAVE_ALL

  move  a0, sp
  //la.abs t8, la64_tlbrefill_handler
  jirl  ra, t8, 0

  RESTORE_ALL_AND_RET
SYM_FUNC_END(real_tlbrefill_error_entry)

/****************************************************************************
 * Data Section (Interrupt Stack Space)
 ****************************************************************************/

#define STACK_ALLOC_SIZE (INT_STACK_SIZE * CONFIG_SMP_NCPUS)
#if CONFIG_ARCH_INTERRUPTSTACK > 15
  .section .bss
  .balign 16
  .global g_intstackalloc
  .global g_intstacktop
  .type   g_intstackalloc, object
  .type   g_intstacktop, object
g_intstackalloc:
  .skip STACKFRAME_ALIGN_UP(STACK_ALLOC_SIZE)
g_intstacktop:
  .size g_intstacktop, 0
  .size g_intstackalloc, STACKFRAME_ALIGN_DOWN(STACK_ALLOC_SIZE)
#endif
