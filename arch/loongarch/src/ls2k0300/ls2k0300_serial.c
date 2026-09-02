/****************************************************************************
 * arch/loongarch/src/ls2k0300/ls2k0300_serial.c
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

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <nuttx/debug.h>

#ifdef CONFIG_SERIAL_TERMIOS
#include <termios.h>
#endif

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/spinlock.h>
#include <nuttx/init.h>
#include <nuttx/fs/ioctl.h>
#include <nuttx/semaphore.h>
#include <nuttx/serial/serial.h>

#include <arch/board/board.h>

#include "la64_internal.h"
#include "ls2k0300_config.h"
#include "chip.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* If we are not using the serial driver for the console, then we still must
 * provide some minimal implementation of up_putc.
 */

#ifdef USE_SERIALDRIVER

#ifndef CONFIG_UART0_BAUD
# define CONFIG_UART0_BAUD 115200
#endif

#ifndef CONFIG_UART0_BITS
# define CONFIG_UART0_BITS 8
#endif

#ifndef CONFIG_UART0_PARITY
# define CONFIG_UART0_PARITY 0
#endif

#ifndef CONFIG_UART0_2STOP
# define CONFIG_UART0_2STOP 0
#endif

#ifndef CONFIG_UART0_RXBUFSIZE
# define CONFIG_UART0_RXBUFSIZE 256
#endif

#ifndef CONFIG_UART0_TXBUFSIZE
# define CONFIG_UART0_TXBUFSIZE 256
#endif

#define UART_TIMEOUT_MS 100

/* Which UART with be tty0/console and which tty1?  The console will always
 * be ttyS0.  If there is no console then will use the lowest numbered UART.
 */

#ifdef HAVE_SERIAL_CONSOLE
#  if defined(CONFIG_UART0_SERIAL_CONSOLE)
#    define CONSOLE_DEV     g_uart0port     /* UART0 is console */
#    define TTYS0_DEV       g_uart0port     /* UART0 is ttyS0 */
#    define SERIAL_CONSOLE  1
#  else
#    error "I'm confused... Do we have a serial console or not?"
#  endif
#else
#  undef  CONSOLE_DEV                        /* No console */
#  undef  CONFIG_UART0_SERIAL_CONSOLE
#  if defined(CONFIG_LS2K0300_UART0)
#    define TTYS0_DEV       g_uart0port     /* UART0 is ttyS0 */
#    undef  TTYS1_DEV                       /* No ttyS1 */
#    define SERIAL_CONSOLE  1
#  else
#    undef  TTYS0_DEV
#    undef  TTYS1_DEV
#  endif
#endif

/* Common initialization logic will not not know that the all of the UARTs
 * have been disabled.  So, as a result, we may still have to provide
 * stub implementations of la64_earlyserialinit(), la64_serialinit(), and
 * up_putc().
 */

#ifdef HAVE_UART_DEVICE

/****************************************************************************
 * Private Types
 ****************************************************************************/
struct up_dev_s
{
  uintptr_t uartbase; /* Base address of UART registers */
  uint32_t  baud;     /* Configured baud */
  uint8_t   irq;      /* IRQ associated with this UART */
  uint8_t   im;       /* Interrupt mask state */
  spinlock_t lock;    /* Spinlock */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/* Low-level helpers */

static uint32_t la64_serialin(struct up_dev_s *priv, int offset);
static void la64_serialout(struct up_dev_s *priv, int offset, uint32_t value);
static void la64_restore_uartint(struct up_dev_s *priv, uint8_t im);
static void la64_disable_uartint(struct up_dev_s *priv, uint8_t *im);

/* Serial driver methods */

static int  la64_setup(struct uart_dev_s *dev);
static void la64_shutdown(struct uart_dev_s *dev);
static int  la64_attach(struct uart_dev_s *dev);
static void la64_detach(struct uart_dev_s *dev);
static int  la64_interrupt(int irq, void *context, void *arg);
static int  la64_ioctl(struct file *filep, int cmd, unsigned long arg);
static int  la64_receive(struct uart_dev_s *dev, uint32_t *status);
static void la64_rxint(struct uart_dev_s *dev, bool enable);
static bool la64_rxavailable(struct uart_dev_s *dev);
static void la64_send(struct uart_dev_s *dev, int ch);
static void la64_txint(struct uart_dev_s *dev, bool enable);
static bool la64_txready(struct uart_dev_s *dev);
static bool la64_txempty(struct uart_dev_s *dev);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct uart_ops_s g_uart_ops =
{
  .setup          = la64_setup,
  .shutdown       = la64_shutdown,
  .attach         = la64_attach,
  .detach         = la64_detach,
  .ioctl          = la64_ioctl,
  .receive        = la64_receive,
  .rxint          = la64_rxint,
  .rxavailable    = la64_rxavailable,
#ifdef CONFIG_SERIAL_IFLOWCONTROL
  .rxflowcontrol  = NULL,
#endif
  .send           = la64_send,
  .txint          = la64_txint,
  .txready        = la64_txready,
  .txempty        = la64_txempty,
};

/* I/O buffers */

#ifdef CONFIG_LS2K0300_UART0
static char g_uart0rxbuffer[CONFIG_UART0_RXBUFSIZE];
static char g_uart0txbuffer[CONFIG_UART0_TXBUFSIZE];
static uint32_t g_rxdata;

static struct up_dev_s g_uart0priv =
{
  .uartbase  = UART0_BASE,
  .baud      = CONFIG_UART0_BAUD,
  .irq       = LS2K0300_IRQ_UART0,
  .lock      = SP_UNLOCKED
};

static uart_dev_t g_uart0port =
{
#if SERIAL_CONSOLE == 1
  .isconsole = 1,
#endif
  .recv      =
  {
    .size    = CONFIG_UART0_RXBUFSIZE,
    .buffer  = g_uart0rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_UART0_TXBUFSIZE,
    .buffer  = g_uart0txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_uart0priv,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static uint32_t la64_serialin(struct up_dev_s *priv, int offset)
{
  return getreg8(priv->uartbase + offset);
}

static void la64_serialout(struct up_dev_s *priv, int offset, uint32_t value)
{
  putreg8(value, priv->uartbase + offset);
}

static void la64_restore_uartint(struct up_dev_s *priv, uint8_t im)
{
  irqstate_t flags = spin_lock_irqsave(&priv->lock);

  priv->im = im;
  la64_serialout(priv, UART_IER, im);

  spin_unlock_irqrestore(&priv->lock, flags);
}

static void la64_disable_uartint(struct up_dev_s *priv, uint8_t *im)
{
  irqstate_t flags = spin_lock_irqsave(&priv->lock);

  /* Return the current interrupt mask value */

  if (im)
  {
    *im = priv->im;
  }

  /* Disable all interrupts */

  priv->im = 0;
  la64_serialout(priv, UART_IER, 0);
  spin_unlock_irqrestore(&priv->lock, flags);
}

/****************************************************************************
 * Name: la64_serial_init
 ****************************************************************************/

static void la64_serial_init(struct uart_dev_s *dev)
{
}

/****************************************************************************
 * Name: la64_setup
 *
 * Description:
 *   Configure the UART baud, bits, parity, etc. This method is called the
 *   first time that the serial port is opened.
 *
 ****************************************************************************/

static int la64_setup(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  uint64_t apb_clock = 120000000; //uart use apb clock, 125Mhz default
  uint64_t brtc, dec;
  unsigned char cfg;

  la64_serialout(priv, UART_IER, 0x0);
  la64_serialout(priv, UART_LCR, 0x0);

  /* set baud rate */
  brtc = apb_clock / priv->baud / 16;
  dec = apb_clock - (brtc * priv->baud * 16);
  dec = (dec * 255) / priv->baud / 16;

  la64_serialout(priv, UART_LCR, UART_LCR_DLAB);

  la64_serialout(priv, UART_DLL, (brtc&0xff));
  la64_serialout(priv, UART_DLH, ((brtc>>8)&0xff));
  la64_serialout(priv, UART_DLD, dec);

  //cfg = la64_serialin(priv, UART_LCR);
  //la64_serialout(priv, UART_LCR, cfg & ~UART_LCR_DLAB);
  la64_serialout(priv, UART_LCR, UART_LCR_WLS_8);

  /* reset FIFO */
  la64_serialout(priv, UART_FCR, UART_FCR_TL_1B |
                                 UART_FCR_TXSET |
                                 UART_FCR_RXSET);

  la64_serialout(priv, UART_IER, 0x0);

  la64_serialout(priv, UART_MCR, 0x0);

  cfg = la64_serialin(priv, UART_LSR);
  cfg = la64_serialin(priv, UART_RBR);
  cfg = la64_serialin(priv, UART_MSR);

  priv->im = la64_serialin(priv, UART_IER) & UART_IER_ALL;

  return OK;
}

/****************************************************************************
 * Name: la64_shutdown
 *
 * Description:
 *   Disable the UART.  This method is called when the serial
 *   port is closed
 *
 ****************************************************************************/

static void la64_shutdown(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  /* Disable interrupts */

  la64_disable_uartint(priv, NULL);
}

/****************************************************************************
 * Name: la64_attach
 *
 * Description:
 *   Configure the UART to operation in interrupt driven mode. This method is
 *   called when the serial port is opened.  Normally, this is just after the
 *   the setup() method is called, however, the serial console may operate in
 *   a non-interrupt driven mode during the boot phase.
 *
 *   RX and TX interrupts are not enabled by the attach method (unless the
 *   hardware supports multiple levels of interrupt enabling).  The RX and TX
 *   interrupts are not enabled until the txint() and rxint() are called.
 *
 ****************************************************************************/

static int la64_attach(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  int ret;

  /* Initialize interrupt generation on the peripheral */

  //la64_serialout(priv, UART_IER, UART_IER_IRxE | UART_IER_ITxE);

  ret = irq_attach(priv->irq, la64_interrupt, dev);

  if (ret == OK)
  {
    /* Enable the interrupt (RX and TX interrupts are still disabled
     * in the UART
     */

    up_enable_irq(priv->irq);
  }
  else
  {
    _err("IRQ attach failed, ret=%d\n", ret);
  }

  return ret;
}

/****************************************************************************
 * Name: la64_detach
 *
 * Description:
 *   Detach UART interrupts.  This method is called when the serial port is
 *   closed normally just before the shutdown method is called. The exception
 *   is the serial console which is never shutdown.
 *
 ****************************************************************************/

static void la64_detach(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  /* Disable interrupts */

  up_disable_irq(priv->irq);

  /* Detach from the interrupt */

  irq_detach(priv->irq);
}

/****************************************************************************
 * Name: la64_interrupt
 *
 * Description:
 *   This is the UART interrupt handler.  It will be invoked when an
 *   interrupt is received on the 'irq'.  It should call uart_xmitchars or
 *   uart_recvchars to perform the appropriate data transfers.  The
 *   interrupt handling logic must be able to map the 'arg' to the
 *   appropriate uart_dev_s structure in order to call these functions.
 *
 ****************************************************************************/

static int la64_interrupt(int irq, void *context, void *arg)
{
  struct uart_dev_s *dev = (struct uart_dev_s *)arg;
  struct up_dev_s *priv;
  uint32_t status;
  int passes;

  DEBUGASSERT(dev != NULL && dev->priv != NULL);
  priv = (struct up_dev_s *)dev->priv;

  /* Loop until there are no characters to be transferred or,
   * until we have been looping for a long time.
   */

  for (passes = 0; passes < 256; passes++)
    {
      /* Retrieve interrupt pending status */

      status = la64_serialin(priv, UART_IIR);

      if (status == 0)
        {
          break;
        }

      if (status & UART_IIR_RxTRIG)
        {
          /* Process incoming bytes */
          uart_recvchars(dev);
        }

      if (status & UART_IIR_TxEMPTY)
        {
          /* Process outgoing bytes */

          uart_xmitchars(dev);
        }
    }

  return OK;
}

/****************************************************************************
 * Name: la64_ioctl
 *
 * Description:
 *   All ioctl calls will be routed through this method
 *
 ****************************************************************************/

static int la64_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  return -ENOTTY;
}

/****************************************************************************
 * Name: la64_receive
 *
 * Description:
 *   Called (usually) from the interrupt level to receive one
 *   character from the UART.  Error bits associated with the
 *   receipt are provided in the return 'status'.
 *
 ****************************************************************************/

static int la64_receive(struct uart_dev_s *dev, uint32_t *status)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  uint32_t rbr;

  /* Return status information */
  *status = la64_serialin(priv, UART_LSR);
  rbr = la64_serialin(priv, UART_RBR);

  /* Return cached data */

  return rbr;
}

/****************************************************************************
 * Name: la64_rxint
 *
 * Description:
 *   Call to enable or disable RX interrupts
 *
 ****************************************************************************/

static void la64_rxint(struct uart_dev_s *dev, bool enable)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  irqstate_t flags = enter_critical_section();

  if (enable)
    {
      priv->im |= UART_IER_IRxE;
    }
  else
    {
      priv->im &= ~UART_IER_IRxE;
    }

  la64_serialout(priv, UART_IER, priv->im);
  leave_critical_section(flags);
}

/****************************************************************************
 * Name: la64_rxavailable
 *
 * Description:
 *   Return true if the receive register is not empty
 *
 ****************************************************************************/

static bool la64_rxavailable(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  /* Return true is data is available in the receive data buffer */

  return la64_serialin(priv, UART_LSR) & UART_LSR_DR;
}

/****************************************************************************
 * Name: la64_send
 *
 * Description:
 *   This method will send one byte on the UART.
 *
 ****************************************************************************/

static void la64_send(struct uart_dev_s *dev, int ch)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  la64_serialout(priv, UART_THR, (uint32_t)ch);
}

/****************************************************************************
 * Name: la64_txint
 *
 * Description:
 *   Call to enable or disable TX interrupts
 *
 ****************************************************************************/

static void la64_txint(struct uart_dev_s *dev, bool enable)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  irqstate_t flags;

  flags = enter_critical_section();
  if (enable)
  {
    /* Enable the TX interrupt */
    priv->im |= UART_IER_ITxE;
  }
  else
  {
    /* Disable the TX interrupt */
    priv->im &= ~UART_IER_ITxE;
  }

  la64_serialout(priv, UART_IER, priv->im);

  leave_critical_section(flags);
}

/****************************************************************************
 * Name: la64_txready
 *
 * Description:
 *   Return true if the tranmsit data register is not full
 *
 ****************************************************************************/

static bool la64_txready(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  uint8_t lsr = la64_serialin(priv, UART_LSR);

  /* Return TRUE if the TX FIFO is not full */

  return (lsr & UART_LSR_TFE) != 0;
}

/****************************************************************************
 * Name: la64_txempty
 *
 * Description:
 *   Return true if the tranmsit data register is empty
 *
 ****************************************************************************/

static bool la64_txempty(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  uint8_t lsr = la64_serialin(priv, UART_LSR);

  return (lsr & UART_LSR_TE) != 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef USE_EARLYSERIALINIT

/****************************************************************************
 * Name: la64_earlyserialinit
 *
 * Description:
 *   Performs the low level UART initialization early in debug so that the
 *   serial console will be available during boot up.  This must be called
 *   before la64_serialinit.  NOTE:  This function depends on GPIO pin
 *   configuration performed in up_consoleinit() and main clock iniialization
 *   performed in up_clkinitialize().
 *
 ****************************************************************************/

void la64_earlyserialinit(void)
{
  /* Disable interrupts from all UARTS.  The console is enabled in
   * la64_consoleinit().
   */

  la64_disable_uartint(TTYS0_DEV.priv, NULL);
#ifdef TTYS1_DEV
  la64_disable_uartint(TTYS1_DEV.priv, NULL);
#endif

  /* Configuration whichever one is the console */

#ifdef HAVE_SERIAL_CONSOLE
  CONSOLE_DEV.isconsole = true;
  la64_setup(&CONSOLE_DEV);
#endif
}
#endif

/****************************************************************************
 * Name: la64_serialinit
 *
 * Description:
 *   Register serial console and serial ports.  This assumes
 *   that la64_earlyserialinit was called previously.
 *
 ****************************************************************************/

void la64_serialinit(void)
{
  /* Register the console */

#ifdef HAVE_SERIAL_CONSOLE
  uart_register("/dev/console", &CONSOLE_DEV);
#endif

  /* Register all UARTs */

  uart_register("/dev/ttyS0", &TTYS0_DEV);
#ifdef TTYS1_DEV
  uart_register("/dev/ttyS1", &TTYS1_DEV);
#endif
}

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Provide priority, low-level access to support OS debug  writes
 *
 ****************************************************************************/

void up_putc(int ch)
{
#ifdef HAVE_SERIAL_CONSOLE
  struct up_dev_s *priv = (struct up_dev_s *)CONSOLE_DEV.priv;
  uint8_t imr;

  if (up_interrupt_context())
  {
    la64_lowputc(ch);
  }
  else
  {
    irqstate_t flags = spin_lock_irqsave(&priv->lock);
      la64_lowputc(ch);
    spin_unlock_irqrestore(&priv->lock, flags);
  }
  /*
  la64_disable_uartint(priv, &imr);
  la64_lowputc(ch);
  la64_restore_uartint(priv, imr);
  */
#endif
}

#endif /* HAVE_UART_DEVICE */

#else /* USE_SERIALDRIVER */

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Provide priority, low-level access to support OS debug writes
 *
 ****************************************************************************/

void up_putc(int ch)
{
#ifdef HAVE_SERIAL_CONSOLE
  la64_lowputc(ch);
#endif
}

#endif /* USE_SERIALDRIVER */


/****************************************************************************
 * Low Serial
 ****************************************************************************/

void la64_lowputc(char ch)
{
#ifdef HAVE_SERIAL_CONSOLE
  while ((readb(UART0_BASE + 0x5) & 0x20) == 0);
  writeb(ch, UART0_BASE +0x0);
#endif  /* HAVE_CONSOLE */
}

void la64_lowsetup(void)
{
  /* UART0 initiaze */
  writeb(UART_LCR_DLAB, UART0_BASE + UART_LCR);

  writeb(UART_DIV_HI, UART0_BASE + UART_DLH);
  writeb(UART_DIV_LO, UART0_BASE + UART_DLL);

  writeb(0x03, UART0_BASE + UART_LCR);
  writeb(71, UART0_BASE + 0x2);
}


