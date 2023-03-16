/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/version.h>
#include <mach/platform.h>

	.macro	addruart, rp, rv, tmp
	ldr	\rp, =(PB_UART_PHYS_BASE)	@ System peripherals (phys address)
	ldr	\rv, =(PB_UART_VIRT_BASE)	@ System peripherals (virt address)
	.endm

	.macro	senduart,rd,rx
	strb	\rd, [\rx, #(PB_UART_THB)]	@ Write to Transmitter Holding Register
	.endm

	.macro	waituart,rd,rx
1001:	ldr	\rd, [\rx, #(PB_UART_TRSTATUS)]		@ Read Status Register
	tst	\rd, #(PB_UART_FIFO_EMPTY)		@ PB_UART_FIFO_EMPTY = 1 when ready to transmit
	beq	1001b
	.endm

	.macro	busyuart,rd,rx
1001:
	ldr	\rd, [\rx, #(PB_UART_TRSTATUS)]		@ Read Status Register
	tst	\rd, #(PB_UART_TX_EMPTY)	@ PB_UART_TX_EMPTY = 1 when transmission complete
	beq	1001b
	.endm

#if KERNEL_VERSION(2,6,37) > LINUX_VERSION_CODE
ENTRY(printtest)
		ldr	r1, =PB_UART_PHYS_BASE
		ldr	r0, =PB_UART_VIRT_BASE
ENDPROC(printtest)
#endif