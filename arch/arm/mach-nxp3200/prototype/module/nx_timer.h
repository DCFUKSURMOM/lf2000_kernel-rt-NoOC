//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: TIMER
//	File		: nx_timer.h
//	Description	:
//	Author		: Goofy
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_TIMER_H__
#define __NX_TIMER_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	TIMER TIMER
//------------------------------------------------------------------------------
//@{

	//--------------------------------------------------------------------------
	/// @brief	TIMER Module's Register List
	struct	NX_TIMER_RegisterSet
	{
		volatile U32 TMRCOUNT;			///< 0x00 : Timer counter register
		volatile U32 TMRMATCH;			///< 0x04 : Timer match register
		volatile U32 TMRCONTROL;		///< 0x08 : Timer control register
		volatile U32 _RESERVED[0x0D];	///< 0x0C ~ 0x3C : Reserved region
		volatile U32 TMRCLKENB;			///< 0x40 : Timer clock generation enable register
		volatile U32 TMRCLKGEN;			///< 0x44 : Timer clock generation control register
	};

	/// @brief TIMER interrupt for interrupt interface
	enum
	{
		NX_TIMER_INT_COUNTER = 0		///< TIMER Counter interrupt.
	};

	/// @brief	timer clock source
	typedef enum 
	{
		NX_TIMER_CLOCK_TCLK		= 3,	///< TCLK = Source clock
		NX_TIMER_CLOCK_TCLK2	= 0,	///< TCLK = Source clock / 2
		NX_TIMER_CLOCK_TCLK4	= 1,	///< TCLK = Source clock / 4
		NX_TIMER_CLOCK_TCLK8	= 2		///< TCLK = Source clock / 8

	} NX_TIMER_CLOCK;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_TIMER_Initialize( void );
U32		NX_TIMER_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_TIMER_GetPhysicalAddress( U32 ModuleIndex );
U32		NX_TIMER_GetSizeOfRegisterSet( void );
void	NX_TIMER_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress );
U32		NX_TIMER_GetBaseAddress( U32 ModuleIndex );
CBOOL	NX_TIMER_OpenModule( U32 ModuleIndex );
CBOOL	NX_TIMER_CloseModule( U32 ModuleIndex );
CBOOL	NX_TIMER_CheckBusy( U32 ModuleIndex );
CBOOL	NX_TIMER_CanPowerDown( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_TIMER_GetInterruptNumber( U32 ModuleIndex );

void	NX_TIMER_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable );
CBOOL	NX_TIMER_GetInterruptEnable( U32 ModuleIndex, S32 IntNum );
CBOOL	NX_TIMER_GetInterruptPending( U32 ModuleIndex, S32 IntNum );
void	NX_TIMER_ClearInterruptPending( U32 ModuleIndex, S32 IntNum );

void	NX_TIMER_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable );
CBOOL	NX_TIMER_GetInterruptEnableAll( U32 ModuleIndex );
CBOOL	NX_TIMER_GetInterruptPendingAll( U32 ModuleIndex );
void	NX_TIMER_ClearInterruptPendingAll( U32 ModuleIndex );

void	NX_TIMER_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag );
U32		NX_TIMER_GetInterruptEnable32( U32 ModuleIndex );
U32		NX_TIMER_GetInterruptPending32( U32 ModuleIndex );
void	NX_TIMER_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag );

S32		NX_TIMER_GetInterruptPendingNumber( U32 ModuleIndex );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_TIMER_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode );
NX_PCLKMODE	NX_TIMER_GetClockPClkMode( U32 ModuleIndex );
void		NX_TIMER_SetClockDivisorEnable( U32 ModuleIndex, CBOOL Enable );
CBOOL		NX_TIMER_GetClockDivisorEnable( U32 ModuleIndex );

void		NX_TIMER_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc );
U32			NX_TIMER_GetClockSource( U32 ModuleIndex, U32 Index );
void		NX_TIMER_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divisor );
U32			NX_TIMER_GetClockDivisor( U32 ModuleIndex, U32 Index );
//@}

//------------------------------------------------------------------------------
///	@name	TIMER Operation.
//@{
void	NX_TIMER_Run( U32 ModuleIndex );
void	NX_TIMER_Stop( U32 ModuleIndex );
void	NX_TIMER_SetWatchDogEnable( U32 ModuleIndex, CBOOL Enable );

void	NX_TIMER_SetTClkDivider( U32 ModuleIndex, NX_TIMER_CLOCK Clock );

void	NX_TIMER_SetMatchCounter( U32 ModuleIndex, U32 MatchCounter );
U32		NX_TIMER_GetMatchCounter( U32 ModuleIndex );
void	NX_TIMER_SetTimerCounter( U32 ModuleIndex, U32 TimerCounter );
U32		NX_TIMER_GetTimerCounter( U32 ModuleIndex );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_TIMER_H__
