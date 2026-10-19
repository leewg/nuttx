/****************************************************************************
 * arch/loongarch/src/common/la64_backtrace_fp.c
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

#include <assert.h>
#include <nuttx/debug.h>

#include <nuttx/config.h>
#include <stdbool.h>
#include <nuttx/arch.h>
#include <nuttx/addrenv.h>
#include <nuttx/compiler.h>
#include "sched/sched.h"
#include "la64_internal.h"

/*
 * $r1(r1): return address
 * $r3(sp): Stack Pointer, flow down(high addr --> low addr), 16-byte align
 * $r22(fp/s9): Frame Pointer, must enable -fno-omit-frame-pointer
 *
 *  High Address
 *  +-------------------+
 *  |  Previous $ra     |  <-- [fp + 8] (Caller's return addr)
 *  +-------------------+
 *  |  Previous $fp     |  <-- [fp + 0] (Caller's fp)
 *  +-------------------+  <-- current $fp ($r22) point here
 *  |  Local Variables  |
 *  |  & Callee-saved   |
 *  +-------------------+  <-- current $sp ($r3) point here
 *  Low Address
 *
 */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct stack_frame_s
{
  uintptr_t fp;
  uintptr_t ra;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

#define CONFIG_SCHED_BACKTRACE_MAX 16

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: backtrace_is_valid_stack
 * Description:
 ****************************************************************************/
static bool backtrace_is_valid_stack(uintptr_t sp, uintptr_t low, uintptr_t high)
{
  if ((sp & STACKFRAME_ALIGN_MASK) != 0)
    return false;

  return (sp > low && sp < high);
}

/****************************************************************************
 * Name: backtrace_get_stack_bound
 * Description:
 ****************************************************************************/
static void backtrace_get_stack_bound(struct tcb_s *tcb, uintptr_t sp, uintptr_t *low, uintptr_t *high)
{
#if CONFIG_ARCH_INTERRUPTSTACK > 15
  uintptr_t intstack_base = (uintptr_t)g_intstackalloc;
  uintptr_t intstack_top = (intstack_base + INT_STACK_SIZE) & STACKFRAME_ALIGN_MASK;

  if (sp >= intstack_base && sp < intstack_top)
  {
    *low = intstack_base;
    *high = intstack_top;
    return;
  }
#endif

  /* user task/kernel sp */
  if (tcb == NULL)
    tcb = running_task();

  if (tcb != NULL) {
    *low = (uintptr_t)tcb->stack_alloc_ptr;
    *high = *low + tcb->adj_stack_size;
  } else {
    *low = 0;
    *high = 0;
  }
}

/****************************************************************************
 * Name: getfp
 *
 * Description:
 *  getfp() returns current frame pointer
 *
 ****************************************************************************/
static always_inline_function uintptr_t getfp(void)
{
  register uintptr_t fp;

  __asm__ __volatile__
  (
   "move %0, $r22 \n\t"
    : "=r"(fp)
    :
    : "memory"
  );
  return fp;
}

/****************************************************************************
 * Name: backtrace
 *
 * Description:
 *  backtrace() parsing the return address through frame pointer
 *
 ****************************************************************************/

static int la64_backtrace_fp(uintptr_t *base, uintptr_t *limit,
    uintptr_t *fp, uintptr_t *pc,
    void **buffer, int size, int *skip)
{
  int i = 0;

  /* If caller provided a PC (current function), include it first (after skip). */
  if (pc)
  {
    if ((*skip)-- <= 0)
    {
      /* For LoongArch, saved return address is the calling PC.
       * Use pc adjusted to return address if necessary. Here we
       * emit pc as-is (matches other arch implementations trend).
       */
      buffer[i++] = (void *)pc;
    }
  }

  while (i < size)
  {
    /* Basic validation: fp within stack range, aligned, and non-zero */
    if (fp == NULL || fp < base || fp >= limit ||
        ((uintptr_t)fp & (sizeof(uintptr_t) - 1)) != 0)
    {
      break;
    }

    /* read previous frame pointer (first word at fp), if prev_fp == 0 or obviously invalid, stop */
    if (fp[0] == 0)
    {
      break;
    }

    /* Frame layout: fp[0] = previous fp, fp[1] = return address */
    if ((*skip)-- <= 0)
    {
      buffer[i++] = (void *)fp[1];
    }

    /* Move to previous frame */
    fp = (uintptr_t *)fp[0];
  }

  return i;
}

int la64_backtrace_reg(const uint64_t *regs, void **buf)
{
  struct stack_frame_s *frame;
  uintptr_t high=0, low=0, fp, era;
  int depth = 0;

  if (regs == NULL)
    return -1;

  era = (uintptr_t)regs[REG_ERA];
  fp = (uintptr_t)regs[REG_FP];

  _alert("backtrace_reg:\n");
  _alert("[%d] PC: 0x%016lx\n", depth++, era);

  backtrace_get_stack_bound(NULL, fp, &low, &high);
  frame = (struct stack_frame_s *)fp;

  while (frame != NULL && depth < CONFIG_SCHED_BACKTRACE_MAX)
  {
    if (!backtrace_is_valid_stack((uintptr_t)frame, low, high))
      break;

    if (frame->ra == 0) //ra
      break;

    buf[depth] = (void *)frame->ra;

    _alert("[%d] PC: 0x%016lx\n", depth++, frame->ra);
    uintptr_t next_fp = frame->fp;
    if (next_fp <= (uintptr_t)frame)
      break;

    frame = (struct stack_frame_s *)next_fp;
  }

  return depth;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_backtrace
 *
 * Description:
 *  up_backtrace()  returns  a backtrace for the TCB, in the array
 *  pointed to by buffer.  A backtrace is the series of currently active
 *  function calls for the program.  Each item in the array pointed to by
 *  buffer is of type void *, and is the return address from the
 *  corresponding stack frame.  The size argument specifies the maximum
 *  number of addresses that can be stored in buffer.   If  the backtrace is
 *  larger than size, then the addresses corresponding to the size most
 *  recent function calls are returned; to obtain the complete backtrace,
 *  make sure that buffer and size are large enough.
 *
 *  A task that is (or was, at the last trap) mid-syscall has its call
 *  chain split across two stacks: the kernel stack (from the trap that
 *  entered the syscall up to the ecall itself) and, continuing from
 *  there, the user stack (the application code that issued the ecall).
 *  Both segments are walked and spliced together into one buffer.
 *
 * Input Parameters:
 *   tcb    - Address of the task's TCB
 *   buffer - Return address from the corresponding stack frame
 *   size   - Maximum number of addresses that can be stored in buffer
 *   skip   - number of addresses to be skipped
 *
 * Returned Value:
 *   up_backtrace() returns the number of addresses returned in buffer
 *
 * Assumptions:
 *   Have to make sure tcb keep safe during function executing, it means
 *   1. Tcb have to be self or not-running.  In SMP case, the running task
 *      PC & SP cannot be backtrace, as whose get from tcb is not the newest.
 *   2. Tcb have to keep not be freed.  In task exiting case, have to
 *      make sure the tcb get from pid and up_backtrace in one critical
 *      section procedure.
 *
 ****************************************************************************/

int up_backtrace(struct tcb_s *tcb, void **buffer, int size, int skip)
{
  struct tcb_s *rtcb = running_task();
  int ret = 0;

  if (size <= 0 || buffer == NULL)
  {
    return 0;
  }

  /* If no tcb provided assume current task */
  if (tcb == NULL)
  {
    tcb = rtcb;
  }

  /* If collecting for the current (running) task */
  if (tcb == rtcb)
  {
    /* If in interrupt context, walk the interrupt stack */
    if (up_interrupt_context())
    {
#if CONFIG_ARCH_INTERRUPTSTACK > 7
      void *istackbase = (void *)up_get_intstackbase(this_cpu());
      /* Use current frame pointer and no PC provided (pc == NULL) */
      ret = la64_backtrace_fp(istackbase,
                           istackbase + INT_STACK_SIZE,
                           (void *)__builtin_frame_address(0),
                           NULL, buffer, size, &skip);
#else
      ret = la64_backtrace_fp(rtcb->stack_base_ptr,
                           rtcb->stack_base_ptr + rtcb->adj_stack_size,
                           (void *)__builtin_frame_address(0),
                           NULL, buffer, size, &skip);
#endif  /* CONFIG_ARCH_INTERRUPTSTACK > 7 */
      if (ret < size)
      {
        ret = la64_backtrace_fp(rtcb->stack_base_ptr,
                             rtcb->stack_base_ptr + rtcb->adj_stack_size,
                             running_regs()[REG_FP],
                             running_regs()[REG_RA],
                             &buffer[ret], size-ret, &skip);
      }
    }
    else
    {
      /* Use compiler frame address as starting fp, and include current PC */
      ret = la64_backtrace_fp(rtcb->stack_base_ptr,
                           rtcb->stack_base_ptr + rtcb->adj_stack_size,
                           (void *)__builtin_frame_address(0),
                           (void *)__builtin_return_address(0),
                           buffer, size, &skip);
    }
  }
  else
  {
    /* Backtrace for another task: use saved register context in tcb->xcp.regs.
     * We expect JB_FP and JB_PC indices to be available and to contain the
     * saved frame pointer and saved return address / pc respectively.
     *
     * Follow same validation as above.
     */
    //uintptr_t saved_fp = (uintptr_t)tcb->xcp.regs[REG_FP];
    //uintptr_t saved_pc = (uintptr_t)tcb->xcp.regs[REG_RA];

    ret = la64_backtrace_fp(tcb->stack_base_ptr,
                         tcb->stack_base_ptr + tcb->adj_stack_size,
                         (void *)tcb->xcp.regs[REG_FP],
                         (void *)tcb->xcp.regs[REG_RA],
                         buffer, size, &skip);
  }

  return ret;
}
