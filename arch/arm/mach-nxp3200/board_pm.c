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

#include <mach/hardware.h>
#include <asm/memory.h>
#include <asm/system.h>

#if defined(CONFIG_PM)
#include <mach/platform.h>
#include <mach/board_pm.h>

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "board-pm: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

extern void __init board_init(void);
/*------------------------------------------------------------------------------
 * 	PM ops
 */
static int 	board_pm_prepare(void)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

static int board_pm_enter(suspend_state_t state)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

#if (1)
static int board_pm_poweroff(void)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}
#else
#include <linux/random.h>
static int board_pm_poweroff(void)
{
	unsigned int rand;
	int ret = 0;

	PM_DBGOUT("%s\n", __func__);

	rand  = random32();
	rand %= 10;
	if (1 == rand)
		ret = 1;

	lldebugout("\n++++++++++++++++++++++++++++\n");
	lldebugout(" board power off <%s, %d>\n", ret?"FAIL":"DONE", rand);
	lldebugout("++++++++++++++++++++++++++++\n\n");
	return ret;
}
#endif

static void board_pm_poweron(void)
{
	PM_DBGOUT("+%s\n", __func__);
	board_init();
	PM_DBGOUT("-%s\n", __func__);
}

static void board_pm_finish(void)
{
	PM_DBGOUT("%s\n", __func__);
}

static void board_pm_end(void)
{
	PM_DBGOUT("%s\n", __func__);
}

/*-----------------------------------------------------------------------------*/
static struct board_pm_fns board_fns = {
	.prepare	= board_pm_prepare,
	.enter		= board_pm_enter,
	.poweroff	= board_pm_poweroff,
	.poweron	= board_pm_poweron,
	.finish		= board_pm_finish,
	.end		= board_pm_end,
};

/* register pm ops function */
static int __init board_pm_init(void)
{
	DBGOUT("%s\n", __func__);

	cpu_register_board_pm(&board_fns);
	return 0;
}
postcore_initcall(board_pm_init);

#endif	/* CONFIG_PM */