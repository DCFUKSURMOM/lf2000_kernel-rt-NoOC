@-------------------------------------------------------------------------------
@
@	Copyright (C) 2009 Nexell Co., All Rights Reserved
@	Nexell Co. Proprietary & Confidential
@
@	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
@	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
@	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
@	FOR A PARTICULAR PURPOSE.
@
@	Module		: Bug turn arround code
@	File		: TurnArround.s
@	Description	: ARM assembly functions
@	Author		: Hans
@	History		:
@		2010-10-05	Hans	create
@-------------------------------------------------------------------------------

#include <linux/linkage.h>
#include <asm/assembler.h>

@-------------------------------------------------------------------------------
@	current program status register mode definition
@-------------------------------------------------------------------------------
#define	I_Bit			(0x80)			@ when I bit is set, IRQ is disabled

#define	UC_MEM_BASE		(0xF0000000)
#define	UC_MEM_OFFS		(0x00900000)	@ SRAM: 0x00900000,	DRAM: 0x00B00000

#define	ST_MEM_BASE		(0xF0000000)
#define	ST_MEM_OFFS		(0x00900000)

@-------------------------------------------------------------------------------
@	void	WriteIODW( Addr, Data )
@-------------------------------------------------------------------------------

	ALIGN
	ENTRY(WriteIODW)

	@ R0:IOA, R1:IOD, R2:UCA, R3:CPSR, R4:TMP
	PUSH	{R2-R4}

	MOV		R2, #UC_MEM_BASE
	ORR		R2, R2, #UC_MEM_OFFS

	MRS		R3, CPSR			@ read interrupt enable status
	ORR		R4, R3, #I_Bit		@ clear interrupt enable bit
	MSR		CPSR_cxsf, R4		@ save and interrupt disable

	STRB	R4, [R2]			@ before memory write
	LDRB	R4, [R2]			@ before memory read

	STR		R1, [R0]			@ write io data

	LDRB	R4, [R2]			@ after memory read

	MSR		CPSR_cxsf, R3		@ restore CPSR

	POP		{R2-R4}

	BX		LR

@-------------------------------------------------------------------------------
@	void	WriteIOW( Addr, Data )
@-------------------------------------------------------------------------------
	ALIGN
	ENTRY(WriteIOW)

	@ R0:IOA, R1:IOD, R2:UCA, R3:CPSR, R4:TMP

	PUSH	{R2-R4}

	MOV		R2, #UC_MEM_BASE
	ORR		R2, R2, #UC_MEM_OFFS

	MRS		R3, CPSR			@ read interrupt enable status
	ORR		R4, R3, #I_Bit		@ clear interrupt enable bit
	MSR		CPSR_cxsf, R4		@ save and interrupt disable

	STRB	R4, [R2]			@ before memory write
	LDRB	R4, [R2]			@ before memory read

	STRH	R1, [R0]			@ write io data

	LDRB	R4, [R2]			@ after memory read

	MSR		CPSR_cxsf, R3		@ restore CPSR

	POP		{R2-R4}

	BX		LR

@-------------------------------------------------------------------------------
@	void	WriteIOB( Addr, Data )
@-------------------------------------------------------------------------------
	ALIGN
	ENTRY(WriteIOB)

	@ R0:IOA, R1:IOD, R2:UCA, R3:CPSR, R4:TMP

	PUSH	{R2-R4}

	MOV		R2, #UC_MEM_BASE
	ORR		R2, R2, #UC_MEM_OFFS

	MRS		R3, CPSR			@ read interrupt enable status
	ORR		R4, R3, #I_Bit		@ clear interrupt enable bit
	MSR		CPSR_cxsf, R4		@ save and interrupt disable

	STRB	R4, [R2]			@ before memory write
	LDRB	R4, [R2]			@ before memory read

	STRB	R1, [R0]			@ write io data

	LDRB	R4, [R2]			@ after memory read

	MSR		CPSR_cxsf, R3		@ restore CPSR

	POP		{R2-R4}

	BX		LR

