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
#define INT_PCOV    10
#define INT_TI      11
#define INT_IPI     12
#define INT_NMI     13
#define INT_AVEC    14

/* Interrupts */
#define LA_LOC_IRQ_BASE  64
#define LA_IRQ_SWI0 0
#define LA_IRQ_SWI1 1
#define LA_IRQ_HWI2 4
#define LA_IRQ_TIMER  12
#define LA_EXT_IRQ_BASE  96

/* Register definitions (indices into exception context regs array) */
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
#define REG_CRMD    (32) /* Thread switch */
#define REG_PRMD    (33) /* save these */
#define REG_EUEN    (34) /* FPU */
#define REG_ESTAT   (35) /* IP status, ECODE */
#define REG_ERA     (36) /* Exception Return Addr */
#define REG_ECFG    (37) /* IP enable */
#define REG_BADV    (38) /* Bad Vaddr */

#define REG_ZERO  (REG_R0  * 8)     /* $0  - Always zero */
#define REG_RA    (REG_R1  * 8)     /* $1  - Return address */
#define REG_TP    (REG_R2  * 8)     /* $2  - Stack pointer */
#define REG_SP    (REG_R3  * 8)     /* $3  - Global pointer */
#define REG_A0    (REG_R4  * 8)     /* $4  - Thread pointer */
#define REG_A1    (REG_R5  * 8)     /* $5  - Argument/Return 0 */
#define REG_A2    (REG_R6  * 8)     /* $6  - Argument/Return 1 */
#define REG_A3    (REG_R7  * 8)     /* $7  - Argument 2 */
#define REG_A4    (REG_R8  * 8)     /* $8  - Argument 3 */
#define REG_A5    (REG_R9  * 8)     /* $9  - Argument 4 */
#define REG_A6    (REG_R10 * 8)    /* $10 - Argument 5 */
#define REG_A7    (REG_R11 * 8)    /* $11 - Argument 6 */
#define REG_T0    (REG_R12 * 8)    /* $12 - Argument 7 */
#define REG_T1    (REG_R13 * 8)    /* $13 - Temporary 0 */
#define REG_T2    (REG_R14 * 8)    /* $14 - Temporary 1 */
#define REG_T3    (REG_R15 * 8)    /* $15 - Temporary 2 */
#define REG_T4    (REG_R16 * 8)    /* $16 - Temporary 3 */
#define REG_T5    (REG_R17 * 8)    /* $17 - Temporary 4 */
#define REG_T6    (REG_R18 * 8)    /* $18 - Temporary 5 */
#define REG_T7    (REG_R19 * 8)    /* $19 - Temporary 6 */
#define REG_T8    (REG_R20 * 8)    /* $20 - Temporary 7 */
#define REG_U0    (REG_R21 * 8)    /* $21 - Temporary 8 */
#define REG_FP    (REG_R22 * 8)    /* $22 - Reserved (nomips16) */
#define REG_S0    (REG_R23 * 8)    /* $23 - Frame pointer */
#define REG_S1    (REG_R24 * 8)    /* $24 - Saved 0 */
#define REG_S2    (REG_R25 * 8)    /* $25 - Saved 1 */
#define REG_S3    (REG_R26 * 8)    /* $26 - Saved 2 */
#define REG_S4    (REG_R27 * 8)    /* $27 - Saved 3 */
#define REG_S5    (REG_R28 * 8)    /* $28 - Saved 4 */
#define REG_S6    (REG_R29 * 8)    /* $29 - Saved 5 */
#define REG_S7    (REG_R30 * 8)    /* $30 - Saved 6 */
#define REG_S8    (REG_R31 * 8)    /* $31 - Saved 7 */

/* Interrupt Context register */
#define INT_XCPT_REGS    (40)
#define INT_XCPT_SIZE    (8 * INT_XCPT_REGS)

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
#  define REG_FCSR      (33)

#  define FPU_XCPT_REGS     (34)
#else /* !CONFIG_ARCH_FPU */
#  define FPU_XCPT_REGS     (0)
#endif /* CONFIG_ARCH_FPU */

#define FPU_XCPT_SIZE     (8 * FPU_XCPT_REGS)

#define XCPTCONTEXT_REGS (INT_XCPT_REGS + FPU_XCPT_REGS)
#define XCPTCONTEXT_SIZE (8 * XCPTCONTEXT_REGS)

#if XCPTCONTEXT_SIZE % 16 != 0
#  error "INT_CTX_REGS and FPU_CTX_REGS must be an even number to ensure the stack is aligned to 16 bytes"
#endif

#define zero  $r0
#define ra    $r1
#define gp    $r2
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
#define tp    $r21
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
       "move %0, $sp\n"
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

#ifdef CONFIG_ARCH_HAVE_MULTICPU
static inline_function int up_cpu_index(void)
{
  uint64_t cpuid;

  __asm__ __volatile__ (
    "csrrd %0, 0x20\n" /* Read cpuid register */
    : "=r" (cpuid)
    :
    : "memory"
  );

  return (int)(cpuid & 0xff);
}
#endif /* CONFIG_ARCH_HAVE_MULTICPU */

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
    uint64_t flags = 0;

    __asm__ __volatile__(
        "csrxchg %[val], %[mask], %[reg]\n\t"
        : [val] "+r" (flags)
        : [mask] "r" (CSR_CRMD_IE), [reg] "i" (LOONGARCH_CSR_CRMD)
        : "memory");

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
    __asm__ __volatile__(
        "csrxchg %[val], %[mask], %[reg]\n\t"
        : [val] "+r" (flags)
        : [mask] "r" (CSR_CRMD_IE), [reg] "i" (LOONGARCH_CSR_CRMD)
        : "memory");
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

  __asm__ __volatile__(
    "csrxchg %[val], %[mask], %[reg]\n\t"
    : [val] "+r" (flags)
    : [mask] "r" (CSR_CRMD_IE), [reg] "i" (LOONGARCH_CSR_CRMD)
    : "memory");

  return flags;
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
    u32 flags = 0;

    __asm__ __volatile__(
        "csrxchg %[val], %[mask], %[reg]\n\t"
        : [val] "+r" (flags)
        : [mask] "r" (CSR_CRMD_IE), [reg] "i" (LOONGARCH_CSR_CRMD)
        : "memory");
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
    __asm__ __volatile__(
        "csrwr %0, %1"
        : "+r"(flag)
        : "i"(LOONGARCH_CSR_KS3)
        : "memory");
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
    unsigned long ctx;

    __asm__ __volatile__(
        "csrrd %0, %1\n\t"
        : "=r" (ctx)
        : "i" (LOONGARCH_CSR_KS3)
        : "memory");

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
