/****************************************************************************
 * arch/loongarch/src/common/la64_arch_timer.c
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

#include <stdint.h>
#include <nuttx/debug.h>
#include <assert.h>
#include <stdio.h>

#include <nuttx/timers/oneshot.h>
#include <nuttx/arch.h>
#include <nuttx/irq.h>

#include <arch/ls2k0300/hpet.h>
#include <arch/ls2k0300/chip.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* This structure provides the private representation of the "lower-half"
 * driver state structure.  This structure must be cast-compatible with the
 * oneshot_lowerhalf_s structure.
 */

struct la64_oneshot_lowerhalf_s
{
  struct oneshot_lowerhalf_s lower;
  volatile void              *tbase;
  int                        irq;
  bool                       running;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static clkcnt_t la64_oneshot_max_delay(struct oneshot_lowerhalf_s *lower);
static clkcnt_t la64_oneshot_current(struct oneshot_lowerhalf_s *lower);
static void la64_oneshot_start_absolute(struct oneshot_lowerhalf_s *lower,
                                       clkcnt_t expected);
static void la64_oneshot_start(struct oneshot_lowerhalf_s *lower,
                              clkcnt_t delta);
static void la64_oneshot_cancel(struct oneshot_lowerhalf_s *lower);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct oneshot_operations_s g_la64_oneshot_ops =
{
  .current        = la64_oneshot_current,
  .start          = la64_oneshot_start,
  .start_absolute = la64_oneshot_start_absolute,
  .cancel         = la64_oneshot_cancel,
  .max_delay      = la64_oneshot_max_delay
};

static struct la64_oneshot_lowerhalf_s g_la64_oneshot_lowerhalf =
{
  .lower.ops = &g_la64_oneshot_ops,
  .running = false,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static clkcnt_t la64_oneshot_max_delay(struct oneshot_lowerhalf_s *lower)
{
  uint64_t cfg = hpet_get_tn_cfg(0);

  if ((cfg & TN_32MODE_CNF) == 0)
  {
    return UINT32_MAX;
  } else {
    return UINT64_MAX;
  }
}

static clkcnt_t la64_oneshot_current(struct oneshot_lowerhalf_s *lower)
{
  return hpet_get_mcounter();
}

static void la64_oneshot_start(struct oneshot_lowerhalf_s *lower,
                              clkcnt_t delta)
{
  struct la64_oneshot_lowerhalf_s *priv = &g_la64_oneshot_lowerhalf;
  irqstate_t flags = up_irq_save();
  uint64_t curr, target;

  curr  = hpet_get_mcounter();
  target = curr + delta;

  priv->running = true;

  hpet_set_tn_cmp(0, target);
  hpet_set_tn_cfg(0, hpet_get_tn_cfg(0) | TN_INT_ENB_CNF);

  up_irq_restore(flags);
}

static void la64_oneshot_start_absolute(struct oneshot_lowerhalf_s *lower,
                                       clkcnt_t expected)
{
  struct la64_oneshot_lowerhalf_s *priv = &g_la64_oneshot_lowerhalf;

  priv->running = true;

  hpet_set_tn_cmp(0, expected);
  hpet_set_tn_cfg(0, hpet_get_tn_cfg(0) | TN_INT_ENB_CNF);
}

static void la64_oneshot_cancel(struct oneshot_lowerhalf_s *lower)
{
  struct la64_oneshot_lowerhalf_s *priv = &g_la64_oneshot_lowerhalf;
  uint64_t cfg;
  irqstate_t flags = up_irq_save();

  cfg = hpet_get_tn_cfg(0);
  if ((cfg & TN_32MODE_CNF) == 0)
  {
    cfg &= ~TN_INT_ENB_CNF;
    hpet_set_tn_cfg(0, cfg);
    hpet_clr_isr(0);
  } else {
    hpet_set_tn_cmp(0, UINT64_MAX);
  }
  priv->running = false;

  up_irq_restore(flags);
}

static int la64_oneshot_interrupt(int irq, void *context, void *arg)
{
  struct la64_oneshot_lowerhalf_s *priv = &g_la64_oneshot_lowerhalf;

  up_putc('T');
  up_putc('\r');
  up_putc('\n');

  hpet_set_tn_cfg(0, hpet_get_tn_cfg(0)&(~TN_INT_ENB_CNF));
  priv->running = false;

  oneshot_process_callback(&priv->lower);

  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: la64_mtimer_oneshot_initialize_per_cpu
 *
 * Description:
 *   Initialize the la main timer for secondary CPUs.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

static void la64_mtimer_oneshot_initialize_per_cpu(void)
{
  struct la64_oneshot_lowerhalf_s *priv = &g_la64_oneshot_lowerhalf;
  uint64_t tn_config = hpet_get_tn_cfg(0);

  if ((tn_config & TN_32MODE_CNF) == 0)
  {
    hpet_set_tn_cmp(0, UINT32_MAX);
  } else {
    hpet_set_tn_cmp(0, UINT64_MAX);
  }
  tn_config |= TN_INT_TYPE_CNF;
  tn_config &= ~TN_TYPE_CNF;
  tn_config |= TN_INT_ENB_CNF;

  hpet_set_tn_cfg(0, tn_config);
  hpet_set_tn_cmp(0, 0x10000000);

  up_enable_irq(priv->irq);
  hpet_set_cfg(ENABLE_CNF);

  syslog(LOG_INFO, " GEN_CAP  :0x%016" PRIxPTR "\n", hpet_get_cap());
  syslog(LOG_INFO, " GEN_CFG  :0x%016" PRIxPTR "\n", hpet_get_cfg());
  syslog(LOG_INFO, " GEN_ISR  :0x%016" PRIxPTR "\n", hpet_get_isr());
  syslog(LOG_INFO, " MAIN_CNT :0x%016" PRIxPTR "\n", hpet_get_mcounter());
  syslog(LOG_INFO, " T0_CFG   :0x%016" PRIxPTR "\n", hpet_get_tn_cfg(0));
  syslog(LOG_INFO, " T0_CMP   :0x%016" PRIxPTR "\n", hpet_get_tn_cmp(0));

}

struct oneshot_lowerhalf_s *
la64_mtimer_initialize(volatile void *tbase, int irq, uint64_t freq)
{
  struct la64_oneshot_lowerhalf_s *priv = &g_la64_oneshot_lowerhalf;

  tmrinfo("oneshot_initialize");

  priv->tbase   = tbase;
  priv->irq     = irq;
  priv->running = false;

  irq_attach(priv->irq, la64_oneshot_interrupt, priv);

  la64_mtimer_oneshot_initialize_per_cpu();

  oneshot_count_init(&priv->lower, freq);

  tmrinfo("oneshot_initialize ok %p \n", priv);

  return &priv->lower;
}

#ifdef CONFIG_SMP
void la64_mtimer_secondary_init(void)
{
  la64_mtimer_oneshot_initialize_per_cpu();
}
#endif
