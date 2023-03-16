//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: SSPSPI
//	File		: nx_sspspi.h
//	Description	:
//	Author		: Firmeare Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_SSPSPI_H__
#define __NX_SSPSPI_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	SSPSPI SSP/SPI
//------------------------------------------------------------------------------
//@{

	/// @brief	SSPSPI Module's Register List
	struct	NX_SSPSPI_RegisterSet
	{
		volatile U16	CONT0;					///< 0x00 : SSP/SPI Control Register 0
		volatile U16	CONT1;					///< 0x02 : SSP/SPI Control Register 1
		volatile U16	DATA;					///< 0x04 : SSP/SPI Data Register
		volatile U16	STATE;					///< 0x06 : SSP/SPI Status Register
		volatile U16	RXBUSTSIZE;				///< 0x08 : SSP/SPI Burst Receive Size Register
		volatile U8		__Reserved[0x40-0x0A];	///< 0x0A ~ 0x3E : Reserved Region
		volatile U32	CLKENB;					///< 0x40 : SSP/SPI Clock Enable Register
		volatile U32	CLKGEN;					///< 0x44 : SSP/SPI Clock Generator Register
	};

	/// @brief	Data Transfer Format
	typedef enum
	{
		NX_SSPSPI_FORMAT_A	= 0,	///< Format A
		NX_SSPSPI_FORMAT_B	= 1		///< Format B
	}NX_SSPSPI_FORMAT;

	/// @brief	Communication protocol
	typedef enum
	{
		NX_SSPSPI_PROTOCOL_SSP		= 0,	///< SSP (Synchronous Serial Protocol)
		NX_SSPSPI_PROTOCOL_SPI		= 1		///< SPI (Serial Peripheral Interface Protocol )
	} NX_SSPSPI_PROTOCOL ;

	/// @brief	SSPSPI Interrupts for interrupt interfaces
	enum	NX_SSPSPI_INT
	{
		NX_SSPSPI_INT_RX	=	0,	///< Rx Buffer Full.
		NX_SSPSPI_INT_TX	=	1,	///< Tx Buffer Empty.
		NX_SSPSPI_INT_END	=	2	///< Tx/Rx End.
	};

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_SSPSPI_Initialize( void );
U32		NX_SSPSPI_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_SSPSPI_GetPhysicalAddress( U32 ModuleIndex );
U32		NX_SSPSPI_GetSizeOfRegisterSet( void );
void	NX_SSPSPI_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress );
U32		NX_SSPSPI_GetBaseAddress( U32 ModuleIndex );
CBOOL	NX_SSPSPI_OpenModule( U32 ModuleIndex );
CBOOL	NX_SSPSPI_CloseModule( U32 ModuleIndex );
CBOOL	NX_SSPSPI_CheckBusy( U32 ModuleIndex );
CBOOL	NX_SSPSPI_CanPowerDown( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_SSPSPI_GetInterruptNumber( U32 ModuleIndex );

void	NX_SSPSPI_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable );
CBOOL	NX_SSPSPI_GetInterruptEnable( U32 ModuleIndex, S32 IntNum );
CBOOL	NX_SSPSPI_GetInterruptPending( U32 ModuleIndex, S32 IntNum );
void	NX_SSPSPI_ClearInterruptPending( U32 ModuleIndex, S32 IntNum );

void	NX_SSPSPI_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable );
CBOOL	NX_SSPSPI_GetInterruptEnableAll( U32 ModuleIndex );
CBOOL	NX_SSPSPI_GetInterruptPendingAll( U32 ModuleIndex );
void	NX_SSPSPI_ClearInterruptPendingAll( U32 ModuleIndex );

void	NX_SSPSPI_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag );
U32		NX_SSPSPI_GetInterruptEnable32( U32 ModuleIndex );
U32		NX_SSPSPI_GetInterruptPending32( U32 ModuleIndex );
void	NX_SSPSPI_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag );

S32		NX_SSPSPI_GetInterruptPendingNumber( U32 ModuleIndex );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	DMA Interface
//@{
U32		NX_SSPSPI_GetDMAIndex_Tx( U32 ModuleIndex );
U32		NX_SSPSPI_GetDMAIndex_Rx( U32 ModuleIndex );
U32		NX_SSPSPI_GetDMABusWidth( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_SSPSPI_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode );
NX_PCLKMODE	NX_SSPSPI_GetClockPClkMode( U32 ModuleIndex );
void		NX_SSPSPI_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc );
U32			NX_SSPSPI_GetClockSource( U32 ModuleIndex, U32 Index );
void		NX_SSPSPI_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divisor );
U32			NX_SSPSPI_GetClockDivisor( U32 ModuleIndex, U32 Index );
void		NX_SSPSPI_SetClockDivisorEnable( U32 ModuleIndex, CBOOL Enable );
CBOOL		NX_SSPSPI_GetClockDivisorEnable( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
/// @name Configuration Function
//--------------------------------------------------------------------------
//@{
void	NX_SSPSPI_SetDMATransferMode( U32 ModuleIndex, CBOOL bDMA );
CBOOL	NX_SSPSPI_GetDMATransferMode( U32 ModuleIndex );
void	NX_SSPSPI_SetExternalClockSource( U32 ModuleIndex, CBOOL bExt );
CBOOL	NX_SSPSPI_GetExternalClockSource( U32 ModuleIndex );
void	NX_SSPSPI_SetBitWidth( U32 ModuleIndex, U32 bitWidth );
U32		NX_SSPSPI_GetBitWidth( U32 ModuleIndex );
void	NX_SSPSPI_SetDividerCount( U32 ModuleIndex, U32 dividerCNT );
U32		NX_SSPSPI_GetDividerCount( U32 ModuleIndex );
void	NX_SSPSPI_SetByteSwap( U32 ModuleIndex, CBOOL bSwap );
CBOOL	NX_SSPSPI_GetByteSwap( U32 ModuleIndex );
void	NX_SSPSPI_SetSlaveMode( U32 ModuleIndex, CBOOL bSlave );
CBOOL	NX_SSPSPI_GetSlaveMode( U32 ModuleIndex );
void	NX_SSPSPI_SetClockPolarityInvert( U32 ModuleIndex, CBOOL bInvert );
CBOOL	NX_SSPSPI_GetClockPolarityInvert( U32 ModuleIndex );

void				NX_SSPSPI_SetSPIFormat( U32 ModuleIndex, NX_SSPSPI_FORMAT format);
NX_SSPSPI_FORMAT	NX_SSPSPI_GetSPIFormat( U32 ModuleIndex );
void				NX_SSPSPI_SetProtocol( U32 ModuleIndex, NX_SSPSPI_PROTOCOL protocol );
NX_SSPSPI_PROTOCOL	NX_SSPSPI_GetProtocol( U32 ModuleIndex );

//@}

//--------------------------------------------------------------------------
/// @name Operation Function
//--------------------------------------------------------------------------
//@{
void	NX_SSPSPI_ResetFIFO( U32 ModuleIndex );
void	NX_SSPSPI_StartRxBurst( U32 ModuleIndex );
void	NX_SSPSPI_SetRxBurstEnable( U32 ModuleIndex, CBOOL bEnable );
CBOOL	NX_SSPSPI_GetRxBurstEnable( U32 ModuleIndex );
void	NX_SSPSPI_SetBurstRxSize( U32 ModuleIndex, U32 size );
U32		NX_SSPSPI_GetBurstRxSize( U32 ModuleIndex );
void	NX_SSPSPI_SetEnable( U32 ModuleIndex, CBOOL bEnable );
CBOOL	NX_SSPSPI_GetEnable( U32 ModuleIndex );
U8		NX_SSPSPI_GetByte(U32 ModuleIndex);
U16		NX_SSPSPI_GetHalfWord(U32 ModuleIndex);
void	NX_SSPSPI_PutByte(U32 ModuleIndex, U8 ByteData);
void	NX_SSPSPI_PutHalfWord(U32 ModuleIndex, U16 HalfWordData);
void	NX_SSPSPI_SetHiZwhenTransmit( U32 ModuleIndex , CBOOL bEnable	);

//@}


//--------------------------------------------------------------------------
/// @name FIFO State Check Function
//--------------------------------------------------------------------------
//@{
CBOOL	NX_SSPSPI_IsTxShiftEmpty( U32 ModuleIndex );
CBOOL	NX_SSPSPI_IsTxFIFOFull(U32 ModuleIndex);
CBOOL	NX_SSPSPI_IsTxFIFOEmpty(U32 ModuleIndex);
CBOOL	NX_SSPSPI_IsRxFIFOFull(U32 ModuleIndex);
CBOOL	NX_SSPSPI_IsRxFIFOEmpty(U32 ModuleIndex);
CBOOL	NX_SSPSPI_IsTxRxEnd( U32 ModuleIndex );
//@}

//@}

#ifdef	__cplusplus
}
#endif

#endif // __NX_SSPSPI_H__
