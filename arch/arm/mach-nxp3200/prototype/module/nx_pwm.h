//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: PWM
//	File		: nx_pwm.h
//	Description	:
//	Author		:
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_PWM_H__
#define __NX_PWM_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	PWM PWM
//------------------------------------------------------------------------------
//@{

	/// @brief	PWM Module's Register List
	struct	NX_PWM_RegisterSet
	{
		struct {
			volatile U16	PWM_PREPOL;			///< 0x00		: Prescaler Register
			volatile U16	PWM_DUTY[2];		///< 0x02, 0x04 : Duty Cycle Register
			volatile U16	PWM_PERIOD[2];		///< 0x06, 0x08 : Period Cycle Register
			volatile U16	RESERVED0[3];		///< 0xA, 0xC, 0xE : Reserved Region
		} PWM2[2];

		volatile U32		RESERVED1[0x08];	///< 0x20~0x3C	: Reserved Region
		volatile U32		PWM_CLKENB;			///< 0x40		: Clock Enable Register
		volatile U32		PWM_CLKGEN;			///< 0x44		: Clock Generater Register
	};

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_PWM_Initialize( void );
U32		NX_PWM_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_PWM_GetPhysicalAddress( void );
U32		NX_PWM_GetSizeOfRegisterSet( void );
void	NX_PWM_SetBaseAddress( U32 BaseAddress );
U32		NX_PWM_GetBaseAddress( void );
CBOOL	NX_PWM_OpenModule( void );
CBOOL	NX_PWM_CloseModule( void );
CBOOL	NX_PWM_CheckBusy( void );
CBOOL	NX_PWM_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_PWM_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_PWM_GetClockPClkMode( void );
void		NX_PWM_SetClockDivisorEnable( CBOOL Enable );
CBOOL		NX_PWM_GetClockDivisorEnable( void );
void		NX_PWM_SetClockSource( U32 Index, U32 ClkSrc );
U32			NX_PWM_GetClockSource( U32 Index );
void		NX_PWM_SetClockDivisor( U32 Index, U32 Divisor );
U32			NX_PWM_GetClockDivisor( U32 Index );
//@}

//------------------------------------------------------------------------------
///	@name	PWM Operation.
//@{
void	NX_PWM_SetPreScale( U32 channel, U32 prescale );
U32		NX_PWM_GetPreScale( U32 channel );
void	NX_PWM_SetPolarity( U32 channel, CBOOL bypass );
CBOOL	NX_PWM_GetPolarity( U32 channel );
void	NX_PWM_SetPeriod( U32 channel, U32 period );
U32		NX_PWM_GetPeriod( U32 channel );
void	NX_PWM_SetDutyCycle( U32 channel, U32 duty );
U32		NX_PWM_GetDutyCycle( U32 channel );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_PWM_H__
