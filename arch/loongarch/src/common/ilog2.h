/****************************************************************************
 * include/nuttx/log2.h
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

#ifndef __INCLUDE_NUTTX_LOG2_H
#define __INCLUDE_NUTTX_LOG2_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

static inline_function int __ilog2_u32(uint32_t n)
{
  return n ? (31 - __builtin_clz(n)) : 0;
}

static inline_function int __ilog2_u64(uint64_t n)
{
  return n ? (63 - __builtin_clzll(n)) : 0;
}

/****************************************************************************
 * Public Macro Definitions
 ****************************************************************************/

/**
 * ilog2 - 计算 floor(log2(n))
 * @n: 输入的正整数 (支持 uint32_t / uint64_t / unsigned long 等)
 *
 * 返回值：以 2 为底的对数向下取整结果。若 n == 0 则返回 0。
 */
#define ilog2(n) \
  (__builtin_constant_p(n) ? \
   ((n) < 2 ? 0 : \
    (sizeof(n) <= 4) ? (31 - __builtin_clz((unsigned int)(n))) : \
                       (63 - __builtin_clzll((unsigned long long)(n)))) : \
   ((sizeof(n) <= 4) ? __ilog2_u32((uint32_t)(n)) : \
                       __ilog2_u64((uint64_t)(n))))
#endif /* __INCLUDE_NUTTX_LOG2_H */
