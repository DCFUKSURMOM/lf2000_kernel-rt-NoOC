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
//	Module		: DMA
//	File		: nx_dma.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//		2007/05/17	Sunny	rewrite ( using command buffer )
//		2007/08/16	Goofy	DMACMDBUSY -> Reserved
//------------------------------------------------------------------------------
#include "nx_dma.h"
#include <mach/nxp3200.h>

static	struct tag_ModuleVariables
{
	struct NX_DMA_RegisterSet *pRegister;
	U32		DMASRCADDR;
	U32		DMADSTADDR;
	U16		DMALENGTH;
	U16		DMAREQID;
	U32		DMAMODE;
	CBOOL	bCommandBufferMode;
} __g_ModuleVariables[NUMBER_OF_DMA_MODULE] = { {CNULL,}, };

//------------------------------------------------------------------------------
//	Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.\n
 *	@see	NX_DMA_GetNumberOfModule
 */
CBOOL	NX_DMA_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_DMA_MODULE; i++ )
		{
			__g_ModuleVariables[i].pRegister			= CNULL;
			__g_ModuleVariables[i].DMASRCADDR			= 0;
			__g_ModuleVariables[i].DMADSTADDR			= 0;
			__g_ModuleVariables[i].DMALENGTH			= 0;
			__g_ModuleVariables[i].DMAREQID				= 0;
			__g_ModuleVariables[i].DMAMODE				= 0;
			__g_ModuleVariables[i].bCommandBufferMode	= CFALSE;
		}
		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 *	@see		NX_DMA_Initialize
 */
U32		NX_DMA_GetNumberOfModule( void )
{
	return NUMBER_OF_DMA_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Module's physical address
 *	@see										NX_DMA_GetSizeOfRegisterSet,
 *				NX_DMA_SetBaseAddress,			NX_DMA_GetBaseAddress,
 *				NX_DMA_OpenModule,				NX_DMA_CloseModule,
 *				NX_DMA_CheckBusy,				NX_DMA_CanPowerDown
 */				
U32		NX_DMA_GetPhysicalAddress( U32 ModuleIndex )
{				
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	return	(U32)( PHY_BASEADDR_DMA_MODULE + (OFFSET_OF_DMA_MODULE * ModuleIndex) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_DMA_GetPhysicalAddress,
 *				NX_DMA_SetBaseAddress,			NX_DMA_GetBaseAddress,
 *				NX_DMA_OpenModule,				NX_DMA_CloseModule,
 *				NX_DMA_CheckBusy,				NX_DMA_CanPowerDown
 */
U32		NX_DMA_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_DMA_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	BaseAddress	Module's base address
 *	@return		None.
 *	@see		NX_DMA_GetPhysicalAddress,		NX_DMA_GetSizeOfRegisterSet,
 *												NX_DMA_GetBaseAddress,
 *				NX_DMA_OpenModule,				NX_DMA_CloseModule,
 *				NX_DMA_CheckBusy,				NX_DMA_CanPowerDown
 */
void	NX_DMA_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	__g_ModuleVariables[ModuleIndex].pRegister = (struct NX_DMA_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Module's base address.
 *	@see		NX_DMA_GetPhysicalAddress,		NX_DMA_GetSizeOfRegisterSet,
 *				NX_DMA_SetBaseAddress,
 *				NX_DMA_OpenModule,				NX_DMA_CloseModule,
 *				NX_DMA_CheckBusy,				NX_DMA_CanPowerDown
 */
U32		NX_DMA_GetBaseAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_DMA_GetPhysicalAddress,		NX_DMA_GetSizeOfRegisterSet,
 *				NX_DMA_SetBaseAddress,			NX_DMA_GetBaseAddress,
 *												NX_DMA_CloseModule,
 *				NX_DMA_CheckBusy,				NX_DMA_CanPowerDown
 */
CBOOL	NX_DMA_OpenModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT(	! NX_DMA_CheckBusy(ModuleIndex) );

//	__g_ModuleVariables[ModuleIndex].pRegister->DMASRCADDR	= 0;
//	__g_ModuleVariables[ModuleIndex].pRegister->DMADSTADDR	= 0;
//	__g_ModuleVariables[ModuleIndex].pRegister->DMALENGTH	= 0;
//	__g_ModuleVariables[ModuleIndex].pRegister->DMAREQID	= 0;
//	__g_ModuleVariables[ModuleIndex].pRegister->DMAMODE		= 1UL<<17;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->DMASRCADDR, 0);
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->DMADSTADDR, 0);
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->DMALENGTH, 0);
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->DMAREQID, 0);
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->DMAMODE, 1UL<<17);
	
	__g_ModuleVariables[ModuleIndex].DMASRCADDR				= 0;
	__g_ModuleVariables[ModuleIndex].DMADSTADDR				= 0;
	__g_ModuleVariables[ModuleIndex].DMALENGTH				= 0;
	__g_ModuleVariables[ModuleIndex].DMAREQID				= 0;
	__g_ModuleVariables[ModuleIndex].DMAMODE				= 0;
	__g_ModuleVariables[ModuleIndex].bCommandBufferMode		= CFALSE;

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_DMA_GetPhysicalAddress,		NX_DMA_GetSizeOfRegisterSet,
 *				NX_DMA_SetBaseAddress,			NX_DMA_GetBaseAddress,
 *				NX_DMA_OpenModule,
 *				NX_DMA_CheckBusy,				NX_DMA_CanPowerDown
 */
CBOOL	NX_DMA_CloseModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT(	! NX_DMA_CheckBusy(ModuleIndex) );

//	__g_ModuleVariables[ModuleIndex].pRegister->DMASRCADDR	= 0;
//	__g_ModuleVariables[ModuleIndex].pRegister->DMADSTADDR	= 0;
//	__g_ModuleVariables[ModuleIndex].pRegister->DMALENGTH	= 0;
//	__g_ModuleVariables[ModuleIndex].pRegister->DMAREQID	= 0;
//	__g_ModuleVariables[ModuleIndex].pRegister->DMAMODE		= 1UL<<17;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->DMASRCADDR, 0);
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->DMADSTADDR, 0);
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->DMALENGTH, 0);
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->DMAREQID, 0);
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->DMAMODE, 1UL<<17);

	__g_ModuleVariables[ModuleIndex].DMASRCADDR				= 0;
	__g_ModuleVariables[ModuleIndex].DMADSTADDR				= 0;
	__g_ModuleVariables[ModuleIndex].DMALENGTH				= 0;
	__g_ModuleVariables[ModuleIndex].DMAREQID				= 0;
	__g_ModuleVariables[ModuleIndex].DMAMODE				= 0;
	__g_ModuleVariables[ModuleIndex].bCommandBufferMode		= CFALSE;

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_DMA_GetPhysicalAddress,		NX_DMA_GetSizeOfRegisterSet,
 *				NX_DMA_SetBaseAddress,			NX_DMA_GetBaseAddress,
 *				NX_DMA_OpenModule,				NX_DMA_CloseModule,
 *												NX_DMA_CanPowerDown
 */
CBOOL	NX_DMA_CheckBusy( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	return NX_DMA_CheckRunning(ModuleIndex);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_DMA_GetPhysicalAddress,		NX_DMA_GetSizeOfRegisterSet,
 *				NX_DMA_SetBaseAddress,			NX_DMA_GetBaseAddress,
 *				NX_DMA_OpenModule,				NX_DMA_CloseModule,
 *				NX_DMA_CheckBusy
 */
CBOOL	NX_DMA_CanPowerDown( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	if( CTRUE == NX_DMA_CheckRunning(ModuleIndex) )
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
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Interrupt number
 *	@see											NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
S32		NX_DMA_GetInterruptNumber( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	return	INTNUM_OF_DMA_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Interrupt Number .
 *	@param[in]	Enable			\b CTRUE	indicate that Interrupt Enable. \n
 *								\b CFALSE	indicate that Interrupt Disable.
 *	@return		None.
 *	@see		NX_DMA_GetInterruptNumber,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
void	NX_DMA_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable )
{
	const U32 PEND_POS		= 17;
	const U32 PEND_MASK		= (1UL<<PEND_POS);
	const U32 INTENB_POS	= 18;
	const U32 INTENB_MASK	= (1UL<<INTENB_POS);

	register struct NX_DMA_RegisterSet *pRegister;
	register	U32 DmaMode;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	DmaMode = pRegister->DMAMODE;
	DmaMode &= ~(INTENB_MASK | PEND_MASK);
	DmaMode |= (Enable << INTENB_POS);
//	pRegister->DMAMODE = DmaMode;
	WriteIODW(&pRegister->DMAMODE, DmaMode);

	// We have to update local DMAMODE also for INTENB coherency.
	DmaMode = __g_ModuleVariables[ModuleIndex].DMAMODE;
	DmaMode &= ~(INTENB_MASK | PEND_MASK);
	DmaMode |= (Enable << INTENB_POS);
	__g_ModuleVariables[ModuleIndex].DMAMODE = DmaMode;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Interrupt Number.
 *	@return		\b CTRUE	indicate that Interrupt is enabled. \n
 *				\b CFALSE	indicate that Interrupt is disabled.
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *													NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
CBOOL	NX_DMA_GetInterruptEnable( U32 ModuleIndex, S32 IntNum )
{
	const U32 INTENB_POS	= 18;
	const U32 INTENB_MASK	= (1UL<<INTENB_POS);

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->DMAMODE & INTENB_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex	an index of module.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set Transfer Done interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
void	NX_DMA_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag )
{
	const U32 PEND_POS		= 17;
	const U32 PEND_MASK		= (1UL<<PEND_POS);
	const U32 INTENB_POS	= 18;
	const U32 INTENB_MASK	= (1UL<<INTENB_POS);

	register struct NX_DMA_RegisterSet *pRegister;
	register	U32 DmaMode;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	DmaMode = pRegister->DMAMODE;
	DmaMode &= ~(INTENB_MASK | PEND_MASK);
	DmaMode |= ((EnableFlag&0x01) << INTENB_POS);
//	pRegister->DMAMODE = DmaMode;
	WriteIODW(&pRegister->DMAMODE, DmaMode);

	// We have to update local DMAMODE also for INTENB coherency.
	DmaMode = __g_ModuleVariables[ModuleIndex].DMAMODE;
	DmaMode &= ~(INTENB_MASK | PEND_MASK);
	DmaMode |= ((EnableFlag&0x01) << INTENB_POS);
	__g_ModuleVariables[ModuleIndex].DMAMODE = DmaMode;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : Transfer Done interrupt's setting value. \n
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *													NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
U32		NX_DMA_GetInterruptEnable32( U32 ModuleIndex )
{
	const U32 INTENB_POS	= 18;
	const U32 INTENB_MASK	= (1UL<<INTENB_POS);

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)((__g_ModuleVariables[ModuleIndex].pRegister->DMAMODE & INTENB_MASK) >> INTENB_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Interrupt Number.
 *	@return		\b CTRUE	indicate that Pending is seted. \n
 *				\b CFALSE	indicate that Pending is Not Seted.
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
CBOOL	NX_DMA_GetInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	const U32 INTPEND_POS	= 17;
	const U32 INTPEND_MASK	= (1UL<<INTPEND_POS);

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->DMAMODE & INTPEND_MASK )
	{
		return CTRUE;
	}

	return CFALSE;

}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : Transfer Done pending state. \n
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *													NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
U32		NX_DMA_GetInterruptPending32( U32 ModuleIndex )
{
	const U32 INTPEND_POS	= 17;
	const U32 INTPEND_MASK	= (1UL<<INTPEND_POS);

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)((__g_ModuleVariables[ModuleIndex].pRegister->DMAMODE & INTPEND_MASK ) >> INTPEND_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	IntNum			Interrupt number.
 *	@return		None.
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
void	NX_DMA_ClearInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	const U32 INTPEND_POS	= 17;
	const U32 INTPEND_MASK	= (1UL<<INTPEND_POS);

	register	struct NX_DMA_RegisterSet		*pRegister;
	register	U32 DmaMode;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	DmaMode = pRegister->DMAMODE;

	DmaMode |= INTPEND_MASK;

//	pRegister->DMAMODE = DmaMode;
	WriteIODW(&pRegister->DMAMODE, DmaMode);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : Transfer Done pending bit. \n
 *	@return		None.
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *													NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
void	NX_DMA_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag )
{
	const U32 INTPEND_POS	= 17;
	const U32 INTPEND_MASK	= (1UL<<INTPEND_POS);

	register	struct NX_DMA_RegisterSet		*pRegister;
	register	U32 DmaMode;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	DmaMode = pRegister->DMAMODE & ~INTPEND_MASK;

	DmaMode |= (PendingFlag & 0x01) << INTPEND_POS;

//	pRegister->DMAMODE = DmaMode;
	WriteIODW(&pRegister->DMAMODE, DmaMode);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	Enable			\b CTRUE	indicate that Set to all interrupt enable. \n
 *								\b CFALSE	indicate that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
void	NX_DMA_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable )
{
	const U32 PEND_POS		= 17;
	const U32 PEND_MASK		= (1UL<<PEND_POS);
	const U32 INTENB_POS	= 18;
	const U32 INTENB_MASK	= (1UL<<INTENB_POS);

	register struct NX_DMA_RegisterSet *pRegister;
	register	U32 DmaMode;

	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	DmaMode = pRegister->DMAMODE;
	DmaMode &= ~(INTENB_MASK | PEND_MASK);
	DmaMode |= (Enable << INTENB_POS);
//	pRegister->DMAMODE = DmaMode;
	WriteIODW(&pRegister->DMAMODE, DmaMode);

	// We have to update local DMAMODE also for INTENB coherency.
	DmaMode = __g_ModuleVariables[ModuleIndex].DMAMODE;
	DmaMode &= ~(INTENB_MASK | PEND_MASK);
	DmaMode |= (Enable << INTENB_POS);
	__g_ModuleVariables[ModuleIndex].DMAMODE = DmaMode;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicate that All interrupt is disabled.
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *													NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
CBOOL	NX_DMA_GetInterruptEnableAll( U32 ModuleIndex )
{
	const U32 INTENB_POS	= 18;
	const U32 INTENB_MASK	= (1UL<<INTENB_POS);

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->DMAMODE_WB & INTENB_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicate that All pending is NOT seted.
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,
 *				NX_DMA_ClearInterruptPendingAll,	NX_DMA_GetInterruptPendingNumber
 */
CBOOL	NX_DMA_GetInterruptPendingAll( U32 ModuleIndex )
{
	const U32 INTPEND_POS	= 17;
	const U32 INTPEND_MASK	= (1UL<<INTPEND_POS);

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->DMAMODE & INTPEND_MASK )
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
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *													NX_DMA_GetInterruptPendingNumber
 */
void	NX_DMA_ClearInterruptPendingAll( U32 ModuleIndex )
{
	const U32 INTPEND_POS	= 17;
	const U32 INTPEND_MASK	= (1UL<<INTPEND_POS);

	register	struct NX_DMA_RegisterSet		*pRegister;
	register	U32 DmaMode;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	DmaMode = pRegister->DMAMODE;

	DmaMode |= INTPEND_MASK;

//	pRegister->DMAMODE = DmaMode;
	WriteIODW(&pRegister->DMAMODE, DmaMode);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_DMA_GetInterruptNumber,			NX_DMA_SetInterruptEnable,
 *				NX_DMA_GetInterruptEnable,			NX_DMA_SetInterruptEnable32,
 *				NX_DMA_GetInterruptEnable32,		NX_DMA_GetInterruptPending,
 *				NX_DMA_GetInterruptPending32,		NX_DMA_ClearInterruptPending,
 *				NX_DMA_ClearInterruptPending32,		NX_DMA_SetInterruptEnableAll,
 *				NX_DMA_GetInterruptEnableAll,		NX_DMA_GetInterruptPendingAll,
 *				NX_DMA_ClearInterruptPendingAll	
 */
S32		NX_DMA_GetInterruptPendingNumber( U32 ModuleIndex )	// -1 if None
{
	const U32 INTPEND_POS	= 17;
	const U32 INTPEND_MASK	= (1UL<<INTPEND_POS);

	register	struct NX_DMA_RegisterSet		*pRegister;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( pRegister->DMAMODE & INTPEND_MASK )
	{
		return 0;
	}

	return -1;
}


//------------------------------------------------------------------------------
// DMA Configuration Function
//------------------------------------------------------------------------------
/**
 *	@brief		Transfer memory to memory.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	pSource			source buffer address.\n this value must be aligned by 64 bits(8 bytes).
 *	@param[in]	pDestination	destination buffer address.\n this value must be aligned by 64 bits(8 bytes).
 *	@param[in]	TransferSize	transfer size in bytes.
 *	@return		None.
 *	@see										NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
void	NX_DMA_TransferMemToMem( U32 ModuleIndex, const void* pSource, void* pDestination, U32 TransferSize )
{
	const		U32	INTENB	= (1UL<<18);
	register	struct tag_ModuleVariables*		pVariables;
	register	U32	regvalue;

	NX_ASSERT( 0 == (((U32)pSource) % 8) );
	NX_ASSERT( 0 == (((U32)pDestination) % 8) );
	NX_ASSERT( (NX_DMA_MAX_TRANSFER_SIZE >= TransferSize) && (0 != TransferSize) );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pVariables = &__g_ModuleVariables[ModuleIndex];
	regvalue	= pVariables->DMAMODE & INTENB;

	pVariables->DMASRCADDR = (U32)pSource;
	pVariables->DMADSTADDR = (U32)pDestination;
	pVariables->DMALENGTH	= (U16)(TransferSize-1);

	regvalue	|= (U32)( 0
				| (0UL<< 0) // SRCIOSIZE 0:8bit 1:16bit 2:32bit
				| (0UL<< 2) // SRCIOMODE 0:memory to memory 1:i/o to memory
				| (0UL<< 3) // PACKMODE must be 0 (not supported)
				| (0UL<< 4) // SRCNOTINC 0:Normal, 1:not incremented
				| (1UL<< 5) // SRCNOTREQCHK 1:No Chk, 0:Chk Request, when SRCIOMODE=1
				| (0UL<< 8) // DSTIOSIZE 0:8bit 1:16bit 2:32bit
				| (0UL<<10) // DSTIOMODE 0:memory to memory 1:memory to i/o
				| (0UL<<12)	// DSTNOTINC 0:Normal, 1:not incremented
				| (1UL<<13)	// DSTNOTREQCHK	1:No Chk, 0:Chk Request, when DSTIOMODE=1
				| (1UL<<19) // RUN
				| (0UL<<20) // STOP
				);
	pVariables->DMAMODE = regvalue;

	NX_DMA_Run(ModuleIndex);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Transfer memory to peripheral.
 *	@param[in]	ModuleIndex				an index of module.
 *	@param[in]	pSource					Source buffer address.
 *	@param[in]	DestinationBaseAddress	A baseaddress of peripheral device.
 *	@param[in]	DestinationPeriID		An index of peripheral device.
 *	@param[in]	DestinationBitWidth		IO size, in bits, of peripheral device.
 *	@param[in]	TransferSize			Transfer size in bytes.
 *	@return		None.
 *	@see		NX_DMA_TransferMemToMem,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
void	NX_DMA_TransferMemToIO(	U32 ModuleIndex,
								const void* pSource,
								U32 DestinationBaseAddress,
								U32 DestinationPeriID,
								U32 DestinationBitWidth,
								U32 TransferSize )
{
	const		U32	INTENB	= (1UL<<18);
	register	struct tag_ModuleVariables*		pVariables;
	register	U32	regvalue;

	NX_ASSERT( 0 == (((U32)pSource) % 4) );
	NX_ASSERT( 64 > DestinationPeriID );
	NX_ASSERT( 8 == DestinationBitWidth || 16 == DestinationBitWidth || 32 == DestinationBitWidth );
	NX_ASSERT( (NX_DMA_MAX_TRANSFER_SIZE >= TransferSize) && (0 != TransferSize) );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pVariables = &__g_ModuleVariables[ModuleIndex];
	regvalue	= pVariables->DMAMODE & INTENB;

	pVariables->DMASRCADDR = (U32)pSource;
	pVariables->DMADSTADDR = (U32)DestinationBaseAddress;
	pVariables->DMALENGTH	= (U16)(TransferSize-1);
	pVariables->DMAREQID	= (U16)DestinationPeriID;

	regvalue	|= (U32)( 0
				| (0UL<< 0) // SRCIOSIZE 0:8bit 1:16bit 2:32bit
				| (0UL<< 2) // SRCIOMODE 0:memory to memory 1:i/o to memory
				| (0UL<< 3) // PACKMODE must be 0 (not supported)
				| (0UL<< 4) // SRCNOTINC 0:Normal, 1:not incremented
				| (1UL<< 5) // SRCNOTREQCHK 1:No Chk, 0:Chk Request, when SRCIOMODE=1
				| ((DestinationBitWidth>>4)<< 8) // DSTIOSIZE 0:8bit 1:16bit 2:32bit
				| (1UL<<10) // DSTIOMODE 0:memory to memory 1:memory to i/o
				| (1UL<<12)	// DSTNOTINC 0:Normal, 1:not incremented
				| (0UL<<13)	// DSTNOTREQCHK	1:No Chk, 0:Chk Request, when DSTIOMODE=1
				| (1UL<<19) // RUN
				| (0UL<<20) // STOP
				);
	pVariables->DMAMODE = regvalue;

	NX_DMA_Run(ModuleIndex);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Transfer peripheral to memory.
 *	@param[in]	ModuleIndex			an index of module.
 *	@param[in]	SourceBaseAddress	A baseaddress of peripheral device.
 *	@param[in]	SourcePeriID		An index of peripheral device.
 *	@param[in]	SourceBitWidth		IO size, in bits, of peripheral device.
 *	@param[in]	pDestination		Destination buffer address.
 *	@param[in]	TransferSize		Transfer size in bytes.
 *	@return		None.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *												NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
void	NX_DMA_TransferIOToMem(	U32 ModuleIndex,
								U32 SourceBaseAddress,
								U32 SourcePeriID,
								U32 SourceBitWidth,
								void* pDestination,
								U32 TransferSize )
{
	const		U32	INTENB	= (1UL<<18);
	register	struct tag_ModuleVariables*		pVariables;
	register	U32	regvalue;

	NX_ASSERT( 64 > SourcePeriID );
	NX_ASSERT( 8 == SourceBitWidth || 16 == SourceBitWidth || 32 == SourceBitWidth );
	NX_ASSERT( 0 == (((U32)pDestination) % 4) );
	NX_ASSERT( (NX_DMA_MAX_TRANSFER_SIZE >= TransferSize) && (0 != TransferSize) );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pVariables = &__g_ModuleVariables[ModuleIndex];
	regvalue	= pVariables->DMAMODE & INTENB;

	pVariables->DMASRCADDR = (U32)SourceBaseAddress;
	pVariables->DMADSTADDR = (U32)pDestination;
	pVariables->DMALENGTH	= (U16)(TransferSize-1);
	pVariables->DMAREQID	= (U16)SourcePeriID;

#if 1	// 29aug11
	if (SourceBaseAddress == PHY_BASEADDR_NAND_STATIC) {
		regvalue	|= (U32)( 0
					| ((SourceBitWidth>>4)<< 0) // SRCIOSIZE 0:8bit 1:16bit 2:32bit
					| (1UL<< 2) // SRCIOMODE 0:memory to memory 1:i/o to memory
					| (0UL<< 3) // PACKMODE must be 0 (not supported)
					| (1UL<< 4) // SRCNOTINC 0:Normal, 1:not incremented
					| (1UL<< 5) // SRCNOTREQCHK 1:No Chk, 0:Chk Request, when SRCIOMODE=1
					| (0UL<< 8) // DSTIOSIZE 0:8bit 1:16bit 2:32bit
					| (0UL<<10) // DSTIOMODE 0:memory to memory 1:memory to i/o
					| (0UL<<12)	// DSTNOTINC 0:Normal, 1:not incremented
					| (1UL<<13)	// DSTNOTREQCHK	1:No Chk, 0:Chk Request, when DSTIOMODE=1
					| (1UL<<19) // RUN
					| (0UL<<20) // STOP
					);
	}
	else
#endif
	regvalue	|= (U32)( 0
				| ((SourceBitWidth>>4)<< 0) // SRCIOSIZE 0:8bit 1:16bit 2:32bit
				| (1UL<< 2) // SRCIOMODE 0:memory to memory 1:i/o to memory
				| (0UL<< 3) // PACKMODE must be 0 (not supported)
				| (1UL<< 4) // SRCNOTINC 0:Normal, 1:not incremented
				| (0UL<< 5) // SRCNOTREQCHK 1:No Chk, 0:Chk Request, when SRCIOMODE=1
				| (0UL<< 8) // DSTIOSIZE 0:8bit 1:16bit 2:32bit
				| (0UL<<10) // DSTIOMODE 0:memory to memory 1:memory to i/o
				| (0UL<<12)	// DSTNOTINC 0:Normal, 1:not incremented
				| (1UL<<13)	// DSTNOTREQCHK	1:No Chk, 0:Chk Request, when DSTIOMODE=1
				| (1UL<<19) // RUN
				| (0UL<<20) // STOP
				);
	pVariables->DMAMODE = regvalue;

	NX_DMA_Run(ModuleIndex);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a maximum size for DMA transfer at once.
 *	@return		A maximum transfer size.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
U32	NX_DMA_GetMaxTransferSize ( void )
{
	return	NX_DMA_MAX_TRANSFER_SIZE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a source address.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	dwAddr			A source address
 *	@return		None.
 *	@remarks
 *				- If the source is peripheral device, set it to base address of peripheral device.
 *				- If the source is memory and destination is peripheral device,
 *					the source address must be aligned by IO size of peripheral device.
 *				- If the source and the destination are memory, source address must
 *					be aligned by 64 bit(8 bytes).
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *												NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
void	NX_DMA_SetSourceAddress( U32 ModuleIndex, U32 dwAddr )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	__g_ModuleVariables[ModuleIndex].DMASRCADDR = (U32)dwAddr;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a source address.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A source address.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
U32		NX_DMA_GetSourceAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->DMASRCADDR;

}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a destination address.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	dwAddr			A destination address.
 *	@return		None.
 *	@remarks
 *				- If the destination is peripheral device, set it to base address of
 *					peripheral device.
 *				- If the source is peripheral device and destination is memory,
 *					the destination address must be aligned by IO size of peripheral
 *					device.
 *				- If the source and the destination are memory, destination address
 *					must be aligned by 64 bit(8 bytes).
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *												NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
void	NX_DMA_SetDestinationAddress( U32 ModuleIndex, U32 dwAddr )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	__g_ModuleVariables[ModuleIndex].DMADSTADDR = (U32)dwAddr;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a destination address.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		a destination address.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
U32		NX_DMA_GetDestinationAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->DMADSTADDR;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a transfer size in bytes.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	dwSize			A transfer size in bytes.
 *	@return		None.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *												NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
void	NX_DMA_SetDataSize( U32 ModuleIndex, U32 dwSize )
{
	NX_ASSERT( (NX_DMA_MAX_TRANSFER_SIZE >= dwSize) && (0 != dwSize) );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if ( !__g_ModuleVariables[ModuleIndex].bCommandBufferMode )
	{
		__g_ModuleVariables[ModuleIndex].pRegister->DMALENGTH = (U16)(dwSize - 1);
	}
	__g_ModuleVariables[ModuleIndex].DMALENGTH = (U16)(dwSize - 1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a transfer size in bytes.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A transfer size in bytes.
 *	@remarks	This function informs a remained data size to be transferred while
 *				DMA transfer is running.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
U32		NX_DMA_GetDataSize( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister->DMALENGTH;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a peripheral index.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	dwIndex		A peripheral index.
 *	@return		None.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *												NX_DMA_GetIOIndex,
 *				NX_DMA_SetAttribute
 */
void	NX_DMA_SetIOIndex( U32 ModuleIndex, U32 dwIndex )
{
	NX_ASSERT( 64 > dwIndex );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	__g_ModuleVariables[ModuleIndex].DMAREQID = (U16)dwIndex;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a peripheral index.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		A peripheral index.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,
 *				NX_DMA_SetAttribute
 */
U32		NX_DMA_GetIOIndex( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].DMAREQID;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a DMA transfer mode.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	OpMode			A DMA transfer mode.
 *	@param[in]	IOBitWidth		IO size, in bits, of peripheral device.\n
 *								If OpMode is TRANSFER_MEM_TO_MEM, this parameter is not used.
 *	@param[in]	bSrcMemInc		Specifies whether to increase source address or not.\n
 *								This value is only used when source is memory and
 *								default	is CTRUE.
 *	@param[in]	bDstMemInc		Specifies whether to increase destination address or not.\n
 *								This value is only used when destination is memory and
 *								default	is CTRUE.
 *	@return		None.
 *	@remark		You have not to call this function while DMA transfer is busy.
 *	@see		NX_DMA_TransferMemToMem,		NX_DMA_TransferMemToIO,
 *				NX_DMA_TransferIOToMem,			NX_DMA_GetMaxTransferSize,
 *				NX_DMA_SetSourceAddress,		NX_DMA_GetSourceAddress,
 *				NX_DMA_SetDestinationAddress,	NX_DMA_GetDestinationAddress,
 *				NX_DMA_SetDataSize,				NX_DMA_GetDataSize,
 *				NX_DMA_SetIOIndex,				NX_DMA_GetIOIndex
 */
void	NX_DMA_SetAttribute( U32 ModuleIndex, NX_DMA_OPMODE OpMode, U32 IOBitWidth, CBOOL bSrcMemInc, CBOOL bDstMemInc )
{
	const U32	INTENB	= (1UL<<18);
	register	struct tag_ModuleVariables		*pVariables;
	register U32 regvalue;

	NX_ASSERT( 3 > OpMode );
	NX_ASSERT( NX_DMA_OPMODE_MEM_TO_MEM == OpMode || 8 == IOBitWidth || 16 == IOBitWidth || 32 == IOBitWidth );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	pVariables = &__g_ModuleVariables[ModuleIndex];
	regvalue = pVariables->DMAMODE & INTENB;

	switch( OpMode )
	{
	case NX_DMA_OPMODE_MEM_TO_MEM :
		regvalue	|= (U32)( 0
					| (0UL<< 0) // SRCIOSIZE 0:8bit 1:16bit 2:32bit
					| (0UL<< 2) // SRCIOMODE 0:memory to memory 1:i/o to memory
					| (0UL<< 3) // PACKMODE must be 0 (not supported)
					| (0UL<< 4) // SRCNOTINC 0:Normal, 1:not incremented
					| (1UL<< 5) // SRCNOTREQCHK 1:No Chk, 0:Chk Request, when SRCIOMODE=1
					| (0UL<< 8) // DSTIOSIZE 0:8bit 1:16bit 2:32bit
					| (0UL<<10) // DSTIOMODE 0:memory to memory 1:memory to i/o
					| (0UL<<12)	// DSTNOTINC 0:Normal, 1:not incremented
					| (1UL<<13)	// DSTNOTREQCHK	1:No Chk, 0:Chk Request, when DSTIOMODE=1
					| (0UL<<19) // RUN
					| (0UL<<20) // STOP
					);
		if( bSrcMemInc == CFALSE )	regvalue |= (1UL<< 4);	// SRCNOTINC
		if( bDstMemInc == CFALSE )	regvalue |= (1UL<<12);	// DSTNOTINC
		break;

	case NX_DMA_OPMODE_MEM_TO_IO :
		regvalue	|= (U32)( 0
					| (0UL<< 0) // SRCIOSIZE 0:8bit 1:16bit 2:32bit
					| (0UL<< 2) // SRCIOMODE 0:memory to memory 1:i/o to memory
					| (0UL<< 3) // PACKMODE must be 0 (not supported)
					| (0UL<< 4) // SRCNOTINC 0:Normal, 1:not incremented
					| (1UL<< 5) // SRCNOTREQCHK 1:No Chk, 0:Chk Request, when SRCIOMODE=1
					| ((IOBitWidth>>4)<< 8) // DSTIOSIZE 0:8bit 1:16bit 2:32bit
					| (1UL<<10) // DSTIOMODE 0:memory to memory 1:memory to i/o
					| (1UL<<12)	// DSTNOTINC 0:Normal, 1:not incremented
					| (0UL<<13)	// DSTNOTREQCHK	1:No Chk, 0:Chk Request, when DSTIOMODE=1
					| (0UL<<19) // RUN
					| (0UL<<20) // STOP
					);
		if( bSrcMemInc == CFALSE )	regvalue |= (1UL<< 4);	// SRCNOTINC
		break;

	case NX_DMA_OPMODE_IO_TO_MEM :
		regvalue	|= (U32)( 0
					| ((IOBitWidth>>4)<< 0) // SRCIOSIZE 0:8bit 1:16bit 2:32bit
					| (1UL<< 2) // SRCIOMODE 0:memory to memory 1:i/o to memory
					| (0UL<< 3) // PACKMODE must be 0 (not supported)
					| (1UL<< 4) // SRCNOTINC 0:Normal, 1:not incremented
					| (0UL<< 5) // SRCNOTREQCHK 1:No Chk, 0:Chk Request, when SRCIOMODE=1
					| (0UL<< 8) // DSTIOSIZE 0:8bit 1:16bit 2:32bit
					| (0UL<<10) // DSTIOMODE 0:memory to memory 1:memory to i/o
					| (0UL<<12)	// DSTNOTINC 0:Normal, 1:not incremented
					| (1UL<<13)	// DSTNOTREQCHK	1:No Chk, 0:Chk Request, when DSTIOMODE=1
					| (0UL<<19) // RUN
					| (0UL<<20) // STOP
					);
		if( bDstMemInc == CFALSE )	regvalue |= (1UL<<12);	// DSTNOTINC
		break;
	}
	pVariables->DMAMODE = regvalue;
}

//------------------------------------------------------------------------------
// DMA Operation Function
//------------------------------------------------------------------------------
/**
 *	@brief		Set DMA operation mode ( Write Back or Write Through )
 *	@param[in]	ModuleIndex		an index of module.
 *	@param[in]	bEnb			\b CTRUE	indicate that DMA opeate Write Back mode. \n
 *								\b CFALSE	indicate that DMA opeate Write Through mode.
 *	@return		None.
 *	@remarks	\b Write \b Back Mode can save next DMA setting value using command buffer during DMA is run.\n
 *				But, Save only one setting value.\n
 *					- 1. DMA setting
 *					- 2. DMA run
 *					- 3. save next DMA command
 *					- 4. if DMA is completed, saved DMA command execute.
 *					- 5. DMA complete
 *						- If next command exist, then save next DMA command ( repeat 3 - 4 ).
 *
 *	@remarks	\b Write \b Through Mode can't save next DMA command. so DMA operate below.
 *					- 1. DMA setting
 *					- 2. DMA run
 *					- 3. DMA complete
 *					- 4. repeat 1 - 3.
 *	@see										NX_DMA_GetCommandBufferMode,
 *				NX_DMA_Run,						NX_DMA_CheckRunning,
 *				NX_DMA_Stop,					NX_DMA_CommandBufferFlush,
 *				NX_DMA_IsCommandBufferFlush,	NX_DMA_IsCommandBufferReady,
 *				NX_DMA_IsCommandBufferEmpty,	NX_DMA_GetCommandBufferFreeCount
 */
void	NX_DMA_SetCommandBufferMode( U32 ModuleIndex, CBOOL bEnb )
{
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	__g_ModuleVariables[ModuleIndex].bCommandBufferMode = bEnb;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA operation mode
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that DMA opeation mode is Write Back. \n
 *				\b CFALSE	indicate that DMA opeation mode is Write Through.
 *	@see		NX_DMA_SetCommandBufferMode,
 *				NX_DMA_Run,						NX_DMA_CheckRunning,
 *				NX_DMA_Stop,					NX_DMA_CommandBufferFlush,
 *				NX_DMA_IsCommandBufferFlush,	NX_DMA_IsCommandBufferReady,
 *				NX_DMA_IsCommandBufferEmpty,	NX_DMA_GetCommandBufferFreeCount
 */
CBOOL	NX_DMA_GetCommandBufferMode( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );

	return __g_ModuleVariables[ModuleIndex].bCommandBufferMode;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Run DMA transfer.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remarks	DMA have different sequence by DMA operation mode ( Wirte Back or Write Through ). \n
 *	@code
 *				//--------------------------------------------------------------
 *				// Write Back Mode Operation
 *				//--------------------------------------------------------------
 *
 *				NX_DMA_SetCommandBufferMode( CTRUE );				// DMA set Write Back Mode
 *				...													// DMA configuration
 *
 *				while( CFALSE == NX_DMA_IsCommandBufferReady() )	// Check command buffer
 *				{
 *					CNULL;
 *				}
 *				NX_DMA_Run();										// DMA run
 *
 *				//--------------------------------------------------------------
 *				// Write Through Mode Operation
 *				//--------------------------------------------------------------
 *
 *				NX_DMA_SetCommandBufferMode( CFALSE );				// DMA set Write Through Mode
 *				...
 *
 *				NX_DMA_Run();										// DMA run

 *	@endcode
 *	@see		NX_DMA_SetCommandBufferMode,	NX_DMA_GetCommandBufferMode,
 *												NX_DMA_CheckRunning,
 *				NX_DMA_Stop,					NX_DMA_CommandBufferFlush,
 *				NX_DMA_IsCommandBufferFlush,	NX_DMA_IsCommandBufferReady,
 *				NX_DMA_IsCommandBufferEmpty,	NX_DMA_GetCommandBufferFreeCount
 */
void	NX_DMA_Run( U32 ModuleIndex )
{
	const U32 RUN		= (1UL<<19);
	NX_DEBUG_CODE(
	const U32 INTPEND	= (1UL<<17);
	const U32 STOP		= (1UL<<20); )

	register	struct	tag_ModuleVariables		*pVariables;
	register	struct	NX_DMA_RegisterSet		*pRegister;
	volatile	U32 *pSrcAddr, *pDestAddr;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	// save loacl variable DMAMODE for run bit setting
	pRegister	= __g_ModuleVariables[ModuleIndex].pRegister;
	pVariables	= &__g_ModuleVariables[ModuleIndex];

	NX_ASSERT( 0 == (pVariables->DMAMODE & (INTPEND|STOP) ) );
	pVariables->DMAMODE |= RUN;

	// command copy according to Write Back or Wirte Through Mode.
	pSrcAddr		=	&(pVariables->DMASRCADDR);

	NX_ASSERT( NX_DMA_GetCommandBufferMode(ModuleIndex) || CFALSE==NX_DMA_CheckRunning(ModuleIndex) );
	NX_ASSERT( !NX_DMA_GetCommandBufferMode(ModuleIndex) || NX_DMA_IsCommandBufferReady(ModuleIndex) );

	if( pVariables->bCommandBufferMode )
	{
		pDestAddr	=	&(pRegister->DMASRCADDR_WB);
//		pRegister->DMACMDWAIT = 1;	// command wait
		WriteIODW(&pRegister->DMACMDWAIT, 1);	// command wait
	}
	else	// write through
	{
		pDestAddr	=	&(pRegister->DMASRCADDR);
	}

//	*pDestAddr++ = *pSrcAddr++;
//	*pDestAddr++ = *pSrcAddr++;
//	*pDestAddr++ = *pSrcAddr++;
//	*pDestAddr++ = *pSrcAddr++;
	WriteIODW(pDestAddr++, *pSrcAddr++);
	WriteIODW(pDestAddr++, *pSrcAddr++);
	WriteIODW(pDestAddr++, *pSrcAddr++);
	WriteIODW(pDestAddr++, *pSrcAddr++);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check whether DMA transfer is running or not.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTURE	indicates DMA transfer is running.\n
 *				\b CFALSE	indicates DMA transfer is idle.
 *	@see	NX_DMA_SetCommandBufferMode,	NX_DMA_GetCommandBufferMode,
 *			NX_DMA_Run,
 *			NX_DMA_Stop,					NX_DMA_CommandBufferFlush,
 *			NX_DMA_IsCommandBufferFlush,	NX_DMA_IsCommandBufferReady,
 *			NX_DMA_IsCommandBufferEmpty,	NX_DMA_GetCommandBufferFreeCount
 */
CBOOL	NX_DMA_CheckRunning ( U32 ModuleIndex )
{
	const U32	BUSY_POS	= 16;
	const U32	BUSY_MASK	= (1UL<<BUSY_POS);
	register	struct NX_DMA_RegisterSet		*pRegister;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	return (CBOOL)((pRegister->DMAMODE & BUSY_MASK) >> BUSY_POS);
}


//------------------------------------------------------------------------------
/**
 *	@brief		Stop/Cancel DMA Transfer.
 *	@param[in]	ModuleIndex		an index of module.
 *	@param		Enable		\b CTRUE	indicate that DMA Stop is Enable. \n
 *							\b CFALSE	indicate that DMA Stop is Disable.
 *	@return		None.
 *	@remark		If DMA running write back mode, then user have to clear command buffer.\n
 *				Also, user should check command buffer flush is completed or not.\n \n
 *				After NX_DMA_Stop( CTRUE ), user should check DMA running status. \n
 *				If DMA is really stopped, user should setting NX_DMA_Stop( CFALSE ).
 *	@code
 *				if( NX_DMA_GetCommandBufferMode() )
 *				{
 *					NX_DMA_CommandBufferFlush();						// Command Buffer Flush
 *					while(	CTRUE != NX_DMA_IsCommandBufferFlush() )	// wait during command buffer flushing
 *					{ CNULL; }
 *				}
 *
 *				NX_DMA_Stop(CTRUE);
 *				while( NX_DMA_CheckRunning() )
 *				{
 *					CNULL;												// wait during DMA stop
 *				}
 *				NX_DMA_Stop(CFALSE);
 *	@endcode
 *	@see		NX_DMA_SetCommandBufferMode,	NX_DMA_GetCommandBufferMode,
 *				NX_DMA_Run,						NX_DMA_CheckRunning,
 *												NX_DMA_CommandBufferFlush,
 *				NX_DMA_IsCommandBufferFlush,	NX_DMA_IsCommandBufferReady,
 *				NX_DMA_IsCommandBufferEmpty,	NX_DMA_GetCommandBufferFreeCount
 */
void	NX_DMA_Stop ( U32 ModuleIndex, CBOOL Enable )
{
	const U32 RUN	= (1UL<<19);
	const U32 STOP	= (1UL<<20);

	register	struct NX_DMA_RegisterSet		*pRegister;
	register	U32 DmaMode;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	DmaMode = pRegister->DMAMODE;

	if( Enable )
	{
		DmaMode =	( DmaMode & ~RUN ) | STOP ;
	}
	else
	{
		DmaMode = ( DmaMode & ~STOP );
	}

//	pRegister->DMAMODE = DmaMode;
	WriteIODW(&pRegister->DMAMODE, DmaMode);
}

//------------------------------------------------------------------------------
/**
 *	@brief		DMA's command buffer flushing.
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		None.
 *	@remarks	After command buffer flushing, should check command buffer state.
 *	@code
 *				NX_DMA_CommandBufferFlush();						// command buffer flushing
 *				while( CTRUE != NX_DMA_IsCommandBufferFlush() )		// wait during command buffer flushing
 *				{
 *					CNULL;
 *				}
 *	@endcode
 *	@see		NX_DMA_SetCommandBufferMode,	NX_DMA_GetCommandBufferMode,
 *				NX_DMA_Run,						NX_DMA_CheckRunning,
 *				NX_DMA_Stop,
 *				NX_DMA_IsCommandBufferFlush,	NX_DMA_IsCommandBufferReady,
 *				NX_DMA_IsCommandBufferEmpty,	NX_DMA_GetCommandBufferFreeCount
 */
void	NX_DMA_CommandBufferFlush( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	__g_ModuleVariables[ModuleIndex].pRegister->DMACMDSTOP = 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get state of DMA's command buffer
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that command buffer flush is completed. \n
 *				\b CFALSE	indicate that command buffer flush is NOT completed.
 *	@remarks
 *	@see		NX_DMA_SetCommandBufferMode,	NX_DMA_GetCommandBufferMode,
 *				NX_DMA_Run,						NX_DMA_CheckRunning,
 *				NX_DMA_Stop,					NX_DMA_CommandBufferFlush,
 *												NX_DMA_IsCommandBufferReady,
 *				NX_DMA_IsCommandBufferEmpty,	NX_DMA_GetCommandBufferFreeCount
 */
CBOOL	NX_DMA_IsCommandBufferFlush( U32 ModuleIndex )
{
	const U32 STOP_MASK = 1UL;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->DMACMDSTOP & STOP_MASK) ^ 0x01);	// 0: Complete,	1: flushing.
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check DMA's available command buffer
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that command buffer is available. \n
 *				\b CFALSE	indicate that command buffer is NOT available. \n
 *	@see		NX_DMA_SetCommandBufferMode,	NX_DMA_GetCommandBufferMode,
 *				NX_DMA_Run,						NX_DMA_CheckRunning,
 *				NX_DMA_Stop,					NX_DMA_CommandBufferFlush,
 *				NX_DMA_IsCommandBufferFlush,	NX_DMA_IsCommandBufferEmpty, 
 *				NX_DMA_GetCommandBufferFreeCount
 */
CBOOL	NX_DMA_IsCommandBufferReady( U32 ModuleIndex )
{
	register U32 reg;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	reg = __g_ModuleVariables[ModuleIndex].pRegister->DMACMDEMPTY;
	return (CBOOL)((reg >> 2) & (reg & 1));	// DMACMDEMPTY[2] & DMACMDEMPTY[0]
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check command buffer is empty or Not
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		\b CTRUE	indicate that command buffer is Empty. \n
 *				\b CFALSE	indicate that command buffer is NOT Empty. \n
 *	@see		NX_DMA_SetCommandBufferMode,	NX_DMA_GetCommandBufferMode,
 *				NX_DMA_Run,						NX_DMA_CheckRunning,
 *				NX_DMA_Stop,					NX_DMA_CommandBufferFlush,
 *				NX_DMA_IsCommandBufferFlush,	NX_DMA_GetCommandBufferFreeCount
 */
CBOOL	NX_DMA_IsCommandBufferEmpty( U32 ModuleIndex )
{
	register U32 reg;

	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	reg = __g_ModuleVariables[ModuleIndex].pRegister->DMACMDEMPTY;
	return (CBOOL)((reg >> 2) & (reg & 1));	// DMACMDEMPTY[2] & DMACMDEMPTY[0]
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check count of DMA's available command buffer
 *	@param[in]	ModuleIndex		an index of module.
 *	@return		Get count of DMA's available command buffer
 *	@see		NX_DMA_SetCommandBufferMode,	NX_DMA_GetCommandBufferMode,
 *				NX_DMA_Run,						NX_DMA_CheckRunning,
 *				NX_DMA_Stop,					NX_DMA_CommandBufferFlush,
 *				NX_DMA_IsCommandBufferFlush,	NX_DMA_IsCommandBufferEmpty
 */
U32		NX_DMA_GetCommandBufferFreeCount( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_DMA_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->DMACMDEMPTY;
}
