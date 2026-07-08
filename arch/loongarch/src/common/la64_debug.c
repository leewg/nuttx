#include <nuttx/config.h>

#include <stdio.h>
#include <stdint.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <arch/csr.h>
#include <nuttx/debug.h>
#include <assert.h>

#include "la64_internal.h"

/****************************************************************************
 * 1. LoongArch CSR 寄存器及位域定义
 ****************************************************************************/

/* CRMD.IE: 全局中断使能位 (Bit 2) */
#define CRMD_IE_SHIFT            2
#define CRMD_IE_MASK             (1U << CRMD_IE_SHIFT)

/* ECFG.IM: 局部中断屏蔽位 (Bits 12:0, 共13个中断源) */
#define ECFG_IM_SHIFT            0
#define ECFG_IM_MASK             (0x1fffU << ECFG_IM_SHIFT)

/* ESTAT.IS: 软硬件中断状态悬挂位 (Bits 12:0) */
#define ESTAT_IS_SHIFT           0
#define ESTAT_IS_MASK            (0x1fffU << ESTAT_IS_SHIFT)

/****************************************************************************
 * 2. ExtIOI 扩展中断控制器 (IOCSR 空间) 地址定义
 ****************************************************************************/

#define IOCSR_EXTIOI_EN_BASE     0x1600  /* 中断使能寄存器基址 (0-31 号中断) */
#define IOCSR_EXTIOI_ISR_BASE    0x1800  /* 中断状态寄存器基址 (0-31 号中断) */

/****************************************************************************
 * 3. 硬件底层访问内联函数
 ****************************************************************************/

/* 读取 CSR 寄存器 (64位) */
static inline uint64_t la64_read_csr(uint32_t csr)
{
  uint64_t val;
  __asm__ __volatile__(
      "csrrd %0, %1"
      : "=r"(val)
      : "i"(csr)
      );
  return val;
}

/* 通过 IOCSR 空间读取 ExtIOI 寄存器 (32位) */
static inline uint32_t la64_read_iocsr32(uint32_t addr)
{
  uint32_t val;
  __asm__ __volatile__(
      "iocsrrd.w %0, %1"
      : "=r"(val)
      : "r"(addr)
      );
  return val;
}

/****************************************************************************
 * 4. 中断状态读取与打印主函数
 ****************************************************************************/

void dump_loongarch_interrupt_info(void)
{
  /* 1. 读取 CSR 寄存器原始值 */
  uint64_t crmd  = la64_read_csr(LOONGARCH_CSR_CRMD);
  uint64_t ecfg  = la64_read_csr(LOONGARCH_CSR_ECFG);
  uint64_t estat = la64_read_csr(LOONGARCH_CSR_ESTAT);

  /* 2. 解析核心位域 */
  uint8_t  crmd_ie  = (crmd & CRMD_IE_MASK) >> CRMD_IE_SHIFT;
  uint16_t ecfg_im  = (ecfg & ECFG_IM_MASK) >> ECFG_IM_SHIFT;
  uint16_t estat_is = (estat & ESTAT_IS_MASK) >> ESTAT_IS_SHIFT;


  syslog(LOG_INFO, "\n================ LoongArch Interrupt Status Dump ================\n");

  /* --- 打印 CPU 核心全局使能与掩码 --- */
  syslog(LOG_INFO, "[CPU CORE CSR]\n");
  syslog(LOG_INFO, "  CRMD.IE  (Global Interrupt Enable) : %u (%s)\n",
      crmd_ie, crmd_ie ? "ENABLED" : "DISABLED");

  syslog(LOG_INFO, "  ECFG.IM  (Interrupt Mask        )  : 0x%04x\n", ecfg_im);
  syslog(LOG_INFO, "  ESTAT.IS (CPU Pending Interrupts)  : 0x%04x\n", estat_is);

  /* --- 打印 ExtIOI 外设中断控制器状态 --- */
  syslog(LOG_INFO, "\n[EXTIOI INTERRUPT CONTROLLER (IOCSR)]\n");
  uint32_t ext_b19  = readl(0x16000100);
  uint32_t ext_map  = readl(0x160014c0);
  uint32_t ext_en0  = readl(0x16001600);
  uint32_t ext_en1  = readl(0x16001604);
  uint32_t ext_en2  = readl(0x16001608);
  uint32_t ext_en3  = readl(0x1600160c);
  uint32_t ext_isr0 = readl(0x16001700);
  uint32_t ext_isr1 = readl(0x16001704);
  uint32_t ext_isr2 = readl(0x16001708);
  uint32_t ext_isr3 = readl(0x1600170c);

  syslog(LOG_INFO, "  ExtIOI_BIT19 (Extern Mode) : 0x%08x\n", ext_b19);
  syslog(LOG_INFO, "  ExtIOI_MAP   (Map Core IP) : 0x%08x\n", ext_map);
  syslog(LOG_INFO, "  ExtIOI_EN    (EN0 ~ EN3)   : 0x%08x, 0x%08x, 0x%08x, 0x%08x\n", ext_en0, ext_en1, ext_en2, ext_en3);
  syslog(LOG_INFO, "  ExtIOI_ISR0  (IRS0 ~ ISR3) : 0x%08x, 0x%08x, 0x%08x, 0x%08x\n", ext_isr0, ext_isr1, ext_isr2, ext_isr3);

  syslog(LOG_INFO, "=================================================================\n\n");
}
