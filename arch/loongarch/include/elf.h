/****************************************************************************
 * arch/loongarch/include/elf.h
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

#ifndef __ARCH_LOONGARCH_INCLUDE_ELF_H
#define __ARCH_LOONGARCH_INCLUDE_ELF_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* ELF header e_flags defines. */

/* The ABI of a file. */
#define EF_LARCH_ABI_LP32   0x00000001  /* LP32 ABI.  */
#define EF_LARCH_ABI_LP64   0x00000003  /* LP64 ABI  */
#define EF_LARCH_ABI        0x00000003

/* LoongArch relocation types used by the dynamic linker */
#define R_LARCH_NONE            0
#define R_LARCH_32              1
#define R_LARCH_64              2
#define R_LARCH_RELATIVE        3
#define R_LARCH_COPY            4
#define R_LARCH_JUMP_SLOT       5
#define R_LARCH_TLS_DTPMOD32    6
#define R_LARCH_TLS_DTPMOD64    7
#define R_LARCH_TLS_DTPREL32    8
#define R_LARCH_TLS_DTPREL64    9
#define R_LARCH_TLS_TPREL32     10
#define R_LARCH_TLS_TPREL64     11
#define R_LARCH_IRELATIVE       12
#define R_LARCH_MARK_LA         20
#define R_LARCH_MARK_PCREL      21
#define R_LARCH_SOP_PUSH_PCREL      22
#define R_LARCH_SOP_PUSH_ABSOLUTE   23
#define R_LARCH_SOP_PUSH_DUP        24
#define R_LARCH_SOP_PUSH_GPREL      25
#define R_LARCH_SOP_PUSH_TLS_TPREL  26
#define R_LARCH_SOP_PUSH_TLS_GOT    27
#define R_LARCH_SOP_PUSH_TLS_GD     28
#define R_LARCH_SOP_PUSH_PLT_PCREL  29
#define R_LARCH_SOP_ASSERT          30
#define R_LARCH_SOP_NOT             31
#define R_LARCH_SOP_SUB             32
#define R_LARCH_SOP_SL              33
#define R_LARCH_SOP_SR              34
#define R_LARCH_SOP_ADD             35
#define R_LARCH_SOP_AND             36
#define R_LARCH_SOP_IF_ELSE         37
#define R_LARCH_SOP_POP_32_S_10_5   38
#define R_LARCH_SOP_POP_32_U_10_12  39
#define R_LARCH_SOP_POP_32_S_10_12  40
#define R_LARCH_SOP_POP_32_S_10_16  41
#define R_LARCH_SOP_POP_32_S_10_16_S2       42
#define R_LARCH_SOP_POP_32_S_5_20           43
#define R_LARCH_SOP_POP_32_S_0_5_10_16_S2   44
#define R_LARCH_SOP_POP_32_S_0_10_10_16_S2  45
#define R_LARCH_SOP_POP_32_U        46
#define R_LARCH_ADD8                47
#define R_LARCH_ADD16               48
#define R_LARCH_ADD24               49
#define R_LARCH_ADD32               50
#define R_LARCH_ADD64               51
#define R_LARCH_SUB8                52
#define R_LARCH_SUB16               53
#define R_LARCH_SUB24               54
#define R_LARCH_SUB32               55
#define R_LARCH_SUB64               56
#define R_LARCH_GNU_VTINHERIT       57
#define R_LARCH_GNU_VTENTRY         58

#define EM_ARCH                     EM_LOONGARCH
#define EF_FLAG                     0


/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifndef __ASSEMBLY__

/* ELF register definitions */

/*
 * General purpose have the following registers:
 *  Register    Number
 *  GPRs          32
 *  ORIG_A0       1
 *  ERA           1
 *  BADVADDR      1
 *  CRMD          1
 *  PRMD          1
 *  EUEN          1
 *  ECFG          1
 *  ESTAT         1
 *  Reserved      5
 */
#define ELF_NGREG   45

/*
 * Floating point have the following registers:
 *  Register    Number
 *  FPR       32
 *  FCC       1
 *  FCSR      1
 */
#define ELF_NFPREG	34

typedef unsigned long elf_greg_t;
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef double elf_fpreg_t;
typedef elf_fpreg_t elf_fpregset_t[ELF_NFPREG];

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_LOONGARCH_INCLUDE_ELF_H */
