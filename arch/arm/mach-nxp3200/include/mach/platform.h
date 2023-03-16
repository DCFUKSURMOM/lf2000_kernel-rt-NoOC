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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifndef __ASSEMBLY__
#include <mach/cfg_type.h>
#include <mach/cfg_gpio.h>
#include <mach/cfg_main.h>
#include <mach/cfg_mem.h>
#endif

#include "nxp3200.h"
#include <mach/iomap.h>
/*------------------------------------------------------------------------------
 * Interrupt pend register offset (entry-macro.S)
 */
#define	PB_INTC_PHYS_BASE	 	PHY_BASEADDR_INTC
#define	PB_INTC_VIRT_BASE		IO_ADDRESS(PB_INTC_PHYS_BASE)
#define	PB_INTC_LPEND_OFFSET	0x20
#define	PB_INTC_HPEND_OFFSET	0x24

/*------------------------------------------------------------------------------
 * For low level uart debug (debug-macro.S)
 */
#ifdef CONFIG_DEBUG_LL_UART
	#if	(0 == CONFIG_DEBUG_LL_UART_PORT)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART0
	#elif (1 == CONFIG_DEBUG_LL_UART_PORT)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART1
	#elif (2 == CONFIG_DEBUG_LL_UART_PORT)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART2
	#elif (3 == CONFIG_DEBUG_LL_UART_PORT)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART3
	#elif (4 == CONFIG_DEBUG_LL_UART_PORT)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART4
	#elif (5 == CONFIG_DEBUG_LL_UART_PORT)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART5
	#else
		#error not support low level debug uart port (0 ~ 5)
	#endif
#else
	#if	(0 == CFG_UART_DEBUG_CH)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART0
	#elif (1 == CFG_UART_DEBUG_CH)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART1
	#elif (2 == CFG_UART_DEBUG_CH)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART2
	#elif (3 == CFG_UART_DEBUG_CH)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART3
	#elif (4 == CFG_UART_DEBUG_CH)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART4
	#elif (5 == CFG_UART_DEBUG_CH)
		#define	PB_UART_PHYS_BASE		PHY_BASEADDR_UART5
	#else
		#error not support low level debug uart port (0 ~ 5)
	#endif
#endif	/* CONFIG_DEBUG_LL && CONFIG_DEBUG_LL_UART */

#define	PB_UART_VIRT_BASE		IO_ADDRESS(PB_UART_PHYS_BASE)
#define	PB_UART_THB				0x10	/* Transmit buffer register */

#define	PB_UART_TRSTATUS		0x08 	/* FIFO TR status register */
#define PB_UART_TX_EMPTY		0x04	/* TX register empty and TX done */
#define	PB_UART_FIFO_EMPTY		0x02	/* TX buffer or FIFO empty */


/*------------------------------------------------------------------------------
 * Get virtual interrupt number: gpio, alive, dma
 */
#define	PB_PIO_IRQ(nr)		(IRQ_GPIO_START   + nr)
#define	PB_ALV_IRQ(nr)		(IRQ_ALIVE_START  + PAD_GET_BIT(nr))
#define	PB_DMA_IRQ(nr)		(IRQ_DMA_START    + nr)
#define	PB_SYS_IRQ(nr)		(IRQ_SYSCTL_START + nr)

/*------------------------------------------------------------------------------
 * Debugs
 */
#ifndef __ASSEMBLY__

/* Direct debug out to serial port */
extern void lldebugout(const char *fmt, ...);

/* system stop */
#ifndef halt
#define	halt()	{ 											\
		printk(KERN_ERR "%s(%d) : %s system halt ...\n", 	\
		__FILE__, __LINE__, __FUNCTION__); 					\
		do {} while(1);										\
	}
#endif

#if (0)
#define	PM_DBGOUT			lldebugout
#define	PM_DBGOUT_ON		1
#else
#define	PM_DBGOUT(msg...)	do {} while (0)
#define	PM_DBGOUT_ON		0
#endif

#endif	/* __ASSEMBLY__ */
#endif	/* __PLATFORM_H__ */


