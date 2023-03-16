/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <linux/init.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/delay.h>	/* mdelay */

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

/* nexell soc headers */
#include <mach/platform.h>

#if (1)
#define DBGOUT(msg...)		{ printk("cpu: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#if defined(CONFIG_PROTO_FUNC_DEBUG)
#define	PROTO_DEBUG		1
#else
#define	PROTO_DEBUG		0
#endif

/* local function */
static void	proto_init(void);
static void bus_init(void);
static void pll_init(void);
static void pll_info(void);

#if defined(CONFIG_PM)
static void	boot_mode(void);
#endif

/* 0=PLL0, 1=PLL1, 2=FCLK, 3=MCLK, 4=BCLK, 5=PCLK */
u_int cpu_get_clock_hz(cfg_sys_clksrc clk);

/*------------------------------------------------------------------------------
 * 	cpu interface.
 */
void cpu_base_init(void)
{
	/* Make sure the peripheral register window is closed, since
	 * we will use PTE flags (TEX[1]=1,B=0,C=1) to determine which
	 * pages are peripheral interface or not.
	 */
	DBGOUT("base init\n");

	proto_init();
	bus_init();
	pll_init();
	pll_info();
#if defined(CONFIG_PM)
	boot_mode();
#endif

	DBGOUT("Prototype : %s mode \n\n", PROTO_DEBUG?"Debug":"Release");
}

/*------------------------------------------------------------------------------
 * Set prototype base address.
 */
static void proto_init(void)
{
	U32 idx = 0;

	/* Alive pad */
	NX_ALIVE_Initialize();
	NX_ALIVE_SetBaseAddress((U32)IO_ADDRESS(NX_ALIVE_GetPhysicalAddress()));
	NX_ALIVE_OpenModule();

	/* Gpio */
	NX_GPIO_Initialize();
	for (idx = 0; NX_GPIO_GetNumberOfModule() > idx; idx++) {
		NX_GPIO_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_GPIO_GetPhysicalAddress(idx)));
		NX_GPIO_OpenModule(idx);
	}

	/* Clock and Power for PLL */
	NX_CLKPWR_Initialize();
	NX_CLKPWR_SetBaseAddress((U32)IO_ADDRESS(NX_CLKPWR_GetPhysicalAddress()));
	NX_CLKPWR_OpenModule();

	/* MCUD For Dynamic Memory. */
	NX_MCUD_Initialize();
	NX_MCUD_SetBaseAddress((U32)IO_ADDRESS(NX_MCUD_GetPhysicalAddress()));
	NX_MCUD_OpenModule();

	/* MCUS for Static Memory. */
	NX_MCUS_Initialize();
	NX_MCUS_SetBaseAddress((U32)IO_ADDRESS(NX_MCUS_GetPhysicalAddress()));
	NX_MCUS_OpenModule();

	/* Interrupt controller */
	NX_INTC_Initialize();
	NX_INTC_SetBaseAddress((U32)IO_ADDRESS(NX_INTC_GetPhysicalAddress()));
	NX_INTC_OpenModule();

	/* Timer */
	NX_TIMER_Initialize();
	for (idx = 0; NX_TIMER_GetNumberOfModule() > idx; idx++) {
		NX_TIMER_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_TIMER_GetPhysicalAddress(idx)));
		NX_TIMER_OpenModule(idx);
	}

	/* DMA */
	NX_DMA_Initialize();
	for (idx = 0; NX_DMA_GetNumberOfModule() > idx; idx++)	{
		NX_DMA_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_DMA_GetPhysicalAddress(idx)));
		NX_DMA_OpenModule(idx);
	}

	/* RTC. */
	NX_RTC_Initialize();
	NX_RTC_SetBaseAddress((U32)IO_ADDRESS(NX_RTC_GetPhysicalAddress()));
	NX_RTC_OpenModule();

	/* DPC For Display Logo (Primary/Secondary). */
	NX_DPC_Initialize();
	for (idx = 0; NX_DPC_GetNumberOfModule() > idx; idx++) {
		NX_DPC_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_DPC_GetPhysicalAddress(idx)));
		NX_DPC_OpenModule(idx);
	}

	/* MLC For Display Logo (Primary/Secondary). */
	NX_MLC_Initialize();
	for (idx = 0; NX_MLC_GetNumberOfModule() > idx; idx++) {
		NX_MLC_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_MLC_GetPhysicalAddress(0)));
		NX_MLC_OpenModule(idx);
	}

	/* PWM For BLU(Back light unit). */
	NX_PWM_Initialize();
	NX_PWM_SetBaseAddress((U32)IO_ADDRESS(NX_PWM_GetPhysicalAddress()));
 	NX_PWM_OpenModule();

	/*
	 * NOTE> ALIVE Power Gate must enable for RTC register access.
	 */
	NX_ALIVE_SetWriteEnable(CTRUE);
}


/*------------------------------------------------------------------------------
 * intialize soc bus status.
 */
static void bus_init(void)
{
	/*
	 * Update Fast Channel Arbiter
	 */
	#if (CFG_SYS_UPDATE_FASTCH == CTRUE)
	{
		NX_MCUD_SetFastArbiter(  0, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_00);
		NX_MCUD_SetFastArbiter(  1, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_01);
		NX_MCUD_SetFastArbiter(  2, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_02);
		NX_MCUD_SetFastArbiter(  3, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_03);
		NX_MCUD_SetFastArbiter(  4, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_04);
		NX_MCUD_SetFastArbiter(  5, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_05);
		NX_MCUD_SetFastArbiter(  6, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_06);
		NX_MCUD_SetFastArbiter(  7, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_07);
		NX_MCUD_SetFastArbiter(  8, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_08);
		NX_MCUD_SetFastArbiter(  9, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_09);
		NX_MCUD_SetFastArbiter( 10, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_10);
		NX_MCUD_SetFastArbiter( 11, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_11);
		NX_MCUD_SetFastArbiter( 12, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_12);
		NX_MCUD_SetFastArbiter( 13, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_13);
		NX_MCUD_SetFastArbiter( 14, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_14);
		NX_MCUD_SetFastArbiter( 15, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_15);
	}
	#endif

	/*
	 * Update Slow Channel Arbiter
	 */
	#if (CFG_SYS_UPDATE_SLOWCH == CTRUE)
	{
		NX_MCUD_SetSlowArbiter(  0, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_00);
		NX_MCUD_SetSlowArbiter(  1, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_01);
		NX_MCUD_SetSlowArbiter(  2, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_02);
		NX_MCUD_SetSlowArbiter(  3, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_03);
		NX_MCUD_SetSlowArbiter(  4, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_04);
		NX_MCUD_SetSlowArbiter(  5, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_05);
		NX_MCUD_SetSlowArbiter(  6, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_06);
		NX_MCUD_SetSlowArbiter(  7, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_07);
		NX_MCUD_SetSlowArbiter(  8, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_08);
		NX_MCUD_SetSlowArbiter(  9, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_09);
		NX_MCUD_SetSlowArbiter( 10, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_10);
		NX_MCUD_SetSlowArbiter( 11, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_11);
		NX_MCUD_SetSlowArbiter( 12, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_12);
		NX_MCUD_SetSlowArbiter( 13, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_13);
		NX_MCUD_SetSlowArbiter( 14, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_14);
		NX_MCUD_SetSlowArbiter( 15, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_15);
	}
	#endif

#if 0	/* 18jan12 */
	/*
	 * NAND Bus config
	 */
	NX_MCUS_SetStaticBUSConfig
	(
		NX_MCUS_SBUSID_NAND,	// bus type 		: NAND
		0,						// bit width 		: Not used
		CFG_SYS_NAND_TACS,		// tACS  ( 0 ~ 3 )
		CFG_SYS_NAND_TCAH,		// tCAH  ( 0 ~ 3 )
		CFG_SYS_NAND_TCOS,		// tCOS  ( 0 ~ 3 )
		CFG_SYS_NAND_TCOH,		// tCOH  ( 0 ~ 3 )
		CFG_SYS_NAND_TACC,		// tACC  ( 1 ~ 16)
		0,						// tSACC ( 1 ~ 16) : Not used
		(NX_MCUS_WAITMODE)0,	// Wait mode		: Not used
		(NX_MCUS_BURSTMODE)0,	// Read  burst mode	: Not used
		(NX_MCUS_BURSTMODE)0	// Write burst mode : Not used
	);

	/*
	 * MCU-Static config: Static Bus #0 ~ #7
	 */
	#define STATIC_BUS_CONFIGUTATION( _n_ )								\
	NX_MCUS_SetStaticBUSConfig											\
	( 																	\
		NX_MCUS_SBUSID_STATIC ## _n_, 									\
		CFG_SYS_STATIC ## _n_ ## _BW, 									\
		CFG_SYS_STATIC ## _n_ ## _TACS, 								\
		CFG_SYS_STATIC ## _n_ ## _TCAH, 								\
		CFG_SYS_STATIC ## _n_ ## _TCOS, 								\
		CFG_SYS_STATIC ## _n_ ## _TCOH, 								\
		CFG_SYS_STATIC ## _n_ ## _TACC, 								\
		CFG_SYS_STATIC ## _n_ ## _TSACC,								\
		(NX_MCUS_WAITMODE ) CFG_SYS_STATIC ## _n_ ## _WAITMODE, 		\
		(NX_MCUS_BURSTMODE) CFG_SYS_STATIC ## _n_ ## _RBURST, 			\
		(NX_MCUS_BURSTMODE) CFG_SYS_STATIC ## _n_ ## _WBURST			\
	);

	STATIC_BUS_CONFIGUTATION( 0);
	STATIC_BUS_CONFIGUTATION( 1);
	STATIC_BUS_CONFIGUTATION( 2);
	STATIC_BUS_CONFIGUTATION( 3);
	STATIC_BUS_CONFIGUTATION( 4);
	STATIC_BUS_CONFIGUTATION( 5);
	STATIC_BUS_CONFIGUTATION( 6);
	STATIC_BUS_CONFIGUTATION( 7);
#endif	/* 18jan12 */
}

/*------------------------------------------------------------------------------
 * intialize nexell soc frequency.
 */
static u_int  pre_fclk = 0;
static u_int  pre_mclk = 0;

static void pll_init(void)
{
	pre_fclk = cpu_get_clock_hz(2);
	pre_mclk = cpu_get_clock_hz(3);

	/* Update CPU PLL only */
	#if (CTRUE == CFG_SYS_CLKPWR_UPDATE && CTRUE != CFG_SYS_MCUD_UPDATE)
	{
		/* Bus mode */
		if (CFG_SYS_CPU_BUSDIV != (CFG_SYS_MCLK_DIV * CFG_SYS_BCLK_DIV))
			NX_CLKPWR_SetCPUBUSSyncMode(CFALSE);

		#if (CFG_SYS_CLKPWR_SYNC_BUS)
			if (CFG_SYS_CPU_BUSDIV == (CFG_SYS_MCLK_DIV * CFG_SYS_BCLK_DIV))
				NX_CLKPWR_SetCPUBUSSyncMode(CTRUE);
		#endif

		/* Pll change */
		NX_CLKPWR_SetClockCPU(CFG_SYS_CPU_CLKSRC, CFG_SYS_CPU_COREDIV, CFG_SYS_CPU_BUSDIV);
		NX_CLKPWR_SetClockMCLK(CFG_SYS_MCLK_CLKSRC, CFG_SYS_MCLK_DIV, CFG_SYS_BCLK_DIV, CFG_SYS_PCLK_DIV);
		NX_CLKPWR_SetPLLPMS(0, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S);
		NX_CLKPWR_SetPLLPowerOn(CFG_SYS_PLL1_USE);
		NX_CLKPWR_SetPLLPMS(1, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S);
		NX_CLKPWR_DoPLLChange();

		while (CFALSE == NX_CLKPWR_IsPLLStable()) { ; }
	}
	#endif

	/* Update CPU PLL and Memory Clock */
	#if (CTRUE == CFG_SYS_CLKPWR_UPDATE && CTRUE == CFG_SYS_MCUD_UPDATE)
	{
		/* When faster more than current */
		if (CFG_SYS_MCLK_FREQ > pre_mclk)
		{
			/* 1. SetMemCfg */
			NX_MCUD_SetCASLatency	(CFG_SYS_MCUD_CASLAT);
			NX_MCUD_SetDelayLatency	(CFG_SYS_MCUD_DLYLAT);

			/* 2. InitialMemConfig */
			NX_MCUD_ApplyModeSetting();

			while (NX_MCUD_IsBusyModeSetting()) { ; }

			/* 3. SetMemTime */
			NX_MCUD_SetMRD	(CFG_SYS_MCUD_TMRD);
			NX_MCUD_SetRP	(CFG_SYS_MCUD_TRP);
			NX_MCUD_SetRCD	(CFG_SYS_MCUD_TRCD);
			NX_MCUD_SetRAS	(CFG_SYS_MCUD_TRAS);
			NX_MCUD_SetWR	(CFG_SYS_MCUD_TWR);
			NX_MCUD_SetRFC	(CFG_SYS_MCUD_TRFC);
			NX_MCUD_SetWTR	(CFG_SYS_MCUD_TWTR);
			NX_MCUD_SetRTP	(CFG_SYS_MCUD_TRTP);
			NX_MCUD_SetActivePowerDownEnable(CFG_SYS_MCUD_ACTPDENB);
			NX_MCUD_SetActivePowerDownPeriod(CFG_SYS_MCUD_ACTPDPRED);

			/* 4. SetDLL */
			#if 	(CFG_SYS_MCLK_FREQ >= 400000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x14, 0x14);
			#elif 	(CFG_SYS_MCLK_FREQ >= 333333333)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x18, 0x18);
			#elif 	(CFG_SYS_MCLK_FREQ >= 266666666)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x1E, 0x1E);
			#elif 	(CFG_SYS_MCLK_FREQ >= 200000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x28, 0x28);
			#elif 	(CFG_SYS_MCLK_FREQ >= 166666666)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x2F, 0x2F);
			#elif 	(CFG_SYS_MCLK_FREQ >= 133333333)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x3B, 0x3B);
			#elif	(CFG_SYS_MCLK_FREQ >= 100000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x4F, 0x4F);
			#else
			NX_MCUD_PHYDLL_SetConfig(CTRUE , 0x7F, 0x7F);
			#endif

			NX_MCUD_SetDLLReset(CFG_SYS_MCUD_DLLRESET);
			NX_MCUD_SetMCLKConfig((CFG_SYS_BCLK_DIV == 2) ? CTRUE : CFALSE, (CFG_SYS_MCLK_FREQ >= 200000000) ? CTRUE : CFALSE);

			/* 5. Pll change */
			if (CFG_SYS_CPU_CORE_FREQ != pre_fclk)
			{
				NX_CLKPWR_SetClockCPU(CFG_SYS_CPU_CLKSRC, CFG_SYS_CPU_COREDIV, CFG_SYS_CPU_BUSDIV);
				NX_CLKPWR_SetClockMCLK(CFG_SYS_MCLK_CLKSRC, CFG_SYS_MCLK_DIV, CFG_SYS_BCLK_DIV, CFG_SYS_PCLK_DIV);
				NX_CLKPWR_SetPLLPMS(0, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S);
				NX_CLKPWR_SetPLLPowerOn(CFG_SYS_PLL1_USE);
				NX_CLKPWR_SetPLLPMS(1, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S);
				NX_CLKPWR_DoPLLChange();

				while (CFALSE == NX_CLKPWR_IsPLLStable()) { ; }
			}

			/* 6. SetMemRefresh */
			NX_MCUD_SetRefreshPeriod(CFG_SYS_MCUD_REFPRED);
		}
		else if (pre_mclk > CFG_SYS_MCLK_FREQ)
		{
			/* 1. SetMemCfg */
			NX_MCUD_SetDelayLatency(CFG_SYS_MCUD_DLYLAT);

			/* 2. SetMemRefresh */
			NX_MCUD_SetRefreshPeriod(CFG_SYS_MCUD_REFPRED);

			/* 3. SetDLL */
			#if 	(CFG_SYS_MCLK_FREQ >= 400000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x14, 0x14);
			#elif 	(CFG_SYS_MCLK_FREQ >= 333333333)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x18, 0x18);
			#elif 	(CFG_SYS_MCLK_FREQ >= 266666666)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x1E, 0x1E);
			#elif 	(CFG_SYS_MCLK_FREQ >= 200000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x28, 0x28);
			#elif 	(CFG_SYS_MCLK_FREQ >= 166666666)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x2F, 0x2F);
			#elif 	(CFG_SYS_MCLK_FREQ >= 133333333)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x3B, 0x3B);
			#elif	(CFG_SYS_MCLK_FREQ >= 100000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x4F, 0x4F);
			#else
			NX_MCUD_PHYDLL_SetConfig(CTRUE , 0x7F, 0x7F);
			#endif

			NX_MCUD_SetDLLReset(CFG_SYS_MCUD_DLLRESET);
			NX_MCUD_SetMCLKConfig((CFG_SYS_BCLK_DIV == 2) ? CTRUE : CFALSE, (CFG_SYS_MCLK_FREQ >= 200000000) ? CTRUE : CFALSE);

			/* 4. Pll change */
			if (CFG_SYS_CPU_CORE_FREQ != pre_fclk)
			{
				NX_CLKPWR_SetClockCPU(CFG_SYS_CPU_CLKSRC, CFG_SYS_CPU_COREDIV, CFG_SYS_CPU_BUSDIV);
				NX_CLKPWR_SetClockMCLK(CFG_SYS_MCLK_CLKSRC, CFG_SYS_MCLK_DIV, CFG_SYS_BCLK_DIV, CFG_SYS_PCLK_DIV);
				NX_CLKPWR_SetPLLPMS(0, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S);
				NX_CLKPWR_SetPLLPowerOn(CFG_SYS_PLL1_USE);
				NX_CLKPWR_SetPLLPMS(1, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S);
				NX_CLKPWR_DoPLLChange();

				while (CFALSE == NX_CLKPWR_IsPLLStable()) { ; }
			}

			/* 5. SetMemTime */
			NX_MCUD_SetMRD	(CFG_SYS_MCUD_TMRD);
			NX_MCUD_SetRP	(CFG_SYS_MCUD_TRP);
			NX_MCUD_SetRCD	(CFG_SYS_MCUD_TRCD);
			NX_MCUD_SetRAS	(CFG_SYS_MCUD_TRAS);
			NX_MCUD_SetWR	(CFG_SYS_MCUD_TWR);
			NX_MCUD_SetRFC	(CFG_SYS_MCUD_TRFC);
			NX_MCUD_SetWTR	(CFG_SYS_MCUD_TWTR);
			NX_MCUD_SetRTP	(CFG_SYS_MCUD_TRTP);
			NX_MCUD_SetActivePowerDownEnable(CFG_SYS_MCUD_ACTPDENB);
			NX_MCUD_SetActivePowerDownPeriod(CFG_SYS_MCUD_ACTPDPRED);

			/* 6. SetMemCfg */
			NX_MCUD_SetCASLatency(CFG_SYS_MCUD_CASLAT);

			/* 7. InitialMemConfig */
			NX_MCUD_ApplyModeSetting();

			while (NX_MCUD_IsBusyModeSetting()) { ; }
		}
		else
		{
			if (CFG_SYS_CPU_CORE_FREQ != pre_fclk)
			{
				NX_CLKPWR_SetClockCPU(CFG_SYS_CPU_CLKSRC, CFG_SYS_CPU_COREDIV, CFG_SYS_CPU_BUSDIV);
				NX_CLKPWR_SetClockMCLK(CFG_SYS_MCLK_CLKSRC, CFG_SYS_MCLK_DIV, CFG_SYS_BCLK_DIV, CFG_SYS_PCLK_DIV);
				NX_CLKPWR_SetPLLPMS(0, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S);
				NX_CLKPWR_SetPLLPowerOn(CFG_SYS_PLL1_USE);
				NX_CLKPWR_SetPLLPMS(1, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S);
				NX_CLKPWR_DoPLLChange();

				while (CFALSE == NX_CLKPWR_IsPLLStable()) { ; }
			}
		}
	}
	#endif	// CFG_SYS_CLKPWR_UPDATE
}

/*------------------------------------------------------------------------------
 * memory clock status.
 */
#define MEMCFG		*(volatile unsigned long *)IO_ADDRESS(PHY_BASEADDR_MCUD + 0x00000000)
#define MEMTIME0	*(volatile unsigned long *)IO_ADDRESS(PHY_BASEADDR_MCUD + 0x00000004)
#define MEMCTRL		*(volatile unsigned long *)IO_ADDRESS(PHY_BASEADDR_MCUD + 0x00000008)
#define MEMACTPWD	*(volatile unsigned long *)IO_ADDRESS(PHY_BASEADDR_MCUD + 0x0000000C)
#define MEMTIME1	*(volatile unsigned long *)IO_ADDRESS(PHY_BASEADDR_MCUD + 0x00000010)

#define PHYDELAYCTRL	*(volatile unsigned long *)IO_ADDRESS(PHY_BASEADDR_MCUD + 0x00000094)
#define PHYDLLCTRL0	*(volatile unsigned long *)IO_ADDRESS(PHY_BASEADDR_MCUD + 0x00000098)
#define PHYTERMCTRL	*(volatile unsigned long *)IO_ADDRESS(PHY_BASEADDR_MCUD + 0x000000B4)

void mem_clk_info(void)
{
	unsigned int config = MEMCFG;
	unsigned int time_0 = MEMTIME0;
	unsigned int time_1 = MEMTIME1;
        unsigned int phytermctrl = PHYTERMCTRL;

	int DS		= (int)((config >> 10) & 0x1);
	int ADDLAT	= (int)((config >> 18) & 0x7);
	int CASLAT	= (int)((config >> 21) & 0x7);
	int DLYLAT	= (int)((config >> 24) & 0x7);

	int TRCD	= (int)(((time_0 >>  0) & 0x0F) + 1);
	int TRP		= (int)(((time_0 >>  4) & 0x0F) + 1);
	int TRAS	= (int)(((time_0 >> 12) & 0xFF) + 1);
	int TRFC	= (int)(((time_0 >> 24) & 0xFF) + 1);

	int TMRD	= (int)(((time_1 >> 16) & 0x0F) + 1);
	int TRTP	= (int)(((time_1 >> 20) & 0x0F) + 1);
	int TWR		= (int)(((time_1 >> 24) & 0x0F) + 1);
	int TWTR	= (int)(((time_1 >> 28) & 0x0F) + 1);

	DBGOUT("Memory Clock Configuration :\n");
	DBGOUT("MEMCFG   =0x%8.8X  PHYTERMCTRL =0x%8.8X\n", config, phytermctrl);
	DBGOUT("MEMTIME0 =0x%8.8X  MEMTIME1    =0x%8.8X\n", time_0, time_1);
	DBGOUT("CLAT =%4d, ALAT =%4d, DLAT =%4d, DS   =%4d \n", CASLAT, ADDLAT, DLYLAT, DS);
	DBGOUT("TRCD =%4d, TRP  =%4d, TRAS =%4d, TMRD =%4d \n", TRCD, TRP , TRAS, TMRD);
	DBGOUT("TWR  =%4d, TWTR =%4d, TRTP =%4d, TRFC =%4d \n", TWR , TWTR, TRTP, TRFC);
}

/*------------------------------------------------------------------------------
 * cpu pll status functions.
 */
typedef struct	__NX_CLKPWR_RegSet
{
	volatile unsigned int CLKMODE[2]				; 	// 0x00, 0x04
	volatile unsigned int PLLSET[2]					; 	// 0x08, 0x0C
	volatile unsigned int __Reserved[(0x68-0x10)/4]	; 	// 0x10 ~ 0x64
	volatile unsigned int PWRMODE					; 	// 0x68
} NX_CLKPWR_REGSET;

#define NX_CLKPWR_BASEADDR		IO_ADDRESS(PHY_BASEADDR_CLKPWR)

static void cpu_clk_info(void)
{
	unsigned int clkmode[2], pllreg[2];
	unsigned int pll[4], sel, dvo, div2, div3, fclk, mclk, bclk, pclk, sync;
	int i;

	NX_CLKPWR_REGSET *pReg = (NX_CLKPWR_REGSET *)(NX_CLKPWR_BASEADDR);

	DBGOUT("%s: %s Frequency Configuration : \n", CFG_SYS_CPU_NAME, (CFG_SYS_CLKPWR_UPDATE==CTRUE?"DONE":"NONE"));

#if (0)
	DBGOUT( "\n" );
	DBGOUT( "< CLKPWR register dump >\n" );
	DBGOUT( "CLKMODE[0] = 0x%08X\n", pReg->CLKMODE[0]);
	DBGOUT( "CLKMODE[1] = 0x%08X\n", pReg->CLKMODE[1]);
	DBGOUT( "PLLSET [0] = 0x%08X\n", pReg->PLLSET[0]);
	DBGOUT( "PLLSET [1] = 0x%08X\n", pReg->PLLSET[1]);
	DBGOUT( "\n" );
#endif

	clkmode[0] 	= pReg->CLKMODE[0];
	clkmode[1] 	= pReg->CLKMODE[1];
	pllreg [0]	= pReg->PLLSET[0];
	pllreg [1]	= pReg->PLLSET[1];
	sync		= pReg->PWRMODE & (1<<28);

	for ( i=0 ; i<2 ; i++ )
	{
		unsigned int pdiv, mdiv, sdiv;
		unsigned int P_bitpos, M_bitpos, S_bitpos;
		unsigned int P_mask, M_mask, S_mask;

		const unsigned int PLL0_PDIV_BIT_POS 	= 18;
		const unsigned int PLL0_MDIV_BIT_POS 	=  8;
		const unsigned int PLL0_SDIV_BIT_POS 	=  0;
		const unsigned int PLL0_PDIV_MASK       = 0xFF;
		const unsigned int PLL0_MDIV_MASK       = 0x3FF;
		const unsigned int PLL0_SDIV_MASK       = 0xFF;

		const unsigned int PLL1_PDIV_BIT_POS 	= 18;
		const unsigned int PLL1_MDIV_BIT_POS 	=  8;
		const unsigned int PLL1_SDIV_BIT_POS 	=  0;
		const unsigned int PLL1_PDIV_MASK       = 0x3F;
		const unsigned int PLL1_MDIV_MASK       = 0x3FF;
		const unsigned int PLL1_SDIV_MASK       = 0xFF;

        if ( 0 == i ) {
            P_bitpos = PLL0_PDIV_BIT_POS;
            P_mask   = PLL0_PDIV_MASK;
            M_bitpos = PLL0_MDIV_BIT_POS;
            M_mask   = PLL0_MDIV_MASK;
            S_bitpos = PLL0_SDIV_BIT_POS;
            S_mask   = PLL0_SDIV_MASK;
        } else {
            P_bitpos = PLL1_PDIV_BIT_POS;
            P_mask   = PLL1_PDIV_MASK;
            M_bitpos = PLL1_MDIV_BIT_POS;
            M_mask   = PLL1_MDIV_MASK;
            S_bitpos = PLL1_SDIV_BIT_POS;
            S_mask   = PLL1_SDIV_MASK;
        }

		pdiv	= (pllreg[i] >> P_bitpos) & P_mask;
		mdiv	= (pllreg[i] >> M_bitpos) & M_mask;
		sdiv	= (pllreg[i] >> S_bitpos) & S_mask;

		pll[i]	= CFG_SYS_PLL_PMSFUNC( CFG_SYS_PLLFIN, pdiv, mdiv, sdiv );

		DBGOUT( "PLL%d [P = %2d, M=%2d, S=%2d] = %d Hz. \n", i, pdiv, mdiv, sdiv, pll[i] );
	}

	dvo 	= (clkmode[0]>>( 0+ 0)) & 0xF;
	sel		= (clkmode[0]>>( 4+ 0)) & 0x3;	//	NX_ASSERT( 2 > sel );
	div2	= (clkmode[0]>>( 8+ 0)) & 0xF;
	DBGOUT( "CPU : Core clock = PLL%d / %d = %d, AXI Bus Clock = Core clock / %d = %d, [%s]\n",
		sel, dvo+1, pll[sel] / (dvo+1), div2+1, (pll[sel] / (dvo+1)) / (div2+1),
		sync ? "SYNC" : "ASYNC");

	fclk = pll[sel] / (dvo+1);
	pll[3] = fclk;

	dvo	    = (clkmode[1]>>(  0+ 0)) & 0xF;
	sel		= (clkmode[1]>>(  4+ 0)) & 0x3;//	NX_ASSERT( 2 != sel );
	div2	= (clkmode[1]>>(  8+ 0)) & 0xF;
	div3	= (clkmode[1]>>( 12+ 0)) & 0xF;

	mclk = pll[sel] / (dvo  + 1);
	bclk = mclk     / (div2 + 1);
	pclk = bclk     / (div3 + 1);

	DBGOUT( "BUS : MCLK = %s / %d = %d, BCLK = MCLK / %d = %d, PCLK = BCLK / %d = %d\n",
						((sel==3) ? "FCLK" : ((sel==1) ? "PLL1" : "PLL0")),
						dvo+1, 	mclk,
                		div2+1, bclk,
                		div3+1, pclk );

	/* PLL1 Power down status */
	DBGOUT("PLL1 Power : %s \n", (clkmode[0] & (1 << 30)) ? "down" : "normal" );
	DBGOUT("FCLK: %d Hz -> %d Hz, change [%s]\n", pre_fclk, fclk, (pre_fclk == fclk) ? "NONE" : "DONE");
	DBGOUT("MCLK: %d Hz -> %d Hz, change [%s]\n", pre_mclk, mclk, (pre_mclk == mclk) ? "NONE" : "DONE");
}

static void pll_info(void)
{
	cpu_clk_info();
	mem_clk_info();
}

#if defined(CONFIG_PM)
/* check boot mode */
extern u_int cpu_get_reserve_pm_mem(int phys);

static void	boot_mode(void)
{
	u32  base = cpu_get_reserve_pm_mem(0);
	U32  csum = 0, size = 0;
	U32 *data = NULL;

	int  val = 0, xor = 0, i = 0;
	bool coldboot = true;
	u32  scratch  = 0;

	if (! base) {
		DBGOUT("Boot mode : Cold boot ...\n");
		return;
	}

	csum 	 = *(U32*)(base + 0x0);
	size 	 = *(U32*)(base + 0x4);
	data 	 =  (U32*)(base + 0x8);
	scratch  = NX_ALIVE_GetScratchReg();

	if (CFG_PWR_SLEEP_SIGNATURE == scratch) {

		if (size <= 0 || size >= 0x100) {
			coldboot = true;
		} else {
			for (i = 0; i<size; i++, data++)
				xor = (*data ^ 0xAA), val ^= xor;

			if (csum != val)
				coldboot = true;
			else
				coldboot = false;
		}
	}

	DBGOUT("Boot mode : %s boot [0x%08x, 0x%08x, 0x%08x]...\n",
		coldboot==true?"Cold":"Warm", scratch, csum, val);
}
#endif

/*------------------------------------------------------------------------------
 * 	cpu clock interface.
 */
u_int cpu_get_clock_hz(cfg_sys_clksrc clk)
{
	unsigned int clkmode[2], pllreg[2], clock_hz = 0;
	unsigned int pll[4], sel, dvo, div2, div3, fclk, mclk, bclk, pclk;
	int i;

	NX_CLKPWR_REGSET *pReg = (NX_CLKPWR_REGSET *)(NX_CLKPWR_BASEADDR);

	clkmode[0] 	= pReg->CLKMODE[0];
	clkmode[1] 	= pReg->CLKMODE[1];
	pllreg [0]	= pReg->PLLSET[0];
	pllreg [1]	= pReg->PLLSET[1];

	for ( i=0 ; i<2 ; i++ ) {
		unsigned int 	   pdiv, mdiv, sdiv;
		unsigned int       P_bitpos, M_bitpos, S_bitpos;
		unsigned int       P_mask, M_mask, S_mask;

		const unsigned int PLL0_PDIV_BIT_POS = 18;
		const unsigned int PLL0_MDIV_BIT_POS =  8;
		const unsigned int PLL0_SDIV_BIT_POS =  0;
		const unsigned int PLL0_PDIV_MASK    = 0xFF;
		const unsigned int PLL0_MDIV_MASK    = 0x3FF;
		const unsigned int PLL0_SDIV_MASK    = 0xFF;

		const unsigned int PLL1_PDIV_BIT_POS = 18;
		const unsigned int PLL1_MDIV_BIT_POS =  8;
		const unsigned int PLL1_SDIV_BIT_POS =  0;
		const unsigned int PLL1_PDIV_MASK    = 0x3F;
		const unsigned int PLL1_MDIV_MASK    = 0x3FF;
		const unsigned int PLL1_SDIV_MASK    = 0xFF;

		if ( 0 == i ) {
			P_bitpos = PLL0_PDIV_BIT_POS;
			P_mask   = PLL0_PDIV_MASK;
			M_bitpos = PLL0_MDIV_BIT_POS;
			M_mask   = PLL0_MDIV_MASK;
			S_bitpos = PLL0_SDIV_BIT_POS;
			S_mask   = PLL0_SDIV_MASK;
		} else {
			P_bitpos = PLL1_PDIV_BIT_POS;
			P_mask   = PLL1_PDIV_MASK;
			M_bitpos = PLL1_MDIV_BIT_POS;
			M_mask   = PLL1_MDIV_MASK;
			S_bitpos = PLL1_SDIV_BIT_POS;
			S_mask   = PLL1_SDIV_MASK;
		}

		pdiv  = (pllreg[i] >> P_bitpos) & P_mask;
		mdiv  = (pllreg[i] >> M_bitpos) & M_mask;
		sdiv  = (pllreg[i] >> S_bitpos) & S_mask;

		pll[i]= CFG_SYS_PLL_PMSFUNC( CFG_SYS_PLLFIN, pdiv, mdiv, sdiv );
	}

	dvo   = (clkmode[0]>>( 0+ 0)) & 0xF;
	sel   = (clkmode[0]>>( 4+ 0)) & 0x3;	// NX_ASSERT( 2 > sel );
	div2  = (clkmode[0]>>( 8+ 0)) & 0xF;

	fclk = pll[sel] / (dvo+1);
	pll[3] = fclk;

	dvo   = (clkmode[1]>>(  0+ 0)) & 0xF;
	sel   = (clkmode[1]>>(  4+ 0)) & 0x3;	// NX_ASSERT( 2 != sel );
	div2  = (clkmode[1]>>(  8+ 0)) & 0xF;
	div3  = (clkmode[1]>>( 12+ 0)) & 0xF;

	mclk = pll[sel] / (dvo  + 1);
	bclk = mclk     / (div2 + 1);
	pclk = bclk     / (div3 + 1);

	switch((cfg_sys_clksrc)clk) {
	case CFG_SYS_CLKSRC_PLL0:	clock_hz =	pll[0];	break;
	case CFG_SYS_CLKSRC_PLL1:	clock_hz =	pll[1];	break;
	case CFG_SYS_CLKSRC_FCLK:	clock_hz =	fclk;	break;
	case CFG_SYS_CLKSRC_MCLK:	clock_hz =	mclk;	break;
	case CFG_SYS_CLKSRC_BCLK:	clock_hz =	bclk;	break;
	case CFG_SYS_CLKSRC_PCLK:	clock_hz =	pclk;	break;
	default:
		DBGOUT("error: unknown clock [%d], (0~6)\n", clk);
		break;
	}

	return clock_hz;
}
EXPORT_SYMBOL(cpu_get_clock_hz);
