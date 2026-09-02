/****************************************************************************
 * arch/loongarch64/include/csr.h
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

/* This file should never be included directly but, rather, only indirectly
 * through nuttx/irq.h
 */

#ifndef __ARCH_LOONGARCH64_INCLUDE_CSR_H
#define __ARCH_LOONGARCH64_INCLUDE_CSR_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/*
 *  Configure language
 */
#ifdef __ASSEMBLY__
#define _ULCAST_
#define _U64CAST_
#else
#define _ULCAST_ (unsigned long)
#define _U64CAST_ (u64)
#endif

/* Basic Control and Status Registers (CSRs) */

#define LA_CSR_CRMD      0x0   /* Current Mode Information */
#define  CSR_CRMD_WE_SHIFT      9
#define  CSR_CRMD_WE			(_ULCAST_(0x1) << CSR_CRMD_WE_SHIFT)
#define  CSR_CRMD_DACM_SHIFT		7
#define  CSR_CRMD_DACM_WIDTH		2
#define  CSR_CRMD_DACM			(_ULCAST_(0x3) << CSR_CRMD_DACM_SHIFT)
#define  CSR_CRMD_DACF_SHIFT		5
#define  CSR_CRMD_DACF_WIDTH		2
#define  CSR_CRMD_DACF			(_ULCAST_(0x3) << CSR_CRMD_DACF_SHIFT)
#define  CSR_CRMD_PG_SHIFT		4
#define  CSR_CRMD_PG			(_ULCAST_(0x1) << CSR_CRMD_PG_SHIFT)
#define  CSR_CRMD_DA_SHIFT		3
#define  CSR_CRMD_DA			(_ULCAST_(0x1) << CSR_CRMD_DA_SHIFT)
#define  CSR_CRMD_IE_SHIFT		2
#define  CSR_CRMD_IE			(_ULCAST_(0x1) << CSR_CRMD_IE_SHIFT)
#define  CSR_CRMD_PLV_SHIFT		0
#define  CSR_CRMD_PLV_WIDTH		2
#define  CSR_CRMD_PLV			(_ULCAST_(0x3) << CSR_CRMD_PLV_SHIFT)

#define PLV_KERN			0
#define PLV_USER			3
#define PLV_MASK			0x3

#define LA_CSR_PRMD        0x1   /* Pre-exception Mode Information */
#define  CSR_PRMD_PWE_SHIFT		3
#define  CSR_PRMD_PWE			(_ULCAST_(0x1) << CSR_PRMD_PWE_SHIFT)
#define  CSR_PRMD_PIE_SHIFT		2
#define  CSR_PRMD_PIE			(_ULCAST_(0x1) << CSR_PRMD_PIE_SHIFT)
#define  CSR_PRMD_PPLV_SHIFT		0
#define  CSR_PRMD_PPLV_WIDTH		2
#define  CSR_PRMD_PPLV			(_ULCAST_(0x3) << CSR_PRMD_PPLV_SHIFT)

#define LA_CSR_EUEN        0x2   /* Extended Unit Enable */
#define  CSR_EUEN_LBTEN_SHIFT		3
#define  CSR_EUEN_LBTEN			(_ULCAST_(0x1) << CSR_EUEN_LBTEN_SHIFT)
#define  CSR_EUEN_LASXEN_SHIFT		2
#define  CSR_EUEN_LASXEN		(_ULCAST_(0x1) << CSR_EUEN_LASXEN_SHIFT)
#define  CSR_EUEN_LSXEN_SHIFT		1
#define  CSR_EUEN_LSXEN			(_ULCAST_(0x1) << CSR_EUEN_LSXEN_SHIFT)
#define  CSR_EUEN_FPEN_SHIFT		0
#define  CSR_EUEN_FPEN			(_ULCAST_(0x1) << CSR_EUEN_FPEN_SHIFT)


#define LA_CSR_MISC        0x3   /* Miscellaneous Control */

#define LA_CSR_ECFG        0x4   /* Exception Configuration */
#define  CSR_ECFG_VS_SHIFT		16
#define  CSR_ECFG_VS_WIDTH		3
#define  CSR_ECFG_VS_SHIFT_END		(CSR_ECFG_VS_SHIFT + CSR_ECFG_VS_WIDTH - 1)
#define  CSR_ECFG_VS			(_ULCAST_(0x7) << CSR_ECFG_VS_SHIFT)
#define  CSR_ECFG_IM_SHIFT		0
#define  CSR_ECFG_IM_WIDTH		14
#define  CSR_ECFG_IM			(_ULCAST_(0x3fff) << CSR_ECFG_IM_SHIFT)

#define LA_CSR_ESTAT       0x5   /* Exception Status */
#define  CSR_ESTAT_ESUBCODE_SHIFT	22
#define  CSR_ESTAT_ESUBCODE_WIDTH	9
#define  CSR_ESTAT_ESUBCODE		(_ULCAST_(0x1ff) << CSR_ESTAT_ESUBCODE_SHIFT)
#define  CSR_ESTAT_EXC_SHIFT		16
#define  CSR_ESTAT_EXC_WIDTH		6
#define  CSR_ESTAT_EXC			(_ULCAST_(0x3f) << CSR_ESTAT_EXC_SHIFT)
#define  CSR_ESTAT_IS_SHIFT		0
#define  CSR_ESTAT_IS_WIDTH		15
#define  CSR_ESTAT_IS			(_ULCAST_(0x7fff) << CSR_ESTAT_IS_SHIFT)

#define LA_CSR_ERA         0x6   /* Exception Return Address */
#define LA_CSR_BADV        0x7   /* Bad Virtual Address */
#define LA_CSR_BADI        0x8   /* Bad Instruction */
#define LA_CSR_EENTRY      0xC   /* Exception Entry Point */

/* Memory Management Registers */

#define LA_CSR_TLBIDX      0x10  /* TLB Index, EHINV, PageSize, NP */
#define  CSR_TLBIDX_EHINV_SHIFT		31
#define  CSR_TLBIDX_EHINV		(_ULCAST_(1) << CSR_TLBIDX_EHINV_SHIFT)
#define  CSR_TLBIDX_PS_SHIFT		24
#define  CSR_TLBIDX_PS_WIDTH		6
#define  CSR_TLBIDX_PS			(_ULCAST_(0x3f) << CSR_TLBIDX_PS_SHIFT)
#define  CSR_TLBIDX_IDX_SHIFT		0
#define  CSR_TLBIDX_IDX_WIDTH		12
#define  CSR_TLBIDX_IDX			(_ULCAST_(0xfff) << CSR_TLBIDX_IDX_SHIFT)
#define  CSR_TLBIDX_SIZEM		0x3f000000
#define  CSR_TLBIDX_SIZE		CSR_TLBIDX_PS_SHIFT
#define  CSR_TLBIDX_IDXM		0xfff
#define  CSR_INVALID_ENTRY(e)		(CSR_TLBIDX_EHINV | e)

#define LA_CSR_TLBEHI      0x11  /* TLB Entry High */
#define LA_CSR_TLBELO0     0x12  /* TLB Entry Low 0 */
#define  CSR_TLBLO0_RPLV_SHIFT		63
#define  CSR_TLBLO0_RPLV		(_ULCAST_(0x1) << CSR_TLBLO0_RPLV_SHIFT)
#define  CSR_TLBLO0_NX_SHIFT		62
#define  CSR_TLBLO0_NX			(_ULCAST_(0x1) << CSR_TLBLO0_NX_SHIFT)
#define  CSR_TLBLO0_NR_SHIFT		61
#define  CSR_TLBLO0_NR			(_ULCAST_(0x1) << CSR_TLBLO0_NR_SHIFT)
#define  CSR_TLBLO0_PFN_SHIFT		12
#define  CSR_TLBLO0_PFN_WIDTH		36
#define  CSR_TLBLO0_PFN			(_ULCAST_(0xfffffffff) << CSR_TLBLO0_PFN_SHIFT)
#define  CSR_TLBLO0_GLOBAL_SHIFT	6
#define  CSR_TLBLO0_GLOBAL		(_ULCAST_(0x1) << CSR_TLBLO0_GLOBAL_SHIFT)
#define  CSR_TLBLO0_CCA_SHIFT		4
#define  CSR_TLBLO0_CCA_WIDTH		2
#define  CSR_TLBLO0_CCA			(_ULCAST_(0x3) << CSR_TLBLO0_CCA_SHIFT)
#define  CSR_TLBLO0_PLV_SHIFT		2
#define  CSR_TLBLO0_PLV_WIDTH		2
#define  CSR_TLBLO0_PLV			(_ULCAST_(0x3) << CSR_TLBLO0_PLV_SHIFT)
#define  CSR_TLBLO0_WE_SHIFT		1
#define  CSR_TLBLO0_WE			(_ULCAST_(0x1) << CSR_TLBLO0_WE_SHIFT)
#define  CSR_TLBLO0_V_SHIFT		0
#define  CSR_TLBLO0_V			(_ULCAST_(0x1) << CSR_TLBLO0_V_SHIFT)

#define LA_CSR_TLBELO1     0x13  /* TLB Entry Low 1 */
#define  CSR_TLBLO1_RPLV_SHIFT		63
#define  CSR_TLBLO1_RPLV		(_ULCAST_(0x1) << CSR_TLBLO1_RPLV_SHIFT)
#define  CSR_TLBLO1_NX_SHIFT		62
#define  CSR_TLBLO1_NX			(_ULCAST_(0x1) << CSR_TLBLO1_NX_SHIFT)
#define  CSR_TLBLO1_NR_SHIFT		61
#define  CSR_TLBLO1_NR			(_ULCAST_(0x1) << CSR_TLBLO1_NR_SHIFT)
#define  CSR_TLBLO1_PFN_SHIFT		12
#define  CSR_TLBLO1_PFN_WIDTH		36
#define  CSR_TLBLO1_PFN			(_ULCAST_(0xfffffffff) << CSR_TLBLO1_PFN_SHIFT)
#define  CSR_TLBLO1_GLOBAL_SHIFT	6
#define  CSR_TLBLO1_GLOBAL		(_ULCAST_(0x1) << CSR_TLBLO1_GLOBAL_SHIFT)
#define  CSR_TLBLO1_CCA_SHIFT		4
#define  CSR_TLBLO1_CCA_WIDTH		2
#define  CSR_TLBLO1_CCA			(_ULCAST_(0x3) << CSR_TLBLO1_CCA_SHIFT)
#define  CSR_TLBLO1_PLV_SHIFT		2
#define  CSR_TLBLO1_PLV_WIDTH		2
#define  CSR_TLBLO1_PLV			(_ULCAST_(0x3) << CSR_TLBLO1_PLV_SHIFT)
#define  CSR_TLBLO1_WE_SHIFT		1
#define  CSR_TLBLO1_WE			(_ULCAST_(0x1) << CSR_TLBLO1_WE_SHIFT)
#define  CSR_TLBLO1_V_SHIFT		0
#define  CSR_TLBLO1_V			(_ULCAST_(0x1) << CSR_TLBLO1_V_SHIFT)

#define LA_CSR_ASID        0x18  /* Address Space Identifier */
#define  CSR_ASID_BIT_SHIFT		16	/* ASIDBits */
#define  CSR_ASID_BIT_WIDTH		8
#define  CSR_ASID_BIT			(_ULCAST_(0xff) << CSR_ASID_BIT_SHIFT)
#define  CSR_ASID_ASID_SHIFT		0
#define  CSR_ASID_ASID_WIDTH		10
#define  CSR_ASID_ASID			(_ULCAST_(0x3ff) << CSR_ASID_ASID_SHIFT)

#define LA_CSR_PGDL        0x19  /* Page Global Directory base Low */
#define LA_CSR_PGDH        0x1A  /* Page Global Directory base High */
#define LA_CSR_PGD         0x1B  /* Page Global Directory base */

#define LA_CSR_PWCTL0      0x1C  /* Page Walk Control Low */
#define  CSR_PWCTL0_PTEW_SHIFT		30
#define  CSR_PWCTL0_PTEW_WIDTH		2
#define  CSR_PWCTL0_PTEW		(_ULCAST_(0x3) << CSR_PWCTL0_PTEW_SHIFT)
#define  CSR_PWCTL0_DIR1WIDTH_SHIFT	25
#define  CSR_PWCTL0_DIR1WIDTH_WIDTH	5
#define  CSR_PWCTL0_DIR1WIDTH		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR1WIDTH_SHIFT)
#define  CSR_PWCTL0_DIR1BASE_SHIFT	20
#define  CSR_PWCTL0_DIR1BASE_WIDTH	5
#define  CSR_PWCTL0_DIR1BASE		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR1BASE_SHIFT)
#define  CSR_PWCTL0_DIR0WIDTH_SHIFT	15
#define  CSR_PWCTL0_DIR0WIDTH_WIDTH	5
#define  CSR_PWCTL0_DIR0WIDTH		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR0WIDTH_SHIFT)
#define  CSR_PWCTL0_DIR0BASE_SHIFT	10
#define  CSR_PWCTL0_DIR0BASE_WIDTH	5
#define  CSR_PWCTL0_DIR0BASE		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR0BASE_SHIFT)
#define  CSR_PWCTL0_PTWIDTH_SHIFT	5
#define  CSR_PWCTL0_PTWIDTH_WIDTH	5
#define  CSR_PWCTL0_PTWIDTH		(_ULCAST_(0x1f) << CSR_PWCTL0_PTWIDTH_SHIFT)
#define  CSR_PWCTL0_PTBASE_SHIFT	0
#define  CSR_PWCTL0_PTBASE_WIDTH	5
#define  CSR_PWCTL0_PTBASE		(_ULCAST_(0x1f) << CSR_PWCTL0_PTBASE_SHIFT)

#define LA_CSR_PWCTL1      0x1D  /* Page Walk Control High */
#define  CSR_PWCTL1_PTW_SHIFT		24
#define  CSR_PWCTL1_PTW_WIDTH		1
#define  CSR_PWCTL1_PTW			(_ULCAST_(0x1) << CSR_PWCTL1_PTW_SHIFT)
#define  CSR_PWCTL1_DIR3WIDTH_SHIFT	18
#define  CSR_PWCTL1_DIR3WIDTH_WIDTH	5
#define  CSR_PWCTL1_DIR3WIDTH		(_ULCAST_(0x1f) << CSR_PWCTL1_DIR3WIDTH_SHIFT)
#define  CSR_PWCTL1_DIR3BASE_SHIFT	12
#define  CSR_PWCTL1_DIR3BASE_WIDTH	5
#define  CSR_PWCTL1_DIR3BASE		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR3BASE_SHIFT)
#define  CSR_PWCTL1_DIR2WIDTH_SHIFT	6
#define  CSR_PWCTL1_DIR2WIDTH_WIDTH	5
#define  CSR_PWCTL1_DIR2WIDTH		(_ULCAST_(0x1f) << CSR_PWCTL1_DIR2WIDTH_SHIFT)
#define  CSR_PWCTL1_DIR2BASE_SHIFT	0
#define  CSR_PWCTL1_DIR2BASE_WIDTH	5
#define  CSR_PWCTL1_DIR2BASE		(_ULCAST_(0x1f) << CSR_PWCTL0_DIR2BASE_SHIFT)

#define LA_CSR_STLBPSIZE   0x1E  /* STLB Page Size */
#define  CSR_STLBPGSIZE_PS_WIDTH	6
#define  CSR_STLBPGSIZE_PS		(_ULCAST_(0x3f))

#define LA_CSR_RVACFG      0x1F  /* Reduced Virtual Address Configuration */
#define  CSR_RVACFG_RDVA_WIDTH		4
#define  CSR_RVACFG_RDVA		(_ULCAST_(0xf))

/* CPU Identification and Configuration */

#define LA_CSR_CPUID       0x20  /* CPU ID */
#define  CSR_CPUID_COREID_WIDTH		9
#define  CSR_CPUID_COREID		_ULCAST_(0x1ff)

#define LA_CSR_PRCFG2		0x22	/* Config2 */

#define LA_CSR_PRCFG1      0x21  /* Privileged Resource Config 1 */
#define  CSR_CONF1_VSMAX_SHIFT		12
#define  CSR_CONF1_VSMAX_WIDTH		3
#define  CSR_CONF1_VSMAX		(_ULCAST_(7) << CSR_CONF1_VSMAX_SHIFT)
#define  CSR_CONF1_TMRBITS_SHIFT	4
#define  CSR_CONF1_TMRBITS_WIDTH	8
#define  CSR_CONF1_TMRBITS		(_ULCAST_(0xff) << CSR_CONF1_TMRBITS_SHIFT)
#define  CSR_CONF1_KSNUM_WIDTH		4
#define  CSR_CONF1_KSNUM		_ULCAST_(0xf)

#define LA_CSR_PRCFG2      0x22  /* Privileged Resource Config 2 */
#define  CSR_CONF2_PGMASK_SUPP		0x3ffff000

#define LA_CSR_PRCFG3      0x23  /* Privileged Resource Config 3 */
#define  CSR_CONF3_STLBIDX_SHIFT	20
#define  CSR_CONF3_STLBIDX_WIDTH	6
#define  CSR_CONF3_STLBIDX		(_ULCAST_(0x3f) << CSR_CONF3_STLBIDX_SHIFT)
#define  CSR_CONF3_STLBWAYS_SHIFT	12
#define  CSR_CONF3_STLBWAYS_WIDTH	8
#define  CSR_CONF3_STLBWAYS		(_ULCAST_(0xff) << CSR_CONF3_STLBWAYS_SHIFT)
#define  CSR_CONF3_MTLBSIZE_SHIFT	4
#define  CSR_CONF3_MTLBSIZE_WIDTH	8
#define  CSR_CONF3_MTLBSIZE		(_ULCAST_(0xff) << CSR_CONF3_MTLBSIZE_SHIFT)
#define  CSR_CONF3_TLBTYPE_SHIFT	0
#define  CSR_CONF3_TLBTYPE_WIDTH	4
#define  CSR_CONF3_TLBTYPE		(_ULCAST_(0xf) << CSR_CONF3_TLBTYPE_SHIFT)

/* Data Save Registers (0x30 - 0x3F) */

#define LA_CSR_KS0       0x30  /* Data Save Register 0 */
#define LA_CSR_KS1       0x31  /* Data Save Register 1 */
#define LA_CSR_KS2       0x32  /* Data Save Register 2 */
#define LA_CSR_KS3       0x33  /* Data Save Register 3 */
#define LA_CSR_KS4       0x34  /* Data Save Register 4 */
#define LA_CSR_KS5       0x35  /* Data Save Register 5 */
#define LA_CSR_KS6       0x36  /* Data Save Register 6 */
#define LA_CSR_KS7       0x37  /* Data Save Register 7 */
#define LA_CSR_KS8       0x38  /* Data Save Register 8 */

/* Exception allocated KS0, KS1 and KS2 statically */
#define EXCEPTION_KS0			LA_CSR_KS0  // Stash tmp reg $t0
#define EXCEPTION_KS1			LA_CSR_KS1  // Stash tmp reg $t1
#define EXCEPTION_KS2			LA_CSR_KS2  // Stash stack reg $sp
#define EXC_KSAVE_MASK			(1 << 0 | 1 << 1 | 1 << 2)

/* Percpu-data base allocated KS3 statically */
#define PERCPU_BASE_KS3			LA_CSR_KS3
#define PERCPU_KSAVE_MASK		(1 << 3)

/* Flag indicate whether in interrupt context */
#define IN_INTERRUPT_KS4			LA_CSR_KS4
#define PERCPU_KSAVE_MASK		(1 << 4)

/* Timer Registers */

#define LA_CSR_TMID        0x40  /* Timer ID */

#define LA_CSR_TCFG        0x41  /* Timer Configuration */
#define  CSR_TCFG_VAL_SHIFT		2
#define  CSR_TCFG_VAL_WIDTH		48
#define  CSR_TCFG_VAL			(_ULCAST_(0x3fffffffffff) << CSR_TCFG_VAL_SHIFT)
#define  CSR_TCFG_PERIOD_SHIFT		1
#define  CSR_TCFG_PERIOD		(_ULCAST_(0x1) << CSR_TCFG_PERIOD_SHIFT)
#define  CSR_TCFG_EN			(_ULCAST_(0x1))

#define LA_CSR_TVAL      0x42  /* Timer Value (Count) */
#define LA_CSR_CNTC      0x43  /* Counter Compensation */

#define LA_CSR_TINTCLR   0x44  /* Timer Interrupt Clear */
#define  CSR_TINTCLR_TI_SHIFT   0
#define  CSR_TINTCLR_TI         (1 << CSR_TINTCLR_TI_SHIFT)

/* LLBit and Implementation-Dependent Registers */

#define LA_CSR_LLBCTL      0x60  /* LLBit Control */
#define  CSR_LLBCTL_ROLLB_SHIFT		0
#define  CSR_LLBCTL_ROLLB		(_ULCAST_(1) << CSR_LLBCTL_ROLLB_SHIFT)
#define  CSR_LLBCTL_WCLLB_SHIFT		1
#define  CSR_LLBCTL_WCLLB		(_ULCAST_(1) << CSR_LLBCTL_WCLLB_SHIFT)
#define  CSR_LLBCTL_KLO_SHIFT		2
#define  CSR_LLBCTL_KLO			(_ULCAST_(1) << CSR_LLBCTL_KLO_SHIFT)

#define LA_CSR_IMPCTL1     0x80  /* Loongson config1 */
#define  CSR_LDSTORDER_SHIFT		28
#define  CSR_LDSTORDER_WIDTH		3
#define  CSR_LDSTORDER_MASK		(_ULCAST_(0x7) << CSR_LDSTORDER_SHIFT)
/* 000 = No Load No Store */
#define  CSR_LDSTORDER_NLD_NST		(_ULCAST_(0x0) << CSR_LDSTORDER_SHIFT)
/* 001 = All Load No Store */
#define  CSR_LDSTORDER_ALD_NST		(_ULCAST_(0x1) << CSR_LDSTORDER_SHIFT)
/* 011 = Same Load No Store */
#define  CSR_LDSTORDER_SLD_NST		(_ULCAST_(0x3) << CSR_LDSTORDER_SHIFT)
/* 100 = No Load All Store */
#define  CSR_LDSTORDER_NLD_AST		(_ULCAST_(0x4) << CSR_LDSTORDER_SHIFT)
/* 101 = All Load All Store */
#define  CSR_LDSTORDER_ALD_AST		(_ULCAST_(0x5) << CSR_LDSTORDER_SHIFT)
/* 111 = Same Load All Store */
#define  CSR_LDSTORDER_SLD_AST		(_ULCAST_(0x7) << CSR_LDSTORDER_SHIFT)
#define  CSR_MISPEC_SHIFT		20
#define  CSR_MISPEC_WIDTH		8
#define  CSR_MISPEC			(_ULCAST_(0xff) << CSR_MISPEC_SHIFT)
#define  CSR_SSEN_SHIFT			18
#define  CSR_SSEN			(_ULCAST_(1) << CSR_SSEN_SHIFT)
#define  CSR_SCRAND_SHIFT		17
#define  CSR_SCRAND			(_ULCAST_(1) << CSR_SCRAND_SHIFT)
#define  CSR_LLEXCL_SHIFT		16
#define  CSR_LLEXCL			(_ULCAST_(1) << CSR_LLEXCL_SHIFT)
#define  CSR_DISVC_SHIFT		15
#define  CSR_DISVC			(_ULCAST_(1) << CSR_DISVC_SHIFT)
#define  CSR_VCLRU_SHIFT		14
#define  CSR_VCLRU			(_ULCAST_(1) << CSR_VCLRU_SHIFT)
#define  CSR_DCLRU_SHIFT		13
#define  CSR_DCLRU			(_ULCAST_(1) << CSR_DCLRU_SHIFT)
#define  CSR_FASTLDQ_SHIFT		12
#define  CSR_FASTLDQ			(_ULCAST_(1) << CSR_FASTLDQ_SHIFT)
#define  CSR_USERCAC_SHIFT		11
#define  CSR_USERCAC			(_ULCAST_(1) << CSR_USERCAC_SHIFT)
#define  CSR_ANTI_MISPEC_SHIFT		10
#define  CSR_ANTI_MISPEC		(_ULCAST_(1) << CSR_ANTI_MISPEC_SHIFT)
#define  CSR_AUTO_FLUSHSFB_SHIFT	9
#define  CSR_AUTO_FLUSHSFB		(_ULCAST_(1) << CSR_AUTO_FLUSHSFB_SHIFT)
#define  CSR_STFILL_SHIFT		8
#define  CSR_STFILL			(_ULCAST_(1) << CSR_STFILL_SHIFT)
#define  CSR_LIFEP_SHIFT		7
#define  CSR_LIFEP			(_ULCAST_(1) << CSR_LIFEP_SHIFT)
#define  CSR_LLSYNC_SHIFT		6
#define  CSR_LLSYNC			(_ULCAST_(1) << CSR_LLSYNC_SHIFT)
#define  CSR_BRBTDIS_SHIFT		5
#define  CSR_BRBTDIS			(_ULCAST_(1) << CSR_BRBTDIS_SHIFT)
#define  CSR_RASDIS_SHIFT		4
#define  CSR_RASDIS			(_ULCAST_(1) << CSR_RASDIS_SHIFT)
#define  CSR_STPRE_SHIFT		2
#define  CSR_STPRE_WIDTH		2
#define  CSR_STPRE			(_ULCAST_(3) << CSR_STPRE_SHIFT)
#define  CSR_INSTPRE_SHIFT		1
#define  CSR_INSTPRE			(_ULCAST_(1) << CSR_INSTPRE_SHIFT)
#define  CSR_DATAPRE_SHIFT		0
#define  CSR_DATAPRE			(_ULCAST_(1) << CSR_DATAPRE_SHIFT)

#define LA_CSR_IMPCTL2     0x81  /* Loongson config2 */
#define  CSR_FLUSH_MTLB_SHIFT		0
#define  CSR_FLUSH_MTLB			(_ULCAST_(1) << CSR_FLUSH_MTLB_SHIFT)
#define  CSR_FLUSH_STLB_SHIFT		1
#define  CSR_FLUSH_STLB			(_ULCAST_(1) << CSR_FLUSH_STLB_SHIFT)
#define  CSR_FLUSH_DTLB_SHIFT		2
#define  CSR_FLUSH_DTLB			(_ULCAST_(1) << CSR_FLUSH_DTLB_SHIFT)
#define  CSR_FLUSH_ITLB_SHIFT		3
#define  CSR_FLUSH_ITLB			(_ULCAST_(1) << CSR_FLUSH_ITLB_SHIFT)
#define  CSR_FLUSH_BTAC_SHIFT		4
#define  CSR_FLUSH_BTAC			(_ULCAST_(1) << CSR_FLUSH_BTAC_SHIFT)

#define LA_CSR_GNMI		0x82

/* TLB Refill Exception Registers */

#define LA_CSR_TLBRENTRY   0x88  /* TLB refill exceptino entry */
#define LA_CSR_TLBRBADV    0x89  /* TLB refill badvaddr */
#define LA_CSR_TLBRERA     0x8A  /* TLB refill exception return address */
#define LA_CSR_TLBRSAVE    0x8B  /* TLB Refill Data Save */
#define LA_CSR_TLBRELO0    0x8C  /* TLB refill entrylow0 */
#define LA_CSR_TLBRELO1    0x8D  /* TLB refill entrylow1 */
#define LA_CSR_TLBREHI     0x8E  /* TLB refill entryhi */
#define  CSR_TLBREHI_PS_SHIFT       0
#define  CSR_TLBREHI_PS         (_ULCAST_(0x3f) << CSR_TLBREHI_PS_SHIFT)
#define LA_CSR_TLBRPRMD    0x8F  /* TLB refill mode info */

/* Machine Error Registers */

#define LA_CSR_MERRCTL     0x90  /* Machine Error control */
#define LA_CSR_MERRINFO1   0x91  /* Machine Error information 1 */
#define LA_CSR_MERRINFO2   0x92  /* Machine Error information 2 */
#define LA_CSR_MERRENTRY   0x93  /* Machine Error exception entry */
#define LA_CSR_MERRERA     0x94  /* Machine Error return address */
#define LA_CSR_MERRSAVE    0x95  /* Machine Error data save */

/* Cache Tag Register */

#define LA_CSR_CTAG        0x98  /* Cache TagLo + TagHi */

/* Shadow MCSR : 0xc0 ~ 0xff */
#define LA_CSR_MCSR0		0xc0	/* CPUCFG0 and CPUCFG1 */
#define  MCSR0_INT_IMPL_SHIFT		58
#define  MCSR0_INT_IMPL			0
#define  MCSR0_IOCSR_BRD_SHIFT		57
#define  MCSR0_IOCSR_BRD		(_ULCAST_(1) << MCSR0_IOCSR_BRD_SHIFT)
#define  MCSR0_HUGEPG_SHIFT		56
#define  MCSR0_HUGEPG			(_ULCAST_(1) << MCSR0_HUGEPG_SHIFT)
#define  MCSR0_RPLMTLB_SHIFT		55
#define  MCSR0_RPLMTLB			(_ULCAST_(1) << MCSR0_RPLMTLB_SHIFT)
#define  MCSR0_EP_SHIFT			54
#define  MCSR0_EP			(_ULCAST_(1) << MCSR0_EP_SHIFT)
#define  MCSR0_RI_SHIFT			53
#define  MCSR0_RI			(_ULCAST_(1) << MCSR0_RI_SHIFT)
#define  MCSR0_UAL_SHIFT		52
#define  MCSR0_UAL			(_ULCAST_(1) << MCSR0_UAL_SHIFT)
#define  MCSR0_VABIT_SHIFT		44
#define  MCSR0_VABIT_WIDTH		8
#define  MCSR0_VABIT			(_ULCAST_(0xff) << MCSR0_VABIT_SHIFT)
#define  VABIT_DEFAULT			0x2f
#define  MCSR0_PABIT_SHIFT		36
#define  MCSR0_PABIT_WIDTH		8
#define  MCSR0_PABIT			(_ULCAST_(0xff) << MCSR0_PABIT_SHIFT)
#define  PABIT_DEFAULT			0x2f
#define  MCSR0_IOCSR_SHIFT		35
#define  MCSR0_IOCSR			(_ULCAST_(1) << MCSR0_IOCSR_SHIFT)
#define  MCSR0_PAGING_SHIFT		34
#define  MCSR0_PAGING			(_ULCAST_(1) << MCSR0_PAGING_SHIFT)
#define  MCSR0_GR64_SHIFT		33
#define  MCSR0_GR64			(_ULCAST_(1) << MCSR0_GR64_SHIFT)
#define  GR64_DEFAULT			1
#define  MCSR0_GR32_SHIFT		32
#define  MCSR0_GR32			(_ULCAST_(1) << MCSR0_GR32_SHIFT)
#define  GR32_DEFAULT			0
#define  MCSR0_PRID_WIDTH		32
#define  MCSR0_PRID			0x14C010

#define LA_CSR_MCSR1		0xc1	/* CPUCFG2 and CPUCFG3 */
#define  MCSR1_HPFOLD_SHIFT		43
#define  MCSR1_HPFOLD			(_ULCAST_(1) << MCSR1_HPFOLD_SHIFT)
#define  MCSR1_SPW_LVL_SHIFT		40
#define  MCSR1_SPW_LVL_WIDTH		3
#define  MCSR1_SPW_LVL			(_ULCAST_(7) << MCSR1_SPW_LVL_SHIFT)
#define  MCSR1_ICACHET_SHIFT		39
#define  MCSR1_ICACHET			(_ULCAST_(1) << MCSR1_ICACHET_SHIFT)
#define  MCSR1_ITLBT_SHIFT		38
#define  MCSR1_ITLBT			(_ULCAST_(1) << MCSR1_ITLBT_SHIFT)
#define  MCSR1_LLDBAR_SHIFT		37
#define  MCSR1_LLDBAR			(_ULCAST_(1) << MCSR1_LLDBAR_SHIFT)
#define  MCSR1_SCDLY_SHIFT		36
#define  MCSR1_SCDLY			(_ULCAST_(1) << MCSR1_SCDLY_SHIFT)
#define  MCSR1_LLEXC_SHIFT		35
#define  MCSR1_LLEXC			(_ULCAST_(1) << MCSR1_LLEXC_SHIFT)
#define  MCSR1_UCACC_SHIFT		34
#define  MCSR1_UCACC			(_ULCAST_(1) << MCSR1_UCACC_SHIFT)
#define  MCSR1_SFB_SHIFT		33
#define  MCSR1_SFB			(_ULCAST_(1) << MCSR1_SFB_SHIFT)
#define  MCSR1_CCDMA_SHIFT		32
#define  MCSR1_CCDMA			(_ULCAST_(1) << MCSR1_CCDMA_SHIFT)
#define  MCSR1_LAMO_SHIFT		22
#define  MCSR1_LAMO			(_ULCAST_(1) << MCSR1_LAMO_SHIFT)
#define  MCSR1_LSPW_SHIFT		21
#define  MCSR1_LSPW			(_ULCAST_(1) << MCSR1_LSPW_SHIFT)
#define  MCSR1_MIPSBT_SHIFT		20
#define  MCSR1_MIPSBT			(_ULCAST_(1) << MCSR1_MIPSBT_SHIFT)
#define  MCSR1_ARMBT_SHIFT		19
#define  MCSR1_ARMBT			(_ULCAST_(1) << MCSR1_ARMBT_SHIFT)
#define  MCSR1_X86BT_SHIFT		18
#define  MCSR1_X86BT			(_ULCAST_(1) << MCSR1_X86BT_SHIFT)
#define  MCSR1_LLFTPVERS_SHIFT		15
#define  MCSR1_LLFTPVERS_WIDTH		3
#define  MCSR1_LLFTPVERS		(_ULCAST_(7) << MCSR1_LLFTPVERS_SHIFT)
#define  MCSR1_LLFTP_SHIFT		14
#define  MCSR1_LLFTP			(_ULCAST_(1) << MCSR1_LLFTP_SHIFT)
#define  MCSR1_VZVERS_SHIFT		11
#define  MCSR1_VZVERS_WIDTH		3
#define  MCSR1_VZVERS			(_ULCAST_(7) << MCSR1_VZVERS_SHIFT)
#define  MCSR1_VZ_SHIFT			10
#define  MCSR1_VZ			(_ULCAST_(1) << MCSR1_VZ_SHIFT)
#define  MCSR1_CRYPTO_SHIFT		9
#define  MCSR1_CRYPTO			(_ULCAST_(1) << MCSR1_CRYPTO_SHIFT)
#define  MCSR1_COMPLEX_SHIFT		8
#define  MCSR1_COMPLEX			(_ULCAST_(1) << MCSR1_COMPLEX_SHIFT)
#define  MCSR1_LASX_SHIFT		7
#define  MCSR1_LASX			(_ULCAST_(1) << MCSR1_LASX_SHIFT)
#define  MCSR1_LSX_SHIFT		6
#define  MCSR1_LSX			(_ULCAST_(1) << MCSR1_LSX_SHIFT)
#define  MCSR1_FPVERS_SHIFT		3
#define  MCSR1_FPVERS_WIDTH		3
#define  MCSR1_FPVERS			(_ULCAST_(7) << MCSR1_FPVERS_SHIFT)
#define  MCSR1_FPDP_SHIFT		2
#define  MCSR1_FPDP			(_ULCAST_(1) << MCSR1_FPDP_SHIFT)
#define  MCSR1_FPSP_SHIFT		1
#define  MCSR1_FPSP			(_ULCAST_(1) << MCSR1_FPSP_SHIFT)
#define  MCSR1_FP_SHIFT			0
#define  MCSR1_FP			(_ULCAST_(1) << MCSR1_FP_SHIFT)

#define LA_CSR_MCSR2		0xc2	/* CPUCFG4 and CPUCFG5 */
#define  MCSR2_CCDIV_SHIFT		48
#define  MCSR2_CCDIV_WIDTH		16
#define  MCSR2_CCDIV			(_ULCAST_(0xffff) << MCSR2_CCDIV_SHIFT)
#define  MCSR2_CCMUL_SHIFT		32
#define  MCSR2_CCMUL_WIDTH		16
#define  MCSR2_CCMUL			(_ULCAST_(0xffff) << MCSR2_CCMUL_SHIFT)
#define  MCSR2_CCFREQ_WIDTH		32
#define  MCSR2_CCFREQ			(_ULCAST_(0xffffffff))
#define  CCFREQ_DEFAULT			0x5f5e100	/* 100MHz */

#define LA_CSR_MCSR3		0xc3	/* CPUCFG6 */
#define  MCSR3_UPM_SHIFT		14
#define  MCSR3_UPM			(_ULCAST_(1) << MCSR3_UPM_SHIFT)
#define  MCSR3_PMBITS_SHIFT		8
#define  MCSR3_PMBITS_WIDTH		6
#define  MCSR3_PMBITS			(_ULCAST_(0x3f) << MCSR3_PMBITS_SHIFT)
#define  PMBITS_DEFAULT			0x40
#define  MCSR3_PMNUM_SHIFT		4
#define  MCSR3_PMNUM_WIDTH		4
#define  MCSR3_PMNUM			(_ULCAST_(0xf) << MCSR3_PMNUM_SHIFT)
#define  MCSR3_PAMVER_SHIFT		1
#define  MCSR3_PAMVER_WIDTH		3
#define  MCSR3_PAMVER			(_ULCAST_(0x7) << MCSR3_PAMVER_SHIFT)
#define  MCSR3_PMP_SHIFT		0
#define  MCSR3_PMP			(_ULCAST_(1) << MCSR3_PMP_SHIFT)

#define LA_CSR_MCSR8		0xc8	/* CPUCFG16 and CPUCFG17 */
#define  MCSR8_L1I_SIZE_SHIFT		56
#define  MCSR8_L1I_SIZE_WIDTH		7
#define  MCSR8_L1I_SIZE			(_ULCAST_(0x7f) << MCSR8_L1I_SIZE_SHIFT)
#define  MCSR8_L1I_IDX_SHIFT		48
#define  MCSR8_L1I_IDX_WIDTH		8
#define  MCSR8_L1I_IDX			(_ULCAST_(0xff) << MCSR8_L1I_IDX_SHIFT)
#define  MCSR8_L1I_WAY_SHIFT		32
#define  MCSR8_L1I_WAY_WIDTH		16
#define  MCSR8_L1I_WAY			(_ULCAST_(0xffff) << MCSR8_L1I_WAY_SHIFT)
#define  MCSR8_L3DINCL_SHIFT		16
#define  MCSR8_L3DINCL			(_ULCAST_(1) << MCSR8_L3DINCL_SHIFT)
#define  MCSR8_L3DPRIV_SHIFT		15
#define  MCSR8_L3DPRIV			(_ULCAST_(1) << MCSR8_L3DPRIV_SHIFT)
#define  MCSR8_L3DPRE_SHIFT		14
#define  MCSR8_L3DPRE			(_ULCAST_(1) << MCSR8_L3DPRE_SHIFT)
#define  MCSR8_L3IUINCL_SHIFT		13
#define  MCSR8_L3IUINCL			(_ULCAST_(1) << MCSR8_L3IUINCL_SHIFT)
#define  MCSR8_L3IUPRIV_SHIFT		12
#define  MCSR8_L3IUPRIV			(_ULCAST_(1) << MCSR8_L3IUPRIV_SHIFT)
#define  MCSR8_L3IUUNIFY_SHIFT		11
#define  MCSR8_L3IUUNIFY		(_ULCAST_(1) << MCSR8_L3IUUNIFY_SHIFT)
#define  MCSR8_L3IUPRE_SHIFT		10
#define  MCSR8_L3IUPRE			(_ULCAST_(1) << MCSR8_L3IUPRE_SHIFT)
#define  MCSR8_L2DINCL_SHIFT		9
#define  MCSR8_L2DINCL			(_ULCAST_(1) << MCSR8_L2DINCL_SHIFT)
#define  MCSR8_L2DPRIV_SHIFT		8
#define  MCSR8_L2DPRIV			(_ULCAST_(1) << MCSR8_L2DPRIV_SHIFT)
#define  MCSR8_L2DPRE_SHIFT		7
#define  MCSR8_L2DPRE			(_ULCAST_(1) << MCSR8_L2DPRE_SHIFT)
#define  MCSR8_L2IUINCL_SHIFT		6
#define  MCSR8_L2IUINCL			(_ULCAST_(1) << MCSR8_L2IUINCL_SHIFT)
#define  MCSR8_L2IUPRIV_SHIFT		5
#define  MCSR8_L2IUPRIV			(_ULCAST_(1) << MCSR8_L2IUPRIV_SHIFT)
#define  MCSR8_L2IUUNIFY_SHIFT		4
#define  MCSR8_L2IUUNIFY		(_ULCAST_(1) << MCSR8_L2IUUNIFY_SHIFT)
#define  MCSR8_L2IUPRE_SHIFT		3
#define  MCSR8_L2IUPRE			(_ULCAST_(1) << MCSR8_L2IUPRE_SHIFT)
#define  MCSR8_L1DPRE_SHIFT		2
#define  MCSR8_L1DPRE			(_ULCAST_(1) << MCSR8_L1DPRE_SHIFT)
#define  MCSR8_L1IUUNIFY_SHIFT		1
#define  MCSR8_L1IUUNIFY		(_ULCAST_(1) << MCSR8_L1IUUNIFY_SHIFT)
#define  MCSR8_L1IUPRE_SHIFT		0
#define  MCSR8_L1IUPRE			(_ULCAST_(1) << MCSR8_L1IUPRE_SHIFT)

#define LA_CSR_MCSR9		0xc9	/* CPUCFG18 and CPUCFG19 */
#define  MCSR9_L2U_SIZE_SHIFT		56
#define  MCSR9_L2U_SIZE_WIDTH		7
#define  MCSR9_L2U_SIZE			(_ULCAST_(0x7f) << MCSR9_L2U_SIZE_SHIFT)
#define  MCSR9_L2U_IDX_SHIFT		48
#define  MCSR9_L2U_IDX_WIDTH		8
#define  MCSR9_L2U_IDX			(_ULCAST_(0xff) << MCSR9_IDX_LOG_SHIFT)
#define  MCSR9_L2U_WAY_SHIFT		32
#define  MCSR9_L2U_WAY_WIDTH		16
#define  MCSR9_L2U_WAY			(_ULCAST_(0xffff) << MCSR9_L2U_WAY_SHIFT)
#define  MCSR9_L1D_SIZE_SHIFT		24
#define  MCSR9_L1D_SIZE_WIDTH		7
#define  MCSR9_L1D_SIZE			(_ULCAST_(0x7f) << MCSR9_L1D_SIZE_SHIFT)
#define  MCSR9_L1D_IDX_SHIFT		16
#define  MCSR9_L1D_IDX_WIDTH		8
#define  MCSR9_L1D_IDX			(_ULCAST_(0xff) << MCSR9_L1D_IDX_SHIFT)
#define  MCSR9_L1D_WAY_SHIFT		0
#define  MCSR9_L1D_WAY_WIDTH		16
#define  MCSR9_L1D_WAY			(_ULCAST_(0xffff) << MCSR9_L1D_WAY_SHIFT)

#define LA_CSR_MCSR10		0xca	/* CPUCFG20 */
#define  MCSR10_L3U_SIZE_SHIFT		24
#define  MCSR10_L3U_SIZE_WIDTH		7
#define  MCSR10_L3U_SIZE		(_ULCAST_(0x7f) << MCSR10_L3U_SIZE_SHIFT)
#define  MCSR10_L3U_IDX_SHIFT		16
#define  MCSR10_L3U_IDX_WIDTH		8
#define  MCSR10_L3U_IDX			(_ULCAST_(0xff) << MCSR10_L3U_IDX_SHIFT)
#define  MCSR10_L3U_WAY_SHIFT		0
#define  MCSR10_L3U_WAY_WIDTH		16
#define  MCSR10_L3U_WAY			(_ULCAST_(0xffff) << MCSR10_L3U_WAY_SHIFT)

#define LA_CSR_MCSR24		0xf0	/* cpucfg48 */
#define  MCSR24_RAMCG_SHIFT		3
#define  MCSR24_RAMCG			(_ULCAST_(1) << MCSR24_RAMCG_SHIFT)
#define  MCSR24_VFPUCG_SHIFT		2
#define  MCSR24_VFPUCG			(_ULCAST_(1) << MCSR24_VFPUCG_SHIFT)
#define  MCSR24_NAPEN_SHIFT		1
#define  MCSR24_NAPEN			(_ULCAST_(1) << MCSR24_NAPEN_SHIFT)
#define  MCSR24_MCSRLOCK_SHIFT		0
#define  MCSR24_MCSRLOCK		(_ULCAST_(1) << MCSR24_MCSRLOCK_SHIFT)


/* Direct Mapping Configuration Window Registers (0x180 + n, 0 ≤ n ≤ 3) */

#define LA_CSR_DMWIN0        0x180 /* 64 direct map win0: MEM & IF */
#define LA_CSR_DMWIN1        0x181 /* 64 direct map win1: MEM & IF */
#define LA_CSR_DMWIN2        0x182 /* 64 direct map win2: MEM */
#define LA_CSR_DMWIN3        0x183 /* 64 direct map win3: MEM */

/* Direct Map window 0/1/2/3 */
#define CSR_DMW0_PLV0		_CONST64_(1 << 0)
#define CSR_DMW0_VSEG		_CONST64_(0x8000)
#define CSR_DMW0_BASE		(CSR_DMW0_VSEG << DMW_PABITS)
#define CSR_DMW0_INIT		(CSR_DMW0_BASE | CSR_DMW0_PLV0)

#define CSR_DMW1_PLV0		_CONST64_(1 << 0)
#define CSR_DMW1_MAT		_CONST64_(1 << 4)
#define CSR_DMW1_VSEG		_CONST64_(0x9000)
#define CSR_DMW1_BASE		(CSR_DMW1_VSEG << DMW_PABITS)
#define CSR_DMW1_INIT		(CSR_DMW1_BASE | CSR_DMW1_MAT | CSR_DMW1_PLV0)

#define CSR_DMW2_PLV0		_CONST64_(1 << 0)
#define CSR_DMW2_MAT		_CONST64_(2 << 4)
#define CSR_DMW2_VSEG		_CONST64_(0xa000)
#define CSR_DMW2_BASE		(CSR_DMW2_VSEG << DMW_PABITS)
#define CSR_DMW2_INIT		(CSR_DMW2_BASE | CSR_DMW2_MAT | CSR_DMW2_PLV0)

#define CSR_DMW3_INIT		0x0

/* Performance Monitor Registers */

#define LA_CSR_PERFCTRL0  0x200 /* 32 perf event 0 config */
#define LA_CSR_PERFCTRL0  0x201 /* 64 perf event 0 count value */
#define LA_CSR_PERFCTRL1  0x202 /* 32 perf event 1 config */
#define LA_CSR_PERFCTRL1  0x203 /* 64 perf event 1 count value */
#define LA_CSR_PERFCTRL2  0x204 /* 32 perf event 2 config */
#define LA_CSR_PERFCTRL2  0x205 /* 64 perf event 2 count value */
#define LA_CSR_PERFCTRL3  0x206 /* 32 perf event 3 config */
#define LA_CSR_PERFCTRL3  0x207 /* 64 perf event 3 count value */
#define  CSR_PERFCTRL_PLV0		(_ULCAST_(1) << 16)
#define  CSR_PERFCTRL_PLV1		(_ULCAST_(1) << 17)
#define  CSR_PERFCTRL_PLV2		(_ULCAST_(1) << 18)
#define  CSR_PERFCTRL_PLV3		(_ULCAST_(1) << 19)
#define  CSR_PERFCTRL_IE		(_ULCAST_(1) << 20)
#define  CSR_PERFCTRL_EVENT		0x3ff

/* Memory Watchpoint Registers */

#define LA_CSR_MWPC      0x300 /* data breakpoint config */
#define LA_CSR_MWPS      0x301 /* data breakpoint status */

#define LA_CSR_DB0ADDR  0x310 /* data breakpoint 0 address */
#define LA_CSR_DB0MASK  0x311 /* data breakpoint 0 mask */
#define LA_CSR_DB0CTRL  0x312 /* data breakpoint 0 control */
#define LA_CSR_DB0ASID  0x313 /* data breakpoint 0 asid */

#define LA_CSR_FWPC      0x380 /* intruction breakpoint config */
#define LA_CSR_FWPS      0x381 /* intruction breakpoint status */

#define LA_CSR_IB0ADDR  0x390 /* inst breakpoint 0 address */
#define LA_CSR_IB0MASK  0x391 /* inst breakpoint 0 mask */
#define LA_CSR_IB0CTRL  0x392 /* inst breakpoint 0 control */
#define LA_CSR_IB0ASID  0x393 /* inst breakpoint 0 asid */

/* Debug Registers */

#define LA_CSR_DEBUG     0x500 /* Debug config */
#define LA_CSR_DERA      0x501 /* Debug era */
#define LA_CSR_DESAVE    0x502 /* Debug save */

#define CSR_FWPC_SKIP_SHIFT		16
#define CSR_FWPC_SKIP			(_ULCAST_(1) << CSR_FWPC_SKIP_SHIFT)

/*
 * CSR_ECFG IM
 */
#define ECFG0_IM		0x00005fff
#define ECFGB_SIP0		0
#define ECFGF_SIP0		(_ULCAST_(1) << ECFGB_SIP0)
#define ECFGB_SIP1		1
#define ECFGF_SIP1		(_ULCAST_(1) << ECFGB_SIP1)
#define ECFGB_IP0		2
#define ECFGF_IP0		(_ULCAST_(1) << ECFGB_IP0)
#define ECFGB_IP1		3
#define ECFGF_IP1		(_ULCAST_(1) << ECFGB_IP1)
#define ECFGB_IP2		4
#define ECFGF_IP2		(_ULCAST_(1) << ECFGB_IP2)
#define ECFGB_IP3		5
#define ECFGF_IP3		(_ULCAST_(1) << ECFGB_IP3)
#define ECFGB_IP4		6
#define ECFGF_IP4		(_ULCAST_(1) << ECFGB_IP4)
#define ECFGB_IP5		7
#define ECFGF_IP5		(_ULCAST_(1) << ECFGB_IP5)
#define ECFGB_IP6		8
#define ECFGF_IP6		(_ULCAST_(1) << ECFGB_IP6)
#define ECFGB_IP7		9
#define ECFGF_IP7		(_ULCAST_(1) << ECFGB_IP7)
#define ECFGB_PMC		10
#define ECFGF_PMC		(_ULCAST_(1) << ECFGB_PMC)
#define ECFGB_TIMER		11
#define ECFGF_TIMER		(_ULCAST_(1) << ECFGB_TIMER)
#define ECFGB_IPI		12
#define ECFGF_IPI		(_ULCAST_(1) << ECFGB_IPI)
#define ECFGF(hwirq)		(_ULCAST_(1) << hwirq)

#define ESTATF_IP       0x00003fff

#define LOONGARCH_IOCSR_FEATURES    0x8
#define  IOCSRF_TEMP        BIT_ULL(0)
#define  IOCSRF_NODECNT     BIT_ULL(1)
#define  IOCSRF_MSI         BIT_ULL(2)
#define  IOCSRF_EXTIOI      BIT_ULL(3)
#define  IOCSRF_CSRIPI      BIT_ULL(4)
#define  IOCSRF_FREQCSR     BIT_ULL(5)
#define  IOCSRF_FREQSCALE   BIT_ULL(6)
#define  IOCSRF_DVFSV1      BIT_ULL(7)
#define  IOCSRF_EIODECODE   BIT_ULL(9)
#define  IOCSRF_FLATMODE    BIT_ULL(10)
#define  IOCSRF_VM          BIT_ULL(11)
#define  IOCSRF_AVEC        BIT_ULL(15)
#define  IOCSRF_REDIRECT    BIT_ULL(16)

#define LOONGARCH_IOCSR_VENDOR      0x10
#define LOONGARCH_IOCSR_CPUNAME     0x20
#define LOONGARCH_IOCSR_NODECNT     0x408

#define LOONGARCH_IOCSR_MISC_FUNC   0x420
#define  IOCSR_MISC_FUNC_SOFT_INT       BIT_ULL(10)
#define  IOCSR_MISC_FUNC_TIMER_RESET    BIT_ULL(21)
#define  IOCSR_MISC_FUNC_EXT_IOI_EN     BIT_ULL(48)
#define  IOCSR_MISC_FUNC_AVEC_EN        BIT_ULL(51)

#define LOONGARCH_IOCSR_CPUTEMP     0x428
#define LOONGARCH_IOCSR_SMCMBX      0x51c
/* PerCore CSR, only accessible by local cores */
#define LOONGARCH_IOCSR_IPI_STATUS  0x1000
#define LOONGARCH_IOCSR_IPI_EN      0x1004
#define LOONGARCH_IOCSR_IPI_SET     0x1008
#define LOONGARCH_IOCSR_IPI_CLEAR   0x100c
#define LOONGARCH_IOCSR_MBUF0       0x1020
#define LOONGARCH_IOCSR_MBUF1       0x1028
#define LOONGARCH_IOCSR_MBUF2       0x1030
#define LOONGARCH_IOCSR_MBUF3       0x1038

#define LOONGARCH_IOCSR_IPI_SEND    0x1040
#define  IOCSR_IPI_SEND_IP_SHIFT        0
#define  IOCSR_IPI_SEND_CPU_SHIFT       16
#define  IOCSR_IPI_SEND_BLOCKING        BIT(31)

#define LOONGARCH_IOCSR_MBUF_SEND   0x1048
#define  IOCSR_MBUF_SEND_BLOCKING       BIT_ULL(31)
#define  IOCSR_MBUF_SEND_BOX_SHIFT      2
#define  IOCSR_MBUF_SEND_BOX_LO(box)    (box << 1)
#define  IOCSR_MBUF_SEND_BOX_HI(box)    ((box << 1) + 1)
#define  IOCSR_MBUF_SEND_CPU_SHIFT      16
#define  IOCSR_MBUF_SEND_BUF_SHIFT      32
#define  IOCSR_MBUF_SEND_H32_MASK   0xFFFFFFFF00000000ULL

#define LOONGARCH_IOCSR_ANY_SEND    0x1158
#define  IOCSR_ANY_SEND_BLOCKING        BIT_ULL(31)
#define  IOCSR_ANY_SEND_CPU_SHIFT       16
#define  IOCSR_ANY_SEND_MASK_SHIFT      27
#define  IOCSR_ANY_SEND_BUF_SHIFT       32
#define  IOCSR_ANY_SEND_H32_MASK        0xFFFFFFFF00000000ULL

/* Register offset and bit definition for CSR access */
#define LOONGARCH_IOCSR_TIMER_CFG       0x1060
#define LOONGARCH_IOCSR_TIMER_TICK      0x1070
#define  IOCSR_TIMER_CFG_RESERVED       (_ULCAST_(1) << 63)
#define  IOCSR_TIMER_CFG_PERIODIC       (_ULCAST_(1) << 62)
#define  IOCSR_TIMER_CFG_EN             (_ULCAST_(1) << 61)
#define  IOCSR_TIMER_MASK               0x0ffffffffffffULL
#define  IOCSR_TIMER_INITVAL_RST        (_ULCAST_(0xffff) << 48)

#define LOONGARCH_IOCSR_EXTIOI_NODEMAP_BASE 0x14a0
#define LOONGARCH_IOCSR_EXTIOI_IPMAP_BASE   0x14c0
#define LOONGARCH_IOCSR_EXTIOI_EN_BASE      0x1600
#define LOONGARCH_IOCSR_EXTIOI_BOUNCE_BASE  0x1680
#define LOONGARCH_IOCSR_EXTIOI_ISR_BASE     0x1800
#define LOONGARCH_IOCSR_EXTIOI_ROUTE_BASE   0x1c00

#define  IOCSR_EXTIOI_VECTOR_NUM 256


/****************************************************************************
 * Exception Code Definitions (ECODE)
 ****************************************************************************/

/* ExStatus.ExcCode */
#define EXCCODE_RSV     0   /* Reserved */
#define EXCCODE_TLBL    1   /* TLB miss on a load */
#define EXCCODE_TLBS    2   /* TLB miss on a store */
#define EXCCODE_TLBI    3   /* TLB miss on a ifetch */
#define EXCCODE_TLBM    4   /* TLB modified fault */
#define EXCCODE_TLBNR   5   /* TLB Read-Inhibit exception */
#define EXCCODE_TLBNX   6   /* TLB Execution-Inhibit exception */
#define EXCCODE_TLBPE   7   /* TLB Privilege Error */
#define EXCCODE_ADE     8   /* Address Error */
    #define EXSUBCODE_ADEF  0   /* Fetch Instruction */
    #define EXSUBCODE_ADEM  1   /* Access Memory*/
#define EXCCODE_ALE     9   /* Unalign Access */
#define EXCCODE_BCE     10  /* Bounds Check Error */
#define EXCCODE_SYS     11  /* System call */
#define EXCCODE_BP      12  /* Breakpoint */
#define EXCCODE_INE     13  /* Inst. Not Exist */
#define EXCCODE_IPE     14  /* Inst. Privileged Error */
#define EXCCODE_FPDIS   15  /* FPU Disabled */
#define EXCCODE_LSXDIS  16  /* LSX Disabled */
#define EXCCODE_LASXDIS 17  /* LASX Disabled */
#define EXCCODE_FPE     18  /* Floating Point Exception */
    #define EXCSUBCODE_FPE  0   /* Floating Point Exception */
    #define EXCSUBCODE_VFPE 1   /* Vector Exception */
#define EXCCODE_WATCH   19  /* WatchPoint Exception */
    #define EXCSUBCODE_WPEF 0   /* ... on Instruction Fetch */
    #define EXCSUBCODE_WPEM 1   /* ... on Memory Accesses */
#define EXCCODE_BTDIS   20  /* Binary Trans. Disabled */
#define EXCCODE_BTE     21  /* Binary Trans. Exception */
#define EXCCODE_GSPR    22  /* Guest Privileged Error */
#define EXCCODE_HVC     23  /* Hypercall */
#define EXCCODE_GCM     24  /* Guest CSR modified */
    #define EXCSUBCODE_GCSC 0   /* Software caused */
    #define EXCSUBCODE_GCHC 1   /* Hardware caused */
#define EXCCODE_SE      25  /* Security */

#define EXCCODE_INT_START 64
#define EXCCODE_INT_SIP0  64
#define EXCCODE_INT_SIP1  65
#define EXCCODE_INT_IP0   66
#define EXCCODE_INT_IP1   67
#define EXCCODE_INT_IP2   68
#define EXCCODE_INT_IP3   69
#define EXCCODE_INT_IP4   70
#define EXCCODE_INT_IP5   71
#define EXCCODE_INT_IP6   72
#define EXCCODE_INT_IP7   73
#define EXCCODE_INT_PMC   74
#define EXCCODE_INT_TIMER 75
#define EXCCODE_INT_IPI   76
#define EXCCODE_INT_NMI   77
#define EXCCODE_INT_END   78

/****************************************************************************
 * CSR Read/Write Inline Macros
 ****************************************************************************/

#ifndef __ASSEMBLY__

/* CSR */
static inline u32 csr_read32(u32 reg)
{
	u32 val;
	asm volatile (
		"csrrd %[val], %[reg] \n\t"
		: [val] "=r" (val)
		: [reg] "i" (reg)
		: "memory");
	return val;
}

static inline u64 csr_read64(u32 reg)
{
	u64 val;
	asm volatile (
		"csrrd %[val], %[reg] \n\t"
		: [val] "=r" (val)
		: [reg] "i" (reg)
		: "memory");
	return val;
}

static inline void csr_write32(u32 val, u32 reg)
{
	asm volatile (
		"csrwr %[val], %[reg] \n\t"
		: [val] "+r" (val)
		: [reg] "i" (reg)
		: "memory");
}

static inline void csr_write64(u64 val, u32 reg)
{
	asm volatile (
		"csrwr %[val], %[reg] \n\t"
		: [val] "+r" (val)
		: [reg] "i" (reg)
		: "memory");
}

static inline u32 csr_xchg32(u32 val, u32 mask, u32 reg)
{
	asm volatile (
		"csrxchg %[val], %[mask], %[reg] \n\t"
		: [val] "+r" (val)
		: [mask] "r" (mask), [reg] "i" (reg)
		: "memory");
	return val;
}

static inline u64 csr_xchg64(u64 val, u64 mask, u32 reg)
{
	asm volatile (
		"csrxchg %[val], %[mask], %[reg] \n\t"
		: [val] "+r" (val)
		: [mask] "r" (mask), [reg] "i" (reg)
		: "memory");
	return val;
}

/* IOCSR */
static inline u32 iocsr_read32(u32 reg)
{
	u32 val;
	asm volatile (
		"iocsrrd.w %[val], %[reg] \n\t"
		: [val] "=r" (val)
		: [reg] "r" (reg)
		: "memory");
	return val;
}

static inline u64 iocsr_read64(u32 reg)
{
	u64 val;
	asm volatile (
		"iocsrrd.d %[val], %[reg] \n\t"
		: [val] "=r" (val)
		: [reg] "r" (reg)
		: "memory");
	return val;
}

static inline void iocsr_write8(u8 val, u32 reg)
{
	asm volatile (
		"iocsrwr.b %[val], %[reg] \n\t"
		:
		: [val] "r" (val), [reg] "r" (reg)
		: "memory");
}

static inline void iocsr_write32(u32 val, u32 reg)
{
	asm volatile (
		"iocsrwr.w %[val], %[reg] \n\t"
		:
		: [val] "r" (val), [reg] "r" (reg)
		: "memory");
}

static inline void iocsr_write64(u64 val, u32 reg)
{
	asm volatile (
		"iocsrwr.d %[val], %[reg] \n\t"
		:
		: [val] "r" (val), [reg] "r" (reg)
		: "memory");
}

/* drdtime */
static inline u64 drdtime(void)
{
	int rID = 0;
	u64 val = 0;

	__asm__ __volatile__(
		"rdtime.d %0, %1 \n\t"
		: "=r"(val), "=r"(rID)
		:
		);
	return val;
}

static inline unsigned int get_csr_cpuid(void)
{
	return csr_read32(LA_CSR_CPUID);
}

static inline void csr_any_send(unsigned int addr, unsigned int data,
				unsigned int data_mask, unsigned int cpu)
{
	uint64_t val = 0;

	val = IOCSR_ANY_SEND_BLOCKING | addr;
	val |= (cpu << IOCSR_ANY_SEND_CPU_SHIFT);
	val |= (data_mask << IOCSR_ANY_SEND_MASK_SHIFT);
	val |= ((uint64_t)data << IOCSR_ANY_SEND_BUF_SHIFT);
	iocsr_write64(val, LOONGARCH_IOCSR_ANY_SEND);
}

static inline unsigned int read_csr_excode(void)
{
    return (csr_read32(LA_CSR_ESTAT) & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;
}

static inline void write_csr_index(unsigned int idx)
{
    csr_xchg32(idx, CSR_TLBIDX_IDXM, LA_CSR_TLBIDX);
}

static inline unsigned int read_csr_pagesize(void)
{
    return (csr_read32(LA_CSR_TLBIDX) & CSR_TLBIDX_SIZEM) >> CSR_TLBIDX_SIZE;
}

static inline void write_csr_pagesize(unsigned int size)
{
    csr_xchg32(size << CSR_TLBIDX_SIZE, CSR_TLBIDX_SIZEM, LA_CSR_TLBIDX);
}

static inline unsigned int read_csr_tlbrefill_pagesize(void)
{
    return (csr_read64(LA_CSR_TLBREHI) & CSR_TLBREHI_PS) >> CSR_TLBREHI_PS_SHIFT;
}

static inline void write_csr_tlbrefill_pagesize(unsigned int size)
{
    csr_xchg64(size << CSR_TLBREHI_PS_SHIFT, CSR_TLBREHI_PS, LA_CSR_TLBREHI);
}

/* Read-Only CSR macros for specific registers */
#define read_csr_asid()			csr_read32(LA_CSR_ASID)
#define write_csr_asid(val)		csr_write32(val, LA_CSR_ASID)
#define read_csr_entryhi()		csr_read64(LA_CSR_TLBEHI)
#define write_csr_entryhi(val)		csr_write64(val, LA_CSR_TLBEHI)
#define read_csr_entrylo0()		csr_read64(LA_CSR_TLBELO0)
#define write_csr_entrylo0(val)		csr_write64(val, LA_CSR_TLBELO0)
#define read_csr_entrylo1()		csr_read64(LA_CSR_TLBELO1)
#define write_csr_entrylo1(val)		csr_write64(val, LA_CSR_TLBELO1)
#define read_csr_ecfg()			csr_read32(LA_CSR_ECFG)
#define write_csr_ecfg(val)		csr_write32(val, LA_CSR_ECFG)
#define read_csr_estat()		csr_read32(LA_CSR_ESTAT)
#define write_csr_estat(val)		csr_write32(val, LA_CSR_ESTAT)
#define read_csr_tlbidx()		csr_read32(LA_CSR_TLBIDX)
#define write_csr_tlbidx(val)		csr_write32(val, LA_CSR_TLBIDX)
#define read_csr_euen()			csr_read32(LA_CSR_EUEN)
#define write_csr_euen(val)		csr_write32(val, LA_CSR_EUEN)
#define read_csr_cpuid()		csr_read32(LA_CSR_CPUID)
#define read_csr_prcfg1()		csr_read64(LA_CSR_PRCFG1)
#define write_csr_prcfg1(val)		csr_write64(val, LA_CSR_PRCFG1)
#define read_csr_prcfg2()		csr_read64(LA_CSR_PRCFG2)
#define write_csr_prcfg2(val)		csr_write64(val, LA_CSR_PRCFG2)
#define read_csr_prcfg3()		csr_read64(LA_CSR_PRCFG3)
#define write_csr_prcfg3(val)		csr_write64(val, LA_CSR_PRCFG3)
#define read_csr_stlbpgsize()		csr_read32(LA_CSR_STLBPGSIZE)
#define write_csr_stlbpgsize(val)	csr_write32(val, LA_CSR_STLBPGSIZE)
#define read_csr_rvacfg()		csr_read32(LA_CSR_RVACFG)
#define write_csr_rvacfg(val)		csr_write32(val, LA_CSR_RVACFG)
#define write_csr_tintclear(val)	csr_write32(val, LA_CSR_TINTCLR)
#define read_csr_impctl1()		csr_read64(LA_CSR_IMPCTL1)
#define write_csr_impctl1(val)		csr_write64(val, LA_CSR_IMPCTL1)
#define write_csr_impctl2(val)		csr_write64(val, LA_CSR_IMPCTL2)

#define set_csr_estat(val)      csr_xchg32(val, val, LA_CSR_ESTAT)
#define clear_csr_estat(val)    csr_xchg32(~(val), val, LA_CSR_ESTAT)

#endif /* !__ASSEMBLY__ */
#endif /* __ARCH_LOONGARCH64_INCLUDE_CSR_H */
