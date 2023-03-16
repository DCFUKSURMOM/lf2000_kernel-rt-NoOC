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
//	Module		: Rotator
//	File		: nx_rotator.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_rotator.h"

static	struct	NX_ROTATOR_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicates that Initialize is successed.\n
 *			\b CFALSE	indicates that Initialize is failed.\n
 *	@see	NX_ROTATOR_GetNumberOfModule
 */
CBOOL	NX_ROTATOR_Initialize( void )
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
 *	@see		NX_ROTATOR_Initialize
 */
U32		NX_ROTATOR_GetNumberOfModule( void )
{
	return NUMBER_OF_ROTATOR_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_ROTATOR_GetSizeOfRegisterSet,
 *				NX_ROTATOR_SetBaseAddress,		NX_ROTATOR_GetBaseAddress,
 *				NX_ROTATOR_OpenModule,			NX_ROTATOR_CloseModule,
 *				NX_ROTATOR_CheckBusy,			NX_ROTATOR_CanPowerDown
 */
U32		NX_ROTATOR_GetPhysicalAddress( void )
{
	return	(U32)(PHY_BASEADDR_ROTATOR_MODULE);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_ROTATOR_GetPhysicalAddress,
 *				NX_ROTATOR_SetBaseAddress,		NX_ROTATOR_GetBaseAddress,
 *				NX_ROTATOR_OpenModule,			NX_ROTATOR_CloseModule,
 *				NX_ROTATOR_CheckBusy,			NX_ROTATOR_CanPowerDown
 */
U32		NX_ROTATOR_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_ROTATOR_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_ROTATOR_GetPhysicalAddress,	NX_ROTATOR_GetSizeOfRegisterSet,
 *				NX_ROTATOR_GetBaseAddress,
 *				NX_ROTATOR_OpenModule,			NX_ROTATOR_CloseModule,
 *				NX_ROTATOR_CheckBusy,			NX_ROTATOR_CanPowerDown
 */
void	NX_ROTATOR_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_ROTATOR_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_ROTATOR_GetPhysicalAddress,	NX_ROTATOR_GetSizeOfRegisterSet,
 *				NX_ROTATOR_SetBaseAddress,
 *				NX_ROTATOR_OpenModule,			NX_ROTATOR_CloseModule,
 *				NX_ROTATOR_CheckBusy,			NX_ROTATOR_CanPowerDown
 */
U32		NX_ROTATOR_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicates that Initialize is successed. \n
 *				\b CFALSE	indicates that Initialize is failed.
 *	@see		NX_ROTATOR_GetPhysicalAddress,	NX_ROTATOR_GetSizeOfRegisterSet,
 *				NX_ROTATOR_SetBaseAddress,		NX_ROTATOR_GetBaseAddress,
 *				NX_ROTATOR_CloseModule,
 *				NX_ROTATOR_CheckBusy,			NX_ROTATOR_CanPowerDown
 */
CBOOL	NX_ROTATOR_OpenModule( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->RTRUNREG			= 0;
//	__g_pRegister->RTINTREG			= 0x0100;
//	__g_pRegister->RTDIRREG			= 0;
//	__g_pRegister->RTSRCBASEREG		= 0;
//	__g_pRegister->RTDESTBASEREG	= 0;
//	__g_pRegister->RTSIZEREG		= 0;
	WriteIODW(&__g_pRegister->RTRUNREG, 0);
	WriteIODW(&__g_pRegister->RTINTREG, 0x0100);
	WriteIODW(&__g_pRegister->RTDIRREG, 0);
	WriteIODW(&__g_pRegister->RTSRCBASEREG, 0);
	WriteIODW(&__g_pRegister->RTDESTBASEREG, 0);
	WriteIODW(&__g_pRegister->RTSIZEREG, 0);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicates that Deinitialize is successed. \n
 *				\b CFALSE	indicates that Deinitialize is failed.
 *	@see		NX_ROTATOR_GetPhysicalAddress,	NX_ROTATOR_GetSizeOfRegisterSet,
 *				NX_ROTATOR_SetBaseAddress,		NX_ROTATOR_GetBaseAddress,
 *				NX_ROTATOR_OpenModule,
 *				NX_ROTATOR_CheckBusy,			NX_ROTATOR_CanPowerDown
 */
CBOOL	NX_ROTATOR_CloseModule( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->RTRUNREG			= 0;
//	__g_pRegister->RTINTREG			= 0x0100;
//	__g_pRegister->RTDIRREG			= 0;
//	__g_pRegister->RTSRCBASEREG		= 0;
//	__g_pRegister->RTDESTBASEREG	= 0;
//	__g_pRegister->RTSIZEREG		= 0;
	WriteIODW(&__g_pRegister->RTRUNREG, 0);
	WriteIODW(&__g_pRegister->RTINTREG, 0x0100);
	WriteIODW(&__g_pRegister->RTDIRREG, 0);
	WriteIODW(&__g_pRegister->RTSRCBASEREG, 0);
	WriteIODW(&__g_pRegister->RTDESTBASEREG, 0);
	WriteIODW(&__g_pRegister->RTSIZEREG, 0);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicates that Module is Busy. \n
 *				\b CFALSE	indicates that Module is NOT Busy.
 *	@see		NX_ROTATOR_GetPhysicalAddress,	NX_ROTATOR_GetSizeOfRegisterSet,
 *				NX_ROTATOR_SetBaseAddress,		NX_ROTATOR_GetBaseAddress,
 *				NX_ROTATOR_OpenModule,			NX_ROTATOR_CloseModule,
 *				NX_ROTATOR_CanPowerDown
 */
CBOOL	NX_ROTATOR_CheckBusy( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return NX_ROTATOR_IsBusy();
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicates that Ready to enter power-down stage. \n
 *				\b CFALSE	indicates that This module can't enter to power-down stage.
 *	@see		NX_ROTATOR_GetPhysicalAddress,	NX_ROTATOR_GetSizeOfRegisterSet,
 *				NX_ROTATOR_SetBaseAddress,		NX_ROTATOR_GetBaseAddress,
 *				NX_ROTATOR_OpenModule,			NX_ROTATOR_CloseModule,
 *				NX_ROTATOR_CheckBusy
 */
CBOOL	NX_ROTATOR_CanPowerDown( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see											NX_ROTATOR_GetClockPClkMode,
 */
void			NX_ROTATOR_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_ROTATOR_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->RTCLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//	pRegister->RTCLKENB = regvalue;
	WriteIODW(&pRegister->RTCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@return		Current PCLK mode
 *	@see		NX_ROTATOR_SetClockPClkMode,
 *				NX_ROTATOR_SetClockBClkMode,		NX_ROTATOR_GetClockBClkMode,
 */
NX_PCLKMODE	NX_ROTATOR_GetClockPClkMode( void )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->RTCLKENB & ( 1UL << PCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set System Bus Clock's operation Mode
 *	@param[in]	mode		BCLK Mode
 *	@return		None.
 *	@see		NX_ROTATOR_SetClockPClkMode,		NX_ROTATOR_GetClockPClkMode,
 *													NX_ROTATOR_GetClockBClkMode,
 */
void NX_ROTATOR_SetClockBClkMode( NX_BCLKMODE mode )
{
	register U32 regvalue;
	register struct NX_ROTATOR_RegisterSet* pRegister;
	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_BCLKMODE_DISABLE:	clkmode = 0;		break;
		case NX_BCLKMODE_DYNAMIC:	clkmode = 2;		break;
		case NX_BCLKMODE_ALWAYS:	clkmode = 3;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->RTCLKENB;
	regvalue &= ~(0x03);
	regvalue |= clkmode & 0x03;
//	pRegister->RTCLKENB = regvalue;
	WriteIODW(&pRegister->RTCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get System Bus Clock's operation Mode
 *	@return		BCLK Mode
 *	@see		NX_ROTATOR_SetClockPClkMode,		NX_ROTATOR_GetClockPClkMode,
 *				NX_ROTATOR_SetClockBClkMode,
 */
NX_BCLKMODE	NX_ROTATOR_GetClockBClkMode( void )
{
	const U32 BCLKMODE	= 3UL<<0;

	NX_ASSERT( CNULL != __g_pRegister );

	switch( __g_pRegister->RTCLKENB & BCLKMODE )
	{
		case 0 :	return NX_BCLKMODE_DISABLE;
		case 2 :	return NX_BCLKMODE_DYNAMIC;
		case 3 :	return NX_BCLKMODE_ALWAYS;
	}

	NX_ASSERT( CFALSE );
	return NX_BCLKMODE_DISABLE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see												NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
S32		NX_ROTATOR_GetInterruptNumber( void )
{
	return	INTNUM_OF_ROTATOR_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0:Rotate Done ).
 *	@param[in]	Enable	\b CTRUE	indicates that Interrupt Enable. \n
 *						\b CFALSE	indicates that Interrupt Disable.
 *	@return		None.
 *	@see		NX_ROTATOR_GetInterruptNumber,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
void	NX_ROTATOR_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	const U32	RT_INT_ENB_BITPOS = 16;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( (0 == Enable) || (1 == Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	IntNum = IntNum;

//	__g_pRegister->RTINTREG = ((U32)Enable << RT_INT_ENB_BITPOS);
	WriteIODW(&__g_pRegister->RTINTREG, ((U32)Enable << RT_INT_ENB_BITPOS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number( 0:Rotate Done ).
 *	@return		\b CTRUE	indicates that Interrupt is enabled. \n
 *				\b CFALSE	indicates that Interrupt is disabled.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *														NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
CBOOL	NX_ROTATOR_GetInterruptEnable( S32 IntNum )
{
	const U32	RT_INT_ENB_BITPOS = 16;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	IntNum = IntNum;

	return (CBOOL)( (__g_pRegister->RTINTREG >> RT_INT_ENB_BITPOS) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set Rotate Done interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
void	NX_ROTATOR_SetInterruptEnable32( U32 EnableFlag )
{
	const U32	RT_INT_ENB_BITPOS = 16;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->RTINTREG = ((EnableFlag & 0x01) << RT_INT_ENB_BITPOS);
	WriteIODW(&__g_pRegister->RTINTREG, ((EnableFlag & 0x01) << RT_INT_ENB_BITPOS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : Rotate Done interrupt's setting value. \n
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *														NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
U32		NX_ROTATOR_GetInterruptEnable32( void )
{
	const U32	RT_INT_ENB_BITPOS	= 16;
	const U32	RT_INT_ENB_MASK		= 1<<RT_INT_ENB_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->RTINTREG & RT_INT_ENB_MASK)>>RT_INT_ENB_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number( 0:Rotate Done ).
 *	@return		\b CTRUE	indicates that Pending is seted. \n
 *				\b CFALSE	indicates that Pending is Not Seted.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
CBOOL	NX_ROTATOR_GetInterruptPending( S32 IntNum )
{
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	IntNum = IntNum;

	return (CBOOL)( __g_pRegister->RTINTREG & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : Rotate Done pending state. \n
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *														NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
U32		NX_ROTATOR_GetInterruptPending32( void )
{
	const U32 PEND_MASK = 0x01;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->RTINTREG & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number( 0:Rotate Done ).
 *	@return		None.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
void	NX_ROTATOR_ClearInterruptPending( S32 IntNum )
{
	const U32 RT_INT_CLRN_MASK = 1 << 8;
	register U32 regval;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	IntNum = IntNum;

//	__g_pRegister->RTINTREG |= RT_INT_CLRN_MASK;
	regval = __g_pRegister->RTINTREG | RT_INT_CLRN_MASK;
	WriteIODW(&__g_pRegister->RTINTREG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : Rotate Done pending bit. \n
 *	@return		None.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *														NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
void	NX_ROTATOR_ClearInterruptPending32( U32 PendingFlag )
{
	const U32	PEND_BITPOS = 8;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->RTINTREG |=	(PendingFlag&0x01) << PEND_BITPOS;
	regval = __g_pRegister->RTINTREG | (PendingFlag&0x01) << PEND_BITPOS;
	WriteIODW(&__g_pRegister->RTINTREG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicates that Set to all interrupt enable. \n
 *						\b CFALSE	indicates that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
void	NX_ROTATOR_SetInterruptEnableAll( CBOOL Enable )
{
	const U32	RT_INT_ENB_BITPOS = 16;

	NX_ASSERT( (0 == Enable) || (1 == Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->RTINTREG = ((U32)Enable << RT_INT_ENB_BITPOS);
	WriteIODW(&__g_pRegister->RTINTREG, ((U32)Enable << RT_INT_ENB_BITPOS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicates that All interrupt is disabled.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *														NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
CBOOL	NX_ROTATOR_GetInterruptEnableAll( void )
{
	const U32	RT_INT_ENB_BITPOS	= 16;
	const U32	RT_INT_ENB_MASK		= 1 << RT_INT_ENB_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	if( RT_INT_ENB_MASK & __g_pRegister->RTINTREG )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicates that All pending is NOT seted.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,
 *				NX_ROTATOR_ClearInterruptPendingAll,	NX_ROTATOR_GetInterruptPendingNumber
 */
CBOOL	NX_ROTATOR_GetInterruptPendingAll( void )
{
	const U32 RT_INT_PEND_MASK = 0x01;

	NX_ASSERT( CNULL != __g_pRegister );

	if( RT_INT_PEND_MASK & __g_pRegister->RTINTREG )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *														NX_ROTATOR_GetInterruptPendingNumber
 */
void	NX_ROTATOR_ClearInterruptPendingAll( void )
{
	const U32 RT_INT_CLRN_MASK = 1 << 8;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->RTINTREG |= RT_INT_CLRN_MASK;
	regval = __g_pRegister->RTINTREG | RT_INT_CLRN_MASK;
	WriteIODW(&__g_pRegister->RTINTREG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).\n
 *				0 (Rotate Done)
 *	@see		NX_ROTATOR_GetInterruptNumber,			NX_ROTATOR_SetInterruptEnable,
 *				NX_ROTATOR_GetInterruptEnable,			NX_ROTATOR_SetInterruptEnable32,
 *				NX_ROTATOR_GetInterruptEnable32,		NX_ROTATOR_GetInterruptPending,
 *				NX_ROTATOR_GetInterruptPending32,		NX_ROTATOR_ClearInterruptPending,
 *				NX_ROTATOR_ClearInterruptPending32,		NX_ROTATOR_SetInterruptEnableAll,
 *				NX_ROTATOR_GetInterruptEnableAll,		NX_ROTATOR_GetInterruptPendingAll,
 *				NX_ROTATOR_ClearInterruptPendingAll
 */
S32		NX_ROTATOR_GetInterruptPendingNumber( void )	// -1 if None
{
	const U32 RT_INT_ENB_BITPOS	= 16;
	const U32 RT_INT_PEND_MASK	= 1 << 0;

	register struct NX_ROTATOR_RegisterSet	*pRegister;
	register U32 Pend;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	Pend = ((pRegister->RTINTREG >> RT_INT_ENB_BITPOS) & pRegister->RTINTREG);

	if( Pend & RT_INT_PEND_MASK )
	{
		return 0;
	}

	return -1;
}

//------------------------------------------------------------------------------
// NX_ROTATIOR Hardware Interface Implementation
//------------------------------------------------------------------------------
/**
 *	@brief		Run Rotator
 *	@return		None.
 *	@see		NX_ROTATOR_Stop, NX_ROTATOR_IsBusy
 */
void	NX_ROTATOR_Run( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->RTRUNREG = 0x01;
	WriteIODW(&__g_pRegister->RTRUNREG, 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Stop Rotator
 *	@return		None.
 *	@remarks	While the rotate processing is running, it can be halted by clearing this bit.
 *	@see		NX_ROTATOR_Run, NX_ROTATOR_IsBusy
 */
void	NX_ROTATOR_Stop( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	WriteIODW(&__g_pRegister->RTRUNREG, 0);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check rotator's state ( busy or idle )
 *	@return		CTRUE	indicates that rotator is running.\n
 *				CFALSE	indicates that rotator is idle.
 *	@see		NX_ROTATOR_Run, NX_ROTATOR_Stop
 */
CBOOL	NX_ROTATOR_IsBusy( void )
{
	const U32	BUSY_MASK	= ( 0x01 << 24 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->RTINTREG & BUSY_MASK )	{ return CTRUE; }
	else										{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Direction of Destination Image
 *	@param[in]	direction		Set Direction ( 90, 270, 90 and Horizontal Flip, 270 and Horizontal Flip )
 *	@return		None.
 *	@see		NX_ROTATOR_GetDirection
 */
void	NX_ROTATOR_SetDirection
(
	NX_ROTATOR_DIR direction
)
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0 == ( 0x11101110 & direction ) );	// invalid data check

//	__g_pRegister->RTDIRREG = direction;
	WriteIODW(&__g_pRegister->RTDIRREG, direction);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Direction Value of Rotator
 *	@return		Direction Value( 90, 270, 90 and Horizontal Flip, 270 and Horizontal Flip )
 *	@see		NX_ROTATOR_SetDirection
 */
NX_ROTATOR_DIR		NX_ROTATOR_GetDirection( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_ROTATOR_DIR)__g_pRegister->RTDIRREG;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Source Image's Base Address
 *	@param[in]	BaseAddr		Set Source Base Address ( 0x8 ~ 0x0FFF8FF8 )
 *	@return		None.
 *	@remarks	Address Format is 2D Address. so each bit have meaning.\n\n
 *				SrcAddr[28:24]	: Segment Address.\n
 *				SrcAddr[23:12]	: Base Address Y.\n
 *				SrcAddr[11:0]	: Base Address X.\n\n
 *				LSB 3 bits must be 0.\n
 *				User must set source address this format.
 *	@see		NX_ROTATOR_GetSrcBase
 */
void	NX_ROTATOR_SetSrcBase
(
	U32 BaseAddr
)
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0 ==( 0x07 & BaseAddr) );

//	__g_pRegister->RTSRCBASEREG = BaseAddr;
	WriteIODW(&__g_pRegister->RTSRCBASEREG, BaseAddr);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Source Base Address
 *	@return		Source Image's 2D Address( 0x08 ~ 0x0FFF8FF8 )
 *	@remarks	Address Format is 2D Address. so each bit have meaning.\n\n
 *				SrcAddr[28:24]	: Segment Address.\n
 *				SrcAddr[23:12]	: Base Address Y.\n
 *				SrcAddr[11:0]	: Base Address X.\n
 *	@see		NX_ROTATOR_SetSrcBase
 */
U32		NX_ROTATOR_GetSrcBase()
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->RTSRCBASEREG;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Destination Base Address
 *	@param[in]	BaseAddr		Set Destination Base Address( 0x08 ~ 0x0FFF8FF8 )
 *	@return		None.
 *	@remarks	Address Format is 2D Address. so each bit have meaning.\n\n
 *				DestAddr[28:24] : Segment Address.\n
 *				DestAddr[23:12] : Base Address Y.\n
 *				DestAddr[11:0]	: Base Address X.\n\n
 *				LSB 3 bits must be 0.\n
 *				If use Horizontal or Vertical rotation, destination address must calculate.\n\n
 *
 *				- 90 \n
 *					DestAddr[23:12] : DestX = BaseX + Height(source image) - 8.\n
 *					DestAddr[11:0]	: DestY = BaseY.\n\n
 *
 *				- 90 and Horizontal Flip \n
 *					DestAddr[23:12] : DestX = BaseX.\n
 *					DestAddr[11:0]	: DestY = BaseY.\n\n
 *
 *				- 270 \n
 *					DestAddr[23:12] : DestX = BaseX.\n
 *					DestAddr[11:0]	: DestY = BaseY + Width(source image) - 1.\n\n
 *
 *				- 270 and Horizontal Flip \n
 *					DestAddr[23:12] : DestX = BaseX + Height(source image) - 8.\n
 *					DestAddr[11:0]	: DestY = BaseY + Width(source image) - 1.\n\n
 *
 *	@code
 *
 *		destSegAddr = dest & 0xFF000000;
 *		destYAddr	= ( dest >> 12 ) & 0xFFF;
 *		destXAddr	= ( dest & 0xFFF );
 *
 *		if( NX_ROTATOR_DIR_90_HFLIP == dir )
 *		{
 *			destXAddr = destXAddr;
 *			destYAddr = destYAddr;
 *		}
 *		else if( NX_ROTATOR_DIR_90 == dir )
 *		{
 *			destXAddr = destXAddr + ( height - 8 );
 *			destYAddr = destYAddr;
 *		}
 *		else if( NX_ROTATOR_DIR_270 == dir )
 *		{
 *			destXAddr = destXAddr;
 *			destYAddr = destYAddr + ( width - 1 );
 *		}
 *		// NX_ROTATOR_DIR_270_HFLIP
 *		else
 *		{
 *			destXAddr = destXAddr + ( height - 8 );
 *			destYAddr = destYAddr + ( width - 1 );
 *		}
 *
 *		destAddress = (	destSegAddr & 0xFF000000 )	|
 *					(( destYAddr & 0xFFF ) << 12 ) |
 *					(	destXAddr & 0xFFF ) ;
 *
 *		NX_ROTATOR_SetSrcBase( destAddress );
 *
 *	@endcode
 *	@see		NX_ROTATOR_GetDestBase
 */
void	NX_ROTATOR_SetDestBase
(
	U32 BaseAddr
)
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0 ==( 0x07 & BaseAddr) );

//	__g_pRegister->RTDESTBASEREG = BaseAddr;
	WriteIODW(&__g_pRegister->RTDESTBASEREG, BaseAddr);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Destination Image's Base Address
 *	@return		Destination 2D Address ( 0x8 ~ 0x0FFF8FF8 )
 *	@remarks	Address Format is 2D Address. so each bit have meaning.\n\n
 *				DestAddr[28:24] : Segment Address.\n
 *				DestAddr[23:12] : Base Address Y.\n
 *				DestAddr[11:0]	: Base Address X.\n\n
 *	@see		NX_ROTATOR_SetDestBase
 */
U32		NX_ROTATOR_GetDestBase( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->RTDESTBASEREG;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Image's width and height size
 *	@param[in]	width	value of width ( 8 ~ 4088 )
 *	@param[in]	height	value of height( 8 ~ 4088 )
 *	@return		None.
 *	@remarks	LSB 3bit must be 0.\n
 *				Width and Height is 4096 when set to 0.
 *	@see		NX_ROTATOR_GetImageSize
 */
void	NX_ROTATOR_SetImageSize
(
	U32 width,
	U32 height
)
{
	const U32	WIDTH_BITPOS	= 0;
	const U32	HEIGHT_BITPOS	= 16;
	register U32	temp;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0 == ( width & 0x07 ) );
	NX_ASSERT( 0 == ( height & 0x07 ) );
	NX_ASSERT( ( 0xFF8 >= width ) && ( 0xFF8 >= height ) );

	temp = ( width << WIDTH_BITPOS ) | ( height << HEIGHT_BITPOS );

//	__g_pRegister->RTSIZEREG = temp;
	WriteIODW(&__g_pRegister->RTSIZEREG, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Image's width and height size
 *	@param[out]	width		value of width ( 8 ~ 4088 )
 *	@param[out]	height		value of height( 8 ~ 4088 )
 *	@return		None.
 *	@remarks	Parameter(width, height) can set to CNULL, when the information is not need.
 *	@see		NX_ROTATOR_SetImageSize
 */
void	NX_ROTATOR_GetImageSize
(
	U32	*width,
	U32	*height
)
{
	const U32	WIDTH_MASK		= ( 0xFFF << 0 );
	const U32	WIDTH_BITPOS	= 0;
	const U32	HEIGH_MASK		= ( 0xFFF << 16 );
	const U32	HEIGHT_BITPOS	= 16;
	register U32	temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->RTSIZEREG;

	if( CNULL != width	){ *width	= ( temp & WIDTH_MASK ) >> WIDTH_BITPOS ; }
	if( CNULL != height ){ *height = ( temp & HEIGH_MASK ) >> HEIGHT_BITPOS; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set rotator's configuration
 *	@param[in]	src		Set Source 2D Address
 *	@param[in]	dest	Set Destination 2D Address
 *	@param[in]	width	value of width ( 8 ~ 4088 )
 *	@param[in]	height	value of height( 8 ~ 4088 )
 *	@param[in]	dir		Set Direction ( 90, 270, 90 and Horizontal Inversion, 270 and Horizontal Inversion )
 *	@return		None.
 *	@see		NX_ROTATOR_GetRotator
 */
void	NX_ROTATOR_SetRotator
(
	U32 src,
	U32 dest,
	U32 width,
	U32 height,
	NX_ROTATOR_DIR dir
)
{
	U32 destAddress;
	U32 destSegAddr;
	U32 destYAddr;
	U32 destXAddr;

	NX_ROTATOR_SetSrcBase( src );
	NX_ROTATOR_SetImageSize( width, height );
	NX_ROTATOR_SetDirection( dir );

	//--------------------------------------------------------------------------
	// destination address calculate
	//--------------------------------------------------------------------------

	destSegAddr = dest & 0xFF000000;
	destYAddr	= ( dest >> 12 ) & 0xFFF;
	destXAddr	= ( dest & 0xFFF );

	/*
	// Right Postion
	if( 0x00000001 == ( dir & 0x00000001 ) )
	{
		destXAddr = destXAddr + ( height - 8 );
		NX_ASSERT( destXAddr < 4096 );
	}

	// Bottom Postion
	if( 0x00010000 == ( dir & 0x00010000 ) )
	{
		destYAddr = destYAddr + ( width - 1 );
		NX_ASSERT( destYAddr < 4096 );
	}
	*/
	if( NX_ROTATOR_DIR_90_HFLIP == dir )
	{
		destXAddr = destXAddr;
		destYAddr = destYAddr;
	}
	else if( NX_ROTATOR_DIR_90 == dir )
	{
		destXAddr = destXAddr + ( height - 8 );
		destYAddr = destYAddr;
	}
	else if( NX_ROTATOR_DIR_270 == dir )
	{
		destXAddr = destXAddr;
		destYAddr = destYAddr + ( width - 1 );
	}
	// NX_ROTATOR_DIR_270_HFLIP
	else
	{
		destXAddr = destXAddr + ( height - 8 );
		destYAddr = destYAddr + ( width - 1 );
	}

	destAddress = (	destSegAddr & 0xFF000000 ) |
					(( destYAddr & 0xFFF ) << 12 ) |
					(	destXAddr & 0xFFF ) ;

	NX_ROTATOR_SetDestBase( destAddress );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get rotator's configuration
 *	@param[out]	src		Source Image's 2D Address
 *	@param[out]	dest	Destination Image's 2D Address
 *	@param[out]	width	value of width ( 8 ~ 4088 )
 *	@param[out]	height	value of height( 8 ~ 4088 )
 *	@param[out]	dir		Direction ( 90, 270, 90 and Horizontal Inversion, 270 and Horizontal Inversion )
 *	@return		None.
 *	@remarks	Parameter(src, dest, width, height, dir) can set to CNULL, when the information is not need.
 *	@see		NX_ROTATOR_SetRotator
 */
void	NX_ROTATOR_GetRotator
(
	U32* src,
	U32* dest,
	U32* width,
	U32* height,
	NX_ROTATOR_DIR* dir
)
{
	U32 destAddr;
	U32 destSegAddr;
	U32 destYAddr;
	U32 destXAddr;

	if( CNULL != src ){ *src = NX_ROTATOR_GetSrcBase(); }
	if( CNULL != dir ){ *dir = NX_ROTATOR_GetDirection(); }

	NX_ROTATOR_GetImageSize( width, height );

	NX_ASSERT( CNULL != width );
	NX_ASSERT( CNULL != height );

	destAddr = NX_ROTATOR_GetDestBase();

	destSegAddr = destAddr & 0x0F000000;
	destYAddr	= ( destAddr >> 12 ) & 0xFFF;
	destXAddr	= ( destAddr & 0xFFF );

	// Right Postion
	if( 0x00000001 == ( *dir & 0x00000001 ) )
	{
		destXAddr = destXAddr - ( *height - 8 );
	}

	// Bottom Postion
	if( 0x00010000 == ( *dir & 0x00010000 ) )
	{
		destYAddr = destYAddr - ( *width - 1 );
	}

	*dest = (	destSegAddr & 0x0F000000 ) |
			(( destYAddr & 0xFFF ) << 12 ) |
			(	destXAddr & 0xFFF ) ;
}
