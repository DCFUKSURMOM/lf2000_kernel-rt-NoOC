//------------------------------------------------------------------------------
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: MPEG TS Interface
//	File		: nx_mpegtsif.c
//	Description	:
//	Author		: Goofy
//	History		:
//------------------------------------------------------------------------------

#include "nx_mpegtsif.h"

//------------------------------------------------------------------------------
// Internal global varialbles
//------------------------------------------------------------------------------
static struct NX_MPEGTSIF_RegisterSet *__g_pRegister = CNULL;


//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize prototype environment and local variables.
 *	@return		CTRUE indicates success.\n
 *				CFALSE indicates failure.
 *	@remark		You have to call this function before using other functions.
 *	@see		NX_MPEGTSIF_GetNumberOfModule
 */
CBOOL	NX_MPEGTSIF_Initialize( void )
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
 *	@return		number of module in the chip.
 *	@see		NX_MPEGTSIF_Initialize
 */
U32		NX_MPEGTSIF_GetNumberOfModule( void )
{
	return NUMBER_OF_MPEGTSIF_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a physical address of mudule.
 *	@return		a physical address of module.
 *	@see		NX_MPEGTSIF_GetSizeOfRegisterSet,
 *				NX_MPEGTSIF_SetBaseAddress,		NX_MPEGTSIF_GetBaseAddress,
 *				NX_MPEGTSIF_OpenModule,			NX_MPEGTSIF_CloseModule,
 *				NX_MPEGTSIF_CheckBusy,			NX_MPEGTSIF_CanPowerDown
 */
U32		NX_MPEGTSIF_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_MPEGTSIF_MODULE + (OFFSET_OF_MPEGTSIF_MODULE * 0) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in bytes, of register set.
 *	@return		Byte size of module's register set.
 *	@see		NX_MPEGTSIF_GetPhysicalAddress,
 *				NX_MPEGTSIF_SetBaseAddress,		NX_MPEGTSIF_GetBaseAddress,
 *				NX_MPEGTSIF_OpenModule,			NX_MPEGTSIF_CloseModule,
 *				NX_MPEGTSIF_CheckBusy,			NX_MPEGTSIF_CanPowerDown
 */
U32		NX_MPEGTSIF_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_MPEGTSIF_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress	Module's base address
 *	@return		None.
 *	@remark		You have to call this function before using NX_MPEGTSIF_OpenModule(),
 *				NX_MPEGTSIF_CloseModule(), NX_MPEGTSIF_CheckBusy(), NX_MPEGTSIF_CanPowerDown(),
 *				Interrupt Interfaces, DMA Interfaces, Clock Control Interfaces,
 *				and Module Specific Functions.\n
 *				You can use this function with:\n
 *				- virtual address system such as WinCE or Linux.
 *	@code
 *		U32 PhyAddr, VirAddr;
 *		PhyAddr = NX_MPEGTSIF_GetPhysicalAddress();
 *		VirAddr = MEMORY_MAP_FUNCTION_TO_VIRTUAL( PhyAddr, ... );
 *		NX_MPEGTSIF_SetBaseAddress( VirAddr );
 *	@endcode
 *				- physical address system such as Non-OS.
 *	@code
 *		NX_MPEGTSIF_SetBaseAddress( NX_MPEGTSIF_GetPhysicalAddress() );
 *	@endcode
 *
 *	@see		NX_MPEGTSIF_GetPhysicalAddress,	NX_MPEGTSIF_GetSizeOfRegisterSet,
 *				NX_MPEGTSIF_GetBaseAddress,
 *				NX_MPEGTSIF_OpenModule,			NX_MPEGTSIF_CloseModule,
 *				NX_MPEGTSIF_CheckBusy,			NX_MPEGTSIF_CanPowerDown
 */
void	NX_MPEGTSIF_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	__g_pRegister = (struct NX_MPEGTSIF_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_MPEGTSIF_GetPhysicalAddress,	NX_MPEGTSIF_GetSizeOfRegisterSet,
 *				NX_MPEGTSIF_SetBaseAddress,
 *				NX_MPEGTSIF_OpenModule,			NX_MPEGTSIF_CloseModule,
 *				NX_MPEGTSIF_CheckBusy,			NX_MPEGTSIF_CanPowerDown
 */
U32		NX_MPEGTSIF_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		CTRUE indicates success.\n
 *				CFALSE indicates failure.
 *	@remark		This function makes a module is initialized by default value.
 *				Thererfore after calling this function, some registers may be
 *				modified to default state. If you don't want to change any
 *				setting of registers, you can skip to call this function. But
 *				you have to call this function at least once to use these
 *				prototype functions after Power-On-Reset or NX_MPEGTSIF_CloseModule().\n
 *				\b IMPORTANT : you have to enable a PCLK before calling this function.
 *	@code
 *		// Initialize the clock generator
 *		NX_MPEGTSIF_SetClockPClkMode( NX_PCLKMODE_ALWAYS );
 *
 *		NX_MPEGTSIF_OpenModule();
 *
 *		// ......
 *		// Do something
 *		// ......
 *
 *		NX_MPEGTSIF_CloseModule();
 *	@endcode
 *	@see		NX_MPEGTSIF_GetPhysicalAddress,	NX_MPEGTSIF_GetSizeOfRegisterSet,
 *				NX_MPEGTSIF_SetBaseAddress,		NX_MPEGTSIF_GetBaseAddress,
 *				NX_MPEGTSIF_CloseModule,
 *				NX_MPEGTSIF_CheckBusy,			NX_MPEGTSIF_CanPowerDown
 */
CBOOL	NX_MPEGTSIF_OpenModule( void )
{
	const U32 DATAMODE	= 1UL<<7;	// supports only 1-bit serial mode.
	const U32 IRQPEND	= 1UL<<15;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->CTRL = IRQPEND | DATAMODE;
	WriteIODW(&__g_pRegister->CTRL, IRQPEND | DATAMODE);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		De-initialize selected module to the proper stage.
 *	@return		CTRUE indicates success.\n
 *				CFALSE indicates failure.
 *	@see		NX_MPEGTSIF_GetPhysicalAddress,	NX_MPEGTSIF_GetSizeOfRegisterSet,
 *				NX_MPEGTSIF_SetBaseAddress,		NX_MPEGTSIF_GetBaseAddress,
 *				NX_MPEGTSIF_OpenModule,
 *				NX_MPEGTSIF_CheckBusy,			NX_MPEGTSIF_CanPowerDown
 */
CBOOL	NX_MPEGTSIF_CloseModule( void )
{
	const U32 DATAMODE	= 1UL<<7;	// supports only 1-bit serial mode.
	const U32 IRQPEND	= 1UL<<15;

	NX_ASSERT( CNULL != __g_pRegister );

	WriteIODW(&__g_pRegister->CTRL, IRQPEND | DATAMODE);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		CTRUE indicates the selected module is busy.\n
 *				CFALSE indicates the selected module is idle.
 *	@see		NX_MPEGTSIF_GetPhysicalAddress,	NX_MPEGTSIF_GetSizeOfRegisterSet,
 *				NX_MPEGTSIF_SetBaseAddress,		NX_MPEGTSIF_GetBaseAddress,
 *				NX_MPEGTSIF_OpenModule,			NX_MPEGTSIF_CloseModule,
 *				NX_MPEGTSIF_CanPowerDown
 */
CBOOL	NX_MPEGTSIF_CheckBusy( void )
{
	const U32 ENB	= 1UL<<6;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->CTRL & ENB )		return CTRUE;

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage.
 *	@return		CTRUE indicates the selected module is ready to enter power-down stage.\n
 *				CFALSE indicates the selected module is busy and not ready to enter power-down stage.
 *	@see		NX_MPEGTSIF_GetPhysicalAddress,	NX_MPEGTSIF_GetSizeOfRegisterSet,
 *				NX_MPEGTSIF_SetBaseAddress,		NX_MPEGTSIF_GetBaseAddress,
 *				NX_MPEGTSIF_OpenModule,			NX_MPEGTSIF_CloseModule,
 *				NX_MPEGTSIF_CheckBusy
 */
CBOOL	NX_MPEGTSIF_CanPowerDown( void )
{
	if( NX_MPEGTSIF_CheckBusy() )	return CFALSE;
	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt number for the interrupt controller.
 *	@return		An interrupt number.
 *	@remark		Return value can be used for the interrupt controller module's
 *				functions.
 *	@see												NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
S32		NX_MPEGTSIF_GetInterruptNumber( void )
{
	return	INTNUM_OF_MPEGTSIF_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enabled or disabled.
 *	@param[in]	IntNum			Specifies an interrupt number.
 *	@param[in]	bEnb				Set it as CTRUE to enable an interrupt specified by @a IntNum. \n
 *								Set it as CFALSE to diable an interrupt specified by @a IntNum.
 *	@return		None.
 *	@remark		Since the MPEGTSIF module has one interrupt source, you have to set
 *				@a IntNum as '0' only.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,		
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
void	NX_MPEGTSIF_SetInterruptEnable( S32 IntNum, CBOOL bEnb )
{
	register U32 regval;

	const U32 IRQENB_POS	= 8;
	const U32 IRQENB		= 1UL<<IRQENB_POS;
	const U32 IRQPEND		= 1UL<<15;

	NX_ASSERT( 0==IntNum );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL;
	regval &= ~(IRQENB | IRQPEND);
	regval |= ((U32)bEnb)<<IRQENB_POS;
//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum			Specifies an interrupt number.
 *	@return		CTRUE indicates an interrupt specified by @a IntNum is enabled.\n
 *				CFALSE indicates an interrupt specified by @a IntNum is disabled.
 *	@remark		Since the MPEGTSIF module has one interrupt source, you have to set
 *				@a IntNum as '0' only.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *														NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
CBOOL	NX_MPEGTSIF_GetInterruptEnable( S32 IntNum )
{
	const U32 IRQENB_POS	= 8;

	NX_ASSERT( 0==IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->CTRL >> IRQENB_POS) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set Transfer Done interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
void	NX_MPEGTSIF_SetInterruptEnable32( U32 EnableFlag )
{
	register U32 regval;

	const U32 IRQENB_POS	= 8;
	const U32 IRQENB		= 1UL<<IRQENB_POS;
	const U32 IRQPEND		= 1UL<<15;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL;
	regval &= ~(IRQENB | IRQPEND);
	regval |= ((U32)EnableFlag & 0x01)<<IRQENB_POS;
//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : Transfer Done interrupt's setting value. \n
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *														NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
U32		NX_MPEGTSIF_GetInterruptEnable32( void )
{
	const U32 IRQENB_POS	= 8;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->CTRL >> IRQENB_POS) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not.
 *	@param[in]	IntNum			Specifies an interrupt number.
 *	@return		CTRUE indicates an interrupt specified by @a IntNum is pended.\n
 *				CFALSE indicates an interrupt specified by @a IntNum is not pended.
 *	@remark		The interrupt pending status are logged regardless of interrupt
 *				enable status. Therefore the return value can be CTRUE even
 *				though the specified interrupt has been disabled.\n
 *				Since the MPEGTSIF module has one interrupt source, you have to set
 *				@a IntNum as '0' only.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
CBOOL	NX_MPEGTSIF_GetInterruptPending( S32 IntNum )
{
	const U32 IRQPEND_POS	= 15;

	NX_ASSERT( 0==IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->CTRL >> IRQPEND_POS) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : Transfer Done pending state. \n
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *														NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
U32		NX_MPEGTSIF_GetInterruptPending32( void )
{
	const U32 IRQPEND_POS	= 15;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->CTRL >> IRQPEND_POS) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum			Specifies an interrupt number.
 *	@return		None.
 *	@remark		Since the MPEGTSIF module has one interrupt source, you have to set
 *				@a IntNum as '0' only.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
void	NX_MPEGTSIF_ClearInterruptPending( S32 IntNum )
{
	const U32 IRQPEND	= 1UL<<15;
	register U32 regval;

	NX_ASSERT( 0==IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->CTRL |= IRQPEND;
	regval = __g_pRegister->CTRL | IRQPEND;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : Transfer Done pending bit. \n
 *	@return		None.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *														NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
void	NX_MPEGTSIF_ClearInterruptPending32( U32 PendingFlag )
{
	const U32 IRQPEND_POS	= 15;
	const U32 IRQPEND_MASK	= 1UL<<IRQPEND_POS;

	register U32 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->CTRL & ~IRQPEND_MASK;

//	__g_pRegister->CTRL = (U16)(ReadValue | ((PendingFlag & 0x01)<<IRQPEND_POS));
	WriteIODW(&__g_pRegister->CTRL, (U16)(ReadValue | ((PendingFlag & 0x01)<<IRQPEND_POS)));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enabled or disabled.
 *	@param[in]	bEnb			Set it as CTRUE to enable all interrupts. \n
 *								Set it as CFALSE to disable all interrupts.
 *	@return		None.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
void	NX_MPEGTSIF_SetInterruptEnableAll( CBOOL bEnb )
{
	register U32 regval;

	const U32 IRQENB_POS	= 8;
	const U32 IRQENB		= 1UL<<IRQENB_POS;
	const U32 IRQPEND		= 1UL<<15;

	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL;
	regval &= ~(IRQENB | IRQPEND);
	regval |= ((U32)bEnb)<<IRQENB_POS;
//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enabled or not.
 *	@return		CTURE indicates there's interrupts which are enabled.\n
 *				CFALSE indicates there's no interrupt which are enabled.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *														NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
CBOOL	NX_MPEGTSIF_GetInterruptEnableAll( void )
{
	const U32 IRQENB_POS	= 8;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->CTRL >> IRQENB_POS) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		CTURE indicates there's some interrupts which are pended.\n
 *				CFALSE indicates there's no interrupt which are pended.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll,	NX_MPEGTSIF_GetInterruptPendingNumber
 */
CBOOL	NX_MPEGTSIF_GetInterruptPendingAll( void )
{
	const U32 IRQPEND_POS	= 15;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->CTRL >> IRQPEND_POS) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *														NX_MPEGTSIF_GetInterruptPendingNumber
 */
void	NX_MPEGTSIF_ClearInterruptPendingAll( void )
{
	const U32 IRQPEND	= 1UL<<15;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->CTRL |= IRQPEND;
	regval = __g_pRegister->CTRL | IRQPEND;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt number which has the most prority of pended interrupts.
 *	@return		an interrupt number which has the most priority of pended interrupts.
 *				If there's no interrupt which is pended, it returns -1.
 *	@see		NX_MPEGTSIF_GetInterruptNumber,			NX_MPEGTSIF_SetInterruptEnable,
 *				NX_MPEGTSIF_GetInterruptEnable,			NX_MPEGTSIF_SetInterruptEnable32,
 *				NX_MPEGTSIF_GetInterruptEnable32,		NX_MPEGTSIF_GetInterruptPending,
 *				NX_MPEGTSIF_GetInterruptPending32,		NX_MPEGTSIF_ClearInterruptPending,
 *				NX_MPEGTSIF_ClearInterruptPending32,	NX_MPEGTSIF_SetInterruptEnableAll,
 *				NX_MPEGTSIF_GetInterruptEnableAll,		NX_MPEGTSIF_GetInterruptPendingAll,
 *				NX_MPEGTSIF_ClearInterruptPendingAll	
 */
S32		NX_MPEGTSIF_GetInterruptPendingNumber( void )
{
	const U32 IRQPEND	= 1UL<<15;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->CTRL & IRQPEND )		return 0;

	return -1;
}


//------------------------------------------------------------------------------
// DMA Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a DMA peripheral index for the DMA controller.
 *	@return		a DMA peripheral index.
 *	@remark		Return value can be used for the DMA controller module's
 *				functions.
 *	@see		NX_MPEGTSIF_GetDMABusWidth
 */
U32		NX_MPEGTSIF_GetDMAIndex( void )
{
	return DMAINDEX_OF_MPEGTSIF_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a DMA peripheral bus width for the DMA controller.
 *	@return		a DMA peripheral bus width.
 *	@remark		Return value can be used for the DMA controller module's
 *				functions.
 *	@see		NX_MPEGTSIF_GetDMAIndex
 */
U32		NX_MPEGTSIF_GetDMABusWidth( void )
{
	return 32;		// only support 32 bit data width.
}


//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode.
 *	@param[in]	mode			Specifies a PCLK mode to change.
 *	@return		None.
 *	@remark		MPEGTSIF doesn't support NX_PCLKMODE_DYNAMIC. If you call this
 *				function with NX_PCLKMODE_DYNAMIC, it makes that PCLK doesn't
 *				provide to the MPEGTSIF module and MPEGTSIF module doesn't operate
 *				correctly. You have to set a PCLK mode as NX_PCLKMODE_ALWAYS
 *				to operate and control the MPEGTSIF module. But you can set a PCLK
 *				mode as NX_PCLKMODE_DYNAMIC to slightly reduce a power
 *				cunsumption if you don't want to use the MPEGTSIF module.\n
 *				\b IMPORTANT : \n
 *				Since changing a clock controller register of the MPEGTSIF module
 *				causes a reset of controller register of the MPEGTSIF module,
 *				you have to reconfigure all settings of the MPEGTSIF module
 *				after calling this function.
 *	@see		NX_PCLKMODE,	NX_MPEGTSIF_GetClockPClkMode,
 */
void	NX_MPEGTSIF_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE	= 1UL<<3;
	const U32 IRQPEND	= 1UL<<15;

	register U32 regvalue, regctrl;

	NX_ASSERT( CNULL != __g_pRegister );

	regctrl	= __g_pRegister->CTRL;				// back-up control register before changing CLKENB.
	regvalue	= __g_pRegister->CLKENB;

	switch( mode )
	{
		case NX_PCLKMODE_DYNAMIC:	regvalue &= ~PCLKMODE;	break;
		case NX_PCLKMODE_ALWAYS :	regvalue |=	PCLKMODE;	break;
		default:					NX_ASSERT( CFALSE );	break;
	}

//	__g_pRegister->CLKENB	= regvalue;
//	__g_pRegister->CTRL		= regctrl & ~IRQPEND;	// restore control register after changing CLKENB.
	WriteIODW(&__g_pRegister->CLKENB, regvalue);
	WriteIODW(&__g_pRegister->CTRL, regctrl & ~IRQPEND);	// restore control register after changing CLKENB.
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode.
 *	@return		Current PCLK mode.
 *	@see		NX_PCLKMODE,	NX_MPEGTSIF_SetClockPClkMode,
 */
NX_PCLKMODE	NX_MPEGTSIF_GetClockPClkMode( void )
{
	const U32 PCLKMODE = 1UL<<3;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->CLKENB & PCLKMODE ) ? NX_PCLKMODE_ALWAYS : NX_PCLKMODE_DYNAMIC;
}


//------------------------------------------------------------------------------
// MPEGTSIF Specific Interfaces
//------------------------------------------------------------------------------
/**
 *	@brief		Set the clock polarity to fetch data.
 *	@param[in]	pol		Specifies the clock polarity to fetch data which is one
 *						one of @ref NX_MPEGTSIF_CLOCKPOL enum.
 *	@return		None.
 *	@see		NX_MPEGTSIF_CLOCKPOL,
 *												NX_MPEGTSIF_GetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,	NX_MPEGTSIF_GetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,		NX_MPEGTSIF_GetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,		NX_MPEGTSIF_GetSyncMode,
 *				NX_MPEGTSIF_SetWordCount,		NX_MPEGTSIF_GetWordCount
 */
void	NX_MPEGTSIF_SetClockPolarity( NX_MPEGTSIF_CLOCKPOL pol )
{
	const U32 IRQPEND	= 1UL<<15;
	const U32 CLKPOL	= 1UL<<9;
	register U32 regval;

	NX_ASSERT( NX_MPEGTSIF_CLOCKPOL_RISING==pol || NX_MPEGTSIF_CLOCKPOL_FALLING==pol );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL & ~(IRQPEND | CLKPOL);
	if( pol == NX_MPEGTSIF_CLOCKPOL_RISING )
		regval |= CLKPOL;
//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the clock polarity to fetch data.
 *	@return		the clock polarity to fetch data which is one of @ref NX_MPEGTSIF_CLOCKPOL enum.
 *	@see		NX_MPEGTSIF_CLOCKPOL,
 *				NX_MPEGTSIF_SetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,	NX_MPEGTSIF_GetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,		NX_MPEGTSIF_GetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,		NX_MPEGTSIF_GetSyncMode,
 *				NX_MPEGTSIF_SetWordCount,		NX_MPEGTSIF_GetWordCount
 */
NX_MPEGTSIF_CLOCKPOL	NX_MPEGTSIF_GetClockPolarity( void )
{
	const U32 CLKPOL	= 1UL<<9;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(__g_pRegister->CTRL & CLKPOL) ? NX_MPEGTSIF_CLOCKPOL_RISING : NX_MPEGTSIF_CLOCKPOL_FALLING;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the data polarity of external DP signal.
 *	@param[in]	pol		Specifies the data polarity of external DP signal which
 *						is one of @ref NX_MPEGTSIF_DATAPOL enum.
 *	@return		None.
 *	@see		NX_MPEGTSIF_DATAPOL,
 *				NX_MPEGTSIF_SetClockPolarity,	NX_MPEGTSIF_GetClockPolarity,
 *												NX_MPEGTSIF_GetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,		NX_MPEGTSIF_GetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,		NX_MPEGTSIF_GetSyncMode,
 *				NX_MPEGTSIF_SetWordCount,		NX_MPEGTSIF_GetWordCount
 */
void	NX_MPEGTSIF_SetDataPolarity( NX_MPEGTSIF_DATAPOL pol )
{
	const U32 IRQPEND	= 1UL<<15;
	const U32 DPPOL		= 1UL<<10;
	register U32 regval;

	NX_ASSERT( NX_MPEGTSIF_DATAPOL_HIGH==pol || NX_MPEGTSIF_DATAPOL_LOW==pol );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL & ~(IRQPEND | DPPOL);
	if( pol == NX_MPEGTSIF_DATAPOL_LOW )
		regval |= DPPOL;
//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the data polarity of external DP signal.
 *	@return		the data polarity of external DP signal which is one of @ref NX_MPEGTSIF_DATAPOL enum.
 *	@see		NX_MPEGTSIF_DATAPOL,
 *				NX_MPEGTSIF_SetClockPolarity,	NX_MPEGTSIF_GetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,		NX_MPEGTSIF_GetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,		NX_MPEGTSIF_GetSyncMode,
 *				NX_MPEGTSIF_SetWordCount,		NX_MPEGTSIF_GetWordCount
 */
NX_MPEGTSIF_DATAPOL	NX_MPEGTSIF_GetDataPolarity( void )
{
	const U32 DPPOL		= 1UL<<10;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(__g_pRegister->CTRL & DPPOL) ? NX_MPEGTSIF_DATAPOL_LOW : NX_MPEGTSIF_DATAPOL_HIGH;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the SYNC source.
 *	@param[in]	sync	Specifies the SYNC source which is one of @ref NX_MPEGTSIF_SYNC enum.
 *	@return		None.
 *	@see		NX_MPEGTSIF_SYNC,
 *				NX_MPEGTSIF_SetClockPolarity,	NX_MPEGTSIF_GetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,	NX_MPEGTSIF_GetDataPolarity,
 *												NX_MPEGTSIF_GetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,		NX_MPEGTSIF_GetSyncMode,
 *				NX_MPEGTSIF_SetWordCount,		NX_MPEGTSIF_GetWordCount
 */
void	NX_MPEGTSIF_SetSyncSource( NX_MPEGTSIF_SYNC sync )
{
	const U32 IRQPEND	= 1UL<<15;
	const U32 SYNCSEL	= 1UL<<11;
	register U32 regval;

	NX_ASSERT( NX_MPEGTSIF_SYNC_INTERNAL==sync || NX_MPEGTSIF_SYNC_EXTERNAL==sync );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL & ~(IRQPEND | SYNCSEL);
	if( sync == NX_MPEGTSIF_SYNC_INTERNAL )
		regval |= SYNCSEL;
//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the SYNC source.
 *	@return		the SYNC source which is one of @ref NX_MPEGTSIF_SYNC enum.
 *	@see		NX_MPEGTSIF_SYNC,
 *				NX_MPEGTSIF_SetClockPolarity,	NX_MPEGTSIF_GetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,	NX_MPEGTSIF_GetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,		NX_MPEGTSIF_GetSyncMode,
 *				NX_MPEGTSIF_SetWordCount,		NX_MPEGTSIF_GetWordCount
 */
NX_MPEGTSIF_SYNC	NX_MPEGTSIF_GetSyncSource( void )
{
	const U32 SYNCSEL	= 1UL<<11;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(__g_pRegister->CTRL & SYNCSEL) ? NX_MPEGTSIF_SYNC_INTERNAL : NX_MPEGTSIF_SYNC_EXTERNAL;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the SYNC mode.
 *	@param[in]	syncmode	Specifies the SYNC mode which is one of @ref NX_MPEGTSIF_SYNCMODE enum.
 *	@return		None.
 *	@remarks	Bit Sync means that active sync period is a one bit time. \n
 *				Byte Sync means that active sync period is a one byte time.
 *	@see		NX_MPEGTSIF_SYNCMODE,
 *				NX_MPEGTSIF_SetClockPolarity,	NX_MPEGTSIF_GetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,	NX_MPEGTSIF_GetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,		NX_MPEGTSIF_GetSyncSource,
 *												NX_MPEGTSIF_GetSyncMode,
 *				NX_MPEGTSIF_SetWordCount,		NX_MPEGTSIF_GetWordCount
 */
void	NX_MPEGTSIF_SetSyncMode( NX_MPEGTSIF_SYNCMODE syncmode )
{
	const U32 IRQPEND			= 1UL<<15;
	const U32 SYNCMODE_BITPOS	= 12;
	const U32 SYNCMODE_MASK		= 0x01 << SYNCMODE_BITPOS;

	register U32 regval;

	NX_ASSERT( (NX_MPEGTSIF_SYNCMODE_BIT==syncmode) || (NX_MPEGTSIF_SYNCMODE_BYTE==syncmode) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL & ~(IRQPEND | SYNCMODE_MASK);

	if( NX_MPEGTSIF_SYNCMODE_BYTE == syncmode )
		regval |= SYNCMODE_MASK;

//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the SYNC mode.
 *	@return		the SYNC mode which is one of @ref NX_MPEGTSIF_SYNCMODE enum.
 *	@see		NX_MPEGTSIF_SYNCMODE,
 *				NX_MPEGTSIF_SetClockPolarity,	NX_MPEGTSIF_GetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,	NX_MPEGTSIF_GetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,		NX_MPEGTSIF_GetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,
 *				NX_MPEGTSIF_SetWordCount,		NX_MPEGTSIF_GetWordCount
 */
NX_MPEGTSIF_SYNCMODE	NX_MPEGTSIF_GetSyncMode( void )
{
	const U32 SYNCMODE_BITPOS = 12;
	const U32 SYNCMODE_MASK = 0x01 << SYNCMODE_BITPOS;

	return	(__g_pRegister->CTRL & SYNCMODE_MASK) ? NX_MPEGTSIF_SYNCMODE_BYTE : NX_MPEGTSIF_SYNCMODE_BIT;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Set the number of receiving 32-bit data between SYNC signals.
 *	@param[in]	WordCnt	Specifies the number of receiving 32-bit data between 1 and 64.
 *	@return		None.
 *	@remarks	The MPEGTSIF module generates an interrupt after data corresponding
 *				to the number assinged by @a WordCnt of this function are received.
 *	@see		NX_MPEGTSIF_SetClockPolarity,	NX_MPEGTSIF_GetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,	NX_MPEGTSIF_GetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,		NX_MPEGTSIF_GetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,		NX_MPEGTSIF_GetSyncMode,
 *												NX_MPEGTSIF_GetWordCount
 */
void	NX_MPEGTSIF_SetWordCount( U32 WordCnt )
{
	const U32 IRQPEND = 1UL<<15;
	const U32 WORDCNT = 0x3F;
	register U32 regval;

	NX_ASSERT( 1<=WordCnt &&	64>=WordCnt );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL;
	regval &= ~(WORDCNT | IRQPEND);
	regval |= ((WordCnt-1) & 0x3F);		// set n-1
//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the number of receiving 32-bit data between SYNC signals.
 *	@return		the number of receiving 32-bit data.
 *	@see		NX_MPEGTSIF_SetClockPolarity,	NX_MPEGTSIF_GetClockPolarity,
 *				NX_MPEGTSIF_SetDataPolarity,	NX_MPEGTSIF_GetDataPolarity,
 *				NX_MPEGTSIF_SetSyncSource,		NX_MPEGTSIF_GetSyncSource,
 *				NX_MPEGTSIF_SetSyncMode,		NX_MPEGTSIF_GetSyncMode,
 *				NX_MPEGTSIF_SetWordCount
 */
U32		NX_MPEGTSIF_GetWordCount( void )
{
	const U32 WORDCNT = 0x3F;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->CTRL & WORDCNT) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable the MPEG-TS Interface module.
 *	@param[in]	bEnb	Set this as CTRUE to enable the MPEG-TS interface module. \n
 *						Set this as CFALSE to disable the MPEG-TS interface module.
 *	@return		None.
 *	@see		NX_MPEGTSIF_GetEnable, NX_MPEGTSIF_GetErrorFlag
 */
void	NX_MPEGTSIF_SetEnable( CBOOL bEnb )
{
	const U32 IRQPEND	= 1UL<<15;
	const U32 ENB_POS	= 6;
	const U32 ENB		= 1UL<<ENB_POS;
	register U32 regval;

	NX_ASSERT( 0==bEnb || 1==bEnb );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CTRL;
	regval &= ~(ENB | IRQPEND);
	regval |= ((bEnb&1)<<ENB_POS);
//	__g_pRegister->CTRL = regval;
	WriteIODW(&__g_pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the MPEG-TS interface module is enabled or not.
 *	@return		CTRUE indicates the MPEG-TS interface module is enabled.\n
 *				CFALSE indicates the MPEG-TS interface module is disabled.
 *	@see		NX_MPEGTSIF_SetEnable, NX_MPEGTSIF_GetErrorFlag
 */
CBOOL	NX_MPEGTSIF_GetEnable( void )
{
	const U32 ENB_POS	= 6;
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->CTRL >> ENB_POS) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the external error signal state.
 *	@return		CTRUE indicates that the external error signal is set.\n
 *				CFALSE indicates that the external error signal is not set.
 *	@remarks	You can check the error state receiving from the MPEG-TS decoder
 *				after finishing to receive a packet by using this function.
 *	@see		NX_MPEGTSIF_SetEnable, NX_MPEGTSIF_GetEnable
 */
CBOOL	NX_MPEGTSIF_GetErrorFlag( void )
{
	const U32 ERRFLAG	= 1UL<<14;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->CTRL & ERRFLAG) ? CTRUE : CFALSE;
}
