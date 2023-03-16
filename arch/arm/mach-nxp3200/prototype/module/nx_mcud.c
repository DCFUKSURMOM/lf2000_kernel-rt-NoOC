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
//	Module		: MCUD
//	File		: nx_mcud.c
//	Description	:
//	Author		:
//	History		:
//------------------------------------------------------------------------------

#include "nx_mcud.h"

static	struct NX_MCUD_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.\n
 *	@see	NX_MCUD_GetNumberOfModule
 */
CBOOL	NX_MCUD_Initialize( void )
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
 *	@see		NX_MCUD_Initialize
 */
U32		NX_MCUD_GetNumberOfModule( void )
{
	return NUMBER_OF_MCUD_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_MCUD_GetSizeOfRegisterSet,
 *				NX_MCUD_SetBaseAddress,		NX_MCUD_GetBaseAddress,
 *				NX_MCUD_OpenModule,			NX_MCUD_CloseModule,
 *				NX_MCUD_CheckBusy,			NX_MCUD_CanPowerDown
 */
U32		NX_MCUD_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_MCUD_MODULE + (OFFSET_OF_MCUD_MODULE * 0) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_MCUD_GetPhysicalAddress,
 *				NX_MCUD_SetBaseAddress,		NX_MCUD_GetBaseAddress,
 *				NX_MCUD_OpenModule,			NX_MCUD_CloseModule,
 *				NX_MCUD_CheckBusy,			NX_MCUD_CanPowerDown
 */
U32		NX_MCUD_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_MCUD_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_MCUD_GetPhysicalAddress,	NX_MCUD_GetSizeOfRegisterSet,
 *				NX_MCUD_GetBaseAddress,
 *				NX_MCUD_OpenModule,			NX_MCUD_CloseModule,
 *				NX_MCUD_CheckBusy,			NX_MCUD_CanPowerDown
 */
void	NX_MCUD_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_MCUD_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_MCUD_GetPhysicalAddress,	NX_MCUD_GetSizeOfRegisterSet,
 *				NX_MCUD_SetBaseAddress,
 *				NX_MCUD_OpenModule,			NX_MCUD_CloseModule,
 *				NX_MCUD_CheckBusy,			NX_MCUD_CanPowerDown
 */
U32		NX_MCUD_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE indicate that Initialize is failed.
 *	@see		NX_MCUD_GetPhysicalAddress,	NX_MCUD_GetSizeOfRegisterSet,
 *				NX_MCUD_SetBaseAddress,		NX_MCUD_GetBaseAddress,
 *				NX_MCUD_CloseModule,
 *				NX_MCUD_CheckBusy,			NX_MCUD_CanPowerDown
 */
CBOOL	NX_MCUD_OpenModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_MCUD_GetPhysicalAddress,	NX_MCUD_GetSizeOfRegisterSet,
 *				NX_MCUD_SetBaseAddress,		NX_MCUD_GetBaseAddress,
 *				NX_MCUD_OpenModule,
 *				NX_MCUD_CheckBusy,			NX_MCUD_CanPowerDown
 */
CBOOL	NX_MCUD_CloseModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_MCUD_GetPhysicalAddress,	NX_MCUD_GetSizeOfRegisterSet,
 *				NX_MCUD_SetBaseAddress,		NX_MCUD_GetBaseAddress,
 *				NX_MCUD_OpenModule,			NX_MCUD_CloseModule,
 *				NX_MCUD_CanPowerDown
 */
CBOOL	NX_MCUD_CheckBusy( void )
{
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_MCUD_GetPhysicalAddress,	NX_MCUD_GetSizeOfRegisterSet,
 *				NX_MCUD_SetBaseAddress,		NX_MCUD_GetBaseAddress,
 *				NX_MCUD_OpenModule,			NX_MCUD_CloseModule,
 *				NX_MCUD_CheckBusy
 */
CBOOL	NX_MCUD_CanPowerDown( void )
{
	return CTRUE;
}


//------------------------------------------------------------------------------
//	Module Customized Functions
//------------------------------------------------------------------------------
//	MCU Control Settings
//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable MCU-D.
 *	@param[in]	bEnb	Set this as CTRUE to enable MCU-D.
 *	@return		None.
 *	@see		NX_MCUD_SetEnable,			NX_MCUD_GetEnable,
 *				NX_MCUD_SetDLLReset,		NX_MCUD_GetDLLReset,
 *				NX_MCUD_SetMCLKConfig,		NX_MCUD_GetMCLKConfig,
 *				NX_MCUD_ApplyModeSetting,	NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetEnable( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 31;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( bEnb )	__g_pRegister->MEMCTRL &= ~BIT_MASK;		// 0 : Enable
//	else		__g_pRegister->MEMCTRL |=	BIT_MASK;		// 1 : Disable
	SetValue = __g_pRegister->MEMCTRL;
	if( bEnb )	SetValue &= ~BIT_MASK;		// 0 : Enable
	else		SetValue |=	BIT_MASK;		// 1 : Disable
	WriteIODW(&__g_pRegister->MEMCTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether MCU-D is enabled or not.
 *	@return		\b CTRUE indicates MCU-D is enabled.\n
 *				\b CFALSE indicates MCU-D is disabled.
 *	@see		NX_MCUD_SetEnable,			NX_MCUD_GetEnable,
 *				NX_MCUD_SetDLLReset,		NX_MCUD_GetDLLReset,
 *				NX_MCUD_SetMCLKConfig,		NX_MCUD_GetMCLKConfig,
 *				NX_MCUD_ApplyModeSetting,	NX_MCUD_IsBusyModeSetting
 */
CBOOL	NX_MCUD_GetEnable( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 31;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->MEMCTRL & BIT_MASK) ? CFALSE : CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Determines whether to reset the DLL of the DRAM after it exits
 *				from the Self Refresh.
 *	@param[in]	bEnb	Set this as CTRUE to reset the DLL.
 *	@return		None.
 *	@see		NX_MCUD_SetEnable,			NX_MCUD_GetEnable,
 *				NX_MCUD_SetDLLReset,		NX_MCUD_GetDLLReset,
 *				NX_MCUD_SetMCLKConfig,		NX_MCUD_GetMCLKConfig,
 *				NX_MCUD_ApplyModeSetting,	NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetDLLReset( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 2;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( bEnb )	__g_pRegister->MEMCTRL |=	BIT_MASK;
//	else		__g_pRegister->MEMCTRL &= ~BIT_MASK;
	SetValue = __g_pRegister->MEMCTRL;
	if( bEnb )	SetValue |= BIT_MASK;
	else		SetValue &=	~BIT_MASK;
	WriteIODW(&__g_pRegister->MEMCTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether to reset the DLL of the DRAM after it exits
 *				from the Self Refresh.
 *	@return		\b CTRUE	indicates MCU-D will reset the DLL of the DRAM.\n
 *				\b CFALSE	indicates MCU-D will not reset the DLL of the DRAM.
 *	@see		NX_MCUD_SetEnable,			NX_MCUD_GetEnable,
 *				NX_MCUD_SetDLLReset,		NX_MCUD_GetDLLReset,
 *				NX_MCUD_SetMCLKConfig,		NX_MCUD_GetMCLKConfig,
 *				NX_MCUD_ApplyModeSetting,	NX_MCUD_IsBusyModeSetting
 */
CBOOL	NX_MCUD_GetDLLReset( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 2;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->MEMCTRL & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set MCLK information.
 *	@param[in]	bDouble		Set this as CTRUE if MCLK is BCLK x 2.
 *	@param[in]	bHighSpeed	Set this as CTRUE if MCLK is greater than or equal to 200 Mhz.
 *	@return		None.
 *	@see		NX_MCUD_SetEnable,			NX_MCUD_GetEnable,
 *				NX_MCUD_SetDLLReset,		NX_MCUD_GetDLLReset,
 *				NX_MCUD_SetMCLKConfig,		NX_MCUD_GetMCLKConfig,
 *				NX_MCUD_ApplyModeSetting,	NX_MCUD_IsBusyModeSetting
 */ 
void	NX_MCUD_SetMCLKConfig( CBOOL bDouble, CBOOL bHighSpeed )
{
	const U32 MCURATE	= 1UL<<1;
	const U32 MCUPPADD	= 1UL<<0;

	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->MEMCTRL;

	if( bDouble )		temp |=	MCURATE;
	else				temp &= ~MCURATE;

	if( bHighSpeed )	temp |=	MCUPPADD;
	else				temp &= ~MCUPPADD;

//	__g_pRegister->MEMCTRL = temp;
	WriteIODW(&__g_pRegister->MEMCTRL, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get MCLK information which is set by user.
 *	@param[in]	pbDouble	Indicates whether MCLK is BCLK x 2 or not.
 *	@param[in]	pbHighSpeed	Indicates whether MCLK is greater than or equal to 200 Mhz.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUD_SetEnable,			NX_MCUD_GetEnable,
 *				NX_MCUD_SetDLLReset,		NX_MCUD_GetDLLReset,
 *				NX_MCUD_SetMCLKConfig,		NX_MCUD_GetMCLKConfig,
 *				NX_MCUD_ApplyModeSetting,	NX_MCUD_IsBusyModeSetting
 */ 
void	NX_MCUD_GetMCLKConfig( CBOOL *pbDouble, CBOOL *pbHighSpeed )
{
	const U32 MCURATE	= 1UL<<1;
	const U32 MCUPPADD	= 1UL<<0;

	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->MEMCTRL;

	if( pbDouble != CNULL )
	{
		*pbDouble = (temp & MCURATE) ? CTRUE : CFALSE;
	}

	if( pbHighSpeed != CNULL )
	{
		*pbHighSpeed = (temp & MCUPPADD) ? CTRUE : CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Apply Mode Register setting on .
 *	@return		None.
 *	@remark		The system has affected by mode register settings after calling this function.
 *				After calling this function, you have to wait until a mode setting sequence has been completed.
 *				Therefore the mode setting sequence is as follows.
 *	@code
 *				Some functions for MRS or EMRS.
 *				NX_MCUD_ApplyModeSetting();
 *				while( NX_MCUD_IsBusyModeSetting() );
 *	@endcode
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetReadDelayLatency,	NX_MCUD_GetReadDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_ApplyModeSetting( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 15;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->MEMCTRL |= BIT_MASK;
	SetValue = __g_pRegister->MEMCTRL | BIT_MASK;
	WriteIODW(&__g_pRegister->MEMCTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether a mode setting sequence is busy or not.
 *	@return		\b CTRUE	indicates a mode setting sequence is busy.\n
 *				\b CFALSE	indicates a mode setting sequence has been completed.
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetReadDelayLatency,	NX_MCUD_GetReadDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
CBOOL	NX_MCUD_IsBusyModeSetting( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 15;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->MEMCTRL & BIT_MASK) ? CTRUE : CFALSE;
}


//------------------------------------------------------------------------------
//	Memory type settings
//------------------------------------------------------------------------------
/**
 *	@brief		Set memory type.
 *	@param[in]	type	Specifies a memory type which is one of @ref NX_MCUD_TYPE enum.
 *	@return		None.
 *	@see		NX_MCUD_TYPE,
 *				NX_MCUD_SetType,		NX_MCUD_GetType, 
 *				NX_MCUD_SetBusWidth,	NX_MCUD_GetBusWidth, 
 *				NX_MCUD_SetDataWidth,	NX_MCUD_GetDataWidth, 
 *				NX_MCUD_SetCapacity,	NX_MCUD_GetCapacity
 */
void	NX_MCUD_SetType( NX_MCUD_TYPE type )
{
	register U32 regval;

	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 7;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( NX_MCUD_TYPE_DDR == type || NX_MCUD_TYPE_DDR2 == type || NX_MCUD_TYPE_MDDR == type );
	NX_ASSERT( (1<<BIT_SIZE) > type );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMCFG;
	regval &= ~BIT_MASK;
	regval |= ((U32)type << BIT_POS);
//	__g_pRegister->MEMCFG = regval;
	WriteIODW(&__g_pRegister->MEMCFG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get memory type.
 *	@return		Memory type which is one of @ref NX_MCUD_TYPE enum.
 *	@see		NX_MCUD_TYPE,
 *				NX_MCUD_SetType,		NX_MCUD_GetType, 
 *				NX_MCUD_SetBusWidth,	NX_MCUD_GetBusWidth, 
 *				NX_MCUD_SetDataWidth,	NX_MCUD_GetDataWidth, 
 *				NX_MCUD_SetCapacity,	NX_MCUD_GetCapacity
 */
NX_MCUD_TYPE		NX_MCUD_GetType( void )
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 7;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_MCUD_TYPE)((__g_pRegister->MEMCFG & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set memory bus bit-width of MCU.
 *	@param[in]	buswidth	Specifies a memory bus bit-width, 16.
 *	@return		None.
 *	@remark		You have to set \e buswidth as 16 only.
 *	@see		NX_MCUD_SetType,		NX_MCUD_GetType, 
 *				NX_MCUD_SetBusWidth,	NX_MCUD_GetBusWidth, 
 *				NX_MCUD_SetDataWidth,	NX_MCUD_GetDataWidth, 
 *				NX_MCUD_SetCapacity,	NX_MCUD_GetCapacity
 */
void	NX_MCUD_SetBusWidth( U32 buswidth )
{
	register U32 regval;

	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 5;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (16==buswidth) );
	NX_ASSERT( CNULL != __g_pRegister );
	
	buswidth >>= 4;		//	divide by 16

	regval = __g_pRegister->MEMCFG;
	regval &= ~BIT_MASK;
	regval |= (buswidth << BIT_POS);
//	__g_pRegister->MEMCFG = regval;
	WriteIODW(&__g_pRegister->MEMCFG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get memory bus bit-width of MCU.
 *	@return		Memory bus bit-width of MCU.
 *	@see		NX_MCUD_SetType,		NX_MCUD_GetType, 
 *				NX_MCUD_SetBusWidth,	NX_MCUD_GetBusWidth, 
 *				NX_MCUD_SetDataWidth,	NX_MCUD_GetDataWidth, 
 *				NX_MCUD_SetCapacity,	NX_MCUD_GetCapacity
 */
U32		NX_MCUD_GetBusWidth ( void )
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 5;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(((__g_pRegister->MEMCFG & BIT_MASK) >> BIT_POS) << 4);		// * 16
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set data bit-width of memory.
 *	@param[in]	datawidth	Specifies a data bit-width, 8 or 16.
 *	@return		None.
 *	@see		NX_MCUD_SetType,		NX_MCUD_GetType, 
 *				NX_MCUD_SetBusWidth,	NX_MCUD_GetBusWidth, 
 *				NX_MCUD_SetDataWidth,	NX_MCUD_GetDataWidth, 
 *				NX_MCUD_SetCapacity,	NX_MCUD_GetCapacity
 */
void	NX_MCUD_SetDataWidth( U32 datawidth )
{
	register U32 regval;

	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 3;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (8==datawidth) || (16==datawidth) );
	NX_ASSERT( CNULL != __g_pRegister );

	datawidth >>= 3;		//	divide by 8

	regval = __g_pRegister->MEMCFG;
	regval &= ~BIT_MASK;
	regval |= (datawidth << BIT_POS);
//	__g_pRegister->MEMCFG = regval;
	WriteIODW(&__g_pRegister->MEMCFG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get data bit-width of MCU.
 *	@return		data bit-width of MCU.
 *	@see		NX_MCUD_SetType,		NX_MCUD_GetType, 
 *				NX_MCUD_SetBusWidth,	NX_MCUD_GetBusWidth, 
 *				NX_MCUD_SetDataWidth,	NX_MCUD_GetDataWidth, 
 *				NX_MCUD_SetCapacity,	NX_MCUD_GetCapacity
 */
U32		NX_MCUD_GetDataWidth ( void )
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 3;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(((__g_pRegister->MEMCFG & BIT_MASK) >> BIT_POS) << 3);		// * 8
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set memory capacity.
 *	@param[in]	capacity	Specifies a memory capacity in MBits, 256, 512 or 1024.
 *	@return		None.
 *	@see		NX_MCUD_SetType,		NX_MCUD_GetType, 
 *				NX_MCUD_SetBusWidth,	NX_MCUD_GetBusWidth, 
 *				NX_MCUD_SetDataWidth,	NX_MCUD_GetDataWidth, 
 *				NX_MCUD_SetCapacity,	NX_MCUD_GetCapacity
 */
void	NX_MCUD_SetCapacity( U32 capacity )
{
	register U32 regval;

	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (256==capacity) || (512==capacity) || (1024==capacity) );
	NX_ASSERT( CNULL != __g_pRegister );

	// 2:256, 3:512, 4:1024, others:Resserved.
	// capacity <= (capacity / 512) + 2
	capacity = (capacity >> 9) + 2;

	regval = __g_pRegister->MEMCFG;
	regval &= ~BIT_MASK;
	regval |= (capacity << BIT_POS);
//	__g_pRegister->MEMCFG = regval;
	WriteIODW(&__g_pRegister->MEMCFG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get memory capacity.
 *	@return		Memory capacity in MBits.
 *	@see		NX_MCUD_SetType,		NX_MCUD_GetType, 
 *				NX_MCUD_SetBusWidth,	NX_MCUD_GetBusWidth, 
 *				NX_MCUD_SetDataWidth,	NX_MCUD_GetDataWidth, 
 *				NX_MCUD_SetCapacity,	NX_MCUD_GetCapacity
 */
U32		NX_MCUD_GetCapacity ( void )
{
	register U32 regval;

	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = (__g_pRegister->MEMCFG & BIT_MASK) >> BIT_POS;

	// 2:256, 3:512, 4:1024, others:Resserved.
	NX_ASSERT( (4>=regval) && (2<=regval) );
	return (256UL<<(regval-2));
}


//------------------------------------------------------------------------------
//	Mode Register Settings
//------------------------------------------------------------------------------
/**
 *	@brief		Set new CAS latency.
 *	@param[in]	cycle	New CAS latency, 2 ~ 6.
 *	@return		None.
 *	@remark		The CAS latency between 4 and 6 is only valid for DDR2-SDRAM.\n
 *				The system has affected by this setting after calling the function NX_MCUD_ApplyModeSetting(). 
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetCASLatency( U32 cycle )
{
	register U32 regval;

	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 21;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (2 <= cycle) && (6 >= cycle) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMCFG;
	regval &= ~BIT_MASK;
	regval |= (cycle << BIT_POS);
//	__g_pRegister->MEMCFG = regval;
	WriteIODW(&__g_pRegister->MEMCFG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current CAS latency.
 *	@return		Current CAS latency.
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
U32		NX_MCUD_GetCASLatency( void )
{
	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 21;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->MEMCFG & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set new additive latency.
 *	@param[in]	cycle	New additive latency, 0 ~ 4.
 *	@return		None.
 *	@remark		The additive latency is only for DDR2-SDRAM.\n
 *				The system has affected by this setting after calling the function NX_MCUD_ApplyModeSetting(). 
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetAdditiveLatency( U32 cycle )
{
	register U32 regval;

	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 18;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( 4 >= cycle );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMCFG;
	regval &= ~BIT_MASK;
	regval |= (cycle << BIT_POS);
//	__g_pRegister->MEMCFG = regval;
	WriteIODW(&__g_pRegister->MEMCFG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current additive latency.
 *	@return		Current additive latency.
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
U32		NX_MCUD_GetAdditiveLatency( void )
{
	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 18;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->MEMCFG & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set additional CAS latency delay for memory reading.
 *	@param[in]	cycle	Specifies additional CAS latency delay for memory reading, 1 ~ 5.
 *	@return		None.
 *	@remark		CAS latency for memory reading = CASLAT + DLYLAT.
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetDelayLatency( U32 cycle )
{
	register U32 regval;

	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 24;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= cycle) && (5 >= cycle) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMCFG;
	regval &= ~BIT_MASK;
	regval |= ((cycle-1) << BIT_POS);
//	__g_pRegister->MEMCFG = regval;
	WriteIODW(&__g_pRegister->MEMCFG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get additional CAS latency delay for memory reading.
 *	@return		Current additional CAS latency delay for memory reading.
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
U32		NX_MCUD_GetDelayLatency( void )
{
	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 24;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMCFG & BIT_MASK) >> BIT_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set output driver impedance control.
 *	@param[in]	dic	output driver impedance control.
 *	@return		None.
 *	@remark		The system has affected by this setting after calling the function NX_MCUD_ApplyModeSetting(). 
 *	@see		NX_MCUD_DIC,
 *				NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetDIC( NX_MCUD_DIC dic )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 10;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( 2 > dic );
	NX_ASSERT( CNULL != __g_pRegister );

//	if( NX_MCUD_DIC_WEAK == dic )	__g_pRegister->MEMCFG |=	BIT_MASK;
//	else							__g_pRegister->MEMCFG &= ~BIT_MASK;
	SetValue = __g_pRegister->MEMCFG;
	if( NX_MCUD_DIC_WEAK == dic )	SetValue |=	BIT_MASK;
	else							SetValue &= ~BIT_MASK;
	WriteIODW(&__g_pRegister->MEMCFG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current output driver impedance control.
 *	@return		Current output driver impedance control.
 *	@see		NX_MCUD_DIC,
 *				NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
NX_MCUD_DIC	NX_MCUD_GetDIC( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 10;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->MEMCFG & BIT_MASK) ? NX_MCUD_DIC_WEAK : NX_MCUD_DIC_NORMAL;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the On-Die-Termination resistance value of DDR2-SDRAM.
 *	@param[in]	ODT	Specifies the On-Die-Termination resistance value.
 *	@return		None.
 *	@remark		The system has affected by this setting after calling the function NX_MCUD_ApplyModeSetting(). 
 *	@see		NX_MCUD_ODT,
 *				NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetODT( NX_MCUDRAM_ODT ODT )
{
	register U32 regval;

	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( 4 > ODT );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMCFG;
	regval &= ~BIT_MASK;
	regval |= ((U32)ODT << BIT_POS);
//	__g_pRegister->MEMCFG = regval;
	WriteIODW(&__g_pRegister->MEMCFG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the On-Die-Termination resistance value of DDR2-SDRAM.
 *	@return		The On-Die-Termination resistance value.
 *	@see		NX_MCUD_ODT,
 *				NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
NX_MCUDRAM_ODT	NX_MCUD_GetODT( void )
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	return (NX_MCUDRAM_ODT)((__g_pRegister->MEMCFG & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the DQS mode.
 *	@param[in]	DQSmode	Specifies the DQS mode.
 *	@return		None.
 *	@remark		The system has affected by this setting after calling the function NX_MCUD_ApplyModeSetting(). 
 *	@see		NX_MCUD_DQSMODE,
 *				NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetDQSMode( NX_MCUD_DQSMODE DQSmode )
{
	const U32 NDQSDISABLE	= 1UL<<27;	// MEMCFG
	const U32 CTRL_SEMODE	= 1UL<<1;	// PHYMODE
	register U32 SetValue0, SetValue1;

	NX_ASSERT( 2 > DQSmode );
	NX_ASSERT( CNULL != __g_pRegister );

//	if( NX_MCUD_DQSMODE_SINGLE == DQSmode )
//	{
//		__g_pRegister->MEMCFG	|=	NDQSDISABLE;
//		__g_pRegister->PHYMODE	|=	CTRL_SEMODE;
//	}
//	else
//	{
//		__g_pRegister->MEMCFG	&= ~NDQSDISABLE;
//		__g_pRegister->PHYMODE	&= ~CTRL_SEMODE;
//	}
	SetValue0 = __g_pRegister->MEMCFG;
	SetValue1 = __g_pRegister->PHYMODE;
	if( NX_MCUD_DQSMODE_SINGLE == DQSmode )
	{
		SetValue0	|=	NDQSDISABLE;
		SetValue1	|=	CTRL_SEMODE;
	}
	else
	{
		SetValue0	&= ~NDQSDISABLE;
		SetValue1	&= ~CTRL_SEMODE;
	}
	WriteIODW(&__g_pRegister->MEMCFG, SetValue0);
	WriteIODW(&__g_pRegister->PHYMODE, SetValue1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the DQS mode.
 *	@return		The DQS mode.
 *	@see		NX_MCUD_DQSMODE,
 *				NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
NX_MCUD_DQSMODE	NX_MCUD_GetDQSMode( void )
{
	const U32 NDQSDISABLE	= 1UL<<27;	// MEMCFG

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( ((__g_pRegister->MEMCFG>>27)&1) == ((__g_pRegister->PHYMODE>>1)&1) );

	return (__g_pRegister->MEMCFG & NDQSDISABLE) ? NX_MCUD_DQSMODE_SINGLE : NX_MCUD_DQSMODE_DIFFER;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable DLL.
 *	@param[in]	bEnb	Set this as CTRUE to enable DLL.
 *	@return		None.
 *	@remark		The system has affected by this setting after calling the function NX_MCUD_ApplyModeSetting(). 
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
void	NX_MCUD_SetDLLEnable( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 9;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( bEnb )	__g_pRegister->MEMCFG &=	~BIT_MASK;		// 0 : Enable
//	else		__g_pRegister->MEMCFG |=	 BIT_MASK;		// 1 : Disable
	SetValue = __g_pRegister->MEMCFG;
	if( bEnb )	SetValue &=	~BIT_MASK;		// 0 : Enable
	else		SetValue |=	 BIT_MASK;		// 1 : Disable
	WriteIODW(&__g_pRegister->MEMCFG, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether DLL is enabled or not.
 *	@return		\b CTRUE	indicates DLL is enabled.\n
 *				\b CFALSE	indicates DLL is disabled.
 *	@see		NX_MCUD_SetCASLatency,			NX_MCUD_GetCASLatency,
 *				NX_MCUD_SetAdditiveLatency,		NX_MCUD_GetAdditiveLatency,
 *				NX_MCUD_SetDelayLatency,		NX_MCUD_GetDelayLatency,
 *				NX_MCUD_SetDIC,					NX_MCUD_GetDIC,
 *				NX_MCUD_SetODT,					NX_MCUD_GetODT,
 *				NX_MCUD_SetDQSMode,				NX_MCUD_GetDQSMode,
 *				NX_MCUD_SetDLLEnable,			NX_MCUD_GetDLLEnable,
 *				NX_MCUD_ApplyModeSetting,		NX_MCUD_IsBusyModeSetting
 */
CBOOL	NX_MCUD_GetDLLEnable( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 9;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->MEMCFG & BIT_MASK) ? CFALSE : CTRUE;
}


//------------------------------------------------------------------------------
//	Timing settings
//------------------------------------------------------------------------------
/**
 *	@brief		Set the tRCD (RAS to CAS delay).
 *	@param[in]	clocks	Memory clocks for tRCD, 1 ~ 16.
 *	@return		None.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
void	NX_MCUD_SetRCD( U32 clocks )
{
	register U32 regval;

	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= clocks) && (16 >= clocks) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME0;
	regval &= ~BIT_MASK;
	regval |= ((clocks-1) << BIT_POS);
//	__g_pRegister->MEMTIME0 = regval;
	WriteIODW(&__g_pRegister->MEMTIME0, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the tRCD (RAS to CAS delay).
 *	@return		memory clocks for tRCD.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
U32		NX_MCUD_GetRCD( void )
{
	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMTIME0 & BIT_MASK) >> BIT_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the tRP (Row precharge time).
 *	@param[in]	clocks	Memory clocks for tRP, 1 ~ 16.
 *	@return		None.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
void	NX_MCUD_SetRP( U32 clocks )
{
	register U32 regval;

	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 4;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= clocks) && (16 >= clocks) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME0;
	regval &= ~BIT_MASK;
	regval |= ((clocks-1) << BIT_POS);
//	__g_pRegister->MEMTIME0 = regval;
	WriteIODW(&__g_pRegister->MEMTIME0, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the tRP (Row precharge time).
 *	@return		memory clocks for tRP.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
U32		NX_MCUD_GetRP( void )
{
	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 4;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMTIME0 & BIT_MASK) >> BIT_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the tRAS (Row active time, ACT to PRE command).
 *	@param[in]	clocks	Memory clocks for tRAS, 1 ~ 256.
 *	@return		None.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
void	NX_MCUD_SetRAS( U32 clocks	)
{
	register U32 regval;

	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 12;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= clocks) && (256 >= clocks) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME0;
	regval &= ~BIT_MASK;
	regval |= ((clocks-1) << BIT_POS);
//	__g_pRegister->MEMTIME0 = regval;
	WriteIODW(&__g_pRegister->MEMTIME0, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the tRAS (Row active time).
 *	@return		memory clocks for tRAS.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
U32		NX_MCUD_GetRAS( void )
{
	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 12;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMTIME0 & BIT_MASK) >> BIT_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the tRFC (Auto Refresh to RAS time).
 *	@param[in]	clocks	Memory clocks for tRFC, 1 ~ 256.
 *	@return		None.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
void	NX_MCUD_SetRFC(	U32	clocks )
{
	register U32 regval;

	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 24;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= clocks) && (256 >= clocks) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME0;
	regval &= ~BIT_MASK;
	regval |= ((clocks-1) << BIT_POS);
//	__g_pRegister->MEMTIME0 = regval;
	WriteIODW(&__g_pRegister->MEMTIME0, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the tRFC (Auto Refresh to RAS time).
 *	@return		memory clocks for tRFC.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
U32		NX_MCUD_GetRFC( void )
{
	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 24;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMTIME0 & BIT_MASK) >> BIT_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the tMRD (Mode register set cycle).
 *	@param[in]	clocks	Memory clocks for tMRD, 1 ~ 16.
 *	@return		None.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
void	NX_MCUD_SetMRD( U32 clocks )
{
	register U32 regval;

	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= clocks) && (16 >= clocks) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME1;
	regval &= ~BIT_MASK;
	regval |= ((clocks-1) << BIT_POS);
//	__g_pRegister->MEMTIME1 = regval;
	WriteIODW(&__g_pRegister->MEMTIME1, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the tMRD (Mode register set cycle).
 *	@return		memory clocks for tMRD.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
U32		NX_MCUD_GetMRD( void )
{
	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMTIME1 & BIT_MASK) >> BIT_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the tRTP (Read to Precharge time).
 *	@param[in]	clocks	Memory clocks for tRTP, 1 ~ 16.
 *	@return		None.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
void	NX_MCUD_SetRTP( U32 clocks )
{
	register U32 regval;

	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 20;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= clocks) && (16 >= clocks) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME1;
	regval &= ~BIT_MASK;
	regval |= ((clocks-1) << BIT_POS);
//	__g_pRegister->MEMTIME1 = regval;
	WriteIODW(&__g_pRegister->MEMTIME1, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the tRTP (Read to Precharge time).
 *	@return		memory clocks for tRTP.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
U32		NX_MCUD_GetRTP( void )
{
	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 20;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMTIME1 & BIT_MASK) >> BIT_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the tWR (Write recovery time).
 *	@param[in]	clocks	Memory clocks for tWR, 1 ~ 16.
 *	@return		None.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
void	NX_MCUD_SetWR( U32 clocks )
{
	register U32 regval;

	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 24;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= clocks) && (16 >= clocks) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME1;
	regval &= ~BIT_MASK;
	regval |= ((clocks-1) << BIT_POS);
//	__g_pRegister->MEMTIME1 = regval;
	WriteIODW(&__g_pRegister->MEMTIME1, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the tWR (Write recovery time).
 *	@return		memory clocks for tWR.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
U32		NX_MCUD_GetWR( void )
{
	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 24;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMTIME1 & BIT_MASK) >> BIT_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the tWTR (Write to Read time).
 *	@param[in]	clocks	Memory clocks for tWTR, 1 ~ 16.
 *	@return		None.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
void	NX_MCUD_SetWTR( U32 clocks )
{
	register U32 regval;

	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 28;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= clocks) && (16 >= clocks) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME1;
	regval &= ~BIT_MASK;
	regval |= ((clocks-1) << BIT_POS);
//	__g_pRegister->MEMTIME1 = regval;
	WriteIODW(&__g_pRegister->MEMTIME1, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the tWTR (Write to Read time).
 *	@return		memory clocks for tWTR.
 *	@see		NX_MCUD_SetRCD,	NX_MCUD_GetRCD,
 *				NX_MCUD_SetRP ,	NX_MCUD_GetRP ,
 *				NX_MCUD_SetRAS,	NX_MCUD_GetRAS,
 *				NX_MCUD_SetRFC,	NX_MCUD_GetRFC,
 *				NX_MCUD_SetMRD,	NX_MCUD_GetMRD,
 *				NX_MCUD_SetWR ,	NX_MCUD_GetWR ,
 *				NX_MCUD_SetWTR,	NX_MCUD_GetWTR,
 *				NX_MCUD_SetRTP,	NX_MCUD_GetRTP
 */
U32		NX_MCUD_GetWTR( void )
{
	const U32 BIT_SIZE	= 4;
	const U32 BIT_POS	= 28;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMTIME1 & BIT_MASK) >> BIT_POS) + 1;
}


//------------------------------------------------------------------------------
//	Active power down settings
//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable the active power down mode.
 *	@param[in]	bEnb	Set this as CTRUE to enable the active power down mode.
 *	@return		None.
 *	@see		NX_MCUD_SetActivePowerDownEnable,	NX_MCUD_GetActivePowerDownEnable, 
 *				NX_MCUD_SetActivePowerDownPeriod,	NX_MCUD_GetActivePowerDownPeriod
 */
void	NX_MCUD_SetActivePowerDownEnable( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 8;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( bEnb )		__g_pRegister->MEMACTPWD |=	BIT_MASK;
//	else			__g_pRegister->MEMACTPWD &= ~BIT_MASK;
	SetValue = __g_pRegister->MEMACTPWD;
	if( bEnb )		SetValue |=	BIT_MASK;
	else			SetValue &= ~BIT_MASK;
	WriteIODW(&__g_pRegister->MEMACTPWD, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Informs whether the active power down mode is enabled or not.
 *	@return		\b CTRUE	indicates the active power down mode is enabled.\n
 *				\b CFALSE	indicates the active power down mode is disabled.
 *	@see		NX_MCUD_SetActivePowerDownEnable,	NX_MCUD_GetActivePowerDownEnable, 
 *				NX_MCUD_SetActivePowerDownPeriod,	NX_MCUD_GetActivePowerDownPeriod
 */
CBOOL	NX_MCUD_GetActivePowerDownEnable( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 8;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->MEMACTPWD & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the active power down period.
 *	@param[in]	period	Memory clocks for the active power down period, 1 ~ 256.
 *	@return		None.
 *	@remark		The unit is a memory clock.
 *	@see		NX_MCUD_SetActivePowerDownEnable,	NX_MCUD_GetActivePowerDownEnable, 
 *				NX_MCUD_SetActivePowerDownPeriod,	NX_MCUD_GetActivePowerDownPeriod
 */
void	NX_MCUD_SetActivePowerDownPeriod( U32 period )
{
	register U32 regval;

	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( (1 <= period) && (256 >= period) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMACTPWD;
	regval &= ~BIT_MASK;
	regval |= ((period-1) << BIT_POS);
//	__g_pRegister->MEMACTPWD = regval;
	WriteIODW(&__g_pRegister->MEMACTPWD, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the active power down period.
 *	@return		memory clocks for the active power down period.
 *	@remark		The unit is a memory clock.
 *	@see		NX_MCUD_SetActivePowerDownEnable,	NX_MCUD_GetActivePowerDownEnable, 
 *				NX_MCUD_SetActivePowerDownPeriod,	NX_MCUD_GetActivePowerDownPeriod
 */
U32		NX_MCUD_GetActivePowerDownPeriod( void )
{
	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->MEMACTPWD & BIT_MASK) >> BIT_POS) + 1;
}


//------------------------------------------------------------------------------
//	Refresh period settings
//------------------------------------------------------------------------------
/**
 *	@brief		Set the refresh period.
 *	@param[in]	period	The refresh period, 1 ~ 65535 and 0 to disable.
 *	@return		None.
 *	@remark		The unit is two MCLKs and the actual refresh period is as follow: \n
 *				- Actual refresh period = (period * 2) + 1 MCLKs
 *	@see		NX_MCUD_GetRefreshPeriod
 */
void	NX_MCUD_SetRefreshPeriod( U32 period )
{
	register U32 regval;

	const U32 BIT_SIZE	= 16;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( 65535 >= period );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->MEMTIME1;
	regval &= ~BIT_MASK;

	if( period )	regval |= (period-1) << BIT_POS;
	else			regval |= (0xFFFF)	<< BIT_POS;

//	__g_pRegister->MEMTIME1 = regval;
	WriteIODW(&__g_pRegister->MEMTIME1, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the refresh period.
 *	@return		The refresh period. The unit is two MCLKs. \n
 *				If the Auto Refresh is disabled, This function returns '0'.
 *	@see		NX_MCUD_SetRefreshPeriod
 */
U32	NX_MCUD_GetRefreshPeriod( void )
{
	const U32 BIT_SIZE	= 16;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((((__g_pRegister->MEMTIME1 & BIT_MASK) >> BIT_POS) + 1) & 0xFFFF);
}


//------------------------------------------------------------------------------
//	PHY DLL Control Functions
//------------------------------------------------------------------------------
/**
 *	@brief		Determines how to lock the DLL.
 *	@param[in]	bEnb	Set this as CTRUE to lock the DLL automatically. \n
 *						Set this as CFALSE to lock the DLL manually.
 *	@return		None.
 *	@remark		"DLL" is used to get how many delay cells should be passed
 *				through the "delay line" to delay a signal to an amount of one
 *				clock period.\n
 *				This module can automatically determine how many delay cells are
 *				required to lock the DLL. To do this, You have to call this
 *				function with CTRUE argument and then call NX_MCUD_PHYDLL_Run()
 *				to try to lock the DLL. \n
 *				You can call this function with CFALSE argument to set the
 *				amount of the delay cells to lock the DLL manually.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,		NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,		NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,				NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,			NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,	NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,	NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,		NX_MCUD_PHYDLL_GetDQSDelay
 */
void	NX_MCUD_PHYDLL_SetAutoMode( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 1;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( bEnb )		__g_pRegister->PHYDLLCTRL1 |=	BIT_MASK;
//	else			__g_pRegister->PHYDLLCTRL1 &= ~BIT_MASK;
	SetValue = __g_pRegister->PHYDLLCTRL1;
	if( bEnb )		SetValue |=	BIT_MASK;
	else			SetValue &= ~BIT_MASK;
	WriteIODW(&__g_pRegister->PHYDLLCTRL1, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates how to lock the DLL.
 *	@return		CTRUE indicates the DLL is locked automatically.\n
 *				CFALSE indicates the DLL is locked manually.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,		NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,		NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,				NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,			NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,	NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,	NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,		NX_MCUD_PHYDLL_GetDQSDelay
 */
CBOOL	NX_MCUD_PHYDLL_GetAutoMode( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 1;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYDLLCTRL1 & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the configuration for the DLL.
 *	@param[in]	bLowSpeed	Set this as CTRUE if MCLK is between 80Mhz and 100 Mhz.
 *	@param[in]	dwStart		Set the initial amount of the delay cells, 0 ~ 255.\n
 *							This value is used as the initial delay when the DLL 
 *							starts tracing to find the relevant amount of the
 *							delay cells.
 *	@param[in]	dwInc		Set the increase/decrease amount of the delay cells, 0 ~ 255.\n
 *							This value is used to increase/decrease the amount
 *							of the delay cells while the DLL traces to find the
 *							relevant amount of the delay cells.
 *	@return		None.
 *	@remark		The recommended values for \e dwStart and \e dwInc are as follows:
 *				100 MHz : 8'h10 <= dwStart, dwInc <=8'h4F \n
 *				133 MHz : 8'h10 <= dwStart, dwInc <=8'h3B \n
 *				166 MHz : 8'h10 <= dwStart, dwInc <=8'h2F \n
 *				200 MHz : 8'h10 <= dwStart, dwInc <=8'h28 \n
 *				266 MHz : 8'h10 <= dwStart, dwInc <=8'h1E \n
 *				333 MHz : 8'h10 <= dwStart, dwInc <=8'h18 \n
 *				400 MHz : 8'h10 <= dwStart, dwInc <=8'h14
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,		NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,		NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,				NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,			NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,	NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,	NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,		NX_MCUD_PHYDLL_GetDQSDelay
 */
void	NX_MCUD_PHYDLL_SetConfig( CBOOL bLowSpeed, U32 dwStart, U32 dwInc )
{
	register U32 regval;

	const U32 CTRL_HALF		= 1UL<<19;
	const U32 CTRL_INC		= 0xFFUL<<11;
	const U32 CTRL_START	= 0xFFUL<< 3;

	NX_ASSERT( 256 > dwStart );
	NX_ASSERT( 256 > dwInc );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYDLLCTRL0;

	if( bLowSpeed )		regval |=	CTRL_HALF;
	else				regval &= ~CTRL_HALF;

	regval &= ~(CTRL_INC | CTRL_START);
	regval |= (dwInc	<<11);
	regval |= (dwStart<< 3);

//	__g_pRegister->PHYDLLCTRL0 = regval;
	WriteIODW(&__g_pRegister->PHYDLLCTRL0, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the configuration for the DLL.
 *	@param[out]	pbLowSpeed	CTRUE indicates MCLK is between 80Mhz and 100 Mhz.
 *	@param[out]	pdwStart	Get the initial amount of the delay cells.
 *	@param[out]	pdwInc		Get the increase/decrease amount of the delay cells.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
void	NX_MCUD_PHYDLL_GetConfig( CBOOL *pbLowSpeed, U32 *pdwStart, U32 *pdwInc )
{
	register U32 regval;

	const U32 CTRL_HALF		= 1UL<<19;
	const U32 CTRL_INC		= 0xFFUL<<11;
	const U32 CTRL_START	= 0xFFUL<< 3;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYDLLCTRL0;

	if( CNULL != pbLowSpeed )	*pbLowSpeed = (regval & CTRL_HALF ) ? CTRUE : CFALSE;
	if( CNULL != pdwInc )		*pdwInc	= (regval & CTRL_INC	) >> 11;
	if( CNULL != pdwStart )		*pdwStart	= (regval & CTRL_START) >>	3;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Start the DLL.
 *	@return		None.
 *	@remark		The following shows how to lock the DLL automaically.
 *	@code
 *		NX_MCUD_PHYDLL_SetAutoMode( CTRUE );
 *		NX_MCUD_PHYDLL_SetConfig( bLowSpeed, dwStart, dwInc );
 *		NX_MCUD_PHYDLL_Run();
 *
 *		// Do PLL change here in order to update PHY settings.
 *
 *		while( CFALSE == NX_MCUD_PHYDLL_IsLockCoarse() );
 *		while( CFALSE == NX_MCUD_PHYDLL_IsLockFine() );
 *	@endcode
 *				The followsing shows how to lock the DLL manually.
 *	@code
 *		NX_MCUD_PHYDLL_SetAutoMode( CFALSE );
 *		NX_MCUD_PHYDLL_SetConfig( bLowSpeed, dwStart, dwInc );
 *		NX_MCUD_PHYDLL_SetManualDelay( CoarseDelay );
 *		NX_MCUD_PHYDLL_Run();
 *
 *		// Do PLL change here in order to update PHY settings.
 *	@endcode
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,		NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,		NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,				NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,			NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,	NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,	NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,		NX_MCUD_PHYDLL_GetDQSDelay
 */
void	NX_MCUD_PHYDLL_Run( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PHYDLLCTRL1 |=	BIT_MASK;
	SetValue = __g_pRegister->PHYDLLCTRL1 | BIT_MASK;
	WriteIODW(&__g_pRegister->PHYDLLCTRL1, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Stop the DLL.
 *	@return		None.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
void	NX_MCUD_PHYDLL_Stop( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PHYDLLCTRL1 &= ~BIT_MASK;
	SetValue = __g_pRegister->PHYDLLCTRL1 & ~BIT_MASK;
	WriteIODW(&__g_pRegister->PHYDLLCTRL1, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the DLL is running or not.
 *	@return		CTRUE	indicates the DLL is running.\n
 *				CFALSE	indicates the DLL is not running.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
CBOOL	NX_MCUD_PHYDLL_IsRun( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYDLLCTRL1 & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the DLL is coarse locked.
 *	@return		CTRUE	indicates the DLL is coarse locked.\n
 *				CFALSE	indicates the DLL is not coarse locked.
 *	@remark		The phase offset error is less than 160 ps @ worst if DLL is
 *				coarse locked.\n
 *				You can get the amount of the delay cells by using NX_MCUD_PHYDLL_GetAutoDelay()
 *				after the DLL is locked automatically.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
CBOOL	NX_MCUD_PHYDLL_IsLockCoarse( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 3;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYDLLCTRL1 & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the DLL is fine locked.
 *	@return		CTRUE	indicates the DLL is fine locked.\n
 *				CFALSE	indicates the DLL is not fine locked.
 *	@remark		The phase offset error is less than 80 ps @ worst if DLL is
 *				fine locked.\n
 *				You can get the amount of the delay cells by using NX_MCUD_PHYDLL_GetAutoDelay()
 *				after the DLL is locked automatically.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
CBOOL	NX_MCUD_PHYDLL_IsLockFine( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 2;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYDLLCTRL1 & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the amount of the delay cells which is found by this module automatically.
 *	@param[out]	pCoarseDelay	Get the amount of the delay cells for coarse lock.
 *	@param[out]	pFineDelay		Get the amount of a quarter of the delay cells for fine lock.
 *	@return		The amount of the delay with the form as { CoarseDelay[7:0], FineDelay[1:0] }.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
U32		NX_MCUD_PHYDLL_GetAutoDelay( U32 *pCoarseDelay, U32 *pFineDelay )
{
	register U32 regval;

	const U32 CTRL_LOCKVALUE_COARSE	= 0xFFUL<<10;
	const U32 CTRL_LOCKVALUE_FINE	= 0x03UL<< 8;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYDLLFORCE;

	if( CNULL != pCoarseDelay )		*pCoarseDelay	= (regval & CTRL_LOCKVALUE_COARSE) >> 10;
	if( CNULL != pFineDelay	)		*pFineDelay	= (regval & CTRL_LOCKVALUE_FINE	) >>	8;

	return (regval & ( CTRL_LOCKVALUE_COARSE | CTRL_LOCKVALUE_FINE )) >> 8;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the amount of the delay cells manually.
 *	@param[in]	CoarseDelay		Specifies the amount of the delay cells, 0 ~ 255.
 *	@return		None.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
void	NX_MCUD_PHYDLL_SetManualDelay( U32 CoarseDelay )
{
	register U32 regval;

	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( 256 >= CoarseDelay );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYDLLFORCE;
	regval &= ~BIT_MASK;
	regval |= (CoarseDelay << BIT_POS);
//	__g_pRegister->PHYDLLFORCE = regval;
	WriteIODW(&__g_pRegister->PHYDLLFORCE, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the amount of the delay cells which was set by user.
 *	@return		The amount of the delay cells for the manual DLL lock.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
U32		NX_MCUD_PHYDLL_GetManualDelay( void )
{
	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYDLLFORCE & BIT_MASK) >> BIT_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the delay for DQS signals.
 *	@param[in]	WrOffset		Specifies the delay for the DQS output signal, -63 ~ +63.
 *	@param[in]	RdOffset		Specifies the delay for the DQS input signal, -63 ~ +63.
 *	@return		None.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
void	NX_MCUD_PHYDLL_SetDQSDelay( int WrOffset, int RdOffset )
{
	register U32 regval, offset;

	const U32 CTRL_OFFSETD_POS	= 14;
	const U32 CTRL_OFFSETD_DATA	= 0x3FUL<<CTRL_OFFSETD_POS;
	const U32 CTRL_OFFSETD_DIR	= 1UL<<20;

	const U32 CTRL_OFFSET1_POS	= 7;
	const U32 CTRL_OFFSET1_DATA	= 0x3FUL<<CTRL_OFFSET1_POS;
	const U32 CTRL_OFFSET1_DIR	= 1UL<<13;

	const U32 CTRL_OFFSET0_POS	= 0;
	const U32 CTRL_OFFSET0_DATA	= 0x3FUL<<CTRL_OFFSET0_POS;
	const U32 CTRL_OFFSET0_DIR	= 1UL<<6;

	NX_ASSERT( (-63 <= WrOffset)	&& (63 >= WrOffset) );
	NX_ASSERT( (-63 <= RdOffset)	&& (63 >= RdOffset) );
	NX_ASSERT( CNULL != __g_pRegister );

	offset = 0;
	if( 0 > WrOffset )	{	offset |= CTRL_OFFSETD_DIR;							WrOffset = -WrOffset;		}
	if( 0 > RdOffset )	{	offset |= (CTRL_OFFSET1_DIR | CTRL_OFFSET0_DIR);	RdOffset = -RdOffset;		}

	offset |= ((U32)WrOffset<<CTRL_OFFSETD_POS) | ((U32)RdOffset<<CTRL_OFFSET1_POS) | ((U32)RdOffset<<CTRL_OFFSET0_POS);

	regval = __g_pRegister->PHYDELAYCTRL;
	regval &= ~(CTRL_OFFSETD_DATA | CTRL_OFFSETD_DIR | CTRL_OFFSET1_DATA | CTRL_OFFSET1_DIR | CTRL_OFFSET0_DATA | CTRL_OFFSET0_DIR);
	regval |= offset;
//	__g_pRegister->PHYDELAYCTRL = regval;
	WriteIODW(&__g_pRegister->PHYDELAYCTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the delay for DQS signals.
 *	@param[out]	pWrOffset	Get the delay for the DQS output signal, -63 ~ +63.
 *	@param[out]	pRdOffset	Get the delay for the DQS input signal, -63 ~ +63.
 *	@return		None.
 *	@remark		The unit is 40 ps @ worst.\n
 *				Arguments which does not required can be CNULL.
 *	@see		NX_MCUD_PHYDLL_SetAutoMode,			NX_MCUD_PHYDLL_GetAutoMode,
 *				NX_MCUD_PHYDLL_SetConfig,			NX_MCUD_PHYDLL_GetConfig,
 *				NX_MCUD_PHYDLL_Run,					NX_MCUD_PHYDLL_Stop,
 *				NX_MCUD_PHYDLL_IsRun,				NX_MCUD_PHYDLL_GetAutoDelay,
 *				NX_MCUD_PHYDLL_IsLockCoarse,		NX_MCUD_PHYDLL_IsLockFine,
 *				NX_MCUD_PHYDLL_SetManualDelay,		NX_MCUD_PHYDLL_GetManualDelay,
 *				NX_MCUD_PHYDLL_SetDQSDelay,			NX_MCUD_PHYDLL_GetDQSDelay
 */
void	NX_MCUD_PHYDLL_GetDQSDelay( int *pWrOffset, int *pRdOffset )
{
	register U32 regval;
	register int offset;

	const U32 CTRL_OFFSETD_POS	= 14;
	const U32 CTRL_OFFSETD_DATA	= 0x3FUL<<CTRL_OFFSETD_POS;
	const U32 CTRL_OFFSETD_DIR	= 1UL<<20;

	#if defined(NX_DEBUG)
	const U32 CTRL_OFFSET1_POS		= 7;
	//const U32 CTRL_OFFSET1_DATA	= 0x3FUL<<CTRL_OFFSET1_POS;
	//const U32 CTRL_OFFSET1_DIR	= 1UL<<13;
	#endif

	const U32 CTRL_OFFSET0_POS	= 0;
	const U32 CTRL_OFFSET0_DATA	= 0x3FUL<<CTRL_OFFSET0_POS;
	const U32 CTRL_OFFSET0_DIR	= 1UL<<6;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYDELAYCTRL;

	if( CNULL != pRdOffset )
	{
		NX_ASSERT( ((regval>>CTRL_OFFSET1_POS) & 0x7F) == ((regval>>CTRL_OFFSET0_POS) & 0x7F) );
		offset = (int)((regval & CTRL_OFFSET0_DATA)>>CTRL_OFFSET0_POS);
		if( regval & CTRL_OFFSET0_DIR )		offset = -offset;
		
		*pRdOffset = offset;
	}

	if( CNULL != pWrOffset )
	{
		offset = (int)((regval & CTRL_OFFSETD_DATA)>>CTRL_OFFSETD_POS);
		if( regval & CTRL_OFFSETD_DIR )		offset = -offset;
		
		*pWrOffset = offset;
	}
}


//------------------------------------------------------------------------------
//	PHY ZQ Control Functions
//------------------------------------------------------------------------------
/**
 *	@brief		Determines how to do the ZQ I/O calibration for DDR2-SDRAM.
 *	@param[in]	bEnb	Set this as CTRUE to do the ZQ I/O calibration automatically. \n
 *						Set this as CFALSE to do the ZQ I/O calibration manually.
 *	@return		None.
 *	@remark		ZQ I/O calibrates the I/Os to match the driving and termination
 *				impedance by referencing resistor value of resistor(RZQ)
 *				connected externally from ZQ pin to ground. \n
 *				You can call this function with CTRUE argument to find 
 *				calibration values by this module automaically.\n
 *				If the result of the auto calibration is fail or you want to 
 *				set calibration values manually, You should call this function
 *				with CFALSE argument.\n
 *				The following shows how to do the ZQ I/O calibration.
 *	@code
 *				NX_MCUD_PHYZQ_Stop();
 *				NX_MCUD_PHYZQ_SetAutoMode( CTRUE );
 *				NX_MCUD_PHYZQ_Run();
 *					
 *				do
 *				{
 *					if( NX_MCUD_PHYZQ_IsError() )
 *					{
 *						NX_MCUD_PHYZQ_Stop();
 *						NX_MCUD_PHYZQ_SetAutoMode( CFALSE );
 *						NX_MCUD_PHYZQ_SetControlCode( PullUp, PullDown );
 *						break;
 *					}
 *				} while( CFALSE == NX_MCUD_PHYZQ_IsCompleted() );
 *				
 *				NX_MCUD_PHYZQ_Update();
 *	@endcode
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
void	NX_MCUD_PHYZQ_SetAutoMode( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 1;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	if( bEnb )		__g_pRegister->PHYZQCTRL &= ~BIT_MASK;	// 0 : Auto
//	else			__g_pRegister->PHYZQCTRL |=	BIT_MASK;	// 1 : Manual
	SetValue = __g_pRegister->PHYZQCTRL;
	if( bEnb )		SetValue &= ~BIT_MASK;	// 0 : Auto
	else			SetValue |=	BIT_MASK;	// 1 : Manual
	WriteIODW(&__g_pRegister->PHYZQCTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates how to do the ZQ I/O calibration for DDR2-SDRAM.
 *	@return		CTRUE indicates this module sets the calibration values automaically.\n
 *				CFALSE indicates the user sets the calibration values manually.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
CBOOL	NX_MCUD_PHYZQ_GetAutoMode( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 1;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYZQCTRL & BIT_MASK) ? CFALSE : CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the configuration for the termination resistor control.
 *	@param[in]	ODT			Specifies the On-Die-Termination resistor value.
 *	@return		None.
 *	@see		NX_MCUD_ODT,
 *				NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
void	NX_MCUD_PHYZQ_SetODT( NX_MCUDPHY_ODT ODT )
{
	register U32 regval;
//	U32 dwODT;

	const U32 ODTENB_MASK	= 1UL<<0;
	const U32 ODT_POS		= 1;
	const U32 ODT_MASK		= 7UL<<ODT_POS;

	NX_ASSERT( 4 > ODT );
	NX_ASSERT( CNULL != __g_pRegister );

//	switch( ODT )
//	{
//	case NX_MCUDPHY_ODT_50 :		dwODT = 3;	break;
//	case NX_MCUDPHY_ODT_75 :		dwODT = 2;	break;
//	case NX_MCUDPHY_ODT_DISABLE:
//	case NX_MCUDPHY_ODT_150:		
//	default:					dwODT = 1;	break;
//	}

	regval = __g_pRegister->PHYTERMCTRL;

	regval &= ~(ODT_MASK);
//	regval |= (dwODT << ODT_POS);
	regval |= (ODT << ODT_POS);

	if( NX_MCUDPHY_ODT_DISABLE==ODT )		regval |=	ODTENB_MASK;	// 1 : disable Termination
	else								regval &= ~ODTENB_MASK;		// 0 : enable Termination

//	__g_pRegister->PHYTERMCTRL = regval;
	WriteIODW(&__g_pRegister->PHYTERMCTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the On-Die-Termination resistance value for PHY ZQ.
 *	@return		The On-Die-Termination resistance value for PHY ZQ.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUD_ODT,
 *				NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
NX_MCUDPHY_ODT	NX_MCUD_PHYZQ_GetODT( void )
{
	register U32 regval;

	const U32 ODTENB_MASK	= 1UL<<0;
	const U32 ODT_POS		= 1;
	const U32 ODT_MASK		= 7UL<<ODT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYTERMCTRL;

	if( regval & ODTENB_MASK )
	{
		return NX_MCUDPHY_ODT_DISABLE;
	}

	regval = (regval & ODT_MASK) >> ODT_POS;
	NX_ASSERT( (4 > regval) && (0 != regval) );

	if( 1 == regval )			return NX_MCUDPHY_ODT_150;
	else if( 2 == regval )		return NX_MCUDPHY_ODT_75;
	else if( 3 == regval )		return NX_MCUDPHY_ODT_50;

	return NX_MCUDPHY_ODT_DISABLE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the immediate control code for the ZQ I/O calibration of DDR2-SDRAM.
 *	@param[in]	PullUp		Specifies the immediate control code for pull-up, 0 ~ 7.\n
 *							It's recommended to set this as 2.
 *	@param[in]	PullDown	Specifies the immediate control code for pull-down, 0 ~ 7.\n
 *							It's recommended to set this as 5.
 *	@return		None.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
void	NX_MCUD_PHYZQ_SetControlCode( U32 PullUp, U32 PullDown )
{
	register U32 regval;

	const U32 IMPP_POS		= 3;
	const U32 IMPP_MASK		= 7UL<<IMPP_POS;
	const U32 IMPN_POS		= 0;
	const U32 IMPN_MASK		= 7UL<<IMPN_POS;

	NX_ASSERT( 8 > PullUp );
	NX_ASSERT( 8 > PullDown );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYZQFORCE;

	regval &= ~(IMPP_MASK | IMPN_MASK);
	regval |= (PullUp	<< IMPP_POS);
	regval |= (PullDown << IMPN_POS);

//	__g_pRegister->PHYZQFORCE = regval;
	WriteIODW(&__g_pRegister->PHYZQFORCE, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the immediate control code for the ZQ I/O calibration of DDR2-SDRAM.
 *	@param[out]	pPullUp		Get the immediate control code for pull-up, 0 ~ 7.
 *	@param[out]	pPullDown	Get the immediate control code for pull-down, 0 ~ 7.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
void	NX_MCUD_PHYZQ_GetControlCode( U32 *pPullUp, U32 *pPullDown )
{
	register U32 regval;

	const U32 IMPP_POS		= 3;
	const U32 IMPP_MASK		= 7UL<<IMPP_POS;
	const U32 IMPN_POS		= 0;
	const U32 IMPN_MASK		= 7UL<<IMPN_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYZQFORCE;

	if( CNULL != pPullUp )		*pPullUp	= (regval & IMPP_MASK) >> IMPP_POS;
	if( CNULL != pPullDown )	*pPullDown	= (regval & IMPN_MASK) >> IMPN_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the control codes found by the auto ZQ I/O calibration for DDR2-SDRAM.
 *	@param[out]	pPullUp		Get the control code for pull-up found by the auto ZQ I/O calibration.
 *	@param[out]	pPullDown	Get the control code for pull-down found by the auto ZQ I/O calibration.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
void	NX_MCUD_PHYZQ_GetAutoResult( U32 *pPullUp, U32 *pPullDown )
{
	register U32 regval;

	const U32 PMON_POS		= 9;
	const U32 PMON_MASK		= 7UL<<PMON_POS;
	const U32 NMON_POS		= 6;
	const U32 NMON_MASK		= 7UL<<NMON_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PHYZQFORCE;

	if( CNULL != pPullUp )		*pPullUp	= (regval & PMON_MASK) >> PMON_POS;
	if( CNULL != pPullDown )	*pPullDown	= (regval & NMON_MASK) >> NMON_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Start the auto ZQ I/O calibration for DDR2-SDRAM.
 *	@return		None.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
void	NX_MCUD_PHYZQ_Run( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

	
//	__g_pRegister->PHYZQCTRL |= BIT_MASK;
	SetValue = __g_pRegister->PHYZQCTRL | BIT_MASK;
	WriteIODW(&__g_pRegister->PHYZQCTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Stop the auto ZQ I/O calibration for DDR2-SDRAM.
 *	@return		None.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
void	NX_MCUD_PHYZQ_Stop( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	register U32 SetValue;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PHYZQCTRL &= ~BIT_MASK;
	SetValue = __g_pRegister->PHYZQCTRL & ~BIT_MASK;
	WriteIODW(&__g_pRegister->PHYZQCTRL, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Determines whether the auto ZQ I/O calibration for DDR2-SDRAM is started or not.
 *	@return		CTRUE indicates the auto ZQ I/O calibration is started.\n
 *				CFALSE indicates the auto ZQ I/O calibration is not started.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
CBOOL	NX_MCUD_PHYZQ_IsRun( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYZQCTRL & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Determines whether the auto ZQ I/O calibration is completed with error.
 *	@return		CTRUE indicates the auto ZQ I/O calibration is completed with error.\n
 *				CFALSE indicates the auto ZQ I/O calibration is not completed or the
 *				auto ZQ I/O calibartion is completed without error.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
CBOOL	NX_MCUD_PHYZQ_IsError( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 3;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYZQCTRL & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Determines whether the auto ZQ I/O calibration is completed without error.
 *	@return		CTRUE indicates the auto ZQ I/O calibration is completed without error.\n
 *				CFALSE indicates the auto ZQ I/O calibration is not completed or the
 *				auto ZQ I/O calibartion is completed with error.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
CBOOL	NX_MCUD_PHYZQ_IsCompleted( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 2;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->PHYZQCTRL & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Updates control codes for the ZQ I/O calibration.
 *	@return		None.
 *	@see		NX_MCUD_PHYZQ_SetAutoMode	,	NX_MCUD_PHYZQ_GetAutoMode	,
 *				NX_MCUD_PHYZQ_SetODT		,	NX_MCUD_PHYZQ_GetODT		,
 *				NX_MCUD_PHYZQ_SetControlCode,	NX_MCUD_PHYZQ_GetControlCode,
 *				NX_MCUD_PHYZQ_GetAutoResult ,	NX_MCUD_PHYZQ_Run			,
 *				NX_MCUD_PHYZQ_Stop			,	NX_MCUD_PHYZQ_IsRun			,
 *				NX_MCUD_PHYZQ_IsError		,	NX_MCUD_PHYZQ_IsCompleted	,
 *				NX_MCUD_PHYZQ_Update
 */
void	NX_MCUD_PHYZQ_Update( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PHYUPDATE = BIT_MASK;
	WriteIODW(&__g_pRegister->PHYUPDATE, BIT_MASK);
}


//------------------------------------------------------------------------------
//	Memory Arbiter Settings
//------------------------------------------------------------------------------
/**
 *	@brief		Set a master for fast arbiter channel.
 *	@param[in]	channel		Specifies a channel to modify, 0 ~ 15.
 *	@param[in]	master		Specifies a master, which is one of @ref NX_MCUD_FAST enum, for selected channel.
 *	@return		None.
 *	@see		NX_MCUD_FAST,
 *				NX_MCUD_SetFastArbiter,	NX_MCUD_GetFastArbiter,
 *				NX_MCUD_SetSlowArbiter,	NX_MCUD_GetSlowArbiter
 */
void	NX_MCUD_SetFastArbiter( U32 channel, NX_MCUD_FAST master )
{
	register U32 regval, bitpos, index;
	register volatile U32 *pRegister;

	const U32 BIT_SIZE	= (5);
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1);

	NX_ASSERT( 16 > channel );
	NX_ASSERT( (2 > master) || (16==master) );
	NX_ASSERT( CNULL != __g_pRegister );

	/*
	bitpos	= (channel % 6) * 5;
	index	= (channel / 6);
	*/
	if( 6 > channel )
	{
		bitpos	= channel;
		index	= 0;
	}
	else if( 12 > channel )
	{
		bitpos	= channel - 6;
		index	= 1;
	}
	else
	{
		bitpos	= channel - 12;
		index	= 2;
	}

	bitpos = (bitpos << 2) + bitpos;
	pRegister = &__g_pRegister->FASTCH[ index ];

	regval = *pRegister;
	regval &= ~(BIT_MASK << bitpos);
	regval |= (master << bitpos);
//	*pRegister = regval;
	WriteIODW(pRegister, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a master for fast arbiter channel.
 *	@param[in]	channel		Specifies a channel, 0 ~ 15.
 *	@return		A master, which is one of @ref NX_MCUD_FAST enum, for selected channel.
 *	@see		NX_MCUD_FAST,
 *				NX_MCUD_SetFastArbiter,	NX_MCUD_GetFastArbiter,
 *				NX_MCUD_SetSlowArbiter,	NX_MCUD_GetSlowArbiter
 */
NX_MCUD_FAST	NX_MCUD_GetFastArbiter( U32 channel )
{
	register U32 regval, bitpos, index;
	register volatile U32 *pRegister;

	const U32 BIT_SIZE	= (5);
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1);

	NX_ASSERT( 16 > channel );
	NX_ASSERT( CNULL != __g_pRegister );

	/*
	bitpos	= (channel % 6) * 5;
	index	= (channel / 6);
	*/
	if( 6 > channel )
	{
		bitpos	= channel;
		index	= 0;
	}
	else if( 12 > channel )
	{
		bitpos	= channel - 6;
		index	= 1;
	}
	else
	{
		bitpos	= channel - 12;
		index	= 2;
	}

	bitpos = (bitpos << 2) + bitpos;
	pRegister = &__g_pRegister->FASTCH[ index ];

	regval = *pRegister;
	return (NX_MCUD_FAST)((regval >> bitpos) & BIT_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a master for slow arbiter channel.
 *	@param[in]	channel		Specifies a channel to modify, 0 ~ 15.
 *	@param[in]	master		Specifies a master, which is one of @ref NX_MCUD_SLOW enum, for selected channel.
 *	@return		None.
 *	@see		NX_MCUD_SLOW,
 *				NX_MCUD_SetFastArbiter,	NX_MCUD_GetFastArbiter,
 *				NX_MCUD_SetSlowArbiter,	NX_MCUD_GetSlowArbiter
 */
void	NX_MCUD_SetSlowArbiter( U32 channel, NX_MCUD_SLOW master )
{
	register U32 regval, bitpos, index;
	register volatile U32 *pRegister;

	const U32 BIT_SIZE	= (5);
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1);

	NX_ASSERT( 16 > channel );
	NX_ASSERT( 16 >= master );
	NX_ASSERT( CNULL != __g_pRegister );

	/*
	bitpos	= (channel % 6) * 5;
	index	= (channel / 6);
	*/
	if( 6 > channel )
	{
		bitpos	= channel;
		index	= 0;
	}
	else if( 12 > channel )
	{
		bitpos	= channel - 6;
		index	= 1;
	}
	else
	{
		bitpos	= channel - 12;
		index	= 2;
	}

	bitpos = (bitpos << 2) + bitpos;
	pRegister = &__g_pRegister->SLOWCH[ index ];

	regval = *pRegister;
	regval &= ~(BIT_MASK << bitpos);
	regval |= (master << bitpos);
//	*pRegister = regval;
	WriteIODW(pRegister, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a master for slow arbiter channel.
 *	@param[in]	channel		Specifies a channel, 0 ~ 15.
 *	@return		A master, which is one of @ref NX_MCUD_SLOW enum, for selected channel.
 *	@see		NX_MCUD_SLOW,
 *				NX_MCUD_SetFastArbiter,	NX_MCUD_GetFastArbiter,
 *				NX_MCUD_SetSlowArbiter,	NX_MCUD_GetSlowArbiter
 */
NX_MCUD_SLOW	NX_MCUD_GetSlowArbiter( U32 channel )
{
	register U32 regval, bitpos, index;
	register volatile U32 *pRegister;

	const U32 BIT_SIZE	= (5);
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1);

	NX_ASSERT( 16 > channel );
	NX_ASSERT( CNULL != __g_pRegister );

	/*
	bitpos	= (channel % 6) * 5;
	index	= (channel / 6);
	*/
	if( 6 > channel )
	{
		bitpos	= channel;
		index	= 0;
	}
	else if( 12 > channel )
	{
		bitpos	= channel - 6;
		index	= 1;
	}
	else
	{
		bitpos	= channel - 12;
		index	= 2;
	}

	bitpos = (bitpos << 2) + bitpos;
	pRegister = &__g_pRegister->SLOWCH[ index ];

	regval = *pRegister;
	return (NX_MCUD_SLOW)((regval >> bitpos) & BIT_MASK);
}


//------------------------------------------------------------------------------
// Performance Counter Functions
//------------------------------------------------------------------------------
/**
 *	@brief		Initializes the performance counters with initial value.
 *	@param[in]	initvalue		Specifies an initial value for counters.
 *	@return		None.
 *	@see		NX_MCUD_SetInitPerformanceCounter,	NX_MCUD_GetInitPerformanceCounter,
 *				NX_MCUD_GetPerformanceCounter
 */
void	NX_MCUD_SetInitPerformanceCounter( U32 initvalue )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->INITPERFCNT = initvalue;
	WriteIODW(&__g_pRegister->INITPERFCNT, initvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an initial value for the performance counters.
 *	@return		An initial value for the performance counters.
 *	@see		NX_MCUD_SetInitPerformanceCounter,	NX_MCUD_GetInitPerformanceCounter,
 *				NX_MCUD_GetPerformanceCounter
 */
U32		NX_MCUD_GetInitPerformanceCounter( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->INITPERFCNT;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a current value for the selected counter.
 *	@param[in]	index		Specifies a counter which is one of @ref NX_MCUD_COUNTER enum.
 *	@return		A current counter value.
 *	@see		NX_MCUD_COUNTER,
 *				NX_MCUD_SetInitPerformanceCounter,	NX_MCUD_GetInitPerformanceCounter,
 *				NX_MCUD_GetPerformanceCounter
 */
U32		NX_MCUD_GetPerformanceCounter( NX_MCUD_COUNTER index )
{
	NX_ASSERT( 22 > index );
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->VALIDCNT[ index ];
}
