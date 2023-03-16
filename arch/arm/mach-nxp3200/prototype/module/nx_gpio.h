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
//	Module		: GPIO
//	File		: nx_gpio.h
//	Description:
//	Author		: Firmware Team
//	History	:
//------------------------------------------------------------------------------
#ifndef __NX_GPIO_H__
#define __NX_GPIO_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	GPIO GPIO
//------------------------------------------------------------------------------
//@{

	/// @brief	GPIO Module's Register List
	struct	NX_GPIO_RegisterSet
	{
		volatile U32 GPIOxOUT;			///< 0x00	: Output Register
		volatile U32 GPIOxOUTENB;		///< 0x04	: Output Enable Register
		volatile U32 GPIOxDETMODE[2];	///< 0x08	: Event Detect Mode Register
		volatile U32 GPIOxINTENB;		///< 0x10	: Interrupt Enable Register
		volatile U32 GPIOxDET;			///< 0x14	: Event Detect Register
		volatile U32 GPIOxPAD;			///< 0x18	: PAD Status Register
		volatile U32 GPIOxPUENB;		///< 0x1C	: Pull Up Enable Register
		volatile U32 GPIOxALTFN[2];		///< 0x20	: Alternate Function Select Register
		volatile U32 GPIOxDETMODEEX;	///< 0x28	: Event Detect Mode extended Register
		volatile U32 __Reserved[4];		///< 0x2C	:
		volatile U32 GPIOxDETENB;		///< 0x3C	: IntPend Detect Enable Register
	};

	///@brief	GPIO Interrupts for interrupt interface
	enum
	{
		NX_GPIO_INT_GPIO0	= 0,	///<	GPIO 0 Interrupt
		NX_GPIO_INT_GPIO1	= 1,	///<	GPIO 1 Interrupt
		NX_GPIO_INT_GPIO2	= 2,	///<	GPIO 2 Interrupt
		NX_GPIO_INT_GPIO3	= 3,	///<	GPIO 3 Interrupt
		NX_GPIO_INT_GPIO4	= 4,	///<	GPIO 4 Interrupt
		NX_GPIO_INT_GPIO5	= 5,	///<	GPIO 5 Interrupt
		NX_GPIO_INT_GPIO6	= 6,	///<	GPIO 6 Interrupt
		NX_GPIO_INT_GPIO7	= 7,	///<	GPIO 7 Interrupt
		NX_GPIO_INT_GPIO8	= 8,	///<	GPIO 8 Interrupt
		NX_GPIO_INT_GPIO9	= 9,	///<	GPIO 9 Interrupt
		NX_GPIO_INT_GPIO10	= 10,	///<	GPIO 10 Interrupt
		NX_GPIO_INT_GPIO11	= 11,	///<	GPIO 11 Interrupt
		NX_GPIO_INT_GPIO12	= 12,	///<	GPIO 12 Interrupt
		NX_GPIO_INT_GPIO13	= 13,	///<	GPIO 13 Interrupt
		NX_GPIO_INT_GPIO14	= 14,	///<	GPIO 14 Interrupt
		NX_GPIO_INT_GPIO15	= 15,	///<	GPIO 15 Interrupt
		NX_GPIO_INT_GPIO16	= 16,	///<	GPIO 16 Interrupt
		NX_GPIO_INT_GPIO17	= 17,	///<	GPIO 17 Interrupt
		NX_GPIO_INT_GPIO18	= 18,	///<	GPIO 18 Interrupt
		NX_GPIO_INT_GPIO19	= 19,	///<	GPIO 19 Interrupt
		NX_GPIO_INT_GPIO20	= 20,	///<	GPIO 20 Interrupt
		NX_GPIO_INT_GPIO21	= 21,	///<	GPIO 21 Interrupt
		NX_GPIO_INT_GPIO22	= 22,	///<	GPIO 22 Interrupt
		NX_GPIO_INT_GPIO23	= 23,	///<	GPIO 23 Interrupt
		NX_GPIO_INT_GPIO24	= 24,	///<	GPIO 24 Interrupt
		NX_GPIO_INT_GPIO25	= 25,	///<	GPIO 25 Interrupt
		NX_GPIO_INT_GPIO26	= 26,	///<	GPIO 26 Interrupt
		NX_GPIO_INT_GPIO27	= 27,	///<	GPIO 27 Interrupt
		NX_GPIO_INT_GPIO28	= 28,	///<	GPIO 28 Interrupt
		NX_GPIO_INT_GPIO29	= 29,	///<	GPIO 29 Interrupt
		NX_GPIO_INT_GPIO30	= 30,	///<	GPIO 30 Interrupt
		NX_GPIO_INT_GPIO31	= 31	///<	GPIO 31 Interrupt
	};

	/// @brief	GPIO interrupt mode
	typedef enum
	{
		NX_GPIO_INTMODE_LOWLEVEL	= 0UL,		///< Low level detect
		NX_GPIO_INTMODE_HIGHLEVEL	= 1UL,		///< High level detect
		NX_GPIO_INTMODE_FALLINGEDGE = 2UL,		///< Falling edge detect
		NX_GPIO_INTMODE_RISINGEDGE	= 3UL,		///< Rising edge detect
		NX_GPIO_INTMODE_RISINGNFALLINGEDGE	= 4UL		///< Rising & Falling edge detect
	}NX_GPIO_INTMODE;

	/// @brief	I/O mode
	typedef enum
	{
		NX_GPIO_PADFUNC_GPIO		= 0UL,			///< GPIO mode
		NX_GPIO_PADFUNC_1			= 1UL,			///< Alternate function 1
		NX_GPIO_PADFUNC_2			= 2UL,			///< Alternate function 2
		NX_GPIO_PADFUNC_3			= 3UL,			///< Alternate function 3

	}NX_GPIO_PADFUNC ;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_GPIO_Initialize( void );
U32		NX_GPIO_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_GPIO_GetPhysicalAddress( U32 ModuleIndex );
U32		NX_GPIO_GetSizeOfRegisterSet( void );
void	NX_GPIO_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress );
U32		NX_GPIO_GetBaseAddress( U32 ModuleIndex );
CBOOL	NX_GPIO_OpenModule( U32 ModuleIndex );
CBOOL	NX_GPIO_CloseModule( U32 ModuleIndex );
CBOOL	NX_GPIO_CheckBusy( U32 ModuleIndex );
CBOOL	NX_GPIO_CanPowerDown( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_GPIO_GetInterruptNumber( void );

void	NX_GPIO_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable );
CBOOL	NX_GPIO_GetInterruptEnable( U32 ModuleIndex, S32 IntNum );
CBOOL	NX_GPIO_GetInterruptPending( U32 ModuleIndex, S32 IntNum );
void	NX_GPIO_ClearInterruptPending( U32 ModuleIndex, S32 IntNum );

void	NX_GPIO_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable );
CBOOL	NX_GPIO_GetInterruptEnableAll( U32 ModuleIndex );
CBOOL	NX_GPIO_GetInterruptPendingAll( U32 ModuleIndex );
void	NX_GPIO_ClearInterruptPendingAll( U32 ModuleIndex );

void	NX_GPIO_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag );
U32		NX_GPIO_GetInterruptEnable32( U32 ModuleIndex );
U32		NX_GPIO_GetInterruptPending32( U32 ModuleIndex );
void	NX_GPIO_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag );

S32		NX_GPIO_GetInterruptPendingNumber( U32 ModuleIndex );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	GPIO Operation.
//@{
void			NX_GPIO_SetInterruptMode( U32 ModuleIndex, U32 BitNumber, NX_GPIO_INTMODE IntMode );
NX_GPIO_INTMODE	NX_GPIO_GetInterruptMode( U32 ModuleIndex, U32 BitNumber );
void			NX_GPIO_SetDetectEnable ( U32 ModuleIndex, U32 BitNumber, CBOOL DetectEnb );
CBOOL			NX_GPIO_GetDetectEnable ( U32 ModuleIndex, U32 BitNumber );
void			NX_GPIO_SetDetectEnable32 ( U32 ModuleIndex, U32 EnableFlag );
CBOOL			NX_GPIO_GetDetectEnable32 ( U32 ModuleIndex );
void			NX_GPIO_SetOutputEnable ( U32 ModuleIndex, U32 BitNumber, CBOOL OutputEnb );
CBOOL			NX_GPIO_GetOutputEnable ( U32 ModuleIndex, U32 BitNumber );
void			NX_GPIO_SetOutputValue	( U32 ModuleIndex, U32 BitNumber, CBOOL Value );
CBOOL			NX_GPIO_GetOutputValue	( U32 ModuleIndex, U32 BitNumber );
CBOOL			NX_GPIO_GetInputValue		( U32 ModuleIndex, U32 BitNumber );
void			NX_GPIO_SetPullUpEnable ( U32 ModuleIndex, U32 BitNumber, CBOOL enable);
CBOOL			NX_GPIO_GetPullUpEnable ( U32 ModuleIndex, U32 BitNumber );

void			NX_GPIO_SetPadFunction( U32 ModuleIndex, U32 BitNumber, NX_GPIO_PADFUNC padfunc );
NX_GPIO_PADFUNC	NX_GPIO_GetPadFunction( U32 ModuleIndex, U32 BitNumber );

U32				NX_GPIO_GetValidBit( U32 ModuleIndex );
//@}

//@}

#ifdef	__cplusplus
}
#endif

#endif //__NX_GPIO_H__
