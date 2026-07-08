/****************************************************************************
 * arch/loongarch64/src/common/la64_fatal.S
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
#include <arch/csr.h>
#include <arch/irq.h>

#include "la64_internal.h"
#include "la64_macros.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Symbols
 ****************************************************************************/

/****************************************************************************
 * Name: la64_machine_check_entry
 *
 * Description:
 *
 ****************************************************************************/

  .section .text
  .p2align  4
  .global la64_machine_check_entry

  .extern la64_fatal_handler

la64_machine_check_entry:
    la.pcrel  t0, g_cpu_fatal_panic_stack_top
    addi.d    sp, t0, -XCPTCONTEXT_SIZE

    SAVE_ALL

    csrrd   a0, LOONGARCH_CSR_ESTAT
    csrrd   a1, LOONGARCH_CSR_ERA
    csrrd   a2, LOONGARCH_CSR_BADV
    move    a3, sp

    bl  la64_fatal_handler

_fatal_dead_loop:
    b   _fatal_dead_loop

/****************************************************************************
 * Data Section (Fatal Panic Stack Space)
 ****************************************************************************/
#define FATAL_STACK_SIZE  2048
  .section .bss
  .balign 16
  .global g_cpu_fatal_panic_stack
  .global g_cpu_fatal_panic_stack_top
  .type g_cpu_fatal_panic_stack, object
  .type   g_cpu_fatal_panic_stack_top, object

g_cpu_fatal_panic_stack:
  .skip (FATAL_STACK_SIZE * CONFIG_SMP_NCPUS)
g_cpu_fatal_panic_stack_top:
  .size g_cpu_fatal_panic_stack_top, 0
  .size g_cpu_fatal_panic_stack, .-g_cpu_fatal_panic_stack
