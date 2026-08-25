/****************************************************************************
 * arch/loongarch64/src/common/la64_tlb_refill.S
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
#include <arch/csr.h>
#include <arch/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Symbols
 ****************************************************************************/

/****************************************************************************
 * Name: la64_machine_check_entry
 *
 * Description:
 *
 ****************************************************************************/

  .section .text
  .p2align  4
  .global la64_tlb_refill_entry
  .type   la64_tlb_refill_entry, @function

la64_tlb_refill_entry:
    csrwr   a0, LA_CSR_TLBRSAVE
    csrwr   a1, LA_CSR_KS0
    csrwr   a2, LA_CSR_KS1

    /* get tlb miss orig vaddr */
    csrrd   a0, LA_CSR_BADV

    /* TODO */

    /* retore */
    csrrd   a2, LA_CSR_KS1
    csrrd   a1, LA_CSR_KS0
    csrrd   a0, LA_CSR_TLBRSAVE

    ertn
