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
#include <linux/version.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>

#include <asm/mach/time.h>
#include <asm/mach/irq.h>

/* nexell soc headers */
#include <mach/platform.h>

#if (0)
#define DBGOUT(msg...)		{ printk("timer: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*------------------------------------------------------------------------*/
 struct timer_data {
	int		ch;
	u_int 	clksrc;
	u_int 	clkdiv;
	u_int 	period;
	int   	irq;
};

static struct timer_data src_timer;
static struct timer_data evt_timer;

/*--------------------------------------------------------------------------
 * Timer clock source
 */
#if KERNEL_VERSION(2,6,36) > LINUX_VERSION_CODE
static void timer_source_resume(void)
#else
static void timer_source_resume(struct clocksource *cs)
#endif
{
	int    ch 	  = src_timer.ch;
	u_int  clksrc = src_timer.clksrc;
	u_int  clkdiv = src_timer.clkdiv;
	u_long flags;

	PM_DBGOUT("+%s\n", __func__);

	local_irq_save(flags);

	/* setup timer as free-running clocksource */
	if (! NX_TIMER_GetClockDivisorEnable(ch)) {
		PM_DBGOUT("timer: resume source timer clock reset ...\n");
		NX_TIMER_SetClockSource(ch, 0, clksrc);
		NX_TIMER_SetClockDivisor(ch, 0, clkdiv);
		NX_TIMER_SetClockPClkMode(ch, NX_PCLKMODE_ALWAYS);
		NX_TIMER_SetClockDivisorEnable(ch, CTRUE);
	}

	NX_TIMER_Stop(ch);
	NX_TIMER_SetWatchDogEnable(ch, CFALSE);
	NX_TIMER_SetInterruptEnableAll(ch, CFALSE);
	NX_TIMER_SetTClkDivider(ch, NX_TIMER_CLOCK_TCLK);
	NX_TIMER_ClearInterruptPendingAll(ch);

	/* source timer run */
	NX_TIMER_SetTimerCounter(ch, 0x00000000);
	NX_TIMER_SetMatchCounter(ch, 0xffffffff);
	NX_TIMER_Run(ch);

	local_irq_restore(flags);

	PM_DBGOUT("-%s\n", __func__);
}

static cycle_t timer_source_read(struct clocksource *cs)
{
	int ch = src_timer.ch;
	return NX_TIMER_GetTimerCounter(ch);
}

static struct clocksource timer_clocksource = {
	.name 		= "timer_source",
 	.rating		= 300,
 	.read		= timer_source_read,
	.mask		= CLOCKSOURCE_MASK(32),
 	.shift 		= 20,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
	.resume		= timer_source_resume,
};

static int __init timer_source_init(int ch, u_int clksrc, u_int clkdiv, u_int freq)
{
	DBGOUT("%s (ch:%d, src:%d, div:%d, freq:%d)\n", __func__, ch, clksrc, clkdiv, freq);
	src_timer.ch 	 = ch;
	src_timer.clksrc = clksrc;
	src_timer.clkdiv = clkdiv;
	src_timer.period = 0xffffffff;
	src_timer.irq	 = -1;

	/* setup timer as free-running clocksource */
	NX_TIMER_SetClockSource(ch, 0, clksrc);
	NX_TIMER_SetClockDivisor(ch, 0, clkdiv);
	NX_TIMER_SetClockPClkMode(ch, NX_PCLKMODE_ALWAYS);
	NX_TIMER_SetClockDivisorEnable(ch, CTRUE);

	NX_TIMER_Stop(ch);
	NX_TIMER_SetWatchDogEnable(ch, CFALSE);
	NX_TIMER_SetInterruptEnableAll(ch, CFALSE);
	NX_TIMER_SetTClkDivider(ch, NX_TIMER_CLOCK_TCLK);
	NX_TIMER_ClearInterruptPendingAll(ch);

	/* register timer source */
	timer_clocksource.mult = clocksource_hz2mult(freq, timer_clocksource.shift);
	clocksource_register(&timer_clocksource);

	/* source timer run */
	NX_TIMER_SetTimerCounter(ch, 0x00000000);
	NX_TIMER_SetMatchCounter(ch, 0xffffffff);
	NX_TIMER_Run(ch);

	printk("timer: src ch(%d), freq:%d, mult:%u\n", ch, freq, timer_clocksource.mult);

 	return 0;
}

/*--------------------------------------------------------------------------
 * Timer clock event
 */
static void clock_event_shutdown(void)
{
	PM_DBGOUT("%s\n", __func__);
}

static void clock_event_resume(void)
{
	int    ch     = evt_timer.ch;
	u_int  clksrc = evt_timer.clksrc;
	u_int  clkdiv = evt_timer.clkdiv;
	u_long flags;

	local_irq_save(flags);

	PM_DBGOUT("+%s\n", __func__);

	if (! NX_TIMER_GetClockDivisorEnable(ch)) {
		PM_DBGOUT("timer: resume event timer clock reset ...\n");
		NX_TIMER_SetClockSource(ch, 0, clksrc);
		NX_TIMER_SetClockDivisor(ch, 0, clkdiv);
		NX_TIMER_SetClockPClkMode(ch, NX_PCLKMODE_ALWAYS);
		NX_TIMER_SetClockDivisorEnable(ch, CTRUE);
	}

	NX_TIMER_Stop(ch);
	NX_TIMER_SetWatchDogEnable(ch, CFALSE);
	NX_TIMER_SetInterruptEnableAll(ch, CFALSE);
	NX_TIMER_SetTClkDivider(ch, NX_TIMER_CLOCK_TCLK);
	NX_TIMER_ClearInterruptPendingAll(ch);

	/* resume event timer */
	NX_TIMER_SetInterruptEnableAll(ch, CTRUE);

	local_irq_restore(flags);

	PM_DBGOUT("-%s\n", __func__);
}

static void timer_event_set_mode(enum clock_event_mode mode, struct clock_event_device *clk)
{
	int   ch 	 = evt_timer.ch;
	u_int period = evt_timer.period;
	unsigned long flags;

	DBGOUT("%s (ch:%d, mode:0x%x, period:%d)\n", __func__, ch, mode, period);

	raw_local_irq_save(flags);

	switch(mode) {
	case CLOCK_EVT_MODE_UNUSED:		// 0x0
	case CLOCK_EVT_MODE_ONESHOT:	// 0x3
		break;

	case CLOCK_EVT_MODE_SHUTDOWN:	// 0x1
		clock_event_shutdown();
		break;

	case CLOCK_EVT_MODE_RESUME:		// 0x4
		clock_event_resume();

	case CLOCK_EVT_MODE_PERIODIC:	// 0x2
		NX_TIMER_Stop(ch);
		NX_TIMER_SetTimerCounter(ch, 0);
		NX_TIMER_SetMatchCounter(ch, period);
		NX_TIMER_Run(ch);
		break;

	default:
		break;
	}

	raw_local_irq_restore(flags);
}

static int timer_event_set_next_event(unsigned long evt, struct clock_event_device *unused)
{
	int ch = evt_timer.ch;
	unsigned long flags;

	raw_local_irq_save(flags);

	NX_TIMER_Stop(ch);
	NX_TIMER_ClearInterruptPendingAll(ch);
	NX_TIMER_SetTimerCounter(ch, 0);
	NX_TIMER_SetMatchCounter(ch, evt);
	NX_TIMER_Run(ch);

	raw_local_irq_restore(flags);

	return 0;
}

static struct clock_event_device timer_clockevent =	 {
	.name			= "timer_event",
	.shift			= 32,
	.features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.set_mode		= timer_event_set_mode,
	.set_next_event	= timer_event_set_next_event,
	.rating			= 300,
};

/* timer irq handler */
static irqreturn_t timer_irq_handler(int irq, void *dev_id)
{
	int ch = evt_timer.ch;
	struct clock_event_device *evt = &timer_clockevent;

	NX_TIMER_ClearInterruptPendingAll(ch);

	evt->event_handler(evt);

	return IRQ_HANDLED;
}

static struct irqaction timer_irqaction = {
	.name		= "Nexell Timer Tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.handler	= timer_irq_handler,
};

static int __init timer_event_init(int ch, u_int clksrc, u_int clkdiv, u_int freq)
{
	DBGOUT("%s (ch:%d, src:%d, div:%d, freq:%d)\n", __func__, ch, clksrc, clkdiv, freq);
	evt_timer.ch 	 = ch;
	evt_timer.clksrc = clksrc;
	evt_timer.clkdiv = clkdiv;
	evt_timer.period = freq/HZ;
	evt_timer.irq	 = NX_TIMER_GetInterruptNumber(ch);

	/* setup timer as free-running clocksource */
	NX_TIMER_SetClockSource(ch, 0, clksrc);
	NX_TIMER_SetClockDivisor(ch, 0, clkdiv);
	NX_TIMER_SetClockPClkMode(ch, NX_PCLKMODE_ALWAYS);
	NX_TIMER_SetClockDivisorEnable(ch, CTRUE);
	NX_TIMER_Stop(ch);

	NX_TIMER_SetWatchDogEnable(ch, CFALSE);
	NX_TIMER_SetInterruptEnableAll(ch, CFALSE);
	NX_TIMER_SetTClkDivider(ch, NX_TIMER_CLOCK_TCLK);
	NX_TIMER_ClearInterruptPendingAll(ch);

	/* Make irqs happen for the system timer */
	NX_TIMER_SetInterruptEnableAll(ch, CTRUE);

	setup_irq(evt_timer.irq, &timer_irqaction);

	/* register timer event device */
	timer_clockevent.mult 		  = div_sc(freq, NSEC_PER_SEC, timer_clockevent.shift);
	timer_clockevent.max_delta_ns = clockevent_delta2ns(0xffffffff, &timer_clockevent);
	timer_clockevent.min_delta_ns = clockevent_delta2ns(0xf, &timer_clockevent);
	timer_clockevent.cpumask 	  = cpumask_of(0);

	clockevents_register_device(&timer_clockevent);

	printk("timer: evt ch(%d), freq:%d, HZ:%d, mult:%u \n",
		ch, freq, HZ, (u_int)timer_clockevent.mult);

	return 0;
}

/*--------------------------------------------------------------------------
 * 	register timer init function
 */

static void __init cpu_timer_init(void)
{
	DBGOUT("%s\n", __func__);

	timer_source_init(
			CFG_TIMER_SYS_TICK_CH,
			CFG_TIMER_SYS_TICK_CLKSRC,
			CFG_TIMER_SYS_TICK_CLKDIV,
			cpu_get_clock_hz(CFG_TIMER_SYS_TICK_CLKSRC)
				/ CFG_TIMER_SYS_TICK_CLKDIV
			);

	timer_event_init(
			CFG_TIMER_SUB_TICK_CH,
			CFG_TIMER_SUB_TICK_CLKSRC,
			CFG_TIMER_SUB_TICK_CLKDIV,
			cpu_get_clock_hz(CFG_TIMER_SUB_TICK_CLKSRC)
				/ CFG_TIMER_SUB_TICK_CLKDIV
			);

	return;
}

struct sys_timer cpu_sys_timer = {
	.init		= cpu_timer_init,
};

