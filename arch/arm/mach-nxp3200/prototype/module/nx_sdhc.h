//------------------------------------------------------------------------------
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: SD Host Controller
//	File		: nx_sspspi.h
//	Description	:
//	Author		:
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_SDHC_H__
#define __NX_SDHC_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
/// @defgroup	SDHC SD Host Controller
//------------------------------------------------------------------------------
//@{

//------------------------------------------------------------------------------
/// @brief	SDHC Module's Register Set
//------------------------------------------------------------------------------
struct	NX_SDHC_RegisterSet
{
	volatile U32	CTRL;						///< 0x000 : Control
	volatile U32	__Reserved00;				///< 0x004 : Reserved
	volatile U32	CLKDIV;						///< 0x008 : Clock Divider 
	volatile U32	__Reserved01;				///< 0x00C : Reserved
	volatile U32	CLKENA;						///< 0x010 : Clock Enable
	volatile U32	TMOUT;						///< 0x014 : Time-Out
	volatile U32	CTYPE;						///< 0x018 : Card Type
	volatile U32	BLKSIZ;						///< 0x01C : Block Size
	volatile U32	BYTCNT;						///< 0x020 : Byte Count
	volatile U32	INTMASK;					///< 0x024 : Interrupt Mask
	volatile U32	CMDARG;						///< 0x028 : Command Argument
	volatile U32	CMD;						///< 0x02C : Command
	volatile U32	RESP0;						///< 0x030 : Response 0
	volatile U32	RESP1;						///< 0x034 : Response 1
	volatile U32	RESP2;						///< 0x038 : Response 2
	volatile U32	RESP3;						///< 0x03C : Response 3
	volatile U32	MINTSTS;					///< 0x040 : Masked Interrupt
	volatile U32	RINTSTS;					///< 0x044 : Raw Interrupt
	volatile U32	STATUS;						///< 0x048 : Status
	volatile U32	FIFOTH;						///< 0x04C : FIFO Threshold Watermark
	volatile U8		__Reserved02[0x05C-0x050];	///< 0x050 ~ 0x05B : Reserved
	volatile U32	TCBCNT;						///< 0x05C : Transferred Card Byte Count
	volatile U32	TBBCNT;						///< 0x060 : Transferred Host Byte Count
	volatile U8		__Reserved03[0x100-0x064];	///< 0x064 ~ 0x0FF : Reserved
	volatile U32	DATA;						///< 0x100 : Data
	volatile U8		__Reserved04[0x7C0-0x104];	///< 0x104 ~ 0x7BF : Reserved
	volatile U32	CLKENB;						///< 0x7C0 : Clock Enable for CLKGEN module
	volatile U32	CLKGEN;						///< 0x7C4 : Clock Generator for CLKGEN module
};

//------------------------------------------------------------------------------
// To remove following waring on RVDS compiler
//		Warning : #66-D: enumeration value is out of "int" range
//------------------------------------------------------------------------------
#ifdef __arm	
#pragma diag_remark 66		// disable #66 warining
#endif

//------------------------------------------------------------------------------
/// @brief	SDHC Interrupts for interrupt interfaces
enum	NX_SDHC_INT
{
	NX_SDHC_INT_SDIO	= 16,	///< SDIO Interrupt.\n
								///		Interrupt from SDIO card.
	NX_SDHC_INT_EBE		= 15,	///< End Bit Error for reading, Write no CRC for wrting.\n
								///		Error in end-bit during read operations, or no data CRC or negative CRC
								///		received during write operation.\n\n
								///		\b Note> \n
								///		For MMC CMD19, there may be no CRC status returned by the card. 
								///		Hence, NX_SDHC_INT_EBE is set for CMD19. The application should not treat
								///		this as an error.
	NX_SDHC_INT_ACD		= 14,	///< Auto Command Done.\n
								///		Stop/abort commands automatically sent by card unit and not initiated by
								///		host; similar to Command Done (NX_SDHC_INT_CD) interrupt.
	NX_SDHC_INT_SBE		= 13,	///< Start Bit Error.\n
								///		Error in data start bit when data is read from a card. In 4-bit mode, if all
								///		data bits do not have start bit, then this error is set.
	NX_SDHC_INT_HLE		= 12,	///< Hardware Locked Error.\n
								///		This interrupt is genearted when the SDHC module cannot load a command
								///		issued by the user. When the user sets a command with NX_SDHC_CMDFLAG_STARTCMD
								///		flag, the SDHC module tries to load the command. If the command buffer is
								///		already filled with a command, this error is raised.
								///		And also if the user try to modify any of settings while a command loading is in
								///		progress, then the modification is ignored and the SDHC module generates this
								///		error.
	NX_SDHC_INT_FRUN	= 11,	///< FIFO underrun/overrun Error.\n
								///		Host tried to push data when FIFO was full, or host tried to read data
								///		when FIFO was empty. Typically this should not happen, except due to
								///		error in software.\n
								///		SDHC module never pushes data into FIFO when FIFO is full, and pop data
								///		when FIFO is empty.
	NX_SDHC_INT_HTO		= 10,	///< Data Startvation by Host Timeout.\n
								///		To avoid data loss, SDCLK is stopped if FIFO is empty
								///		when writing to card, or FIFO is full when reading from card. Whenever
								///		SDCLK is stopped to avoid data loss, data-starvation timeout counter
								///		is started with data-timeout value. This interrupt is set if host does not fill
								///		data into FIFO during write to card, or does not read from FIFO during
								///		read from card before timeout period.\n
								///		Even after timeout, SDCLK stays in stopped state with SDHC module waiting.
								///		It is responsibility of host to push or pop data into FIFO upon interrupt, 
								///		which automatically restarts SDCLK and SDHC module.\n
								///		Even if host wants to send stop/abort command, it still needs to ensure it
								///		has to push or pop FIFO so that clock starts in order for stop/abort
								///		command to send on cmd signal along with data that is sent or received
								///		on data line.
	NX_SDHC_INT_DRTO	=	9,	///< Data Read Timeout.\n
								///		Data timeout occurred. Data Transfer Over (NX_SDHC_INT_DTO) also set if data
								///		timeout occurs.
	NX_SDHC_INT_RTO		=	8,	///< Response Timeout.\n
								///		Response timeout occurred. Command Done (NX_SDHC_INT_CD) also set if response
								///		timeout occurs. If command involves data transfer and when response
								///		times out, no data transfer is attempted by SDHC module.
	NX_SDHC_INT_DCRC	=	7,	///< Data CRC Error.\n	
								///		Received Data CRC does not match with locally-generated CRC in SDHC module.
	NX_SDHC_INT_RCRC	=	6,	///< Response CRC Error.\n
								///		Response CRC does not match with locally-generated CRC in SDHC module.
	NX_SDHC_INT_RXDR	=	5,	///< Receive FIFO Data Request.\n
								///		Interrupt set during read operation from card when FIFO level is greater
								///		than Receive-Threshold level.\n\n
								///		\b Recommendation> In DMA modes, this interrupt should not be enabled.\n\n
								///		ISR, in non-DMA mode: \n
								///	@code
								///	pop RX_WMark + 1 data from FIFO
								///	@endcode
	NX_SDHC_INT_TXDR	=	4,	///< Transmit FIFO Data Request.\n
								///		Interrupt set during write operation to card when FIFO level reaches less
								///		than or equal to Transmit-Threshold level.\n\n
								///		\b Recommendation> In DMA modes, this interrupt should not be enabled.\n\n
								///		ISR in non-DMA mode: \n
								///	@code
								///	if (pending_bytes > (16 - TX_WMark))
								///		push (16 - TX_WMark) data into FIFO 
								///	else
								///		push pending_bytes data into FIFO
								///	@endcode
	NX_SDHC_INT_DTO		=	3,	///< Data Transfer Over.\n
								///		Data transfer completed, even if there is Start Bit Error or CRC error.
								///		This bit is also set when "read data-timeout" occurs.\n\n
								///		\b Recommendation> \n
								///		In non-DMA mode, when data is read from card, on seeing interrupt,
								///		host should read any pending data from FIFO.\n
								///		In DMA mode, DMA controllers guarantee FIFO is flushed before interrupt.\n\n
								///		\b Note> \n
								///		NX_SDHC_INT_DTO is set at the end of the last data block, even if
								///		the device asserts MMC busy after the last data block.
	NX_SDHC_INT_CD		=	2,	///< Command Done.\n
								///		Command sent to card and got response from card, even if Response
								///		Error or CRC error occurs. Also set when response timeout occurs.
	NX_SDHC_INT_RE		=	1	///< Response Error.\n
								///		Error in received response set if one of following occurs:
								///		- Transmission bit != 0
								///		- Command index mismatch
								///		- End-bit != 1
};

//------------------------------------------------------------------------------
/// @brief	Command Flags for NX_SDHC_SetCommand()
enum	NX_SDHC_CMDFLAG
{
	NX_SDHC_CMDFLAG_STARTCMD		= 1UL<<31,	///< Start Command. 
												///		Set a command with this flag to update modified settings to the
												///		SDHC module. If this flag is set, you have not to modify any
												///		any settings until the command is taken by the SDHC module.
	NX_SDHC_CMDFLAG_UPDATECLKONLY	= 1UL<<21,	///< Just update clock settings for SDCLK into card clock domain.\n
												///		Changes card clocks (change frequency, truncate off or on, and
												///		set low-frequency mode); provided in order to change clock
												///		frequency or stop clock without having to send command to
												///		cards.\n
												///		Even though this flag is set with NX_SDHC_CMDFLAG_STARTCMD flag,
												///		SDHC module does not issue NX_SDHC_INT_CD signal upon command completion.
	NX_SDHC_CMDFLAG_SENDINIT		= 1UL<<15,	///< Send initialization sequence before sending this command.\n
												///		After power on, 80 clocks must be sent to card for initialization
												///		before sending any commands to card. This flag should be set while
												///		sending first command to card so that controller will initialize
												///		clocks before sending command to card. 
	NX_SDHC_CMDFLAG_STOPABORT		= 1UL<<14,	///< Stop or abort command intended to stop current data transfer in progress.\n
												///		When open-ended or predefined data transfer is in progress, and
												///		host issues stop or abort command to stop data transfer, this flag should
												///		be set so that command/data state-machines of SDHC module can return
												///		correctly to idle state.
	NX_SDHC_CMDFLAG_WAITPRVDAT		= 1UL<<13,	///< Wait for previous data transfer completion before sending command.\n
												///		Command without this flag typically used to query 
												///		status of card during data transfer or to stop current data transfer.
	NX_SDHC_CMDFLAG_SENDAUTOSTOP	= 1UL<<12,	///< Send stop command at end of data transfer.\n
												///		When set, SDHC module sends stop command to card at end of data transfer. \n
												///		* when this flag should be set, since some data
												///		transfers do not need explicit stop commands.\n
												///		* open-ended transfers that software should explicitly send to
												///		stop command.\n
												///		Additionally, when "resume" is sent to resume . suspended
												///		memory access of SD-Combo card . this falg should be set correctly if
												///		suspended data transfer needs send_auto_stop.\n
												///		Don't care if no data expected from card.
	NX_SDHC_CMDFLAG_BLOCK			= 0UL<<11,	///< Block data transfer command.\n Don't care if no data expected from card.
	NX_SDHC_CMDFLAG_STREAM			= 1UL<<11,	///< Stream data transfer command.\n Don't care if no data expected from card.
	NX_SDHC_CMDFLAG_TXDATA			= 3UL<< 9,	///< Write to card.\n Do not set this flag if no data expected from card.
	NX_SDHC_CMDFLAG_RXDATA			= 1UL<< 9,	///< Read form card.\n Do not set this flag if no data expected from card.
	NX_SDHC_CMDFLAG_CHKRSPCRC		= 1UL<< 8,	///< Check response CRC.\n
												///		Some of command responses do not return valid CRC bits.
												///		Software should disable CRC checks for those commands in
												///		order to disable CRC checking by SDHC module.
	NX_SDHC_CMDFLAG_SHORTRSP		= 1UL<< 6,	///< Short response expected from card.\n Do not set this flag if no response is expected from card.
	NX_SDHC_CMDFLAG_LONGRSP			= 3UL<< 6	///< Long response expected from card.\n Do not set this flag if no response is expected from card.
};

//------------------------------------------------------------------------------
/// @brief	Command FSM state for NX_SDHC_GetCommandFSM().
typedef enum
{
	NX_SDHC_CMDFSM_IDLE				=	0,	///< Idle.
	NX_SDHC_CMDFSM_SENDINIT			=	1,	///< Send init sequence.
	NX_SDHC_CMDFSM_TXCMDSTART		=	2,	///< Tx command start bit
	NX_SDHC_CMDFSM_TXCMDTX			=	3,	///< Tx command tx bit.
	NX_SDHC_CMDFSM_TXCMDINDEXARG	=	4,	///< Tx command index + arg.
	NX_SDHC_CMDFSM_TXCMDCRC7		=	5,	///< Tx command CRC7.
	NX_SDHC_CMDFSM_TXCMDEND			=	6,	///< Tx command end bit.
	NX_SDHC_CMDFSM_RXRSPSTART		=	7,	///< Rx response start bit.
	NX_SDHC_CMDFSM_RXRSPIRQ			=	8,	///< Rx response IRQ response.
	NX_SDHC_CMDFSM_RXRSPTX			=	9,	///< Rx response tx bit.
	NX_SDHC_CMDFSM_TXRSPCMDIDX		= 10,	///< Rx response command index.
	NX_SDHC_CMDFSM_RXRSPDATA		= 11,	///< Rx response data.
	NX_SDHC_CMDFSM_RXRSPCRC7		= 12,	///< Rx response CRC7.
	NX_SDHC_CMDFSM_RXRSPEND			= 13,	///< Rx response end bit.
	NX_SDHC_CMDFSM_CMDWAITNCC		= 14,	///< Command path wait NCC.
	NX_SDHC_CMDFSM_WAIT				= 15	///< Wait; Comamnd to Response turnaround.
} NX_SDHC_CMDFSM;	

//------------------------------------------------------------------------------
/// @brief	SDHC Source Clock for Clock Generator.
enum	NX_SDHC_CLKSRC
{
	NX_SDHC_CLKSRC_PCLK	= 0,	///< PCLK.
	NX_SDHC_CLKSRC_PLL0	= 1,	///< PLL0.
	NX_SDHC_CLKSRC_PLL1	= 2		///< PLL1.
};


//------------------------------------------------------------------------------
/// @name	Module Interface
//------------------------------------------------------------------------------
//@{
CBOOL	NX_SDHC_Initialize( void );
U32		NX_SDHC_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//------------------------------------------------------------------------------
//@{
U32		NX_SDHC_GetPhysicalAddress( U32 ModuleIndex );
U32		NX_SDHC_GetSizeOfRegisterSet( void );

void	NX_SDHC_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress );
U32		NX_SDHC_GetBaseAddress( U32 ModuleIndex );

CBOOL	NX_SDHC_OpenModule( U32 ModuleIndex );
CBOOL	NX_SDHC_CloseModule( U32 ModuleIndex );

CBOOL	NX_SDHC_CheckBusy( U32 ModuleIndex );
CBOOL	NX_SDHC_CanPowerDown( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//------------------------------------------------------------------------------
//@{
S32		NX_SDHC_GetInterruptNumber( U32 ModuleIndex );

void	NX_SDHC_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable );
CBOOL	NX_SDHC_GetInterruptEnable( U32 ModuleIndex, S32 IntNum );
CBOOL	NX_SDHC_GetInterruptPending( U32 ModuleIndex, S32 IntNum );
void	NX_SDHC_ClearInterruptPending( U32 ModuleIndex, S32 IntNum );

void	NX_SDHC_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable );
CBOOL	NX_SDHC_GetInterruptEnableAll( U32 ModuleIndex );
CBOOL	NX_SDHC_GetInterruptPendingAll( U32 ModuleIndex );
void	NX_SDHC_ClearInterruptPendingAll( U32 ModuleIndex );

void	NX_SDHC_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag );
U32		NX_SDHC_GetInterruptEnable32( U32 ModuleIndex );
U32		NX_SDHC_GetInterruptPending32( U32 ModuleIndex );
void	NX_SDHC_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag );

S32		NX_SDHC_GetInterruptPendingNumber( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	DMA Interface
//------------------------------------------------------------------------------
//@{
U32		NX_SDHC_GetDMAIndex( U32 ModuleIndex );
U32		NX_SDHC_GetDMABusWidth( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//------------------------------------------------------------------------------
//@{
void		NX_SDHC_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode );
NX_PCLKMODE	NX_SDHC_GetClockPClkMode( U32 ModuleIndex );

void	NX_SDHC_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc );
U32		NX_SDHC_GetClockSource( U32 ModuleIndex, U32 Index );
void	NX_SDHC_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divisor );
U32		NX_SDHC_GetClockDivisor( U32 ModuleIndex, U32 Index );
void	NX_SDHC_SetClockDivisorEnable( U32 ModuleIndex, CBOOL Enable );
CBOOL	NX_SDHC_GetClockDivisorEnable( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Module customized operations
//------------------------------------------------------------------------------
//@{
void	NX_SDHC_AbortReadData( U32 ModuleIndex );
void	NX_SDHC_SendIRQResponse( U32 ModuleIndex );
void	NX_SDHC_SetReadWait( U32 ModuleIndex, CBOOL bAssert );

void	NX_SDHC_SetDMAMode( U32 ModuleIndex, CBOOL bEnb );

void	NX_SDHC_ResetDMA( U32 ModuleIndex );
CBOOL	NX_SDHC_IsResetDMA( U32 ModuleIndex );
void	NX_SDHC_ResetFIFO( U32 ModuleIndex );
CBOOL	NX_SDHC_IsResetFIFO( U32 ModuleIndex );
void	NX_SDHC_ResetController( U32 ModuleIndex );
CBOOL	NX_SDHC_IsResetController( U32 ModuleIndex );

void	NX_SDHC_SetOutputClockDivider( U32 ModuleIndex, U32 divider );
U32		NX_SDHC_GetOutputClockDivider( U32 ModuleIndex );
void	NX_SDHC_SetLowPowerClockMode( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_SDHC_GetLowPowerClockMode( U32 ModuleIndex );
void	NX_SDHC_SetOutputClockEnable( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_SDHC_GetOutputClockEnable( U32 ModuleIndex );

void	NX_SDHC_SetDataTimeOut( U32 ModuleIndex, U32 dwTimeOut );
U32		NX_SDHC_GetDataTimeOut( U32 ModuleIndex );
void	NX_SDHC_SetResponseTimeOut( U32 ModuleIndex, U32 dwTimeOut );
U32		NX_SDHC_GetResponseTimeOut( U32 ModuleIndex );

void	NX_SDHC_SetDataBusWidth( U32 ModuleIndex, U32 width );
U32		NX_SDHC_GetDataBusWidth( U32 ModuleIndex );

void	NX_SDHC_SetBlockSize( U32 ModuleIndex, U32 SizeInByte );
U32		NX_SDHC_GetBlockSize( U32 ModuleIndex );
void	NX_SDHC_SetByteCount( U32 ModuleIndex, U32 SizeInByte );
U32		NX_SDHC_GetByteCount( U32 ModuleIndex );

void	NX_SDHC_SetCommand( U32 ModuleIndex, U32 Cmd, U32 flag );
void	NX_SDHC_StartCommand( U32 ModuleIndex );
CBOOL	NX_SDHC_IsCommandBusy( U32 ModuleIndex );
void	NX_SDHC_SetCommandArgument( U32 ModuleIndex, U32 argument );

U32		NX_SDHC_GetShortResponse( U32 ModuleIndex );
void	NX_SDHC_GetLongResponse( U32 ModuleIndex, U32 *pLongResponse );
U32		NX_SDHC_GetAutoStopResponse( U32 ModuleIndex );
U32		NX_SDHC_GetResponseIndex( U32 ModuleIndex );

void	NX_SDHC_SetFIFORxThreshold( U32 ModuleIndex, U32 Threshold );
U32		NX_SDHC_GetFIFORxThreshold( U32 ModuleIndex );
void	NX_SDHC_SetFIFOTxThreshold( U32 ModuleIndex, U32 Threshold );
U32		NX_SDHC_GetFIFOTxThreshold( U32 ModuleIndex );

U32		NX_SDHC_GetFIFOCount( U32 ModuleIndex );
CBOOL	NX_SDHC_IsFIFOFull( U32 ModuleIndex );
CBOOL	NX_SDHC_IsFIFOEmpty( U32 ModuleIndex );
CBOOL	NX_SDHC_IsFIFOTxThreshold( U32 ModuleIndex );
CBOOL	NX_SDHC_IsFIFORxThreshold( U32 ModuleIndex );

U32		NX_SDHC_GetDataTransferSize( U32 ModuleIndex );
U32		NX_SDHC_GetFIFOTransferSize( U32 ModuleIndex );

void	NX_SDHC_SetData( U32 ModuleIndex, U32 dwData );
U32		NX_SDHC_GetData( U32 ModuleIndex );
void	NX_SDHC_SetData32( U32 ModuleIndex, const U32 *pdwData );
void	NX_SDHC_GetData32( U32 ModuleIndex, U32 *pdwData );
volatile U32*	NX_SDHC_GetDataPointer( U32 ModuleIndex );

CBOOL	NX_SDHC_IsDMAReq( U32 ModuleIndex );
CBOOL	NX_SDHC_IsDMAAck( U32 ModuleIndex );

CBOOL	NX_SDHC_IsDataTransferBusy( U32 ModuleIndex );
CBOOL	NX_SDHC_IsCardDataBusy( U32 ModuleIndex );
CBOOL	NX_SDHC_IsCardPresent( U32 ModuleIndex );

NX_SDHC_CMDFSM	NX_SDHC_GetCommandFSM( U32 ModuleIndex );
//@}


//------------------------------------------------------------------------------
#ifdef __arm	// for RVDS
#pragma diag_default 66		// return to default setting for #66 warning
#endif

//@}
// End of SDHC module
//------------------------------------------------------------------------------

#ifdef	__cplusplus
}
#endif

#endif // __NX_SDHC_H__
