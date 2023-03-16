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
//	Module		: DMA
//	File		: nx_dma.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_DMA_H__
#define __NX_DMA_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	DMA DMA
//------------------------------------------------------------------------------
//@{

	/// @brief	DMA Module's Register List
	struct	NX_DMA_RegisterSet
	{
		volatile U32	DMASRCADDR;		///< 0x00 : Source Address Register
		volatile U32	DMADSTADDR;		///< 0x04 :	Destination Address Register
		volatile U16	DMALENGTH;		///< 0x08 :	Transfer Length Register
		volatile U16	DMAREQID;		///< 0x0A : Peripheral ID Register
		volatile U32	DMAMODE;		///< 0x0C : DMA Mode Register
		volatile U32	DMASRCADDR_WB;	///< 0x10 : Source Address Write Back Register
		volatile U32	DMADSTADDR_WB;	///< 0x24 :	Destination Address Write Back Register
		volatile U16	DMALENGTH_WB;	///< 0x18 :	Transfer Length Write Back Register
		volatile U16	DMAREQID_WB;	///< 0x1A : Peripheral ID Write Back Register
		volatile U32	DMAMODE_WB;		///< 0x1C : DMA Mode Write Back Register
		volatile U32	DMACMDWAIT;		///< 0x20 : Command Wait Register
		volatile U32	DMACMDSTOP;		///< 0x24 : Command Stop Register
	//	volatile U32	DMACMDBUSY;		///< 0x28 : Command Busy Register
		volatile U32	Reserved;		///< 0x28 : Reserved
		volatile U32	DMACMDEMPTY;	///< 0x2C : Command Empty Space Register
	};

	/// @brief DMA interrupt for interrupt interface
	enum
	{
		NX_DMA_INT_TRANSFERDONE = 0		///< DMA Transfer Complete interrupt.
	};

	/// @brief Indicate DMA module's maximum transfer size
	enum NX_DMA_TS
	{
		NX_DMA_MAX_TRANSFER_SIZE		= 0x10000UL	///< Max Transfer Size of DMA Module
	};
	
	
	/// @brief Use for DMA's Operation Mode
	typedef enum
	{
		NX_DMA_OPMODE_MEM_TO_MEM	= 0UL,			///< Memory to Memory operation
		NX_DMA_OPMODE_MEM_TO_IO		= 1UL,			///< Memory to Peripheral operation
		NX_DMA_OPMODE_IO_TO_MEM		= 2UL			///< Peripheral to Memory operation
	
	} NX_DMA_OPMODE ;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{

CBOOL	NX_DMA_Initialize( void );
U32		NX_DMA_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{

U32		NX_DMA_GetPhysicalAddress( U32 ModuleIndex );
U32		NX_DMA_GetSizeOfRegisterSet( void );
void	NX_DMA_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress );
U32		NX_DMA_GetBaseAddress( U32 ModuleIndex );
CBOOL	NX_DMA_OpenModule( U32 ModuleIndex );
CBOOL	NX_DMA_CloseModule( U32 ModuleIndex );
CBOOL	NX_DMA_CheckBusy( U32 ModuleIndex );
CBOOL	NX_DMA_CanPowerDown( U32 ModuleIndex );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_DMA_GetInterruptNumber( U32 ModuleIndex );

void	NX_DMA_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable );
CBOOL	NX_DMA_GetInterruptEnable( U32 ModuleIndex, S32 IntNum );
CBOOL	NX_DMA_GetInterruptPending( U32 ModuleIndex, S32 IntNum );
void	NX_DMA_ClearInterruptPending( U32 ModuleIndex, S32 IntNum );

void	NX_DMA_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable );
CBOOL	NX_DMA_GetInterruptEnableAll( U32 ModuleIndex );
CBOOL	NX_DMA_GetInterruptPendingAll( U32 ModuleIndex );
void	NX_DMA_ClearInterruptPendingAll( U32 ModuleIndex );

void	NX_DMA_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag );
U32		NX_DMA_GetInterruptEnable32( U32 ModuleIndex );
U32		NX_DMA_GetInterruptPending32( U32 ModuleIndex );
void	NX_DMA_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag );

S32		NX_DMA_GetInterruptPendingNumber( U32 ModuleIndex );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	DMA Configuration Function
//@{
void	NX_DMA_TransferMemToMem( U32 ModuleIndex, const void* pSource, void* pDestination, U32 TransferSize );
void	NX_DMA_TransferMemToIO( U32 ModuleIndex,
								const void* pSource,
								U32 DestinationBaseAddress,
								U32 DestinationPeriID,
								U32 DestinationBitWidth,
								U32 TransferSize );
void	NX_DMA_TransferIOToMem( U32 ModuleIndex,
								U32 SourceBaseAddress,
								U32 SourcePeriID,
								U32 SourceBitWidth,
								void* pDestination,
								U32 TransferSize );

U32		NX_DMA_GetMaxTransferSize ( void );
void	NX_DMA_SetSourceAddress( U32 ModuleIndex, U32 dwAddr );
U32		NX_DMA_GetSourceAddress( U32 ModuleIndex );
void	NX_DMA_SetDestinationAddress( U32 ModuleIndex, U32 dwAddr );
U32		NX_DMA_GetDestinationAddress( U32 ModuleIndex );
void	NX_DMA_SetDataSize( U32 ModuleIndex, U32 dwSize );
U32		NX_DMA_GetDataSize( U32 ModuleIndex );
void	NX_DMA_SetIOIndex( U32 ModuleIndex, U32 dwIndex );
U32		NX_DMA_GetIOIndex( U32 ModuleIndex );
void	NX_DMA_SetAttribute( U32 ModuleIndex, NX_DMA_OPMODE OpMode, U32 IOBitWidth, CBOOL bSrcMemInc, CBOOL bDstMemInc );
//@}

//------------------------------------------------------------------------------
///	@name	DMA Operation Function
//@{
void	NX_DMA_SetCommandBufferMode( U32 ModuleIndex, CBOOL bEnb );
CBOOL	NX_DMA_GetCommandBufferMode( U32 ModuleIndex );
void	NX_DMA_Run( U32 ModuleIndex );
CBOOL	NX_DMA_CheckRunning ( U32 ModuleIndex );
void	NX_DMA_Stop ( U32 ModuleIndex, CBOOL Enable );
void	NX_DMA_CommandBufferFlush( U32 ModuleIndex );
CBOOL	NX_DMA_IsCommandBufferFlush( U32 ModuleIndex );
CBOOL	NX_DMA_IsCommandBufferEmpty( U32 ModuleIndex );
CBOOL	NX_DMA_IsCommandBufferReady( U32 ModuleIndex );
U32		NX_DMA_GetCommandBufferFreeCount( U32 ModuleIndex );
//@}

//@}

#ifdef	__cplusplus
}
#endif

#endif // __NX_DMA_H__
