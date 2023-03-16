/*------------------------------------------------------------------------------
 *
 *	Copyright (C) 2005 Nexell Co., Ltd All Rights Reserved
 *	Nexell Co. Proprietary & Confidential
 *
 *	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 *  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 *  FOR A PARTICULAR PURPOSE.
 *
 *	Module     : System memory config
 *	Description:
 *	Author     : Platform Team
 *	Export     :
 *	History    :
 *	   2009/05/13 first implementation
 ------------------------------------------------------------------------------*/
#ifndef __CFG_SYS_H__
#define __CFG_SYS_H__

#include <nx_type.h>


//------------------------------------------------------------------------------
// IMPORTANT : You should set a PMS value carefully.
//             Below setting is from Samsung is manufacture of PLL core.
//			   Contact Nexell or Samsung if you wish to change it.
//------------------------------------------------------------------------------
// PLL 0/1 setting
//------------------------------------------------------------------------------
// Fout = (M*Fin) / (P*S), where P = PDIV, M = MDIV, S = 2^SDIV
//------------------------------------------------------------------------------
#define	CFG_SYS_PLL_PMSFUNC(_FIN_, _P_, _M_, _S_)			\
	((_M_) * ((_FIN_)/((_P_)*(1UL<<(_S_)))))

//------------------------------------------------------------------------------
// PLL input crystal
//------------------------------------------------------------------------------
#define	CFG_SYS_PLLFIN		12000000UL

//------------------------------------------------------------------------------
// PLL0 PMS :
//------------------------------------------------------------------------------
// +-------------+-----+-----+-----+
// |    Output   |  P  |  M  |  S  |
// +-------------+-----+-----+-----+
// | 528,000,000 |   3 | 264 |   1 | default
// | 534,000,000 |   3 | 267 |   1 | default
// +-------------+-----+-----+-----+
//------------------------------------------------------------------------------
#define	CFG_SYS_PLL0_P		3		//  1 <=, <= 63
//#define	CFG_SYS_PLL0_M		400		// 64 <=, <= 1023
#define	CFG_SYS_PLL0_M		350		// 64 <=, <= 1023
//#define	CFG_SYS_PLL0_M		534		// 64 <=, <= 1023
//#define	CFG_SYS_PLL0_M		133		// 64 <=, <= 1023
#define	CFG_SYS_PLL0_S		1		// 0, 1, 2, 3
#define	CFG_SYS_PLL0_FREQ	CFG_SYS_PLL_PMSFUNC(CFG_SYS_PLLFIN, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S)

//------------------------------------------------------------------------------
// PLL1 PMS : 147,500,000 Hz    P=3, M=295, S=3 (default)
// PLL1 PMS : 192,000,000 Hz    P=2, M=256, S=3
// PLL1 PMS : 266,000,000 Hz    P=3, M=532, S=3
// PLL1 PMS : 275,000,000 Hz    P=3, M=550, S=3
// PLL1 PMS : 333,000,000 Hz    P=3, M=666, S=3
// PLL1 PMS : 400,000,000 Hz    P=3, M=800, S=3
//------------------------------------------------------------------------------

#if  defined(CONFIG_550_PLL1_147P5_MHZ)
#define CFG_SYS_PLL1_USE	CTRUE
#define	CFG_SYS_PLL1_P		3		/*  1 <=, <= 63 */
#define	CFG_SYS_PLL1_M		295		/* 56 <=, <= 1023 */
#define	CFG_SYS_PLL1_S		3		/* 0, 1, 2, 3, 4 */
#define	CFG_SYS_PLL1_FREQ	CFG_SYS_PLL_PMSFUNC(CFG_SYS_PLLFIN, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S)

#elif defined(CONFIG_PLL1_266_MHZ)
#define CFG_SYS_PLL1_USE	CTRUE
#define	CFG_SYS_PLL1_P		3		/*  1 <=, <= 63 */
#define	CFG_SYS_PLL1_M		532		/* 56 <=, <= 1023 */
#define	CFG_SYS_PLL1_S		3		/* 0, 1, 2, 3, 4 */
#define	CFG_SYS_PLL1_FREQ	CFG_SYS_PLL_PMSFUNC(CFG_SYS_PLLFIN, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S)

#elif defined(CONFIG_PLL1_275_MHZ)
#define CFG_SYS_PLL1_USE	CTRUE
#define	CFG_SYS_PLL1_P		3		/*  1 <=, <= 63 */
#define	CFG_SYS_PLL1_M		550		/* 56 <=, <= 1023 */
#define	CFG_SYS_PLL1_S		3		/* 0, 1, 2, 3, 4 */
#define	CFG_SYS_PLL1_FREQ	CFG_SYS_PLL_PMSFUNC(CFG_SYS_PLLFIN, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S)

#elif defined(CONFIG_PLL1_333_MHZ)
#define CFG_SYS_PLL1_USE	CTRUE
#define	CFG_SYS_PLL1_P		3		/*  1 <=, <= 63 */
#define	CFG_SYS_PLL1_M		666		/* 56 <=, <= 1023 */
#define	CFG_SYS_PLL1_S		3		/* 0, 1, 2, 3, 4 */
#define	CFG_SYS_PLL1_FREQ	CFG_SYS_PLL_PMSFUNC(CFG_SYS_PLLFIN, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S)

#elif defined(CONFIG_PLL1_400_MHZ)
#define CFG_SYS_PLL1_USE	CTRUE
#define	CFG_SYS_PLL1_P		3		/*  1 <=, <= 63 */
#define	CFG_SYS_PLL1_M		800		/* 56 <=, <= 1023 */
#define	CFG_SYS_PLL1_S		3		/* 0, 1, 2, 3, 4 */
#define	CFG_SYS_PLL1_FREQ	CFG_SYS_PLL_PMSFUNC(CFG_SYS_PLLFIN, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S)

#else
#error DDR speed not configured, expected CONFIG_PLL1_yyy_MHZ setting
#endif

//------------------------------------------------------------------------------
// Setting for System Source Clock
//------------------------------------------------------------------------------
typedef enum {
	CFG_SYS_CLKSRC_PLL0 = 0,
	CFG_SYS_CLKSRC_PLL1 = 1,
	CFG_SYS_CLKSRC_FCLK = 2,
	CFG_SYS_CLKSRC_MCLK = 3,
	CFG_SYS_CLKSRC_BCLK = 4,
	CFG_SYS_CLKSRC_PCLK = 5,
} cfg_sys_clksrc;

extern u_int cpu_get_clock_hz(cfg_sys_clksrc clk);

#if 0	// sesters: removed as the DDR and CPU speeds are set by U-Boot
//------------------------------------------------------------------------------
// CPU clock
//------------------------------------------------------------------------------
// - core clock = Source clock / COREDIV, where COREDIV = 1 ~ 16
// - bus clock  = core clock   /  BUSDIV, where  BUSDIV = 1 ~ 16
//------------------------------------------------------------------------------
#define	CFG_SYS_CPU_CLKSRC			CFG_SYS_CLKSRC_PLL0
#define	CFG_SYS_CPU_COREDIV			1
#define	CFG_SYS_CPU_BUSDIV_550			4
#define CFG_SYS_CPU_BUSDIV_700			4
#define	CFG_SYS_CPU_BUSDIV_800			4

//------------------------------------------------------------------------------
// System clock
//------------------------------------------------------------------------------
// MCLK = Source clock / MCLK_DIV, where MCLK_DIV = 1 ~ 16
// BCLK = MCLK / BCLK_DIV, where BCLK_DIV = 1 ~ 16
// PCLK = BCLK / PCLK_DIV, where PCLK_DIV = 1 ~ 16
//------------------------------------------------------------------------------
//#define	CFG_SYS_MCLK_CLKSRC			CFG_SYS_CLKSRC_FCLK
#define	CFG_SYS_MCLK_CLKSRC			CFG_SYS_CLKSRC_PLL0
#define	CFG_SYS_MCLK_DIV			2
#define	CFG_SYS_BCLK_DIV			2
#define	CFG_SYS_PCLK_DIV			2
#define CFG_SYS_MCLK_FREQ			(CFG_SYS_PLL0_FREQ/CFG_SYS_MCLK_DIV)
#define	CFG_SYS_BCLK_FREQ			(CFG_SYS_MCLK_FREQ/CFG_SYS_BCLK_DIV)
#define	CFG_SYS_PCLK_FREQ			(CFG_SYS_BCLK_FREQ/CFG_SYS_PCLK_DIV)
#endif

//------------------------------------------------------------------------------
// MCU-D
//------------------------------------------------------------------------------
#define CFG_SYS_MCUD_DRAMTYPE			(2)		// DRAM type : 1(DDR), 2(DDR2), 3(MDDR)
#define CFG_SYS_MCUD_DRAMBUSWIDTH		(8)	// DRAM bus width : 8 or 16
#define CFG_SYS_MCUD_DRAMCAPACITY		(1024)	// DRAM capacity in Mbits : 256, 512 or 1024

enum
{
	CFG_SYS_MCUD_NDQSUSE	= CTRUE,	// CTRUE(Differental DQS mode for DDR2), CFALSE(Single ended DQS mode for DDR)
	CFG_SYS_MCUD_USEDLL		= CTRUE,	// DLL control : CTRUE(enable), CFALSE(disable)
	CFG_SYS_MCUD_DIC		= 0,		// Output Drive Impedance : 0(Normal), 1(Weak)
	CFG_SYS_MCUD_ODT		= 0,		// ODT(On-Die-Termination) resistance : 0(Disable), 1(75ohm), 2(150ohm), 3(50ohm)

	CFG_SYS_MCUD_CASLAT		=  6,		// CAS latency 				: 2 ~ 3 (DDR), 2 ~ 6 (DDR2)
	CFG_SYS_MCUD_ADDLAT		=  0,		// Additive latency 		: 0 ~ 4 (DDR2 only)
	CFG_SYS_MCUD_DLYLAT		=  3,		// Read dealy latency 		: 1 ~   5, CAS latency for the reading = CASLAT + DLYLAT -> 1(199Mhz>=), 2(299Mhz>=), 3(400Mhz>=)
	CFG_SYS_MCUD_TRCD		=  6,		// RAS to CAS delay         : 1 ~  16 clocks
	CFG_SYS_MCUD_TRP		=  6,		// Row Prechage Time        : 1 ~  16 clocks
	CFG_SYS_MCUD_TRAS		= 18,		// Row Active Time          : 1 ~ 256 clocks
	CFG_SYS_MCUD_TMRD		=  3,		// Mode Register Set Cycle  : 1 ~  16 clocks
	CFG_SYS_MCUD_TWR		=  6,		// Write Recovery Time      : 1 ~  16 clocks
	CFG_SYS_MCUD_TWTR		=  7,		// Write To Read time		: 1 ~  16 clocks : CL + AL + tWTR(DRAM) - 2
	CFG_SYS_MCUD_TRTP		=  3,		// Read To Precharge time	: 1 ~  16 clocks
	CFG_SYS_MCUD_TRFC		= 51,		// auto Refresh to RAS time	: 1 ~ 256 clocks

	CFG_SYS_MCUD_ACTPDENB	= CFALSE,	// Active Powerdown Enable	: CTRUE(enable), CFALSE(disable)
	CFG_SYS_MCUD_ACTPDPRED	= 256,		// Active Powerdown Period	: 1 ~ 256 clocks

	CFG_SYS_MCUD_REFPRED	= 256		// Refresh period			: 0(disable), 1 ~ 65535 : Actual Refresh period = (period * 2) + 1 + tRP
};

// PHY ZQ
#define	CFG_SYS_MCUD_PHYZQ_USE			(CTRUE)
#define	CFG_SYS_MCUD_PHYZQ_AUTOMODE		(CTRUE)
#define	CFG_SYS_MCUD_PHYZQ_ODT			(1)			// ODT resistor value : 0(Disable), 1(75ohm), 2(150ohm), 3(50ohm)
#define	CFG_SYS_MCUD_PHYZQ_PULLUP		(2)			// The immediate control code for Pull-up   : 0 ~ 7 (2 is recommended)
#define	CFG_SYS_MCUD_PHYZQ_PULLDN		(5)			// The immediate control code for Pull-down : 0 ~ 7 (5 is recommended)

// PHY DLL
#define	CFG_SYS_MCUD_PHYDLL_USE			(CTRUE)
#define	CFG_SYS_MCUD_PHYDLL_AUTOMODE	(CFALSE)
#define	CFG_SYS_MCUD_PHYDLL_DQSOFFSET	(0)			// The delay of DQS singnal : -64 ~ +63
#define	CFG_SYS_MCUD_PHYDLL_MANUALDELAY	(127)		// The amount of the delay cells : 0 ~ 255


//------------------------------------------------------------------------------
//	Fast channel arbiter master
enum
{
	CFG_SYS_MCUD_FAST_MLC		= 0,	// MLC
	CFG_SYS_MCUD_FAST_VIP		= 1,	// VIP
	CFG_SYS_MCUD_FAST_DISABLE	= 16	// Disable
};

#define	CFG_SYS_MCUD_FASTCH_00		CFG_SYS_MCUD_FAST_MLC
#define	CFG_SYS_MCUD_FASTCH_01		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_02		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_03		CFG_SYS_MCUD_FAST_DISABLE

#define	CFG_SYS_MCUD_FASTCH_04		CFG_SYS_MCUD_FAST_VIP
#define	CFG_SYS_MCUD_FASTCH_05		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_06		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_07		CFG_SYS_MCUD_FAST_DISABLE

#define	CFG_SYS_MCUD_FASTCH_08		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_09		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_10		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_11		CFG_SYS_MCUD_FAST_DISABLE

#define	CFG_SYS_MCUD_FASTCH_12		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_13		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_14		CFG_SYS_MCUD_FAST_DISABLE
#define	CFG_SYS_MCUD_FASTCH_15		CFG_SYS_MCUD_FAST_DISABLE

//------------------------------------------------------------------------------
//	Slow channel arbiter master
enum
{
	CFG_SYS_MUCD_SLOW_CPUI			=  0,	// CPU instruction
	CFG_SYS_MUCD_SLOW_CPUD			=  1,	// CPU data
	CFG_SYS_MUCD_SLOW_GRP3D			=  2,	// 3D Graphic Engine
	CFG_SYS_MUCD_SLOW_MPEGTSP		=  3,	// MPEG-TS Demux
	CFG_SYS_MUCD_SLOW_UHC			=  4,	// USB Host Controller
	CFG_SYS_MUCD_SLOW_CSC			=  5,	// Color Space Converter
	CFG_SYS_MUCD_SLOW_MPEG			=  6,	// MPEG Engine
	CFG_SYS_MUCD_SLOW_ROTATOR		=  7,	// Rotator
	CFG_SYS_MUCD_SLOW_SCALER		=  8,	// Scaler
	CFG_SYS_MUCD_SLOW_H264			=  9,	// H.264 Engine
	CFG_SYS_MUCD_SLOW_VLC			= 10,	// VLC
	CFG_SYS_MUCD_SLOW_DEINTERLACE	= 11,	// Deinterlace
	CFG_SYS_MUCD_SLOW_DMAGROUP0		= 12,	// DMA Group 0
	CFG_SYS_MUCD_SLOW_DMAGROUP1		= 13,	// DMA Group 1
	CFG_SYS_MUCD_SLOW_DMAGROUP2		= 14,	// DMA Group 2
	CFG_SYS_MUCD_SLOW_DMAGROUP3		= 15,	// DMA Group 3
	CFG_SYS_MCUD_SLOW_DISABLE		= 16	// Disable
};

#define	CFG_SYS_MCUD_SLOWCH_00		CFG_SYS_MUCD_SLOW_DMAGROUP0
#define	CFG_SYS_MCUD_SLOWCH_01		CFG_SYS_MUCD_SLOW_CPUI
#define	CFG_SYS_MCUD_SLOWCH_02		CFG_SYS_MUCD_SLOW_CPUD
#define	CFG_SYS_MCUD_SLOWCH_03		CFG_SYS_MUCD_SLOW_GRP3D

#define	CFG_SYS_MCUD_SLOWCH_04		CFG_SYS_MUCD_SLOW_DMAGROUP1
#define	CFG_SYS_MCUD_SLOWCH_05		CFG_SYS_MUCD_SLOW_MPEGTSP
#define	CFG_SYS_MCUD_SLOWCH_06		CFG_SYS_MUCD_SLOW_UHC
#define	CFG_SYS_MCUD_SLOWCH_07		CFG_SYS_MUCD_SLOW_CSC

#define	CFG_SYS_MCUD_SLOWCH_08		CFG_SYS_MUCD_SLOW_DMAGROUP2
#define	CFG_SYS_MCUD_SLOWCH_09		CFG_SYS_MUCD_SLOW_MPEG
#define	CFG_SYS_MCUD_SLOWCH_10		CFG_SYS_MUCD_SLOW_ROTATOR
#define	CFG_SYS_MCUD_SLOWCH_11		CFG_SYS_MUCD_SLOW_SCALER

#define	CFG_SYS_MCUD_SLOWCH_12		CFG_SYS_MUCD_SLOW_DMAGROUP3
#define	CFG_SYS_MCUD_SLOWCH_13		CFG_SYS_MUCD_SLOW_H264
#define	CFG_SYS_MCUD_SLOWCH_14		CFG_SYS_MUCD_SLOW_VLC
#define	CFG_SYS_MCUD_SLOWCH_15		CFG_SYS_MUCD_SLOW_DEINTERLACE

//------------------------------------------------------------------------------
// Static Bus #0 ~ #9, NAND, IDE configuration
//------------------------------------------------------------------------------
//	_BW	  : Static Bus width for Static #0 ~ #9           : 8 or 16
//
//	_TACS : address setup time before chip select         : 0 ~ 15
//	_TCOS : chip select setup time before nOE is asserted : 0 ~ 15
//	_TACC : access cycle                                  : 1 ~ 256
//	_TSACC: burst access cycle for Static #0 ~ #9 & IDE   : 1 ~ 256
//	_TOCH : chip select hold time after nOE not asserted  : 0 ~ 15
//	_TCAH : address hold time after nCS is not asserted   : 0 ~ 15
//
//	_WAITMODE : wait enable control for Static #0 ~ #9 & IDE : 1=disable, 2=Active High, 3=Active Low
//	_WBURST	  : burst write mode for Static #0 ~ #9          : 0=disable, 1=4byte, 2=8byte, 3=16byte
//	_RBURST   : burst  read mode for Static #0 ~ #9          : 0=disable, 1=4byte, 2=8byte, 3=16byte
//
//------------------------------------------------------------------------------
#define CFG_SYS_STATICBUS_CONFIG( _name_, bw, tACS, tCOS, tACC, tSACC, tCOH, tCAH, wm, rb, wb )	\
	enum {											\
		CFG_SYS_ ## _name_ ## _BW		= bw,		\
		CFG_SYS_ ## _name_ ## _TACS		= tACS,		\
		CFG_SYS_ ## _name_ ## _TCOS		= tCOS,		\
		CFG_SYS_ ## _name_ ## _TACC		= tACC,		\
		CFG_SYS_ ## _name_ ## _TSACC	= tSACC,	\
		CFG_SYS_ ## _name_ ## _TCOH		= tCOH,		\
		CFG_SYS_ ## _name_ ## _TCAH		= tCAH,		\
		CFG_SYS_ ## _name_ ## _WAITMODE	= wm, 		\
		CFG_SYS_ ## _name_ ## _RBURST	= rb, 		\
		CFG_SYS_ ## _name_ ## _WBURST	= wb		\
	};

//                      ( _name_ , bw, tACS tCOS tACC tSACC tCOH tCAH, wm, rb, wb )
CFG_SYS_STATICBUS_CONFIG( STATIC0,  8,    1,   4,   7,    7,   4,   1,  1,  0,  0 )		// NOR
CFG_SYS_STATICBUS_CONFIG( STATIC1, 16,    1,   0,   6,    6,   0,   1,  1,  0,  0 )		// Not used
CFG_SYS_STATICBUS_CONFIG( STATIC2, 16,    3,   3,  16,   16,   3,   3,  1,  0,  0 )		// Not used
CFG_SYS_STATICBUS_CONFIG( STATIC3, 16,    3,   3,  16,   16,   3,   3,  1,  0,  0 )		// Not used
CFG_SYS_STATICBUS_CONFIG( STATIC4, 16,    3,   3,  16,   16,   3,   3,  1,  0,  0 )		// Not used
CFG_SYS_STATICBUS_CONFIG( STATIC5,  8,    3,   3,  16,   16,   3,   3,  1,  0,  0 )		// Not used
CFG_SYS_STATICBUS_CONFIG( STATIC6,  8,    0,   0,   2,    2,   0,   0,  1,  0,  0 )		// Not used
CFG_SYS_STATICBUS_CONFIG( STATIC7, 16,    1,   1,  20,   20,   1,   1,  1,  3,  0 )		// Not used
CFG_SYS_STATICBUS_CONFIG(    NAND,  8,   15,   1,   5,    1,   0,  15,  1,  0,  0 )
//CFG_SYS_STATICBUS_CONFIG(    NAND,  8,    0,   1,   7,    1,   3,   1,  1,  0,  0 )		// NAND  tCOH, tCAH must be greater than 0 for NAND
CFG_SYS_STATICBUS_CONFIG( STATIC13, 8,    1,   4,   7,    7,   4,   1,  1,  0,  0 )		// NOR

#endif //__CFG_SYS_H__
