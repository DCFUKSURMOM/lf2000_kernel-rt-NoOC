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
#include <linux/hardirq.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/soc.h>

#if (0)
#define DBGOUT(msg...)		{ printk("pwm: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*-----------------------------------------------------------------------------*/
struct pwm_device{
	u_int 	ch;			/* 0 ~ 1,  2 channels */
	u_int 	duty;		/* unit % 0% ~ 100% */
	u_int 	freq;		/* unit Hz 237Hz ~ 30KHz, default = 1000 */
	u_int 	enable;
	u_int	suspend_ch;
	u_int 	io;
	u_int 	iofunc;
};

static struct pwm_device gpwm[] = {
	{ 0,	0,	0,	0,	0, (PAD_GPIO_B + 2),	NX_GPIO_PADFUNC_1	},
	{ 1,	0, 	0,	0, 	0, (PAD_GPIO_B + 3), 	NX_GPIO_PADFUNC_1	},
};

#define	PWM_CHANNEL_NUM		(sizeof(gpwm)/sizeof(gpwm[0]))
#define	PWM_CLOCK_FREQ		(CFG_PWM_CLK_FREQ / CFG_PWM_CLK_DIV)
#define	PWM_CLOCK_COUNTER	(PWM_CLOCK_FREQ / CFG_PWM_PERIOD)

static DEFINE_MUTEX(mutex);
static u_int g_clk_enb = 0;

#define	PWM_LOCK(id)		{ if (! preempt_count() && ! in_interrupt()) mutex_lock(&mutex);   }
#define	PWM_UNLOCK(id)		{ if (mutex_is_locked(&mutex)) mutex_unlock(&mutex); }

/*-----------------------------------------------------------------------------*/
void soc_pwm_init(void)
{
	DBGOUT("%s (total ch:%d)\n", __func__, PWM_CHANNEL_NUM);

	NX_PWM_SetBaseAddress((U32)IO_ADDRESS(NX_PWM_GetPhysicalAddress()));
	NX_PWM_OpenModule();

	NX_PWM_SetClockSource(0, CFG_PWM_CLK_SOURCE);
	NX_PWM_SetClockDivisor(0, CFG_PWM_CLK_DIV);
	NX_PWM_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);
	NX_PWM_SetClockDivisorEnable(CFALSE);

	printk(KERN_INFO "pwm : %lu hz ~ %lu hz, pll:%lu\n",
		PWM_CLOCK_COUNTER/128, PWM_CLOCK_COUNTER/1, CFG_PWM_CLK_FREQ);
}

static int set_pwm_freq(int ch, u_int freq, u_int duty)
{
	struct pwm_device * pwm = &gpwm[ch];
	u_int io = pwm->io;
	u_int fn = pwm->iofunc;
	u_int scale  = 0;
	int   clkenb = 0;
	int   i = 0;
	DBGOUT("%s (ch:%d, freq:%d, duty:%d)\n", __func__, ch, freq, duty);

	if (freq > PWM_CLOCK_COUNTER || PWM_CLOCK_COUNTER/128 > freq) {
		printk("\nInvalid pwm[%d], freq:%d hz (%lu ~ %lu) \n",
			ch, freq, PWM_CLOCK_COUNTER/128, PWM_CLOCK_COUNTER/1);
		return (-1);
	}

	if (duty > 100 || 0 > duty) {
		printk("\nInvalid pwm[%d], duty:%d percent (0 ~ 100)\n", ch, duty);
		return (-1);
	}

	/* lock */
	PWM_LOCK(0);

	scale = PWM_CLOCK_COUNTER / freq;

	NX_PWM_SetPreScale(ch, scale);
	NX_PWM_SetPeriod(ch, CFG_PWM_PERIOD);	/* period is always 100 */
	NX_PWM_SetDutyCycle(ch, duty);
	printk("pwm ch[%d]: freq:%lu hz, duty:%d\n", ch, PWM_CLOCK_COUNTER/scale, duty);

	if (CFG_PWM_PERIOD >= duty && duty > 0)
		pwm->enable = 1;
	else
		pwm->enable = 0;

	/* check pwm status */
	for (i = 0; PWM_CHANNEL_NUM > i; i++) {
		DBGOUT("pwm ch[%d] - on:%d\n", i, gpwm[i].enable);
		if (gpwm[i].enable)
			clkenb = 1;
	}
	DBGOUT("clock %d, current clock:%d \n", clkenb, g_clk_enb);

	/* pwm clock */
	if (clkenb && ! g_clk_enb) {
		NX_PWM_SetClockSource(0, CFG_PWM_CLK_SOURCE);
		NX_PWM_SetClockDivisor(0, CFG_PWM_CLK_DIV);
		NX_PWM_SetClockPClkMode(NX_PCLKMODE_ALWAYS);
		NX_PWM_SetClockDivisorEnable(CTRUE);
	}

	if (!clkenb)	{
		DBGOUT("disable pwm clock ...\n");
		NX_PWM_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);
		NX_PWM_SetClockDivisorEnable(CFALSE);
	}

	/* pwm pad */
	if (pwm->enable) {
		soc_gpio_set_io_func(io, fn);
	} else {
		soc_gpio_set_out_value(io, 0);
		soc_gpio_set_io_dir(io, 1);
		soc_gpio_set_io_pullup(io, 0);
		soc_gpio_set_io_func(io, NX_GPIO_PADFUNC_GPIO);
	}

	/* clock status */
	g_clk_enb = clkenb;

	/* unlock */
	PWM_UNLOCK(0);

	return 0;
}

/*------------------------------------------------------------------------------
 * 	Description	: set pwm frequncy, and save frequency info
 *	In[ch]		: pwm channel, 0 ~ 3
 *	In[freq]	: pwm frequency, unit (Hz), : 30Hz ~ 237KHz
 *	In[duty]	: pwm pulse width, unit (%), 0 % ~ 100 %
 *				: if 0, pulse is low out.
 *	Return 		: none.
 */
void soc_pwm_set_frequency(int ch, u_int freq, u_int duty)
{
	struct pwm_device * pwm = NULL;
	u_int  org_freq, org_duty;

	DBGOUT("%s\n", __func__);

	NX_ASSERT(PWM_CHANNEL_NUM > ch && ch >= 0);

	pwm = &gpwm[ch];

	org_freq  = pwm->freq;
	org_duty  = pwm->duty;
	pwm->freq = freq;
	pwm->duty = duty;

	if (0 > set_pwm_freq(ch, freq, duty)) {
		pwm->freq = org_freq;
		pwm->duty = org_duty;
		return;
	}
}
EXPORT_SYMBOL_GPL(soc_pwm_set_frequency);

/*------------------------------------------------------------------------------
 * 	Description	: get pwm frequncy info
 *	In[ch]		: pwm channel, 0 ~ 3
 *	out[freq]	: pwm frequency, unit (Hz), : 30Hz ~ 237KHz
 *	out[duty]	: pwm pulse width, unit (%), 0 % ~ 100 %
 *	Return 		: none.
 */
void soc_pwm_get_frequency(int ch, uint *freq, uint *duty)
{
	struct pwm_device * pwm = NULL;

	DBGOUT("%s\n", __func__);

	NX_ASSERT(PWM_CHANNEL_NUM > ch && ch >= 0);

	pwm   = &gpwm[ch];
	*freq = pwm->freq;
	*duty = pwm->duty;

	DBGOUT("%s (ch:%d, freq:%d, duty:%d)\n", __func__, ch, *freq, *duty);
}
EXPORT_SYMBOL_GPL(soc_pwm_get_frequency);

/*------------------------------------------------------------------------------
 * 	Description	: set suspend mode,
 *	Return 		: none.
 */
void soc_pwm_set_suspend(void)
{
	PM_DBGOUT("+%s\n", __func__);
	/* clock status */
	g_clk_enb = 0;

	PM_DBGOUT("-%s\n", __func__);
}
EXPORT_SYMBOL_GPL(soc_pwm_set_suspend);

/*------------------------------------------------------------------------------
 * 	Description	: resume mode, restore saved pwm frequency.
 *	In[ch]		: pwm channel, 0 ~ 3
 *	Return 		: none.
 */
void soc_pwm_set_resume(void)
{
	struct pwm_device * pwm = NULL;
	int ch = 0;

	PM_DBGOUT("+%s\n", __func__);

	for (; PWM_CHANNEL_NUM > ch; ch++) {
		pwm = &gpwm[ch];
		if (! pwm->suspend_ch)
			set_pwm_freq(ch, pwm->freq, pwm->duty);
	}
	PM_DBGOUT("-%s\n", __func__);
}
EXPORT_SYMBOL_GPL(soc_pwm_set_resume);

/*------------------------------------------------------------------------------
 * 	Description	: ch suspend mode, it's not save frequency info
 *	In[ch]		: pwm channel, 0 ~ 3
 *	In[freq]	: pwm frequency, unit (Hz), : 30Hz ~ 237KHz
 *	In[duty]	: pwm pulse width, unit (%), 0 % ~ 100 %
 *				: if 0, pulse is low out.
 *	Return 		: none.
 */
void soc_pwm_set_suspend_ch(int ch, u_int freq, u_int duty)
{
	struct pwm_device * pwm = NULL;

	PM_DBGOUT("+%s\n", __func__);

	NX_ASSERT(PWM_CHANNEL_NUM > ch && ch >= 0);

	pwm = &gpwm[ch];
	pwm->suspend_ch = 1;

	set_pwm_freq(ch, freq, duty);

	/* clock status */
	g_clk_enb = 0;

	PM_DBGOUT("-%s\n", __func__);
}
EXPORT_SYMBOL_GPL(soc_pwm_set_suspend_ch);

/*------------------------------------------------------------------------------
 * 	Description	: ch resume mode, restore saved pwm frequency.
 *	In[ch]		: pwm channel, 0 ~ 3
 *	Return 		: none.
 */
void soc_pwm_set_resume_ch(int ch)
{
	struct pwm_device * pwm = NULL;

	PM_DBGOUT("+%s\n", __func__);

	NX_ASSERT(PWM_CHANNEL_NUM > ch && ch >= 0);

	pwm = &gpwm[ch];
	pwm->suspend_ch = 0;

	set_pwm_freq(ch, pwm->freq, pwm->duty);

	PM_DBGOUT("-%s\n", __func__);
}
EXPORT_SYMBOL_GPL(soc_pwm_set_resume_ch);

