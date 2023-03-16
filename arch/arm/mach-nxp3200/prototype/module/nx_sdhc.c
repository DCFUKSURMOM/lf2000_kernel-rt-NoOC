//------------------------------------------------------------------------------
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: SDHC
//	File		: nx_sdhc.c
//	Description	:
//	Author		: Goofy
//	History		:
//------------------------------------------------------------------------------

#include "nx_sdhc.h"
#ifdef CONFIG_MMC_NEXELL_SDHC_MODULE
#include <linux/module.h>
#endif

//------------------------------------------------------------------------------
// Internal global varialbles
//------------------------------------------------------------------------------
static	struct
{
	struct NX_SDHC_RegisterSet *pRegister;
} __g_ModuleVariables[NUMBER_OF_SDHC_MODULE] = { {CNULL,}, };


//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize prototype environment and local variables.
 *	@return		CTRUE indicates success.\n
 *				CFALSE indicates failure.
 *	@remark		You have to call this function before using other functions.
 *	@see		NX_SDHC_GetNumberOfModule
 */
CBOOL	NX_SDHC_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_SDHC_MODULE; i++ )
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
 *	@see		NX_SDHC_Initialize
 */
U32		NX_SDHC_GetNumberOfModule( void )
{
	return NUMBER_OF_SDHC_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a physical address of mudule.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a physical address of module.
 *	@see		NX_SDHC_GetSizeOfRegisterSet,
 *				NX_SDHC_SetBaseAddress,		NX_SDHC_GetBaseAddress,
 *				NX_SDHC_OpenModule,			NX_SDHC_CloseModule,
 *				NX_SDHC_CheckBusy,			NX_SDHC_CanPowerDown
 */
U32		NX_SDHC_GetPhysicalAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );

	return	(U32)( PHY_BASEADDR_SDHC_MODULE + (OFFSET_OF_SDHC_MODULE * ModuleIndex) );
}
#ifdef CONFIG_MMC_NEXELL_SDHC_MODULE
EXPORT_SYMBOL(NX_SDHC_GetPhysicalAddress);
#endif

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in bytes, of register set.
 *	@return		Byte size of module's register set.
 *	@see		NX_SDHC_GetPhysicalAddress,
 *				NX_SDHC_SetBaseAddress,		NX_SDHC_GetBaseAddress,
 *				NX_SDHC_OpenModule,			NX_SDHC_CloseModule,
 *				NX_SDHC_CheckBusy,			NX_SDHC_CanPowerDown
 */
U32		NX_SDHC_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_SDHC_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	BaseAddress	Module's base address
 *	@return		None.
 *	@remark		You have to call this function before using NX_SDHC_OpenModule(),
 *				NX_SDHC_CloseModule(), NX_SDHC_CheckBusy(), NX_SDHC_CanPowerDown(),
 *				Interrupt Interfaces, DMA Interfaces, Clock Control Interfaces,
 *				and Module Specific Functions.\n
 *				You can use this function with:\n
 *				- virtual address system such as WinCE or Linux.
 *	@code
 *		U32 PhyAddr, VirAddr;
 *		PhyAddr = NX_SDHC_GetPhysicalAddress( index );
 *		VirAddr = MEMORY_MAP_FUNCTION_TO_VIRTUAL( PhyAddr, ... );
 *		NX_SDHC_SetBaseAddress( index, VirAddr );
 *	@endcode
 *				- physical address system such as Non-OS.
 *	@code
 *		NX_SDHC_SetBaseAddress( index, NX_SDHC_GetPhysicalAddress( index ) );
 *	@endcode
 *
 *	@see		NX_SDHC_GetPhysicalAddress,	NX_SDHC_GetSizeOfRegisterSet,
 *				NX_SDHC_GetBaseAddress,
 *				NX_SDHC_OpenModule,			NX_SDHC_CloseModule,
 *				NX_SDHC_CheckBusy,			NX_SDHC_CanPowerDown
 */
void	NX_SDHC_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );

	__g_ModuleVariables[ModuleIndex].pRegister = (struct NX_SDHC_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Module's base address.
 *	@see		NX_SDHC_GetPhysicalAddress,	NX_SDHC_GetSizeOfRegisterSet,
 *				NX_SDHC_SetBaseAddress,
 *				NX_SDHC_OpenModule,			NX_SDHC_CloseModule,
 *				NX_SDHC_CheckBusy,			NX_SDHC_CanPowerDown
 */
U32		NX_SDHC_GetBaseAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE indicates success.\n
 *				CFALSE indicates failure.
 *	@remark		This function makes a module is initialized by default value.
 *				Thererfore after calling this function, some registers may be
 *				modified to default state. If you don't want to change any
 *				setting of registers, you can skip to call this function. But
 *				you have to call this function at least once to use these
 *				prototype functions after Power-On-Reset or NX_SDHC_CloseModule().\n
 *				\b IMPORTANT : you have to enable a PCLK before calling this function.
 *	@code
 *		// Initialize the clock generator
 *		NX_SDHC_SetClockPClkMode( index, NX_PCLKMODE_ALWAYS );
 *		NX_SDHC_SetClockSource( index, 0, NX_SDHC_CLKSRC_PCLK );
 *		NX_SDHC_SetClockDivisor( index, 0, n );	// n = 1 ~ 64
 *		NX_SDHC_SetClockDivisorEnable( index, CTRUE );
 *
 *		NX_SDHC_OpenModule( index );
 *
 *		// ......
 *		// Do something
 *		// ......
 *
 *		NX_SDHC_CloseModule( index );
 *	@endcode
 *	@see		NX_SDHC_GetPhysicalAddress,	NX_SDHC_GetSizeOfRegisterSet,
 *				NX_SDHC_SetBaseAddress,		NX_SDHC_GetBaseAddress,
 *				NX_SDHC_CloseModule,
 *				NX_SDHC_CheckBusy,			NX_SDHC_CanPowerDown
 */
CBOOL	NX_SDHC_OpenModule( U32 ModuleIndex )
{
	const U32 INT_ENA = 1UL<<4;
	register U32 SetValue;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->CTRL | INT_ENA;
//	__g_ModuleVariables[ModuleIndex].pRegister->CTRL |= INT_ENA;		// global interrupt enable bit for SDHC module.
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CTRL, SetValue);		// global interrupt enable bit for SDHC module.
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		De-initialize selected module to the proper stage.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE indicates success.\n
 *				CFALSE indicates failure.
 *	@see		NX_SDHC_GetPhysicalAddress,	NX_SDHC_GetSizeOfRegisterSet,
 *				NX_SDHC_SetBaseAddress,		NX_SDHC_GetBaseAddress,
 *				NX_SDHC_OpenModule,
 *				NX_SDHC_CheckBusy,			NX_SDHC_CanPowerDown
 */
CBOOL	NX_SDHC_CloseModule( U32 ModuleIndex )
{
	const U32 INT_ENA = 1UL<<4;
	register U32 SetValue;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->CTRL &= ~INT_ENA;	// global interrupt enable bit for SDHC module.
	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->CTRL & ~INT_ENA;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CTRL, SetValue);	// global interrupt enable bit for SDHC module.

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates the selected module is busy.\n
 *				CFALSE	indicates the selected module is idle.
 *	@remark		If the command FSM state of SDHC is idle, it returns CFALSE.
 *	@see		NX_SDHC_GetPhysicalAddress,	NX_SDHC_GetSizeOfRegisterSet,
 *				NX_SDHC_SetBaseAddress,		NX_SDHC_GetBaseAddress,
 *				NX_SDHC_OpenModule,			NX_SDHC_CloseModule,
 *				NX_SDHC_CanPowerDown
 */
CBOOL	NX_SDHC_CheckBusy( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );

	if( NX_SDHC_CMDFSM_IDLE == NX_SDHC_GetCommandFSM(ModuleIndex) )	return CFALSE;
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE indicates the selected module is ready to enter power-down stage.\n
 *				CFALSE indicates the selected module is busy and not ready to enter power-down stage.
 *	@remark		If the command FSM state of SDHC is idle, it returns CTRUE.
 *	@see		NX_SDHC_GetPhysicalAddress,	NX_SDHC_GetSizeOfRegisterSet,
 *				NX_SDHC_SetBaseAddress,		NX_SDHC_GetBaseAddress,
 *				NX_SDHC_OpenModule,			NX_SDHC_CloseModule,
 *				NX_SDHC_CheckBusy
 */
CBOOL	NX_SDHC_CanPowerDown( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );

	if( NX_SDHC_CheckBusy(ModuleIndex) )	return CFALSE;
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
 *	@see		NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
S32		NX_SDHC_GetInterruptNumber( U32 ModuleIndex )
{
	const U32	InterruptNumber[NUMBER_OF_SDHC_MODULE] =
				{ INTNUM_OF_SDHC0_MODULE, INTNUM_OF_SDHC1_MODULE };

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );

	return	InterruptNumber[ModuleIndex];
}
#if defined(CONFIG_MMC_NEXELL_SDHC_MODULE) && defined(CONFIG_MMC_NEXELL)
EXPORT_SYMBOL(NX_SDHC_GetInterruptNumber);
#endif

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Specifies an interrupt number which is one of @ref NX_SDHC_INT enum.
 *	@param[in]	bEnb			Set it as CTRUE to enable an interrupt specified by @a IntNum. \n
 *								Set it as CFALSE to diable an interrupt specified by @a IntNum.
 *	@return		None.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
void	NX_SDHC_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL bEnb )
{
	register U32 regval;

	NX_ASSERT( (1<=IntNum) && (16>=IntNum) );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	regval = __g_ModuleVariables[ModuleIndex].pRegister->INTMASK;
	regval &= ~(1UL<<IntNum);
	regval |= ((U32)bEnb)<<IntNum;
//	__g_ModuleVariables[ModuleIndex].pRegister->INTMASK = regval;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->INTMASK, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Specifies an interrupt number which is one of @ref NX_SDHC_INT enum.
 *	@return		CTRUE	indicates an interrupt specified by @a IntNum is enabled.\n
 *				CFALSE	indicates an interrupt specified by @a IntNum is disabled.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
CBOOL	NX_SDHC_GetInterruptEnable( U32 ModuleIndex, S32 IntNum )
{
	NX_ASSERT( (1<=IntNum) && (16>=IntNum) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->INTMASK >> IntNum) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Specifies an interrupt number which is one of @ref NX_SDHC_INT enum.
 *	@return		CTRUE	indicates an interrupt specified by @a IntNum is pended.\n
 *				CFALSE	indicates an interrupt specified by @a IntNum is not pended.
 *	@remark		The interrupt pending status are logged regardless of interrupt
 *				enable status. Therefore the return value can be CTRUE even
 *				though the specified interrupt has been disabled.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
CBOOL	NX_SDHC_GetInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	NX_ASSERT( (1<=IntNum) && (16>=IntNum) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->RINTSTS >> IntNum) & 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Specifies an interrupt number which is one of @ref NX_SDHC_INT enum.
 *	@return		None.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
void	NX_SDHC_ClearInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	NX_ASSERT( (1<=IntNum) && (16>=IntNum) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->RINTSTS = (U32)1<<IntNum;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->RINTSTS, (U32)1<<IntNum);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnb			Set it as CTRUE to enable all interrupts. \n
 *								Set it as CFALSE to disable all interrupts.
 *	@return		None.
 *	@see		NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
void	NX_SDHC_SetInterruptEnableAll( U32 ModuleIndex, CBOOL bEnb )
{
	register U32 SetValue;
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	if( bEnb )	__g_ModuleVariables[ModuleIndex].pRegister->INTMASK = 0x1FFFE;	// 1 to 16
//	else		__g_ModuleVariables[ModuleIndex].pRegister->INTMASK = 0;
	if( bEnb )	SetValue = 0x1FFFE;	// 1 to 16
	else		SetValue = 0;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->INTMASK, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTURE	indicates there's interrupts which are enabled.\n
 *				CFALSE	indicates there's no interrupt which are enabled.
 *	@see		NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
CBOOL	NX_SDHC_GetInterruptEnableAll( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->INTMASK )	return CTRUE;
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of unmasked interrupts are pended or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTURE	indicates there's unmasked interrupts which are pended.\n
 *				CFALSE	indicates there's no unmasked interrupt which are pended.
 *	@remark		Since this function doesn't consider about pending status of
 *				interrupts which are disabled, the return value can be CFALSE
 *				even though some interrupts are pended unless a relevant
 *				interrupt is enabled.
 *	@see		NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
CBOOL	NX_SDHC_GetInterruptPendingAll( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->MINTSTS )	return CTRUE;
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@see		NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
void	NX_SDHC_ClearInterruptPendingAll( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->RINTSTS = 0xFFFFFFFF;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->RINTSTS, 0xFFFFFFFF);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt number which has the most prority of pended interrupts.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		an interrupt number which has the most priority of pended and
 *				unmasked interrupts. This value is one of @ref NX_SDHC_INT enum.
 *				If there's no interrupt which is pended and unmasked, it returns -1.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
S32		NX_SDHC_GetInterruptPendingNumber( U32 ModuleIndex )
{
	register S32 i;
	register U32 pend;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pend = __g_ModuleVariables[ModuleIndex].pRegister->MINTSTS;

	if( 0 == pend )		return -1;

	for( i=0 ;; i++, pend >>= 1 )
	{
		NX_ASSERT( i < 32 );
		if( pend & 1 )	break;
	}

	NX_ASSERT( (1<=i) && (16>=i) );
	return i;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Set interrupts to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	EnableFlag		Specifies an interrupt enable flag that each bit
 *								represents an interrupt enable status to be changed -
 *								Value of 0 masks interrupt and value of 1 enables
 *								interrupt. EnableFlag[16:1] are only valid.
 *	@return		None.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *													NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
void	NX_SDHC_SetInterruptEnable32 ( U32 ModuleIndex, U32 EnableFlag )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->INTMASK = EnableFlag & 0x1FFFE;	// 1 to 16
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->INTMASK, EnableFlag & 0x1FFFE);	// 1 to 16
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt enable status.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		An interrupt enable status that each bit represents current
 *				interrupt enable status - Value of 0 indicates relevant interrupt
 *				is masked and value of 1 indicates relevant interrupt is enabled.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,		NX_SDHC_ClearInterruptPending32
 */
U32	NX_SDHC_GetInterruptEnable32 ( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->INTMASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt pending status.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		An interrupt pending status that each bit represents current
 *				interrupt pending status - Value of 0 indicates relevant interrupt
 *				is not pended and value of 1 indicates relevant interrupt is pended.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *													NX_SDHC_ClearInterruptPending32
 */
U32	NX_SDHC_GetInterruptPending32 ( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->RINTSTS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear an interrupt pending status.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	PendingFlag		Specifies an interrupt pending clear flag. An
 *								interrupt pending status cleared only if
 *								corresponding bit in PendingFlag is set.
 *	@return		None.
 *	@see		NX_SDHC_INT,
 *				NX_SDHC_GetInterruptNumber,			NX_SDHC_SetInterruptEnable,
 *				NX_SDHC_GetInterruptEnable,			NX_SDHC_GetInterruptPending,
 *				NX_SDHC_ClearInterruptPending,		NX_SDHC_SetInterruptEnableAll,
 *				NX_SDHC_GetInterruptEnableAll,		NX_SDHC_GetInterruptPendingAll,
 *				NX_SDHC_ClearInterruptPendingAll,	NX_SDHC_GetInterruptPendingNumber,
 *				NX_SDHC_SetInterruptEnable32,		NX_SDHC_GetInterruptEnable32,
 *				NX_SDHC_GetInterruptPending32,
 */
void	NX_SDHC_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->RINTSTS = PendingFlag;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->RINTSTS, PendingFlag);
}


//------------------------------------------------------------------------------
// DMA Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a DMA peripheral index for the DMA controller.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a DMA peripheral index.
 *	@remark		Return value can be used for the DMA controller module's
 *				functions.
 *	@see		NX_SDHC_GetDMABusWidth
 */
U32		NX_SDHC_GetDMAIndex( U32 ModuleIndex )
{
	const U32	dmaindex[NUMBER_OF_SDHC_MODULE] =
		{ DMAINDEX_OF_SDHC0_MODULE, DMAINDEX_OF_SDHC1_MODULE };

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );

	return dmaindex[ModuleIndex];
}
#if defined(CONFIG_MMC_NEXELL_SDHC_MODULE) && defined(CONFIG_MMC_NEXELL)
EXPORT_SYMBOL(NX_SDHC_GetDMAIndex);
#endif

//------------------------------------------------------------------------------
/**
 *	@brief		Get a DMA peripheral bus width for the DMA controller.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a DMA peripheral bus width.
 *	@remark		Return value can be used for the DMA controller module's
 *				functions.
 *	@see		NX_SDHC_GetDMAIndex
 */
U32		NX_SDHC_GetDMABusWidth( U32 ModuleIndex )
{
	return 32;		// only support 32 bit data width.
}
#ifdef CONFIG_MMC_NEXELL_SDHC_MODULE
EXPORT_SYMBOL(NX_SDHC_GetDMABusWidth);
#endif


//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	mode			Specifies a PCLK mode to change.
 *	@return		None.
 *	@remark		SDHC doesn't support NX_PCLKMODE_DYNAMIC. If you call this
 *				function with NX_PCLKMODE_DYNAMIC, it makes that PCLK doesn't
 *				provide to the SDHC module and SDHC module doesn't operate
 *				correctly. You have to set a PCLK mode as NX_PCLKMODE_ALWAYS
 *				to operate and control the SDHC module. But you can set a PCLK
 *				mode as NX_PCLKMODE_DYNAMIC to slightly reduce a power
 *				cunsumption if you don't want to use the SDHC module.
 *	@see		NX_PCLKMODE,
 *				NX_SDHC_GetClockPClkMode,
 *				NX_SDHC_SetClockSource,			NX_SDHC_GetClockSource,
 *				NX_SDHC_SetClockDivisor,		NX_SDHC_GetClockDivisor,
 *				NX_SDHC_SetClockDivisorEnable,	NX_SDHC_GetClockDivisorEnable
 */
void	NX_SDHC_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode )
{
	const U32 PCLKMODE = 1UL<<3;

	register U32 regvalue;
	register struct NX_SDHC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue = pRegister->CLKENB;

	switch( mode )
	{
		case NX_PCLKMODE_DYNAMIC:	regvalue &= ~PCLKMODE;	break;
		case NX_PCLKMODE_ALWAYS :	regvalue |=	PCLKMODE;	break;
		default:					NX_ASSERT( CFALSE );	break;
	}

//	pRegister->CLKENB = regvalue;
	WriteIODW(&pRegister->CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current PCLK mode.
 *	@see		NX_PCLKMODE,
 *				NX_SDHC_SetClockPClkMode,
 *				NX_SDHC_SetClockSource,			NX_SDHC_GetClockSource,
 *				NX_SDHC_SetClockDivisor,		NX_SDHC_GetClockDivisor,
 *				NX_SDHC_SetClockDivisorEnable,	NX_SDHC_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_SDHC_GetClockPClkMode( U32 ModuleIndex )
{
	const U32 PCLKMODE = 1UL<<3;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->CLKENB & PCLKMODE ) ? NX_PCLKMODE_ALWAYS : NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock source of specified clock generator
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			Specifies an index of the clock generator.
 *	@param[in]	ClkSrc			Specifies a clock source.(0 ~ 2)
 *	@remark		Since the SDHC module has one clock generator, you have to set
 *				\e Index as '0' only.\n
 *				The SDHC module has three clock source - 0 : PCLK, 1 : PLL0, 2 : PLL1
 *	@return		None.
 *	@see		NX_SDHC_SetClockPClkMode,		NX_SDHC_GetClockPClkMode,
 *				NX_SDHC_GetClockSource,
 *				NX_SDHC_SetClockDivisor,		NX_SDHC_GetClockDivisor,
 *				NX_SDHC_SetClockDivisorEnable,	NX_SDHC_GetClockDivisorEnable
 */
void	NX_SDHC_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register U32 regval;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( 3 > ClkSrc );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	regval = __g_ModuleVariables[ModuleIndex].pRegister->CLKGEN;
	regval &= ~CLKSRCSEL_MASK;
	regval |= ClkSrc << CLKSRCSEL_POS;
//	__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN = regval;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a clock source of specified clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			Specifies an index of the clock generator.
 *	@return		A clock source of specified clock generator.
 *	@remark		Since the SDHC module has one clock generator, you have to set
 *				\e Index as '0' only.\n
 *				The SDHC module has three clock source - 0 : PCLK, 1 : PLL0, 2 : PLL1
 *	@see		NX_SDHC_SetClockPClkMode,		NX_SDHC_GetClockPClkMode,
 *				NX_SDHC_SetClockSource,
 *				NX_SDHC_SetClockDivisor,		NX_SDHC_GetClockDivisor,
 *				NX_SDHC_SetClockDivisorEnable,	NX_SDHC_GetClockDivisorEnable
 */
U32		NX_SDHC_GetClockSource( U32 ModuleIndex, U32 Index )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN & CLKSRCSEL_MASK) >> CLKSRCSEL_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock divider of specified clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			Specifies an index of the clock generator.
 *	@param[in]	Divider			Specifies a clock divider.
 *	@return		None.
 *	@remark		Since the SDHC module has one clock generator, you have to set
 *				\e Index as '0' only.\n
 *				The source clock can be calcurated by following formula:
 *				- the source clock = Clock Source / Divider, where Divider is between
 *				1 and 256.
 *	@remark		\b NOTE : The output clock of the clock generator must be less
 *				than or equal to PCLK.
 *	@see		NX_SDHC_SetClockPClkMode,		NX_SDHC_GetClockPClkMode,
 *				NX_SDHC_SetClockSource,			NX_SDHC_GetClockSource,
 *				NX_SDHC_GetClockDivisor,
 *				NX_SDHC_SetClockDivisorEnable,	NX_SDHC_GetClockDivisorEnable
 */
void	NX_SDHC_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divider )
{
	const U32 CLKDIV_POS	= 5;
	const U32 CLKDIV_MASK	= 0xFF << CLKDIV_POS;

	register U32 regval;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( 1 <= Divider && Divider <= 256 );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	regval =	__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN;
	regval &= ~CLKDIV_MASK;
	regval |= (Divider-1) << CLKDIV_POS;
//	__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN = regval;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a clock divider of specified clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			Specifies an index of the clock generator.
 *	@return		A clock divider of specified clock generator.
 *	@remark		Since the SDHC module has one clock generator, you have to set
 *				\e Index as '0' only.\n
 *	@see		NX_SDHC_SetClockPClkMode,		NX_SDHC_GetClockPClkMode,
 *				NX_SDHC_SetClockSource,			NX_SDHC_GetClockSource,
 *				NX_SDHC_SetClockDivisor,
 *				NX_SDHC_SetClockDivisorEnable,	NX_SDHC_GetClockDivisorEnable
 */
U32		NX_SDHC_GetClockDivisor( U32 ModuleIndex, U32 Index )
{
	const U32 CLKDIV_POS	= 5;
	const U32 CLKDIV_MASK = 0xFF << CLKDIV_POS;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)((__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN & CLKDIV_MASK)>>CLKDIV_POS) + 1;
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
 *	@see		NX_SDHC_SetClockPClkMode,		NX_SDHC_GetClockPClkMode,
 *				NX_SDHC_SetClockSource,			NX_SDHC_GetClockSource,
 *				NX_SDHC_SetClockDivisor,		NX_SDHC_GetClockDivisor,
 *				NX_SDHC_GetClockDivisorEnable
 */
void	NX_SDHC_SetClockDivisorEnable( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 CLKGENENB = 1UL<<2;

	register U32 regval;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	regval = __g_ModuleVariables[ModuleIndex].pRegister->CLKENB;
	if( bEnb )	regval |=	CLKGENENB;
	else		regval &= ~CLKGENENB;
//	__g_ModuleVariables[ModuleIndex].pRegister->CLKENB = regval;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKENB, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the clock generator is enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates the clock generator is enabled and provides a divided clock for the module.\n
 *				CFALSE	indicates the clock generator is disabled and doesn't provied a divided clock for the module.
 *	@see		NX_SDHC_SetClockPClkMode,		NX_SDHC_GetClockPClkMode,
 *				NX_SDHC_SetClockSource,			NX_SDHC_GetClockSource,
 *				NX_SDHC_SetClockDivisor,		NX_SDHC_GetClockDivisor,
 *				NX_SDHC_SetClockDivisorEnable
 */
CBOOL	NX_SDHC_GetClockDivisorEnable( U32 ModuleIndex )
{
	const U32 CLKGENENB = 1UL<<2;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->CLKENB & CLKGENENB) ? CTRUE : CFALSE;
}


//------------------------------------------------------------------------------
// SDHC Specific Interfaces
//------------------------------------------------------------------------------
/**
 *	@brief		Abort Read Data for SDIO suspend sequence.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remark		If the read data transfer is suspended by the user, the user
 *				should call NX_SDHC_AbortReadData() to reset the data state
 *				machine, which is waiting for next block of data.
 */
void	NX_SDHC_AbortReadData( U32 ModuleIndex )
{
	const U32 ABORT_RDATA = 1UL<<8;
	register U32 SetValue;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->CTRL |= ABORT_RDATA;
	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->CTRL | ABORT_RDATA;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Send IRQ response for MMC interrupt mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remark		To wait for MMC card interrupt, the user issues CMD40, and SDHC
 *				waits for interrupt response from MMC card. In meantime, if the
 *				user wants SDHC to exit waiting for interrupt state, the user
 *				should call NX_SDHC_SendIRQResponse() to send CMD40 response
 *				on bus and SDHC returns to idle state.
 */
void	NX_SDHC_SendIRQResponse( U32 ModuleIndex )
{
	const U32 SEND_IRQ_RESP = 1UL<<7;
	register U32 SetValue;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->CTRL |= SEND_IRQ_RESP;
	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->CTRL | SEND_IRQ_RESP;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Assert or clear Read Wait signal to SDIO card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bAssert			Set it as CTRUE to assert Read-Wait signal to SD card.\n
 *								Set it as CFALSE to clear Read-Wait signal to SD card.
 *	@return		None.
 *	@remark		Read-Wait is used with only the SDIO card and can temporarily
 *				stall the data transfer - either from function or memory - and
 *				allow the user to send commands to any function within the SDIO
 *				device.
 */
void	NX_SDHC_SetReadWait( U32 ModuleIndex, CBOOL bAssert )
{
	const U32 READ_WAIT_POS = 6;
	register struct NX_SDHC_RegisterSet* pRegister;
	register U32 regval;

	NX_ASSERT( (0==bAssert) || (1==bAssert) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regval = pRegister->CTRL;
	regval &= ~(1UL<<READ_WAIT_POS);
	regval |= (U32)bAssert << READ_WAIT_POS;
//	pRegister->CTRL = regval;
	WriteIODW(&pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable DMA transfer mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnb			Set it as CTRUE to enable DMA transfer mode.\n
 *								Set it as CFALSE to disable DMA transfer mode.
 *	@return		None.
 *	@see		NX_SDHC_ResetDMA,	NX_SDHC_IsResetDMA
 */
void	NX_SDHC_SetDMAMode( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 DMA_ENA_POS = 5;
	register struct NX_SDHC_RegisterSet* pRegister;
	register U32 regval;

	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regval = pRegister->CTRL;
	regval &= ~(1UL<<DMA_ENA_POS);
	regval |= (U32)bEnb << DMA_ENA_POS;
//	pRegister->CTRL = regval;
	WriteIODW(&pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Reset the DMA interface of SDHC.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remark		Reset the DMA interface abruptly terminates any DMA transer in
 *				progress. After calling NX_SDHC_ResetDMA(), you have to wait
 *				until the DMA interface is in normal state.\n
 *				The example for this is as following.
 *	@code
 *		NX_SDHC_ResetDMA( index );				// Reset the DMA interface.
 *		while( NX_SDHC_IsResetDMA( index ) );	// Wait until the DMA reset is completed.
 *	@endcode
 *	@see		NX_SDHC_SetDMAMode,
 *											NX_SDHC_IsResetDMA,
 *				NX_SDHC_ResetFIFO,			NX_SDHC_IsResetFIFO,
 *				NX_SDHC_ResetController,	NX_SDHC_IsResetController
 */
void	NX_SDHC_ResetDMA( U32 ModuleIndex )
{
	const U32 DMARST	= 1UL<<2;
	const U32 FIFORST	= 1UL<<1;
	const U32 CTRLRST	= 1UL<<0;
	register struct NX_SDHC_RegisterSet* pRegister;
	register U32 regval;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regval = pRegister->CTRL;
	regval &= ~(FIFORST | CTRLRST);
	regval |=	DMARST;
//	pRegister->CTRL = regval;
	WriteIODW(&pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the DMA interface is in reset or normal state.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates reset of the DMA interface is in progress.\n
 *				CFALSE	indicates the DMA interface is in normal state.
 *	@see		NX_SDHC_SetDMAMode,
 *				NX_SDHC_ResetDMA,
 *				NX_SDHC_ResetFIFO,			NX_SDHC_IsResetFIFO,
 *				NX_SDHC_ResetController,	NX_SDHC_IsResetController
 */
CBOOL	NX_SDHC_IsResetDMA( U32 ModuleIndex )
{
	const U32 DMARST_POS	= 2;
	const U32 DMARST_MASK	= 1UL<<DMARST_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)(__g_ModuleVariables[ModuleIndex].pRegister->CTRL & DMARST_MASK)>>DMARST_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Reset the FIFO.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remark		Resets the FIFO pointers and counters of the FIFO.\n
 *				If any of the previous data commands do not properly terminate,
 *				the the user should the FIFO reset in order to remove any
 *				residual data, if any, in the FIFO.\n
 *				After calling NX_SDHC_ResetFIFO(), you have to wait until the
 *				FIFO reset is completed. The example for this is as following.
 *	@code
 *		NX_SDHC_ResetFIFO( index );					// Reest the FIFO.
 *		while( NX_SDHC_IsResetFIFO( index ) );		// Wait until the FIFO reset is completed.
 *	@endcode
 *	@see		NX_SDHC_ResetDMA,			NX_SDHC_IsResetDMA,
											NX_SDHC_IsResetFIFO,
				NX_SDHC_ResetController,	NX_SDHC_IsResetController
 */
void	NX_SDHC_ResetFIFO( U32 ModuleIndex )
{
	const U32 DMARST	= 1UL<<2;
	const U32 FIFORST	= 1UL<<1;
	const U32 CTRLRST	= 1UL<<0;
	register struct NX_SDHC_RegisterSet* pRegister;
	register U32 regval;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regval = pRegister->CTRL;
	regval &= ~(DMARST | CTRLRST);
	regval |=	FIFORST;
//	pRegister->CTRL = regval;
	WriteIODW(&pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the FIFO is in reset or normal state.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates reset of the FIFO is in pregress.\n
 *				CFALSE	indicates the FIFO is in normal state.
 *	@see		NX_SDHC_ResetDMA,			NX_SDHC_IsResetDMA,
				NX_SDHC_ResetFIFO,
				NX_SDHC_ResetController,	NX_SDHC_IsResetController
 */
CBOOL	NX_SDHC_IsResetFIFO( U32 ModuleIndex )
{
	const U32 FIFORST_POS	= 1;
	const U32 FIFORST_MASK	= 1UL<<FIFORST_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)(__g_ModuleVariables[ModuleIndex].pRegister->CTRL & FIFORST_MASK)>>FIFORST_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Reset the SDHC internal logic.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remark		This resets:
 *				- the Card Interface Unit and stete machine.
 *				- AbortReadData, SendIRQResponse and ReadWait control.
 *				- NX_SDHC_CMD_STARTCMD state.
 *	@remark		After calling NX_SDHC_ResetController(), you have to wait until
 *				the controller reset is completed. The example for this is as
 *				following.
 *	@code
 *		NX_SDHC_ResetController( index );				// Reest the controller.
 *		while( NX_SDHC_IsResetController( index ) );	// Wait until the controller reset is completed.
 *	@endcode
 *	@see		NX_SDHC_ResetDMA,			NX_SDHC_IsResetDMA,
 *				NX_SDHC_ResetFIFO,			NX_SDHC_IsResetFIFO,
 *											NX_SDHC_IsResetController
 */
void	NX_SDHC_ResetController( U32 ModuleIndex )
{
	const U32 DMARST	= 1UL<<2;
	const U32 FIFORST	= 1UL<<1;
	const U32 CTRLRST	= 1UL<<0;
	register struct NX_SDHC_RegisterSet* pRegister;
	register U32 regval;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regval = pRegister->CTRL;
	regval &= ~(DMARST | FIFORST);
	regval |=	CTRLRST;
//	pRegister->CTRL = regval;
	WriteIODW(&pRegister->CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the controller is in reset or normal state.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates reset of the controller is in pregress.\n
 *				CFALSE	indicates the controller is in normal state.
 *	@see		NX_SDHC_ResetDMA,			NX_SDHC_IsResetDMA,
 *				NX_SDHC_ResetFIFO,			NX_SDHC_IsResetFIFO,
 *				NX_SDHC_ResetController
 */
CBOOL	NX_SDHC_IsResetController( U32 ModuleIndex )
{
	const U32 CTRLRST_POS	= 0;
	const U32 CTRLRST_MASK	= 1UL<<CTRLRST_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)(__g_ModuleVariables[ModuleIndex].pRegister->CTRL & CTRLRST_MASK)>>CTRLRST_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a divider value for an output clock.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	divider			a divider value to generate SDCLK, 1 ~ 510.\n
 *								It must be 1 or a even number between 2 and 510.
 *	@return		None.
 *	@remark		a clock divider is used to select a freqeuncy of SDCLK.
 *				A clock source of this divider is an output clock of the clock
 *				generator. For example, if PCLK is 50Mhz, then the SDCLK of
 *				400 KHz can be output by following.
 *	@code
 *		// a source clock = 25 MHz, where PCLK is 50 MHz
 *		NX_SDHC_SetClockSource( index, 0, NX_SDHC_CLKSRC_PCLK );
 *		NX_SDHC_SetClockDivisor( index, 0, 2 );
 *		NX_SDHC_SetClockDivisorEnable( index, CTRUE );
 *
 *		// 25,000,000 / 400,000 = 62.5 <= 64 (even number)
 *		// actual SDCLK = 25,000,000 / 64 = 390,625 Hz
 *		NX_SDHC_SetOutputClockDivider( index, 64 );
 *	@endcode
 *	@remark		The result of this function will be applied to the SDHC module
 *				after calling function NX_SDHC_SetCommand() with
 *				NX_SDHC_CMDFLAG_UPDATECLKONLY flag.\n\n
 *				The user should not modify clock settings while a command is
 *				being executed.\n
 *				The following shows how to update clock settings.
 *	@code
 *		// 1. Confirm that no card is engaged in any transaction.
 *		//	If there's a transaction, wait until it finishes.
 *		while( NX_SDHC_IsDataTransferBusy( index ) );
 *
 *		// 2. Disable the output clock.
 *		NX_SDHC_SetOutputClockEnable( index, CFALSE );
 *
 *		// 3. Program the clock divider as required.
 *		NX_SDHC_SetOutputClockDivider( index, n );
 *
 *		// 4. Start a command with NX_SDHC_CMDFLAG_UPDATECLKONLY flag.
 *		repeat_4 :
 *		NX_SDHC_SetCommand( index, 0, NX_SDHC_CMDFLAG_STARTCMD | NX_SDHC_CMDFLAG_UPDATECLKONLY | NX_SDHC_CMDFLAG_WAITPRVDAT );
 *
 *		// 5. Wait until a update clock command is taken by the SDHC module.
 *		//	If a HLE is occurred, repeat 4.
 *		while( NX_SDHC_IsCommandBusy( index ) );
 *
 *		if( NX_SDHC_GetInterruptPending( index, NX_SDHC_INT_HLE ) )
 *		{
 *			NX_SDHC_ClearInterruptPending( index, NX_SDHC_INT_HLE );
 *			goto repeat_4;
 *		}
 *
 *		// 6. Enable the output clock.
 *		NX_SDHC_SetOutputClockEnable( index, CTRUE );
 *
 *		// 7. Start a command with NX_SDHC_CMDFLAG_UPDATECLKONLY flag.
 *		repeat_7 :
 *		NX_SDHC_SetCommand( index, 0, NX_SDHC_CMDFLAG_STARTCMD | NX_SDHC_CMDFLAG_UPDATECLKONLY | NX_SDHC_CMDFLAG_WAITPRVDAT );
 *
 *		// 8. Wait until a update clock command is taken by the SDHC module.
 *		//	If a HLE is occurred, repeat 7.
 *		while( NX_SDHC_IsCommandBusy( index ) );
 *
 *		if( NX_SDHC_GetInterruptPending( index, NX_SDHC_INT_HLE ) )
 *		{
 *			NX_SDHC_ClearInterruptPending( index, NX_SDHC_INT_HLE );
 *			goto repeat_7;
 *		}
 *	@endcode
 *	@see		NX_SDHC_SetLowPowerClockMode,	NX_SDHC_GetLowPowerClockMode,
 *												NX_SDHC_GetOutputClockEnable,
 *				NX_SDHC_SetOutputClockDivider,	NX_SDHC_GetOutputClockDivider
 */
void	NX_SDHC_SetOutputClockDivider( U32 ModuleIndex, U32 divider )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( (1==divider) || (0==(divider&1)) );	// 1 or even number
	NX_ASSERT( (1<=divider) && (510>=divider) );	// 1 <= divider <= 510

//	__g_ModuleVariables[ModuleIndex].pRegister->CLKDIV = divider>>1;		//	2*n divider (0 : bypass)
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKDIV, divider>>1);		//	2*n divider (0 : bypass)
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a divider value for an output clock.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a divider value for an output clock.
 *	@see		NX_SDHC_SetLowPowerClockMode,	NX_SDHC_GetLowPowerClockMode,
 *				NX_SDHC_SetOutputClockDivider,
 *				NX_SDHC_SetOutputClockDivider,	NX_SDHC_GetOutputClockDivider
 */
U32		NX_SDHC_GetOutputClockDivider( U32 ModuleIndex )
{
	register U32 divider;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	divider = __g_ModuleVariables[ModuleIndex].pRegister->CLKDIV;

	return (divider) ? divider<<1 : 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the Low-Power-Clock mode to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnb			Set it as CTRUE to enable the Low-Power-Clock mode.\n
 *								Set it as CFALSE to disable the Low-Power-Clock mode.
 *	@return		None.
 *	@remark		The Low-Power-Clock mode means it stops the output clock when
 *				card in IDLE (should be normally set to only MMC and SD memory
 *				card; for SDIO cards, if interrupts must be detected, the output
 *				clock should not be stopped).\n
 *				The result of this function will be applied to the SDHC module
 *				after calling function NX_SDHC_SetCommand() with
 *				NX_SDHC_CMDFLAG_UPDATECLKONLY flag.
 *	@see										NX_SDHC_GetLowPowerClockMode,
 *				NX_SDHC_SetOutputClockEnable,	NX_SDHC_GetOutputClockEnable,
 *				NX_SDHC_SetOutputClockDivider,	NX_SDHC_GetOutputClockDivider
 */
void	NX_SDHC_SetLowPowerClockMode( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 LOWPWR = 1UL<<16;
	register U32 SetValue;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	if( bEnb )	__g_ModuleVariables[ModuleIndex].pRegister->CLKENA |=	LOWPWR;
//	else		__g_ModuleVariables[ModuleIndex].pRegister->CLKENA &= ~LOWPWR;
	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->CLKENA;
	if( bEnb )	SetValue |=	LOWPWR;
	else		SetValue &= ~LOWPWR;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKENA, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the Low-Power-Clock mode is enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates the Low-Power-Clock mode is enabled.\n
 *				CFALSE	indicates the Low-Power-Clock mode is disabled.
 *	@see		NX_SDHC_SetLowPowerClockMode,
 *				NX_SDHC_SetOutputClockEnable,	NX_SDHC_GetOutputClockEnable,
 *				NX_SDHC_SetOutputClockDivider,	NX_SDHC_GetOutputClockDivider
 */
CBOOL	NX_SDHC_GetLowPowerClockMode( U32 ModuleIndex )
{
	const U32 LOWPWR = 1UL<<16;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->CLKENA & LOWPWR) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set an output clock (SDCLK) to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnb			Set it as CTRUE to enable an output clock (SDCLK).\n
 *								Set it as CFALSE to disable an output clock (SDCLK).
 *	@return		None.
 *	@remark		The result of this function will be applied to the SDHC module
 *				after calling function NX_SDHC_SetCommand() with
 *				NX_SDHC_CMDFLAG_UPDATECLKONLY flag.
 *	@see		NX_SDHC_SetLowPowerClockMode,	NX_SDHC_GetLowPowerClockMode,
 *												NX_SDHC_GetOutputClockEnable,
 *				NX_SDHC_SetOutputClockDivider,	NX_SDHC_GetOutputClockDivider
 */
void	NX_SDHC_SetOutputClockEnable( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 CLKENA = 1UL<<0;
	register U32 SetValue;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	if( bEnb )	__g_ModuleVariables[ModuleIndex].pRegister->CLKENA |=	CLKENA;
//	else		__g_ModuleVariables[ModuleIndex].pRegister->CLKENA &= ~CLKENA;
	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->CLKENA;
	if( bEnb )	SetValue |=	CLKENA;
	else		SetValue &= ~CLKENA;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKENA, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether an output clock (SDCLK) is enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates an output clock is enabled.\n
 *				CFALSE	indicates an output clock is disabled.
 *	@see		NX_SDHC_SetLowPowerClockMode,	NX_SDHC_GetLowPowerClockMode,
 *				NX_SDHC_SetOutputClockEnable,
 *				NX_SDHC_SetOutputClockDivider, NX_SDHC_GetOutputClockDivider
 */
CBOOL	NX_SDHC_GetOutputClockEnable( U32 ModuleIndex )
{
	const U32 CLKENA = 1UL<<0;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->CLKENA & CLKENA) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a period for data timeout.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	dwTimeOut		a timeout period in SDCLKs, 0 ~ 0xFFFFFF.
 *	@return		None.
 *	@remark		This value is used for Data Read Timeout and Data Starvation by
 *				Host Timeout.
 *				- Data Read Timeout:
 *					During a read-data transfer, if the data start bit is not
 *					received before the number of data timeout clocks, the SDHC
 *					module signals data-timeout error and data transfer done to
 *					the host and terminates further data transfer.
 *				- Data Starvation by Host Timeout:
 *					If the FIFO becomes empty during a write data transmission,
 *					or if the card clock is stopped and the FIFO remains empty for
 *					data timeout clocks, then a data-starvation error is signaled
 *					to the host and the SDHC module continues to wait for data in
 *					the FIFO.\n
 *					During a read data transmission and when the FIFO becomes full,
 *					the card clock is stopped. If the FIFO remains full for data
 *					timeout clocks, a data starvation error is signaled to the host
 *					and the SDHC module continues to wait for the FIFO to start to
 *					empty.
 *	@see										NX_SDHC_GetDataTimeOut,
 *				NX_SDHC_SetResponseTimeOut,		NX_SDHC_GetResponseTimeOut
 */
void	NX_SDHC_SetDataTimeOut( U32 ModuleIndex, U32 dwTimeOut )
{
	const U32 DTMOUT_POS	= 8;
	const U32 DTMOUT_MASK	= 0xFFFFFFUL<<DTMOUT_POS;
	register U32 regval;

	NX_ASSERT( 0xFFFFFF >= dwTimeOut );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	regval = __g_ModuleVariables[ModuleIndex].pRegister->TMOUT;
	regval &= ~DTMOUT_MASK;
	regval |= dwTimeOut << DTMOUT_POS;
//	__g_ModuleVariables[ModuleIndex].pRegister->TMOUT = regval;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->TMOUT, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a period for data timeout.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a number of data timeout clocks in SDCLKs.
 *	@see		NX_SDHC_SetDataTimeOut,
 *				NX_SDHC_SetResponseTimeOut,	NX_SDHC_GetResponseTimeOut
 */
U32		NX_SDHC_GetDataTimeOut( U32 ModuleIndex )
{
	const U32 DTMOUT_POS	= 8;
	const U32 DTMOUT_MASK	= 0xFFFFFFUL<<DTMOUT_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->TMOUT & DTMOUT_MASK) >> DTMOUT_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a period for response timeout.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	dwTimeOut		a timeout period in SDCLKs, 0 ~ 255.
 *	@return		None.
 *	@remark		This value is used for Response Timeout.\n
 *				If a response is expected for the command after the command is
 *				sent out, the SDHC module receives a 48-bit or
 *				136-bit response and sends it to the host. If the start bit of
 *				the card response is not received within the number of response
 *				timeout clocks, then the SDHC module signals response timeout
 *				error and command done to the host.
 *	@see		NX_SDHC_SetDataTimeOut,			NX_SDHC_GetDataTimeOut,
 *												NX_SDHC_GetResponseTimeOut
 */
void	NX_SDHC_SetResponseTimeOut( U32 ModuleIndex, U32 dwTimeOut )
{
	const U32 RSPTMOUT_POS	= 0;
	const U32 RSPTMOUT_MASK = 0xFFUL<<RSPTMOUT_POS;
	register U32 regval;

	NX_ASSERT( 0xFF >= dwTimeOut );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	regval = __g_ModuleVariables[ModuleIndex].pRegister->TMOUT;
	regval &= ~RSPTMOUT_MASK;
	regval |= dwTimeOut << RSPTMOUT_POS;
//	__g_ModuleVariables[ModuleIndex].pRegister->TMOUT = regval;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->TMOUT, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a period for response timeout.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a number of response timeout clocks in SDCLKs.
 *	@see		NX_SDHC_SetDataTimeOut,		NX_SDHC_GetDataTimeOut,
 *				NX_SDHC_SetResponseTimeOut
 */
U32		NX_SDHC_GetResponseTimeOut( U32 ModuleIndex )
{
	const U32 RSPTMOUT_POS	= 0;
	const U32 RSPTMOUT_MASK = 0xFFUL<<RSPTMOUT_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->TMOUT & RSPTMOUT_MASK) >> RSPTMOUT_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a data bus width.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	width			a data bus width, 1 or 4.
 *	@return		None.
 *	@remark		1-bit width is standard bus mode and SDHC uses only SDDAT[0]
 *				for data transfer. 4-bit width is wide bus mode and SDHC uses
 *				SDDAT[3:0] for data transfer.
 *	@see		NX_SDHC_GetDataBusWidth
 */
void	NX_SDHC_SetDataBusWidth( U32 ModuleIndex, U32 width )
{
	NX_ASSERT( (1==width) || (4==width) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->CTYPE = width >> 2;	// 0 : 1-bit mode, 1 : 4-bit mode
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CTYPE, width >> 2);	// 0 : 1-bit mode, 1 : 4-bit mode
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a data bus width.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a data bus width.
 *	@see		NX_SDHC_SetDataBusWidth
 */
U32		NX_SDHC_GetDataBusWidth( U32 ModuleIndex )
{
	const U32 WIDTH = 1UL<<0;
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->CTYPE & WIDTH) ? 4 : 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the block size for block data transfer.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	SizeInByte		Specifies the number of bytes in a block, 0 ~ 65535.
 *	@return		None.
 *	@remark		The Block size is normally set to 512 for MMC/SD module data
 *				transactions. The value is specified in the card's CSD.
 *	@see								NX_SDHC_GetBlockSize,
 *				NX_SDHC_SetByteCount,	NX_SDHC_GetByteCount
 */
void	NX_SDHC_SetBlockSize( U32 ModuleIndex, U32 SizeInByte )
{
	NX_ASSERT( 65535>=SizeInByte );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->BLKSIZ = SizeInByte;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->BLKSIZ, SizeInByte);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the block size for block data transfer.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Indicates the number of bytes in a block.
 *	@see		NX_SDHC_SetBlockSize,
 *				NX_SDHC_SetByteCount,	NX_SDHC_GetByteCount
 */
U32		NX_SDHC_GetBlockSize( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->BLKSIZ;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the data size to be transferred.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	SizeInByte		Specifies the data size in bytes.
 *	@return		None.
 *	@remark		The data size should be integer multiple of the block size for
 *				block data transfers. However, for undefined number of byte
 *				transfers, the data size should be set to 0. When the data size
 *				is set to 0, it is responsibility of host to explicitly send
 *				stop/abort command to terminate data transfer.
 *	@see		NX_SDHC_SetBlockSize,	NX_SDHC_GetBlockSize,
 *										NX_SDHC_GetByteCount
 */
void	NX_SDHC_SetByteCount( U32 ModuleIndex, U32 SizeInByte )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->BYTCNT = SizeInByte;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->BYTCNT, SizeInByte);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the data size to be transferred.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Indicates the number of bytes to be transferred.
 *	@see		NX_SDHC_SetBlockSize,	NX_SDHC_GetBlockSize,
 *				NX_SDHC_SetByteCount
 */
U32		NX_SDHC_GetByteCount( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->BYTCNT;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set	a command argument to be passed to the card..
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	argument		Specifies a command argument.
 *	@return		None.
 *	@see		NX_SDHC_SetCommand
 */
void	NX_SDHC_SetCommandArgument( U32 ModuleIndex, U32 argument )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->CMDARG = argument;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CMDARG, argument);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a command.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Cmd				Specifies a command index, 0 ~ 63.
 *	@param[in]	flag			Specifies a command flag. This flag has to consist of ORed @ref NX_SDHC_CMDFLAG combination.
 *	@return		None.
 *	@remark		This function is for providing user-specific and general command
 *				format of SD/MMC interface.
 *				- If flag has NX_SDHC_CMDFLAG_STARTCMD, a corresponding command
 *					will be send to the card immediately.
 *				- If flag does not have NX_SDHC_CMDFLAG_STARTCMD, you have to
 *					call NX_SDHC_StartCommand() function to send a command to the
 *					card.
 *	@remark		You have to wait unitl NX_SDHC_IsCommandBusy() returns CFALSE
 *				After calling NX_SDHC_SetCommand() with NX_SDHC_CMDFLAG_STARTCMD
 *				flag.
 *	@code
 *		NX_SDHC_SetCommand( index, cmd, flag | NX_SDHC_CMDFLAG_STARTCMD );
 *		while( NX_SDHC_IsCommandBusy( index ) );
 *	@endcode
 *	@see		NX_SDHC_CMDFLAG, NX_SDHC_StartCommand, NX_SDHC_IsCommandBusy
 *				NX_SDHC_SetCommandArgument
 */
void	NX_SDHC_SetCommand( U32 ModuleIndex, U32 Cmd, U32 flag )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 64 > Cmd );
	NX_ASSERT( (0==flag) || (64 < flag) );

//	__g_ModuleVariables[ModuleIndex].pRegister->CMD = Cmd | flag;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CMD, Cmd | flag);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Start a command which was specified by NX_SDHC_SetCommand() function.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remark		You have to wait unitl NX_SDHC_IsCommandBusy() returns CFALSE
 *				After calling NX_SDHC_StartCommand().
 *	@code
 *		NX_SDHC_StartCommand( index );
 *		while( NX_SDHC_IsCommandBusy( index ) );
 *	@endcode
 *	@see		NX_SDHC_SetCommand, NX_SDHC_IsCommandBusy
 */
void	NX_SDHC_StartCommand( U32 ModuleIndex )
{
	register U32 SetValue;
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->CMD |= NX_SDHC_CMDFLAG_STARTCMD;
	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->CMD | NX_SDHC_CMDFLAG_STARTCMD;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CMD, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a new command is taken by the SDHC or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE indicates a command loading is in progress.\n
 *				CFALSE indicates a command is taken by the SDHC module.
 *	@remark		While a command loading is in progress, The user should not
 *				modify of SDHC module's settings. If the user try to modify any
 *				of settings while a command loading is in progress, then the
 *				modification is ignored and the SDHC module generates a HLE.\n
 *				The following happens when the command is loaded into the SDHC:
 *				- The SDHC module accepts the command for execution, unless one
 *					command is in progress, at which point the SDHC module can
 *					load and keep the second command in the buffer.
 *				- If the SDHC module is unable to load command - that is, a
 *					command is already in progress, a second command is in the
 *					buffer, and a third command is attempted - then it generates
 *					a HLE(Hardware-locked error).
 *	@see		NX_SDHC_SetCommand, NX_SDHC_StartCommand
 */
CBOOL	NX_SDHC_IsCommandBusy( U32 ModuleIndex )
{
	const U32 STARTCMD_POS = 31;
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)(__g_ModuleVariables[ModuleIndex].pRegister->CMD >> STARTCMD_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a short response.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A 32-bit short response which represents card status or some argument.
 *	@see										NX_SDHC_GetLongResponse,
 *				NX_SDHC_GetAutoStopResponse,	NX_SDHC_GetResponseIndex
 */
U32		NX_SDHC_GetShortResponse( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->RESP0;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a long response.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[out]	pLongResponse	Specifies a pointer to 4 x 32bit memory that receives a 128-bit
 *								long response including CIS/CSD, CRC7 and End bit.
 *	@return		None.
 *	@see		NX_SDHC_GetShortResponse,
 *				NX_SDHC_GetAutoStopResponse,	NX_SDHC_GetResponseIndex
 */
void	NX_SDHC_GetLongResponse( U32 ModuleIndex, U32 *pLongResponse )
{
	volatile U32 *pSrc;

	NX_ASSERT( CNULL != pLongResponse );
	NX_ASSERT( 0 == ((U32)pLongResponse & 3) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pSrc = &__g_ModuleVariables[ModuleIndex].pRegister->RESP0;

	pLongResponse[0] = pSrc[0];
	pLongResponse[1] = pSrc[1];
	pLongResponse[2] = pSrc[2];
	pLongResponse[3] = pSrc[3];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a short response for auto-stop command.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A 32-bit short response for auto-stop command.
 *	@remark		When the SDHC module sends auto-stop comamnd(NX_SDHC_CMDFLAG_SENDAUTOSTOP),
 *				then the user retrieves a response for auto-stop command by this function.
 *	@see		NX_SDHC_CMDFLAG_SENDAUTOSTOP,
 *				NX_SDHC_GetShortResponse,		NX_SDHC_GetLongResponse,
 *												NX_SDHC_GetResponseIndex
 */
U32		NX_SDHC_GetAutoStopResponse( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->RESP1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an index of previous response.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		An index of previous response including any auto-stop sent by the SDHC module.
 *	@see		NX_SDHC_GetShortResponse,		NX_SDHC_GetLongResponse,
 *				NX_SDHC_GetAutoStopResponse,
 */
U32		NX_SDHC_GetResponseIndex( U32 ModuleIndex )
{
	const U32 RSPINDEX_POS	= 11;
	const U32 RSPINDEX_MASK = 0x3F << RSPINDEX_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->STATUS & RSPINDEX_MASK) >> RSPINDEX_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a DMA request signal state.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates a DMA request signal is asserted.\n
 *				CFALSE	indicates a DAM request signal is cleared.
 *	@see		NX_SDHC_IsDMAAck,	NX_SDHC_SetDMAMode
 */
CBOOL	NX_SDHC_IsDMAReq( U32 ModuleIndex )
{
	const U32 DMAREQ_POS	= 31;
	const U32 DMAREQ_MASK	= 1UL << DMAREQ_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & DMAREQ_MASK) >> DMAREQ_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a DMA acknowledge signal state.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates a DMA acknowledge signal is asserted.\n
 *				CFALSE	indicates a DAM acknowledge signal is cleared.
 *	@see		NX_SDHC_IsDMAReq,	NX_SDHC_SetDMAMode
 */
CBOOL	NX_SDHC_IsDMAAck( U32 ModuleIndex )
{
	const U32 DMAACK_POS	= 30;
	const U32 DMAACK_MASK	= 1UL << DMAACK_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & DMAACK_MASK) >> DMAACK_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a number of filled locations in FIFO.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A number of filled locations, in 32-bits, in FIFO.
 *	@see		NX_SDHC_IsFIFOFull,			NX_SDHC_IsFIFOEmpty,
 *				NX_SDHC_IsFIFOTxThreshold,	NX_SDHC_IsFIFORxThreshold,
 *				NX_SDHC_SetFIFORxThreshold,	NX_SDHC_GetFIFORxThreshold,
 *				NX_SDHC_SetFIFOTxThreshold,	NX_SDHC_GetFIFOTxThreshold
 */
U32		NX_SDHC_GetFIFOCount( U32 ModuleIndex )
{
	const U32 FIFOCOUNT_POS		= 17;
	const U32 FIFOCOUNT_MASK	= 0x1FUL << FIFOCOUNT_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->STATUS & FIFOCOUNT_MASK) >> FIFOCOUNT_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a data transfer is busy or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates a data transfer is busy.\n
 *				CFALSE	indicates a data transfer is not busy.
 *	@see
 */
CBOOL	NX_SDHC_IsDataTransferBusy( U32 ModuleIndex )
{
	const U32 FSMBUSY_POS	= 10;
	const U32 FSMBUSY_MASK	= 1UL << FSMBUSY_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & FSMBUSY_MASK) >> FSMBUSY_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a card data is busy or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates a card data is busy.\n
 *				CFALSE	indicates a card data is not busy.
 *	@remark		The return value is an inverted state of SDDAT[0].
 *	@see		NX_SDHC_IsCardPresent
 */
CBOOL	NX_SDHC_IsCardDataBusy( U32 ModuleIndex )
{
	const U32 DATBUSY_POS	= 9;
	const U32 DATBUSY_MASK	= 1UL << DATBUSY_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & DATBUSY_MASK) >> DATBUSY_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a card is present or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE indicates a card is present.\n
 *				CFALSE indicates a card is not present.
 *	@remark		The return value is a state of SDDAT[3] pin.\n
 *				SDDAT[3] pin can be used to detect card presence if it is pulled
 *				low by external H/W circuit. When there's no card on the bus,
 *				SDDAT[3] shows a low voltage level.\n
 *				There's a 50 KOhm pull-up resistor on SDDAT[3] in the card.
 *				Therefore SDDAT[3] pin pulls the bus line high when any card is
 *				inserted on the bus. This pull-up in the card should be
 *				disconnected by the user, during regular data transfer, with
 *				SET_CLR_CARD_DETECT (ACMD42) command.
 *	@see		NX_SDHC_IsCardDataBusy
 */
CBOOL	NX_SDHC_IsCardPresent( U32 ModuleIndex )
{
	const U32 CPRESENT_POS	= 8;
	const U32 CPRESENT_MASK = 1UL << CPRESENT_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & CPRESENT_MASK) >> CPRESENT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a command FSM state.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A command FSM state represents by @ref NX_SDHC_CMDFSM enum.
 *	@see		NX_SDHC_CMDFSM
 */
NX_SDHC_CMDFSM	NX_SDHC_GetCommandFSM( U32 ModuleIndex )
{
	const U32 CMDFSM_POS	= 4;
	const U32 CMDFSM_MASK	= 0xFUL << CMDFSM_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (NX_SDHC_CMDFSM)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & CMDFSM_MASK) >> CMDFSM_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether FIFO is full or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates FIFO is full.\n
 *				CFALSE	indicates FIFO is not full.
 *	@see		NX_SDHC_GetFIFOCount,
 *											NX_SDHC_IsFIFOEmpty,
 *				NX_SDHC_IsFIFOTxThreshold,	NX_SDHC_IsFIFORxThreshold,
 *				NX_SDHC_SetFIFORxThreshold,	NX_SDHC_GetFIFORxThreshold,
 *				NX_SDHC_SetFIFOTxThreshold,	NX_SDHC_GetFIFOTxThreshold
 */
CBOOL	NX_SDHC_IsFIFOFull( U32 ModuleIndex )
{
	const U32 FIFOFULL_POS	= 3;
	const U32 FIFOFULL_MASK = 1UL << FIFOFULL_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & FIFOFULL_MASK) >> FIFOFULL_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether FIFO is empty or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates FIFO is empty.\n
 *				CFALSE	indicates FIFO is not empty.
 *	@see		NX_SDHC_GetFIFOCount,
 *				NX_SDHC_IsFIFOFull,
 *				NX_SDHC_IsFIFOTxThreshold,	NX_SDHC_IsFIFORxThreshold,
 *				NX_SDHC_SetFIFORxThreshold,	NX_SDHC_GetFIFORxThreshold,
 *				NX_SDHC_SetFIFOTxThreshold,	NX_SDHC_GetFIFOTxThreshold
 */
CBOOL	NX_SDHC_IsFIFOEmpty( U32 ModuleIndex )
{
	const U32 FIFOEMPTY_POS		= 2;
	const U32 FIFOEMPTY_MASK	= 1UL << FIFOEMPTY_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & FIFOEMPTY_MASK) >> FIFOEMPTY_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether FIFO reached transmit watermark level.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates FIFO reached trassmit watermark level.\n
 *				CFALSE	indicates FIFO does not reach transmit watermark level.
 *	@remark		Recommend to use NX_SDHC_GetInterruptPending( NX_SDHC_INT_TXDR )
 *				instead of this function.
 *	@see		NX_SDHC_GetFIFOCount,
 *				NX_SDHC_IsFIFOFull,				NX_SDHC_IsFIFOEmpty,
 *												NX_SDHC_IsFIFORxThreshold,
 *				NX_SDHC_SetFIFORxThreshold,		NX_SDHC_GetFIFORxThreshold,
 *				NX_SDHC_SetFIFOTxThreshold,		NX_SDHC_GetFIFOTxThreshold
 */
CBOOL	NX_SDHC_IsFIFOTxThreshold( U32 ModuleIndex )
{
	const U32 TXWMARK_POS	= 1;
	const U32 TXWMARK_MASK	= 1UL << TXWMARK_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & TXWMARK_MASK) >> TXWMARK_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether FIFO reached receive watermark level.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		CTRUE	indicates FIFO reached receive watermark level.\n
 *				CFALSE	indicates FIFO does not reach receive watermark level.
 *	@remark		Recommend to use NX_SDHC_GetInterruptPending( NX_SDHC_INT_RXDR )
 *				instead of this function.
 *	@see		NX_SDHC_GetFIFOCount,
 *				NX_SDHC_IsFIFOFull,				NX_SDHC_IsFIFOEmpty,
 *				NX_SDHC_IsFIFOTxThreshold,
 *				NX_SDHC_SetFIFORxThreshold,		NX_SDHC_GetFIFORxThreshold,
 *				NX_SDHC_SetFIFOTxThreshold,		NX_SDHC_GetFIFOTxThreshold
 */
CBOOL	NX_SDHC_IsFIFORxThreshold( U32 ModuleIndex )
{
	const U32 RXWMARK_POS	= 0;
	const U32 RXWMARK_MASK	= 1UL << RXWMARK_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->STATUS & RXWMARK_MASK) >> RXWMARK_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a FIFO threshold watermark level when receiving data from card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Threshold		Specifies a FIFO threshold watermark level when
 *								receiving data from card. This value represents a
 *								number of 32-bit data and should be less than or
 *								equal to 14. Recommend to set it as 7.
 *	@return		None.
 *	@remark		When FIFO data count reaches greater than FIFO RX threshold
 *				watermark level, DMA/FIFO request is raised. During end of
 *				packet, request is generated regardless of threshold programming
 *				in order to complete any remaining data.\n
 *				In non-DMA mode, when receiver FIFO threshold (RXDR) interrupt
 *				is enabled, then interrupt is generated instead of DMA request.
 *				During end of packet, interrupt is not generated if threshold
 *				programming is larger than any remaining data. It is responsibility
 *				of host to read remaining bytes on seeing Data Transfer Done
 *				interrupt.\n
 *				In DMA mode, at end of packet, even if remaining bytes are less
 *				than threshold, DMA request does single transfers to flush out
 *				any remaining bytes before Data Transfer Done interrupt is set.
 *	@see		NX_SDHC_GetFIFOCount,
 *				NX_SDHC_IsFIFOFull,				NX_SDHC_IsFIFOEmpty,
 *				NX_SDHC_IsFIFOTxThreshold,		NX_SDHC_IsFIFORxThreshold,
 *												NX_SDHC_GetFIFORxThreshold,
 *				NX_SDHC_SetFIFOTxThreshold,		NX_SDHC_GetFIFOTxThreshold
 */
void	NX_SDHC_SetFIFORxThreshold( U32 ModuleIndex, U32 Threshold )
{
	const U32 RXTH_POS	= 16;
	const U32 RXTH_MASK = 0xFFFUL << RXTH_POS;
	register U32 regval;

	NX_ASSERT( 14 >= Threshold );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	regval = __g_ModuleVariables[ModuleIndex].pRegister->FIFOTH;
	regval &= ~RXTH_MASK;
	regval |= Threshold << RXTH_POS;
//	__g_ModuleVariables[ModuleIndex].pRegister->FIFOTH = regval;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->FIFOTH, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a FIFO threshold watermark level when receiving data from card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A FIFO threshold watermark level, in 32-bits, when receiving data from card.
 *	@see		NX_SDHC_GetFIFOCount,
 *				NX_SDHC_IsFIFOFull,				NX_SDHC_IsFIFOEmpty,
 *				NX_SDHC_IsFIFOTxThreshold,		NX_SDHC_IsFIFORxThreshold,
 *				NX_SDHC_SetFIFORxThreshold,
 *				NX_SDHC_SetFIFOTxThreshold,		NX_SDHC_GetFIFOTxThreshold
 */
U32		NX_SDHC_GetFIFORxThreshold( U32 ModuleIndex )
{
	const U32 RXTH_POS	= 16;
	const U32 RXTH_MASK = 0xFFFUL << RXTH_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	return (__g_ModuleVariables[ModuleIndex].pRegister->FIFOTH & RXTH_MASK) >> RXTH_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a FIFO threshold watermark level when transmitting data to card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Threshold		Specifies a FIFO threshold watermark level when
 *								transmitting data from card. This value represents a
 *								number of 32-bit data and should be greater than 1
 *								and less than or equal to 16.
 *								Recommend to set it as 8.
 *	@return		None.
 *	@remark		When FIFO data count is less than or equal to FIFO TX threshold
 *				watermark level, DMA/FIFO request is raised. If Interrupt is
 *				enabled, then interrupt occurs. During end of packet, request or
 *				interrupt is generated, regardless of threshold programming.\n
 *				In non-DMA mode, when transmit FIFO threshold (TXDR) interrupt
 *				is enabled, then interrupt is generated instead of DMA request.
 *				During end of packet, on last interrupt, host is responsible for
 *				filling FIFO with only required remaining bytes (not before FIFO
 *				is full or after SDHC module completes data transfers, because
 *				FIFO may not be empty).
 *	@see		NX_SDHC_GetFIFOCount,
 *				NX_SDHC_IsFIFOFull,				NX_SDHC_IsFIFOEmpty,
 *				NX_SDHC_IsFIFOTxThreshold,		NX_SDHC_IsFIFORxThreshold,
 *				NX_SDHC_SetFIFORxThreshold,		NX_SDHC_GetFIFORxThreshold,
 *												NX_SDHC_GetFIFOTxThreshold
 */
void	NX_SDHC_SetFIFOTxThreshold( U32 ModuleIndex, U32 Threshold )
{
	const U32 TXTH_POS	= 0;
	const U32 TXTH_MASK = 0xFFFUL << TXTH_POS;
	register U32 regval;

	NX_ASSERT( (16 >= Threshold) && (0 < Threshold) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	regval = __g_ModuleVariables[ModuleIndex].pRegister->FIFOTH;
	regval &= ~TXTH_MASK;
	regval |= Threshold << TXTH_POS;
//	__g_ModuleVariables[ModuleIndex].pRegister->FIFOTH = regval;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->FIFOTH, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a FIFO threshold watermark level when transmitting data from card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A FIFO threshold watermark level, in 32-bits, when transmitting data from card.
 *	@see		NX_SDHC_GetFIFOCount,
 *				NX_SDHC_IsFIFOFull,				NX_SDHC_IsFIFOEmpty,
 *				NX_SDHC_IsFIFOTxThreshold,		NX_SDHC_IsFIFORxThreshold,
 *				NX_SDHC_SetFIFORxThreshold,		NX_SDHC_GetFIFORxThreshold,
 *				NX_SDHC_SetFIFOTxThreshold
 */
U32		NX_SDHC_GetFIFOTxThreshold( U32 ModuleIndex )
{
	const U32 TXTH_POS	= 0;
	const U32 TXTH_MASK = 0xFFFUL << TXTH_POS;

	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->FIFOTH & TXTH_MASK) >> TXTH_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a number of bytes transferred by the SDHC module to card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A number of bytes transferred by the SDHC module to card.
 *	@see		NX_SDHC_GetFIFOTransferSize
 */
U32		NX_SDHC_GetDataTransferSize( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->TCBCNT;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a number of bytes transferred between Host/DMA and FIFO.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A number of bytes transferred	between Host/DMA and FIFO.
 *	@see		NX_SDHC_GetDataTransferSize
 */
U32		NX_SDHC_GetFIFOTransferSize( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->TBBCNT;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a data to be transmitted to the card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	dwData	Specifies a 32-bit data to be transmitted.
 *	@return		None
 *	@remark		This function doesn't check an availablility of FIFO. Therefore
 *				you have to check a FIFO count before using this function.
 *				If you tried to push data when FIFO is full, @ref NX_SDHC_INT_FRUN
 *				is issued by the SDHC module.
 *	@see								NX_SDHC_GetData,
 *				NX_SDHC_SetData32,		NX_SDHC_GetData32,
 *				NX_SDHC_GetDataPointer
 */
void	NX_SDHC_SetData( U32 ModuleIndex, U32 dwData )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->DATA = dwData;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->DATA, dwData);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a data which was received from the card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A 32-bit data which was received from the card.
 *	@remark		This function doesn't check an availablility of FIFO. Therefore
 *				you have to check a FIFO count before using this function.
 *				If you tried to read data when FIFO was empty, @ref NX_SDHC_INT_FRUN
 *				is issued by the SDHC module.
 *	@see		NX_SDHC_SetData,
 *				NX_SDHC_SetData32,		NX_SDHC_GetData32,
 *				NX_SDHC_GetDataPointer
 */
U32		NX_SDHC_GetData( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->DATA;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set 32 bytes of data to be transmitted to the card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	pdwData			a pointer to a buffer include 8 x 32-bit data to be
 *								transmitted to the card.
 *	@return		None.
 *	@remark		This function doesn't check an availablility of FIFO. Therefore
 *				you have to check a FIFO count before using this function.
 *				If you tried to push data when FIFO is full, @ref NX_SDHC_INT_FRUN
 *				is issued by the SDHC module.
 *	@see		NX_SDHC_SetData,		NX_SDHC_GetData,
 *										NX_SDHC_GetData32,
 *				NX_SDHC_GetDataPointer
 */
void	NX_SDHC_SetData32( U32 ModuleIndex, const U32 *pdwData )
{
	volatile U32 *pDst;

	NX_ASSERT( CNULL != pdwData );
	NX_ASSERT( 0 == ((U32)pdwData&3) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pDst = &__g_ModuleVariables[ModuleIndex].pRegister->DATA;

	// Loop is unrolled to decrease CPU pipeline broken for a performance.
//	*pDst = pdwData[0];
//	*pDst = pdwData[1];
//	*pDst = pdwData[2];
//	*pDst = pdwData[3];
//	*pDst = pdwData[4];
//	*pDst = pdwData[5];
//	*pDst = pdwData[6];
//	*pDst = pdwData[7];

	WriteIODW(pDst, pdwData[0]);
	WriteIODW(pDst, pdwData[1]);
	WriteIODW(pDst, pdwData[2]);
	WriteIODW(pDst, pdwData[3]);
	WriteIODW(pDst, pdwData[4]);
	WriteIODW(pDst, pdwData[5]);
	WriteIODW(pDst, pdwData[6]);
	WriteIODW(pDst, pdwData[7]);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get 32 bytes of data which were received from the card.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[out]	pdwData			a pointer to a buffer will be stored 8 x 32-bit data
 *								which were received from the card.
 *	@return		None.
 *	@remark		This function doesn't check an availablility of FIFO. Therefore
 *				you have to check a FIFO count before using this function.
 *				If you tried to read data when FIFO was empty, @ref NX_SDHC_INT_FRUN
 *				is issued by the SDHC module.
 *	@see		NX_SDHC_SetData,		NX_SDHC_GetData,
 *				NX_SDHC_SetData32,
 *				NX_SDHC_GetDataPointer
 */
void	NX_SDHC_GetData32( U32 ModuleIndex, U32 *pdwData )
{
	volatile U32 *pSrc;

	NX_ASSERT( CNULL != pdwData );
	NX_ASSERT( 0 == ((U32)pdwData&3) );
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pSrc = &__g_ModuleVariables[ModuleIndex].pRegister->DATA;

	// Loop is unrolled to decrease CPU pipeline broken for a performance.
	pdwData[0] = *pSrc;
	pdwData[1] = *pSrc;
	pdwData[2] = *pSrc;
	pdwData[3] = *pSrc;
	pdwData[4] = *pSrc;
	pdwData[5] = *pSrc;
	pdwData[6] = *pSrc;
	pdwData[7] = *pSrc;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a pointer to set or get a 32-bit data on FIFO.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a 32-bit data pointer.
 *	@remark		You have only to aceess this pointer by 32-bit mode and do not
 *				increase or decrease this pointer.\n
 *				The example for this is as following.
 *	@code
 *		volatile U32 *pData = NX_SDHC_GetDataPointer( index );
 *		*pData = dwData;	// Push a 32-bit data to FIFO.
 *		dwData = *pData;	// Pop a 32-bit data from FIFO.
 *	@endcode
 *	@see		NX_SDHC_SetData,	NX_SDHC_GetData,
 *				NX_SDHC_SetData32,	NX_SDHC_GetData32
 */
volatile U32*	NX_SDHC_GetDataPointer( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_SDHC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return &__g_ModuleVariables[ModuleIndex].pRegister->DATA;
}
