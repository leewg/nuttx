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

#ifdef CONFIG_ARCH_FPU
.macro SAVE_FPU
  fst.d f0,  sp, TP_F0
  fst.d f1,  sp, TP_F1
  fst.d f2,  sp, TP_F2
  fst.d f3,  sp, TP_F3
  fst.d f4,  sp, TP_F4
  fst.d f5,  sp, TP_F5
  fst.d f6,  sp, TP_F6
  fst.d f7,  sp, TP_F7
  fst.d f8,  sp, TP_F8
  fst.d f9,  sp, TP_F9
  fst.d f10, sp, TP_F10
  fst.d f11, sp, TP_F11
  fst.d f12, sp, TP_F12
  fst.d f13, sp, TP_F13
  fst.d f14, sp, TP_F14
  fst.d f15, sp, TP_F15
  fst.d f16, sp, TP_F16
  fst.d f17, sp, TP_F17
  fst.d f18, sp, TP_F18
  fst.d f19, sp, TP_F19
  fst.d f20, sp, TP_F20
  fst.d f21, sp, TP_F21
  fst.d f22, sp, TP_F22
  fst.d f23, sp, TP_F23
  fst.d f24, sp, TP_F24
  fst.d f25, sp, TP_F25
  fst.d f26, sp, TP_F26
  fst.d f27, sp, TP_F27
  fst.d f28, sp, TP_F28
  fst.d f29, sp, TP_F29
  fst.d f30, sp, TP_F30
  fst.d f31, sp, TP_F31
  movcf2gr  t8, fcc0
  st.d      t8, sp, TP_FCC0
  movcf2gr  t8, fcc1
  st.d      t8, sp, TP_FCC1
  movcf2gr  t8, fcc2
  st.d      t8, sp, TP_FCC2
  movcf2gr  t8, fcc3
  st.d      t8, sp, TP_FCC3
  movcf2gr  t8, fcc4
  st.d      t8, sp, TP_FCC4
  movcf2gr  t8, fcc5
  st.d      t8, sp, TP_FCC5
  movcf2gr  t8, fcc6
  st.d      t8, sp, TP_FCC6
  movcf2gr  t8, fcc7
  st.d      t8, sp, TP_FCC7
  movfcsr2gr  t8, r0
  st.d      t8, sp, TP_FCSR0
.endm

.macro RESTORE_FPU
  fld.d f0,  sp, TP_F0
  fld.d f1,  sp, TP_F1
  fld.d f2,  sp, TP_F2
  fld.d f3,  sp, TP_F3
  fld.d f4,  sp, TP_F4
  fld.d f5,  sp, TP_F5
  fld.d f6,  sp, TP_F6
  fld.d f7,  sp, TP_F7
  fld.d f8,  sp, TP_F8
  fld.d f9,  sp, TP_F9
  fld.d f10, sp, TP_F10
  fld.d f11, sp, TP_F11
  fld.d f12, sp, TP_F12
  fld.d f13, sp, TP_F13
  fld.d f14, sp, TP_F14
  fld.d f15, sp, TP_F15
  fld.d f16, sp, TP_F16
  fld.d f17, sp, TP_F17
  fld.d f18, sp, TP_F18
  fld.d f19, sp, TP_F19
  fld.d f20, sp, TP_F20
  fld.d f21, sp, TP_F21
  fld.d f22, sp, TP_F22
  fld.d f23, sp, TP_F23
  fld.d f24, sp, TP_F24
  fld.d f25, sp, TP_F25
  fld.d f26, sp, TP_F26
  fld.d f27, sp, TP_F27
  fld.d f28, sp, TP_F28
  fld.d f29, sp, TP_F29
  fld.d f30, sp, TP_F30
  fld.d f31, sp, TP_F31
  ld.d  u0, sp, TP_FCC0
  movgr2fr  fcc0, u0
  ld.d  u0, sp, TP_FCC1
  movgr2fr  fcc1, u0
  ld.d  u0, sp, TP_FCC2
  movgr2fr  fcc2, u0
  ld.d  u0, sp, TP_FCC3
  movgr2fr  fcc3, u0
  ld.d  u0, sp, TP_FCC4
  movgr2fr  fcc4, u0
  ld.d  u0, sp, TP_FCC5
  movgr2fr  fcc5, u0
  ld.d  u0, sp, TP_FCC6
  movgr2fr  fcc6, u0
  ld.d  u0, sp, TP_FCC7
  movgr2fr  fcc7, u0
  ld.d  u0, sp, TP_FCSR0
  movgr2fr  r0, u0
.endm
#endif  // #if CONFIG_ARCH_FPU

.macro BACKUP_T0T1
  csrwr	t0, LA_CSR_KS0
  csrwr	t1, LA_CSR_KS1
.endm

.macro RELOAD_T0T1
  csrrd   t0, LA_CSR_KS0
  csrrd   t1, LA_CSR_KS1
.endm

.macro	SAVE_TEMP
  RELOAD_T0T1
  st.d	t0, sp, TP_T0
  st.d	t1, sp, TP_T1
  st.d	t2, sp, TP_T2
  st.d	t3, sp, TP_T3
  st.d	t4, sp, TP_T4
  st.d	t5, sp, TP_T5
  st.d	t6, sp, TP_T6
  st.d	t7, sp, TP_T7
  st.d	t8, sp, TP_T8
.endm

.macro	SAVE_STATIC
  st.d	s0, sp, TP_S0
  st.d	s1, sp, TP_S1
  st.d	s2, sp, TP_S2
  st.d	s3, sp, TP_S3
  st.d	s4, sp, TP_S4
  st.d	s5, sp, TP_S5
  st.d	s6, sp, TP_S6
  st.d	s7, sp, TP_S7
  st.d	s8, sp, TP_S8
.endm

.macro	SAVE_SOME
  move	t0, sp
  addi.d  sp, sp, -XCPTCONTEXT_SIZE
  st.d    t0, sp, TP_SP
  st.d    zero, sp, TP_ZERO

  csrrd   t0, LA_CSR_PRMD
  st.d    t0, sp, TP_PRMD
  csrrd   t0, LA_CSR_CRMD
  st.d    t0, sp, TP_CRMD
  csrrd   t0, LA_CSR_EUEN
  st.d    t0, sp, TP_EUEN
  csrrd   t0, LA_CSR_ECFG
  st.d    t0, sp, TP_ECFG
  csrrd   t0, LA_CSR_ESTAT
  st.d  t0, sp, TP_ESTAT

  st.d  ra, sp, TP_RA
  st.d  a0, sp, TP_A0
  st.d  a1, sp, TP_A1
  st.d  a2, sp, TP_A2
  st.d  a3, sp, TP_A3
  st.d  a4, sp, TP_A4
  st.d  a5, sp, TP_A5
  st.d  a6, sp, TP_A6
  st.d  a7, sp, TP_A7

  csrrd t0, LA_CSR_ERA
  st.d  t0, sp, TP_ERA

  st.d  tp, sp, TP_TP
  st.d  fp, sp, TP_FP

  /* Set thread_info if we're coming from user mode */
  csrrd t0, LA_CSR_PRMD
  andi  t0, t0, 0x3 /* extract pplv bit */
  beqz  t0, 9f

  st.d  u0, sp, TP_U0
  csrrd u0, LA_CSR_KS3
9:
.endm

.macro	SAVE_ALL
  SAVE_SOME
  SAVE_TEMP
  SAVE_STATIC
.endm

.macro RESTORE_TEMP
  ld.d  t0, sp, TP_T0
  ld.d  t1, sp, TP_T1
  ld.d  t2, sp, TP_T2
  ld.d  t3, sp, TP_T3
  ld.d  t4, sp, TP_T4
  ld.d  t5, sp, TP_T5
  ld.d  t6, sp, TP_T6
  ld.d  t7, sp, TP_T7
  ld.d  t8, sp, TP_T8
.endm

.macro RESTORE_STATIC
  ld.d  s0, sp, TP_S0
  ld.d  s1, sp, TP_S1
  ld.d  s2, sp, TP_S2
  ld.d  s3, sp, TP_S3
  ld.d  s4, sp, TP_S4
  ld.d  s5, sp, TP_S5
  ld.d  s6, sp, TP_S6
  ld.d  s7, sp, TP_S7
  ld.d  s8, sp, TP_S8
.endm

.macro RESTORE_SOME
  ld.d  a0, sp, TP_ERA
  csrwr a0, LA_CSR_ERA
  ld.d  a0, sp, TP_PRMD
  csrwr a0, LA_CSR_PRMD
  ld.d  ra, sp, TP_RA
  ld.d  a0, sp, TP_A0
  ld.d  a1, sp, TP_A1
  ld.d  a2, sp, TP_A2
  ld.d  a3, sp, TP_A3
  ld.d  a4, sp, TP_A4
  ld.d  a5, sp, TP_A5
  ld.d  a6, sp, TP_A6
  ld.d  a7, sp, TP_A7
  ld.d  tp, sp, TP_TP
  ld.d  fp, sp, TP_FP

  addi.d  tp, sp, XCPTCONTEXT_SIZE
  csrwr tp, LA_CSR_KS2

  ld.d  tp, sp, TP_TP
.endm

.macro RESTORE_SP_AND_RET
  ld.d  sp, sp, TP_SP
  ertn
.endm

.macro RESTORE_ALL_AND_RET
  RESTORE_STATIC
  RESTORE_TEMP
  RESTORE_SOME
  RESTORE_SP_AND_RET
.endm

/* For thread context */
.macro cpu_save_nonscratch thread
  st.d  s0, \thread, TP_S0
  st.d  s1, \thread, TP_S1
  st.d  s2, \thread, TP_S2
  st.d  s3, \thread, TP_S3
  st.d  s4, \thread, TP_S4
  st.d  s5, \thread, TP_S5
  st.d  s6, \thread, TP_S6
  st.d  s7, \thread, TP_S7
  st.d  s8, \thread, TP_S8
  st.d  sp, \thread, TP_SP
  st.d  fp, \thread, TP_FP
  st.d  tp, \thread, TP_TP
.endm

.macro cpu_save_csr_reg thread
  csrrd t1, LA_CSR_PRMD
  st.d  t1, \thread, TP_PRMD
  csrrd t1, LA_CSR_CRMD
  st.d  t1, \thread, TP_CRMD
.endm

.macro cpu_restore_nonscratch thread
  ld.d  ra, \thread, TP_RA
  ld.d  s0, \thread, TP_S0
  ld.d  s1, \thread, TP_S1
  ld.d  s2, \thread, TP_S2
  ld.d  s3, \thread, TP_S3
  ld.d  s4, \thread, TP_S4
  ld.d  s5, \thread, TP_S5
  ld.d  s6, \thread, TP_S6
  ld.d  s7, \thread, TP_S7
  ld.d  s8, \thread, TP_S8
  ld.d  fp, \thread, TP_FP
  ld.d  tp, \thread, TP_TP
.endm

.macro cpu_restore_csr_reg thread
  ld.d  t1, \thread, TP_PRMD
  csrwr t1, LA_CSR_PRMD
  ld.d  t1, \thread, TP_CRMD
  csrwr t1, LA_CSR_CRMD
.endm

.macro cpu_restore_sp thread
  ld.d  sp, \thread, TP_SP
.endm

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_LOONGARCH_SRC_COMMON_LA64_MACROS_H */
