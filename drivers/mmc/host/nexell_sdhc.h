#ifndef __SDMMC_H__
#define __SDMMC_H__

#define	 _DRIVER_				_T("DISK")
//------------------------------------------------------------------------------
//	MACRO
//------------------------------------------------------------------------------
#define BLOCK_IO_RETRY			5
#define STORAGE_ID_SIZE    		12
#define	SDMMC_POWER_STATE		D4

//------------------------------------------------------------------------------
//	Set SDHC clock source for sd/mmc card
//------------------------------------------------------------------------------
#define CCLK_IN_MAX			50000000	//ref. 14.3.4 "Clocks"
#define SD_INIT_FREQ			400000		//ref. SD spec.

#define ceil_div(x, y) \
	((x) + (y) - 1) / (y)

// use system PLL1 configuration
#define	SDHC_CLK_SRC_CLOCK		(2)    	// CLKSRCSEL0 : 0=PCLK, 1=PLL0, 2=PLL1
#define SDHC_CLK_SRC_FREQ		CFG_SYS_PLL1_FREQ

#define SDHC_CLK_SRC_DIVID		ceil_div(SDHC_CLK_SRC_FREQ, CCLK_IN_MAX)
#define CCLK_IN_FREQ			(CFG_SYS_PLL1_FREQ)/(SDHC_CLK_SRC_DIVID)

//------------------------------------------------------------------------------
//	Registers
//------------------------------------------------------------------------------
#define SDI_CTRL		0x000
#define SDI_PWREN		0x004
#define SDI_CLKDIV		0x008
#define SDI_CLKENA		0x010
#define SDI_TMOUT		0x014
#define SDI_CTYPE		0x018
#define SDI_BLKSIZ		0x01C
#define SDI_BYTCNT		0x020
#define SDI_INTMASK		0x024
#define SDI_CMDARG		0x028
#define SDI_CMD			0x02C
#define SDI_RESP0		0x030
#define SDI_RESP1		0x034
#define SDI_RESP2		0x038
#define SDI_RESP3		0x03C
#define SDI_MINTSTS		0x040
#define SDI_RINTSTS		0x044
#define SDI_STATUS		0x048
#define SDI_FIFOTH		0x04C
#define SDI_TCBCNT		0x05C
#define SDI_TBBCNT		0x060
#define SDI_DEBNCE		0x064
#define SDI_DAT			0x100
#define SDI_CICTRL		0x200
#define SDI_SYSCLKENB		0x7C0
#define SDI_CLKGEN		0x7C4

//CTRL
#define SDI_READ_WAIT	6
#define DMA_ENA_POS		5
#define SDI_INT_ENABLE	4
#define DMARST  		2	
#define FIFORST 		1	
#define CTRLRST 		0	

//CLKENA
#define SDI_CLK_ENABLE 	0x1	
#define LOWPWR 			1UL<<16

//CLKGEN
#define CLKSRCSEL_POS	2
#define CLKSRCSEL_MASK	0x1C
#define CLKDIV_POS  	5
#define CLKDIV_MASK 	0x3E0

//SYSCLKENB
#define SDI_CLK_GENENB	0x4			//(1<<2)
#define SDI_PCLKMODE	0x8			//(1<<3)

//FIFOTH
#define RXTH_POS  		16
#define RXTH_MASK 		0xFFFUL << RXTH_POS
#define TXTH_POS  		0
#define TXTH_MASK 		0xFFFUL << TXTH_POS

//CMD
#define STARTCMD_POS 	31

//STATUS
#define FSMBUSY_POS  	10
#define FSMBUSY_MASK 	1UL << FSMBUSY_POS
#define DATBUSY_POS  	9
#define DATBUSY_MASK 	1UL << DATBUSY_POS

//TMOUT
#define DTMOUT_POS  	8
#define DTMOUT_MASK 	0xFFFFFFUL<<DTMOUT_POS
#define RSPTMOUT_POS  	0
#define RSPTMOUT_MASK 	0xFFUL<<RSPTMOUT_POS

typedef enum
{
	SDHC_PCLKMODE_DYNAMIC = 0UL,	    ///< PCLK is provided only when CPU has access to registers of this module.
	SDHC_PCLKMODE_ALWAYS  = 1UL  	///< PCLK is always provided for this module.
} CL7700_SDHC_PCLKMODE;

typedef enum 
{
	SDHC_BCLKMODE_DISABLE  = 0UL,	///< BCLK is disabled.
	SDHC_BCLKMODE_DYNAMIC  = 2UL,	///< BCLK is provided only when this module requests it.
	SDHC_BCLKMODE_ALWAYS   = 3UL		///< BCLK is always provided for this module.
}CL7700_SDHC_BCLKMODE;

//------------------------------------------------------------------------------
/// @brief	SDHC Interrupts for interrupt interfaces
enum	CL7700_SDHC_INT
{
	SDHC_INT_SDIO	= 16,	///< SDIO Interrupt.\n
								///		Interrupt from SDIO card.
	SDHC_INT_EBE	= 15,	///< End Bit Error for reading, Write no CRC for wrting.\n
								///	 	Error in end-bit during read operations, or no data CRC or negative CRC
								///		received during write operation.\n\n
								///		\b Note> \n
								///		For MMC CMD19, there may be no CRC status returned by the card. 
								///		Hence, MES_SDHC_INT_EBE is set for CMD19. The application should not treat
								///	 	this as an error.
	SDHC_INT_ACD	= 14,	///< Auto Command Done.\n
								///		Stop/abort commands automatically sent by card unit and not initiated by
								///		host; similar to Command Done (MES_SDHC_INT_CD) interrupt.
	SDHC_INT_SBE	= 13,	///< Start Bit Error.\n
								///		Error in data start bit when data is read from a card. In 4-bit mode, if all
								///		data bits do not have start bit, then this error is set.
	SDHC_INT_HLE	= 12,	///< Hardware Locked Error.\n
								///		This interrupt is genearted when the SDHC module cannot load a command
								///	 	issued by the user. When the user sets a command with MES_SDHC_CMDFLAG_STARTCMD
								///		flag, the SDHC module tries to load the command. If the command buffer is
								///		already filled with a command, this error is raised.
								///		And also if the user try to modify any of settings while a command loading is in
								///		progress, then the modification is ignored and the SDHC module generates this
								///		error.
	SDHC_INT_FRUN	= 11,	///< FIFO underrun/overrun Error.\n
								///		Host tried to push data when FIFO was full, or host tried to read data
								///		when FIFO was empty. Typically this should not happen, except due to
								///		error in software.\n
								///		SDHC module never pushes data into FIFO when FIFO is full, and pop data
								///		when FIFO is empty.
	SDHC_INT_HTO	= 10,	///< Data Startvation by Host Timeout.\n
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
	SDHC_INT_DRTO	=  9,	///< Data Read Timeout.\n
								///		Data timeout occurred. Data Transfer Over (MES_SDHC_INT_DTO) also set if data
								///		timeout occurs.
	SDHC_INT_RTO	=  8,	///< Response Timeout.\n
								///		Response timeout occurred. Command Done (MES_SDHC_INT_CD) also set if response
								///		timeout occurs. If command involves data transfer and when response
								///		times out, no data transfer is attempted by SDHC module.
	SDHC_INT_DCRC	=  7,	///< Data CRC Error.\n	
								///		Received Data CRC does not match with locally-generated CRC in SDHC module.
	SDHC_INT_RCRC	=  6,	///< Response CRC Error.\n
								///		Response CRC does not match with locally-generated CRC in SDHC module.
	SDHC_INT_RXDR	=  5,	///< Receive FIFO Data Request.\n
								///		Interrupt set during read operation from card when FIFO level is greater
								///		than Receive-Threshold level.\n\n
								///		\b Recommendation> In DMA modes, this interrupt should not be enabled.\n\n
								///		ISR, in non-DMA mode: \n
								///	@code
								///	pop RX_WMark + 1 data from FIFO
								///	@endcode
	SDHC_INT_TXDR	=  4,	///< Transmit FIFO Data Request.\n
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
	SDHC_INT_DTO	=  3,	///< Data Transfer Over.\n
								///		Data transfer completed, even if there is Start Bit Error or CRC error.
								///		This bit is also set when "read data-timeout" occurs.\n\n
								///		\b Recommendation> \n
								///		In non-DMA mode, when data is read from card, on seeing interrupt,
								///		host should read any pending data from FIFO.\n
								///		In DMA mode, DMA controllers guarantee FIFO is flushed before interrupt.\n\n
								///		\b Note> \n
								///		MES_SDHC_INT_DTO is set at the end of the last data block, even if
								///		the device asserts MMC busy after the last data block.
	SDHC_INT_CD		=  2,	///< Command Done.\n
								///		Command sent to card and got response from card, even if Response
								///		Error or CRC error occurs. Also set when response timeout occurs.
	SDHC_INT_RE		=  1	///< Response Error.\n
								///		Error in received response set if one of following occurs:
								///		- Transmission bit != 0
								///		- Command index mismatch
								///		- End-bit != 1
};

//------------------------------------------------------------------------------
/// @brief	Command Flags for MES_SDHC_SetCommand()
enum	CL7700_SDHC_CMDFLAG
{
	SDHC_CMDFLAG_STARTCMD		= 1UL<<31,	///< Start Command. 
												///		Set a command with this flag to update modified settings to the
												///		SDHC module. If this flag is set, you have not to modify any
												///		any settings until the command is taken by the SDHC module.
	SDHC_CMDFLAG_UPDATECLKONLY	= 1UL<<21,	///< Just update clock settings for SDCLK into card clock domain.\n
												///		Changes card clocks (change frequency, truncate off or on, and
												///		set low-frequency mode); provided in order to change clock
												///		frequency or stop clock without having to send command to
												///		cards.\n
												///		Even though this flag is set with MES_SDHC_CMDFLAG_STARTCMD flag,
												///		SDHC module does not issue MES_SDHC_INT_CD signal upon command completion.
	SDHC_CMDFLAG_SENDINIT		= 1UL<<15,	///< Send initialization sequence before sending this command.\n
												///		After power on, 80 clocks must be sent to card for initialization
												///		before sending any commands to card. This flag should be set while
												///		sending first command to card so that controller will initialize
												///		clocks before sending command to card. 
	SDHC_CMDFLAG_STOPABORT		= 1UL<<14,	///< Stop or abort command intended to stop current data transfer in progress.\n
												///		When open-ended or predefined data transfer is in progress, and
												///		host issues stop or abort command to stop data transfer, this flag should
												///		be set so that command/data state-machines of SDHC module can return
												///		correctly to idle state.
	SDHC_CMDFLAG_WAITPRVDAT		= 1UL<<13,	///< Wait for previous data transfer completion before sending command.\n
												///		Command without this flag typically used to query 
												///		status of card during data transfer or to stop current data transfer.
	SDHC_CMDFLAG_SENDAUTOSTOP	= 1UL<<12,	///< Send stop command at end of data transfer.\n
												///		When set, SDHC module sends stop command to card at end of data transfer. \n
												///		* when this flag should be set, since some data
												///		transfers do not need explicit stop commands.\n
												///		* open-ended transfers that software should explicitly send to
												///		stop command.\n
												///		Additionally, when "resume" is sent to resume . suspended
												///		memory access of SD-Combo card . this falg should be set correctly if
												///		suspended data transfer needs send_auto_stop.\n
												///		Don't care if no data expected from card.
	SDHC_CMDFLAG_BLOCK			= 0UL<<11,	///< Block data transfer command.\n Don't care if no data expected from card.
	SDHC_CMDFLAG_STREAM			= 1UL<<11,	///< Stream data transfer command.\n Don't care if no data expected from card.
	SDHC_CMDFLAG_TXDATA			= 3UL<< 9,	///< Write to card.\n Do not set this flag if no data expected from card.
	SDHC_CMDFLAG_RXDATA			= 1UL<< 9,	///< Read form card.\n Do not set this flag if no data expected from card.
	SDHC_CMDFLAG_CHKRSPCRC		= 1UL<< 8,	///< Check response CRC.\n
												///		Some of command responses do not return valid CRC bits.
												///		Software should disable CRC checks for those commands in
												///		order to disable CRC checking by SDHC module.
	SDHC_CMDFLAG_SHORTRSP		= 1UL<< 6,	///< Short response expected from card.\n Do not set this flag if no response is expected from card.
	SDHC_CMDFLAG_LONGRSP		= 3UL<< 6	///< Long response expected from card.\n Do not set this flag if no response is expected from card.
};

//------------------------------------------------------------------------------
/// @brief	Command FSM state for MES_SDHC_GetCommandFSM().
typedef enum
{
	SDHC_CMDFSM_IDLE			=  0,	///< Idle.
	SDHC_CMDFSM_SENDINIT		=  1,	///< Send init sequence.
	SDHC_CMDFSM_TXCMDSTART		=  2,	///< Tx command start bit
	SDHC_CMDFSM_TXCMDTX			=  3,	///< Tx command tx bit.
	SDHC_CMDFSM_TXCMDINDEXARG	=  4,	///< Tx command index + arg.
	SDHC_CMDFSM_TXCMDCRC7		=  5,	///< Tx command CRC7.
	SDHC_CMDFSM_TXCMDEND		=  6,	///< Tx command end bit.
	SDHC_CMDFSM_RXRSPSTART		=  7,	///< Rx response start bit.
	SDHC_CMDFSM_RXRSPIRQ		=  8,	///< Rx response IRQ response.
	SDHC_CMDFSM_RXRSPTX			=  9,	///< Rx response tx bit.
	SDHC_CMDFSM_TXRSPCMDIDX		= 10,	///< Rx response command index.
	SDHC_CMDFSM_RXRSPDATA		= 11,	///< Rx response data.
	SDHC_CMDFSM_RXRSPCRC7		= 12,	///< Rx response CRC7.
	SDHC_CMDFSM_RXRSPEND		= 13,	///< Rx response end bit.
	SDHC_CMDFSM_CMDWAITNCC		= 14,	///< Command path wait NCC.
	SDHC_CMDFSM_WAIT			= 15	///< Wait; Comamnd to Response turnaround.
} CL7700_SDHC_CMDFSM;	

//------------------------------------------------------------------------------
/// @brief	SDHC Source Clock for Clock Generator.
enum	CL7700_SDHC_CLKSRC
{
	SDHC_CLKSRC_PCLK	= 0,	///< PCLK.
	SDHC_CLKSRC_PLL0	= 1,	///< PLL0.
	SDHC_CLKSRC_PLL1	= 2		///< PLL1.
};

//------------------------------------------------------------------------------
//	Set SD PIO number
//------------------------------------------------------------------------------
#ifdef ATTACH_SDMMC0
#define	SD_WP0			CFG_PIO_SD_0_WP
#define	SD_DETECT0		CFG_PIO_SD_0_DETECT
#else
#define	SD_WP0			PAD_NOT_USED
#define	SD_DETECT0		PAD_NOT_USED
#endif

#endif   // __SDMMC_H__
