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
//	Module		: Clock & Power Manager
//	File		: nx_clkpwr.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_clkpwr.h"


static	struct NX_CLKPWR_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize of prototype enviroment & local variables.
 *	@return		\b CTRUE	indicates that Initialize is succeeded.\n
 *				\b CFALSE	indicates that Initialize is failed.\n
 *	@see									NX_CLKPWR_GetNumberOfModule
 */
CBOOL	NX_CLKPWR_Initialize( void )
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
 *	@see		NX_CLKPWR_Initialize
 */
U32		NX_CLKPWR_GetNumberOfModule( void )
{
	return NUMBER_OF_CLKPWR_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see											NX_CLKPWR_GetSizeOfRegisterSet,
 *				NX_CLKPWR_SetBaseAddress,			NX_CLKPWR_GetBaseAddress,
 *				NX_CLKPWR_OpenModule,				NX_CLKPWR_CloseModule,
 *				NX_CLKPWR_CheckBusy,				NX_CLKPWR_CanPowerDown
 */
U32		NX_CLKPWR_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_CLKPWR_MODULE );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_CLKPWR_GetPhysicalAddress,
 *				NX_CLKPWR_SetBaseAddress,			NX_CLKPWR_GetBaseAddress,
 *				NX_CLKPWR_OpenModule,				NX_CLKPWR_CloseModule,
 *				NX_CLKPWR_CheckBusy,				NX_CLKPWR_CanPowerDown
 */
U32		NX_CLKPWR_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_CLKPWR_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_CLKPWR_GetPhysicalAddress,		NX_CLKPWR_GetSizeOfRegisterSet,
 *													NX_CLKPWR_GetBaseAddress,
 *				NX_CLKPWR_OpenModule,				NX_CLKPWR_CloseModule,
 *				NX_CLKPWR_CheckBusy,				NX_CLKPWR_CanPowerDown
 */
void	NX_CLKPWR_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_CLKPWR_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_CLKPWR_GetPhysicalAddress,		NX_CLKPWR_GetSizeOfRegisterSet,
 *				NX_CLKPWR_SetBaseAddress,
 *				NX_CLKPWR_OpenModule,				NX_CLKPWR_CloseModule,
 *				NX_CLKPWR_CheckBusy,				NX_CLKPWR_CanPowerDown
 */
U32		NX_CLKPWR_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicates that Initialize is succeeded. \n
 *				\b CFALSE	indicates that Initialize is failed.
 *	@see		NX_CLKPWR_GetPhysicalAddress,		NX_CLKPWR_GetSizeOfRegisterSet,
 *				NX_CLKPWR_SetBaseAddress,			NX_CLKPWR_GetBaseAddress,
 *													NX_CLKPWR_CloseModule,
 *				NX_CLKPWR_CheckBusy,				NX_CLKPWR_CanPowerDown
 */
CBOOL	NX_CLKPWR_OpenModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicates that Deinitialize is succeeded. \n
 *				\b CFALSE	indicates that Deinitialize is failed.
 *	@see		NX_CLKPWR_GetPhysicalAddress,		NX_CLKPWR_GetSizeOfRegisterSet,
 *				NX_CLKPWR_SetBaseAddress,			NX_CLKPWR_GetBaseAddress,
 *				NX_CLKPWR_OpenModule,
 *				NX_CLKPWR_CheckBusy,				NX_CLKPWR_CanPowerDown
 */
CBOOL	NX_CLKPWR_CloseModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicates that Module is Busy. \n
 *				\b CFALSE	indicates that Module is NOT Busy.
 *	@see		NX_CLKPWR_GetPhysicalAddress,		NX_CLKPWR_GetSizeOfRegisterSet,
 *				NX_CLKPWR_SetBaseAddress,			NX_CLKPWR_GetBaseAddress,
 *				NX_CLKPWR_OpenModule,				NX_CLKPWR_CloseModule,
 *													NX_CLKPWR_CanPowerDown
 */
CBOOL	NX_CLKPWR_CheckBusy( void )
{
	if( NX_CLKPWR_IsPLLStable() )
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicates that Ready to enter power-down stage. \n
 *				\b CFALSE	indicates that This module can't enter to power-down stage.
 *	@see		NX_CLKPWR_GetPhysicalAddress,		NX_CLKPWR_GetSizeOfRegisterSet,
 *				NX_CLKPWR_SetBaseAddress,			NX_CLKPWR_GetBaseAddress,
 *				NX_CLKPWR_OpenModule,				NX_CLKPWR_CloseModule,
 *				NX_CLKPWR_CheckBusy
 */
CBOOL	NX_CLKPWR_CanPowerDown( void )
{
	if( NX_CLKPWR_IsPLLStable() )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see												NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
S32		NX_CLKPWR_GetInterruptNumber( void )
{
	return	INTNUM_OF_CLKPWR_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number(0 ~ 10). \n
 *						0:ALIVEGPIO0, 1:ALIVEGPIO1, 2:ALIVEGPIO2, 3:ALIVEGPIO3, 4:ALIVEGPIO4, 5:ALIVEGPIO5, \n
 *						6:ALIVEGPIO6, 7:ALIVEGPIO7, 8:VDDTOGGLE, 9:RTC, 10:BATF
 *	@param[in]	Enable	\b CTRUE	indicates that Interrupt Enable. \n
 *						\b CFALSE	indicates that Interrupt Disable.
 *	@return		None.
 *	@see		NX_CLKPWR_GetInterruptNumber,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
void	NX_CLKPWR_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	U32 ReadValue;

	NX_ASSERT( 11 > IntNum );
	NX_ASSERT( (0==Enable) | (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	if( 8 >= IntNum )
	{
		ReadValue = __g_pRegister->GPIOINTENB;

		ReadValue &= ~( 0x01 << IntNum );
		ReadValue |= (U32)Enable << IntNum;

//		__g_pRegister->GPIOINTENB = ReadValue;
		WriteIODW(&__g_pRegister->GPIOINTENB, ReadValue);
	}
	else	// RTC, BATF
	{
		ReadValue = __g_pRegister->INTENABLE;

		ReadValue &= ~( 0x01 << (IntNum-9) );
		ReadValue |= (U32)Enable << (IntNum-9);

//		__g_pRegister->INTENABLE = ReadValue;
		WriteIODW(&__g_pRegister->INTENABLE, ReadValue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number(0 ~ 10). \n
 *						0:ALIVEGPIO0, 1:ALIVEGPIO1, 2:ALIVEGPIO2, 3:ALIVEGPIO3, 4:ALIVEGPIO4, 5:ALIVEGPIO5, \n
 *						6:ALIVEGPIO6, 7:ALIVEGPIO7, 8:VDDTOGGLE, 9:RTC, 10:BATF
 *	@return		\b CTRUE	indicates that Interrupt is enabled. \n
 *				\b CFALSE	indicates that Interrupt is disabled.
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *														NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
CBOOL	NX_CLKPWR_GetInterruptEnable( S32 IntNum )
{
	NX_ASSERT( 11 > IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	if( 8 >= IntNum )
	{
		return (CBOOL)( (__g_pRegister->GPIOINTENB >> IntNum) & 0x01 );
	}
	else
	{
		return (CBOOL)( (__g_pRegister->INTENABLE >> (IntNum-9)) & 0x01 );
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : ALIVE GPIO0 interrupt enable or disable. \n
 *							...
 *							- EnableFlag[7] : ALIVE GPIO7 interrupt enable or disable. \n
 *							- EnableFlag[8] : VDD power toggle interrupt enable or disable. \n
 *							- EnableFlag[9] : RTC interrupt enable or disable. \n
 *							- EnableFlag[10]: Battery Fault interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
void	NX_CLKPWR_SetInterruptEnable32( U32 EnableFlag )
{
	const U32 GPIOENB_MASK	= 0x1FF;
	const U32 ENB_MASK		= 0x03;	

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->GPIOINTENB	= EnableFlag & GPIOENB_MASK;
//	__g_pRegister->INTENABLE	= (EnableFlag >> 9) & ENB_MASK;
	WriteIODW(&__g_pRegister->GPIOINTENB, EnableFlag & GPIOENB_MASK);
	WriteIODW(&__g_pRegister->INTENABLE, (EnableFlag >> 9) & ENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : ALIVE GPIO0 interrupt's setting value. \n
 *				...													
 *				- Return Value[7] : ALIVE GPIO7 interrupt's setting value. \n
 *				- Return Value[8] : VDD power toggle interrupt's setting value. \n
 *				- Return Value[9] : RTC interrupt's setting value. \n
 *				- Return Value[10]: Battery Fault interrupt's setting value. \n
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *														NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
U32		NX_CLKPWR_GetInterruptEnable32( void )
{
	const U32 GPIOENB_MASK	= 0x1FF;
	const U32 ENB_MASK		= 0x03;	
	
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->GPIOINTENB & GPIOENB_MASK) | ((__g_pRegister->INTENABLE & ENB_MASK)<<9));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number(0 ~ 10). \n
 *						0:ALIVEGPIO0, 1:ALIVEGPIO1, 2:ALIVEGPIO2, 3:ALIVEGPIO3, 4:ALIVEGPIO4, 5:ALIVEGPIO5, \n
 *						6:ALIVEGPIO6, 7:ALIVEGPIO7, 8:VDDTOGGLE, 9:RTC, 10:BATF
 *	@return		\b CTRUE	indicates that Pending is seted. \n
 *				\b CFALSE	indicates that Pending is Not Seted.
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,		
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
CBOOL	NX_CLKPWR_GetInterruptPending( S32 IntNum )
{
	NX_ASSERT( 11 > IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	if( 8 >= IntNum )
	{
		return ( (__g_pRegister->GPIOINTPEND >> IntNum) & 0x01 );
	}
	else
	{
		return ( (__g_pRegister->INTPEND >> (IntNum-9)) & 0x01 );
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : ALIVE GPIO0 pending state. \n
 *				...								
 *				- Return Value[7] : ALIVE GPIO7 pending state. \n
 *				- Return Value[8] : VDD power toggle pending state. \n
 *				- Return Value[9] : RTC pending state. \n
 *				- Return Value[10]: Battery Fault pending state. \n
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *														NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
U32		NX_CLKPWR_GetInterruptPending32( void )
{
	const U32 GPIOPEND_MASK	= 0x1FF;
	const U32 PEND_MASK		= 0x03;	
	
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->GPIOINTPEND & GPIOPEND_MASK) | ((__g_pRegister->INTPEND & PEND_MASK)<<9));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt Number(0 ~ 10). \n
 *						0:ALIVEGPIO0, 1:ALIVEGPIO1, 2:ALIVEGPIO2, 3:ALIVEGPIO3, 4:ALIVEGPIO4, 5:ALIVEGPIO5, \n
 *						6:ALIVEGPIO6, 7:ALIVEGPIO7, 8:VDDTOGGLE, 9:RTC, 10:BATF
 *	@return		None.
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
void	NX_CLKPWR_ClearInterruptPending( S32 IntNum )
{
	NX_ASSERT( 11 > IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	if( 8 >= IntNum )
	{
//		__g_pRegister->GPIOINTPEND = 1 << IntNum;
		WriteIODW(&__g_pRegister->GPIOINTPEND, 1 << IntNum);
	}
	else
	{
//		__g_pRegister->INTPEND = 1 << (IntNum-9);
		WriteIODW(&__g_pRegister->INTPEND, 1 << (IntNum-9));
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : ALIVE GPIO0 pending bit. \n
 *								...
 *								- PendingFlag[7] : ALIVE GPIO7 pending bit. \n
 *								- PendingFlag[8] : VDD power toggle pending bit. \n
 *								- PendingFlag[9] : RTC pending bit. \n
 *								- PendingFlag[10]: Battery Fault pending bit. \n
 *	@return		None.
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *														NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
void	NX_CLKPWR_ClearInterruptPending32( U32 PendingFlag )
{
	const U32 GPIOPEND_MASK = 0x1FF;
	const U32 PEND_MASK = 0x03;
	
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->GPIOINTPEND	= (PendingFlag & GPIOPEND_MASK);
//	__g_pRegister->INTPEND		= (PendingFlag >> 9 ) & PEND_MASK;
	WriteIODW(&__g_pRegister->GPIOINTPEND, (PendingFlag & GPIOPEND_MASK));
	WriteIODW(&__g_pRegister->INTPEND, (PendingFlag >> 9 ) & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicates that Set to all interrupt enable. \n
 *						\b CFALSE	indicates that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
void	NX_CLKPWR_SetInterruptEnableAll( CBOOL Enable )
{
	NX_ASSERT( CNULL != __g_pRegister );

	if( Enable )
	{
//		__g_pRegister->GPIOINTENB = 0x1FF;
//		__g_pRegister->INTENABLE = 0x03;
		WriteIODW(&__g_pRegister->GPIOINTENB, 0x1FF);
		WriteIODW(&__g_pRegister->INTENABLE, 0x03);
	}
	else
	{
//		__g_pRegister->GPIOINTENB = 0x00;
//		__g_pRegister->INTENABLE = 0x00;
		WriteIODW(&__g_pRegister->GPIOINTENB, 0x00);
		WriteIODW(&__g_pRegister->INTENABLE, 0x00);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicates that All interrupt is disabled.
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *														NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
CBOOL	NX_CLKPWR_GetInterruptEnableAll( void )
{
	const U32 GPIOINTENB_MASK	= 0x1FF;
	const U32 INTENB_MASK		= 0x03;

	NX_ASSERT( CNULL != __g_pRegister );

	if( (__g_pRegister->GPIOINTENB & GPIOINTENB_MASK) || (__g_pRegister->INTENABLE & INTENB_MASK) )
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
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		
 *				NX_CLKPWR_ClearInterruptPendingAll,		NX_CLKPWR_GetInterruptPendingNumber
 */
CBOOL	NX_CLKPWR_GetInterruptPendingAll( void )
{
	const U32 GPIOINTPEND_MASK	= 0x1FF;
	const U32 INTPEND_MASK		= 0x03;

	NX_ASSERT( CNULL != __g_pRegister );

	if( (__g_pRegister->GPIOINTPEND & GPIOINTPEND_MASK) || (__g_pRegister->INTPEND & INTPEND_MASK) )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *														NX_CLKPWR_GetInterruptPendingNumber
 */
void	NX_CLKPWR_ClearInterruptPendingAll( void )
{
	const U32 GPIOINTPEND_MASK	= 0x1FF;
	const U32 INTPEND_MASK		= 0x03;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->GPIOINTPEND = GPIOINTPEND_MASK;
//	__g_pRegister->INTPEND = INTPEND_MASK;
	WriteIODW(&__g_pRegister->GPIOINTPEND, GPIOINTPEND_MASK);
	WriteIODW(&__g_pRegister->INTPEND, INTPEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_CLKPWR_GetInterruptNumber,			NX_CLKPWR_SetInterruptEnable,
 *				NX_CLKPWR_GetInterruptEnable,			NX_CLKPWR_SetInterruptEnable32,
 *				NX_CLKPWR_GetInterruptEnable32,			NX_CLKPWR_GetInterruptPending,
 *				NX_CLKPWR_GetInterruptPending32,		NX_CLKPWR_ClearInterruptPending,
 *				NX_CLKPWR_ClearInterruptPending32,		NX_CLKPWR_SetInterruptEnableAll,
 *				NX_CLKPWR_GetInterruptEnableAll,		NX_CLKPWR_GetInterruptPendingAll,
 *				NX_CLKPWR_ClearInterruptPendingAll	
 */
S32		NX_CLKPWR_GetInterruptPendingNumber( void )	// -1 if None
{
	register struct NX_CLKPWR_RegisterSet	*pRegister;
	register U32 Pend;
	U32 dwIntNum;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	Pend = (pRegister->GPIOINTPEND & pRegister->GPIOINTENB);

	Pend |= ((pRegister->INTPEND & pRegister->INTENABLE)<<9 );

	for( dwIntNum = 0; dwIntNum < 11; dwIntNum++ )
	{
		if( Pend & ( 1 << dwIntNum) )
		{
			return dwIntNum;
		}
	}

	return -1;
}

//------------------------------------------------------------------------------
//	Clock Management
//--------------------------------------------------------------------------
/**
 *	@brief		Set PLL by P, M, S.
 *	@param[in]	pllnumber	PLL to be changed ( 0 : PLL0, 1 : PLL1 )
 *	@param[in]	PDIV		Input frequency divider : 1 ~ 63
 *	@param[in]	MDIV		VCO frequency divider	: PLL0(56~1023), PLL1(13~255)
 *	@param[in]	SDIV		Output frequency scaler : PLL0(0~5), PLL1(0~4)
 *	@return		None.
 *	@remark		PLL can be calculated by following fomula. \n
 *				(m * Fin)/(p * 2^s ).
 *				- PLL0 : 16 ~ 800 Mhz \n
 *				- PLL1 : 16 ~ 300 Mhz \n
 *				- IMPORTANT : \n
 *					You should set a PMS value carefully. \n
 *					Please refer PMS table we are recommend or Contact Nexell or
 *						Samsung(manufacture of PLL core) if you wish to change it.
 *
 *	@remark		PLL is not changed until NX_CLKPWR_DoPLLChange() function is called.
 *	@see									NX_CLKPWR_SetPLLPowerOn,
 *				NX_CLKPWR_DoPLLChange,		NX_CLKPWR_IsPLLStable,
 *				NX_CLKPWR_SetClockCPU,		NX_CLKPWR_SetClockMCLK
 */
void	NX_CLKPWR_SetPLLPMS ( U32 pllnumber, U32 PDIV, U32 MDIV, U32 SDIV )
{
	const U32 PLL_PDIV_BIT_POS =	18;
	const U32 PLL_MDIV_BIT_POS =	8;
	const U32 PLL_SDIV_BIT_POS =	0;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2	> pllnumber );
	NX_ASSERT( 1	<= PDIV && PDIV <= 63 );
//	NX_ASSERT( (0 != pllnumber) || (56	<= MDIV && MDIV <= 1023) );
	NX_ASSERT( (0 != pllnumber) || (24	<= MDIV && MDIV <= 1023) );
	NX_ASSERT( (0 != pllnumber) || (5	>= SDIV) );
//	NX_ASSERT( (1 != pllnumber) || (13	<= MDIV && MDIV <= 255) );
	NX_ASSERT( (1 != pllnumber) || (13	<= MDIV && MDIV <= 1023) );
	NX_ASSERT( (1 != pllnumber) || (4	>= SDIV) );

//	__g_pRegister->PLLSETREG[pllnumber] = (PDIV<<PLL_PDIV_BIT_POS) | (MDIV<<PLL_MDIV_BIT_POS) | (SDIV<<PLL_SDIV_BIT_POS);
	WriteIODW(&__g_pRegister->PLLSETREG[pllnumber], (PDIV<<PLL_PDIV_BIT_POS) | (MDIV<<PLL_MDIV_BIT_POS) | (SDIV<<PLL_SDIV_BIT_POS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set PLL1 power On/Off.
 *	@param[in]	On	\b CTRUE	indicates that Normal mode . \n
 *					\b CFALSE	indicates that Power Down.
 *	@return		None.
 *	@remark		PLL1 is only available.
 *	@see		NX_CLKPWR_SetPLLPMS,
 *				NX_CLKPWR_DoPLLChange,		NX_CLKPWR_IsPLLStable,
 *				NX_CLKPWR_SetClockCPU,		NX_CLKPWR_SetClockMCLK

 */
void	NX_CLKPWR_SetPLLPowerOn ( CBOOL On )
{
	register U32 PLLPWDN1 = (U32)(1UL << 30);
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( On )	__g_pRegister->CLKMODEREG0 &=	~PLLPWDN1;
//	else		__g_pRegister->CLKMODEREG0 |=	PLLPWDN1;

	SetValue = __g_pRegister->CLKMODEREG0;
	if( On )	SetValue &=	~PLLPWDN1;
	else		SetValue |=	PLLPWDN1;
	WriteIODW(&__g_pRegister->CLKMODEREG0, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Change PLL with P, M, S value on PLLSETREG.
 *	@return		None.
 *	@remark		If you call this function, you must check by NX_CLKPWR_IsPLLStable(),
 *				because PLL change need stable time.\n
 *				Therefore the sequence for changing PLL is as follows.
 *	@code
 *		NX_CLKPWR_SetPLLPMS( PLLn, P, M, S );	// Change P, M, S values
 *		NX_CLKPWR_DoPLLChange();				// Change PLL
 *		while( !NX_CLKPWR_IsPLLStable() );		// wait until PLL is stable.
 *	@endcode
 *	@see		NX_CLKPWR_SetPLLPMS,		NX_CLKPWR_SetPLLPowerOn,
 *											NX_CLKPWR_IsPLLStable,
 *				NX_CLKPWR_SetClockCPU,		NX_CLKPWR_SetClockMCLK
 */
void	NX_CLKPWR_DoPLLChange ( void )
{
	const U32 CHGPLL = (1<<15);
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PWRMODE |= CHGPLL;
	SetValue = __g_pRegister->PWRMODE | CHGPLL;
	WriteIODW(&__g_pRegister->PWRMODE, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check whether PLL are stable or not.
 *	@return		If PLL is stable, return \b CTRUE. Otherwise, return \b CFALSE.
 *	@see		NX_CLKPWR_SetPLLPMS,		NX_CLKPWR_SetPLLPowerOn,
 *				NX_CLKPWR_DoPLLChange,
 *				NX_CLKPWR_SetClockCPU,		NX_CLKPWR_SetClockMCLK
 */
CBOOL	NX_CLKPWR_IsPLLStable ( void )
{
	const U32 CHGPLL = (1<<15);
	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->PWRMODE & CHGPLL ) ? CFALSE : CTRUE );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock for CPU and CPU Bus.
 *	@param[in]	ClkSrc		Source clock for CPU ( 0 : PLL0, 1 : PLL1 ).
 *	@param[in]	CoreDivider	Divider for CPU core clock, 1 ~ 16.
 *	@param[in]	BusDivider	Divider for CPU Bus clock, 2 ~ 16.
 *	@return		None.
 *	@remark		\n
 *				- CPU core clock = Source clock / CoreDivider, CoreDivider = 1 ~ 16
 *				- CPU bus clock = CPU core clock / BusDivider, BusDivider = 2 ~ 16
 *	@see		NX_CLKPWR_SetPLLPMS,		NX_CLKPWR_SetPLLPowerOn,
 *				NX_CLKPWR_DoPLLChange,		NX_CLKPWR_IsPLLStable,
 *											NX_CLKPWR_SetClockMCLK
 */
void	NX_CLKPWR_SetClockCPU	( U32 ClkSrc, U32 CoreDivider, U32 BusDivider )
{
	const U32	CLKDIV1CPU0_POS	= 0;
	const U32	CLKSELCPU0_POS	= 4;
	const U32	CLKDIV2CPU0_POS	= 8;
	register U32 temp;

	NX_ASSERT( 2 > ClkSrc );
	NX_ASSERT( 1 <= CoreDivider && CoreDivider <= 16 );
	NX_ASSERT( 2 <= BusDivider && BusDivider <= 16 );
	NX_ASSERT( CNULL != __g_pRegister );

	temp	= (U32)( ((CoreDivider-1) << CLKDIV1CPU0_POS)
						| ((U32)ClkSrc << CLKSELCPU0_POS)
						| ((BusDivider-1)	<< CLKDIV2CPU0_POS) );

//	__g_pRegister->CLKMODEREG0 = temp;
	WriteIODW(&__g_pRegister->CLKMODEREG0, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock for MCLK, BCLK and PCKL.
 *	@param[in]	ClkSrc			Source clock for MCLK ( 0 : PLL0, 1 : PLL1, 3:FCLK )
 *	@param[in]	MCLKDivider		Divide value for MCLK, 1 ~ 16.
 *	@param[in]	BCLKDivider		Divide value for BCLK, 1 or 2.
 *	@param[in]	PCLKDivider		Divide value for PCLK, 2.
 *	@return		None.
 *	@remark
 *				MCLK is memory bus clock. It must be lesser than 400MHz. \n
 *				- MCLK = PLL0 or PLL1 / MCLKDivider, MCLKDivider = 1 ~ 16.
 *
 *				BCLK is system bus clock. BCLK must set same clock of MCLK or half clock. \n
 *				- BCLK = MCLK / BCLKDivider, BCLKDivider = 1 or 2.
 *
 *				PCLK is registe access clock.	PCLKDivide commonly set 2. \n
 *				- PCLK = BCLK / PCLKDivider, PCLKDivider = 2.
 *
 *	@see		NX_CLKPWR_SetPLLPMS,		NX_CLKPWR_SetPLLPowerOn,
 *				NX_CLKPWR_DoPLLChange,		NX_CLKPWR_IsPLLStable,
 *				NX_CLKPWR_SetClockCPU
 */
void	NX_CLKPWR_SetClockMCLK( U32 ClkSrc, U32 MCLKDivider, U32 BCLKDivider, U32 PCLKDivider )
{
	const U32	CLKDIVMCLK_BITPOS	= 0;
	const U32	CLKSELMCLK_BITPOS	= 4;
	const U32	CLKDIVBCLK_BITPOS	= 8;
	const U32	CLKDIVPCLK_BITPOS	= 12;
	register U32 temp=0;

	NX_ASSERT( (3>=ClkSrc) && (2!=ClkSrc) );
	NX_ASSERT( 1 <= MCLKDivider && MCLKDivider <= 16 );
	NX_ASSERT( (1==BCLKDivider) || (2==BCLKDivider) );
	NX_ASSERT( 2==PCLKDivider );

	temp = (U32)( (ClkSrc << CLKSELMCLK_BITPOS)
						| ((MCLKDivider-1) << CLKDIVMCLK_BITPOS)
						| ((BCLKDivider-1) << CLKDIVBCLK_BITPOS)
						| ((PCLKDivider-1) << CLKDIVPCLK_BITPOS)
						);

//	__g_pRegister->CLKMODEREG1 = temp;
	WriteIODW(&__g_pRegister->CLKMODEREG1, temp);
}

void NX_CLKPWR_SetCPUBUSSyncMode(CBOOL Enable)
{
	const U32 SYNCMODE = (1<<28);
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if(Enable)
//		__g_pRegister->PWRMODE |= SYNCMODE;
//	else
//		__g_pRegister->PWRMODE &= ~SYNCMODE;

	SetValue = __g_pRegister->PWRMODE;
	if(Enable)
		SetValue |= SYNCMODE;
	else
		SetValue &= ~SYNCMODE;
	WriteIODW(&__g_pRegister->PWRMODE, SetValue);
}
//--------------------------------------------------------------------------
// Wakeup Management
//--------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable RTC Wakeup.
 *	@param[in]	Enable	Set to \b CTRUE to enable RTC wakeup. Otherwise, set to \b CFALSE.
 *	@return		None.
 *	@remark		Wakeup control for RTC.
 *	@see															NX_CLKPWR_GetRTCWakeUpEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeupEnable,					NX_CLKPWR_GetALIVEGPIOWakeupEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable
 */
void	NX_CLKPWR_SetRTCWakeUpEnable ( CBOOL Enable )
{
	const U32 RTCWKENB_BITPOS	= 1;
	const U32 RTCWKENB_MASK		= 1 << RTCWKENB_BITPOS;

	register U32 ReadValue;

	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->PWRCONT;

	ReadValue &= ~RTCWKENB_MASK;
	ReadValue |= (U32)Enable << RTCWKENB_BITPOS;

//	__g_pRegister->PWRCONT = ReadValue;
	WriteIODW(&__g_pRegister->PWRCONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a status of RTC Wakeup Enable.
 *	@return	If RTC Wakeup is enabled, return \b CTRUE. Otherwise, return \b CFALSE.
 *	@remark		Wakeup control for RTC.
 *	@see		NX_CLKPWR_SetRTCWakeUpEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeupEnable,					NX_CLKPWR_GetALIVEGPIOWakeupEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable
 */
CBOOL	NX_CLKPWR_GetRTCWakeUpEnable ( void )
{
	const U32 RTCWKENB_BITPOS = 1;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->PWRCONT >> RTCWKENB_BITPOS ) & 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set wakeup source of ALIVEGPIO
 *	@param[in]	dwBitNumber		Select wakeup source (0 ~ 8). AliveGPIO(0~7), VDD Power Toggle Pin(8)
 *	@param[in]	bEnable			\b CTRUE indicates that ALIVEGPIO is use wakeup source. \n
 *								\b CFALSE indicates that ALIVEGPIO is NOT use wakeup source. \n
 *	@return		None.
 *	@remark		ALIVEGPIO only chose one function(reset or wakeup source). \n
 *	@see		NX_CLKPWR_SetRTCWakeUpEnable,						NX_CLKPWR_GetRTCWakeUpEnable,
 *																	NX_CLKPWR_GetALIVEGPIOWakeupEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable
 */
void	NX_CLKPWR_SetALIVEGPIOWakeupEnable( U32 dwBitNumber, CBOOL bEnable )
{
	register U32 ReadValue;

	NX_ASSERT( 8 >= dwBitNumber );
	NX_ASSERT( (0==bEnable) || (1==bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->GPIOWAKEUPENB;

	ReadValue &= ~(1 << dwBitNumber);
	ReadValue |= ((U32)bEnable << dwBitNumber);

//	__g_pRegister->GPIOWAKEUPENB = ReadValue;
	WriteIODW(&__g_pRegister->GPIOWAKEUPENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of wakeup source
 *	@param[in]	dwBitNumber		Select wakeup source (0 ~ 8). AliveGPIO(0~7), VDD Power Toggle Pin(8)
 *	@return		bEnable			\b CTRUE indicates that ALIVEGPIO is use wakeup source. \n
 *								\b CFALSE indicates that ALIVEGPIO is NOT use wakeup source. \n
 *	@see		NX_CLKPWR_SetRTCWakeUpEnable,						NX_CLKPWR_GetRTCWakeUpEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeupEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable
 */
CBOOL	NX_CLKPWR_GetALIVEGPIOWakeupEnable( U32 dwBitNumber )
{
	NX_ASSERT( 8 >= dwBitNumber );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->GPIOWAKEUPENB >> dwBitNumber) & 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set enable or disable of ALIVEGPIO's rising edge detection.
 *	@param[in]	dwBitNumber		Select wakeup source (0 ~ 8). AliveGPIO(0~7), VDD Power Toggle Pin(8)
 *	@param[in]	bEnable			\b CTRUE indicates that rising edge detection enable.\n
 *								\b CFALSE indicates that rising edge detection disable.\n
 *	@return		None.
 *	@see		NX_CLKPWR_SetRTCWakeUpEnable,						NX_CLKPWR_GetRTCWakeUpEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeupEnable,					NX_CLKPWR_GetALIVEGPIOWakeupEnable,
 *																	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable
 */
void	NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable( U32 dwBitNumber, CBOOL bEnable )
{
	register U32 ReadValue;

	NX_ASSERT( 8 >= dwBitNumber );
	NX_ASSERT( (0==bEnable) || (1==bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->GPIOWAKEUPRISEENB;

	ReadValue &= ~(1 << dwBitNumber);
	ReadValue |= ((U32)bEnable << dwBitNumber);

//	__g_pRegister->GPIOWAKEUPRISEENB = ReadValue;
	WriteIODW(&__g_pRegister->GPIOWAKEUPRISEENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of rising edge detection is enabled or disabled.
 *	@param[in]	dwBitNumber		Select wakeup source (0 ~ 8). AliveGPIO(0~7), VDD Power Toggle Pin(8)
 *	@return		bEnable			\b CTRUE indicates that rising edge detection is enabled. \n
 *								\b CFALSE indicates that rising edge detection is disabled. \n
 *	@see		NX_CLKPWR_SetRTCWakeUpEnable,						NX_CLKPWR_GetRTCWakeUpEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeupEnable,					NX_CLKPWR_GetALIVEGPIOWakeupEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable
 */
CBOOL	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable( U32 dwBitNumber )
{
	NX_ASSERT( 8 >= dwBitNumber );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->GPIOWAKEUPRISEENB >> dwBitNumber) & 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set enable or disable of ALIVEGPIO's faling edge detection.
 *	@param[in]	dwBitNumber		Select wakeup source (0 ~ 8). AliveGPIO(0~7), VDD Power Toggle Pin(8)
 *	@param[in]	bEnable			\b CTRUE indicates that falling edge detection enable.\n
 *								\b CFALSE indicates that falling edge detection disable.\n
 *	@return		None.
 *	@see		NX_CLKPWR_SetRTCWakeUpEnable,						NX_CLKPWR_GetRTCWakeUpEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeupEnable,					NX_CLKPWR_GetALIVEGPIOWakeupEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable,
 *																	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable
 */
void	NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable( U32 dwBitNumber, CBOOL bEnable )
{
	register U32 ReadValue;

	NX_ASSERT( 8 >= dwBitNumber );
	NX_ASSERT( (0==bEnable) || (1==bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->GPIOWAKEUPFALLENB;

	ReadValue &= ~(1 << dwBitNumber);
	ReadValue |= ((U32)bEnable << dwBitNumber);

//	__g_pRegister->GPIOWAKEUPFALLENB = ReadValue;
	WriteIODW(&__g_pRegister->GPIOWAKEUPFALLENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of falling edge detection is enabled or disabled.
 *	@param[in]	dwBitNumber		Select wakeup source (0 ~ 8). AliveGPIO(0~7), VDD Power Toggle Pin(8)
 *	@return		\b CTRUE	indicates that falling edge detection is enabled. \n
 *				\b CFALSE	indicates that falling edge detection is disabled. \n
 *	@see		NX_CLKPWR_SetRTCWakeUpEnable,						NX_CLKPWR_GetRTCWakeUpEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeupEnable,					NX_CLKPWR_GetALIVEGPIOWakeupEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable,	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable,
 *				NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable
 */
CBOOL	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable( U32 dwBitNumber )
{
	NX_ASSERT( 8 >= dwBitNumber );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->GPIOWAKEUPFALLENB >> dwBitNumber) & 0x01);
}

//--------------------------------------------------------------------------
// Reset Management
//--------------------------------------------------------------------------
/** @brief		Set enable or disable of software reset.
 *	@param[in]	bEnable		\b CTRUE indicates that software reset enable.\n
 *							\b CFALSEindicates that software reset disable.
 *	@return		None.
 *	@see													NX_CLKPWR_GetSoftwareResetEnable,
 *				NX_CLKPWR_DoSoftwareReset,					NX_CLKPWR_SetALIVEGPIOResetEnable,
 *				NX_CLKPWR_GetALIVEGPIOResetEnable
 */
void	NX_CLKPWR_SetSoftwareResetEnable( CBOOL bEnable )
{
	const U32 SWRSTENB_BITPOS	= 3;
	const U32 SWRSTENB_MASK		= 1 << SWRSTENB_BITPOS;

	register U32 ReadValue;

	NX_ASSERT( (0==bEnable) || (1==bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->PWRCONT;

	ReadValue &= ~SWRSTENB_MASK;
	ReadValue |= (U32)bEnable << SWRSTENB_BITPOS;

//	__g_pRegister->PWRCONT = ReadValue;
	WriteIODW(&__g_pRegister->PWRCONT, ReadValue);
}

//--------------------------------------------------------------------------
/** @brief		Get setting value of software reset is enabled or disabled.
 *	@return		\b CTRUE	indicates that software reset is enabled.\n
 *				\b CFALSE	indicates that software reset is disabled.
 *	@see		NX_CLKPWR_SetSoftwareResetEnable,
 *				NX_CLKPWR_DoSoftwareReset,					NX_CLKPWR_SetALIVEGPIOResetEnable,
 *				NX_CLKPWR_GetALIVEGPIOResetEnable
 */
CBOOL	NX_CLKPWR_GetSoftwareResetEnable( void )
{
	const U32 SWRSTENB_BITPOS	= 3;
	const U32 SWRSTENB_MASK		= 1 << SWRSTENB_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PWRCONT & SWRSTENB_MASK) ? CTRUE : CFALSE ;
}

//--------------------------------------------------------------------------
/** @brief		Generate a software reset.
 *	@return		None.
 *	@see		NX_CLKPWR_SetSoftwareResetEnable,			NX_CLKPWR_GetSoftwareResetEnable,
 *															NX_CLKPWR_SetALIVEGPIOResetEnable,
 *				NX_CLKPWR_GetALIVEGPIOResetEnable
 */
void	NX_CLKPWR_DoSoftwareReset( void )
{
	const U32 SWREST_BITPOS	= 12;
	const U32 SWREST_MASK	= 1 << SWREST_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PWRMODE = SWREST_MASK;
	WriteIODW(&__g_pRegister->PWRMODE, SWREST_MASK);
}

//------------------------------------------------------------------------------
/** @brief		Set enable or disable of ALIVEGPIO's reset source.
 *	@param[in]	dwBitNumber		Select ALIVEGPIO (0 ~7).
 *	@param[in]	bEnable			\b CTRUE indicates that ALIVEGPIO is use to reset source.\n
 *								\b CFALSE indicates that ALIVEGPIO is NOT use to reset source.\n
 *	@return		None.
 *	@remark		ALIVEGPIO only chose one function(reset or wakeup source). \n
 *	@see		NX_CLKPWR_SetSoftwareResetEnable,			NX_CLKPWR_GetSoftwareResetEnable,
 *				NX_CLKPWR_DoSoftwareReset,
 *				NX_CLKPWR_GetALIVEGPIOResetEnable
 */
void	NX_CLKPWR_SetALIVEGPIOResetEnable( U32 dwBitNumber, CBOOL bEnable )
{
	register U32 ReadValue;

	NX_ASSERT( 8 > dwBitNumber );
	NX_ASSERT( (0==bEnable) || (1==bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->GPIORSTENB;

	ReadValue &= ~(1 << dwBitNumber);
	ReadValue |= ((U32)bEnable << dwBitNumber);

//	__g_pRegister->GPIORSTENB = ReadValue;
	WriteIODW(&__g_pRegister->GPIORSTENB, ReadValue);
}

//------------------------------------------------------------------------------
/** @brief		Get setting value that ALIVEGPIO's reset source is enabled or disabled.
 *	@param[in]	dwBitNumber		Select ALIVEGPIO (0 ~7).
 *	@return		\b CTRUE indicates that ALIVEGPIO's reset source is enabled.\n
 *				\b CFALSEindicates that ALIVEGPIO's reset source is disabled.
 *	@see		NX_CLKPWR_SetSoftwareResetEnable,			NX_CLKPWR_GetSoftwareResetEnable,
 *				NX_CLKPWR_DoSoftwareReset,					NX_CLKPWR_SetALIVEGPIOResetEnable
 */
CBOOL	NX_CLKPWR_GetALIVEGPIOResetEnable( U32 dwBitNumber )
{
	NX_ASSERT( 8 > dwBitNumber );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->GPIORSTENB >> dwBitNumber) & 0x01);
}

//------------------------------------------------------------------------------
// Check Last Reset Status
//------------------------------------------------------------------------------
/** @brief		Get last reset status
 *	@return		Reset status
 *	@remarks	Reset priority is \n
 *				POR > GPIO > Watchdog > Software > DSleep > Sleep.
 *	@code
 *				NX_CLKPWR_RESETSTATUS ResetStatus;
 *
 *				ResetStatus = NX_CLKPWR_GetResetStatus();
 *
 *				if( NX_CLKPWR_RESETSTATUS_POWERON = ResetStatus )		// Last Reset status is "Power on Reset".
 *				{
 *					...
 *				}
 *				else if( NX_CLKPWR_RESETSTATUS_GPIO = ResetStatus )	// Last Reset status is "GPIO Reset".
 *				{
 *					...
 *				}
 *	@endcode
 */
NX_CLKPWR_RESETSTATUS NX_CLKPWR_GetResetStatus( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_CLKPWR_RESETSTATUS)(__g_pRegister->RESETSTATUS & 0x1F);
}

//------------------------------------------------------------------------------
// Power Management
//------------------------------------------------------------------------------
/** @brief		Change operation mode ( Normal => Stop )
 *	@return		None.
 *	@see										NX_CLKPWR_GoIdleMode,
 *				NX_CLKPWR_GetLastPowerMode
 */
void	NX_CLKPWR_GoStopMode( void )
{
	const U32 STOP_MASK = 1<<1;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PWRMODE = STOP_MASK;
	WriteIODW(&__g_pRegister->PWRMODE, STOP_MASK);
}

//------------------------------------------------------------------------------
/** @brief		Change operation mode ( Normal => Idle )
 *	@return		None.
 *	@see		NX_CLKPWR_GoStopMode,
 *				NX_CLKPWR_GetLastPowerMode
 */
void	NX_CLKPWR_GoIdleMode( void )
{
	const U32 IDLE_MASK = 1<<0;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PWRMODE = IDLE_MASK;
	WriteIODW(&__g_pRegister->PWRMODE, IDLE_MASK);
}

//--------------------------------------------------------------------------
/** @brief		Get Last Power Mode.
 *	@return		Last power mode.
 *	@code
 *				NX_CLKPWR_LASTPOWERMODE PowerMode = NX_CLKPWR_GetLastPowerMode();
 *
 *				if( NX_CLKPWR_POWERMODE_STOP == PowerMode )			// Last power mode is Stop.
 *				{
 *					...
 *				}
 *				else if( NX_CLKPWR_POWERMODE_IDLE == PowerMode )	// Last power mode is Idle.
 *				{
 *					...
 *				}
 *				...
 *	@endcode
 *	@remarks	Only one power mode is return.
 *				This bit(LASTPWRSTOP, LASTPWRIDLE) is cleared. when reset ocure.

 *	@see		NX_CLKPWR_GoStopMode,			NX_CLKPWR_GoIdleMode
 */
NX_CLKPWR_POWERMODE	NX_CLKPWR_GetLastPowerMode ( void )
{
	const U32 LASTPWRIDLE_BITPOS = 4;
	const U32 LASTPWR_MASK = 0x03 << LASTPWRIDLE_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_CLKPWR_POWERMODE)(__g_pRegister->PWRMODE & LASTPWR_MASK );
}

//------------------------------------------------------------------------------
// Scratch
//------------------------------------------------------------------------------
/** @brief		Set data value to scratch register.
 *	@param[in]	dwIndex		Index of scratch register(0~2).
 *	@param[in]	dwValue		save value.
 *	@return		none.
 *	@remarks	scratch regiser is cleared when coure power reset.
 *	@see		NX_CLKPWR_GetScratch
 */
void	NX_CLKPWR_SetScratchPad( U32 dwIndex, U32 dwValue )
{
	NX_ASSERT( 3 > dwIndex );
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->SCRATCH[dwIndex] = dwValue;
	WriteIODW(&__g_pRegister->SCRATCH[dwIndex], dwValue);
}

//------------------------------------------------------------------------------
/** @brief		Get saved value of scratch register
 *	@param[in]	dwIndex		Index of scratch register(0~2).
 *	@return		data value
 *	@remarks	scratch regiser is cleared when core power reset.
 *	@see		NX_CLKPWR_SetScratch
 */
U32		NX_CLKPWR_GetScratchPad( U32 dwIndex )
{
	NX_ASSERT( 3 > dwIndex );
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->SCRATCH[dwIndex];
}

//------------------------------------------------------------------------------
/** @brief		Get system configuration value.
 *	@return		None.
 */
U32		NX_CLKPWR_GetSystemResetConfiguration( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)__g_pRegister->SYSRSTCONFIG;
}

// PAD Strength Definition
/**
 * GPIO pads support two different drive ranges, {2, 4, 6, 8} mA and {4, 8, 12, 16} mA.
 * Define bit arrays to distinguish the two ranges.
 *
 *                    1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1
 * GPIOA  8 mA pins: 31 30 29 28  27 26 25 24  23 22 21 20  19 18 17 16  15 14 13 12  11 10  9  8   7  6  5  4   3  2  1  0
 * GPIOA 16 mA pins: 
 *                    0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0
 *
 *                    1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1
 * GPIOA  8 mA pins: 31 30 29 28  27 26 25 24  23 22 21 20  19 18 17 16  15 14 13 12  11 10  9  8   7  6  5  4   3  2  1  0
 * GPIOA 16 mA pins: 
 *                    0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0
 *
 *                    0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 1  1  1  1 . 1  1  1  1
 * GPIOC  8 mA pins:                                                                                7  6  5  4   3  2  1  0
 * GPIOC 16 mA pins: 31 30 29 28  27 26 25 24  23 22 21 20  19 18 17 16  15 14 13 12  11 10  9  8
 *                    1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 0  0  0  0 . 0  0  0  0
 *
 *                    0  0  0  0 . 0  0  0  1 . 1  1  1  1 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 0  0  0  0
 * GPIOD  8 mA pins:                       24  23 22 21 20
 * GPIOD 16 mA pins:                                        19 18 17 16  15 14 13 12  11 10  9  8   7  6  5  4   3  2  1  0
 *                    0  0  0  0 . 0  0  0  0 . 0  0  0  0 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1 . 1  1  1  1
 */

/* bit array            GPIOA,      GPIOB,      GPIOC,      GPIOD */
U32 max_8ma[4]  = {0xFFFFFFFF, 0xFFFFFFFF, 0x000000FF, 0x01F00000};
U32 max_16ma[4] = {0x00000000, 0x00000000, 0xFFFFFF00, 0x000FFFFF};


//------------------------------------------------------------------------------
// PAD Strength Management
//------------------------------------------------------------------------------
/**
 *	@brief		Set GPIO Pad's output drive strength(current)
 *	@param[in]	Group		Set gpio group ( 0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Set bit number ( GPIOA/B/C (0~31), GPIOD(0~24) )
 *	@param[in]	mA			Set gpio pad's output drive strength(current) ( 2mA, 4mA, 6mA, 8mA )
 *	@return		None.
 *	@remark		GPIOD group only can set bit 0 ~ 24.
 *	@see		NX_CLKPWR_GetGpioPadStrength,
 *				NX_CLKPWR_SetBusPadStrength, NX_CLKPWR_GetBusPadStrength
 *	@todo		check pad number
 */
void	NX_CLKPWR_SetGPIOPadStrength( U32 Group, U32 BitNumber, U32 mA )
{
	register U32 regvalue;
	U32 SetmA=0;
	U32 shift;
	U32 SelectReg;

	NX_ASSERT( 4 >= Group );
	NX_ASSERT( (0!=Group) || ( BitNumber <= 31 ) );
	NX_ASSERT( (1!=Group) || ( BitNumber <= 31 ) );
	NX_ASSERT( (2!=Group) || ( BitNumber <= 31 ) );
	NX_ASSERT( (3!=Group) || ( BitNumber <= 24 ) );
	NX_ASSERT( (2==mA) || (4==mA) || (6==mA) || (8==mA) || (12==mA) || (16==mA) );
	NX_ASSERT( CNULL != __g_pRegister );

	/* ensure bit not set in both ranges */
	if ( ((max_8ma[Group] >> BitNumber) & 1) && ((max_16ma[Group] >> BitNumber) & 1) ) {
		printk(KERN_ERR "%s.%d:%s  Both ranges selected: max_8ma[%d] >> %d and max_16ma[%d] >> %d are set\n",
				__FILE__, __LINE__, __func__, Group, BitNumber, Group, BitNumber);
		NX_ASSERT(CFALSE);
	}



	/* switch range based on bit array */
	if ((max_8ma[Group] >> BitNumber) & 1) {
		switch (mA) {
		case  2: SetmA = 0; break;
		case  4: SetmA = 1; break;
		case  6: SetmA = 2; break;
		case  8: SetmA = 3; break;
		default: NX_ASSERT( CFALSE );
		}
	} else if ((max_16ma[Group] >> BitNumber) & 1) {
		switch (mA) {
		case  4: SetmA = 0; break;
		case  8: SetmA = 1; break;
		case 12: SetmA = 2; break;
		case 16: SetmA = 3; break;
		default: NX_ASSERT( CFALSE );
		}
	} else
		NX_ASSERT( CFALSE );

	if (BitNumber >= 16) {
		shift = (BitNumber - 16) * 2;
	} else {
		shift = BitNumber * 2;
	}

	SelectReg = BitNumber/16;

	regvalue = __g_pRegister->PADSTRENGTHGPIO[Group][SelectReg];

	regvalue &= ~( 0x03 << shift );
	regvalue |= SetmA << shift;

//	__g_pRegister->PADSTRENGTHGPIO[Group][SelectReg] = regvalue;
	WriteIODW(&__g_pRegister->PADSTRENGTHGPIO[Group][SelectReg], regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get GPIO Pad's output drive strength(current)
 *	@param[in]	Group		Set gpio group ( 0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Set bit number ( GPIOA/B/C (0~31), GPIOD(0~24) )
 *	@return		GPIO Pad's output drive strength(current) ( 2mA, 4mA, 6mA, 8mA )
 *	@remark		GPIOD group only can set bit 0 ~ 24.
 *	@see		NX_CLKPWR_SetGPIOPadStrength,
 *				NX_CLKPWR_SetBusPadStrength, NX_CLKPWR_GetBusPadStrength
 */

U32		NX_CLKPWR_GetGPIOPadStrength( U32 Group, U32 BitNumber )
{
	U32 shift;
	U32 SelectReg;
	U32 Value;
	U32 RetValue=0;

	NX_ASSERT( 4 >= Group );
	NX_ASSERT( (0!=Group) || ( BitNumber <= 31 ) );
	NX_ASSERT( (1!=Group) || ( BitNumber <= 31 ) );
	NX_ASSERT( (2!=Group) || ( BitNumber <= 31 ) );
	NX_ASSERT( (3!=Group) || ( BitNumber <= 24 ) );
	NX_ASSERT( CNULL != __g_pRegister );

	/* ensure bit not set for both ranges */
	NX_ASSERT( ((max_8ma[Group] >> BitNumber) & 1) && ((max_16ma[Group] >> BitNumber) & 1) )

	if( BitNumber >= 16 ) { shift	= (BitNumber-16) * 2; }
	else		      { shift	=  BitNumber	 * 2; }

	SelectReg = BitNumber/16;

	Value = ( __g_pRegister->PADSTRENGTHGPIO[Group][SelectReg] >> shift ) & 0x03;

	/* switch range based on bit array */
	if ((max_8ma[Group] >> BitNumber) & 1) {
		switch (Value) {
		case 0:	 RetValue = 2; break;
		case 1:	 RetValue = 4; break;
		case 2:	 RetValue = 6; break;
		case 3:	 RetValue = 8; break;
		default: NX_ASSERT( CFALSE );
		}
	} else if ((max_16ma[Group] >> BitNumber) & 1) {
		switch (Value) {
		case  0: RetValue =  4; break;
		case  1: RetValue =  8; break;
		case  2: RetValue = 12; break;
		case  3: RetValue = 16; break;
		default: NX_ASSERT( CFALSE );
		}
	} else
		NX_ASSERT( CFALSE );

	return RetValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set BUS Pad's output drive strength(current)
 *	@param[in]	Bus		Select bus to setting.
 *	@param[in]	mA		Set output drive strenght(current). \n
 *
 *				- Select Bus : NX_CLKPWR_BUSPAD_DDR_CNTL, NX_CLKPWR_BUSPAD_DDR_ADDR, NX_CLKPWR_BUSPAD_DDR_DATA
 *					- Set to 2mA, 4mA, 6mA, 8mA
 *
 *				- Select Bus : NX_CLKPWR_BUSPAD_VSYNC, NX_CLKPWR_BUSPAD_HSYNC, NX_CLKPWR_BUSPAD_DE
 *					- Set to 2mA, 4mA, 6mA, 8mA
 *
 *	@return		None.
 *	@see		NX_CLKPWR_SetGPIOPadStrength, NX_CLKPWR_GetGPIOPadStrength,
 *				NX_CLKPWR_GetBusPadStrength
 */
void	NX_CLKPWR_SetBusPadStrength( NX_CLKPWR_BUSPAD Bus, U32 mA )
{
	register U32 regvalue;
	U32 SetmA=0;
	U32 shift;

	NX_ASSERT(
				(Bus == NX_CLKPWR_BUSPAD_STATIC_CNTL	) ||
				(Bus == NX_CLKPWR_BUSPAD_STATIC_ADDR	) ||
				(Bus == NX_CLKPWR_BUSPAD_STATIC_DATA	) ||
				(Bus == NX_CLKPWR_BUSPAD_VSYNC		) ||
				(Bus == NX_CLKPWR_BUSPAD_HSYNC		) ||
				(Bus == NX_CLKPWR_BUSPAD_DE			)
				);

	NX_ASSERT( CNULL != __g_pRegister );

	switch( mA )
	{
	case 2:	SetmA = 0; break;
	case 4:	SetmA = 1; break;
	case 6: SetmA = 2; break;
	case 8: SetmA = 3; break;
	default:	NX_ASSERT( CFALSE );
	}

	shift = Bus;

	regvalue = __g_pRegister->PADSTRENGTHBUS;

	regvalue &= ~( 0x03 << shift );
	regvalue |= SetmA << shift;

//	__g_pRegister->PADSTRENGTHBUS = regvalue;
	WriteIODW(&__g_pRegister->PADSTRENGTHBUS, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get BUS Pad's output drive strength(current)
 *	@param[in]	Bus Select bus
 *	@return		BUS Pad's output drive strength(current) ( 2mA ~ 8mA )
 *	@see		NX_CLKPWR_SetGPIOPadStrength,	NX_CLKPWR_GetGPIOPadStrength,
 *				NX_CLKPWR_SetBusPadStrength
 */
U32		NX_CLKPWR_GetBusPadStrength( NX_CLKPWR_BUSPAD Bus )
{
	U32 Value;
	U32 RetValue=0;

	NX_ASSERT(
			(Bus == NX_CLKPWR_BUSPAD_STATIC_CNTL	) ||
			(Bus == NX_CLKPWR_BUSPAD_STATIC_ADDR	) ||
			(Bus == NX_CLKPWR_BUSPAD_STATIC_DATA	) ||
			(Bus == NX_CLKPWR_BUSPAD_VSYNC			) ||
			(Bus == NX_CLKPWR_BUSPAD_HSYNC			) ||
			(Bus == NX_CLKPWR_BUSPAD_DE				)
				);

	NX_ASSERT( CNULL != __g_pRegister );

	Value = ( __g_pRegister->PADSTRENGTHBUS >> Bus ) & 0x03;

	switch( Value )
	{
		case 0:	RetValue =	2; break;
		case 1:	RetValue =	4; break;
		case 2:	RetValue =	6; break;
		case 3:	RetValue =	8; break;
		default:	NX_ASSERT( CFALSE );
	}

	return RetValue;
}
