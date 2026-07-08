/****************************************************************************
 * arch/loongarch/include/barriers.h
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

#ifndef __ARCH_LOONGARCH_INCLUDE_BARRIERS_H
#define __ARCH_LOONGARCH_INCLUDE_BARRIERS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* LoongArch memory barriers:
 *
 * The LoongArch architecture provides the following barrier instructions:
 *   - dbar: Data barrier - orders data memory accesses.
 *   - ibar: Instruction barrier - flushes the instruction pipeline and
 *           ensures instruction fetch ordering.
 *
 * The dbar instruction takes an immediate operand (barrier type):
 *   - 0: Full data barrier (orders all load/store operations)
 *   - 1: Load-load barrier (orders loads)
 *   - 2: Store-store barrier (orders stores)
 *   - 3: Load-store barrier (orders loads before stores)
 *   - 4: Store-load barrier (orders stores before loads)
 *
 * For simplicity, we use dbar 0 as a full memory barrier.
 */

#define UP_DSB() __asm__ __volatile__ ("dbar 0" : : : "memory")
#define UP_DMB() __asm__ __volatile__ ("dbar 0" : : : "memory")
#define UP_RMB() __asm__ __volatile__ ("dbar 0" : : : "memory")
#define UP_WMB() __asm__ __volatile__ ("dbar 0" : : : "memory")

#define UP_ISB() __asm__ __volatile__ ("ibar 0" : : : "memory")

#endif  /* __ASSEMBLY__ */
#endif  /* __ARCH_LOONGARCH_INCLUDE_BARRIERS_H */
