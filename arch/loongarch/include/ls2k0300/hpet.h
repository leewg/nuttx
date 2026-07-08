/****************************************************************************
 * arch/loongarch/include/ls2k0300/hpet.h
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

#ifndef __ARCH_LOONGARCH_INCLUDE_LS2K0300_HPET_H
#define __ARCH_LOONGARCH_INCLUDE_LS2K0300_HPET_H

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <arch/arch.h>
#include <arch/ls2k0300/irq.h>
#include <arch/ls2k0300/chip.h>

#include "la64_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* HPET Gen regs ops */
static inline uint64_t hpet_get_cap(void)
{
    return getreg64(HPET_BASE + HPET_GEN_CAP);
}

static inline void hpet_set_cfg(uint64_t value)
{
    putreg64(value, HPET_BASE + HPET_GEN_CFG);
}

static inline uint64_t hpet_get_cfg(void)
{
    return getreg64(HPET_BASE + HPET_GEN_CFG);
}

static inline uint64_t hpet_get_isr(void)
{
  return getreg64(HPET_BASE + HPET_GEN_ISR);
}

static inline void hpet_clr_isr(int n)
{
    putreg64((1ULL << n), HPET_BASE + HPET_GEN_ISR);
}

static inline uint64_t hpet_get_mcounter(void)
{
    return getreg64(HPET_BASE + HPET_MAIN_CNT);
}

/* Timer N regs ops */
static inline void hpet_set_tn_cmp(int n, uint64_t value)
{
    putreg64(value, HPET_BASE + HPET_TN_CMP(n));
}

static inline uint64_t hpet_get_tn_cmp(int n)
{
    return getreg64(HPET_BASE + HPET_TN_CMP(n));
}

static inline void hpet_set_tn_cfg(int n, uint64_t value)
{
    putreg64(value, HPET_BASE + HPET_TN_CFG(n));
}

static inline uint64_t hpet_get_tn_cfg(int n)
{
    return getreg64(HPET_BASE + HPET_TN_CFG(n));
}

#endif /* __ARCH_LOONGARCH_INCLUDE_LS2K0300_CHIP_H */
