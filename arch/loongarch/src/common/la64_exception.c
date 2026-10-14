/****************************************************************************
 * arch/loongarch64/src/common/la64_exception.c
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

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <nuttx/debug.h>
#include <syscall.h>

#include <arch/irq.h>
#include <nuttx/arch.h>
#include <nuttx/sched.h>
#include <arch/csr.h>
#include <arch/barriers.h>
#include <arch/ls2k0300/chip.h>

#include "sched/sched.h"
#include "signal/signal.h"
#include "la64_internal.h"
#include "la64_percpu.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

extern void handle_ade(void);
extern void handle_ale(void);
extern void handle_sys(void);
extern void handle_bp(void);
extern void handle_ri(void);
extern void handle_fpu(void);
extern void handle_fpe(void);
extern void handle_reserved(void);
extern void handle_watch(void);
extern void handle_vint(void);
extern void handle_tlb_refill(void);
extern char except_vec_cex;
extern dispatch_syscall(uint64_t arg0, uint64_t arg1, uint64_t arg2,
    uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t nbr, uint64_t *cxt);

void *g_exception_table[EXCCODE_INT_START] =
{
  [0 ... EXCCODE_INT_START - 1] = handle_reserved,

  [EXCCODE_ADE]  = handle_ade,
  [EXCCODE_ALE]  = handle_ale,
  [EXCCODE_SYS]  = handle_sys,
  [EXCCODE_BP]   = handle_bp,
  [EXCCODE_INE]  = handle_ri,
  [EXCCODE_FPDIS]= handle_fpu,
  [EXCCODE_FPE]  = handle_fpe,
  [EXCCODE_WATCH]= handle_watch,
};

#define ECFG_VS_128B        (7 << CSR_ECFG_VS_SHIFT)
#define VECSIZE             0x200
#define SZ_4K               0x1000
#define SZ_16K              0x4000
#define SZ_64K              0x10000

unsigned long eentry;
unsigned long tlbrentry;
long exception_handlers[VECSIZE * 128 / sizeof(long)] aligned_data(SZ_64K);

static void configure_exception_vector(void)
{
  eentry    = (unsigned long)exception_handlers;
  tlbrentry = (unsigned long)exception_handlers + 80*VECSIZE;

  csr_write64(eentry, LA_CSR_EENTRY);
  csr_write64(eentry, LA_CSR_MERRENTRY);
  csr_write64(tlbrentry, LA_CSR_TLBRENTRY);
}

void set_handler(unsigned long offset, void *addr, unsigned long size)
{
  memcpy((void *)(eentry + offset), addr, size);
  UP_DSB();
  UP_ISB();
}

void set_merr_handler(unsigned long offset, void *addr, unsigned long size)
{
  unsigned long uncac_eentry = TO_UNCAC(eentry);
  memcpy((void *)(uncac_eentry + offset), addr, size);

  UP_DSB();
  UP_ISB();
}

void dispatch_ext_irq_default(void)
{
  _err("ext irq! no one pay attention to\n");
}

void dispatch_hwi_irq_default(void)
{
  _err("hwi4~7 irq! no one pay attention to\n");
}

void dispatch_swi_irq_default(void)
{
}

void la64_exception_attach(void)
{
  uint32_t i;

  //irq_attach(LA_IRQ_SWI0, dispatch_syscall, NULL);
  //irq_attach(LA_IRQ_SWI1, la64_swint, NULL);

  irq_attach(LA_IRQ_SWI0, dispatch_swi_irq_default, NULL);
  irq_attach(LA_IRQ_HWI4, dispatch_hwi_irq_default, NULL);
  irq_attach(LA_IRQ_HWI5, dispatch_hwi_irq_default, NULL);
  irq_attach(LA_IRQ_HWI6, dispatch_hwi_irq_default, NULL);
  irq_attach(LA_IRQ_HWI7, dispatch_hwi_irq_default, NULL);

  for (i = LA_EXT_IRQ_BASE; i < LA_EXT_IRQ_BASE + 128; i++)
    irq_attach(i, dispatch_ext_irq_default, NULL);
}

/****************************************************************************
 * Name: trap_init
 *
 * Description:
 *
 ****************************************************************************/

void trap_init(void)
{
  uint32_t ecfg, i;

  ecfg = csr_read32(LA_CSR_ECFG);
  ecfg &= ~CSR_ECFG_VS;
  ecfg |= ECFG_VS_128B;
  csr_write32(ecfg, LA_CSR_ECFG);

  configure_exception_vector();

  for (i = 0; i < 64; i++)
    set_handler(i*VECSIZE, handle_reserved, VECSIZE);

  set_handler(80*VECSIZE, handle_tlb_refill, VECSIZE);

  for (i = EXCCODE_TLBL; i <= EXCCODE_TLBPE; i++) {
    set_handler(i * VECSIZE, g_exception_table[i], VECSIZE);
  }

  for (i = EXCCODE_ADE; i < EXCCODE_BTDIS; i++)
    set_handler(i * VECSIZE, g_exception_table[i], VECSIZE);

  for (i = EXCCODE_INT_START; i < EXCCODE_INT_END; i++)
    set_handler(i * VECSIZE, handle_vint, VECSIZE);

  set_merr_handler(0x0, &except_vec_cex, VECSIZE);
}

/****************************************************************************
 * Name: la64_regs_dump
 *
 * Description:
 *
 ****************************************************************************/

static void la64_regs_dump(uint64_t *regs)
{
  int i;

  _alert("========== NuttX Core Register Dump ==========\n");
  for (i = 0; i < 32; i+= 4)
    _alert("R%d-R%d: 0x%016lx 0x%016lx 0x%016lx 0x%016lx\n",
        i, i+3, regs[i], regs[i+1], regs[i+2], regs[i+3]);

  _alert("ERA  : 0x%016lx\n", regs[REG_ERA]);
  _alert("CRMD : 0x%016lx\n", regs[REG_CRMD]);
  _alert("PRMD : 0x%016lx\n", regs[REG_PRMD]);
  _alert("ESTAT: 0x%016lx\n", regs[REG_ESTAT]);
  _alert("BADV : 0x%016lx\n", regs[REG_BADV]);
}

void do_ade(uint64_t *regs, bool user)
{
  _err("ADE\n");

  la64_regs_dump(regs);

  PANIC();
}

void do_ale(uint64_t *regs, bool user)
{
  _err("ALE\n");
  la64_regs_dump(regs);
  PANIC();
}

void do_bp(uint64_t *regs, bool user)
{
  _err("BP: Breakpoint at ERA=%p\n", regs[REG_ERA]);
  regs[REG_ERA] += 4;
}

void do_ri(uint64_t *regs, bool user)
{
  _err("INE: Illegal instruction at ERA=%p\n", regs[REG_ERA]);

  if (!user)
    {
      PANIC();
    }
  else
    {
      //nxsched_abort_task(this_task());
    PANIC();
    }
}

void do_reserved(uint64_t *regs)
{
  _err("Reserved exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}

void cache_parity_error(void)
{
  uint64_t merrera, merrctl;

  __asm__ __volatile__("csrrd %0, 0x30" : "=r"(merrera));
  __asm__ __volatile__("csrrd %0, 0x31" : "=r"(merrctl));

  _err("Cache error: MERRERA=%016llx MERRCTL=%016llx\n", merrera, merrctl);
  PANIC();
}

void do_fpe(uint64_t *regs)
{
  _err("FPE exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}

void do_fpu(uint64_t *regs)
{
  _err("FPU exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}
void do_lsx(uint64_t *regs)
{
  _err("LSX exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}
void do_lasx(uint64_t *regs)
{
  _err("LASX exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}
void do_lbt(uint64_t *regs)
{
  _err("LBT exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}
void do_watch(uint64_t *regs)
{
  _err("WATCH exception: ERA=%p\n", regs[REG_ERA]);
  la64_regs_dump(regs);
  PANIC();
}

void *hwi_dispatch(uint32_t ip, void *regs)
{
  uint32_t map_off, route_ip;
  uint32_t en, sr, idx, irq;

  route_ip = ip >> 2;

  for (map_off = 0; map_off < 4; map_off++) {
    if (getreg8(INTC_BASE + LS2K0300_EXTIOI_MAP + map_off) != route_ip)
      continue;

    en = getreg32(INTC_BASE + LS2K0300_EXTIOI_IEN0 + map_off * 4);
    sr = getreg32(INTC_BASE + LS2K0300_EXTIOI_CORE_ISR0 + map_off * 4);

    //syslog(LOG_EMERG, "hwi: map_off=%d,route_ip=%d, en=0x%08x, sr=0x%08x\n", map_off, route_ip, en, sr);

    sr &= en;
    idx = 0;

    while (sr != 0) {
      idx = __builtin_ctz(sr);
      putreg32(1U << idx, INTC_BASE + LS2K0300_EXTIOI_ISR0 + map_off * 4);
      irq = LA_EXT_IRQ_BASE + map_off * 32 + idx;

      regs = la64_doirq(irq, regs);

      sr &= ~(1U << idx);
    }
  }
  return regs;
}

/****************************************************************************
 * Name: do_vint
 *
 * Description: IS[1~13]
 *
 ****************************************************************************/
uint64_t do_vint(uint64_t *regs)
{
  struct la64_percpu_s *percpu = la64_my_percpu();
  percpu->cur_regs = (uintptr_t)regs;

  uint32_t estat = csr_read32(LA_CSR_ESTAT);
  uint32_t irq = (estat & 0x3fff);

  //syslog(LOG_EMERG, "VINT! ESTAT = 0x%x, irq = 0x%x, regs = 0x%p\n", estat, irq, regs);
  if ((estat & CSR_ESTAT_IS) == 0) {
    percpu->cur_regs = 0;

    return regs;
  }

  if (irq & ECFGF_TIMER) {  // TI entry
    irq = LA_LOC_IRQ_BASE + INT_TI; /* TI */
    regs = la64_doirq(irq, regs);
  }

  if (irq & ECFGF_IPI) {
    irq = LA_LOC_IRQ_BASE + INT_IPI; /* TI */
  }

  if (irq & 0x3fc) { // hwi(HWI0 ~ HWI7) entry
    if (irq & ECFGF_IP0) {
      regs = hwi_dispatch(ECFGF_IP0, regs);
    }
    if (irq & ECFGF_IP1) {
      regs = hwi_dispatch(ECFGF_IP1, regs);
    }
    if (irq & ECFGF_IP2) {
      regs = hwi_dispatch(ECFGF_IP2, regs);
    }
    if (irq & ECFGF_IP3) {
      regs = hwi_dispatch(ECFGF_IP3, regs);
    }
    if (irq & (ECFGF_IP4 | ECFGF_IP5 | ECFGF_IP6 | ECFGF_IP7)) { // HWI7~HWI7 unused in ext mode
      int hwi = __builtin_ctz(irq & 0x3c0);
      irq = LA_LOC_IRQ_BASE + hwi;

      regs = la64_doirq(irq, regs);
    }
  }

  if (irq & ECFGF_SIP0) { // swi entry
    int swi = __builtin_ctz(irq & 0x3);

    csr_xchg32(0x0, 0x1 << swi, LA_CSR_ESTAT);
    irq = LA_LOC_IRQ_BASE + swi;

    //_err("SWI%d RA=0x%lx, SP=0x%lx\n", swi, regs[REG_RA], regs[REG_SP]);

    regs = la64_doirq(irq, regs);
  }

  percpu->cur_regs = 0;
  //syslog(LOG_EMERG, "VINT_END! ESTAT = 0x%x, regs = 0x%x\n", csr_read32(LA_CSR_ESTAT), regs);
  return regs;
}

#ifdef CONFIG_ARCH_FPU
/****************************************************************************
 * Name: la64_fpu_lazy_load
 *
 * Description:
 *
 ****************************************************************************/
void la64_fpu_disable_dispatch(uint64_t *regs)
{
  sderr("========== NuttX Core Register Dump ==========\n");
#if 0
    int cpu = up_cpu_index();
    struct tcb_s *curr_tcb  = (struct tcb_s *)g_percpu_ctx[cpu].current_tcb;
    struct tcb_s *owner_tcb = (struct tcb_s *)g_percpu_ctx[cpu].current_fpu_owner;
    uint32_t euen;

    /* The task that triggers the FPU exception must be the current tcb */
    DEBUGASSERT(curr_tcb != NULL);

    euen = read_csr_euen();
    if (!(euen & 1))
        write_csr_euen(euen | 1);

    if (owner_tcb != curr_tcb)
    {
        /* Physical FPU is being consumed by other tasks (the previous task) */
        if (owner_tcb != NULL)
        {
            /* FPU of previous task is tripped and writen to its tcb backup area */
            la64_fpu_save_to_tcb(&owner_tcb->xcp.fpu);

            /* To re-trigger the lazy flow next run, clear the FPU flag of previous task */
            if (owner_tcb->xcp.regs != NULL)
            {
                owner_tcb->xcp.regs[REG_EUEN] &= ~1;
            }
        }

        la64_fpu_restore_from_tcb(&curr_tcb->xcp.fpu);

        g_percpu_ctx[cpu].current_fpu_owner = (uint64_t)curr_tcb;
    }

    /* The EUEN value of stack is forcibly set to 1. When the assembly layer subsequently executes
     * restore_all and ertn, the hardware allows the FPU to run.
     */
    regs[REG_EUEN] |= 1;
#endif
}
#endif

#if 1
/****************************************************************************
 * Name: la64_fatal_handler
 *
 * Description:
 *
 ****************************************************************************/

void la64_fatal_handler(uint64_t *regs)
{
  FAR struct la64_percpu_s *percpu = la64_my_percpu();

  uint64_t estat = regs[REG_ESTAT];
  uint64_t era   = regs[REG_ERA];
  uint64_t badv  = csr_read64(LA_CSR_BADV);
  uint64_t ecode = (estat & CSR_ESTAT_EXC) >> CSR_ESTAT_EXC_SHIFT;

  percpu->cur_regs = (uintptr_t)regs;

  syslog(LOG_EMERG, "\n==================================================\n");
  syslog(LOG_EMERG, "FATAL HARDWARE EXCEPTION: Machine Check / Bus Error\n");
  syslog(LOG_EMERG, "==================================================\n");
#if CONFIG_SMP_NCPUS > 1
  syslog(LOG_EMERG, "  CPU:    %" PRIu32 "\n", percpu->cpu);
#endif
  syslog(LOG_EMERG, "  ESTAT:  0x%016" PRIx64 " [Ecode: 0x%02" PRIx32 "]\n", estat, ecode);
  syslog(LOG_EMERG, "  ERA:    0x%016" PRIx64 " (Fault PC)\n", era);
  syslog(LOG_EMERG, "  BADV:   0x%016" PRIx64 " (Fault Address)\n", badv);
  syslog(LOG_EMERG, "--------------------------------------------------\n");

  up_dump_register(regs);

  PANIC_WITH_REGS("Fatal Machine Check / Bus Error!", regs);

  for (;;);
}
#endif
