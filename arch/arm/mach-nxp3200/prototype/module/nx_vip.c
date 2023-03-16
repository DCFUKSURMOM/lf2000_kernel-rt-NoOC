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
//	Module		: Video Input Processor Interface
//	File		: nx_vip.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_vip.h"

static  struct
{
    struct NX_VIP_RegisterSet *pRegister;

} __g_ModuleVariables[NUMBER_OF_VIP_MODULE] = { {CNULL,}, };

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.\n
 *	@see	NX_VIP_GetNumberOfModule
 */
CBOOL	NX_VIP_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_VIP_MODULE; i++ )
		{
			__g_ModuleVariables[i].pRegister = CNULL;
		}

		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 *	@see		NX_VIP_Initialize
 */
U32		NX_VIP_GetNumberOfModule( void )
{
	return NUMBER_OF_VIP_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		Module's physical address
 *	@see		NX_VIP_GetSizeOfRegisterSet,
 *				NX_VIP_SetBaseAddress,		NX_VIP_GetBaseAddress,
 *				NX_VIP_OpenModule,			NX_VIP_CloseModule,
 *				NX_VIP_CheckBusy,			NX_VIP_CanPowerDown
 */
U32		NX_VIP_GetPhysicalAddress( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );

    return  (U32)( PHY_BASEADDR_VIP_MODULE + (OFFSET_OF_VIP_MODULE * ModuleIndex) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_VIP_GetPhysicalAddress,
 *				NX_VIP_SetBaseAddress,		NX_VIP_GetBaseAddress,
 *				NX_VIP_OpenModule,			NX_VIP_CloseModule,
 *				NX_VIP_CheckBusy,			NX_VIP_CanPowerDown
 */
U32		NX_VIP_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_VIP_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_VIP_GetPhysicalAddress,	NX_VIP_GetSizeOfRegisterSet,
 *				NX_VIP_GetBaseAddress,
 *				NX_VIP_OpenModule,			NX_VIP_CloseModule,
 *				NX_VIP_CheckBusy,			NX_VIP_CanPowerDown
 */
void	NX_VIP_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != BaseAddress );

	__g_ModuleVariables[ModuleIndex].pRegister = (struct NX_VIP_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		Module's base address.
 *	@see		NX_VIP_GetPhysicalAddress,	NX_VIP_GetSizeOfRegisterSet,
 *				NX_VIP_SetBaseAddress,
 *				NX_VIP_OpenModule,			NX_VIP_CloseModule,
 *				NX_VIP_CheckBusy,			NX_VIP_CanPowerDown
 */
U32		NX_VIP_GetBaseAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_VIP_GetPhysicalAddress,	NX_VIP_GetSizeOfRegisterSet,
 *				NX_VIP_SetBaseAddress,		NX_VIP_GetBaseAddress,
 *				NX_VIP_CloseModule,
 *				NX_VIP_CheckBusy,			NX_VIP_CanPowerDown
 */
CBOOL	NX_VIP_OpenModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_VIP_GetPhysicalAddress,	NX_VIP_GetSizeOfRegisterSet,
 *				NX_VIP_SetBaseAddress,		NX_VIP_GetBaseAddress,
 *				NX_VIP_OpenModule,
 *				NX_VIP_CheckBusy,			NX_VIP_CanPowerDown
 */
CBOOL	NX_VIP_CloseModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_VIP_GetPhysicalAddress,	NX_VIP_GetSizeOfRegisterSet,
 *				NX_VIP_SetBaseAddress,		NX_VIP_GetBaseAddress,
 *				NX_VIP_OpenModule,			NX_VIP_CloseModule,
 *				NX_VIP_CanPowerDown
 */
CBOOL	NX_VIP_CheckBusy( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_VIP_GetPhysicalAddress,	NX_VIP_GetSizeOfRegisterSet,
 *				NX_VIP_SetBaseAddress,		NX_VIP_GetBaseAddress,
 *				NX_VIP_OpenModule,			NX_VIP_CloseModule,
 *				NX_VIP_CheckBusy
 */
CBOOL	NX_VIP_CanPowerDown( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		Interrupt number
 *	@see											NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
S32		NX_VIP_GetInterruptNumber( U32 ModuleIndex )
{
	static const U32 IntNumVIP[NUMBER_OF_VIP_MODULE] = { INTNUM_OF_VIP_MODULE };

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );

	return	IntNumVIP[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	IntNum	Interrupt Number ( 0:VSYNC, 1:HSYNC, 2:Clipper & Decimator Done ).
 *	@param[in]	Enable	\b CTRUE	indicate that Interrupt Enable. \n
 *						\b CFALSE	indicate that Interrupt Disable.
 *	@return		None.
 *	@see		NX_VIP_GetInterruptNumber,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
void	NX_VIP_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable )
{
	const U16 ODINTENB_BITPOS = 8;

	register U16 regvalue;
	register struct NX_VIP_RegisterSet*	pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( 3>IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 2 > IntNum )
	{
		regvalue = pRegister->VIP_HVINT & ~( 1 << (8+IntNum) );

		if( Enable ){	regvalue |=	( 1 << (8+IntNum));	}
		else		{	regvalue &= ~( 1 << (8+IntNum));	}

	//	pRegister->VIP_HVINT = regvalue;
		WriteIOW(&pRegister->VIP_HVINT, regvalue);
	}
	else
	{
		if( Enable )
		{
		//	pRegister->VIP_ODINT = 1 << ODINTENB_BITPOS;
			WriteIOW(&pRegister->VIP_ODINT, (1 << ODINTENB_BITPOS));
		}
		else
		{
		//	pRegister->VIP_ODINT = 0;
			WriteIOW(&pRegister->VIP_ODINT, 0);
		}
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	IntNum	Interrupt Number( 0:VSYNC, 1:HSYNC, 2:Clipper & Decimator Done ).
 *	@return		\b CTRUE	indicate that Interrupt is enabled. \n
 *				\b CFALSE	indicate that Interrupt is disabled.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *													NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
CBOOL	NX_VIP_GetInterruptEnable( U32 ModuleIndex, S32 IntNum )
{
	const U16 ODINTENB_BITPOS	= 8;
	const U16 ODINTENB_MASK		= 1 << ODINTENB_BITPOS;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( 3>IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( 2 > IntNum )
	{
		return (__g_ModuleVariables[ModuleIndex].pRegister->VIP_HVINT & ( 1 << (8+IntNum)) ) ? CTRUE : CFALSE;
	}

	return (__g_ModuleVariables[ModuleIndex].pRegister->VIP_ODINT & ODINTENB_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set VSYNC interrupt enable or disable. \n
 *							- EnableFlag[1] : Set HSYNC interrupt enable or disable. \n
 *							- EnableFlag[2] : Set Clipper & Decimator Done interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
void	NX_VIP_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag )
{
	const U32 VHINTENB_BITPOS = 8;
	const U32 ODINTENB_BITPOS = 8;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->VIP_HVINT = (U16)((EnableFlag & 0x03) << VHINTENB_BITPOS);
	WriteIOW(&pRegister->VIP_HVINT, (U16)((EnableFlag & 0x03) << VHINTENB_BITPOS));

//	pRegister->VIP_ODINT =	(U16)((EnableFlag & 0x04) << (ODINTENB_BITPOS-2));
	WriteIOW(&pRegister->VIP_ODINT, (U16)((EnableFlag & 0x04) << (ODINTENB_BITPOS-2)));
}
//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : VSYNC interrupt's setting value. \n
 *				- Return Value[1] : HSYNC interrupt's setting value. \n
 *				- Return Value[2] : Clipper & Decimator Done interrupt's setting value. \n
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *													NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
U32		NX_VIP_GetInterruptEnable32( U32 ModuleIndex )
{
	const U32 VHINTENB_BITPOS	= 8;
	const U32 VHINTENB_MASK		= 0x03 << VHINTENB_BITPOS;
	const U32 ODINTENB_MASK		= 0x01 << 8;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	return (U32)(((pRegister->VIP_HVINT & VHINTENB_MASK)>>8) | ((pRegister->VIP_ODINT & ODINTENB_MASK)>>6) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	IntNum	Interrupt Number( 0:VSYNC, 1:HSYNC, 2:Clipper & Decimator Done ).
 *	@return		\b CTRUE	indicate that Pending is seted. \n
 *				\b CFALSE	indicate that Pending is Not Seted.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
CBOOL	NX_VIP_GetInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	const U16 ODINTPEND_BITPOS	 = 0;
	const U16 ODINTPEND_MASK	 = 1 << ODINTPEND_BITPOS;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( 3 > IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( 2 > IntNum )
	{
		return (__g_ModuleVariables[ModuleIndex].pRegister->VIP_HVINT & (1 << IntNum) ) ? CTRUE : CFALSE;
	}

	return (__g_ModuleVariables[ModuleIndex].pRegister->VIP_ODINT & ODINTPEND_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : VSYNC pending state. \n
 *				- Return Value[1] : HSYNC pending state. \n
 *				- Return Value[2] : Clipper & Decimator Done pending state. \n
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *													NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
U32		NX_VIP_GetInterruptPending32( U32 ModuleIndex )
{
	const U32 VHINTPEND_MASK = 0x03;
	const U32 ODINTPEND_MASK = 0x01;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	return (U32)((pRegister->VIP_HVINT & VHINTPEND_MASK) | (pRegister->VIP_ODINT & ODINTPEND_MASK)<<2 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	IntNum	Interrupt number( 0:VSYNC, 1:HSYNC, 2:Clipper & Decimator Done ).
 *	@return		None.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
void	NX_VIP_ClearInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	const U32 VIPINTENB_MASK = 0x03 << 8;

	register struct NX_VIP_RegisterSet	*pRegister;
	register U16 regvalue;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( 3>IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 2 > IntNum )
	{
		regvalue = pRegister->VIP_HVINT & VIPINTENB_MASK;

		regvalue |= (0x01 << IntNum);

	//	pRegister->VIP_HVINT = regvalue;
		WriteIOW(&pRegister->VIP_HVINT, regvalue);
	}
	else
	{
		regvalue  = pRegister->VIP_ODINT;
		regvalue |= 1;

	//	pRegister->VIP_ODINT = regvalue;
		WriteIOW(&pRegister->VIP_ODINT, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *  @param[in]  ModuleIndex		An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : VSYNC pending bit. \n
 *								- PendingFlag[1] : HSYNC pending bit. \n
 *								- PendingFlag[2] : Clipper & Decimator Done pending bit. \n
 *	@return		None.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *													NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
void	NX_VIP_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag )
{
	const U32 VHINTPEND_MASK = 0x03;
	const U32 ODINTPEND_MASK = 0x01;

	register U16 regvalue;
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->VIP_HVINT & ~VHINTPEND_MASK;
	pRegister->VIP_HVINT = (U16)(regvalue | (PendingFlag & 0x03));

	regvalue = pRegister->VIP_ODINT & ~ODINTPEND_MASK;

//	pRegister->VIP_ODINT = (U16)(regvalue | (PendingFlag & 0x04)>>2 );
	WriteIOW(&pRegister->VIP_ODINT, (U16)(regvalue | (PendingFlag & 0x04)>>2 ));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *  @param[in]  ModuleIndex			An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Enable	\b CTRUE	indicate that Set to all interrupt enable. \n
 *						\b CFALSE	indicate that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
void	NX_VIP_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable )
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( Enable )
	{
	//	pRegister->VIP_HVINT	= (U16)(0x03<<8);
		WriteIOW(&pRegister->VIP_HVINT, (U16)(0x03<<8));

	//	pRegister->VIP_ODINT	= (U16)(0x01<<8);
		WriteIOW(&pRegister->VIP_ODINT, (U16)(0x01<<8));
	}
	else
	{
	//	pRegister->VIP_HVINT	= (U16)0x00;
		WriteIOW(&pRegister->VIP_HVINT, (U16)0x00);

	//	pRegister->VIP_ODINT	= (U16)0x00;
		WriteIOW(&pRegister->VIP_ODINT, (U16)0x00);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		\b CTRUE	indicate that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicate that All interrupt is disabled.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *													NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
CBOOL	NX_VIP_GetInterruptEnableAll( U32 ModuleIndex )
{
	const U16 VHSINTENB_MASK	= 0x03 << 8;
	const U16 ODINTENB_MASK		= 0x01 << 8;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( (pRegister->VIP_HVINT & VHSINTENB_MASK) || (pRegister->VIP_ODINT & ODINTENB_MASK) )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		\b CTRUE	indicate that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicate that All pending is NOT seted.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,
 *				NX_VIP_ClearInterruptPendingAll,	NX_VIP_GetInterruptPendingNumber
 */
CBOOL	NX_VIP_GetInterruptPendingAll( U32 ModuleIndex )
{
	const U16 VHSINTPEND_MASK	= 0x03;
	const U16 ODINTPEND_MASK	= 0x01;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( (pRegister->VIP_HVINT & VHSINTPEND_MASK) || (pRegister->VIP_ODINT & ODINTPEND_MASK) )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		None.
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *													NX_VIP_GetInterruptPendingNumber
 */
void	NX_VIP_ClearInterruptPendingAll( U32 ModuleIndex )
{
	const U16 VHSINTPEND_MASK	= 0x03;
	const U16 ODINTPEND_MASK	= 0x01;

	register struct NX_VIP_RegisterSet	*pRegister;
	register U16 	regvalue;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue  = pRegister->VIP_HVINT;
	regvalue |= VHSINTPEND_MASK;

//	pRegister->VIP_HVINT = regvalue;
	WriteIOW(&pRegister->VIP_HVINT, regvalue);

	regvalue  = pRegister->VIP_ODINT;
	regvalue |= ODINTPEND_MASK;

//	pRegister->VIP_ODINT = regvalue;
	WriteIOW(&pRegister->VIP_ODINT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		Pending Number( If all pending is not set then return -1 ).\n
 *				0 (VSYNC), 1 (HSYNC), 2 (Clipper & Decimator Done)
 *	@see		NX_VIP_GetInterruptNumber,			NX_VIP_SetInterruptEnable,
 *				NX_VIP_GetInterruptEnable,			NX_VIP_SetInterruptEnable32,
 *				NX_VIP_GetInterruptEnable32,		NX_VIP_GetInterruptPending,
 *				NX_VIP_GetInterruptPending32,		NX_VIP_ClearInterruptPending,
 *				NX_VIP_ClearInterruptPending32,		NX_VIP_SetInterruptEnableAll,
 *				NX_VIP_GetInterruptEnableAll,		NX_VIP_GetInterruptPendingAll,
 *				NX_VIP_ClearInterruptPendingAll
 */
S32		NX_VIP_GetInterruptPendingNumber( U32 ModuleIndex )	// -1 if None
{
	const S16	VIP_PEND[8] = { -1, 0, 1, 0, 2, 0, 1, 0 };

	register struct NX_VIP_RegisterSet	*pRegister;
	register U16 Pend;
	register U16 Pend0;
	register U16 Pend1;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	Pend0 = pRegister->VIP_HVINT;
	Pend0 = Pend0 & (Pend0>>8);

	Pend1 = pRegister->VIP_ODINT;
	Pend1 = Pend1 & (Pend1>>8);

	Pend = ( Pend1<<2 | Pend0 );

	NX_ASSERT( 8 > Pend );

	return VIP_PEND[Pend];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see										NX_VIP_GetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,		NX_VIP_GetClockBClkMode,
 *				NX_VIP_SetClockSource,			NX_VIP_GetClockSource,
 *				NX_VIP_SetClockDivisor,			NX_VIP_GetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable,	NX_VIP_GetClockDivisorEnable
 */
void		NX_VIP_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_VIP_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->VIPCLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//	pRegister->VIPCLKENB = regvalue;
	WriteIODW(&pRegister->VIPCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		Current PCLK mode
 *	@see		NX_VIP_SetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,		NX_VIP_GetClockBClkMode,
 *				NX_VIP_SetClockSource,			NX_VIP_GetClockSource,
 *				NX_VIP_SetClockDivisor,			NX_VIP_GetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable,	NX_VIP_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_VIP_GetClockPClkMode( U32 ModuleIndex )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->VIPCLKENB & ( 1UL << PCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set System Bus Clock's operation Mode
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	mode		BCLK Mode
 *	@return		None.
 *	@see		NX_VIP_SetClockPClkMode,		NX_VIP_GetClockPClkMode,
 *												NX_VIP_GetClockBClkMode,
 *				NX_VIP_SetClockSource,			NX_VIP_GetClockSource,
 *				NX_VIP_SetClockDivisor,			NX_VIP_GetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable,	NX_VIP_GetClockDivisorEnable
 */
void NX_VIP_SetClockBClkMode( U32 ModuleIndex, NX_BCLKMODE mode )
{
	register U32 regvalue;
	register struct NX_VIP_RegisterSet* pRegister;
	U32 clkmode=0;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	switch(mode)
	{
		case NX_BCLKMODE_DISABLE:	clkmode = 0;		break;
		case NX_BCLKMODE_DYNAMIC:	clkmode = 2;		break;
		case NX_BCLKMODE_ALWAYS:	clkmode = 3;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->VIPCLKENB;
	regvalue &= ~(0x03);
	regvalue |= clkmode & 0x03;

//	pRegister->VIPCLKENB = regvalue;
	WriteIODW(&pRegister->VIPCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get System Bus Clock's operation Mode
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		BCLK Mode
 *	@see		NX_VIP_SetClockPClkMode,		NX_VIP_GetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,
 *				NX_VIP_SetClockSource,			NX_VIP_GetClockSource,
 *				NX_VIP_SetClockDivisor,			NX_VIP_GetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable,	NX_VIP_GetClockDivisorEnable
 */
NX_BCLKMODE	NX_VIP_GetClockBClkMode( U32 ModuleIndex )
{
	const U32 BCLKMODE	= 3UL<<0;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	switch( __g_ModuleVariables[ModuleIndex].pRegister->VIPCLKENB & BCLKMODE )
	{
		case 0 :	return NX_BCLKMODE_DISABLE;
		case 2 :	return NX_BCLKMODE_DYNAMIC;
		case 3 :	return NX_BCLKMODE_ALWAYS;
	}

	NX_ASSERT( CFALSE );
	return NX_BCLKMODE_DISABLE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock source of clock generator
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@param[in]	ClkSrc	Select clock source of clock generator.\n
 *						0:PLL0, 1:PLL1, 2:Reserved, 3:ICLKIN, 4: Invert ICLKIN\n
 *	@remarks	VIP controller have one clock generator. so \e Index must set to 0.
 *	@return		None.
 *	@see		NX_VIP_SetClockPClkMode,		NX_VIP_GetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,		NX_VIP_GetClockBClkMode,
 *												NX_VIP_GetClockSource,
 *				NX_VIP_SetClockDivisor,			NX_VIP_GetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable,	NX_VIP_GetClockDivisorEnable
 */
void NX_VIP_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register struct NX_VIP_RegisterSet* pRegister;
	register U32 	regvalue;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( (5 > ClkSrc) && (2 !=ClkSrc) );
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->VIPCLKGEN[Index][0];

	regvalue &= ~CLKSRCSEL_MASK;
	regvalue |= ClkSrc << CLKSRCSEL_POS;

//	pRegister->VIPCLKGEN[Index][0] = regvalue;
	WriteIODW(&pRegister->VIPCLKGEN[Index][0], regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock source of specified clock generator.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@return		Clock source of clock generator \n
 *				0:PLL0, 1:PLL1, 2:Reserved, 3:ICLKIN, 4: Invert ICLKIN\n
 *	@remarks	VIP controller have one clock generator. so \e Index must set to 0.
 *	@see		NX_VIP_SetClockPClkMode,		NX_VIP_GetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,		NX_VIP_GetClockBClkMode,
 *				NX_VIP_SetClockSource,
 *				NX_VIP_SetClockDivisor,			NX_VIP_GetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable,	NX_VIP_GetClockDivisorEnable
 */
U32				NX_VIP_GetClockSource( U32 ModuleIndex, U32 Index )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	return ( pRegister->VIPCLKGEN[Index][0] & CLKSRCSEL_MASK ) >> CLKSRCSEL_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock divisor of specified clock generator.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 );
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 256 ).
 *	@return		None.
 *	@remarks	VIP controller have one clock generator. so \e Index must set to 0.
 *	@see		NX_VIP_SetClockPClkMode,		NX_VIP_GetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,		NX_VIP_GetClockBClkMode,
 *				NX_VIP_SetClockSource,			NX_VIP_GetClockSource,
 *												NX_VIP_GetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable,	NX_VIP_GetClockDivisorEnable
 */
void NX_VIP_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divisor )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	register struct NX_VIP_RegisterSet	*pRegister;
	register U32 regvalue;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( 1 <= Divisor && Divisor <= 256 );
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue	=	pRegister->VIPCLKGEN[Index][0];

	regvalue	&= ~CLKDIV_MASK;
	regvalue	|= (Divisor-1) << CLKDIV_POS;

//	pRegister->VIPCLKGEN[Index][0] = regvalue;
	WriteIODW(&pRegister->VIPCLKGEN[Index][0], regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock divisor of specified clock generator.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 );
 *	@return		Clock divisor ( 1 ~ 256 ).
 *	@remarks	VIP controller have one clock generator. so \e Index must set to 0.
 *	@see		NX_VIP_SetClockPClkMode,		NX_VIP_GetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,		NX_VIP_GetClockBClkMode,
 *				NX_VIP_SetClockSource,			NX_VIP_GetClockSource,
 *				NX_VIP_SetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable,	NX_VIP_GetClockDivisorEnable
 */
U32 NX_VIP_GetClockDivisor( U32 ModuleIndex, U32 Index )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	return ((pRegister->VIPCLKGEN[Index][0] & CLKDIV_MASK) >> CLKDIV_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock generator's operation
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Enable	\b CTRUE	indicate that Enable of clock generator. \n
 *						\b CFALSE	indicate that Disable of clock generator.
 *	@return		None.
 *	@see		NX_VIP_SetClockPClkMode,		NX_VIP_GetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,		NX_VIP_GetClockBClkMode,
 *				NX_VIP_SetClockSource,			NX_VIP_GetClockSource,
 *				NX_VIP_SetClockDivisor,			NX_VIP_GetClockDivisor,
 *												NX_VIP_GetClockDivisorEnable
 */
void NX_VIP_SetClockDivisorEnable( U32 ModuleIndex, CBOOL Enable )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register struct NX_VIP_RegisterSet	*pRegister;
	register U32 regvalue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister   = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue	=	pRegister->VIPCLKENB;

	regvalue	&=	~CLKGENENB_MASK;
	regvalue	|= (U32)Enable << CLKGENENB_POS;

//	pRegister->VIPCLKENB	=	regvalue;
	WriteIODW(&pRegister->VIPCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of clock generator's operation
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		\b CTRUE	indicate that Clock generator is enabled.\n
 *				\b CFALSE	indicate that Clock generator is disabled.
 *	@see		NX_VIP_SetClockPClkMode,		NX_VIP_GetClockPClkMode,
 *				NX_VIP_SetClockBClkMode,		NX_VIP_GetClockBClkMode,
 *				NX_VIP_SetClockSource,			NX_VIP_GetClockSource,
 *				NX_VIP_SetClockDivisor,			NX_VIP_GetClockDivisor,
 *				NX_VIP_SetClockDivisorEnable
 */
CBOOL NX_VIP_GetClockDivisorEnable( U32 ModuleIndex )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	return	(CBOOL)( (pRegister->VIPCLKENB & CLKGENENB_MASK) >> CLKGENENB_POS );
}

//------------------------------------------------------------------------------
//	Module customized operations
//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable Video Input Port
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	bVIPEnb		Set it to CTURE to enable VIP.
 *	@param[in]	bSepEnb		Set it to CTURE to enable Separator.
 *	@param[in]	bClipEnb	Set it to CTRUE to enable Clipper.
 *	@param[in]	bDeciEnb	Set it to CTRUE to enable Decimator.
 *	@return		None
 *	@see		NX_VIP_GetVIPEnable
 */
void
NX_VIP_SetVIPEnable
(
	U32   ModuleIndex,
	CBOOL bVIPEnb,
	CBOOL bSepEnb,
	CBOOL bClipEnb,
	CBOOL bDeciEnb
)
{
	const U16 VIPENB	= 1U<<0;
	const U16 SEPENB	= 1U<<8;
	const U16 CLIPENB	= 1U<<1;
	const U16 DECIENB	= 1U<<0;

	register U16 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	temp = pRegister->VIP_CONFIG;
	if( bVIPEnb )	temp |= (U16) VIPENB;
	else			temp &= (U16)~VIPENB;

//	pRegister->VIP_CONFIG = temp;
	WriteIOW(&pRegister->VIP_CONFIG, temp);

	temp = 0;
	if( bSepEnb )	temp |= (U16)SEPENB;
	if( bClipEnb )	temp |= (U16)CLIPENB;
	if( bDeciEnb )	temp |= (U16)DECIENB;

//	pRegister->VIP_CDENB = temp;
	WriteIOW(&pRegister->VIP_CDENB, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether VIP controllers are enabled or disabled.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pbVIPEnb	Determines whether VIP is enabled or not.\n
 *							CTRUE = Enable, CFALSE = Disable.
 *	@param[out] pbSepEnb	Determines whether Separator is enabled or not.\n
 *							CTRUE = Enable, CFALSE = Disable.
 *	@param[out] pbClipEnb	Determines whether Clipper is enabled or not.\n
 *							CTRUE = Enable, CFALSE = Disable.
 *	@param[out] pbDeciEnb	Determines whether Decimator is enabled or not.\n
 *							CTRUE = Enable, CFALSE = Disable.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetVIPEnable
 */
void
NX_VIP_GetVIPEnable
(
	U32    ModuleIndex,
	CBOOL *pbVIPEnb,
	CBOOL *pbSepEnb,
	CBOOL *pbClipEnb,
	CBOOL *pbDeciEnb
)
{
	const U16 VIPENB	= 1U<<0;
	const U16 SEPENB	= 1U<<8;
	const U16 CLIPENB	= 1U<<1;
	const U16 DECIENB	= 1U<<0;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( CNULL != pbVIPEnb )		*pbVIPEnb	= (pRegister->VIP_CONFIG & VIPENB ) ? CTRUE : CFALSE;
	if( CNULL != pbSepEnb )		*pbSepEnb	= (pRegister->VIP_CDENB	& SEPENB ) ? CTRUE : CFALSE;
	if( CNULL != pbClipEnb )	*pbClipEnb	= (pRegister->VIP_CDENB	& CLIPENB) ? CTRUE : CFALSE;
	if( CNULL != pbDeciEnb )	*pbDeciEnb	= (pRegister->VIP_CDENB	& DECIENB) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Select VIP module's input port.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	InputPort	Select input port \n.
 *							- Port A : Use VIDCLK0, VIDHSYNC0, VIDVSYNC0, FIELD/DVALID, VID0 pin for connection to Video decoder. \n
 *							- Port B : Use VIDCLK1, VIDHSYNC1, VIDVSYNC1, SIPFIELD/DVALID, VID1 pin for connection to Video decoder.
 *	@return		None.
 *	@remark		SPICA only has one VIP module. but VIP module have 2 set GPIO for connection to video decoder. \n
 *				so, Should select which pin use.
 *	@see		NX_VIP_GetInputPort
 */
void	NX_VIP_SetInputPort( U32 ModuleIndex, NX_VIP_INPUTPORT InputPort )
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NX_VIP_INPUTPORT_B >= InputPort );
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->VIP_VIP1 = (U32)InputPort;
	WriteIOW(&pRegister->VIP_VIP1, (U32)InputPort);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get selected input port of VIP modules.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return		NX_VIP_INPUTPORT_A	: Port A connected to VIP module(VIDCLK0, VIDHSYNC0, VIDVSYNC0, FIELD/DVALID, VID0). \n
 *				NX_VIP_INPUTPORT_B	: Port B connected to VIP module(VIDCLK1, VIDHSYNC1, VIDVSYNC1, SIPFIELD/DVALID, VID1). \n
 *	@see		NX_VIP_SetInputPort
 */
NX_VIP_INPUTPORT	NX_VIP_GetInputPort( U32 ModuleIndex )
{
	const U16 SIPENB_MASK = 1 << 0;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (NX_VIP_INPUTPORT)(__g_ModuleVariables[ModuleIndex].pRegister->VIP_VIP1 & SIPENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set settings for the data mode.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	DataOrder	Specifies the input video data order.
 *	@param[in]	DataWidth	Specifies the input data width. 8 or 16 are only valid.
 *	@return		None
 *	@see		NX_VIP_GetDataMode
 */
void
NX_VIP_SetDataMode
(
	U32 				ModuleIndex,
	NX_VIP_DATAORDER	DataOrder,
	U32					DataWidth
)
{
	const U32 DORDER_POS	= 2;
	const U32 DORDER_MASK	= 3UL<<DORDER_POS;
	const U32 DWIDTH_POS	= 1;
	const U32 DWIDTH_MASK	= 1UL<<DWIDTH_POS;
	register U32 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 4 > DataOrder );
	NX_ASSERT( 8 == DataWidth || 16 == DataWidth );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	temp = (U32)pRegister->VIP_CONFIG;
	temp &= ~(DORDER_MASK | DWIDTH_MASK);
	temp |= ((U32)DataOrder<<DORDER_POS);
	temp |= ((8 == DataWidth) ? DWIDTH_MASK : 0);

//	pRegister->VIP_CONFIG = (U16)temp;
	WriteIOW(&pRegister->VIP_CONFIG, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current settings for the data mode.
 *  @param[in]  ModuleIndex		An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pDataOrder		Indicates the input video data order.
 *	@param[out] pDataWidth		Indicates the input data width.
 *	@return		None
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_VIP_GetDataConfig
 */
void
NX_VIP_GetDataMode
(
	U32 				 ModuleIndex,
	NX_VIP_DATAORDER	*pDataOrder,
	U32					*pDataWidth
)
{
	const U32 DORDER_POS	= 2;
	const U32 DORDER_MASK	= 3UL<<DORDER_POS;
	const U32 DWIDTH_POS	= 1;
	const U32 DWIDTH_MASK	= 1UL<<DWIDTH_POS;
	register U32 temp;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	temp = (U32)__g_ModuleVariables[ModuleIndex].pRegister->VIP_CONFIG;

	if( CNULL != pDataOrder )	*pDataOrder = (NX_VIP_DATAORDER)((temp & DORDER_MASK)>>DORDER_POS);
	if( CNULL != pDataWidth )	*pDataWidth = (temp & DWIDTH_MASK) ? 8 : 16;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set configuration for sync generation.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	bExtSync	Specifies whether external H/V Sync signals are used or not.\n
 *							CTRUE = External H/V Sync, CFALSE = H/V Sync in Embedded Sync.

 *	@param[in]	AVW		Specifies the active video width in clocks, 0 ~ 8190.\n
 *							This value must be a multiple of 2.
 *	@param[in]	AVH		Specifies the active video height in lines. 0 ~ 2047.

 *	@param[in]	HSW		Specifies the horizontal sync width in clocks.\n
 *							This argument is only valid when bExtSync is CFALSE.
 *	@param[in]	HFP		Specifies the horizontal sync front porch in clocks.\n
 *							This value must be greater than 7.\n
 *							This argument is only valid when bExtSync is CFALSE.
 *	@param[in]	HBP		Specifies the horizontal sync back porch in clocks.
 *							This value must be greater than 0.\n
 *							This argument is only valid when bExtSync is CTRUE.

 *	@param[in]	VSW		Specifies the vertical sync width in lines.\n
 *							This argument is only valid when bExtSync is CFALSE.
 *	@param[in]	VFP		Specifies the vertical sync front porch in lines.\n
 *							This argument is only valid when bExtSync is CFALSE.
 *	@param[in]	VBP		Specifies the vertical sync back porch in lines.\n
 *							This argument is only valid when bExtSync is CTRUE.

 *	@return		None.
 *	@remark		You have to set sync parameters even if Embedded sync is used.\n
 *				A sum of arguments(HSW + HFP + HBP or VSW + VFP + VBP) has to be less than 65536.\n
 *				See follwing figure for more details.
 *	@code
 *
 *						<----------------TOTAL----------------->
 *						<-SW->
 *						+-------+								+-------+
 *						|		|								|		|
 *		Sync	--------+		+--------------/-/--------------+		+---
 *				<-FP->		<-BP-> <--ACTIVE VIDEO-->
 *		Active	--+					+-------/-/--------+
 *		Video	|		(BLANK)		|	(ACTIVE DATA)	|		(BLANK)
 *				+--------------------+					+-----------------
 *	@endcode
 *	@see		NX_VIP_GetHVSync
 */
void
NX_VIP_SetHVSync
(
	U32 		ModuleIndex,
	CBOOL		bExtSync,
	U32			AVW,
	U32			AVH,
	U32			HSW,
	U32			HFP,
	U32			HBP,
	U32			VSW,
	U32			VFP,
	U32			VBP
)
{
	const U32 DRANGE		= 1UL<<9;
	const U32 EXTSYNCENB	= 1UL<<8;

	register U32 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	temp = (U32)pRegister->VIP_CONFIG;
	temp &= ~DRANGE;	// Reserved 0.
	if( bExtSync )
	{
		temp |=	EXTSYNCENB;
	}
	else
	{
		temp &= ~EXTSYNCENB;
	}

//	pRegister->VIP_CONFIG = (U16)temp;
	WriteIOW(&pRegister->VIP_CONFIG, (U16)temp);

	temp = (U32)pRegister->VIP_SYNCCTRL;
	temp |= 0x0300;		// EXTHSPOL = EXTVSPOL = 1

//	pRegister->VIP_SYNCCTRL = (U16)temp;
	WriteIOW(&pRegister->VIP_SYNCCTRL, (U16)temp);

	NX_ASSERT( 8192 > AVW && 0 == (AVW & 1) );
	NX_ASSERT( 2048 > AVH );

	if( bExtSync )
	{
	//	pRegister->VIP_IMGWIDTH	= (U16)(AVW>>1);
		WriteIOW(&pRegister->VIP_IMGWIDTH, (U16)(AVW>>1));
	}
	else
	{
	//	pRegister->VIP_IMGWIDTH	= (U16)((AVW>>1) + 2);
		WriteIOW(&pRegister->VIP_IMGWIDTH, (U16)((AVW>>1) + 2));
	}

//	pRegister->VIP_IMGHEIGHT	= (U16)AVH;
	WriteIOW(&pRegister->VIP_IMGHEIGHT, (U16)AVH);

	if( bExtSync )
	{
		NX_ASSERT( 0 < VBP );
		NX_ASSERT( 65536 >= (VBP + AVH));
		NX_ASSERT( 0 < HBP );
		NX_ASSERT( 65536 >= (HBP + AVW));

	//	pRegister->VIP_VBEGIN	= (U16)(VBP - 1);
		WriteIOW(&pRegister->VIP_VBEGIN, (U16)(VBP - 1));

	//	pRegister->VIP_VEND		= (U16)(VBP + AVH - 1);
		WriteIOW(&pRegister->VIP_VEND, (U16)(VBP + AVH - 1));

	//	pRegister->VIP_HBEGIN	= (U16)(HBP - 1);
		WriteIOW(&pRegister->VIP_HBEGIN, (U16)(HBP - 1));

	//	pRegister->VIP_HEND		= (U16)(HBP + AVW - 1);
		WriteIOW(&pRegister->VIP_HEND, (U16)(HBP + AVW - 1));
	}
	else
	{
		NX_ASSERT( 65536 > (VFP + VSW) );
		NX_ASSERT( 7 <= HFP );
		NX_ASSERT( 65536 > (HFP + HSW - 7) );

	//	pRegister->VIP_VBEGIN	= (U16)(VFP + 1);
		WriteIOW(&pRegister->VIP_VBEGIN, (U16)(VFP + 1));

	//	pRegister->VIP_VEND		= (U16)(VFP + VSW + 1);
		WriteIOW(&pRegister->VIP_VEND, (U16)(VFP + VSW + 1));

	//	pRegister->VIP_HBEGIN	= (U16)(HFP - 7);
		WriteIOW(&pRegister->VIP_HBEGIN, (U16)(HFP - 7));

	//	pRegister->VIP_HEND		= (U16)(HFP + HSW - 7);
		WriteIOW(&pRegister->VIP_HEND, (U16)(HFP + HSW - 7));
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current configuration for sync generation.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pbExtSync	Indicates whether the external H/V Sync signals are used or not.\n
 *							CTRUE = External H/V Sync, CFALSE = H/V Sync in Embedded Sync.

 *	@param[out] pAVW		Indicates the active video width in clocks.
 *	@param[out] pAVH		Indicates the active video height in lines.

 *	@param[out] pHBEGIN	When *pbExtSync is CTRUE, this value is used to generate a internal horizontal blank.
 *							This value specifies the number of clocks from the end of horizontal sync to the end of horizontal blank. \n
 *							-> HBEGIN = tHBP - 1 \n
 *							When *pbExtSync is CFALSE, this value is used to generate a internal horizontal sync.
 *							This value specifies the number of clocks from the begin of horizontal blank to the begin of horizontal sync.\n
 *							-> HBEGIN = tHFP - 7
 *	@param[out] pHEND		When EXTSYNCENB is set, this value is used to generate a internal horizontal blank.
 *							This value specifies the number of clocks from the end of horizontal sync to the begin of horizontal blank.\n
 *							-> HEND = tHBP + tAVW - 1 \n
 *							When EXTSYNCENB is clear, this value is used to generate a internal horizontal sync.
 *							This value specifies the number of clocks from the begin of horizontal blank to the end of horizontal sync.\n
 *							-> HEND = tHFP + tHSW - 7
 *	@param[out] pVBEGIN	When *pbExtSync is CTRUE, this value is used to generate a internal vertical blank.
 *							This value specifies the number of lines from the end of vertical sync to the end of vertical blank.\n
 *							-> VBEGIN = tVBP - 1 \n
 *								When *pbExtSync is CFALSE, this value is used to generate a internal vertical sync.
 *							This value specifies the number of lines from the begin of vertical blank to the begin of vertical sync.\n
 *							-> VBEGIN = tVFP + 1
 *	@param[out] pVEND		When *pbExtSync is CTRUE, this value is used to generate a internal vertical blank.
 *							This value specifies the number of lines from the end of vertical sync to the begin of vertical blank.\n
 *							-> VEND = tVBP + tAVH - 1 \n
 *								When *pbExtSync is CFALSE, this value is used to generate a internal vertical sync.
 *							This value specifies the number of lines from the begin of vertical blank to the end of vertical sync.\n
 *							-> VEND = tVFP + tVSW + 1
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetHVSync
 */
void
NX_VIP_GetHVSync
(
	U32 	 ModuleIndex,
	CBOOL	*pbExtSync,
	U32		*pAVW,
	U32		*pAVH,
	U32		*pHBEGIN,
	U32		*pHEND,
	U32		*pVBEGIN,
	U32		*pVEND
)
{
	const U16 EXTSYNCENB = 1U<<8;
	CBOOL	bExtSync;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	bExtSync = (pRegister->VIP_CONFIG & EXTSYNCENB) ? CTRUE : CFALSE;
	if( CNULL != pbExtSync )	*pbExtSync = bExtSync;

	if( CNULL != pAVW )		*pAVW	 = ((U32)(pRegister->VIP_IMGWIDTH)<<1) - ((bExtSync) ? 0 : 4);
	if( CNULL != pAVH )		*pAVH	 = (U32)pRegister->VIP_IMGHEIGHT;

	if( CNULL != pVBEGIN )	*pVBEGIN = (U32)pRegister->VIP_VBEGIN;
	if( CNULL != pVEND )	*pVEND	 = (U32)pRegister->VIP_VEND;
	if( CNULL != pHBEGIN )	*pHBEGIN = (U32)pRegister->VIP_HBEGIN;
	if( CNULL != pHEND )	*pHEND	 = (U32)pRegister->VIP_HEND;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set configuration for data valid signal.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	bExtDValid	Specifies whether external DValid signal is used or not.\n
 *							CTRUE = External DVaild & Internal generation, CFALSE = Internal generation only.
 *	@param[in]	bDValidPol	Specifies the external DValid polarity.\n
 *							This argument is only valid when bExtDValid is set to CTRUE.\n
 *							CTRUE = Active High, CFALSE = Active Low.
 *	@return		None.
 *	@see		NX_VIP_GetDValidMode
 */
void
NX_VIP_SetDValidMode
(
	U32 	ModuleIndex,
	CBOOL	bExtDValid,
	CBOOL	bDValidPol
)
{
	const U32 DVALIDPOL	= 1UL<<4;
	const U32 EXTDVENB	= 1UL<<2;

	register U32 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	temp = (U32)pRegister->VIP_SYNCCTRL;

	if( bExtDValid )	temp |=	EXTDVENB;
	else				temp &= ~EXTDVENB;
	if( bDValidPol )	temp |=	DVALIDPOL;
	else				temp &= ~DVALIDPOL;

//	pRegister->VIP_SYNCCTRL = (U16)temp;
	WriteIOW(&pRegister->VIP_SYNCCTRL, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current configuration for data valid signal.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pbExtDValid	Indicates whether external DValid signal is used or not.\n
 *								CTRUE = External DVaild & Internal generation, CFALSE = Internal generation only.
 *	@param[out] pbDValidPol		Indicates the external DValid polarity.\n
 *								This argument is only valid when *pbExtDValid is CTRUE.\n
 *								CTRUE = Active High, CFALSE = Active Low.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetDValidMode
 */
void
NX_VIP_GetDValidMode
(
	U32 	 ModuleIndex,
	CBOOL	*pbExtDValid,
	CBOOL	*pbDValidPol
)
{
	const U32 DVALIDPOL	= 1UL<<4;
	const U32 EXTDVENB	= 1UL<<2;

	register U32 temp;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	temp = (U32)__g_ModuleVariables[ModuleIndex].pRegister->VIP_SYNCCTRL;

	if( CNULL != pbExtDValid )	*pbExtDValid = (temp & EXTDVENB	) ? CTRUE : CFALSE;
	if( CNULL != pbDValidPol )	*pbDValidPol = (temp & DVALIDPOL ) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set configuration for field signal processing.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	bExtField	Specifies whether the external Field signal is used or not.\n
 *							CTRUE = External field, CFALSE = Field signal in Embedded Sync.
 *	@param[in]	FieldSel	Specifies the field selection.

 *	@param[in]	bInterlace	Specifies whether the scan mode for the Clipper & Decimator is interlace or not.\n
 *							CTRUE = Interlace mode, CFALSE = Progressive mode.
 *	@param[in]	bInvField	Specifies the Field polarity which is used for the Clipper & Decimator.\n
 *							CTRUE = Bypass (Low is odd field), CFALSE = Invert (Low is even field).
 *	@return		None.
 *	@see		NX_VIP_GetFieldMode, NX_VIP_GetFieldStatus
 */
void
NX_VIP_SetFieldMode
(
	U32 	 	    ModuleIndex,
	CBOOL			bExtField,
	NX_VIP_FIELDSEL	FieldSel,
	CBOOL			bInterlace,
	CBOOL			bInvField
)
{
	const U32 EXTFIELDENB			= 1UL<<3;
	const U32 NX_VIP_FIELDSEL_MASK	= 3UL<<0;
	const U32 INTERLACEENB			= 1UL<<1;
	const U32 FIELDINV				= 1UL<<0;
	register U32 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( 4 > FieldSel );
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	temp = (U32)pRegister->VIP_SYNCCTRL;

	if( bExtField )		temp |=	EXTFIELDENB;
	else				temp &= ~EXTFIELDENB;

	temp &= ~NX_VIP_FIELDSEL_MASK;
	temp |= (U32)(FieldSel);

//	pRegister->VIP_SYNCCTRL = (U16)temp;
	WriteIOW(&pRegister->VIP_SYNCCTRL, (U16)temp);

//	pRegister->VIP_SCANMODE = (U16)(((bInterlace) ? INTERLACEENB : 0)
//									| ((bInvField ) ? FIELDINV		: 0));

	WriteIOW(&pRegister->VIP_SCANMODE, (U16)(((bInterlace) ? INTERLACEENB : 0) | ((bInvField ) ? FIELDINV : 0)));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current configuration for field signal processing.
 *  @param[in]  ModuleIndex		An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pbExtField		Indicates whether the external Field signal is used or not.\n
 *								CTRUE = External field, CFALSE = Field signal in Embedded Sync.
 *	@param[out] pFieldSel		Indicates the field selection.

 *	@param[out] pbInterlace	Indicates whether the scan mode for the Clipper & Decimator is interlace or not.\n
 *								CTRUE = Interlace mode, CFALSE = Progressive mode.
 *	@param[out] pbInvField		Indicates the Field polarity which is used for the Clipper & Decimator.\n
 *								CTRUE = Bypass (Low is odd field), CFALSE = Invert (Low is even field).
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetFieldMode, NX_VIP_GetFieldStatus
 */
void
NX_VIP_GetFieldMode
(
	U32 	 	     ModuleIndex,
	CBOOL			*pbExtField,
	NX_VIP_FIELDSEL	*pFieldSel,
	CBOOL			*pbInterlace,
	CBOOL			*pbInvField
)
{
	const U32 EXTFIELDENB			= 1UL<<3;
	const U32 NX_VIP_FIELDSEL_MASK	= 3UL<<0;
	const U32 INTERLACEENB			= 1UL<<1;
	const U32 FIELDINV				= 1UL<<0;
	register U32 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	temp = (U32)pRegister->VIP_SYNCCTRL;
	if( CNULL != pbExtField )	*pbExtField = (temp & EXTFIELDENB) ? CTRUE : CFALSE;
	if( CNULL != pFieldSel )	*pFieldSel	= (NX_VIP_FIELDSEL)(temp & NX_VIP_FIELDSEL_MASK);

	temp = (U32)pRegister->VIP_SCANMODE;
	if( CNULL != pbInterlace )	*pbInterlace = (temp & INTERLACEENB) ? CTRUE : CFALSE;
	if( CNULL != pbInvField )	*pbInvField	 = (temp & FIELDINV	) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get the last field status.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return	CTRUE	indicates the last field is even field.\n
 *			CFALSE	indicate the last field is odd field.
 *	@remark	This field signal is generated by VIP block and transferred to the
 *			Clipper & Decimator block. Therefore this field status does not
 *			represent the result of the field signal polarity control in the
 *			Clipper & Decimator block.\n
 *			This field signal is update at every FrameStart(the begin of the
 *			vertical active video). Therefore If you call this function after
 *			the VSYNC interrupt, It returns the last field status which is
 *			already processed for the input data.\n
 *			Because the field signal is captured by PCLK, you have to call the
 *			function SetClockPClkMode() with PCLKMODE_ALWAYS value to get
 *			correct field status.
 *	@see	NX_VIP_SetFieldMode, NX_VIP_GetFieldMode
 */
CBOOL
NX_VIP_GetFieldStatus( U32 ModuleIndex )
{
	const U16 LASTFIELD = 1U<<5;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->VIP_SYNCCTRL & LASTFIELD) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get current status of the internal HSYNC signal.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return	CTRUE	indicates the internal HSYNC is in activate status.\n
 *			CFALSE	indicate the internal HSYNC is in inactivate status.
 *	@remark	The internal HSYNC signal is generated by internal sync generator
 *			for embedded sync. Therefore In case of external sync mode, the
 *			return value of this function is meaningless.
 *	@see	NX_VIP_GetVSyncStatus
 */
CBOOL
NX_VIP_GetHSyncStatus( U32 ModuleIndex )
{
	const U16 CURHSYNC = 1U<<1;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->VIP_SYNCMON & CURHSYNC) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get current status of the internal VSYNC signal.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return	CTRUE	indicates the internal VSYNC is in activate status.\n
 *			CFALSE	indicate the internal VSYNC is in inactivate status.
 *	@remark	The internal VSYNC signal is generated by internal sync generator
 *			for embedded sync. Therefore In case of external sync mode, the
 *			return value of this function is meaningless.
 *	@see	NX_VIP_GetHSyncStatus
 */
CBOOL
NX_VIP_GetVSyncStatus( U32 ModuleIndex )
{
	const U16 CURVSYNC = 1U<<0;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->VIP_SYNCMON & CURVSYNC) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Specifies the FIFO reset mode.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	FIFOReset	Specifies the FIFO reset mode.
 *	@return		None.
 *	@see		NX_VIP_GetFIFOResetMode, NX_VIP_GetFIFOStatus, NX_VIP_ResetFIFO
 */
void
NX_VIP_SetFIFOResetMode
(
	U32 				ModuleIndex,
	NX_VIP_FIFORESET	FIFOReset
)
{
	const U32 RESETFIFOSEL_POS = 1UL;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 4 > FIFOReset );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->VIP_FIFOCTRL = (U16)(FIFOReset<<RESETFIFOSEL_POS);
	WriteIOW(&pRegister->VIP_FIFOCTRL, (U16)(FIFOReset<<RESETFIFOSEL_POS));
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get the FIFO reset mode.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return	the current FIFO reset mode..
 *	@see	NX_VIP_SetFIFOResetMode, NX_VIP_GetFIFOStatus, NX_VIP_ResetFIFO
 */
NX_VIP_FIFORESET
NX_VIP_GetFIFOResetMode( U32 ModuleIndex )
{
	const U16 RESETFIFOSEL_POS	= 1;
	const U16 RESETFIFOSEL_MASK = 3U<<RESETFIFOSEL_POS;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	return (NX_VIP_FIFORESET)((pRegister->VIP_FIFOCTRL & RESETFIFOSEL_MASK)>>RESETFIFOSEL_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get current FIFO status.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return FIFO status which consists of ORed NX_VIP_FSTATUS combination.
 *	@see	NX_VIP_SetFIFOResetMode, NX_VIP_GetFIFOResetMode, NX_VIP_ResetFIFO
 */
U32
NX_VIP_GetFIFOStatus( U32 ModuleIndex )
{
	const U32 FIFOSTATUS_POS = 8UL;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)(__g_ModuleVariables[ModuleIndex].pRegister->VIP_FIFOCTRL >> FIFOSTATUS_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief	Reset the internal FIFO.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return	None.
 *	@remark	This function has only affect when the FIFO reset mode is NX_VIP_FIFORESET_CPU or NX_VIP_FIFORESET_ALL.
 *	@see	NX_VIP_SetFIFOResetMode, NX_VIP_GetFIFOResetMode, NX_VIP_GetFIFOStatus
 */
void
NX_VIP_ResetFIFO( U32 ModuleIndex )
{
	const U16 RESETFIFO = 1U<<0;
	register U16 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	temp 	  = pRegister->VIP_FIFOCTRL;

//	pRegister->VIP_FIFOCTRL = (U16)(temp | RESETFIFO);
	WriteIOW(&pRegister->VIP_FIFOCTRL, (U16)(temp | RESETFIFO));

//	pRegister->VIP_FIFOCTRL = (U16)temp;	// clear it manually.
	WriteIOW(&pRegister->VIP_FIFOCTRL, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get the number of total clocks in a horizontal active video.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return	the number of total clocks in a horizontal active video.
 *	@see	NX_VIP_GetVerCount
 */
U32
NX_VIP_GetHorCount( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister->VIP_HCOUNT;
}

//------------------------------------------------------------------------------
/**
 *	@brief	Get the number of total lines in a vertical active video.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@return	the number of total lines in a vertical active video.
 *	@see	NX_VIP_GetHorCount
 */
U32
NX_VIP_GetVerCount( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister->VIP_VCOUNT;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a region to crop the input image by the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Left	Specifies the x-coordinate, in pixels, of the upper-left corner of the clip region in the input image, 0 ~ 4095.
 *	@param[in]	Top		Specifies the y-coordinate, in lines, of the upper-left corner of the clip region in the input image, 0 ~ 2047.
 *	@param[in]	Right	Specifies the x-coordinate, in pixels, of the lower-right corner of the clip region in the input image, 0 ~ 4095.
 *	@param[in]	Bottom	Specifies the y-coordinate, in lines, of the lower-right corner of the clip region in the input image, 0 ~ 2047.
 *	@return		None.
 *	@remark		In case of interlace scan mode, The input image is a field data not
 *				a frame data. Therefore you should set the y-coordinate carefully.
 *				The clipping width (Right - Left) must be a multiple of 16 and
 *				the clipping height (Bottom - Top) must be a even number.
 *	@see		NX_VIP_GetClipRegion
 */
void
NX_VIP_SetClipRegion
(
	U32 ModuleIndex,
	U32	Left,
	U32	Top,
	U32	Right,
	U32	Bottom
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 4096 > Left );
	NX_ASSERT( 2048 > Top );
	NX_ASSERT( 4096 > Right );
	NX_ASSERT( 2048 > Bottom );
	NX_ASSERT( Left < Right );
	NX_ASSERT( Top < Bottom );
	NX_ASSERT( 0 == ((Right-Left)%16) );
	NX_ASSERT( 0 == ((Bottom-Top)&1) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->CLIP_LEFT	= (U16)Left;
	WriteIOW(&pRegister->CLIP_LEFT, (U16)Left);

//	pRegister->CLIP_RIGHT	= (U16)Right;
	WriteIOW(&pRegister->CLIP_RIGHT, (U16)Right);

//	pRegister->CLIP_TOP		= (U16)Top;
	WriteIOW(&pRegister->CLIP_TOP, (U16)Top);

//	pRegister->CLIP_BOTTOM	= (U16)Bottom;
	WriteIOW(&pRegister->CLIP_BOTTOM, (U16)Bottom);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a region to crop the input image by the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pLeft		Indicates the x-coordinate, in pixels, of the upper-left corner of the clip region in the input image.
 *	@param[out] pTop		Indicates the y-coordinate, in lines, of the upper-left corner of the clip region in the input image.
 *	@param[out] pRight		Indicates the x-coordinate, in pixels, of the lower-right corner of the clip region in the input image.
 *	@param[out] pBottom		Indicates the y-coordinate, in lines, of the lower-right corner of the clip region in the input image.
 *	@return		None.
 *	@remark		In case of interlace scan mode, The input image is a field data not
 *				a frame data.\n
 *				Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetClipRegion
 */
void
NX_VIP_GetClipRegion
(
	U32  ModuleIndex,
	U32	*pLeft,
	U32	*pTop,
	U32	*pRight,
	U32	*pBottom
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( CNULL != pLeft )	*pLeft		= (U32)pRegister->CLIP_LEFT;
	if( CNULL != pTop )		*pTop		= (U32)pRegister->CLIP_TOP;
	if( CNULL != pRight )	*pRight		= (U32)pRegister->CLIP_RIGHT;
	if( CNULL != pBottom )	*pBottom	= (U32)pRegister->CLIP_BOTTOM;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the image size to scale-down the clipped image by the Decimator.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	SrcWidth	Specifies the width of the clip region, 0 ~ 4095.
 *	@param[in]	SrcHeight	Specifies the height of the clip region, 0 ~ 2047.
 *	@param[in]	DstWidth	Specifies the width of the output image of the decimator.
 *							This argument must be less than or equal to SrcWidth,
 *							and a multiple of 16.
 *	@param[in]	DstHeight	Specifies the height of the output image of the decimator.
 *							This argument must be less than or equal to SrcHeight,
 *							and a even number.
 *	@return		None.
 *	@remark		The input image data transferred to the Decimator via the Clipper.
 *				Thererfore the source image of the Decimator is the result image of
 *				the Clipper. and the Decimator can only handle the even field data
 *				in the interlace scan mode.
 *	@see		NX_VIP_GetDecimation
 */
void
NX_VIP_SetDecimation
(
	U32 ModuleIndex,
	U32	SrcWidth,
	U32	SrcHeight,
	U32	DstWidth,
	U32	DstHeight
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 4096 > SrcWidth );
	NX_ASSERT( 2048 > SrcHeight );
	NX_ASSERT( DstWidth <= SrcWidth );
	NX_ASSERT( DstHeight <= SrcHeight );
	NX_ASSERT( 0 == (DstWidth%16) );
	NX_ASSERT( 0 == (DstHeight&1) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->DECI_TARGETW	= (U16)DstWidth;
	WriteIOW(&pRegister->DECI_TARGETW, (U16)DstWidth);

//	pRegister->DECI_TARGETH	= (U16)DstHeight;
	WriteIOW(&pRegister->DECI_TARGETH, (U16)DstHeight);

//	pRegister->DECI_DELTAW	= (U16)(SrcWidth - DstWidth);
	WriteIOW(&pRegister->DECI_DELTAW, (U16)(SrcWidth - DstWidth));

//	pRegister->DECI_DELTAH	= (U16)(SrcHeight - DstHeight);
	WriteIOW(&pRegister->DECI_DELTAH, (U16)(SrcHeight - DstHeight));

//	pRegister->DECI_CLEARW	= (S16)((DstWidth<<1) - SrcWidth);
	WriteIOW((volatile U16 *)&pRegister->DECI_CLEARW, (S16)((DstWidth<<1) - SrcWidth));

//	pRegister->DECI_CLEARH	= (S16)((DstHeight<<1) - SrcHeight);
	WriteIOW((volatile U16 *)&pRegister->DECI_CLEARH, (S16)((DstHeight<<1) - SrcHeight));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the paramters for the decimation.
 *  @param[in]  ModuleIndex		An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pDstWidth		Indicates the width of the output image of the decimator.
 *	@param[out] pDstHeight		Indicates the height of the output image of the decimator.
 *	@param[out] pDeltaWidth		Indicates the difference between clipped image width and destination image width of decimator.\n
 *								-> DeltaWidth = the clipped image width - DstWidth
 *	@param[out] pDeltaHeight	Indicates the difference between clipped image height and destination image height of decimator.\n
 *								-> DeltaHeight = the clipped image height - DstHeight
 *	@param[out] pClearWidth		Indicates the difference between destination image width and DeltaWidth of decimator.\n
 *								-> ClearWidth = DstWidth - DeltaWidth
 *	@param[out] pClearHeight	Indicates the difference between destination image height and DeltaHeight of decimator.\n
 *								-> ClearHeight = DstHeight - DeltaHeight
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetDecimation
 */
void
NX_VIP_GetDecimation
(
	U32  ModuleIndex,
	U32	*pDstWidth,
	U32	*pDstHeight,
	U32	*pDeltaWidth,
	U32	*pDeltaHeight,
	S32	*pClearWidth,
	S32 *pClearHeight
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( CNULL != pDstWidth )	*pDstWidth		= (U32)pRegister->DECI_TARGETW;
	if( CNULL != pDstHeight )	*pDstHeight		= (U32)pRegister->DECI_TARGETH;
	if( CNULL != pDeltaWidth )	*pDeltaWidth	= (U32)pRegister->DECI_DELTAW;
	if( CNULL != pDeltaHeight )	*pDeltaHeight	= (U32)pRegister->DECI_DELTAH;
	if( CNULL != pClearWidth )	*pClearWidth	= (S32)((U32)pRegister->DECI_CLEARW | ((pRegister->DECI_CLEARW & (1<<12)) ? 0xFFFFE000 : 0));
	if( CNULL != pClearHeight )	*pClearHeight	= (S32)((U32)pRegister->DECI_CLEARH | ((pRegister->DECI_CLEARH & (1<<11)) ? 0xFFFFF000 : 0));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the data Format, rotation and flip for the output image of the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Format		Specifies the data Format of the Clipper.
 *	@param[in]	bRotation	Set it to CTRUE to rotate the image clockwise by 90 degree.
 *							This argument must be CFALSE when Format is not NX_VIP_FORMAT_YUYV.
 *	@param[in]	bHFlip		Set it to CTRUE to flip the image horizontally.
 *							This argument is only valid when Format is not NX_VIP_FORMAT_YUYV.
 *	@param[in]	bVFlip		Set it to CTRUE to flip the image vertically.
 *							This argument is only valid when Format is not NX_VIP_FORMAT_YUYV.
 *	@return		None.
 *	@see		NX_VIP_GetClipperFormat
 */
void
NX_VIP_SetClipperFormat
(
	U32  			ModuleIndex,
	NX_VIP_FORMAT	Format,
	CBOOL			bRotation,
	CBOOL			bHFlip,
	CBOOL			bVFlip
)
{
	const U32 ROTATION	= 1UL<<2;
	const U32 VFLIP		= 1UL<<1;
	const U32 HFLIP		= 1UL<<0;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 4 > Format );
	NX_ASSERT( (NX_VIP_FORMAT_YUYV != Format) || (CFALSE == bRotation) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( NX_VIP_FORMAT_YUYV == Format )
	{
	//	pRegister->CLIP_YUYVENB = 1;
		WriteIOW(&pRegister->CLIP_YUYVENB, 1);
	}
	else
	{
	//	pRegister->CLIP_YUYVENB	= 0;
		WriteIOW(&pRegister->CLIP_YUYVENB, 0);

	//	pRegister->CLIP_FORMAT	= (U16)Format;
		WriteIOW(&pRegister->CLIP_FORMAT, (U16)Format);
	}

//	pRegister->CLIP_ROTFLIP	= (U16)(((bRotation) ? ROTATION : 0)
//										| ((bVFlip	) ? VFLIP	: 0)
//										| ((bHFlip	) ? HFLIP	: 0));
	WriteIOW(&pRegister->CLIP_ROTFLIP, (U16)(((bRotation) ? ROTATION : 0)
										| ((bVFlip	) ? VFLIP	: 0)
										| ((bHFlip	) ? HFLIP	: 0)));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the data Format, rotation and flip for the output image of the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pFormat		Indicates the data Format of the Clipper.
 *	@param[out] pbRotation		Indicates whether the image is rotated clockwise by 90 degree.
 *								CTRUE = 90' Rotation, CFALSE = No Rotation
 *	@param[out] pbHFlip			Indicates whether to flip an image horizontally or not.
 *								This argument is only valid when Format is not Format_YUYV.\n
 *								CTRUE = Horizontal Flip, CFALSE = No Horizontal Flip
 *	@param[out] pbVFlip			Indicates whether to flip an image vertically or not.
 *								This argument is only valid when Format is not Format_YUYV.\n
 *								CTRUE = Vertical Flip, CFALSE = No Vertical Flip
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetClipperFormat
 */
void
NX_VIP_GetClipperFormat
(
	U32  			 ModuleIndex,
	NX_VIP_FORMAT	*pFormat,
	CBOOL			*pbRotation,
	CBOOL			*pbHFlip,
	CBOOL			*pbVFlip
)
{
	const U32 ROTATION	= 1UL<<2;
	const U32 VFLIP		= 1UL<<1;
	const U32 HFLIP		= 1UL<<0;
	register U32 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( CNULL != pFormat )
	{
		if( pRegister->CLIP_YUYVENB )	*pFormat = NX_VIP_FORMAT_YUYV;
		else								*pFormat = (NX_VIP_FORMAT)pRegister->CLIP_FORMAT;
	}

	temp = (U32)pRegister->CLIP_ROTFLIP;
	if( CNULL != pbRotation )	*pbRotation = (temp & ROTATION) ? CTRUE : CFALSE;
	if( CNULL != pbHFlip	)	*pbHFlip	= (temp & HFLIP	) ? CTRUE : CFALSE;
	if( CNULL != pbVFlip	)	*pbVFlip	= (temp & VFLIP	) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the data Format, rotation and flip for the output image of the Decimator.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Format		Specifies the data Format of the Decimator. This argument must not be Format_YUYV;
 *	@param[in]	bRotation	Set it to CTRUE to rotate the image clockwise by 90 degree.
 *	@param[in]	bHFlip		Set it to CTRUE to flip the image horizontally.
 *	@param[in]	bVFlip		Set it to CTRUE to flip the image vertically.
 *	@return		None.
 *	@see		NX_VIP_GetDecimatorFormat
 */
void
NX_VIP_SetDecimatorFormat
(
	U32  			ModuleIndex,
	NX_VIP_FORMAT	Format,
	CBOOL			bRotation,
	CBOOL			bHFlip,
	CBOOL			bVFlip
)
{
	const U32 ROTATION	= 1UL<<2;
	const U32 VFLIP		= 1UL<<1;
	const U32 HFLIP		= 1UL<<0;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 3 > Format );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->DECI_FORMAT	= (U16)Format;
	WriteIOW(&pRegister->DECI_FORMAT, (U16)Format);

//	pRegister->DECI_ROTFLIP	= (U16)(((bRotation) ? ROTATION : 0)
//										| ((bVFlip	) ? VFLIP	: 0)
//										| ((bHFlip	) ? HFLIP	: 0));
	WriteIOW(&pRegister->DECI_ROTFLIP, (U16)(((bRotation) ? ROTATION : 0)
										| ((bVFlip	) ? VFLIP	: 0)
										| ((bHFlip	) ? HFLIP	: 0)));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the data Format, rotation and flip for the output image of the Decimator.
 *  @param[in]  ModuleIndex		An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pFormat			Indicates the data Format of the Decimator.
 *	@param[out] pbRotation		Indicates whether the image is rotated clockwise by 90 degree.\n
 *								CTRUE = 90' Rotation, CFALSE = No Rotation
 *	@param[out] pbHFlip			Indicates whether to flip an image horizontally or not.\n
 *								CTRUE = Horizontal Flip, CFALSE = No Horizontal Flip
 *	@param[out] pbVFlip			Indicates whether to flip an image vertically or not.\n
 *								CTRUE = Vertical Flip, CFALSE = No Vertical Flip
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetDecimatorFormat
 */
void
NX_VIP_GetDecimatorFormat
(
	U32  			 ModuleIndex,
	NX_VIP_FORMAT	*pFormat,
	CBOOL			*pbRotation,
	CBOOL			*pbHFlip,
	CBOOL			*pbVFlip
)
{
	const U32 ROTATION	= 1UL<<2;
	const U32 VFLIP		= 1UL<<1;
	const U32 HFLIP		= 1UL<<0;
	register U32 temp;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( CNULL != pFormat )		*pFormat = (NX_VIP_FORMAT)pRegister->DECI_FORMAT;

	temp = (U32)pRegister->DECI_ROTFLIP;
	if( CNULL != pbRotation )	*pbRotation = (temp & ROTATION) ? CTRUE : CFALSE;
	if( CNULL != pbHFlip	)	*pbHFlip	= (temp & HFLIP	) ? CTRUE : CFALSE;
	if( CNULL != pbVFlip	)	*pbVFlip	= (temp & VFLIP	) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the base address to store the output image of the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	Format		Specifies the data Format of the Clipper.\n
 *							It must not be Format_YUYV.
 *	@param[in]	Width		Specifies the width of the clip region.
 *	@param[in]	Height		Specifies the height of the clip region.
 *							It must be a height of the frame image even if the scan mode is interlace.
 *	@param[in]	LuAddr		Specifies the base address of the upper-left corner of Y data in a segment.
 *	@param[in]	CbAddr		Specifies the base address of the upper-left corner of Cb data in a segment.
 *	@param[in]	CrAddr		Specifies the base address of the upper-left corner of Cr data in a segment.
 *	@return		None.
 *	@remark		The argument Format, Width and Height, are only used to calculate
 *				image address. so you have to call the function NX_VIP_SetClipperFormat()
 *				and NX_VIP_SetClipRegion() to set these values.\n
 *				This function is for YUV 420, 422, 444 not YUYV. You have to call
 *				the function NX_VIP_SetClipperAddrYUYV for YUYV Format.\n
 *				The Format of LuAddr, CbAddr, CrAddr is as follows,
 *				- [31:24] : Specifies the index of the segment.
 *				- [23:12] : Sepcifies the y-coordinate in the segment.
 *				- [11: 0] : Specifies the x-coordinate in the segment.
 *				.
 *				The Clipper stores a frame image even if the scan mode is interlace
 *				scan mode. Therefore you have to allocate and set memory pool which
 *				can store a frame image.\n
 *				Memory address must align by 8.\n
 *	@see		NX_VIP_SetClipperAddr2D, NX_VIP_GetClipperAddr2D
 */
void
NX_VIP_SetClipperAddr
(
	U32  			ModuleIndex,
	NX_VIP_FORMAT	Format,
	U32				Width,
	U32				Height,
	U32				LuAddr,
	U32				CbAddr,
	U32				CrAddr
)
{
	register U32 segment, left, top;
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 3 > Format );
//	NX_ASSERT( 4096 > (((LuAddr & 0x00000FFF)>> 0) + Width ) );
//	NX_ASSERT( 4096 > (((LuAddr & 0x00FFF000)>>12) + Height) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	segment   =	LuAddr >> 24;
	left	  =	LuAddr & 0x00000FFFUL;
	top		  = (LuAddr & 0x00FFF000UL) >> 12;

//	pRegister->CLIP_LUSEG		= (U16)segment;
	WriteIOW(&pRegister->CLIP_LUSEG, (U16)segment);

//	pRegister->CLIP_LULEFT		= (U16)left;
	WriteIOW(&pRegister->CLIP_LULEFT, (U16)left);

//	pRegister->CLIP_LURIGHT		= (U16)(left + Width);
	WriteIOW(&pRegister->CLIP_LURIGHT, (U16)(left + Width));

//	pRegister->CLIP_LUTOP		= (U16)top;
	WriteIOW(&pRegister->CLIP_LUTOP, (U16)top);

//	pRegister->CLIP_LUBOTTOM	= (U16)(top + Height);
	WriteIOW(&pRegister->CLIP_LUBOTTOM, (U16)(top + Height));


	if( NX_VIP_FORMAT_420 == Format )
	{
		NX_ASSERT( 0 == (Width&1) );
		NX_ASSERT( 0 == (Height&1) );
		Width	>>= 1;
		Height	>>= 1;
	}
	else if( NX_VIP_FORMAT_422 == Format )
	{
		NX_ASSERT( 0 == (Width&1) );
		Width	>>= 1;
	}

//	NX_ASSERT( 4096 > (((CbAddr & 0x00000FFF)>> 0) + Width ) );
//	NX_ASSERT( 4096 > (((CbAddr & 0x00FFF000)>>12) + Height) );
//	NX_ASSERT( 4096 > (((CrAddr & 0x00000FFF)>> 0) + Width ) );
//	NX_ASSERT( 4096 > (((CrAddr & 0x00FFF000)>>12) + Height) );

	segment =	CbAddr >> 24;
	left	=	CbAddr & 0x00000FFFUL;
	top		= (CbAddr & 0x00FFF000UL) >> 12;

//	pRegister->CLIP_CBSEG		= (U16)segment;
	WriteIOW(&pRegister->CLIP_CBSEG, (U16)segment);

//	pRegister->CLIP_CBLEFT		= (U16)left;
	WriteIOW(&pRegister->CLIP_CBLEFT, (U16)left);

//	pRegister->CLIP_CBRIGHT		= (U16)(left + Width);
	WriteIOW(&pRegister->CLIP_CBRIGHT, (U16)(left + Width));

//	pRegister->CLIP_CBTOP		= (U16)top;
	WriteIOW(&pRegister->CLIP_CBTOP, (U16)top);

//	pRegister->CLIP_CBBOTTOM	= (U16)(top + Height);
	WriteIOW(&pRegister->CLIP_CBBOTTOM, (U16)(top + Height));

	segment =	CrAddr >> 24;
	left	=	CrAddr & 0x00000FFFUL;
	top		= (CrAddr & 0x00FFF000UL) >> 12;

//	pRegister->CLIP_CRSEG		= (U16)segment;
	WriteIOW(&pRegister->CLIP_CRSEG, (U16)segment);

//	pRegister->CLIP_CRLEFT		= (U16)left;
	WriteIOW(&pRegister->CLIP_CRLEFT, (U16)left);

//	pRegister->CLIP_CRRIGHT		= (U16)(left + Width);
	WriteIOW(&pRegister->CLIP_CRRIGHT, (U16)(left + Width));

//	pRegister->CLIP_CRTOP		= (U16)top;
	WriteIOW(&pRegister->CLIP_CRTOP, (U16)top);

//	pRegister->CLIP_CRBOTTOM	= (U16)(top + Height);
	WriteIOW(&pRegister->CLIP_CRBOTTOM, (U16)(top + Height));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the base address to store the output image of the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	LuSAddr	Specifies the base address of the upper-left corner of Y data in a segment.
 *	@param[in]	LuEAddr	Specifies the base address of the lower-right corner of Y data in a segment.
 *	@param[in]	CbSAddr	Specifies the base address of the upper-left corner of Cb data in a segment.
 *	@param[in]	CbEAddr	Specifies the base address of the lower-right corner of Cb data in a segment.
 *	@param[in]	CrSAddr	Specifies the base address of the upper-left corner of Cr data in a segment.
 *	@param[in]	CrEAddr	Specifies the base address of the lower-right corner of Cr data in a segment.
 *	@return		None.
 *	@remark		This function is for YUV 420, 422, 444 not YUYV. You have to call
 *				the function NX_VIP_SetClipperAddr for YUYV NX_VIP_FORMAT.\n
 *				The NX_VIP_FORMAT of arguments is as follows,
 *				- [31:24] : Specifies the index of the segment.
 *				- [23:12] : Sepcifies the y-coordinate in the segment.
 *				- [11: 0] : Specifies the x-coordinate in the segment.
 *				.
 *				The Clipper stores a frame image even if the scan mode is interlace
 *				scan mode. Therefore you have to allocate and set memory pool which
 *				can store a frame image.\n
 *				Memory address must align by 8.\n
 *	@see		NX_VIP_SetClipperAddr, NX_VIP_GetClipperAddr2D
 */
void
NX_VIP_SetClipperAddr2D
(
	U32 ModuleIndex,
	U32	LuSAddr,
	U32	LuEAddr,
	U32	CbSAddr,
	U32	CbEAddr,
	U32	CrSAddr,
	U32	CrEAddr
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_ASSERT( (LuSAddr & 0xFF000000) == (LuEAddr & 0xFF000000) );
	NX_ASSERT( (CbSAddr & 0xFF000000) == (CbEAddr & 0xFF000000) );
	NX_ASSERT( (CrSAddr & 0xFF000000) == (CrEAddr & 0xFF000000) );

	NX_ASSERT( (0 == (LuSAddr&0x00007007)) && (0 == (LuEAddr&0x00007007)) );
	NX_ASSERT( (0 == (CbSAddr&0x00007007)) && (0 == (CbEAddr&0x00007007)) );
	NX_ASSERT( (0 == (CrSAddr&0x00007007)) && (0 == (CrEAddr&0x00007007)) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->CLIP_LUSEG		= (U16)(LuSAddr >> 24);
	WriteIOW(&pRegister->CLIP_LUSEG, (U16)(LuSAddr >> 24));

//	pRegister->CLIP_CRSEG		= (U16)(CrSAddr >> 24);
	WriteIOW(&pRegister->CLIP_CRSEG, (U16)(CrSAddr >> 24));

//	pRegister->CLIP_CBSEG		= (U16)(CbSAddr >> 24);
	WriteIOW(&pRegister->CLIP_CBSEG, (U16)(CbSAddr >> 24));

//	pRegister->CLIP_LULEFT		= (U16)(LuSAddr);
	WriteIOW(&pRegister->CLIP_LULEFT, (U16)(LuSAddr));

//	pRegister->CLIP_CRLEFT		= (U16)(CrSAddr);
	WriteIOW(&pRegister->CLIP_CRLEFT, (U16)(CrSAddr));

//	pRegister->CLIP_CBLEFT		= (U16)(CbSAddr);
	WriteIOW(&pRegister->CLIP_CBLEFT, (U16)(CbSAddr));

//	pRegister->CLIP_LURIGHT		= (U16)(LuEAddr);
	WriteIOW(&pRegister->CLIP_LURIGHT, (U16)(LuEAddr));

//	pRegister->CLIP_CRRIGHT		= (U16)(CrEAddr);
	WriteIOW(&pRegister->CLIP_CRRIGHT, (U16)(CrEAddr));

//	pRegister->CLIP_CBRIGHT		= (U16)(CbEAddr);
	WriteIOW(&pRegister->CLIP_CBRIGHT, (U16)(CbEAddr));

//	pRegister->CLIP_LUTOP		= (U16)(LuSAddr >> 12);
	WriteIOW(&pRegister->CLIP_LUTOP, (U16)(LuSAddr >> 12));

//	pRegister->CLIP_CRTOP		= (U16)(CrSAddr >> 12);
	WriteIOW(&pRegister->CLIP_CRTOP, (U16)(CrSAddr >> 12));

//	pRegister->CLIP_CBTOP		= (U16)(CbSAddr >> 12);
	WriteIOW(&pRegister->CLIP_CBTOP, (U16)(CbSAddr >> 12));

//	pRegister->CLIP_LUBOTTOM	= (U16)(LuEAddr >> 12);
	WriteIOW(&pRegister->CLIP_LUBOTTOM, (U16)(LuEAddr >> 12));

//	pRegister->CLIP_CRBOTTOM	= (U16)(CrEAddr >> 12);
	WriteIOW(&pRegister->CLIP_CRBOTTOM, (U16)(CrEAddr >> 12));

//	pRegister->CLIP_CBBOTTOM	= (U16)(CbEAddr >> 12);
	WriteIOW(&pRegister->CLIP_CBBOTTOM, (U16)(CbEAddr >> 12));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the base address to store the output image of the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pLuSAddr	Indicates the base address of the upper-left corner of Y data in a segment.
 *	@param[out] pLuEAddr	Indicates the base address of the lower-right corner of Y data in a segment.
 *	@param[out] pCbSAddr	Indicates the base address of the upper-left corner of Cb data in a segment.
 *	@param[out] pCbEAddr	Indicates the base address of the lower-right corner of Cb data in a segment.
 *	@param[out] pCrSAddr	Indicates the base address of the upper-left corner of Cr data in a segment.
 *	@param[out] pCrEAddr	Indicates the base address of the lower-right corner of Cr data in a segment.
 *	@return		None.
 *	@remark		This function is for YUV 420, 422, 444 not YUYV. You have to call
 *				the function NX_VIP_GetClipperAddrYUYV for YUYV NX_VIP_FORMAT.\n
 *				The NX_VIP_FORMAT of arguments is as follows,
 *				- [31:24] : Specifies the index of the segment.
 *				- [23:12] : Sepcifies the y-coordinate in the segment.
 *				- [11: 0] : Specifies the x-coordinate in the segment.
 *				.
 *				The Clipper stores a frame image even if the scan mode is interlace
 *				scan mode.\n
 *				Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetClipperAddr, NX_VIP_SetClipperAddr2D
 */
void
NX_VIP_GetClipperAddr2D
(
	U32  ModuleIndex,
	U32	*pLuSAddr,
	U32	*pLuEAddr,
	U32	*pCbSAddr,
	U32	*pCbEAddr,
	U32	*pCrSAddr,
	U32	*pCrEAddr
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( CNULL != pLuSAddr )		*pLuSAddr = (((U32)pRegister->CLIP_LUSEG	)<<24)
											| (((U32)pRegister->CLIP_LUTOP	)<<12)
											| (((U32)pRegister->CLIP_LULEFT	)<< 0);
	if( CNULL != pLuEAddr )		*pLuEAddr = (((U32)pRegister->CLIP_LUSEG	)<<24)
											| (((U32)pRegister->CLIP_LUBOTTOM)<<12)
											| (((U32)pRegister->CLIP_LURIGHT )<< 0);
	if( CNULL != pCbSAddr )		*pCbSAddr = (((U32)pRegister->CLIP_CBSEG	)<<24)
											| (((U32)pRegister->CLIP_CBTOP	)<<12)
											| (((U32)pRegister->CLIP_CBLEFT	)<< 0);
	if( CNULL != pCbEAddr )		*pCbEAddr = (((U32)pRegister->CLIP_CBSEG	)<<24)
											| (((U32)pRegister->CLIP_CBBOTTOM)<<12)
											| (((U32)pRegister->CLIP_CBRIGHT )<< 0);
	if( CNULL != pCrSAddr )		*pCrSAddr = (((U32)pRegister->CLIP_CRSEG	)<<24)
											| (((U32)pRegister->CLIP_CRTOP	)<<12)
											| (((U32)pRegister->CLIP_CRLEFT	)<< 0);
	if( CNULL != pCrEAddr )		*pCrEAddr = (((U32)pRegister->CLIP_CRSEG	)<<24)
											| (((U32)pRegister->CLIP_CRBOTTOM)<<12)
											| (((U32)pRegister->CLIP_CRRIGHT )<< 0);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the base address to store the output image of the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	BaseAddr	Specifies the base address of linear YUV data(YUYV).
 *							This argument must be a multiple of 8.
 *	@param[in]	Stride		Specifies the stride of linear YUV data(YUYV).
 *							Generally, Stride has bytes per a line.
 *	@return		None.
 *	@remark		This function is for linear YUYV NX_VIP_FORMAT. You have to call the function
 *				NX_VIP_SetClipperAddr2D for separated YUV
 *				420, 422, 444 NX_VIP_FORMAT.
 *				The Clipper stores a frame image even if the scan mode is interlace
 *				scan mode. Therefore you have to allocate and set memory pool which
 *				can store a frame image.
 *	@see		NX_VIP_GetClipperAddrYUVY
 */
void
NX_VIP_SetClipperAddrYUYV
(
	U32 ModuleIndex,
	U32	BaseAddr,
	U32	Stride
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_ASSERT( 0 == (BaseAddr%8) );
	NX_ASSERT( 0 == (Stride%8) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->CLIP_BASEADDRH	= (U16)(BaseAddr >> 16);
	WriteIOW(&pRegister->CLIP_BASEADDRH, (U16)(BaseAddr >> 16));

//	pRegister->CLIP_BASEADDRL	= (U16)(BaseAddr & 0xFFFFUL);
	WriteIOW(&pRegister->CLIP_BASEADDRL, (U16)(BaseAddr & 0xFFFFUL));

//	pRegister->CLIP_STRIDEH		= (U16)(Stride >> 16);
	WriteIOW(&pRegister->CLIP_STRIDEH, (U16)(Stride >> 16));

//	pRegister->CLIP_STRIDEL		= (U16)(Stride & 0xFFFFUL);
	WriteIOW(&pRegister->CLIP_STRIDEL, (U16)(Stride & 0xFFFFUL));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the base address to store the output image of the Clipper.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pBaseAddr	Specifies the base address of linear YUV data(YUYV).
 *	@param[out] pStride		Specifies the stride of linear YUV data(YUYV). Generally, Stride has bytes per a line.
 *	@return		None.
 *	@remark		This function is for linear YUYV NX_VIP_FORMAT. You have to call the function
 *				NX_VIP_GetClipperAddr2D for Block
 *				Separated YUV 420, 422, 444 NX_VIP_FORMAT.
 *				The Clipper stores a frame image even if the scan mode is interlace
 *				scan mode.\n
 *				Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetClipperAddrYUYV
 */
void
NX_VIP_GetClipperAddrYUYV
(
	U32  ModuleIndex,
	U32	*pBaseAddr,
	U32	*pStride
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( CNULL != pBaseAddr )	*pBaseAddr = (((U32)pRegister->CLIP_BASEADDRH)<<16) | ((U32)pRegister->CLIP_BASEADDRL);
	if( CNULL != pStride	)	*pStride   = (((U32)pRegister->CLIP_STRIDEH	)<<16) | ((U32)pRegister->CLIP_STRIDEL	);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the base address to store the output image of the Decimator.
 *  @param[in]  ModuleIndex		An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	NX_VIP_FORMAT	Specifies the data NX_VIP_FORMAT of the Decimator.\n
 *								It must not be NX_VIP_FORMAT_YUYV.
 *	@param[in]	Width		Specifies the width of the target image of the Decimator.
 *	@param[in]	Height		Specifies the height of the target image of the Decimator.
 *	@param[in]	LuAddr		Specifies the base address of the upper-left corner of Y data in a segment.
 *	@param[in]	CbAddr		Specifies the base address of the upper-left corner of Cb data in a segment.
 *	@param[in]	CrAddr		Specifies the base address of the upper-left corner of Cr data in a segment.
 *	@return		None.
 *	@remark		The argument NX_VIP_FORMAT, Width and Height, are only used to calculate
 *				image address. so you have to call the function SetDecimatorNX_VIP_FORMAT()
 *				and SetDecimation() to set these values.\n
 *				The NX_VIP_FORMAT of LuAddr, CbAddr, CrAddr is as follows,
 *				- [31:24] : Specifies the index of the segment.
 *				- [23:12] : Sepcifies the y-coordinate in the segment.
 *				- [11: 0] : Specifies the x-coordinate in the segment.
 *				.
 *				The Decimator can only store the even field data when the scan mode
 *				is interlace.
 *	@see		NX_VIP_SetDecimatorAddr2D, NX_VIP_GetDecimatorAddr2D
 */
void
NX_VIP_SetDecimatorAddr
(
	U32  			ModuleIndex,
	NX_VIP_FORMAT	NX_VIP_FORMAT,
	U32				Width,
	U32				Height,
	U32				LuAddr,
	U32				CbAddr,
	U32				CrAddr
)
{
	register U32 segment, left, top;

	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_ASSERT( 3 > NX_VIP_FORMAT );

	NX_ASSERT( 4096 > (((LuAddr & 0x00000FFF)>> 0) + Width ) );
	NX_ASSERT( 4096 > (((LuAddr & 0x00FFF000)>>12) + Height) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	segment   =	LuAddr >> 24;
	left	  =	LuAddr & 0x00000FFFUL;
	top		  = (LuAddr & 0x00FFF000UL) >> 12;

	pRegister->DECI_LUSEG		= (U16)segment;
	pRegister->DECI_LULEFT		= (U16)left;
	pRegister->DECI_LURIGHT		= (U16)(left + Width);
	pRegister->DECI_LUTOP		= (U16)top;
	pRegister->DECI_LUBOTTOM	= (U16)(top + Height);

	if( NX_VIP_FORMAT_420 == NX_VIP_FORMAT )
	{
		NX_ASSERT( 0 == (Width&1) );
		NX_ASSERT( 0 == (Height&1) );
		Width	>>= 1;
		Height	>>= 1;
	}
	else if( NX_VIP_FORMAT_422 == NX_VIP_FORMAT )
	{
		NX_ASSERT( 0 == (Width&1) );
		Width	>>= 1;
	}

	NX_ASSERT( 4096 > (((CbAddr & 0x00000FFF)>> 0) + Width ) );
	NX_ASSERT( 4096 > (((CbAddr & 0x00FFF000)>>12) + Height) );
	NX_ASSERT( 4096 > (((CrAddr & 0x00000FFF)>> 0) + Width ) );
	NX_ASSERT( 4096 > (((CrAddr & 0x00FFF000)>>12) + Height) );

	segment =	CbAddr >> 24;
	left	=	CbAddr & 0x00000FFFUL;
	top		= (CbAddr & 0x00FFF000UL) >> 12;

	pRegister->DECI_CBSEG		= (U16)segment;
	pRegister->DECI_CBLEFT		= (U16)left;
	pRegister->DECI_CBRIGHT		= (U16)(left + Width);
	pRegister->DECI_CBTOP		= (U16)top;
	pRegister->DECI_CBBOTTOM	= (U16)(top + Height);

	segment =	CrAddr >> 24;
	left	=	CrAddr & 0x00000FFFUL;
	top		= (CrAddr & 0x00FFF000UL) >> 12;

	pRegister->DECI_CRSEG		= (U16)segment;
	pRegister->DECI_CRLEFT		= (U16)left;
	pRegister->DECI_CRRIGHT		= (U16)(left + Width);
	pRegister->DECI_CRTOP		= (U16)top;
	pRegister->DECI_CRBOTTOM	= (U16)(top + Height);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the base address to store the output image of the Decimator.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[in]	LuSAddr	Specifies the base address of the upper-left corner of Y data in a segment.
 *	@param[in]	LuEAddr	Specifies the base address of the lower-right corner of Y data in a segment.
 *	@param[in]	CbSAddr	Specifies the base address of the upper-left corner of Cb data in a segment.
 *	@param[in]	CbEAddr	Specifies the base address of the lower-right corner of Cb data in a segment.
 *	@param[in]	CrSAddr	Specifies the base address of the upper-left corner of Cr data in a segment.
 *	@param[in]	CrEAddr	Specifies the base address of the lower-right corner of Cr data in a segment.
 *	@return		None.
 *	@remark		The NX_VIP_FORMAT of arguments is as follows,
 *				- [31:24] : Specifies the index of the segment.
 *				- [23:12] : Sepcifies the y-coordinate in the segment.
 *				- [11: 0] : Specifies the x-coordinate in the segment.
 *				.
 *				The Decimator can only store the even field data when the scan mode
 *				is interlace.
 *	@see		NX_VIP_SetDecimatorAddr, NX_VIP_GetDecimatorAddr2D
 */
void
NX_VIP_SetDecimatorAddr2D
(
	U32 ModuleIndex,
	U32	LuSAddr,
	U32	LuEAddr,
	U32	CbSAddr,
	U32	CbEAddr,
	U32	CrSAddr,
	U32	CrEAddr
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_ASSERT( (LuSAddr & 0xFF000000) == (LuEAddr & 0xFF000000) );
	NX_ASSERT( (CbSAddr & 0xFF000000) == (CbEAddr & 0xFF000000) );
	NX_ASSERT( (CrSAddr & 0xFF000000) == (CrEAddr & 0xFF000000) );

	NX_ASSERT( (0 == (LuSAddr&0x00007007)) && (0 == (LuEAddr&0x00007007)) );
	NX_ASSERT( (0 == (CbSAddr&0x00007007)) && (0 == (CbEAddr&0x00007007)) );
	NX_ASSERT( (0 == (CrSAddr&0x00007007)) && (0 == (CrEAddr&0x00007007)) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	pRegister->DECI_LUSEG		= (U16)(LuSAddr >> 24);
	pRegister->DECI_CRSEG		= (U16)(CrSAddr >> 24);
	pRegister->DECI_CBSEG		= (U16)(CbSAddr >> 24);
	pRegister->DECI_LULEFT		= (U16)(LuSAddr);
	pRegister->DECI_CRLEFT		= (U16)(CrSAddr);
	pRegister->DECI_CBLEFT		= (U16)(CbSAddr);
	pRegister->DECI_LURIGHT		= (U16)(LuEAddr);
	pRegister->DECI_CRRIGHT		= (U16)(CrEAddr);
	pRegister->DECI_CBRIGHT		= (U16)(CbEAddr);
	pRegister->DECI_LUTOP		= (U16)(LuSAddr >> 12);
	pRegister->DECI_CRTOP		= (U16)(CrSAddr >> 12);
	pRegister->DECI_CBTOP		= (U16)(CbSAddr >> 12);
	pRegister->DECI_LUBOTTOM	= (U16)(LuEAddr >> 12);
	pRegister->DECI_CRBOTTOM	= (U16)(CrEAddr >> 12);
	pRegister->DECI_CBBOTTOM	= (U16)(CbEAddr >> 12);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the base address to store the output image of the Decimator.
 *  @param[in]  ModuleIndex	An index of module ( 0 : VIP 0, 1 : VIP 1 ).
 *	@param[out] pLuSAddr	Indicates the base address of the upper-left corner of Y data in a segment.
 *	@param[out] pLuEAddr	Indicates the base address of the lower-right corner of Y data in a segment.
 *	@param[out] pCbSAddr	Indicates the base address of the upper-left corner of Cb data in a segment.
 *	@param[out] pCbEAddr	Indicates the base address of the lower-right corner of Cb data in a segment.
 *	@param[out] pCrSAddr	Indicates the base address of the upper-left corner of Cr data in a segment.
 *	@param[out] pCrEAddr	Indicates the base address of the lower-right corner of Cr data in a segment.
 *	@return		None.
 *	@remark		The NX_VIP_FORMAT of arguments is as follows,
 *				- [31:24] : Specifies the index of the segment.
 *				- [23:12] : Sepcifies the y-coordinate in the segment.
 *				- [11: 0] : Specifies the x-coordinate in the segment.
 *				.
 *				The Decimator can only store the even field data when the scan mode
 *				is interlace.\n
 *				Arguments which does not required can be CNULL.
 *	@see		NX_VIP_SetDecimatorAddr, NX_VIP_SetDecimatorAddr2D
 */
void
NX_VIP_GetDecimatorAddr2D
(
	U32  ModuleIndex,
	U32	*pLuSAddr,
	U32	*pLuEAddr,
	U32	*pCbSAddr,
	U32	*pCbEAddr,
	U32	*pCrSAddr,
	U32	*pCrEAddr
)
{
	register struct NX_VIP_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_OF_VIP_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( CNULL != pLuSAddr )		*pLuSAddr = (((U32)pRegister->DECI_LUSEG	)<<24)
											| (((U32)pRegister->DECI_LUTOP	)<<12)
											| (((U32)pRegister->DECI_LULEFT	)<< 0);
	if( CNULL != pLuEAddr )		*pLuEAddr = (((U32)pRegister->DECI_LUSEG	)<<24)
											| (((U32)pRegister->DECI_LUBOTTOM)<<12)
											| (((U32)pRegister->DECI_LURIGHT )<< 0);
	if( CNULL != pCbSAddr )		*pCbSAddr = (((U32)pRegister->DECI_CBSEG	)<<24)
											| (((U32)pRegister->DECI_CBTOP	)<<12)
											| (((U32)pRegister->DECI_CBLEFT	)<< 0);
	if( CNULL != pCbEAddr )		*pCbEAddr = (((U32)pRegister->DECI_CBSEG	)<<24)
											| (((U32)pRegister->DECI_CBBOTTOM)<<12)
											| (((U32)pRegister->DECI_CBRIGHT )<< 0);
	if( CNULL != pCrSAddr )		*pCrSAddr = (((U32)pRegister->DECI_CRSEG	)<<24)
											| (((U32)pRegister->DECI_CRTOP	)<<12)
											| (((U32)pRegister->DECI_CRLEFT	)<< 0);
	if( CNULL != pCrEAddr )		*pCrEAddr = (((U32)pRegister->DECI_CRSEG	)<<24)
											| (((U32)pRegister->DECI_CRBOTTOM)<<12)
											| (((U32)pRegister->DECI_CRRIGHT )<< 0);
}
