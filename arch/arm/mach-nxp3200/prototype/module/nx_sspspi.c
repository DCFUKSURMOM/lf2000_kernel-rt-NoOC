//------------------------------------------------------------------------------
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: SSPSPI
//	File		: nx_sspspi.c
//	Description	:
//	Author		:
//	History		:
//------------------------------------------------------------------------------

#include "nx_sspspi.h"

//------------------------------------------------------------------------------
// Internal global varialbles
//------------------------------------------------------------------------------
static	struct
{
	struct NX_SSPSPI_RegisterSet *pRegister;
} __g_ModuleVariables[NUMBER_OF_SSPSPI_MODULE] = { {CNULL,}, };


//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize prototype environment and local variables.
 *	@return		CTRUE	indicates success.\n
 *				CFALSE	indicates failure.
 *	@remark		You have to call this function before using other functions.
 *	@see		NX_SSPSPI_GetNumberOfModule
 */
CBOOL	NX_SSPSPI_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_SSPSPI_MODULE; i++ )
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
 *	@return		number of module in the chip.
 *	@see		NX_SSPSPI_Initialize
 */
U32		NX_SSPSPI_GetNumberOfModule( void )
{
	return NUMBER_OF_SSPSPI_MODULE;
}


//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a physical address of mudule.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a physical address of module.
 *	@see		NX_SSPSPI_GetSizeOfRegisterSet,
 *				NX_SSPSPI_SetBaseAddress,		NX_SSPSPI_GetBaseAddress,
 *				NX_SSPSPI_OpenModule,			NX_SSPSPI_CloseModule,
 *				NX_SSPSPI_CheckBusy,			NX_SSPSPI_CanPowerDown
 */
U32		NX_SSPSPI_GetPhysicalAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	return	(U32)( PHY_BASEADDR_SSPSPI_MODULE + (OFFSET_OF_SSPSPI_MODULE * ModuleIndex) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in bytes, of register set.
 *	@return		Byte size of module's register set.
 *	@see		NX_SSPSPI_GetPhysicalAddress,
 *				NX_SSPSPI_SetBaseAddress,		NX_SSPSPI_GetBaseAddress,
 *				NX_SSPSPI_OpenModule,			NX_SSPSPI_CloseModule,
 *				NX_SSPSPI_CheckBusy,			NX_SSPSPI_CanPowerDown
 */
U32		NX_SSPSPI_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_SSPSPI_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	BaseAddress	Module's base address
 *	@return		None.
 *	@remark		You have to call this function before using NX_SSPSPI_OpenModule(),
 *				NX_SSPSPI_CloseModule(), NX_SSPSPI_CheckBusy(), NX_SSPSPI_CanPowerDown(),
 *				Interrupt Interfaces, DMA Interfaces, Clock Control Interfaces,
 *				and Module Specific Functions.\n
 *				You can use this function with:\n
 *				- virtual address system such as WinCE or Linux.
 *	@code
 *		U32 PhyAddr, VirAddr;
 *		PhyAddr = NX_SSPSPI_GetPhysicalAddress( index );
 *		VirAddr = MEMORY_MAP_FUNCTION_TO_VIRTUAL( PhyAddr, ... );
 *		NX_SSPSPI_SetBaseAddress( index, VirAddr );
 *	@endcode
 *				- physical address system such as Non-OS.
 *	@code
 *		NX_SSPSPI_SetBaseAddress( index, NX_SSPSPI_GetPhysicalAddress( index ) );
 *	@endcode
 *
 *	@see		NX_SSPSPI_GetPhysicalAddress,	NX_SSPSPI_GetSizeOfRegisterSet,
 *				NX_SSPSPI_GetBaseAddress,
 *				NX_SSPSPI_OpenModule,			NX_SSPSPI_CloseModule,
 *				NX_SSPSPI_CheckBusy,			NX_SSPSPI_CanPowerDown
 */
void	NX_SSPSPI_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	__g_ModuleVariables[ModuleIndex].pRegister = (struct NX_SSPSPI_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Module's base address.
 *	@see		NX_SSPSPI_GetPhysicalAddress,	NX_SSPSPI_GetSizeOfRegisterSet,
 *				NX_SSPSPI_SetBaseAddress,
 *				NX_SSPSPI_OpenModule,			NX_SSPSPI_CloseModule,
 *				NX_SSPSPI_CheckBusy,			NX_SSPSPI_CanPowerDown
 */
U32		NX_SSPSPI_GetBaseAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates success.\n
 *				CFALSE	indicates failure.
 *	@see		NX_SSPSPI_GetPhysicalAddress,	NX_SSPSPI_GetSizeOfRegisterSet,
 *				NX_SSPSPI_SetBaseAddress,		NX_SSPSPI_GetBaseAddress,
 *				NX_SSPSPI_CloseModule,
 *				NX_SSPSPI_CheckBusy,			NX_SSPSPI_CanPowerDown
 */
CBOOL	NX_SSPSPI_OpenModule( U32 ModuleIndex )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		De-initialize selected module to the proper stage.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates success.\n
 *				CFALSE	indicates failure.
 *	@see		NX_SSPSPI_GetPhysicalAddress,	NX_SSPSPI_GetSizeOfRegisterSet,
 *				NX_SSPSPI_SetBaseAddress,		NX_SSPSPI_GetBaseAddress,
 *				NX_SSPSPI_OpenModule,
 *				NX_SSPSPI_CheckBusy,			NX_SSPSPI_CanPowerDown
 */
CBOOL	NX_SSPSPI_CloseModule( U32 ModuleIndex )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates the selected module is busy.\n
 *				CFALSE	indicates the selected module is idle.
 *	@see		NX_SSPSPI_GetPhysicalAddress,	NX_SSPSPI_GetSizeOfRegisterSet,
 *				NX_SSPSPI_SetBaseAddress,		NX_SSPSPI_GetBaseAddress,
 *				NX_SSPSPI_OpenModule,			NX_SSPSPI_CloseModule,
 *				NX_SSPSPI_CanPowerDown
 */
CBOOL	NX_SSPSPI_CheckBusy( U32 ModuleIndex )
{
	if( CTRUE == NX_SSPSPI_GetEnable(ModuleIndex) )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE indicates the selected module is ready to enter power-down stage.\n
 *				CFALSE indicates the selected module is busy and not ready to enter power-down stage.
 *	@see		NX_SSPSPI_GetPhysicalAddress,	NX_SSPSPI_GetSizeOfRegisterSet,
 *				NX_SSPSPI_SetBaseAddress,		NX_SSPSPI_GetBaseAddress,
 *				NX_SSPSPI_OpenModule,			NX_SSPSPI_CloseModule,
 *				NX_SSPSPI_CheckBusy
 */
CBOOL	NX_SSPSPI_CanPowerDown( U32 ModuleIndex )
{
	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt number for the interrupt controller.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		An interrupt number.
 *	@remark		Return value can be used for the interrupt controller module's
 *				functions.
 *	@see												NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
S32		NX_SSPSPI_GetInterruptNumber( U32 ModuleIndex )
{
	const U32	SSPSPIInterruptNumber[NUMBER_OF_SSPSPI_MODULE] =
				{ INTNUM_OF_SSPSPI0_MODULE };

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	return	SSPSPIInterruptNumber[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Specifies an interrupt number which is one of @ref NX_SSPSPI_INT enum.
 *	@param[in]	bEnb			Set it as CTRUE to enable an interrupt specified by @a IntNum. \n
 *								Set it as CFALSE to disable an interrupt specified by @a IntNum.
 *	@return		None.
 *	@see		NX_SSPSPI_GetInterruptNumber,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
void	NX_SSPSPI_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL bEnb )
{
	const U32	PEND_MASK = 7<<4;

	register struct NX_SSPSPI_RegisterSet* pRegister;
	register U32	shift;
	register U32	ReadValue;

	NX_ASSERT( 3 > IntNum );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	shift = IntNum + 13;

	ReadValue = ReadIOW(&pRegister->STATE);
	ReadValue &= ~(PEND_MASK | (1UL << shift));
	ReadValue |= (U32)bEnb << shift;

	WriteIOW(&pRegister->STATE, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Specifies an interrupt number which is one of @ref NX_SSPSPI_INT enum.
 *	@return		CTRUE	indicates an interrupt specified by @a IntNum is enabled.\n
 *				CFALSE	indicates an interrupt specified by @a IntNum is disabled.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *														NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
CBOOL	NX_SSPSPI_GetInterruptEnable( U32 ModuleIndex, S32 IntNum )
{
	register U32	shift;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 3 > IntNum );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	shift = IntNum + 13;
	return (CBOOL)((pRegister->STATE >> shift) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set NX_SSPSPI_INT_RX	interrupt enable or disable. \n
 *							- EnableFlag[1] : Set NX_SSPSPI_INT_TX	interrupt enable or disable. \n
 *							- EnableFlag[2] : Set NX_SSPSPI_INT_END interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
void	NX_SSPSPI_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag )
{
	const U32	INTENB_BITPOS	= 13;
	const U32	INTENB_MASK		= 0x07 << INTENB_BITPOS;
	const U32	PEND_MASK		= 7<<4;

	register struct NX_SSPSPI_RegisterSet* pRegister;
	register U32	ReadValue;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue = ReadIOW(&pRegister->STATE);
	ReadValue &= ~(PEND_MASK | INTENB_MASK);
	ReadValue |= (U32)(EnableFlag & 0x07) << INTENB_BITPOS;

	WriteIOW(&pRegister->STATE, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : NX_SSPSPI_INT_RX	interrupt's setting value. \n
 *				- Return Value[1] : NX_SSPSPI_INT_TX	interrupt's setting value. \n
 *				- Return Value[2] : NX_SSPSPI_INT_END interrupt's setting value. \n
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *														NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
U32		NX_SSPSPI_GetInterruptEnable32( U32 ModuleIndex )
{
	const U32 INTENB_BITPOS	= 13;
	const U32 INTENB_MASK	= 0x07 << INTENB_BITPOS;
	
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return (CBOOL)((ReadIOW(&pRegister->STATE) & INTENB_MASK) >> INTENB_BITPOS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Specifies an interrupt number which is one of @ref NX_SSPSPI_INT enum.
 *	@return		CTRUE	indicates an interrupt specified by @a IntNum is pended.\n
 *				CFALSE	indicates an interrupt specified by @a IntNum is not pended.
 *	@remark		The interrupt pending status are logged regardless of interrupt
 *				enable status. Therefore the return value can be CTRUE even
 *				though the specified interrupt has been disabled.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
CBOOL	NX_SSPSPI_GetInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	register U32	shift;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 3 > IntNum );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	shift = IntNum + 4;
	return (CBOOL)((ReadIOW(&pRegister->STATE) >> shift) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- PendingFlag[0] : NX_SSPSPI_INT_RX pending bit. \n
 *				- PendingFlag[1] : NX_SSPSPI_INT_TX pending bit. \n
 *				- PendingFlag[2] : NX_SSPSPI_INT_END pending bit. \n
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *														NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
U32		NX_SSPSPI_GetInterruptPending32( U32 ModuleIndex )
{
	const U32	PEND_BITPOS	= 4;
	const U32	PEND_MASK	= 7<<PEND_BITPOS;

	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return (U32)((ReadIOW(&pRegister->STATE) & PEND_MASK) >> PEND_BITPOS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Specifies an interrupt number which is one of @ref NX_SSPSPI_INT enum.
 *	@return		None.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
void	NX_SSPSPI_ClearInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	const U32	PEND_MASK = 7<<4;

	register struct NX_SSPSPI_RegisterSet* pRegister;
	register U32	shift;
	register U32	ReadValue;

	NX_ASSERT( 3 > IntNum );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	shift = IntNum + 4;

	ReadValue = ReadIOW(&pRegister->STATE) & ~PEND_MASK;
	ReadValue |= 1UL << shift;
	WriteIOW(&pRegister->STATE, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : NX_SSPSPI_INT_RX pending bit. \n
 *								- PendingFlag[1] : NX_SSPSPI_INT_TX pending bit. \n
 *								- PendingFlag[2] : NX_SSPSPI_INT_END pending bit. \n
 *	@return		None.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *														NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
void	NX_SSPSPI_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag )
{
	const U32	PEND_BITPOS	= 4;
	const U32	PEND_MASK	= 7<<PEND_BITPOS;

	register struct NX_SSPSPI_RegisterSet* pRegister;
	register U32	ReadValue;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue = ReadIOW(&pRegister->STATE) & ~PEND_MASK;
	ReadValue |= (PendingFlag & 0x07) << PEND_BITPOS;
	WriteIOW(&pRegister->STATE, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnb			Set it as CTRUE to enable all interrupts. \n
 *								Set it as CFALSE to disable all interrupts.
 *	@return		None.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
void	NX_SSPSPI_SetInterruptEnableAll( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 IRQ_MASK = 7<<13;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( (0==bEnb) ||(1==bEnb) );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if( bEnb )
	{
		WriteIOW(&pRegister->STATE, IRQ_MASK);
	}
	else
	{
		WriteIOW(&pRegister->STATE, 0);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTURE	indicates there's interrupts which are enabled.\n
 *				CFALSE	indicates there's no interrupt which are enabled.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *														NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
CBOOL	NX_SSPSPI_GetInterruptEnableAll( U32 ModuleIndex )
{
	const U32 IRQ_MASK = 7<<13;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if( ReadIOW(&pRegister->STATE) & IRQ_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTURE	indicates there's interrupts which are pended.\n
 *				CFALSE	indicates there's no interrupt which are pended.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		
 *				NX_SSPSPI_ClearInterruptPendingAll,		NX_SSPSPI_GetInterruptPendingNumber
 */
CBOOL	NX_SSPSPI_GetInterruptPendingAll( U32 ModuleIndex )
{
	const U32 PEND_MASK = 7<<4;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if( ReadIOW(&pRegister->STATE) & PEND_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *														NX_SSPSPI_GetInterruptPendingNumber
 */
void	NX_SSPSPI_ClearInterruptPendingAll( U32 ModuleIndex )
{
	const U32	PEND_MASK = 7<<4;
	register struct NX_SSPSPI_RegisterSet* pRegister;
	register U32	ReadValue;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue = ReadIOW(&pRegister->STATE);
	ReadValue |= PEND_MASK;
	WriteIOW(&pRegister->STATE, (U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt number which has the most prority of pended interrupts.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		an interrupt number which has the most priority of pended interrupts. \n
 *				This value is one of @ref NX_SSPSPI_INT enum.
 *				If there's no interrupt which is pended and unmasked, it returns -1.
 *	@see		NX_SSPSPI_GetInterruptNumber,			NX_SSPSPI_SetInterruptEnable,
 *				NX_SSPSPI_GetInterruptEnable,			NX_SSPSPI_SetInterruptEnable32,
 *				NX_SSPSPI_GetInterruptEnable32,			NX_SSPSPI_GetInterruptPending,
 *				NX_SSPSPI_GetInterruptPending32,		NX_SSPSPI_ClearInterruptPending,
 *				NX_SSPSPI_ClearInterruptPending32,		NX_SSPSPI_SetInterruptEnableAll,
 *				NX_SSPSPI_GetInterruptEnableAll,		NX_SSPSPI_GetInterruptPendingAll,
 *				NX_SSPSPI_ClearInterruptPendingAll	
 */
S32		NX_SSPSPI_GetInterruptPendingNumber( U32 ModuleIndex )	// -1 if None
{
	const U32 IRQR_MASK		= 1UL << 4;
	const U32 IRQW_MASK		= 1UL << 5;
	const U32 IRQE_MASK		= 1UL << 6;

	register U32 Pend;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	Pend	=	(ReadIOW(&pRegister->STATE) & (ReadIOW(&pRegister->STATE) >> 9));

	if( Pend & IRQR_MASK )
	{
		return 0;
	}
	else if( Pend & IRQW_MASK )
	{
		return 1;
	}
	else if( Pend & IRQE_MASK )
	{
		return 2;
	}

	return -1;
}


//------------------------------------------------------------------------------
// DMA Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA peripheral index for SSP/SPI Tx.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		DMA peripheral index for SSP/SPI Tx.
 *	@see		NX_SSPSPI_GetDMAIndex_Rx, NX_SSPSPI_GetDMABusWidth
 */
U32		NX_SSPSPI_GetDMAIndex_Tx( U32 ModuleIndex )
{
	const U32	SspspiDmaIndexTx[NUMBER_OF_SSPSPI_MODULE] =
		{ DMAINDEX_OF_SSPSPI0_MODULE_TX };

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	return SspspiDmaIndexTx[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA peripheral index for SSP/SPI Rx.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		DMA peripheral index for SSP/SPI Rx.
 *	@see		NX_SSPSPI_GetDMAIndex_Tx, NX_SSPSPI_GetDMABusWidth
 */
U32		NX_SSPSPI_GetDMAIndex_Rx( U32 ModuleIndex )
{
	const U32	SspspiDmaIndexRx[NUMBER_OF_SSPSPI_MODULE] =
		{ DMAINDEX_OF_SSPSPI0_MODULE_RX };

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );

	return SspspiDmaIndexRx[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA bus width of SSP/SPI.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		DMA bus width of SSP/SPI.\n.
 *				It returns 8 in case of the data bit width is between 1 and 8,\n
 *				16 in case of the data bit width is between 9 and 16.\n.
 *	@remark		You have to call NX_SSPSPI_SetBitWidth() to set the data bit
 *				width properly before using	this function.
 *	@see		NX_SSPSPI_GetDMAIndex_Tx, NX_SSPSPI_GetDMAIndex_Rx
 */
U32		NX_SSPSPI_GetDMABusWidth( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	NX_ASSERT(NX_SSPSPI_GetBitWidth(ModuleIndex)<=16);

	if( 9 <= NX_SSPSPI_GetBitWidth(ModuleIndex) )
	{
		return 16;
	}
	else
	{
		return 8;
	}
}


//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	mode			Specifies a PCLK mode to change.
 *	@return		None.
 *	@remark		SSPSPI doesn't support NX_PCLKMODE_DYNAMIC. If you call this
 *				function with NX_PCLKMODE_DYNAMIC, it makes that PCLK doesn't
 *				provide to the SSPSPI module and SSPSPI module doesn't operate
 *				correctly. You have to set a PCLK mode as NX_PCLKMODE_ALWAYS
 *				to operate and control the SSPSPI module. But you can set a PCLK
 *				mode as NX_PCLKMODE_DYNAMIC to slightly reduce a power
 *				cunsumption if you don't want to use the SSPSPI module.
 *	@see		NX_PCLKMODE,
 *				NX_SSPSPI_GetClockPClkMode,
 *				NX_SSPSPI_SetClockSource,			NX_SSPSPI_GetClockSource,
 *				NX_SSPSPI_SetClockDivisor,			NX_SSPSPI_GetClockDivisor,
 *				NX_SSPSPI_SetClockDivisorEnable,	NX_SSPSPI_GetClockDivisorEnable
 */
void			NX_SSPSPI_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode )
{
	const U32 PCLKMODE	=	1UL<<3;
	register U32 regvalue;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	NX_ASSERT( (NX_PCLKMODE_ALWAYS==mode) || (NX_PCLKMODE_DYNAMIC==mode) );

	regvalue = ReadIODW(&pRegister->CLKENB);
	if( NX_PCLKMODE_ALWAYS==mode )		regvalue |=	PCLKMODE;
	else								regvalue &= ~PCLKMODE;
	WriteIODW(&pRegister->CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current PCLK mode.
 *	@see		NX_PCLKMODE,
 *				NX_SSPSPI_SetClockPClkMode,
 *				NX_SSPSPI_SetClockSource,			NX_SSPSPI_GetClockSource,
 *				NX_SSPSPI_SetClockDivisor,			NX_SSPSPI_GetClockDivisor,
 *				NX_SSPSPI_SetClockDivisorEnable,	NX_SSPSPI_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_SSPSPI_GetClockPClkMode( U32 ModuleIndex )
{
	const U32 PCLKMODE	=	1UL<<3;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if( ReadIODW(&pRegister->CLKENB) & PCLKMODE )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock source of specified clock generator
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			Specifies an index of the clock generator( 0 ).
 *	@param[in]	ClkSrc			Specifies a clock source( 0:PLL0, 1:PLL1 ).
 *	@remark		Since the SSPSPI module has one clock generator, you have to set
 *				\e Index as '0' only.\n
 *				The SSPSPI module has two clock source - 0 : PLL0, 1 : PLL1.
 *	@return		None.
 *	@see		NX_SSPSPI_SetClockPClkMode,			NX_SSPSPI_GetClockPClkMode,
 *				NX_SSPSPI_GetClockSource,
 *				NX_SSPSPI_SetClockDivisor,			NX_SSPSPI_GetClockDivisor,
 *				NX_SSPSPI_SetClockDivisorEnable,	NX_SSPSPI_GetClockDivisorEnable
 */
void	NX_SSPSPI_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register U32 ReadValue;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( 2 > ClkSrc );

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue = ReadIODW(&pRegister->CLKGEN);
	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;
	WriteIODW(&pRegister->CLKGEN, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a clock source of specified clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			Specifies an index of the clock generator( 0 ).
 *	@return		A clock source of specified clock generator( 0:PLL0, 1:PLL1 ).
 *	@remark		Since the SSPSPI module has one clock generator, you have to set
 *				\e Index as '0' only.\n
 *				The SSPSPI module has two clock source - 0 : PLL0, 1 : PLL1.
 *	@see		NX_SSPSPI_SetClockPClkMode,			NX_SSPSPI_GetClockPClkMode,
 *				NX_SSPSPI_SetClockSource,
 *				NX_SSPSPI_SetClockDivisor,			NX_SSPSPI_GetClockDivisor,
 *				NX_SSPSPI_SetClockDivisorEnable,	NX_SSPSPI_GetClockDivisorEnable
 */
U32				NX_SSPSPI_GetClockSource( U32 ModuleIndex, U32 Index )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 0 == Index );

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return (ReadIODW(&pRegister->CLKGEN) & CLKSRCSEL_MASK) >> CLKSRCSEL_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock divider of specified clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			Specifies an index of the clock generator( 0 ).
 *	@param[in]	Divider			Specifies a clock divider( 1 ~ 256 ).
 *	@return		None.
 *	@remark		Since the SSPSPI module has one clock generator, you have to set
 *				\e Index as '0' only.\n
 *				The source clock can be calcurated by following formula:
 *				- the source clock = Clock Source / Divider, where Divider is between
 *				1 and 64.
 *	@remark		\b NOTE : The output clock of the clock generator must be less
 *				than or equal to 100 Mhz.
 *	@see		NX_SSPSPI_SetClockPClkMode,			NX_SSPSPI_GetClockPClkMode,
 *				NX_SSPSPI_SetClockSource,			NX_SSPSPI_GetClockSource,
 *				NX_SSPSPI_GetClockDivisor,
 *				NX_SSPSPI_SetClockDivisorEnable,	NX_SSPSPI_GetClockDivisorEnable
 */
void	NX_SSPSPI_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divider )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	register U32 ReadValue;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( 1 <= Divider && Divider <= 256 );

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue	=	ReadIODW(&pRegister->CLKGEN);
	ReadValue	&= ~CLKDIV_MASK;
	ReadValue	|= (Divider-1) << CLKDIV_POS;
	WriteIODW(&pRegister->CLKGEN, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a clock divider of specified clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			Specifies an index of the clock generator( 0 ).
 *	@return		A clock divider of specified clock generator( 1 ~ 256 ).
 *	@remark		Since the SSPSPI module has one clock generator, you have to set
 *				\e Index as '0' only.
 *	@see		NX_SSPSPI_SetClockPClkMode,			NX_SSPSPI_GetClockPClkMode,
 *				NX_SSPSPI_SetClockSource,			NX_SSPSPI_GetClockSource,
 *				NX_SSPSPI_SetClockDivisor,
 *				NX_SSPSPI_SetClockDivisorEnable,	NX_SSPSPI_GetClockDivisorEnable
 */
U32				NX_SSPSPI_GetClockDivisor( U32 ModuleIndex, U32 Index )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 0 == Index );

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return ((ReadIODW(&pRegister->CLKGEN) & CLKDIV_MASK) >> CLKDIV_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock generator to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnb			Set it as CTRUE to enable a clock generator. \n
 *								Set it as CFALSE to disable a clock generator.
 *	@return		None.
 *	@remark		It makes that the clock generator provides a divided clock for
 *				the module to call this function with CTRUE.
 *	@see		NX_SSPSPI_SetClockPClkMode,			NX_SSPSPI_GetClockPClkMode,
 *				NX_SSPSPI_SetClockSource,			NX_SSPSPI_GetClockSource,
 *				NX_SSPSPI_SetClockDivisor,			NX_SSPSPI_GetClockDivisor,
 *				NX_SSPSPI_GetClockDivisorEnable
 */
void	NX_SSPSPI_SetClockDivisorEnable( U32 ModuleIndex, CBOOL bEnb )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register U32 ReadValue;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( (0==bEnb) ||(1==bEnb) );

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue	=	ReadIODW(&pRegister->CLKENB);
	ReadValue	&=	~CLKGENENB_MASK;
	ReadValue	|= (U32)bEnb << CLKGENENB_POS;
	WriteIODW(&pRegister->CLKENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the clock generator is enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates the clock generator is enabled and provides a divided clock for the module.\n
 *				CFALSE	indicates the clock generator is disabled and doesn't provied a divided clock for the module.
 *	@see		NX_SSPSPI_SetClockPClkMode,			NX_SSPSPI_GetClockPClkMode,
 *				NX_SSPSPI_SetClockSource,			NX_SSPSPI_GetClockSource,
 *				NX_SSPSPI_SetClockDivisor,			NX_SSPSPI_GetClockDivisor,
 *				NX_SSPSPI_SetClockDivisorEnable
 */
CBOOL			NX_SSPSPI_GetClockDivisorEnable( U32 ModuleIndex )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return	(CBOOL)( (ReadIODW(&pRegister->CLKENB) & CLKGENENB_MASK) >> CLKGENENB_POS );
}

//------------------------------------------------------------------------------
// Configuration Functions
//------------------------------------------------------------------------------
/**
 *	@brief		Set Transfer Mode ( DMA mode or PIO mode )
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bDMA	\b CTRUE indicate that DMA mode. \n
 *						\b CFALSE indicate that PIO mode.
 *	@return		None.
 *	@see											NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
void	NX_SSPSPI_SetDMATransferMode( U32 ModuleIndex, CBOOL bDMA )
{
	const U32	DMAENB_POS	= 12;
	const U32	DMAENB_MASK	= 1UL << DMAENB_POS;

	register U32	temp;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );
	NX_ASSERT( (0==bDMA) || (1==bDMA) );

	temp = ReadIOW(&pRegister->CONT0);
	temp &= ~DMAENB_MASK;
	temp |= (U32)bDMA << DMAENB_POS;
	WriteIOW(&pRegister->CONT0, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Transfer Mode ( DMA mode or PIO mode )
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE indicate that DMA mode. \n
 *				\b CFALSE indicate that PIO mode.
 *	@see		NX_SSPSPI_SetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
CBOOL	NX_SSPSPI_GetDMATransferMode( U32 ModuleIndex )
{
	const U32	DMAENB_POS	= 12;
	const U32	DMAENB_MASK	= 1UL << DMAENB_POS;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return	(CBOOL)((ReadIOW(&pRegister->CONT0) & DMAENB_MASK) >> DMAENB_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Clock Source of SSP/SPI
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bExt	\b CTRUE indicate that External clock. \n
 *						\b CFALSE indicate that Internal clock. \n
 *	@return		None.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *													NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
void	NX_SSPSPI_SetExternalClockSource( U32 ModuleIndex, CBOOL bExt )
{
	const U32	EXTCLKSEL_BITPOS	= 9;
	const U32	EXTCLKSEL_MASK		= ( 0x01 << EXTCLKSEL_BITPOS );

	register U32	temp;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );
	NX_ASSERT( (0==bExt) || (1==bExt) );

	temp = ReadIOW(&pRegister->CONT0);

	if( bExt )
	{
		temp |= EXTCLKSEL_MASK;
	}
	else
	{
		temp &= ~EXTCLKSEL_MASK;
	}

	WriteIOW(&pRegister->CONT0, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Clock Source of SSP/SPI
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE indicate that External clock. \n
 *				\b CFALSE indicate that Internal clock.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
CBOOL	NX_SSPSPI_GetExternalClockSource( U32 ModuleIndex )
{
	const U32	EXTCLKSEL_BITPOS	= 9;
	const U32	EXTCLKSEL_MASK		= ( 0x01 << EXTCLKSEL_BITPOS );
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if( ReadIOW(&pRegister->CONT0) & EXTCLKSEL_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Data bit width
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bitWidth	Value of Data Bit Width( 1 ~ 16 )
 *	@return		None.
 *	@remarks	NX_SSPSPI_SetBitWidth() must invoke before NX_SSPSPI_GetDMABusWidth()
 *	@code
 *				NX_SSPSPI_SetBitWidth();
 *				NX_SSPSPI_GetDMABusWidth();
 *	@endcode
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *													NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
void	NX_SSPSPI_SetBitWidth( U32 ModuleIndex, U32 bitWidth )
{
	const U32	NUMBIT_POS		= 5;
	const U32	NUMBIT_MASK		= ( 0x0F << NUMBIT_POS );

	register U32	temp;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );
	NX_ASSERT( 1 <= bitWidth && bitWidth <= 16 );

	temp = ReadIOW(&pRegister->CONT0);
	temp &= ~NUMBIT_MASK;
	temp |=	(bitWidth-1) << NUMBIT_POS ;
	WriteIOW(&pRegister->CONT0, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Data Bit Width
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Data Bit Width( 1 ~ 16 )
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
U32		NX_SSPSPI_GetBitWidth( U32 ModuleIndex )
{
	const U32	NUMBIT_POS		= 5;
	const U32	NUMBIT_MASK		= ( 0x0F << NUMBIT_POS );
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return(U32) (((ReadIOW(&pRegister->CONT0) & NUMBIT_MASK ) >> NUMBIT_POS) + 1 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Divide Value
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	dividerCNT	Divide Value ( 4 ~ 32 )
 *	@return		None.
 *	@remarks	Recommand Even Value ( 4, 6 ~ ).\n
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *													NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
void	NX_SSPSPI_SetDividerCount( U32 ModuleIndex, U32 dividerCNT )
{
	const U32	DIVCNT_MASK		= 0x1F;
	register U32	temp;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );
	NX_ASSERT( ( 32 >= dividerCNT ) && ( 4 <= dividerCNT ) );

	temp = ReadIOW(&pRegister->CONT0);
	temp &= ~DIVCNT_MASK;
	temp |= (dividerCNT-1);
	WriteIOW(&pRegister->CONT0, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Divide Value
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Divide Value
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
U32		NX_SSPSPI_GetDividerCount( U32 ModuleIndex )
{
	const U32	DIVCNT_MASK		= 0x1F;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return(U32)( ReadIOW(&pRegister->CONT0) & DIVCNT_MASK )+1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set location of data byte in 16 bit FIFO.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bSwap	\b CTRUE indicate that Byte swap Enable.\n
 *						\b CFALSE indicate that Byte swap Disable.
 *	@return		None.
 *	@remarks	It allows using bandwidth more efficiently in 8bit reception device.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *													NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
void	NX_SSPSPI_SetByteSwap( U32 ModuleIndex, CBOOL bSwap )
{
	const U32	BYTE_SWAP_POS	= 5;
	const U32	BYTE_SWAP_MASK	= 1UL << BYTE_SWAP_POS;
	register U32	ReadValue;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( (0==bSwap) || (1==bSwap) );

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue	=	ReadIOW(&pRegister->CONT1);
	ReadValue	&= ~BYTE_SWAP_MASK;
	ReadValue	|= (U32)bSwap << BYTE_SWAP_POS;
	WriteIOW(&pRegister->CONT1,	(U16)ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of byte swap is enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRU indicate that Byte swap is enabled.\n
 *				\b CFALSE indicate that Byte swap is disabled.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
CBOOL	NX_SSPSPI_GetByteSwap( U32 ModuleIndex )
{
	const U32	BYTE_SWAP_POS	= 5;
	const U32	BYTE_SWAP_MASK	= 1UL << BYTE_SWAP_POS;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return (CBOOL)((ReadIOW(&pRegister->CONT1) & BYTE_SWAP_MASK) >> BYTE_SWAP_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Operation Mode ( Master or Slave )
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bSlave	\b CTRIE indicate that Slave mode enable. \n
 *						\b CFALSE indicate that Master mode enable.
 *	@return		None.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *													NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
void	NX_SSPSPI_SetSlaveMode( U32 ModuleIndex, CBOOL bSlave )
{
	const U32	SLAVE_SEL_POS	= 4;
	const U32	SLAVE_SEL_MASK	= ( 0x01 << SLAVE_SEL_POS );
	register U32	temp;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( (0==bSlave) || (1==bSlave) );

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	temp = ReadIOW(&pRegister->CONT1);

	if( bSlave )
	{
		temp	|= SLAVE_SEL_MASK;
	}
	else
	{
		temp	&= ~SLAVE_SEL_MASK;
	}

	WriteIOW(&pRegister->CONT1, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Operation Mode ( Master or Slave )
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRIE indicate that Slave mode enable. \n
 *				\b CFALSE indicate that Master mode enable.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
CBOOL	NX_SSPSPI_GetSlaveMode( U32 ModuleIndex )
{
	const U32	SLAVE_SEL_POS	= 4;
	const U32	SLAVE_SEL_MASK	= ( 0x01 << SLAVE_SEL_POS );
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if( ReadIOW(&pRegister->CONT1) & SLAVE_SEL_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Polarity of SSPCLK
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bInvert	\b CTRUE indicate that SSPCLK Invert enable. \n
 *						\b CFALSE indicate that SSPCLK Invert disable. \n
 *	@return		None.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *													NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
void	NX_SSPSPI_SetClockPolarityInvert( U32 ModuleIndex, CBOOL bInvert )
{
	const U32	SCLKPOL_POS		= 3;
	const U32	SCLKPOL_MASK	= ( 0x01 << SCLKPOL_POS );
	register U32	temp;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( (0==bInvert) || (1==bInvert) );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	temp = ReadIOW(&pRegister->CONT1);

	if( bInvert )
	{
		temp	&= ~SCLKPOL_MASK;
	}
	else
	{
		temp	|= SCLKPOL_MASK;
	}

	WriteIOW(&pRegister->CONT1, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of SSPCLK's polarity is invert or normal.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that SSPCLK's polarity is invert. \n
 *				\b CFALSE	indicate that SSPCLK's polarity is normal.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
CBOOL	NX_SSPSPI_GetClockPolarityInvert( U32 ModuleIndex )
{
	const U32	SCLKPOL_POS		= 3;
	const U32	SCLKPOL_MASK	= ( 0x01 << SCLKPOL_POS );
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if( ReadIOW(&pRegister->CONT1) & SCLKPOL_MASK )
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set data size,in bytes, for burst receive mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	size	Byte size ( 1 ~ 65536 )
 *	@return		None.
 *	@remarks	This setting use to Rx Burst Receive in master mode of SPI's Format B protocol.
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *											NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
void	NX_SSPSPI_SetBurstRxSize( U32 ModuleIndex, U32 size )
{
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 1 <= size && size <= 65536 );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	WriteIOW(&pRegister->RXBUSTSIZE, (U16)size-1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get data size,in bytes, for burst receive mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Byte size ( 1 ~ 65536 )
 *	@remarks	This setting use to Rx Burst Receive in master mode of SPI's Format B protocol.
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
U32		NX_SSPSPI_GetBurstRxSize( U32 ModuleIndex )
{
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return (U32)ReadIOW(&pRegister->RXBUSTSIZE) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set SPI format
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	format		SPI format ( NX_SSPSPI_FORMAT_A or NX_SSPSPI_FORMAT_B )
 *	@return		None.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *													NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
void			NX_SSPSPI_SetSPIFormat( U32 ModuleIndex, NX_SSPSPI_FORMAT format)
{
	const U32	SCLKSH_POS	= 2;
	const U32	SCLKSH_MASK = ( 0x01 << SCLKSH_POS );
	register U32	temp;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 2 > format );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	temp = ReadIOW(&pRegister->CONT1);

	if( NX_SSPSPI_FORMAT_A == format )
	{
		temp &= ~SCLKSH_MASK;
	}
	else
	{
		temp |= SCLKSH_MASK;
	}

	WriteIOW(&pRegister->CONT1, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get SPI format
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		SPI format ( NX_SSPSPI_FORMAT_A or NX_SSPSPI_FORMAT_B )
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,
 *				NX_SSPSPI_SetProtocol,				NX_SSPSPI_GetProtocol
 */
NX_SSPSPI_FORMAT	NX_SSPSPI_GetSPIFormat( U32 ModuleIndex )
{
	const U32	SCLKSH_POS	= 2;
	const U32	SCLKSH_MASK = ( 0x01 << SCLKSH_POS );
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if(ReadIOW(&pRegister->CONT1) & SCLKSH_MASK )
	{
		return NX_SSPSPI_FORMAT_B;
	}

	return NX_SSPSPI_FORMAT_A;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set SSP/SPI Mode
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	protocol		Mode ( NX_SSPSPI_PROTOCOL_SSP or NX_SSPSPI_PROTOCOL_SPI )
 *	@return		None.
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *													NX_SSPSPI_GetProtocol
 */
void			NX_SSPSPI_SetProtocol( U32 ModuleIndex, NX_SSPSPI_PROTOCOL protocol )
{
	const U32	TYPE_POS		= 0;
	const U32	TYPE_MASK		= ( 0x03 << TYPE_POS );
	register U32	temp;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( 2 > protocol );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	temp = ReadIOW(&pRegister->CONT1);
	temp &= ~TYPE_MASK;

	if( NX_SSPSPI_PROTOCOL_SPI == protocol )
		temp |= 1<<TYPE_POS;

	WriteIOW(&pRegister->CONT1, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get SSP/SPI Mode
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Mode ( NX_SSPSPI_PROTOCOL_SSP or NX_SSPSPI_PROTOCOL_SPI )
 *	@see		NX_SSPSPI_SetDMATransferMode,		NX_SSPSPI_GetDMATransferMode,
 *				NX_SSPSPI_SetExternalClockSource,	NX_SSPSPI_GetExternalClockSource,
 *				NX_SSPSPI_SetBitWidth,				NX_SSPSPI_GetBitWidth,
 *				NX_SSPSPI_SetDividerCount,			NX_SSPSPI_GetDividerCount,
 *				NX_SSPSPI_SetByteSwap,				NX_SSPSPI_GetByteSwap,
 *				NX_SSPSPI_SetSlaveMode,				NX_SSPSPI_GetSlaveMode,
 *				NX_SSPSPI_SetClockPolarityInvert,	NX_SSPSPI_GetClockPolarityInvert,
 *				NX_SSPSPI_SetSPIFormat,				NX_SSPSPI_GetSPIFormat,
 *				NX_SSPSPI_SetProtocol
 */
NX_SSPSPI_PROTOCOL NX_SSPSPI_GetProtocol( U32 ModuleIndex )
{
	const U32	TYPE_POS		= 0;
	const U32	TYPE_MASK		= ( 0x03 << TYPE_POS );
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	if(ReadIOW(&pRegister->CONT1) & TYPE_MASK )
	{
		return NX_SSPSPI_PROTOCOL_SPI;
	}

	return NX_SSPSPI_PROTOCOL_SSP;
}


//------------------------------------------------------------------------------
// Operation Function
//------------------------------------------------------------------------------
/**
 *	@brief		FIFO Reset
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remarks	Reset TX & RX FIFO.
 *	@see									NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
void		NX_SSPSPI_ResetFIFO( U32 ModuleIndex )
{
	const U32	FFCLR_MASK		= ( 0x01 << 10 );

	register struct NX_SSPSPI_RegisterSet* pRegister;
	register U32	temp;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	pRegister	=	pRegister;

	temp = ReadIOW(&pRegister->CONT0);
	// IMPORTANT : You have to clear this bit after writing '1'.
	WriteIOW(&pRegister->CONT0, (U16)(temp | FFCLR_MASK));
	WriteIOW(&pRegister->CONT0, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Start RX Burst receive.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remarks	Start Rx Burst Receiving in master mode of SPI format B protocol.
 *	@code
 *			...
 *			NX_SSPSPI_SetSlaveMode( index, CFALSE);					// Master Mode
 *			NX_SSPSPI_SetProtocol( index, NX_SSPSPI_PROTOCOL_SPI );	// SPI Protocol
 *			NX_SSPSPI_SetSPIFormat( index, NX_SSPSPI_FORMAT_B );	// SPI Format B
 *			...
 *			NX_SSPSPI_SetBurstRxSize( index, n );					// Set N byte
 *			...
 *			NX_SSPSPI_SetEnable( index, CTRUE );					// SPI Enable
 *			NX_SSPSPI_SetRxBurstEnable( index, CTRUE );				// Burst Rx Enable
 *			NX_SSPSPI_StartRxBurst( index );						// Burst Rx Start
 *			...
 *	@endcode
 *	@see		NX_SSPSPI_ResetFIFO,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
void		NX_SSPSPI_StartRxBurst( U32 ModuleIndex )
{
	const	U32	ST_RXBRECEIVE_MASK	=	1UL << 15;
	register U32	ReadValue;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue	=	ReadIOW(&pRegister->CONT0);
	ReadValue	|= ST_RXBRECEIVE_MASK;
	WriteIOW(&pRegister->CONT0, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the burst receive mode to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnable			Set it as CTRUE to enable the burst receive mode.\n
 *								Set it as CFALSE to disable the burst receive mode.
 *	@return		None.
 *	@remarks	The burst receive mode can be used only for the master mode of SPI
 *				format B. It maintains the state of SSPFRM signal is low until
 *				the received data size reaches the burst receive size.
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *											NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
void		NX_SSPSPI_SetRxBurstEnable( U32 ModuleIndex, CBOOL bEnable )
{
	const U32 BRECEIVE_POS	= 13;
	const U32 BRECEIVE_MASK	= 1UL << BRECEIVE_POS;

	register struct NX_SSPSPI_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( (0==bEnable) ||(1==bEnable) );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue	=	ReadIOW(&pRegister->CONT0);
	ReadValue	&=	~BRECEIVE_MASK;
	ReadValue	|=	(U32)bEnable << BRECEIVE_POS;
	WriteIOW(&pRegister->CONT0, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the burst receive mode is enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicates the burst receive mode is enabled.\n
 *				\b CFALSE	indicates the burst receive mode is disabled.
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
CBOOL		NX_SSPSPI_GetRxBurstEnable( U32 ModuleIndex )
{
	const U32 BRECEIVE_POS	= 13;
	const U32 BRECEIVE_MASK	= 1UL << BRECEIVE_POS;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return	(CBOOL)( (ReadIOW(&pRegister->CONT0) & BRECEIVE_MASK) >> BRECEIVE_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set SSP/SPI enable or disable.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnable		\b CTRUE indicate that SSP/SPI Enable. \n
 *							\b CFALSE indicate that SSP/SPI Disable.
 *	@return		None.
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *											NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
void		NX_SSPSPI_SetEnable( U32 ModuleIndex, CBOOL bEnable )
{
	const U32 ENB_POS	= 11;
	const U32 ENB_MASK	= 1UL << ENB_POS;

	register struct NX_SSPSPI_RegisterSet* pRegister;
	register U32 ReadValue;

	NX_ASSERT( (0==bEnable) ||(1==bEnable) );
	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	ReadValue	=	ReadIOW(&pRegister->CONT0);
	ReadValue	&=	~ENB_MASK;
	ReadValue	|=	(U32)bEnable << ENB_POS;
	WriteIOW(&pRegister->CONT0, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of SSP/SPI is enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that SSP/SPI is enabled. \n
 *				\b CFALSE	indicate that SSP/SPI is disabled.
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
CBOOL		NX_SSPSPI_GetEnable( U32 ModuleIndex )
{
	const U32 ENB_POS	= 11;
	const U32 ENB_MASK	= 1UL << ENB_POS;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return	(CBOOL)( (ReadIOW(&pRegister->CONT1) & ENB_MASK) >> ENB_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Read Byte Data.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Byte Data
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *											NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
U8		NX_SSPSPI_GetByte( U32 ModuleIndex )
{
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );
	NX_ASSERT( 8 >= NX_SSPSPI_GetBitWidth(ModuleIndex) );

	return (U8)((ReadIOW(&pRegister->DATA)) & 0xff);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Read Half Word Data( 2byte ).
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Half Word Data( 2byte )
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,
 *				NX_SSPSPI_PutByte,			NX_SSPSPI_PutHalfWord
 */
U16		NX_SSPSPI_GetHalfWord( U32 ModuleIndex )
{
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );
	NX_ASSERT( 9 <= NX_SSPSPI_GetBitWidth(ModuleIndex) );

	return ( ReadIOW(&pRegister->DATA) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Send Byte.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	ByteData		Value of Data ( 0x0 ~ 0xFF )
 *	@return		None.
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutHalfWord
 */
void		NX_SSPSPI_PutByte(U32 ModuleIndex, U8 ByteData)
{
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );
	NX_ASSERT( 8 >= NX_SSPSPI_GetBitWidth(ModuleIndex) );

	WriteIOW(&pRegister->DATA, (U16)ByteData);	//put the byte data
}

//------------------------------------------------------------------------------
/**
 *	@brief		Send Half Word( 2byte ).
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	HalfWordData	Value of Data ( 0x0 ~ 0xFFFF )
 *	@return		None.
 *	@see		NX_SSPSPI_ResetFIFO,		NX_SSPSPI_StartRxBurst,
 *				NX_SSPSPI_SetRxBurstEnable, NX_SSPSPI_GetRxBurstEnable,
 *				NX_SSPSPI_SetBurstRxSize,	NX_SSPSPI_GetBurstRxSize,
 *				NX_SSPSPI_SetEnable,		NX_SSPSPI_GetEnable,
 *				NX_SSPSPI_GetByte,			NX_SSPSPI_GetHalfWord,
 *				NX_SSPSPI_PutByte
 */
void		NX_SSPSPI_PutHalfWord(U32 ModuleIndex, U16 HalfWordData)
{
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );
	NX_ASSERT( 9 <= NX_SSPSPI_GetBitWidth(ModuleIndex) );

	WriteIOW(&pRegister->DATA, HalfWordData);
}


//------------------------------------------------------------------------------
// FIFO State Check Function
//------------------------------------------------------------------------------
/**
 *	@brief		Check Tx Shift Register is Empty or Not
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	Indicate that Tx Shift Register is Empty.\n
 *				\b CFALSE	Indicate that Tx Shift Register is NOT Empty.
 *	@see		NX_SSPSPI_IsTxFIFOFull,		NX_SSPSPI_IsTxFIFOEmpty,
 *				NX_SSPSPI_IsRxFIFOFull,		NX_SSPSPI_IsRxFIFOEmpty,
 *				NX_SSPSPI_IsTxRxEnd,
 */
CBOOL		NX_SSPSPI_IsTxShiftEmpty( U32 ModuleIndex )
{
	const U32	TXSHIFTEREMPTY_POS = 8;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return (CBOOL)(( ReadIOW(&pRegister->STATE) >> TXSHIFTEREMPTY_POS ) & 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Tx FIFO is Full or Not
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	Indicate that FIFO Full.\n
 *				\b CFALSE	Indicate that FIFO NOT Full.
 *	@see		NX_SSPSPI_IsTxShiftEmpty,
 *				NX_SSPSPI_IsTxFIFOEmpty,
 *				NX_SSPSPI_IsRxFIFOFull,		NX_SSPSPI_IsRxFIFOEmpty,
 *				NX_SSPSPI_IsTxRxEnd,
 */
CBOOL		NX_SSPSPI_IsTxFIFOFull(U32 ModuleIndex)
{
	const U32	WFFFULL_POS = 3;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return(CBOOL)((ReadIOW(&pRegister->STATE) >> WFFFULL_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Tx FIFO is Empty or Not
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE Indicate that FIFO Empty.\n
 *				\b CFALSE Indicate that FIFO NOT Empty
 *	@see		NX_SSPSPI_IsTxShiftEmpty,
 *				NX_SSPSPI_IsTxFIFOFull,
 *				NX_SSPSPI_IsRxFIFOFull,		NX_SSPSPI_IsRxFIFOEmpty,
 *				NX_SSPSPI_IsTxRxEnd,
 */
CBOOL		NX_SSPSPI_IsTxFIFOEmpty(U32 ModuleIndex)
{
	const U32	WFFEMPTY_POS		= 2;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return(CBOOL)((ReadIOW(&pRegister->STATE) >> WFFEMPTY_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Rx FIFO is Full or Not
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	Indicate that FIFO Full.\n
 *				\b CFALSE	Indicate that FIFO NOT Full.
 *	@see		NX_SSPSPI_IsTxShiftEmpty,
 *				NX_SSPSPI_IsTxFIFOFull,		NX_SSPSPI_IsTxFIFOEmpty,
 *				NX_SSPSPI_IsRxFIFOEmpty,
 *				NX_SSPSPI_IsTxRxEnd,
 */
CBOOL		NX_SSPSPI_IsRxFIFOFull(U32 ModuleIndex)
{
	const U32	RFFFULL_POS = 1;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return(CBOOL)((ReadIOW(&pRegister->STATE) >> RFFFULL_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Rx FIFO is Empty or Not
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	Indicate that FIFO Empty.\n
 *				\b CFALSE	Indicate that FIFO NOT Empty.
 *	@see		NX_SSPSPI_IsTxShiftEmpty,
 *				NX_SSPSPI_IsTxFIFOFull,		NX_SSPSPI_IsTxFIFOEmpty,
 *				NX_SSPSPI_IsRxFIFOFull,
 *				NX_SSPSPI_IsTxRxEnd,
 */
CBOOL		NX_SSPSPI_IsRxFIFOEmpty(U32 ModuleIndex)
{
	const U32	RFFEMPTY_POS		= 0;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return(CBOOL)((ReadIOW(&pRegister->STATE) >> RFFEMPTY_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Tx/Rx is End or Not
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	Indicate that Tx/Rx is End.\n
 *				\b CFALSE	Indicate that Tx/Rx is NOT End.
 *	@remarks	This function is same to NX_SSPSPI_GetInterruptPending(2)
 *	@see		NX_SSPSPI_IsTxShiftEmpty,
 *				NX_SSPSPI_IsTxFIFOFull,		NX_SSPSPI_IsTxFIFOEmpty,
 *				NX_SSPSPI_IsRxFIFOFull,		NX_SSPSPI_IsRxFIFOEmpty
 */
CBOOL		NX_SSPSPI_IsTxRxEnd( U32 ModuleIndex )
{
	const U32	IRQE_POS = 6;
	register struct NX_SSPSPI_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SSPSPI_MODULE > ModuleIndex );
	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	NX_ASSERT( CNULL != pRegister );

	return(CBOOL)((ReadIOW(&pRegister->STATE) >> IRQE_POS ) & 0x01 );
}
