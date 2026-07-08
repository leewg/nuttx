/****************************************************************************
 * arch/loongarch/include/setjmp.h
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

#ifndef __ARCH_LOONGARCH_INCLUDE_SETJMP_H
#define __ARCH_LOONGARCH_INCLUDE_SETJMP_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

struct setjmp_buf_s
{
    uintptr_t regs[12]; // s0 ~ s8(r23 ~ r31), $fp(r22), ra(r1), sp(r3)

    /* Float callee register : fs0 ~ fs7 */
#ifdef CONFIG_ARCH_FPU
    double fregs[8];    // fs0 ~ fs7(f24 ~ f31)
#endif
};

/* Traditional typedef for setjmp_buf */
typedef struct setjmp_buf_s jmp_buf[1];

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

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val) noreturn_function;

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __ARCH_LOONGARCH_INCLUDE_SETJMP_H */
