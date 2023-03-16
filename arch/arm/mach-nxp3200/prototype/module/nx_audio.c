//------------------------------------------------------------------------------
//
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		:
//	File		: nx_audio.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_audio.h"

static	struct NX_AUDIO_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return	\b CTRUE	indicates that Initialize is successed.\n
 *			\b CFALSE	indicates that Initialize is failed.\n
 *	@see	NX_AUDIO_GetNumberOfModule
 */
CBOOL	NX_AUDIO_Initialize( void )
{
	static CBOOL bInit = CFALSE;

	if( CFALSE == bInit )
	{
		__g_pRegister = CNULL;

		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 *	@see		NX_AUDIO_Initialize
 */
U32		NX_AUDIO_GetNumberOfModule( void )
{
	return NUMBER_OF_AUDIO_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_AUDIO_GetSizeOfRegisterSet,
 *				NX_AUDIO_SetBaseAddress,		NX_AUDIO_GetBaseAddress,
 *				NX_AUDIO_OpenModule,			NX_AUDIO_CloseModule,
 *				NX_AUDIO_CheckBusy,				NX_AUDIO_CanPowerDown
 */
U32		NX_AUDIO_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_AUDIO_MODULE );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_AUDIO_GetPhysicalAddress,
 *				NX_AUDIO_SetBaseAddress,		NX_AUDIO_GetBaseAddress,
 *				NX_AUDIO_OpenModule,			NX_AUDIO_CloseModule,
 *				NX_AUDIO_CheckBusy,				NX_AUDIO_CanPowerDown
 */
U32		NX_AUDIO_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_AUDIO_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_AUDIO_GetPhysicalAddress,	NX_AUDIO_GetSizeOfRegisterSet,
 *				NX_AUDIO_GetBaseAddress,
 *				NX_AUDIO_OpenModule,			NX_AUDIO_CloseModule,
 *				NX_AUDIO_CheckBusy,				NX_AUDIO_CanPowerDown
 */
void	NX_AUDIO_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_AUDIO_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_AUDIO_GetPhysicalAddress,	NX_AUDIO_GetSizeOfRegisterSet,
 *				NX_AUDIO_SetBaseAddress,
 *				NX_AUDIO_OpenModule,			NX_AUDIO_CloseModule,
 *				NX_AUDIO_CheckBusy,				NX_AUDIO_CanPowerDown
 */
U32		NX_AUDIO_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicates that Initialize is successed. \n
 *				\b CFALSE indicates that Initialize is failed.
 *	@see		NX_AUDIO_GetPhysicalAddress,	NX_AUDIO_GetSizeOfRegisterSet,
 *				NX_AUDIO_SetBaseAddress,		NX_AUDIO_GetBaseAddress,
 *				NX_AUDIO_CloseModule,
 *				NX_AUDIO_CheckBusy,				NX_AUDIO_CanPowerDown
 */
CBOOL	NX_AUDIO_OpenModule( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->AC97_CTRL			= 0x00;
//	__g_pRegister->AC97_CONFIG			= 0x00;
//	__g_pRegister->I2S_CTRL				= 0x00;
//	__g_pRegister->I2S_CONFIG			= 0x00;
//	__g_pRegister->AUDIO_BUFF_CTRL		= 0x00;
//	__g_pRegister->AUDIO_BUFF_CONFIG	= 0x00;
//	__g_pRegister->AUDIO_IRQ_ENA		= 0x00;
//	__g_pRegister->AUDIO_IRQ_PEND		= 0xFF;

	WriteIOW(&__g_pRegister->AC97_CTRL, 0x00);
	WriteIOW(&__g_pRegister->AC97_CONFIG, 0x00);
	WriteIOW(&__g_pRegister->I2S_CTRL, 0x00);
	WriteIOW(&__g_pRegister->I2S_CONFIG, 0x00);
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CTRL, 0x00);
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CONFIG, 0x00);
	WriteIOW(&__g_pRegister->AUDIO_IRQ_ENA, 0x00);
	WriteIOW(&__g_pRegister->AUDIO_IRQ_PEND, 0xFF);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicates that Deinitialize is successed. \n
 *				\b CFALSE indicates that Deinitialize is failed.
 *	@see		NX_AUDIO_GetPhysicalAddress,	NX_AUDIO_GetSizeOfRegisterSet,
 *				NX_AUDIO_SetBaseAddress,		NX_AUDIO_GetBaseAddress,
 *				NX_AUDIO_OpenModule,
 *				NX_AUDIO_CheckBusy,				NX_AUDIO_CanPowerDown
 */
CBOOL	NX_AUDIO_CloseModule( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->AC97_CTRL			= 0x00;
//	__g_pRegister->AC97_CONFIG			= 0x00;
//	__g_pRegister->I2S_CTRL				= 0x00;
//	__g_pRegister->I2S_CONFIG			= 0x00;
//	__g_pRegister->AUDIO_BUFF_CTRL		= 0x00;
//	__g_pRegister->AUDIO_BUFF_CONFIG	= 0x00;
//	__g_pRegister->AUDIO_IRQ_ENA		= 0x00;
//	__g_pRegister->AUDIO_IRQ_PEND		= 0xFF;

	WriteIOW(&__g_pRegister->AC97_CTRL, 0x00);
	WriteIOW(&__g_pRegister->AC97_CONFIG, 0x00);
	WriteIOW(&__g_pRegister->I2S_CTRL, 0x00);
	WriteIOW(&__g_pRegister->I2S_CONFIG, 0x00);
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CTRL, 0x00);
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CONFIG, 0x00);
	WriteIOW(&__g_pRegister->AUDIO_IRQ_ENA, 0x00);
	WriteIOW(&__g_pRegister->AUDIO_IRQ_PEND, 0xFF);
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicates that Module is Busy. \n
 *				\b CFALSE indicates that Module is NOT Busy.
 *	@see		NX_AUDIO_GetPhysicalAddress,	NX_AUDIO_GetSizeOfRegisterSet,
 *				NX_AUDIO_SetBaseAddress,		NX_AUDIO_GetBaseAddress,
 *				NX_AUDIO_OpenModule,			NX_AUDIO_CloseModule,
 *				NX_AUDIO_CanPowerDown
 */
CBOOL	NX_AUDIO_CheckBusy( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	if( CTRUE == NX_AUDIO_IsAC97CodecBusy() )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicates that Ready to enter power-down stage. \n
 *				\b CFALSE indicates that This module can't enter to power-down stage.
 *	@see		NX_AUDIO_GetPhysicalAddress,	NX_AUDIO_GetSizeOfRegisterSet,
 *				NX_AUDIO_SetBaseAddress,		NX_AUDIO_GetBaseAddress,
 *				NX_AUDIO_OpenModule,			NX_AUDIO_CloseModule,
 *				NX_AUDIO_CheckBusy
 */
CBOOL	NX_AUDIO_CanPowerDown( void )
{
	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see											NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
S32		NX_AUDIO_GetInterruptNumber( void )
{
	return	INTNUM_OF_AUDIO_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0: PCM Out Buffer Underflow,	1: SPDIF Out Buffer Underflow,
 *											2: PCM IN Buffer Overflow,		3: MIC IN Buffer Overflow,
 *											4: ADC1 IN Buffer Overflow ,	5: ADC2 IN Buffer Overflow,
 *											6: AC97 Codec Read Done ).
 *	@param[in]	Enable	\b CTRUE	indicates that Interrupt Enable. \n
 *						\b CFALSE indicates that Interrupt Disable.
 *	@return		None.
 *	@see		NX_AUDIO_GetInterruptNumber,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
void	NX_AUDIO_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	register U32 ReadValue;

	NX_ASSERT( (7>IntNum) || (0<=IntNum) );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->AUDIO_IRQ_ENA;

	ReadValue &= ~( 1UL << IntNum );
	ReadValue |= ( (U32)Enable << (IntNum) );

//	__g_pRegister->AUDIO_IRQ_ENA = ReadValue;
	WriteIOW(&__g_pRegister->AUDIO_IRQ_ENA, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number ( 0: PCM Out Buffer Underflow,	1: SPDIF Out Buffer Underflow,
 *											2: PCM IN Buffer Overflow,	3: MIC IN Buffer Overflow,
 *											4: ADC1 IN Buffer Overflow ,	5: ADC2 IN Buffer Overflow,
 *											6: AC97 Codec Read Done ).
 *	@return		\b CTRUE	indicate that Interrupt is enabled. \n
 *				\b CFALSE indicate that Interrupt is disabled.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *													NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
CBOOL	NX_AUDIO_GetInterruptEnable( S32 IntNum )
{
	NX_ASSERT( (7 > IntNum) && (0 <= IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->AUDIO_IRQ_ENA) >> (IntNum)) & 0x01;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set PCM Out Buffer Underflow	interrupt enable or disable. \n
 *							- EnableFlag[1] : Set SPDIF Out Buffer Underflow interrupt enable or disable. \n
 *							- EnableFlag[2] : Set PCM IN Buffer Overflow		interrupt enable or disable. \n
 *							- EnableFlag[3] : Set MIC IN Buffer Overflow		interrupt enable or disable. \n
 *							- EnableFlag[4] : Set ADC1 IN Buffer Overflow		interrupt enable or disable. \n
 *							- EnableFlag[5] : Set ADC2 IN Buffer Overflow		interrupt enable or disable. \n
 *							- EnableFlag[6] : Set AC97 Codec Read Done			interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
void	NX_AUDIO_SetInterruptEnable32( U32 EnableFlag )
{
	const U32	ENB_MASK = (1<<7)-1 ;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->AUDIO_IRQ_ENA = EnableFlag & ENB_MASK;
	WriteIOW(&__g_pRegister->AUDIO_IRQ_ENA, EnableFlag & ENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : PCM Out Buffer Underflow	interrupt's setting value. \n
 *				- Return Value[1] : SPDIF Out Buffer Underflow interrupt's setting value. \n
 *				- Return Value[2] : PCM IN Buffer Overflow		interrupt's setting value. \n
 *				- Return Value[3] : MIC IN Buffer Overflow		interrupt's setting value. \n
 *				- Return Value[4] : ADC1 IN Buffer Overflow	interrupt's setting value. \n
 *				- Return Value[5] : ADC2 IN Buffer Overflow	interrupt's setting value. \n
 *				- Return Value[6] : AC97 Codec Read Done		interrupt's setting value. \n
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *													NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
U32		NX_AUDIO_GetInterruptEnable32( void )
{
	const U32	ENB_MASK = (1<<7)-1 ;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->AUDIO_IRQ_ENA & ENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number( 0: PCM Out Buffer Underflow,	1: SPDIF Out Buffer Underflow,
 *										2: PCM IN Buffer Overflow,		3: MIC IN Buffer Overflow,
 *										4: ADC1 IN Buffer Overflow ,	5: ADC2 IN Buffer Overflow,
 *										6: AC97 Codec Read Done ).
 *	@return		\b CTRUE	indicate that Pending is seted. \n
 *				\b CFALSE indicate that Pending is Not Seted.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
CBOOL	NX_AUDIO_GetInterruptPending( S32 IntNum )
{
	NX_ASSERT( (7 > IntNum) && (0 <= IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->AUDIO_IRQ_PEND) >> (IntNum)) & 0x01;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : PCM Out Buffer Underflow	pending state. \n
 *				- Return Value[1] : SPDIF Out Buffer Underflow pending state. \n
 *				- Return Value[2] : PCM IN Buffer Overflow		pending state. \n
 *				- Return Value[3] : MIC IN Buffer Overflow		pending state. \n
 *				- Return Value[4] : ADC1 IN Buffer Overflow	pending state. \n
 *				- Return Value[5] : ADC2 IN Buffer Overflow	pending state. \n
 *				- Return Value[6] : AC97 Codec Read Done		pending state. \n
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *													NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
U32		NX_AUDIO_GetInterruptPending32( void )
{
	const U32	PEND_MASK	=	(1<<7)-1;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->AUDIO_IRQ_PEND & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number( 0: PCM Out Buffer Underflow,	1: SPDIF Out Buffer Underflow,
 *										2: PCM IN Buffer Overflow,		3: MIC IN Buffer Overflow,
 *										4: ADC1 IN Buffer Overflow ,	5: ADC2 IN Buffer Overflow,
 *										6: AC97 Codec Read Done ).
 *	@return		None.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
void	NX_AUDIO_ClearInterruptPending( S32 IntNum )
{
	NX_ASSERT( (7 > IntNum) && (0 <= IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->AUDIO_IRQ_PEND = (U16)(1UL << (IntNum)) ;
	WriteIOW(&__g_pRegister->AUDIO_IRQ_PEND, (U16)(1UL << (IntNum))) ;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : PCM Out Buffer Underflow	pending bit. \n
 *								- PendingFlag[1] : SPDIF Out Buffer Underflow pending bit. \n
 *								- PendingFlag[2] : PCM IN Buffer Overflow		pending bit. \n
 *								- PendingFlag[3] : MIC IN Buffer Overflow		pending bit. \n
 *								- PendingFlag[4] : ADC1 IN Buffer Overflow	pending bit. \n
 *								- PendingFlag[5] : ADC2 IN Buffer Overflow	pending bit. \n
 *								- PendingFlag[6] : AC97 Codec Read Done		pending bit. \n
 *	@return		None.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *													NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
void	NX_AUDIO_ClearInterruptPending32( U32 PendingFlag )
{
	const U32	PEND_MASK	=	(1<<7)-1;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->AUDIO_IRQ_PEND = (U16)(PendingFlag & PEND_MASK);
	WriteIOW(&__g_pRegister->AUDIO_IRQ_PEND, (U16)(PendingFlag & PEND_MASK));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicate that Set to all interrupt enable. \n
 *						\b CFALSE indicate that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
void	NX_AUDIO_SetInterruptEnableAll( CBOOL Enable )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	if( Enable ){ __g_pRegister->AUDIO_IRQ_ENA = 0x07F;	}
//	else		{ __g_pRegister->AUDIO_IRQ_ENA = 0;	}
	if( Enable ){ WriteIOW(&__g_pRegister->AUDIO_IRQ_ENA, 0x07F);	}
	else		{ WriteIOW(&__g_pRegister->AUDIO_IRQ_ENA, 0);	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicate that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE indicate that All interrupt is disabled.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *													NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
CBOOL	NX_AUDIO_GetInterruptEnableAll( void )
{
	const U32 IRQENA_MASK = 0x7F;
	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_IRQ_ENA & IRQENA_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicate that At least one( or more ) pending is seted. \n
 *				\b CFALSE indicate that All pending is NOT seted.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,
 *				NX_AUDIO_ClearInterruptPendingAll,	NX_AUDIO_GetInterruptPendingNumber
 */
CBOOL	NX_AUDIO_GetInterruptPendingAll( void )
{
	const U32 IRQPEND_MASK = 0x7F;
	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_IRQ_PEND & IRQPEND_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *													NX_AUDIO_GetInterruptPendingNumber
 */
void	NX_AUDIO_ClearInterruptPendingAll( void )
{
	const U32 IRQPEND_MASK = 0x7F;
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->AUDIO_IRQ_PEND = IRQPEND_MASK;
	WriteIOW(&__g_pRegister->AUDIO_IRQ_PEND, IRQPEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).\n
 *				0: PCM Out Buffer Underflow,	1: SPDIF Out Buffer Underflow,
 *				2: PCM IN Buffer Overflow,		3: MIC IN Buffer Overflow,
 *				4: ADC1 IN Buffer Overflow ,	5: ADC2 IN Buffer Overflow,
 *				6: AC97 Codec Read Done
 *	@see		NX_AUDIO_GetInterruptNumber,		NX_AUDIO_SetInterruptEnable,
 *				NX_AUDIO_GetInterruptEnable,		NX_AUDIO_SetInterruptEnable32,
 *				NX_AUDIO_GetInterruptEnable32,		NX_AUDIO_GetInterruptPending,
 *				NX_AUDIO_GetInterruptPending32,		NX_AUDIO_ClearInterruptPending,
 *				NX_AUDIO_ClearInterruptPending32,	NX_AUDIO_SetInterruptEnableAll,
 *				NX_AUDIO_GetInterruptEnableAll,		NX_AUDIO_GetInterruptPendingAll,
 *				NX_AUDIO_ClearInterruptPendingAll
 */
S32		NX_AUDIO_GetInterruptPendingNumber( void )	// -1 if None
{
	register struct NX_AUDIO_RegisterSet *pRegister;
	register U16 Pend;
	U32 intnum;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;
	Pend = ( pRegister->AUDIO_IRQ_ENA & pRegister->AUDIO_IRQ_PEND );

	for( intnum=0; intnum < 7; intnum++ )
	{
		if( Pend & (1<<intnum ) )
		{
			return intnum;
		}
	}

	return -1;
}

//------------------------------------------------------------------------------
// DMA Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Get DMA peripheral index of pcm output
 *	@return DMA peripheral index of audio controller's pcm output
 *	@see									NX_AUDIO_GetDMAIndex_SPDIFOut,
 *			NX_AUDIO_GetDMAIndex_PCMIn,		NX_AUDIO_GetDMAIndex_MICIn,
 *			NX_AUDIO_GetDMAIndex_ADC1In,	NX_AUDIO_GetDMAIndex_ADC2In,
 *			NX_AUDIO_GetDMABusWidth
 */
U32		NX_AUDIO_GetDMAIndex_PCMOut( void )
{
	return DMAINDEX_OF_AUDIO_MODULE_PCMOUT;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get DMA peripheral index of SPDIF output
 *	@return DMA peripheral index of audio controller's SPDIF output
 *	@see	NX_AUDIO_GetDMAIndex_PCMOut,
 *			NX_AUDIO_GetDMAIndex_PCMIn,		NX_AUDIO_GetDMAIndex_MICIn,
 *			NX_AUDIO_GetDMAIndex_ADC1In,	NX_AUDIO_GetDMAIndex_ADC2In,
 *			NX_AUDIO_GetDMABusWidth
 */
U32		NX_AUDIO_GetDMAIndex_SPDIFOut( void )
{
	return DMAINDEX_OF_AUDIO_MODULE_SPDIFOUT;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get DMA peripheral index of audio controller's pcm input
 *	@return DMA peripheral index of audio controller's pcm input
 *	@see	NX_AUDIO_GetDMAIndex_PCMOut,	NX_AUDIO_GetDMAIndex_SPDIFOut,
 *											NX_AUDIO_GetDMAIndex_MICIn,
 *			NX_AUDIO_GetDMAIndex_ADC1In,	NX_AUDIO_GetDMAIndex_ADC2In,
 *			NX_AUDIO_GetDMABusWidth
 */
U32		NX_AUDIO_GetDMAIndex_PCMIn( void )
{
	return DMAINDEX_OF_AUDIO_MODULE_PCMIN;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get DMA peripheral index of audio controller's MIC input
 *	@return DMA peripheral index of audio controller's MIC input
 *	@see	NX_AUDIO_GetDMAIndex_PCMOut,	NX_AUDIO_GetDMAIndex_SPDIFOut,
 *			NX_AUDIO_GetDMAIndex_PCMIn,
 *			NX_AUDIO_GetDMAIndex_ADC1In,	NX_AUDIO_GetDMAIndex_ADC2In,
 *			NX_AUDIO_GetDMABusWidth
 */
U32		NX_AUDIO_GetDMAIndex_MICIn( void )
{
	return DMAINDEX_OF_AUDIO_MODULE_MICIN;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get DMA peripheral index of audio controller's ADC1 input
 *	@return DMA peripheral index of audio controller's ADC1 input
 *	@see	NX_AUDIO_GetDMAIndex_PCMOut,	NX_AUDIO_GetDMAIndex_SPDIFOut,
 *			NX_AUDIO_GetDMAIndex_PCMIn,		NX_AUDIO_GetDMAIndex_MICIn,
 *											NX_AUDIO_GetDMAIndex_ADC2In,
 *			NX_AUDIO_GetDMABusWidth
 */
U32		NX_AUDIO_GetDMAIndex_ADC1In( void )
{
	return DMAINDEX_OF_AUDIO_MODULE_ADC1IN;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get DMA peripheral index of audio controller's ADC2 input
 *	@return DMA peripheral index of audio controller's ADC2 input
 *	@see	NX_AUDIO_GetDMAIndex_PCMOut,	NX_AUDIO_GetDMAIndex_SPDIFOut,
 *			NX_AUDIO_GetDMAIndex_PCMIn,		NX_AUDIO_GetDMAIndex_MICIn,
 *			NX_AUDIO_GetDMAIndex_ADC1In,
 *			NX_AUDIO_GetDMABusWidth
 */
U32		NX_AUDIO_GetDMAIndex_ADC2In( void )
{
	return DMAINDEX_OF_AUDIO_MODULE_ADC2IN;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get bus width of audio controller
 *	@return		DMA bus width of audio controller.
 *	@see		NX_AUDIO_GetDMAIndex_PCMOut,	NX_AUDIO_GetDMAIndex_SPDIFOut,
 *				NX_AUDIO_GetDMAIndex_PCMIn,		NX_AUDIO_GetDMAIndex_MICIn,
 *				NX_AUDIO_GetDMAIndex_ADC1In,	NX_AUDIO_GetDMAIndex_ADC2In
 */
U32		NX_AUDIO_GetDMABusWidth( void )
{
	return 16;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@remarks	Audio controller only support NX_PCLKMODE_ALWAYS.\n
 *				If user set to NX_PCLKMODE_DYNAMIC, then audio controller \b NOT operate.
 *	@see		NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
void	NX_AUDIO_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_AUDIO_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:		clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:		clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->CLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//	pRegister->CLKENB = regvalue;
	WriteIODW(&pRegister->CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@return		Current PCLK mode
 *	@see		NX_AUDIO_SetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_AUDIO_GetClockPClkMode( void )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->CLKENB & ( 1UL << PCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock source of clock generator
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@param[in]	ClkSrc	Select clock source of clock generator.\n
 *						0:PLL0, 1:PLL1, 2:None, 3:Bit Clock, 4: Invert Bit Clock\n
 *						5:AV Clock, 6:Invert AV Clock, 7: CLKGEN0's output clock ( Only clock generator1 use ).
 *	@remarks	Audio controller have two clock generator. so \e Index must set to 0 or 1.
 *	@return		None.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
void	NX_AUDIO_SetClockSource( U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS	= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register U32 ReadValue;

	NX_ASSERT( 2 > Index );
	NX_ASSERT( (0!=Index) || ( (2!=ClkSrc) && (ClkSrc<=6) ) );
	NX_ASSERT( (1!=Index) || ( (2!=ClkSrc) && (ClkSrc<=7) ) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->CLKGEN[Index][0];

	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;

//	__g_pRegister->CLKGEN[Index][0] = ReadValue;

	WriteIODW(&__g_pRegister->CLKGEN[Index][0], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock source of specified clock generator.
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@return		Clock source of clock generator \n
 *				0:PLL0, 1:PLL1, 2:None, 3:Bit Clock, 4: Invert Bit Clock\n
 *				5:AV Clock, 6:Invert AV Clock, 7: CLKGEN0's output clock
 *	@remarks	Audio controller have two clock generator. so \e Index must set to 0 or 1.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
U32		NX_AUDIO_GetClockSource( U32 Index )
{
	const U32 CLKSRCSEL_POS	= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	NX_ASSERT( 2 > Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return ( __g_pRegister->CLKGEN[Index][0] & CLKSRCSEL_MASK ) >> CLKSRCSEL_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock divisor of specified clock generator.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 256 ).
 *	@return		None.
 *	@remarks	Audio controller have two clock generator. so \e Index must set to 0 or 1.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
void			NX_AUDIO_SetClockDivisor( U32 Index, U32 Divisor )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	register U32 ReadValue;

	NX_ASSERT( 2 > Index );
	NX_ASSERT( 1 <= Divisor && Divisor <= 256 );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->CLKGEN[Index][0];

	ReadValue	&= ~CLKDIV_MASK;
	ReadValue	|= (Divisor-1) << CLKDIV_POS;

//	__g_pRegister->CLKGEN[Index][0] = ReadValue;
	WriteIODW(&__g_pRegister->CLKGEN[Index][0], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock divisor of specified clock generator.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@return		Clock divisor ( 1 ~ 256 ).
 *	@remarks	Audio controller have two clock generator. so \e Index must set to 0 or 1.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
U32		NX_AUDIO_GetClockDivisor( U32 Index )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	NX_ASSERT( 2 > Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->CLKGEN[Index][0] & CLKDIV_MASK) >> CLKDIV_POS) + 1;

}

//------------------------------------------------------------------------------
/**
 *	@brief		Set inverting of output clock
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0, 1: clock generator1 );
 *	@param[in]	OutClkInv	\b CTRUE indicate that Output clock Invert.\n
 *							\b CFALSE indicate that Output clock Normal.
 *	@return		None.
 *	@remarks	Audio controller have two clock generator. so \e Index must set to 0 or 1.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
void	NX_AUDIO_SetClockOutInv( U32 Index, CBOOL OutClkInv )
{
	const U32 OUTCLKINV_POS	=	1;
	const U32 OUTCLKINV_MASK	=	1UL << OUTCLKINV_POS;

	register U32 ReadValue;

	NX_ASSERT( 2 > Index );
	NX_ASSERT( (0==OutClkInv) ||(1==OutClkInv) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->CLKGEN[Index][0];

	ReadValue	&=	~OUTCLKINV_MASK;
	ReadValue	|=	OutClkInv << OUTCLKINV_POS;

	__g_pRegister->CLKGEN[Index][0]	=	ReadValue;
	WriteIODW(&__g_pRegister->CLKGEN[Index][0], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get invert status of output clock.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0, 1: clock generator1 )
 *	@return		\b CTRUE indicate that Output clock is Inverted. \n
 *				\b CFALSE indicate that Output clock is Normal.
 *	@remarks	Audio controller have two clock generator. so \e Index must set to 0 or 1.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
CBOOL			NX_AUDIO_GetClockOutInv( U32 Index )
{
	const U32 OUTCLKINV_POS		=	1;
	const U32 OUTCLKINV_MASK	=	1UL << OUTCLKINV_POS;

	NX_ASSERT( 2 > Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->CLKGEN[Index][0] & OUTCLKINV_MASK ) >> OUTCLKINV_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock output pin's direction.
 *	@param[in]	Index		Select clock generator( 1: clock generator1 );
 *	@param[in]	OutClkEnb	\b CTRUE indicate that Pin's direction is Output.\n
 *							\b CFALSE indicate that Pin's direction is Iutput.
 *	@return		None.
 *	@remarks	Decides the I/O direction when the output clock is connected to a bidirectional PAD.\n
 *				Only clock generator 1 can set the output pin's direction.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
void	NX_AUDIO_SetClockOutEnb( U32 Index, CBOOL OutClkEnb )
{
	const U32	OUTCLKENB1_POS	=	15;
	const U32	OUTCLKENB1_MASK =	1UL << OUTCLKENB1_POS;

	register U32 ReadValue;

	NX_ASSERT( 1 == Index );
	NX_ASSERT( (0==OutClkEnb) ||(1==OutClkEnb) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->CLKGEN[Index][0];

	ReadValue	&=	~OUTCLKENB1_MASK;

	if( ! OutClkEnb )
	{
		ReadValue	|=	1UL << OUTCLKENB1_POS;
	}

//	__g_pRegister->CLKGEN[Index][0] = ReadValue;
	WriteIODW(&__g_pRegister->CLKGEN[Index][0], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock output pin's direction.
 *	@param[in]	Index		Select clock generator( 1: clock generator1 );
 *	@return		\b CTRUE indicate that Pin's direction is Output.\n
 *				\b CFALSE indicate that Pin's direction is Input.
 *	@remarks	Only clock generator 1 can set the output pin's direction. so \e Index must set to 1.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable,		NX_AUDIO_GetClockDivisorEnable
 */
CBOOL	NX_AUDIO_GetClockOutEnb( U32 Index )
{
	const U32	OUTCLKENB1_POS	=	15;
	const U32	OUTCLKENB1_MASK =	1UL << OUTCLKENB1_POS;

	NX_ASSERT( 1 == Index );
	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->CLKGEN[Index][0] & OUTCLKENB1_MASK )
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock generator's operation
 *	@param[in]	Enable	\b CTRUE indicate that Enable of clock generator. \n
 *						\b CFALSE indicate that Disable of clock generator.
 *	@return		None.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_GetClockDivisorEnable
 */
void	NX_AUDIO_SetClockDivisorEnable( CBOOL Enable )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->CLKENB;

	ReadValue	&=	~CLKGENENB_MASK;
	ReadValue	|= (U32)Enable << CLKGENENB_POS;

	__g_pRegister->CLKENB	=	ReadValue;

	WriteIODW(&__g_pRegister->CLKENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of clock generator's operation
 *	@return		\b CTRUE indicate that Clock generator is enabled.\n
 *				\b CFALSE indicate that Clock generator is disabled.
 *	@see		NX_AUDIO_SetClockPClkMode,			NX_AUDIO_GetClockPClkMode,
 *				NX_AUDIO_SetClockSource,			NX_AUDIO_GetClockSource,
 *				NX_AUDIO_SetClockDivisor,			NX_AUDIO_GetClockDivisor,
 *				NX_AUDIO_SetClockOutInv,			NX_AUDIO_GetClockOutInv,
 *				NX_AUDIO_SetClockOutEnb,			NX_AUDIO_GetClockOutEnb,
 *				NX_AUDIO_SetClockDivisorEnable
 */
CBOOL	NX_AUDIO_GetClockDivisorEnable( void )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (__g_pRegister->CLKENB & CLKGENENB_MASK) >> CLKGENENB_POS );
}

//------------------------------------------------------------------------------
// Audio Buffer Configure Function
//------------------------------------------------------------------------------
/**
 *	@brief		Set data width of pcm output buffer
 *	@param[in]	DataWidth	Data width ( 16, 18, 20, 24 )
 *	@return		None.
 *	@remarks	AC97 support	16, 18, 20, 24 bit data width. \n
 *				I2S only support	16, 24 bit data width.
 *	@see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *														NX_AUDIO_GetPCMOUTDataWidth,
 *				NX_AUDIO_SetPCMINDataWidth,				NX_AUDIO_GetPCMINDataWidth,
 *				NX_AUDIO_IsPCMInBufferReady,			NX_AUDIO_IsPCMOutBufferReady
 */
void	NX_AUDIO_SetPCMOUTDataWidth( U32 DataWidth )
{
	const U32	PO_WIDTH_POS	=	0;
	const U32	PO_WIDTH_MASK	=	0x03UL << PO_WIDTH_POS;

	U32 ReadValue;
	U32 SetValue=0;

	NX_ASSERT( (16==DataWidth) || (18==DataWidth) || (20==DataWidth) || (24==DataWidth) );
	NX_ASSERT( CNULL != __g_pRegister );

	switch( DataWidth )
	{
		case 16:	SetValue = 0; break;
		case 18:	SetValue = 1; break;
		case 20:	SetValue = 2; break;
		case 24:	SetValue = 3; break;
		default:	NX_ASSERT( CFALSE );
	}

	ReadValue	=	__g_pRegister->AUDIO_BUFF_CONFIG;

	ReadValue	&=	~PO_WIDTH_MASK;
	ReadValue	|=	(U32)SetValue << PO_WIDTH_POS;

	__g_pRegister->AUDIO_BUFF_CONFIG	=	(U16)ReadValue;
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get data width of pcm output buffer
 *	@return		Data width ( 16, 18, 20, 24 bit )
 *	@remarks	AC97 support		16, 18, 20, 24 bit data width. \n
 *				I2S only support	16, 24 bit data width.
 *	@see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetPCMOUTDataWidth,
 *				NX_AUDIO_SetPCMINDataWidth,				NX_AUDIO_GetPCMINDataWidth,
 *				NX_AUDIO_IsPCMInBufferReady,			NX_AUDIO_IsPCMOutBufferReady
 */
U32	NX_AUDIO_GetPCMOUTDataWidth( void )
{
	const U32	PO_WIDTH_POS	=	0;
	const U32	PO_WIDTH_MASK	=	0x03UL << PO_WIDTH_POS;

	U32 ReadValue;
	U32 RetValue=0;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = ((__g_pRegister->AUDIO_BUFF_CONFIG & PO_WIDTH_MASK) >> PO_WIDTH_POS );

	switch( ReadValue )
	{
		case	0:	RetValue = 16;	break;
		case	1:	RetValue = 18;	break;
		case	2:	RetValue = 20;	break;
		case	3:	RetValue = 24;	break;
		default:	NX_ASSERT( CFALSE );
	}

	return RetValue;
}

//------------------------------------------------------------------------------
/**
 * @brief		Set Data Width ( 16, 18, 20bit ) of SPDIF Output Audio Buffer
 * @param[in]	DataWidth	Data Width( 16, 18, 20bit )
 * @return		None.
 * @remarks	AC97 only use SPDIF out width.
 * @see		NX_AUDIO_GetSPDIFOUTDataWidth
 */
void	NX_AUDIO_SetSPDIFOUTDataWidth( U32 DataWidth )
{
	const U32	SP_WIDTH_POS	=	2;
	const U32	SP_WIDTH_MASK	=	0x03UL << SP_WIDTH_POS;

	U32 ReadValue;
	U32 SetValue=0;

	NX_ASSERT( (16==DataWidth) || (18==DataWidth) || (20==DataWidth) );
	NX_ASSERT( CNULL != __g_pRegister );

	switch( DataWidth )
	{
		case 16:	SetValue = 0; break;
		case 18:	SetValue = 1; break;
		case 20:	SetValue = 2; break;
		default:	NX_ASSERT( CFALSE );
	}

	ReadValue	=	__g_pRegister->AUDIO_BUFF_CONFIG;

	ReadValue	&=	~SP_WIDTH_MASK;
	ReadValue	|=	(U32)SetValue << SP_WIDTH_POS;

//	__g_pRegister->AUDIO_BUFF_CONFIG	=	(U16)ReadValue;
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 * @brief		Get Data Width ( 16, 18, 20bit ) of SPDIF Output Audio Buffer
 * @return		Data Width ( 16, 18, 20it )
 * @remarks		AC97 only use SPDIF out width.
 * @see			SetSPDIFOUTDataWidth
 */
U32		NX_AUDIO_GetSPDIFOUTDataWidth( void )
{
	const U32	SP_WIDTH_POS	=	2;
	const U32	SP_WIDTH_MASK	=	0x03UL << SP_WIDTH_POS;

	U32 ReadValue;
	U32 RetValue=0;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = ((__g_pRegister->AUDIO_BUFF_CONFIG & SP_WIDTH_MASK) >> SP_WIDTH_POS );

	switch( ReadValue )
	{
		case	0:	RetValue = 16;	break;
		case	1:	RetValue = 18;	break;
		case	2:	RetValue = 20;	break;
		default:	NX_ASSERT( CFALSE );
	}

	return RetValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set data width of pcm input buffer
 *	@param[in]	DataWidth	Data width ( 16, 18, 20, 24 bit )
 *	@return		None.
 *	@remarks	AC97 support the 16, 18, 20, 24 bit data width. \n
 *				I2S only support the 16, 24 bit data width. \n
 *	@see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetPCMOUTDataWidth,			NX_AUDIO_GetPCMOUTDataWidth,
 *														NX_AUDIO_GetPCMINDataWidth,
 *				NX_AUDIO_IsPCMInBufferReady,			NX_AUDIO_IsPCMOutBufferReady
 */
void	NX_AUDIO_SetPCMINDataWidth( U32 DataWidth )
{
	const U32	PI_WIDTH_POS	=	4;
	const U32	PI_WIDTH_MASK	=	0x03UL << PI_WIDTH_POS;

	U32 ReadValue;
	U32 SetValue=0;

	NX_ASSERT( (16==DataWidth) || (18==DataWidth) || (20==DataWidth) || (24==DataWidth));
	NX_ASSERT( CNULL != __g_pRegister );

	switch( DataWidth )
	{
		case 16:	SetValue = 0; break;
		case 18:	SetValue = 1; break;
		case 20:	SetValue = 2; break;
		case 24:	SetValue = 3; break;
		default:	NX_ASSERT( CFALSE );
	}

	ReadValue	=	__g_pRegister->AUDIO_BUFF_CONFIG;

	ReadValue	&=	~PI_WIDTH_MASK;
	ReadValue	|=	(U32)SetValue << PI_WIDTH_POS;

//	__g_pRegister->AUDIO_BUFF_CONFIG	=	(U16)ReadValue;
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get data width of pcm input buffer
 *	@return		Data width ( 16, 18, 20, 24 bit )
 *	@remarks	AC97 support the 16, 18, 20, 24 bit data width. \n
 *				I2S only support the 16, 24 bit data width. \n
 *	@see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetPCMOUTDataWidth,			NX_AUDIO_GetPCMOUTDataWidth,
 *				NX_AUDIO_SetPCMINDataWidth,
 *				NX_AUDIO_IsPCMInBufferReady,			NX_AUDIO_IsPCMOutBufferReady
 */
U32		NX_AUDIO_GetPCMINDataWidth( void )
{
	const U32	PI_WIDTH_POS	=	4;
	const U32	PI_WIDTH_MASK	=	0x03UL << PI_WIDTH_POS;

	U32 ReadValue;
	U32 RetValue=0;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = ((__g_pRegister->AUDIO_BUFF_CONFIG & PI_WIDTH_MASK) >> PI_WIDTH_POS );

	switch( ReadValue )
	{
		case	0:	RetValue = 16;	break;
		case	1:	RetValue = 18;	break;
		case	2:	RetValue = 20;	break;
		case	3:	RetValue = 24;	break;
		default:	NX_ASSERT( CFALSE );
	}

	return RetValue;
}

//------------------------------------------------------------------------------
// Audio Buffer Control Function
//------------------------------------------------------------------------------
/**
 *	@brief		Set audio's output buffer operation.
 *	@param[in]	Enable	\b CTRUE indicates that PCM output buffer Enable. \n
 *						\b CFALSE indicates that PCM output buffer Disable.
 *	@return		None.
 *	@remarks	Audio output buffer's enable and disable means that setting data transfer to the
 *				Audio buffer throught DMA.\n
 *				Enable( DMA \b can transfer data to Audio's output buffer )\n
 *				Disable( DMA \b can't transfer data to Audio's output buffer ).\n
 *				Both(AC97, I2S) use PCM out buffer.
 *	@see												NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */
void	NX_AUDIO_SetAudioBufferPCMOUTEnable( CBOOL Enable )
{
	const U32 PCMOBUF_EN_POS	= 0;
	const U32 PCMOBUF_EN_MASK	= 1UL << PCMOBUF_EN_POS;

	register struct NX_AUDIO_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	ReadValue	=	pRegister->AUDIO_BUFF_CTRL;

	ReadValue	&=	~PCMOBUF_EN_MASK;
	ReadValue	|=	(U32)Enable << PCMOBUF_EN_POS;

//	pRegister->AUDIO_BUFF_CTRL	=	(U16)ReadValue;
	WriteIOW(&pRegister->AUDIO_BUFF_CTRL, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get operation status of audio output buffer
 *	@return		\b CTRUE indicate that PCM output buffer is Enabled. \n
 *				\b CFALSE indicate that PCM output buffer is Disabled.
 *	@remarks	Audio output buffer's enable and disable means that setting data transfer to the
 *				Audio buffer throught DMA. \n
 *				Enable( DMA \b can transfer data to Audio's output buffer ). \n
 *				Disable( DMA \b can't transfer data to Audio's output buffer ). \n
 *				Both(AC97, I2S) use PCM out buffer.
 *	@see		NX_AUDIO_SetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */
CBOOL NX_AUDIO_GetAudioBufferPCMOUTEnable( void )
{
	const U32 PCMOBUF_EN_POS	= 0;
	const U32 PCMOBUF_EN_MASK	= 1UL << PCMOBUF_EN_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (__g_pRegister->AUDIO_BUFF_CTRL & PCMOBUF_EN_MASK) >> PCMOBUF_EN_POS );
}

//------------------------------------------------------------------------------
/**
 * @brief		Set Audio Buffer ( SPDIF Output Buffer's Operation )
 * @param[in]	Enable	CTRUE indicates that SPDIF Out Buffer Enable. \n
 *						CFALSE indicates that SPDIF Out Buffer Disable.
 * @return		None.
 * @remarks		AC97 only use SPDIF out buffer.
 * @see			NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *														NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */

void NX_AUDIO_SetAudioBufferSPDIFOUTEnable
(
	CBOOL Enable
)
{
	const U32	BUFSPDIFOUT_MASK	=	( 0x01 << 1 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AUDIO_BUFF_CTRL	|=	(U16)BUFSPDIFOUT_MASK;
//	}
//	else
//	{
//		__g_pRegister->AUDIO_BUFF_CTRL	&=	~BUFSPDIFOUT_MASK;
//	}

	SetValue = __g_pRegister->AUDIO_BUFF_CTRL;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)BUFSPDIFOUT_MASK;
	}
	else
	{
		SetValue	&=	~BUFSPDIFOUT_MASK;
	}
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 * @brief	Get State of Audio Buffer ( SPDIF Output is Enabled or Disabled )
 * @return	CTRUE indicates that SPDIF Out is Enabled.\n
 *			CFALSE indicates that SPDIF Out is Disabled.
 * @remarks AC97 only use SPDIF out buffer.
 * @see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *			NX_AUDIO_SetAudioBufferSPDIFOUTEnable,
 *			NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *			NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *			NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *			NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *			NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *			NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */

CBOOL NX_AUDIO_GetAudioBufferSPDIFOUTEnable( void )
{
	const U32	BUFSPDIFOUT_MASK	=	( 0x01 << 1 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_BUFF_CTRL & BUFSPDIFOUT_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set audio's input buffer operation.
 *	@param[in]	Enable	\b CTRUE indicates that PCM input buffer Enable. \n
 *						\b CFALSE indicates that PCM input buffer Disable.
 *	@return		None.
 *	@remarks	Audio input buffer's enable and disable means that setting data transfer to the
 *				Audio buffer throught DMA.\n
 *				Enable( DMA \b can receive data from	Audio's input buffer ).\n
 *				Disable( DMA \b can't receive data from Audio's input buffer ). \n
 *				Both(AC97, I2S) use PCM input buffer.
 *	@see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *														NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */
void	NX_AUDIO_SetAudioBufferPCMINEnable( CBOOL Enable )
{
	const U32 PCMIBUF_EN_POS	= 2;
	const U32 PCMIBUF_EN_MASK	= 1UL << PCMIBUF_EN_POS;

	register struct NX_AUDIO_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	ReadValue	=	pRegister->AUDIO_BUFF_CTRL;

	ReadValue	&=	~PCMIBUF_EN_MASK;
	ReadValue	|=	(U32)Enable << PCMIBUF_EN_POS;

//	pRegister->AUDIO_BUFF_CTRL	=	(U16)ReadValue;
	WriteIOW(&pRegister->AUDIO_BUFF_CTRL, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get operation status of audio input buffer
 *	@return		\b CTRUE indicate that PCM input buffer is Enabled. \n
 *				\b CFALSE indicate that PCM input buffer is Disabled.
 *	@remarks	Audio input buffer's enable and disable means that setting data transfer to the
 *				Audio buffer throught DMA.\n
 *				Enable( DMA \b can receive data from	Audio's input buffer ).\n
 *				Disable( DMA \b can't receive data from Audio's input buffer ).\n
 *				Both(AC97, I2S) use PCM input buffer.
 *	@see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */
CBOOL NX_AUDIO_GetAudioBufferPCMINEnable( void )
{
	const U32 PCMIBUF_EN_POS	= 2;
	const U32 PCMIBUF_EN_MASK	= 1UL << PCMIBUF_EN_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (__g_pRegister->AUDIO_BUFF_CTRL & PCMIBUF_EN_MASK) >> PCMIBUF_EN_POS );
}

//------------------------------------------------------------------------------
/**
 * @brief		Set Audio Buffer ( MIC Input Buffer's Operation )
 * @param[in]	Enable		CTRUE( MIC Input Buffer Enable ), CFALSE( MIC Input Buffer Disable );
 * @return		None.
 * @remarks		AC97 only use MIC input buffer.
 * @see			NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *														NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */
void NX_AUDIO_SetAudioBufferMICINEnable
(
	CBOOL Enable
)
{
	const U32	BUFMICIN_MASK	=	( 0x01 << 3 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AUDIO_BUFF_CTRL	|=	(U16)BUFMICIN_MASK;
//	}
//	else
//	{
//		__g_pRegister->AUDIO_BUFF_CTRL	&=	~BUFMICIN_MASK;
//	}
	SetValue = __g_pRegister->AUDIO_BUFF_CTRL;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)BUFMICIN_MASK;
	}
	else
	{
		SetValue	&=	~BUFMICIN_MASK;
	}
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 * @brief	Get State of Audio Buffer ( MIC Input Buffer	is Enabled or Disabled )
 * @return	CTRUE indicates that MIC Input Buffer is Enabled.\n
 *			CFALSE indicates that MIC Input Buffer is Disabled.
 * @remarks	AC97 only use MIC input buffer.
 * @see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *			NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *			NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *			NX_AUDIO_SetAudioBufferMICINEnable,
 *			NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *			NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *			NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *			NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */

CBOOL NX_AUDIO_GetAudioBufferMICINEnable( void )
{
	const U32	BUFMICIN_MASK	=	( 0x01 << 3 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_BUFF_CTRL & BUFMICIN_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 * @brief		Set Audio Buffer ( ADC1 Input Buffer's Operation )
 * @param[in]	Enable		CTRUE( ADC1 In Buffer Enable ), CFALSE( ADC1 In Buffer Disable );
 * @return		None.
 * @remarks		AC97 only use ADC1 IN buffer.
 * @see			NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *														NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */
void NX_AUDIO_SetAudioBufferADC1Enable
(
	CBOOL Enable
)
{
	const U32	BUFADC1_MASK	=	( 0x01 << 4 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AUDIO_BUFF_CTRL	|=	(U16)BUFADC1_MASK;
//	}
//	else
//	{
//		__g_pRegister->AUDIO_BUFF_CTRL	&=	~BUFADC1_MASK;
//	}
	SetValue = __g_pRegister->AUDIO_BUFF_CTRL;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)BUFADC1_MASK;
	}
	else
	{
		SetValue	&=	~BUFADC1_MASK;
	}
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 * @brief	Get State of Audio Buffer ( ADC1 Input Buffer	is Enabled or Disabled )
 * @return	CTRUE indicates that ADC1 Input Buffer is Enabled.\n
 *			CFALSE indicates that ADC1 Input Buffer is Disabled.
 * @remarks	AC97 only use ADC1 input buffer.
 * @see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *			NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *			NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *			NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *			NX_AUDIO_SetAudioBufferADC1Enable,
 *			NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *			NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *			NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */

CBOOL NX_AUDIO_GetAudioBufferADC1Enable( void )
{
	const U32	BUFADC1_MASK	=	( 0x01 << 4 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_BUFF_CTRL & BUFADC1_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 * @brief		Set Audio Buffer ( ADC2 Input Buffer's Operation )
 * @param[in]	Enable		CTRUE( ADC2 Input Buffer Enable ), CFALSE( ADC2 Input Buffer Disable );
 * @return		None.
 * @remarks		AC97 only use ADC2 input buffer.
 * @see			NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *														NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */
void NX_AUDIO_SetAudioBufferADC2Enable
(
	CBOOL Enable
)
{
	const U32	BUFADC2_MASK	=	( 0x01 << 5 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AUDIO_BUFF_CTRL	|=	(U16)BUFADC2_MASK;
//	}
//	else
//	{
//		__g_pRegister->AUDIO_BUFF_CTRL	&=	~BUFADC2_MASK;
//	}
	SetValue = __g_pRegister->AUDIO_BUFF_CTRL;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)BUFADC2_MASK;
	}
	else
	{
		SetValue	&=	~BUFADC2_MASK;
	}
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 * @brief		Get State of Audio Buffer ( ADC2 Input Buffer	is Enabled or Disabled )
 * @return		CTRUE indicate that ADC2 Input Buffer is Enabled.\n
 *				CFALSE indicate that ADC2 Input Buffer is Disabled.
 * @remarks		AC97 only use ADC2 input buffer.
 * @see			NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */

CBOOL NX_AUDIO_GetAudioBufferADC2Enable( void )
{
	const U32	BUFADC2_MASK	=	( 0x01 << 5 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_BUFF_CTRL & BUFADC2_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 * @brief		Set output audio buffer ( PCM, SPDIF )
 * @param[in]	pcm		CTRUE( PCM output buffer enable ),	CFALSE( PCM output buffer disable )
 * @param[in]	spdif	CTRUE( SPDIF output buffer enable ), CFALSE( SPDIF output buffer disable )
 * @return		None.
 * @remarks		AC97 only use SPDIF output buffer.
 * @see			NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *														NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */

void	NX_AUDIO_SetAudioBufferOUTEnable
(
	CBOOL pcm,
	CBOOL spdif
)
{
	const U32	PCM_MASK	= (0x01 << 0);
	const U32	SPDIF_MASK	= (0x01 << 1);
	register U32	temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->AUDIO_BUFF_CTRL;

	temp &= ~( PCM_MASK | SPDIF_MASK );

	if( CTRUE == pcm	){ temp |= PCM_MASK; }
	if( CTRUE == spdif ){ temp |= SPDIF_MASK; }

//	__g_pRegister->AUDIO_BUFF_CTRL = (U16)temp;
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CTRL, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 * @brief		Get State of Output audio buffer( PCM ,SPDIF )
 * @param[out]	pcm	CTRUE : ( PCM output buffer is enabled ),	CFALSE : ( PCM output buffer is disabled )
 * @param[out]	spdif	CTRUE : ( SPDIF output buffer is enabled ), CFALSE : ( SPDIF output buffer is disabled )
 * @return		None.
 * @remarks		AC97 only use SPDIF output buffer. \n
 *				A parameter can set to CNULL, when the information is not need.
 * @see			NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable,		NX_AUDIO_GetAudioBufferINEnable
 */

void	NX_AUDIO_GetAudioBufferOUTEnable
(
	CBOOL* pcm,
	CBOOL* spdif
)
{
	const U32	PCM_BITPOS		= 0;
	const U32	PCM_MASK		= (0x01 << PCM_BITPOS);
	const U32	SPDIF_BITPOS	= 1;
	const U32	SPDIF_MASK		= (0x01 << SPDIF_BITPOS);

	register U32	temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->AUDIO_BUFF_CTRL;

	if( CNULL != pcm	){ *pcm		= (CBOOL)(( temp & PCM_MASK	) >> PCM_BITPOS	); }
	if( CNULL != spdif ){ *spdif	= (CBOOL)(( temp & SPDIF_MASK ) >> SPDIF_BITPOS ); }
}

//------------------------------------------------------------------------------
/**
 * @brief		Set input audio buffer ( PCM, MIC, ADC1, ADC2 )
 * @param[in]	pcm		CTRUE( PCM	input buffer Enable ),	CFALSE( PCM	input buffer Disable )
 * @param[in]	mic		CTRUE( MIC	input buffer Enable ),	CFALSE( MIC	input buffer Disable )
 * @param[in]	adc1	CTRUE( ADC1 input buffer Enable ),	CFALSE( ADC1 input buffer Disable )
 * @param[in]	adc2	CTRUE( ADC2 input buffer Enable ),	CFALSE( ADC2 input buffer Disable )
 * @return		None.
 * @remarks		AC97 only use MIC, ADC1, and ADC2 input buffer.\n
 *				I2S not use MIC, ADC1, and ADC2 input buffer.
 * @see			NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *														NX_AUDIO_GetAudioBufferINEnable
 */

void	NX_AUDIO_SetAudioBufferINEnable
(
	CBOOL pcm,
	CBOOL mic,
	CBOOL adc1,
	CBOOL adc2
)
{
	const U32	PCM_MASK	=	( 1 << 2 );
	const U32	MIC_MASK	=	( 1 << 3 );
	const U32	ADC1_MASK	=	( 1 << 4 );
	const U32	ADC2_MASK	=	( 1 << 5 );
	const U32	IN_MASK		=	( 0x0F << 2 );
	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->AUDIO_BUFF_CTRL;
	temp &= ~IN_MASK;

	if( CTRUE == pcm	){ temp |= PCM_MASK; }
	if( CTRUE == mic	){ temp |= MIC_MASK; }
	if( CTRUE == adc1 ){ temp |= ADC1_MASK; }
	if( CTRUE == adc2 ){ temp |= ADC2_MASK; }

//	__g_pRegister->AUDIO_BUFF_CTRL = (U16)temp;
	WriteIOW(&__g_pRegister->AUDIO_BUFF_CTRL, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 * @brief		Get State of input audio buffer( PCM, MIC, ADC1, ADC2 )
 * @param[out]	pcm		CTRUE : ( PCM input buffer is enabled ),	CFALSE : ( PCM input buffer is disabled )
 * @param[out]	mic		CTRUE : ( MIC input buffer is enabled ),	CFALSE : ( MIC input buffer is disabled )
 * @param[out]	adc1	CTRUE : ( ADC1 input buffer is enabled ),	CFALSE : ( ADC1 input buffer is disabled )
 * @param[out]	adc2	CTRUE : ( ADC2 input buffer is enabled ),	CFALSE : ( ADC2 input buffer is disabled )
 * @return		None.
 * @remarks		AC97 only use MIC, ADC1, and ADC2 input buffer.\n
 *				I2S not use MIC, ADC1, and ADC2 input buffer.\n
 *				A parameter can set to CNULL, when the information is not need.
 *	@see		NX_AUDIO_SetAudioBufferPCMOUTEnable,	NX_AUDIO_GetAudioBufferPCMOUTEnable,
 *				NX_AUDIO_SetAudioBufferSPDIFOUTEnable,	NX_AUDIO_GetAudioBufferSPDIFOUTEnable,
 *				NX_AUDIO_SetAudioBufferPCMINEnable,		NX_AUDIO_GetAudioBufferPCMINEnable,
 *				NX_AUDIO_SetAudioBufferMICINEnable,		NX_AUDIO_GetAudioBufferMICINEnable,
 *				NX_AUDIO_SetAudioBufferADC1Enable,		NX_AUDIO_GetAudioBufferADC1Enable,
 *				NX_AUDIO_SetAudioBufferADC2Enable,		NX_AUDIO_GetAudioBufferADC2Enable,
 *				NX_AUDIO_SetAudioBufferOUTEnable,		NX_AUDIO_GetAudioBufferOUTEnable,
 *				NX_AUDIO_SetAudioBufferINEnable
 */
void	NX_AUDIO_GetAudioBufferINEnable
(
	CBOOL *pcm,
	CBOOL *mic,
	CBOOL *adc1,
	CBOOL *adc2
)
{
	const U32	PCM_BITPOS	= 2;
	const U32	MIC_BITPOS	= 3;
	const U32	ADC1_BITPOS	= 4;
	const U32	ADC2_BITPOS	= 5;

	const U32	PCM_MASK		=	( 1 << PCM_BITPOS );
	const U32	MIC_MASK		=	( 1 << MIC_BITPOS );
	const U32	ADC1_MASK	=	( 1 << ADC1_BITPOS );
	const U32	ADC2_MASK	=	( 1 << ADC2_BITPOS );

	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->AUDIO_BUFF_CTRL;

	if( CNULL != pcm	){ *pcm	= (CBOOL)(( temp &	PCM_MASK	) >> PCM_BITPOS	); }
	if( CNULL != mic	){ *mic	= (CBOOL)(( temp &	MIC_MASK	) >> MIC_BITPOS	); }
	if( CNULL != adc1 ){ *adc1	= (CBOOL)(( temp &	ADC1_MASK ) >> ADC1_BITPOS ); }
	if( CNULL != adc2 ){ *adc2	= (CBOOL)(( temp &	ADC2_MASK ) >> ADC2_BITPOS ); }
}

//------------------------------------------------------------------------------
/// @name	Check of Audio's Buffer State
//------------------------------------------------------------------------------
/**
 * @brief	Check ADC2 Input Buffer is Ready or Not
 * @return	CTRUE indicates that ADC2 Input Buffer is Ready.\n
 *			CFALSE indicates that ADC2 Input Buffer is NOT Ready.
 * @remarks AC97 only use ADC2 input buffer.
 * @see											NX_AUDIO_IsADC1InBufferReady,
 *			NX_AUDIO_IsMICInBufferReady,		NX_AUDIO_IsPCMInBufferReady,
 *			NX_AUDIO_IsSPDIFOutBufferReady,		NX_AUDIO_IsPCMOutBufferReady
 */

CBOOL	NX_AUDIO_IsADC2InBufferReady( void )
{
	const U32	ADC2_MASK = ( 1 << 5 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_STATUS1 & ADC2_MASK )	{ return CTRUE; }
	else											{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 * @brief	Check ADC1 Input Buffer is Ready or Not
 * @return	CTRUE indicates that ADC1 Input Buffer is Ready.\n
 *			CFALSE indicates that ADC1 Input Buffer is NOT Ready.
 * @remarks	AC97 only use ADC1 input buffer.
 * @see		NX_AUDIO_IsADC2InBufferReady,
 *			NX_AUDIO_IsMICInBufferReady,		NX_AUDIO_IsPCMInBufferReady,
 *			NX_AUDIO_IsSPDIFOutBufferReady,		NX_AUDIO_IsPCMOutBufferReady
 */
CBOOL	NX_AUDIO_IsADC1InBufferReady( void )
{
	const U32	ADC1_MASK = ( 1 << 4 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_STATUS1 & ADC1_MASK )	{ return CTRUE; }
	else											{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 * @brief	Check MIC Input Buffer is Ready or Not
 * @return	CTRUE indicates that MIC Input Buffer is Ready.\n
 *			CFALSE indicates that MIC Input Buffer is NOT Ready.
 * @remarks	AC97 only use MIC input buffer.
 * @see		NX_AUDIO_IsADC2InBufferReady,		NX_AUDIO_IsADC1InBufferReady,
 *												NX_AUDIO_IsPCMInBufferReady,
 *			NX_AUDIO_IsSPDIFOutBufferReady,		NX_AUDIO_IsPCMOutBufferReady
 */
CBOOL	NX_AUDIO_IsMICInBufferReady( void )
{
	const U32	MIC_MASK = ( 1 << 3 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_STATUS1 & MIC_MASK ){ return CTRUE; }
	else											{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check PCM input buffer's status
 *	@return		\b CTRUE indicates that Input buffer is ready. \n
 *				\b CFALSE indicates that None.
 *	@remarks	Input buffer's ready means that Input buffer have some space to receive data.
 *				Both (AC97 and I2S ) use PCM input buffer.
 *	@see		NX_AUDIO_IsADC2InBufferReady,		NX_AUDIO_IsADC1InBufferReady,
 *				NX_AUDIO_IsMICInBufferReady,
 *				NX_AUDIO_IsSPDIFOutBufferReady,		NX_AUDIO_IsPCMOutBufferReady
 */
CBOOL NX_AUDIO_IsPCMInBufferReady( void )
{
	const U32	PIBUF_RDY_POS	= 2;
	const U32	PIBUF_RDY_MASK	= 1UL << PIBUF_RDY_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->AUDIO_STATUS1 & PIBUF_RDY_MASK) >> PIBUF_RDY_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check SPDIF Output Buffer is Ready or Not
 *	@return		CTRUE indicates that SPDIF Output Buffer is Ready.\n
 *				CFALSE indicates that SPDIF Output Buffer is NOT Ready.
 *	@remarks	AC97 only use SPDIF input buffer.
 *	@see		NX_AUDIO_IsADC2InBufferReady,		NX_AUDIO_IsADC1InBufferReady,
 *				NX_AUDIO_IsMICInBufferReady,		NX_AUDIO_IsPCMInBufferReady,
 *													NX_AUDIO_IsPCMOutBufferReady
 */
CBOOL	NX_AUDIO_IsSPDIFOutBufferReady( void )
{
	const U32	SPDIF_MASK = ( 1 << 1 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AUDIO_STATUS1 & SPDIF_MASK )	{ return CTRUE; }
	else											{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check PCM output buffer's status
 *	@return		\b CTRUE indicate that output buffer is ready. \n
 *				\b CFALSE indicate that None.
 *	@remarks	Output buffer's ready means that Output buffer have some data to send.\n
 *				Both (AC97 and I2S ) use PCM output buffer.
 *	@see		NX_AUDIO_IsADC2InBufferReady,		NX_AUDIO_IsADC1InBufferReady,
 *				NX_AUDIO_IsMICInBufferReady,		NX_AUDIO_IsPCMInBufferReady,
 *				NX_AUDIO_IsSPDIFOutBufferReady
 */
CBOOL NX_AUDIO_IsPCMOutBufferReady( void )
{
	const U32	POBUF_RDY_POS	= 0;
	const U32	POBUF_RDY_MASK	= 1UL << POBUF_RDY_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)(__g_pRegister->AUDIO_STATUS1 & POBUF_RDY_MASK);
}

//------------------------------------------------------------------------------
// AC97 Configuration Function
//------------------------------------------------------------------------------
/**
 *	@brief		Warm Reset of AC97 Controller
 *	@param[in]	Enable	CTRUE indicates that Set to Warm Reset. \n
 *						CFALSE indicates that Set to Nomal Operation.
 *	@return		None.
 *	@remarks	Wait 1 us to warm reset.\n
 *				After Warm Reset You should set to normal operation
 *	@code
 *				SetAC97WarmReset( CTRUE );		// Warm Reset
 *				// Wait 1us
 *				SetAC97WarmReset( CFALSE );		// Normal Operation
 *	@endcode
 *	@see											NX_AUDIO_GetAC97WarmReset,
 *				NX_AUDIO_SetAC97ControllerReset,	NX_AUDIO_GetAC97ControllerReset,
 *				NX_AUDIO_SetAC97ColdReset,			NX_AUDIO_GetAC97ColdReset,
 *				NX_AUDIO_SetAC97LinkOn,				NX_AUDIO_GetAC97LinkOn
 */
void NX_AUDIO_SetAC97WarmReset
(
	CBOOL Enable
)
{
	const U32	WARMRESET_MASK		=	( 1 << 3 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CTRL	=	(U16)( __g_pRegister->AC97_CTRL | WARMRESET_MASK );
//	}
//	else
//	{
//		__g_pRegister->AC97_CTRL	&=	~WARMRESET_MASK;
//	}
	SetValue = __g_pRegister->AC97_CTRL;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)( WARMRESET_MASK );
	}
	else
	{
		SetValue	&=	~WARMRESET_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of AC97 Controller's Warm Reset
 *	@return		CTRUE indicates that Warm Reset.\n
 *				CFALSE indicates that Normal Operation.
 *	@see		NX_AUDIO_SetAC97WarmReset,
 *				NX_AUDIO_SetAC97ControllerReset,	NX_AUDIO_GetAC97ControllerReset,
 *				NX_AUDIO_SetAC97ColdReset,			NX_AUDIO_GetAC97ColdReset,
 *				NX_AUDIO_SetAC97LinkOn,				NX_AUDIO_GetAC97LinkOn
 */
CBOOL NX_AUDIO_GetAC97WarmReset( void )
{
	const U32	WARMRESET_MASK		=	( 1 << 3 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CTRL & WARMRESET_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Reset of AC97 Controller
 *	@param[in]	Enable	\b CTRUE indicates that set to Contoller Reset. \n
 *						\b CFALSE indicates that set to	Nomal Operation.
 *	@return		None.
 *	@remarks	After Reset You should set to normal operation
 *	@code
 *				NX_AUDIO_SetAC97ControllerReset( CTRUE );	// Controller Reset
 *				NX_AUDIO_SetAC97ControllerReset( CFALSE ); // Normal Operation
 *	@endcode
 *	@see		NX_AUDIO_SetAC97WarmReset,		NX_AUDIO_GetAC97WarmReset,
 *												NX_AUDIO_GetAC97ControllerReset,
 *				NX_AUDIO_SetAC97ColdReset,		NX_AUDIO_GetAC97ColdReset,
 *				NX_AUDIO_SetAC97LinkOn,			NX_AUDIO_GetAC97LinkOn
 */
void NX_AUDIO_SetAC97ControllerReset
(
	CBOOL Enable
)
{
	const U32	CTRLRESET_MASK		=	( 1 << 1 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CTRL	&=	~CTRLRESET_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CTRL	=	(U16)( __g_pRegister->AC97_CTRL | CTRLRESET_MASK );
//	}
	SetValue = __g_pRegister->AC97_CTRL;
	if( CTRUE == Enable )
	{
		SetValue	&=	~CTRLRESET_MASK;
	}
	else
	{
		SetValue	|=	(U16)( CTRLRESET_MASK );
	}
	WriteIOW(&__g_pRegister->AC97_CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of AC97 Controller's reset
 *	@return		\b CTRUE indicates that Controller is Reset.\n
 *				\b CFALSE indicates that Controller is Normal Operation.
 *	@see		NX_AUDIO_SetAC97WarmReset,			NX_AUDIO_GetAC97WarmReset,
 *				NX_AUDIO_SetAC97ControllerReset,
 *				NX_AUDIO_SetAC97ColdReset,			NX_AUDIO_GetAC97ColdReset,
 *				NX_AUDIO_SetAC97LinkOn,				NX_AUDIO_GetAC97LinkOn
 */
CBOOL NX_AUDIO_GetAC97ControllerReset( void )
{
	const U32	CTRLRESET_MASK		=	( 1 << 1 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CTRL & CTRLRESET_MASK )
	{
		return CFALSE;
	}
	else
	{
		return CTRUE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Cold Reset of Audio Controller( AC97, I2S )
 *	@param[in]	Enable	\b CTRUE indicates that Set to Cold Reset. \n
 *						\b CFALSE indicates that set to Nomal Operation. \n
 *	@return		None.
 *	@remarks	After Reset You should set to normal operation
 *	@code
 *				SetAC97ColdReset( CTRUE );	// Cold Reset
 *				SetAC97ColdReset( CFALSE );	// Normal Operation
 *	@endcode
 *	@see		NX_AUDIO_SetAC97WarmReset,			NX_AUDIO_GetAC97WarmReset,
 *				NX_AUDIO_SetAC97ControllerReset,	NX_AUDIO_GetAC97ControllerReset,
 *													NX_AUDIO_GetAC97ColdReset,
 *				NX_AUDIO_SetAC97LinkOn,				NX_AUDIO_GetAC97LinkOn
 */
void NX_AUDIO_SetAC97ColdReset
(
	CBOOL Enable
)
{
	const U32	COLDRESET_MASK		=	( 1 << 0 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CTRL	&=	~COLDRESET_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CTRL	=	(U16)( __g_pRegister->AC97_CTRL | COLDRESET_MASK );
//	}
	SetValue = __g_pRegister->AC97_CTRL;
	if( CTRUE == Enable )
	{
		SetValue	&=	~COLDRESET_MASK;
	}
	else
	{
		SetValue	|=	(U16)( COLDRESET_MASK );
	}
	WriteIOW(&__g_pRegister->AC97_CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of AC97 Controller's Cold Reset
 *	@return		CTRUE indicate that Controller is Cold Reset.\n
 *				CFALSE indicate that Controller is Normal Operation.
 *	@see		NX_AUDIO_SetAC97WarmReset,			NX_AUDIO_GetAC97WarmReset,
 *				NX_AUDIO_SetAC97ControllerReset,	NX_AUDIO_GetAC97ControllerReset,
 *				NX_AUDIO_SetAC97ColdReset,
 *				NX_AUDIO_SetAC97LinkOn,				NX_AUDIO_GetAC97LinkOn
 */
CBOOL NX_AUDIO_GetAC97ColdReset( void )
{
	const U32	COLDRESET_MASK		=	( 1 << 0 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CTRL & COLDRESET_MASK )
	{
		return CFALSE;
	}
	else
	{
		return CTRUE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's AC-Link Operation
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97WarmReset,			NX_AUDIO_GetAC97WarmReset,
 *				NX_AUDIO_SetAC97ControllerReset,	NX_AUDIO_GetAC97ControllerReset,
 *				NX_AUDIO_SetAC97ColdReset,			NX_AUDIO_GetAC97ColdReset,
 *													NX_AUDIO_GetAC97LinkOn
 */
void NX_AUDIO_SetAC97LinkOn( )
{
	const U16	LINK_MASK		=	( 1 << 2 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->AC97_CTRL	|=	(U16)LINK_MASK;
	SetValue = __g_pRegister->AC97_CTRL	| (U16)LINK_MASK;
	WriteIOW(&__g_pRegister->AC97_CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get AC97's AC-Link State
 *	@return		CTRUE indicate that AC-LINK is Enabled.\n
 *				CFALSE indicate that AC-LINK is Disabled.
 *	@see		NX_AUDIO_SetAC97WarmReset,			NX_AUDIO_GetAC97WarmReset,
 *				NX_AUDIO_SetAC97ControllerReset,	NX_AUDIO_GetAC97ControllerReset,
 *				NX_AUDIO_SetAC97ColdReset,			NX_AUDIO_GetAC97ColdReset,
 *				NX_AUDIO_SetAC97LinkOn
 */
CBOOL NX_AUDIO_GetAC97LinkOn( void )
{
	const U32	LINK_MASK		=	( 1 << 2 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CTRL & LINK_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
// AC97 Control Function
//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's Front Output Channel Enable
 *	@param[in]	Enable	\b CTRUE indicates that Front Out Enable. \n
 *						\b CFALSE indicates that Front Out Disable.
 *	@return		None.
 *	@see											NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */
void NX_AUDIO_SetAC97FrontOutEnable
(
	CBOOL Enable
)
{
	const U32	FRONT_MASK		=	( 1 << 0 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CONFIG	|=	(U16)FRONT_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CONFIG	&=	~FRONT_MASK;
//	}
	SetValue = __g_pRegister->AC97_CONFIG;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)FRONT_MASK;
	}
	else
	{
		SetValue	&=	~FRONT_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CONFIG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of	AC97's Front Output Channel is Enabled or Disabled
 *	@return		\b CTRUE indicates that Front Output is Enabled.\n
 *				\b CFALSE indicates that Front Output is Disabled.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

CBOOL NX_AUDIO_GetAC97FrontOutEnable( void )
{
	const U32	FRONT_MASK		=	( 1 << 0 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CONFIG & FRONT_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's Rear Output Channel Enable
 *	@param[in]	Enable	\b CTRUE indicates that Rear Out Enable. \n
 *						\b CFALSE indicates that Rear Out Disable.
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *													NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

void NX_AUDIO_SetAC97RearOutEnable
(
	CBOOL Enable
)
{
	const U32	REAR_MASK		=	( 1 << 1 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CONFIG	|=	(U16)REAR_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CONFIG	&=	~REAR_MASK;
//	}
	SetValue = __g_pRegister->AC97_CONFIG;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)REAR_MASK;
	}
	else
	{
		SetValue	&=	~REAR_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CONFIG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of	AC97's Rear Output Channel is Enabled or Disabled
 *	@return		\b CTRUE indicates that Rear Output is Enabled.\n
 *				\b CFALSE indicates that Rear Output is Disabled.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

CBOOL NX_AUDIO_GetAC97RearOutEnable( void )
{
	const U32	REAR_MASK		=	( 1 << 1 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CONFIG & REAR_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's Center Output Channel Enable
 *	@param[in]	Enable	\b CTRUE indicates that Center Out Enable.\n
 *						\b CFALSE indicates that enter Out Disable.
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *													NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */
void NX_AUDIO_SetAC97CenterOutEnable
(
	CBOOL Enable
)
{
	const U32	CENTER_MASK		=	( 1 << 2 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CONFIG	|=	(U16)CENTER_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CONFIG	&=	~CENTER_MASK;
//	}
	SetValue = __g_pRegister->AC97_CONFIG;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)CENTER_MASK;
	}
	else
	{
		SetValue	&=	~CENTER_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CONFIG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of	AC97's Center Output Channel is Enabled or Disabled
 *	@return		\b CTRUE indicates that Center Output is Enabled.\n
 *				\b CFALSE indicates that Center Output is Disabled.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

CBOOL NX_AUDIO_GetAC97CenterOutEnable( void )
{
	const U32	CENTER_MASK		=	( 1 << 2 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CONFIG & CENTER_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's SPDIF Output Channel Enable
 *	@param[in]	Enable	\b CTRUE indicates that SPDIF Out Enable. \n
 *						\b CFALSE indicates that SPDIF Out Disable.
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *													NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */
void NX_AUDIO_SetAC97SPDIFOutEnable
(
	CBOOL Enable
)
{
	const U32	SPDIF_MASK		=	( 1 << 3 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CONFIG	|=	(U16)SPDIF_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CONFIG	&=	~SPDIF_MASK;
//	}
	SetValue = __g_pRegister->AC97_CONFIG;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)SPDIF_MASK;
	}
	else
	{
		SetValue	&=	~SPDIF_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CONFIG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of	AC97's SPDIF Output Channel is Enabled or Disabled
 *	@return		\b CTRUE indicates that SPDIF Output is Enabled.\n
 *				\b CFALSE indicates that SPDIF Output is Disabled.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

CBOOL NX_AUDIO_GetAC97SPDIFOutEnable( void )
{
	const U32	SPDIF_MASK		=	( 1 << 3 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CONFIG & SPDIF_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's	PCM input Channel Enable
 *	@param[in]	Enable	\b CTRUE indicates that PCM input Enable. \n
 *						\b CFALSE indicates that PCM input Disable.
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *													NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

void NX_AUDIO_SetAC97PCMInEnable
(
	CBOOL Enable
)
{
	const U32	PCMIN_MASK		=	( 1 << 4 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CONFIG	|=	(U16)PCMIN_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CONFIG	&=	~PCMIN_MASK;
//	}
	SetValue = __g_pRegister->AC97_CONFIG;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)PCMIN_MASK;
	}
	else
	{
		SetValue	&=	~PCMIN_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CONFIG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of	AC97's PCM Input Channel is Enabled or Disabled
 *	@return		\b CTRUE indicates that PCM Input is Enabled.\n
 *				\b CFALSE indicates that PCM Input is Disabled.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

CBOOL NX_AUDIO_GetAC97PCMInEnable	( void )
{
	const U32	PCMIN_MASK		=	( 1 << 4 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CONFIG & PCMIN_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}
//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's	MIC input Channel Enable
 *	@param[in]	Enable	\b CTRUE indicates that MIC input Enable.\n
 *						\b CFALSE indicates that MIC input Disable.
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *													NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */
void NX_AUDIO_SetAC97MICInEnable
(
	CBOOL Enable
)
{
	const U32	MICIN_MASK		=	( 1 << 5 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CONFIG	|=	(U16)MICIN_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CONFIG	&=	~MICIN_MASK;
//	}
	SetValue = __g_pRegister->AC97_CONFIG;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)MICIN_MASK;
	}
	else
	{
		SetValue	&=	~MICIN_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CONFIG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of	AC97's MIC Input Channel is Enabled or Disabled
 *	@return		\b CTRUE indicates that MIC Input is Enabled.\n
 *				\b CFALSE indicates that MIC Input is Disabled.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

CBOOL NX_AUDIO_GetAC97MICInEnable( void )
{
	const U32	MICIN_MASK		=	( 1 << 5 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CONFIG & MICIN_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}
//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's	ADC1 input Channel Enable
 *	@param[in]	Enable	\b CTRUE indicates that ADC1 input Enable. \n
 *						\b CFALSE indicates that ADC1 input Disable.
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *													NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */
void NX_AUDIO_SetAC97ADC1InEnable
(
	CBOOL Enable
)
{
	const U32	ADC1_MASK		=	( 1 << 6 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CONFIG	|=	(U16)ADC1_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CONFIG	&=	~ADC1_MASK;
//	}
	SetValue = __g_pRegister->AC97_CONFIG;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)ADC1_MASK;
	}
	else
	{
		SetValue	&=	~ADC1_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CONFIG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of	AC97's ADC1 Input Channel is Enabled or Disabled
 *	@return		\b CTRUE indicates that ADC1 Input is Enabled.\n
 *				\b CFALSE indicates that ADC1 Input is Disabled.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

CBOOL NX_AUDIO_GetAC97ADC1InEnable( void )
{
	const U32	ADC1_MASK		=	( 1 << 6 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CONFIG & ADC1_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}
//------------------------------------------------------------------------------
/**
 *	@brief		Set AC97's	ADC2 input Channel Enable
 *	@param[in]	Enable	\b CTRUE indicates that ADC2 input Enable.\n
 *						\b CFALSE indicates that ADC2 input Disable.
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *													NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */
void NX_AUDIO_SetAC97ADC2InEnable
(
	CBOOL Enable
)
{
	const U32	ADC2_MASK		=	( 1 << 7 );
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( CTRUE == Enable )
//	{
//		__g_pRegister->AC97_CONFIG	|=	(U16)ADC2_MASK;
//	}
//	else
//	{
//		__g_pRegister->AC97_CONFIG	&=	~ADC2_MASK;
//	}
	SetValue = __g_pRegister->AC97_CONFIG;
	if( CTRUE == Enable )
	{
		SetValue	|=	(U16)ADC2_MASK;
	}
	else
	{
		SetValue	&=	~ADC2_MASK;
	}
	WriteIOW(&__g_pRegister->AC97_CONFIG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of	AC97's ADC2 Input Channel is Enabled or Disabled
 *	@return		\b CTRUE indicates that ADC2 Input is Enabled.\n
 *				\b CFALSE indicates that ADC2 Input is Disabled.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */

CBOOL NX_AUDIO_GetAC97ADC2InEnable	( void )
{
	const U32	ADC2_MASK		=	( 1 << 7 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->AC97_CONFIG & ADC2_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Output channel( FRONT, REAR, CENTER, SPDIF )
 *	@param[in]	front	CTRUE ( FRONT	output channel Enable ), CFALSE ( FRONT	output channel Disable )
 *	@param[in]	rear		CTRUE ( REAR	output channel Enable ), CFALSE ( REAR	output channel Disable )
 *	@param[in]	center	CTRUE ( CENTER output channel Enable ), CFALSE ( CENTER output channel Disable )
 *	@param[in]	spdif	CTRUE ( SPDIF	output channel Enable ), CFALSE ( SPDIF	output channel Disable )
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *													NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */
void	NX_AUDIO_SetOutputEnable
(
	CBOOL front,
	CBOOL rear,
	CBOOL center,
	CBOOL spdif
)
{
	const U32	F_MASK = ( 1 << 0 );
	const U32	R_MASK = ( 1 << 1 );
	const U32	C_MASK = ( 1 << 2 );
	const U32	S_MASK = ( 1 << 3 );
	const U32	OUT_MASK = 0x0F;
	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp	=	__g_pRegister->AC97_CONFIG;
	temp	&=	~OUT_MASK;

	if( CTRUE == front ){ temp |= F_MASK; }
	if( CTRUE == rear	){ temp |= R_MASK; }
	if( CTRUE == center){ temp |= C_MASK; }
	if( CTRUE == spdif ){ temp |= S_MASK; }

//	__g_pRegister->AC97_CONFIG = (U16)temp;
	WriteIOW(&__g_pRegister->AC97_CONFIG, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of Output channel( FRONT, REAR, CENTER, SPDIF )
 *	@param[out]	front	CTRUE ( FRONT	output channel is Enabled ), CFALSE ( FRONT	output channel is Disabled )
 *	@param[out]	rear	CTRUE ( REAR	output channel is Enabled ), CFALSE ( REAR	output channel is Disabled )
 *	@param[out]	center	CTRUE ( CENTER output channel is Enabled ), CFALSE ( CENTER output channel is Disabled )
 *	@param[out]	spdif	CTRUE ( SPDIF	output channel is Enabled ), CFALSE ( SPDIF	output channel is Disabled )
 *	@return		None.
 *	@remarks	A parameter can set to CNULL, when the information is not need.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,
 *				NX_AUDIO_SetInputEnable,			NX_AUDIO_GetInputEnable
 */
void	NX_AUDIO_GetOutputEnable
(
	CBOOL* front,
	CBOOL* rear,
	CBOOL* center,
	CBOOL* spdif
)
{
	const U32	F_BITPOS = 0;
	const U32	R_BITPOS = 1;
	const U32	C_BITPOS = 2;
	const U32	S_BITPOS = 3;

	const U32	F_MASK = ( 1 << F_BITPOS );
	const U32	R_MASK = ( 1 << R_BITPOS );
	const U32	C_MASK = ( 1 << C_BITPOS );
	const U32	S_MASK = ( 1 << S_BITPOS );

	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp	=	__g_pRegister->AC97_CONFIG;

	if( CNULL != front ){ *front	= ( temp & F_MASK ) ? CTRUE : CFALSE; }
	if( CNULL != rear	){ *rear	= ( temp & R_MASK ) ? CTRUE : CFALSE; }
	if( CNULL != center){ *center	= ( temp & C_MASK ) ? CTRUE : CFALSE; }
	if( CNULL != spdif ){ *spdif	= ( temp & S_MASK ) ? CTRUE : CFALSE; }

	return;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Input channel( PCM, MIC, ADC0, ADC1 )
 *	@param[in]	pcm		CTRUE( PCM	input channel Enable ), CFALSE( PCM	input channel Disable )
 *	@param[in]	mic		CTRUE( MIC	input channel Enable ), CFALSE( MIC	input channel Disable )
 *	@param[in]	adc1	CTRUE( ADC1 input channel Enable ), CFALSE( ADC1 input channel Disable )
 *	@param[in]	adc2	CTRUE( ADC2 input channel Enable ), CFALSE( ADC2 input channel Disable )
 *	@return		None.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *													NX_AUDIO_GetInputEnable
 */
void	NX_AUDIO_SetInputEnable
(
	CBOOL pcm,
	CBOOL mic,
	CBOOL adc1,
	CBOOL adc2
)
{
	const U32	P_MASK	= ( 1 << 4 );
	const U32	M_MASK	= ( 1 << 5 );
	const U32	A1_MASK = ( 1 << 6 );
	const U32	A2_MASK = ( 1 << 7 );
	const U32	IN_MASK = ( 0x0F << 4 );
	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp	=	__g_pRegister->AC97_CONFIG;
	temp	&=	~IN_MASK;

	if( CTRUE == pcm	){ temp |= P_MASK; }
	if( CTRUE == mic	){ temp |= M_MASK; }
	if( CTRUE == adc1 ){ temp |= A1_MASK; }
	if( CTRUE == adc2 ){ temp |= A2_MASK; }

//	__g_pRegister->AC97_CONFIG = (U16)temp;
	WriteIOW(&__g_pRegister->AC97_CONFIG, (U16)temp);
}



//------------------------------------------------------------------------------
/**
 *	@brief		Get State of Input channel( PCM, MIC, ADC0, ADC1 )
 *	@param[out]	pcm	CTRUE ( PCM	input channel is Enabled ), CFALSE ( PCM	input channel is Disabled )
 *	@param[out]	mic	CTRUE ( MIC	input channel is Enabled ), CFALSE ( MIC	input channel is Disabled )
 *	@param[out]	adc1	CTRUE ( ADC1 input channel is Enabled ), CFALSE ( ADC1 input channel is Disabled )
 *	@param[out]	adc2	CTRUE ( ADC2 input channel is Enabled ), CFALSE ( ADC2 input channel is Disabled )
 *	@return		None.
 *	@remarks	A parameter can set to CNULL, when the information is not need.
 *	@see		NX_AUDIO_SetAC97FrontOutEnable,		NX_AUDIO_GetAC97FrontOutEnable,
 *				NX_AUDIO_SetAC97RearOutEnable,		NX_AUDIO_GetAC97RearOutEnable,
 *				NX_AUDIO_SetAC97CenterOutEnable,	NX_AUDIO_GetAC97CenterOutEnable,
 *				NX_AUDIO_SetAC97SPDIFOutEnable,		NX_AUDIO_GetAC97SPDIFOutEnable,
 *				NX_AUDIO_SetAC97PCMInEnable,		NX_AUDIO_GetAC97PCMInEnable,
 *				NX_AUDIO_SetAC97MICInEnable,		NX_AUDIO_GetAC97MICInEnable,
 *				NX_AUDIO_SetAC97ADC1InEnable,		NX_AUDIO_GetAC97ADC1InEnable,
 *				NX_AUDIO_SetAC97ADC2InEnable,		NX_AUDIO_GetAC97ADC2InEnable,
 *				NX_AUDIO_SetOutputEnable,			NX_AUDIO_GetOutputEnable,
 *				NX_AUDIO_SetInputEnable
 */
void	NX_AUDIO_GetInputEnable
(
	CBOOL* pcm,
	CBOOL* mic,
	CBOOL* adc1,
	CBOOL* adc2
)
{
	const U32	P_BITPOS	= 4 ;
	const U32	M_BITPOS	= 5 ;
	const U32	A1_BITPOS	= 6 ;
	const U32	A2_BITPOS	= 7 ;

	const U32	P_MASK	= ( 1 << P_BITPOS );
	const U32	M_MASK	= ( 1 << M_BITPOS );
	const U32	A1_MASK = ( 1 << A1_BITPOS );
	const U32	A2_MASK = ( 1 << A2_BITPOS );

	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp	=	__g_pRegister->AC97_CONFIG;

	if( CNULL != pcm	){ *pcm	= ( temp & P_MASK )	? CTRUE : CFALSE; }
	if( CNULL != mic	){ *mic	= ( temp & M_MASK )	? CTRUE : CFALSE; }
	if( CNULL != adc1 ){ *adc1 = ( temp & A1_MASK ) ? CTRUE : CFALSE; }
	if( CNULL != adc2 ){ *adc2 = ( temp & A2_MASK ) ? CTRUE : CFALSE; }

	return;
}

//------------------------------------------------------------------------------
// AC97 State Function
//------------------------------------------------------------------------------
/**
 *	@brief		Get State of AC97's State Machine
 *	@return		State of AC97's State Machine( idle, initialize, active, power down, warm reset)
 *	@remarks	Check state Using NX_AUDIO_AC97STATE.
 *	@code
 *		U32 state;
 *		state = NX_AUDIO_GetAC97State();
 *		if( NX_AUDIO_AC97STATE_IDLE & state ){ ... }
 *		if( NX_AUDIO_AC97STATE_ACTIVE & state ) { ... }
 *		...
 *	@endcode
 *	@see		NX_AUDIO_GetI2SState
 */
NX_AUDIO_AC97STATE	NX_AUDIO_GetAC97State( void )
{
	const U32 AC97_BITPOS	= 3;
	const U32 AC97_MASK		= ( 0x1F << AC97_BITPOS );

	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_AUDIO_AC97STATE)(( __g_pRegister->AUDIO_STATUS0 & AC97_MASK ) >> AC97_BITPOS );
}

//------------------------------------------------------------------------------
// AC97 Codec Control Operation
//------------------------------------------------------------------------------
/**
 *	@brief		Get Address of the last accessed AC97 Codec Register
 *	@return		Codec Address of accessed lastly
 *	@remarks	return value's range is 7bit ( 0 ~ 127 )
 *	@see											NX_AUDIO_GetAC97CodecWData,
 *				NX_AUDIO_GetAC97CodecRData,			NX_AUDIO_WriteAC97CodecReg,
 *				NX_AUDIO_ReadAC97CodecReg,			NX_AUDIO_WriteAC97CodecRegTime,
 *				NX_AUDIO_ReadAC97CodecRegTime
 */
U32 NX_AUDIO_GetAC97CodecAddr( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)__g_pRegister->AC97_CODEC_ADDR;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Data of last Writed Data to AC97 Codec
 *	@return		Codec Data of last Writed
 *	@remarks	return value's range is 16bit ( 0 ~ 0xFFFF )
 *	@see		NX_AUDIO_GetAC97CodecAddr,
 *				NX_AUDIO_GetAC97CodecRData,		NX_AUDIO_WriteAC97CodecReg,
 *				NX_AUDIO_ReadAC97CodecReg,		NX_AUDIO_WriteAC97CodecRegTime,
 *				NX_AUDIO_ReadAC97CodecRegTime
 */
U32 NX_AUDIO_GetAC97CodecWData( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)__g_pRegister->AC97_CODEC_WDATA;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Data of last Readed Data from AC97 CODEC
 *	@return		Codec Data of last readed
 *	@remarks	return value's range is 16bit ( 0 ~ 0xFFFF )
 *	@see		NX_AUDIO_GetAC97CodecAddr,			NX_AUDIO_GetAC97CodecWData,
 *													NX_AUDIO_WriteAC97CodecReg,
 *				NX_AUDIO_ReadAC97CodecReg,			NX_AUDIO_WriteAC97CodecRegTime,
 *				NX_AUDIO_ReadAC97CodecRegTime
 */
U32 NX_AUDIO_GetAC97CodecRData( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->AC97_CODEC_RDATA;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Write Data to AC97 Codec Register
 *	@param[in]	offset	offset of Codec Register ( 0 ~ 0xFF )
 *	@param[in]	value	Write Data ( 0 ~ 0xFFFF );
 *	@return		None.
 *	@remarks	Codec Register is accessed by offset.\n
 *				Offset value must even data. ( 0, 2, 4, 6... ).\n\n
 *				This function is directly write to memory that is mapped AC97 CODEC.\n
 *				So. User use WriteAC97CodecReg( U32 offset, U32 writeValue, U32 timeOut ) for Write AC97CODEC Register.
 *	@see		NX_AUDIO_GetAC97CodecAddr,		NX_AUDIO_GetAC97CodecWData,
 *				NX_AUDIO_GetAC97CodecRData,
 *				NX_AUDIO_ReadAC97CodecReg,		NX_AUDIO_WriteAC97CodecRegTime,
 *				NX_AUDIO_ReadAC97CodecRegTime
 */

void	NX_AUDIO_WriteAC97CodecReg
(
	U32 offset,
	U32 value
)
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0x100 > offset);
	NX_ASSERT( 1 != ( offset & 0x01 ) ) ;
	NX_ASSERT( 0x10000 > value );

//	__g_pRegister->AC97_CODEC_REG[offset/2] = (U16)value;
	WriteIOW(&__g_pRegister->AC97_CODEC_REG[offset/2], (U16)value);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Read Data From AC97 Codec Register
 *	@param[in]	offset	offset of Codec Register ( 0 ~ 0xFF )
 *	@return		Data of Codec Register
 *	@remarks	Codec Register is accessed by offset.\n
 *				Offset value must even data. ( 0, 2, 4, 6... ).\n\n
 *				This function is directly read from memory that is mapped AC97 CODEC.\n
 *				So. User use ReadAC97CodecReg( U32 offset, U32 writeValue, U32 timeOut ) for Read AC97CODEC Register.
 *	@see		NX_AUDIO_GetAC97CodecAddr,			NX_AUDIO_GetAC97CodecWData,
 *				NX_AUDIO_GetAC97CodecRData,			NX_AUDIO_WriteAC97CodecReg,
 *													NX_AUDIO_WriteAC97CodecRegTime,
 *				NX_AUDIO_ReadAC97CodecRegTime
 */

U32		NX_AUDIO_ReadAC97CodecReg
(
	U32 offset
)
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0x100 > offset);
	NX_ASSERT( 1 != ( offset & 0x01 ) ) ;

	return (U32)__g_pRegister->AC97_CODEC_REG[offset/2];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Write Data to AC97 Codec
 *	@param[in]	offset		Codec's register offset( 0 ~ 0xFF )
 *	@param[in]	writeValue	Write value of codec register ( 0 ~ 0xFFFF )
 *	@param[in]	timeOut		wait count ( 0 ~ 0xFFFF );
 *	@return		CTRUE indicate that Write is Successed.\n
 *				CFALSE indicate that Write is Failed.
 *	@remarks	User use this function for Write AC97CODEC Register.\n\n
 *				Codec Register is accessed by offset.\n
 *				Offset value must even data. ( 0, 2, 4, 6... ).\n\n
 *				TimeOut Value is used for wait time.\n
 *				If data is Not writed to Codec Register during TimeOut Value, then write is failed.
 *	@see		NX_AUDIO_GetAC97CodecAddr,		NX_AUDIO_GetAC97CodecWData,
 *				NX_AUDIO_GetAC97CodecRData,		NX_AUDIO_WriteAC97CodecReg,
 *				NX_AUDIO_ReadAC97CodecReg,
 *				NX_AUDIO_ReadAC97CodecRegTime
 */

CBOOL	NX_AUDIO_WriteAC97CodecRegTime
(
	U32 offset,
	U32 writeValue,
	U32 timeOut
)
{
	U32 count=0;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0x100 > offset);
	NX_ASSERT( 1 != ( offset & 0x01 ) ) ;
	NX_ASSERT( 0x10000 > writeValue );
	NX_ASSERT( 0x1000000 > timeOut );

//	NX_ASSERT( CTRUE == IsAC97CodecReady() );

	// write data
//	__g_pRegister->AC97_CODEC_REG[offset/2] = (U16)writeValue;
	WriteIOW(&__g_pRegister->AC97_CODEC_REG[offset/2], (U16)writeValue);

	// write done?
	count = 0;
	while( CTRUE != NX_AUDIO_IsAC97CodecWriteDone() )
	{
		count++;
		if( count == timeOut ){ return CFALSE; }
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Read Data From AC97 Codec
 *	@param[in]	offset		Codec's register offset( 0 ~ 0xFF )
 *	@param[in]	timeOut	wait count ( 0 ~ 0xFFFF );
 *	@return		Register value of Codec.\n
 *	@remarks	User use this function for Read AC97CODEC Register.\n\n
 *				Codec Register is accessed by offset.\n
 *				Offset value must even data. ( 0, 2, 4, 6... ).\n\n
 *				TimeOut Value is used for wait time.\n
 *				If data is Not read from Codec Register during TimeOut Value, then read is failed.
 *				If return value is 0xFFFFFFFF,then read is failed.
 *	@see		NX_AUDIO_GetAC97CodecAddr,		NX_AUDIO_GetAC97CodecWData,
 *				NX_AUDIO_GetAC97CodecRData,		NX_AUDIO_WriteAC97CodecReg,
 *				NX_AUDIO_ReadAC97CodecReg,		NX_AUDIO_WriteAC97CodecRegTime
 */

U32		NX_AUDIO_ReadAC97CodecRegTime
(
	U32 offset,
	U32 timeOut
)
{
	U32 count;
	U16 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0x100 > offset);
	NX_ASSERT( 1 != ( offset & 0x01 ) ) ;
	NX_ASSERT( 0x1000000 > timeOut );

	count = 0;

//	NX_ASSERT( CTRUE == IsAC97CodecReady() );

	// read data
	ReadValue = __g_pRegister->AC97_CODEC_REG[offset/2];

	ReadValue = ReadValue;

	// read done?
	count = 0;
	while( CTRUE != NX_AUDIO_IsAC97CodecReadDone() )
	{
		count++;
		if( count == timeOut ){ return 0xFFFFFFFF; }
	}

	// read data from controller
	return NX_AUDIO_GetAC97CodecRData();
}


//------------------------------------------------------------------------------
// Check of AC97 Codec's State
//------------------------------------------------------------------------------
/**
 *	@brief		Check AC97 Codec's Busy State
 *	@return		\b CTRUE indicates that Codec is busy.\n
 *				\b CFALSE indicates that Codec is idle.
 *	@see										NX_AUDIO_IsAC97CodecReadDone
 *				NX_AUDIO_IsAC97CodecWriteDone,	NX_AUDIO_IsAC97CodecReady
 */

CBOOL	NX_AUDIO_IsAC97CodecBusy( void )
{
	const U16	CODECBUSY_MASK	=	( 1 << 11 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( CODECBUSY_MASK & __g_pRegister->AUDIO_STATUS0 )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check AC97 Codec's Read state
 *	@return		\b CTRUE indicates that Codec's Register reading is completed.\n
 *				\b CFALSE indicates that Codec's Register reading is NOT completed.
 *	@see		NX_AUDIO_IsAC97CodecBusy,
 *				NX_AUDIO_IsAC97CodecWriteDone,	NX_AUDIO_IsAC97CodecReady
 */
CBOOL	NX_AUDIO_IsAC97CodecReadDone( void )
{
	const U16	READDONE_MASK	=	( 1 << 10 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( READDONE_MASK & __g_pRegister->AUDIO_STATUS0 )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check AC97 Codec's Write state
 *	@return		CTRUE indicate that Codec's Register writing is completed.\n
 *				CFALSE indicate that Codec's Register writing is NOT completed.
 *	@see		NX_AUDIO_IsAC97CodecBusy,		NX_AUDIO_IsAC97CodecReadDone
 *												NX_AUDIO_IsAC97CodecReady
 */
CBOOL	NX_AUDIO_IsAC97CodecWriteDone( void )
{
	const U16	WRITEDONE_MASK	=	( 1 << 9 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( WRITEDONE_MASK & __g_pRegister->AUDIO_STATUS0 )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check AC97 Codec's Ready State
 *	@return		CTRUE indicate that Codec is Ready.\n
 *				CFALSE indicate that Codec is NOT Ready.
 *	@see		NX_AUDIO_IsAC97CodecBusy,		NX_AUDIO_IsAC97CodecReadDone
 *				NX_AUDIO_IsAC97CodecWriteDone
 */

CBOOL	NX_AUDIO_IsAC97CodecReady( void )
{
	const U16	CODECREADY_MASK	=	( 1 << 8 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( CODECREADY_MASK & __g_pRegister->AUDIO_STATUS0 )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//--------------------------------------------------------------------------
// I2S Configuration Function
//--------------------------------------------------------------------------
/**
 *	@brief		Set I2S's Controller Mode ( Master or Slave )
 *	@param[in]	Enable	\b CTRUE indicates that Master mode. \n
 *						\b CFALSE indicates that Slave mode.
 *	@return		None.
 *	@see		NX_AUDIO_GetI2SMasterMode,
 *				NX_AUDIO_SetSyncPeriod,		NX_AUDIO_GetSyncPeriod,
 *				NX_AUDIO_SetInterfaceMode,	NX_AUDIO_GetInterfaceMode
 */
void	NX_AUDIO_SetI2SMasterMode( CBOOL Enable )
{
	const U32	MST_SLV_POS		=	0;
	const U32	MST_SLV_MASK	=	1UL << MST_SLV_POS;
	register U32 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->I2S_CONFIG;

	ReadValue	&=	~MST_SLV_MASK;

	if( CFALSE == Enable )
	{
		ReadValue	|=	MST_SLV_MASK;
	}

//	__g_pRegister->I2S_CONFIG	=	(U16)ReadValue;
	WriteIOW(&__g_pRegister->I2S_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get I2S's Controller Mode ( Master or Slave )
 *	@return		\b CTRUE indicates that Master mode.\n
 *				\b CFALSE indicates that Slave mode.
 *	@see		NX_AUDIO_SetI2SMasterMode,
 *				NX_AUDIO_SetSyncPeriod,		NX_AUDIO_GetSyncPeriod,
 *				NX_AUDIO_SetInterfaceMode,	NX_AUDIO_GetInterfaceMode
 */
CBOOL	NX_AUDIO_GetI2SMasterMode( void )
{
	const U32	MST_SLV_POS		=	0;
	const U32	MST_SLV_MASK	=	1UL << MST_SLV_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	if(__g_pRegister->I2S_CONFIG & MST_SLV_MASK)
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set audio interface mode.
 *	@param[in]	mode	Interface mode ( 0 : I2S, 1 : None, 2 : Left-Justified, 3 : Right-Justified )
 *	@return		None.
 *	@see		NX_AUDIO_SetI2SMasterMode,	NX_AUDIO_GetI2SMasterMode,
 *				NX_AUDIO_GetInterfaceMode
 *				NX_AUDIO_SetSyncPeriod,		NX_AUDIO_GetSyncPeriod,
 */
void	NX_AUDIO_SetInterfaceMode( NX_AUDIO_IF mode )
{
	const U32	IF_MODE_POS		=	6;
	const U32	IF_MODE_MASK	=	0x03UL << IF_MODE_POS;
	register U32 ReadValue;

	NX_ASSERT( (NX_AUDIO_IF_I2S==mode) || (NX_AUDIO_IF_LEFT==mode) || (NX_AUDIO_IF_RIGHT==mode) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->I2S_CONFIG;

	ReadValue	&=	~IF_MODE_MASK;
	ReadValue	|=	(U32)mode << IF_MODE_POS;

//	__g_pRegister->I2S_CONFIG	=	(U16)ReadValue;
	WriteIOW(&__g_pRegister->I2S_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of audio interface mode.
 *	@return		Interface mode ( 0 : I2S, 1 : None, 2 : Left-Justified, 3 : Right-Justified	)
 *	@see		NX_AUDIO_SetI2SMasterMode,	NX_AUDIO_GetI2SMasterMode,
 *				NX_AUDIO_SetInterfaceMode,
 *				NX_AUDIO_SetSyncPeriod,		NX_AUDIO_GetSyncPeriod
 */
NX_AUDIO_IF	NX_AUDIO_GetInterfaceMode( void )
{
	const U32	IF_MODE_POS		=	6;
	const U32	IF_MODE_MASK	=	0x03UL << IF_MODE_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(NX_AUDIO_IF)( (__g_pRegister->I2S_CONFIG & IF_MODE_MASK) >> IF_MODE_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Sync Period of I2S ( 32fs, 48fs, 64fs )
 *	@param[in]	period		Sync Period ( 32fs, 48fs, 64fs )
 *	@return		None.
 *	@remarks	The setting is required in mater mode also.
 *	@see		NX_AUDIO_SetI2SMasterMode,	NX_AUDIO_GetI2SMasterMode,
 *				NX_AUDIO_SetInterfaceMode,	NX_AUDIO_GetInterfaceMode
 *				NX_AUDIO_GetSyncPeriod,
 */
void	NX_AUDIO_SetSyncPeriod( U32 period )
{
	const U32	SYNC_PERIOD_POS		=	4;
	const U32	SYNC_PERIOD_MASK	=	0x03UL << SYNC_PERIOD_POS;

	register U32 ReadValue;
	register U32 SetValue=0;

	NX_ASSERT( (32==period) ||(48==period) || (64==period) );
	NX_ASSERT( CNULL != __g_pRegister );

	switch( period )
	{
		case	32: SetValue = 0;	break;
		case	48: SetValue = 1;	break;
		case	64: SetValue = 2;	break;
		default:	NX_ASSERT( CFALSE );
	}

	ReadValue	=	__g_pRegister->I2S_CONFIG;

	ReadValue	&=	~SYNC_PERIOD_MASK;
	ReadValue	|=	(U32)SetValue << SYNC_PERIOD_POS;

//	__g_pRegister->I2S_CONFIG	=	(U16)ReadValue;
	WriteIOW(&__g_pRegister->I2S_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Sync Period of I2S ( 32fs, 48fs, 64fs )
 *	@return		Sync Period ( 32fs, 48fs, 64fs )
 *	@see		NX_AUDIO_SetI2SMasterMode,	NX_AUDIO_GetI2SMasterMode,
 *				NX_AUDIO_SetInterfaceMode,	NX_AUDIO_GetInterfaceMode,
 *				NX_AUDIO_SetSyncPeriod
 */
U32		NX_AUDIO_GetSyncPeriod( void )
{
	const U32	SYNC_PERIOD_POS		=	4;
	const U32	SYNC_PERIOD_MASK	=	0x03UL << SYNC_PERIOD_POS;

	register U32 ReadValue;
	register U32 RetValue=0;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = ((__g_pRegister->I2S_CONFIG & SYNC_PERIOD_MASK) >> SYNC_PERIOD_POS );

	switch( ReadValue )
	{
		case	0: RetValue = 32;	break;
		case	1: RetValue = 48;	break;
		case	2: RetValue = 64;	break;
		default:	NX_ASSERT( CFALSE );
	}

	return RetValue;
}

//--------------------------------------------------------------------------
// I2S Control Function
//--------------------------------------------------------------------------
/**
 *	@brief		Set I2S controller Link On
 *	@return		None.
 *	@remarks	If user want I2S link off, reset the I2S controller
 *	@see											NX_AUDIO_GetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,		NX_AUDIO_GetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,		NX_AUDIO_GetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,			NX_AUDIO_GetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable,		NX_AUDIO_GetI2SLoopBackEnable
 */
void	NX_AUDIO_SetI2SLinkOn( void )
{
	const U32 I2SLINK_RUN_MASK	= 1UL << 1;

	register struct NX_AUDIO_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	ReadValue	=	pRegister->I2S_CTRL;

	ReadValue	|=	I2SLINK_RUN_MASK;

//	pRegister->I2S_CTRL =	(U16)ReadValue;
	WriteIOW(&pRegister->I2S_CTRL,	(U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get link status of I2S controller
 *	@return		\b CTRUE indicate that Link On. \n
 *				\b CFALSE indicate that None.
 *	@see		NX_AUDIO_SetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,		NX_AUDIO_GetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,		NX_AUDIO_GetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,			NX_AUDIO_GetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable,		NX_AUDIO_GetI2SLoopBackEnable
 */
CBOOL NX_AUDIO_GetI2SLinkOn( void )
{
	const U32 I2SLINK_RUN_POS	= 1;
	const U32 I2SLINK_RUN_MASK	= 1UL << I2SLINK_RUN_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->I2S_CTRL & I2SLINK_RUN_MASK) ? CTRUE : CFALSE;
}

 //------------------------------------------------------------------------------
/**
 *	@brief		I2S Controller Reset
 *	@param[in]	Enable		\b CTRUE indicates that Contoller Reset.\n
 *							\b CFALSE indicates that Nomal Operation.
 *	@return		None.
 *	@remarks	After Reset You should set normal operation
 *	@code
 *				NX_AUDIO_SetI2SControllerReset( CTRUE );	// I2S Controller Reset
 *				NX_AUDIO_SetI2SControllerReset( CFALSE );	// Normal Operation
 *	@endcode
 *	@see		NX_AUDIO_SetI2SLinkOn,				NX_AUDIO_GetI2SLinkOn,
 *													NX_AUDIO_GetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,		NX_AUDIO_GetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,			NX_AUDIO_GetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable,		NX_AUDIO_GetI2SLoopBackEnable
 */
void	NX_AUDIO_SetI2SControllerReset( CBOOL Enable )
{
	const U32 I2S_EN_MASK	= 1UL << 0;

	register struct NX_AUDIO_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	ReadValue	=	pRegister->I2S_CTRL;

	ReadValue	&=	~I2S_EN_MASK;

	if( Enable )
	{
		ReadValue = 0x00;
	}
	else
	{
		ReadValue	|=	(U32)(Enable^1);	// exclusive OR ( 0=>1, 1=>0 )
	}

//	pRegister->I2S_CTRL	=	(U16)ReadValue;
	WriteIOW(&pRegister->I2S_CTRL, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of I2S Controller
 *	@return		\b CTRUE indicates that Contoller Reset.\n
 *				\b CFALSE indicates that Nomal Operation.
 *	@see		NX_AUDIO_SetI2SLinkOn,				NX_AUDIO_GetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,		NX_AUDIO_GetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,			NX_AUDIO_GetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable,		NX_AUDIO_GetI2SLoopBackEnable
 */
CBOOL NX_AUDIO_GetI2SControllerReset( void )
{
	const U32 I2S_EN_MASK	= 1UL << 0;

	NX_ASSERT( CNULL != __g_pRegister );

	if(__g_pRegister->I2S_CTRL & I2S_EN_MASK)
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set I2S controller's output operation
 *	@param[in]	Enable	\b CTRUE indicates that Output Enable. \n
 *						\b CFALSE indicates that Output Disable.
 *	@return		None.
 *	@see		NX_AUDIO_SetI2SLinkOn,				NX_AUDIO_GetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,		NX_AUDIO_GetI2SControllerReset,
 *													NX_AUDIO_GetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,			NX_AUDIO_GetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable,		NX_AUDIO_GetI2SLoopBackEnable
 */
void	NX_AUDIO_SetI2SOutputEnable( CBOOL Enable )
{
	const U32 I2SO_EN_POS	= 1;
	const U32 I2SO_EN_MASK	= 1UL << I2SO_EN_POS;

	register struct NX_AUDIO_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	ReadValue	=	pRegister->I2S_CONFIG;

	ReadValue	&=	~I2SO_EN_MASK;
	ReadValue	|=	(U32)Enable << I2SO_EN_POS;

//	pRegister->I2S_CONFIG	=	(U16)ReadValue;
	WriteIOW(&pRegister->I2S_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get output operation status of I2S controller
 *	@return		\b CTRUE indicate that Output is Enabled. \n
 *				\b CFALSE indicate that Output is Disabled.
 *	@see		NX_AUDIO_SetI2SLinkOn,				NX_AUDIO_GetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,		NX_AUDIO_GetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,			NX_AUDIO_GetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable,		NX_AUDIO_GetI2SLoopBackEnable
 */
CBOOL NX_AUDIO_GetI2SOutputEnable( void )
{
	const U32 I2SO_EN_POS	= 1;
	const U32 I2SO_EN_MASK	= 1UL << I2SO_EN_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(__g_pRegister->I2S_CONFIG & I2SO_EN_MASK) ? CTRUE : CFALSE ;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set I2S controller's input operation
 *	@param[in]	Enable	\b CTRUE indicates that Input Enable. \n
 *						\b CFALSE indicates that Input Disable.
 *	@see		NX_AUDIO_SetI2SLinkOn,				NX_AUDIO_GetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,		NX_AUDIO_GetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,		NX_AUDIO_GetI2SOutputEnable,
 *													NX_AUDIO_GetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable,		NX_AUDIO_GetI2SLoopBackEnable
 */
void	NX_AUDIO_SetI2SInputEnable( CBOOL Enable )
{
	const U32 I2SI_EN_POS	= 2;
	const U32 I2SI_EN_MASK	= 1UL << I2SI_EN_POS;

	register struct NX_AUDIO_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	ReadValue	=	pRegister->I2S_CONFIG;

	ReadValue	&=	~I2SI_EN_MASK;
	ReadValue	|=	(U32)Enable << I2SI_EN_POS;

//	pRegister->I2S_CONFIG	=	(U16)ReadValue;
	WriteIOW(&pRegister->I2S_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Input operation status of I2S controller
 *	@return		\b CTRUE indicate that Input is Enabled. \n
 *				\b CFALSE indicate that Input is Disabled.
 *	@see		NX_AUDIO_SetI2SLinkOn,				NX_AUDIO_GetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,		NX_AUDIO_GetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,		NX_AUDIO_GetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable,		NX_AUDIO_GetI2SLoopBackEnable
 */
CBOOL NX_AUDIO_GetI2SInputEnable( void )
{
	const U32 I2SI_EN_POS	= 2;
	const U32 I2SI_EN_MASK	= 1UL << I2SI_EN_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(__g_pRegister->I2S_CONFIG & I2SI_EN_MASK) ? CTRUE : CFALSE ;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Loop Back operation
 *	@param[in]	Enable	\b CTRUE indicates that Loop Back mode Enable. \n
 *						\b CFALSE indicates that Loop Back mode Disable.
 *	@return		None.
 *	@remarks	User need to set NX_AUDIO_SetI2SInputEnable( CTRUE ) for look back operation.
 *	@see		NX_AUDIO_SetI2SLinkOn,				NX_AUDIO_GetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,		NX_AUDIO_GetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,		NX_AUDIO_GetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,			NX_AUDIO_GetI2SInputEnable,
 *													NX_AUDIO_GetI2SLoopBackEnable
 */
void	NX_AUDIO_SetI2SLoopBackEnable( CBOOL Enable )
{
	const U32 LOOP_BACK_POS		= 3;
	const U32 LOOP_BACK_MASK	= 1UL << LOOP_BACK_POS;

	register struct NX_AUDIO_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	ReadValue	=	pRegister->I2S_CONFIG;

	ReadValue	&=	~LOOP_BACK_MASK;
	ReadValue	|=	(U32)Enable << LOOP_BACK_POS;

//	pRegister->I2S_CONFIG	=	(U16)ReadValue;
	WriteIOW(&pRegister->I2S_CONFIG, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of Loop Back operation
 *	@return		\b CTRUE indicates that Loop Back mode is Enabled.\n
 *				\b CFALSE indicates that Loop Back mode is Disabled.
 *	@see		NX_AUDIO_SetI2SLinkOn,				NX_AUDIO_GetI2SLinkOn,
 *				NX_AUDIO_SetI2SControllerReset,		NX_AUDIO_GetI2SControllerReset,
 *				NX_AUDIO_SetI2SOutputEnable,		NX_AUDIO_GetI2SOutputEnable,
 *				NX_AUDIO_SetI2SInputEnable,			NX_AUDIO_GetI2SInputEnable,
 *				NX_AUDIO_SetI2SLoopBackEnable
 */
CBOOL NX_AUDIO_GetI2SLoopBackEnable( void )
{
	const U32 LOOP_BACK_POS		= 3;
	const U32 LOOP_BACK_MASK	= 1UL << LOOP_BACK_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(__g_pRegister->I2S_CONFIG & LOOP_BACK_MASK) ? CTRUE : CFALSE;
}

//--------------------------------------------------------------------------
// I2S State Function
//--------------------------------------------------------------------------
/**
 *	@brief		Get I2S controller's state.
 *	@return		I2S controller's state ( NX_AUDIO_I2SSTATE_IDLE, NX_AUDIO_I2SSTATE_READY, NX_AUDIO_I2SSTATE_RUN )
 *	@remarks	Idle	: controller OFF, link OFF. \n
 *				ready : controller ON, link OFF. \n
 *				run	: controller ON, link ON.	\n
 *	@code
 *			State = NX_AUDIO_GetI2SState();
 *
 *			if( State & NX_AUDIO_I2SSTATE_IDLE )
 *			{
 *				// I2S's current state is IDLE...
 *			}
 *			...
 *	@endcode
 */
NX_AUDIO_I2SSTATE	NX_AUDIO_GetI2SState( void )
{
	const U32 I2S_FSM_MASK	=	0x07 << 0;

	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_AUDIO_I2SSTATE)(__g_pRegister->AUDIO_STATUS0 & I2S_FSM_MASK );
}

U32 * NX_AUDIO_I2S_GetDirectPCMOUTDataRegPtr( void )
{
	NX_ASSERT( CNULL != __g_pRegister );
	return (U32*)&__g_pRegister->PCMOUTDATA;
}
void NX_AUDIO_I2S_DirectPCMOUTDataWrite( U16 Data )
{
	NX_ASSERT( CNULL != __g_pRegister );
	WriteIOW(&__g_pRegister->PCMOUTDATA, Data);
}

U32 NX_AUDIO_I2S_DirectPCMOUTDataArrayWrite( U32 Size, U16* pData)
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pData );
	NX_ASSERT( 64 >= Size );	// output fifo depth is 64
	while(!NX_AUDIO_I2S_IsOutFifoFull())
	{
		WriteIOW(&__g_pRegister->PCMOUTDATA, *pData++);
		Size--;
	}
	return Size;
}
CBOOL NX_AUDIO_I2S_IsOutFifoFull( void )
{
	const U16 I2S_PCMOUTFIFOFULL_BIT = 1;
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((ReadIOW(&__g_pRegister->PCMOUT_STATUS) >> I2S_PCMOUTFIFOFULL_BIT) & 0x0001);
}

CBOOL NX_AUDIO_I2S_IsOutFifoEmpty( void )
{
	const U16 I2S_PCMOUTFIFOEMPTY_BIT = 0;
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((ReadIOW(&__g_pRegister->PCMOUT_STATUS) >> I2S_PCMOUTFIFOEMPTY_BIT) & 0x0001);
}

