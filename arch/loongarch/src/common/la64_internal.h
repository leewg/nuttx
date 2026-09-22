/****************************************************************************
 * arch/loongarch/src/common/la64_internal.h
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

#ifndef __ARCH_LOONGARCH_SRC_COMMON_LA64_INTERNAL_H
#define __ARCH_LOONGARCH_SRC_COMMON_LA64_INTERNAL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <nuttx/compiler.h>
#  include <nuttx/sched.h>
#  include <sys/types.h>
#  include <stdint.h>
#  include <syscall.h>
#endif

#include <nuttx/irq.h>
#include <arch/barriers.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* This is the value used to mark the stack for subsequent stack monitoring
 * logic.
 */

#define STACK_COLOR    0xdeadbeef
#define INTSTACK_COLOR 0xdeadbeef
#define HEAP_COLOR     'h'

#define STACK_FRAME_SIZE __XSTR(STACKFRAME_ALIGN)

#ifdef CONFIG_ARCH_INTERRUPTSTACK
/* Interrupt Stack macros */
#define INT_STACK_SIZE  (STACKFRAME_ALIGN_DOWN(CONFIG_ARCH_INTERRUPTSTACK))
#else
#define INT_STACK_SIZE   0
#endif

/* Determine which (if any) console driver to use.  If a console is enabled
 * and no other console device is specified, then a serial console is
 * assumed.
 */

#ifndef CONFIG_DEV_CONSOLE
#  undef  USE_SERIALDRIVER
#  undef  USE_EARLYSERIALINIT
#else
#  if defined(CONFIG_CONSOLE_SYSLOG)
#    undef  USE_SERIALDRIVER
#    undef  USE_EARLYSERIALINIT
#  else
#    define USE_SERIALDRIVER 1
#    define USE_EARLYSERIALINIT 1
#  endif
#endif

#ifndef __ASSEMBLY__
static inline uint8_t getreg8(const volatile uintreg_t addr)
{
    uint8_t val = *(volatile uint8_t *)TO_UNCAC(addr);
    return val;
}

static inline void putreg8(uint8_t val, const volatile uintreg_t addr)
{
    *(volatile uint8_t *)TO_UNCAC(addr) = val;
}

static inline uint16_t getreg16(const volatile uintreg_t addr)
{
    uint16_t val = *(volatile uint16_t *)TO_UNCAC(addr);
    return val;
}

static inline void putreg16(uint16_t val, const volatile uintreg_t addr)
{
    *(volatile uint16_t *)TO_UNCAC(addr) = val;
}

static inline uint32_t getreg32(const volatile uintreg_t addr)
{
    uint32_t val = *(volatile uint32_t *)TO_UNCAC(addr);
    return val;
}

static inline void putreg32(uint32_t val, const volatile uintreg_t addr)
{
    *(volatile uint32_t *)TO_UNCAC(addr) = val;
}

static inline uint64_t getreg64(const volatile uintreg_t addr)
{
    uint64_t val = *(volatile uint64_t *)TO_UNCAC(addr);
    return val;
}

static inline void putreg64(uint64_t val, const volatile uintreg_t addr)
{
    *(volatile uint64_t *)TO_UNCAC(addr) = val;
}

/* Linker defined symbols */
extern uint8_t _end;

#endif // __ASSEMBLY


#define _START_TEXT  _stext
#define _END_TEXT    _etext
#define _START_BSS   _sbss
#define _END_BSS     _ebss
#define _DATA_INIT   _eronly
#define _START_DATA  _sdata
#define _END_DATA    _edata
#define _START_TDATA _stdata
#define _END_TDATA   _etdata
#define _START_TBSS  _stbss
#define _END_TBSS    _etbss

#define SMP_STACK_MASK (15)
#define SMP_STACK_SIZE \
   ((CONFIG_IDLETHREAD_STACKSIZE + SMP_STACK_MASK) & ~SMP_STACK_MASK)

/* thread default CRMD/PRMD configuration */
#define DEFAULT_THREAD_CRMD 0xb4
#define DEFAULT_THREAD_PRMD 0x04

/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifndef __ASSEMBLY__
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Data
 ****************************************************************************/

#if 0
#define INIT_STACK_DEFINE(sym, size)  \
  char locate_data(".initstack") \
  aligned_data(STACK_ALIGNMENT) sym[size]

#define INIT_STACK_ARRAY_DEFINE(sym, n, size)  \
  char locate_data(".initstack") \
  aligned_data(STACK_ALIGNMENT) sym[n][size]

#define INIT_STACK_DEFINE_EXTERN(sym, size)  \
  EXTERN char sym[size]
#define INIT_STACK_ARRAY_DEFINE_EXTERN(sym, n, size)  \
  EXTERN char sym[n][size]
#endif

EXTERN uintptr_t g_idle_topstack;

#define g_cpux_idlestack(cpuid) \
  (g_idle_topstack - SMP_STACK_SIZE * (CONFIG_SMP_NCPUS - (cpuid)))

#if CONFIG_ARCH_INTERRUPTSTACK > 15
EXTERN uint8_t g_intstackalloc[]; /* Allocated stack base */
EXTERN uint8_t g_intstacktop[];   /* Initial top of interrupt stack */
#else
# error CONFIG_ARCH_INTERRUPTSTACK must be defined (4096 at least) at loongarch
#endif

/* These symbols are setup by the linker script. */
EXTERN uint8_t _stext[];           /* Start of .text */
EXTERN uint8_t _etext[];           /* End_1 of .text + .rodata */
EXTERN const uint8_t _eronly[];    /* End+1 of read only section (.text + .rodata) */
EXTERN uint8_t _sdata[];           /* Start of .data */
EXTERN uint8_t _edata[];           /* End+1 of .data */
EXTERN uint8_t _sbss[];            /* Start of .bss */
EXTERN uint8_t _ebss[];            /* End+1 of .bss */
EXTERN uint8_t _stdata[];          /* Start of .tdata */
EXTERN uint8_t _etdata[];          /* End+1 of .tdata */
EXTERN uint8_t _stbss[];           /* Start of .tbss */
EXTERN uint8_t _etbss[];           /* End+1 of .tbss */

#endif


/****************************************************************************
* Public Function Prototypes
  ***************************************************************************/

#ifndef __ASSEMBLY__

/* Memory allocation ********************************************************/
#if CONFIG_MM_REGIONS > 1
void la64_addregion(void);
#else
#  define la64_addregion()
#endif

/* Power management *********************************************************/
#ifdef CONFIG_PM
void la64_pminitialize(void);
#else
#  define la64_pminitialize()
#endif

/* DMA **********************************************************************/
#ifdef CONFIG_ARCH_DMA
void weak_function la64_dma_initialize(void);
#else
#  define la64_dma_initialize()
#endif

/* Low level serial output **************************************************/

void la64_lowputc(char ch);
void la64_lowputs(const char *str);

#ifdef USE_SERIALDRIVER
void la64_serialinit(void);
#endif

#ifdef USE_EARLYSERIALINIT
void la64_earlyserialinit(void);
#endif

/* Exception Handler ********************************************************/


/* Debug ********************************************************************/

#ifdef CONFIG_STACK_COLORATION
size_t la64_stack_check(uintptr_t alloc, size_t size);
void la64_stack_color(void *stackbase, size_t nbytes);
#endif

#if defined(CONFIG_STACK_COLORATION) && \
    defined(CONFIG_ARCH_INTERRUPTSTACK) && CONFIG_ARCH_INTERRUPTSTACK > 15
void la64_color_intstack(void);
#else
#  define la64_color_intstack()
#endif

void la64_sigdeliver(void);
int la64_swint(int irq, void *context, void *arg);


/****************************************************************************
 * Name: la64_jump_to_user
 *
 * Description:
 *   Routine to jump to user space, called when a user process is started and
 *   the kernel is ready to give control to the user task in user space.
 *
 * Parameters:
 *   entry - Process entry point.
 *   a0    - Parameter 0 for the process.
 *   a1    - Parameter 1 for the process.
 *   a2    - Parameter 2 for the process.
 *   sp    - User stack pointer.
 *   regs  - Integer register save area to use.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void la64_jump_to_user(uintptr_t entry, uintreg_t a0, uintreg_t a1,
                        uintreg_t a2, uintreg_t sp,
                        uintreg_t *regs) noreturn_function;

/* Context switching via system calls ***************************************/

/****************************************************************************
 * Name: la64_fullcontextrestore
 *
 * Description:
 *   Restores the full context.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#define la64_fullcontextrestore()    \
  do                                  \
    {                                 \
      sys_call0(SYS_restore_context); \
    }                                 \
  while (1)

/****************************************************************************
 * Name: la64_switchcontext
 *
 * Description:
 *   Switches the context.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#define la64_switchcontext()        \
  do                                 \
    {                                \
      sys_call0(SYS_switch_context); \
    }                                \
  while (0)

#undef EXTERN
#ifdef __cplusplus
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_LOONGARCH_SRC_COMMON_LA64_INTERNAL_H */
