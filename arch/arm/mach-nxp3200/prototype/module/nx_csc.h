//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: CSC
//	File		: nx_csc.h
//	Description:
//	Author		: Firmware Team
//	History	:
//------------------------------------------------------------------------------
#ifndef __NX_CSC_H__
#define __NX_CSC_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	CSC CSC
//------------------------------------------------------------------------------
//@{

	/// @brief	CSC Module's Register List
	struct	NX_CSC_RegisterSet
	{
		volatile U32 CONTROL;			///< 0x00 [RW]		: Control Register
		volatile U32 SIZE	;			///< 0x04 [ W]		: Size Register
		volatile U32 ADDRY	;			///< 0x08 [ W]		: Address Y Channel Register
		volatile U32 ADDRCB ;			///< 0x0C [ W]		: Address Cb Channel Register
		volatile U32 ADDRCR ;			///< 0x10 [ W]		: Address Cr Channel Register
		volatile U32 removed;//volatile U32 ADDRTEX;			///< 0x14 [ W]		: Address Texture Register
		volatile U32 TPCTRL ;			///< 0x18 [ W]		: Transparency Control Register
		volatile U32 TPY	;			///< 0x1C [ W]		: Transparency Y Range Register
		volatile U32 TPCB	;			///< 0x20 [ W]		: Transparency Cb Range Register
		volatile U32 TPCR	;			///< 0x24 [ W]		: Transparency Cr Range Register
		volatile U32 STRIDETEX;			///< 0x28 [ W]		: Stride Texture Register ((Number of pixels between adjacent lines)<<16)
		volatile U32 ADDRTEX;			///< 0x2C [ W]		: Address Texture Register
		volatile U32 RESERVED[0x1E4];	///< 0x30 ~ 0x7BC	: Reserved Region
		volatile U32 CSCCLKENB;			///< 0x7C0			: Clock Enable Register
	};

	/// @brief	CSC interrupt for interrupt interface
	enum
	{
		NX_CSC_INT_DONE = 0	///< CSC done interrupt
	};

	/// @brief	Quantization mode
	typedef enum
	{
		NX_CSC_QMODE_220 = 0,	///< 220 quantization mode
		NX_CSC_QMODE_256 = 1	///< 256 quantization mode

	} NX_CSC_QMODE ;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_CSC_Initialize( void );
U32		NX_CSC_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_CSC_GetPhysicalAddress( void );
U32		NX_CSC_GetSizeOfRegisterSet( void );
void	NX_CSC_SetBaseAddress( U32 BaseAddress );
U32		NX_CSC_GetBaseAddress( void );
CBOOL	NX_CSC_OpenModule( void );
CBOOL	NX_CSC_CloseModule( void );
CBOOL	NX_CSC_CheckBusy( void );
CBOOL	NX_CSC_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_CSC_GetInterruptNumber( void );

void	NX_CSC_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_CSC_GetInterruptEnable( S32 IntNum );
CBOOL	NX_CSC_GetInterruptPending( S32 IntNum );
void	NX_CSC_ClearInterruptPending( S32 IntNum );

void	NX_CSC_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_CSC_GetInterruptEnableAll( void );
CBOOL	NX_CSC_GetInterruptPendingAll( void );
void	NX_CSC_ClearInterruptPendingAll( void );

void	NX_CSC_SetInterruptEnable32( U32 EnableFlag );
U32		NX_CSC_GetInterruptEnable32( void );
U32		NX_CSC_GetInterruptPending32( void );
void	NX_CSC_ClearInterruptPending32( U32 PendingFlag );

S32		NX_CSC_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{

void			NX_CSC_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_CSC_GetClockPClkMode( void );
void			NX_CSC_SetClockBClkMode( NX_BCLKMODE mode );
NX_BCLKMODE	NX_CSC_GetClockBClkMode( void );

//@}

//------------------------------------------------------------------------------
///	@name	CSC Operation.
//@{
void			NX_CSC_SetDitherEnable( CBOOL bEnb );
CBOOL			NX_CSC_GetDitherEnable( void );
void			NX_CSC_SetQuantizationMode( NX_CSC_QMODE mode );
NX_CSC_QMODE	NX_CSC_GetQuantizationMode( void );
void			NX_CSC_SetTransparency( U16 ReplaceColor, CBOOL bEnb,
										CBOOL bInvY,	U8 MinY,	U8 MaxY,
										CBOOL bInvCb,	U8 MinCb,	U8 MaxCb,
										CBOOL bInvCr,	U8 MinCr,	U8 MaxCr );
void	NX_CSC_SetSize( U32 width, U32 height );
void	NX_CSC_SetSourceAddr( U32 AddrY, U32 AddrCb, U32 AddrCr );
void	NX_CSC_SetTargetAddr( U32 AddrTex );
void	NX_CSC_Run( void );
CBOOL	NX_CSC_IsBusy( void );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_CSC_H__
