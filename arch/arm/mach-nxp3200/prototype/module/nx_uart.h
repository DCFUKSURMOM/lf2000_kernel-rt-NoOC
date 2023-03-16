//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: UART
//	File		: nx_uart.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_UART_H__
#define __NX_UART_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	UART	UART
//------------------------------------------------------------------------------
//@{

	/// @brief	UART Module's Register List
	struct	NX_UART_RegisterSet
	{
		volatile U16	LCON;			///< 0x00 : Line Control Register
		volatile U16	UCON;			///< 0x02 : Control Register
		volatile U16	FCON;			///< 0x04 : FIFO Control Register
		volatile U16	MCON;			///< 0x06 : Modem Control Register
		volatile U16	TRSTATUS;		///< 0x08 : Tx/Rx Status Register
		volatile U16	ESTATUS;		///< 0x0a : Error Status Register
		volatile U16	FSTATUS;		///< 0x0c : FIFO Status Register
		volatile U16	MSTATUS;		///< 0x0e : Modem Status Register
		volatile U16	THB;			///< 0x10 : Transmit Buffer Register
		volatile U16	RHB;			///< 0x12 : Receive Buffer Register
		volatile U16	BRD;			///< 0x14 : Baud Rate Divisor Register
		volatile U16	TIMEOUT;		///< 0x16 : Receive TimeOut Register
		volatile U16	INTCON;			///< 0x18 : Interrupt Control Register
		volatile U16	__Reserved[0x13];///< 0x1A ~ 0x3E : Reserved Region
		volatile U32	CLKENB;			///< 0x40 : Clock Enable Register
		volatile U32	CLKGEN;			///< 0x44 : Clock Generate Register
	};

	/// @brief	UART interrupt for interrupt interface
	enum
	{
		NX_UART_INT_TX		= 0,			///< Tx interrupt
		NX_UART_INT_RX		= 1,			///< Rx interrupt
		NX_UART_INT_ERROR	= 2,			///< Error interrupt
		NX_UART_INT_MODEM	= 3,			///< Modem interrupt
	};

	/// @brief Parity generation/check type
	typedef enum
	{
		NX_UART_PARITYMODE_NONE		= 0,	///< No parity
		NX_UART_PARITYMODE_ODD		= 4,	///< Odd parity
		NX_UART_PARITYMODE_EVEN		= 5,	///< Even parity
		NX_UART_PARITYMODE_FORCED1	= 6,	///< Forced/checked as 1
		NX_UART_PARITYMODE_FORCED0	= 7		///< Forced/checked as 0

	}	NX_UART_PARITYMODE ;

	/// @brief Tx/Rx IRQ Type
	typedef enum
	{
		NX_UART_IRQTYPE_PULSE		= 0,	///< Pulse
		NX_UART_IRQTYPE_LEVEL		= 1		///< Level

	}	NX_UART_IRQTYPE;

	/// @brief Tx/Rx Operation Mode
	typedef enum
	{
		NX_UART_OPERMODE_DISABLE	= 0,	///< Disable
		NX_UART_OPERMODE_NORMAL		= 1,	///< Interrupt or Polling
		NX_UART_OPERMODE_DMA		= 2		///< DMA

	}	NX_UART_OPERMODE	;

	/// @brief Smartmedia Card Operation Mode
	typedef enum
	{
		NX_UART_SCMODE_DISABLE	=	0,		///< Smart Card Tx/Rx Disable
		NX_UART_SCMODE_TX		=	1,		///< Smart Card Tx Enable
		NX_UART_SCMODE_RX		=	2		///< Smart Card Rx Enable

	}	NX_UART_SCMODE ;

	///	@brief Tx/Rx Status
	typedef enum
	{
		NX_UART_RX_BUFFER_READY		= 0x01,			///< Rx buffer contains valid data
		NX_UART_TX_BUFFER_EMPTY		= 0x02,			///< Tx buffer is empty
		NX_UART_TRANSMITTER_EMPTY	= 0x04			///< Tx buffer and the transmit shifter are empty

	}	NX_UART_TXRXSTATUS ;

	/// @brief Error Status
	typedef enum
	{
		NX_UART_ERRSTAT_OVERRUN		= 0x01,			///< Overrun Error
		NX_UART_ERRSTAT_PARITY		= 0x02,			///< Parity Error
		NX_UART_ERRSTAT_FRAME		= 0x04,			///< Frame Error
		NX_UART_ERRSTAT_BREAK		= 0x08			///< Break Signal receive

	}	NX_UART_ERRSTAT ;

	/// @brief FIFO Status
	typedef enum
	{
		NX_UART_FIFOSTAT_RXFULL		= 0x01,			///< RX FIFO Full
		NX_UART_FIFOSTAT_TXFULL		= 0x02,			///< RX FIFO Full
		NX_UART_FIFOSTAT_RXERR		= 0x04			///< RX FIFO Error

	}	NX_UART_FIFOSTAT ;

	/// @brief Modem Status
	typedef enum
	{
		NX_UART_MODEMSTAT_DCD_CHANGED	= 1 << 7,	///< Modem Status DCD Pin Changed
		NX_UART_MODEMSTAT_RI_CHANGED	= 1 << 6,	///< Modem Status RI Pin Changed
		NX_UART_MODEMSTAT_DSR_CHANGED	= 1 << 5,	///< Modem Status DSR Pin Changed
		NX_UART_MODEMSTAT_CTS_CHANGED	= 1 << 4,	///< Modem Status CTS Pin Changed
		NX_UART_MODEMSTAT_DCD_LOW		= 1 << 3,	///< Modem DCD Pin High
		NX_UART_MODEMSTAT_RI_LOW		= 1 << 2,	///< Modem RI Pin	Low
		NX_UART_MODEMSTAT_DSR_LOW		= 1 << 1,	///< Modem DSR Pin Low
		NX_UART_MODEMSTAT_CTS_ACTIVE	= 1 << 0	///< Modem CTS Pin Active

	}	NX_UART_MODEMSTAT ;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_UART_Initialize( void );
U32		NX_UART_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_UART_GetPhysicalAddress( U32 ModuleIndex );
U32		NX_UART_GetSizeOfRegisterSet( void );
void	NX_UART_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress );
U32		NX_UART_GetBaseAddress( U32 ModuleIndex );
CBOOL	NX_UART_OpenModule( U32 ModuleIndex );
CBOOL	NX_UART_CloseModule( U32 ModuleIndex );
CBOOL	NX_UART_CheckBusy( U32 ModuleIndex );
CBOOL	NX_UART_CanPowerDown( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_UART_GetInterruptNumber( U32 ModuleIndex );

void	NX_UART_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable );
CBOOL	NX_UART_GetInterruptEnable( U32 ModuleIndex, S32 IntNum );
CBOOL	NX_UART_GetInterruptPending( U32 ModuleIndex, S32 IntNum );
void	NX_UART_ClearInterruptPending( U32 ModuleIndex, S32 IntNum );

void	NX_UART_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable );
CBOOL	NX_UART_GetInterruptEnableAll( U32 ModuleIndex );
CBOOL	NX_UART_GetInterruptPendingAll( U32 ModuleIndex );
void	NX_UART_ClearInterruptPendingAll( U32 ModuleIndex );

void	NX_UART_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag );
U32		NX_UART_GetInterruptEnable32( U32 ModuleIndex );
U32		NX_UART_GetInterruptPending32( U32 ModuleIndex );
void	NX_UART_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag );

S32		NX_UART_GetInterruptPendingNumber( U32 ModuleIndex );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	DMA Interface
//@{
U32		NX_UART_GetDMAIndex_Tx( U32 ModuleIndex );
U32		NX_UART_GetDMAIndex_Rx( U32 ModuleIndex );
U32		NX_UART_GetDMABusWidth( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_UART_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode );
NX_PCLKMODE	NX_UART_GetClockPClkMode( U32 ModuleIndex );
void		NX_UART_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc );
U32			NX_UART_GetClockSource( U32 ModuleIndex, U32 Index );
void		NX_UART_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divisor );
U32			NX_UART_GetClockDivisor( U32 ModuleIndex, U32 Index );
void		NX_UART_SetClockDivisorEnable( U32 ModuleIndex, CBOOL Enable );
CBOOL		NX_UART_GetClockDivisorEnable( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
/// @name	Configuration operations
//--------------------------------------------------------------------------
//@{
void	NX_UART_SetSIRMode( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_UART_GetSIRMode( U32 ModuleIndex );
void	NX_UART_SetLoopBackMode( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_UART_GetLoopBackMode( U32 ModuleIndex );
void	NX_UART_SetIntEnbWhenExceptionOccur( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_UART_GetIntEnbWhenExceptionOccur( U32 ModuleIndex );
void	NX_UART_SetFrameConfiguration( U32 ModuleIndex, NX_UART_PARITYMODE Parity,	U32 DataWidth,	U32 StopBit );
void	NX_UART_GetFrameConfiguration( U32 ModuleIndex, NX_UART_PARITYMODE* pParity, U32* pDataWidth, U32* pStopBit );
void	NX_UART_SetBRD( U32 ModuleIndex, U16 BRD );
U16		NX_UART_GetBRD( U32 ModuleIndex );
U16		NX_UART_MakeBRD( U32 BaudRate, U32 CLKinHz );
void	NX_UART_SetRxTimeOutEnb( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_UART_GetRxTimeOutEnb( U32 ModuleIndex );
void	NX_UART_SetRxTimeOut( U32 ModuleIndex, U16 TimeOut );
U16		NX_UART_GetRxTimeOut( U32 ModuleIndex );
void	NX_UART_SetSYNCPendClear( U32 ModuleIndex );
void			NX_UART_SetTxIRQType( U32 ModuleIndex, NX_UART_IRQTYPE IrqType );
NX_UART_IRQTYPE	NX_UART_GetTxIRQType( U32 ModuleIndex );
void			NX_UART_SetRxIRQType( U32 ModuleIndex, NX_UART_IRQTYPE IrqType );
NX_UART_IRQTYPE	NX_UART_GetRxIRQType( U32 ModuleIndex );
void	NX_UART_SendBreak( U32 ModuleIndex );
void				NX_UART_SetTxOperationMode( U32 ModuleIndex, NX_UART_OPERMODE mode );
NX_UART_OPERMODE	NX_UART_GetTxOperationMode( U32 ModuleIndex );
void				NX_UART_SetRxOperationMode( U32 ModuleIndex, NX_UART_OPERMODE mode );
NX_UART_OPERMODE	NX_UART_GetRxOperationMode( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
/// @name	FIFO control operations
//--------------------------------------------------------------------------
//@{
void	NX_UART_SetTxFIFOTriggerLevel( U32 ModuleIndex, U32 level );
U32		NX_UART_GetTxFIFOTriggerLevel( U32 ModuleIndex );
void	NX_UART_SetRxFIFOTriggerLevel( U32 ModuleIndex, U32 level );
U32		NX_UART_GetRxFIFOTriggerLevel( U32 ModuleIndex );
void	NX_UART_ResetTxFIFO( U32 ModuleIndex );
CBOOL	NX_UART_IsTxFIFOReset( U32 ModuleIndex );
void	NX_UART_ResetRxFIFO( U32 ModuleIndex );
CBOOL	NX_UART_IsRxFIFOReset( U32 ModuleIndex );
void	NX_UART_SetFIFOEnb( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_UART_GetFIFOEnb( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
/// @name	Smartmedia card control operations
//--------------------------------------------------------------------------
//@{
void	NX_UART_SetSCTxEnb( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_UART_GetSCTxEnb( U32 ModuleIndex );
void	NX_UART_SetSCRxEnb( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_UART_GetSCRxEnb( U32 ModuleIndex );
void	NX_UART_SetSCTxMode( U32 ModuleIndex );
void	NX_UART_SetSCRxMode( U32 ModuleIndex );
NX_UART_SCMODE	NX_UART_GetSCMode( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
/// @name	Status operations
//--------------------------------------------------------------------------
//@{
U32		NX_UART_GetTxRxStatus( U32 ModuleIndex );
U32		NX_UART_GetErrorStatus( U32 ModuleIndex );
U32		NX_UART_GetFIFOStatus( U32 ModuleIndex );
U32		NX_UART_GetTxFIFOCount( U32 ModuleIndex );
U32		NX_UART_GetRxFIFOCount( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
/// @name	Modem operations
//--------------------------------------------------------------------------
//@{
void	NX_UART_SetDTR( U32 ModuleIndex, CBOOL bActive );
CBOOL	NX_UART_GetDTR( U32 ModuleIndex );
void	NX_UART_SetRTS( U32 ModuleIndex, CBOOL bActive );
CBOOL	NX_UART_GetRTS( U32 ModuleIndex );
void	NX_UART_SetAutoFlowControl( U32 ModuleIndex, CBOOL enable );
CBOOL	NX_UART_GetAutoFlowControl( U32 ModuleIndex );
void	NX_UART_SetHalfChannelEnable( U32 ModuleIndex, CBOOL enable );
CBOOL	NX_UART_GetHalfChannelEnable( U32 ModuleIndex );
U32		NX_UART_GetModemStatus( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
/// @name	Basic operations
//--------------------------------------------------------------------------
//@{
void	NX_UART_SendByte( U32 ModuleIndex, U8 Data );
U8		NX_UART_GetByte( U32 ModuleIndex );
//@}

//--------------------------------------------------------------------------
/// @name	UART Configuration Function
//--------------------------------------------------------------------------
//@{
void	NX_UART_SetLineConfig( U32 ModuleIndex, CBOOL sirMode, NX_UART_PARITYMODE Parity, U32 DataWidth, U32 StopBit );
void	NX_UART_SetControlConfig( U32 ModuleIndex, CBOOL syncPendClear, NX_UART_IRQTYPE txType, NX_UART_IRQTYPE rxType, NX_UART_OPERMODE txOper, NX_UART_OPERMODE rxOper );
void	NX_UART_SetFIFOConfig( U32 ModuleIndex, CBOOL fifoEnb, CBOOL txFIFOReset, CBOOL rxFIFOReset, U32 txLevel, U32 rxLevel );
void	NX_UART_SetBaudRateConfig( U32 ModuleIndex, U16 baudRate, U16 timeOut );
void	NX_UART_SetModemConfig( U32 ModuleIndex, CBOOL autoflow, CBOOL halfChannel, CBOOL SCTxEnb, CBOOL SCRxEnb, CBOOL DTR, CBOOL RTS );

void	NX_UART_GetLineConfig( U32 ModuleIndex, U32* pSirMode, NX_UART_PARITYMODE* pParity, U32* pDataWidth, U32* pStopBit );
void	NX_UART_GetControlConfig( U32 ModuleIndex, NX_UART_IRQTYPE* pTxType, NX_UART_IRQTYPE* pRxType, NX_UART_OPERMODE* pTxOper, NX_UART_OPERMODE* pRxOper );
void	NX_UART_GetFIFOConfig( U32 ModuleIndex, U32* pFIFOEnb, U32* pTxFIFOReset, U32* pRxFIFOReset, U32* pTxLevel, U32* pRxLevel );
void	NX_UART_GetBaudRateConfig( U32 ModuleIndex, U16* pBaudRate, U16* pTimeOut );
void	NX_UART_GetModemConfig( U32 ModuleIndex, U32* pAutoflow, U32* pHalfChannel, U32* pSCTxEnb, U32* pSCRxEnb, U32* pDTR, U32* pRTS );
//@}

//--------------------------------------------------------------------------
/// @name	Register Backup & Store Function
//--------------------------------------------------------------------------
//@{
void	NX_UART_SetLCON( U32 ModuleIndex, U16 value );
void	NX_UART_SetUCON( U32 ModuleIndex, U16 value );
void	NX_UART_SetFCON( U32 ModuleIndex, U16 value );
void	NX_UART_SetMCON( U32 ModuleIndex, U16 value );
void	NX_UART_SetTIMEOUT( U32 ModuleIndex, U16 value );
void	NX_UART_SetINTCON( U32 ModuleIndex, U16 value );

U16		NX_UART_GetLCON( U32 ModuleIndex );
U16		NX_UART_GetUCON( U32 ModuleIndex );
U16		NX_UART_GetFCON( U32 ModuleIndex );
U16		NX_UART_GetMCON( U32 ModuleIndex );
U16		NX_UART_GetTIMEOUT( U32 ModuleIndex );
U16		NX_UART_GetINTCON( U32 ModuleIndex );
//@}

//@}

#ifdef	__cplusplus
}
#endif

#endif // __NX_UART_H__
