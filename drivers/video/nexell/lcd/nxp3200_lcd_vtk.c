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
#include <linux/delay.h>
#include <linux/workqueue.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/soc.h>
#include <mach/dpc.h>

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "lcd: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define	RESUME_TIME		100 /* ms */

struct timer_list		resume_timer;
static int				blu_ch = -1;

#define CFG_LCD_PRI_LCD_PCI		(PAD_GPIO_A + 14)
/*----------------------------------------------------------------------------*/

static void do_blu_lcd_timer(unsigned long p)
{
	int ch = *(int *)p;
	PM_DBGOUT("+%s (ch:%d)\n", __func__, ch);

	/* Set lcd control */
#if (CFG_LCD_PRI_LCD_ON)
	if(0 == ch) {
		soc_gpio_set_io_func(CFG_LCD_PRI_LCD_PCI, 0);	/* 0 = GPIO mode */
		soc_gpio_set_io_dir(CFG_LCD_PRI_LCD_PCI, 1);	/* 1 = GPIO out */
		soc_gpio_set_out_value(CFG_LCD_PRI_LCD_PCI, 1);

		soc_gpio_set_io_func(CFG_PIO_LCD_PCI_ENB, 0);	/* 0 = GPIO mode */
		soc_gpio_set_io_dir(CFG_PIO_LCD_PCI_ENB, 1);	/* 1 = GPIO out */
		soc_gpio_set_out_value(CFG_PIO_LCD_PCI_ENB, 1);
	}
#endif

	/* Set pwm device */
	soc_pwm_set_resume_ch(ch);

	PM_DBGOUT("-%s (ch:%d)\n", __func__, ch);
}

static void nx_blu_ops_enable(u_int dev)
{
	DBGOUT("%s (dev:%d)\n", __func__, dev);

	/* Set pwm device */
#if (CFG_LCD_PRI_BLU_ON)
	if(0 == dev)
		soc_pwm_set_frequency(CFG_LCD_PRI_PWM_CH, CFG_LCD_PRI_PWM_FREQ, CFG_LCD_PRI_PWM_DUTYCYCLE);
#endif
}

static void nx_blu_ops_disable(u_int dev)
{
	DBGOUT("%s (dev:%d)\n", __func__, dev);

	/* Set pwm device */
#if (CFG_LCD_PRI_BLU_ON)
	if(0 == dev)
		soc_pwm_set_frequency(CFG_LCD_PRI_PWM_CH, CFG_LCD_PRI_PWM_FREQ, 0);
#endif
}

static void nx_blu_ops_suspend(u_int dev)
{
	u_long flags;

	PM_DBGOUT("%s (dev:%d)\n", __func__, dev);

	local_irq_save(flags);

	del_timer (&resume_timer);

	local_irq_restore(flags);

	/* Set pwm device */
#if (CFG_LCD_PRI_BLU_ON)
	blu_ch = CFG_LCD_PRI_PWM_CH;
	if(0 == dev)
		soc_pwm_set_suspend_ch(CFG_LCD_PRI_PWM_CH, CFG_LCD_PRI_PWM_FREQ, 0);
#endif

	PM_DBGOUT("-%s (dev:%d)\n", __func__, dev);
}

static void nx_blu_ops_resume(u_int dev)
{
	PM_DBGOUT("%s (dev:%d)\n", __func__, dev);

#if defined (CFG_LCD_PRI_BLU_ON)
	/* add timer */
	del_timer (&resume_timer);
	init_timer(&resume_timer);
	resume_timer.expires  = get_jiffies_64() + RESUME_TIME/(1000/msecs_to_jiffies(1000));
	resume_timer.function = do_blu_lcd_timer;
	resume_timer.data 	  = (unsigned long)&blu_ch;
	add_timer(&resume_timer);
#endif
}

static struct dpc_blu_ops blu_ops = {
	.enable		= nx_blu_ops_enable,
	.disable	= nx_blu_ops_disable,
	.suspend	= nx_blu_ops_suspend,
	.resume		= nx_blu_ops_resume,
};

/*----------------------------------------------------------------------------*/
static void nx_lcd_ops_enable(u_int dev)
{
	DBGOUT("%s (dev:%d)\n", __func__, dev);
	/* Set lcd control */
#if (CFG_LCD_PRI_LCD_ON)
	if(0 == dev) {
		soc_gpio_set_io_func(CFG_LCD_PRI_LCD_PCI, 0);	/* 0 = GPIO mode */
		soc_gpio_set_io_dir(CFG_LCD_PRI_LCD_PCI, 1);	/* 1 = GPIO out */
		soc_gpio_set_out_value(CFG_LCD_PRI_LCD_PCI, 1);

		soc_gpio_set_io_func(CFG_PIO_LCD_PCI_ENB, 0);	/* 0 = GPIO mode */
		soc_gpio_set_io_dir(CFG_PIO_LCD_PCI_ENB, 1);	/* 1 = GPIO out */
		soc_gpio_set_out_value(CFG_PIO_LCD_PCI_ENB, 1);
	}
#endif

}

static void nx_lcd_ops_disable(u_int dev)
{
	DBGOUT("%s (dev:%d)\n", __func__, dev);
	/* Set lcd control */
#if (CFG_LCD_PRI_LCD_ON)
	if(0 == dev) {
		soc_gpio_set_io_func(CFG_LCD_PRI_LCD_PCI, 0);	/* 0 = GPIO mode */
		soc_gpio_set_io_dir(CFG_LCD_PRI_LCD_PCI, 1);	/* 1 = GPIO out */
		soc_gpio_set_out_value(CFG_LCD_PRI_LCD_PCI, 0);

		soc_gpio_set_io_func(CFG_PIO_LCD_PCI_ENB, 0);	/* 0 = GPIO mode */
		soc_gpio_set_io_dir(CFG_PIO_LCD_PCI_ENB, 1);	/* 1 = GPIO out */
		soc_gpio_set_out_value(CFG_PIO_LCD_PCI_ENB, 0);
	}
#endif
}

static void nx_lcd_ops_suspend(u_int dev)
{
	PM_DBGOUT("%s (dev:%d)\n", __func__, dev);
	/* Set lcd control */
#if (CFG_LCD_PRI_LCD_ON)
	if(0 == dev) {
		soc_gpio_set_io_func(CFG_PIO_LCD_PCI_ENB, 0);	/* 0 = GPIO mode */
		soc_gpio_set_io_dir(CFG_PIO_LCD_PCI_ENB, 1);	/* 1 = GPIO out */
		soc_gpio_set_out_value(CFG_PIO_LCD_PCI_ENB, 0);
	}
#endif
}

static void nx_lcd_ops_resume(u_int dev)
{
	PM_DBGOUT("%s (dev:%d)\n", __func__, dev);
}

static int nx_lcd_ops_setformat(u_int dev, u_int format, u_int type, u_int opt)
{
	DBGOUT("%s (dev:%d)\n", __func__, dev);
	return 0;
}

static int nx_lcd_ops_getformat(u_int dev, u_int *format, u_int *type)
{
	DBGOUT("%s (dev:%d)\n", __func__, dev);
	return 0;
}

static struct dpc_lcd_ops lcd_ops = {
	.enable		= nx_lcd_ops_enable,
	.disable	= nx_lcd_ops_disable,
	.suspend	= nx_lcd_ops_suspend,
	.resume		= nx_lcd_ops_resume,
	.setformat	= nx_lcd_ops_setformat,
	.getformat	= nx_lcd_ops_getformat,
};

/*----------------------------------------------------------------------------*/
static int __init nx_lcd_init(void)
{
	DBGOUT("%s(blu:0x%x, lcd:0x%x)\n", __func__, (u_int)&blu_ops, (u_int)&lcd_ops);

	/* register blu and lcd */
#if defined(CONFIG_FB_NEXELL_PRIMARY)
	soc_dpc_register_blu(0, &blu_ops);
	soc_dpc_register_lcd(0, &lcd_ops);
#endif

	return 0;
}

static void __exit nx_lcd_exit(void)
{
	DBGOUT("%s\n", __func__);
}

module_init(nx_lcd_init);
module_exit(nx_lcd_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Backlight & LCD for nxp3200 vtk board");
MODULE_LICENSE("GPL");
