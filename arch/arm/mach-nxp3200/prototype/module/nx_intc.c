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
//	Module		: Interrupt Controller
//	File		: nx_intc.c
//	Description	:
//	Author		:
//	History		:
//					2007/04/03	first
//------------------------------------------------------------------------------

#include "nx_intc.h"

static	struct NX_INTC_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
//	Module Interface
//------------------------------------------------------------------------------

/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.
 *	@see	NX_INTC_GetNumberOfModule
 */
CBOOL	NX_INTC_Initialize( void )
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
 *	@see		NX_INTC_Initialize
 */
U32		NX_INTC_GetNumberOfModule( void )
{
	return NUMBER_OF_INTC_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see										NX_INTC_GetSizeOfRegisterSet,
 *				NX_INTC_SetBaseAddress,			NX_INTC_GetBaseAddress,
 *				NX_INTC_OpenModule,				NX_INTC_CloseModule,
 *				NX_INTC_CheckBusy,				NX_INTC_CanPowerDown
 */
U32		NX_INTC_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_INTC_MODULE );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_INTC_GetPhysicalAddress,
 *				NX_INTC_SetBaseAddress,			NX_INTC_GetBaseAddress,
 *				NX_INTC_OpenModule,				NX_INTC_CloseModule,
 *				NX_INTC_CheckBusy,				NX_INTC_CanPowerDown
 */
U32		NX_INTC_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_INTC_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_INTC_GetPhysicalAddress,		NX_INTC_GetSizeOfRegisterSet,
 *												NX_INTC_GetBaseAddress,
 *				NX_INTC_OpenModule,				NX_INTC_CloseModule,
 *				NX_INTC_CheckBusy,				NX_INTC_CanPowerDown
 */
void	NX_INTC_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	__g_pRegister = (struct NX_INTC_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_INTC_GetPhysicalAddress,		NX_INTC_GetSizeOfRegisterSet,
 *				NX_INTC_SetBaseAddress,
 *				NX_INTC_OpenModule,				NX_INTC_CloseModule,
 *				NX_INTC_CheckBusy,				NX_INTC_CanPowerDown
 */
U32		NX_INTC_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_INTC_GetPhysicalAddress,		NX_INTC_GetSizeOfRegisterSet,
 *				NX_INTC_SetBaseAddress,			NX_INTC_GetBaseAddress,
 *												NX_INTC_CloseModule,
 *				NX_INTC_CheckBusy,				NX_INTC_CanPowerDown
 */
CBOOL	NX_INTC_OpenModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_INTC_GetPhysicalAddress,		NX_INTC_GetSizeOfRegisterSet,
 *				NX_INTC_SetBaseAddress,			NX_INTC_GetBaseAddress,
 *				NX_INTC_OpenModule,
 *				NX_INTC_CheckBusy,				NX_INTC_CanPowerDown
 */
CBOOL	NX_INTC_CloseModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_INTC_GetPhysicalAddress,		NX_INTC_GetSizeOfRegisterSet,
 *				NX_INTC_SetBaseAddress,			NX_INTC_GetBaseAddress,
 *				NX_INTC_OpenModule,				NX_INTC_CloseModule,
 *												NX_INTC_CanPowerDown
 */
CBOOL	NX_INTC_CheckBusy( void )
{
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_INTC_GetPhysicalAddress,		NX_INTC_GetSizeOfRegisterSet,
 *				NX_INTC_SetBaseAddress,			NX_INTC_GetBaseAddress,
 *				NX_INTC_OpenModule,				NX_INTC_CloseModule,
 *				NX_INTC_CheckBusy
 */
CBOOL	NX_INTC_CanPowerDown( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number .
 *	@param[in]	Enable	\b CTRUE	indicate that Interrupt Enable. \n
 *						\b CFALSE	indicate that Interrupt Disable.
 *	@return		None.
 *	@see											NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
void	NX_INTC_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	register struct NX_INTC_RegisterSet *pRegister;
	register int RegSel, RegBit;
	register U32 regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( NX_INTC_NUM_OF_INT > IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	Enable = (CBOOL)((U32)Enable ^ 1);	// Service:0, Masked:1

	pRegister = __g_pRegister;
	RegSel	= IntNum >> 5;		// 0 or 1
	RegBit	= IntNum & 0x1F;	// 0 ~ 31

	regvalue = pRegister->INTMASK[RegSel];
	regvalue &= ~( (U32)1		<< RegBit);
	regvalue |=	( (U32)Enable << RegBit);

//	pRegister->INTMASK[RegSel] = regvalue;
	WriteIODW(&pRegister->INTMASK[RegSel], regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number.
 *	@return		\b CTRUE	indicate that Interrupt is enabled. \n
 *				\b CFALSE	indicate that Interrupt is disabled.
 *	@see		NX_INTC_SetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
CBOOL	NX_INTC_GetInterruptEnable( S32 IntNum )
{
	register struct NX_INTC_RegisterSet *pRegister;
	register int RegSel, RegBit;
	register U32 regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( NX_INTC_NUM_OF_INT > IntNum );

	pRegister = __g_pRegister;
	RegSel	= IntNum >> 5;		// 0 or 1
	RegBit	= IntNum & 0x1F;	// 0 ~ 31

	regvalue = pRegister->INTMASK[RegSel];

	if( regvalue & ( 1UL << RegBit) )
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableHigh	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n\n
 *							-EnableHigh[0] : I2C0_______, EnableHigh[10] : _____________, EnableHigh[20] : OHCI_____, EnableHigh[30] : ECTOUCH \n
 *							-EnableHigh[1] : I2C1_______, EnableHigh[11] : TIMER4_______, EnableHigh[21] : _________, EnableHigh[31] : - \n
 *							-EnableHigh[2] : UART1______, EnableHigh[12] : _____________, EnableHigh[22] : _________\n
 *							-EnableHigh[3] : ___________, EnableHigh[13] : _____________, EnableHigh[23] : _________\n
 *							-EnableHigh[4] : ___________, EnableHigh[14] : I2S__________, EnableHigh[24] : _________\n
 *							-EnableHigh[5] : ___________, EnableHigh[15] : _____________, EnableHigh[25] : _________\n
 *							-EnableHigh[6] : ___________, EnableHigh[16] : _____________, EnableHigh[26] : _________\n
 *							-EnableHigh[7] : ___________, EnableHigh[17] : CDROM________, EnableHigh[27] : _________\n
 *							-EnableHigh[8] : ___________, EnableHigh[18] : ALIVE________, EnableHigh[28] : _________\n
 *							-EnableHigh[9] : CSC________, EnableHigh[19] : EHCI_________, EnableHigh[29] : _________\n\n
 *
 *	@param[in]	EnableLow	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n\n
 *							-EnableLow[0] : PDISPLAY____, EnableLow[10] : UART0_____, EnableLow[20] : UDC___________, EnableLow[30] : SCALER	\n
 *							-EnableLow[1] : ____________, EnableLow[11] : TIMER1____, EnableLow[21] : TIMER3________, EnableLow[31] : RTC		\n
 *							-EnableLow[2] : VIP_________, EnableLow[12] : SSPSPI____, EnableLow[22] : DEINTERLACE___\n
 *							-EnableLow[3] : DMA_________, EnableLow[13] : GPIO______, EnableLow[23] : PPM___________\n
 *							-EnableLow[4] : TIMER0______, EnableLow[14] : SDMMC0____, EnableLow[24] : AUDIOIF_______\n
 *							-EnableLow[5] : SYSCTRL_____, EnableLow[15] : TIMER2____, EnableLow[25] : ADC___________\n
 *							-EnableLow[6] : ____________, EnableLow[16] : H264______, EnableLow[26] : MCUSTATIC_____\n
 *							-EnableLow[7] : MPEGTSI_____, EnableLow[17] : MPEG______, EnableLow[27] : GRP3D_________\n
 *							-EnableLow[8] : ____________, EnableLow[18] : __________, EnableLow[28] : UHC___________\n
 *							-EnableLow[9] : ____________, EnableLow[19] : VLC_______, EnableLow[29] : ROTATOR_______\n\n
 *	@return		None.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *													NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
void	NX_INTC_SetInterruptEnable64( U32 EnableHigh, U32 EnableLow )
{
	const U32 INTMASKL_MASK = ~((1<<6)|(1<<8)|(1<<9)|(1<<18));		// Reserved Region
	const U32 INTMASKH_MASK = ~((1<<12)|(1UL<<31)|(((1<<10)-1)<<21));	// Reserved Region

	register struct NX_INTC_RegisterSet *pRegister;
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

//	pRegister->INTMASK[0] = EnableLow  & INTMASKL_MASK;
//	pRegister->INTMASK[1] = EnableHigh & INTMASKH_MASK;
	WriteIODW(&pRegister->INTMASK[0], EnableLow	 & INTMASKL_MASK);
	WriteIODW(&pRegister->INTMASK[1], EnableHigh & INTMASKH_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[out]	pEnableHigh		High part setting value of interrupt. \n\n
 *								-pEnableHigh[0] : I2C0_______, pEnableHigh[10] : _____________, pEnableHigh[20] : OHCI_____, pEnableHigh[30] : ECTOUCH \n
 *								-pEnableHigh[1] : I2C1_______, pEnableHigh[11] : TIMER4_______, pEnableHigh[21] : _________, pEnableHigh[31] : - \n
 *								-pEnableHigh[2] : UART1______, pEnableHigh[12] : _____________, pEnableHigh[22] : _________\n
 *								-pEnableHigh[3] : ___________, pEnableHigh[13] : _____________, pEnableHigh[23] : _________\n
 *								-pEnableHigh[4] : ___________, pEnableHigh[14] : I2S__________, pEnableHigh[24] : _________\n
 *								-pEnableHigh[5] : ___________, pEnableHigh[15] : _____________, pEnableHigh[25] : _________\n
 *								-pEnableHigh[6] : ___________, pEnableHigh[16] : _____________, pEnableHigh[26] : _________\n
 *								-pEnableHigh[7] : ___________, pEnableHigh[17] : CDROM________, pEnableHigh[27] : _________\n
 *								-pEnableHigh[8] : ___________, pEnableHigh[18] : ALIVE________, pEnableHigh[28] : _________\n
 *								-pEnableHigh[9] : CSC________, pEnableHigh[19] : EHCI_________, pEnableHigh[29] : _________\n\n
 *
 *	@param[out]	pEnableLow		Low part	setting value of interrupt. \n
 *								-pEnableLow[0] : PDISPLAY____, pEnableLow[10] : UART0_____, pEnableLow[20] : UDC___________, pEnableLow[30] : SCALER	\n
 *								-pEnableLow[1] : ____________, pEnableLow[11] : TIMER1____, pEnableLow[21] : TIMER3________, pEnableLow[31] : RTC		\n
 *								-pEnableLow[2] : VIP_________, pEnableLow[12] : SSPSPI____, pEnableLow[22] : DEINTERLACE___\n
 *								-pEnableLow[3] : DMA_________, pEnableLow[13] : GPIO______, pEnableLow[23] : PPM___________\n
 *								-pEnableLow[4] : TIMER0______, pEnableLow[14] : SDMMC0____, pEnableLow[24] : AUDIOIF_______\n
 *								-pEnableLow[5] : SYSCTRL_____, pEnableLow[15] : TIMER2____, pEnableLow[25] : ADC___________\n
 *								-pEnableLow[6] : ____________, pEnableLow[16] : H264______, pEnableLow[26] : MCUSTATIC_____\n
 *								-pEnableLow[7] : MPEGTSI_____, pEnableLow[17] : MPEG______, pEnableLow[27] : GRP3D_________\n
 *								-pEnableLow[8] : ____________, pEnableLow[18] : __________, pEnableLow[28] : UHC___________\n
 *								-pEnableLow[9] : ____________, pEnableLow[19] : VLC_______, pEnableLow[29] : ROTATOR_______\n\n
 *	@return		None.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
void	NX_INTC_GetInterruptEnable64( U32 *pEnableHigh, U32 *pEnableLow )
{
	const U32 INTMASKL_MASK = ~((1<<6)|(1<<8)|(1<<9)|(1<<18));		// Reserved Region
	const U32 INTMASKH_MASK = ~((1<<12)|(1UL<<31)|(((1<<10)-1)<<21));	// Reserved Region

	register struct NX_INTC_RegisterSet *pRegister;
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	*pEnableLow	= pRegister->INTMASK[0] & INTMASKL_MASK;
	*pEnableHigh = pRegister->INTMASK[1] & INTMASKH_MASK;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number.
 *	@return		\b CTRUE	indicate that Pending is seted. \n
 *				\b CFALSE	indicate that Pending is Not Seted.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *													NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
CBOOL	NX_INTC_GetInterruptPending( S32 IntNum )
{
	register struct NX_INTC_RegisterSet *pRegister;
	register int RegSel, RegBit;
	register U32 regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( NX_INTC_NUM_OF_INT > IntNum );

	pRegister = __g_pRegister;
	RegSel	= IntNum >> 5;		// 0 or 1
	RegBit	= IntNum & 0x1F;	// 0 ~ 31

	regvalue = pRegister->INTPEND[RegSel];

	if( regvalue & ( 1UL << RegBit) )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[out]	pPendingHigh	Interrupt controller's pending bit of HIGH part. \n\n
 *								-pPendingHigh[0] : I2C0_______, pPendingHigh[10] : SDMMC1_______, pPendingHigh[20] : OHCI_____, pPendingHigh[30] : ECTOUCH \n
 *								-pPendingHigh[1] : I2C1_______, pPendingHigh[11] : TIMER4_______, pPendingHigh[21] : _________, pPendingHigh[31] : - \n
 *								-pPendingHigh[2] : UART1______, pPendingHigh[12] : _____________, pPendingHigh[22] : _________\n
 *								-pPendingHigh[3] : ___________, pPendingHigh[13] : _____________, pPendingHigh[23] : _________\n
 *								-pPendingHigh[4] : ___________, pPendingHigh[14] : I2S__________, pPendingHigh[24] : _________\n
 *								-pPendingHigh[5] : ___________, pPendingHigh[15] : _____________, pPendingHigh[25] : _________\n
 *								-pPendingHigh[6] : ___________, pPendingHigh[16] : _____________, pPendingHigh[26] : _________\n
 *								-pPendingHigh[7] : ___________, pPendingHigh[17] : CDROM________, pPendingHigh[27] : _________\n
 *								-pPendingHigh[8] : ___________, pPendingHigh[18] : ALIVE________, pPendingHigh[28] : _________\n
 *								-pPendingHigh[9] : CSC________, pPendingHigh[19] : EHCI_________, pPendingHigh[29] : _________\n\n
 *
 *	@param[out]	pPendingLow		Interrupt controller's pending bit of LOW part.\n\n
 *								-pPendingLow[0] : PDISPLAY____, pPendingLow[10] : UART0_____, pPendingLow[20] : UDC___________, pPendingLow[30] : SCALER	\n
 *								-pPendingLow[1] : ____________, pPendingLow[11] : TIMER1____, pPendingLow[21] : TIMER3________, pPendingLow[31] : RTC		\n
 *								-pPendingLow[2] : VIP_________, pPendingLow[12] : SSPSPI____, pPendingLow[22] : DEINTERLACE___\n
 *								-pPendingLow[3] : DMA_________, pPendingLow[13] : GPIO______, pPendingLow[23] : PPM___________\n
 *								-pPendingLow[4] : TIMER0______, pPendingLow[14] : SDMMC0____, pPendingLow[24] : AUDIOIF_______\n
 *								-pPendingLow[5] : SYSCTRL_____, pPendingLow[15] : TIMER2____, pPendingLow[25] : ADC___________\n
 *								-pPendingLow[6] : ____________, pPendingLow[16] : H264______, pPendingLow[26] : MCUSTATIC_____\n
 *								-pPendingLow[7] : MPEGTSI_____, pPendingLow[17] : MPEG______, pPendingLow[27] : GRP3D_________\n
 *								-pPendingLow[8] : ____________, pPendingLow[18] : __________, pPendingLow[28] : UHC___________\n
 *								-pPendingLow[9] : ____________, pPendingLow[19] : VLC_______, pPendingLow[29] : ROTATOR_______\n\n
 *	@return		None.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
void	NX_INTC_GetInterruptPending64( U32 *pPendingHigh, U32 *pPendingLow )
{
	const U32 INTPENDL_MASK = ~((1<<6)|(1<<8)|(1<<9)|(1<<18));			// Reserved Region
	const U32 INTPENDH_MASK = ~((1<<12)|(1UL<<31)|(((1<<10)-1)<<21));	// Reserved Region

	register struct NX_INTC_RegisterSet *pRegister;
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	*pPendingLow  = pRegister->INTPEND[0] & INTPENDL_MASK;
	*pPendingHigh =	pRegister->INTPEND[1] & INTPENDH_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number.
 *	@return		None.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *													NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
void	NX_INTC_ClearInterruptPending( S32 IntNum )
{
	register struct NX_INTC_RegisterSet *pRegister;
	register int RegSel, RegBit;
	register U32 regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( NX_INTC_NUM_OF_INT > IntNum );

	pRegister = __g_pRegister;
	RegSel	= IntNum >> 5;		// 0 or 1
	RegBit	= IntNum & 0x1F;	// 0 ~ 31

	regvalue = pRegister->INTPEND[RegSel];
	regvalue = 1UL << RegBit;

//	pRegister->INTPEND[RegSel] = regvalue;
	WriteIODW(&pRegister->INTPEND[RegSel], regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingHigh		Interrupt controller's HIGH part pending bit to clear.\n\n
 *								-PendingHigh[0] : I2C0_______, PendingHigh[10] : _____________, PendingHigh[20] : OHCI_____, PendingHigh[30] : ECTOUCH \n
 *								-PendingHigh[1] : I2C1_______, PendingHigh[11] : TIMER4_______, PendingHigh[21] : _________, PendingHigh[31] : - \n
 *								-PendingHigh[2] : ___________, PendingHigh[12] : _____________, PendingHigh[22] : _________\n
 *								-PendingHigh[3] : ___________, PendingHigh[13] : _____________, PendingHigh[23] : _________\n
 *								-PendingHigh[4] : ___________, PendingHigh[14] : I2S__________, PendingHigh[24] : _________\n
 *								-PendingHigh[5] : ___________, PendingHigh[15] : _____________, PendingHigh[25] : _________\n
 *								-PendingHigh[6] : ___________, PendingHigh[16] : _____________, PendingHigh[26] : _________\n
 *								-PendingHigh[7] : ___________, PendingHigh[17] : CDROM________, PendingHigh[27] : _________\n
 *								-PendingHigh[8] : ___________, PendingHigh[18] : ALIVE________, PendingHigh[28] : _________\n
 *								-PendingHigh[9] : CSC________, PendingHigh[19] : EHCI_________, PendingHigh[29] : _________\n\n
 *
 *	@param[in]	PendingLow		Interrupt controller's LOW part pending bit to Clear. \n\n
 *								-PendingLow[0] : PDISPLAY____, PendingLow[10] : UART0_____, PendingLow[20] : UDC___________, PendingLow[30] : SCALER	\n
 *								-PendingLow[1] : ____________, PendingLow[11] : TIMER1____, PendingLow[21] : TIMER3________, PendingLow[31] : RTC		\n
 *								-PendingLow[2] : VIP_________, PendingLow[12] : SSPSPI____, PendingLow[22] : DEINTERLACE___\n
 *								-PendingLow[3] : DMA_________, PendingLow[13] : GPIO______, PendingLow[23] : PPM___________\n
 *								-PendingLow[4] : TIMER0______, PendingLow[14] : SDMMC0____, PendingLow[24] : AUDIOIF_______\n
 *								-PendingLow[5] : SYSCTRL_____, PendingLow[15] : TIMER2____, PendingLow[25] : ADC___________\n
 *								-PendingLow[6] : ____________, PendingLow[16] : H264______, PendingLow[26] : MCUSTATIC_____\n
 *								-PendingLow[7] : MPEGTSI_____, PendingLow[17] : MPEG______, PendingLow[27] : GRP3D_________\n
 *								-PendingLow[8] : ____________, PendingLow[18] : __________, PendingLow[28] : UHC___________\n
 *								-PendingLow[9] : ____________, PendingLow[19] : VLC_______, PendingLow[29] : ROTATOR_______\n\n
 *	@return		None.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
void	NX_INTC_ClearInterruptPending64( U32 PendingHigh, U32 PendingLow )
{
	const U32 INTPENDL_MASK = ~((1<<6)|(1<<8)|(1<<9)|(1<<18));			// Reserved Region
	const U32 INTPENDH_MASK = ~((1<<12)|(1UL<<31)|(((1<<10)-1)<<21));	// Reserved Region

	register struct NX_INTC_RegisterSet *pRegister;
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

//	pRegister->INTPEND[0] = PendingLow	& INTPENDL_MASK;
//	pRegister->INTPEND[1] = PendingHigh & INTPENDH_MASK;
	WriteIODW(&pRegister->INTPEND[0], PendingLow  & INTPENDL_MASK);
	WriteIODW(&pRegister->INTPEND[1], PendingHigh & INTPENDH_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicate that Set to all interrupt enable. \n
 *						\b CFALSE	indicate that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *													NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
void	NX_INTC_SetInterruptEnableAll( CBOOL Enable )
{
	register struct NX_INTC_RegisterSet *pRegister;
	register U32 regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( (0==Enable) || (1==Enable) );

	pRegister = __g_pRegister;

	if( Enable )
	{	regvalue = 0;	}
	else
	{	regvalue = (U32)(~0); }

//	pRegister->INTMASK[0] = regvalue;
//	pRegister->INTMASK[1] = regvalue;
	WriteIODW(&pRegister->INTMASK[0], regvalue);
	WriteIODW(&pRegister->INTMASK[1], regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicate that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicate that All interrupt is disabled.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
CBOOL	NX_INTC_GetInterruptEnableAll( void )
{
	register struct NX_INTC_RegisterSet *pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	return (CBOOL)( (0xFFFFFFFF != pRegister->INTMASK[0]) || (0xFFFFFFFF != pRegister->INTMASK[1]) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicate that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicate that All pending is NOT seted.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *													NX_INTC_ClearInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
CBOOL	NX_INTC_GetInterruptPendingAll( void )
{
	register struct NX_INTC_RegisterSet *pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	return (CBOOL)( (0 != pRegister->INTPEND[0]) || (0 != pRegister->INTPEND[1]) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,
 *				NX_INTC_GetInterruptPendingNumber
 */
void	NX_INTC_ClearInterruptPendingAll( void )
{
	register struct NX_INTC_RegisterSet *pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;


//	pRegister->INTPEND[0] = (U32)(0xFFFFFFFF);
//	pRegister->INTPEND[1] = (U32)(0xFFFFFFFF);
	WriteIODW(&pRegister->INTPEND[0], (0xFFFFFFFF));
	WriteIODW(&pRegister->INTPEND[1], (0xFFFFFFFF));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_INTC_SetInterruptEnable,			NX_INTC_GetInterruptEnable,
 *				NX_INTC_SetInterruptEnable64,		NX_INTC_GetInterruptEnable64,
 *				NX_INTC_GetInterruptPending,		NX_INTC_GetInterruptPending64,
 *				NX_INTC_ClearInterruptPending,		NX_INTC_ClearInterruptPending64,
 *				NX_INTC_SetInterruptEnableAll,		NX_INTC_GetInterruptEnableAll,
 *				NX_INTC_GetInterruptPendingAll,		NX_INTC_ClearInterruptPendingAll
 */
S32		NX_INTC_GetInterruptPendingNumber( void )	// -1 if None
{
	U32 intnum, intoff;
	U32 intpend;
	register struct NX_INTC_RegisterSet *pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	intoff	= 0;
	intpend = pRegister->INTPEND[0];
	if( intpend == 0)
	{
		intoff	= 32;
		intpend = pRegister->INTPEND[1];
	}

	for( intnum=0 ; intnum<32 ; intnum++ )
	{
		if( 0 != (intpend & (1UL<<intnum)) )
		{
			return ( intnum + intoff );
		}
	}

	return -1;
}


//------------------------------------------------------------------------------
//Interrupt Controller Operation.
//------------------------------------------------------------------------------
/**
 *	@brief		Set an interrupt mode to be generated when the corresponding interrupt is issued.
 *	@param[in]	IntNum		An interrupt number.
 *	@param[in]	IntMode		An interrupt mode, IRQ or FIQ.
 *	@return		None.
 */
void	NX_INTC_SetInterruptMode ( U32 IntNum, NX_INTC_INTMODE IntMode	)
{
	register struct NX_INTC_RegisterSet *pRegister;
	register int RegSel, RegBit;
	register U32 regvalue = 0;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( NX_INTC_NUM_OF_INT > IntNum );
	NX_ASSERT( NX_INTC_INTMODE_FIQ >= IntMode );

	pRegister = __g_pRegister;
	RegSel	= IntNum >> 5;		// 0 or 1
	RegBit	= IntNum & 0x1F;	// 0 ~ 31

	switch( IntMode )
	{
	case NX_INTC_INTMODE_IRQ:
			regvalue &= (~(U32)(1UL<<RegBit));

		//	pRegister->INTMODE[RegSel] = regvalue;
			WriteIODW(&pRegister->INTMODE[RegSel],  regvalue);
		break;

	case NX_INTC_INTMODE_FIQ:
			regvalue |= ( (U32)(1UL<<RegBit));

		//	pRegister->INTMODE[RegSel] = regvalue;
			WriteIODW(&pRegister->INTMODE[RegSel],  regvalue);
		break;

	default:
		NX_ASSERT( CFALSE );
	}

}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a priority control.
 *	@param[in]	ArbiterNum	An arbiter number to be changed, 0 ~ 11 are valid.
 *	@param[in]	OrderSel	Determines the order of priority manually.
 *	@return		None.
 *	@remark		Arbiter0	: for interrupt	0 ~	5 \n
 *				Arbiter1	: for interrupt	6 ~ 11 \n
 *				Arbiter2	: for interrupt 12 ~ 17 \n
 *				Arbiter3	: for interrupt 18 ~ 23 \n
 *				Arbiter4	: for interrupt 24 ~ 29 \n
 *				Arbiter5	: for interrupt 30 ~ 35 \n
 *				Arbiter6	: for interrupt 36 ~ 41 \n
 *				Arbiter7	: for interrupt 42 ~ 47 \n
 *				Arbiter8	: for interrupt 48 ~ 53 \n
 *				Arbiter9	: for interrupt 54 ~ 59 \n
 *				Arbiter10 : for interrupt 60 ~ 63 \n
 *				Arbiter11 : arbiter 0 ~ 10
 */
void	NX_INTC_SetPriorityMode( U32 ArbiterNum, U32 OrderSel )
{
	register struct NX_INTC_RegisterSet *pRegister;
	register U32 reg;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 12 > ArbiterNum );
	NX_ASSERT( ((4 > OrderSel) && (11 > ArbiterNum)) || ((8 > OrderSel) && (11 == ArbiterNum)) );

	pRegister = __g_pRegister;

	reg = pRegister->PRIORDER;
	if( 11 > ArbiterNum )	reg &= ~(3UL<<(ArbiterNum<<1));
	else					reg &= ~(7UL<<(ArbiterNum<<1));
	reg |= (OrderSel<<(ArbiterNum<<1));

//	pRegister->PRIORDER = reg;
	WriteIODW(&pRegister->PRIORDER, reg);
}
