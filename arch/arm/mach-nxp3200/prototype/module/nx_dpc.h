//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: DPC
//	File		: nx_dpc.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_DPC_H__
#define __NX_DPC_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	DPC DPC
//------------------------------------------------------------------------------
//@{

	/// @brief	DPC Module's Register List
	struct	NX_DPC_RegisterSet
	{
		volatile U16 __Reserved00	;			///< 0x000
		volatile U16 VENCCTRLA		;			///< 0x002 : Video Encoder Control A Register
		volatile U16 VENCCTRLB		;			///< 0x004 : Video Encoder Control B Register
		volatile U16 __Reserved01	;			///< 0x006
		volatile U16 VENCSCH		;			///< 0x008 : Video Encoder SCH Phase Control Register
		volatile U16 VENCHUE		;			///< 0x00A : Video Encoder HUE Phase Control Register
		volatile U16 VENCSAT		;			///< 0x00C : Video Encoder Chroma Saturation Control Register
		volatile U16 VENCCRT		;			///< 0x00E : Video Encoder Contrast Control Register
		volatile U16 VENCBRT    	;			///< 0x010 : Video Encoder Bright Control Register
		volatile U16 VENCFSCADJH	;			///< 0x012 : Video Encoder Color Burst Frequency Adjustment High Register
		volatile U16 VENCFSCADJL	;			///< 0x014 : Video Encoder Color Burst Frequency Adjustment Low Register
		volatile U16 __Reserved02[5];			///< 0x016 ~ 0x01F
		volatile U16 VENCDACSEL		;			///< 0x020 : Video Encoder DAC Output Select Register
		volatile U16 __Reserved03[15];			///< 0x022 ~ 0x03F
		volatile U16 VENCICNTL		;			///< 0x040 : Video Encoder Timing Configuration Register
		volatile U16 __Reserved04[3];			///< 0x042 ~ 0x047
		volatile U16 VENCHSVSO		;			///< 0x048 : Video Encoder Horizontal & Vertical Sync Register
		volatile U16 VENCHSOS		;			///< 0x04A : Video Encoder Horizontal Sync Start Register
		volatile U16 VENCHSOE		;			///< 0x04C : Video Encoder Horizontal Sync End Register
		volatile U16 VENCVSOS		;			///< 0x04E : Video Encoder Vertical Sync Start Register
		volatile U16 VENCVSOE		;			///< 0x050 : Video Encoder Vertical Sync End Register
		volatile U16 __Reserved05[21];			///< 0x052 ~ 0x07B

		volatile U16 DPCHTOTAL		;			///< 0x07C : DPC Horizontal Total Length Register
		volatile U16 DPCHSWIDTH		;			///< 0x07E : DPC Horizontal Sync Width Register
		volatile U16 DPCHASTART		;			///< 0x080 : DPC Horizontal Active Video Start Register
		volatile U16 DPCHAEND		;			///< 0x082 : DPC Horizontal Active Video End Register
		volatile U16 DPCVTOTAL		;			///< 0x084 : DPC Vertical Total Length Register
		volatile U16 DPCVSWIDTH		;			///< 0x086 : DPC Vertical Sync Width Register
		volatile U16 DPCVASTART		;			///< 0x088 : DPC Vertical Active Video Start Register
		volatile U16 DPCVAEND		;			///< 0x08A : DPC Vertical Active Video End Register
		volatile U16 DPCCTRL0		;			///< 0x08C : DPC Control 0 Register
		volatile U16 DPCCTRL1		;			///< 0x08E : DPC Control 1 Register
		volatile U16 DPCEVTOTAL		;			///< 0x090 : DPC Even Field Vertical Total Length Register
		volatile U16 DPCEVSWIDTH	;			///< 0x092 : DPC Even Field Vertical Sync Width Register
		volatile U16 DPCEVASTART	;			///< 0x094 : DPC Even Field Vertical Active Video Start Register
		volatile U16 DPCEVAEND		;			///< 0x096 : DPC Even Field Vertical Active Video End Register
		volatile U16 DPCCTRL2		;			///< 0x098 : DPC Control 2 Register
		volatile U16 DPCVSEOFFSET	;			///< 0x09A : DPC Vertical Sync End Offset Register
		volatile U16 DPCVSSOFFSET	;			///< 0x09C : DPC Vertical Sync Start Offset Register
		volatile U16 DPCEVSEOFFSET	;			///< 0x09E : DPC Even Field Vertical Sync End Offset Register
		volatile U16 DPCEVSSOFFSET	;			///< 0x0A0 : DPC Even Field Vertical Sync Start Offset Register
		volatile U16 DPCDELAY0		;			///< 0x0A2 : DPC Delay 0 Register
		volatile U16 DPCUPSCALECON0 ;			///< 0x0A4 : DPC Sync Upscale Control Register 0
		volatile U16 DPCUPSCALECON1 ;			///< 0x0A6 : DPC Sync Upscale Control Register 1
		volatile U16 DPCUPSCALECON2 ;			///< 0x0A8 : DPC Sync Upscale Control Register 2
		volatile U16 __Reserved06[9];			///< 0x0AA ~ 0x0BA
		volatile U16 DPCDELAY1		;			///< 0x0BC : DPC Delay 1 Register
		volatile U16 __Reserved07[(0x0E8 - 0x0BC)/2];		///< 0x0BE ~ 0x0E8
		volatile U16 DPCDIRTYFLAG	;			///< 0x0EA : DPC Dirty Flag Register
		volatile U16 __Reserved08[(0x1BE - 0x0EA)/2];		///< 0x0EC ~ 0x1BE
		volatile U32 DPCCLKENB		;			///< 0x1C0 : DPC Clock Generation Enable Register
		volatile U32 DPCCLKGEN[2][2];			///< 0x1C4 : DPC Clock Generation Control 0 Low Register
												///< 0x1C8 : DPC Clock Generation Control 0 High Register
												///< 0x1CC : DPC Clock Generation Control 1 Low Register
												///< 0x1D0 : DPC Clock Generation Control 1 High Register
	};

	/// @brief	DPC interrupt for interrupt interface
	enum
	{
		NX_DPC_INT_VSYNC = 0		///< DPC VSYNC interrupt
	};

	/// @brief	the data output format.
	typedef	enum
	{
		NX_DPC_FORMAT_RGB555		= 0UL,	///< RGB555 Format
		NX_DPC_FORMAT_RGB565		= 1UL,	///< RGB565 Format
		NX_DPC_FORMAT_RGB666		= 2UL,	///< RGB666 Format
		NX_DPC_FORMAT_RGB888		= 3UL,	///< RGB888 Format
		NX_DPC_FORMAT_MRGB555A		= 4UL,	///< MRGB555A Format
		NX_DPC_FORMAT_MRGB555B		= 5UL,	///< MRGB555B Format
		NX_DPC_FORMAT_MRGB565		= 6UL,	///< MRGB565 Format
		NX_DPC_FORMAT_MRGB666		= 7UL,	///< MRGB666 Format
		NX_DPC_FORMAT_MRGB888A		= 8UL,	///< MRGB888A Format
		NX_DPC_FORMAT_MRGB888B		= 9UL,	///< MRGB888B Format
		NX_DPC_FORMAT_CCIR656		= 10UL,	///< ITU-R BT.656 / 601(8-bit)
		NX_DPC_FORMAT_CCIR601A		= 12UL,	///< ITU-R BT.601A
		NX_DPC_FORMAT_CCIR601B		= 13UL,	///< ITU-R BT.601B
		NX_DPC_FORMAT_SRGB888		= 14UL,	///< SRGB888 Format
		NX_DPC_FORMAT_SRGBD8888		= 15UL,	///< SRGBD8888 Format
		NX_DPC_FORMAT_4096COLOR		= 1UL,	///< 4096 Color Format
		NX_DPC_FORMAT_16GRAY		= 3UL	///< 16 Level Gray Format

	}	NX_DPC_FORMAT;

	/// @brief	the data output order in case of ITU-R BT.656 / 601.
	typedef enum
	{
		NX_DPC_YCORDER_CbYCrY		= 0UL,	///< Cb, Y, Cr, Y
		NX_DPC_YCORDER_CrYCbY		= 1UL,	///< Cr, Y, Cb, Y
		NX_DPC_YCORDER_YCbYCr		= 2UL,	///< Y, Cb, Y, Cr
		NX_DPC_YCORDER_YCrYCb		= 3UL	///< Y, Cr, Y, Cb

	}	NX_DPC_YCORDER;

	///	@brief the PAD output clock.
	typedef enum
	{
		NX_DPC_PADCLK_VCLK		= 0UL,	///< VCLK
		NX_DPC_PADCLK_VCLK2		= 1UL,	///< VCLK2
		NX_DPC_PADCLK_VCLK3		= 2UL	///< VCLK3

	}	NX_DPC_PADCLK;

		/// @brief	RGB dithering mode.
	typedef	enum
	{
		NX_DPC_DITHER_BYPASS	= 0UL,	///< Bypass mode.
		NX_DPC_DITHER_4BIT		= 1UL,	///< 8 bit -> 4 bit mode.
		NX_DPC_DITHER_5BIT		= 2UL,	///< 8 bit -> 5 bit mode.
		NX_DPC_DITHER_6BIT		= 3UL	///< 8 bit -> 6 bit mode.

	}	NX_DPC_DITHER;

	///	@brief	Video Broadcast Standards.
	typedef enum
	{
		NX_DPC_VBS_NTSC_M		= 0UL,	///< NTSC-M				59.94 Hz(525)
		NX_DPC_VBS_NTSC_N		= 1UL,	///< NTSC-N				50 Hz(625)
		NX_DPC_VBS_NTSC_443		= 2UL,	///< NTSC-4.43			60 Hz(525)
		NX_DPC_VBS_PAL_M		= 3UL,	///< PAL-M				59.952 Hz(525)
		NX_DPC_VBS_PAL_N		= 4UL,	///< PAL-combination N	50 Hz(625)
		NX_DPC_VBS_PAL_BGHI		= 5UL,	///< PAL-B,D,G,H,I,N	50 Hz(625)
		NX_DPC_VBS_PSEUDO_PAL	= 6UL,	///< Pseudo PAL			60 Hz(525)
		NX_DPC_VBS_PSEUDO_NTSC	= 7UL	///< Pseudo NTSC		50 Hz (625)

	}	NX_DPC_VBS;

	///	@brief Luma/Chroma bandwidth control
	typedef enum
	{
		NX_DPC_BANDWIDTH_LOW		= 0UL,	///< Low bandwidth
		NX_DPC_BANDWIDTH_MEDIUM		= 1UL,	///< Medium bandwidth
		NX_DPC_BANDWIDTH_HIGH		= 2UL	///< High bandwidth

	}	NX_DPC_BANDWIDTH;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_DPC_Initialize( void );
U32		NX_DPC_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_DPC_GetPhysicalAddress( U32 ModuleIndex );
U32		NX_DPC_GetSizeOfRegisterSet( void );
void	NX_DPC_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress );
U32		NX_DPC_GetBaseAddress( U32 ModuleIndex );
CBOOL	NX_DPC_OpenModule( U32 ModuleIndex );
CBOOL	NX_DPC_CloseModule( U32 ModuleIndex );
CBOOL	NX_DPC_CheckBusy( U32 ModuleIndex );
CBOOL	NX_DPC_CanPowerDown( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_DPC_GetInterruptNumber( U32 ModuleIndex );

void	NX_DPC_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable );
CBOOL	NX_DPC_GetInterruptEnable( U32 ModuleIndex, S32 IntNum );
CBOOL	NX_DPC_GetInterruptPending( U32 ModuleIndex, S32 IntNum );
void	NX_DPC_ClearInterruptPending( U32 ModuleIndex, S32 IntNum );

void	NX_DPC_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable );
CBOOL	NX_DPC_GetInterruptEnableAll( U32 ModuleIndex );
CBOOL	NX_DPC_GetInterruptPendingAll( U32 ModuleIndex );
void	NX_DPC_ClearInterruptPendingAll( U32 ModuleIndex );

void	NX_DPC_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag );
U32		NX_DPC_GetInterruptEnable32( U32 ModuleIndex );
U32		NX_DPC_GetInterruptPending32( U32 ModuleIndex );
void	NX_DPC_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag );

S32		NX_DPC_GetInterruptPendingNumber( U32 ModuleIndex );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_DPC_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode );
NX_PCLKMODE	NX_DPC_GetClockPClkMode( U32 ModuleIndex );
void		NX_DPC_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc );
U32			NX_DPC_GetClockSource( U32 ModuleIndex, U32 Index );
void		NX_DPC_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divisor );
U32			NX_DPC_GetClockDivisor( U32 ModuleIndex, U32 Index );
void		NX_DPC_SetClockOutInv( U32 ModuleIndex, U32 Index, CBOOL OutClkInv );
CBOOL		NX_DPC_GetClockOutInv( U32 ModuleIndex, U32 Index );
void		NX_DPC_SetClockOutSelect( U32 ModuleIndex, U32 Index, CBOOL bBypass );
CBOOL		NX_DPC_GetClockOutSelect( U32 ModuleIndex, U32 Index );
void		NX_DPC_SetClockPolarity( U32 ModuleIndex, CBOOL bPolarity );
CBOOL		NX_DPC_GetClockPolarity( U32 ModuleIndex );
void		NX_DPC_SetClockOutEnb( U32 ModuleIndex, U32 Index, CBOOL OutClkEnb );
CBOOL		NX_DPC_GetClockOutEnb( U32 ModuleIndex, U32 Index );
void		NX_DPC_SetClockOutDelay( U32 ModuleIndex, U32 Index, U32 delay );
U32			NX_DPC_GetClockOutDelay( U32 ModuleIndex, U32 Index );
void		NX_DPC_SetClockDivisorEnable( U32 ModuleIndex, CBOOL Enable );
CBOOL		NX_DPC_GetClockDivisorEnable( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
///	@name	Display Controller(TFT) Operations
//--------------------------------------------------------------------------
//@{
void	NX_DPC_SetDPCEnable( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_DPC_GetDPCEnable( U32 ModuleIndex );

void	NX_DPC_SetDelay( U32 ModuleIndex, U32 DelayRGB_PVD, U32 DelayHS_CP1, U32 DelayVS_FRAM, U32 DelayDE_CP2 );
void	NX_DPC_GetDelay( U32 ModuleIndex, U32 *pDelayRGB_PVD, U32 *pDelayHS_CP1, U32 *pDelayVS_FRAM, U32 *pDelayDE_CP2 );

void	NX_DPC_SetDither( U32 ModuleIndex, NX_DPC_DITHER DitherR, NX_DPC_DITHER DitherG, NX_DPC_DITHER DitherB );
void	NX_DPC_GetDither( U32 ModuleIndex, NX_DPC_DITHER *pDitherR, NX_DPC_DITHER *pDitherG, NX_DPC_DITHER *pDitherB );

void	NX_DPC_SetHorizontalUpScaler( U32 ModuleIndex, CBOOL bEnb, U32 sourceWidth, U32 destWidth );
void	NX_DPC_GetHorizontalUpScaler( U32 ModuleIndex, CBOOL* pbEnb, U32* psourceWidth, U32* pdestWidth );
//@}

//--------------------------------------------------------------------------
/// @name TFT LCD Specific Control Function
//--------------------------------------------------------------------------
//@{
void	NX_DPC_SetMode( U32 ModuleIndex,
							NX_DPC_FORMAT format,
							CBOOL bInterlace, CBOOL bInvertField,
							CBOOL bRGBMode, CBOOL bSwapRB,
							NX_DPC_YCORDER ycorder, CBOOL bClipYC, CBOOL bEmbeddedSync,
							NX_DPC_PADCLK clock, CBOOL bInvertClock, CBOOL bDualView );

void	NX_DPC_GetMode( U32 ModuleIndex,
							NX_DPC_FORMAT *pFormat,
							CBOOL *pbInterlace, CBOOL *pbInvertField,
							CBOOL *pbRGBMode, CBOOL *pbSwapRB,
							NX_DPC_YCORDER *pYCorder, CBOOL *pbClipYC, CBOOL *pbEmbeddedSync,
							NX_DPC_PADCLK *pClock, CBOOL *pbInvertClock, CBOOL *pbDualView );

void	NX_DPC_SetHSync( U32 ModuleIndex, U32 AVWidth, U32 HSW, U32 HFP, U32 HBP, CBOOL bInvHSYNC );
void	NX_DPC_GetHSync( U32 ModuleIndex, U32 *pAVWidth, U32 *pHSW, U32 *pHFP, U32 *pHBP, CBOOL *pbInvHSYNC );

void	NX_DPC_SetVSync( U32 ModuleIndex, U32 AVHeight, U32 VSW, U32 VFP, U32 VBP, CBOOL bInvVSYNC,
							U32 EAVHeight, U32 EVSW, U32 EVFP, U32 EVBP );

void	NX_DPC_GetVSync( U32 ModuleIndex, U32 *pAVHeight, U32 *pVSW, U32 *pVFP, U32 *pVBP, CBOOL *pbInvVSYNC,
							U32 *pEAVHeight, U32 *pEVSW, U32 *pEVFP, U32 *pEVBP );

void	NX_DPC_SetVSyncOffset( U32 ModuleIndex, U32 VSSOffset, U32 VSEOffset, U32 EVSSOffset, U32 EVSEOffset );
void	NX_DPC_GetVSyncOffset( U32 ModuleIndex, U32 *pVSSOffset, U32 *pVSEOffset, U32 *pEVSSOffset, U32 *pEVSEOffset );
//@}

//--------------------------------------------------------------------------
///	@name	Internal Video encoder operations
//--------------------------------------------------------------------------
//@{
void    NX_DPC_SetSecondaryDPCSync( U32 ModuleIndex, CBOOL bEnb );
CBOOL   NX_DPC_GetSecondaryDPCSync( U32 ModuleIndex );

void	NX_DPC_SetENCEnable( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_DPC_GetENCEnable( U32 ModuleIndex );
void	NX_DPC_SetVideoEncoderPowerDown( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_DPC_GetVideoEncoderPowerDown( U32 ModuleIndex );

void	NX_DPC_SetVideoEncoderMode( U32 ModuleIndex, NX_DPC_VBS vbs, CBOOL bPedestal );
void	NX_DPC_SetVideoEncoderSCHLockControl( U32 ModuleIndex, CBOOL bFreeRun );
CBOOL	NX_DPC_GetVideoEncoderSCHLockControl( U32 ModuleIndex );

void	NX_DPC_SetVideoEncoderBandwidth( U32 ModuleIndex, NX_DPC_BANDWIDTH Luma, NX_DPC_BANDWIDTH Chroma );
void	NX_DPC_GetVideoEncoderBandwidth( U32 ModuleIndex, NX_DPC_BANDWIDTH *pLuma, NX_DPC_BANDWIDTH *pChroma );
void	NX_DPC_SetVideoEncoderColorControl( U32 ModuleIndex, _S8 SCH, _S8 HUE, _S8 SAT, _S8 CRT, _S8 BRT );              
void	NX_DPC_GetVideoEncoderColorControl( U32 ModuleIndex, _S8 *pSCH, _S8 *pHUE, _S8 *pSAT, _S8 *pCRT, _S8 *pBRT );    
void	NX_DPC_SetVideoEncoderFSCAdjust( U32 ModuleIndex, S16 adjust );
U16		NX_DPC_GetVideoEncoderFSCAdjust( U32 ModuleIndex );
void	NX_DPC_SetVideoEncoderTiming( U32 ModuleIndex, U32 HSOS, U32 HSOE, U32 VSOS, U32 VSOE );
void	NX_DPC_GetVideoEncoderTiming( U32 ModuleIndex, U32 *pHSOS, U32 *pHSOE, U32 *pVSOS, U32 *pVSOE );
//--------------------------------------------------------------------------
/// @name LCD Specific Control Function
//--------------------------------------------------------------------------
void	NX_DPC_SetDirtyFlag( U32 ModuleIndex );
CBOOL	NX_DPC_GetDirtyFlag( U32 ModuleIndex );

//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_DPC_H__
