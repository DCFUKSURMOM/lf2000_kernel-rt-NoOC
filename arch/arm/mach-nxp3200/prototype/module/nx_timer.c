//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: TIMER
//	File		: nx_timer.c
//	Description	:
//	Author		: Goofy
//	History		:
//------------------------------------------------------------------------------

#include "nx_timer.h"
#include <linux/module.h>

static	struct
{
	struct NX_TIMER_RegisterSet *pRegister;
} __g_ModuleVariables[NUMBER_OF_TIMER_MODULE] = { {CNULL,}, };


//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize the prototype enviroment and local variables.
 *	@return		\b CTRUE	indicates that the initialization is succeeded.\n
 *				\b CFALSE	indicates that the initialization is failed.\n
 *	@see		NX_TIMER_GetNumberOfModule
 */
CBOOL	NX_TIMER_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_TIMER_MODULE; i++ )
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
 *	@return		A number of module in the chip.
 *	@see		NX_TIMER_Initialize
 */
U32		NX_TIMER_GetNumberOfModule( void )
{
	return NUMBER_OF_TIMER_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get the module's physical address.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		The module's physical address.
 *	@see										NX_TIMER_GetSizeOfRegisterSet,
 *				NX_TIMER_SetBaseAddress,		NX_TIMER_GetBaseAddress,
 *				NX_TIMER_OpenModule,			NX_TIMER_CloseModule,
 *				NX_TIMER_CheckBusy,				NX_TIMER_CanPowerDown
 */
U32		NX_TIMER_GetPhysicalAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );

	return	(U32)( PHY_BASEADDR_TIMER_MODULE + (OFFSET_OF_TIMER_MODULE * ModuleIndex) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		A size of module's register set.
 *	@see		NX_TIMER_GetPhysicalAddress,
 *				NX_TIMER_SetBaseAddress,		NX_TIMER_GetBaseAddress,
 *				NX_TIMER_OpenModule,			NX_TIMER_CloseModule,
 *				NX_TIMER_CheckBusy,				NX_TIMER_CanPowerDown
 */
U32		NX_TIMER_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_TIMER_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	BaseAddress	A module's base address.
 *	@return		None.
 *	@see		NX_TIMER_GetPhysicalAddress,	NX_TIMER_GetSizeOfRegisterSet,
 *												NX_TIMER_GetBaseAddress,
 *				NX_TIMER_OpenModule,			NX_TIMER_CloseModule,
 *				NX_TIMER_CheckBusy,				NX_TIMER_CanPowerDown
 */
void	NX_TIMER_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );

	__g_ModuleVariables[ModuleIndex].pRegister = (struct NX_TIMER_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A module's base address.
 *	@see		NX_TIMER_GetPhysicalAddress,	NX_TIMER_GetSizeOfRegisterSet,
 *				NX_TIMER_SetBaseAddress,
 *				NX_TIMER_OpenModule,			NX_TIMER_CloseModule,
 *				NX_TIMER_CheckBusy,				NX_TIMER_CanPowerDown
 */
U32		NX_TIMER_GetBaseAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected module with default value.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicates that the initialization is succeeded.\n
 *				\b CFALSE	indicates that the initialization is failed.
 *	@see		NX_TIMER_GetPhysicalAddress,	NX_TIMER_GetSizeOfRegisterSet,
 *				NX_TIMER_SetBaseAddress,		NX_TIMER_GetBaseAddress,
 *												NX_TIMER_CloseModule,
 *				NX_TIMER_CheckBusy,				NX_TIMER_CanPowerDown
 */
CBOOL	NX_TIMER_OpenModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicates that the deinitialization is successed. \n
 *				\b CFALSE	indicates that the deinitialization is failed.
 *	@see		NX_TIMER_GetPhysicalAddress,	NX_TIMER_GetSizeOfRegisterSet,
 *				NX_TIMER_SetBaseAddress,		NX_TIMER_GetBaseAddress,
 *				NX_TIMER_OpenModule,
 *				NX_TIMER_CheckBusy,				NX_TIMER_CanPowerDown
 */
CBOOL	NX_TIMER_CloseModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected module is busy or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicates that the module is busy. \n
 *				\b CFALSE	indicates that the module is idle.
 *	@see		NX_TIMER_GetPhysicalAddress,	NX_TIMER_GetSizeOfRegisterSet,
 *				NX_TIMER_SetBaseAddress,		NX_TIMER_GetBaseAddress,
 *				NX_TIMER_OpenModule,			NX_TIMER_CloseModule,
 *												NX_TIMER_CanPowerDown
 */
CBOOL	NX_TIMER_CheckBusy( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected module is ready to enter power-down stage.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicates that the module is ready to enter power-down stage. \n
 *				\b CFALSE	indicates that the module is busy and not proper to enter power-down stage.
 *	@see		NX_TIMER_GetPhysicalAddress,	NX_TIMER_GetSizeOfRegisterSet,
 *				NX_TIMER_SetBaseAddress,		NX_TIMER_GetBaseAddress,
 *				NX_TIMER_OpenModule,			NX_TIMER_CloseModule,
 *				NX_TIMER_CheckBusy
 */
CBOOL	NX_TIMER_CanPowerDown( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for the interrupt controller.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A interrupt number
 *	@see											NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
S32		NX_TIMER_GetInterruptNumber( U32 ModuleIndex )
{
	static	const S32 NX_TIMER_IntNumber[] = { INTNUM_OF_TIMER0_MODULE, INTNUM_OF_TIMER1_MODULE,
												INTNUM_OF_TIMER2_MODULE, INTNUM_OF_TIMER3_MODULE,
												INTNUM_OF_TIMER4_MODULE };

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );

	return	NX_TIMER_IntNumber[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			a interrupt Number .
 *	@param[in]	Enable	\b Set as CTRUE to enable a specified interrupt. \n
 *						\b Set as CFALSE to disable a specified interrupt.
 *	@return		None.
 *	@see		NX_TIMER_GetInterruptNumber,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
void	NX_TIMER_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable )
{
	register struct NX_TIMER_RegisterSet* pRegister;
	register U32	regvalue;

	const	U32 INTPEND_POS = 5;
	const	U32 INTENB_POS	= 4;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCONTROL;

	regvalue &=	~(( 1UL << INTPEND_POS ) | ( 1UL << INTENB_POS ) );
	regvalue |= (U32)Enable << INTENB_POS;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			a interrupt Number.
 *	@return		\b CTRUE	indicates that a specified interrupt is enabled. \n
 *				\b CFALSE	indicates that a specified interrupt is disabled.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *													NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
CBOOL	NX_TIMER_GetInterruptEnable( U32 ModuleIndex, S32 IntNum )
{
	const	U32 INTENB_POS	= 4;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)( ( __g_ModuleVariables[ModuleIndex].pRegister->TMRCONTROL >> INTENB_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set TIMER Counter	interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
void	NX_TIMER_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag )
{
	register struct NX_TIMER_RegisterSet* pRegister;
	register U32	regvalue;

	const	U32 INTPEND_POS = 5;
	const	U32 INTENB_POS	= 4;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCONTROL;

	regvalue &=	~(( 1UL << INTPEND_POS ) | ( 1UL << INTENB_POS ) );
	regvalue |= (U32)((EnableFlag&0x01) << INTENB_POS);

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : TIMER Counter interrupt's setting value. \n
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *													NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
U32		NX_TIMER_GetInterruptEnable32( U32 ModuleIndex )
{
	const	U32 INTENB_POS	= 4;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)( ( __g_ModuleVariables[ModuleIndex].pRegister->TMRCONTROL >> INTENB_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			a interrupt Number.
 *	@return		\b CTRUE	indicates that a specified interrupt is pended. \n
 *				\b CFALSE	indicates that a specified interrupt is not pended.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
CBOOL	NX_TIMER_GetInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	const	U32 INTPEND_POS	= 5;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)( ( __g_ModuleVariables[ModuleIndex].pRegister->TMRCONTROL >> INTPEND_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : TIMER Counter pending state. \n
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *													NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
U32		NX_TIMER_GetInterruptPending32( U32 ModuleIndex )
{
	const	U32 INTPEND_POS	= 5;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)( ( __g_ModuleVariables[ModuleIndex].pRegister->TMRCONTROL >> INTPEND_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			a interrupt number.
 *	@return		None.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
void	NX_TIMER_ClearInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	register struct NX_TIMER_RegisterSet* pRegister;
	register U32	regvalue;

	const	U32 INTPEND_POS	= 5;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCONTROL;

	regvalue |= 1UL << INTPEND_POS;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : TIMER Counter pending bit. \n
 *	@return		None.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *													NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
void	NX_TIMER_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag )
{
	register struct NX_TIMER_RegisterSet* pRegister;

	const	U32 INTPEND_POS		= 5;
	const	U32 INTPEND_MASK	= 1 << INTPEND_POS;

	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue  = pRegister->TMRCONTROL & ~INTPEND_MASK;
	regvalue |= (PendingFlag & 0x01) << INTPEND_POS;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Enable	\b Set as CTRUE to enable all interrupts. \n
 *						\b Set as CFALSE to disable all interrupts.
 *	@return		None.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
void	NX_TIMER_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable )
{
	register struct NX_TIMER_RegisterSet* pRegister;
	register U32	regvalue;

	const	U32 INTPEND_POS = 5;
	const	U32 INTENB_POS	= 4;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCONTROL;

	regvalue &=	~(( 1UL << INTPEND_POS ) | ( 1UL << INTENB_POS ));
	regvalue |= (U32)Enable << INTENB_POS ;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicates that one or more interrupts are enabled. \n
 *				\b CFALSE	indicates that all interrupts are disabled.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *													NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
CBOOL	NX_TIMER_GetInterruptEnableAll( U32 ModuleIndex )
{
	const	U32 INTENB_POS	= 4;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->TMRCONTROL >> INTENB_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicates that one or more interrupts are pended. \n
 *				\b CFALSE	indicates that no interrupt is pended.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,
 *				NX_TIMER_ClearInterruptPendingAll,	NX_TIMER_GetInterruptPendingNumber
 */
CBOOL	NX_TIMER_GetInterruptPendingAll( U32 ModuleIndex )
{
	const	U32 INTPEND_POS = 5;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)( ( __g_ModuleVariables[ModuleIndex].pRegister->TMRCONTROL >> INTPEND_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *													NX_TIMER_GetInterruptPendingNumber
 */
void	NX_TIMER_ClearInterruptPendingAll( U32 ModuleIndex )
{
	register struct NX_TIMER_RegisterSet* pRegister;

	const	 U32 INTPEND_POS = 5;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue  = pRegister->TMRCONTROL;
	regvalue |= 1UL << INTPEND_POS;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a interrupt number. A value of '-1' means that no interrupt is pended.
 *	@see		NX_TIMER_GetInterruptNumber,		NX_TIMER_SetInterruptEnable,
 *				NX_TIMER_GetInterruptEnable,		NX_TIMER_SetInterruptEnable32,
 *				NX_TIMER_GetInterruptEnable32,		NX_TIMER_GetInterruptPending,
 *				NX_TIMER_GetInterruptPending32,		NX_TIMER_ClearInterruptPending,
 *				NX_TIMER_ClearInterruptPending32,	NX_TIMER_SetInterruptEnableAll,
 *				NX_TIMER_GetInterruptEnableAll,		NX_TIMER_GetInterruptPendingAll,
 *				NX_TIMER_ClearInterruptPendingAll
 */
S32		NX_TIMER_GetInterruptPendingNumber( U32 ModuleIndex )	// -1 if None
{
	const	U32 INTPEND_POS	= 5;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->TMRCONTROL &	(1UL << INTPEND_POS) )
	{
		return 0;		// Timer module have only ONE pending bit. so return 0.
	}

	return -1;
}


//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	mode			the PCLK mode
 *	@return		None.
 *	@see											NX_TIMER_GetClockPClkMode,
 *				NX_TIMER_SetClockDivisorEnable,		NX_TIMER_GetClockDivisorEnable,
 *				NX_TIMER_SetClockSource,			NX_TIMER_GetClockSource,
 *				NX_TIMER_SetClockDivisor,			NX_TIMER_GetClockDivisor
 */
void			NX_TIMER_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode )
{
	U32 clkmode	=	0;
	const U32 TCLKMODE_POS	= 3;
	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->TMRCLKENB;

	regvalue &= ~(1UL<<TCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << TCLKMODE_POS;

//	pRegister->TMRCLKENB = regvalue;
	WriteIODW(&pRegister->TMRCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		the current PCLK mode
 *	@see		NX_TIMER_SetClockPClkMode,
 *				NX_TIMER_SetClockDivisorEnable,		NX_TIMER_GetClockDivisorEnable,
 *				NX_TIMER_SetClockSource,			NX_TIMER_GetClockSource,
 *				NX_TIMER_SetClockDivisor,			NX_TIMER_GetClockDivisor
 */
NX_PCLKMODE	NX_TIMER_GetClockPClkMode( U32 ModuleIndex )
{
	const U32 TCLKMODE_POS	= 3;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->TMRCLKENB & ( 1UL << TCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock generator to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Enable	\b Set as CTRUE to enable a clock generator. \n
 *						\b Set as CFALSE to disable a clock generator.
 *	@return		None.
 *	@see		NX_TIMER_SetClockPClkMode,			NX_TIMER_GetClockPClkMode,
 *													NX_TIMER_GetClockDivisorEnable,
 *				NX_TIMER_SetClockSource,			NX_TIMER_GetClockSource,
 *				NX_TIMER_SetClockDivisor,			NX_TIMER_GetClockDivisor
 */
void			NX_TIMER_SetClockDivisorEnable( U32 ModuleIndex, CBOOL Enable )
{
	const U32 CLKGENENB_POS	= 2;
	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCLKENB;

	regvalue &= ~(1UL		<< CLKGENENB_POS);
	regvalue |=	(U32)Enable << CLKGENENB_POS;

//	pRegister->TMRCLKENB = regvalue;
	WriteIODW(&pRegister->TMRCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a clock generator is enabled or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicates that a clock generator is enabled. \n
 *				\b CFALSE	indicates that a clock generator is disabled.
 *	@see		NX_TIMER_SetClockPClkMode,			NX_TIMER_GetClockPClkMode,
 *				NX_TIMER_SetClockDivisorEnable,
 *				NX_TIMER_SetClockSource,			NX_TIMER_GetClockSource,
 *				NX_TIMER_SetClockDivisor,			NX_TIMER_GetClockDivisor
 */
CBOOL			NX_TIMER_GetClockDivisorEnable( U32 ModuleIndex )
{
	const U32 CLKGENENB_POS	= 2;
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)( ( __g_ModuleVariables[ModuleIndex].pRegister->TMRCLKENB >> CLKGENENB_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock source for clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			an index of clock generator
 *	@param[in]	ClkSrc			a source clock for specified clock generator, 0 for PLL0 or 1 for PLL1.
 *	@return		None.
 *	@remarks	Since the timer module has only one clock generator, you have to only set @a Index as 0.
 *	@see		NX_TIMER_SetClockPClkMode,			NX_TIMER_GetClockPClkMode,
 *				NX_TIMER_SetClockDivisorEnable,		NX_TIMER_GetClockDivisorEnable,
 *													NX_TIMER_GetClockSource,
 *				NX_TIMER_SetClockDivisor,			NX_TIMER_GetClockDivisor
 */
void			NX_TIMER_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc )
{
	const U32	CLKSRCSEL_POS	= 2;
	const U32	CLKSRCSEL_MASK	= 0x7;
	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 0 == Index );
	NX_ASSERT( 1 >= ClkSrc );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCLKGEN;

	regvalue &= ~(CLKSRCSEL_MASK << CLKSRCSEL_POS);
	regvalue |= ( ClkSrc << CLKSRCSEL_POS );

//	pRegister->TMRCLKGEN = regvalue;
	WriteIODW(&pRegister->TMRCLKGEN, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current clock source for clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			an index of clock generator
 *	@return		a source clock for specified clock generator, 0 for PLL0 or 1 for PLL1.
 *	@remarks	Since the timer module has only one clock generator, you have to only set @a Index as 0.
 *	@see		NX_TIMER_SetClockPClkMode,			NX_TIMER_GetClockPClkMode,
 *				NX_TIMER_SetClockDivisorEnable,		NX_TIMER_GetClockDivisorEnable,
 *				NX_TIMER_SetClockSource,
 *				NX_TIMER_SetClockDivisor,			NX_TIMER_GetClockDivisor
 */
U32				NX_TIMER_GetClockSource( U32 ModuleIndex, U32 Index )
{
	const U32	CLKSRCSEL_POS	= 2;
	const U32	CLKSRCSEL_MASK	= 0x7;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 0 == Index );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)(( __g_ModuleVariables[ModuleIndex].pRegister->TMRCLKGEN >> CLKSRCSEL_POS ) & CLKSRCSEL_MASK );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock divider for clock generator.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			an index of clock generator
 *	@param[in]	Divisor		a divide value of clock generator from 1 to 256.
 *	@return		None.
 *	@remarks	Since the timer module has only one clock generator, you have to only set @a Index as 0.
 *	@see		NX_TIMER_SetClockPClkMode,			NX_TIMER_GetClockPClkMode,
 *				NX_TIMER_SetClockDivisorEnable,		NX_TIMER_GetClockDivisorEnable,
 *				NX_TIMER_SetClockSource,			NX_TIMER_GetClockSource,
 *													NX_TIMER_GetClockDivisor
 */
void			NX_TIMER_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divisor )
{
	const U32 CLKDIV_POS	= 5;
	const U32 CLKDIV_MASK	= 0xFF;

	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 0 == Index );
	NX_ASSERT( (1 <= Divisor) && ( 256 >= Divisor ) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCLKGEN;

	regvalue &= ~(CLKDIV_MASK << CLKDIV_POS);
	regvalue |= (Divisor-1) << CLKDIV_POS;

//	pRegister->TMRCLKGEN = regvalue;
	WriteIODW(&pRegister->TMRCLKGEN, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current clock divider of clock generator
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Index			an index of clock generator
 *	@return		a divide value of the specified clock generator.
 *	@remarks	Since the timer module has only one clock generator, you have to only set @a Index as 0.
 *	@see		NX_TIMER_SetClockPClkMode,			NX_TIMER_GetClockPClkMode,
 *				NX_TIMER_SetClockDivisorEnable,		NX_TIMER_GetClockDivisorEnable,
 *				NX_TIMER_SetClockSource,			NX_TIMER_GetClockSource,
 *				NX_TIMER_SetClockDivisor
 */
U32				NX_TIMER_GetClockDivisor( U32 ModuleIndex, U32 Index )
{
	const U32 CLKDIV_POS	= 5;
	const U32 CLKDIV_MASK	= 0xFF;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 0 == Index );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)(((__g_ModuleVariables[ModuleIndex].pRegister->TMRCLKGEN >> CLKDIV_POS ) & CLKDIV_MASK ) +1 ) ;
}

//------------------------------------------------------------------------------
// TIMER Operation.
//------------------------------------------------------------------------------
/**
 *	@brief		Run the timer.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@see										NX_TIMER_Stop,
 *				NX_TIMER_SetWatchDogEnable,		NX_TIMER_SetTClkDivider,
 *				NX_TIMER_SetMatchCounter,		NX_TIMER_GetMatchCounter,
 *				NX_TIMER_SetTimerCounter,		NX_TIMER_GetTimerCounter
 */
void NX_TIMER_Run( U32 ModuleIndex )
{
	const U32 INTPEND	= (1UL<<5);
	const U32 RUN		= (1UL<<3);

	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCONTROL;

	regvalue &= ~INTPEND;
	regvalue |=	RUN;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Stop the timer.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@see		NX_TIMER_Run,
 *				NX_TIMER_SetWatchDogEnable,		NX_TIMER_SetTClkDivider,
 *				NX_TIMER_SetMatchCounter,		NX_TIMER_GetMatchCounter,
 *				NX_TIMER_SetTimerCounter,		NX_TIMER_GetTimerCounter
 */
void NX_TIMER_Stop( U32 ModuleIndex )
{
	const U32 INTPEND	= (1UL<<5);
	const U32 RUN		= (1UL<<3);

	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCONTROL;

	regvalue &= ~(INTPEND | RUN);

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the watchdog function to be enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Enable	\b Set as CTRUE to use sepcified timer for the watchdog. \n
 *						\b Set as CFALSE to use specified timer for normal timer.
 *	@see		NX_TIMER_Run,					NX_TIMER_Stop,
 *												NX_TIMER_SetTClkDivider,
 *				NX_TIMER_SetMatchCounter,		NX_TIMER_GetMatchCounter,
 *				NX_TIMER_SetTimerCounter,		NX_TIMER_GetTimerCounter
 */
void NX_TIMER_SetWatchDogEnable( U32 ModuleIndex, CBOOL Enable )
{
	const U32 INTPEND	= (1UL<<5);
	const U32 WDENB		= (1UL<<2);

	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue  = pRegister->TMRCONTROL;

	regvalue &= ~(INTPEND|WDENB);

	if( Enable )	regvalue |= WDENB;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a internal clock divider for a timer clock.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Clock			a TCLK to be used.
 *	@see		NX_TIMER_Run,					NX_TIMER_Stop,
 *				NX_TIMER_SetWatchDogEnable,
 *				NX_TIMER_SetMatchCounter,		NX_TIMER_GetMatchCounter,
 *				NX_TIMER_SetTimerCounter,		NX_TIMER_GetTimerCounter
 */
void NX_TIMER_SetTClkDivider( U32 ModuleIndex, NX_TIMER_CLOCK Clock )
{
	const U32 INTPEND	= (1UL<<5);
	const U32 SELTCLK	= (3UL<<0);

	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( 4 > Clock );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue = pRegister->TMRCONTROL;

	regvalue &= ~(INTPEND | SELTCLK);
	regvalue |=	(U32)Clock;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set new match counter value.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	MatchCounter	New match counter value.
 *	@return		None.
 *	@see		NX_TIMER_Run,					NX_TIMER_Stop,
 *				NX_TIMER_SetWatchDogEnable,		NX_TIMER_SetTClkDivider,
 *												NX_TIMER_GetMatchCounter,
 *				NX_TIMER_SetTimerCounter,		NX_TIMER_GetTimerCounter
 */
void NX_TIMER_SetMatchCounter( U32 ModuleIndex, U32 MatchCounter )
{
	register struct NX_TIMER_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->TMRMATCH = MatchCounter;
	WriteIODW(&pRegister->TMRMATCH, MatchCounter);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a match counter value.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		current match counter value.
 *	@see		NX_TIMER_Run,					NX_TIMER_Stop,
 *				NX_TIMER_SetWatchDogEnable,		NX_TIMER_SetTClkDivider,
 *				NX_TIMER_SetMatchCounter,
 *				NX_TIMER_SetTimerCounter,		NX_TIMER_GetTimerCounter
 */
U32	NX_TIMER_GetMatchCounter( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	return __g_ModuleVariables[ModuleIndex].pRegister->TMRMATCH;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set new timer counter value.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	TimerCounter	New timer counter value.
 *	@return		None.
 *	@see		NX_TIMER_Run,					NX_TIMER_Stop,
 *				NX_TIMER_SetWatchDogEnable,		NX_TIMER_SetTClkDivider,
 *				NX_TIMER_SetMatchCounter,		NX_TIMER_GetMatchCounter,
 *												NX_TIMER_GetTimerCounter
 */
void NX_TIMER_SetTimerCounter( U32 ModuleIndex, U32 TimerCounter )
{
	register struct NX_TIMER_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->TMRCOUNT = TimerCounter;
	WriteIODW(&pRegister->TMRCOUNT, TimerCounter);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a timer counter value.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current timer counter value.
 *	@see		NX_TIMER_Run,					NX_TIMER_Stop,
 *				NX_TIMER_SetWatchDogEnable,		NX_TIMER_SetTClkDivider,
 *				NX_TIMER_SetMatchCounter,		NX_TIMER_GetMatchCounter,
 *				NX_TIMER_SetTimerCounter
 */
U32	NX_TIMER_GetTimerCounter( U32 ModuleIndex )
{
	const U32 INTPEND	= (1UL<<5);
	const U32 LDCNT		= (1UL<<6);

	register struct NX_TIMER_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_TIMER_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CTRUE == NX_TIMER_GetClockDivisorEnable(ModuleIndex) );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	regvalue = pRegister->TMRCONTROL;

	regvalue &= ~INTPEND;
	regvalue |=	LDCNT;

//	pRegister->TMRCONTROL = regvalue;
	WriteIODW(&pRegister->TMRCONTROL, regvalue);

	return pRegister->TMRCOUNT;
}
EXPORT_SYMBOL(NX_TIMER_GetTimerCounter);
