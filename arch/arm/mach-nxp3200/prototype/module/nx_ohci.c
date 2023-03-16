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
//	Module		: USB Host Controller
//	File		: nx_ohci.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_ohci.h"

static	struct
{
	struct NX_OHCI_RegisterSet *pRegister;

} __g_ModuleVariables[NUMBER_OF_OHCI_MODULE] = { {CNULL,}, };

static	U32 __g_CurModuleIndex = 0;
static	struct NX_OHCI_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
//	Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize of prototype enviroment & local variables.
 *	@return		\b CTRUE	indicate that Initialize is successed.\n
 *				\b CFALSE	indicate that Initialize is failed.\n
 *	@see		NX_OHCI_GetNumberOfModule
 */
CBOOL	NX_OHCI_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		__g_CurModuleIndex = 0;

		for( i=0; i < NUMBER_OF_OHCI_MODULE; i++ )
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
 *	@see		NX_OHCI_Initialize
 */
U32		NX_OHCI_GetNumberOfModule( void )
{
	return NUMBER_OF_OHCI_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_OHCI_GetSizeOfRegisterSet,
 *				NX_OHCI_SetBaseAddress,		NX_OHCI_GetBaseAddress,
 *				NX_OHCI_OpenModule,			NX_OHCI_CloseModule,
 *				NX_OHCI_CheckBusy,			NX_OHCI_CanPowerDown
 */
U32		NX_OHCI_GetPhysicalAddress( void )
{
	NX_ASSERT( NUMBER_OF_OHCI_MODULE > __g_CurModuleIndex );

	return	(U32)( PHY_BASEADDR_OHCI_MODULE + (OFFSET_OF_OHCI_MODULE * __g_CurModuleIndex) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_OHCI_GetPhysicalAddress,
 *				NX_OHCI_SetBaseAddress,		NX_OHCI_GetBaseAddress,
 *				NX_OHCI_OpenModule,			NX_OHCI_CloseModule,
 *				NX_OHCI_CheckBusy,			NX_OHCI_CanPowerDown
 */
U32		NX_OHCI_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_OHCI_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_OHCI_GetPhysicalAddress,	NX_OHCI_GetSizeOfRegisterSet,
 *				NX_OHCI_GetBaseAddress,
 *				NX_OHCI_OpenModule,			NX_OHCI_CloseModule,
 *				NX_OHCI_CheckBusy,			NX_OHCI_CanPowerDown
 */
void	NX_OHCI_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	NX_ASSERT( NUMBER_OF_OHCI_MODULE > __g_CurModuleIndex );

	__g_ModuleVariables[__g_CurModuleIndex].pRegister = (struct NX_OHCI_RegisterSet *)BaseAddress;
	__g_pRegister = (struct NX_OHCI_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_OHCI_GetPhysicalAddress,	NX_OHCI_GetSizeOfRegisterSet,
 *				NX_OHCI_SetBaseAddress,
 *				NX_OHCI_OpenModule,			NX_OHCI_CloseModule,
 *				NX_OHCI_CheckBusy,			NX_OHCI_CanPowerDown
 */
U32		NX_OHCI_GetBaseAddress( void )
{
	NX_ASSERT( NUMBER_OF_OHCI_MODULE > __g_CurModuleIndex );

	return (U32)__g_ModuleVariables[__g_CurModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_OHCI_GetPhysicalAddress,	NX_OHCI_GetSizeOfRegisterSet,
 *				NX_OHCI_SetBaseAddress,		NX_OHCI_GetBaseAddress,
 *				NX_OHCI_CloseModule,
 *				NX_OHCI_CheckBusy,			NX_OHCI_CanPowerDown
 */
CBOOL	NX_OHCI_OpenModule( void )
{
		return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_OHCI_GetPhysicalAddress,	NX_OHCI_GetSizeOfRegisterSet,
 *				NX_OHCI_SetBaseAddress,		NX_OHCI_GetBaseAddress,
 *				NX_OHCI_OpenModule,
 *				NX_OHCI_CheckBusy,			NX_OHCI_CanPowerDown
 */
CBOOL	NX_OHCI_CloseModule( void )
{
		return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_OHCI_GetPhysicalAddress,	NX_OHCI_GetSizeOfRegisterSet,
 *				NX_OHCI_SetBaseAddress,		NX_OHCI_GetBaseAddress,
 *				NX_OHCI_OpenModule,			NX_OHCI_CloseModule,
 *				NX_OHCI_CanPowerDown
 */
CBOOL	NX_OHCI_CheckBusy( void )
{
		return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_OHCI_GetPhysicalAddress,	NX_OHCI_GetSizeOfRegisterSet,
 *				NX_OHCI_SetBaseAddress,		NX_OHCI_GetBaseAddress,
 *				NX_OHCI_OpenModule,			NX_OHCI_CloseModule,
 *				NX_OHCI_CheckBusy
 */
CBOOL	NX_OHCI_CanPowerDown( void )
{
	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see											NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
S32		NX_OHCI_GetInterruptNumber( void )
{
	return	INTNUM_OF_OHCI_MODULE;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0 ~ 6, 30, 31 ).
 *	@param[in]	Enable	\b CTRUE indicate that	Interrupt Enable. \n
 *						\b CFALSE indicate that	Interrupt Disable.
 *	@return		None.
 *	@see		NX_OHCI_GetInterruptNumber,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
void	NX_OHCI_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	register struct NX_OHCI_RegisterSet*	pRegister;
	register U16 regvalue;

	NX_ASSERT( (NX_OHCI_INT_RHSC < IntNum) && (NX_OHCI_INT_OC > IntNum) );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = pRegister->HcInterruptEnable;

	regvalue &= ~( 1 << IntNum );
	regvalue |= ((U16)Enable << IntNum );

	pRegister->HcInterruptEnable	= regvalue;
	pRegister->HcInterruptDisable = ~regvalue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number ( 0 ~ 6, 30, 31 ).
 *	@return		\b CTRUE indicate that	Interrupt is enabled. \n
 *				\b CFALSE indicate that	Interrupt is disabled.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *													NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
CBOOL	NX_OHCI_GetInterruptEnable( S32 IntNum )
{
	NX_ASSERT( (NX_OHCI_INT_RHSC < IntNum) && (NX_OHCI_INT_OC > IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->HcInterruptEnable & (1<<IntNum));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set Scheduling Overrun		interrupt enable or disable. \n
 *							- EnableFlag[1] : Set HcDoneHead Writeback		interrupt enable or disable. \n
 *							- EnableFlag[2] : Set Start of Frame			interrupt enable or disable. \n
 *							- EnableFlag[3] : Set Resume Detect			interrupt enable or disable. \n
 *							- EnableFlag[4] : Set Unrecoverable Error		interrupt enable or disable. \n
 *							- EnableFlag[5] : Set Frame Number Overflow	interrupt enable or disable. \n
 *							- EnableFlag[6] : Set Root Hub Status Change interrupt enable or disable. \n
 *							- EnableFlag[30]: Set Ownership Chagne			interrupt enable or disable. \n
 *							- EnableFlag[31]: Set Master					interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
void	NX_OHCI_SetInterruptEnable32( U32 EnableFlag )
{
	const U32	ENB_MASK = ((3UL<<30) | ((1UL<<7)-1));

	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->HcInterruptEnable	=	(EnableFlag & ENB_MASK);
	__g_pRegister->HcInterruptDisable = ~(EnableFlag & ENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : Scheduling Overrun		interrupt's setting value. \n
 *				- Return Value[1] : HcDoneHead Writeback	interrupt's setting value. \n
 *				- Return Value[2] : Start of Frame			interrupt's setting value. \n
 *				- Return Value[3] : Resume Detect			interrupt's setting value. \n
 *				- Return Value[4] : Unrecoverable Error		interrupt's setting value. \n
 *				- Return Value[5] : Frame Number Overflow	interrupt's setting value. \n
 *				- Return Value[6] : Root Hub Status Change	interrupt's setting value. \n
 *				- Return Value[30]: Ownership Chagne		interrupt's setting value. \n
 *				- Return Value[31]: Master					interrupt's setting value. \n
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *													NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
U32		NX_OHCI_GetInterruptEnable32( void )
{
	const U32	ENB_MASK = ((3UL<<30) | ((1UL<<7)-1));

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->HcInterruptEnable & ENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number ( 0 ~ 6, 30, 31 ).
 *	@return		\b CTRUE	indicate that	Pending is seted. \n
 *				\b CFALSE	indicate that	Pending is Not Seted.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
CBOOL	NX_OHCI_GetInterruptPending( S32 IntNum )
{
	NX_ASSERT( (NX_OHCI_INT_RHSC < IntNum) && (NX_OHCI_INT_OC > IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->HcInterruptStatus & (1<<IntNum));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : Scheduling Overrun		pending state. \n
 *				- Return Value[1] : HcDoneHead Writeback	pending state. \n
 *				- Return Value[2] : Start of Frame			pending state. \n
 *				- Return Value[3] : Resume Detect			pending state. \n
 *				- Return Value[4] : Unrecoverable Error		pending state. \n
 *				- Return Value[5] : Frame Number Overflow	pending state. \n
 *				- Return Value[6] : Root Hub Status Change	pending state. \n
 *				- Return Value[30]: Ownership Chagne		pending state. \n
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *													NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
U32		NX_OHCI_GetInterruptPending32( void )
{
	const U32	PEND_MASK = (1UL<<30) | ((1U<<7)-1);

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->HcInterruptStatus & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number ( 0 ~ 6, 30 ).
 *	@return		None.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
void	NX_OHCI_ClearInterruptPending( S32 IntNum )
{
	NX_ASSERT( (NX_OHCI_INT_OC == IntNum) || (NX_OHCI_INT_RHSC >= IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->HcInterruptStatus = (1<<IntNum);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : Scheduling Overrun		pending bit. \n
 *								- PendingFlag[1] : HcDoneHead Writeback		pending bit. \n
 *								- PendingFlag[2] : Start of Frame			pending bit. \n
 *								- PendingFlag[3] : Resume Detect			pending bit. \n
 *								- PendingFlag[4] : Unrecoverable Error		pending bit. \n
 *								- PendingFlag[5] : Frame Number Overflow	pending bit. \n
 *								- PendingFlag[6] : Root Hub Status Change	pending bit. \n
 *								- PendingFlag[30]: Ownership Chagne			pending bit. \n
 *	@return		None.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *													NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
void	NX_OHCI_ClearInterruptPending32( U32 PendingFlag )
{
	const U32	PEND_MASK = (1UL<<30) | ((1U<<7)-1);

	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->HcInterruptStatus = (PendingFlag & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE indicate that	Set to all interrupt enable. \n
 *						\b CFALSE indicate that	Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
void	NX_OHCI_SetInterruptEnableAll( CBOOL Enable )
{
	const U32 INTENBMASK = 0xC000007F;

	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	if( Enable )
	{
		__g_pRegister->HcInterruptEnable = INTENBMASK;
		__g_pRegister->HcInterruptDisable = ~INTENBMASK;
	}
	else
	{
		__g_pRegister->HcInterruptEnable = ~INTENBMASK;
		__g_pRegister->HcInterruptDisable = INTENBMASK;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicate that	At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicate that	All interrupt is disabled.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *													NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
CBOOL	NX_OHCI_GetInterruptEnableAll( void )
{
	const U32 INTENBMASK = 0xC000007F;
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->HcInterruptEnable & INTENBMASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicate that	At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicate that	All pending is NOT seted.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,
 *				NX_OHCI_ClearInterruptPendingAll,	NX_OHCI_GetInterruptPendingNumber
 */
CBOOL	NX_OHCI_GetInterruptPendingAll( void )
{
	const U32 INTPENDMASK = 0xC000007F;
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->HcInterruptStatus & INTPENDMASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *													NX_OHCI_GetInterruptPendingNumber
 */
void	NX_OHCI_ClearInterruptPendingAll( void )
{
	const U32 INTPENDMASK = 0xC000007F;
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->HcInterruptStatus = INTPENDMASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_OHCI_GetInterruptNumber,			NX_OHCI_SetInterruptEnable,
 *				NX_OHCI_GetInterruptEnable,			NX_OHCI_SetInterruptEnable32,
 *				NX_OHCI_GetInterruptEnable32,		NX_OHCI_GetInterruptPending,
 *				NX_OHCI_GetInterruptPending32,		NX_OHCI_ClearInterruptPending,
 *				NX_OHCI_ClearInterruptPending32,	NX_OHCI_SetInterruptEnableAll,
 *				NX_OHCI_GetInterruptEnableAll,		NX_OHCI_GetInterruptPendingAll,
 *				NX_OHCI_ClearInterruptPendingAll
 */
S32		NX_OHCI_GetInterruptPendingNumber( void )	// -1 if None
{
	U32 index;
	U32 IntPend;
	U32 IntEnb;
	U32	IntBitPos[9] = { 0, 1, 2, 3, 4, 5, 6, 30, 31 };
	register struct NX_OHCI_RegisterSet	*pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	IntEnb	= pRegister->HcInterruptEnable;
	IntPend = pRegister->HcInterruptStatus;

	IntPend &= IntEnb;


	for( index=0; index < 9; index++ )
	{
		if( IntPend & (1<<IntBitPos[index]) )
			return	IntBitPos[index];
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
 *	@see										NX_OHCI_GetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,		NX_OHCI_GetClockBClkMode,
 *				NX_OHCI_SetClockSource,			NX_OHCI_GetClockSource,
 *				NX_OHCI_SetClockDivisor,		NX_OHCI_GetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable,	NX_OHCI_GetClockDivisorEnable
 */
void			NX_OHCI_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_OHCI_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:		clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->CLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

	pRegister->CLKENB = regvalue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@return		Current PCLK mode
 *	@see		NX_OHCI_SetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,		NX_OHCI_GetClockBClkMode,
 *				NX_OHCI_SetClockSource,			NX_OHCI_GetClockSource,
 *				NX_OHCI_SetClockDivisor,		NX_OHCI_GetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable,	NX_OHCI_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_OHCI_GetClockPClkMode( void )
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
 *	@brief		Set a BCLK mode
 *	@param[in]	mode	BCLK mode
 *	@return		None.
 *	@see		NX_OHCI_SetClockPClkMode,		NX_OHCI_GetClockPClkMode,
 *												NX_OHCI_GetClockBClkMode,
 *				NX_OHCI_SetClockSource,			NX_OHCI_GetClockSource,
 *				NX_OHCI_SetClockDivisor,		NX_OHCI_GetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable,	NX_OHCI_GetClockDivisorEnable
 */
void			NX_OHCI_SetClockBClkMode( NX_BCLKMODE mode )
{
	register U32 regvalue;
	register struct NX_OHCI_RegisterSet* pRegister;
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

	regvalue = pRegister->CLKENB;
	regvalue &= ~(0x3);
	regvalue |= clkmode & 0x3;
	pRegister->CLKENB = regvalue;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Get current BCLK mode
 *	@return		Current BCLK mode
 *	@see		NX_OHCI_SetClockPClkMode,		NX_OHCI_GetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,
 *				NX_OHCI_SetClockSource,			NX_OHCI_GetClockSource,
 *				NX_OHCI_SetClockDivisor,		NX_OHCI_GetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable,	NX_OHCI_GetClockDivisorEnable
 */
NX_BCLKMODE	NX_OHCI_GetClockBClkMode( void )
{
	const U32 BCLKMODE	= 3UL<<0;

	NX_ASSERT( CNULL != __g_pRegister );


	switch( __g_pRegister->CLKENB & BCLKMODE )
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
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 ).
 *	@param[in]	ClkSrc	Select clock source of clock generator( 0: PLL0, 1: PLL1 ).
 *	@remarks	USB OHCI controller have one clock generator. so \e Index must set to 0.\n
 *	@return		None.
 *	@see		NX_OHCI_SetClockPClkMode,		NX_OHCI_GetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,		NX_OHCI_GetClockBClkMode,
 *												NX_OHCI_GetClockSource,
 *				NX_OHCI_SetClockDivisor,		NX_OHCI_GetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable,	NX_OHCI_GetClockDivisorEnable
 */
void			NX_OHCI_SetClockSource( U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS	= 2;
	const U32 CLKSRCSEL_MASK	= 0x03 << CLKSRCSEL_POS;

	register U32 ReadValue;

	NX_ASSERT( 1 > Index );
	NX_ASSERT( 2 > ClkSrc );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->CLKGEN;

	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;

	__g_pRegister->CLKGEN = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock source of specified clock generator.
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 ).
 *	@return		Clock source of clock generator( 0: PLL0, 1: PLL1 ).
 *	@remarks	USB Device controller have one clock generator. so \e Index must set to 0.\n
 *	@see		NX_OHCI_SetClockPClkMode,		NX_OHCI_GetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,		NX_OHCI_GetClockBClkMode,
 *				NX_OHCI_SetClockSource,
 *				NX_OHCI_SetClockDivisor,		NX_OHCI_GetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable,	NX_OHCI_GetClockDivisorEnable
 */
U32				NX_OHCI_GetClockSource( U32 Index )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x03 << CLKSRCSEL_POS;

	NX_ASSERT( 1 > Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return ( __g_pRegister->CLKGEN & CLKSRCSEL_MASK ) >> CLKSRCSEL_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock divisor of specified clock generator.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 );
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 64 ).
 *	@return		None.
 *	@remarks	USB OHCI controller have one clock generator. so \e Index must set to 0.\n
 *	@see		NX_OHCI_SetClockPClkMode,		NX_OHCI_GetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,		NX_OHCI_GetClockBClkMode,
 *				NX_OHCI_SetClockSource,			NX_OHCI_GetClockSource,
 *												NX_OHCI_GetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable,	NX_OHCI_GetClockDivisorEnable
 */
void			NX_OHCI_SetClockDivisor( U32 Index, U32 Divisor )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0x3F << CLKDIV_POS;

	register U32 ReadValue;

	NX_ASSERT( 1 > Index );
	NX_ASSERT( 1 <= Divisor && Divisor <= 64 );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->CLKGEN;

	ReadValue	&= ~CLKDIV_MASK;
	ReadValue	|= (Divisor-1) << CLKDIV_POS;

	__g_pRegister->CLKGEN = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock divisor of specified clock generator.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 );
 *	@return		Clock divisor ( 1 ~ 64 ).
 *	@remarks	USB OHCI controller have one clock generator. so \e Index must set to 0.\n
 *	@see		NX_OHCI_SetClockPClkMode,		NX_OHCI_GetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,		NX_OHCI_GetClockBClkMode,
 *				NX_OHCI_SetClockSource,			NX_OHCI_GetClockSource,
 *				NX_OHCI_SetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable,	NX_OHCI_GetClockDivisorEnable
 */
U32				NX_OHCI_GetClockDivisor( U32 Index )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0x3F << CLKDIV_POS;

	NX_ASSERT( 2 > Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->CLKGEN & CLKDIV_MASK) >> CLKDIV_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock generator's operation
 *	@param[in]	Enable	\b CTRUE indicate that Enable of clock generator. \n
 *						\b CFALSE indicate that Disable of clock generator.
 *	@return		None.
 *	@see		NX_OHCI_SetClockPClkMode,		NX_OHCI_GetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,		NX_OHCI_GetClockBClkMode,
 *				NX_OHCI_SetClockSource,			NX_OHCI_GetClockSource,
 *				NX_OHCI_SetClockDivisor,		NX_OHCI_GetClockDivisor,
 *												NX_OHCI_GetClockDivisorEnable
 */
void			NX_OHCI_SetClockDivisorEnable( CBOOL Enable )
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
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of clock generator's operation
 *	@return		\b CTRUE	indicate that Clock generator is enabled.\n
 *				\b CFALSE	indicate that Clock generator is disabled.
 *	@see		NX_OHCI_SetClockPClkMode,		NX_OHCI_GetClockPClkMode,
 *				NX_OHCI_SetClockBClkMode,		NX_OHCI_GetClockBClkMode,
 *				NX_OHCI_SetClockSource,			NX_OHCI_GetClockSource,
 *				NX_OHCI_SetClockDivisor,		NX_OHCI_GetClockDivisor,
 *				NX_OHCI_SetClockDivisorEnable
 */
CBOOL			NX_OHCI_GetClockDivisorEnable( void )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (__g_pRegister->CLKENB & CLKGENENB_MASK) >> CLKGENENB_POS );
}
