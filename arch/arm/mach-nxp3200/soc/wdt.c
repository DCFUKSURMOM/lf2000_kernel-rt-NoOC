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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/delay.h>

/* nexell soc headers */
#include <mach/platform.h>

#if (0)
#define DBGOUT(msg...)		{ printk("wdt: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*-----------------------------------------------------------------------------*/
static u_int	wdt_run  = 0;
static u_int	wdt_time = 0;

/*-----------------------------------------------------------------------------*/
void soc_wdt_init(void)
{
	DBGOUT("%s\n", __func__);
}

/*------------------------------------------------------------------------------
 * 	Description	: start watchdog and set time.
 *	In[msec]	: watchdog time (unit: mili second)
 *	Return 		: -1 = failure, current run status or invalid time,
 *				:  0 = success
 */
int soc_wdt_start(u_int msec)
{
	u_long flags;
	int ret = 0;

	/* disable irq */
	local_irq_save(flags);

	DBGOUT("%s (current %s status, %d msec)\n", __func__, wdt_run?"run":"stop", msec);
	if (wdt_run) {
		printk(KERN_ERR "fail, watchdog is running (%d msec, remain %d msec)\n",
			wdt_time, wdt_time-(NX_TIMER_GetTimerCounter(CFG_TIMER_WDT_TICK_CH)/1000));
		ret = -1;
		goto err;
	}

	if ((CFG_TIMER_WDT_TICK_CLKFREQ/1000)*wdt_time > 0xFFFFFFFF) {
		printk(KERN_ERR "fail, watchdog time overflow %lu\n",
			(CFG_TIMER_WDT_TICK_CLKFREQ/1000)*wdt_time);
		ret = -1;
		goto err;
	}

	wdt_time = msec;
	wdt_run  = 1;

	if (! NX_TIMER_GetClockDivisorEnable(CFG_TIMER_WDT_TICK_CH)) {
		NX_TIMER_SetClockSource(CFG_TIMER_WDT_TICK_CH, 0, CFG_TIMER_WDT_TICK_CLKSRC);
		NX_TIMER_SetClockDivisor(CFG_TIMER_WDT_TICK_CH, 0, CFG_TIMER_WDT_TICK_CLKDIV);
		NX_TIMER_SetClockPClkMode(CFG_TIMER_WDT_TICK_CH, NX_PCLKMODE_ALWAYS);
		NX_TIMER_SetClockDivisorEnable(CFG_TIMER_WDT_TICK_CH, CTRUE);
	}

	NX_TIMER_Stop(CFG_TIMER_WDT_TICK_CH);
	NX_TIMER_SetWatchDogEnable(CFG_TIMER_WDT_TICK_CH, CFALSE);
	NX_TIMER_SetInterruptEnableAll(CFG_TIMER_WDT_TICK_CH, CFALSE);
	NX_TIMER_SetTClkDivider(CFG_TIMER_WDT_TICK_CH, NX_TIMER_CLOCK_TCLK);
	NX_TIMER_ClearInterruptPendingAll(CFG_TIMER_WDT_TICK_CH);

	NX_TIMER_SetTimerCounter(CFG_TIMER_WDT_TICK_CH, 0);
	NX_TIMER_SetMatchCounter(CFG_TIMER_WDT_TICK_CH, (CFG_TIMER_WDT_TICK_CLKFREQ/1000)*wdt_time);

	NX_TIMER_SetInterruptEnableAll(CFG_TIMER_WDT_TICK_CH, CFALSE);
	NX_TIMER_SetWatchDogEnable(CFG_TIMER_WDT_TICK_CH, CTRUE);
	NX_TIMER_Run(CFG_TIMER_WDT_TICK_CH);

err:
	/* enable irq */
	local_irq_restore(flags);

	return ret;
}
EXPORT_SYMBOL(soc_wdt_start);

/*------------------------------------------------------------------------------
 * 	Description	: stop watchdog.
 *	Return 		: none
 */
void soc_wdt_stop(void)
{
	u_long flags;

	/* disable irq */
	local_irq_save(flags);

	DBGOUT("%s (%s, %d msec)\n", __func__, wdt_run?"run":"stop", wdt_time);

	if (! wdt_run) {
		local_irq_restore(flags);
		return;
	}

	NX_TIMER_Stop(CFG_TIMER_WDT_TICK_CH);
	NX_TIMER_ClearInterruptPendingAll(CFG_TIMER_WDT_TICK_CH);
	NX_TIMER_SetInterruptEnableAll(CFG_TIMER_WDT_TICK_CH, CFALSE);

	wdt_time = 0;
	wdt_run  = 0;

	local_irq_restore(flags);
}
EXPORT_SYMBOL(soc_wdt_stop);

/*------------------------------------------------------------------------------
 * 	Description	: clear watchdog time
 *	Return 		: none
 */
void soc_wdt_clear(void)
{
	u_long flags;

	/* disable irq */
	local_irq_save(flags);

	DBGOUT("%s (%s, %d msec)\n", __func__, wdt_run?"run":"stop", wdt_time);

	if (! wdt_run) {
		local_irq_restore(flags);
		return;
	}

	NX_TIMER_SetTimerCounter(CFG_TIMER_WDT_TICK_CH, 0);

	local_irq_restore(flags);
}
EXPORT_SYMBOL(soc_wdt_clear);

/*------------------------------------------------------------------------------
 * 	Description	: return watchdog remain time
 *	Return 		: watchdog remain time (mili sec)
 */
u_int soc_wdt_remain(void)
{
	u_int time = 0;
	DBGOUT("%s (%s, %d msec)\n", __func__, wdt_run?"run":"stop", wdt_time);

	if (! wdt_run)
		return 0;

	time = NX_TIMER_GetTimerCounter(CFG_TIMER_WDT_TICK_CH);

	return (wdt_time * 1000 - time)/1000;
}
EXPORT_SYMBOL(soc_wdt_remain);