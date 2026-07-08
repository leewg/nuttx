/****************************************************************************
 * arch/loongarch/src/common/la64_percpu.c
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
#include <nuttx/irq.h>
#include <nuttx/queue.h>
#include <nuttx/spinlock.h>

#include <arch/barriers.h>

#include <assert.h>
#include <stdint.h>

#include "la64_internal.h"
#include "la64_percpu.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

struct la64_percpu_s g_percpu[CONFIG_SMP_NCPUS];

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void up_percpu_initialize(int cpu)
{
  FAR struct la64_percpu_s *percpu = &g_percpu[cpu];

  percpu->cur_regs = 0;

#if CONFIG_ARCH_INTERRUPTSTACK > 0
  percpu->irq_stack = (uintptr_t)g_intstackalloc[cpu] + CONFIG_ARCH_INTERRUPTSTACK;
  percpu->irq_stack &= ~0xf;
#else
  percpu->irq_stack = 0;
#endif

#if CONFIG_SMP_NCPUS > 1
  percpu->cpu = cpu;
#endif

  la64_percpu_init(cpu);
}

FAR uintptr_t *up_current_regs(void)
{
  return (FAR uintptr_t *)la64_my_percpu()->cur_regs;
}
