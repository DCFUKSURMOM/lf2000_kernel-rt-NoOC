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
//	Module		: ROTATOR
//	File		: nx_rotator.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_ROTATOR_H__
#define __NX_ROTATOR_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
///	@defgroup	ROTATOR ROTATOR
//------------------------------------------------------------------------------
//@{

	/// @brief	ROTATOR Module's Register List
	struct NX_ROTATOR_RegisterSet
	{
		volatile U32		RTRUNREG;						///< 0x00 : Rotator Configuration Register
		volatile U32		RTINTREG;						///< 0x04 : Rotator Interrupt Register
		volatile U32		RTDIRREG;						///< 0x08 : Rotator Configuration Register1
		volatile U32		RTSRCBASEREG;					///< 0x0C : Rotator Source Address Register
		volatile U32		RTDESTBASEREG;					///< 0x10 : Rotator Destination Register
		volatile U32		RTSIZEREG;						///< 0x14 : Rotator Size Register
		volatile U32		__Reserved00[(0x7C0-0x18)/4];	///< 0x18 ~ 0x7BC : Reserved region
		volatile U32		RTCLKENB;						///< 0x7C0 :
	};

	/// @brief ROTATOR interrupt for interrupt interface
	enum
	{
		NX_ROTATOR_INT_DONE = 0		///< Rotator done interrupt.
	};

	/// @brief Config Direction of Rotator
	typedef enum
	{
		NX_ROTATOR_DIR_90			=	0x00000001,		///< Rotate 90.
		NX_ROTATOR_DIR_270			=	0x00010000,		///< Rotate 270.
		NX_ROTATOR_DIR_90_HFLIP		=	0x00000000,		///< Rotate 90 and Horizontal Flip.
		NX_ROTATOR_DIR_270_HFLIP	=	0x00010001,		///< Rotate 270 and Horizontal Flip.

	}	NX_ROTATOR_DIR;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_ROTATOR_Initialize( void );
U32		NX_ROTATOR_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_ROTATOR_GetPhysicalAddress( void );
U32		NX_ROTATOR_GetSizeOfRegisterSet( void );
void	NX_ROTATOR_SetBaseAddress( U32 BaseAddress );
U32		NX_ROTATOR_GetBaseAddress( void );
CBOOL	NX_ROTATOR_OpenModule( void );
CBOOL	NX_ROTATOR_CloseModule( void );
CBOOL	NX_ROTATOR_CheckBusy( void );
CBOOL	NX_ROTATOR_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_ROTATOR_GetInterruptNumber( void );

void	NX_ROTATOR_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_ROTATOR_GetInterruptEnable( S32 IntNum );
CBOOL	NX_ROTATOR_GetInterruptPending( S32 IntNum );
void	NX_ROTATOR_ClearInterruptPending( S32 IntNum );

void	NX_ROTATOR_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_ROTATOR_GetInterruptEnableAll( void );
CBOOL	NX_ROTATOR_GetInterruptPendingAll( void );
void	NX_ROTATOR_ClearInterruptPendingAll( void );

void	NX_ROTATOR_SetInterruptEnable32( U32 EnableFlag );
U32		NX_ROTATOR_GetInterruptEnable32( void );
U32		NX_ROTATOR_GetInterruptPending32( void );
void	NX_ROTATOR_ClearInterruptPending32( U32 PendingFlag );

S32		NX_ROTATOR_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_ROTATOR_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_ROTATOR_GetClockPClkMode( void );
void		NX_ROTATOR_SetClockBClkMode( NX_BCLKMODE mode );
NX_BCLKMODE	NX_ROTATOR_GetClockBClkMode( void );
//@}

//--------------------------------------------------------------------------
/// @name Operation Function of Rotator
//@{
void	NX_ROTATOR_Run( void );
void	NX_ROTATOR_Stop( void );
CBOOL	NX_ROTATOR_IsBusy( void );
//@}

//--------------------------------------------------------------------------
/// @name Configuration Function of Rotator
//@{
void			NX_ROTATOR_SetDirection( NX_ROTATOR_DIR direction );
NX_ROTATOR_DIR	NX_ROTATOR_GetDirection( void );

void	NX_ROTATOR_SetSrcBase(U32 baseAddr);
U32		NX_ROTATOR_GetSrcBase( void );

void	NX_ROTATOR_SetDestBase(U32 baseAddr);
U32		NX_ROTATOR_GetDestBase( void );

void	NX_ROTATOR_SetImageSize( U32 width, U32 height );
void	NX_ROTATOR_GetImageSize( U32* width, U32* height );
//@}

//--------------------------------------------------------------------------
/// @name Util Function of Rotator
//@{
void	NX_ROTATOR_SetRotator( U32 src, U32 dest, U32 width, U32 height, NX_ROTATOR_DIR dir );
void	NX_ROTATOR_GetRotator( U32* src, U32* dest, U32* width, U32* height, NX_ROTATOR_DIR* dir );
//@}

//@}

#ifdef	__cplusplus
}
#endif

#endif // __NX_ROTATOR_H__
