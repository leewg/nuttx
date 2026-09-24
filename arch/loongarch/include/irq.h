/****************************************************************************
 * arch/loongarch/include/irq.h
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

#ifndef __ARCH_LOONGARCH_INCLUDE_IRQ_H
#define __ARCH_LOONGARCH_INCLUDE_IRQ_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <arch/types.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#endif

#include <arch/csr.h>
#include <arch/chip/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef __ASSEMBLY__
#define __STR(s)    s
#else
#define __STR(s)    #s
#endif
#define __XSTR(s)   __STR(s)

/* Loongarch requires a 16-byte stack alignment */
#define STACKFRAME_ALIGN  16

/* Core Local IRQs */
#define INT_SWI0    0
#define INT_SWI1    1
#define INT_HWI0    2
#define INT_HWI1    3
#define INT_HWI2    4
#define INT_HWI3    5
#define INT_HWI4    6
#define INT_HWI5    7
#define INT_HWI6    8
#define INT_HWI7    9
#define INT_PMC     10
#define INT_TI      11
#define INT_IPI     12
#define INT_NMI     13
#define INT_AVEC    14

/* Interrupts */
#define LA_LOC_IRQ_BASE (32)
#define LA_IRQ_SWI0     (LA_LOC_IRQ_BASE + INT_SWI0)
#define LA_IRQ_SWI1     (LA_LOC_IRQ_BASE + INT_SWI1)
#define LA_IRQ_HWI0     (LA_LOC_IRQ_BASE + INT_HWI0)
#define LA_IRQ_HWI1     (LA_LOC_IRQ_BASE + INT_HWI1)
#define LA_IRQ_HWI2     (LA_LOC_IRQ_BASE + INT_HWI2)
#define LA_IRQ_HWI3     (LA_LOC_IRQ_BASE + INT_HWI3)
#define LA_IRQ_HWI4     (LA_LOC_IRQ_BASE + INT_HWI4)
#define LA_IRQ_HWI5     (LA_LOC_IRQ_BASE + INT_HWI5)
#define LA_IRQ_HWI6     (LA_LOC_IRQ_BASE + INT_HWI6)
#define LA_IRQ_HWI7     (LA_LOC_IRQ_BASE + INT_HWI7)
#define LA_IRQ_PMC      (LA_LOC_IRQ_BASE + INT_PMC)
#define LA_IRQ_TIMER    (LA_LOC_IRQ_BASE + INT_TI)
#define LA_IRQ_IPI      (LA_LOC_IRQ_BASE + INT_IPI)
#define LA_IRQ_NMI      (LA_LOC_IRQ_BASE + INT_NMI)
#define LA_IRQ_AVEC     (LA_LOC_IRQ_BASE + INT_AVEC)

#define LA_EXT_IRQ_BASE   64

/* Register definitions (indices into exception context regs array) */
#ifdef CONFIG_ARCH_LA64
#define INT_REG_WIDTH   8
#define FPU_REG_WIDTH   8
#else
#define INT_REG_WIDTH   4
#define FPU_REG_WIDTH   4
#endif

#define REG_R0   (0)
#define REG_R1   (1)
#define REG_R2   (2)
#define REG_R3   (3)
#define REG_R4   (4)
#define REG_R5   (5)
#define REG_R6   (6)
#define REG_R7   (7)
#define REG_R8   (8)
#define REG_R9   (9)
#define REG_R10  (10)
#define REG_R11  (11)
#define REG_R12  (12)
#define REG_R13  (13)
#define REG_R14  (14)
#define REG_R15  (15)
#define REG_R16  (16)
#define REG_R17  (17)
#define REG_R18  (18)
#define REG_R19  (19)
#define REG_R20  (20)
#define REG_R21  (21)
#define REG_R22  (22)
#define REG_R23  (23)
#define REG_R24  (24)
#define REG_R25  (25)
#define REG_R26  (26)
#define REG_R27  (27)
#define REG_R28  (28)
#define REG_R29  (29)
#define REG_R30  (30)
#define REG_R31  (31)
/* Saved special registers */
#define CSR_CRMD    (32) /* Thread switch */
#define CSR_PRMD    (33) /* save these */
#define CSR_EUEN    (34) /* FPU */
#define CSR_ESTAT   (35) /* IP status, ECODE */
#define CSR_ERA     (36) /* Exception Return Addr */
#define CSR_ECFG    (37) /* IP enable */
#define CSR_BADV    (38) /* Bad Vaddr */

#define INT_XCPT_REGS    (40)

#define TP_ZERO  (REG_R0  * INT_REG_WIDTH)     /* $0  - Always zero */
#define TP_RA    (REG_R1  * INT_REG_WIDTH)     /* $1  - Return address */
#define TP_TP    (REG_R2  * INT_REG_WIDTH)     /* $2  - Thread pointer */
#define TP_SP    (REG_R3  * INT_REG_WIDTH)     /* $3  - Stack pointer */
#define TP_A0    (REG_R4  * INT_REG_WIDTH)     /* $4  - Argument/Return 0 */
#define TP_A1    (REG_R5  * INT_REG_WIDTH)     /* $5  - Argument/Return 1 */
#define TP_A2    (REG_R6  * INT_REG_WIDTH)     /* $6  - Argument 2 */
#define TP_A3    (REG_R7  * INT_REG_WIDTH)     /* $7  - Argument 3 */
#define TP_A4    (REG_R8  * INT_REG_WIDTH)     /* $8  - Argument 4 */
#define TP_A5    (REG_R9  * INT_REG_WIDTH)     /* $9  - Argument 5 */
#define TP_A6    (REG_R10 * INT_REG_WIDTH)    /* $10 - Argument 6 */
#define TP_A7    (REG_R11 * INT_REG_WIDTH)    /* $11 - Argument 7 */
#define TP_T0    (REG_R12 * INT_REG_WIDTH)    /* $12 - Temporary 0 */
#define TP_T1    (REG_R13 * INT_REG_WIDTH)    /* $13 - Temporary 1 */
#define TP_T2    (REG_R14 * INT_REG_WIDTH)    /* $14 - Temporary 2 */
#define TP_T3    (REG_R15 * INT_REG_WIDTH)    /* $15 - Temporary 3 */
#define TP_T4    (REG_R16 * INT_REG_WIDTH)    /* $16 - Temporary 4 */
#define TP_T5    (REG_R17 * INT_REG_WIDTH)    /* $17 - Temporary 5 */
#define TP_T6    (REG_R18 * INT_REG_WIDTH)    /* $18 - Temporary 5 */
#define TP_T7    (REG_R19 * INT_REG_WIDTH)    /* $19 - Temporary 6 */
#define TP_T8    (REG_R20 * INT_REG_WIDTH)    /* $20 - Temporary 8 */
#define TP_U0    (REG_R21 * INT_REG_WIDTH)    /* $21 - Reserved */
#define TP_FP    (REG_R22 * INT_REG_WIDTH)    /* $22 - Frame pointer */
#define TP_S0    (REG_R23 * INT_REG_WIDTH)    /* $23 - Saved 0 */
#define TP_S1    (REG_R24 * INT_REG_WIDTH)    /* $24 - Saved 1 */
#define TP_S2    (REG_R25 * INT_REG_WIDTH)    /* $25 - Saved 2 */
#define TP_S3    (REG_R26 * INT_REG_WIDTH)    /* $26 - Saved 3 */
#define TP_S4    (REG_R27 * INT_REG_WIDTH)    /* $27 - Saved 4 */
#define TP_S5    (REG_R28 * INT_REG_WIDTH)    /* $28 - Saved 5 */
#define TP_S6    (REG_R29 * INT_REG_WIDTH)    /* $29 - Saved 6 */
#define TP_S7    (REG_R30 * INT_REG_WIDTH)    /* $30 - Saved 7 */
#define TP_S8    (REG_R31 * INT_REG_WIDTH)    /* $31 - Saved 8 */

#define TP_CRMD    (CSR_CRMD  * INT_REG_WIDTH) /* Thread switch */
#define TP_PRMD    (CSR_PRMD  * INT_REG_WIDTH) /* save these */
#define TP_EUEN    (CSR_EUEN  * INT_REG_WIDTH) /* FPU */
#define TP_ESTAT   (CSR_ESTAT * INT_REG_WIDTH) /* IP status, ECODE */
#define TP_ERA     (CSR_ERA   * INT_REG_WIDTH) /* Exception Return Addr */
#define TP_ECFG    (CSR_ECFG  * INT_REG_WIDTH) /* IP enable */
#define TP_BADV    (CSR_BADV  * INT_REG_WIDTH) /* Bad Vaddr */

#ifdef CONFIG_ARCH_FPU
#  define REG_F0        (0)
#  define REG_F1        (1)
#  define REG_F2        (2)
#  define REG_F3        (3)
#  define REG_F4        (4)
#  define REG_F5        (5)
#  define REG_F6        (6)
#  define REG_F7        (7)
#  define REG_F8        (8)
#  define REG_F9        (9)
#  define REG_F10       (10)
#  define REG_F11       (11)
#  define REG_F12       (12)
#  define REG_F13       (13)
#  define REG_F14       (14)
#  define REG_F15       (15)
#  define REG_F16       (16)
#  define REG_F17       (17)
#  define REG_F18       (18)
#  define REG_F19       (19)
#  define REG_F20       (20)
#  define REG_F21       (21)
#  define REG_F22       (22)
#  define REG_F23       (23)
#  define REG_F24       (24)
#  define REG_F25       (25)
#  define REG_F26       (26)
#  define REG_F27       (27)
#  define REG_F28       (28)
#  define REG_F29       (29)
#  define REG_F30       (30)
#  define REG_F31       (31)
#  define REG_FCC0      (32)
#  define REG_FCC1      (33)
#  define REG_FCC2      (34)
#  define REG_FCC3      (35)
#  define REG_FCC4      (36)
#  define REG_FCC5      (37)
#  define REG_FCC6      (38)
#  define REG_FCC7      (39)
#  define REG_FCSR      (40)
#  define FPU_XCPT_REGS     (40)

#  define TP_F0   (FPU_REG_WIDTH * REG_F0)
#  define TP_F1   (FPU_REG_WIDTH * REG_F1)
#  define TP_F2   (FPU_REG_WIDTH * REG_F2)
#  define TP_F3   (FPU_REG_WIDTH * REG_F3)
#  define TP_F4   (FPU_REG_WIDTH * REG_F4)
#  define TP_F5   (FPU_REG_WIDTH * REG_F5)
#  define TP_F6   (FPU_REG_WIDTH * REG_F6)
#  define TP_F7   (FPU_REG_WIDTH * REG_F7)
#  define TP_F8   (FPU_REG_WIDTH * REG_F8)
#  define TP_F9   (FPU_REG_WIDTH * REG_F9)
#  define TP_F10  (FPU_REG_WIDTH * REG_F10)
#  define TP_F11  (FPU_REG_WIDTH * REG_F11)
#  define TP_F12  (FPU_REG_WIDTH * REG_F12)
#  define TP_F13  (FPU_REG_WIDTH * REG_F13)
#  define TP_F14  (FPU_REG_WIDTH * REG_F14)
#  define TP_F15  (FPU_REG_WIDTH * REG_F15)
#  define TP_F16  (FPU_REG_WIDTH * REG_F16)
#  define TP_F17  (FPU_REG_WIDTH * REG_F17)
#  define TP_F18  (FPU_REG_WIDTH * REG_F18)
#  define TP_F19  (FPU_REG_WIDTH * REG_F19)
#  define TP_F20  (FPU_REG_WIDTH * REG_F20)
#  define TP_F21  (FPU_REG_WIDTH * REG_F21)
#  define TP_F22  (FPU_REG_WIDTH * REG_F22)
#  define TP_F23  (FPU_REG_WIDTH * REG_F23)
#  define TP_F24  (FPU_REG_WIDTH * REG_F24)
#  define TP_F25  (FPU_REG_WIDTH * REG_F25)
#  define TP_F26  (FPU_REG_WIDTH * REG_F26)
#  define TP_F27  (FPU_REG_WIDTH * REG_F27)
#  define TP_F28  (FPU_REG_WIDTH * REG_F28)
#  define TP_F29  (FPU_REG_WIDTH * REG_F29)
#  define TP_F30  (FPU_REG_WIDTH * REG_F30)
#  define TP_F31  (FPU_REG_WIDTH * REG_F31)
#  define TP_FCC0 (FPU_REG_WIDTH * REG_FCC0)
#  define TP_FCC1 (FPU_REG_WIDTH * REG_FCC1)
#  define TP_FCC2 (FPU_REG_WIDTH * REG_FCC2)
#  define TP_FCC3 (FPU_REG_WIDTH * REG_FCC3)
#  define TP_FCC4 (FPU_REG_WIDTH * REG_FCC4)
#  define TP_FCC5 (FPU_REG_WIDTH * REG_FCC5)
#  define TP_FCC6 (FPU_REG_WIDTH * REG_FCC6)
#  define TP_FCC7 (FPU_REG_WIDTH * REG_FCC7)
#  define TP_FCSR (FPU_REG_WIDTH * REG_FCSR)

#else /* !CONFIG_ARCH_FPU */
#  define FPU_XCPT_REGS     (0)
#endif /* CONFIG_ARCH_FPU */

#define INT_XCPT_SIZE    (INT_REG_WIDTH * INT_XCPT_REGS)
#define FPU_XCPT_SIZE    (FPU_REG_WIDTH * FPU_XCPT_REGS)

#define XCPTCONTEXT_REGS (INT_XCPT_REGS + FPU_XCPT_REGS)
#define XCPTCONTEXT_SIZE (INT_XCPT_SIZE + FPU_XCPT_SIZE)

#if XCPTCONTEXT_SIZE % 16 != 0
#  error "INT_CTX_REGS and FPU_CTX_REGS must be an even number to ensure the stack is aligned to 16 bytes"
#endif

/* friendly define */
#define REG_ZERO  (REG_R0)     /* $0  - Always zero */
#define REG_RA    (REG_R1)     /* $1  - Return address */
#define REG_TP    (REG_R2)     /* $2  - Stack pointer */
#define REG_SP    (REG_R3)     /* $3  - Global pointer */
#define REG_A0    (REG_R4)     /* $4  - Thread pointer */
#define REG_A1    (REG_R5)     /* $5  - Argument/Return 0 */
#define REG_A2    (REG_R6)     /* $6  - Argument/Return 1 */
#define REG_A3    (REG_R7)     /* $7  - Argument 2 */
#define REG_A4    (REG_R8)     /* $8  - Argument 3 */
#define REG_A5    (REG_R9)     /* $9  - Argument 4 */
#define REG_A6    (REG_R10)    /* $10 - Argument 5 */
#define REG_A7    (REG_R11)    /* $11 - Argument 6 */
#define REG_T0    (REG_R12)    /* $12 - Argument 7 */
#define REG_T1    (REG_R13)    /* $13 - Temporary 0 */
#define REG_T2    (REG_R14)    /* $14 - Temporary 1 */
#define REG_T3    (REG_R15)    /* $15 - Temporary 2 */
#define REG_T4    (REG_R16)    /* $16 - Temporary 3 */
#define REG_T5    (REG_R17)    /* $17 - Temporary 4 */
#define REG_T6    (REG_R18)    /* $18 - Temporary 5 */
#define REG_T7    (REG_R19)    /* $19 - Temporary 6 */
#define REG_T8    (REG_R20)    /* $20 - Temporary 7 */
#define REG_U0    (REG_R21)    /* $21 - Temporary 8 */
#define REG_FP    (REG_R22)    /* $22 - Reserved (nomips16) */
#define REG_S0    (REG_R23)    /* $23 - Frame pointer */
#define REG_S1    (REG_R24)    /* $24 - Saved 0 */
#define REG_S2    (REG_R25)    /* $25 - Saved 1 */
#define REG_S3    (REG_R26)    /* $26 - Saved 2 */
#define REG_S4    (REG_R27)    /* $27 - Saved 3 */
#define REG_S5    (REG_R28)    /* $28 - Saved 4 */
#define REG_S6    (REG_R29)    /* $29 - Saved 5 */
#define REG_S7    (REG_R30)    /* $30 - Saved 6 */
#define REG_S8    (REG_R31)    /* $31 - Saved 7 */

#define REG_CRMD    CSR_CRMD  /* Thread switch */
#define REG_PRMD    CSR_PRMD  /* save these */
#define REG_EUEN    CSR_EUEN  /* FPU */
#define REG_ESTAT   CSR_ESTAT /* IP status, ECODE */
#define REG_ERA     CSR_ERA   /* Exception Return Addr */
#define REG_ECFG    CSR_ECFG  /* IP enable */
#define REG_BADV    CSR_BADV  /* Bad Vaddr */

#define zero  $r0
#define ra    $r1
#define tp    $r2
#define sp    $r3
#define v0    $r4
#define v1    $r5
#define a0    $r4
#define a1    $r5
#define a2    $r6
#define a3    $r7
#define a4    $r8
#define a5    $r9
#define a6    $r10
#define a7    $r11
#define t0    $r12
#define t1    $r13
#define t2    $r14
#define t3    $r15
#define t4    $r16
#define t5    $r17
#define t6    $r18
#define t7    $r19
#define t8    $r20
#define u0    $r21
#define fp    $r22
#define s0    $r23
#define s1    $r24
#define s2    $r25
#define s3    $r26
#define s4    $r27
#define s5    $r28
#define s6    $r29
#define s7    $r30
#define s8    $r31

/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifndef __ASSEMBLY__

/* The LoongArch64 exception context structure.
 *
 * This structure defines the full processor state that must be saved and
 * restored during exception handling and context switching.
 */

struct xcptcontext
{
#ifdef CONFIG_ENABLE_ALL_SIGNALS
  /* task context, for signal process */

  uint64_t *saved_regs;

#if defined(CONFIG_BUILD_KERNEL) || defined(CONFIG_BUILD_PROTECTED)
  /* This is the saved address to use when returning from a user-space
   * signal handler.
   */

  uintptr_t sigreturn;

#endif
#endif /* CONFIG_ENABLE_ALL_SIGNALS */

#ifdef CONFIG_ARCH_ADDRENV
#ifdef CONFIG_ARCH_KERNEL_STACK
  /* In this configuration, all syscalls execute from an internal kernel
   * stack.
   */
  uintptr_t *ustkptr;   /* Saved user stack pointer */
  uintptr_t *kstack;    /* Allocate base of the (aligned) kernel stack */
  uintptr_t *ktopstk;   /* Top of kernel stack */
  uintptr_t *kstkptr;   /* Saved kernel stack pointer */
#endif
#endif

  /* Interger register save area */

  uint64_t *regs;
#ifndef CONFIG_BUILD_FLAG
  uint64_t *initregs;
#endif

#ifdef CONFIG_LIB_SYSCALL
  /* User integer registers upon system call entry */
  uint64_t *sregs;
#endif

  /* FPU register save area */
#ifdef CONFIG_ARCH_FPU
  uint64_t *fprs;     /* Floating-point registers */
#endif
};

/****************************************************************************
 * Inline functions
 ****************************************************************************/

/* Return the current value of the stack pointer */
static inline_function uintptr_t up_getsp(void)
{
  register uintptr_t sp;
  __asm__ __volatile__
      (
       "move %0, sp\n"
       : "=r"(sp)
       );

  return sp;
}

/****************************************************************************
 * Public Data
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: up_cpu_index
 *
 * Description:
 *   Return the real core number regardless CONFIG_SMP setting,
 *
 ****************************************************************************/

static inline_function int up_cpu_index(void)
{
#ifdef CONFIG_ARCH_HAVE_MULTICPU
  uint32_t cpuid = csr_read32(LA_CSR_CPUID);

  return (int)(cpuid & 0x1ff);
#else
  return 0;
#endif /* CONFIG_ARCH_HAVE_MULTICPU */
}

/****************************************************************************
 * Name: up_irq_save
 *
 * Description:
 *   Disable all interrupts and return the previous interrupt state.
 *
 *   This function operates on the CRMD.IE (Interrupt Enable) bit.
 *   Returns the full CRMD value so that it can be restored later
 *   via up_irq_restore().
 *
 * Returned Value:
 *   The full CRMD register value before interrupts were disabled.
 *
 ****************************************************************************/

static inline irqstate_t up_irq_save(void)
{
    uint32_t flags = 0;

    return __csrxchg(flags, CSR_CRMD_IE, LA_CSR_CRMD);

    return flags;
}

/****************************************************************************
 * Name: up_irq_restore
 *
 * Description:
 *   Restore the previous interrupt state.
 *
 *   This function writes the saved value back to the CRMD register.
 *
 * Input Parameters:
 *   flags - The value previously returned by up_irq_save().
 *
 ****************************************************************************/

static inline void up_irq_restore(irqstate_t flags)
{
  /* if flags[bit2] == 1 or flags == 1/true, enable IE (0x4) */
  irqstate_t ie = ((flags & CSR_CRMD_IE) || flags == 1) ? CSR_CRMD_IE : 0;

  __csrxchg(ie, CSR_CRMD_IE, LA_CSR_CRMD);
}

/****************************************************************************
 * Name: up_irq_enable
 *
 * Description:
 *   Enable all interrupts.
 *
 ****************************************************************************/

static inline irqstate_t up_irq_enable(void)
{
  irqstate_t flags = CSR_CRMD_IE;

  return __csrxchg(flags, CSR_CRMD_IE, LA_CSR_CRMD);
}

/****************************************************************************
 * Name: up_irq_disable
 *
 * Description:
 *   Disable all interrupts.
 *
 ****************************************************************************/

static inline void up_irq_disable(void)
{
  uint32_t flags = 0;

  __csrxchg(flags, CSR_CRMD_IE, LA_CSR_CRMD);
}

/****************************************************************************
 * Name: up_set_interrupt_context
 *
 * Description:
 *   Set the interrupt handler context.
 *
 ****************************************************************************/

noinstrument_function
static inline_function void up_set_interrupt_context(bool flag)
{
  __dcsrwr(flag, LA_CSR_KS4);
}

/****************************************************************************
 * Name: up_interrupt_context
 *
 * Description:
 *   Return true is we are currently executing in the interrupt
 *   handler context.
 *
 ****************************************************************************/

noinstrument_function static inline_function bool up_interrupt_context(void)
{
  uint64_t ctx =  __dcsrrd(LA_CSR_KS4);

  return ctx > 0;
}

/****************************************************************************
 * Name: up_getusrpc
 ****************************************************************************/

#define up_getusrpc(regs) \
    (((uintptr_t *)((regs) ? (regs) : running_regs()))[REG_ERA])

/****************************************************************************
 * Name: up_getusrsp
 ****************************************************************************/

#define up_getusrsp(regs) \
    (((uintptr_t*)(regs))[REG_SP])

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_LOONGARCH64_INCLUDE_IRQ_H */
