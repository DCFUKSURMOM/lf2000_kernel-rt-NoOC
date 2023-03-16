;-------------------------------------------------------------------------------
;
;	Copyright (C) 2009 Nexell Co., All Rights Reserved
;	Nexell Co. Proprietary & Confidential
;
;	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
;	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
;	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
;	FOR A PARTICULAR PURPOSE.
;
;	Module		: Bug turn around code
;	File		: TurnArround.s
;	Description	: ARM assembly functions
;	Author		: Hans
;	History		:
;		2010-10-05	Hans	create
;-------------------------------------------------------------------------------

	PRESERVE8	; 8-byte alignment of the stack for RVDS
	AREA	TURNAROUND, CODE, READONLY

	ENTRY

;-------------------------------------------------------------------------------
;	current program status register mode definition
;-------------------------------------------------------------------------------
I_Bit		EQU		0x80		; when I bit is set, IRQ is disabled

;-------------------------------------------------------------------------------
;	void	WriteIODW( Addr, Data )
;-------------------------------------------------------------------------------
		ALIGN
		EXPORT	WriteIODW

	;R0:IOA, R1:IOD, R2:UCA, R3:CPSR, R4:TMP
WriteIODW	PROC
	PUSH	{R2-R4}

	MOV		R2, #0x18000000;

	MRS		R3, CPSR			; read interrupt enable status
	ORR		R4, R3, #I_Bit		; clear interrupt enable bit
	MSR		CPSR_cxsf, R4		; save and interrupt disable

	LDRB	R4, [R2]			; before memory bus read
	STRB	R4, [R2]			; before memory bus write

	STR		R1, [R0]			; write io data

	LDRB	R4, [R2]			; after memory bus read

	MSR		CPSR_cxsf, R3		; restore CPSR

	POP		{R2-R4}

	BX		LR

	ENDP

;-------------------------------------------------------------------------------
;	void	WriteIOW( Addr, Data )
;-------------------------------------------------------------------------------
		ALIGN
		EXPORT	WriteIOW

	;R0:IOA, R1:IOD, R2:UCA, R3:CPSR, R4:TMP
WriteIOW	PROC
	PUSH	{R2-R4}

	MOV		R2, #0x18000000;

	MRS		R3, CPSR			; read interrupt enable status
	ORR		R4, R3, #I_Bit		; clear interrupt enable bit
	MSR		CPSR_cxsf, R4		; save and interrupt disable

	LDRB	R4, [R2]			; before memory bus read
	STRB	R4, [R2]			; before memory bus write

	STRH	R1, [R0]			; write io data

	LDRB	R4, [R2]			; after memory bus read

	MSR		CPSR_cxsf, R3		; restore CPSR

	POP		{R2-R4}

	BX		LR

	ENDP

;-------------------------------------------------------------------------------
;	void	WriteIOB( Addr, Data )
;-------------------------------------------------------------------------------
		ALIGN
		EXPORT	WriteIOB

	;R0:IOA, R1:IOD, R2:UCA, R3:CPSR, R4:TMP
WriteIOB	PROC
	PUSH	{R2-R4}

	MOV		R2, #0x18000000;

	MRS		R3, CPSR			; read interrupt enable status
	ORR		R4, R3, #I_Bit		; clear interrupt enable bit
	MSR		CPSR_cxsf, R4		; save and interrupt disable

	LDRB	R4, [R2]			; before memory bus read
	STRB	R4, [R2]			; before memory bus write

	STRB	R1, [R0]			; write io data

	LDRB	R4, [R2]			; after memory bus read

	MSR		CPSR_cxsf, R3		; restore CPSR

	POP		{R2-R4}

	BX		LR

	ENDP

	END
