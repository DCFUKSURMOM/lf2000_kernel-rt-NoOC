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
//	Module		: CSC
//	File		: nx_csc.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_csc.h"

enum
{
	NX_CSC_ALIGN_WIDTH		= 16UL,
	NX_CSC_MIN_WIDTH		= 16UL,
	NX_CSC_MAX_WIDTH		= 1024UL,
	NX_CSC_ALIGN_HEIGHT		= 16UL,
	NX_CSC_MIN_HEIGHT		= 16UL,
	NX_CSC_MAX_HEIGHT		= 1024UL
};

NX_DEBUG_CODE
(
	// Store widht and height to verify paramters of the function
	// SetSourceAddr and SetTargetAddr in debug build environment.
	U32		NX_CSC_dbg_dwWidth;
	U32		NX_CSC_dbg_dwHeight;
)

static	struct NX_CSC_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize of prototype enviroment & local variables.
 *	@return		\b CTRUE	indicates that Initialize is successed.\n
 *				\b CFALSE	indicates that Initialize is failed.\n
 *	@see		NX_CSC_GetNumberOfModule
 */
CBOOL	NX_CSC_Initialize( void )
{
	static CBOOL bInit = CFALSE;

	if( CFALSE == bInit )
	{
		__g_pRegister = CNULL;

		bInit = CTRUE;
	}

	NX_DEBUG_CODE
	(
		NX_CSC_dbg_dwWidth	= 0;
		NX_CSC_dbg_dwHeight	= 0;
	)

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 *	@see		NX_CSC_Initialize
 */
U32		NX_CSC_GetNumberOfModule( void )
{
	return NUMBER_OF_CSC_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see										NX_CSC_GetSizeOfRegisterSet,
 *				NX_CSC_SetBaseAddress,			NX_CSC_GetBaseAddress,
 *				NX_CSC_OpenModule,				NX_CSC_CloseModule,
 *				NX_CSC_CheckBusy,				NX_CSC_CanPowerDown
 */
U32		NX_CSC_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_CSC_MODULE );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_CSC_GetPhysicalAddress,
 *				NX_CSC_SetBaseAddress,			NX_CSC_GetBaseAddress,
 *				NX_CSC_OpenModule,				NX_CSC_CloseModule,
 *				NX_CSC_CheckBusy,				NX_CSC_CanPowerDown
 */
U32		NX_CSC_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_CSC_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_CSC_GetPhysicalAddress,		NX_CSC_GetSizeOfRegisterSet,
 *												NX_CSC_GetBaseAddress,
 *				NX_CSC_OpenModule,				NX_CSC_CloseModule,
 *				NX_CSC_CheckBusy,				NX_CSC_CanPowerDown
 */
void	NX_CSC_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_CSC_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_CSC_GetPhysicalAddress,		NX_CSC_GetSizeOfRegisterSet,
 *				NX_CSC_SetBaseAddress,
 *				NX_CSC_OpenModule,				NX_CSC_CloseModule,
 *				NX_CSC_CheckBusy,				NX_CSC_CanPowerDown
 */
U32		NX_CSC_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicates that Initialize is successed. \n
 *				\b CFALSE	indicates that Initialize is failed.
 *	@see		NX_CSC_GetPhysicalAddress,		NX_CSC_GetSizeOfRegisterSet,
 *				NX_CSC_SetBaseAddress,			NX_CSC_GetBaseAddress,
 *												NX_CSC_CloseModule,
 *				NX_CSC_CheckBusy,				NX_CSC_CanPowerDown
 */
CBOOL	NX_CSC_OpenModule( void )
{
	NX_ASSERT(	!NX_CSC_CheckBusy() );
	NX_CSC_SetInterruptEnableAll( CFALSE );
	NX_CSC_ClearInterruptPendingAll();
	NX_CSC_SetTransparency( 0, CFALSE,
								CFALSE, 0, 255,
								CFALSE, 0, 255,
								CFALSE, 0, 255 );
	NX_CSC_SetDitherEnable( CTRUE );
	NX_CSC_SetQuantizationMode( NX_CSC_QMODE_256 );
	NX_ASSERT(	!NX_CSC_CheckBusy() );

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicates that Deinitialize is successed. \n
 *				\b CFALSE	indicates that Deinitialize is failed.
 *	@see		NX_CSC_GetPhysicalAddress,		NX_CSC_GetSizeOfRegisterSet,
 *				NX_CSC_SetBaseAddress,			NX_CSC_GetBaseAddress,
 *				NX_CSC_OpenModule,
 *				NX_CSC_CheckBusy,				NX_CSC_CanPowerDown
 */
CBOOL	NX_CSC_CloseModule( void )
{
	NX_ASSERT(	!NX_CSC_CheckBusy() );
	NX_CSC_SetInterruptEnableAll( CFALSE );
	NX_CSC_ClearInterruptPendingAll();
	NX_CSC_SetTransparency( 0, CFALSE,
								CFALSE, 0, 255,
								CFALSE, 0, 255,
								CFALSE, 0, 255 );
	NX_CSC_SetDitherEnable( CTRUE );
	NX_CSC_SetQuantizationMode( NX_CSC_QMODE_256 );
	NX_ASSERT(	!NX_CSC_CheckBusy() );

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicates that Module is Busy. \n
 *				\b CFALSE	indicates that Module is NOT Busy.
 *	@see		NX_CSC_GetPhysicalAddress,		NX_CSC_GetSizeOfRegisterSet,
 *				NX_CSC_SetBaseAddress,			NX_CSC_GetBaseAddress,
 *				NX_CSC_OpenModule,				NX_CSC_CloseModule,
 *												NX_CSC_CanPowerDown
 */
CBOOL	NX_CSC_CheckBusy( void )
{
	return NX_CSC_IsBusy();
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicates that Ready to enter power-down stage. \n
 *				\b CFALSE	indicates that This module can't enter to power-down stage.
 *	@see		NX_CSC_GetPhysicalAddress,		NX_CSC_GetSizeOfRegisterSet,
 *				NX_CSC_SetBaseAddress,			NX_CSC_GetBaseAddress,
 *				NX_CSC_OpenModule,				NX_CSC_CloseModule,
 *				NX_CSC_CheckBusy
 */
CBOOL	NX_CSC_CanPowerDown( void )
{
	if( NX_CSC_IsBusy() )
	{
		return CFALSE;
	}

	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see											NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
S32		NX_CSC_GetInterruptNumber( void )
{
	return	INTNUM_OF_CSC_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0 ).
 *	@param[in]	Enable	\b CTRUE	indicates that Interrupt Enable. \n
 *						\b CFALSE	indicates that Interrupt Disable.
 *	@return		None.
 *	@remarks	CSC Module Only have one interrupt. So always \e IntNum set to 0.
 *	@see		NX_CSC_GetInterruptNumber,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
void	NX_CSC_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	const U32 INTENB_POS	= 1;
	const U32 DITHER		= 1UL<<5;
	const U32 QMODE_MASK	= 1UL<<4;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	= __g_pRegister;
	regvalue	= pRegister->CONTROL;

	regvalue	&= ( DITHER | QMODE_MASK );
	regvalue	|= ( (U32)Enable << INTENB_POS );

//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number ( 0 ).
 *	@return		\b CTRUE	indicates that Interrupt is enabled. \n
 *				\b CFALSE	indicates that Interrupt is disabled.
 *	@remarks	CSC Module Only have one interrupt. So always \e IntNum set to 0.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *													NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
CBOOL	NX_CSC_GetInterruptEnable( S32 IntNum )
{
	const U32 INTENB_MASK	= (1UL << 1);
	const U32 INTENB_POS	= 1;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->CONTROL & INTENB_MASK) >> INTENB_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set CSC Done interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
void	NX_CSC_SetInterruptEnable32( U32 EnableFlag )
{
	const U32 INTENB_POS	= 1;
	const U32 DITHER		= 1UL<<5;
	const U32 QMODE_MASK	= 1UL<<4;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	= __g_pRegister;
	regvalue	= pRegister->CONTROL;

	regvalue	&= ( DITHER | QMODE_MASK );
	regvalue	|= ( (U32)(EnableFlag & 0x01) << INTENB_POS );

//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : CSC Done interrupt's setting value. \n
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *													NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
U32		NX_CSC_GetInterruptEnable32( void )
{
	const U32 INTENB_MASK	= (1UL << 1);
	const U32 INTENB_POS	= 1;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)( (__g_pRegister->CONTROL & INTENB_MASK) >> INTENB_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number ( 0 ).
 *	@return		\b CTRUE	indicates that Pending is seted. \n
 *				\b CFALSE	indicates that Pending is Not Seted.
 *	@remarks	CSC Module Only have one interrupt. So always \e IntNum set to 0.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
CBOOL	NX_CSC_GetInterruptPending( S32 IntNum )
{
	const U32 INTPEND_MASK	= (1UL << 2);
	const U32 INTPEND_POS	= 2;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->CONTROL & INTPEND_MASK) >> INTPEND_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : CSC Done pending state. \n
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *													NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
U32		NX_CSC_GetInterruptPending32( void )
{
	const U32 INTPEND_MASK	= (1UL << 2);
	const U32 INTPEND_POS	= 2;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->CONTROL & INTPEND_MASK) >> INTPEND_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number ( 0 ).
 *	@return		None.
 *	@remarks	CSC Module Only have one interrupt. So always \e IntNum set to 0.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
void	NX_CSC_ClearInterruptPending( S32 IntNum )
{
	const U32 DITHER		= 1UL<<5;
	const U32 QMODE_MASK	= 1UL<<4;
	const U32 INTENB		= 1UL<<1;
	const U32 INTPEND_POS	= 2;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	regvalue	= pRegister->CONTROL;

	regvalue	&= ( DITHER | QMODE_MASK | INTENB );

	regvalue	|= ( 1UL << INTPEND_POS );

//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : CSC Done pending bit. \n
 *	@return		None.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *													NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
void	NX_CSC_ClearInterruptPending32( U32 PendingFlag )
{
	const U32 DITHER		= 1UL<<5;
	const U32 QMODE_MASK	= 1UL<<4;
	const U32 INTENB		= 1UL<<1;
	const U32 INTPEND_POS	= 2;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	regvalue	= pRegister->CONTROL;

	regvalue	&= ( DITHER | QMODE_MASK | INTENB );

	regvalue	|= ((PendingFlag & 0x01) << INTPEND_POS );

//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicates that Set to all interrupt enable.	\n
 *						\b CFALSE	indicates that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
void	NX_CSC_SetInterruptEnableAll( CBOOL Enable )
{
	const U32 INTENB_POS	= 1;
	const U32 DITHER		= 1UL<<5;
	const U32 QMODE_MASK	= 1UL<<4;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	= __g_pRegister;

	regvalue	= pRegister->CONTROL;

	regvalue	&= ( DITHER | QMODE_MASK );
	regvalue	|= ( (U32)Enable << INTENB_POS );

//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicates that All interrupt is disabled.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *													NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
CBOOL	NX_CSC_GetInterruptEnableAll( void )
{
	const U32 INTENB_MASK	= (1UL << 1);
	const U32 INTENB_POS	= 1;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->CONTROL & INTENB_MASK) >> INTENB_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicates that All pending is NOT seted.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,
 *				NX_CSC_ClearInterruptPendingAll,	NX_CSC_GetInterruptPendingNumber
 */
CBOOL	NX_CSC_GetInterruptPendingAll( void )
{
	const U32 INTPEND_MASK	= (1UL << 2);
	const U32 INTPEND_POS	= 2;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->CONTROL & INTPEND_MASK) >> INTPEND_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *													NX_CSC_GetInterruptPendingNumber
 */
void	NX_CSC_ClearInterruptPendingAll( void )
{

	const U32 DITHER		= 1UL<<5;
	const U32 QMODE_MASK	= 1UL<<4;
	const U32 INTENB		= 1UL<<1;

	const U32 INTPEND_POS	= 2;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister	=	__g_pRegister;

	regvalue	= pRegister->CONTROL;

	regvalue	&= ( DITHER | QMODE_MASK | INTENB );

	regvalue	|= ( 1UL << INTPEND_POS );

//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_CSC_GetInterruptNumber,			NX_CSC_SetInterruptEnable,
 *				NX_CSC_GetInterruptEnable,			NX_CSC_SetInterruptEnable32,
 *				NX_CSC_GetInterruptEnable32,		NX_CSC_GetInterruptPending,
 *				NX_CSC_GetInterruptPending32,		NX_CSC_ClearInterruptPending,
 *				NX_CSC_ClearInterruptPending32,		NX_CSC_SetInterruptEnableAll,
 *				NX_CSC_GetInterruptEnableAll,		NX_CSC_GetInterruptPendingAll,
 *				NX_CSC_ClearInterruptPendingAll
 */
S32		NX_CSC_GetInterruptPendingNumber( void )	// -1 if None
{
	const U32 INTPEND_BITPOS	= 2;
	const U32 INTENB_BITPOS		= 1;

	register U32 Pend;

	NX_ASSERT( CNULL != __g_pRegister );

	Pend = __g_pRegister->CONTROL;

	Pend = (Pend >> INTPEND_BITPOS) & (Pend >> INTENB_BITPOS);

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
 *	@see										NX_CSC_GetClockPClkMode,
 *				NX_CSC_SetClockBClkMode,		NX_CSC_GetClockBClkMode
 */
void	NX_CSC_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->CSCCLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//	pRegister->CSCCLKENB = regvalue;
	WriteIODW(&pRegister->CSCCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@return		Current PCLK mode
 *	@see		NX_CSC_SetClockPClkMode,
 *				NX_CSC_SetClockBClkMode,		NX_CSC_GetClockBClkMode
 */
NX_PCLKMODE	NX_CSC_GetClockPClkMode( void )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->CSCCLKENB & ( 1UL << PCLKMODE_POS ) )
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
 *	@see		NX_CSC_SetClockPClkMode,		NX_CSC_GetClockPClkMode,
 *												NX_CSC_GetClockBClkMode
 */
void	NX_CSC_SetClockBClkMode( NX_BCLKMODE mode )
{
	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;
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

	regvalue = pRegister->CSCCLKENB;
	regvalue &= ~(0x3);
	regvalue |= clkmode & 0x3;
//	pRegister->CSCCLKENB = regvalue;
	WriteIODW(&pRegister->CSCCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get System Bus Clock's operation Mode
 *	@return		BCLK Mode
 *	@see		NX_CSC_SetClockPClkMode,		NX_CSC_GetClockPClkMode,
 *				NX_CSC_SetClockBClkMode
 */
NX_BCLKMODE	NX_CSC_GetClockBClkMode( void )
{
	const U32 BCLKMODE	= 3UL<<0;

	NX_ASSERT( CNULL != __g_pRegister );

	switch( __g_pRegister->CSCCLKENB & BCLKMODE )
	{
		case 0 :	return NX_BCLKMODE_DISABLE;
		case 2 :	return NX_BCLKMODE_DYNAMIC;
		case 3 :	return NX_BCLKMODE_ALWAYS;
	}

	NX_ASSERT( CFALSE );
	return NX_BCLKMODE_DISABLE;
}

//------------------------------------------------------------------------------
// CSC Operation.
//------------------------------------------------------------------------------
/**
 *	@brief		Specifies whether to apply the dithering.
 *	@param[in]	bEnb	Set it to CTRUE to apply the dithering.
 *	@return		None.
 *	@remarks	The dithering is useful method for case which is that the color
 *				depth of destination is less than one of source. It's recommeded
 *				to apply the dithering because the texture has RGB565 format.
 *	@see										NX_CSC_GetDitherEnable,
 *				NX_CSC_SetQuantizationMode,		NX_CSC_GetQuantizationMode,
 *				NX_CSC_SetTransparency,			NX_CSC_SetSize,
 *				NX_CSC_SetSourceAddr,			NX_CSC_SetTargetAddr,
 *				NX_CSC_Run,						NX_CSC_IsBusy
 */
void	NX_CSC_SetDitherEnable( CBOOL bEnb )
{
	const U32 DITHER_POS	= 5;
	const U32 QMODE_MASK	= 1UL<<4;
	const U32 INTENB		= 1UL<<1;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CFALSE == NX_CSC_IsBusy() );	// You have to ensure the CSC is idle.

	pRegister = __g_pRegister;

	regvalue = pRegister->CONTROL;

	regvalue &= (QMODE_MASK | INTENB);

	regvalue |= ( (U32)bEnb << DITHER_POS );

//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether to enable the ditheing.
 *	@return		\b CTRUE	indicates the dithering is applied.\n
 *				\b CFALSE	indicates the dithering is not applied.
 *	@see		NX_CSC_SetDitherEnable,
 *				NX_CSC_SetQuantizationMode,		NX_CSC_GetQuantizationMode,
 *				NX_CSC_SetTransparency,			NX_CSC_SetSize,
 *				NX_CSC_SetSourceAddr,			NX_CSC_SetTargetAddr,
 *				NX_CSC_Run,						NX_CSC_IsBusy
 */
CBOOL	NX_CSC_GetDitherEnable( void )
{
	const U32 DITHER		= 1UL<<5;
	const U32 DITHER_POS	= 5;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->CONTROL & DITHER) >> DITHER_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the quantization mode.
 *	@param[in]	mode	specifies the type of quantization mode.
 *	@return		None.
 *	@remark		The following fomula is used to convert YCbCr to RGB.\n
 *				for 220 quantization mode,
 *				- R = Y + 1.375 * Cr
 *				- G = Y - 0.3359375 * Cb - 0.6953125 * Cr
 *				- B = Y + 1.734375 * Cb
 *				.
 *				for 256 quantization mode,
 *				- R = Y + 1.4020 * Cr
 *				- G = Y - 0.34414 * Cb - 0.71414 * Cr
 *				- B = Y + 1.7720 * Cb
 *	@see		NX_CSC_SetDitherEnable,			NX_CSC_GetDitherEnable,
 *												NX_CSC_GetQuantizationMode,
 *				NX_CSC_SetTransparency,			NX_CSC_SetSize,
 *				NX_CSC_SetSourceAddr,			NX_CSC_SetTargetAddr,
 *				NX_CSC_Run,						NX_CSC_IsBusy
 */
void	NX_CSC_SetQuantizationMode( NX_CSC_QMODE mode )
{
	const U32 DITHER		= 1UL<<5;
	const U32 INTENB		= 1UL<<1;
	const U32 QMODE_POS		= 4;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > mode );
	NX_ASSERT( CFALSE == NX_CSC_IsBusy() );	// You have to ensure the CSC is idle.

	pRegister = __g_pRegister;

	regvalue = pRegister->CONTROL;
	regvalue &= (DITHER | INTENB);
	regvalue |= ((U32)mode)<<QMODE_POS;
//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the quantization mode.
 *	@return		the current quantization mode.
 *	@see		NX_CSC_SetDitherEnable,			NX_CSC_GetDitherEnable,
 *				NX_CSC_SetQuantizationMode,
 *				NX_CSC_SetTransparency,			NX_CSC_SetSize,
 *				NX_CSC_SetSourceAddr,			NX_CSC_SetTargetAddr,
 *				NX_CSC_Run,						NX_CSC_IsBusy
 */
NX_CSC_QMODE	NX_CSC_GetQuantizationMode( void )
{
	const U32 QMODE_POS		= 4;
	const U32 QMODE_MASK	= 1UL<<QMODE_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_CSC_QMODE)((__g_pRegister->CONTROL & QMODE_MASK)>>QMODE_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set configutaion for transparency function.
 *	@param[in]	ReplaceColor	the color which has RGB565 format is used to replace
 *								the result color of CSC in case satisfying the
 *								condition.
 *	@param[in]	bEnb			Specifies whether to test Y/Cb/Cr channel for transparency
 *	@param[in]	bInvY			Specifies whether to invert color test range for Y channel.
 *	@param[in]	MinY			the minimum value of Y channel.
 *	@param[in]	MaxY			the maximum value of Y channel.
 *	@param[in]	bInvCb			Specifies whether to invert color test range for Cb channel.
 *	@param[in]	MinCb			the minimum value of Cb channel.
 *	@param[in]	MaxCb			the maximum value of Cb channel.
 *	@param[in]	bInvCr			Specifies whether to invert color test range for Cr channel.
 *	@param[in]	MinCr			the minimum value of Cr channel.
 *	@param[in]	MaxCr			the maximum value of Cr channel.
 *	@return		None.
 *	@remark		The result color of CSC can be replaced by specified color when
 *				the each component is in individually specified range. The condition
 *				is satisfied when the component is in between minimum value and
 *				maximum value. and you can also invert this	condition.
 *	@see		NX_CSC_SetDitherEnable,			NX_CSC_GetDitherEnable,
 *				NX_CSC_SetQuantizationMode,		NX_CSC_GetQuantizationMode,
 *												NX_CSC_SetSize,
 *				NX_CSC_SetSourceAddr,			NX_CSC_SetTargetAddr,
 *				NX_CSC_Run,						NX_CSC_IsBusy
 */
void	NX_CSC_SetTransparency( U16 ReplaceColor, CBOOL bEnb,
											CBOOL bInvY,	U8 MinY,	U8 MaxY,
											CBOOL bInvCb,	U8 MinCb,	U8 MaxCb,
											CBOOL bInvCr,	U8 MinCr,	U8 MaxCr )
{
	const U32 TPENB_POS		= 19;
	const U32 TPINVCR_POS	= 18;
	const U32 TPINVCB_POS	= 17;
	const U32 TPINVY_POS	= 16;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CFALSE == NX_CSC_IsBusy() );

	pRegister	=	__g_pRegister;

	regvalue	= (U32)ReplaceColor;
	regvalue	|= ( ((bEnb<<(TPENB_POS+2)) | (bEnb<<(TPENB_POS+1)) | (bEnb<<(TPENB_POS+0)))
				| (bInvCr<<TPINVCR_POS) | (bInvCb<<TPINVCB_POS) | (bInvY<<TPINVY_POS) );

//	pRegister->TPCTRL = regvalue;
//	pRegister->TPY	= ((U32)MaxY	<< 8) | ((U32)MinY);
//	pRegister->TPCB	= ((U32)MaxCb	<< 8) | ((U32)MinCb);
//	pRegister->TPCR	= ((U32)MaxCr	<< 8) | ((U32)MinCr);
	WriteIODW(&pRegister->TPCTRL, regvalue);
	WriteIODW(&pRegister->TPY, ((U32)MaxY	<< 8) | ((U32)MinY));
	WriteIODW(&pRegister->TPCB, ((U32)MaxCb	<< 8) | ((U32)MinCb));
	WriteIODW(&pRegister->TPCR, ((U32)MaxCr	<< 8) | ((U32)MinCr));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Specifes the size of the image to convert.
 *	@param[in]	width	the image width in pixels, 16 ~ 1024.
 *	@param[in]	height	the image height in pixels, 16 ~ 1024.
 *	@return		None.
 *	@remark		The result of this function is used to verify parameters of the
 *				function NX_CSC_SetSourceAddr() and NX_CSC_SetTargetAddr() in debug build
 *				environment. Therefore you have to set relevant image size once
 *				at least before	calling the function NX_CSC_SetSourceAddr() or
 *				NX_CSC_SetTargetAddr().
 *	@see		NX_CSC_SetDitherEnable,			NX_CSC_GetDitherEnable,
 *				NX_CSC_SetQuantizationMode,		NX_CSC_GetQuantizationMode,
 *				NX_CSC_SetTransparency,
 *				NX_CSC_SetSourceAddr,			NX_CSC_SetTargetAddr,
 *				NX_CSC_Run,						NX_CSC_IsBusy
 */
void	NX_CSC_SetSize( U32 width, U32 height )
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0 == (width % NX_CSC_ALIGN_WIDTH) );
	NX_ASSERT( 0 == (height % NX_CSC_ALIGN_HEIGHT) );
	NX_ASSERT( NX_CSC_MIN_WIDTH <= width && width <= NX_CSC_MAX_WIDTH );
	NX_ASSERT( NX_CSC_MIN_HEIGHT <= height && height <= NX_CSC_MAX_HEIGHT );
	// You have to ensure the CSC is idle.
	NX_ASSERT( CFALSE == NX_CSC_IsBusy() );

	NX_DEBUG_CODE
	(
		NX_CSC_dbg_dwWidth = width;
		NX_CSC_dbg_dwHeight = height;
	)
//	__g_pRegister->SIZE = ((height - 1) << 16) | (width - 1);
	WriteIODW(&__g_pRegister->SIZE, ((height - 1) << 16) | (width - 1));
	{
		int bsize; // texture swizzling block size
		for( bsize=0; ((width|height) & (1<<bsize))==0; bsize++ ){ }
		WriteIODW(&__g_pRegister->STRIDETEX, (width << 16)|bsize);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Specifies addresses of the source iamge to convert.
 *	@param[in]	AddrY		the Y channel address which has 2D block addressing format.
 *	@param[in]	AddrCb		the Cb channel address which has 2D block addressing format.
 *	@param[in]	AddrCr		the Cr channel address which has 2D block addressing format.
 *	@return		None.
 *	@remark		The 2D block addressing format is as follwings.
 *				- Addr[31:24] specifies the index of segment.
 *				- Addr[23:12] specifies the y-coordinate of upper-left corner of the
 *					image in segment.
 *				- Addr[11: 0] specifies the x-coordinate of upper-left corner of the
 *					image in segment.
 *				.
 *				There's some restriction for the address of CSC and you should set
 *				the address carefully. The address for source image has following
 *				restrictions.
 *				- The x-coordinate of the image must be mulitple of 2^n, where
 *					2^n is greater than or equal to the image width in pixels.
 *					The image width for Cb, Cr has half of the image width for Y.
 *				- The y-coordinate of the image must be multiple of 2^n, where
 *					2^n is greater than or equal to the image height in pixels.
 *					The image height for Cb, Cr has half of the image height for Y.
 *				- The whole image must be in a segment which has 2048x1024 region.
 *				.
 *				If the image size is modified or you did not set the relevant image
 *				size yet, you have to call the function NX_CSC_SetSize() before calling
 *				this function to verify paramters of this function in debug build
 *				environment.
 *	@see		NX_CSC_SetDitherEnable,			NX_CSC_GetDitherEnable,
 *				NX_CSC_SetQuantizationMode,		NX_CSC_GetQuantizationMode,
 *				NX_CSC_SetTransparency,			NX_CSC_SetSize,
 *												NX_CSC_SetTargetAddr,
 *				NX_CSC_Run,						NX_CSC_IsBusy
 */
void	NX_CSC_SetSourceAddr( U32 AddrY, U32 AddrCb, U32 AddrCr )
{
	NX_DEBUG_CODE
	(
		U32 x;
		U32 y;
		U32 width2n;
		U32 height2n;
	)

	//NX_ASSERT( 0 == (AddrY	& 0xE0000000) );
	//NX_ASSERT( 0 == (AddrCb & 0xE0000000) );
	//NX_ASSERT( 0 == (AddrCr & 0xE0000000) );

	// You have to set the size of image before this function is called.
	NX_ASSERT( NX_CSC_MIN_WIDTH	<= NX_CSC_dbg_dwWidth );
	NX_ASSERT( NX_CSC_MIN_HEIGHT <= NX_CSC_dbg_dwHeight );

	NX_DEBUG_CODE
	(
		// Find minimum 2^n which is greater than or equal to the image width / height
		for( width2n = 16; width2n < NX_CSC_dbg_dwWidth; width2n <<= 1 ){	}
		for( height2n = 16; height2n < NX_CSC_dbg_dwHeight; height2n <<= 1 ){	}
	)
	//--------------------------------------------------------------------------
	// Verify Y address
	NX_DEBUG_CODE
	(
		x = AddrY & 0x000007FFUL;
		y = (AddrY >> 12) & 0x3FFUL;
	)
	// x & y must be multiple of 2^n
	NX_ASSERT( 0 == (x % width2n) );
	NX_ASSERT( 0 == (y % height2n) );
	// The whole image must be in a segment.
	NX_ASSERT( (NX_CSC_MAX_WIDTH*2) >= (x + NX_CSC_dbg_dwWidth) );
	NX_ASSERT( NX_CSC_MAX_HEIGHT	>= (y + NX_CSC_dbg_dwHeight) );
	//--------------------------------------------------------------------------
	// Verfify Cb address
	NX_DEBUG_CODE
	(
		x = AddrCb & 0x000007FFUL;
		y = (AddrCb >> 12) & 0x3FFUL;
	)
	// x & y must be multiple of 2^n
	NX_ASSERT( 0 == (x % (width2n /2)) );
	NX_ASSERT( 0 == (y % (height2n/2)) );
	// The whole image must be in a segment.
	NX_ASSERT( (NX_CSC_MAX_WIDTH*2) >= (x + (NX_CSC_dbg_dwWidth /2)) );
	NX_ASSERT( NX_CSC_MAX_HEIGHT	>= (y + (NX_CSC_dbg_dwHeight/2)) );
	//--------------------------------------------------------------------------
	// Verify Cr address
	NX_DEBUG_CODE
	(
		x = AddrCr & 0x000007FFUL;
		y = (AddrCr >> 12) & 0x3FFUL;
	)
	// x & y must be multiple of 2^n
	NX_ASSERT( 0 == (x % (width2n /2)) );
	NX_ASSERT( 0 == (y % (height2n/2)) );
	// the whole image must be in a segment.
	NX_ASSERT( (NX_CSC_MAX_WIDTH*2) >= (x + (NX_CSC_dbg_dwWidth /2)) );
	NX_ASSERT( NX_CSC_MAX_HEIGHT	>= (y + (NX_CSC_dbg_dwHeight/2)) );

	// You have to ensure the CSC is idle.
	NX_ASSERT( CFALSE == NX_CSC_IsBusy() );
	NX_ASSERT( CNULL != __g_pRegister );
//	__g_pRegister->ADDRY	=	(AddrY	& 0xFFC007FF)		// Segment[10:1] & X[10:0]
//							| ((AddrY	& 0x00000800)<<10)	// Segment[0]
//							| ((AddrY	& 0x003FF000)>>1);	// Y[9:0]
//	__g_pRegister->ADDRCB	=	(AddrCb	& 0xFFC007FF)		// Segment[10:1] & X[10:0]
//							| ((AddrCb	& 0x00000800)<<10)	// Segment[0]
//							| ((AddrCb	& 0x003FF000)>>1);	// Y[9:0]
//	__g_pRegister->ADDRCR	=	(AddrCr	& 0xFFC007FF)		// Segment[10:1] & X[10:0]
//							| ((AddrCr	& 0x00000800)<<10)	// Segment[0]
//							| ((AddrCr	& 0x003FF000)>>1);	// Y[9:0]
	WriteIODW(&__g_pRegister->ADDRY, (AddrY	& 0xFFC007FF)		// Segment[10:1] & X[10:0]
							| ((AddrY	& 0x00000800)<<10)	// Segment[0]
							| ((AddrY	& 0x003FF000)>>1));	// Y[9:0]
	WriteIODW(&__g_pRegister->ADDRCB, (AddrCb	& 0xFFC007FF)		// Segment[10:1] & X[10:0]
							| ((AddrCb	& 0x00000800)<<10)	// Segment[0]
							| ((AddrCb	& 0x003FF000)>>1));	// Y[9:0]
	WriteIODW(&__g_pRegister->ADDRCR, (AddrCr	& 0xFFC007FF)		// Segment[10:1] & X[10:0]
							| ((AddrCr	& 0x00000800)<<10)	// Segment[0]
							| ((AddrCr	& 0x003FF000)>>1));	// Y[9:0]
}

//------------------------------------------------------------------------------
/**
 *	@brief		Specifies the texture address to be written.
 *	@param[in]	AddrTex		the texture address which has 2D block addressing format.
 *	@return		None.
 *	@remark		The 2D block addressing format is as follwings.
 *				- Addr[31:24] specifies the index of segment.
 *				- Addr[23:12] specifies the y-coordinate of upper-left corner of the
 *					image in segment.
 *				- Addr[11: 0] specifies the x-coordinate of upper-left corner of the
 *					image in segment.
 *				.
 *				There's some restriction for the address of CSC and you should set
 *				the address carefully. The address for texture image has following
 *				restrictions.
 *				- The x-coordinate of texture image must be mulitple of 2^n, where
 *					2^n is greater than or equal to double of the image width in
 *					pixels.
 *				- The y-coordinate of texture image must be multiple of 2^n, where
 *					2^n is greater than or equal to the image height in pixels.
 *				- The segment of texture image must be in the normal
 *					addressing region.
 *				- The whole image must be in a segment which has 2048x1024 region.
 *				.
 *				If the image size is modified or you did not set the relevant image
 *				size yet, you have to call the function NX_CSC_SetSize() before calling
 *				this function to verify paramters of this function in debug build
 *				environment.
 *	@see		NX_CSC_SetDitherEnable,			NX_CSC_GetDitherEnable,
 *				NX_CSC_SetQuantizationMode,		NX_CSC_GetQuantizationMode,
 *				NX_CSC_SetTransparency,			NX_CSC_SetSize,
 *				NX_CSC_SetSourceAddr,
 *				NX_CSC_Run,						NX_CSC_IsBusy
 */
void	NX_CSC_SetTargetAddr( U32 AddrTex )
{
	NX_DEBUG_CODE
	(
		U32 x;
		U32 y;
		U32 width2n;
		U32 height2n;
	)

	// You have to set the size of image before this function is called.
	NX_ASSERT( NX_CSC_MIN_WIDTH	<= NX_CSC_dbg_dwWidth );
	NX_ASSERT( NX_CSC_MIN_HEIGHT <= NX_CSC_dbg_dwHeight );
	NX_DEBUG_CODE
	(
		// Find minimum 2^n which is greater than or equal to the image width / height
		for( width2n	= 16; width2n	< NX_CSC_dbg_dwWidth;	width2n <<= 1 )	{	}
		for( height2n	= 16; height2n	< NX_CSC_dbg_dwHeight;	height2n <<= 1 ){	}
	)
	NX_DEBUG_CODE
	(
		x = AddrTex & 0x000007FFUL;
		y = (AddrTex >> 12) & 0x3FFUL;
	)
	// x & y must be multiple of 2^n
	NX_ASSERT( 0 == (x % (width2n*2)) );
	NX_ASSERT( 0 == (y % height2n) );
	// The whole image must be in a segment.
	NX_ASSERT( (NX_CSC_MAX_WIDTH*2) >= (x + (NX_CSC_dbg_dwWidth*2)) );
	NX_ASSERT( NX_CSC_MAX_HEIGHT	>= (y + NX_CSC_dbg_dwHeight) );
	// The segment must be in the normal addressing region.
	//NX_ASSERT( 0 == (AddrTex & 0xE0000000) );

	// You have to ensure the CSC is idle.
	NX_ASSERT( CFALSE == NX_CSC_IsBusy() );
	NX_ASSERT( CNULL != __g_pRegister );
//	__g_pRegister->ADDRTEX	=	(AddrTex & 0xFFC007FF)		// Segment[10:1] & X[10:0]
//							| ((AddrTex & 0x00000800)<<10)	// Segment[0]
//							| ((AddrTex & 0x003FF000)>>1);	// Y[9:0]
	/*WriteIODW(&__g_pRegister->ADDRTEX, (AddrTex & 0xFFC007FF)		// Segment[10:1] & X[10:0]
							| ((AddrTex & 0x00000800)<<10)	// Segment[0]
							| ((AddrTex & 0x003FF000)>>1));*/	// Y[9:0]
	WriteIODW(&__g_pRegister->ADDRTEX, AddrTex );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Run CSC to perform a color space conversion.
 *	@return		None.
 *	@see		NX_CSC_SetDitherEnable,			NX_CSC_GetDitherEnable,
 *				NX_CSC_SetQuantizationMode,		NX_CSC_GetQuantizationMode,
 *				NX_CSC_SetTransparency,			NX_CSC_SetSize,
 *				NX_CSC_SetSourceAddr,			NX_CSC_SetTargetAddr,
 *												NX_CSC_IsBusy
 */
void	NX_CSC_Run( void )
{
	const U32 DITHER		= 1UL<<5;
	const U32 QMODE_MASK	= 1UL<<4;
	const U32 INTENB		= 1UL<<1;
	const U32 RUN			= 1UL<<0;

	register U32 regvalue;
	register struct NX_CSC_RegisterSet* pRegister;


	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CFALSE == NX_CSC_IsBusy() );

	pRegister = __g_pRegister;

	regvalue = pRegister->CONTROL;
	regvalue &= (DITHER | QMODE_MASK | INTENB);
	regvalue |= RUN;
//	pRegister->CONTROL = regvalue;
	WriteIODW(&pRegister->CONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether the CSC is busy or not.
 *	@return		\b CTRUE	indicates the CSC is busy.\n
 *				\b CFALSE	indicates the CSC is idle.
 *	@see		NX_CSC_SetDitherEnable,			NX_CSC_GetDitherEnable,
 *				NX_CSC_SetQuantizationMode,		NX_CSC_GetQuantizationMode,
 *				NX_CSC_SetTransparency,			NX_CSC_SetSize,
 *				NX_CSC_SetSourceAddr,			NX_CSC_SetTargetAddr,
 *				NX_CSC_Run
 */
CBOOL	NX_CSC_IsBusy( void )
{
	const U32 RUN		= 1UL<<0;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->CONTROL & RUN );
}
