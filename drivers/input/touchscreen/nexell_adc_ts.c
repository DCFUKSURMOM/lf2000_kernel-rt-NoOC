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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/freezer.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "ts: " msg); }
#define	ERROUT(msg...)		{ printk(KERN_ERR  "ts: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#define	ERROUT(msg...)		do {} while (0)
#endif

#define	TS_SLEEP_TIME		10		/* msec */
#define	TSDN_DET_MODE		2		/* Falling edge detect mode */
#define	TS_CALIB_COUNT		7

#define	TS_SAMPLING_NUM		1
#define TS_COORDINATE_CNT	3

/*------------------------------------------------------------------------------
 * 	local data and macro
 */
struct ts_io {
	int		xm_p;
	int		xm_n;
	int		ym_p;
	int		ym_n;
	int		detect;		/* detect pen down */
	int		control;	/* detect pen condition */
	int 	delay;
	int		change;		/* change X/Y */
};

struct ts_adc {
	int		x_ch;
	int		y_ch;
	int 	x_min;
	int 	x_max;
	int 	y_min;
	int 	y_max;
};

struct ts_calibration {
	int		use;
	int		point[TS_CALIB_COUNT];
};

struct ts_info {
	struct input_dev  	* 	 inp;
	struct ts_io			 io;
	struct ts_adc			 adc;
	struct ts_calibration	 calib;

	struct workqueue_struct* wqueue;
	struct work_struct		 work;
	int						 pendown;
	int						 status;	/* open = 1, close = 0 */
};

#define	TS_STAT_OPEN	(1<<0)
#define	TS_STAT_RUN		(1<<1)
#define	TS_STAT_SUSPEND	(1<<2)

/* TS ADC position sampling */
struct ts_sample{
	struct {
	int		x;
	int		y;
	} coord[TS_SAMPLING_NUM];
	int 	count;
};

static struct ts_info  *__pts_info = NULL;
static struct ts_sample	__ts_sample;
/*------------------------------------------------------------------------------
 * 	local functions
 */

extern unsigned lf2000_gpio_l2p(struct gpio_chip* chip, unsigned offset);

static void ts_init_io_base(struct ts_info *tsi, struct adc_ts_plat_data *plat)
{
	DBGOUT("%s\n", __func__);

	/* set touch io pad */
	tsi->io.xm_p	= lf2000_gpio_l2p(NULL, plat->io.logical_xm_p);
	tsi->io.xm_n	= lf2000_gpio_l2p(NULL, plat->io.logical_xm_n);
	tsi->io.ym_p	= lf2000_gpio_l2p(NULL, plat->io.logical_ym_p);
	tsi->io.ym_n	= lf2000_gpio_l2p(NULL, plat->io.logical_ym_n);
	tsi->io.detect 	= lf2000_gpio_l2p(NULL, plat->io.logical_detect);
	tsi->io.control	= lf2000_gpio_l2p(NULL, plat->io.logical_control);
	tsi->io.delay	= plat->io.delay;

	/* set touch adc channel */
	tsi->adc.x_ch 	= plat->adc.x_ch;
	tsi->adc.y_ch	= plat->adc.y_ch;
	tsi->adc.x_min 	= plat->adc.x_min;
	tsi->adc.x_max	= plat->adc.x_max;
	tsi->adc.y_min 	= plat->adc.y_min;
	tsi->adc.y_max	= plat->adc.y_max;

	/* set calibration */
	if (plat->adc.pointercal[0] != 0 || plat->adc.pointercal[1] != 0 ||
		plat->adc.pointercal[2] != 0 || plat->adc.pointercal[3] != 0 ||
		plat->adc.pointercal[4] != 0 || plat->adc.pointercal[5] != 0 ||
		plat->adc.pointercal[6] != 0 )
	{
		tsi->calib.point[0] = plat->adc.pointercal[0];
		tsi->calib.point[1] = plat->adc.pointercal[1];
		tsi->calib.point[2] = plat->adc.pointercal[2];
		tsi->calib.point[3] = plat->adc.pointercal[3];
		tsi->calib.point[4] = plat->adc.pointercal[4];
		tsi->calib.point[5] = plat->adc.pointercal[5];
		tsi->calib.point[6] = plat->adc.pointercal[6];
		tsi->calib.use   = 1;
	} else {
		tsi->calib.use   = 0;
	}
	printk("%s: set calibrate '%s'...\n", TS_DEV_NAME, tsi->calib.use?"done":"pass");
}

static void ts_init_io_func(struct ts_info *tsi)
{
	DBGOUT("%s\n", __func__);

	/* io function */
	soc_gpio_set_io_func(tsi->io.xm_p 	 , NX_GPIO_PADFUNC_GPIO);
	soc_gpio_set_io_func(tsi->io.xm_n 	 , NX_GPIO_PADFUNC_GPIO);
	soc_gpio_set_io_func(tsi->io.ym_p 	 , NX_GPIO_PADFUNC_GPIO);
	soc_gpio_set_io_func(tsi->io.ym_n 	 , NX_GPIO_PADFUNC_GPIO);
	soc_gpio_set_io_func(tsi->io.control , NX_GPIO_PADFUNC_GPIO);
	soc_gpio_set_io_func(tsi->io.detect	 , NX_GPIO_PADFUNC_GPIO);

	/* io pullup */
	soc_gpio_set_io_pullup(tsi->io.xm_p    , 0);
	soc_gpio_set_io_pullup(tsi->io.xm_n    , 0);
	soc_gpio_set_io_pullup(tsi->io.ym_p    , 0);
	soc_gpio_set_io_pullup(tsi->io.ym_n    , 0);
	soc_gpio_set_io_pullup(tsi->io.control , 1);
	soc_gpio_set_io_pullup(tsi->io.detect  , 1);

	/* io direction: in / out */
	soc_gpio_set_io_dir(tsi->io.xm_p 	, 1);
	soc_gpio_set_io_dir(tsi->io.xm_n 	, 1);
	soc_gpio_set_io_dir(tsi->io.ym_p 	, 1);
	soc_gpio_set_io_dir(tsi->io.ym_n 	, 1);
	soc_gpio_set_io_dir(tsi->io.control	, 1);
	soc_gpio_set_io_dir(tsi->io.detect	, 0);

	/* io detect mode */
	soc_gpio_set_int_mode(tsi->io.detect, TSDN_DET_MODE);
}

static inline void ts_set_io_xm_p(int io, bool on)
{
	soc_gpio_set_out_value(io, (on ? 1 : 0));	// HIGH -> ON
}

static inline void ts_set_io_xm_n(int io, bool on)
{
	soc_gpio_set_out_value(io, (on ? 0 : 1));	// LOW -> ON
}

static inline void ts_set_io_ym_p(int io, bool on)
{
	soc_gpio_set_out_value(io, (on ? 1 : 0));	// HIGH -> ON
}

static inline void ts_set_io_ym_n(int io, bool on)
{
	soc_gpio_set_out_value(io, (on ? 0 : 1));	// LOW -> ON
}

static inline void ts_set_io_det(int io, bool on)
{
	soc_gpio_set_int_enable(io, on ? 1 : 0);
}

static inline bool ts_get_io_det(int io)
{
	return soc_gpio_get_in_value(io) ? 1 : 0;
}

static inline void ts_set_det_mode(struct ts_info *tsi, bool on)
{
	if (on) {
		ts_set_io_xm_p(tsi->io.xm_p, false);
		ts_set_io_xm_n(tsi->io.xm_n, false);
		ts_set_io_ym_n(tsi->io.ym_n, false);
		ts_set_io_ym_p(tsi->io.ym_p, true );	/* Y_M is Low */
	}

	/* Set control pad */
	soc_gpio_set_out_value(tsi->io.control, on ? 1 : 0);

	if (on)
		mdelay(1);
}

static int ts_get_coordinate(struct ts_info *tsi, int *tx, int *ty)
{
	int i;
	int ax[TS_COORDINATE_CNT];
	int ay[TS_COORDINATE_CNT];

	DBGOUT("%s\n", __func__);

	*tx = 0, *ty = 0;

	ts_set_io_xm_p(tsi->io.xm_p, true );
	ts_set_io_xm_n(tsi->io.xm_n, true );
	ts_set_io_ym_p(tsi->io.ym_p, false);
	ts_set_io_ym_n(tsi->io.ym_n, false);

	mdelay(tsi->io.delay);

	for (i = 0; TS_COORDINATE_CNT > i; i++) {
		ax[i] = soc_adc_read(tsi->adc.x_ch, 1000);
		*tx  += ax[i];
		if (0 > ax[i]) {
			printk("fail, read adc:%d value ...\n", tsi->adc.x_ch);
			return -1;
		}
	}
	*tx /= TS_COORDINATE_CNT;

	if (CFG_TOUCH_X_REV_VAL)
		*tx = ((1<<CFG_TOUCH_X_ADC_BIT) - *tx);

	ts_set_io_xm_p(tsi->io.xm_p, false);
	ts_set_io_xm_n(tsi->io.xm_n, false);
	ts_set_io_ym_p(tsi->io.ym_p, true );
	ts_set_io_ym_n(tsi->io.ym_n, true );

	mdelay(tsi->io.delay);

	for (i = 0; TS_COORDINATE_CNT > i; i++) {
		ay[i] = soc_adc_read(tsi->adc.y_ch, 1000);
		*ty  += ay[i];
		if (0 > ay[i]) {
			printk("fail, read adc:%d value ...\n", tsi->adc.y_ch);
			return -1;
		}
	}
	*ty /= TS_COORDINATE_CNT;

	if (CFG_TOUCH_Y_REV_VAL)
		*ty = ((1<<CFG_TOUCH_X_ADC_BIT) - *ty);

	/* swap */
	if (tsi->io.change) {
		*tx = *tx + *ty;
		*ty = *tx - *ty;
		*tx = *tx - *ty;
	}

	/* calibrate */
	if (tsi->calib.use) {
      int x = *tx, y = *ty;
      *tx = (tsi->calib.point[2] + tsi->calib.point[0]*x + tsi->calib.point[1]*y) / tsi->calib.point[6];
      *ty = (tsi->calib.point[5] + tsi->calib.point[3]*x + tsi->calib.point[4]*y) / tsi->calib.point[6];
	}

	if (0 > *tx || 0 > *ty)
		return -1;

	return 0;
}

static inline void ts_pendown(struct ts_info *tsi, int tx, int ty)
{
	DBGOUT("DN[x=%3d, y=%3d]\n", tx, ty);

	input_report_abs(tsi->inp, ABS_X, tx);
	input_report_abs(tsi->inp, ABS_Y, ty);
	input_report_abs(tsi->inp, ABS_PRESSURE, 1);
	input_report_key(tsi->inp, BTN_TOUCH, 1);

	input_sync(tsi->inp);
}

static inline void ts_penup(struct ts_info *tsi)
{
	DBGOUT("UP\n");

	input_report_key(tsi->inp, BTN_TOUCH, 0);
	input_report_abs(tsi->inp, ABS_PRESSURE, 0);
	input_sync(tsi->inp);
}

/*------------------------------------------------------------------------------
 * 	Touchscreen interrupt handler, pen down detect
 */
static void ts_do_thread(struct work_struct *data)
{
	struct ts_info *tsi = __pts_info;
	int io = tsi->io.detect;

#if (0)
	struct task_struct * tsk = current;
	struct sched_param param = { .sched_priority = 1 };
	sched_setscheduler(tsk, SCHED_FIFO, &param);
#endif

	DBGOUT("Enter %s (preempt:%d)\n", __func__, preempt_count());
	set_freezable();
	soc_adc_attach();
	tsi->status |= TS_STAT_RUN;

	while (1) {
		int  x = 0,  y = 0;
		int sx = 0, sy = 0;

	#if (TS_SAMPLING_NUM >= 2)
		struct ts_sample *tss = &__ts_sample;
		int i = 0, cnt = 0;
	#endif

		ts_set_det_mode(tsi, true);

		/* close or suspend  */
		if (! (TS_STAT_OPEN & tsi->status) ||
		   (TS_STAT_SUSPEND & tsi->status))
			break;

		/* pen up */
		if (ts_get_io_det(io))
			break;

		ts_set_det_mode(tsi, false);

		if (0 > ts_get_coordinate(tsi, &x, &y))
			continue;

		sx = x, sy = y;

	#if (TS_SAMPLING_NUM >= 2)
		tss->coord[tss->count].x = x;
		tss->coord[tss->count].y = y;
		tss->count++;

		sx  = 0, sy = 0;
		cnt = tss->count;

		if (tss->count >= TS_SAMPLING_NUM)
			tss->count = 0;

		for ( i = 0; cnt > i; i++){
			sx += tss->coord[i].x;
			sy += tss->coord[i].y;
		}
		sx /= cnt, sy /= cnt;
	#endif

		ts_pendown(tsi, sx, sy);
		tsi->pendown = 1;
		msleep(TS_SLEEP_TIME);
	}

	if (tsi->pendown)
		ts_penup(tsi);

	enable_irq(PB_PIO_IRQ(io));
	soc_adc_detach();
	tsi->status &= ~TS_STAT_RUN;

	DBGOUT("Exit  %s (preempt:%d, stat:%x)\n", __func__, preempt_count(), tsi->status);
}

static irqreturn_t ts_do_handler(int irqno, void *dev_id)
{
	struct ts_info 	 *tsi = dev_id;
#if (TS_SAMPLING_NUM >= 2)
	struct ts_sample *tss = &__ts_sample;
	tss->count = 0;
#endif
	DBGOUT("irq ts [DN, stat:0x%x] ...\n", tsi->status);

	tsi->pendown = 0;

	/* disable irq until occur penup event */
	disable_irq_nosync(irqno);
	queue_work(tsi->wqueue, &tsi->work);

	return IRQ_HANDLED;
}

/*------------------------------------------------------------------------------
 * 	Touchscreen functions
 */

static int nx_ts_open(struct input_dev *dev)
{
	struct ts_info   *tsi = input_get_drvdata(dev);
	struct ts_sample *tss = &__ts_sample;
	int io = tsi->io.detect;

	DBGOUT("+++ %s\n", __func__);

	memset(&__ts_sample, 0, sizeof(struct ts_sample));

	/* set status flags */
	tsi->pendown = 0;
	tsi->status  = TS_STAT_OPEN;	/* open */
	tss->count   = 0;

	/* set detect mode */
	ts_set_io_det  (io , true);
	ts_set_det_mode(tsi, true);

	DBGOUT("--- %s (stat:0x%x)\n", __func__, tsi->status);
	return 0;
}

static void nx_ts_close(struct input_dev *dev)
{
	struct ts_info *tsi = input_get_drvdata(dev);
	int io = tsi->io.detect;
	volatile int wait = 500;

	DBGOUT("+++ %s\n", __func__);
	tsi->status &= ~TS_STAT_OPEN; 	/* close */

	while (TS_STAT_RUN & tsi->status && wait-- >=0)
		msleep(1);

	ts_set_io_det  (io , false);
	ts_set_det_mode(tsi, false);
	DBGOUT("--- %s (stat:0x%x)\n", __func__, tsi->status);
}

/*--------------------------------------------------------------------------------
 * Touchscreen platform_driver functions
 ---------------------------------------------------------------------------------*/
/*
 * cat /sys/devices/platform/touch/calibration
 */
static ssize_t ts_show_calibration(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	struct ts_info *pts = __pts_info;
	char *s = buf;

	DBGOUT("%s:\n", __func__);

	s += sprintf(s, "%d,%d,%d,%d,%d,%d,%d\n",
		pts->calib.point[0], pts->calib.point[1],
		pts->calib.point[2], pts->calib.point[3],
		pts->calib.point[4], pts->calib.point[5],
		pts->calib.point[6]);

	if (s != buf)
		*(s-1) = '\n';

	return (s - buf);
}

/*
 * step 1:
 *	- clear default calibration value
 * 	#> echo > /sys/devices/platform/touch/calibration
 *
 * step 2:
 *	- calibration with "ts_calibrate"
 *
 * step 3:
 *	- get new calibration value
 *	#> cat .../tslib/etc/pointercal
 *
 * step 4:
 *	- set new calibration value
 *	#> echo xxxx.xxx.xx.... > /sys/devices/platform/touch/calibration
*
 * step 5:
 *	- delete .../tslib/etc/pointercal
 *	#> rm .../tslib/etc/pointercal
 *
 */
static ssize_t ts_store_calibration(struct kobject *kobj,
			 struct kobj_attribute *attr, const char *buf, size_t n)
{
	struct ts_info *pts = __pts_info;

	DBGOUT("%s:\n", __func__);

	memset(&pts->calib, 0, sizeof(struct ts_calibration));

	if (NULL == buf)
		return n;

	sscanf(buf,"%d,%d,%d,%d,%d,%d,%d",
		&pts->calib.point[0], &pts->calib.point[1],
		&pts->calib.point[2], &pts->calib.point[3],
		&pts->calib.point[4], &pts->calib.point[5],
		&pts->calib.point[6]);

	if (pts->calib.point[0] != 0 || pts->calib.point[1] != 0 ||
		pts->calib.point[2] != 0 || pts->calib.point[3] != 0 ||
		pts->calib.point[4] != 0 || pts->calib.point[5] != 0 ||
		pts->calib.point[6] != 0 )
		pts->calib.use = 1;
	else
		pts->calib.use = 0;

	return n;
}

static struct kobj_attribute attr_calibration = {
    .attr   = {
        .name = __stringify(calibration),
        .mode = 0666,
    },
    .show   = ts_show_calibration,
    .store  = ts_store_calibration,
};

static struct attribute *ts_attrs[] = {
	&attr_calibration.attr,
	NULL
};

static struct attribute_group ts_attr_group = {
	.attrs = ts_attrs,
};

static int nx_ts_probe(struct platform_device *pdev)
{
	struct adc_ts_plat_data * plat = pdev->dev.platform_data;
	struct kobject   *kobj = NULL;
	struct input_dev *inp  = NULL;
	struct ts_info   *tsi  = NULL;
	int irq, ret = 0;

	DBGOUT("%s (device name:%s, id:%d)\n", __func__, pdev->name, pdev->id);

	/*	allocate ts_info data */
	tsi = kzalloc(sizeof(struct ts_info), GFP_KERNEL);
	if (! tsi) {
		printk(KERN_ERR "fail, %s allocate driver info ...\n", pdev->name);
		return -ENOMEM;
	}

	ts_init_io_base(tsi, plat);
	ts_init_io_func(tsi);

	ts_set_io_det(tsi->io.detect, false);
	ts_set_det_mode(tsi, false);

	irq = PB_PIO_IRQ(tsi->io.detect);
	ret = request_irq(irq, ts_do_handler, IRQF_DISABLED, pdev->name, tsi);
	if (ret) {
		printk(KERN_ERR "fail, %s request detect dectet io %d ...\n", pdev->name, irq);
		goto err_mem;
	}

	inp = input_allocate_device();
	if (! inp) {
		printk(KERN_ERR "fail, %s allocate input device\n", pdev->name);
		ret = -ENOMEM;
		goto err_irq;
	}

	inp->name	  	= "Nexell Touchscreen";
	inp->phys 	  	= "nexell/event0";
	inp->open	  	= nx_ts_open;
	inp->close	  	= nx_ts_close;
	inp->dev.parent	= &pdev->dev;

	inp->id.bustype = BUS_HOST;
	inp->id.vendor  = 0x0001;
	inp->id.product = 0x0001;
	inp->id.version = 0x0100;

	inp->absbit[0] = BIT(ABS_X) | BIT(ABS_Y);	/* for android */
	inp->evbit [0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	inp->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);

	input_set_abs_params(inp, ABS_X, tsi->adc.x_min, tsi->adc.x_max, 0, 0);
	input_set_abs_params(inp, ABS_Y, tsi->adc.y_min, tsi->adc.y_max, 0, 0);
	input_set_abs_params(inp, ABS_PRESSURE, 0, 1, 0, 0);	/* Touchscreen param */
	input_set_abs_params(inp, ABS_TOOL_WIDTH, 0, 1, 0, 0);

	input_set_drvdata(inp, tsi);

	ret = input_register_device(inp);
	if (ret) {
		printk(KERN_ERR "fail, %s register for input device ...\n", pdev->name);
		goto err_dev;
	}

	/* create attribute interface */
	kobj = kobject_create_and_add("touch", &platform_bus.kobj);
	if (! kobj) {
		printk(KERN_ERR "fail, create touch kobject ...\n");
		ret = -ENOMEM;
		goto err_dev;
	}

	ret = sysfs_create_group(kobj, &ts_attr_group);
	if (ret) {
		printk(KERN_ERR "fail, create touch sysfs group ...\n");
		goto err_dev;
	}

	/* set touch input device */
	__pts_info  = tsi;
	tsi->inp    = inp;
	tsi->wqueue = create_singlethread_workqueue("ts-kthread");
	if (NULL == tsi->wqueue) {
		printk(KERN_ERR "fail, run 'ts-kthread' create thread workqueue ...\n");
		goto err_dev;
	}
    INIT_WORK(&tsi->work, ts_do_thread);

	/* set driver data */
	platform_set_drvdata(pdev, tsi);
	printk("%s.%d: register to %s\n", pdev->name, pdev->id,
		(char*)(struct kobject*)(&inp->dev.kobj)->name);

	return ret;

err_dev:
	input_free_device(inp);

err_irq:
	free_irq(irq, tsi);

err_mem:
	kfree(tsi);

	return ret;
}

static int nx_ts_remove(struct platform_device *pdev)
{
	struct ts_info   *tsi = platform_get_drvdata(pdev);
	struct input_dev *inp = tsi->inp;
	int irq = PB_PIO_IRQ(tsi->io.detect);
	int io  = tsi->io.detect;

	DBGOUT("%s\n", __func__);

	ts_set_io_det  (io , false);
	ts_set_det_mode(tsi, false);

	sysfs_remove_group(&pdev->dev.kobj, &ts_attr_group);
	input_unregister_device(inp);
	if (tsi->wqueue)
		destroy_workqueue(tsi->wqueue);
	free_irq(irq, tsi);
	kfree(tsi);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static int nx_ts_suspend(struct platform_device *dev, pm_message_t state)
{
	struct ts_info *tsi = platform_get_drvdata(dev);
	PM_DBGOUT("+++ %s\n", __func__);
	tsi->status |= TS_STAT_SUSPEND;

	while (TS_STAT_RUN & tsi->status)
		msleep(1);

	PM_DBGOUT("--- %s\n", __func__);
	return 0;
}

static int nx_ts_resume(struct platform_device *dev)
{
	struct ts_info *tsi = platform_get_drvdata(dev);
	int io  = tsi->io.detect;
	int irq = PB_PIO_IRQ(tsi->io.detect);

	PM_DBGOUT("+%s\n", __func__);

	disable_irq(irq);

	ts_init_io_func(tsi);
	tsi->status &= ~TS_STAT_SUSPEND;

	if (TS_STAT_OPEN & tsi->status) {
		ts_set_io_det  (io , true);
		ts_set_det_mode(tsi, true);
	}
	enable_irq(irq);

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

static struct platform_driver ts_plat_driver = {
	.probe		= nx_ts_probe,
	.remove		= nx_ts_remove,
	.suspend	= nx_ts_suspend,
	.resume		= nx_ts_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= TS_DEV_NAME,
	},
};

static int __init nx_ts_init(void)
{
	return platform_driver_register(&ts_plat_driver);
}

static void __exit nx_ts_exit(void)
{
	platform_driver_unregister(&ts_plat_driver);
}

module_init(nx_ts_init);
module_exit(nx_ts_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Touchscreen (with ADC) driver for the Nexell");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:nexell touchscreen");

