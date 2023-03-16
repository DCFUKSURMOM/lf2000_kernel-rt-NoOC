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
//	Module		: Interrupt Controller
//	File		: nx_intc.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_INTC_H__
#define __NX_INTC_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	INTC INTERRUPT
//------------------------------------------------------------------------------
//@{

	//--------------------------------------------------------------------------
	/// @brief	INTC register set structure.
	struct NX_INTC_RegisterSet
	{
		volatile U32 __Reserved[2];	///< 0x00 ~ 0x07 : Reserved
		volatile U32 INTMODE[2];	///< 0x08, 0x0C : Interrupt Mode Register
		volatile U32 INTMASK[2];	///< 0x10, 0x14 : Interrupt Mask Register
		volatile U32 PRIORDER;		///< 0x18 : Priority Order Register
		volatile U32 __Reserved01;	///< 0x1C : Reserved
		volatile U32 INTPEND[2];	///< 0x20, 0x24 : Interrupt Pending Register
		volatile U32 __Reserved02;	///< 0x28 : Reserved
	};

	enum NX_INTC_NOI { NX_INTC_NUM_OF_INT = 62UL };

	/// @brief INTERRUPT CONTROLLER interrupt for interrupt interface
	enum
	{
		NX_INTC_INT_PDISPLAY	= 0,		///< Primary Display Interrupt
		NX_INTC_INT_SDISPLAY	= 1,		///< Secondary Display Interrupt
		NX_INTC_INT_VIP			= 2,		///< VIP Interrupt
		NX_INTC_INT_DMA			= 3,		///< DMA Interrupt
		NX_INTC_INT_TIMER0		= 4,		///< TIMER0 Interrupt
		NX_INTC_INT_SYSCTRL		= 5,		///< System Control Interrupt
		NX_INTC_INT_MPEGTSI		= 7,		///< MPEGTSI Interrupt
		NX_INTC_INT_UART0		= 10,		///< UART0 Interrupt
		NX_INTC_INT_TIMER1		= 11,		///< TIMER1 Interrupt
		NX_INTC_INT_SSPSPI		= 12,		///< SSPSPI Interrupt
		NX_INTC_INT_GPIO		= 13,		///< GPIO Interrupt
		NX_INTC_INT_SDMMC0		= 14,		///< SD/MMC0 Interrupt
		NX_INTC_INT_TIMER2		= 15,		///< TIMER2 Interrupt
		NX_INTC_INT_TIMER3		= 21,		///< TIMER3 Interrupt
		NX_INTC_INT_DEINTERLACE	= 22,		///< DEINTERLACE Interrupt
		NX_INTC_INT_AUDIOIF		= 24,		///< Audio Interface Interrupt
		NX_INTC_INT_ADC			= 25,		///< ADC Interrupt
		NX_INTC_INT_MCUSTATIC	= 26,		///< MCUS Interrupt
		NX_INTC_INT_GRP3D		= 27,		///< 3D Graphic Controller Interrupt
		NX_INTC_INT_ROTATOR		= 29,		///< Rotator Interrupt
		NX_INTC_INT_RTC			= 31,		///< RTC Interrupt
		NX_INTC_INT_I2C0		= 32,		///< I2C0 Interrupt
		NX_INTC_INT_I2C1		= 33,		///< I2C1 Interrupt
		NX_INTC_INT_UART1		= 34,		///< UART1 Interrupt
		NX_INTC_INT_UART2		= 35,		///< UART2 Interrupt
		NX_INTC_INT_UART3		= 36,		///< UART3 Interrupt
		NX_INTC_INT_UART4		= 37,		///< UART4 Interrupt
		NX_INTC_INT_UART5		= 38,		///< UART5 Interrupt
		NX_INTC_INT_SSPSPI1		= 39,		///< SSPSPI1 Interrupt
		NX_INTC_INT_SSPSPI2		= 40,		///< SSPSPI2 Interrupt
		NX_INTC_INT_CSC			= 41,		///< CSC Interrupt
		NX_INTC_INT_SDMMC1		= 42,		///< SDMMC1 Interrupt
		NX_INTC_INT_TIMER4		= 43,		///< TIMER4 Interrupt
		NX_INTC_INT_ALIVE		= 50,		///< ALIVE Interrupt
		NX_INTC_INT_OHCI		= 52		///< OHCI Interrupt
	};

	/// @brief	interrupt mode.
	typedef enum
	{
		NX_INTC_INTMODE_IRQ = 0UL,		///< IRQ mode
		NX_INTC_INTMODE_FIQ = 1UL		///< FIQ mode

	}NX_INTC_INTMODE ;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{

CBOOL	NX_INTC_Initialize( void );
U32		NX_INTC_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{

U32		NX_INTC_GetPhysicalAddress( void );
U32		NX_INTC_GetSizeOfRegisterSet( void );
void	NX_INTC_SetBaseAddress( U32 BaseAddress );
U32		NX_INTC_GetBaseAddress( void );
CBOOL	NX_INTC_OpenModule( void );
CBOOL	NX_INTC_CloseModule( void );
CBOOL	NX_INTC_CheckBusy( void );
CBOOL	NX_INTC_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
void	NX_INTC_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_INTC_GetInterruptEnable( S32 IntNum );
CBOOL	NX_INTC_GetInterruptPending( S32 IntNum );
void	NX_INTC_ClearInterruptPending( S32 IntNum );

void	NX_INTC_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_INTC_GetInterruptEnableAll( void );
CBOOL	NX_INTC_GetInterruptPendingAll( void );
void	NX_INTC_ClearInterruptPendingAll( void );

void	NX_INTC_SetInterruptEnable64( U32 EnableHigh, U32 EnableLow );
void	NX_INTC_GetInterruptEnable64( U32 *pEnableHigh, U32 *pEnableLow );
void	NX_INTC_GetInterruptPending64( U32 *pPendingHigh, U32 *pPendingLow );
void	NX_INTC_ClearInterruptPending64( U32 PendingHigh, U32 PendingLow );

S32		NX_INTC_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Controller Operation.
//------------------------------------------------------------------------------
//@{
void	NX_INTC_SetInterruptMode ( U32 IntNum, NX_INTC_INTMODE IntMode	);
void	NX_INTC_SetPriorityMode( U32 ArbiterNum, U32 OrderSel );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_INTC_H__
