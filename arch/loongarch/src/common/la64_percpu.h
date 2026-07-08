/****************************************************************************
 * arch/loongarch/src/common/la64_percpu.h
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

#ifndef __ARCH_LOONGARCH_SRC_COMMON_LA64_PERCPU_H
#define __ARCH_LOONGARCH_SRC_COMMON_LA64_PERCPU_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <arch/irq.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#  include <nuttx/arch.h>
#endif /* __ASSEMBLY__ */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define PERCPU_CURR_REGS_OFFSET 0x0
#define PERCPU_IRQ_STACK_OFFSET 0x8

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct la64_percpu_s
{
  uintreg_t cur_regs;
  uintreg_t irq_stack;
#if CONFIG_SMP_NCPUS > 1
  uint32_t  cpu; /* cpu id */
#endif
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

extern struct la64_percpu_s g_percpu[CONFIG_SMP_NCPUS];

/* Get current CPU's Per-CPU struct pointer */
static inline_function FAR struct la64_percpu_s *la64_my_percpu(void)
{
  uintptr_t val;
  __asm__ __volatile__("csrrd %0, %1" : "=r"(val) : "i"(LOONGARCH_CSR_KS3));

  return (FAR struct la64_percpu_s *)val;
}

/* Init current CPU's KS3 reg */
static inline_function void la64_percpu_init(int cpu)
{
  uintptr_t val = (uintptr_t)&g_percpu[cpu];
  __asm__ __volatile__("csrwr %0, %1" : : "r"(val), "i"(LOONGARCH_CSR_KS3));
}

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_LOONGARCH_SRC_COMMON_LA64_PERCPU_H */
