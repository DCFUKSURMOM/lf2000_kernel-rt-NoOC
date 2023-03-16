//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: USB Host Controller
//	File		: nx_ohci.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_OHCI_H__
#define __NX_OHCI_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	OHCI OHCI
//------------------------------------------------------------------------------
//@{

/// @brief USB Host OHCI Module's Register List
struct	NX_OHCI_RegisterSet
{
	volatile U32	HcRevision;					///< 0x00 : Revision Register.
	volatile U32	HcControl;					///< 0x04		Control Register.
	volatile U32	HcCommandStatus;			///< 0x08 : Command Status Register.
	volatile U32	HcInterruptStatus;			///< 0x0C : Interrupt Status Register.
	volatile U32	HcInterruptEnable;			///< 0x10 : Interrupt Enable Register.
	volatile U32	HcInterruptDisable;			///< 0x14 : Interrupt Disable Register.
	volatile U32	HcHCCA;						///< 0x18 : Host Controller Communication Area Register.
	volatile U32	HcPeriodCurrentED;			///< 0x1C : PeriodCurrentED Register.
	volatile U32	HcControlHeadED;			///< 0x20 : ControlHeadED Register.
	volatile U32	HcControlCurrentED;			///< 0x24 : ControlCurrentED Register.
	volatile U32	HcBulkHeadED;				///< 0x28 : BulkHeadED Register.
	volatile U32	HcBulkCurrentED;			///< 0x2C : BulkCurrentED Register.
	volatile U32	HcDoneHead;					///< 0x30 : DoneHead Register.
	volatile U32	HcFmInterval;				///< 0x34 : Frame Interval Register.
	volatile U32	HcFmRemaining;				///< 0x38 : Frame Remaining Register.
	volatile U32	HcFmNumber;					///< 0x3C : Frame Number Register.
	volatile U32	HcPeriodicStart;			///< 0x40 : PeriodicStart Register.
	volatile U32	HcLSThreshold;				///< 0x44 : LSThreshold Register.
	volatile U32	HcRhDescriptorA;			///< 0x48 : Root Hub DescriptorA Register.
	volatile U32	HcRhDescriptorB;			///< 0x4C : Root Hub DescriptorB Register.
	volatile U32	HcRhStatus;					///< 0x50 : Root Hub Status Register.
	volatile U32	HcRhPortStatus;				///< 0x54 : Root Hub Port Status Register.
	volatile U32	Reserved0[(0x3C0-0x58)/4];	///< 0x58 ~ 0x3BC : Reserved Region.
	volatile U32	CLKENB;						///< 0x3C0 : Clock Enable Register.
	volatile U32	CLKGEN;						///< 0x3C4 : Clock Generator Register.
};

/// @brief OHCI interrupt for interrupt interface
enum NX_OHCI_INT
{
	NX_OHCI_INT_SO		= 0,		///< Scheduling Overrun Interrupt.
	NX_OHCI_INT_WDH		= 1,		///< HcDoneHead Writeback Interrupt.
	NX_OHCI_INT_SF		= 2,		///< Start of Frame Interrupt.
	NX_OHCI_INT_RD		= 3,		///< Resume Detect Interrupt.
	NX_OHCI_INT_UE		= 4,		///< Unrecoverable Error Interrupt.
	NX_OHCI_INT_FNO		= 5,		///< Frame Number Overflow Interrupt.
	NX_OHCI_INT_RHSC	= 6,		///< Root Hub Status Change Interrupt.
	NX_OHCI_INT_OC		= 30,		///< Ownership Change Interrupt.
	NX_OHCI_INT_MIE		= 31		///< Master Interrupt.
};

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_OHCI_Initialize( void );
U32		NX_OHCI_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_OHCI_GetPhysicalAddress( void );
U32		NX_OHCI_GetSizeOfRegisterSet( void );
void	NX_OHCI_SetBaseAddress( U32 BaseAddress );
U32		NX_OHCI_GetBaseAddress( void );
CBOOL	NX_OHCI_OpenModule( void );
CBOOL	NX_OHCI_CloseModule( void );
CBOOL	NX_OHCI_CheckBusy( void );
CBOOL	NX_OHCI_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_OHCI_GetInterruptNumber( void );

void	NX_OHCI_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_OHCI_GetInterruptEnable( S32 IntNum );
CBOOL	NX_OHCI_GetInterruptPending( S32 IntNum );
void	NX_OHCI_ClearInterruptPending( S32 IntNum );

void	NX_OHCI_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_OHCI_GetInterruptEnableAll( void );
CBOOL	NX_OHCI_GetInterruptPendingAll( void );
void	NX_OHCI_ClearInterruptPendingAll( void );

void	NX_OHCI_SetInterruptEnable32( U32 EnableFlag );
U32		NX_OHCI_GetInterruptEnable32( void );
U32		NX_OHCI_GetInterruptPending32( void );
void	NX_OHCI_ClearInterruptPending32( U32 PendingFlag );

S32		NX_OHCI_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_OHCI_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_OHCI_GetClockPClkMode( void );
void		NX_OHCI_SetClockBClkMode( NX_BCLKMODE mode );
NX_BCLKMODE	NX_OHCI_GetClockBClkMode( void );
void		NX_OHCI_SetClockSource( U32 Index, U32 ClkSrc );
U32			NX_OHCI_GetClockSource( U32 Index );
void		NX_OHCI_SetClockDivisor( U32 Index, U32 Divisor );
U32			NX_OHCI_GetClockDivisor( U32 Index );
void		NX_OHCI_SetClockDivisorEnable( CBOOL Enable );
CBOOL		NX_OHCI_GetClockDivisorEnable( void );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_OHCI_H__
