/****************************************************************************
 * arch/loongarch/include/syscall.h
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
 * through include/syscall.h or include/sys/sycall.h
 */

#ifndef __ARCH_LOONGARCH_INCLUDE_SYSCALL_H
#define __ARCH_LOONGARCH_INCLUDE_SYSCALL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SYS_syscall 0x00

/* Configuration ************************************************************/

/* This logic uses three system calls {0,1,2} for context switching and one
 * for the syscall return.  So a minimum of four syscall values must be
 * reserved.  If CONFIG_BUILD_FLAT isn't defined, then four more syscall
 * values must be reserved.
 */

#ifndef CONFIG_BUILD_FLAT
#  define CONFIG_SYS_RESERVED 8
#else
#  define CONFIG_SYS_RESERVED 4
#endif

/* LoongArch64 system calls ******************************************************/

#define SYS_save_context          (0)

/* SYS call 1:
 *
 * void loongarch64_fullcontextrestore() noreturn_function;
 */

#define SYS_restore_context       (1)

/* SYS call 2:
 *
 * void loongarch64_switchcontext();
 */

#define SYS_switch_context        (2)

#ifndef CONFIG_BUILD_FLAT

/* SYS call 4:
 *
 * void signal_handler(_sa_sigaction_t sighand, int signo,
 *                     siginfo_t *info, void *ucontext);
 */

#define SYS_signal_handler        (4)

/* SYS call 5:
 *
 * void signal_handler_return(void);
 */

#define SYS_signal_handler_return (5)
#endif /* !CONFIG_BUILD_FLAT */

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Inline functions
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Data
 ****************************************************************************/

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

/****************************************************************************
 * Name: sys_call0
 *
 * Description:
 *   System call SYS_ argument and no additional parameters.
 *
 *   LoongArch64 Calling Convention:
 *   - $a7: syscall number
 *   - Return value in $a0
 ****************************************************************************/
#define SYSCALL_CLOBBERLIST \
  "$t0", "$t1", "$t2", "$t3", \
  "$t4", "$t5", "$t6", "$t7", "$t8", "memory"

static inline uintptr_t sys_call0(unsigned int nbr)
{
  register long a7 __asm__("$a7") = nbr;
  register long a0 __asm__("$a0");

  __asm__ __volatile__(
      "syscall 0 \n"
      : "=r"(a0)
      : "r"(a7)
      : SYSCALL_CLOBBERLIST);

  return (uintptr_t)a0;
}

/****************************************************************************
 * Name: sys_call1
 *
 * Description:
 *   System call SYS_ argument and one additional parameter.
 *
 *   LoongArch64 Calling Convention:
 *   - $a7: syscall number
 *   - $a0: parameter 1
 *   - Return value in $a0
 ****************************************************************************/

static inline uintptr_t sys_call1(unsigned int nbr, uintptr_t parm1)
{
  register long sys_nr __asm__("a7") = nbr;
  register long arg1   __asm__("a0")  = (long)parm1;
  register long ret    __asm__("a0");

  __asm__ __volatile__(
      "syscall 0\n"
      : "=r"(ret)
      : "r"(sys_nr), "0"(arg1)
      : "memory"
    );

  return (uintptr_t)ret;
}

/****************************************************************************
 * Name: sys_call2
 *
 * Description:
 *   System call SYS_ argument and two additional parameters.
 *
 *   LoongArch64 Calling Convention:
 *   - $a7: syscall number
 *   - $a0,a1: parameter 1,2
 *   - Return value in $a0
 ****************************************************************************/

static inline uintptr_t sys_call2(unsigned int nbr, uintptr_t parm1,
                                  uintptr_t parm2)
{
  register long sys_nr __asm__("a7") = nbr;
  register long arg1   __asm__("a0")  = (long)parm1;
  register long arg2   __asm__("a1")  = (long)parm2;
  register long ret    __asm__("a0");

  __asm__ __volatile__(
      "syscall 0\n"
      : "=r"(ret)
      : "r"(sys_nr), "0"(arg1), "r"(arg2)
      : "memory"
    );

  return (uintptr_t)ret;
}

/****************************************************************************
 * Name: sys_call3
 *
 * Description:
 *   System call SYS_ argument and three additional parameters.
 *
 *   LoongArch64 Calling Convention:
 *   - $a7: syscall number
 *   - $a0,a1,a2: parameter 1,2,3
 *   - Return value in $a0
 ****************************************************************************/

static inline uintptr_t sys_call3(unsigned int nbr, uintptr_t parm1,
                                  uintptr_t parm2, uintptr_t parm3)
{
  register long sys_nr __asm__("a7") = nbr;
  register long arg1   __asm__("a0")  = (long)parm1;
  register long arg2   __asm__("a1")  = (long)parm2;
  register long arg3   __asm__("a2")  = (long)parm3;
  register long ret    __asm__("a0");

  __asm__ __volatile__(
      "syscall 0\n"
      : "=r"(ret)
      : "r"(sys_nr), "0"(arg1), "r"(arg2), "r"(arg3)
      : "memory"
    );

  return (uintptr_t)ret;
}

/****************************************************************************
 * Name: sys_call4
 *
 * Description:
 *   System call SYS_ argument and four additional parameters.
 *
 *   LoongArch64 Calling Convention:
 *   - $a7: syscall number
 *   - $a0,a1,a2,a3: parameter 1,2,3,4
 *   - Return value in $a0
 ****************************************************************************/

static inline uintptr_t sys_call4(unsigned int nbr, uintptr_t parm1,
                                  uintptr_t parm2, uintptr_t parm3,
                                  uintptr_t parm4)
{
  register long sys_nr __asm__("a7") = nbr;
  register long arg1   __asm__("a0")  = (long)parm1;
  register long arg2   __asm__("a1")  = (long)parm2;
  register long arg3   __asm__("a2")  = (long)parm3;
  register long arg4   __asm__("a3")  = (long)parm4;
  register long ret    __asm__("a0");

  __asm__ __volatile__(
      "syscall 0\n"
      : "=r"(ret)
      : "r"(sys_nr), "0"(arg1), "r"(arg2), "r"(arg3), "r"(arg4)
      : "memory"
    );

  return (uintptr_t)ret;
}

/****************************************************************************
 * Name: sys_call5
 *
 * Description:
 *   System call SYS_ argument and five additional parameters.
 *
 *   LoongArch64 Calling Convention:
 *   - $a7: syscall number
 *   - $a0,a1,a2,a3,a4: parameter 1,2,3,4,5
 *   - Return value in $a0
 ****************************************************************************/

static inline uintptr_t sys_call5(unsigned int nbr, uintptr_t parm1,
                                  uintptr_t parm2, uintptr_t parm3,
                                  uintptr_t parm4, uintptr_t parm5)
{
  register long sys_nr __asm__("a7") = nbr;
  register long arg1   __asm__("a0")  = (long)parm1;
  register long arg2   __asm__("a1")  = (long)parm2;
  register long arg3   __asm__("a2")  = (long)parm3;
  register long arg4   __asm__("a3")  = (long)parm4;
  register long arg5   __asm__("a4")  = (long)parm5;
  register long ret    __asm__("a0");

  __asm__ __volatile__(
      "syscall 0\n"
      : "=r"(ret)
      : "r"(sys_nr), "0"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5)
      : "memory"
    );

  return (uintptr_t)ret;
}

/****************************************************************************
 * Name: sys_call6
 *
 * Description:
 *   System call SYS_ argument and six additional parameters.
 *
 *   LoongArch64 Calling Convention:
 *   - $a7: syscall number
 *   - $a0,a1,a2,a3,a4,a5: parameter 1,2,3,4,5,6
 *   - Return value in $a0
 ****************************************************************************/

static inline uintptr_t sys_call6(unsigned int nbr, uintptr_t parm1,
                                  uintptr_t parm2, uintptr_t parm3,
                                  uintptr_t parm4, uintptr_t parm5,
                                  uintptr_t parm6)
{
  register long sys_nr __asm__("a7") = nbr;
  register long arg1   __asm__("a0")  = (long)parm1;
  register long arg2   __asm__("a1")  = (long)parm2;
  register long arg3   __asm__("a2")  = (long)parm3;
  register long arg4   __asm__("a3")  = (long)parm4;
  register long arg5   __asm__("a4")  = (long)parm5;
  register long arg6   __asm__("a5")  = (long)parm6;
  register long ret    __asm__("a0");

  __asm__ __volatile__ (
      "syscall 0\n"
      : "=r"(ret)
      : "r"(sys_nr), "0"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5),
        "r"(arg6)
      : "memory"
    );

  return (uintptr_t)ret;
}

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_LOONGARCH_INCLUDE_SYSCALL_H */
