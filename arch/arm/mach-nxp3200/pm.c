/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/bootmem.h>

#include <mach/hardware.h>
#include <asm/memory.h>
#include <asm/system.h>
#include <asm/io.h>

#if defined(CONFIG_PM)

#include <mach/platform.h>
#include <mach/board_pm.h>
#include <mach/system.h>

#if (0)
#define DBGOUT(msg...)		{ printk("pm: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*------------------------------------------------------------------------------
 * 	PM sleep data memory
 */
#define	SLEEP_PM_SIZE 	0x100
#define	SLEEP_MM_SIZE 	PAGE_SIZE

unsigned int sleep_phys_base = CFG_SLEEP_DATA_BASE;
unsigned int sleep_virt_base = 0;
unsigned int sleep_save_base = 0;
unsigned int sleep_save_data[PAGE_SIZE] = { 0, };

/*------------------------------------------------------------------------------
 * 	PM soc function
 */
static  int   g_wakeup_source = -1;

/* backup register */
static struct NX_INTC_RegisterSet irq_reg;				/* INTC */
static 	u_int	PIODETMOD[NUMBER_OF_GPIO_MODULE][2];	/* GPIO */
static 	u_int   PIOINTENB[NUMBER_OF_GPIO_MODULE];
static 	u_int   DMAINTMOD[NUMBER_OF_DMA_MODULE ];		/* DMA */
static  u_int   ALVINTENB;								/* ALIVE */

static void soc_pm_interrupt(int suspend)
{
	static struct NX_INTC_RegisterSet * r_irq = NULL;
	static struct NX_GPIO_RegisterSet * r_pio = NULL;
	static struct NX_DMA_RegisterSet  * r_dma = NULL;
	u_int r_val;
	int   i = 0;

	PM_DBGOUT("%s: %s\n", __func__, suspend?"suspend":"wakeup");

	if (suspend) {
		/* save: INTC */
		r_irq = (struct NX_INTC_RegisterSet*)NX_INTC_GetBaseAddress();
		irq_reg.INTMODE[0] = r_irq->INTMODE[0];
		irq_reg.INTMODE[1] = r_irq->INTMODE[1];
		irq_reg.INTMASK[0] = r_irq->INTMASK[0];
		irq_reg.INTMASK[1] = r_irq->INTMASK[1];
		irq_reg.PRIORDER   = r_irq->PRIORDER;

		/* save: GPIO */
		for (i = 0; NUMBER_OF_GPIO_MODULE > i; i++) {
			r_pio = (struct NX_GPIO_RegisterSet*)NX_GPIO_GetBaseAddress(i);

			WriteIODW(&r_pio->GPIOxDET, 0xFFFFFFFF);

			PIODETMOD[i][0] = r_pio->GPIOxDETMODE[0];
			PIODETMOD[i][1] = r_pio->GPIOxDETMODE[1];
			PIOINTENB[i]	= r_pio->GPIOxINTENB;
		}

		/* save: DMA */
		for (i = 0; NUMBER_OF_DMA_MODULE > i; i++) {
			r_dma = (struct NX_DMA_RegisterSet*)NX_DMA_GetBaseAddress(i);
			DMAINTMOD[i]    = r_dma->DMAMODE;

			WriteIODW(&r_dma->DMAMODE, r_dma->DMAMODE | (1UL<<17));
		}

		/* save: ALIVE */
		ALVINTENB = NX_ALIVE_GetInterruptEnable32();

	} else {
		/* restore: INTC */
		r_irq = (struct NX_INTC_RegisterSet*)NX_INTC_GetBaseAddress();

		WriteIODW(&r_irq->INTMODE[0], irq_reg.INTMODE[0]);
		WriteIODW(&r_irq->INTMODE[1], irq_reg.INTMODE[1]);
		WriteIODW(&r_irq->INTMASK[0], irq_reg.INTMASK[0]);
		WriteIODW(&r_irq->INTMASK[1], irq_reg.INTMASK[1]);
		WriteIODW(&r_irq->PRIORDER  , irq_reg.PRIORDER);
		WriteIODW(&r_irq->INTPEND[0], 0xFFFFFFFF);
		WriteIODW(&r_irq->INTPEND[1], 0xFFFFFFFF);

		/* restore: GPIO */
		for (i = 0; NUMBER_OF_GPIO_MODULE > i; i++) {
			r_pio = (struct NX_GPIO_RegisterSet*)NX_GPIO_GetBaseAddress(i);

			WriteIODW(&r_pio->GPIOxDET    	 , 0xFFFFFFFF);
			WriteIODW(&r_pio->GPIOxDETMODE[0], PIODETMOD[i][0]);
			WriteIODW(&r_pio->GPIOxDETMODE[1], PIODETMOD[i][1]);
			WriteIODW(&r_pio->GPIOxINTENB	 , PIOINTENB[i]);
		}

		/* restore: DMA */
		for (i = 0; NUMBER_OF_DMA_MODULE > i; i++) {
			r_dma = (struct NX_DMA_RegisterSet*)NX_DMA_GetBaseAddress(i);
			r_val = r_dma->DMAMODE;

			WriteIODW(&r_dma->DMAMODE, DMAINTMOD[i]);
			WriteIODW(&r_dma->DMAMODE, r_val | (1UL<<17));
		}

		/* restore: ALIVE */
		NX_ALIVE_SetInterruptEnable32(ALVINTENB);
	}
}

static void soc_pm_wakeup_src(int suspend)
{
	const U32 alvsrc[] = {
		CFG_PWR_WAKEUP_SRC_ALIVE0, CFG_PWR_WAKEUP_SRC_ALIVE1,
		CFG_PWR_WAKEUP_SRC_ALIVE2, CFG_PWR_WAKEUP_SRC_ALIVE3 };

	const U32 detmod[] = {
		CFG_PWR_WAKEUP_MOD_ALIVE0, CFG_PWR_WAKEUP_MOD_ALIVE1,
		CFG_PWR_WAKEUP_MOD_ALIVE2, CFG_PWR_WAKEUP_MOD_ALIVE3 };

	int i = 0, alvnum;

	alvnum = sizeof(alvsrc)/sizeof(alvsrc[0]);

	PM_DBGOUT("%s: %s\n", __func__, suspend?"suspend":"wakeup");

	NX_ALIVE_SetWriteEnable(CTRUE);
	NX_CLKPWR_ClearInterruptPendingAll();

	/* set wake up source */
	if (suspend)	{

		for (i = 0 ; alvnum > i; i++) {
			NX_ALIVE_SetInterruptEnable(i, CFALSE);
			NX_ALIVE_SetDetectEnable(i, CFALSE);
			if (alvsrc[i]) {
				NX_ALIVE_SetOutputEnable(i, CFALSE);
				NX_ALIVE_SetDetectMode(detmod[i], i, CTRUE);
				NX_ALIVE_SetDetectEnable(i, CTRUE);
			}
		}

	/* get wake up source */
	} else {
		uint alv_pnd = 0, rtc_pnd = 0, rtc_enb = 0;
		int  wak_src = -1;

		if (CFALSE == NX_ALIVE_GetWriteEnable())
			NX_ALIVE_SetWriteEnable(CTRUE);

  		alv_pnd = NX_ALIVE_GetInterruptPending32();
		if (alv_pnd) {
			for (i = 0 ; alvnum > i; i++) {
				if (0x1 & (alv_pnd>>i)) {
					wak_src = i;
					printk("WakeUp source: ALIVE GPIO [%d]\n", i);
				}
			}
		}

		rtc_enb = NX_RTC_GetInterruptEnable32();
		rtc_pnd = NX_RTC_GetInterruptPending32();
		if (rtc_pnd) {
			if ((rtc_pnd & (1 << 1)) && (rtc_enb  & (1 << 1))) {
				wak_src = 8;
				printk("WakeUp source: RTC Alarm\n");
			}
			if ((rtc_pnd & (1 << 0)) && (rtc_enb  & (1 << 0))) {
				wak_src = 9;
				printk("WakeUp source: RTC Count\n");
			}
		}

		if (NX_ALIVE_GetVDDPWRTOGGLE()) {
			wak_src = 10;
			printk("WakeUp source: VDDPWRTOGGLE\n");
		}

		if (-1 == wak_src)
			printk("WakeUp source: Unknown status !!!\n");

		g_wakeup_source	= wak_src;
	}

	NX_ALIVE_ClearInterruptPendingAll();
}

static void soc_pm_power_down(void)
{
	volatile int   delay;

	PM_DBGOUT("%s\n", __func__);

	NX_ALIVE_SetPadRetentionHold(NX_ALIVE_PADGROUP0, CFG_PWR_SLEEP_PAD_HOLD_GROUP0);
	NX_ALIVE_SetPadRetentionHold(NX_ALIVE_PADGROUP1, CFG_PWR_SLEEP_PAD_HOLD_GROUP1);
	NX_ALIVE_SetPadRetentionHold(NX_ALIVE_PADGROUP2, CFG_PWR_SLEEP_PAD_HOLD_GROUP2);

	/* sum delay */
	for (delay = 0 ; 0xFFFFF > delay; delay++ ) { ; }

#if (1)
	NX_ALIVE_SetVDDPWRON(CFALSE, CTRUE);
	NX_CLKPWR_GoStopMode();
#else
	{
	volatile u_int r_alv = (volatile u_int)IO_ADDRESS(0xC0019000);
	volatile u_int r_clk = (volatile u_int)IO_ADDRESS(0xC000F000);

	*(volatile u_int*)(r_alv + 0x9C) = 0;
	*(volatile u_int*)(r_alv + 0x98) = 0;
	*(volatile u_int*)(r_alv + 0x9C) = (1UL<<1);
	*(volatile u_int*)(r_alv + 0x98) = (1UL<<0);
	*(volatile u_int*)(r_alv + 0x9C) = 0;
	*(volatile u_int*)(r_alv + 0x98) = 0;
	*(volatile u_int*)(r_clk + 0x68) = (1<<1);		// PWRMODE (STOP_MASK)
	}
#endif
}

static void soc_pm_data_save(int suspend)
{
	static u_int csum = 0;
	void  *dest = NULL, *src = NULL;
	u_int *data = NULL;
	int    val = 0, xor = 0, i = 0;
	int    len = SLEEP_MM_SIZE/4 ;

	PM_DBGOUT("%s\n", __func__);

	if (suspend) {
		dest = (void *)sleep_save_base;
		src  = (void *)sleep_virt_base;
		data = (u_int*)src;

		for (i = 0; len > i; i++, data++) {
			xor  = (*data ^ 0xAA);
			val ^= xor;
		}

		csum = val;

	} else {
		dest = (void *)sleep_virt_base;
		src  = (void *)sleep_save_base;
		data = (u_int*)src;

		for (i = 0; len > i; i++, data++) {
			xor  = (*data ^ 0xAA);
			val ^= xor;
		}

		if (csum != val) {
			lldebugout("Error:%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
			lldebugout("Fail : wakeup, corrupt memory, reset cpu ...\n");

			NX_ALIVE_SetScratchReg(0);
			arch_reset(0, NULL);
		}
	}

	memcpy(dest, src, SLEEP_MM_SIZE);
	PM_DBGOUT("done: %s\n", __func__);
}
/*------------------------------------------------------------------------------
 * 	PM ops
 */
static struct board_pm_fns * board_pm = NULL;
static u_int  wake_time = 0;

extern void asm_cpu_ready_sleep(void);
extern void asm_cpu_restore_reg(void);
extern void cpu_base_init(void);

static inline unsigned int getcpsr(void)
{
	unsigned int reg;
	asm("mrs %0, cpsr":"=r" (reg));
	return reg;
}

static int 	cpu_pm_prepare(void)
{
	u_int cpsr = getcpsr();

	PM_DBGOUT("%s\n", __func__);

	if (board_pm && board_pm->prepare)
		board_pm->prepare();

	printk("pm: prepare, cpsr (irq:%s, fiq:%s, mode:0x%x)\n",
		((cpsr>>7&0x1)?"off":"on"), ((cpsr>>6&0x1)?"off":"on"), (cpsr&0x1F));
	return 0;
}

static int cpu_pm_enter(suspend_state_t state)
{
	u_int cpsr = getcpsr();
	int   ret  = 0;

	PM_DBGOUT("%s (state:%d)\n", __func__, state);

	switch (state) {
	case PM_SUSPEND_MEM:
		printk("pm: goto suspend to mem, cpsr (irq:%s, fiq:%s, mode:0x%x)\n",
			((cpsr>>7&0x1)?"off":"on"), ((cpsr>>6&0x1)?"off":"on"), (cpsr&0x1F) );
		break;
	case PM_SUSPEND_STANDBY:
		printk("pm: not support enter state(0x%x) \n", state);
		return 0;
	default:
		printk("pm: unknown enter state(0x%x)\n", state);
		return 0;
	}

	if (board_pm && board_pm->enter)
		board_pm->enter(state);

	/*-------------------------------------------------------------------------*/
	/* save sleep base data */
	soc_pm_data_save (1);
	soc_pm_interrupt (1);
	soc_pm_wakeup_src(1);

	/* Backup registers to ready goto suspend mode */
	asm_cpu_ready_sleep();

	if (CFG_PWR_SLEEP_SIGNATURE != NX_ALIVE_GetScratchReg()) {
		/* Set scratch register */
		NX_ALIVE_SetScratchReg(CFG_PWR_SLEEP_SIGNATURE);

		if (board_pm && board_pm->poweroff)
			ret = board_pm->poweroff();

		/* cpu power down */
		if (0 == ret) {
			soc_pm_power_down();
			mdelay(500);
		}

		/* Fail to suspend mode -> wake up */
		printk("pm: Fail, goto suspend to mem, so wakeup ...\n");
		asm_cpu_restore_reg();
	}

	/* restore sleep base data */
	soc_pm_wakeup_src(0);

	cpu_base_init();

	soc_pm_interrupt(0);
	soc_pm_data_save(0);

	if (board_pm && board_pm->poweron)
		board_pm->poweron();

	NX_ALIVE_SetScratchReg(0);
	/*-------------------------------------------------------------------------*/

	wake_time = jiffies;
	cpsr 	  = getcpsr();
	printk("pm: wakeup from suspend, cpsr (irq:%s, fiq:%s, mode:0x%x)\n",
		((cpsr>>7&0x1)?"off":"on"), ((cpsr>>6&0x1)?"off":"on"), (cpsr&0x1F));
	return 0;
}

static void cpu_pm_finish(void)
{
	u_int cpsr = getcpsr();

	PM_DBGOUT("%s\n", __func__);

	if (board_pm && board_pm->finish)
		board_pm->finish();

	printk("pm: finish, cpsr (irq:%s, fiq:%s, mode:0x%x)\n",
		((cpsr>>7&0x1)?"off":"on"), ((cpsr>>6&0x1)?"off":"on"), (cpsr&0x1F));
}

static void cpu_pm_end(void)
{
	PM_DBGOUT("%s\n", __func__);

	if (board_pm && board_pm->end)
		board_pm->end();

	printk("pm: end suspend (wake time:%lu ms)\n", (jiffies-wake_time)*(1000/HZ));
}

/*-----------------------------------------------------------------------------*/
static struct platform_suspend_ops cpu_pm_ops = {
	.valid		= suspend_valid_only_mem,
	.prepare	= cpu_pm_prepare,
	.enter		= cpu_pm_enter,
	.finish		= cpu_pm_finish,
	.end		= cpu_pm_end,
};

/* register pm ops function */
static int __init cpu_pm_init(void)
{
	u_int addr;
	DBGOUT("%s\n", __func__);
	sleep_virt_base = phys_to_virt(CFG_SLEEP_DATA_BASE);

	suspend_set_ops(&cpu_pm_ops);

	/* remap save data region */
	addr = (u_int)__virt_to_phys(sleep_save_data);
	sleep_save_base = (u_int)__arm_ioremap_pfn(__phys_to_pfn(addr), 0, SLEEP_MM_SIZE, MT_DEVICE);

	printk("CPU: sleep phys=0x%x to virt=0x%x, data phys=0x%x, virt=0x%x\n",
		sleep_phys_base, sleep_virt_base, (u_int)__virt_to_phys(addr), sleep_save_base);

	return 0;
}
core_initcall(cpu_pm_init);

/*------------------------------------------------------------------------------
 * 	PM export
 */
int cpu_get_wakeup_source(void)
{
	DBGOUT("%s\n", __func__);

	/*
	 * WakeUp source
	 *-----------------
 	 *  0 : Alive [0]
 	 *  1 : Alive [1]
 	 *  2 : Alive [2]
 	 *  3 : Alive [3]
 	 *  4 : Alive [4]
  	 *  5 : Alive [5]
 	 *  6 : Alive [6]
 	 *  7 : Alive [7]
 	 *  8 : Alarm
 	 *  9 : RTC
	 * 10 : VDDPWRTOGGLE
 	 */
	return g_wakeup_source;
}
EXPORT_SYMBOL(cpu_get_wakeup_source);

void cpu_register_board_pm(struct board_pm_fns *fns)
{
	if (! fns)
		return;
	else
		board_pm = fns;
}
EXPORT_SYMBOL(cpu_register_board_pm);

u_int cpu_get_reserve_pm_mem(int phys)
{
	if (phys)
		return sleep_phys_base;
	else
		return sleep_virt_base;
}
EXPORT_SYMBOL(cpu_get_reserve_pm_mem);

#endif	/* CONFIG_PM */
