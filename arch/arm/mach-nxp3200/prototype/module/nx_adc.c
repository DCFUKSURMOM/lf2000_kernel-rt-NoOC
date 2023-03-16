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
//	Module		: ADC
//	File		: nx_adc.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_adc.h"

static	struct NX_ADC_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize of prototype enviroment & local variables.
 *	@return		\b CTRUE indicate that	Initialize is successed.\n
 *				\b CFALSE indicate that	Initialize is failed.\n
 *	@see		NX_ADC_GetNumberOfModule
 */
CBOOL	NX_ADC_Initialize( void )
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
 *	@see		NX_ADC_Initialize
 */
U32		NX_ADC_GetNumberOfModule( void )
{
	return NUMBER_OF_ADC_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see										NX_ADC_GetSizeOfRegisterSet,
 *				NX_ADC_SetBaseAddress,			NX_ADC_GetBaseAddress,
 *				NX_ADC_OpenModule,				NX_ADC_CloseModule,
 *				NX_ADC_CheckBusy,				NX_ADC_CanPowerDown
 */
U32		NX_ADC_GetPhysicalAddress( void )
{
	return	(U32)(PHY_BASEADDR_ADC_MODULE);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_ADC_GetPhysicalAddress,
 *				NX_ADC_SetBaseAddress,			NX_ADC_GetBaseAddress,
 *				NX_ADC_OpenModule,				NX_ADC_CloseModule,
 *				NX_ADC_CheckBusy,				NX_ADC_CanPowerDown
 */
U32		NX_ADC_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_ADC_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_ADC_GetPhysicalAddress,		NX_ADC_GetSizeOfRegisterSet,
 *												NX_ADC_GetBaseAddress,
 *				NX_ADC_OpenModule,				NX_ADC_CloseModule,
 *				NX_ADC_CheckBusy,				NX_ADC_CanPowerDown
 */
void	NX_ADC_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_ADC_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_ADC_GetPhysicalAddress,		NX_ADC_GetSizeOfRegisterSet,
 *				NX_ADC_SetBaseAddress,
 *				NX_ADC_OpenModule,				NX_ADC_CloseModule,
 *				NX_ADC_CheckBusy,				NX_ADC_CanPowerDown
 */
U32		NX_ADC_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE indicate that	Initialize is successed. \n
 *				\b CFALSE indicate that	Initialize is failed.
 *	@see		NX_ADC_GetPhysicalAddress,		NX_ADC_GetSizeOfRegisterSet,
 *				NX_ADC_SetBaseAddress,			NX_ADC_GetBaseAddress,
 *												NX_ADC_CloseModule,
 *				NX_ADC_CheckBusy,				NX_ADC_CanPowerDown
 */
CBOOL	NX_ADC_OpenModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE indicate that	Deinitialize is successed. \n
 *				\b CFALSE indicate that	Deinitialize is failed.
 *	@see		NX_ADC_GetPhysicalAddress,		NX_ADC_GetSizeOfRegisterSet,
 *				NX_ADC_SetBaseAddress,			NX_ADC_GetBaseAddress,
 *				NX_ADC_OpenModule,
 *				NX_ADC_CheckBusy,				NX_ADC_CanPowerDown
 */
CBOOL	NX_ADC_CloseModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE indicate that	Module is Busy. \n
 *				\b CFALSE indicate that	Module is NOT Busy.
 *	@see		NX_ADC_GetPhysicalAddress,		NX_ADC_GetSizeOfRegisterSet,
 *				NX_ADC_SetBaseAddress,			NX_ADC_GetBaseAddress,
 *				NX_ADC_OpenModule,				NX_ADC_CloseModule,
 *												NX_ADC_CanPowerDown
 */
CBOOL	NX_ADC_CheckBusy( void )
{
	const U32	ADEN_BITPOS	=	0;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->ADCCON >> ADEN_BITPOS) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE indicate that	Ready to enter power-down stage. \n
 *				\b CFALSE indicate that	This module can't enter to power-down stage.
 *	@see		NX_ADC_GetPhysicalAddress,		NX_ADC_GetSizeOfRegisterSet,
 *				NX_ADC_SetBaseAddress,			NX_ADC_GetBaseAddress,
 *				NX_ADC_OpenModule,				NX_ADC_CloseModule,
 *				NX_ADC_CheckBusy
 */
CBOOL	NX_ADC_CanPowerDown( void )
{
	if( NX_ADC_IsBusy() )
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see												NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
S32		NX_ADC_GetInterruptNumber( void )
{
	return	INTNUM_OF_ADC_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0 ).
 *	@param[in]	Enable	\b CTRUE indicate that	Interrupt Enable. \n
 *						\b CFALSE indicate that	Interrupt Disable.
 *	@return		None.
 *	@remarks	ADC Module Only have one interrupt. So always \e IntNum set to 0.
 *	@see		NX_ADC_GetInterruptNumber,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
void	NX_ADC_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	const U16 ADCINTENB_MASK	= 0x01;
	const U16 ADCINTENB_POS		= 0;

	register struct NX_ADC_RegisterSet*	pRegister;
	register U16 regvalue;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = pRegister->ADCINTENB;

	regvalue &= ~ADCINTENB_MASK;
	regvalue |= ((U16)Enable << ADCINTENB_POS);

//	pRegister->ADCINTENB = regvalue;
	WriteIOW(&pRegister->ADCINTENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number ( 0 ).
 *	@return		\b CTRUE indicate that	Interrupt is enabled. \n
 *				\b CFALSE indicate that	Interrupt is disabled.
 *	@remarks	ADC Module Only have one interrupt. So always \e IntNum set to 0.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *														NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
CBOOL	NX_ADC_GetInterruptEnable( S32 IntNum )
{
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->ADCINTENB & 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set ADC interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
void	NX_ADC_SetInterruptEnable32( U32 EnableFlag )
{
	const U32	ENB_POS		= 0;
	const U32	ENB_MASK	= 0x01 << ENB_POS;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->ADCINTENB = (U16)(EnableFlag & ENB_MASK);
	WriteIOW(&__g_pRegister->ADCINTENB, (U16)(EnableFlag & ENB_MASK));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : ADC interrupt's setting value. \n
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *														NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
U32		NX_ADC_GetInterruptEnable32( void )
{
	const U32	ENB_POS		= 0;
	const U32	ENB_MASK	= 0x01 << ENB_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->ADCINTENB & ENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number ( 0 ).
 *	@return		\b CTRUE indicate that	Pending is seted. \n
 *				\b CFALSE indicate that	Pending is Not Seted.
 *	@remarks	ADC Module Only have one interrupt. So always \e IntNum set to 0.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
CBOOL	NX_ADC_GetInterruptPending( S32 IntNum )
{
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->ADCINTCLR & 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : ADC pending state. \n
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *														NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
U32		NX_ADC_GetInterruptPending32( void )
{
	const U32 PEND_MASK = 0x01;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->ADCINTCLR & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number ( 0 ).
 *	@return		None.
 *	@remarks	ADC Module Only have one interrupt. So always \e IntNum set to 0.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
void	NX_ADC_ClearInterruptPending( S32 IntNum )
{
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->ADCINTCLR = 0x01;
	WriteIOW(&__g_pRegister->ADCINTCLR, 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : ADC pending bit. \n
 *	@return		None.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *														NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
void	NX_ADC_ClearInterruptPending32( U32 PendingFlag )
{
	const U32 PEND_MASK = 0x01;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->ADCINTCLR = (U16)(PendingFlag & PEND_MASK);
	WriteIOW(&__g_pRegister->ADCINTCLR, (U16)(PendingFlag & PEND_MASK));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE indicate that	Set to all interrupt enable. \n
 *						\b CFALSE indicate that	Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
void	NX_ADC_SetInterruptEnableAll( CBOOL Enable )
{
	const U16 ADCINTENB_MASK	= 0x01;
	const U16 ADCINTENB_POS		= 0;

	register struct NX_ADC_RegisterSet*	pRegister;
	register U16 regvalue;

	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = pRegister->ADCINTENB;

	regvalue &= ~ADCINTENB_MASK;
	regvalue |= ((U16)Enable << ADCINTENB_POS);

//	pRegister->ADCINTENB = regvalue;
	WriteIOW(&pRegister->ADCINTENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE indicate that	At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE indicate that	All interrupt is disabled.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *														NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
CBOOL	NX_ADC_GetInterruptEnableAll( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->ADCINTENB & 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE indicate that	At least one( or more ) pending is seted. \n
 *				\b CFALSE indicate that	All pending is NOT seted.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,
 *				NX_ADC_ClearInterruptPendingAll,		NX_ADC_GetInterruptPendingNumber
 */
CBOOL	NX_ADC_GetInterruptPendingAll( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->ADCINTCLR & 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *														NX_ADC_GetInterruptPendingNumber
 */
void	NX_ADC_ClearInterruptPendingAll( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->ADCINTCLR = 0x01;
	WriteIOW(&__g_pRegister->ADCINTCLR, 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_ADC_GetInterruptNumber,				NX_ADC_SetInterruptEnable,
 *				NX_ADC_GetInterruptEnable,				NX_ADC_SetInterruptEnable32,
 *				NX_ADC_GetInterruptEnable32,			NX_ADC_GetInterruptPending,
 *				NX_ADC_GetInterruptPending32,			NX_ADC_ClearInterruptPending,
 *				NX_ADC_ClearInterruptPending32,			NX_ADC_SetInterruptEnableAll,
 *				NX_ADC_GetInterruptEnableAll,			NX_ADC_GetInterruptPendingAll,
 *				NX_ADC_ClearInterruptPendingAll
 */
S32		NX_ADC_GetInterruptPendingNumber( void )	// -1 if None
{
	register struct NX_ADC_RegisterSet	*pRegister;
	register U32 Pend;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	Pend	= (pRegister->ADCINTENB & pRegister->ADCINTCLR);

	if( Pend & 0x01 )
	{
		return 0;
	}

	return -1;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see		NX_ADC_GetClockPClkMode,
 */
void	NX_ADC_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_ADC_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
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
 *	@see		NX_ADC_SetClockPClkMode,
 */
NX_PCLKMODE	NX_ADC_GetClockPClkMode( void )
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
// ADC Operation.
//------------------------------------------------------------------------------
/**
 *	@brief		Set Clock Prescaler Value of A/D Converter
 *	@param[in]	value		Value of Prescaler ( Range 20 ~ 256 )
 *	@return		None.
 *	@remarks	This Function must be set before SetPrescalerEnable( ) Function.\n
 *				Max ADC Clock is 2.5Mhz(400ns) when Prescaler Value is 256.
 *	@code
 *				NX_ADC_SetPrescalerValue(0x256);
 *				NX_ADC_SetPrescalerEnable(CTRUE);
 *	@endcode
 *	@see									NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
void	NX_ADC_SetPrescalerValue( U32 value )
{
	const U32	APSV_MASK	= ( 0xFFUL << 6 );
	const U32	APSV_BITPOS = 6;

	register		struct NX_ADC_RegisterSet*	pRegister;
	register U32	regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( (256 >= value) && (20 <= value) );

	pRegister = __g_pRegister;

	regvalue	=	pRegister->ADCCON;

	regvalue = ( regvalue & ~APSV_MASK ) | ( (value-1) << APSV_BITPOS ) ;

//	pRegister->ADCCON =	(U16)regvalue;
	WriteIOW(&pRegister->ADCCON, (U16)regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Prescaler Value of A/D Converter
 *	@return		Value of Prescaler
 *	@see		NX_ADC_SetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
U32		NX_ADC_GetPrescalerValue( void )
{
	const	U32	APSV_MASK	= ( 0xFF << 6 );
	const	U32	APSV_BITPOS = 6;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((( __g_pRegister->ADCCON & APSV_MASK ) >> APSV_BITPOS ) + 1 ) ;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Prescaler Enable
 *	@param[in]	enable	\b CTRUE indicate that Prescaler Enable. \n
 *						\b CFALSE indicate that Prescaler Disable.
 *	@return		None.
 *	@remarks	This function is set after SetPrescalerValue() function
 *	@code
 *				NX_ADC_SetPrescalerValue(256);
 *				NX_ADC_SetPrescalerEnable(CTRUE);
 *	@endcode
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *											NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
void	NX_ADC_SetPrescalerEnable( CBOOL enable )
{
	const	U32	APEN_MASK	=	( 0x01UL << 14 );
	const	U32	APEN_POS	=	14;
	register U32	regvalue;
	register struct NX_ADC_RegisterSet*	pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( (0==enable) || (1==enable) );

	pRegister = __g_pRegister;

	regvalue = pRegister->ADCCON;

	regvalue &= ~APEN_MASK;
	regvalue |= (U32)enable << APEN_POS;

//	pRegister->ADCCON = (U16)regvalue;
	WriteIOW(&pRegister->ADCCON, (U16)regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Prescaler is enabled or disabled
 *	@return		\b CTRUE indicate that Prescaler is Enabled.\n
 *				\b CFALSE indicate that Prescaler is Disabled.
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
CBOOL	NX_ADC_GetPrescalerEnable( void )
{
	const	U32	APEN_POS	=	14;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( ( __g_pRegister->ADCCON >> APEN_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Input Channel
 *	@param[in]	channel		Value of Input Channel ( 0 ~ 7 )
 *	@return		None.
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *											NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
void	NX_ADC_SetInputChannel( U32 channel )
{
	const	U32	ASEL_MASK	=	( 0x07 << 3 );
	const	U32	ASEL_BITPOS	=	3;
	register struct NX_ADC_RegisterSet*	pRegister;
	register U32	regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 8 > channel );

	pRegister = __g_pRegister;

	regvalue	=	pRegister->ADCCON;

	regvalue	=	( regvalue & ~ASEL_MASK ) | ( channel << ASEL_BITPOS );

//	pRegister->ADCCON =	(U16)regvalue;
	WriteIOW(&pRegister->ADCCON, (U16)regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Input Channel
 *	@return		Value of Input Channel ( 0 ~ 7 )
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
U32		NX_ADC_GetInputChannel( void )
{
	const	U32	ASEL_MASK	=	( 0x07 << 3 );
	const	U32	ASEL_BITPOS	=	3;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)( ( __g_pRegister->ADCCON & ASEL_MASK ) >> ASEL_BITPOS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Standby Mode
 *	@param[in]	enable	\b CTRUE indicate that Standby Mode ON. \n
 *						\b CFALSE indicate that Standby Mode OFF.
 *	@return		None.
 *	@remark		If Standby Mode is enabled then ADC power is cut offed.\n
 *				You have to disable the standby mode to use ADC.
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *											NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
void	NX_ADC_SetStandbyMode( CBOOL enable )
{
	const	U32 STBY_MASK	= ( 0x01UL << 2 );
	const	U32 STBY_POS	= 2;
	register U32 regvalue;
	register struct NX_ADC_RegisterSet*	pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( (0==enable) || (1==enable) );

	pRegister = __g_pRegister;

	regvalue = pRegister->ADCCON;

	regvalue &= ~STBY_MASK;
	regvalue |= (U32)enable << STBY_POS;

//	pRegister->ADCCON = regvalue;
	WriteIOW(&pRegister->ADCCON, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get ADC's Standby Mode
 *	@return		\b CTRUE indicate that Standby Mode is Enabled.\n
 *				\b CFALSE indicate that Standby Mode is Disabled.
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
CBOOL	NX_ADC_GetStandbyMode( void )
{
	const	U32 STBY_MASK	= ( 0x01UL << 2 );
	const	U32 STBY_POS	= 2;

	return (CBOOL)( ( __g_pRegister->ADCCON & STBY_MASK ) >> STBY_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		ADC Start
 *	@return		None.
 *	@remarks	Sequence of ADC operation
 *	@code
 *		NX_ADC_SetStandbyMode( CFALSE );		// Standby mode disable
 *		...
 *		NX_ADC_SetPrescalerValue( 256 );		// Set prescaler value ( 20 ~ 256 )
 *		NX_ADC_SetPrescalerEnable( CTRUE );		// Prescaler enable
 *		NX_ADC_SetInputChannel( 0 );			// Set input channel
 *		...
 *		NX_ADC_Start();							// Start ADC converting
 *		...
 *		while( NX_ADC_IsBusy() ){ }				// Wait during ADC converting
 *		...
 *		Value = NX_ADC_GetConvertedData();		// Read Converted ADC data
 *
 *	@endcode
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *											NX_ADC_IsBusy,
 *				NX_ADC_GetConvertedData
 */
void	NX_ADC_Start( void )
{
	const	U32	ADEN_MASK = ( 0x01 << 0 );
	register U32	regvalue;
	register struct NX_ADC_RegisterSet*	pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = pRegister->ADCCON;

	regvalue |= ADEN_MASK;

//	pRegister->ADCCON =	(U16)regvalue;
	WriteIOW(&pRegister->ADCCON, (U16)regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check ADC's operation
 *	@return		\b CTRUE indicate that ADC is Busy. \n
 *				\b CFALSE indicate that ADC Conversion is ended.
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,
 *				NX_ADC_GetConvertedData
 */
CBOOL	NX_ADC_IsBusy( void )
{
	const	U32	ADEN_MASK = (0x01 << 0);

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->ADCCON & ADEN_MASK );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Conversioned Data of ADC
 *	@return		10bit Data of ADC
 *	@see		NX_ADC_SetPrescalerValue,	NX_ADC_GetPrescalerValue,
 *				NX_ADC_SetPrescalerEnable,	NX_ADC_GetPrescalerEnable,
 *				NX_ADC_SetInputChannel,		NX_ADC_GetInputChannel,
 *				NX_ADC_SetStandbyMode,		NX_ADC_GetStandbyMode,
 *				NX_ADC_Start,				NX_ADC_IsBusy
 */
U32		NX_ADC_GetConvertedData( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)( __g_pRegister->ADCDAT );
}
