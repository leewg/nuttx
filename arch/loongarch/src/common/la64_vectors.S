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

  .align  5
SYM_FUNC_START(handle_vint)

  li.d  a0, '$'
  bl  tgt_putc
  li.d  a0, '\r'
  bl  tgt_putc
  li.d  a0, '\n'
  bl  tgt_putc

  BACKUP_T0T1
  SAVE_ALL
  move  a0, sp
  move  a1, sp
  la.abs  t0, do_vint
  jirl  ra, t0, 0
  RESTORE_ALL_AND_RET
SYM_FUNC_END(handle_vint)

SYM_FUNC_START(except_vec_cex)
  b cache_parity_error
SYM_FUNC_END(except_vec_cex)

.macro build_prep_badv
  csrrd t0, LOONGARCH_CSR_BADV
  st.d  t0, sp, REG_BADV
.endm

.macro build_prep_fcsr
  movfcsr2gr  a1, fcsr0
.endm

.macro build_prep_none
.endm

.macro BUILD_HANDLER exception handler prep
  .align 5
  SYM_FUNC_START(handle_\exception)

  li.d  a0, '@'
  bl  tgt_putc
  li.d  a0, '\r'
  bl  tgt_putc
  li.d  a0, '\n'
  bl  tgt_putc

  BACKUP_T0T1
  SAVE_ALL
  build_prep_\prep
  move  a0, sp
  move  a1, sp
  la.abs  t0, do_\handler
  jirl  ra, t0, 0
  RESTORE_ALL_AND_RET
  SYM_FUNC_END(handle_\exception)
.endm

#if 0
BUILD_HANDLER ade ade badv
BUILD_HANDLER ale ale badv
BUILD_HANDLER bp bp none
BUILD_HANDLER fpe fpe fcsr
BUILD_HANDLER fpu fpu none
BUILD_HANDLER lbt lbt none
BUILD_HANDLER ri ri none
#endif
BUILD_HANDLER reserved reserved none

SYM_FUNC_START(handle_sys)
  la.abs  t0, handle_syscall
  jirl  zero, t0, 0
SYM_FUNC_END(handle_sys)

SYM_FUNC_START(handle_syscall)
  li.d  a0, '@'
  bl  tgt_putc
SYM_FUNC_END(handle_syscall)

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
