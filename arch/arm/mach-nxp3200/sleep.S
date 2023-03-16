/*
 *  Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 *
 *  Use of this source code is subject to the terms of the Microsoft end-user
 *  license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
 *  If you did not accept the terms of the EULA, you are not authorized to use
 *  this source code. For a copy of the EULA, please see the LICENSE.RTF on your
 *  install media.
 *
 ********************************************************************************
 *
 *   File:  speep.s
 *
 *	CPU power management for Nexell ARM11 SoC.
 *
 ********************************************************************************
 */

#include <linux/linkage.h>
#include <asm/assembler.h>

			.text
@-------------------------------------------------------------------------------
@	CP15 register mask
@-------------------------------------------------------------------------------
#define	C1_SBZ_MASK			(0xCC1A0000)		@ c1, Control Register
#define	C1_SBO_MASK			(0x00000070)		@ c1, Control Register
#define	C2_SBZ_MASK			(0x00001FE0)		@ c2, Control Register
												@ (3FE0=16KB/1FE0=8KB/0FE0=4KB/07E0=2KB boundary size of TTB0)

@------------------------------------------------------------------------------
@ General CPU constants
@------------------------------------------------------------------------------
#define	MODE_USR			(0x10)
#define	MODE_FIQ			(0x11)
#define	MODE_IRQ			(0x12)
#define	MODE_SVC			(0x13)
#define	MODE_ABT			(0x17)
#define	MODE_UND			(0x1B)
#define	MODE_SYS			(0x1F)
#define	NOINT				(0xC0)
#define	I_Bit				(0x80)
#define	F_Bit				(0x40)
#define	R1_iA				(1<<31)
#define	R1_nF				(1<<30)
#define	R1_VE				(1<<24)
#define	R1_I				(1<<12)
#define	R1_BP				(1<<11)				@ Z bit
#define	R1_C				(1<<2)
#define	R1_A				(1<<1)
#define	R1_M				(1<<0)

@------------------------------------------------------------------------------
@ Sleep backup data layout
@------------------------------------------------------------------------------
#define	WORD_SIZE			(4)
#define	Sleep_Data_Start	(0)
#define	Sleep_WakeAddr		(Sleep_Data_Start)
#define	Sleep_SYSCTL		(Sleep_WakeAddr		+ WORD_SIZE )
#define	Sleep_MMUTTB0		(Sleep_SYSCTL		+ WORD_SIZE )
#define	Sleep_MMUTTB1		(Sleep_MMUTTB0		+ WORD_SIZE )
#define	Sleep_MMUTTBCTL		(Sleep_MMUTTB1		+ WORD_SIZE )
#define	Sleep_MMUDOMAIN		(Sleep_MMUTTBCTL	+ WORD_SIZE )
#define	Sleep_SVC_SP		(Sleep_MMUDOMAIN	+ WORD_SIZE )
#define	Sleep_SVC_SPSR		(Sleep_SVC_SP		+ WORD_SIZE )
#define	Sleep_FIQ_SPSR		(Sleep_SVC_SPSR		+ WORD_SIZE )
#define	Sleep_FIQ_R8		(Sleep_FIQ_SPSR		+ WORD_SIZE )
#define	Sleep_FIQ_R9		(Sleep_FIQ_R8		+ WORD_SIZE )
#define	Sleep_FIQ_R10		(Sleep_FIQ_R9		+ WORD_SIZE )
#define	Sleep_FIQ_R11		(Sleep_FIQ_R10		+ WORD_SIZE )
#define	Sleep_FIQ_R12		(Sleep_FIQ_R11		+ WORD_SIZE )
#define	Sleep_FIQ_SP		(Sleep_FIQ_R12		+ WORD_SIZE )
#define	Sleep_FIQ_LR		(Sleep_FIQ_SP		+ WORD_SIZE )
#define	Sleep_ABT_SPSR		(Sleep_FIQ_LR		+ WORD_SIZE )
#define	Sleep_ABT_SP		(Sleep_ABT_SPSR		+ WORD_SIZE )
#define	Sleep_ABT_LR		(Sleep_ABT_SP		+ WORD_SIZE )
#define	Sleep_IRQ_SPSR		(Sleep_ABT_LR		+ WORD_SIZE )
#define	Sleep_IRQ_SP		(Sleep_IRQ_SPSR		+ WORD_SIZE )
#define	Sleep_IRQ_LR		(Sleep_IRQ_SP		+ WORD_SIZE )
#define	Sleep_UND_SPSR		(Sleep_IRQ_LR		+ WORD_SIZE )
#define	Sleep_UND_SP		(Sleep_UND_SPSR		+ WORD_SIZE )
#define	Sleep_UND_LR		(Sleep_UND_SP		+ WORD_SIZE )
#define	Sleep_SYS_SP		(Sleep_UND_LR		+ WORD_SIZE )
#define	Sleep_SYS_LR		(Sleep_SYS_SP		+ WORD_SIZE )
#define	Sleep_Data_End     	(Sleep_SYS_LR		+ WORD_SIZE )
#define	Sleep_Data_Size		(Sleep_Data_End - Sleep_Data_Start) / 4

#define	OFFSET_CSUM			0x0
#define	OFFSET_SIZE			0x4
#define	OFFSET_DATA			0x8

		@-------------------------------------
		@ 	MACRO: INVALID_ENTIRE_ICACHE
		@-------------------------------------
		.macro 	INVALID_ENTIRE_ICACHE
   		mov  	r0,#0x0
   		mcr 	p15, 0, r0, c7, c5, 0
   		mcr 	p15, 0, r0, c7, c5, 0
   		mcr 	p15, 0, r0, c7, c5, 0
   		mcr 	p15, 0, r0, c7, c5, 0
        .endm

		@-------------------------------------
		@ 	MACRO: CLR_INVALID_ENTIRE_DCACHE
		@-------------------------------------
		.macro	CLR_INVALID_ENTIRE_DCACHE
   		mov  	r0,#0x0
		mcr		p15, 0, r0, c7, c14, 0
        .endm


@-------------------------------------------------------------------------------
@  Function:  asm_cpu_ready_sleep
@
@  Ready before goto cpu sleep mode
@-------------------------------------------------------------------------------

ENTRY(asm_cpu_ready_sleep)
        @---------------------------------------------------------------
        @ 1. Push SVC state onto our stack
        @---------------------------------------------------------------
		stmdb	sp!, {r0 - r12, lr}

        @---------------------------------------------------------------
        @ 2. Save WakeUp address and CP15 register
        @---------------------------------------------------------------

		ldr		r3, =sleep_virt_base			@ base of Sleep mode data storage
		ldr		r3, [r3]
		add		r3, r3, #OFFSET_DATA			@ data addr = base addr + 0x8

        @ WakeUp address
        @
		ldr		r2, =asm_cpu_restore_reg		@ store Virtual return address
		str		r2, [r3], #4					@ [Sleep_WakeAddr]

        @ c1, Control Register
        @
		mrc		p15, 0, r2, c1, c0, 0			@ load r2 with c1 Control Register
		ldr		r0, =C1_SBZ_MASK				@ Should Be Zero(SBZ) Mask for c1, Control Register
		bic		r2, r2, r0
		ldr		r0, =C1_SBO_MASK				@ Should Be One (SBO) Mask for c1, Control Register
		orr		r2, r2, r0
		str		r2, [r3], #4					@ [Sleep_SYSCTL]

        @ c2, Translation Table Base Register 0
        @
		mrc		p15, 0, r2, c2, c0, 0			@ load r2 with TTB Base Register 0.
		ldr		r0, =C2_SBZ_MASK				@ Should Be Zero(SBZ) Mask for c2, TTB Base Register 0
		bic		r2, r2, r0
		str		r2, [r3], #4					@ [Sleep_MMUTTB0]

        @ c2, Translation Table Base Register 1
        @
		mrc		p15, 0, r2, c2, c0, 1			@ load r2 with TTB Base Register 1
		str		r2, [r3], #4					@ [Sleep_MMUTTB1]

        @ c2, Translation Table Base Control Register
        @
		mrc		p15, 0, r2, c2, c0, 2			@ load r2 with TTB Control Register
		str		r2, [r3], #4					@ [Sleep_MMUTTBCTL]

		@ c3, Domain Access Control Register
        @
		mrc		p15, 0, r2, c3, c0, 0			@ load r2 with Domain Access Control Register
		str		r2, [r3], #4					@ [Sleep_MMUDOMAIN]

        @---------------------------------------------------------------
        @ 3. Save CPU mode registers
        @---------------------------------------------------------------

        @ SVC sp
        @
		str		sp, [r3], #4					@ [Sleep_SVC_SP]

        @ SVC spsr
        @
		mrs		r2, spsr
		str		r2, [r3], #4					@ [Sleep_SVC_SPSR]

		@ FIQ mode
		@
		mov		r1, #MODE_FIQ | NOINT			@ Enter FIQ mode, no interrupts
		msr		cpsr, r1
		mrs		r2, spsr
		stmia	r3!, {r2, r8-r12, sp, lr}		@ [Sleep_FIQ_SPSR ~ Sleep_FIQ_LR]

		@ Abort mode
		@
		mov		r1, #MODE_ABT | NOINT			@ Enter ABT mode, no interrupts
		msr		cpsr, r1
		mrs		r0, spsr
		stmia	r3!, {r0, sp, lr}				@ [Sleep_ABT_SPSR ~ Sleep_ABT_LR]

		@ IRQ mode
		@
		mov		r1, #MODE_IRQ | NOINT			@ Enter IRQ mode, no interrupts
		msr		cpsr, r1
		mrs		r0, spsr
		stmia	r3!, {r0, sp, lr}				@ [Sleep_IRQ_SPSR ~ Sleep_IRQ_LR]

		@ Undefined mode
		@
		mov		r1, #MODE_UND | NOINT			@ Enter UND mode, no interrupts
		msr		cpsr, r1
		mrs		r0, spsr
		stmia	r3!, {r0, sp, lr}				@ [Sleep_UND_SPSR ~ Sleep_UND_LR]

		@ System mode
		@
		mov		r1, #MODE_SYS | NOINT			@ Enter SYS mode, no interrupts
		msr		cpsr, r1
		stmia	r3!, {sp, lr}					@ [Sleep_SYS_SP, Sleep_SYS_LR]

		mov		r1, #MODE_SVC | NOINT			@ Back to SVC mode, no interrupts
		msr		cpsr, r1

        @---------------------------------------------------------------
        @ 4. Do Checksum on the Sleepdata
        @---------------------------------------------------------------
		ldr		r3, =sleep_virt_base			@ get pointer to SLEEPDATA
		ldr		r3, [r3]
		add		r3, r3, #OFFSET_DATA			@ data addr = base addr + 0x8

		ldr		r2, =0x0
		ldr		r0, =(Sleep_Data_Size)			@ get size of data structure (in words)

30:
		ldr		r1, [r3], #4					@ pointer to SLEEPDATA
		eor		r1, r1, #0xAA
		eor		r2, r2, r1
		subs	r0, r0, #1						@ dec the count
		bne		30b

		ldr		r3, =sleep_virt_base
		ldr		r3, [r3]
		add		r3, r3, #OFFSET_SIZE			@ size addr = base addr + 0x4
		ldr		r0, =(Sleep_Data_Size)			@ get size of data structure (in words)
		str		r0, [r3]						@ Store sleep data size

		ldr		r3, =sleep_virt_base			@ checksum addr equal to base addr
		ldr		r3, [r3]
		add		r3, r3, #OFFSET_CSUM			@ checksum addr = base addr + 0x0
		str		r2, [r3]						@ Store CheckSum

        @---------------------------------------------------------------
        @ 5. Cache Flush
        @---------------------------------------------------------------
		INVALID_ENTIRE_ICACHE

		CLR_INVALID_ENTIRE_DCACHE

        @---------------------------------------------------------------
        @ 6. Return
        @---------------------------------------------------------------
        ldmia	sp!, {r0 - r12, lr}				@ restore register

		ldr		r3, =sleep_virt_base
		ldr		r3, [r3]
		add		r3, r3, #OFFSET_DATA			@ data addr = base addr + 0x8

		ldr		sp, [r3, #Sleep_SVC_SP]			@ reset stack point
		subs	sp, sp, #4						@ dec stack point.

		mov     pc, lr


@-------------------------------------------------------------------------------
@  Function:  asm_cpu_wakeup
@
@  Restore mmu status before goto cpu power up
@-------------------------------------------------------------------------------

ENTRY(asm_cpu_wakeup)

        @---------------------------------------------------------------
        @	Switch SVC Mode And Disable Irq/Fiq
        @---------------------------------------------------------------
		mov		r0, #MODE_SVC | NOINT			@ SVC mode, no interrupts
		msr     cpsr_c, r0                      @ switch to supervisor mode and fiq/irq disable.

        @---------------------------------------------------------------
		@	Disable MMU
        @---------------------------------------------------------------
		mov		r1, #0
		mcr		p15, 0, r1, c7, c5, 0			@ Invalidate Entire Instruction Cache
		mcr		p15, 0, r1, c7, c14, 0			@ Clean and Invalidate Entire Data Cache

		mrc		p15, 0, r1, c1, c0, 0
		bic		r1, r1, #0x0005					@ Disable MMU and Data Cache
		mcr		p15, 0, r1, c1, c0, 0

        @---------------------------------------------------------------
		@	Enable ICache
        @---------------------------------------------------------------
		mov		r0, #0
		mcr		p15, 0, r0, c7, c7, 0			@ Invalidate Entire I&D Cache

		mrc		p15, 0, r0, c1, c0, 0			@ read CP15 register 1 into r0
		orr		r0, r0, #R1_I					@ enable I Cache
		mcr		p15, 0, r0, c1, c0, 0			@ write cp15 register 1, enable I Cache

        @---------------------------------------------------------------
		@	Peripheral Port Setup
        @---------------------------------------------------------------
		ldr		r0, =0xC0000013					@ Base Addres : 0xC0000000, Size : 256 MB (0x13), 1 GB(0x15)
		mcr		p15, 0, r0, c15, c2, 4

        @---------------------------------------------------------------
		@	Flush TLB, Invalidate ICache, DCache
        @---------------------------------------------------------------
		mov     r0, #0
		mcr     p15, 0, r0, c8, c7, 0           @ flush both TLB
		mcr     p15, 0, r0, c7, c5, 0           @ invalidate instruction cache
		mcr     p15, 0, r0, c7, c6, 0           @ invalidate data cache

        @---------------------------------------------------------------
		@	Disable VIC
        @---------------------------------------------------------------
		mrc		p15, 0, r0, c1, c0, 0
		bic		r0, r0, #R1_VE
		mcr		p15, 0, r0, c1, c0, 0

        @---------------------------------------------------------------
		@	Enable Branch Prediction
        @---------------------------------------------------------------
		mrc		p15, 0, r0, c1, c0, 0
		orr		r0, r0, #R1_BP
		mcr		p15, 0, r0, c1, c0, 0

        @---------------------------------------------------------------
		@ Restore CP15 Register
        @---------------------------------------------------------------
		ldr		r10, =sleep_phys_base			@ Sleep mode information data structure
		ldr		r10, [r10]
		add		r10, r10, #OFFSET_DATA			@ data addr = base addr + 0x8

		ldr		r6,	[r10, #Sleep_MMUDOMAIN]		@ Domain Access Control Register
		ldr		r5,	[r10, #Sleep_MMUTTBCTL]		@ TTB Control Register
		ldr		r4,	[r10, #Sleep_MMUTTB1]		@ TTB Register1
		ldr		r3,	[r10, #Sleep_MMUTTB0]		@ TTB Register0
		ldr		r2,	[r10, #Sleep_SYSCTL]		@ System Control Register
		ldr		r1,	[r10, #Sleep_WakeAddr]		@ Return Address

		mcr		p15, 0, r6, c3, c0, 0			@ Restore Domain Access Control Register
		mcr		p15, 0, r5, c2, c0, 2			@ Restore TTB Control Register
		mcr		p15, 0, r4, c2, c0, 1			@ Restore TTB Register1
		mcr		p15, 0, r3, c2, c0, 0			@ Restore TTB Register0

		mov		r0, #0x0
		mcr		p15, 0, r0, c8, c7, 0	   		@ Invalidate I & D TLB

		mcr		p15, 0, r2, c1, c0, 0			@ Restore System Control Register (MMU Control)

		nop
		nop
		nop
		nop
		nop

        @---------------------------------------------------------------
		@ go to asm_cpu_restore_reg
        @---------------------------------------------------------------
		mov		pc, r1							@ Jump to Virtual Return Address
		b		.

@-------------------------------------------------------------------------------
@  Function:  asm_cpu_restore_reg
@
@  Restore before goto cpu power up
@-------------------------------------------------------------------------------

ENTRY(asm_cpu_restore_reg)

        @---------------------------------------------------------------
		@ 1. Recover CPU mode registers
        @---------------------------------------------------------------
		ldr		r3, =sleep_virt_base			@ Sleep mode information data structure
		ldr		r3, [r3]
		add		r3, r3, #OFFSET_DATA			@ data addr = base addr + 0x8

		@ FIQ mode CPU Registers
		@
		mov		r1, #MODE_FIQ | NOINT			@ Enter FIQ mode, no interrupts
		msr		cpsr, r1

		ldr		r0,	 [r3, #Sleep_FIQ_SPSR]
		msr		spsr, r0
		ldr		r8,	 [r3, #Sleep_FIQ_R8]
		ldr		r9,	 [r3, #Sleep_FIQ_R9]
		ldr		r10, [r3, #Sleep_FIQ_R10]
		ldr		r11, [r3, #Sleep_FIQ_R11]
		ldr		r12, [r3, #Sleep_FIQ_R12]
		ldr		sp,	 [r3, #Sleep_FIQ_SP]
		ldr		lr,	 [r3, #Sleep_FIQ_LR]

		@ Abort mode CPU Registers
		@
		mov		r1, #MODE_ABT | I_Bit			@ Enter ABT mode, no IRQ - FIQ is available
		msr		cpsr, r1

		ldr		r0,	[r3, #Sleep_ABT_SPSR]
		msr		spsr, r0
		ldr		sp,	[r3, #Sleep_ABT_SP]
		ldr		lr,	[r3, #Sleep_ABT_LR]

		@ IRQ mode CPU Registers
		@
		mov		r1, #MODE_IRQ | I_Bit			@ Enter IRQ mode, no IRQ - FIQ is available
		msr		cpsr, r1

		ldr		r0,	[r3, #Sleep_IRQ_SPSR]
		msr		spsr, r0
		ldr		sp,	[r3, #Sleep_IRQ_SP]
		ldr		lr,	[r3, #Sleep_IRQ_LR]

		@ Undefined mode CPU Registers
		@
		mov		r1, #MODE_UND | I_Bit			@ Enter UND mode, no IRQ - FIQ is available
		msr		cpsr, r1

		ldr		r0,	[r3, #Sleep_UND_SPSR]
		msr		spsr, r0
		ldr		sp,	[r3, #Sleep_UND_SP]
		ldr		lr,	[r3, #Sleep_UND_LR]

		@ System(User) mode CPU Registers
		@
		mov		r1, #MODE_SYS | I_Bit			@ Enter SYS mode, no IRQ - FIQ is available
		msr		cpsr, r1

		ldr		sp,	[r3, #Sleep_SYS_SP]
		ldr		lr,	[r3, #Sleep_SYS_LR]

		@ Supervisor mode CPU Registers
		@
		mov		r1, #MODE_SVC | I_Bit			@ Enter SVC mode, no IRQ - FIQ is available
		msr		cpsr, r1


		ldr		r0, [r3, #Sleep_SVC_SPSR]
		msr		spsr, r0
		ldr		sp, [r3, #Sleep_SVC_SP]

        @---------------------------------------------------------------
		@ 2. Pop SVC Register from our Stack
        @---------------------------------------------------------------
		ldmia	sp!, {r0 - r12, lr}

        @---------------------------------------------------------------
		@ 3. Return to Caller of asm_cpu_ready_sleep()
        @---------------------------------------------------------------
		mov     pc, lr




