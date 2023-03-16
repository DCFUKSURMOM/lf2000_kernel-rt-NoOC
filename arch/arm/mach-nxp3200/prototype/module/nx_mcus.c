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
//	Module		: MCUS
//	File		: nx_mcus.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_mcus.h"


static	struct NX_MCUS_RegisterSet *__g_pRegister = CNULL;


//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.\n
 *	@see	NX_MCUS_GetNumberOfModule
 */
CBOOL	NX_MCUS_Initialize( void )
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
 *	@see		NX_MCUS_Initialize
 */
U32		NX_MCUS_GetNumberOfModule( void )
{
	return NUMBER_OF_MCUS_MODULE;
}


//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_MCUS_GetSizeOfRegisterSet,
 *				NX_MCUS_SetBaseAddress,		NX_MCUS_GetBaseAddress,
 *				NX_MCUS_OpenModule,			NX_MCUS_CloseModule,
 *				NX_MCUS_CheckBusy,			NX_MCUS_CanPowerDown
 */
U32		NX_MCUS_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_MCUS_MODULE + (OFFSET_OF_MCUS_MODULE * 0) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_MCUS_GetPhysicalAddress,
 *				NX_MCUS_SetBaseAddress,		NX_MCUS_GetBaseAddress,
 *				NX_MCUS_OpenModule,			NX_MCUS_CloseModule,
 *				NX_MCUS_CheckBusy,			NX_MCUS_CanPowerDown
 */
U32		NX_MCUS_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_MCUS_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_MCUS_GetPhysicalAddress,	NX_MCUS_GetSizeOfRegisterSet,
 *				NX_MCUS_GetBaseAddress,
 *				NX_MCUS_OpenModule,			NX_MCUS_CloseModule,
 *				NX_MCUS_CheckBusy,			NX_MCUS_CanPowerDown
 */
void	NX_MCUS_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	__g_pRegister = (struct NX_MCUS_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_MCUS_GetPhysicalAddress,	NX_MCUS_GetSizeOfRegisterSet,
 *				NX_MCUS_SetBaseAddress,
 *				NX_MCUS_OpenModule,			NX_MCUS_CloseModule,
 *				NX_MCUS_CheckBusy,			NX_MCUS_CanPowerDown
 */
U32		NX_MCUS_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_MCUS_GetPhysicalAddress,	NX_MCUS_GetSizeOfRegisterSet,
 *				NX_MCUS_SetBaseAddress,		NX_MCUS_GetBaseAddress,
 *				NX_MCUS_CloseModule,
 *				NX_MCUS_CheckBusy,			NX_MCUS_CanPowerDown
 */
CBOOL	NX_MCUS_OpenModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_MCUS_GetPhysicalAddress,	NX_MCUS_GetSizeOfRegisterSet,
 *				NX_MCUS_SetBaseAddress,		NX_MCUS_GetBaseAddress,
 *				NX_MCUS_OpenModule,
 *				NX_MCUS_CheckBusy,			NX_MCUS_CanPowerDown
 */
CBOOL	NX_MCUS_CloseModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_MCUS_GetPhysicalAddress,	NX_MCUS_GetSizeOfRegisterSet,
 *				NX_MCUS_SetBaseAddress,		NX_MCUS_GetBaseAddress,
 *				NX_MCUS_OpenModule,			NX_MCUS_CloseModule,
 *				NX_MCUS_CanPowerDown
 */
CBOOL	NX_MCUS_CheckBusy( void )
{
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_MCUS_GetPhysicalAddress,	NX_MCUS_GetSizeOfRegisterSet,
 *				NX_MCUS_SetBaseAddress,		NX_MCUS_GetBaseAddress,
 *				NX_MCUS_OpenModule,			NX_MCUS_CloseModule,
 *				NX_MCUS_CheckBusy
 */
CBOOL	NX_MCUS_CanPowerDown( void )
{
	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
S32		NX_MCUS_GetInterruptNumber( void )
{
	return	INTNUM_OF_MCUS_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number .
 *	@param[in]	Enable	\b CTRUE	indicate that Interrupt Enable. \n
 *						\b CFALSE	indicate that Interrupt Disable.
 *	@return		None.
 *	@remarks	MCUS has one interrupt source, therefore you have to set @a IntNum
 *				as '0' only.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32 	,
 *				NX_MCUS_GetInterruptPendingNumber
 */
void	NX_MCUS_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	const U32 IRQPEND_POS	= 15;
	const U32 ECCRST_POS	= 11;
	const U32 IRQENB_POS	= 8;
	const U32 IRQPEND		= (1UL<<IRQPEND_POS);
	const U32 ECCRST		= (1UL<<ECCRST_POS);
	const U32 IRQENB		= (1UL<<IRQENB_POS);

	register U32 regvalue;
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = ReadIODW(&pRegister->NFCONTROL);
	regvalue &= ~(IRQPEND | ECCRST | IRQENB);
	regvalue |= (U32)Enable<<IRQENB_POS;
	WriteIODW(&pRegister->NFCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number.
 *	@return		\b CTRUE	indicates that specified interrupt is enabled. \n
 *				\b CFALSE indicates that specified interrupt is disabled.
 *	@remarks	MCUS has one interrupt source, therefore you have to set @a IntNum
 *				as '0' only.
 *	@see		NX_MCUS_INT					,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending	,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32 ,
 *				NX_MCUS_GetInterruptPendingNumber
 */
CBOOL	NX_MCUS_GetInterruptEnable( S32 IntNum )
{
	const U32 IRQENB_POS	= 8;
	const U32 IRQENB_MASK	= (1UL<<IRQENB_POS);

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (ReadIODW(&__g_pRegister->NFCONTROL) & IRQENB_MASK) >> IRQENB_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number.
 *	@return		\b CTRUE	indicates that specified interrupr is pended. \n
 *				\b CFALSE indicates that specified interrupt is not pended.
 *	@remarks	MCUS has one interrupt source, therefore you have to set @a IntNum
 *				as '0' only.
 *	@see		NX_MCUS_INT					,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending	,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32 ,
 *				NX_MCUS_GetInterruptPendingNumber
 */
CBOOL	NX_MCUS_GetInterruptPending( S32 IntNum )
{
	const U32 IRQPEND_POS	= 15;
	const U32 IRQPEND_MASK	= (1UL<<IRQPEND_POS);

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (ReadIODW(&__g_pRegister->NFCONTROL) & IRQPEND_MASK) >> IRQPEND_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number.
 *	@return		None.
 *	@remarks	MCUS has one interrupt source, therefore you have to set @a IntNum
 *				as '0' only.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
void	NX_MCUS_ClearInterruptPending( S32 IntNum )
{
	const U32 IRQPEND_POS	= 15;
	const U32 ECCRST_POS	= 11;
	const U32 IRQPEND		= (1UL<<IRQPEND_POS);
	const U32 ECCRST		= (1UL<<ECCRST_POS);

	register U32 regvalue;
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = ReadIODW(&pRegister->NFCONTROL);
	regvalue &= ~(IRQPEND | ECCRST );
	regvalue |= IRQPEND;
	WriteIODW(&pRegister->NFCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enabled or disabled.
 *	@param[in]	Enable	Set this as CTURE to enable all interrupts. \n
 *						Set this as CFALSE to disable all interrupts.
 *	@return		None.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
void	NX_MCUS_SetInterruptEnableAll( CBOOL Enable )
{
	const U32 IRQPEND_POS	= 15;
	const U32 ECCRST_POS	= 11;
	const U32 IRQENB_POS	= 8;
	const U32 IRQPEND		= (1UL<<IRQPEND_POS);
	const U32 ECCRST		= (1UL<<ECCRST_POS);
	const U32 IRQENB		= (1UL<<IRQENB_POS);

	register U32 regvalue;
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = ReadIODW(&pRegister->NFCONTROL);
	regvalue &= ~(IRQPEND | ECCRST | IRQENB);
	regvalue |= ( (U32)Enable<<IRQENB_POS );
	WriteIODW(&pRegister->NFCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicates that one or more interrupts are enabled. \n
 *				\b CFALSE	indicates that all interrupts are disabled.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
CBOOL	NX_MCUS_GetInterruptEnableAll( void )
{
	const U32 IRQENB_POS	= 8;
	const U32 IRQENB_MASK	= (1UL<<IRQENB_POS);

	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (ReadIODW(&__g_pRegister->NFCONTROL) & IRQENB_MASK) >> IRQENB_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of unmasked interrupts are pended or not.
 *	@return		CTURE indicates there's unmasked interrupts which are pended.\n
 *				CFALSE indicates there's no unmasked interrupt which are pended.
 *	@remark		Since this function doesn't consider about pending status of
 *				interrupts which are disabled, the return value can be CFALSE
 *				even though some interrupts are pended unless a relevant
 *				interrupt is enabled.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
CBOOL	NX_MCUS_GetInterruptPendingAll( void )
{
	const U32 IRQPEND_POS	= 15;
	const U32 IRQENB_POS	= 8;

	const U32 IRQPEND_MASK	= (1UL<<IRQPEND_POS);
	const U32 IRQENB_MASK	= (1UL<<IRQENB_POS);

	register U32 regval, intpend, intenb;

	NX_ASSERT( CNULL != __g_pRegister );

	regval	= ReadIODW(&__g_pRegister->NFCONTROL);
	intpend	= (regval & IRQPEND_MASK) >> IRQPEND_POS;
	intenb	= (regval & IRQENB_MASK ) >> IRQENB_POS;

	return	(CBOOL)( intpend & intenb );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
void	NX_MCUS_ClearInterruptPendingAll( void )
{
	const U32 IRQPEND_POS	= 15;
	const U32 ECCRST_POS	= 11;
	const U32 IRQPEND		= (1UL<<IRQPEND_POS);
	const U32 ECCRST		= (1UL<<ECCRST_POS);

	register U32 regvalue;
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = ReadIODW(&pRegister->NFCONTROL);
	regvalue &= ~(IRQPEND | ECCRST );
	regvalue |= IRQPEND;
	WriteIODW(&pRegister->NFCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set interrupts to be enabled or disabled.
 *	@param[in]	EnableFlag		Specifies an interrupt enable flag that each bit
 *								represents an interrupt enable status to be changed -
 *								Value of 0 masks interrupt and value of 1 enables
 *								interrupt. EnableFlag[0] are only valid.
 *	@return		None.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
void	NX_MCUS_SetInterruptEnable32 ( U32 EnableFlag )
{
	const U32 IRQPEND_POS	= 15;
	const U32 ECCRST_POS	= 11;
	const U32 IRQENB_POS	= 8;
	const U32 IRQPEND		= (1UL<<IRQPEND_POS);
	const U32 ECCRST		= (1UL<<ECCRST_POS);
	const U32 IRQENB		= (1UL<<IRQENB_POS);

	register U32 regvalue;
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = ReadIODW(&pRegister->NFCONTROL);
	regvalue &= ~(IRQPEND | ECCRST | IRQENB);
	regvalue |= ( (U32)(EnableFlag & 1)<<IRQENB_POS );
	WriteIODW(&pRegister->NFCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt enable status.
 *	@return		An interrupt enable status that each bit represents current
 *				interrupt enable status - Value of 0 indicates relevant interrupt
 *				is masked and value of 1 indicates relevant interrupt is enabled.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
U32	NX_MCUS_GetInterruptEnable32 ( void )
{
	const U32 IRQENB_POS	= 8;
	const U32 IRQENB		= (1UL<<IRQENB_POS);

	NX_ASSERT( CNULL != __g_pRegister );

	return (ReadIODW(&__g_pRegister->NFCONTROL) & IRQENB)>>IRQENB_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt pending status.
 *	@return		An interrupt pending status that each bit represents current
 *				interrupt pending status - Value of 0 indicates relevant interrupt
 *				is not pended and value of 1 indicates relevant interrupt is pended.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
U32	NX_MCUS_GetInterruptPending32 ( void )
{
	const U32 IRQPEND_POS	= 15;
	const U32 IRQPEND		= (1UL<<IRQPEND_POS);

	NX_ASSERT( CNULL != __g_pRegister );

	return (ReadIODW(&__g_pRegister->NFCONTROL) & IRQPEND)>>IRQPEND_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear an interrupt pending status.
 *	@param[in]	PendingFlag		Specifies an interrupt pending clear flag. An
 *								interrupt pending status cleared only if
 *								corresponding bit in PendingFlag is set.
 *	@return		None.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
void	NX_MCUS_ClearInterruptPending32( U32 PendingFlag )
{
	const U32 IRQPEND_POS	= 15;
	const U32 ECCRST_POS	= 11;
	const U32 IRQPEND		= (1UL<<IRQPEND_POS);
	const U32 ECCRST		= (1UL<<ECCRST_POS);

	register U32 regvalue;
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = ReadIODW(&pRegister->NFCONTROL);
	regvalue &= ~(IRQPEND | ECCRST);
	regvalue |= ( (U32)(PendingFlag & 1)<<IRQPEND_POS );
	WriteIODW(&pRegister->NFCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get an interrupt number which has the most prority of pended interrupts.
 *	@return		an interrupt number which has the most priority of pended and
 *				unmasked interrupts.
 *				If there's no interrupt which is pended and unmasked, it returns -1.
 *	@see		NX_MCUS_INT						,	NX_MCUS_GetInterruptNumber			,
 *				NX_MCUS_SetInterruptEnable		,	NX_MCUS_GetInterruptEnable			,
 *				NX_MCUS_GetInterruptPending		,	NX_MCUS_ClearInterruptPending		,
 *				NX_MCUS_SetInterruptEnableAll	,	NX_MCUS_GetInterruptEnableAll		,
 *				NX_MCUS_GetInterruptPendingAll	,	NX_MCUS_ClearInterruptPendingAll	,
 *				NX_MCUS_SetInterruptEnable32	,	NX_SDHC_GetInterruptEnable32		,
 *				NX_MCUS_GetInterruptPending32	,	NX_MCUS_ClearInterruptPending32		,
 *				NX_MCUS_GetInterruptPendingNumber
 */
S32		NX_MCUS_GetInterruptPendingNumber( void )
{
	const U32 IRQPEND_POS	= 15;
	const U32 IRQENB_POS	= 8;

	const U32 IRQPEND_MASK	= (1UL<<IRQPEND_POS);
	const U32 IRQENB_MASK	= (1UL<<IRQENB_POS);

	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFCONTROL);

	if( (regval & IRQENB_MASK) && (regval & IRQPEND_MASK) )
	{
		return 0;
	}

	return -1;
}


//------------------------------------------------------------------------------
// DMA Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Get DMA peripheral index of MCUS
 *	@return DMA peripheral index of MCUS
 *	@see	NX_MCUS_GetDMABusWidth
 */
U32		NX_MCUS_GetDMAIndex( void )
{
	return 0;//DMAINDEX_OF_MCUS_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get bus width of MCUS
 *	@return		DMA bus width of MCUS
 *	@see		NX_MCUS_GetDMAIndex
 */
U32		NX_MCUS_GetDMABusWidth( void )
{
	//const U32 DMAWIDTH = (1UL<<8);

	NX_ASSERT( CNULL != __g_pRegister );

	return 32; //(__g_pRegister->IDEDMACTRL & DMAWIDTH) ? 16 : 8;
}


//------------------------------------------------------------------------------
//	Static BUS Configuration.
//------------------------------------------------------------------------------
/**
 *	@brief		Get the static bus index of internal SRAM.
 *	@return		the static bus index of internal SRAM which is one of @ref NX_MCUS_SBUSID enum.
 *	@remark		The internal SRAM has different static bus index according to
 *				the external configuration - CfgBootMode.\n
 *				For NANDBOOT or UARTBOOT, the internal SRAM must be at static
 *				bus 0 to be mapped at 0x0 of memory address.\n
 *				For booting of ROM or NOR flash memory, static bus 0 should be
 *				used for these memory. therefore the internal SRAM must be
 *				other static bus except static bus 0 - static bus 6 is used for
 *				the internal SRAM.
 *	@see		NX_MCUS_SBUSID,
 *				NX_MCUS_GetISRAMCS,			NX_MCUS_GetIROMCS,
 *				NX_MCUS_SetStaticBUSConfig,	NX_MCUS_GetStaticBUSConfig
 */
NX_MCUS_SBUSID		NX_MCUS_GetISRAMCS( void )
{
	const U32 ISRAMSHADOW = (1UL<<31);

	NX_ASSERT( CNULL != __g_pRegister );

	return (ReadIODW(&__g_pRegister->MEMBW) & ISRAMSHADOW) ? NX_MCUS_SBUSID_STATIC12 : NX_MCUS_SBUSID_STATIC0;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the static bus index of internal ROM.
 *	@return		the static bus index of internal ROM which is one of @ref NX_MCUS_SBUSID enum.
 *	@remark		The internal ROM has different static bus index according to
 *				the external configuration - CfgBootMode.\n
 *				For iROMBOOT, the internal ROM must be at static bus 0 to be
 *				mapped at 0x0 of memory address.\n
 *				For other boot mode, static bus 0 should be used for other
 *				memory. therefore the internal ROM must be other static bus
 *				except static bus 0 - static bus 7 is used for the internal ROM.
 *	@see		NX_MCUS_SBUSID,
 *				NX_MCUS_GetISRAMCS,			NX_MCUS_GetIROMCS,
 *				NX_MCUS_SetStaticBUSConfig,	NX_MCUS_GetStaticBUSConfig
 */
NX_MCUS_SBUSID		NX_MCUS_GetIROMCS( void )
{
	const U32 IROMSHADOW = (1UL<<30);

	NX_ASSERT( CNULL != __g_pRegister );

	return (ReadIODW(&__g_pRegister->MEMBW) & IROMSHADOW) ? NX_MCUS_SBUSID_STATIC13 : NX_MCUS_SBUSID_STATIC0;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set static BUS configration.
 *	@param[in]	Id			Static BUS ID which is one of @ref NX_MCUS_SBUSID enum.
 *	@param[in]	BitWidth	BUS bit width. ( it must be 8 or 16 ). \n This argument is only valid in case of Static #0 ~ #7.
 *	@param[in]	tACS		address setup time before nCS, 0 ~ 15 for Static #0 ~ #7 and NAND, 1 ~ 16 for IDE.
 *	@param[in]	tCAH		address hold time after nCS, 0 ~ 15 for Static #0 ~ #7 and NAND, 1 ~ 16 for IDE.
 *	@param[in]	tCOS		chip select setup time before nOE, 0 ~ 15 for Static #0 ~ #7 and NAND, 1 ~ 16 for IDE.
 *	@param[in]	tCOH		chip select hold time after nOE, 0 ~ 15 for Static #0 ~ #7 and NAND, 1 ~ 16 for IDE.
 *	@param[in]	tACC		data access time, 1 ~ 256 for Static #0 ~ #7, NAND and IDE.
 *	@param[in]	tSACC		data access time in burst access for Static #0 ~ #7, 1 ~ 256.\nwait access time for IDE, 1 ~ 256.
 *	@param[in]	WaitMode	Wait control \n This argument is only valid in case of Static #0 ~ #7 and IDE.
 *	@param[in]	BurstRead	Burst mode control in reading.\n This argument is only valid in case of Static #0 ~ #7.
 *	@param[in]	BurstWrite	Burst mode control in writing.\n This argument is only valid in case of Static #0 ~ #7.
 *	@return		None.
 *	@see		NX_MCUS_GetStaticBUSConfig
 */
void	NX_MCUS_SetStaticBUSConfig
(
	NX_MCUS_SBUSID Id, U32 BitWidth,
	U32 tACS, U32 tCAH, U32 tCOS, U32 tCOH, U32 tACC, U32 tSACC,
	NX_MCUS_WAITMODE WaitMode,	NX_MCUS_BURSTMODE BurstRead, NX_MCUS_BURSTMODE BurstWrite
)
{
	register U32 bitpos2, bitpos4, bitpos8, regindex4, regindex8;
	register struct NX_MCUS_RegisterSet*	pRegister;
	register U32 regval;

	NX_ASSERT( (NX_MCUS_SBUSID_STATIC13>=Id) || (NX_MCUS_SBUSID_IDE==Id) || (NX_MCUS_SBUSID_NAND==Id) );
	NX_ASSERT( 1 <=	tACC &&	tACC <= 256 );
	NX_ASSERT( (1 <= tSACC && tSACC <= 256) || (NX_MCUS_SBUSID_NAND==Id) );
	NX_ASSERT( (8 == BitWidth || 16 == BitWidth) || (NX_MCUS_SBUSID_STATIC13 < Id) );
	NX_ASSERT( (4 > BurstRead) || (NX_MCUS_SBUSID_STATIC13 < Id) );
	NX_ASSERT( (4 > BurstWrite) || (NX_MCUS_SBUSID_STATIC13 < Id) );
	NX_ASSERT( 4 > WaitMode );

	NX_ASSERT( CNULL != __g_pRegister );

	//--------------------------------------------------------------------------
	pRegister	=	__g_pRegister;

	bitpos2		= ((U32)Id) * 2;

	bitpos4		= ((U32)Id) * 4;
	regindex4	= bitpos4 / 32;
	bitpos4		= bitpos4 % 32;

	bitpos8		= ((U32)Id) * 8;
	regindex8	= bitpos8 / 32;
	bitpos8		= bitpos8 % 32;

	if( NX_MCUS_SBUSID_IDE == Id )
	{
		NX_ASSERT( 0 < tACS && 16 >= tACS );
		NX_ASSERT( 0 < tCAH && 16 >= tCAH );
		NX_ASSERT( 0 < tCOS && 16 >= tCOS );
		NX_ASSERT( 0 < tCOH && 16 >= tCOH );

		//--------------------------------------------------------------------------
		// Set timing for tACS, tCOS, tOCH.
		// 0 : 1 cycle, 1 : 2 cycle, 2 : 3 cycle, ......, 15 : 16 cycle
		tACS	-= 1;
		tCOS	-= 1;
		tCOH	-= 1;

		// Set timing for tCAH
		// 0 : 2 cycle, 1 : 3 cycle, 2 : 4 cycle, ......, 15 : 1 cycle
		tCAH = ((tCAH-1) + 15) % 16;
	}
	else
	{
		NX_ASSERT( 16 > tACS );
		NX_ASSERT( 16 > tCAH );
		NX_ASSERT( 16 > tCOS );
		NX_ASSERT( 16 > tCOH );

		//--------------------------------------------------------------------------
		// Set timing for tACS, tCAH, tCOS, tOCH.
		// 0 : 1 cycle, 1 : 2 cycle, 2 : 3 cycle, ......, 15 : 0 cycle
		tACS = (tACS + 15) % 16;
		tCAH = (tCAH + 15) % 16;
		tCOS = (tCOS + 15) % 16;
		tCOH = (tCOH + 15) % 16;
	}

	tACC	-= 1;	// tACC	= n+1 cycle
	tSACC -= 1;	// tSACC = n+1 cycle

	//--------------------------------------------------------------------------
//	pRegister->MEMTIMEACS[regindex4] = (pRegister->MEMTIMEACS[regindex4] & ~(0x0FUL<<bitpos4)) | (tACS<<bitpos4);
//	pRegister->MEMTIMECAH[regindex4] = (pRegister->MEMTIMECAH[regindex4] & ~(0x0FUL<<bitpos4)) | (tCAH<<bitpos4);
//	pRegister->MEMTIMECOS[regindex4] = (pRegister->MEMTIMECOS[regindex4] & ~(0x0FUL<<bitpos4)) | (tCOS<<bitpos4);
//	pRegister->MEMTIMECOH[regindex4] = (pRegister->MEMTIMECOH[regindex4] & ~(0x0FUL<<bitpos4)) | (tCOH<<bitpos4);
//	pRegister->MEMTIMEACC[regindex8] = (pRegister->MEMTIMEACC[regindex8] & ~(0xFFUL<<bitpos8)) | (tACC<<bitpos8);
	regval = ReadIODW(&pRegister->MEMTIMEACS[regindex4]);
	regval &= ~(0x0FUL<<bitpos4);
	regval |= (tACS<<bitpos4);
	WriteIODW(&pRegister->MEMTIMEACS[regindex4], regval);

	regval = ReadIODW(&pRegister->MEMTIMECAH[regindex4]);
	regval &= ~(0x0FUL<<bitpos4);
	regval |= (tCAH<<bitpos4);
	WriteIODW(&pRegister->MEMTIMECAH[regindex4], regval);

	regval = ReadIODW(&pRegister->MEMTIMECOS[regindex4]);
	regval &= ~(0x0FUL<<bitpos4);
	regval |= (tCOS<<bitpos4);
	WriteIODW(&pRegister->MEMTIMECOS[regindex4], regval);

	regval = ReadIODW(&pRegister->MEMTIMECOH[regindex4]);
	regval &= ~(0x0FUL<<bitpos4);
	regval |= (tCOH<<bitpos4);
	WriteIODW(&pRegister->MEMTIMECOH[regindex4], regval);

	regval = ReadIODW(&pRegister->MEMTIMEACC[regindex8]);
	regval &= ~(0xFFUL<<bitpos8);
	regval |= (tACC<<bitpos8);
	WriteIODW(&pRegister->MEMTIMEACC[regindex8], regval);

	if( NX_MCUS_SBUSID_STATIC13 >= Id	)	// For static bus #0 ~ #13
	{
//		pRegister->MEMTIMESACC[regindex8] = (pRegister->MEMTIMESACC[regindex8] & ~(0xFFUL<<bitpos8)) | (tSACC<<bitpos8);
		regval = ReadIODW(&pRegister->MEMTIMESACC[regindex8]);
		regval &= ~(0xFFUL<<bitpos8);
		regval |= (tSACC<<bitpos8);
		WriteIODW(&pRegister->MEMTIMESACC[regindex8], regval);

		// Bus Width
		if( BitWidth == 16 )	ReadIODW(&pRegister->MEMBW) |=	(1UL<<((U32)Id));	// 1: 16-bit
		else					ReadIODW(&pRegister->MEMBW) &= ~(1UL<<((U32)Id));	// 0:	8-bit

		// Burst Mode
		regval = ReadIODW(&pRegister->MEMBURST[0]);
		regval &= ~(0xFUL<<bitpos4);
		regval |= ((U32)BurstRead )<<(bitpos4+0);
		regval |= ((U32)BurstWrite)<<(bitpos4+2);
//		pRegister->MEMBURST = regval;
		WriteIODW(&pRegister->MEMBURST[0], regval);

		// Wait control
//		pRegister->MEMWAIT = (pRegister->MEMWAIT & ~(0x3UL<<bitpos2)) | (((U32)WaitMode)<<bitpos2);
		regval = ReadIODW(&pRegister->MEMWAIT);
		regval &= ~(0x3UL<<bitpos2);
		regval |=(((U32)WaitMode)<<bitpos2);
		WriteIODW(&pRegister->MEMWAIT, regval);
	}
	else if( NX_MCUS_SBUSID_IDE == Id )	// IDE
	{
//		pRegister->MEMTIMEWACC[regindex8] = (pRegister->MEMTIMEWACC[regindex8] & ~(0xFFUL<<bitpos8)) | (tSACC<<bitpos8);
		regval = ReadIODW(&pRegister->MEMTIMEWACC[regindex8]);
		regval &= ~(0xFFUL<<bitpos8);
		regval |= (tSACC<<bitpos8);
		WriteIODW(&pRegister->MEMTIMEWACC[regindex8], regval);

		// Wait control
//		pRegister->MEMWAIT = (pRegister->MEMWAIT & ~(0x3UL<<bitpos2)) | (((U32)WaitMode)<<bitpos2);
		regval = ReadIODW(&pRegister->MEMWAIT);
		regval &= ~(0x3UL<<bitpos2);
		regval |= (((U32)WaitMode)<<bitpos2);
		WriteIODW(&pRegister->MEMWAIT, regval);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get static BUS configration.
 *	@param[in]	Id			Static BUS ID which is one of @ref NX_MCUS_SBUSID enum.
 *	@param[out] pBitWidth	BUS bit width.\n This argument is only valid for Static #0 ~ #7.
 *	@param[out] ptACS		address setup time before nCS
 *	@param[out] ptCAH		address hold time after nCS
 *	@param[out] ptCOS		chip select setup time before nOE
 *	@param[out] ptCOH		chip select hold time after nOE
 *	@param[out] ptACC		data access time
 *	@param[out] ptSACC		data access time in burst access for Static #0 ~ #7.\nwait access time for IDE.
 *	@param[out] pWaitMode	wait control. \n This argument is only valid for Static #0 ~ #7 and IDE.
 *	@param[out] pBurstRead	Burst mode control in reading.\n This argument is only valid for Static #0 ~ #7.
 *	@param[out] pBurstWrite Burst mode control in writing.\n This argument is only valid for Static #0 ~ #7.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUS_SetStaticBUSConfig
 */
void	NX_MCUS_GetStaticBUSConfig
(
	NX_MCUS_SBUSID Id, U32* pBitWidth,
	U32* ptACS, U32* ptCAH, U32* ptCOS, U32* ptCOH, U32* ptACC, U32* ptSACC,
	NX_MCUS_WAITMODE* pWaitMode, NX_MCUS_BURSTMODE* pBurstRead,NX_MCUS_BURSTMODE* pBurstWrite
)
{
	register U32 bitpos2, bitpos4, bitpos8, regindex4, regindex8;
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( (NX_MCUS_SBUSID_STATIC13>=Id) || (NX_MCUS_SBUSID_IDE==Id) || (NX_MCUS_SBUSID_NAND==Id) );
	NX_ASSERT( CNULL != __g_pRegister );

	//--------------------------------------------------------------------------
	pRegister	=	__g_pRegister;

	bitpos2		= ((U32)Id) * 2;

	bitpos4		= ((U32)Id) * 4;
	regindex4	= bitpos4 / 32;
	bitpos4		= bitpos4 % 32;

	bitpos8		= ((U32)Id) * 8;
	regindex8	= bitpos8 / 32;
	bitpos8		= bitpos8 % 32;

	if( NX_MCUS_SBUSID_IDE == Id )
	{
		//--------------------------------------------------------------------------
		// Get timing for tACS, tCOS, tOCH.
		// 0 : 1 cycle, 1 : 2 cycle, 2 : 3 cycle, ......, 15 : 16 cycle
		if( CNULL != ptACS )	*ptACS = ((ReadIODW(&pRegister->MEMTIMEACS[regindex4]) >> bitpos4) & 0x0FUL) + 1;
		if( CNULL != ptCOS )	*ptCOS = ((ReadIODW(&pRegister->MEMTIMECOS[regindex4]) >> bitpos4) & 0x0FUL) + 1;
		if( CNULL != ptCOH )	*ptCOH = ((ReadIODW(&pRegister->MEMTIMECOH[regindex4]) >> bitpos4) & 0x0FUL) + 1;

		//--------------------------------------------------------------------------
		// Get timing for tCAH.
		// 0 : 2 cycle, 1 : 3 cycle, 2 : 4 cycle, ......, 15 : 1 cycle
		if( CNULL != ptCAH )
		{
			*ptCAH = (ReadIODW(&pRegister->MEMTIMECAH[regindex4]) >> bitpos4) & 0x0FUL;
			*ptCAH = ((*ptCAH + 1) % 16) + 1;
		}

		//--------------------------------------------------------------------------
		// Get timing for tACC, tSACC
		// tACC	= n+1 cycle
		if( CNULL != ptACC )	*ptACC = ((ReadIODW(&pRegister->MEMTIMEACC[regindex8]) >> bitpos8) & 0xFFUL) + 1;

		// tSACC = n+1 cycle
		if( CNULL != ptSACC )	*ptSACC = ((ReadIODW(&pRegister->MEMTIMEWACC[regindex8]) >> bitpos8) & 0xFFUL) + 1;

		// Wait control
		if( CNULL != pWaitMode )	{ *pWaitMode	= (NX_MCUS_WAITMODE)((ReadIODW(&pRegister->MEMWAIT) >> bitpos2) & 0x3UL);		}
	}
	else
	{
		//--------------------------------------------------------------------------
		// Get timing for tACS, tCAH, tCOS, tOCH.
		// 0 : 1 cycle, 1 : 2 cycle, 2 : 3 cycle, ......, 15 : 0 cycle
		if( CNULL != ptACS )
		{
			*ptACS = (ReadIODW(&pRegister->MEMTIMEACS[regindex4]) >> bitpos4) & 0x0FUL;
			*ptACS = (*ptACS + 1) % 16;
		}

		if( CNULL != ptCAH )
		{
			*ptCAH = (ReadIODW(&pRegister->MEMTIMECAH[regindex4]) >> bitpos4) & 0x0FUL;
			*ptCAH = (*ptCAH + 1) % 16;
		}

		if( CNULL != ptCOS )
		{
			*ptCOS = (ReadIODW(&pRegister->MEMTIMECOS[regindex4]) >> bitpos4) & 0x0FUL;
			*ptCOS = (*ptCOS + 1) % 16;
		}

		if( CNULL != ptCOH )
		{
			*ptCOH = (ReadIODW(&pRegister->MEMTIMECOH[regindex4]) >> bitpos4) & 0x0FUL;
			*ptCOH = (*ptCOH + 1) % 16;
		}

		//--------------------------------------------------------------------------
		// Get timing for tACC
		// tACC	= n+1 cycle
		if( CNULL != ptACC )
		{
			*ptACC = (ReadIODW(&pRegister->MEMTIMEACC[regindex8]) >> bitpos8) & 0xFFUL;
			*ptACC += 1;
		}

		if( NX_MCUS_SBUSID_STATIC13 >= Id	)	// For static bus #0 ~ #13
		{
			// tSACC = n+1 cycle
			if( CNULL != ptSACC )
			{
				*ptSACC = (ReadIODW(&pRegister->MEMTIMESACC[regindex8]) >> bitpos8) & 0xFFUL;
				*ptSACC += 1;
			}

			if( CNULL != pBitWidth )	{ *pBitWidth		= (ReadIODW(&pRegister->MEMBW) & (1UL<<((U32)Id))) ? 16 : 8;						}
			if( CNULL != pBurstRead )	{ *pBurstRead		= (NX_MCUS_BURSTMODE)((ReadIODW(&pRegister->MEMBURST[0]) >> (bitpos4+0)) & 0x3UL);	}
			if( CNULL != pBurstWrite )	{ *pBurstWrite	= (NX_MCUS_BURSTMODE)((ReadIODW(&pRegister->MEMBURST[0]) >> (bitpos4+2)) & 0x3UL);	}

			// Wait control
			if( CNULL != pWaitMode )	{ *pWaitMode	= (NX_MCUS_WAITMODE)((ReadIODW(&pRegister->MEMWAIT) >> bitpos2) & 0x3UL);		}
		}
	}
}


//------------------------------------------------------------------------------
//	IDE DMA operations
//------------------------------------------------------------------------------
/**
 *	@brief		Reset IDE DMA.
 *	@return		None.
 *	@see		NX_MCUS_ResetIDEDMA,
 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
void	NX_MCUS_ResetIDEDMA ( void )
{
	const U32	DMARST	= (1UL<<0);
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	// @todo
	// 2007/05/03 Need to confirm that I have to clear DMARST after setting this.
//	pRegister->IDEDMARST = DMARST;
//	pRegister->IDEDMARST = 0;
	WriteIODW(&pRegister->IDEDMARST, DMARST);
	WriteIODW(&pRegister->IDEDMARST, 0);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set IDE DMA Timeout configuation.
 *	@param[in]	Timeout		Automatically performs host termination
 *								when there is no response from internal DMA
 *								during assigned this value, 0 ~ 31.\n
 *								The unit is 8 cycles in BCLK.
 *	@param[in]	TransferSize	The maximum number of data transfer for
 *								occupying static bus, 0 ~ 1023.\n
 *								The unit is 1 byte for DMA width is 8-bit or
 *								2 bytes for DMA width is 16-bit.
 *	@remark
 *	@return		None.
 *	@see		NX_MCUS_ResetIDEDMA,
 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
void	NX_MCUS_SetIDEDMATimeOut( U32 Timeout,	U32 TransferSize )
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 32 > Timeout );
	NX_ASSERT( 1024 > TransferSize );

//	__g_pRegister->IDEDMATIMEOUT = (Timeout<<12) | TransferSize;
	WriteIODW(&__g_pRegister->IDEDMATIMEOUT, (Timeout<<12) | TransferSize);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get IDE DMA Timeout configuation.
 *	@param[out]	pTimeout		Automatically performs host termination
 *								when there is no response from internal DMA
 *								during assigned this value.\n
 *								The unit is 8 cycles in BCLK.
 *	@param[out]	pTransferSize	The maximum number of data transfer for
 *								occupying static bus.\n
 *								The unit is 1 byte for DMA width is 8-bit or
 *								2 bytes for DMA width is 16-bit.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUS_ResetIDEDMA,
 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
void	NX_MCUS_GetIDEDMATimeOut( U32* pTimeout, U32* pTransferSize )
{
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->IDEDMATIMEOUT);
	if( pTimeout != CNULL )			*pTimeout		= (regval >> 12);
	if( pTransferSize != CNULL )	*pTransferSize	= (regval & 0x3FF);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable IDE DMA.
 *	@param[in]	bEnb		Set this as CTRUE to enable IDE DMA.
 *	@return		None.
 *	@see		NX_MCUS_ResetIDEDMA,
 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
void	NX_MCUS_SetIDEDMAEnable( CBOOL bEnb )
{
	const U32 DMAENB_POS	= (12);
	const U32 DMAENB_MASK	= (1UL << DMAENB_POS);

	register struct NX_MCUS_RegisterSet*	pRegister;
	register U32 regvalue;

	NX_ASSERT( (0==bEnb) || (1==bEnb) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = ReadIODW(&pRegister->IDEDMACTRL);
	regvalue &= ~DMAENB_MASK;
	regvalue |= (U32)bEnb << DMAENB_POS;
//	pRegister->IDEDMACTRL = regvalue;
	WriteIODW(&pRegister->IDEDMACTRL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether IDE DMA is enabled or not.
 *	@return		\b CTRUE	indicates IDE DMA is enabled.\n
 *				\b CFALSE	indicates IDE DMA is disabled.
 *	@see		NX_MCUS_ResetIDEDMA,
 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
CBOOL	NX_MCUS_GetIDEDMAEnable ( void )
{
	const U32 DMAENB_POS	= (12);
	const U32 DMAENB_MASK	= (1UL << DMAENB_POS);

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((ReadIODW(&__g_pRegister->IDEDMACTRL) & DMAENB_MASK ) >> DMAENB_POS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set IDE DMA transfer mode.
 *	@param[in]	DMAMode		DMA transfer mode
 *	@return		None.
 *	@see		NX_MCUS_ResetIDEDMA,
 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
void	NX_MCUS_SetIDEDMAMode( NX_MCUS_DMAMODE DMAMode )
{
	const U32 DMAMODE_POS	= (0);
	const U32 DMAMODE_MASK	= (3UL << DMAMODE_POS);

	register struct NX_MCUS_RegisterSet*	pRegister;
	register U32 regvalue;

	NX_ASSERT( 4 > DMAMode );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = ReadIODW(&pRegister->IDEDMACTRL);
	regvalue &= ~DMAMODE_MASK;
	regvalue |= (U32)DMAMode << DMAMODE_POS;
//	pRegister->IDEDMACTRL = regvalue;
	WriteIODW(&pRegister->IDEDMACTRL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get IDE DMA transfer mode.
 *	@return		DMA transfer mode.
 *	@see		NX_MCUS_ResetIDEDMA,
 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
NX_MCUS_DMAMODE	NX_MCUS_GetIDEDMAMode( void )
{
	const U32 DMAMODE_POS	= (0);
	const U32 DMAMODE_MASK	= (3UL << DMAMODE_POS);

	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_MCUS_DMAMODE)((ReadIODW(&__g_pRegister->IDEDMACTRL) & DMAMODE_MASK) >> DMAMODE_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set IDE DMA Configuration.
 *	@param[in]	BitWidth	DMA transfer width, 8 or 16.
 *	@param[in]	ReqPol		DMAREQ polarity
 *	@param[in]	AckPol		DMAACK polarity
 *	@param[in]	tDH			data hold time for DMA write, 1 ~ 16.
 *	@param[in]	tD			nIOR/nIOW asserted pulse width, 1 ~ 256.
 *	@param[in]	tK			nIOR/nIOW negated pulse width, 1 ~ 256.
 *	@param[in]	tACK		setup time before nDMAACK, 1 ~ 256.
 *	@param[in]	tENV		envelope time, 1 ~ 256.
 *	@param[in]	tRP			ready to pause time, 1 ~ 256.
 *	@param[in]	tSS			time from DSTROBE edge to negation of DMAREQ, 1 ~ 256.
 *	@param[in]	tMLI		interlock time with minimum, 1 ~ 256.
 *	@param[in]	tCYC		pulse width of HSTROBE, 1 ~ 256.
 *	@return		None.
 *	@remark		Parameters for multi-word DMA are tDH, tD and tK.\n
 *				Parameters for ultra DMA are tDH, tACK, tENV, tRP, tSS, tMLI and tCYC.
 *	@see		NX_MCUS_ResetIDEDMA,
 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
void	NX_MCUS_SetIDEDMAConfig
(
	U32 BitWidth, NX_MCUS_POLARITY ReqPol, NX_MCUS_POLARITY AckPol,
	U32 tDH, U32 tD, U32 tK,
	U32 tACK, U32 tENV, U32 tRP, U32 tSS, U32 tMLI, U32 tCYC
)
{
	const U32 DMAWIDTH_POS	= (8);
	const U32 DMAWIDTH_MASK = (1UL << DMAWIDTH_POS);

	register struct NX_MCUS_RegisterSet*	pRegister;
	register U32 regvalue;

	NX_ASSERT( 8 == BitWidth || 16 == BitWidth );
	NX_ASSERT( 2 > ReqPol );
	NX_ASSERT( 2 > AckPol );
	NX_ASSERT( (1<=tDH ) && ( 16>=tDH ) );
	NX_ASSERT( (1<=tD	) && (256>=tD	) );
	NX_ASSERT( (1<=tK	) && (256>=tK	) );
	NX_ASSERT( (1<=tACK) && (256>=tACK) );
	NX_ASSERT( (1<=tENV) && (256>=tENV) );
	NX_ASSERT( (1<=tRP ) && (256>=tRP ) );
	NX_ASSERT( (1<=tSS ) && (256>=tSS ) );
	NX_ASSERT( (1<=tMLI) && (256>=tMLI) );
	NX_ASSERT( (1<=tCYC) && (256>=tCYC) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	// IDEDMACTRL[8] : 0(8-bit), 1(16-bit)
	regvalue = ReadIODW(&pRegister->IDEDMACTRL);
	if( 16 == BitWidth )	regvalue |=	DMAWIDTH_MASK;
	else					regvalue &= ~DMAWIDTH_MASK;
//	pRegister->IDEDMACTRL = regvalue;
	WriteIODW(&pRegister->IDEDMACTRL, regvalue);

	// IDEDMAPOL[12] : REQPOL, IDEDMAPOL[8] : ACKPOL, IDEDMAPOL[7:0] : Reserved but you have to write '1' only.
//	pRegister->IDEDMAPOL = (((U32)ReqPol)<<12) | (((U32)AckPol)<<8) | (1U<<13) | (1);
	WriteIODW(&pRegister->IDEDMAPOL, (((U32)ReqPol)<<12) | (((U32)AckPol)<<8) | (1U<<13) | (1));

	// tK, tD, tENV, tACK, tSS, tRP, tDH, tMLI, tCYC : tXXX = register value + 1
//	pRegister->IDEDMATIME0 = ((tK	- 1)<<16) | (tD	- 1) | (4U<<24) | (4U<<8);
//	pRegister->IDEDMATIME1 = ((tENV - 1)<<16) | (tACK - 1) | (4U<<24) | (4U<<8);
//	pRegister->IDEDMATIME2 = ((tSS	- 1)<<16) | (tRP	- 1) | (4U<<24) | (4U<<8);
//	pRegister->IDEDMATIME3 = ((tDH	- 1)<<16) | (tMLI - 1) | (1U<<20) | (4U<<8);
//	pRegister->IDEDMATIME4 =	(tCYC - 1)					| (0x22U<<8);
	WriteIODW(&pRegister->IDEDMATIME0, ((tK	- 1)<<16) | (tD	- 1) | (4U<<24) | (4U<<8));
	WriteIODW(&pRegister->IDEDMATIME1, ((tENV - 1)<<16) | (tACK - 1) | (4U<<24) | (4U<<8));
	WriteIODW(&pRegister->IDEDMATIME2, ((tSS	- 1)<<16) | (tRP	- 1) | (4U<<24) | (4U<<8));
	WriteIODW(&pRegister->IDEDMATIME3, ((tDH	- 1)<<16) | (tMLI - 1) | (1U<<20) | (4U<<8));
	WriteIODW(&pRegister->IDEDMATIME4,	(tCYC - 1)					| (0x22U<<8));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get IDE DMA Configuration.
 *	@param[out] pBitWidth	Specifies a pointer to get DMA transfer width.
 *	@param[out] pReqPol		Specifies a pointer to get DMAREQ polarity.
 *	@param[out] pAckPol		Specifies a pointer to get DMAACK polarity.
 *	@param[out] ptDH		Specifies a pointer to get data hold time for DMA write.
 *	@param[out] ptD			Specifies a pointer to get nIOR/nIOW asserted pulse width.
 *	@param[out] ptK			Specifies a pointer to get nIOR/nIOW negated pulse width.
 *	@param[out] ptACK		Specifies a pointer to get setup time before nDMAACK.
 *	@param[out] ptENV		Specifies a pointer to get envelope time.
 *	@param[out] ptRP		Specifies a pointer to get ready to pause time.
 *	@param[out] ptSS		Specifies a pointer to get time from DSTROBE edge to negation of DMAREQ.
 *	@param[out] ptMLI		Specifies a pointer to get interlock time with minimum.
 *	@param[out] ptCYC		Specifies a pointer to get pulse width of HSTROBE.
 *	@return		None.
 *	@remark		Arguments which does not required can be CNULL.
 *	@see		NX_MCUS_ResetIDEDMA,

 *				NX_MCUS_SetIDEDMATimeOut,	NX_MCUS_GetIDEDMATimeOut,
 *				NX_MCUS_SetIDEDMAEnable,	NX_MCUS_GetIDEDMAEnable,
 *				NX_MCUS_SetIDEDMAMode,		NX_MCUS_GetIDEDMAMode,
 *				NX_MCUS_SetIDEDMAConfig,	NX_MCUS_GetIDEDMAConfig
 */
void	NX_MCUS_GetIDEDMAConfig
(
	U32* pBitWidth,	NX_MCUS_POLARITY* pReqPol, NX_MCUS_POLARITY* pAckPol,
	U32* ptDH, U32* ptD, U32* ptK,
	U32* ptACK, U32* ptENV, U32* ptRP, U32* ptSS, U32* ptMLI, U32* ptCYC
)
{
	const U32 DMAWIDTH_POS	= (8);
	const U32 DMAWIDTH_MASK = (1UL << DMAWIDTH_POS);

	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	// IDEDMACTRL[8] : 0(8-bit), 1(16-bit)
	if( CNULL != pBitWidth )	*pBitWidth = (ReadIODW(&pRegister->IDEDMACTRL) & DMAWIDTH_MASK) ? 16 : 0;

	// IDEDMAPOL[12] : REQPOL, IDEDMAPOL[8] : ACKPOL
	if( CNULL != pReqPol )		*pReqPol	= (ReadIODW(&pRegister->IDEDMAPOL) & (1UL<<12)) ? NX_MCUS_POLARITY_ACTIVEHIGH : NX_MCUS_POLARITY_ACTIVELOW;
	if( CNULL != pAckPol )		*pAckPol	= (ReadIODW(&pRegister->IDEDMAPOL) & (1UL<< 8)) ? NX_MCUS_POLARITY_ACTIVEHIGH : NX_MCUS_POLARITY_ACTIVELOW;

	// tK, tD, tENV, tACK, tSS, tRP, tDH, tMLI, tCYC : tXXX = register value + 1
	if( CNULL != ptD	)		*ptD		= ((ReadIODW(&pRegister->IDEDMATIME0) >>  0) & 0xFF) + 1;
	if( CNULL != ptK	)		*ptK		= ((ReadIODW(&pRegister->IDEDMATIME0) >> 16) & 0xFF) + 1;
	if( CNULL != ptACK	)		*ptACK		= ((ReadIODW(&pRegister->IDEDMATIME1) >>  0) & 0xFF) + 1;
	if( CNULL != ptENV	)		*ptENV		= ((ReadIODW(&pRegister->IDEDMATIME1) >> 16) & 0xFF) + 1;
	if( CNULL != ptRP	)		*ptRP		= ((ReadIODW(&pRegister->IDEDMATIME2) >>  0) & 0xFF) + 1;
	if( CNULL != ptSS	)		*ptSS		= ((ReadIODW(&pRegister->IDEDMATIME2) >> 16) & 0xFF) + 1;
	if( CNULL != ptMLI	)		*ptMLI		= ((ReadIODW(&pRegister->IDEDMATIME3) >>  0) & 0xFF) + 1;
	if( CNULL != ptDH	)		*ptDH		= ((ReadIODW(&pRegister->IDEDMATIME3) >> 16) & 0x0F) + 1;
	if( CNULL != ptCYC	)		*ptCYC		= ((ReadIODW(&pRegister->IDEDMATIME4) >>  0) & 0xFF) + 1;
}

void	NX_MCUS_SetNANDBUSConfig
(
	U32 NFBank, 
	U32 tACS, U32 tCAH, U32 tCOS, U32 tOCH, U32 tACC
)
{
	register struct NX_MCUS_RegisterSet*	pRegister;
	register U32 regval;

	NX_ASSERT( (3>=NFBank) );
	NX_ASSERT( 1 <=	tACC &&	tACC <= 256 );
	NX_ASSERT( 16 > tACS );
	NX_ASSERT( 16 > tCAH );
	NX_ASSERT( 16 > tCOS );
	NX_ASSERT( 16 > tOCH );

	NX_ASSERT( CNULL != __g_pRegister );

	//--------------------------------------------------------------------------
	pRegister	=	__g_pRegister;



	//--------------------------------------------------------------------------
	// Set timing for tACS, tCAH, tCOS, tOCH.
	// 0 : 1 cycle, 1 : 2 cycle, 2 : 3 cycle, ......, 15 : 0 cycle
	tACS = (tACS + 15) & 0xF;
	tCAH = (tCAH + 15) & 0xF;
	tCOS = (tCOS + 15) & 0xF;
	tOCH = (tOCH + 15) & 0xF;

	tACC	-= 1;	// tACC	= n+1 cycle

	//--------------------------------------------------------------------------
	regval = ReadIODW(&pRegister->NFTACS);
	regval &= ~(0x0FUL<<(NFBank*4));
	regval |= (tACS<<(NFBank*4));
	WriteIODW(&pRegister->NFTACS, regval);

	regval = ReadIODW(&pRegister->NFTCAH);
	regval &= ~(0x0FUL<<(NFBank*4));
	regval |= (tCAH<<(NFBank*4));
	WriteIODW(&pRegister->NFTCAH, regval);

	regval = ReadIODW(&pRegister->NFTCOS);
	regval &= ~(0x0FUL<<(NFBank*4));
	regval |= (tCOS<<(NFBank*4));
	WriteIODW(&pRegister->NFTCOS, regval);

	regval = ReadIODW(&pRegister->NFTOCH);
	regval &= ~(0x0FUL<<(NFBank*4));
	regval |= (tOCH<<(NFBank*4));
	WriteIODW(&pRegister->NFTOCH, regval);

	regval = ReadIODW(&pRegister->NFTACCL);
	regval &= ~(0xFFUL<<(NFBank*8));
	regval |= (tACC<<(NFBank*8));
	WriteIODW(&pRegister->NFTACCL, regval);
}
void	NX_MCUS_GetNANDBUSConfig
(
	U32 NFBank,
	U32* ptACS, U32* ptCAH, U32* ptCOS, U32* ptOCH, U32* ptACC
)
{
	register struct NX_MCUS_RegisterSet*	pRegister;

	NX_ASSERT( (3>=NFBank) );
	NX_ASSERT( CNULL != __g_pRegister );

	//--------------------------------------------------------------------------
	pRegister	=	__g_pRegister;

	//--------------------------------------------------------------------------
	// Get timing for tACS, tCAH, tCOS, tOCH.
	// 0 : 1 cycle, 1 : 2 cycle, 2 : 3 cycle, ......, 15 : 0 cycle
	if( CNULL != ptACS )
	{
		*ptACS = (ReadIODW(&pRegister->NFTACS ) >> (NFBank*4)) & 0x0FUL;
		*ptACS = (*ptACS + 1) & 0xF;
	}

	if( CNULL != ptCAH )
	{
		*ptCAH = (ReadIODW(&pRegister->NFTCAH ) >> (NFBank*4)) & 0x0FUL;
		*ptCAH = (*ptCAH + 1) & 0xF;
	}

	if( CNULL != ptCOS )
	{
		*ptCOS = (ReadIODW(&pRegister->NFTCOS ) >> (NFBank*4)) & 0x0FUL;
		*ptCOS = (*ptCOS + 1) & 0xF;
	}

	if( CNULL != ptOCH )
	{
		*ptOCH = (ReadIODW(&pRegister->NFTOCH ) >> (NFBank*4)) & 0x0FUL;
		*ptOCH = (*ptOCH + 1) & 0xF;
	}

	//--------------------------------------------------------------------------
	// Get timing for tACC
	// tACC	= n+1 cycle
	if( CNULL != ptACC )
	{
		*ptACC = (ReadIODW(&pRegister->NFTACCL ) >> (NFBank*8)) & 0xFFUL;
		*ptACC += 1;
	}
}

//------------------------------------------------------------------------------
//	NAND Flash Memory Controller Functions
//------------------------------------------------------------------------------
/**
 *	@brief		Set NAND Flash Address Type.
 *	@param[in]	type	NAND Flash address type which is one of @ref NX_MCUS_NFTYPE enum.
 *	@return		None.
 *	@remark		It can also be set NFTYPE by external pin configuration(CfgNFType)
 *				for NAND Booting.
 *	@see		NX_MCUS_NFTYPE,
 *				NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
void	NX_MCUS_SetNFType ( NX_MCUS_NFTYPE type )
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 3;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	const U32 IRQPEND	= (1UL<<15);

	register U32 regval;

	NX_ASSERT( 4 > type );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFCONTROL);
	regval &= ~(IRQPEND | BIT_MASK);		// Unmask bits include IRQPEND.
	regval |= (((U32)type) << BIT_POS);
	WriteIODW(&__g_pRegister->NFCONTROL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current setting for NAND Flash Address Type.
 *	@return		the current setting for NAND Flash Address Type.
 *	@remark		It can also be set NFTYPE by external pin configuration(CfgNFType)
 *				for NAND Booting.
 *	@see		NX_MCUS_NFTYPE,
 *				NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
NX_MCUS_NFTYPE	NX_MCUS_GetNFType ( void )
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 3;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_MCUS_NFTYPE)((ReadIODW(&__g_pRegister->NFCONTROL) & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set whether the NAND Flash Boot to enable or disable.
 *	@param[in]	bEnb	Set this as CTRUE to enable the NAND Booting.
 *	@return		None.
 *	@remark		It can also be enabled by external pin configuration(CfgNFBoot).
 *	@see		NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
void	NX_MCUS_SetNFBootEnable( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 5;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	const U32 IRQPEND	= (1UL<<15);

	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFCONTROL);
	regval &= ~IRQPEND;					// Unmask IRQPEND.
	if( bEnb )	regval |=	BIT_MASK;
	else		regval &= ~BIT_MASK;
	WriteIODW(&__g_pRegister->NFCONTROL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the NAND Flash Booting is enabled or not.
 *	@return		CTRUE indicates the NAND Flash Booting is enabled.\n
 *				CFALSE indicates the NAND Flash Booting is disabled.
 *	@remark		It can also be enabled by external pin configuration(CfgNFBoot).
 *	@see		NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
CBOOL	NX_MCUS_GetNFBootEnable ( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 5;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (ReadIODW(&__g_pRegister->NFCONTROL) & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the NAND Flash bank number to access.
 *	@param[in]	Bank	Specifies the NAND Flash bank number to access, 0 ~ 3.
 *	@return		None.
 *	@see		NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
void	NX_MCUS_SetNFBank( U32 Bank )
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	const U32 IRQPEND	= (1UL<<15);

	register U32 regval;

	NX_ASSERT( 4 > Bank );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFCONTROL);
	regval &= ~(IRQPEND | BIT_MASK);		// Unmask bits include IRQPEND.
	regval |= (Bank << BIT_POS);
	WriteIODW(&__g_pRegister->NFCONTROL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the current NAND Flash bank number.
 *	@return		The current NAND Flash bank number.
 *	@see		NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
U32		NX_MCUS_GetNFBank ( void )
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((ReadIODW(&__g_pRegister->NFCONTROL) & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the external nCS signal of NAND flash to assert or negate.
 *	@param[in]	bEnb	Set this as CTRUE to assert nCS signal for current bank.\n
 *						Set this as CFALSE to negate nCS signal for current bank.
 *	@return		None.
 *	@remark		You have to set @a bEnb as CTRUE to control the external NAND flash
 *				memory. It makes NAND flash memory controller holds the external
 *				chip select signal of selected bank is asserted. You can also
 *				set @a bEnb as CFALSE to negate the chip select signal of the NAND
 *				flash memory chip to reduce a power cunsumption if it doesn't need
 *				to access anymore.
 *	@see		NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
void	NX_MCUS_SetNFCSEnable( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 31;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	const U32 IRQPEND	= (1UL<<15);

	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFCONTROL);
	regval &= ~IRQPEND;					// Unmask IRQPEND.
	if( bEnb )	regval |=	BIT_MASK;
	else		regval &= ~BIT_MASK;
//	__g_pRegister->NFCONTROL = regval;
	WriteIODW(&__g_pRegister->NFCONTROL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the external nCS signal of NAND flash is asserted or not.
 *	@return		CTRUE indicates the external nCS signal is asserted.\n
 *				CFALSE indicates the external nCS signal is negated.
 *	@see		NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
CBOOL	NX_MCUS_GetNFCSEnable ( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 31;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (ReadIODW(&__g_pRegister->NFCONTROL) & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether NAND Flash is ready or not.
 *	@return		CTRUE indicates NAND Flash is ready.\n
 *				CFALSE indicates NAND Flash is busy.
 *	@remark		There's a external pin, RnB, indicates the status of NAND Flash
 *				operation. When low, it indicates that a program, erase or random
 *				read operation is in progress and returns to high state upon
 *				completion. This function returns the state of RnB pin.
 *	@see		NX_MCUS_SetNFType,				NX_MCUS_GetNFType,
 *				NX_MCUS_SetNFBootEnable,		NX_MCUS_GetNFBootEnable,
 *				NX_MCUS_SetNFBank,				NX_MCUS_GetNFBank,
 *				NX_MCUS_SetNFCSEnable,			NX_MCUS_GetNFCSEnable,
 *				NX_MCUS_IsNFReady
 */
CBOOL	NX_MCUS_IsNFReady ( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 9;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((ReadIODW(&__g_pRegister->NFCONTROL) & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Determines whether the auto reset of the H/W ECC block to enable or disable.
 *	@param[in]	bEnb	Set this as CTRUE to reset the H/W ECC block automaically.\n
 *						Set this as CFALSE to reset the H/W ECC block manually.
 *	@return		None.
 *	@remark		You can all this function with CTRUE argument to reset the H/W
 *				ECC block automatically by CLE or ALE.\n
 *				If you want to reset the H/W ECC block only by call
 *				NX_MCUS_ResetNFECCBlock(), you should call this function with
 *				CFALSE argument.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_SetAutoResetEnable( CBOOL bEnb )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 30;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	const U32 IRQPEND	= (1UL<<15);

	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFCONTROL);
	regval &= ~IRQPEND;					// Unmask IRQPEND.
	if( bEnb )	regval |=	BIT_MASK;
	else		regval &= ~BIT_MASK;
//	__g_pRegister->NFCONTROL = regval;
	WriteIODW(&__g_pRegister->NFCONTROL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the auto reset of the H/W ECC block is enabled or not.
 *	@return		CTRUE indicates the auto reset is enabled.\n
 *				CFALSE indicates the auto reset is diabled.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16
 */
CBOOL	NX_MCUS_GetAutoResetEnable( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 30;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (ReadIODW(&__g_pRegister->NFCONTROL) & BIT_MASK) ? CTRUE : CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Reset the Hardware ECC block.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_ResetNFECCBlock( void )
{
	const U32	ECCRST	= (1UL << 11);
	const U32	IRQPEND = (1UL << 15);

	register U32 regvalue;

	NX_ASSERT( CNULL != __g_pRegister );

	regvalue = ReadIODW(&__g_pRegister->NFCONTROL);
	regvalue &= ~IRQPEND;
	regvalue |= ECCRST;
	WriteIODW(&__g_pRegister->NFCONTROL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the H/W ECC mode.
 *	@param[in]	bits	Specifies the number of ECC mode in bits - 4, 8, 16 or 24.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_SetECCMode( U32 bits )
{
	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 27;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;
	const U32 IRQPEND	= (1UL<<15);

	register U32 regval;

	NX_ASSERT( (4==bits) || (8==bits) || (12==bits) || (16==bits) || (24==bits) );
	NX_ASSERT( CNULL != __g_pRegister );

	if(bits == 24)
		bits = 4;
	else
	{
		bits /= 4;	// NFECCMODE[2:0] = 0(4), 1(8), 2(12), 3(16), 4(24)
		bits--;
	}

	regval = ReadIODW(&__g_pRegister->NFCONTROL);
	regval &= ~(IRQPEND | BIT_MASK);		// Unmask bits include IRQPEND.
	regval |= (bits << BIT_POS);
	WriteIODW(&__g_pRegister->NFCONTROL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the H/W ECC mode.
 *	@return		the number of ECC mode in bits.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
U32		NX_MCUS_GetECCMode( void )
{
	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 27;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	// NFECCMODE[1:0] = 0(4), 1(8), 2(16)
	return (4UL << ((ReadIODW(&__g_pRegister->NFCONTROL) & BIT_MASK) >> BIT_POS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get NAND flash write data size in bytes.
 *	@return		NAND flash write data size in bytes.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
U32		NX_MCUS_GetNFWriteDataCount( void )
{
	const U32 BIT_SIZE	= 10;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((ReadIODW(&__g_pRegister->NFCNT) & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get NAND flash read data size in bytes.
 *	@return		NAND flash read data size in bytes.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
U32		NX_MCUS_GetNFReadDataCount( void )
{
	const U32 BIT_SIZE	= 10;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((ReadIODW(&__g_pRegister->NFCNT) & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether there's an error on read data or not.
 *	@return		CTRUE indicates there's an error on read data.\n
 *				CFALSE indicates there's no error on read data.
 *	@remark		After you read 512 bytes on NAND flash memory, you can check the
 *				result of H/W ECC decoding block whether there's an error on read data.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
CBOOL	NX_MCUS_IsNFError( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 2;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((ReadIODW(&__g_pRegister->NFECCSTATUS) & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether H/W ECC decoding has been finished or not.
 *	@return		CTRUE indicates H/W ECC decoding has been finished.\n
 *				CFALSE indicates H/W ECC decoding is busy.
 *	@remark		H/W ECC decoding has been finished after 51 BCLKs from reading
 *				512 bytes on NAND flash memory.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
CBOOL	NX_MCUS_IsNFReadEnd( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 1;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((ReadIODW(&__g_pRegister->NFECCSTATUS) & BIT_MASK) >> BIT_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether H/W ECC encoding has been finished or not.
 *	@return		CTRUE indicates H/W ECC encoding has been finished.\n
 *				CFALSE indicates H/W ECC encoding is busy.
 *	@remark		H/W ECC encoding has been finished after writing 512 bytes on
 *				NAND flash memory.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
CBOOL	NX_MCUS_IsNFWriteEnd( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((ReadIODW(&__g_pRegister->NFECCSTATUS) & BIT_MASK) >> BIT_POS);
}

CBOOL	NX_MCUS_IsDecodeError( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 30;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((ReadIODW(&__g_pRegister->NFECCCTRL) & BIT_MASK) >> BIT_POS);
}

U32		NX_MCUS_GetNumOfError( void )
{
	const U32 BIT_SIZE	= 5;
	const U32 BIT_POS	= 25;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((ReadIODW(&__g_pRegister->NFECCCTRL) & BIT_MASK) >> BIT_POS);
}

void	NX_MCUS_LoadELP( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 23;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFECCCTRL);
	regval |= BIT_MASK;
	WriteIODW(&__g_pRegister->NFECCCTRL, regval);
}

void NX_MCUS_SetNFDecMode( NX_MCUS_DECMODE dMode )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 24;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFECCCTRL);
	regval &= ~BIT_MASK;
	regval |= (U32)dMode<<BIT_POS;
	WriteIODW(&__g_pRegister->NFECCCTRL, regval);
};

NX_MCUS_DECMODE NX_MCUS_GetNFDecMode( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 24;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (NX_MCUS_DECMODE)((ReadIODW(&__g_pRegister->NFECCCTRL) & BIT_MASK) >> BIT_POS);
}

void	NX_MCUS_RunECCEncDec( void )
{
	const U32 BIT_SIZE	= 1;
	const U32 BIT_POS	= 21;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

	regval = ReadIODW(&__g_pRegister->NFECCCTRL);
	regval |= BIT_MASK;
	WriteIODW(&__g_pRegister->NFECCCTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get ECC parity code generated by H/W ECC encoding block for 4-bit ECC Mode.
 *	@param[out] pECC	a pointer to store ECC parity code which is 52 bits.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECC4( U32* pECC )
{
	const U32 BIT_SIZE	= 20;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register volatile U32 *pReg;

	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( CNULL != __g_pRegister );

	pReg = &__g_pRegister->NFECC[0];

	pECC[0] = ReadIODW(&pReg[0]);
	pECC[1] = ReadIODW(&pReg[1]) & BIT_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get ECC parity code generated by H/W ECC encoding block for 8-bit ECC Mode.
 *	@param[out] pECC	a pointer to store ECC parity code which is 104 bits.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECC8( U32* pECC )
{
	const U32 BIT_SIZE	= 8;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register volatile U32 *pReg;

	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( CNULL != __g_pRegister );

	pReg = &__g_pRegister->NFECC[0];

	pECC[0] = ReadIODW(&pReg[0]);
	pECC[1] = ReadIODW(&pReg[1]);
	pECC[2] = ReadIODW(&pReg[2]);
	pECC[3] = ReadIODW(&pReg[3]) & BIT_MASK;
}

void	NX_MCUS_GetNFECC12( U32* pECC )
{
	const U32 BIT_MASK	= 0xF0FFFFFF;
	register volatile U32 *pReg;

	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( CNULL != __g_pRegister );

	pReg = &__g_pRegister->NFECC[0];

	pECC[0] = ReadIODW(&pReg[0]);
	pECC[1] = ReadIODW(&pReg[1]);
	pECC[2] = ReadIODW(&pReg[2]);
	pECC[3] = ReadIODW(&pReg[3]);
	pECC[4] = ReadIODW(&pReg[4]) & BIT_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get ECC parity code generated by H/W ECC encoding block for 16-bit ECC Mode.
 *	@param[out] pECC	a pointer to store ECC parity code which is 208 bits.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECC16( U32* pECC )
{
	const U32 BIT_SIZE	= 16;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register volatile U32 *pReg;

	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( CNULL != __g_pRegister );

	pReg = &__g_pRegister->NFECC[0];

	pECC[0] = ReadIODW(&pReg[0]);
	pECC[1] = ReadIODW(&pReg[1]);
	pECC[2] = ReadIODW(&pReg[2]);
	pECC[3] = ReadIODW(&pReg[3]);
	pECC[4] = ReadIODW(&pReg[4]);
	pECC[5] = ReadIODW(&pReg[5]);
	pECC[6] = ReadIODW(&pReg[6]) & BIT_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get ECC parity code generated by H/W ECC encoding block for 24-bit ECC Mode.
 *	@param[out] pECC	a pointer to store ECC parity code which is 336 bits.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECC24( U32* pECC )
{
	const U32 BIT_SIZE	= 16;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register volatile U32 *pReg, *pRegH;

	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( CNULL != __g_pRegister );

	pReg = &__g_pRegister->NFECC[0];
	pRegH = &__g_pRegister->NFECCH[0];

	pECC[0] = ReadIODW(&pReg[0]);
	pECC[1] = ReadIODW(&pReg[1]);
	pECC[2] = ReadIODW(&pReg[2]);
	pECC[3] = ReadIODW(&pReg[3]);
	pECC[4] = ReadIODW(&pReg[4]);
	pECC[5] = ReadIODW(&pReg[5]);
	pECC[6] = ReadIODW(&pReg[6]);
	pECC[7] = ReadIODW(&pRegH[0]);
	pECC[8] = ReadIODW(&pRegH[1]);
	pECC[9] = ReadIODW(&pRegH[2]);
	pECC[10]= ReadIODW(&pRegH[3]) & BIT_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set ECC parity code for H/W ECC decoding block when 4-bit ECC mode.
 *	@param[in]	pECC	a pointer to load ECC parity code which is 52 bits.
 *	@return		None.
 *	@remark		ECC parity code which is set by this function is used for generating
 *				syndrome data on H/W ECC decoding block when you read 512 bytes on
 *				NAND flash memory.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_SetNFOriginECC4( U32 *pECC )
{
	register volatile U32 *pReg;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( CNULL == (0xFF0F0000 & pECC[1]) );

	pReg = &__g_pRegister->NFORGECC[0];

	WriteIODW(&pReg[0], pECC[0]);
	WriteIODW(&pReg[1], pECC[1]);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set ECC parity code for H/W ECC decoding block when 8-bit ECC mode.
 *	@param[in]	pECC	a pointer to load ECC parity code which is 104 bits.
 *	@return		None.
 *	@remark		ECC parity code which is set by this function is used for generating
 *				syndrome data on H/W ECC decoding block when you read 512 bytes on
 *				NAND flash memory.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_SetNFOriginECC8( U32 *pECC )
{
	register volatile U32 *pReg;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( (1UL<<8) > pECC[3] );

	pReg = &__g_pRegister->NFORGECC[0];

	WriteIODW(&pReg[0], pECC[0]);
	WriteIODW(&pReg[1], pECC[1]);
	WriteIODW(&pReg[2], pECC[2]);
	WriteIODW(&pReg[3], pECC[3]);
}

void	NX_MCUS_SetNFOriginECC12( U32 *pECC )
{
	register volatile U32 *pReg;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( CNULL == (0x0F000000 & pECC[4]) );

	pReg = &__g_pRegister->NFORGECC[0];

	WriteIODW(&pReg[0], pECC[0]);
	WriteIODW(&pReg[1], pECC[1]);
	WriteIODW(&pReg[2], pECC[2]);
	WriteIODW(&pReg[3], pECC[3]);
	WriteIODW(&pReg[4], pECC[4]);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set ECC parity code for H/W ECC decoding block when 16-bit ECC mode.
 *	@param[in]	pECC	a pointer to load ECC parity code which is 208 bits.
 *	@return		None.
 *	@remark		ECC parity code which is set by this function is used for generating
 *				syndrome data on H/W ECC decoding block when you read 512 bytes on
 *				NAND flash memory.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_SetNFOriginECC16( U32 *pECC )
{
	register volatile U32 *pReg;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( (1UL<<16) > pECC[6] );

	pReg = &__g_pRegister->NFORGECC[0];

	WriteIODW(&pReg[0], pECC[0]);
	WriteIODW(&pReg[1], pECC[1]);
	WriteIODW(&pReg[2], pECC[2]);
	WriteIODW(&pReg[3], pECC[3]);
	WriteIODW(&pReg[4], pECC[4]);
	WriteIODW(&pReg[5], pECC[5]);
	WriteIODW(&pReg[6], pECC[6]);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set ECC parity code for H/W ECC decoding block when 24-bit ECC mode.
 *	@param[in]	pECC	a pointer to load ECC parity code which is 336 bits.
 *	@return		None.
 *	@remark		ECC parity code which is set by this function is used for generating
 *				syndrome data on H/W ECC decoding block when you read 1024 bytes on
 *				NAND flash memory.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_SetNFOriginECC24( U32 *pECC )
{
	register volatile U32 *pReg, *pRegH;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pECC );
	NX_ASSERT( (1UL<<16) > pECC[10] );

	pReg = &__g_pRegister->NFORGECC[0];
	pRegH = &__g_pRegister->NFORGECCH[0];

	WriteIODW(&pReg[0], pECC[0]);
	WriteIODW(&pReg[1], pECC[1]);
	WriteIODW(&pReg[2], pECC[2]);
	WriteIODW(&pReg[3], pECC[3]);
	WriteIODW(&pReg[4], pECC[4]);
	WriteIODW(&pReg[5], pECC[5]);
	WriteIODW(&pReg[6], pECC[6]);
	WriteIODW(&pRegH[0], pECC[7]);
	WriteIODW(&pRegH[1], pECC[8]);
	WriteIODW(&pRegH[2], pECC[9]);
	WriteIODW(&pRegH[3], pECC[10]);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get odd syndromes from H/W ECC decoding for 4-bit ECC mode.
 *	@param[out] pSyndrome	a pointer to store 4 odd syndromes.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECCOddSyndrome4( U32* pSyndrome )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);

	register volatile U32 *pReg;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pSyndrome );

	pReg = &__g_pRegister->NFSYNDROME[0];

	regval = ReadIODW(&pReg[0]);
	pSyndrome[0] = (regval				& BIT_MASK);	// Syndrome	1 - NFSYNDROME[0][13: 0]
	pSyndrome[1] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	3 - NFSYNDROME[0][29:16]

	regval = ReadIODW(&pReg[1]);
	pSyndrome[2] = (regval				& BIT_MASK);	// Syndrome	5 - NFSYNDROME[1][13: 0]
	pSyndrome[3] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	7 - NFSYNDROME[1][29:16]
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get odd syndromes from H/W ECC decoding for 8-bit ECC mode.
 *	@param[out] pSyndrome	a pointer to store 8 odd syndromes.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECCOddSyndrome8( U32* pSyndrome )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);

	register volatile U32 *pReg;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pSyndrome );

	pReg = &__g_pRegister->NFSYNDROME[0];

	regval = ReadIODW(&pReg[0]);
	pSyndrome[0] = (regval				& BIT_MASK);	// Syndrome	1 - NFSYNDROME[0][13: 0]
	pSyndrome[1] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	3 - NFSYNDROME[0][29:16]

	regval = ReadIODW(&pReg[1]);
	pSyndrome[2] = (regval				& BIT_MASK);	// Syndrome	5 - NFSYNDROME[1][13: 0]
	pSyndrome[3] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	7 - NFSYNDROME[1][29:16]

	regval = ReadIODW(&pReg[2]);
	pSyndrome[4] = (regval				& BIT_MASK);	// Syndrome	9 - NFSYNDROME[2][13: 0]
	pSyndrome[5] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 11 - NFSYNDROME[2][29:16]

	regval = ReadIODW(&pReg[3]);
	pSyndrome[6] = (regval				& BIT_MASK);	// Syndrome 13 - NFSYNDROME[3][13: 0]
	pSyndrome[7] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 15 - NFSYNDROME[3][29:16]
}
//------------------------------------------------------------------------------
/**
 *	@brief		Get odd syndromes from H/W ECC decoding for 12-bit ECC mode.
 *	@param[out] pSyndrome	a pointer to store 12 odd syndromes.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECCOddSyndrome12( U32* pSyndrome )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);

	register volatile U32 *pReg;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pSyndrome );

	pReg = &__g_pRegister->NFSYNDROME[0];

	regval = ReadIODW(&pReg[0]);
	pSyndrome[0] = (regval				& BIT_MASK);	// Syndrome	1 - NFSYNDROME[0][13: 0]
	pSyndrome[1] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	3 - NFSYNDROME[0][29:16]

	regval = ReadIODW(&pReg[1]);
	pSyndrome[2] = (regval				& BIT_MASK);	// Syndrome	5 - NFSYNDROME[1][13: 0]
	pSyndrome[3] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	7 - NFSYNDROME[1][29:16]

	regval = ReadIODW(&pReg[2]);
	pSyndrome[4] = (regval				& BIT_MASK);	// Syndrome	9 - NFSYNDROME[2][13: 0]
	pSyndrome[5] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 11 - NFSYNDROME[2][29:16]

	regval = ReadIODW(&pReg[3]);
	pSyndrome[6] = (regval				& BIT_MASK);	// Syndrome 13 - NFSYNDROME[3][13: 0]
	pSyndrome[7] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 15 - NFSYNDROME[3][29:16]

	regval = ReadIODW(&pReg[4]);
	pSyndrome[8] = (regval				& BIT_MASK);	// Syndrome 17 - NFSYNDROME[4][13: 0]
	pSyndrome[9] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 19 - NFSYNDROME[4][29:16]

	regval = ReadIODW(&pReg[5]);
	pSyndrome[10] = (regval				& BIT_MASK);	// Syndrome 21 - NFSYNDROME[5][13: 0]
	pSyndrome[11] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 23 - NFSYNDROME[5][29:16]
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get odd syndromes from H/W ECC decoding for 16-bit ECC mode.
 *	@param[out] pSyndrome	a pointer to store 16 odd syndromes.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECCOddSyndrome16( U32* pSyndrome )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);

	register volatile U32 *pReg;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pSyndrome );

	pReg = &__g_pRegister->NFSYNDROME[0];

	regval = ReadIODW(&pReg[0]);
	pSyndrome[0] = (regval				& BIT_MASK);	// Syndrome	1 - NFSYNDROME[0][13: 0]
	pSyndrome[1] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	3 - NFSYNDROME[0][29:16]

	regval = ReadIODW(&pReg[1]);
	pSyndrome[2] = (regval				& BIT_MASK);	// Syndrome	5 - NFSYNDROME[1][13: 0]
	pSyndrome[3] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	7 - NFSYNDROME[1][29:16]

	regval = ReadIODW(&pReg[2]);
	pSyndrome[4] = (regval				& BIT_MASK);	// Syndrome	9 - NFSYNDROME[2][13: 0]
	pSyndrome[5] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 11 - NFSYNDROME[2][29:16]

	regval = ReadIODW(&pReg[3]);
	pSyndrome[6] = (regval				& BIT_MASK);	// Syndrome 13 - NFSYNDROME[3][13: 0]
	pSyndrome[7] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 15 - NFSYNDROME[3][29:16]

	regval = ReadIODW(&pReg[4]);
	pSyndrome[8] = (regval				& BIT_MASK);	// Syndrome 17 - NFSYNDROME[4][13: 0]
	pSyndrome[9] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 19 - NFSYNDROME[4][29:16]

	regval = ReadIODW(&pReg[5]);
	pSyndrome[10] = (regval				& BIT_MASK);	// Syndrome 21 - NFSYNDROME[5][13: 0]
	pSyndrome[11] = (regval >> BIT_POS) & BIT_MASK;		// Syndrome 23 - NFSYNDROME[5][29:16]

	regval = ReadIODW(&pReg[6]);
	pSyndrome[12] = (regval				& BIT_MASK);	// Syndrome 25 - NFSYNDROME[6][13: 0]
	pSyndrome[13] = (regval >> BIT_POS) & BIT_MASK;		// Syndrome 27 - NFSYNDROME[6][29:16]

	regval = ReadIODW(&pReg[7]);
	pSyndrome[14] = (regval				& BIT_MASK);	// Syndrome 29 - NFSYNDROME[7][13: 0]
	pSyndrome[15] = (regval >> BIT_POS) & BIT_MASK;		// Syndrome 31 - NFSYNDROME[7][29:16]
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get odd syndromes from H/W ECC decoding for 24-bit ECC mode.
 *	@param[out] pSyndrome	a pointer to store 24 odd syndromes.
 *	@return		None.
 *	@see		NX_MCUS_ResetNFECCBlock,		NX_MCUS_SetAutoResetEnable,		NX_MCUS_GetAutoResetEnable,
 *				NX_MCUS_SetECCMode,				NX_MCUS_GetECCMode,
 *				NX_MCUS_GetNFWriteDataCount,	NX_MCUS_GetNFReadDataCount,
 *				NX_MCUS_IsNFError,				NX_MCUS_IsNFReadEnd,			NX_MCUS_IsNFWriteEnd,
 *				NX_MCUS_GetNFECC4,				NX_MCUS_GetNFECC8,				NX_MCUS_GetNFECC16,				NX_MCUS_GetNFECC24,
 *				NX_MCUS_SetNFOriginECC4,		NX_MCUS_SetNFOriginECC8,		NX_MCUS_SetNFOriginECC16,		NX_MCUS_SetNFOriginECC24,
 *				NX_MCUS_GetNFECCOddSyndrome4,	NX_MCUS_GetNFECCOddSyndrome8,	NX_MCUS_GetNFECCOddSyndrome16,	NX_MCUS_GetNFECCOddSyndrome24
 */
void	NX_MCUS_GetNFECCOddSyndrome24( U32* pSyndrome )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);

	register volatile U32 *pReg, *pRegH;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pSyndrome );

	pReg = &__g_pRegister->NFSYNDROME[0];
	pRegH = &__g_pRegister->NFSYNDROMEH[0];

	regval = ReadIODW(&pReg[0]);
	pSyndrome[0] = (regval				& BIT_MASK);	// Syndrome	1 - NFSYNDROME[0][13: 0]
	pSyndrome[1] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	3 - NFSYNDROME[0][29:16]

	regval = ReadIODW(&pReg[1]);
	pSyndrome[2] = (regval				& BIT_MASK);	// Syndrome	5 - NFSYNDROME[1][13: 0]
	pSyndrome[3] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome	7 - NFSYNDROME[1][29:16]

	regval = ReadIODW(&pReg[2]);
	pSyndrome[4] = (regval				& BIT_MASK);	// Syndrome	9 - NFSYNDROME[2][13: 0]
	pSyndrome[5] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 11 - NFSYNDROME[2][29:16]

	regval = ReadIODW(&pReg[3]);
	pSyndrome[6] = (regval				& BIT_MASK);	// Syndrome 13 - NFSYNDROME[3][13: 0]
	pSyndrome[7] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 15 - NFSYNDROME[3][29:16]

	regval = ReadIODW(&pReg[4]);
	pSyndrome[8] = (regval				& BIT_MASK);	// Syndrome 17 - NFSYNDROME[4][13: 0]
	pSyndrome[9] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 19 - NFSYNDROME[4][29:16]

	regval = ReadIODW(&pReg[5]);
	pSyndrome[10] = (regval				& BIT_MASK);	// Syndrome 21 - NFSYNDROME[5][13: 0]
	pSyndrome[11] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 23 - NFSYNDROME[5][29:16]

	regval = ReadIODW(&pReg[6]);
	pSyndrome[12] = (regval				& BIT_MASK);	// Syndrome 25 - NFSYNDROME[6][13: 0]
	pSyndrome[13] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 27 - NFSYNDROME[6][29:16]

	regval = ReadIODW(&pReg[7]);
	pSyndrome[14] = (regval				& BIT_MASK);	// Syndrome 29 - NFSYNDROME[7][13: 0]
	pSyndrome[15] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 31 - NFSYNDROME[7][29:16]

	regval = ReadIODW(&pRegH[0]);
	pSyndrome[16] = (regval				& BIT_MASK);	// Syndrome 33 - NFSYNDROMEH[0][13: 0]
	pSyndrome[17] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 35 - NFSYNDROMEH[4][29:16]

	regval = ReadIODW(&pRegH[1]);
	pSyndrome[18] = (regval				& BIT_MASK);	// Syndrome 37 - NFSYNDROMEH[5][13: 0]
	pSyndrome[19] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 39 - NFSYNDROMEH[5][29:16]

	regval = ReadIODW(&pRegH[2]);
	pSyndrome[20] = (regval				& BIT_MASK);	// Syndrome 41 - NFSYNDROMEH[6][13: 0]
	pSyndrome[21] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 43 - NFSYNDROMEH[6][29:16]

	regval = ReadIODW(&pRegH[3]);
	pSyndrome[22] = (regval				& BIT_MASK);	// Syndrome 45 - NFSYNDROMEH[7][13: 0]
	pSyndrome[23] = (regval >> BIT_POS)	& BIT_MASK;		// Syndrome 47 - NFSYNDROMEH[7][29:16]
}

void NX_MCUS_SetNumOfELP(U32 ELPNum)
{
	const U32 BIT_SIZE	= 5;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1)<<BIT_POS;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 4 == ELPNum || 8 == ELPNum || 12 == ELPNum || 16 == ELPNum || 24 == ELPNum );

	regval = ReadIODW(&__g_pRegister->NFECCCTRL);
	regval &= ~BIT_MASK;
	regval |= (ELPNum << BIT_POS) & BIT_MASK;
	WriteIODW(&__g_pRegister->NFECCCTRL, regval);
}

U32 NX_MCUS_GetNumOfELP( void )
{
	const U32 BIT_SIZE	= 5;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1)<<BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ( ReadIODW(&__g_pRegister->NFECCCTRL) & BIT_MASK ) >> BIT_POS;
}

void	NX_MCUS_SetParityCount( U32 ParityByte )
{
	const U32 BIT_SIZE	= 6;
	const U32 BIT_POS	= 10;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1)<<BIT_POS;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 6 == ParityByte || 12 == ParityByte || 19 == ParityByte || 25 == ParityByte || 41 == ParityByte );

	regval = ReadIODW(&__g_pRegister->NFECCCTRL);
	regval &= ~BIT_MASK;
	regval |= (ParityByte << BIT_POS) & BIT_MASK;
	WriteIODW(&__g_pRegister->NFECCCTRL, regval);
}

U32		NX_MCUS_GetParityCount( void )
{
	const U32 BIT_SIZE	= 6;
	const U32 BIT_POS	= 10;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1)<<BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (ReadIODW(&__g_pRegister->NFECCCTRL) & BIT_MASK) >> BIT_POS;
}

void	NX_MCUS_SetNANDRWDataNum( U32 DataNum )
{
	const U32 BIT_SIZE	= 10;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1)<<BIT_POS;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 512 == DataNum || 1024 == DataNum );

	regval = ReadIODW(&__g_pRegister->NFECCCTRL);
	regval &= ~BIT_MASK;
	regval |= ((DataNum-1) << BIT_POS) & BIT_MASK;
	WriteIODW(&__g_pRegister->NFECCCTRL, regval);
}

U32		NX_MCUS_GetNANDRWDataNum( void )
{
	const U32 BIT_SIZE	= 10;
	const U32 BIT_POS	= 0;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1)<<BIT_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ((ReadIODW(&__g_pRegister->NFECCCTRL) & BIT_MASK) >> BIT_POS)+1;
}

void	NX_MCUS_SetELP4( U16 *pELP )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELP );

	regval =  (U32)(pELP[0] & BIT_MASK)
			| (U32)(pELP[1] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[0], regval);
	regval =  (U32)(pELP[2] & BIT_MASK)
			| (U32)(pELP[3] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[1], regval);
}
void	NX_MCUS_SetELP8( U16 *pELP )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELP );

	regval =  (U32)(pELP[0] & BIT_MASK)
			| (U32)(pELP[1] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[0], regval);
	regval =  (U32)(pELP[2] & BIT_MASK)
			| (U32)(pELP[3] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[1], regval);
	regval =  (U32)(pELP[4] & BIT_MASK)
			| (U32)(pELP[5] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[2], regval);
	regval =  (U32)(pELP[6] & BIT_MASK)
			| (U32)(pELP[7] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[3], regval);
}
void	NX_MCUS_SetELP12( U16 *pELP )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELP );

	regval =  (U32)(pELP[0] & BIT_MASK)
			| (U32)(pELP[1] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[0], regval);
	regval =  (U32)(pELP[2] & BIT_MASK)
			| (U32)(pELP[3] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[1], regval);
	regval =  (U32)(pELP[4] & BIT_MASK)
			| (U32)(pELP[5] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[2], regval);
	regval =  (U32)(pELP[6] & BIT_MASK)
			| (U32)(pELP[7] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[3], regval);
	regval =  (U32)(pELP[8] & BIT_MASK)
			| (U32)(pELP[9] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[4], regval);
	regval =  (U32)(pELP[10] & BIT_MASK)
			| (U32)(pELP[11] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[5], regval);
}

void	NX_MCUS_SetELP16( U16 *pELP )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELP );

	regval =  (U32)(pELP[0] & BIT_MASK)
			| (U32)(pELP[1] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[0], regval);
	regval =  (U32)(pELP[2] & BIT_MASK)
			| (U32)(pELP[3] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[1], regval);
	regval =  (U32)(pELP[4] & BIT_MASK)
			| (U32)(pELP[5] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[2], regval);
	regval =  (U32)(pELP[6] & BIT_MASK)
			| (U32)(pELP[7] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[3], regval);
	regval =  (U32)(pELP[8] & BIT_MASK)
			| (U32)(pELP[9] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[4], regval);
	regval =  (U32)(pELP[10] & BIT_MASK)
			| (U32)(pELP[11] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[5], regval);
	regval =  (U32)(pELP[12] & BIT_MASK)
			| (U32)(pELP[13] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[6], regval);
	regval =  (U32)(pELP[14] & BIT_MASK)
			| (U32)(pELP[15] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[7], regval);
}
void	NX_MCUS_SetELP24( U16 *pELP )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELP );

	regval =  (U32)(pELP[0] & BIT_MASK)
			| (U32)(pELP[1] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[0], regval);
	regval =  (U32)(pELP[2] & BIT_MASK)
			| (U32)(pELP[3] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[1], regval);
	regval =  (U32)(pELP[4] & BIT_MASK)
			| (U32)(pELP[5] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[2], regval);
	regval =  (U32)(pELP[6] & BIT_MASK)
			| (U32)(pELP[7] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[3], regval);
	regval =  (U32)(pELP[8] & BIT_MASK)
			| (U32)(pELP[9] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[4], regval);
	regval =  (U32)(pELP[10] & BIT_MASK)
			| (U32)(pELP[11] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[5], regval);
	regval =  (U32)(pELP[12] & BIT_MASK)
			| (U32)(pELP[13] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[6], regval);
	regval =  (U32)(pELP[14] & BIT_MASK)
			| (U32)(pELP[15] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[7], regval);
	regval =  (U32)(pELP[16] & BIT_MASK)
			| (U32)(pELP[17] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[8], regval);
	regval =  (U32)(pELP[18] & BIT_MASK)
			| (U32)(pELP[19] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[9], regval);
	regval =  (U32)(pELP[20] & BIT_MASK)
			| (U32)(pELP[21] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[10], regval);
	regval =  (U32)(pELP[22] & BIT_MASK)
			| (U32)(pELP[23] & BIT_MASK)<<BIT_POS;
	WriteIODW(&__g_pRegister->NFELP[11], regval);
}
void	NX_MCUS_GetErrLoc4( U16 *pELoc )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELoc );

	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[0]);
	pELoc[0] = (U16)(regval & BIT_MASK);
	pELoc[1] = (U16)((regval >> BIT_POS) & BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[1]);
	pELoc[2] = (U16)(regval & BIT_MASK);
	pELoc[3] = (U16)((regval >> BIT_POS) & BIT_MASK);
}
void	NX_MCUS_GetErrLoc8( U16 *pELoc )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELoc );

	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[0]);
	pELoc[0] = (U16)(regval					& BIT_MASK);
	pELoc[1] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[1]);
	pELoc[2] = (U16)(regval					& BIT_MASK);
	pELoc[3] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[2]);
	pELoc[4] = (U16)(regval					& BIT_MASK);
	pELoc[5] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[3]);
	pELoc[6] = (U16)(regval					& BIT_MASK);
	pELoc[7] = (U16)((regval >> BIT_POS)	& BIT_MASK);
}
void	NX_MCUS_GetErrLoc12( U16 *pELoc )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELoc );

	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[0]);
	pELoc[0] = (U16)(regval					& BIT_MASK);
	pELoc[1] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[1]);
	pELoc[2] = (U16)(regval					& BIT_MASK);
	pELoc[3] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[2]);
	pELoc[4] = (U16)(regval					& BIT_MASK);
	pELoc[5] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[3]);
	pELoc[6] = (U16)(regval					& BIT_MASK);
	pELoc[7] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[4]);
	pELoc[8] = (U16)(regval					& BIT_MASK);
	pELoc[9] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[5]);
	pELoc[10] = (U16)(regval				& BIT_MASK);
	pELoc[11] = (U16)((regval >> BIT_POS)	& BIT_MASK);
}
void	NX_MCUS_GetErrLoc16( U16 *pELoc )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELoc );

	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[0]);
	pELoc[0] = (U16)(regval					& BIT_MASK);
	pELoc[1] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[1]);
	pELoc[2] = (U16)(regval					& BIT_MASK);
	pELoc[3] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[2]);
	pELoc[4] = (U16)(regval					& BIT_MASK);
	pELoc[5] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[3]);
	pELoc[6] = (U16)(regval					& BIT_MASK);
	pELoc[7] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[4]);
	pELoc[8] = (U16)(regval					& BIT_MASK);
	pELoc[9] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[5]);
	pELoc[10] = (U16)(regval				& BIT_MASK);
	pELoc[11] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[6]);
	pELoc[12] = (U16)(regval				& BIT_MASK);
	pELoc[13] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[7]);
	pELoc[14] = (U16)(regval				& BIT_MASK);
	pELoc[15] = (U16)((regval >> BIT_POS)	& BIT_MASK);
}

void	NX_MCUS_GetErrLoc24( U16 *pELoc )
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 14;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != pELoc );

	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[0]);
	pELoc[0] = (U16)(regval					& BIT_MASK);
	pELoc[1] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[1]);
	pELoc[2] = (U16)(regval					& BIT_MASK);
	pELoc[3] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[2]);
	pELoc[4] = (U16)(regval					& BIT_MASK);
	pELoc[5] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[3]);
	pELoc[6] = (U16)(regval					& BIT_MASK);
	pELoc[7] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[4]);
	pELoc[8] = (U16)(regval					& BIT_MASK);
	pELoc[9] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[5]);
	pELoc[10] = (U16)(regval				& BIT_MASK);
	pELoc[11] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[6]);
	pELoc[12] = (U16)(regval				& BIT_MASK);
	pELoc[13] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[7]);
	pELoc[14] = (U16)(regval				& BIT_MASK);
	pELoc[15] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[8]);
	pELoc[16] = (U16)(regval				& BIT_MASK);
	pELoc[17] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[9]);
	pELoc[18] = (U16)(regval				& BIT_MASK);
	pELoc[19] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[10]);
	pELoc[20] = (U16)(regval				& BIT_MASK);
	pELoc[21] = (U16)((regval >> BIT_POS)	& BIT_MASK);
	regval = ReadIODW(&__g_pRegister->NFERRLOCATION[11]);
	pELoc[22] = (U16)(regval				& BIT_MASK);
	pELoc[23] = (U16)((regval >> BIT_POS)	& BIT_MASK);
}
