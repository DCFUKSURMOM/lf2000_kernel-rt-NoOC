//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: Clock & Power Manager
//	File		: nx_clkpwr.h
//	Description	:
//	Author		: Firmware
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_CLKPWR_H__
#define __NX_CLKPWR_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	CLKPWR Clock & Power Manager
//------------------------------------------------------------------------------
//@{

	/// @brief	Clock & Power Manager Module's Register List
	struct	NX_CLKPWR_RegisterSet
	{
		volatile U32 CLKMODEREG0;				///< 0x00 : Clock Mode Register 0
		volatile U32 CLKMODEREG1;				///< 0x04 : Clock Mode Register 1
		volatile U32 PLLSETREG[2];				///< 0x08 ~ 0x0C : PLL Setting Register
		volatile U8 __Reserved00[0x40-0x10];	///< 0x10 ~ 0x3C : Reserved Region
		volatile U32 GPIOWAKEUPRISEENB;			///< 0x40 : GPIO Rising Edge Detect Enable Register
		volatile U32 GPIOWAKEUPFALLENB;			///< 0x44 : GPIO Falling Edge Detect Enable Register
		volatile U32 GPIORSTENB;				///< 0x48 : GPIO Reset Enable Register
		volatile U32 GPIOWAKEUPENB;				///< 0x4C : GPIO Wakeup Source Enable
		volatile U32 GPIOINTENB;				///< 0x50 : Interrupt Enable Register
		volatile U32 GPIOINTPEND;				///< 0x54 : Interrupt Pend Register
		volatile U32 RESETSTATUS;				///< 0x58 : Reset Status Register
		volatile U32 INTENABLE;					///< 0x5C : Interrupt Enable Register
		volatile U32 INTPEND;					///< 0x60 : Interrupt Pend Register
		volatile U32 PWRCONT;					///< 0x64 : Power Control Register
		volatile U32 PWRMODE;					///< 0x68 : Power Mode Register
		volatile U32 __Reserved01;				///< 0x6C : Reserved Region
		volatile U32 SCRATCH[3];				///< 0x70 ~ 0x78	: Scratch Register
		volatile U32 SYSRSTCONFIG;				///< 0x7C : System Reset Configuration Register.
		volatile U8 __Reserved02[0x100-0x80];	///< 0x80 ~ 0xFC	: Reserved Region
		volatile U32 PADSTRENGTHGPIO[5][2];		///< 0x100, 0x104 : GPIOA Pad Strength Register
												///< 0x108, 0x10C : GPIOB Pad Strength Register
												///< 0x110, 0x114 : GPIOC Pad Strength Register
												///< 0x118, 0x11C : GPIOD Pad Strength Register
												///< 0x120, 0x124 : GPIOE Pad Strength Register
		volatile U32 __Reserved03[2];			///< 0x128 ~ 0x12C: Reserved Region
		volatile U32 PADSTRENGTHBUS;			///< 0x130		: Bus Pad Strength Register
	};

	///@brief	CLKPWR Interrupts for interrupt interfaces
	enum
	{
		NX_CLKPWR_INT_ALIVEGPIO0		= 0,	///< ALIVE GPIO 0 interrupt
		NX_CLKPWR_INT_ALIVEGPIO1		= 1,	///< ALIVE GPIO 1 interrupt
		NX_CLKPWR_INT_ALIVEGPIO2		= 2,	///< ALIVE GPIO 2 interrupt
		NX_CLKPWR_INT_ALIVEGPIO3		= 3,	///< ALIVE GPIO 3 interrupt
		NX_CLKPWR_INT_ALIVEGPIO4		= 4,	///< ALIVE GPIO 4 interrupt
		NX_CLKPWR_INT_ALIVEGPIO5		= 5,	///< ALIVE GPIO 5 interrupt
		NX_CLKPWR_INT_ALIVEGPIO6		= 6,	///< ALIVE GPIO 6 interrupt
		NX_CLKPWR_INT_ALIVEGPIO7		= 7,	///< ALIVE GPIO 7 interrupt
		NX_CLKPWR_INT_VDDTOGGLE			= 8,	///< VDD Power Toggle interrupt
		NX_CLKPWR_INT_RTC				= 9,	///< RTC interrupt
		NX_CLKPWR_INT_BATF				= 10	///< Battery fault event interrupt
	};

	/// @brief	Reset status
	typedef enum
	{
		NX_CLKPWR_RESETSTATUS_POWERON	= 1 << 0,	///< Power on reset
		NX_CLKPWR_RESETSTATUS_GPIO		= 1 << 1,	///< GPIO reset
		NX_CLKPWR_RESETSTATUS_WATCHDOG	= 1 << 2,	///< Watchdog Reset
		NX_CLKPWR_RESETSTATUS_SOFTWARE	= 1 << 3	///< Software reset

	}	NX_CLKPWR_RESETSTATUS;

	/// @brief	Power mode
	typedef enum
	{
		NX_CLKPWR_POWERMODE_STOP		= 1 << 5,	///< Stop mode
		NX_CLKPWR_POWERMODE_IDLE		= 1 << 4	///< Idle mode

	}	NX_CLKPWR_POWERMODE;

	/// @brief	Bus type
	typedef enum
	{
		NX_CLKPWR_BUSPAD_STATIC_CNTL	= 22UL,		///< Static Control Pad
		NX_CLKPWR_BUSPAD_STATIC_ADDR	= 20UL,		///< Static Address Pad
		NX_CLKPWR_BUSPAD_STATIC_DATA	= 18UL,		///< Static Data Pad
		NX_CLKPWR_BUSPAD_VSYNC			= 6UL,		///< Vertical Sync Pad
		NX_CLKPWR_BUSPAD_HSYNC			= 4UL,		///< Horizontal Sync Pad
		NX_CLKPWR_BUSPAD_DE				= 2UL,		///< Data Enable Pad

	}	NX_CLKPWR_BUSPAD;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_CLKPWR_Initialize( void );
U32		NX_CLKPWR_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_CLKPWR_GetPhysicalAddress( void );
U32		NX_CLKPWR_GetSizeOfRegisterSet( void );
void	NX_CLKPWR_SetBaseAddress( U32 BaseAddress );
U32		NX_CLKPWR_GetBaseAddress( void );
CBOOL	NX_CLKPWR_OpenModule( void );
CBOOL	NX_CLKPWR_CloseModule( void );
CBOOL	NX_CLKPWR_CheckBusy( void );
CBOOL	NX_CLKPWR_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_CLKPWR_GetInterruptNumber( void );

void	NX_CLKPWR_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_CLKPWR_GetInterruptEnable( S32 IntNum );
CBOOL	NX_CLKPWR_GetInterruptPending( S32 IntNum );
void	NX_CLKPWR_ClearInterruptPending( S32 IntNum );

void	NX_CLKPWR_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_CLKPWR_GetInterruptEnableAll( void );
CBOOL	NX_CLKPWR_GetInterruptPendingAll( void );
void	NX_CLKPWR_ClearInterruptPendingAll( void );

void	NX_CLKPWR_SetInterruptEnable32( U32 EnableFlag );
U32		NX_CLKPWR_GetInterruptEnable32( void );
U32		NX_CLKPWR_GetInterruptPending32( void );
void	NX_CLKPWR_ClearInterruptPending32( U32 PendingFlag );

S32		NX_CLKPWR_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
/// @name	Clock Management
//@{
void	NX_CLKPWR_SetPLLPMS ( U32 pllnumber, U32 PDIV, U32 MDIV, U32 SDIV );
void	NX_CLKPWR_SetPLLPowerOn ( CBOOL On );
void	NX_CLKPWR_DoPLLChange( void );
CBOOL	NX_CLKPWR_IsPLLStable( void );
void	NX_CLKPWR_SetClockCPU( U32 ClkSrc, U32 CoreDivider, U32 BusDivider );
void	NX_CLKPWR_SetClockMCLK( U32 ClkSrc, U32 MCLKDivider, U32 BCLKDivider, U32 PCLKDivider );
void	NX_CLKPWR_SetCPUBUSSyncMode(CBOOL Enable);
//@}

//------------------------------------------------------------------------------
/// @name	Wakeup Management
//@{
void	NX_CLKPWR_SetRTCWakeUpEnable ( CBOOL Enable );
CBOOL	NX_CLKPWR_GetRTCWakeUpEnable ( void );
void	NX_CLKPWR_SetALIVEGPIOWakeupEnable( U32 dwBitNumber, CBOOL bEnable );
CBOOL	NX_CLKPWR_GetALIVEGPIOWakeupEnable( U32 dwBitNumber );
void	NX_CLKPWR_SetALIVEGPIOWakeUpRiseEdgeDetectEnable( U32 dwBitNumber, CBOOL bEnable );
CBOOL	NX_CLKPWR_GetALIVEGPIOWakeUpRiseEdgeDetectEnable( U32 dwBitNumber );
void	NX_CLKPWR_SetALIVEGPIOWakeUpFallEdgeDetectEnable( U32 dwBitNumber, CBOOL bEnable );
CBOOL	NX_CLKPWR_GetALIVEGPIOWakeUpFallEdgeDetectEnable( U32 dwBitNumber );
//@}

//------------------------------------------------------------------------------
/// @name	Reset Management
//@{
void	NX_CLKPWR_SetSoftwareResetEnable( CBOOL bEnable );
CBOOL	NX_CLKPWR_GetSoftwareResetEnable( void );
void	NX_CLKPWR_DoSoftwareReset( void );
void	NX_CLKPWR_SetALIVEGPIOResetEnable( U32 dwBitNumber, CBOOL bEnable );
CBOOL	NX_CLKPWR_GetALIVEGPIOResetEnable( U32 dwBitNumber );
//@}

//------------------------------------------------------------------------------
/// @name	Check Last Reset Status
//@{
NX_CLKPWR_RESETSTATUS	NX_CLKPWR_GetResetStatus( void );
//@}

//------------------------------------------------------------------------------
/// @name	Power Management
//@{
void				NX_CLKPWR_GoStopMode( void );
void				NX_CLKPWR_GoIdleMode( void );
NX_CLKPWR_POWERMODE	NX_CLKPWR_GetLastPowerMode ( void );
//@}

//------------------------------------------------------------------------------
/// @name	Scratch Pad
//@{
void	NX_CLKPWR_SetScratchPad( U32 dwIndex, U32 dwValue );
U32		NX_CLKPWR_GetScratchPad( U32 dwIndex );
//@}

//------------------------------------------------------------------------------
/// @name	System Reset Configuration
//@{
U32		NX_CLKPWR_GetSystemResetConfiguration( void );
//@}

//------------------------------------------------------------------------------
/// @name	PAD Strength Management
//@{
void	NX_CLKPWR_SetGPIOPadStrength( U32 Group, U32 BitNumber, U32 mA );
U32		NX_CLKPWR_GetGPIOPadStrength( U32 Group, U32 BitNumber );
void	NX_CLKPWR_SetBusPadStrength( NX_CLKPWR_BUSPAD Bus, U32 mA );
U32		NX_CLKPWR_GetBusPadStrength( NX_CLKPWR_BUSPAD Bus );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_CLKPWR_H__
