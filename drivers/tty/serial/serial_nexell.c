/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/device.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/seqlock.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>
#include <linux/serial.h>
#include <linux/version.h>
#include <asm/irq.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>

/* Debugging stuff */
#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "uart: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#if (0)
#define DBGINTR(msg...)		printk(msg)
#else
#define DBGINTR(msg...)		do {} while (0)
#endif

/*
 * 	Uart macros & functions
 */
#define	PORT_NEXELL			76
#define	UART_TERMINAL		"ttyS"
#define UART_BAUDRATE		115200

#define	UART_TXD_INTNUM		0	/* Tx interrupt bit */
#define	UART_RXD_INTNUM		1	/* Rx interrupt bit */
#define	UART_ERR_INTNUM		2	/* Error interrupt bit */
#define	UART_MOD_INTNUM		3	/* Modem interrupt bit */

#define UART_TXD_LEVEL		0	/* empty */
#define UART_RXD_LEVEL		12	/* 12byte */

#define UART_RXD_DUMMY		(0x10000000)

#define UART_ERR_OVERRUN	(1<<0)	/* overrun error */
#define UART_ERR_PARITY		(1<<1)	/* parity error */
#define UART_ERR_FRAME		(1<<2)	/* frame error */
#define UART_ERR_BREAK		(1<<3)	/* break error */

#define UART_INT_PEND		(0x18)

#define	UART_WAIT_READY		{ volatile int cnt; for (cnt=0; cnt <0xffff; cnt++); }

/* uart ops functions */
static struct uart_ops nx_uart_ops;

static unsigned int nx_uart_ops_tx_empty(struct uart_port *port);
static void nx_uart_ops_set_mctrl(struct uart_port *port, unsigned int mctrl);
static unsigned int nx_uart_ops_get_mctrl(struct uart_port *port);
static void nx_uart_ops_stop_tx(struct uart_port *port);
static void nx_uart_ops_start_tx(struct uart_port *port);
static void nx_uart_ops_stop_rx(struct uart_port *port);
static void nx_uart_ops_enable_ms(struct uart_port *port);
static void nx_uart_ops_break_ctl(struct uart_port *port, int break_state);
static int nx_uart_ops_startup(struct uart_port *port);
static void nx_uart_ops_shutdown(struct uart_port *port);
static void nx_uart_ops_set_termios(struct uart_port *port, struct ktermios *termios, struct ktermios *old);
static const char *nx_uart_ops_type(struct uart_port *port);
static void nx_uart_ops_release_port(struct uart_port *port);
static int nx_uart_ops_request_port(struct uart_port *port);
static void nx_uart_ops_config_port(struct uart_port *port, int flags);
static int nx_uart_ops_verify_port(struct uart_port *port, struct serial_struct *ser);

/* interrupt handler */
static irqreturn_t nx_uart_irq_handler(int irq, void *dev_id);

/*
 * nexell uart struct
 */
struct nx_uart_port {
	struct uart_port	port;	/* Note : First type must be 'struct uart_port' */
	char			*	name;
	int					count;	/* open count */
};

/*
 * 	uart port info
 */
static struct nx_uart_port nx_ports[] = {
#if defined(CONFIG_SERIAL_NEXELL_UART_PORT0)
	{
		.name = "UART0",
		.port = {
			.type 		= PORT_NEXELL,
			.iotype 	= UPIO_MEM,
			.membase 	= (u_char __iomem *)IO_ADDRESS(PHY_BASEADDR_UART0),
			.mapbase 	= PHY_BASEADDR_UART0,
			.irq 		= IRQ_PHY_UART0,
			.uartclk 	= CFG_UART_DEBUG_CLKFREQ,
			.fifosize 	= 16,
			.flags 		= UPF_BOOT_AUTOCONF,
			.ops 		= &nx_uart_ops,
			.line 		= 0,
		},
	},
#endif
#if defined(CONFIG_SERIAL_NEXELL_UART_PORT1)
	{
		.name = "UART1",
		.port = {
			.type 		= PORT_NEXELL,
			.iotype 	= UPIO_MEM,
			.membase 	= (u_char __iomem *)IO_ADDRESS(PHY_BASEADDR_UART1),
			.mapbase 	= PHY_BASEADDR_UART1,
			.irq 		= IRQ_PHY_UART1,
			.uartclk 	= CFG_UART_DEBUG_CLKFREQ,
			.fifosize 	= 16,
			.flags 		= UPF_BOOT_AUTOCONF,
			.ops 		= &nx_uart_ops,
			.line 		= 1,
		},
	},
#endif
#if defined(CONFIG_SERIAL_NEXELL_UART_PORT2)
	{
		.name = "UART2",
		.port = {
			.type 		= PORT_NEXELL,
			.iotype 	= UPIO_MEM,
			.membase 	= (u_char __iomem *)IO_ADDRESS(PHY_BASEADDR_UART2),
			.mapbase 	= PHY_BASEADDR_UART2,
			.irq 		= IRQ_PHY_UART2,
			.uartclk 	= CFG_UART_DEBUG_CLKFREQ,
			.fifosize 	= 16,
			.flags 		= UPF_BOOT_AUTOCONF,
			.ops 		= &nx_uart_ops,
			.line 		= 2,
		},
	},
#endif
#if defined(CONFIG_SERIAL_NEXELL_UART_PORT3)
	{
		.name = "UART3",
		.port = {
			.type 		= PORT_NEXELL,
			.iotype 	= UPIO_MEM,
			.membase 	= (u_char __iomem *)IO_ADDRESS(PHY_BASEADDR_UART3),
			.mapbase 	= PHY_BASEADDR_UART3,
			.irq 		= IRQ_PHY_UART3,
			.uartclk 	= CFG_UART_DEBUG_CLKFREQ,
			.fifosize 	= 16,
			.flags 		= UPF_BOOT_AUTOCONF,
			.ops 		= &nx_uart_ops,
			.line 		= 3,
		},
	},
#endif
#if defined(CONFIG_SERIAL_NEXELL_UART_PORT4)
	{
		.name = "UART4",
		.port = {
			.type 		= PORT_NEXELL,
			.iotype 	= UPIO_MEM,
			.membase 	= (u_char __iomem *)IO_ADDRESS(PHY_BASEADDR_UART4),
			.mapbase 	= PHY_BASEADDR_UART4,
			.irq 		= IRQ_PHY_UART4,
			.uartclk 	= CFG_UART_DEBUG_CLKFREQ,
			.fifosize 	= 16,
			.flags 		= UPF_BOOT_AUTOCONF,
			.ops 		= &nx_uart_ops,
			.line 		= 4,
		},
	},
#endif
#if defined(CONFIG_SERIAL_NEXELL_UART_PORT5)
	{
		.name = "UART5",
		.port = {
			.type 		= PORT_NEXELL,
			.iotype 	= UPIO_MEM,
			.membase 	= (u_char __iomem *)IO_ADDRESS(PHY_BASEADDR_UART5),
			.mapbase 	= PHY_BASEADDR_UART5,
			.irq 		= IRQ_PHY_UART5,
			.uartclk 	= CFG_UART_DEBUG_CLKFREQ,
			.fifosize 	= 16,
			.flags 		= UPF_BOOT_AUTOCONF,
			.ops 		= &nx_uart_ops,
			.line 		= 5,
		},
	},
#endif
};

/*
 * u_int serial_calc_divisor(uart_channel,baud)
 * calculate divisor for given uart channel
 */

u_int calc_uart_divisor(u_int uart_channel, u_int baud)
{
	/* get UART information */
	u_int   clock_source = NX_UART_GetClockSource(uart_channel, 0);
	u_int   clock_divisor = NX_UART_GetClockDivisor(uart_channel, 0);

	/* get PLL frequency */
	u_int   clock_freq_in_hz = cpu_get_clock_hz(clock_source);

	/* calculate divisor */
	u_int   divisor = clock_freq_in_hz;
	divisor /= clock_divisor;
	divisor /= baud;
	divisor += 16 / 2;      /* round number */
	divisor /= 16;
	divisor-= 1;            /* one less for divisor */

	return divisor;
}


static void init_uart_port(void)
{
	int i = 0;
	int array = NX_UART_GetNumberOfModule();
	DBGOUT("%s (array:%d)\n", __func__, array);

	for (i=0; i < array; i++) {
		NX_UART_SetBaseAddress(i, (U32)IO_ADDRESS(NX_UART_GetPhysicalAddress(i)));
		/* Disable debug uart port clock */
#if !(PM_DBGOUT_ON) && !defined(CONFIG_DEBUG_LL_UART)
		NX_UART_OpenModule(i);
		NX_UART_SetClockDivisorEnable(i, CFALSE);
		NX_UART_SetClockPClkMode(i, NX_PCLKMODE_DYNAMIC);
#endif
	}
}

static void setup_uart_port(unsigned int port)
{
	DBGOUT("%s (port:%d)\n", __func__, port);

	/* Select module & initialize */
	NX_UART_OpenModule(port);

	/* Clock & Interrupt Disable */
	NX_UART_SetClockDivisorEnable(port, CFALSE);
	NX_UART_SetClockPClkMode(port, NX_PCLKMODE_DYNAMIC);

	NX_UART_ClearInterruptPendingAll(port);
   	NX_UART_SetInterruptEnableAll(port, CFALSE);

	// UART Mode : Tx, Rx Only
   	NX_UART_SetSIRMode(port, CFALSE);
	NX_UART_SetLoopBackMode(port, CFALSE);
	NX_UART_SetAutoFlowControl(port, CFALSE);
	NX_UART_SetHalfChannelEnable(port, CTRUE);

    NX_UART_SetSCTxEnb(port, CFALSE);
    NX_UART_SetSCRxEnb(port, CTRUE);

	// Frame Configuration : Data 8 - Parity 0 - Stop 1
	NX_UART_SetFrameConfiguration (port, NX_UART_PARITYMODE_NONE, 8, 1);

	// Tx Rx Operation : Default Polling
  	NX_UART_SetTxIRQType(port, NX_UART_IRQTYPE_PULSE);
   	NX_UART_SetTxOperationMode(port, NX_UART_OPERMODE_NORMAL);    	// Interrupt or Polling

   	NX_UART_SetRxIRQType(port, NX_UART_IRQTYPE_LEVEL);
   	NX_UART_SetRxOperationMode(port, NX_UART_OPERMODE_NORMAL);    	// Interrupt or Polling
   	NX_UART_SetRxTimeOutEnb(port, CTRUE);							// NEED Enable for RX interrupt.
   	NX_UART_SetIntEnbWhenExceptionOccur(port, CFALSE);

	NX_UART_SetSYNCPendClear(port);

	/* FCON: fifo control */
	NX_UART_SetTxFIFOTriggerLevel(port, UART_TXD_LEVEL);			// Tx empty irq
    NX_UART_ResetTxFIFO(port);

	NX_UART_SetRxFIFOTriggerLevel(port, UART_RXD_LEVEL);			// Rx full  irq
    NX_UART_ResetRxFIFO(port);
    NX_UART_SetFIFOEnb(port, CFALSE);

	/* MCON: modem control - skip */
    NX_UART_SetDTR(port, CFALSE);
    NX_UART_SetRTS(port, CFALSE);

	/* TIMEOUTREG: receive timeout */
    NX_UART_SetRxTimeOut(port, 0x1F);

	/* INTSTATUSREG: Interrupt status */
    NX_UART_ClearInterruptPendingAll(port);

	/* UARTCLKGEN */
	NX_UART_SetClockSource  (port, 0, CFG_UART_DEBUG_CLKSRC);
	NX_UART_SetClockDivisor (port, 0, CFG_UART_DEBUG_CLKDIV);

	/* BRDn: baud rate divisor */
	NX_UART_SetBRD(port, calc_uart_divisor(port, CFG_UART_DEBUG_BAUDRATE));

	/* UARTCLKENB */
	NX_UART_SetClockDivisorEnable(port, CTRUE);
	NX_UART_SetClockPClkMode(port, NX_PCLKMODE_ALWAYS);

	UART_WAIT_READY;
	printk(KERN_INFO "NX_UART_GetClockDivisor: 0x%X\n", NX_UART_GetClockDivisor(port, 0));
	printk(KERN_INFO "NX_UART_GetBRD:          0x%X\n", NX_UART_GetBRD(port));
}

/*---------------------------------------------------------------------------------------------
 * 	Uart ops functions
 --------------------------------------------------------------------------------------------*/

/*
 * return 0; tx buffer not empty
 * return 1: tx buffer empty
 */
static unsigned int nx_uart_ops_tx_empty(struct uart_port *port)
{
	unsigned long flags;
	unsigned int ret=0;
	DBGOUT("%s (line:%d)\n", __func__, port->line);

	/* disable irq */
	spin_lock_irqsave(&port->lock,flags);

	/* buffer empty */
	if ( (NX_UART_GetTxRxStatus(port->line) & NX_UART_TRANSMITTER_EMPTY) ) {
		ret = TIOCSER_TEMT;
	}

	/* enable irq */
	spin_unlock_irqrestore(&port->lock, flags);
	return ret;
}

/* modem control */
static void nx_uart_ops_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	DBGOUT("%s (line:%d)\n", __func__, port->line);

	/* not implementation */
}

/* modem control */
static unsigned int nx_uart_ops_get_mctrl(struct uart_port *port)
{
	unsigned int ret = (TIOCM_CTS | TIOCM_DSR | TIOCM_CAR);
	DBGOUT("nx_uart_ops_get_mctrl (line:%d)\n", port->line);

	/* not implementation */
	return ret;
}

static void nx_uart_ops_stop_tx(struct uart_port *port)
{
	DBGINTR("%s (line:%d)\n", __func__, port->line);

	if (NX_UART_GetInterruptEnable(port->line, UART_TXD_INTNUM))
		NX_UART_SetInterruptEnable(port->line, UART_TXD_INTNUM, CFALSE);
}

static void nx_uart_ops_start_tx(struct uart_port *port)
{
	DBGINTR("%s (line:%d)\n", __func__, port->line);

	if ( ! NX_UART_GetInterruptEnable(port->line, UART_TXD_INTNUM))
		NX_UART_SetInterruptEnable(port->line, UART_TXD_INTNUM, CTRUE);
}

static void nx_uart_ops_stop_rx(struct uart_port *port)
{
	DBGOUT("%s (line:%d)\n", __func__, port->line);

	if (NX_UART_GetInterruptEnable(port->line, UART_RXD_INTNUM))
		NX_UART_SetInterruptEnable(port->line, UART_RXD_INTNUM, CFALSE);
}

static void nx_uart_ops_enable_ms(struct uart_port *port)
{
	DBGOUT("%s (line:%d)\n", __func__, port->line);
	/* not implementation */
}


static void nx_uart_ops_break_ctl(struct uart_port *port, int break_state)
{
	unsigned long flags;
	DBGOUT("%s (line:%d)\n", __func__, port->line);

	/* disable irq */
	spin_lock_irqsave(&port->lock, flags);

	/* not implementation */

	/* enable irq */
	spin_unlock_irqrestore(&port->lock, flags);
}

static int nx_uart_ops_startup(struct uart_port *port)
{
	struct nx_uart_port *uart = (struct nx_uart_port *)port;
	int ret;
	unsigned long flags;

	DBGOUT("%s (line:%d, count:%d)\n", __func__, port->line, uart->count);

	ret = request_irq(port->irq, nx_uart_irq_handler, IRQF_DISABLED, UART_DEV_NAME, port);
	if ( ret!=0 ) {
		dev_err(port->dev, "unable to grab irq%d\n",port->irq);
		return ret;
	}

	/* disable irq */
	spin_lock_irqsave(&port->lock, flags);

	if(0 == uart->count)
		setup_uart_port(port->line);

	uart->count++;

	/* enable irq */
	spin_unlock_irqrestore(&port->lock, flags);
	return 0;
}

static void nx_uart_ops_shutdown(struct uart_port *port)
{
	struct nx_uart_port *uart = (struct nx_uart_port *)port;
	unsigned long flags;

	DBGOUT("%s (line:%d, count:%d)\n", __func__, port->line, uart->count);

	/* disable irq */
	spin_lock_irqsave(&port->lock,flags);

   	NX_UART_SetInterruptEnableAll(port->line, CFALSE);
    NX_UART_ClearInterruptPendingAll(port->line);

	uart->count--;
	if (0 == uart->count) {
    	NX_UART_SetSYNCPendClear(port->line);
		NX_UART_SetClockDivisorEnable(port->line, CFALSE);
	}

	if (0 >	uart->count)
		uart->count = 0;

	/* enable irq */
	spin_unlock_irqrestore(&port->lock,flags);

	/* release irq */
	free_irq(port->irq, port);
}

static void nx_uart_ops_set_termios(struct uart_port *port, struct ktermios *termios, struct ktermios *old)
{
	unsigned int data, parity, stop;
	unsigned int baud, quot;
	unsigned long flags;
	DBGOUT("%s (line:%d)\n", __func__, port->line);

	/* Data Bit */
	switch (termios->c_cflag & CSIZE) {
	case CS5:
		data =  5;
		break;
	case CS6:
		data =  6;
		break;
	case CS7:
		data =  7;
		break;
	default:
	case CS8:
		data =  8;
		break;
	}

	/* Parity Bit */
	if (termios->c_cflag & PARENB) {
		if (termios->c_cflag & PARODD)
			parity = NX_UART_PARITYMODE_ODD;
		else
			parity = NX_UART_PARITYMODE_EVEN;
	} else {
		parity = NX_UART_PARITYMODE_NONE;
	}

	/* Stop Bit */
	if (termios->c_cflag & CSTOPB)
		stop = 2;
	else
		stop = 1;

	/* Baudrate */
	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk/16);
	quot = uart_get_divisor(port, baud);

	/* disable irq */
	spin_lock_irqsave(&port->lock, flags);

	/* Update the per-port timeout */
	uart_update_timeout(port, termios->c_cflag, baud);

	/*
	 * Which character status flags are we interested in?
	 */
	port->read_status_mask = UART_ERR_OVERRUN;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= UART_ERR_FRAME | UART_ERR_PARITY;

	/*
	 * Which character status flags should we ignore?
	 */
	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= UART_ERR_OVERRUN;
	if ((termios->c_iflag & IGNBRK) && (termios->c_iflag & IGNPAR))
		port->ignore_status_mask |= UART_ERR_FRAME;

	/*
	 * Ignore all characters if CREAD is not set.
	 */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |= UART_RXD_DUMMY;

	DBGOUT("%s (baud:%d, parity:%d, data:%d, stop:%d)\n", __func__, baud , parity, data, stop);

	// set nexell uart register
	NX_UART_SetClockDivisorEnable(port->line, CFALSE);
	NX_UART_SetClockPClkMode(port->line, NX_PCLKMODE_DYNAMIC);
   	NX_UART_SetInterruptEnableAll(port->line, CFALSE);

	NX_UART_SetFrameConfiguration(port->line, parity, data, stop);
	NX_UART_SetBRD(port->line, calc_uart_divisor(port->line, CFG_UART_DEBUG_BAUDRATE));

   	NX_UART_ResetTxFIFO(port->line);
   	NX_UART_ResetRxFIFO(port->line);
   	NX_UART_SetFIFOEnb(port->line, CTRUE);

	// Enable RX Interrupt
	NX_UART_SetInterruptEnable(port->line, UART_MOD_INTNUM, CFALSE);
	NX_UART_SetInterruptEnable(port->line, UART_ERR_INTNUM, CFALSE);
	NX_UART_SetInterruptEnable(port->line, UART_RXD_INTNUM, CTRUE);
	NX_UART_SetInterruptEnable(port->line, UART_TXD_INTNUM, CFALSE);
   	NX_UART_ClearInterruptPendingAll(port->line);
   	NX_UART_SetSYNCPendClear(port->line);

//  NX_UART_SetInterruptEnableAll(port->line, CTRUE);
	NX_UART_SetClockDivisorEnable(port->line, CTRUE);
	NX_UART_SetClockPClkMode(port->line, NX_PCLKMODE_ALWAYS);

	UART_WAIT_READY;

	/* enable irq */
	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *nx_uart_ops_type(struct uart_port *port)
{
	struct nx_uart_port *uart = (struct nx_uart_port *)port;
	DBGOUT("%s (line:%d)\n", __func__, port->line);

	return uart->name;
}

static void nx_uart_ops_release_port(struct uart_port *port)
{
	DBGOUT("%s (line:%d)\n", __func__, port->line);
}

static int nx_uart_ops_request_port(struct uart_port *port)
{
	DBGOUT("%s (line:%d)\n", __func__, port->line);
	return 0;
}

static void nx_uart_ops_config_port(struct uart_port *port, int flags)
{
	DBGOUT("%s (line:%d, type:%s)\n", __func__, port->line, (char *)port->type);
	port->type = PORT_NEXELL;
}

static int nx_uart_ops_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	DBGOUT("%s (line:%d)\n", __func__, port->line);

	/* we don't want the core code to modify any port params */
	return -EINVAL;
}

static struct uart_ops nx_uart_ops = {
	.tx_empty		= nx_uart_ops_tx_empty,
	.set_mctrl		= nx_uart_ops_set_mctrl,
	.get_mctrl		= nx_uart_ops_get_mctrl,
	.stop_tx		= nx_uart_ops_stop_tx,
	.start_tx		= nx_uart_ops_start_tx,
	.stop_rx		= nx_uart_ops_stop_rx,
	.enable_ms		= nx_uart_ops_enable_ms,
	.break_ctl		= nx_uart_ops_break_ctl,
	.startup		= nx_uart_ops_startup,
	.shutdown		= nx_uart_ops_shutdown,
	.set_termios	= nx_uart_ops_set_termios,
	.type			= nx_uart_ops_type,
	.release_port	= nx_uart_ops_release_port,
	.request_port	= nx_uart_ops_request_port,
	.config_port	= nx_uart_ops_config_port,
	.verify_port	= nx_uart_ops_verify_port,
};

/*---------------------------------------------------------------------------------------------
 * 	Uart interrupt functions
 --------------------------------------------------------------------------------------------*/
static void nx_uart_rx_irq_handler(struct uart_port *port)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
	struct tty_struct *tty = port->state->port.tty;
#else
	struct tty_struct *tty = port->info->port.tty;
#endif
	unsigned int ch, flag;
	unsigned int err;
	int max_count = 256;

	DBGINTR(" rxirq...");

	while (max_count-- > 0) {

		if (NX_UART_GetRxFIFOCount(port->line)== 0)
			break;

		err  = NX_UART_GetErrorStatus(port->line);
		ch   = NX_UART_GetByte(port->line);
		flag = TTY_NORMAL;
		port->icount.rx++;

		/* error status */
		if (unlikely(err & (UART_ERR_OVERRUN |
					UART_ERR_PARITY	 |
					UART_ERR_FRAME   |
					UART_ERR_BREAK )) ) {
			printk(KERN_ERR "uart: rx error port:%d, ch=%c, err=%x\n", port->line, ch, err);

			/* break error */
			if (err & UART_ERR_BREAK) {
				port->icount.brk++;
				if (uart_handle_break(port))
					goto ignore_char;
			}
			/* parity error */
			if (err & UART_ERR_PARITY)
				port->icount.parity++;

			if (err & UART_ERR_FRAME)
				port->icount.frame++;

			if (err & UART_ERR_OVERRUN)
				port->icount.overrun++;

			/*
			 * Mask off conditions which should be ignored.
			 */
			err &= port->read_status_mask;

			if (err & UART_ERR_BREAK)
				flag = TTY_BREAK;
			else if (err & UART_ERR_PARITY)
				flag = TTY_PARITY;
			else if (err & UART_ERR_FRAME)
				flag = TTY_FRAME;
		}

		if (uart_handle_sysrq_char(port, ch))
			goto ignore_char;

		uart_insert_char(port, err, UART_LSR_OE, ch, flag);

	ignore_char:
		continue;
	}
	tty_flip_buffer_push(tty);
}

static void nx_uart_tx_irq_handler(struct uart_port *port)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
	struct circ_buf *xmit = &port->state->xmit;
#else
	struct circ_buf *xmit = &port->info->xmit;
#endif
	DBGINTR(" txirq[%d]", port->line);

	if (port->x_char) {
		NX_UART_SendByte(port->line, port->x_char);
		port->icount.tx++;
		port->x_char=0;
		return;
	}

	if (uart_circ_empty(xmit) || uart_tx_stopped(port)) {
		nx_uart_ops_stop_tx(port);
		return;
	}

	do {
		NX_UART_SendByte(port->line, xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	} while (NX_UART_GetTxFIFOCount(port->line) < port->fifosize);


	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	if (uart_circ_empty(xmit))
		nx_uart_ops_stop_tx(port);

	DBGINTR(" txe...");
}

static void nx_uart_err_irq_handler(struct uart_port *port)
{
	printk(KERN_ERR "%s (line:%d): uart error irq, not implement \n", __func__, port->line);
}

static void nx_uart_err_mod_handler(struct uart_port *port)
{
	printk(KERN_ERR "%s (line:%d): uart modem irq, not implement \n", __func__, port->line);
}

static irqreturn_t nx_uart_irq_handler(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	unsigned int ctrl, pend, mask;
	DBGINTR("uart irq(port:%d, irq:%d)", port->line, irq);

	/* get pend and mask num */
	ctrl = (*(unsigned short *)(port->membase + UART_INT_PEND)  & 0xff);
	mask = ((ctrl & 0xf0) >> 4);
	pend = ( ctrl & 0x0f);

	if ( pend & (1 << UART_RXD_INTNUM) ) {	// 1

		nx_uart_rx_irq_handler(port);

		/* clear pend */
    	NX_UART_ClearInterruptPending(port->line, UART_RXD_INTNUM);
		NX_UART_SetSYNCPendClear(port->line);
	}

	if ( pend & (1 << UART_TXD_INTNUM) ) {	// 0

		nx_uart_tx_irq_handler(port);

		/* clear pend */
		NX_UART_ClearInterruptPending(port->line, UART_TXD_INTNUM);
		NX_UART_SetSYNCPendClear(port->line);
	}

	if ( pend & (1 << UART_ERR_INTNUM) ) { 	// 2

		nx_uart_err_irq_handler(port);

		/* clear pend */
		NX_UART_ClearInterruptPending(port->line, UART_ERR_INTNUM);
		NX_UART_SetSYNCPendClear(port->line);
	}

	if ( pend & (1 << UART_MOD_INTNUM) ) { 	// 3

		if (mask & (1 << UART_MOD_INTNUM))
			nx_uart_err_mod_handler(port);

		/* clear pend */
		NX_UART_ClearInterruptPending(port->line, UART_MOD_INTNUM);
		NX_UART_SetSYNCPendClear(port->line);
	}

	DBGINTR(" exit\n");
	return IRQ_HANDLED;
}


/*---------------------------------------------------------------------------------------------
 * 	Uart console functions
 --------------------------------------------------------------------------------------------*/
static void nx_uart_putchar(struct uart_port *port, int ch)
{
	while ( !(NX_UART_GetTxRxStatus(port->line) & NX_UART_TX_BUFFER_EMPTY) ) {
		/* busy wait */;
	}
	NX_UART_SendByte(port->line, (U8)ch);
}

static void nx_console_write(struct console *co, const char *s, unsigned int count)
{
	struct uart_port *port = &nx_ports[co->index].port;
	uart_console_write(port, s, count, nx_uart_putchar);
}

static int __init nx_console_setup(struct console *co, char *options)
{
	struct nx_uart_port *uart;
	struct uart_port *port;

	int baud   =  UART_BAUDRATE;
	int bits   =  8;
	int parity = 'n';
	int flow   = 'n';

	DBGOUT("%s (id:%d, opt:%s)\n", __func__, (int)co->index, options);

	if (co->index == -1 || co->index >= ARRAY_SIZE(nx_ports))
		co->index = 0;

	uart = &nx_ports[co->index];
	port = &uart->port;

	/* uart module initialize */
	setup_uart_port(co->index);

	uart->count++;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	DBGOUT("console_setup (options:%s, baud:%d, parity:%d, bits:%d, flow:0x%x)\n",
		options, baud, parity, bits, flow);

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver uart_drv;

static struct console nx_console = {
	.name		= UART_TERMINAL,
	.write		= nx_console_write,
	.device		= uart_console_device,
	.setup		= nx_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,			/* don't modify */
	.data		= &uart_drv,
};

static int __init nx_console_init(void)
{
	DBGOUT("%s\n", __func__);

	init_uart_port();
	register_console(&nx_console);
	return 0;
}
console_initcall(nx_console_init);

/*---------------------------------------------------------------------------------------------
 * 	Uart platform driver functions
 --------------------------------------------------------------------------------------------*/
static int nx_uart_drv_probe(struct platform_device *pdev)
{
	struct nx_uart_port *uart = ( struct nx_uart_port *)&nx_ports[pdev->id].port;
	struct uart_port *port = &uart->port;

	DBGOUT("%s (id:%d)\n", __func__, pdev->id);

	port->dev = &pdev->dev;

	uart_add_one_port(&uart_drv, port);
	platform_set_drvdata(pdev, uart);
	return 0;
}

static int nx_uart_drv_remove(struct platform_device *pdev)
{
	struct nx_uart_port *uart = platform_get_drvdata(pdev);
	struct uart_port *port = &uart->port;

	DBGOUT("%s (id:%d)\n", __func__, pdev->id);

	platform_set_drvdata(pdev, NULL);
	if (port) {
		uart_remove_one_port(&uart_drv, port);
	}
	return 0;
}

static int nx_uart_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct nx_uart_port *uart = platform_get_drvdata(pdev);
	struct uart_port *port = &uart->port;
	PM_DBGOUT("+%s (line:%d)\n", __func__, port->line);

	if (port) {
		uart_suspend_port(&uart_drv, port);

		NX_UART_ResetTxFIFO(port->line);
		NX_UART_ResetRxFIFO(port->line);
	}

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

static int nx_uart_drv_resume(struct platform_device *pdev)
{
	struct nx_uart_port *uart = platform_get_drvdata(pdev);
	struct uart_port *port = &uart->port;
	PM_DBGOUT("+%s (line:%d)\n", __func__, port->line);

	if (port) {
		uart_resume_port(&uart_drv, port);
	}

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

static struct platform_driver uart_plat_drv = {
	.probe      = nx_uart_drv_probe,
	.remove     = nx_uart_drv_remove,
	.suspend	= nx_uart_drv_suspend,
	.resume		= nx_uart_drv_resume,
	.driver		= {
		.name   = UART_DEV_NAME,
	},
};

/*---------------------------------------------------------------------------------------------
 * 	Uart module init/exit functions
 --------------------------------------------------------------------------------------------*/
static struct uart_driver uart_drv = {
	.owner          = THIS_MODULE,
	.driver_name    = UART_DEV_NAME,
	.dev_name       = UART_TERMINAL,
	.major          = TTY_MAJOR,
	.minor          = 64,
	.nr             = ARRAY_SIZE(nx_ports),
	.cons           = &nx_console,
};

static int _atoi(const char *name)
{
    int val = 0;

    for (;; name++) {
		switch (*name) {
	    	case '0' ... '9':
				val = 10*val+(*name-'0');
			break;
	    	default:
			return val;
		}
    }
}

static int serial_line   = 0;
static int serial_enable = 1;

/*
 * #> cat /sys/console/serial
 */
static ssize_t serial_show(struct kobject *kobj, struct kobj_attribute *attr,
				char *buf)
{
	char *s = buf;
	DBGOUT("%s:\n", __func__);

	s += sprintf(s, "%d %s\n", serial_line, serial_enable ? "on" : "off");
	if (s != buf)
		*(s-1) = '\n';

	return (s - buf);
}

/*
 * #> echo 0 on  > /sys/console/serial
 * #> echo 0 off > /sys/console/serial
 */
static ssize_t serial_store(struct kobject *kobj, struct kobj_attribute *attr,
				const char *buf, size_t n)
{
	struct uart_port *port = NULL;
	char *ps, *pe;
	int len, line;

	ps = memchr(buf,  ' ', n);
	pe = memchr(buf, '\n', n);

	/* check argu */
	if (NULL == ps || NULL == pe)
		return n;

	/* get line num */
	line = _atoi(buf);
	if (line >= ARRAY_SIZE(nx_ports) || 0 > line)
		return n;

	ps  = memchr(buf, 'o', n);
	len = pe - ps;

	port = &nx_ports[line].port;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
	if (port->state->port.tty != get_current_tty())
		return n;
#else
	if (port->info->port.tty != get_current_tty())
		return n;
#endif

	/* start serial */
	if (len == 2 && !strncmp(ps, "on", len)) {
		serial_line   = line;
		serial_enable = 1;

		console_start(&nx_console);		// register_console(&nx_console);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
		tty_ldisc_flush(port->state->port.tty);
		start_tty(port->state->port.tty);
#else
		tty_ldisc_flush(port->info->port.tty);
		start_tty(port->info->port.tty);
#endif
	}

	/* stop serial */
	if (len == 3 && !strncmp(ps, "off", len)) {
		serial_line   = line;
		serial_enable = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
		stop_tty(port->state->port.tty);
#else
		stop_tty(port->info->port.tty);
#endif
		console_stop(&nx_console);		// unregister_console(&nx_console);
	}

	return n;
}

static struct kobj_attribute serial_attr =
			__ATTR(serial, 0644, serial_show, serial_store);

static struct attribute * g[] = {
	&serial_attr.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = g,
};

struct kobject * con_kobj;

static int __init nx_serial_drv_init(void)
{
	int ret;
	DBGOUT("%s\n", __func__);

	ret = uart_register_driver(&uart_drv);
	if (ret) {
		printk(KERN_ERR "serial: failed to register uart device (%d) \n", ret);
		return ret;
	}

	ret = platform_driver_register(&uart_plat_drv);
	if (ret != 0) {
		printk(KERN_ERR "serial: failed to register platform driver (%d) \n", ret);
		uart_unregister_driver(&uart_drv);
	}

	con_kobj = kobject_create_and_add("console", NULL);
	if (! con_kobj)
		return -ENOMEM;

	return sysfs_create_group(con_kobj, &attr_group);
}

static void __exit nx_serial_drv_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&uart_plat_drv);
	uart_unregister_driver(&uart_drv);
}

module_init(nx_serial_drv_init);
module_exit(nx_serial_drv_exit);

MODULE_AUTHOR("jhkim <nexell.co.kr>");
MODULE_DESCRIPTION("Serial driver for Nexell");
MODULE_LICENSE("GPL");


