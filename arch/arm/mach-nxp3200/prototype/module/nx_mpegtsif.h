//------------------------------------------------------------------------------
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: MPEG-TS Interface
//	File		: nx_mpegtsif.h
//	Description	:
//	Author		: Goofy
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_MPEGTSIF_H__
#define __NX_MPEGTSIF_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
/// @defgroup	MPEGTSIF MPEG-TS Interface
//------------------------------------------------------------------------------
//@{

	//------------------------------------------------------------------------------
	// To remove following waring on RVDS compiler
	//		Warning : #66-D: enumeration value is out of "int" range
	//------------------------------------------------------------------------------
	#ifdef __arm
	#pragma diag_remark 66		// disable #66 warining
	#endif

	//------------------------------------------------------------------------------
	/// @brief MPEG TS Interface Module's Register Set
	struct	NX_MPEGTSIF_RegisterSet
	{
		volatile U32	CTRL;						// 0x000 : Control
		volatile U8		__Reserved04[0x040-0x004];	// 0x004 ~ 0x03F : Reserved
		volatile U32	CLKENB;						// 0x040 : Clock Enable for CLKGEN module
	};

	/// @brief MPEG TS interrupt for interrupt interface
	enum
	{
		NX_MPEGTSIF_TRANSFERDONE = 0		///< Transfer Done Interrupt.
	};

	//------------------------------------------------------------------------------
	/// @brief The clock polarity to fetch data.
	typedef enum
	{
		NX_MPEGTSIF_CLOCKPOL_FALLING	= 0,	/// Fetchs data on the falling edge.
		NX_MPEGTSIF_CLOCKPOL_RISING		= 1		/// Fetchs data on the rising edge.
	} NX_MPEGTSIF_CLOCKPOL;

	/// @brief	The data polarity of external DP signal.
	typedef enum
	{
		NX_MPEGTSIF_DATAPOL_HIGH	= 0,	/// Data is high and Parity is low on the external DP signal.
		NX_MPEGTSIF_DATAPOL_LOW		= 1		/// Data is low and Parity is high on the external DP signal.
	} NX_MPEGTSIF_DATAPOL;

	/// @brief	The SYNC source selection.
	typedef enum
	{
		NX_MPEGTSIF_SYNC_EXTERNAL		= 0,	/// Use external SYNC signal.
		NX_MPEGTSIF_SYNC_INTERNAL		= 1		/// No external SYNC signal.
	} NX_MPEGTSIF_SYNC;

	/// @brief	The SYNC Mode selection.
	typedef enum
	{
		NX_MPEGTSIF_SYNCMODE_BIT		= 0,	/// Bit Sync Mode.
		NX_MPEGTSIF_SYNCMODE_BYTE		= 1		/// Byte Sync Mode.
	} NX_MPEGTSIF_SYNCMODE;


//------------------------------------------------------------------------------
/// @name	Module Interface
//------------------------------------------------------------------------------
//@{
CBOOL	NX_MPEGTSIF_Initialize( void );
U32		NX_MPEGTSIF_GetNumberOfModule( void );
//@}


//------------------------------------------------------------------------------
///	@name	Basic Interface
//------------------------------------------------------------------------------
//@{
U32		NX_MPEGTSIF_GetPhysicalAddress( void );
U32		NX_MPEGTSIF_GetSizeOfRegisterSet( void );

void	NX_MPEGTSIF_SetBaseAddress( U32 BaseAddress );
U32		NX_MPEGTSIF_GetBaseAddress( void );

CBOOL	NX_MPEGTSIF_OpenModule( void );
CBOOL	NX_MPEGTSIF_CloseModule( void );

CBOOL	NX_MPEGTSIF_CheckBusy( void );
CBOOL	NX_MPEGTSIF_CanPowerDown( void );
//@}


//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//------------------------------------------------------------------------------
//@{
S32		NX_MPEGTSIF_GetInterruptNumber( void );

void	NX_MPEGTSIF_SetInterruptEnable( S32 IntNum, CBOOL bEnb );
CBOOL	NX_MPEGTSIF_GetInterruptEnable( S32 IntNum );
CBOOL	NX_MPEGTSIF_GetInterruptPending( S32 IntNum );
void	NX_MPEGTSIF_ClearInterruptPending( S32 IntNum );

void	NX_MPEGTSIF_SetInterruptEnableAll( CBOOL bEnb );
CBOOL	NX_MPEGTSIF_GetInterruptEnableAll( void );
CBOOL	NX_MPEGTSIF_GetInterruptPendingAll( void );
void	NX_MPEGTSIF_ClearInterruptPendingAll( void );

void	NX_MPEGTSIF_SetInterruptEnable32( U32 EnableFlag );
U32		NX_MPEGTSIF_GetInterruptEnable32( void );
U32		NX_MPEGTSIF_GetInterruptPending32( void );
void	NX_MPEGTSIF_ClearInterruptPending32( U32 PendingFlag );

S32		NX_MPEGTSIF_GetInterruptPendingNumber( void );
//@}


//------------------------------------------------------------------------------
///	@name	DMA Interface
//------------------------------------------------------------------------------
//@{
U32		NX_MPEGTSIF_GetDMAIndex( void );
U32		NX_MPEGTSIF_GetDMABusWidth( void );
//@}


//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//------------------------------------------------------------------------------
//@{
void		NX_MPEGTSIF_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_MPEGTSIF_GetClockPClkMode( void );
//@}


//------------------------------------------------------------------------------
///	@name	Module customized operations
//------------------------------------------------------------------------------
//@{
void					NX_MPEGTSIF_SetClockPolarity( NX_MPEGTSIF_CLOCKPOL pol );
NX_MPEGTSIF_CLOCKPOL	NX_MPEGTSIF_GetClockPolarity( void );

void				NX_MPEGTSIF_SetDataPolarity( NX_MPEGTSIF_DATAPOL pol );
NX_MPEGTSIF_DATAPOL	NX_MPEGTSIF_GetDataPolarity( void );

void					NX_MPEGTSIF_SetSyncSource( NX_MPEGTSIF_SYNC sync );
NX_MPEGTSIF_SYNC		NX_MPEGTSIF_GetSyncSource( void );

void					NX_MPEGTSIF_SetSyncMode( NX_MPEGTSIF_SYNCMODE syncmode );
NX_MPEGTSIF_SYNCMODE	NX_MPEGTSIF_GetSyncMode( void );

void	NX_MPEGTSIF_SetWordCount( U32 WordCnt );
U32		NX_MPEGTSIF_GetWordCount( void );

void	NX_MPEGTSIF_SetEnable( CBOOL bEnb );
CBOOL	NX_MPEGTSIF_GetEnable( void );

CBOOL	NX_MPEGTSIF_GetErrorFlag( void );
//@}


//------------------------------------------------------------------------------
#ifdef __arm	// for RVDS
#pragma diag_default 66		// return to default setting for #66 warning
#endif

//@}
// End of MPEGTSIF module
//------------------------------------------------------------------------------

#ifdef	__cplusplus
}
#endif

#endif // __NX_MPEGTSIF_H__
