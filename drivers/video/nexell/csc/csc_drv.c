/*
 * CSC (Color Space Converter) driver for LF2000
 * Dave Milici <dmilici@leapfrog.com>
 * Portions Copyright Nexell
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
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>
#include <mach/dpc.h>

#define CSC_DEV_MAJOR		(DPC_DEV_MAJOR+1)
#define CSC_DEV_NAME		"lf2000-csc"

#if (1)
#define DBGOUT(msg...)		{ printk(KERN_INFO "csc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

static int nx_csc_ops_open(struct inode *inode, struct file *flip)
{
	DBGOUT("%s\n", __func__);

	// CSC module init
	NX_CSC_Initialize();
	NX_CSC_SetBaseAddress((U32)IO_ADDRESS(NX_CSC_GetPhysicalAddress()));
	NX_CSC_OpenModule();

	NX_CSC_SetClockPClkMode(NX_PCLKMODE_ALWAYS);
	//NX_CSC_SetClockBClkMode(NX_BCLKMODE_ALWAYS);
	//NX_CSC_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);
	NX_CSC_SetClockBClkMode(NX_BCLKMODE_DYNAMIC);

	return 0;
}

static int nx_csc_ops_release(struct inode *inode, struct file *flip)
{
	DBGOUT("%s\n", __func__);

	NX_CSC_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);
	NX_CSC_SetClockBClkMode(NX_BCLKMODE_DISABLE);

	return 0;
}

static long nx_csc_ops_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
{
	DBGOUT("%s: (cmd:0x%x, type:%c, nr:%d) \n", __func__, cmd, _IOC_TYPE(cmd), _IOC_NR(cmd));

	switch (cmd) {
	case IOCTL_DPC_CSC_SET_VIDEO_TEXTURE:
		{
			uint buf[6]; // width, height, Y addr, U addr, V addr, texture addr
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return -EFAULT;
			NX_CSC_SetSize(buf[0], buf[1]);
			NX_CSC_SetSourceAddr(buf[2], buf[3], buf[4]);
			NX_CSC_SetTargetAddr(buf[5]);
			NX_CSC_Run();
		}
		break;
	case IOCTL_DPC_CSC_GET_VIDEO_TEXTURE:
		if (NX_CSC_IsBusy())
			return 1;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

struct file_operations nx_csc_ops = {
	.owner 			= THIS_MODULE,
	.open 			= nx_csc_ops_open,
	.release		= nx_csc_ops_release,
	.unlocked_ioctl	= nx_csc_ops_ioctl,
};

static int __init nx_csc_drv_probe(struct platform_device *pdev)
{
	int ret = register_chrdev(CSC_DEV_MAJOR, "CSC (Color Space Converter)", &nx_csc_ops);
	DBGOUT("%s: register %s, major=%d, ret=%d\n", __func__, pdev->name, CSC_DEV_MAJOR, ret);
	return ret;
}

static int nx_csc_drv_remove(struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);
	unregister_chrdev(CSC_DEV_MAJOR, "CSC (Color Space Converter)");
	return 0;
}

static int nx_csc_drv_suspend(struct platform_device *dev, pm_message_t state)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_csc_drv_resume(struct platform_device *dev)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static struct platform_driver dpc_plat_driver = {
	.probe		= nx_csc_drv_probe,
	.remove		= nx_csc_drv_remove,
	.suspend	= nx_csc_drv_suspend,
	.resume		= nx_csc_drv_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= CSC_DEV_NAME,
	},
};

static int __init nx_csc_drv_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_register(&dpc_plat_driver);
}

static void __exit nx_csc_drv_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&dpc_plat_driver);
}

module_init(nx_csc_drv_init);
module_exit(nx_csc_drv_exit);

MODULE_AUTHOR("Dave Milici <dmilici@leapfrog.cpm>");
MODULE_DESCRIPTION("CSC (Color Space Converter) driver for LF2000");
MODULE_LICENSE("GPL");

