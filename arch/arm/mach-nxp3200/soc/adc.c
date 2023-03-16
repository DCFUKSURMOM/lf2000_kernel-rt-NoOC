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
#include <linux/platform_device.h>
#include <linux/device.h>

/* nexell soc headers */
#include <mach/devices.h>
#include <mach/platform.h>
#include <mach/soc.h>

extern u_int cpu_get_clock_hz(cfg_sys_clksrc clk);

#if (0)
#define DBGOUT(msg...)		{ printk("adc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*-----------------------------------------------------------------------------*/
#define ADC_SPS		500000
static int 			attach_count = 0;

#if (0)
static spinlock_t 		lock;
#define	ADC_LOCK_INIT()	{ spin_lock_init(&lock); }
#define	ADC_LOCK()		{ spin_lock(&lock); }
#define	ADC_UNLOCK()	{ spin_unlock(&lock); }
#else
static DEFINE_MUTEX(mutex);
#define	ADC_LOCK_INIT()	{ ; }
#define	ADC_LOCK(id)	{ if (! preempt_count() && ! in_interrupt()) mutex_lock(&mutex); }
#define	ADC_UNLOCK(id)	{ if (mutex_is_locked(&mutex)) mutex_unlock(&mutex); }
#endif

/*
 * sysfs Interface
 */

#define	ADC_TIMEOUT_IN_US	1000	/* max time to wait for ADC reading */

static ssize_t show_channel0(struct device* dev, struct device_attribute* attr,
		char *buf)
{
	return sprintf(buf, "%d\n", soc_adc_read(0, ADC_TIMEOUT_IN_US));
}
static DEVICE_ATTR(channel0, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, show_channel0, NULL);

static ssize_t show_channel1(struct device* dev, struct device_attribute* attr,
		char *buf)
{
	return sprintf(buf, "%d\n", soc_adc_read(1, ADC_TIMEOUT_IN_US));
}
static DEVICE_ATTR(channel1, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, show_channel1, NULL);

static ssize_t show_channel2(struct device* dev, struct device_attribute* attr,
		char *buf)
{
	return sprintf(buf, "%d\n", soc_adc_read(2, ADC_TIMEOUT_IN_US));
}
static DEVICE_ATTR(channel2, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, show_channel2, NULL);

static ssize_t show_channel3(struct device* dev, struct device_attribute* attr,
		char *buf)
{
	return sprintf(buf, "%d\n", soc_adc_read(3, ADC_TIMEOUT_IN_US));
}
static DEVICE_ATTR(channel3, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, show_channel3, NULL);

static ssize_t show_channel4(struct device* dev, struct device_attribute* attr,
		char *buf)
{
	return sprintf(buf, "%d\n", soc_adc_read(4, ADC_TIMEOUT_IN_US));
}
static DEVICE_ATTR(channel4, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, show_channel4, NULL);

static ssize_t show_channel5(struct device* dev, struct device_attribute* attr,
		char *buf)
{
	return sprintf(buf, "%d\n", soc_adc_read(5, ADC_TIMEOUT_IN_US));
}
static DEVICE_ATTR(channel5, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, show_channel5, NULL);

static ssize_t show_channel6(struct device* dev, struct device_attribute* attr,
		char *buf)
{
	return sprintf(buf, "%d\n", soc_adc_read(6, ADC_TIMEOUT_IN_US));
}
static DEVICE_ATTR(channel6, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, show_channel6, NULL);

static ssize_t show_channel7(struct device* dev, struct device_attribute* attr,
		char *buf)
{
	return sprintf(buf, "%d\n", soc_adc_read(7, ADC_TIMEOUT_IN_US));
}
static DEVICE_ATTR(channel7, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, show_channel7, NULL);

static struct attribute *adc_attributes[] = {
	&dev_attr_channel0.attr,
	&dev_attr_channel1.attr,
	&dev_attr_channel2.attr,
	&dev_attr_channel3.attr,
	&dev_attr_channel4.attr,
	&dev_attr_channel5.attr,
	&dev_attr_channel6.attr,
	&dev_attr_channel7.attr,
	NULL
};

static struct attribute_group adc_attr_group = {
	.attrs = adc_attributes,
};

static struct bus_type adc_subsys = {
	.name = "adc",
	.dev_name = "adc",
};

/*-----------------------------------------------------------------------------*/
void soc_adc_init(void)
{
	U32 pclk_freq = cpu_get_clock_hz(CFG_SYS_CLKSRC_PCLK);
	DBGOUT("%s\n", __func__);

	NX_ADC_SetBaseAddress((U32)IO_ADDRESS(NX_ADC_GetPhysicalAddress()));
 	NX_ADC_OpenModule();

	NX_ADC_SetInterruptEnableAll(CFALSE);
	NX_ADC_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);

printk(KERN_INFO "soc_adc_init: pclk_freq %u\n", pclk_freq);
	NX_ADC_SetPrescalerValue((pclk_freq + (ADC_SPS>>1))/ADC_SPS);
	NX_ADC_SetPrescalerEnable(CFALSE);
	NX_ADC_SetStandbyMode(CTRUE);	/* FALSE=Power On, TRUE=Power Off */

	ADC_LOCK_INIT();
}

/*------------------------------------------------------------------------------
 * 	Description	: enable clock and prescaler of ADC
 *	Return 		: None
 */
void soc_adc_attach(void)
{
	U32 pclk_freq = cpu_get_clock_hz(CFG_SYS_CLKSRC_PCLK);
	ADC_LOCK();

	DBGOUT("%s(%d)\n", __func__, attach_count);

	if (0 == attach_count) {
		NX_ADC_SetClockPClkMode(NX_PCLKMODE_ALWAYS);
		NX_ADC_SetPrescalerValue((pclk_freq + (ADC_SPS>>1))/ADC_SPS);
		NX_ADC_SetPrescalerEnable(CTRUE);
		NX_ADC_SetStandbyMode(CFALSE);	/* FALSE=Power On, TRUE=Power Off */
	}

	attach_count++;

	ADC_UNLOCK();
}
EXPORT_SYMBOL(soc_adc_attach);

/*------------------------------------------------------------------------------
 * 	Description	: disable clock and prescaler of ADC
 *	Return 		: None
 */
void soc_adc_detach(void)
{
	ADC_LOCK();

	attach_count--;

	if (0 == attach_count) {
		NX_ADC_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);
		NX_ADC_SetPrescalerEnable(CFALSE);
		NX_ADC_SetStandbyMode(CTRUE);	/* FALSE=Power On, TRUE=Power Off */
	}

	/* clear attach count */
	if (0 > attach_count)
		attach_count = 0;

	DBGOUT("%s(%d)\n", __func__, attach_count);

	ADC_UNLOCK();
}
EXPORT_SYMBOL(soc_adc_detach);

/*------------------------------------------------------------------------------
 * 	Description	: get conversioned data of ADC
 *	In[ch]		: value of adc channel ( 0 ~ 7 )
 *	In[timeout]	: wait timer out (us)
 *	Return 		: -1 failure or 10bit data of ADC
 */
unsigned short soc_adc_read(int ch, uint timeout)
{
	ushort value = -1;
	uint   tout  = timeout;

	ADC_LOCK();

	DBGOUT("%s (ch=%d)\n", __func__, ch);
#if 1	// 8sep11
	if (0 == attach_count || ch > 7) 
#else
	if (0 == attach_count || ch > 6) 
#endif	// 8sep11
	{
		printk(KERN_ERR "fail, not attached or not support ADC:%d ...\n", ch);
		return -1;
	}

	while (NX_ADC_IsBusy() && tout--)
		udelay(1);

	if (0 >= tout)
		goto err;

	NX_ADC_SetInputChannel(ch);
	NX_ADC_Start();

	/* wait ADC convert done */
	tout  = timeout;
	while (NX_ADC_IsBusy() && tout--)
		udelay(1);

	if (0 >= tout)
		goto err;

	value = (unsigned short)NX_ADC_GetConvertedData();

err:
	ADC_UNLOCK();

	return value;
}
EXPORT_SYMBOL(soc_adc_read);

/*------------------------------------------------------------------------------
 * 	Description	: set suspend mode,
 *	Return 		: none.
 */
void soc_adc_suspend(void)
{
	PM_DBGOUT("%s\n", __func__);
}
EXPORT_SYMBOL_GPL(soc_adc_suspend);

/*------------------------------------------------------------------------------
 * 	Description	: resume mode
 *	Return 		: none.
 */
void soc_adc_resume(void)
{
	U32 pclk_freq = cpu_get_clock_hz(CFG_SYS_CLKSRC_PCLK);
	PM_DBGOUT("%s\n", __func__);

	if (! attach_count)
		return;

	if (NX_PCLKMODE_ALWAYS == NX_ADC_GetClockPClkMode())
		return;

	NX_ADC_SetClockPClkMode(NX_PCLKMODE_ALWAYS);
	NX_ADC_SetPrescalerValue((pclk_freq + (ADC_SPS>>1))/ADC_SPS);
	NX_ADC_SetPrescalerEnable(CTRUE);
	NX_ADC_SetStandbyMode(CFALSE);	/* FALSE=Power On, TRUE=Power Off */
}
EXPORT_SYMBOL_GPL(soc_adc_resume);

static int __init lf2000_adc_probe(struct platform_device *pdev)
{
	struct resource *res;
	int ret;

	struct resource *mem_adc = NULL;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		printk(KERN_ERR "ADC: failed to get resource\n");
		return -ENXIO;
	}

	mem_adc = request_mem_region(res->start, (res->end - res->start) + 1,
			ADC_DEV_NAME);

	if (mem_adc == NULL) {
		printk(KERN_ERR "ADC: failed to map ADC region\n");
		return -EBUSY;
	}

	ret = subsys_system_register(&adc_subsys, NULL);

	ret = sysfs_create_group(&adc_subsys.dev_root->kobj, &adc_attr_group);
	if (ret) {
		sysfs_remove_group(&adc_subsys.dev_root->kobj, &adc_attr_group);
	}
	return ret;
}

static int __exit lf2000_adc_remove(struct platform_device *pdev)
{
	struct resource *res;

	sysfs_remove_group(&adc_subsys.dev_root->kobj, &adc_attr_group);

	/* free memory */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(res->start, (res->end - res->start) + 1);

	platform_set_drvdata(pdev, NULL);
	return 0;
};


static struct platform_driver lf2000_adc_driver = {
	.remove = __exit_p(lf2000_adc_remove),
	.driver		= {
		.name	= ADC_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init lf2000_adc_init(void)
{
	return platform_driver_probe(&lf2000_adc_driver, lf2000_adc_probe);
}

static void __exit lf2000_adc_exit(void)
{
	platform_driver_unregister(&lf2000_adc_driver);
}

module_init(lf2000_adc_init);
module_exit(lf2000_adc_exit)

MODULE_AUTHOR("leapfrog.com");
MODULE_DESCRIPTION("LF2000 ADC driver");
MODULE_LICENSE("GPL");
