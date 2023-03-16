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

#include <linux/kernel.h>
#include <linux/module.h>

/* nexell soc headers */
#include <mach/platform.h>

/*------------------------------------------------------------------------------
 * Macro
 */
#define	LOCK_INTERRUPT				(1)

/*------------------------------------------------------------------------------
 * Low level debug function.
 * default debug port is '0'
 */
#define	REG_READ16(a)			(*(volatile unsigned short*)(a))
#define	REG_READ32(a)			(*(volatile unsigned   int*)(a))

#if (0)
#define	REG_WRITE16(a, v)		(*(volatile unsigned short*)(a) = v)
#define	REG_WRITE32(a, v)		(*(volatile unsigned   int*)(a) = v)
#else
#define	REG_WRITE16(a, v)		WriteIOW (a, v)
#define	REG_WRITE32(a, v)		WriteIODW(a, v)
#endif

#define	UART_DEBUG_BASE		IO_ADDRESS(PB_UART_PHYS_BASE)

#define REG_UART_LCON		((volatile unsigned short *)(UART_DEBUG_BASE + 0x00))
#define REG_UART_UCON		((volatile unsigned short *)(UART_DEBUG_BASE + 0x02))
#define REG_UART_FCON		((volatile unsigned short *)(UART_DEBUG_BASE + 0x04))
#define REG_UART_MCON		((volatile unsigned short *)(UART_DEBUG_BASE + 0x06))
#define REG_UART_TRSTATUS	((volatile unsigned short *)(UART_DEBUG_BASE + 0x08))
#define REG_UART_ESTATUS	((volatile unsigned short *)(UART_DEBUG_BASE + 0x0A))
#define REG_UART_FSTATUS	((volatile unsigned short *)(UART_DEBUG_BASE + 0x0C))
#define REG_UART_MSTATUS	((volatile unsigned short *)(UART_DEBUG_BASE + 0x0E))
#define REG_UART_THB		((volatile unsigned short *)(UART_DEBUG_BASE + 0x10))
#define REG_UART_RHB		((volatile unsigned short *)(UART_DEBUG_BASE + 0x12))
#define REG_UART_BRD		((volatile unsigned short *)(UART_DEBUG_BASE + 0x14))
#define REG_UART_TIMEOUT	((volatile unsigned short *)(UART_DEBUG_BASE + 0x16))
#define REG_UART_INTCON		((volatile unsigned short *)(UART_DEBUG_BASE + 0x18))
#define REG_UART_CLKENB		((volatile unsigned   int *)(UART_DEBUG_BASE + 0x40))
#define REG_UART_CLKGEN		((volatile unsigned   int *)(UART_DEBUG_BASE + 0x44))

inline static void uart_init(void)
{
	U32 CLKinHz;

	// UART RX0 : dedicated pin
	// UART TX0 : GPIO B[0] ALT1
	#define	UART_GPIO_TX_GROUP		1	// 0: A, 1:B, 2:C
	#define	UART_GPIO_TX_BIT		0

	#define	REG_GPIOB_ALTFN_BASE	IO_ADDRESS( (PHY_BASEADDR_GPIO  + 0x20)  + 	\
									 			(UART_GPIO_TX_GROUP * 0x40)  + 	\
									 			(UART_GPIO_TX_BIT > 15 ? 0x4: 0x0)	)

	REG_WRITE32((volatile unsigned int *)REG_GPIOB_ALTFN_BASE,
		((REG_READ32(REG_GPIOB_ALTFN_BASE) & ~(3<<(UART_GPIO_TX_BIT*2))) | (1<<(UART_GPIO_TX_BIT*2))));

	// calculate divisor
	CLKinHz  = CFG_UART_DEBUG_CLKFREQ;
	CLKinHz /= CFG_UART_DEBUG_BAUDRATE;
	CLKinHz += 16/2;		// + (0.5) * 16
	CLKinHz /= 16;
	CLKinHz -= 1;

	// Set UART
	REG_WRITE16(REG_UART_LCON, 0x0083); 	// Data 8 - Parity 0 - Stop 1, Normal mode, Sync pend clear
	REG_WRITE16(REG_UART_UCON, 0x0005); 	// TX, RX : polling mode
	REG_WRITE16(REG_UART_FCON, 0x0006); 	// TX, RX Fifo Reset
	REG_WRITE16(REG_UART_FCON, 0x0001); 	// FIFO enable
	REG_WRITE16(REG_UART_MCON, 0x00C0); 	// Half channel, Normal UART, No AFC

	REG_WRITE32(REG_UART_CLKENB, (1<<3)); 	// PCLKMODE : always, Clock Gen Disable
	REG_WRITE32(REG_UART_CLKGEN, ((CFG_UART_DEBUG_CLKDIV-1)<<5) | (CFG_UART_DEBUG_CLKSRC<<2));
	REG_WRITE16(REG_UART_BRD, CLKinHz);
	REG_WRITE32(REG_UART_CLKENB, (1<<3) | (1<<2));	// PCLKMODE : always, Clock Gen Enable

#if (1)
	{
	volatile U32 delay;
	for (delay=0; delay < 0xFFFF; delay++) ;
	}
#endif
}

inline static void uart_putc(char ch)
{
	const unsigned short TX_FIFO_EMPTY = 1<<1;

	/* writ to fifo */
	while ( !((REG_READ16(REG_UART_TRSTATUS)  & 0x7) & TX_FIFO_EMPTY) ) { ;	}

	REG_WRITE16(REG_UART_THB, (unsigned short)ch);

/* wait for fifo empty */
#if (1)
	while ( !((REG_READ16(REG_UART_TRSTATUS)  & 0x7) & TX_FIFO_EMPTY) ) { ;	}
#endif
}

inline static char uart_getc(void)
{
	const unsigned short RX_FIFO_COUNT	= 0xF;
	const unsigned short RX_FIFO_FULL	= 1<<8;

	while ( !(REG_READ16(REG_UART_FSTATUS) & (RX_FIFO_FULL | RX_FIFO_COUNT)) ) { ; }

	return (char)REG_READ16(REG_UART_RHB);
}

inline static int uart_rxcount(void)
{
	const unsigned short RX_FIFO_COUNT	= 0xF;
	const unsigned short RX_FIFO_FULL	= 1<<8;

	return (int)(REG_READ16(REG_UART_FSTATUS) & (RX_FIFO_FULL | RX_FIFO_COUNT));
}

/*------------------------------------------------------------------------------
 * Low level uart interface
 */
void lldebug_init(void)
{
	uart_init();
}

void lldebug_putc(const char ch)
{
   /* If \n, also do \r */
	if (ch == '\n') {
    	uart_putc('\r');
	}
	uart_putc(ch);
}

int lldebug_getc(void)
{
	int c = 0;

	do	{
		c = uart_getc();
	} while (!c);

	return c;
}

void lldebug_puts(const char *str)
{
	while (*str) {
		lldebug_putc(*str++);
	}
}

int lldebug_tstc(void)
{
	return uart_rxcount();
}

/*------------------------------------------------------------------------------
 * Low level debug interface.
 */
void lldebugout(const char *fmt, ...)
{
	va_list va;
	char buff[256];
	u_long flags;

#if	LOCK_INTERRUPT
	/* disable irq */
	local_irq_save(flags);
#endif

	lldebug_init();

	va_start(va, fmt);
	vsprintf(buff, fmt, va);
	va_end(va);

	/* direct debug out */
	lldebug_puts(buff);

#if	LOCK_INTERRUPT
	/* enable irq */
	local_irq_restore(flags);
#endif
}
EXPORT_SYMBOL_GPL(lldebugout);
