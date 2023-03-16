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
//	Module		: MLC
//	File		: nx_mlc.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_mlc.h"

static	struct
{
	struct NX_MLC_RegisterSet *pRegister;

} __g_ModuleVariables[NUMBER_OF_MLC_MODULE] = { {CNULL,}, };

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicates that Initialize is successed.\n
 *			\b CFALSE	indicates that Initialize is failed.\n
 *	@see	NX_MLC_GetNumberOfModule
 */
CBOOL	NX_MLC_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_MLC_MODULE; i++ )
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
 *	@see		NX_MLC_Initialize
 */
U32		NX_MLC_GetNumberOfModule( void )
{
	return NUMBER_OF_MLC_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		Module's physical address
 *	@see		NX_MLC_GetSizeOfRegisterSet,
 *				NX_MLC_SetBaseAddress,		NX_MLC_GetBaseAddress,
 *				NX_MLC_OpenModule,			NX_MLC_CloseModule,
 *				NX_MLC_CheckBusy,			NX_MLC_CanPowerDown
 */
U32		NX_MLC_GetPhysicalAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );

	return	(U32)( PHY_BASEADDR_MLC_MODULE + (OFFSET_OF_MLC_MODULE * ModuleIndex) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_MLC_GetPhysicalAddress,
 *				NX_MLC_SetBaseAddress,		NX_MLC_GetBaseAddress,
 *				NX_MLC_OpenModule,			NX_MLC_CloseModule,
 *				NX_MLC_CheckBusy,			NX_MLC_CanPowerDown
 */
U32		NX_MLC_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_MLC_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_MLC_GetPhysicalAddress,	NX_MLC_GetSizeOfRegisterSet,
 *				NX_MLC_GetBaseAddress,
 *				NX_MLC_OpenModule,			NX_MLC_CloseModule,
 *				NX_MLC_CheckBusy,			NX_MLC_CanPowerDown
 */
void	NX_MLC_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != BaseAddress );

	__g_ModuleVariables[ModuleIndex].pRegister = (struct NX_MLC_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		Module's base address.
 *	@see		NX_MLC_GetPhysicalAddress,	NX_MLC_GetSizeOfRegisterSet,
 *				NX_MLC_SetBaseAddress,
 *				NX_MLC_OpenModule,			NX_MLC_CloseModule,
 *				NX_MLC_CheckBusy,			NX_MLC_CanPowerDown
 */
U32		NX_MLC_GetBaseAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that Initialize is successed. \n
 *				\b CFALSE	indicates that Initialize is failed.
 *	@see		NX_MLC_GetPhysicalAddress,	NX_MLC_GetSizeOfRegisterSet,
 *				NX_MLC_SetBaseAddress,		NX_MLC_GetBaseAddress,
 *				NX_MLC_CloseModule,
 *				NX_MLC_CheckBusy,			NX_MLC_CanPowerDown
 */
CBOOL	NX_MLC_OpenModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that Deinitialize is successed. \n
 *				\b CFALSE	indicates that Deinitialize is failed.
 *	@see		NX_MLC_GetPhysicalAddress,	NX_MLC_GetSizeOfRegisterSet,
 *				NX_MLC_SetBaseAddress,		NX_MLC_GetBaseAddress,
 *				NX_MLC_OpenModule,
 *				NX_MLC_CheckBusy,			NX_MLC_CanPowerDown
 */
CBOOL	NX_MLC_CloseModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that Module is Busy. \n
 *				\b CFALSE	indicates that Module is NOT Busy.
 *	@see		NX_MLC_GetPhysicalAddress,	NX_MLC_GetSizeOfRegisterSet,
 *				NX_MLC_SetBaseAddress,		NX_MLC_GetBaseAddress,
 *				NX_MLC_OpenModule,			NX_MLC_CloseModule,
 *				NX_MLC_CanPowerDown
 */
CBOOL	NX_MLC_CheckBusy( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that Ready to enter power-down stage. \n
 *				\b CFALSE	indicates that This module can't enter to power-down stage.
 *	@see		NX_MLC_GetPhysicalAddress,	NX_MLC_GetSizeOfRegisterSet,
 *				NX_MLC_SetBaseAddress,		NX_MLC_GetBaseAddress,
 *				NX_MLC_OpenModule,			NX_MLC_CloseModule,
 *				NX_MLC_CheckBusy
 */
CBOOL	NX_MLC_CanPowerDown( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );

	return CTRUE;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see		NX_MLC_GetClockPClkMode,
 *				NX_MLC_SetClockBClkMode,		NX_MLC_GetClockBClkMode,
 */
void			NX_MLC_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->MLCCLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//	pRegister->MLCCLKENB = regvalue;
	WriteIODW(&pRegister->MLCCLKENB,regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		Current PCLK mode
 *	@see		NX_MLC_SetClockPClkMode,
 *				NX_MLC_SetClockBClkMode,		NX_MLC_GetClockBClkMode,
 */
NX_PCLKMODE	NX_MLC_GetClockPClkMode( U32 ModuleIndex )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->MLCCLKENB & ( 1UL << PCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set System Bus Clock's operation Mode
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	mode		BCLK Mode
 *	@return		None.
 *	@see		NX_MLC_SetClockPClkMode,		NX_MLC_GetClockPClkMode,
 *				NX_MLC_GetClockBClkMode,
 */
void			NX_MLC_SetClockBClkMode( U32 ModuleIndex, NX_BCLKMODE mode )
{
	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;
	U32 clkmode=0;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	switch(mode)
	{
	case NX_BCLKMODE_DISABLE:	clkmode = 0;		break;
	case NX_BCLKMODE_DYNAMIC:	clkmode = 2;		break;
	case NX_BCLKMODE_ALWAYS:	clkmode = 3;		break;
	default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->MLCCLKENB;

	regvalue &= ~(0x3);
	regvalue |= clkmode & 0x3;

//	pRegister->MLCCLKENB = regvalue;
	WriteIODW(&pRegister->MLCCLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get System Bus Clock's operation Mode
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		BCLK Mode
 *	@see		NX_MLC_SetClockPClkMode,		NX_MLC_GetClockPClkMode,
 *				NX_MLC_SetClockBClkMode,
 */
NX_BCLKMODE	NX_MLC_GetClockBClkMode( U32 ModuleIndex )
{
	const U32 BCLKMODE	= 3UL<<0;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	switch( __g_ModuleVariables[ModuleIndex].pRegister->MLCCLKENB & BCLKMODE )
	{
	case 0 :	return NX_BCLKMODE_DISABLE;
	case 2 :	return NX_BCLKMODE_DYNAMIC;
	case 3 :	return NX_BCLKMODE_ALWAYS;
	}

	NX_ASSERT( CFALSE );
	return NX_BCLKMODE_DISABLE;
}

//--------------------------------------------------------------------------
// MLC Main Settings
//--------------------------------------------------------------------------
/**
 *	@brief		Set Power On/Off of MLC's Pixel Buffer Unit
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param		bPower	\b CTRUE indicates that Power ON of pixel buffer unit. \n
 *						\b CFALSE indicates that Power OFF of pixel buffer unit. \n
 *	@return		None.
 *	@remark		When MLC ON, first pixel buffer power ON, set to Normal Mode(pixel buffer) and MLC enable.\n
 *				When MLC Off, first MLC disable, Set pixel buffer to Sleep Mode(pixel buffer) and power OFF.\n
 *	@code
 *				// MLC ON sequence, mi = 0 ( module index )
 *				NX_MLC_SetTopPowerMode( mi, CTRUE );			// pixel buffer power on
 *				NX_MLC_SetTopSleepMode( mi, CFALSE );			// pixel buffer normal mode
 *				NX_MLC_SetMLCEnable( mi, CTRUE );				// mlc enable
 *				NX_MLC_SetTopDirtyFlag( mi );					// apply setting value
 *				...
 *				// MLC OFF sequence
 *				NX_MLC_SetMLCEnable( mi, CFALSE );				// mlc disable
 *				NX_MLC_SetTopDirtyFlag( mi );					// apply setting value
 *				while( CTRUE == NX_MLC_GetTopDirtyFlag(mi, ))	// wait until mlc is disabled
 *				{ CNULL; }
 *				NX_MLC_SetTopSleepMode( mi, CTRUE );			// pixel buffer sleep mode
 *				NX_MLC_SetTopPowerMode( mi, CFALSE );			// pixel buffer power off
 *	@endcode
 *	@see									NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void	NX_MLC_SetTopPowerMode( U32 ModuleIndex, CBOOL bPower )
{
	const U32 PIXELBUFFER_PWD_POS	= 11;
	const U32 PIXELBUFFER_PWD_MASK	= 1UL << PIXELBUFFER_PWD_POS;
	const U32 DITTYFLAG_MASK		= 1UL << 3;

	register struct NX_MLC_RegisterSet*	pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bPower) || (1==bPower) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;

	regvalue &= ~( PIXELBUFFER_PWD_MASK | DITTYFLAG_MASK );
	regvalue |= (bPower << PIXELBUFFER_PWD_POS);

//	pRegister->MLCCONTROLT = regvalue;
	WriteIODW(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get power state of MLC's pixel buffer unit.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that pixel buffer is power ON.\n
 *				\b CFALSE	indicates that pixel buffer is power OFF.\n
 *	@see		NX_MLC_SetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
CBOOL	NX_MLC_GetTopPowerMode( U32 ModuleIndex )
{
	const U32 PIXELBUFFER_PWD_POS	= 11;
	const U32 PIXELBUFFER_PWD_MASK	= 1UL << PIXELBUFFER_PWD_POS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCCONTROLT & PIXELBUFFER_PWD_MASK) >> PIXELBUFFER_PWD_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Sleep Mode Enable/Disalbe of MLC's Pixel Buffer Unit
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param		bSleep	\b CTRUE indicates that Sleep Mode Enable of pixel buffer unit. \n
 *						\b CFALSE indicates that Sleep Mode Disable of pixel buffer unit. \n
 *	@return		None.
 *	@remark		When MLC ON, first pixel buffer power ON, set to Normal Mode(pixel buffer) and MLC enable.\n
 *				When MLC Off, first MLC disable, Set pixel buffer to Sleep Mode(pixel buffer) and power OFF.\n
 *	@code
 *				// MLC ON sequence,	mi = 0  ( module index )
 *				NX_MLC_SetTopPowerMode( mi, CTRUE );			// pixel buffer power on
 *				NX_MLC_SetTopSleepMode( mi, CFALSE );			// pixel buffer normal mode
 *				NX_MLC_SetMLCEnable( mi, CTRUE );				// mlc enable
 *				NX_MLC_SetTopDirtyFlag( mi );					// apply setting value
 *				...
 *				// MLC OFF sequence
 *				NX_MLC_SetMLCEnable( mi, CFALSE );				// mlc disable
 *				NX_MLC_SetTopDirtyFlag( mi );					// apply setting value
 *				while( CTRUE == NX_MLC_GetTopDirtyFlag( mi ))	// wait until mlc is disabled
 *				{ CNULL; }
 *				NX_MLC_SetTopSleepMode( mi, CTRUE );			// pixel buffer sleep mode
 *				NX_MLC_SetTopPowerMode( mi, CFALSE );			// pixel buffer power off
 *	@endcode
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *											NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void	NX_MLC_SetTopSleepMode( U32 ModuleIndex, CBOOL bSleep )
{
	const U32 PIXELBUFFER_SLD_POS	= 10;
	const U32 PIXELBUFFER_SLD_MASK	= 1UL << PIXELBUFFER_SLD_POS;
	const U32 DITTYFLAG_MASK		= 1UL << 3;

	register struct NX_MLC_RegisterSet*	pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bSleep) || (1==bSleep) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	bSleep = (CBOOL)((U32)bSleep ^ 1);

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;

	regvalue &= ~( PIXELBUFFER_SLD_MASK | DITTYFLAG_MASK );
	regvalue |= (bSleep << PIXELBUFFER_SLD_POS);

//	pRegister->MLCCONTROLT = regvalue;
	WriteIODW(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Sleep Mode state of MLC's pixel buffer unit.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that pixel buffer is Sleep Mode.\n
 *				\b CFALSE	indicates that pixel buffer is Normal Mode.\n
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
CBOOL	NX_MLC_GetTopSleepMode( U32 ModuleIndex )
{
	const U32 PIXELBUFFER_SLD_POS	= 11;
	const U32 PIXELBUFFER_SLD_MASK	= 1UL << PIXELBUFFER_SLD_POS;

	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)(((__g_ModuleVariables[ModuleIndex].pRegister->MLCCONTROLT & PIXELBUFFER_SLD_MASK) >> PIXELBUFFER_SLD_POS) ^ 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Apply modified MLC Top registers to MLC.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		 None.
 *	@remark		MLC has dual register set architecture. Therefore you have to set a
 *				dirty flag to apply modified settings to MLC's current settings.
 *				If a dirty flag is set, MLC will update current settings to
 *				register values on a vertical blank. You can also check whether MLC
 *				has been updated by using function NX_MLC_GetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *											NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void	NX_MLC_SetTopDirtyFlag( U32 ModuleIndex )
{
	const U32 DIRTYFLAG = 1UL<<3;

	register struct NX_MLC_RegisterSet* pRegister;
	register U32	regvalue;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue  =	pRegister->MLCCONTROLT;
	regvalue |= DIRTYFLAG;

//	pRegister->MLCCONTROLT = regvalue;
	WriteIODW(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether modified settings is applied to MLC or not.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates MLC does not update to modified settings yet.\n
 *				\b CFALSE	indicates MLC has already been updated to modified settings.
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *											NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize

 */
CBOOL	NX_MLC_GetTopDirtyFlag( U32 ModuleIndex )
{
	const U32 DIRTYFLAG_POS		= 3;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCCONTROLT & DIRTYFLAG_MASK) >> DIRTYFLAG_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable MLC.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnb			Set it to CTRUE to enable MLC.
 *	@return		None.
 *	@remark		The result of this function will be applied	to MLC after calling
 *				function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *											NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void	NX_MLC_SetMLCEnable( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 MLCENB_POS	= 1;
	const U32 MLCENB_MASK	= 1UL<<MLCENB_POS;

	const U32 DIRTYFLAG_POS		= 3;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;

	regvalue &= ~( MLCENB_MASK | DIRTYFLAG_MASK );
	regvalue |= (bEnb<<MLCENB_POS);

//	pRegister->MLCCONTROLT = regvalue;
	WriteIODW(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether MLC is enabled or disabled.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates MLC is enabled.\n
 *				\b CFALSE	indicates MLC is disabled.
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
CBOOL	NX_MLC_GetMLCEnable( U32 ModuleIndex )
{
	const U32 MLCENB_POS	= 1;
	const U32 MLCENB_MASK	= 1UL<<MLCENB_POS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCCONTROLT & MLCENB_MASK) >> MLCENB_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable interlace mode.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnb			Set it to CTRUE to enable interlace mode.
 *	@return		None.
 *	@remark		The result of this function will be applied	to MLC after calling
 *				function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *											NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void	NX_MLC_SetFieldEnable( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 FIELDENB_POS	= 0;
	const U32 FIELDENB_MASK = 1UL<<FIELDENB_POS;

	const U32 DIRTYFLAG_POS		= 3;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;

	regvalue &= ~( FIELDENB_MASK | DIRTYFLAG_MASK );
	regvalue |= (bEnb<<FIELDENB_POS);

//	pRegister->MLCCONTROLT = regvalue;
	WriteIODW(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether MLC is interlace mode or progressive mode.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates MLC is interlace mode.\n
 *				\b CFALSE	indicates MLC is progressive mode.
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
CBOOL	NX_MLC_GetFieldEnable( U32 ModuleIndex )
{
	const U32 FIELDENB_POS	= 0;
	const U32 FIELDENB_MASK = 1UL<<FIELDENB_POS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)(__g_ModuleVariables[ModuleIndex].pRegister->MLCCONTROLT & FIELDENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the priority of layers.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	priority		the priority of layers
 *	@return		None.
 *	@remark		The result of this function will be applied	to MLC after calling
 *				function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *											NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void	NX_MLC_SetLayerPriority( U32 ModuleIndex, NX_MLC_PRIORITY priority )
{
	const U32 PRIORITY_POS	= 8;
	const U32 PRIORITY_MASK	= 0x03 << PRIORITY_POS;

	const U32 DIRTYFLAG_POS		= 3;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register struct NX_MLC_RegisterSet* pRegister;
	register U32    regvalue;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (NX_MLC_PRIORITY_VIDEOFIRST == priority) || (NX_MLC_PRIORITY_VIDEOSECOND == priority) ||
				(NX_MLC_PRIORITY_VIDEOTHIRD == priority) || (NX_MLC_PRIORITY_VIDEOFOURTH == priority) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->MLCCONTROLT;

	regvalue &= ~( PRIORITY_MASK | DIRTYFLAG_MASK );
	regvalue |= (priority<<PRIORITY_POS);

//	pRegister->MLCCONTROLT = regvalue;
	WriteIODW(&pRegister->MLCCONTROLT, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the screen size.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	width		the screen width, 1 ~ 4096.
 *	@param[in]	height		the screen height, 1 ~ 4096.
 *	@return		None.
 *	@remark		The result of this function will be applied	to MLC after calling
 *				function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,
 *				NX_MLC_SetBackground,		NX_MLC_GetScreenSize
 */
void	NX_MLC_SetScreenSize( U32 ModuleIndex, U32 width, U32 height )
{
	register struct NX_MLC_RegisterSet* pRegister;
	register U32    regvalue;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 4096 > (width - 1) );
	NX_ASSERT( 4096 > (height - 1) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue  = ((height-1)<<16) | (width-1);

//	pRegister->MLCSCREENSIZE = regvalue;
	WriteIODW(&pRegister->MLCSCREENSIZE, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the screen size.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[out] pWidth		the screen width, 1 ~ 4096.
 *	@param[out] pHeight		the screen height, 1 ~ 4096.
 *	@return		None.
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize,
 *				NX_MLC_SetBackground
 */
void	NX_MLC_GetScreenSize( U32 ModuleIndex, U32 *pWidth, U32 *pHeight )
{
	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( CNULL != pWidth )
	{
		*pWidth = (__g_ModuleVariables[ModuleIndex].pRegister->MLCSCREENSIZE & 0x0FFF) + 1;
	}

	if( CNULL != pHeight )
	{
		*pHeight = ((__g_ModuleVariables[ModuleIndex].pRegister->MLCSCREENSIZE>>16) & 0x0FFF) + 1;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the background color.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	color	24 bit RGB format, 0xXXRRGGBB = { R[7:0], G[7:0], B[7:0] }
 *	@return		None.
 *	@remark		The background color is default color that is shown in regions which
 *				any layer does not include. the result of this function will be
 *				applied to MLC after calling function NX_MLC_SetTopDirtyFlag().
 *	@see		NX_MLC_SetTopPowerMode,		NX_MLC_GetTopPowerMode,
 *				NX_MLC_SetTopSleepMode,		NX_MLC_GetTopSleepMode,
 *				NX_MLC_SetTopDirtyFlag,		NX_MLC_GetTopDirtyFlag,
 *				NX_MLC_SetMLCEnable,		NX_MLC_GetMLCEnable,
 *				NX_MLC_SetFieldEnable,		NX_MLC_GetFieldEnable,
 *				NX_MLC_SetLayerPriority,	NX_MLC_SetScreenSize
 */
void	NX_MLC_SetBackground( U32 ModuleIndex, U32 color )
{
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCBGCOLOR = color;
	WriteIODW(&pRegister->MLCBGCOLOR, color);
}

//--------------------------------------------------------------------------
//	Per Layer Operations
//------------------------------------------------------------------------------
/**
 *	@brief		Apply modified register values to corresponding layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1, 3: Video ).
 *	@return		None.
 *	@remark		Each layer has dual register set architecture. Therefore you have to
 *				set a dirty flag to apply modified settings to each layer's current
 *				settings. If a dirty flag is set, each layer will update current
 *				settings to register values on a vertical blank. You can also check
 *				whether each layer has been updated by using function NX_MLC_GetDirtyFlag().
 *	@see												NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetDirtyFlag( U32 ModuleIndex, U32 layer )
{
	register struct NX_MLC_RegisterSet* pRegister;
	register U32    regvalue;

	const U32 DIRTYFLG_MASK = 1UL << 4;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 4 > layer );
	NX_ASSERT( 2 != layer );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if ( 0 == layer || 1 == layer )
	{
		regvalue  = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
		regvalue |= DIRTYFLG_MASK;

	//	pRegister->MLCRGBLAYER[layer].MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);
	}
	else if( 3 == layer )
	{
		regvalue  = pRegister->MLCVIDEOLAYER.MLCCONTROL;
		regvalue |= DIRTYFLG_MASK;

	//	pRegister->MLCVIDEOLAYER.MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCONTROL, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether modified settings is applied to corresponding layer or not.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1, 3: Video ).
 *	@return		\b CTRUE	indicates corresponding layer does not update to modified settings yet.\n
 *				\b CFALSE	indicates corresponding layer has already been updated to modified settings.
 *	@see		NX_MLC_SetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
CBOOL	NX_MLC_GetDirtyFlag( U32 ModuleIndex, U32 layer )
{
	const U32 DIRTYFLG_POS	= 4;
	const U32 DIRTYFLG_MASK	= 1UL << DIRTYFLG_POS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 4 > layer );
	NX_ASSERT( 2 != layer );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if ( 0 == layer || 1 == layer )
	{
		return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCRGBLAYER[layer].MLCCONTROL & DIRTYFLG_MASK ) >> DIRTYFLG_POS);
	}
	else if( 3 == layer )
	{
		return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCVIDEOLAYER.MLCCONTROL & DIRTYFLG_MASK ) >> DIRTYFLG_POS);
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable the layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer			the layer number ( 0: RGB0, 1: RGB1, 3: Video ).
 *	@param[in]	bEnb			Set it to CTRUE to enable corresponding layer
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *														NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetLayerEnable( U32 ModuleIndex, U32 layer, CBOOL bEnb )
{
	const U32 LAYERENB_POS	= 5;
	const U32 LAYERENB_MASK	= 0x01 << LAYERENB_POS;

	const U32 DIRTYFLAG_POS		= 4;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 4 > layer );
	NX_ASSERT( 2 != layer );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;

		regvalue &= ~( LAYERENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<LAYERENB_POS);

	//	pRegister->MLCRGBLAYER[layer].MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);
	}
	else if( 3 == layer )
	{
		regvalue = pRegister->MLCVIDEOLAYER.MLCCONTROL;

		regvalue &= ~( LAYERENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<LAYERENB_POS);

	//	pRegister->MLCVIDEOLAYER.MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCONTROL, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Determines whether the layer is enabled or disabled.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1, 3: Video ).
 *	@return		\b CTRUE	indicates the layer is enabled.\n
 *				\b CFALSE	indicates the layer is disabled.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
CBOOL	NX_MLC_GetLayerEnable( U32 ModuleIndex, U32 layer )
{
	const U32 LAYERENB_POS = 5;
	const U32 LAYERENB_MASK = 0x01 << LAYERENB_POS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 4 > layer );
	NX_ASSERT( 2 != layer );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( 0 == layer || 1 == layer )
	{
		return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCRGBLAYER[layer].MLCCONTROL & LAYERENB_MASK ) >> LAYERENB_POS);
	}
	else if( 3 == layer )
	{
		return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCVIDEOLAYER.MLCCONTROL & LAYERENB_MASK ) >> LAYERENB_POS);
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the lock size for memory access.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer			the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	locksize		lock size for memory access, 4, 8, 16 are only valid.
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.\n
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetLockSize( U32 ModuleIndex, U32 layer, U32 locksize )
{
	const U32 LOCKSIZE_MASK		= 3UL<<12;

	const U32 DIRTYFLAG_POS		= 4;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	register struct NX_MLC_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 2 > layer );
	NX_ASSERT( 4 == locksize || 8 == locksize || 16 == locksize );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	locksize >>= 3;	// divide by 8

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;

		regvalue &= ~(LOCKSIZE_MASK|DIRTYFLAG_MASK);
		regvalue |= (locksize<<12);

	//	pRegister->MLCRGBLAYER[layer].MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Alpha blending.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC, 1: Second MLC ).
 *	@param[in]	layer	the layer number( 0: RGB0, 1: RGB1, 3: Video ).
 *	@param[in]	bEnb	Set it to CTRUE to enable alpha blending.
 *	@param[in]	alpha	alpha blending factor, 0 ~ 15.
 *				- When it is set to 0, this layer color is fully transparent.
 *				- When it is set to 15, this layer becomes fully opaque.
 *	@return		None.
 *	@remark		The argument 'alpha' has only affect when the color format has
 *				no alpha component. The formula for alpha blending is as follows.
 *				- If alpha is 0 then a is 0, else a is ALPHA + 1.\n
 *					color = this layer color * a / 16 + lower layer color * (16 - a) / 16.
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.\n
 *				Only one layer must apply to alpha function.
 *
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *														NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetAlphaBlending( U32 ModuleIndex, U32 layer, CBOOL bEnb, U32 alpha )
{
	const U32 BLENDENB_POS	= 2;
	const U32 BLENDENB_MASK	= 0x01 << BLENDENB_POS;

	const U32 DIRTYFLAG_POS		= 4;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	const U32 ALPHA_POS		= 28;
	const U32 ALPHA_MASK	= 0xF << ALPHA_POS;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 4 > layer );
	NX_ASSERT( 2 != layer );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( 16 > alpha );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
			regvalue &= ~( BLENDENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<BLENDENB_POS);

	//	pRegister->MLCRGBLAYER[layer].MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);

		regvalue = pRegister->MLCRGBLAYER[layer].MLCTPCOLOR;
		regvalue &= ~ALPHA_MASK;
		regvalue |= alpha << ALPHA_POS;

	//	pRegister->MLCRGBLAYER[layer].MLCTPCOLOR = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCTPCOLOR, regvalue);
	}
	else if( 3 == layer )
	{
		regvalue = pRegister->MLCVIDEOLAYER.MLCCONTROL;
		regvalue &= ~( BLENDENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<BLENDENB_POS);

	//	pRegister->MLCVIDEOLAYER.MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCONTROL, regvalue);

	//	pRegister->MLCVIDEOLAYER.MLCTPCOLOR = alpha << ALPHA_POS;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCTPCOLOR, alpha << ALPHA_POS);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set transparency.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	bEnb	Set it to CTRUE to enable transparency.
 *	@param[in]	color	Specifies the extended color to be used as the transparency color.\n
 *						24 bit RGB format, 0xXXRRGGBB = { R[7:0], G[7:0], B[7:0] }\n
 *						You can get this argument from specific color format
 *						by using the function NX_MLC_GetExtendedColor().
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetTransparency( U32 ModuleIndex, U32 layer, CBOOL bEnb, U32 color )
{
	const U32 TPENB_POS		= 0;
	const U32 TPENB_MASK	= 0x01 << TPENB_POS;

	const U32 DIRTYFLAG_POS		= 4;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	const U32 TPCOLOR_POS	= 0;
	const U32 TPCOLOR_MASK	= ((1<<24)-1) << TPCOLOR_POS;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 2 > layer );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( (1<<24) > color );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
			regvalue &= ~( TPENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<TPENB_POS);

	//	pRegister->MLCRGBLAYER[layer].MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);

		regvalue = pRegister->MLCRGBLAYER[layer].MLCTPCOLOR;
		regvalue &= ~TPCOLOR_MASK;
		regvalue |= (color & TPCOLOR_MASK);

	//	pRegister->MLCRGBLAYER[layer].MLCTPCOLOR = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCTPCOLOR, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set color inversion.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	bEnb	Set it to CTRUE to enable color inversion.
 *	@param[in]	color	Specifies the extended color to be used for color inversion.\n
 *						24 bit RGB format, 0xXXRRGGBB = { R[7:0], G[7:0], B[7:0] }\n
 *						You can get this argument from specific color format
 *						by using the function NX_MLC_GetExtendedColor().
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *														NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetColorInversion( U32 ModuleIndex, U32 layer, CBOOL bEnb, U32 color )
{
	const U32 INVENB_POS	= 1;
	const U32 INVENB_MASK	= 0x01 << INVENB_POS;

	const U32 DIRTYFLAG_POS		= 4;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	const U32 INVCOLOR_POS	= 0;
	const U32 INVCOLOR_MASK	= ((1<<24)-1) << INVCOLOR_POS;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 2 > layer );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( (1<<24) > color );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;
		regvalue &= ~( INVENB_MASK | DIRTYFLAG_MASK );
		regvalue |= (bEnb<<INVENB_POS);

	//	pRegister->MLCRGBLAYER[layer].MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);

		regvalue = pRegister->MLCRGBLAYER[layer].MLCINVCOLOR;
		regvalue &= ~INVCOLOR_MASK;
		regvalue |= (color & INVCOLOR_MASK);

	//	pRegister->MLCRGBLAYER[layer].MLCINVCOLOR = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCINVCOLOR, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get extended color.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	color	Specifies the color value to be converted.
 *	@param[in]	format	the color format with RGBFMT type.
 *	@return		the color which has 24 bit RGB format\n
 *				0xXXRRGGBB = { R[7:0], G[7:0], B[7:0] }
 *	@remark		This function is for argument 'color' of the function
 *				NX_MLC_SetTransparency() and NX_MLC_SetColorInversion().
 *				For example,
 *	@code
 *		// mi = 0 ~ 1 ( module index )
 *		NX_MLC_SetTransparency	( mi, layer, CTRUE, NX_MLC_GetExtendedColor( 0x0841, RGBFMT_R5G6B5 ) );
 *		NX_MLC_SetColorInversion( mi, layer, CTRUE, NX_MLC_GetExtendedColor( 0x090909, RGBFMT_R8G8B8 ) );
 *	@endcode
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
U32		NX_MLC_GetExtendedColor( U32 ModuleIndex, U32 color, NX_MLC_RGBFMT format )
{
	U32 rgb[3];
	int	bw[3], bp[3], blank, fill, i;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 0 == (format & 0x0000FFFFUL) );
	switch( format )
	{
	case NX_MLC_RGBFMT_R5G6B5	:	// 16bpp { R5, G6, B5 }.
		bw[0] =	5;		bw[1] =	6;		bw[2] =	5;
		bp[0] = 11;	bp[1] =	5;	bp[2] =	0;
		break;
	case NX_MLC_RGBFMT_B5G6R5	:	// 16bpp { B5, G6, R5 }.
		bw[0] =	5;		bw[1] =	6;		bw[2] =	5;
		bp[0] =	0;	bp[1] =	5;	bp[2] = 11;
		break;
	case NX_MLC_RGBFMT_X1R5G5B5 :	// 16bpp { X1, R5, G5, B5 }.
	case NX_MLC_RGBFMT_A1R5G5B5 :	// 16bpp { A1, R5, G5, B5 }.
		bw[0] =	5;		bw[1] =	5;		bw[2] =	5;
		bp[0] = 10;	bp[1] =	5;	bp[2] =	0;
		break;
	case NX_MLC_RGBFMT_X1B5G5R5 :	// 16bpp { X1, B5, G5, R5 }.
	case NX_MLC_RGBFMT_A1B5G5R5 :	// 16bpp { A1, B5, G5, R5 }.
		bw[0] =	5;		bw[1] =	5;		bw[2] =	5;
		bp[0] =	0;	bp[1] =	5;	bp[2] = 10;
		break;
	case NX_MLC_RGBFMT_X4R4G4B4 :	// 16bpp { X4, R4, G4, B4 }.
	case NX_MLC_RGBFMT_A4R4G4B4 :	// 16bpp { A4, R4, G4, B4 }.
		bw[0] =	4;		bw[1] =	4;		bw[2] =	4;
		bp[0] =	8;	bp[1] =	4;	bp[2] =	0;
		break;
	case NX_MLC_RGBFMT_X4B4G4R4 :	// 16bpp { X4, B4, G4, R4 }.
	case NX_MLC_RGBFMT_A4B4G4R4 :	// 16bpp { A4, B4, G4, R4 }.
		bw[0] =	4;		bw[1] =	4;		bw[2] =	4;
		bp[0] =	0;	bp[1] =	4;	bp[2] =	8;
		break;
	case NX_MLC_RGBFMT_X8R3G3B2 :	// 16bpp { X8, R3, G3, B2 }.
	case NX_MLC_RGBFMT_A8R3G3B2 :	// 16bpp { A8, R3, G3, B2 }.
		bw[0] =	3;		bw[1] =	3;		bw[2] =	2;
		bp[0] =	5;	bp[1] =	2;	bp[2] =	0;
		break;
	case NX_MLC_RGBFMT_X8B3G3R2 :	// 16bpp { X8, B3, G3, R2 }.
	case NX_MLC_RGBFMT_A8B3G3R2 :	// 16bpp { A8, B3, G3, R2 }.
		bw[0] =	2;		bw[1] =	3;		bw[2] =	3;
		bp[0] =	0;	bp[1] =	2;	bp[2] =	5;
		break;
	case NX_MLC_RGBFMT_R8G8B8	:	// 24bpp { R8, G8, B8 }.
//	case NX_MLC_RGBFMT_X8R8G8B8 :	// 32bpp { X8, R8, G8, B8 }.
	case NX_MLC_RGBFMT_A8R8G8B8 :	// 32bpp { A8, R8, G8, B8 }.
		bw[0] =	8;		bw[1] =	8;		bw[2] =	8;
		bp[0] = 16;	bp[1] =	8;	bp[2] =	0;
		break;
	case NX_MLC_RGBFMT_B8G8R8	:	// 24bpp { B8, G8, R8 }.
//	case NX_MLC_RGBFMT_X8B8G8R8 :	// 32bpp { X8, B8, G8, R8 }.
	case NX_MLC_RGBFMT_A8B8G8R8 :	// 32bpp { A8, B8, G8, R8 }.
		bw[0] =	8;		bw[1] =	8;		bw[2] =	8;
		bp[0] =	0;	bp[1] =	8;	bp[2] = 16;
		break;
	default : NX_ASSERT( CFALSE );
	}

	for( i=0 ; i<3 ; i++ )
	{
		rgb[i] = (color >> bp[i]) & ((U32)(1<<bw[i])-1);

		fill	= bw[i];
		blank	= 8-fill;
		rgb[i] <<= blank;
		while( blank > 0 )
		{
			rgb[i] |= (rgb[i] >> fill);
			blank	-= fill;
			fill	+= fill;
		}
	}

	return (rgb[0]<<16) | (rgb[1]<<8) | (rgb[2]<<0);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the image format of the RGB layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	format	the RGB format
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *														NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetFormatRGB( U32 ModuleIndex, U32 layer, NX_MLC_RGBFMT format )
{
	const U32 DIRTYFLAG_POS		= 4;
	const U32 DIRTYFLAG_MASK	= 1UL<<DIRTYFLAG_POS;

	const U32 FORMAT_MASK		= 0xFFFF0000UL;
	register U32 regvalue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 2 > layer );
	NX_ASSERT( 0 == (format & 0x0000FFFFUL) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		regvalue = pRegister->MLCRGBLAYER[layer].MLCCONTROL;

		regvalue &= ~(FORMAT_MASK | DIRTYFLAG_MASK);
		regvalue |= (U32)format;

	//	pRegister->MLCRGBLAYER[layer].MLCCONTROL = regvalue;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCCONTROL, regvalue);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the image format of video layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	format			The YUV format
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetFormatYUV( U32 ModuleIndex, NX_MLC_YUVFMT format )
{
	const U32 FORMAT_MASK	= 0xFFFF0000UL;
	register U32 temp;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 0 == (format & 0xFFFCFFFFUL) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	temp = pRegister->MLCVIDEOLAYER.MLCCONTROL;
	temp &= ~FORMAT_MASK;
	temp |= (U32)format;

//	pRegister->MLCVIDEOLAYER.MLCCONTROL	= temp;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCONTROL, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the layer position.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1, 3: Video ).
 *	@param[in]	sx		the x-coordinate of the upper-left corner of the layer, -2048 ~ +2047.
 *	@param[in]	sy		the y-coordinate of the upper-left corner of the layer, -2048 ~ +2047.
 *	@param[in]	ex		the x-coordinate of the lower-right corner of the layer, -2048 or 0 ~ +2047.
 *	@param[in]	ey		the y-coordinate of the lower-right corner of the layer, -2048 or 0 ~ +2047.
 *	@return		None.
 *	@remark		If layer is 2(video layer) then x, y-coordinate of the lower-right
 *				corner of the layer must be a positive value.\n
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *														NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetPosition( U32 ModuleIndex, U32 layer, S32 sx, S32 sy, S32 ex, S32 ey )
{
	register struct NX_MLC_RegisterSet*	pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 4 > layer );
	NX_ASSERT( 2 != layer );
	NX_ASSERT( -2048 <= sx && sx < 2048 );
	NX_ASSERT( -2048 <= sy && sy < 2048 );
	NX_ASSERT( -2048 <= ex && ex < 2048 );
	NX_ASSERT( -2048 <= ey && ey < 2048 );
	NX_ASSERT( (3!=layer) || ((ex>=0) && (ey>=0)) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
	//	pRegister->MLCRGBLAYER[layer].MLCLEFTRIGHT	= (((U32)sx & 0xFFFUL)<<16) | ((U32)ex & 0xFFFUL);
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCLEFTRIGHT, (((U32)sx & 0xFFFUL)<<16) | ((U32)ex & 0xFFFUL));

	//	pRegister->MLCRGBLAYER[layer].MLCTOPBOTTOM =	(((U32)sy & 0xFFFUL)<<16) | ((U32)ey & 0xFFFUL);
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCTOPBOTTOM,	(((U32)sy & 0xFFFUL)<<16) | ((U32)ey & 0xFFFUL));
	}
	else if( 3 == layer )
	{
	//	pRegister->MLCVIDEOLAYER.MLCLEFTRIGHT = (((U32)sx & 0xFFFUL)<<16) | ((U32)ex & 0xFFFUL);
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCLEFTRIGHT, (((U32)sx & 0xFFFUL)<<16) | ((U32)ex & 0xFFFUL));

	//	pRegister->MLCVIDEOLAYER.MLCTOPBOTTOM = (((U32)sy & 0xFFFUL)<<16) | ((U32)ey & 0xFFFUL);
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCTOPBOTTOM, (((U32)sy & 0xFFFUL)<<16) | ((U32)ey & 0xFFFUL));
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set dither enable when using gamma.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnable			\b CTRUE indicates that dither enable. \n
 *								\b CFALSE indicates that dither disable.
 *	@return		None.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetDitherEnableWhenUsingGamma( U32 ModuleIndex, CBOOL bEnable )
{
	const U32 DITHERENB_BITPOS	= 0;
	const U32 DITHERENB_MASK	= 1 << DITHERENB_BITPOS;

	register struct NX_MLC_RegisterSet*	pRegister;
	register U32 ReadValue;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bEnable) || (1==bEnable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	ReadValue &= ~DITHERENB_MASK;
	ReadValue |= ( (U32)bEnable << DITHERENB_BITPOS );

//	pRegister->MLCGAMMACONT = ReadValue;
	WriteIODW(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of dither is enabled or Not.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that dither is enabled when using gamma. \n
 *				\b CFALSE	indicates that dither is disabled when using gamma.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *														NX_MLC_SetGammaPriority,
 *				NX_MLC_GetGammaPriority
 */
CBOOL	NX_MLC_GetDitherEnableWhenUsingGamma( U32 ModuleIndex )
{
	const U32 DITHERENB_BITPOS	= 0;
	const U32 DITHERENB_MASK	= 1 << DITHERENB_BITPOS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)( __g_ModuleVariables[ModuleIndex].pRegister->MLCGAMMACONT & DITHERENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Select layer to apply gamma when video layer and RGB layer are overlaped region for alpha blending.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bVideoLayer		\b CTRUE indicates that Video layer. \n
 *								\b CFALSE indicates that RGB layer.
 *	@return		none.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetGammaPriority
 */
void	NX_MLC_SetGammaPriority( U32 ModuleIndex, CBOOL bVideoLayer )
{
	const U32 ALPHASELECT_BITPOS	= 5;
	const U32 ALPHASELECT_MASK		= 1 << ALPHASELECT_BITPOS;

	register struct NX_MLC_RegisterSet*	pRegister;
	register U32 ReadValue;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bVideoLayer) || (1==bVideoLayer) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	ReadValue &= ~ALPHASELECT_MASK;
	ReadValue |= ( (U32)bVideoLayer << ALPHASELECT_BITPOS );

//	pRegister->MLCGAMMACONT = ReadValue;
	WriteIODW(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of which region to apply gamma.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that Video layer is selected., \n
 *				\b CFALSE	indicates that RGB layer is selected.
 *	@see		NX_MLC_SetDirtyFlag,					NX_MLC_GetDirtyFlag,
 *				NX_MLC_SetLayerEnable,					NX_MLC_GetLayerEnable,
 *				NX_MLC_SetLockSize,
 *				NX_MLC_SetAlphaBlending,				NX_MLC_SetTransparency,
 *				NX_MLC_SetColorInversion,				NX_MLC_GetExtendedColor,
 *				NX_MLC_SetFormatRGB,					NX_MLC_SetFormatYUV,
 *				NX_MLC_SetPosition,						NX_MLC_SetDitherEnableWhenUsingGamma,
 *				NX_MLC_GetDitherEnableWhenUsingGamma,	NX_MLC_SetGammaPriority
 */
CBOOL	NX_MLC_GetGammaPriority( U32 ModuleIndex )
{
	const U32 ALPHASELECT_BITPOS	= 5;
	const U32 ALPHASELECT_MASK		= 1 << ALPHASELECT_BITPOS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCGAMMACONT & ALPHASELECT_MASK) >> ALPHASELECT_BITPOS);
}

//------------------------------------------------------------------------------
//	RGB Layer Specific Operations
//------------------------------------------------------------------------------
/**
 *	@brief		Set the invalid area of RGB Layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	region	select region ( 0 or 1 )
 *	@param[in]	sx		the x-coordinate of the upper-left corner of the layer, 0 ~ +2047.
 *	@param[in]	sy		the y-coordinate of the upper-left corner of the layer, 0 ~ +2047.
 *	@param[in]	ex		the x-coordinate of the lower-right corner of the layer, 0 ~ +2047.
 *	@param[in]	ey		the y-coordinate of the lower-right corner of the layer, 0 ~ +2047.
 *	@param[in]	bEnb	\b CTRUE indicates that invalid region Enable,\n
 *						\b CFALSE indicates that invalid region Disable.
 *	@return		None.
 *	@remark		Each RGB Layer support two invalid region. so \e region argument must set to 0 or 1.\n
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see													NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerInvalidPosition( U32 ModuleIndex, U32 layer, U32 region, S32 sx, S32 sy, S32 ex, S32 ey, CBOOL bEnb )
{
	const U32 INVALIDENB_POS = 28;

	register struct NX_MLC_RegisterSet*	pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 2 > layer );
	NX_ASSERT( 2 > region );
	NX_ASSERT( 0 <= sx && sx < 2048 );
	NX_ASSERT( 0 <= sy && sy < 2048 );
	NX_ASSERT( 0 <= ex && ex < 2048 );
	NX_ASSERT( 0 <= ey && ey < 2048 );
	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
		if( 0 == region )
		{
		//	pRegister->MLCRGBLAYER[layer].MLCINVALIDLEFTRIGHT0 = ((bEnb<<INVALIDENB_POS) | ((sx&0x7FF)<<16) | (ex&0x7FF) );
			WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCINVALIDLEFTRIGHT0, ((bEnb<<INVALIDENB_POS) | ((sx&0x7FF)<<16) | (ex&0x7FF) ));

		//	pRegister->MLCRGBLAYER[layer].MLCINVALIDTOPBOTTOM0 = ( ((sy&0x7FF)<<16) | (ey&0x7FF) );
			WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCINVALIDTOPBOTTOM0, ( ((sy&0x7FF)<<16) | (ey&0x7FF) ));
		}
		else
		{
		//	pRegister->MLCRGBLAYER[layer].MLCINVALIDLEFTRIGHT1 = ((bEnb<<INVALIDENB_POS) | ((sx&0x7FF)<<16) | (ex&0x7FF) );
			WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCINVALIDLEFTRIGHT1, ((bEnb<<INVALIDENB_POS) | ((sx&0x7FF)<<16) | (ex&0x7FF) ));

		//	pRegister->MLCRGBLAYER[layer].MLCINVALIDTOPBOTTOM1 = ( ((sy&0x7FF)<<16) | (ey&0x7FF) );
			WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCINVALIDTOPBOTTOM1, ( ((sy&0x7FF)<<16) | (ey&0x7FF) ));
		}
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set stride for horizontal and vertical.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer		the layer number ( 0: RGB0, 1: RGB1 ).
 *	@param[in]	hstride	the horizontal stride specifying the number of
 *							bytes from one pixel to the next. Generally, this
 *							value has bytes per pixel. You have to set it only
 *							to a positive value.
 *	@param[in]	vstride		the vertical stride specifying the number of
 *							bytes from one scan line of the image buffer to
 *							the next. Generally, this value has bytes per a
 *							line. You can set it to a negative value for
 *							vertical flip.
 *	@return		None.
 *	@remarks	The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerStride( U32 ModuleIndex, U32 layer, S32 hstride, S32 vstride )
{
	register struct NX_MLC_RegisterSet*	pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 2 > layer );
	NX_ASSERT( 0 <= hstride );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
	//	pRegister->MLCRGBLAYER[layer].MLCHSTRIDE = hstride;
		WriteIODW((volatile U32 *)&pRegister->MLCRGBLAYER[layer].MLCHSTRIDE, hstride);

	//	pRegister->MLCRGBLAYER[layer].MLCVSTRIDE = vstride;
		WriteIODW((volatile U32 *)&pRegister->MLCRGBLAYER[layer].MLCVSTRIDE, vstride);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set an address of the image buffer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	layer	the layer number( 0: RGB0, 1: RGB1 ).
 *	@param[in]	addr	an address of the image buffer.
 *	@return		None.
 *	@remark		Normally, the argument 'addr' specifies an address of upper-left
 *				corner of the image. but you have to set it to an address of
 *				lower-left corner for vertical mirror.\n
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *															NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerAddress( U32 ModuleIndex, U32 layer, U32 addr )
{
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 2 > layer );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	if( 0 == layer || 1 == layer )
	{
	//	pRegister->MLCRGBLAYER[layer].MLCADDRESS = addr;
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCADDRESS, addr);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set power of RGB layer's gamma table.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bRed	\b CTRUE indicates that Red gamma table is power on. \n
 *						\b CFALSE indicates that Red gamma table is power off.
 *	@param[in]	bGreen	\b CTRUE indicates that Green gamma table is power on. \n
 *						\b CFALSE indicates that Green gamma table is power off.
 *	@param[in]	bBlue	\b CTRUE indicates that Blue gamma table is power on. \n
 *						\b CFALSE indicates that Blue gamma table is power off.
 *	@return		None.
 *	@remarks	Gamma table must on before MLC gamma enable.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerGamaTablePowerMode( U32 ModuleIndex, CBOOL bRed, CBOOL bGreen, CBOOL bBlue )
{
	const U32 BGAMMATABLE_PWD_BITPOS = 11;
	const U32 GGAMMATABLE_PWD_BITPOS = 9;
	const U32 RGAMMATABLE_PWD_BITPOS = 3;

	const U32 BGAMMATABLE_PWD_MASK = ( 1 << BGAMMATABLE_PWD_BITPOS );
	const U32 GGAMMATABLE_PWD_MASK = ( 1 << GGAMMATABLE_PWD_BITPOS );
	const U32 RGAMMATABLE_PWD_MASK = ( 1 << RGAMMATABLE_PWD_BITPOS );

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bRed) || (1==bRed) );
	NX_ASSERT( (0==bGreen) || (1==bGreen) );
	NX_ASSERT( (0==bBlue) || (1==bBlue) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	ReadValue &= ~( BGAMMATABLE_PWD_MASK | GGAMMATABLE_PWD_MASK | RGAMMATABLE_PWD_MASK );

	ReadValue |= ( ((U32)bRed	<< RGAMMATABLE_PWD_BITPOS) |
					((U32)bGreen << GGAMMATABLE_PWD_BITPOS) |
					((U32)bBlue	<< BGAMMATABLE_PWD_BITPOS) );

//	pRegister->MLCGAMMACONT = ReadValue;
	WriteIODW(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of RGB layer's gamma table is power On or NOT.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[out] pbRed	\b CTRUE indicates that Red gamma table is power on. \n
 *						\b CFALSE indicates that Red gamma table is power off.
 *	@param[out] pbGreen	\b CTRUE indicates that Green gamma table is power on. \n
 *						\b CFALSE indicates that Green gamma table is power off.
 *	@param[out] pbBlue	\b CTRUE indicates that Blue gamma table is power on. \n
 *						\b CFALSE indicates that Blue gamma table is power off.
 *	@return		None.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *															NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_GetRGBLayerGamaTablePowerMode( U32 ModuleIndex, CBOOL *pbRed, CBOOL *pbGreen, CBOOL *pbBlue )
{
	const U32 BGAMMATABLE_PWD_BITPOS = 11;
	const U32 GGAMMATABLE_PWD_BITPOS = 9;
	const U32 RGAMMATABLE_PWD_BITPOS = 3;

	const U32 BGAMMATABLE_PWD_MASK = ( 1 << BGAMMATABLE_PWD_BITPOS );
	const U32 GGAMMATABLE_PWD_MASK = ( 1 << GGAMMATABLE_PWD_BITPOS );
	const U32 RGAMMATABLE_PWD_MASK = ( 1 << RGAMMATABLE_PWD_BITPOS );

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	if( CNULL != pbRed	)	{ *pbRed	= (ReadValue & RGAMMATABLE_PWD_MASK) ? CTRUE : CFALSE;	}
	if( CNULL != pbGreen)	{ *pbGreen = (ReadValue & GGAMMATABLE_PWD_MASK) ? CTRUE : CFALSE;	}
	if( CNULL != pbBlue )	{ *pbBlue	= (ReadValue & BGAMMATABLE_PWD_MASK) ? CTRUE : CFALSE;	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set sleep mode of RGB layer's gamma table.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bRed	\b CTRUE indicates that Red gamma table is sleep on. \n
 *						\b CFALSE indicates that Red gamma table is sleep off.
 *	@param[in]	bGreen	\b CTRUE indicates that Green gamma table is sleep on. \n
 *						\b CFALSE indicates that Green gamma table is sleep off.
 *	@param[in]	bBlue	\b CTRUE indicates that Blue gamma table is sleep on. \n
 *						\b CFALSE indicates that Blue gamma table is sleep off.
 *	@return		None.
 *	@remarks	Sleep mode is only usable when gamma table is power ON.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerGamaTableSleepMode( U32 ModuleIndex, CBOOL bRed, CBOOL bGreen, CBOOL bBlue )
{
	const U32 BGAMMATABLE_SLD_BITPOS = 10;
	const U32 GGAMMATABLE_SLD_BITPOS = 8;
	const U32 RGAMMATABLE_SLD_BITPOS = 2;

	const U32 BGAMMATABLE_SLD_MASK = ( 1 << BGAMMATABLE_SLD_BITPOS );
	const U32 GGAMMATABLE_SLD_MASK = ( 1 << GGAMMATABLE_SLD_BITPOS );
	const U32 RGAMMATABLE_SLD_MASK = ( 1 << RGAMMATABLE_SLD_BITPOS );

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	if( bRed	)	{ ReadValue &= ~RGAMMATABLE_SLD_MASK; }
	else		{ ReadValue |=	RGAMMATABLE_SLD_MASK; }

	if( bGreen)	{ ReadValue &= ~GGAMMATABLE_SLD_MASK; }
	else		{ ReadValue |=	GGAMMATABLE_SLD_MASK; }

	if( bBlue )	{ ReadValue &= ~BGAMMATABLE_SLD_MASK; }
	else		{ ReadValue |=	BGAMMATABLE_SLD_MASK; }

//	pRegister->MLCGAMMACONT = ReadValue;
	WriteIODW(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of RGB layer's gamma table is Sleep mode or NOT.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[out] pbRed	\b CTRUE indicates that Red gamma table is sleep on. \n
 *						\b CFALSE indicates that Red gamma table is sleep off.
 *	@param[out] pbGreen	\b CTRUE indicates that Green gamma table is sleep on. \n
 *						\b CFALSE indicates that Green gamma table is sleep off.
 *	@param[out] pbBlue	\b CTRUE indicates that Blue gamma table is sleep on. \n
 *						\b CFALSE indicates that Blue gamma table is sleep off.
 *	@return		None.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *															NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_GetRGBLayerGamaTableSleepMode( U32 ModuleIndex, CBOOL *pbRed, CBOOL *pbGreen, CBOOL *pbBlue )
{
	const U32 BGAMMATABLE_SLD_BITPOS = 10;
	const U32 GGAMMATABLE_SLD_BITPOS = 8;
	const U32 RGAMMATABLE_SLD_BITPOS = 2;

	const U32 BGAMMATABLE_SLD_MASK = ( 1 << BGAMMATABLE_SLD_BITPOS );
	const U32 GGAMMATABLE_SLD_MASK = ( 1 << GGAMMATABLE_SLD_BITPOS );
	const U32 RGAMMATABLE_SLD_MASK = ( 1 << RGAMMATABLE_SLD_BITPOS );

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	// // 0: Sleep Mode 1: Normal
	if( CNULL != pbRed )	{ *pbRed	= (ReadValue & RGAMMATABLE_SLD_MASK) ? CFALSE : CTRUE;	}
	if( CNULL != pbGreen )	{ *pbGreen = (ReadValue & GGAMMATABLE_SLD_MASK) ? CFALSE : CTRUE;	}
	if( CNULL != pbBlue )	{ *pbBlue	= (ReadValue & BGAMMATABLE_SLD_MASK) ? CFALSE : CTRUE;	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Red gamma table value
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	dwAddress		Red gamma table address (0~255).
 *	@param[in]	dwData			Red gamma table value (10bit data).
 *	@return		None.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerRGammaTable( U32 ModuleIndex, U32 dwAddress, U32 dwData )
{
	register struct NX_MLC_RegisterSet* pRegister;

	const U32 TABLEADDR_BITPOS = 24;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 256 > dwAddress );
	NX_ASSERT( (1<<10) > dwData );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCRGAMMATABLEWRITE = ((dwAddress << TABLEADDR_BITPOS) | dwData);
	WriteIODW(&pRegister->MLCRGAMMATABLEWRITE, ((dwAddress << TABLEADDR_BITPOS) | dwData));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Green gamma table value
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	dwAddress		Green gamma table address (0~255).
 *	@param[in]	dwData			Green gamma table value (10bit data).
 *	@return		None.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *															NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerGGammaTable( U32 ModuleIndex, U32 dwAddress, U32 dwData )
{
	register struct NX_MLC_RegisterSet* pRegister;

	const U32 TABLEADDR_BITPOS = 24;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 256 > dwAddress );
	NX_ASSERT( (1<<10) > dwData );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCGGAMMATABLEWRITE = ((dwAddress << TABLEADDR_BITPOS) | dwData);
	WriteIODW(&pRegister->MLCGGAMMATABLEWRITE, ((dwAddress << TABLEADDR_BITPOS) | dwData));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Blue gamma table value
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	dwAddress		Blue gamma table address (0~255).
 *	@param[in]	dwData			Blue gamma table value (10bit data).
 *	@return		None.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable,				NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerBGammaTable( U32 ModuleIndex, U32 dwAddress, U32 dwData )
{
	register struct NX_MLC_RegisterSet* pRegister;

	const U32 TABLEADDR_BITPOS = 24;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 256 > dwAddress );
	NX_ASSERT( (1<<10) > dwData );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCBGAMMATABLEWRITE = ((dwAddress << TABLEADDR_BITPOS) | dwData);
	WriteIODW(&pRegister->MLCBGAMMATABLEWRITE, ((dwAddress << TABLEADDR_BITPOS) | dwData));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set RGB layer gamma enable or Not.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnable			\b CTRUE indicates that RGB layer's gamma enable. \n
 *								\b CFALSE indicates that RGB layer's gamma disable.
 *	@return		None.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *															NX_MLC_GetRGBLayerGammaEnable
 */
void	NX_MLC_SetRGBLayerGammaEnable( U32 ModuleIndex, CBOOL bEnable )
{
	const U32 RGBGAMMAEMB_BITPOS	= 1;
	const U32 RGBGAMMAEMB_MASK		= 1 << RGBGAMMAEMB_BITPOS;

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bEnable) | (1==bEnable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	ReadValue &= ~RGBGAMMAEMB_MASK;
	ReadValue |= (U32)bEnable << RGBGAMMAEMB_BITPOS;

//	pRegister->MLCGAMMACONT = ReadValue;
	WriteIODW(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of RGB layer's gamma is enabled or disabled.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	Indicates that RGB layer's gamma is enabled. \n
 *				\b CFALSE	Indicates that RGB layer's gamma is disabled.
 *	@see		NX_MLC_SetRGBLayerInvalidPosition,			NX_MLC_SetRGBLayerStride,
 *				NX_MLC_SetRGBLayerAddress,					NX_MLC_SetRGBLayerGamaTablePowerMode,
 *				NX_MLC_GetRGBLayerGamaTablePowerMode,		NX_MLC_SetRGBLayerGamaTableSleepMode,
 *				NX_MLC_GetRGBLayerGamaTableSleepMode,		NX_MLC_SetRGBLayerRGammaTable,
 *				NX_MLC_SetRGBLayerGGammaTable,				NX_MLC_SetRGBLayerBGammaTable,
 *				NX_MLC_SetRGBLayerGammaEnable
 */
CBOOL	NX_MLC_GetRGBLayerGammaEnable( U32 ModuleIndex )
{
	const U32 RGBGAMMAEMB_BITPOS	= 1;
	const U32 RGBGAMMAEMB_MASK		= 1 << RGBGAMMAEMB_BITPOS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return(CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCGAMMACONT & RGBGAMMAEMB_MASK) >> RGBGAMMAEMB_BITPOS);
}

void	NX_MLC_SetPaletteTable( U32 ModuleIndex, U32 layer, U16 *pPaletteTable)
{
	const U32 PALLETADDR_BITPOS		= 24;
	register U8 index;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CNULL != pPaletteTable );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;
	for(index=0; index<256; index++)
	{
		WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCPALETTE0, (U32)index<<PALLETADDR_BITPOS | (U32)*pPaletteTable++);
	}
}

void	NX_MLC_SetPaletteData( U32 ModuleIndex, U32 layer, U8 PaletteAddr, U16 PaletteData)
{
	const U32 PALLETADDR_BITPOS		= 24;
	register U32 PaletteDataFormat;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	PaletteDataFormat = (U32)((U32)PaletteAddr<<PALLETADDR_BITPOS) | (U32)PaletteData;

	WriteIODW(&pRegister->MLCRGBLAYER[layer].MLCPALETTE0, PaletteDataFormat );
}

//--------------------------------------------------------------------------
// Video Layer Specific Operations
//------------------------------------------------------------------------------
/**
 *	@brief		Set the vertical stride for video layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	LuStride	the vertical stride for Y component.
 *	@param[in]	CbStride	the vertical stride for Cb component.
 *	@param[in]	CrStride	the vertical stride for Cr component.
 *	@return		None.
 *	@remark		The vertical stride specifies the number of bytes from one scan line
 *				of the image buffer to the next. Generally, it has bytes per a line.
 *				You have to set it only to a positive value.\n
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see													NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,		NX_MLC_SetVideoLayerGammaEnable,
 *				NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerStride( U32 ModuleIndex, S32 LuStride, S32 CbStride, S32 CrStride )
{
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 0 < LuStride );
	NX_ASSERT( 0 < CbStride );
	NX_ASSERT( 0 < CrStride );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCVIDEOLAYER.MLCVSTRIDE	= LuStride;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCVSTRIDE	, LuStride);

//	pRegister->MLCVIDEOLAYER.MLCVSTRIDECB = CbStride;
	WriteIODW((volatile U32 *)&pRegister->MLCVIDEOLAYER.MLCVSTRIDECB, CbStride);

//	pRegister->MLCVIDEOLAYER.MLCVSTRIDECR = CrStride;
	WriteIODW((volatile U32 *)&pRegister->MLCVIDEOLAYER.MLCVSTRIDECR, CrStride);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set an address of the image buffer for video layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	LuAddr	an address of the Y component buffer which has 2D block addressing format.
 *	@param[in]	CbAddr	an address of the Cb component buffer which has 2D block addressing format.
 *	@param[in]	CrAddr	an address of the Cr component buffer which has 2D block addressing format.
 *	@return		None.
 *	@remark		This function is valid when the format of video layer must has
 *				separated YUV format. Each color component has the buffer address.\n
 *				The 2D block addressing format is as follwings.
 *				- Addr[31:24] specifies the index of segment.
 *				- Addr[23:12] specifies the y-coordinate of upper-left corner of the
 *						image in segment.
 *				- Addr[11: 0] specifies the x-coordinate of upper-left corner of the
 *					image in segment.
 *				.
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetVideoLayerStride,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerAddress( U32 ModuleIndex, U32 LuAddr, U32 CbAddr, U32 CrAddr )
{
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 0 == (LuAddr & 7) );
	NX_ASSERT( 0 == (CbAddr & 7) );
	NX_ASSERT( 0 == (CrAddr & 7) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCVIDEOLAYER.MLCADDRESS	= LuAddr;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCADDRESS,   LuAddr);

//	pRegister->MLCVIDEOLAYER.MLCADDRESSCB = CbAddr;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCADDRESSCB, CbAddr);

//	pRegister->MLCVIDEOLAYER.MLCADDRESSCR	= CrAddr;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCADDRESSCR, CrAddr);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set an address of the image buffer for video layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	Addr	An address of the buffer which has YUYV addressing format.
 *	@param[in]	Stride	The vertical stride.
 *	@return		None.
 *	@remark		This function is valid when the format of video layer is YUVV format.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *															NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerAddressYUYV( U32 ModuleIndex, U32 Addr, S32 Stride )
{
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != Addr );
	NX_ASSERT( CNULL != Stride );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCVIDEOLAYER.MLCADDRESS = Addr;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCADDRESS, Addr);

//	pRegister->MLCVIDEOLAYER.MLCVSTRIDE = Stride;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCVSTRIDE, Stride);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Specifies the scale ratio for video layer with the scale factor.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	hscale		the horizontal scale factor, 23 bit unsigned value.
 *	@param[in]	vscale		the vertical scale factor, 23 bit unsigned value.
 *	@param[in]	bHLumaEnb	Set it CTRUE to apply Bi-linear filter for horizontal scale-up(Luminance).
 *	@param[in]	bHChromaEnb Set it CTRUE to apply Bi-linear filter for horizontal scale-up(Chroma).
 *	@param[in]	bVLumaEnb	Set it CTRUE to apply Bi-linear filter for vertical scale-up(Luminance).
 *	@param[in]	bVChromaEnb Set it CTRUE to apply Bi-linear filter for vertical scale-up(Chroma).
 *	@return		None.
 *	@remark		The scale factor can be calculated by following formula.
 *				If Bilinear Filter is used,
 *				- hscale = (source width-1) * (1<<11) / (destination width-1).
 *				- vscale = (source height-1) * (1<<11) / (destinatin height-1).
 *				.
 *				,else
 *				- hscale = source width * (1<<11) / destination width.
 *				- vscale = source height * (1<<11) / destinatin height.
 *				.
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerScaleFactor( U32 ModuleIndex, U32 hscale, U32 vscale, CBOOL bHLumaEnb, CBOOL bHChromaEnb, CBOOL bVLumaEnb, CBOOL bVChromaEnb )
{
	const U32 FILTER_LUMA_POS	= 28;
	const U32 FILTER_CHOMA_POS	= 29;
	const U32 SCALE_MASK		= ((1<<23)-1);

	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( (1<<23) > hscale );
	NX_ASSERT( (1<<23) > vscale );
	NX_ASSERT( (0==bHLumaEnb) ||(1==bHLumaEnb) );
	NX_ASSERT( (0==bHChromaEnb) ||(1==bHChromaEnb) );
	NX_ASSERT( (0==bVLumaEnb) ||(1==bVLumaEnb) );
	NX_ASSERT( (0==bVChromaEnb) ||(1==bVChromaEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCVIDEOLAYER.MLCHSCALE = ((bHLumaEnb << FILTER_LUMA_POS) | (bHChromaEnb << FILTER_CHOMA_POS) | (hscale & SCALE_MASK) );
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCHSCALE, ((bHLumaEnb << FILTER_LUMA_POS) | (bHChromaEnb << FILTER_CHOMA_POS) | (hscale & SCALE_MASK) ));

//	pRegister->MLCVIDEOLAYER.MLCVSCALE = ((bVLumaEnb << FILTER_LUMA_POS) | (bVChromaEnb << FILTER_CHOMA_POS) | (vscale & SCALE_MASK) );
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCVSCALE, ((bVLumaEnb << FILTER_LUMA_POS) | (bVChromaEnb << FILTER_CHOMA_POS) | (vscale & SCALE_MASK) ));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Specifies the scale filter.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bHLumaEnb	Set it CTRUE to apply Bi-linear filter for horizontal scale-up(Luminance).
 *	@param[in]	bHChromaEnb Set it CTRUE to apply Bi-linear filter for horizontal scale-up(Chroma).
 *	@param[in]	bVLumaEnb	Set it CTRUE to apply Bi-linear filter for vertical scale-up(Luminance).
 *	@param[in]	bVChromaEnb Set it CTRUE to apply Bi-linear filter for vertical scale-up(Chroma).
 *	@return		None.
 *	@remark		The scale factor can be calculated by following formula.
 *				If Bilinear Filter is used,
 *				- hscale = (source width-1) * (1<<11) / (destination width-1).
 *				- vscale = (source height-1) * (1<<11) / (destinatin height-1).
 *				.
 *				,else
 *				- hscale = source width * (1<<11) / destination width.
 *				- vscale = source height * (1<<11) / destinatin height.
 *				.
 *				The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerScaleFilter( U32 ModuleIndex, CBOOL bHLumaEnb, CBOOL bHChromaEnb, CBOOL bVLumaEnb, CBOOL bVChromaEnb )
{
	const U32 FILTER_LUMA_POS	= 28;
	const U32 FILTER_CHOMA_POS	= 29;
	const U32 SCALE_MASK		= ((1<<23)-1);

	register struct NX_MLC_RegisterSet* pRegister;
	register U32 	ReadValue;

	NX_ASSERT( (0==bHLumaEnb) ||(1==bHLumaEnb) );
	NX_ASSERT( (0==bHChromaEnb) ||(1==bHChromaEnb) );
	NX_ASSERT( (0==bVLumaEnb) ||(1==bVLumaEnb) );
	NX_ASSERT( (0==bVChromaEnb) ||(1==bVChromaEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue  = pRegister->MLCVIDEOLAYER.MLCHSCALE;
	ReadValue &= SCALE_MASK;
	ReadValue |= (bHLumaEnb << FILTER_LUMA_POS) | (bHChromaEnb << FILTER_CHOMA_POS);

//	pRegister->MLCVIDEOLAYER.MLCHSCALE = ReadValue;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCHSCALE, ReadValue);

	ReadValue  = pRegister->MLCVIDEOLAYER.MLCVSCALE;
	ReadValue &= SCALE_MASK;
	ReadValue |= (bVLumaEnb << FILTER_LUMA_POS) | (bVChromaEnb << FILTER_CHOMA_POS);

//	pRegister->MLCVIDEOLAYER.MLCVSCALE = ReadValue;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCVSCALE, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the scale filter status.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bHLumaEnb	Get Bi-linear filter status for horizontal scale-up(Luminance).
 *	@param[in]	bHChromaEnb Get Bi-linear filter status for horizontal scale-up(Chroma).
 *	@param[in]	bVLumaEnb	Get Bi-linear filter status for vertical scale-up(Luminance).
 *	@param[in]	bVChromaEnb Get Bi-linear filter status for vertical scale-up(Chroma).
 *	@return		None.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_GetVideoLayerScaleFilter( U32 ModuleIndex, CBOOL *bHLumaEnb, CBOOL *bHChromaEnb, CBOOL *bVLumaEnb, CBOOL *bVChromaEnb )
{
	const U32 FILTER_LUMA_POS	= 28;
	const U32 FILTER_CHOMA_POS	= 29;
	const U32 FILTER_MASK		= 1UL;

	register struct NX_MLC_RegisterSet* pRegister;
	register U32 	ReadValue;

	NX_ASSERT( bHLumaEnb );
	NX_ASSERT( bHChromaEnb );
	NX_ASSERT( bVLumaEnb );
	NX_ASSERT( bVChromaEnb );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue    = pRegister->MLCVIDEOLAYER.MLCHSCALE;
	*bHLumaEnb 	 = (ReadValue >> FILTER_LUMA_POS)  & FILTER_MASK;
	*bHChromaEnb = (ReadValue >> FILTER_CHOMA_POS) & FILTER_MASK;

	ReadValue    = pRegister->MLCVIDEOLAYER.MLCVSCALE;
	*bVLumaEnb 	 = (ReadValue >> FILTER_LUMA_POS)  & FILTER_MASK;
	*bVChromaEnb = (ReadValue >> FILTER_CHOMA_POS) & FILTER_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Specifies the scale ratio for video layer with the width and height.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	sw		the width of the source image.
 *	@param[in]	sh		the height of the source image.
 *	@param[in]	dw		the width of the destination image.
 *	@param[in]	dh		the height of the destination image.
 *	@param[in]	bHLumaEnb	Set it CTRUE to apply Bi-linear filter for horizontal scale-up(Luminance).
 *	@param[in]	bHChromaEnb Set it CTRUE to apply Bi-linear filter for horizontal scale-up(Chroma).
 *	@param[in]	bVLumaEnb	Set it CTRUE to apply Bi-linear filter for vertical scale-up(Luminance).
 *	@param[in]	bVChromaEnb Set it CTRUE to apply Bi-linear filter for vertical scale-up(Chroma).
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *															NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerScale( U32 ModuleIndex, U32 sw, U32 sh, U32 dw, U32 dh, CBOOL bHLumaEnb, CBOOL bHChromaEnb, CBOOL bVLumaEnb, CBOOL bVChromaEnb )
{
	const U32 FILTER_LUMA_POS	= 28;
	const U32 FILTER_CHOMA_POS	= 29;
	const U32 SCALE_MASK		= ((1<<23)-1);
	register U32 hscale, vscale, cal_sh;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( 4096 > sw );
	NX_ASSERT( 4096 > sh );
	NX_ASSERT( 4096 > dw );
	NX_ASSERT( 4096 > dh );
	NX_ASSERT( (0==bHLumaEnb)	|| (1==bHLumaEnb) );
	NX_ASSERT( (0==bHChromaEnb) || (1==bHChromaEnb) );
	NX_ASSERT( (0==bVLumaEnb)	|| (1==bVLumaEnb) );
	NX_ASSERT( (0==bVChromaEnb) || (1==bVChromaEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	// HScale
	if( (bHLumaEnb || bHChromaEnb) && (dw > sw) )
	{
		sw--;
		dw--;
	}

	hscale = (sw << 11) / dw;

	// VScale
	if( (bVLumaEnb || bVChromaEnb) && ( dh > sh ) )
	{
		sh--;
		dh--;
		vscale = (sh << 11) / dh;

		// To remove a noisy vertical line at end of image in case of vertical filter on and scale-up.
		cal_sh = ((vscale * dh) >> 11);
		if( sh <= cal_sh )
		{
			vscale--;
		}
	}
	else
	{
		vscale = (sh << 11) / dh;
	}

//	pRegister->MLCVIDEOLAYER.MLCHSCALE = ((bHLumaEnb << FILTER_LUMA_POS) | (bHChromaEnb << FILTER_CHOMA_POS) | (hscale & SCALE_MASK) );
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCHSCALE, ((bHLumaEnb << FILTER_LUMA_POS) | (bHChromaEnb << FILTER_CHOMA_POS) | (hscale & SCALE_MASK) ));

//	pRegister->MLCVIDEOLAYER.MLCVSCALE = ((bVLumaEnb << FILTER_LUMA_POS) | (bVChromaEnb << FILTER_CHOMA_POS) | (vscale & SCALE_MASK) );
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCVSCALE, ((bVLumaEnb << FILTER_LUMA_POS) | (bVChromaEnb << FILTER_CHOMA_POS) | (vscale & SCALE_MASK) ));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set brightness and contrast for video layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	contrast	the contrast value, 0 ~ 7.\n
 *							0 : 1.0, 1 : 1.125, 2 : 1.25, 3 : 1.375,
 *							4 : 1.5, 5 : 1.625, 6 : 1.75, 7 : 1.875
 *	@param[in]	brightness	the brightness value, -128 ~ +127.
 *	@return		None.
 *	@remark		The result of this function will be applied to corresponding layer
 *				after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerLumaEnhance( U32 ModuleIndex, U32 contrast, S32 brightness )
{
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 8 > contrast );
	NX_ASSERT( -128 <= brightness && brightness < 128 );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->MLCVIDEOLAYER.MLCLUENH = (((U32)brightness & 0xFFUL)<<8) | contrast;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCLUENH, (((U32)brightness & 0xFFUL)<<8) | contrast);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set factors to control hue and satuation for video layer.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	quadrant	a quadrant to apply hue and saturation, 0 ~ 4.\n
 *							Set it to 0 to apply hue and saturation on all quadrant.
 *	@param[in]	CbA		cosine value for B-Y axis, -128 ~ +127.
 *	@param[in]	CbB		sine value for R-Y axis, -128 ~ +127.
 *	@param[in]	CrA		sine value for B-Y axis, -128 ~ +127.
 *	@param[in]	CrB			cosine value for R-Y axis, -128 ~ +127.
 *	@return		None.
 *	@remark		Each quadrant has factors to control hue and satuation.
 *				At each coordinate, HUE and saturation is applied by following formula.\n
 *					(B-Y)' = (B-Y)*CbA + (R-Y)*CbB \n
 *					(R-Y)' = (B-Y)*CrA + (R-Y)*CrB \n
 *				, where \n
 *					CbA = cos(��) * 64 * gain, CbB = -sin(��) * 64 * gain \n
 *					CrA = sin(��) * 64 * gain, CrB =	cos(��) * 64 * gain \n
 *					gain is 0 to 2. \n
 *				The example for this equation is as follows.
 *	@code
 *		// mi = 0 ( module index )
 *		// HUE : phase relationship of the chrominance components, -180 ~ 180 degree.
 *		// Sat : color intensity, 0 ~ 127.
 *		void SetChromaEnhance( mi, int Hue, int Sat )
 *		{
 *			S32 sv, cv;
 *
 *			if( Sat < 0 )			Sat = 0;
 *			else if( Sat > 127 )	Sat = 127;
 *
 *			sv = (S32)(sin( (3.141592654f * Hue) / 180 ) * Sat);
 *			cv = (S32)(cos( (3.141592654f * Hue) / 180 ) * Sat);
 *
 *			NX_MLC_SetVideoLayerChromaEnhance( mi, 0, cv, -sv, sv, cv );
 *		};
 *	@endcode
 *			The result of this function will be applied to corresponding layer
 *			after calling function NX_MLC_SetDirtyFlag() with corresponding layer.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *															NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerChromaEnhance( U32 ModuleIndex, U32 quadrant, S32 CbA, S32 CbB, S32 CrA, S32 CrB )
{
	register struct NX_MLC_RegisterSet* pRegister;
	register U32 temp;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( 5 > quadrant );
	NX_ASSERT( -128 <= CbA && CbA < 128 );
	NX_ASSERT( -128 <= CbB && CbB < 128 );
	NX_ASSERT( -128 <= CrA && CrA < 128 );
	NX_ASSERT( -128 <= CrB && CrB < 128 );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	temp	= (((U32)CrB & 0xFFUL)<<24)
			| (((U32)CrA & 0xFFUL)<<16)
			| (((U32)CbB & 0xFFUL)<< 8)
			| (((U32)CbA & 0xFFUL)<< 0);

	if( 0 < quadrant )
	{
	//	pRegister->MLCVIDEOLAYER.MLCCHENH[quadrant-1] = temp;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCHENH[quadrant-1], temp);
	}
	else
	{
	//	pRegister->MLCVIDEOLAYER.MLCCHENH[0] = temp;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCHENH[0], temp);

	//	pRegister->MLCVIDEOLAYER.MLCCHENH[1] = temp;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCHENH[1], temp);

	//	pRegister->MLCVIDEOLAYER.MLCCHENH[2] = temp;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCHENH[2], temp);

	//	pRegister->MLCVIDEOLAYER.MLCCHENH[3] = temp;
		WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCHENH[3], temp);
	}
}

//-------------------------------------------------------------------------------
/**
 *	@brief		Set power control of video layer's line buffer unit.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnable	\b CTRUE indicates that Power ON of video layer's line buffer unit. \n
 *						\b CFALSE indicates that Power OFF of video layer's line buffer unit. \n
 *	@return		None.
 *	@remark		- Line buffer can power on/off during Video layer operation.
 *				- Line buffer only use when Video layer's Scale Up/Down operation with biliner filter On .
 *				- Line buffer ON sequence
 *					- Power On
 *					- Normal Mode ( Sleep Mode disable )
 *				- Line buffer OFF sequence
 *					- Sleep Mode
 *					- Power Off
 *	@code
 *				// Line Buffer On sequence, mi = 0 ( module index ), l = 3 (video layer )
 *				NX_MLC_SetVideoLayerLineBufferPowerMode( mi, CTRUE );			// power on
 *				NX_MLC_SetVideoLayerLineBufferSleepMode( mi, CFALSE );			// normal mode
 *				NX_MLC_SetLayerEnable( mi, l, CTRUE );						// layer enable
 *				NX_MLC_SetDirtyFlag( mi, l );									// apply now
 *				...
 *				// Line Buffer Off sequence, mi = 0 ( module index ), l = 3 (video layer )
 *				NX_MLC_SetLayerEnable( mi, l, CFALSE );						// layer disable
 *				NX_MLC_SetDirtyFlag( mi, l );									// apply now
 *				while( CTRUE == NX_MLC_GetDirtyFlag( mi, l ))					// wait until layer is disabled
 *				{ CNULL; }
 *				NX_MLC_SetVideoLayerLineBufferSleepMode( mi, CTRUE );			// sleep mode
 *				NX_MLC_SetVideoLayerLineBufferPowerMode( mi, CFALSE );			// layer off
 *	@endcode
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerLineBufferPowerMode( U32 ModuleIndex, CBOOL bEnable )
{
	const U32 LINEBUFF_PWD_POS	= 15;
	const U32 LINEBUFF_PWD_MASK	= 1UL << LINEBUFF_PWD_POS;
	const U32 DIRTYFLAG_MASK	= 1UL << 4;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet*	pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bEnable) || (1==bEnable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->MLCVIDEOLAYER.MLCCONTROL;

	regvalue &= ~( LINEBUFF_PWD_MASK | DIRTYFLAG_MASK );
	regvalue |= ((U32)bEnable << LINEBUFF_PWD_POS);

//	pRegister->MLCVIDEOLAYER.MLCCONTROL = regvalue;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get power state of video layer's line buffer unit
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that video layer's power control unit is power ON.\n
 *				\b CFALSE	indicates that video layer's power control unit is power OFF.\n
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *															NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
CBOOL	NX_MLC_GetVideoLayerLineBufferPowerMode( U32 ModuleIndex )
{
	const U32 LINEBUFF_PWD_POS	= 15;
	const U32 LINEBUFF_PWD_MASK	= 1UL << LINEBUFF_PWD_POS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCVIDEOLAYER.MLCCONTROL & LINEBUFF_PWD_MASK ) >> LINEBUFF_PWD_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Sleep mode of video layer's power control unit.
 *	@param[in]	ModuleIndex	An index of module ( 0 : First MLC ).
 *	@param[in]	bEnable	\b CTRUE indicates that \b SLEEP Mode of video layer's power control unit. \n
 *						\b CFALSE indicates that \b NORMAL Mode of video layer's power control unit. \n
 *	@return		None.
 *	@remark		- Power control unit is Line buffer.\n
 *				- Line buffer can power on/off during Video layer operation.
 *				- Line buffer only use when Video layer's Scale Up/Down operation with biliner filter On .
 *				- Line buffer ON sequence
 *					- Power On
 *					- Normal Mode ( Sleep Mode disable )
 *				- Line buffer OFF sequence
 *					- Sleep Mode
 *					- Power Off
 *	@code
 *				// Line Buffer On sequence, mi = 0 ( module index ), l = 3 (video layer )
 *				NX_MLC_SetVideoLayerLineBufferPowerMode( mi, CTRUE );				// power on
 *				NX_MLC_SetVideoLayerLineBufferSleepMode( mi, CFALSE );			// normal mode
 *				NX_MLC_SetLayerEnable( mi, l, CTRUE );							// layer enable
 *				NX_MLC_SetDirtyFlag( mi, l );										// apply now
 *				...
 *				// Line Buffer Off sequence, mi = 0 ( module index ), l = 3 (video layer )
 *				NX_MLC_SetLayerEnable( mi, l, CFALSE );							// layer disable
 *				NX_MLC_SetDirtyFlag( mi, l );										// apply now
 *				while( CTRUE == NX_MLC_GetDirtyFlag( mi, l ))						// wait until layer is disabled
 *				{ CNULL; }
 *				NX_MLC_SetVideoLayerLineBufferSleepMode( mi, CTRUE );				// sleep mode
 *				NX_MLC_SetVideoLayerLineBufferPowerMode( mi, CFALSE );			// layer off
 *	@endcode
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerLineBufferSleepMode( U32 ModuleIndex, CBOOL bEnable )
{
	const U32 LINEBUFF_SLMD_POS		= 14;
	const U32 LINEBUFF_SLMD_MASK	= 1UL << LINEBUFF_SLMD_POS;
	const U32 DIRTYFLAG_MASK		= 1UL << 4;

	register U32 regvalue;
	register struct NX_MLC_RegisterSet*	pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bEnable) || (1==bEnable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	bEnable = (CBOOL)((U32)bEnable ^ 1);

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	regvalue = pRegister->MLCVIDEOLAYER.MLCCONTROL;

	regvalue &= ~( LINEBUFF_SLMD_MASK | DIRTYFLAG_MASK );
	regvalue |= (bEnable << LINEBUFF_SLMD_POS);				// 0:sleep enable, 1:sleep disable

//	pRegister->MLCVIDEOLAYER.MLCCONTROL = regvalue;
	WriteIODW(&pRegister->MLCVIDEOLAYER.MLCCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get sleep mode state of video layer's power control unit.
 *	@param[in]	ModuleIndex	An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that video layer's power control unit is sleep on.\n
 *				\b CFALSE	indicates that video layer's power control unit is sleep off.\n
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *															NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
CBOOL	NX_MLC_GetVideoLayerLineBufferSleepMode( U32 ModuleIndex )
{
	const U32 LINEBUFF_SLMD_POS		= 14;
	const U32 LINEBUFF_SLMD_MASK	= 1UL << LINEBUFF_SLMD_POS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( LINEBUFF_SLMD_MASK & __g_ModuleVariables[ModuleIndex].pRegister->MLCVIDEOLAYER.MLCCONTROL )
	{
		return CFALSE;
	}
	else
	{
		return CTRUE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set power of YUV layer's gamma table.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bY	\b CTRUE indicates that Y gamma table is power on. \n
 *					\b CFALSE indicates that Y gamma table is power off.
 *	@param[in]	bU	\b CTRUE indicates that U gamma table is power on. \n
 *					\b CFALSE indicates that U gamma table is power off.
 *	@param[in]	bV	\b CTRUE indicates that V gamma table is power on. \n
 *					\b CFALSE indicates that V gamma table is power off.
 *	@return		None.
 *	@remarks	Gamma table must on before MLC gamma enable.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerGamaTablePowerMode( U32 ModuleIndex, CBOOL bY, CBOOL bU, CBOOL bV )
{
	const U32 VGAMMATABLE_PWD_BITPOS = 17;
	const U32 UGAMMATABLE_PWD_BITPOS = 15;
	const U32 YGAMMATABLE_PWD_BITPOS = 13;

	const U32 VGAMMATABLE_PWD_MASK = ( 1 << VGAMMATABLE_PWD_BITPOS );
	const U32 UGAMMATABLE_PWD_MASK = ( 1 << UGAMMATABLE_PWD_BITPOS );
	const U32 YGAMMATABLE_PWD_MASK = ( 1 << YGAMMATABLE_PWD_BITPOS );

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bY) || (1==bY) );
	NX_ASSERT( (0==bU) || (1==bU) );
	NX_ASSERT( (0==bV) || (1==bV) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	ReadValue &= ~( YGAMMATABLE_PWD_MASK | UGAMMATABLE_PWD_MASK | VGAMMATABLE_PWD_MASK );

	ReadValue |= ( ((U32)bY << YGAMMATABLE_PWD_BITPOS) |
					((U32)bU << UGAMMATABLE_PWD_BITPOS) |
					((U32)bV << VGAMMATABLE_PWD_BITPOS) );

//	pRegister->MLCGAMMACONT = ReadValue;
	WriteIODW(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of video layer's gamma table is powered or NOT.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[out] pbY		\b CTRUE	indicates that Y gamma table is powered. \n
 *						\b CFALSE indicates that Y gamma table is NOT powered.
 *	@param[out] pbU		\b CTRUE	indicates that U gamma table is powered. \n
 *						\b CFALSE indicates that U gamma table is NOT powered.
 *	@param[out] pbV		\b CTRUE	indicates that V gamma table is powered. \n
 *						\b CFALSE indicates that V gamma table is NOT powered.
 *	@return		None.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *															NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_GetVideoLayerGamaTablePowerMode( U32 ModuleIndex, CBOOL *pbY, CBOOL *pbU, CBOOL *pbV )
{
	const U32 VGAMMATABLE_PWD_BITPOS = 17;
	const U32 UGAMMATABLE_PWD_BITPOS = 15;
	const U32 YGAMMATABLE_PWD_BITPOS = 13;

	const U32 VGAMMATABLE_PWD_MASK = ( 1 << VGAMMATABLE_PWD_BITPOS );
	const U32 UGAMMATABLE_PWD_MASK = ( 1 << UGAMMATABLE_PWD_BITPOS );
	const U32 YGAMMATABLE_PWD_MASK = ( 1 << YGAMMATABLE_PWD_BITPOS );

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	if( CNULL != pbY )	{ *pbY = ( ReadValue & YGAMMATABLE_PWD_MASK) ? CTRUE : CFALSE ;	}
	if( CNULL != pbU )	{ *pbU = ( ReadValue & UGAMMATABLE_PWD_MASK) ? CTRUE : CFALSE ;	}
	if( CNULL != pbV )	{ *pbV = ( ReadValue & VGAMMATABLE_PWD_MASK) ? CTRUE : CFALSE ;	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set sleep mode of video layer's gamma table.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bY	\b CTRUE indicates that Y gamma table is sleep on. \n
 *					\b CFALSE indicates that Y gamma table is sleep off.
 *	@param[in]	bU	\b CTRUE indicates that U gamma table is sleep on. \n
 *					\b CFALSE indicates that U gamma table is sleep off.
 *	@param[in]	bV	\b CTRUE indicates that V gamma table is sleep on. \n
 *					\b CFALSE indicates that V gamma table is sleep off.
 *	@return		None.
 *	@remarks	Sleep mode is only usable when gamma table is power ON.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerGamaTableSleepMode( U32 ModuleIndex, CBOOL bY, CBOOL bU, CBOOL bV )
{
	const U32 VGAMMATABLE_SLD_BITPOS = 16;
	const U32 UGAMMATABLE_SLD_BITPOS = 14;
	const U32 YGAMMATABLE_SLD_BITPOS = 12;

	const U32 VGAMMATABLE_SLD_MASK = ( 1 << VGAMMATABLE_SLD_BITPOS );
	const U32 UGAMMATABLE_SLD_MASK = ( 1 << UGAMMATABLE_SLD_BITPOS );
	const U32 YGAMMATABLE_SLD_MASK = ( 1 << YGAMMATABLE_SLD_BITPOS );

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	if( bY ){ ReadValue &= ~YGAMMATABLE_SLD_MASK;	}
	else	{ ReadValue |= YGAMMATABLE_SLD_MASK;	}

	if( bU ){ ReadValue &= ~UGAMMATABLE_SLD_MASK;	}
	else	{ ReadValue |= UGAMMATABLE_SLD_MASK;	}

	if( bV ){ ReadValue &= ~VGAMMATABLE_SLD_MASK;	}
	else	{ ReadValue |= VGAMMATABLE_SLD_MASK;	}

//	pRegister->MLCGAMMACONT = ReadValue;
	WriteIODW(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of video layer's gamma table is Sleep mode or NOT.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[out] pbY		\b CTRUE indicates that Y gamma table is sleep on. \n
 *						\b CFALSE indicates that Y gamma table is sleep off.
 *	@param[out] pbU		\b CTRUE indicates that U gamma table is sleep on. \n
 *						\b CFALSE indicates that U gamma table is sleep off.
 *	@param[out] pbV		\b CTRUE indicates that V gamma table is sleep on. \n
 *						\b CFALSE indicates that V gamma table is sleep off.
 *	@return		None.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable,			NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_GetVideoLayerGamaTableSleepMode( U32 ModuleIndex, CBOOL *pbY, CBOOL *pbU, CBOOL *pbV )
{
	const U32 VGAMMATABLE_SLD_BITPOS = 16;
	const U32 UGAMMATABLE_SLD_BITPOS = 14;
	const U32 YGAMMATABLE_SLD_BITPOS = 12;

	const U32 VGAMMATABLE_SLD_MASK = ( 1 << VGAMMATABLE_SLD_BITPOS );
	const U32 UGAMMATABLE_SLD_MASK = ( 1 << UGAMMATABLE_SLD_BITPOS );
	const U32 YGAMMATABLE_SLD_MASK = ( 1 << YGAMMATABLE_SLD_BITPOS );

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	// // 0: Sleep Mode 1: Normal
	if( CNULL != pbY )	{ *pbY = (ReadValue & VGAMMATABLE_SLD_MASK) ? CFALSE : CTRUE;	}
	if( CNULL != pbU )	{ *pbU = (ReadValue & UGAMMATABLE_SLD_MASK) ? CFALSE : CTRUE;	}
	if( CNULL != pbV )	{ *pbV = (ReadValue & YGAMMATABLE_SLD_MASK) ? CFALSE : CTRUE;	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set video layer's gamma enable or disable.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@param[in]	bEnable			\b CTRUE indicates that Video layer's gamma enable. \n
 *								\b CFALSE indicates that Video layer's gamma disable.
 *	@return		None.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,		NX_MLC_GetVideoLayerGammaEnable
 */
void	NX_MLC_SetVideoLayerGammaEnable( U32 ModuleIndex, CBOOL bEnable )
{
	const U32 YUVGAMMAEMB_BITPOS	= 4;
	const U32 YUVGAMMAEMB_MASK		= 1 << YUVGAMMAEMB_BITPOS;

	register U32 ReadValue;
	register struct NX_MLC_RegisterSet* pRegister;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( (0==bEnable) | (1==bEnable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->MLCGAMMACONT;

	ReadValue &= ~YUVGAMMAEMB_MASK;
	ReadValue |= (U32)bEnable << YUVGAMMAEMB_BITPOS;

//	pRegister->MLCGAMMACONT = ReadValue;
	WriteIODW(&pRegister->MLCGAMMACONT, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of video layer's gamma is enabled or disabled.
 *	@param[in]	ModuleIndex		An index of module ( 0 : First MLC ).
 *	@return		\b CTRUE	indicates that video layer's gamma is enabled. \n
 *				\b CFALSE	indicates that video layer's gamma is disabled.
 *	@see		NX_MLC_SetVideoLayerStride,					NX_MLC_SetVideoLayerAddress,
 *				NX_MLC_SetVideoLayerAddressYUYV,			NX_MLC_SetVideoLayerScaleFactor,
 *				NX_MLC_SetVideoLayerScale,					NX_MLC_SetVideoLayerLumaEnhance,
 *				NX_MLC_SetVideoLayerChromaEnhance,			NX_MLC_SetVideoLayerLineBufferPowerMode,
 *				NX_MLC_GetVideoLayerLineBufferPowerMode,	NX_MLC_SetVideoLayerLineBufferSleepMode,
 *				NX_MLC_GetVideoLayerLineBufferSleepMode,	NX_MLC_SetVideoLayerGamaTablePowerMode,
 *				NX_MLC_GetVideoLayerGamaTablePowerMode,		NX_MLC_SetVideoLayerGamaTableSleepMode,
 *				NX_MLC_GetVideoLayerGamaTableSleepMode,
 *				NX_MLC_SetVideoLayerGammaEnable
 */
CBOOL	NX_MLC_GetVideoLayerGammaEnable( U32 ModuleIndex )
{
	const U32 YUVGAMMAEMB_BITPOS	= 4;
	const U32 YUVGAMMAEMB_MASK		= 1 << YUVGAMMAEMB_BITPOS;

	NX_ASSERT( NUMBER_OF_MLC_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return(CBOOL)((__g_ModuleVariables[ModuleIndex].pRegister->MLCGAMMACONT & YUVGAMMAEMB_MASK) >> YUVGAMMAEMB_BITPOS);
}
