/****************************************************************************
 * arch/loongarch/src/ls2k0300/ls2k0300_start.c
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

#include <nuttx/init.h>
#include <nuttx/arch.h>
#include <arch/board/board.h>

#include <stdint.h>
#include <inttypes.h>
#include <debug.h>
#include <assert.h>
#include <nuttx/arch.h>
#include <arch/irq.h>

#include "la64_internal.h"
#include "chip.h"
#include "la64_percpu.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef CONFIG_DEBUG_FEATURES
#  define showprogress(c) la64_lowputc(c)
#else
#  define showprogress(c)
#endif

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * External Symbols from Linker Script and Boot Assembly
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: loongarch_mem_verify
 *
 * Description:
 *   Print LoongArch memory map
 *
 ****************************************************************************/

void loongarch_mem_verify(void)
{
  uintptr_t stext_addr = (uintptr_t)&_stext;
  uintptr_t ebss_addr  = (uintptr_t)&_ebss;
  uintptr_t idle_top   = (uintptr_t)g_idle_topstack;
  uintptr_t ram_end    = (uintptr_t)CONFIG_RAM_END;

  size_t kernel_size = ebss_addr - stext_addr;
  size_t idle_stack_size = idle_top - ebss_addr;
  size_t heap_size = ram_end - idle_top;

  syslog(LOG_INFO, "================ LoongArch Memory Layout ================\n");
  syslog(LOG_INFO, "  RAM Start       : 0x%016" PRIxPTR "\n", (uintptr_t)CONFIG_RAM_START);
  syslog(LOG_INFO, "  Text Section    : 0x%016" PRIxPTR " - 0x%016" PRIxPTR "\n",
         stext_addr, (uintptr_t)&_etext);
  syslog(LOG_INFO, "  Data Section    : 0x%016" PRIxPTR " - 0x%016" PRIxPTR "\n",
         (uintptr_t)&_sdata, (uintptr_t)&_edata);
  syslog(LOG_INFO, "  BSS Section     : 0x%016" PRIxPTR " - 0x%016" PRIxPTR "\n",
         (uintptr_t)&_sbss, ebss_addr);
  syslog(LOG_INFO, "  Kernel Total    : %zu KB\n", kernel_size / 1024);
  syslog(LOG_INFO, "  -----------------------------------------------------\n");
  syslog(LOG_INFO, "  IDLE Stack Base : 0x%016" PRIxPTR "\n", ebss_addr);
  syslog(LOG_INFO, "  g_idle_topstack : 0x%016" PRIxPTR " (Size: %zu bytes)\n",
         idle_top, idle_stack_size);
  syslog(LOG_INFO, "  -----------------------------------------------------\n");
  syslog(LOG_INFO, "  System Heap Base: 0x%016" PRIxPTR "\n", idle_top);
  syslog(LOG_INFO, "  RAM End         : 0x%016" PRIxPTR "\n", ram_end);
  syslog(LOG_INFO, "  Heap Total Size : %zu MB (%zu bytes)\n",
         heap_size / (1024 * 1024), heap_size);
  syslog(LOG_INFO, "=========================================================\n");

  DEBUGASSERT((ebss_addr % 16) == 0);

  DEBUGASSERT((idle_top % 16) == 0);

  DEBUGASSERT(stext_addr < ebss_addr);
  DEBUGASSERT(ebss_addr < idle_top);
  DEBUGASSERT(idle_top < ram_end);

  DEBUGASSERT(idle_stack_size == CONFIG_IDLETHREAD_STACKSIZE);
}

/****************************************************************************
 * Name: __ls2k0300_start
 ****************************************************************************/

void __ls2k0300_start(void)
{
  trap_init();

  up_percpu_initialize(0);
  la64_percpu_init(0);

  loongarch_mem_verify();

  syslog(LOG_INFO, "  LOONGARCH_CSR_CRMD: 0x%016" PRIxPTR "\n",
      csr_read64(LOONGARCH_CSR_CRMD));
  syslog(LOG_INFO, "  LOONGARCH_CSR_PRMD: 0x%016" PRIxPTR "\n",
      csr_read64(LOONGARCH_CSR_PRMD));
  syslog(LOG_INFO, "  LOONGARCH_CSR_ECFG: 0x%016" PRIxPTR "\n",
      csr_read64(LOONGARCH_CSR_ECFG));
  syslog(LOG_INFO, "  LOONGARCH_CSR_ESTAT: 0x%016" PRIxPTR "\n",
      csr_read64(LOONGARCH_CSR_ESTAT));
  syslog(LOG_INFO, "  LOONGARCH_CSR_TID: 0x%016" PRIxPTR "\n",
      csr_read64(LOONGARCH_CSR_TMID));

  syslog(LOG_INFO, "  LOONGARCH_CSR_EENTRY: 0x%016" PRIxPTR "\n",
      csr_read64(LOONGARCH_CSR_EENTRY));
  syslog(LOG_INFO, "  LOONGARCH_CSR_TLBRENTRY: 0x%016" PRIxPTR "\n",
      csr_read64(LOONGARCH_CSR_TLBRENTRY));
  syslog(LOG_INFO, "  LOONGARCH_CSR_MERRENTRY: 0x%016" PRIxPTR "\n",
      csr_read64(LOONGARCH_CSR_MERRENTRY));

  /* Call nx_start() */
  nx_start();
}
