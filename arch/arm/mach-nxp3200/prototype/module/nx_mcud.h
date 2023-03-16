//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: MCUD
//	File		: nx_mcud.h
//	Description	: 
//	Author		: 
//	History		: 
//------------------------------------------------------------------------------
#ifndef __NX_MCUD_H__
#define __NX_MCUD_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	MCUD	DRAM Memory Control Unit
//------------------------------------------------------------------------------
//@{

/// @brief	MCUD Module's Register List
struct	NX_MCUD_RegisterSet
{
	volatile U32 MEMCFG			;	///< 0x00 : Memory Configuration Register
	volatile U32 MEMTIME0		;	///< 0x04 : Memory Timing #0 Register
	volatile U32 MEMCTRL		;	///< 0x08 : Memory Control Register
	volatile U32 MEMACTPWD		;	///< 0x0C : Memory Active Power Down Control Register
	volatile U32 MEMTIME1		;	///< 0x10 : Memory Timing #1 Register
	volatile U32 __Reserved01[3];	///< 0x14 ~ 0x1C : Reserved
	volatile U32 FASTCH[3]		;	///< 0x20, 0x24, 0x28 : Memory Fast Arbiter Channel Registers
	volatile U32 SLOWCH[3]		;	///< 0x2C, 0x30, 0x34 : Memory Slow Arbiter Channel Registers
	volatile U32 INITPERFCNT	;	///< 0x38 : Memory Performance Count Init Value Register
	volatile U32 VALIDCNT[22]	;	///< 0x3C ~ 0x90 : Memory Counter Registers
	volatile U32 PHYDELAYCTRL	;	///< 0x94 : PHY Delay Control Register
	volatile U32 PHYDLLCTRL0	;	///< 0x98 : PHY DLL Control #0 Register
	volatile U32 PHYMODE		;	///< 0x9C : PHY Mode Register
	volatile U32 PHYDLLCTRL1	;	///< 0xA0 : PHY DLL Control #1 Register
	volatile U32 PHYDLLFORCE	;	///< 0xA4 : PHY DLL Force Register
	volatile U32 __Reserved02[1];	///< 0xA8 : Reserved
	volatile U32 PHYZQCTRL		;	///< 0xAC : PHY ZQ Control Register
	volatile U32 PHYZQFORCE		;	///< 0xB0 : PHY ZQ Force Register
	volatile U32 PHYTERMCTRL	;	///< 0xB4 : PHY Termination Control Register
	volatile U32 PHYUPDATE		;	///< 0xB8 : PHY Update Register
};
 
//------------------------------------------------------------------------------
/// @name	Module enumerators
//@{

/// @brief	DRAM Type
typedef enum 
{
	NX_MCUD_TYPE_DDR	= 1,	///< DDR-SDRAM
	NX_MCUD_TYPE_DDR2	= 2,	///< DDR2-SDRAM
	NX_MCUD_TYPE_MDDR	= 3		///< Mobile DDR-SDRAM
} NX_MCUD_TYPE;

/// @brief	Output driver impedance control ( Normal EMRS only )
typedef enum 
{
	NX_MCUD_DIC_NORMAL	= 0,	///< Normal driving
	NX_MCUD_DIC_WEAK	= 1		///< Weak driving
} NX_MCUD_DIC;

/// @brief	ODT(On Die Termination) resistance for DDR2-SDRAM.
typedef enum 
{
	NX_MCUDPHY_ODT_DISABLE	= 0,	///< Disable ODT
	NX_MCUDPHY_ODT_50		= 3,	///< 50 ohm
	NX_MCUDPHY_ODT_75		= 2,	///< 75 ohm
	NX_MCUDPHY_ODT_150		= 1		///< 150 ohm
} NX_MCUDPHY_ODT;

/// @brief	ODT(On Die Termination) resistance for DDR2-SDRAM.
typedef enum 
{
	NX_MCUDRAM_ODT_DISABLE	= 0,	///< Disable ODT
	NX_MCUDRAM_ODT_50		= 3,	///< 50 ohm
	NX_MCUDRAM_ODT_75		= 1,	///< 75 ohm
	NX_MCUDRAM_ODT_150		= 2		///< 150 ohm
} NX_MCUDRAM_ODT;

/// @brief	DQS mode control.
typedef enum 
{
	NX_MCUD_DQSMODE_DIFFER = 0,	///< The differential DQS mode
	NX_MCUD_DQSMODE_SINGLE = 1		///< The single ended DQS mode
} NX_MCUD_DQSMODE;

/// @brief	Fast arbiter master
typedef enum 
{
	NX_MCUD_FAST_MLC		= 0,	///< MLC 
	NX_MCUD_FAST_VIP		= 1,	///< VIP
	NX_MCUD_FAST_DISABLE	= 16	///< Disable
} NX_MCUD_FAST;

/// @brief	Slow arbiter master
typedef enum 
{
	NX_MUCD_SLOW_CPUI			=	0,	///< CPU instruction
	NX_MUCD_SLOW_CPUD			=	1,	///< CPU data
	NX_MUCD_SLOW_GRP3D			=	2,	///< 3D Graphic Engine
	NX_MUCD_SLOW_MPEGTSP		=	3,	///< MPEG-TS Demux
	NX_MUCD_SLOW_UHC			=	4,	///< USB Host Controller
	NX_MUCD_SLOW_CSC			=	5,	///< Color Space Converter
	NX_MUCD_SLOW_MPEG			=	6,	///< MPEG Engine
	NX_MUCD_SLOW_ROTATOR		=	7,	///< Rotator
	NX_MUCD_SLOW_SCALER			=	8,	///< Scaler
	NX_MUCD_SLOW_H264			=	9,	///< H.264 Engine
	NX_MUCD_SLOW_VLC			= 10,	///< VLC
	NX_MUCD_SLOW_DEINTERLACE	= 11,	///< Deinterlace
	NX_MUCD_SLOW_DMAGROUP0		= 12,	///< DMA Group 0
	NX_MUCD_SLOW_DMAGROUP1		= 13,	///< DMA Group 1
	NX_MUCD_SLOW_DMAGROUP2		= 14,	///< DMA Group 2
	NX_MUCD_SLOW_DMAGROUP3		= 15,	///< DMA Group 3
	NX_MCUD_SLOW_DISABLE		= 16	///< Disable
} NX_MCUD_SLOW;

/// @brief	MCU Performance Counter Index
typedef enum
{
	NX_MCUD_COUNTER_MEMTOTAL		=	0,	///< Memory total count
	NX_MCUD_COUNTER_MEMWAIT			=	1,	///< Memory wait count
	NX_MCUD_COUNTER_MEMVALID		=	2,	///< Memory valid count
	NX_MCUD_COUNTER_MLC				=	3,	///< MLC valid count
	NX_MCUD_COUNTER_VIP				=	4,	///< VIP valid count
	NX_MCUD_COUNTER_CDDEC			=	5,	///< CD-ROM Decoder valid count
	NX_MCUD_COUNTER_CPUD			=	6,	///< CPU data valid count
	NX_MCUD_COUNTER_CPUI			=	7,	///< CPU inst valid count
	NX_MCUD_COUNTER_GRP3D			=	8,	///< 3D valid count
	NX_MCUD_COUNTER_MPEGTSP			=	9,	///< MPEG-TSP valid count
	NX_MCUD_COUNTER_UHC				= 10,	///< UHC valid count
	NX_MCUD_COUNTER_CSC				= 11,	///< CSC valid count
	NX_MCUD_COUNTER_MPEG			= 12,	///< MPEG valid count
	NX_MCUD_COUNTER_ROTATOR			= 13,	///< ROTATOR valid count
	NX_MCUD_COUNTER_SCALER			= 14,	///< SCALER valid count
	NX_MCUD_COUNTER_H264			= 15,	///< H.264 valid count
	NX_MCUD_COUNTER_VLC				= 16,	///< VLC valid count
	NX_MCUD_COUNTER_DEINTERLACE		= 17,	///< DEINTERLACE valid count
	NX_MCUD_COUNTER_DMAGROUP0		= 18,	///< DMA Group 0 valid count
	NX_MCUD_COUNTER_DMAGROUP1		= 19,	///< DMA Group 1 valid count
	NX_MCUD_COUNTER_DMAGROUP2		= 20,	///< DMA Group 2 valid count
	NX_MCUD_COUNTER_DMAGROUP3		= 21	///< DMA Group 3 valid count
} NX_MCUD_COUNTER;

//@} 
 
//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_MCUD_Initialize( void );
U32		NX_MCUD_GetNumberOfModule( void );
//@} 

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_MCUD_GetPhysicalAddress( void );
U32		NX_MCUD_GetSizeOfRegisterSet( void );
void	NX_MCUD_SetBaseAddress( U32 BaseAddress );
U32		NX_MCUD_GetBaseAddress( void );
CBOOL	NX_MCUD_OpenModule( void );
CBOOL	NX_MCUD_CloseModule( void );
CBOOL	NX_MCUD_CheckBusy( void );
CBOOL	NX_MCUD_CanPowerDown( void );
//@} 

//--------------------------------------------------------------------------
///	@name	MCU Control Settings
//--------------------------------------------------------------------------
//@{
void	NX_MCUD_SetEnable( CBOOL bEnb );
CBOOL	NX_MCUD_GetEnable( void );

void	NX_MCUD_SetDLLReset( CBOOL bEnb );
CBOOL	NX_MCUD_GetDLLReset( void );

void	NX_MCUD_SetMCLKConfig( CBOOL bDouble, CBOOL bHighSpeed );
void	NX_MCUD_GetMCLKConfig( CBOOL *pbDouble, CBOOL *pbHighSpeed );

void	NX_MCUD_ApplyModeSetting ( void );
CBOOL	NX_MCUD_IsBusyModeSetting( void );
//@}

//--------------------------------------------------------------------------
///	@name	Memory type settings
//--------------------------------------------------------------------------
//@{
void			NX_MCUD_SetType( NX_MCUD_TYPE type );			
NX_MCUD_TYPE	NX_MCUD_GetType( void );							

void	NX_MCUD_SetBusWidth ( U32 buswidth );
U32		NX_MCUD_GetBusWidth ( void );
void	NX_MCUD_SetDataWidth( U32 datawidth ); 
U32		NX_MCUD_GetDataWidth( void );
void	NX_MCUD_SetCapacity ( U32 capacity );
U32		NX_MCUD_GetCapacity ( void );
//@}

//--------------------------------------------------------------------------
///	@name	Mode Register Settings
//--------------------------------------------------------------------------
//@{
void	NX_MCUD_SetCASLatency( U32 cycle );
U32		NX_MCUD_GetCASLatency( void );
void	NX_MCUD_SetAdditiveLatency( U32 cycle );
U32		NX_MCUD_GetAdditiveLatency( void );
void	NX_MCUD_SetDelayLatency( U32 cycle );
U32		NX_MCUD_GetDelayLatency( void );

void			NX_MCUD_SetDIC( NX_MCUD_DIC dic );
NX_MCUD_DIC		NX_MCUD_GetDIC( void );

void			NX_MCUD_SetODT( NX_MCUDRAM_ODT ODT );
NX_MCUDRAM_ODT		NX_MCUD_GetODT( void );

void			NX_MCUD_SetDQSMode( NX_MCUD_DQSMODE DQSmode );
NX_MCUD_DQSMODE	NX_MCUD_GetDQSMode( void );

void	NX_MCUD_SetDLLEnable( CBOOL bEnb );
CBOOL	NX_MCUD_GetDLLEnable ( void );
//@}

//--------------------------------------------------------------------------
///	@name	Timing settings
//--------------------------------------------------------------------------
//@{
void	NX_MCUD_SetMRD( U32 clocks );
U32		NX_MCUD_GetMRD( void );

void	NX_MCUD_SetRP ( U32 clocks );
U32		NX_MCUD_GetRP ( void );

void	NX_MCUD_SetRCD( U32 clocks );
U32		NX_MCUD_GetRCD( void );

void	NX_MCUD_SetRAS( U32 clocks );
U32		NX_MCUD_GetRAS( void );

void	NX_MCUD_SetWR ( U32 clocks );
U32		NX_MCUD_GetWR ( void );

void	NX_MCUD_SetRFC ( U32 clocks );
U32		NX_MCUD_GetRFC ( void );

void	NX_MCUD_SetWTR ( U32 clocks );
U32		NX_MCUD_GetWTR ( void );

void	NX_MCUD_SetRTP ( U32 clocks );
U32		NX_MCUD_GetRTP ( void );
//@}

//------------------------------------------------------------------------------
///	@name	Active power down settings
//------------------------------------------------------------------------------
//@{
void	NX_MCUD_SetActivePowerDownEnable( CBOOL bEnb );
CBOOL	NX_MCUD_GetActivePowerDownEnable( void );
void	NX_MCUD_SetActivePowerDownPeriod( U32 period );
U32		NX_MCUD_GetActivePowerDownPeriod( void );
//@}

//------------------------------------------------------------------------------
///	@name	Refresh period settings
//------------------------------------------------------------------------------
//@{
void	NX_MCUD_SetRefreshPeriod( U32 period );
U32		NX_MCUD_GetRefreshPeriod( void );
//@}

//------------------------------------------------------------------------------
///	@name	PHY DLL Control Functions
//------------------------------------------------------------------------------
//@{
void	NX_MCUD_PHYDLL_SetAutoMode( CBOOL bEnb );
CBOOL	NX_MCUD_PHYDLL_GetAutoMode( void );

void	NX_MCUD_PHYDLL_SetConfig( CBOOL bLowSpeed, U32 dwStart, U32 dwInc );
void	NX_MCUD_PHYDLL_GetConfig( CBOOL *pbLowSpeed, U32 *pdwStart, U32 *pdwInc );

void	NX_MCUD_PHYDLL_Run( void );
void	NX_MCUD_PHYDLL_Stop( void );
CBOOL	NX_MCUD_PHYDLL_IsRun( void );

CBOOL	NX_MCUD_PHYDLL_IsLockCoarse( void );
CBOOL	NX_MCUD_PHYDLL_IsLockFine( void );

U32		NX_MCUD_PHYDLL_GetAutoDelay( U32 *pCoarseDelay, U32 *pFineDelay );

void	NX_MCUD_PHYDLL_SetManualDelay( U32 CoarseDelay );
U32		NX_MCUD_PHYDLL_GetManualDelay( void );

void	NX_MCUD_PHYDLL_SetDQSDelay( int Offset0, int Offset1 );
void	NX_MCUD_PHYDLL_GetDQSDelay( int *pOffset0, int *pOffset1 );
//@}

//------------------------------------------------------------------------------
///	@name	PHY ZQ Control Functions
//------------------------------------------------------------------------------
//@{
void	NX_MCUD_PHYZQ_SetAutoMode( CBOOL bEnb );
CBOOL	NX_MCUD_PHYZQ_GetAutoMode( void );

void		NX_MCUD_PHYZQ_SetODT( NX_MCUDPHY_ODT ODT );
NX_MCUDPHY_ODT	NX_MCUD_PHYZQ_GetODT( void );

void	NX_MCUD_PHYZQ_SetControlCode( U32 PullUp, U32 PullDown );
void	NX_MCUD_PHYZQ_GetControlCode( U32 *pPullUp, U32 *pPullDown );

void	NX_MCUD_PHYZQ_GetAutoResult( U32 *pPullUp, U32 *pPullDown );

void	NX_MCUD_PHYZQ_Run( void );
void	NX_MCUD_PHYZQ_Stop( void );
CBOOL	NX_MCUD_PHYZQ_IsRun( void );

CBOOL	NX_MCUD_PHYZQ_IsError( void );
CBOOL	NX_MCUD_PHYZQ_IsCompleted( void );

void	NX_MCUD_PHYZQ_Update( void );
//@}

//------------------------------------------------------------------------------
///	@name	Memory Arbiter Settings
//------------------------------------------------------------------------------
//@{
void			NX_MCUD_SetFastArbiter( U32 channel, NX_MCUD_FAST master );
NX_MCUD_FAST	NX_MCUD_GetFastArbiter( U32 channel );
void			NX_MCUD_SetSlowArbiter( U32 channel, NX_MCUD_SLOW master );
NX_MCUD_SLOW	NX_MCUD_GetSlowArbiter( U32 channel );
//@}

//------------------------------------------------------------------------------
///	@name	Performance Counter Functions
//------------------------------------------------------------------------------
//@{
void	NX_MCUD_SetInitPerformanceCounter( U32 initvalue );
U32		NX_MCUD_GetInitPerformanceCounter( void );
U32		NX_MCUD_GetPerformanceCounter( NX_MCUD_COUNTER index );
//@}


//@} 

#ifdef	__cplusplus
}
#endif


#endif // __NX_MCUD_H__
