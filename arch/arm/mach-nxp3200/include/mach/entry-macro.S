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
#include <asm/unistd.h>
#include <mach/platform.h>

	.macro	disable_fiq
	.endm

	.macro	arch_ret_to_user, tmp1, tmp2
	.endm

	.macro	get_irqnr_preamble, base, tmp
	ldr	\base, =PB_INTC_VIRT_BASE
	.endm

	.macro	get_irqnr_and_base, irqnr, irqpend, base, tmp

	mov		\irqnr, #0x0
	ldr		\base,  =PB_INTC_VIRT_BASE					@ Interrupt Controller Virtual base
	ldr		\irqpend, [\base, #PB_INTC_LPEND_OFFSET]	@ IRQ Pending Low register
	cmp		\irqpend, #0x0
	bne		1001f										@ if Low IRQ Pending occurs, goto 1001 to get irqnr,
														@ else next and check high IRQ Pending.

	ldr		\irqpend, [\base, #PB_INTC_HPEND_OFFSET]	@ IRQ Pending High register
	cmp		\irqpend, #0x0
	beq		1003f										@ Error, no IRQ Pending, goto 1003.
	movne	\irqnr, #32

1001:		/* get IRQ number */
	and		\tmp, \irqpend, #0x1
	cmp		\tmp, #0x1
	addne	\irqnr, \irqnr, #1
	movne	\irqpend, \irqpend, LSR #1					@ IRQ Pend shift right.
	bne		1001b

1003:
	cmp 	\irqpend, #0x0
	.endm
