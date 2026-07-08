/****************************************************************************
 * arch/loongarch/include/arch.h
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

/* This file should never be included directly but, rather,
 * only indirectly through nuttx/arch.h
 */

#ifndef __ARCH_LOONGARCH_INCLUDE_ARCH_H
#define __ARCH_LOONGARCH_INCLUDE_ARCH_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#  include <stddef.h>
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DMW_PABITS      48

#ifdef __ASSEMBLY__
#define PHYS_OFFSET     0x0
#define TO_PHYS_MASK    ((1 << DMW_PABITS) -1)
#define _CONST64_(x)    x
#else
#define PHYS_OFFSET     0x0ULL
#define TO_PHYS_MASK    ((1ULL << DMW_PABITS) -1)
#define _CONST64_(x)    x ## L
#endif

#define CACHE_BASE      0x9000000000000000
#define UNCACHE_BASE    0x8000000000000000

#define TO_PHYS(x)      (((x) & TO_PHYS_MASK))
#define TO_CAC(x)     (CACHE_BASE | ((x) & TO_PHYS_MASK))
#define TO_UNCAC(x)   (UNCACHE_BASE | ((x) & TO_PHYS_MASK))

#define readb(addr)     (*(volatile unsigned char *)TO_UNCAC(addr))
#define readw(addr)     (*(volatile unsigned short *)TO_UNCAC(addr))
#define readl(addr)     (*(volatile unsigned int *)TO_UNCAC(addr))
#define readq(addr)     (*(volatile unsigned long *)TO_UNCAC(addr))

#define writeb(val, addr)     (*(volatile unsigned char *)TO_UNCAC(addr) = (val))
#define writew(val, addr)     (*(volatile unsigned short *)TO_UNCAC(addr) = (val))
#define writel(val, addr)     (*(volatile unsigned int *)TO_UNCAC(addr) = (val))
#define writeq(val, addr)     (*(volatile unsigned long *)TO_UNCAC(addr) = (val))

#define or_writel(val, addr)     (*(volatile unsigned long *)TO_UNCAC(addr) |= (val))
#define or_writeq(val, addr)     (*(volatile unsigned long *)TO_UNCAC(addr) |= (val))
#define and_writel(val, addr)     (*(volatile unsigned long *)TO_UNCAC(addr) &= (val))
#define and_writeq(val, addr)     (*(volatile unsigned long *)TO_UNCAC(addr) &= (val))

#define STT_FUNC 2

#ifndef ASM_NL
#define ASM_NL  ;
#endif

#ifndef SYM_T_FUNC
#define SYM_T_FUNC  STT_FUNC
#endif

#define __ALIGN .align 2
#define ALIGN __ALIGN
#define SYM_L_GLOBAL(name)  .global name
#define SYM_A_ALIGN   ALIGN

#ifndef SYM_ENTRY
#define SYM_ENTRY(name, linkage, align...)  \
  linkage(name) ASM_NL  \
  align ASM_NL  \
  name:
#endif

#ifndef SYM_START
#define SYM_START(name, linkage, align...)  \
  SYM_ENTRY(name, linkage, align)
#endif

#ifndef SYM_END
#define SYM_END(name, sym_type)  \
  .type name sym_type ASM_NL  \
  .size name, .-name
#endif

#define SYM_FUNC_START(name)  \
  SYM_START(name, SYM_L_GLOBAL, SYM_A_ALIGN)

#define SYM_FUNC_END(name)  \
  SYM_END(name, SYM_T_FUNC)

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __ARCH_LOONGARCH_INCLUDE_ARCH_H */
