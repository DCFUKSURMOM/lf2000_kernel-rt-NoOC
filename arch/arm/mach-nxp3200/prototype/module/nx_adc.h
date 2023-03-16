//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: ADC
//	File		: nx_adc.h
//	Description:
//	Author		: Firmware Team
//	History	:
//------------------------------------------------------------------------------
#ifndef __NX_ADC_H__
#define __NX_ADC_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	ADC ADC
//------------------------------------------------------------------------------
//@{

	/// @brief	ADC Module's Register List
	struct	NX_ADC_RegisterSet
	{
		volatile U16	ADCCON;					///< 0x00 : ADC Control Register
		volatile U16	RESERVED_0;				///< 0x02 : RESERVED_0
		volatile U16	ADCDAT;					///< 0x04 : ADC Output Data Register
		volatile U16	RESERVED_1;				///< 0x06 : RESERVED_1
		volatile U16	ADCINTENB;				///< 0x08 : ADC Interrupt Enable Register
		volatile U16	RESERVED_2;				///< 0x0A : RESERVED_2
		volatile U16	ADCINTCLR;				///< 0x0C : ADC Interrutp Pending and Clear Register
		volatile U16	RESERVED[0x19];			///< 0x0E ~ 0x3E : Reserved Region
		volatile U32	CLKENB;					///< 0x40 : ADC Clock Enable Register
	};

	/// @brief ADC interrupt for interrupt interface
	enum
	{
		NX_ADC_INT_EOC = 0			///< ADC convert end interrupt.
	};

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_ADC_Initialize( void );
U32		NX_ADC_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_ADC_GetPhysicalAddress( void );
U32		NX_ADC_GetSizeOfRegisterSet( void );
void	NX_ADC_SetBaseAddress( U32 BaseAddress );
U32		NX_ADC_GetBaseAddress( void );
CBOOL	NX_ADC_OpenModule( void );
CBOOL	NX_ADC_CloseModule( void );
CBOOL	NX_ADC_CheckBusy( void );
CBOOL	NX_ADC_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_ADC_GetInterruptNumber( void );

void	NX_ADC_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_ADC_GetInterruptEnable( S32 IntNum );
CBOOL	NX_ADC_GetInterruptPending( S32 IntNum );
void	NX_ADC_ClearInterruptPending( S32 IntNum );

void	NX_ADC_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_ADC_GetInterruptEnableAll( void );
CBOOL	NX_ADC_GetInterruptPendingAll( void );
void	NX_ADC_ClearInterruptPendingAll( void );

void	NX_ADC_SetInterruptEnable32( U32 EnableFlag );
U32		NX_ADC_GetInterruptEnable32( void );
U32		NX_ADC_GetInterruptPending32( void );
void	NX_ADC_ClearInterruptPending32( U32 PendingFlag );

S32		NX_ADC_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{

void			NX_ADC_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE		NX_ADC_GetClockPClkMode( void );

//@}

//------------------------------------------------------------------------------
///	@name	ADC Operation.
//@{
void	NX_ADC_SetPrescalerValue( U32 value );
U32		NX_ADC_GetPrescalerValue( void );
void	NX_ADC_SetPrescalerEnable( CBOOL enable );
CBOOL	NX_ADC_GetPrescalerEnable( void );
void	NX_ADC_SetInputChannel( U32 channel );
U32		NX_ADC_GetInputChannel( void );
void	NX_ADC_SetStandbyMode( CBOOL enable );
CBOOL	NX_ADC_GetStandbyMode( void );
void	NX_ADC_Start( void );
CBOOL	NX_ADC_IsBusy( void );
U32		NX_ADC_GetConvertedData( void );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_ADC_H__
