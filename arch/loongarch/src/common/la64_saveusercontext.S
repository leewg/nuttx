/****************************************************************************
 * arch/loongarch/src/common/la64_saveusercontext.S
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
#include <arch/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Symbols
 ****************************************************************************/

/****************************************************************************
 * Name: up_saveusercontext
 *
 * Description:
 *   Save the current thread context
 *
 ****************************************************************************/

  .section .text
  .global up_saveusercontext
  .align  8

up_saveusercontext:

  st.d    $r0,  $r4, (REG_R0 * 8)
  st.d    $r1,  $r4, (REG_R1 * 8)     /* $ra */
  st.d    $r2,  $r4, (REG_R2 * 8)     /* $tp */
  st.d    $r3,  $r4, (REG_R3 * 8)     /* $sp */

  addi.d  t0,  zero, 1
  st.d    t0,  $r4, (REG_R4 * 8)

  st.d    $r5,  $r4, (REG_R5 * 8)     /* $a1 */
  st.d    $r6,  $r4, (REG_R6 * 8)     /* $a2 */
  st.d    $r7,  $r4, (REG_R7 * 8)     /* $a3 */
  st.d    $r8,  $r4, (REG_R8 * 8)     /* $a4 */
  st.d    $r9,  $r4, (REG_R9 * 8)     /* $a5 */
  st.d    $r10, $r4, (REG_R10 * 8)    /* $a6 */
  st.d    $r11, $r4, (REG_R11 * 8)    /* $a7 */

  st.d    $r12, $r4, (REG_R12 * 8)    /* $t0 */
  st.d    $r13, $r4, (REG_R13 * 8)    /* $t1 */
  st.d    $r14, $r4, (REG_R14 * 8)    /* $t2 */
  st.d    $r15, $r4, (REG_R15 * 8)    /* $t3 */
  st.d    $r16, $r4, (REG_R16 * 8)    /* $t4 */
  st.d    $r17, $r4, (REG_R17 * 8)    /* $t5 */
  st.d    $r18, $r4, (REG_R18 * 8)    /* $t6 */
  st.d    $r19, $r4, (REG_R19 * 8)    /* $t7 */
  st.d    $r20, $r4, (REG_R20 * 8)    /* $t8 */

  st.d    $r21, $r4, (REG_R21 * 8)
  st.d    $r22, $r4, (REG_R22 * 8)    /* $fp / $s9 */
  st.d    $r23, $r4, (REG_R23 * 8)    /* $s0 */
  st.d    $r24, $r4, (REG_R24 * 8)    /* $s1 */
  st.d    $r25, $r4, (REG_R25 * 8)    /* $s2 */
  st.d    $r26, $r4, (REG_R26 * 8)    /* $s3 */
  st.d    $r27, $r4, (REG_R27 * 8)    /* $s4 */
  st.d    $r28, $r4, (REG_R28 * 8)    /* $s5 */
  st.d    $r29, $r4, (REG_R29 * 8)    /* $s6 */
  st.d    $r30, $r4, (REG_R30 * 8)    /* $s7 */
  st.d    $r31, $r4, (REG_R31 * 8)    /* $s8 */

  st.d    $r1,  $r4, (REG_ERA * 8)

  csrrd   t0, LOONGARCH_CSR_BADV
  st.d    t0, $r4, (REG_BADV * 8)          /* BADV */

  csrrd   t0, LOONGARCH_CSR_PRMD
  st.d    t0, $r4, (REG_PRMD * 8)    /* PRMD */

  move    $r4,  zero
  jirl    $r0, $r1, 0

  .size	up_saveusercontext, . - up_saveusercontext
  .end
