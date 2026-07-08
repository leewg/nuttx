/****************************************************************************
 * arch/loongarch/include/spinlock.h
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

#ifndef __ARCH_LOONGARCH_INCLUDE_SPINLOCK_H
#define __ARCH_LOONGARCH_INCLUDE_SPINLOCK_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#ifdef __ASSEMBLY__
#include <stdint.h>
#endif  /* __ASSEMBLY__ */

#include <arch/barriers.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Spinlock states */

#define SP_UNLOCKED 0   /* The Un-locked state */
#define SP_LOCKED   1   /* The Locked state */

/* Memory barriers for use with NuttX spinlock logic
 * - Data Memory Barrier (DMB) acts as a memory barrier.
 * - Data Synchronization Barrier (DSB) acts as a stronger memory barrier.
 */

#define SP_DMB() __asm__ __volatile__ ("dbar 0" : : : "memory")
#define SP_DSB() __asm__ __volatile__ ("dbar 0" : : : "memory")

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

#ifndef __ASSEMBLY__

typedef uintptr_t spinlock_t;

#if defined(CONFIG_ARCH_HAVE_TESTSET)
static inline uint8_t up_testset(volatile uint8_t *lock)
{
    uint8_t oldval, tmp;

    __asm__ __volatile__ (
        "1: \n"
        "ll.w   %0, %3, 0   \n"  /* Load linked: oldval = *lock */
        "st.w   %1, %2, 0   \n"  /* Store conditional: tmp = *lock = 1 */
        "bnez   %1, 1b      \n"  /* If store fails (tmp != 0), retry */
        : "=&r" (oldval),       /* Output: oldval */
          "=&r" (tmp)           /* Output: tmp (store result) */
        : "r" (1),              /* Input: value to store (SP_LOCKED) */
          "r" (lock)            /* Input: address of lock */
        : "memory"
    );

    return oldval;
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_LOONGARCH_INCLUDE_SPINLOCK_H */
