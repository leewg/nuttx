/****************************************************************************
 * arch/loongarch/src/common/la64_macros.h
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

#ifndef __ARCH_LOONGARCH_SRC_COMMON_LA64_MACROS_H
#define __ARCH_LOONGARCH_SRC_COMMON_LA64_MACROS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <arch/arch.h>
#include <arch/csr.h>
#include <arch/irq.h>

#include <sys/types.h>

#include "la64_internal.h"
#include "la64_percpu.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#ifdef __ASSEMBLY__
.macro BACKUP_T0T1
  csrwr t0, LOONGARCH_CSR_KS0
  csrwr t1, LOONGARCH_CSR_KS1
.endm

.macro SAVE_ALL
  csrwr sp, LOONGARCH_CSR_KS2

  csrrd sp, LOONGARCH_CSR_KS3
  /* if KS3 un-init */
  beqz  sp, 1f

  ld.d  t0, sp, PERCPU_CURR_REGS_OFFSET
  bnez  t0, 1f

#if CONFIG_ARCH_INTERRUPTSTACK > 0
  ld.d  sp, sp, PERCPU_IRQ_STACK_OFFSET
  b     2f
#endif
1:
  csrrd   sp, LOONGARCH_CSR_KS2

2:
  addi.d  sp, sp, -XCPTCONTEXT_SIZE

  st.d  ra, sp, REG_RA
  st.d  tp, sp, REG_TP
  st.d  a0, sp, REG_A0
  st.d  a1, sp, REG_A1
  st.d  a2, sp, REG_A2
  st.d  a3, sp, REG_A3
  st.d  a4, sp, REG_A4
  st.d  a5, sp, REG_A5
  st.d  a6, sp, REG_A6
  st.d  a7, sp, REG_A7
  st.d  t0, sp, REG_T0
  st.d  t1, sp, REG_T1
  st.d  t2, sp, REG_T2
  st.d  t3, sp, REG_T3
  st.d  t4, sp, REG_T4
  st.d  t5, sp, REG_T5
  st.d  t6, sp, REG_T6
  st.d  t7, sp, REG_T7
  st.d  t8, sp, REG_T8
  st.d  fp, sp, REG_FP
  st.d  s0, sp, REG_S0
  st.d  s1, sp, REG_S1
  st.d  s2, sp, REG_S2
  st.d  s3, sp, REG_S3
  st.d  s4, sp, REG_S4
  st.d  s5, sp, REG_S5
  st.d  s6, sp, REG_S6
  st.d  s7, sp, REG_S7
  st.d  s8, sp, REG_S8

  csrrd t0, LOONGARCH_CSR_KS0
  st.d  t0, sp, REG_T0

  csrrd t0, LOONGARCH_CSR_KS1
  st.d  t0, sp, REG_T1

  csrrd t0, LOONGARCH_CSR_KS2
  st.d  t0, sp, REG_SP

  csrrd   t0, LOONGARCH_CSR_ERA
  st.d    t0, sp, REG_ERA
  csrrd   t0, LOONGARCH_CSR_PRMD
  st.d    t0, sp, REG_PRMD
  csrrd   t0, LOONGARCH_CSR_CRMD
  st.d    t0, sp, REG_CRMD
  csrrd   t0, LOONGARCH_CSR_ESTAT
  st.d    t0, sp, REG_ESTAT
  csrrd   t0, LOONGARCH_CSR_BADV
  st.d    t0, sp, REG_BADV

  csrrd     t0, LOONGARCH_CSR_EUEN
  st.d      t0, sp, REG_EUEN
  bstrins.d t0, zero, 0, 0
  csrwr     t0, LOONGARCH_CSR_EUEN

  /* update Per-CPU's current_reg point to the current context frame base address after the current push */
  csrrd t0, LOONGARCH_CSR_KS3
  beqz  t0, 3f

  ld.d  t1, t0, PERCPU_CURR_REGS_OFFSET
  st.d  t1, sp, REG_BADV
  st.d  sp, t0, PERCPU_CURR_REGS_OFFSET
3:
.endm

/****************************************************************************
 * RESTORE_ALL_AND_RET
 ****************************************************************************/
.macro RELOAD_T0T1
  csrrd t0, LOONGARCH_CSR_KS0
  csrrd t1, LOONGARCH_CSR_KS1
.endm

.macro RESTORE_ALL_AND_RET
  csrrd t0, LOONGARCH_CSR_KS3
  beqz  t0, 1f
  ld.d  t1, sp, REG_BADV
  st.d  t1, t0, PERCPU_CURR_REGS_OFFSET
1:

  /* 1. Restore CSR */
  ld.d  t0, sp, REG_ERA
  csrwr t0, LOONGARCH_CSR_ERA
  ld.d  t0, sp, REG_PRMD
  csrwr t0, LOONGARCH_CSR_PRMD
  ld.d  t0, sp, REG_CRMD
  csrwr t0, LOONGARCH_CSR_CRMD

  ld.d  t0, sp, REG_EUEN
  csrwr t0, LOONGARCH_CSR_EUEN

  /* 2. Restore GPR */
  ld.d  ra, sp, REG_RA
  ld.d  tp, sp, REG_TP
  ld.d  a0, sp, REG_A0
  ld.d  a1, sp, REG_A1
  ld.d  a2, sp, REG_A2
  ld.d  a3, sp, REG_A3
  ld.d  a4, sp, REG_A4
  ld.d  a5, sp, REG_A5
  ld.d  a6, sp, REG_A6
  ld.d  a7, sp, REG_A7
  ld.d  t2, sp, REG_T2
  ld.d  t3, sp, REG_T3
  ld.d  t4, sp, REG_T4
  ld.d  t5, sp, REG_T5
  ld.d  t6, sp, REG_T6
  ld.d  t7, sp, REG_T7
  ld.d  t8, sp, REG_T8
  ld.d  fp, sp, REG_FP
  ld.d  s0, sp, REG_S0
  ld.d  s1, sp, REG_S1
  ld.d  s2, sp, REG_S2
  ld.d  s3, sp, REG_S3
  ld.d  s4, sp, REG_S4
  ld.d  s5, sp, REG_S5
  ld.d  s6, sp, REG_S6
  ld.d  s7, sp, REG_S7
  ld.d  s8, sp, REG_S8

  ld.d  t0, sp, REG_T0
  ld.d  t1, sp, REG_T1
  ld.d  sp, sp, REG_SP
  ertn
 .endm
#endif /* __ASSEMBLY__ */
#endif /* __ARCH_LOONGARCH_SRC_COMMON_LA64_MACROS_H */
