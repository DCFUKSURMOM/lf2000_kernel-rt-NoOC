//------------------------------------------------------------------------------
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: Alive
//	File		: nx_alive.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_ALIVE_H__
#define	__NX_ALIVE_H__

#include "../nx_base/nx_prototype.h"

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	ALIVE
//------------------------------------------------------------------------------
//@{
	struct NX_ALIVE_ScratchReg
	{
		volatile U32 ALIVESCRATCHRSTREG;					///< Alive Scratch Reset Register
		volatile U32 ALIVESCRATCHSETREG;					///< Alive Scratch Set Register
		volatile U32 ALIVESCRATCHREADREG;					///< Alive Scratch Read Register
	};

	/// @brief ALIVE Module's Register List
	struct NX_ALIVE_RegisterSet
	{
		volatile U32 ALIVEPWRGATEREG;						///< 0x000 : Alive Power Gating Register

		volatile U32 ALIVEGPIOASYNCDETECTMODERSTREG0;		///< 0x004 : Alive GPIO ASync Detect Mode Reset Register0
		volatile U32 ALIVEGPIOASYNCDETECTMODESETREG0;		///< 0x008 : Alive GPIO ASync Detect Mode Set Register0
		volatile U32 ALIVEGPIOLOWASYNCDETECTMODEREADREG;	///< 0x00C : Alive GPIO Low ASync Detect Mode Read Register

		volatile U32 ALIVEGPIOASYNCDETECTMODERSTREG1;		///< 0x010 : Alive GPIO ASync Detect Mode Reset Register1
		volatile U32 ALIVEGPIOASYNCDETECTMODESETREG1;		///< 0x014 : Alive GPIO ASync Detect Mode Set Register1
		volatile U32 ALIVEGPIOHIGHASYNCDETECTMODEREADREG;	///< 0x018 : Alive GPIO High ASync Detect Mode Read Register

		volatile U32 ALIVEGPIODETECTMODERSTREG0;			///< 0x01C : Alive GPIO Detect Mode Reset Register0
		volatile U32 ALIVEGPIODETECTMODESETREG0;			///< 0x020 : Alive GPIO Detect Mode Reset Register0
		volatile U32 ALIVEGPIOFALLDETECTMODEREADREG;		///< 0x024 : Alive GPIO Falling Edge Detect Mode Read Register

		volatile U32 ALIVEGPIODETECTMODERSTREG1;			///< 0x028 : Alive GPIO Detect Mode Reset Register1
		volatile U32 ALIVEGPIODETECTMODESETREG1;			///< 0x02C : Alive GPIO Detect Mode Reset Register1
		volatile U32 ALIVEGPIORISEDETECTMODEREADREG;		///< 0x030 : Alive GPIO Rising Edge Detect Mode Read Register

		volatile U32 ALIVEGPIODETECTMODERSTREG2;			///< 0x034 : Alive GPIO Detect Mode Reset Register2
		volatile U32 ALIVEGPIODETECTMODESETREG2;			///< 0x038 : Alive GPIO Detect Mode Reset Register2
		volatile U32 ALIVEGPIOLOWDETECTMODEREADREG;			///< 0x03C : Alive GPIO Low Level Detect Mode Read Register

		volatile U32 ALIVEGPIODETECTMODERSTREG3;			///< 0x040 : Alive GPIO Detect Mode Reset Register3
		volatile U32 ALIVEGPIODETECTMODESETREG3;			///< 0x044 : Alive GPIO Detect Mode Reset Register3
		volatile U32 ALIVEGPIOHIGHDETECTMODEREADREG;		///< 0x048 : Alive GPIO High Level Detect Mode Read Register

		volatile U32 ALIVEGPIODETECTENBRSTREG;				///< 0x04C : Alive GPIO Detect Enable Reset Register
		volatile U32 ALIVEGPIODETECTENBSETREG;				///< 0x050 : Alive GPIO Detect Enable Set Register
		volatile U32 ALIVEGPIODETECTENBREADREG;				///< 0x054 : Alive GPIO Detect Enable Read Register

		volatile U32 ALIVEGPIOINTENBRSTREG;					///< 0x058 : Alive GPIO Interrupt Enable Reset Register
		volatile U32 ALIVEGPIOINTENBSETREG;					///< 0x05C : Alive GPIO Interrupt Enable Set Register
		volatile U32 ALIVEGPIOINTENBREADREG;				///< 0x060 : Alive GPIO Interrupt Enable Read Register

		volatile U32 ALIVEGPIODETECTPENDREG;				///< 0x064 : Alive GPIO Detect Pending Register

//		volatile U32 ALIVESCRATCHRSTREG;					///< 0x068 : Alive Scratch Reset Register
//		volatile U32 ALIVESCRATCHSETREG;					///< 0x06C : Alive Scratch Set Register
//		volatile U32 ALIVESCRATCHREADREG;					///< 0x070 : Alive Scratch Read Register
		struct NX_ALIVE_ScratchReg ALIVESCRATCHREG;			///< 0x068 ~ 0x070

		volatile U32 ALIVEGPIOPADOUTENBRSTREG;				///< 0x074 : Alive GPIO PAD Out Enable Reset Register
		volatile U32 ALIVEGPIOPADOUTENBSETREG;				///< 0x078 : Alive GPIO PAD Out Enable Set Register
		volatile U32 ALIVEGPIOPADOUTENBREADREG;				///< 0x07C : Alive GPIO PAD Out Enable Read Register

		volatile U32 ALIVEGPIOPADPULLUPRSTREG;				///< 0x080 : Alive GPIO PAD Pullup Reset Register
		volatile U32 ALIVEGPIOPADPULLUPSETREG;				///< 0x084 : Alive GPIO PAD Pullup Set Register
		volatile U32 ALIVEGPIOPADPULLUPREADREG;				///< 0x088 : Alive GPIO PAD Pullup Read Register

		volatile U32 ALIVEGPIOPADOUTRSTREG;					///< 0x08C : Alive GPIO PAD Out Reset Register
		volatile U32 ALIVEGPIOPADOUTSETREG;					///< 0x090 : Alive GPIO PAD Out Set Register
		volatile U32 ALIVEGPIOPADOUTREADREG;				///< 0x094 :	Alive GPIO PAD Out Read Register

		volatile U32 VDDCTRLRSTREG;							///< 0x098 : VDD Control Reset Register
		volatile U32 VDDCTRLSETREG;							///< 0x09C : VDD Control Set Register
		volatile U32 VDDCTRLREADREG;						///< 0x0A0 : VDD Control Read Register

		volatile U32 ALIVECLEARWAKEUPSTATUS;				///< 0x0A4 : clear wakeup status
		volatile U32 ALIVESLEEPWAKEUPSTATUS;				///< 0x0A8 : Alive Sleep Wakeup Status

//		volatile U32 ALIVESCRATCHRSTREG1;					///< 0x0AC : Alive Scratch Reset Register 1
//		volatile U32 ALIVESCRATCHSETREG1;					///< 0x0B0 : Alive Scratch Set Register 1
//		volatile U32 ALIVESCRATCHREADREG1;					///< 0x0B4 : Alive Scratch Read Register 1

//		volatile U32 ALIVESCRATCHRSTREG2;					///< 0x0B8 : Alive Scratch Reset Register 2
//		volatile U32 ALIVESCRATCHSETREG2;					///< 0x0BC : Alive Scratch Set Register 2
//		volatile U32 ALIVESCRATCHREADREG2;					///< 0x0C0 : Alive Scratch Read Register 2

//		volatile U32 ALIVESCRATCHRSTREG3;					///< 0x0C4 : Alive Scratch Reset Register 3
//		volatile U32 ALIVESCRATCHSETREG3;					///< 0x0C8 : Alive Scratch Set Register 3
//		volatile U32 ALIVESCRATCHREADREG3;					///< 0x0CC : Alive Scratch Read Register 3

//		volatile U32 ALIVESCRATCHRSTREG4;					///< 0x0D0 : Alive Scratch Reset Register 4
//		volatile U32 ALIVESCRATCHSETREG4;					///< 0x0D4 : Alive Scratch Set Register 4
//		volatile U32 ALIVESCRATCHREADREG4;					///< 0x0D8 : Alive Scratch Read Register 4

//		volatile U32 ALIVESCRATCHRSTREG5;					///< 0x0DC : Alive Scratch Reset Register 5
//		volatile U32 ALIVESCRATCHSETREG5;					///< 0x0E0 : Alive Scratch Set Register 5
//		volatile U32 ALIVESCRATCHREADREG5;					///< 0x0E4 : Alive Scratch Read Register 5

//		volatile U32 ALIVESCRATCHRSTREG6;					///< 0x0E8 : Alive Scratch Reset Register 6
//		volatile U32 ALIVESCRATCHSETREG6;					///< 0x0EC : Alive Scratch Set Register 6
//		volatile U32 ALIVESCRATCHREADREG6;					///< 0x0F0 : Alive Scratch Read Register 6

//		volatile U32 ALIVESCRATCHRSTREG7;					///< 0x0F4 : Alive Scratch Reset Register 7
//		volatile U32 ALIVESCRATCHSETREG7;					///< 0x0F8 : Alive Scratch Set Register 7
//		volatile U32 ALIVESCRATCHREADREG7;					///< 0x0FC : Alive Scratch Read Register 7

//		volatile U32 ALIVESCRATCHRSTREG8;					///< 0x100 : Alive Scratch Reset Register 8
//		volatile U32 ALIVESCRATCHSETREG8;					///< 0x104 : Alive Scratch Set Register 8
//		volatile U32 ALIVESCRATCHREADREG8;					///< 0x108 : Alive Scratch Read Register 8

		struct NX_ALIVE_ScratchReg ALIVESCRATCHAREG[8];			///< 0x0AC ~ 0x108 Alive Scratch Register Set Array

		volatile U32 VDDOFFDELAYRSTREG;						///< 0x10C : Alive VDD Off Delay Value Reset Register
		volatile U32 VDDOFFDELAYSETREG;						///< 0x110 : Alive VDD Off Delay Value Set Register
		volatile U32 VDDOFFDELAYREADREG;					///< 0x114 : Alive VDD Off Delay Value Read Register

		volatile U32 VDDOFFDELAYTIMEREG;					///< 0x118 : Alive VDD Off Delay Current Count Register
	};

	///@brief	ALIVE Interrupts for interrupt interface
	enum
	{
		NX_ALIVE_INT_ALIVEGPIO0	= 0,	///<	ALIVE GPIO 0 Interrupt
		NX_ALIVE_INT_ALIVEGPIO1	= 1,	///<	ALIVE GPIO 1 Interrupt
		NX_ALIVE_INT_ALIVEGPIO2	= 2,	///<	ALIVE GPIO 2 Interrupt
		NX_ALIVE_INT_ALIVEGPIO3	= 3,	///<	ALIVE GPIO 3 Interrupt
		NX_ALIVE_INT_ALIVEGPIO4	= 4,	///<	ALIVE GPIO 4 Interrupt
		NX_ALIVE_INT_ALIVEGPIO5	= 5,	///<	ALIVE GPIO 5 Interrupt
		NX_ALIVE_INT_ALIVEGPIO6	= 6,	///<	ALIVE GPIO 6 Interrupt
		NX_ALIVE_INT_ALIVEGPIO7	= 7		///<	ALIVE GPIO 7 Interrupt
	};

	///@brief	ALIVE GPIO Detect Mode
	typedef enum
	{
		NX_ALIVE_DETECTMODE_ASYNC_LOWLEVEL		= 0,	///< Async Low Level Detect Mode
		NX_ALIVE_DETECTMODE_ASYNC_HIGHLEVEL		= 1,	///< Async High Level Detect Mode
		NX_ALIVE_DETECTMODE_SYNC_FALLINGEDGE	= 2,	///< Sync Falling Edge Detect Mode
		NX_ALIVE_DETECTMODE_SYNC_RISINGEDGE		= 3,	///< Sync Rising Edge Detect Mode
		NX_ALIVE_DETECTMODE_SYNC_LOWLEVEL		= 4,	///< Sync Low Level Detect Mode
		NX_ALIVE_DETECTMODE_SYNC_HIGHLEVEL		= 5		///< Sync High Level Detect Mode

	} NX_ALIVE_DETECTMODE;

	/// @brief	Retention PAD Group
	typedef enum
	{
		NX_ALIVE_PADGROUP0 = 0,		///< IO Power Group 0 ( RX0 ~ RX4 )
		NX_ALIVE_PADGROUP1 = 1,		///< IO Power Group 1 ( USB VBUS )
		NX_ALIVE_PADGROUP2 = 2		///< IO Power Group 2 ( GPIO )

	} NX_ALIVE_PADGROUP;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_ALIVE_Initialize( void );
U32		NX_ALIVE_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_ALIVE_GetPhysicalAddress( void );
U32		NX_ALIVE_GetSizeOfRegisterSet( void );
void	NX_ALIVE_SetBaseAddress( U32 BaseAddress );
U32		NX_ALIVE_GetBaseAddress( void );
CBOOL	NX_ALIVE_OpenModule( void );
CBOOL	NX_ALIVE_CloseModule( void );
CBOOL	NX_ALIVE_CheckBusy( void );
CBOOL	NX_ALIVE_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_ALIVE_GetInterruptNumber( void );

void	NX_ALIVE_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_ALIVE_GetInterruptEnable( S32 IntNum );
CBOOL	NX_ALIVE_GetInterruptPending( S32 IntNum );
void	NX_ALIVE_ClearInterruptPending( S32 IntNum );

void	NX_ALIVE_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_ALIVE_GetInterruptEnableAll( void );
CBOOL	NX_ALIVE_GetInterruptPendingAll( void );
void	NX_ALIVE_ClearInterruptPendingAll( void );

void	NX_ALIVE_SetInterruptEnable32( U32 EnableFlag );
U32		NX_ALIVE_GetInterruptEnable32( void );
U32		NX_ALIVE_GetInterruptPending32( void );
void	NX_ALIVE_ClearInterruptPending32( U32 PendingFlag );

S32		NX_ALIVE_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
/// @name	PAD Configuration
//@{
void	NX_ALIVE_SetWriteEnable( CBOOL bEnable );
CBOOL	NX_ALIVE_GetWriteEnable( void );
void	NX_ALIVE_SetScratchReg( U32 dwData );
U32		NX_ALIVE_GetScratchReg( void );
void	NX_ALIVE_SetScratchArrayOneReg( U32 index, U32 dwData );
U32		NX_ALIVE_GetScratchArrayOneReg( U32 index );
void	NX_ALIVE_SetScratchArrayReg( U32 *pdwInData );
void	NX_ALIVE_GetScratchArrayReg( U32 *pdwOutData );
void	NX_ALIVE_SetPullUpEnable( U32 BitNumber, CBOOL bEnable );
CBOOL	NX_ALIVE_GetPullUpEnable( U32 BitNumber );
void	NX_ALIVE_SetPadRetentionEnable( NX_ALIVE_PADGROUP Group, CBOOL bEnable );
CBOOL	NX_ALIVE_GetPadRetentionEnable( NX_ALIVE_PADGROUP Group );
void	NX_ALIVE_SetPadRetentionHold( NX_ALIVE_PADGROUP Group, CBOOL bEnable );
CBOOL	NX_ALIVE_GetPadRetentionHold( NX_ALIVE_PADGROUP Group );
//@}

//------------------------------------------------------------------------------
/// @name	Input Setting Function
//@{
void	NX_ALIVE_SetDetectEnable( U32 BitNumber, CBOOL bEnable );
CBOOL	NX_ALIVE_GetDetectEnable( U32 BitNumber );
void	NX_ALIVE_SetDetectMode( NX_ALIVE_DETECTMODE DetectMode, U32 BitNumber, CBOOL bEnable );
CBOOL	NX_ALIVE_GetDetectMode( NX_ALIVE_DETECTMODE DetectMode, U32 BitNumber );
CBOOL	NX_ALIVE_GetVDDPWRTOGGLE( void );
//@}

//------------------------------------------------------------------------------
/// @name	Output Setting Function
//@{
void	NX_ALIVE_SetOutputEnable( U32 BitNumber, CBOOL OutputEnb );
CBOOL	NX_ALIVE_GetOutputEnable( U32 BitNumber );
void	NX_ALIVE_SetOutputValue( U32 BitNumber, CBOOL Value );
CBOOL	NX_ALIVE_GetOutputValue( U32 BitNumber );
void	NX_ALIVE_SetVDDPWRON( CBOOL bVDDPWRON, CBOOL bVDDPWRON_DDR );
CBOOL	NX_ALIVE_GetVDDPWRON( void );
CBOOL	NX_ALIVE_GetVDDPWRON_DDR( void );

void	NX_ALIVE_SetVDDPWRONDelay( U32 Delay );
U32		NX_ALIVE_GetVDDPWRONDelay( void );
U32		NX_ALIVE_GetVDDPWRONDelayCurrentCount( void );
//@}

//@}


#ifdef __cplusplus
}
#endif


#endif	// __NX_ALIVE_H__
