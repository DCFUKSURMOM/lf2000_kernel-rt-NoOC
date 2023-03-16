//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: AUDIO
//	File		: nx_audio.h
//	Description	:
//	Author		:
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_AUDIO_H__
#define __NX_AUDIO_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	AUDIO AUDIO
//------------------------------------------------------------------------------
//@{

	/// @brief	AUDIO Module's Register List
	struct	NX_AUDIO_RegisterSet
	{
		volatile U16	AC97_CTRL;						///< 0x00 : AC97 Control Register
		volatile U16	AC97_CONFIG;					///< 0x02 : AC97 Configuration Register
		volatile U16	I2S_CTRL;						///< 0x04 : I2S	Control Register
		volatile U16	I2S_CONFIG;						///< 0x06 : I2S	Configuration Register
		volatile U16	AUDIO_BUFF_CTRL;				///< 0x08 : Audio Buffer Control Register
		volatile U16	AUDIO_BUFF_CONFIG;				///< 0x0A : Audio Buffer Configuration Register
		volatile U16	AUDIO_IRQ_ENA;					///< 0x0C : Audio Interrupt Enable Register
		volatile U16	AUDIO_IRQ_PEND;					///< 0x0E : Audio Interrupt Pending Register
		volatile U16	AC97_CODEC_ADDR;				///< 0x10 : AC97 Codec Address Register
		volatile U16	AC97_CODEC_WDATA;				///< 0x12 : AC97 Codec Write Data Register
		volatile U16	AC97_CODEC_RDATA;				///< 0x14 : AC97 Codec Read Data Register
		volatile U16	AUDIO_STATUS0;					///< 0x16 : Audio Status0 Register
		volatile U16	AUDIO_STATUS1;					///< 0x18 : Audio Status1 Register
		volatile U16	RESERVED_0[51];					///< 0x1A ~ 0x7E : Reserve
		volatile U16	AC97_CODEC_REG[64];				///< 0x80 ~ 0xFF : Codec Register Map
		volatile U16	PCMOUTDATA;						///< 0x100 : pcm out data register
		volatile U16	PCMOUT_STATUS;					///< 0x102 : pcm out data status register
		volatile U32	__Reserved00[(0x3C0-0x104)/4];	///< 0x100 ~ 0x3BF : Reserved Region
		volatile U32	CLKENB;							///< 0x3C0 : Clock Enable Register
		volatile U32	CLKGEN[2][2];					///< 0x3C4 : Clock Generater Register 0
														///< 0x3C8 : Reserved
														///< 0x3CC : Clock Generater Register 1
														///< 0x3D0 : Reserved
	};

	/// @brief	AUDIO interrupt for interrupt interface
	enum NX_AUDIO_INT
	{
		NX_AUDIO_INT_POUDR		=	0,		///< PCM OUT buffer underflow interrupt.
		NX_AUDIO_INT_SPUDER		=	1,		///< SPDIF OUT buffer underflow interrupt.
		NX_AUDIO_INT_PIOVER		=	2,		///< PCM IN buffer overflow interrupt.
		NX_AUDIO_INT_MICOVER	=	3,		///< MIC IN buffer overflow interrupt.
		NX_AUDIO_INT_ADC1OVER	=	4,		///< ADC1 IN buffer overflow interrupt.
		NX_AUDIO_INT_ADC2OVER	=	5,		///< ADC2 IN buffer overflow interrupt.
		NX_AUDIO_INT_RDDONE		=	6		///< AC97 Codec read done interrupt.
	};

	/// @brief AC97 Controller's State
	typedef enum
	{
		NX_AUDIO_AC97STATE_IDLE			=	0x01,		///< All registers excepts AC97_CTRL are reset value.
		NX_AUDIO_AC97STATE_INITIALIZE	=	0x02,		///< Host processor can access controller register.
		NX_AUDIO_AC97STATE_ACTIVE		=	0x04,		///< Host processor can access controller and codec registers.
		NX_AUDIO_AC97STATE_POWERDOWN	=	0x08,		///< AC-link is powerdown state.
		NX_AUDIO_AC97STATE_WARMRESET	=	0x10		///< AC-link wake up process using warm reset is in progress.

	}	NX_AUDIO_AC97STATE;

	/// @brief	I2S Operation Mode
	typedef enum
	{
		NX_AUDIO_IF_I2S		=	0,	///< I2S Mode
		NX_AUDIO_IF_LEFT	=	2,	///< Left-Justified Mode.
		NX_AUDIO_IF_RIGHT	=	3	///< Right-Justified Mode.

	}	NX_AUDIO_IF ;

	/// @brief I2S Controller's State
	typedef enum
	{
		NX_AUDIO_I2SSTATE_IDLE		=	0x01,		///< All registers excepts I2S_CTRL are reset value.
		NX_AUDIO_I2SSTATE_READY		=	0x02,		///< I2S link is inactive, ready to communicate with codec.
		NX_AUDIO_I2SSTATE_RUN		=	0x04		///< I2S link is active.

	}NX_AUDIO_I2SSTATE;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_AUDIO_Initialize( void );
U32		NX_AUDIO_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_AUDIO_GetPhysicalAddress( void );
U32		NX_AUDIO_GetSizeOfRegisterSet( void );
void	NX_AUDIO_SetBaseAddress( U32 BaseAddress );
U32		NX_AUDIO_GetBaseAddress( void );
CBOOL	NX_AUDIO_OpenModule( void );
CBOOL	NX_AUDIO_CloseModule( void );
CBOOL	NX_AUDIO_CheckBusy( void );
CBOOL	NX_AUDIO_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_AUDIO_GetInterruptNumber( void );

void	NX_AUDIO_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_AUDIO_GetInterruptEnable( S32 IntNum );
CBOOL	NX_AUDIO_GetInterruptPending( S32 IntNum );
void	NX_AUDIO_ClearInterruptPending( S32 IntNum );

void	NX_AUDIO_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_AUDIO_GetInterruptEnableAll( void );
CBOOL	NX_AUDIO_GetInterruptPendingAll( void );
void	NX_AUDIO_ClearInterruptPendingAll( void );

void	NX_AUDIO_SetInterruptEnable32( U32 EnableFlag );
U32		NX_AUDIO_GetInterruptEnable32( void );
U32		NX_AUDIO_GetInterruptPending32( void );
void	NX_AUDIO_ClearInterruptPending32( U32 PendingFlag );

S32		NX_AUDIO_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	DMA Interface
//@{
U32		NX_AUDIO_GetDMAIndex_PCMOut( void );
U32		NX_AUDIO_GetDMAIndex_SPDIFOut( void );
U32		NX_AUDIO_GetDMAIndex_PCMIn( void );
U32		NX_AUDIO_GetDMAIndex_MICIn( void );
U32		NX_AUDIO_GetDMAIndex_ADC1In( void );
U32		NX_AUDIO_GetDMAIndex_ADC2In( void );
U32		NX_AUDIO_GetDMABusWidth( void );
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_AUDIO_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_AUDIO_GetClockPClkMode( void );
void		NX_AUDIO_SetClockSource( U32 Index, U32 ClkSrc );
U32			NX_AUDIO_GetClockSource( U32 Index );
void		NX_AUDIO_SetClockDivisor( U32 Index, U32 Divisor );
U32			NX_AUDIO_GetClockDivisor( U32 Index );
void		NX_AUDIO_SetClockOutInv( U32 Index, CBOOL OutClkInv );
CBOOL		NX_AUDIO_GetClockOutInv( U32 Index );
void		NX_AUDIO_SetClockOutEnb( U32 Index, CBOOL OutClkEnb );
CBOOL		NX_AUDIO_GetClockOutEnb( U32 Index );
void		NX_AUDIO_SetClockDivisorEnable( CBOOL Enable );
CBOOL		NX_AUDIO_GetClockDivisorEnable( void );
//@}

//------------------------------------------------------------------------------
/// @name	Audio Buffer Configure Function
//------------------------------------------------------------------------------
//@{
void	NX_AUDIO_SetPCMOUTDataWidth( U32 DataWidth );
U32		NX_AUDIO_GetPCMOUTDataWidth( void );
void	NX_AUDIO_SetSPDIFOUTDataWidth( U32 DataWidth );
U32		NX_AUDIO_GetSPDIFOUTDataWidth( void );
void	NX_AUDIO_SetPCMINDataWidth( U32 DataWidth );
U32		NX_AUDIO_GetPCMINDataWidth( void );
//@}

//--------------------------------------------------------------------------
/// @name Audio Buffer Control Function
//--------------------------------------------------------------------------
//@{
void	NX_AUDIO_SetAudioBufferPCMOUTEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAudioBufferPCMOUTEnable( void );
void	NX_AUDIO_SetAudioBufferSPDIFOUTEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAudioBufferSPDIFOUTEnable( void );
void	NX_AUDIO_SetAudioBufferPCMINEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAudioBufferPCMINEnable( void );
void	NX_AUDIO_SetAudioBufferMICINEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAudioBufferMICINEnable( void );
void	NX_AUDIO_SetAudioBufferADC1Enable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAudioBufferADC1Enable( void );
void	NX_AUDIO_SetAudioBufferADC2Enable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAudioBufferADC2Enable( void );
void	NX_AUDIO_SetAudioBufferOUTEnable( CBOOL pcm, CBOOL spdif );
void	NX_AUDIO_GetAudioBufferOUTEnable( CBOOL* pcm, CBOOL* spdif );
void	NX_AUDIO_SetAudioBufferINEnable( CBOOL pcm, CBOOL mic, CBOOL adc1, CBOOL adc2 );
void	NX_AUDIO_GetAudioBufferINEnable( CBOOL* pcm, CBOOL* mic, CBOOL* adc1, CBOOL* adc2 );
//@}

//------------------------------------------------------------------------------
/// @name	Check of Audio's Buffer State
//------------------------------------------------------------------------------
//@{
CBOOL	NX_AUDIO_IsADC2InBufferReady( void );
CBOOL	NX_AUDIO_IsADC1InBufferReady( void );
CBOOL	NX_AUDIO_IsMICInBufferReady( void );
CBOOL	NX_AUDIO_IsPCMInBufferReady( void );
CBOOL	NX_AUDIO_IsSPDIFOutBufferReady( void );
CBOOL	NX_AUDIO_IsPCMOutBufferReady( void );
//@}

//------------------------------------------------------------------------------
/// @name AC97 Configuration Function
//------------------------------------------------------------------------------
//@{
void	NX_AUDIO_SetAC97WarmReset( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97WarmReset( void );
void	NX_AUDIO_SetAC97ControllerReset( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97ControllerReset( void );
void	NX_AUDIO_SetAC97ColdReset( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97ColdReset( void );
void	NX_AUDIO_SetAC97LinkOn( void );
CBOOL	NX_AUDIO_GetAC97LinkOn( void );
//@}

//------------------------------------------------------------------------------
/// @name AC97 Control Function
//------------------------------------------------------------------------------
//@{
void	NX_AUDIO_SetAC97FrontOutEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97FrontOutEnable( void );
void	NX_AUDIO_SetAC97RearOutEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97RearOutEnable( void );
void	NX_AUDIO_SetAC97CenterOutEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97CenterOutEnable( void );
void	NX_AUDIO_SetAC97SPDIFOutEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97SPDIFOutEnable( void );
void	NX_AUDIO_SetAC97PCMInEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97PCMInEnable( void );
void	NX_AUDIO_SetAC97MICInEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97MICInEnable( void );
void	NX_AUDIO_SetAC97ADC1InEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97ADC1InEnable( void );
void	NX_AUDIO_SetAC97ADC2InEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetAC97ADC2InEnable( void );
void	NX_AUDIO_SetOutputEnable( CBOOL front, CBOOL rear, CBOOL center, CBOOL spdif );
void	NX_AUDIO_GetOutputEnable( CBOOL* front, CBOOL* rear, CBOOL* center, CBOOL* spdif );
void	NX_AUDIO_SetInputEnable( CBOOL pcm, CBOOL mic, CBOOL adc1, CBOOL adc2 );
void	NX_AUDIO_GetInputEnable( CBOOL* pcm, CBOOL* mic, CBOOL* adc1, CBOOL* adc2 );
//@}

//------------------------------------------------------------------------------
/// @name	AC97 State Function
//------------------------------------------------------------------------------
//@{
NX_AUDIO_AC97STATE	NX_AUDIO_GetAC97State( void );
//@}

//------------------------------------------------------------------------------
///	@name AC97 Codec Control Operation
//------------------------------------------------------------------------------
//@{
U32		NX_AUDIO_GetAC97CodecAddr( void );
U32		NX_AUDIO_GetAC97CodecWData (void);
U32		NX_AUDIO_GetAC97CodecRData (void);
void	NX_AUDIO_WriteAC97CodecReg( U32 offset, U32 value );
U32		NX_AUDIO_ReadAC97CodecReg( U32 offset );
CBOOL	NX_AUDIO_WriteAC97CodecRegTime (U32 offset, U32 writeValue, U32 timeOut);
U32		NX_AUDIO_ReadAC97CodecRegTime (U32 offset, U32 timeOut);
//@}

//------------------------------------------------------------------------------
/// @name	Check of AC97 Codec's State
//------------------------------------------------------------------------------
//@{
CBOOL	NX_AUDIO_IsAC97CodecBusy( void );
CBOOL	NX_AUDIO_IsAC97CodecReadDone( void );
CBOOL	NX_AUDIO_IsAC97CodecWriteDone( void );
CBOOL	NX_AUDIO_IsAC97CodecReady( void );
//@}

//------------------------------------------------------------------------------
/// @name I2S Configuration Function
//------------------------------------------------------------------------------
//@{
void			NX_AUDIO_SetI2SMasterMode( CBOOL Enable );
CBOOL			NX_AUDIO_GetI2SMasterMode( void );
void			NX_AUDIO_SetInterfaceMode( NX_AUDIO_IF mode );
NX_AUDIO_IF		NX_AUDIO_GetInterfaceMode( void );
void			NX_AUDIO_SetSyncPeriod( U32 period );
U32				NX_AUDIO_GetSyncPeriod( void );
//@}

//------------------------------------------------------------------------------
/// @name I2S Control Function
//------------------------------------------------------------------------------
//@{
void	NX_AUDIO_SetI2SLinkOn( void );
CBOOL	NX_AUDIO_GetI2SLinkOn( void );
void	NX_AUDIO_SetI2SControllerReset( CBOOL Enable );
CBOOL	NX_AUDIO_GetI2SControllerReset( void );
void	NX_AUDIO_SetI2SOutputEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetI2SOutputEnable( void );
void	NX_AUDIO_SetI2SInputEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetI2SInputEnable( void );
void	NX_AUDIO_SetI2SLoopBackEnable( CBOOL Enable );
CBOOL	NX_AUDIO_GetI2SLoopBackEnable( void );
//@}

//------------------------------------------------------------------------------
/// @name	I2S State Function
//------------------------------------------------------------------------------
//@{
NX_AUDIO_I2SSTATE	NX_AUDIO_GetI2SState( void );
//@}

//------------------------------------------------------------------------------
/// @name	I2S Direct Write Function
//------------------------------------------------------------------------------
U32 * NX_AUDIO_I2S_GetDirectPCMOUTDataRegPtr( void );
void NX_AUDIO_I2S_DirectPCMOUTDataWrite( U16 Data );
U32 NX_AUDIO_I2S_DirectPCMOUTDataArrayWrite( U32 Size, U16* pData);
CBOOL NX_AUDIO_I2S_IsOutFifoFull( void );
CBOOL NX_AUDIO_I2S_IsOutFifoEmpty( void );

//@}

#ifdef	__cplusplus
}
#endif

#endif // __NX_AUDIO_H__
