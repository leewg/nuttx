/****************************************************************************
 * arch/loongarch/src/common/la64_registerdump.c
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

#include <stdio.h>
#include <stdint.h>
#include <nuttx/debug.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include "la64_internal.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_dump_register
 ****************************************************************************/

void up_dump_register(void *dumpregs)
{
  volatile uintreg_t *regs = dumpregs ? dumpregs : running_regs();

  _alert("  ERA  : %0*" PRIxPTR "  PRMD : %0*" PRIxPTR "  CRMD : %0*" PRIxPTR "\n",
         regs[REG_ERA], regs[REG_PRMD], regs[REG_CRMD]);
  _alert("  ESTAT: %0*" PRIxPTR "  BADV : %0*" PRIxPTR "  ECFG : %0*" PRIxPTR "  EUEN : %0*" PRIxPTR "\n",
      regs[REG_ESTAT], regs[REG_BADV], regs[REG_ECFG], regs[REG_EUEN]);

  _alert("  ra/r1 : %0*" PRIxPTR "  tp/r2 : %0*" PRIxPTR "  sp/r3 : %0*" PRIxPTR "\n",
      regs[REG_R1], regs[REG_R2], regs[REG_R3]);

  _alert("  a0/r4 : %0*" PRIxPTR "  a1/r5 : %0*" PRIxPTR "  a2/r6 : %0*" PRIxPTR "  a3/r7 : %0*" PRIxPTR "\n", regs[REG_R4], regs[REG_R5], regs[REG_R6], regs[REG_R7]);

  _alert("  a4/r8 : %0*" PRIxPTR "  a5/r9 : %0*" PRIxPTR "  a6/r10: %0*" PRIxPTR "  a7/r11: %0*" PRIxPTR "\n",
         regs[REG_R8], regs[REG_R9], regs[REG_R10], regs[REG_R11]);

  _alert("  t0/r12: %0*" PRIxPTR "  t1/r13: %0*" PRIxPTR "  t2/r14: %0*" PRIxPTR "  t3/r15: %0*" PRIxPTR "\n",
         regs[REG_R12], regs[REG_R13], regs[REG_R14], regs[REG_R15]);

  _alert("  t4/r16: %0*" PRIxPTR "  t5/r17: %0*" PRIxPTR "  t6/r18: %0*" PRIxPTR "  t7/r19: %0*" PRIxPTR "\n",
         regs[REG_R16], regs[REG_R17], regs[REG_R18], regs[REG_R19]);

  _alert("  t8/r20: %0*" PRIxPTR "  s9/r21: %0*" PRIxPTR "  fp/r22: %0*" PRIxPTR "\n",
         regs[REG_R20], regs[REG_R21], regs[REG_R22]);

  _alert("  s0/r23: %0*" PRIxPTR "  s1/r24: %0*" PRIxPTR "  s2/r25: %0*" PRIxPTR "  s3/r26: %0*" PRIxPTR "\n",
         regs[REG_R23], regs[REG_R24], regs[REG_R25], regs[REG_R26]);

  _alert("  s4/r27: %0*" PRIxPTR "  s5/r28: %0*" PRIxPTR "  s6/r29: %0*" PRIxPTR "  s7/r30: %0*" PRIxPTR "  s8/r31: %0*" PRIxPTR "\n",
         regs[REG_R27], regs[REG_R28], regs[REG_R29], regs[REG_R30], regs[REG_R31]);
}
