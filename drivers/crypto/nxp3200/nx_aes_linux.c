/*
 * (C) Copyright 2010
 * Ray Park, Nexell Co, <ray@nexell.co.kr>
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

#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <mach/platform.h>

#include <nx_type.h>
#include <nx_debug.h>
#include "nx_aes_ioctl.h"
#include "nx_aes.h"

DEFINE_MUTEX(aes_access_lock);

#define	AES_MAX_INSTANCE			1

static int	gstAESInUse = 0;
static int	gstAESHWRun = 0;

static int nx_aes_open(struct inode *inode, struct file *filp)
{
	if( AES_MAX_INSTANCE <= gstAESInUse )
	{
		return -EBUSY;
	}

	filp->private_data = CreateAES();
	if( CNULL == filp->private_data )
	{
		printk("[AES|DRV] %s : CreateAES() failed\n", __func__);
		return -1;
	}

	gstAESInUse++;
	return 0;
}

static int nx_aes_close(struct inode *inode, struct file *filp)
{
	gstAESInUse--;
	if(filp->private_data)
	{
		DestroyAES(filp->private_data);
		filp->private_data = CNULL;
	}
	return 0;
}
static long nx_aes_ioctl( struct file *filp, unsigned int cmd, unsigned long arg )
//static int nx_aes_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
	int ret = 0;
	mutex_lock( &aes_access_lock );
	switch( cmd )
	{
		case IOCTL_AES_RUN:
		{
			AESRunParam runParam;
			gstAESHWRun = 1;
			NX_ASSERT(filp->private_data);
			if( 0 != copy_from_user( &runParam, (const void*)arg, sizeof(AESRunParam) ) )
			{
				printk("[AES|DRV] %s : copy_from_user() failed\n", __func__);
			}
			if( filp->private_data )
			{
				ret = AESRunCipher( filp->private_data, &runParam );
			}
			gstAESHWRun = 0;
			break;
		}
		default:
			ret = -1;
			break;
	}
	mutex_unlock( &aes_access_lock );
	return ret;
}

struct file_operations nx_aes_ops = {
	.owner		= THIS_MODULE,
	.open		= nx_aes_open,
	.release	= nx_aes_close,
	.unlocked_ioctl		= nx_aes_ioctl
};


/*****************************************************************************
 *
 *		Nexell SoC Video firmware platform_driver functions
 *
 ****************************************************************************/
static struct class *nx_aes_class = CNULL;
static int __init nx_aes_probe(struct platform_device *pdev)
{
	int	res;
	res = register_chrdev(NX_DEV_AES_MAJOR, NX_DEV_AES_NAME, &nx_aes_ops);
	if(res<0)
	{
		printk("[AES] regist fail\n");
		return res;
	}
	nx_aes_class = class_create(THIS_MODULE, NX_DEV_AES_NAME);
	device_create(nx_aes_class, NULL, MKDEV(NX_DEV_AES_MAJOR, 0), NULL, NX_DEV_AES_NAME);
	return 0;
}


/*
 *  Cleanup
 */
static int nx_aes_remove(struct platform_device *pdev)
{
//	printk("nx_aes_remove++\n");
	unregister_chrdev(NX_DEV_AES_MAJOR, NX_DEV_AES_NAME);
	class_destroy(nx_aes_class);
//	printk("nx_aes_remove--\n");
	return 0;
}

/* suspend and resume support for the aes */
static int nx_aes_suspend(struct platform_device *dev, pm_message_t state)
{
	while(gstAESHWRun);
	return 0;
}

static int nx_aes_resume(struct platform_device *dev)
{
	return 0;
}

static struct platform_driver nx_aes_driver = {
	.probe		= nx_aes_probe,
	.remove		= nx_aes_remove,
	.suspend	= nx_aes_suspend,
	.resume		= nx_aes_resume,
	.driver		= {
		.name	= NX_DEV_AES_NAME,
		.owner	= THIS_MODULE,
	},
};

/*****************************************************************************
*
*			Nexell AES Module Initialize & Release
*
*****************************************************************************/
static struct platform_device *nx_aes_device;

int __devinit nx_aes_init(void)
{
	int res = -1;

	mutex_init( &aes_access_lock );

	/* register platform device */
	nx_aes_device = platform_device_register_simple(NX_DEV_AES_NAME, -1, CNULL, 0);
	if(IS_ERR(nx_aes_device)){
		printk("[AES] Failed to add platform device %s \n", NX_DEV_AES_NAME );
		return res;
	}

	/* register platform driver, exec platform_driver probe */
	res =  platform_driver_register(&nx_aes_driver);
	if(res){
		platform_device_unregister(nx_aes_device);
		printk("[AES] Failed to add platrom driver %s (%d) \n", nx_aes_driver.driver.name, res );
	}
	gstAESInUse = 0;

	printk("Nexell AES Driver Loaded\n" );

	return 0;
}

static void __exit nx_aes_exit(void)
{
	if(gstAESInUse)
	{
		printk("AES driver is now busy!!\n");
	}
	device_destroy(nx_aes_class, MKDEV(NX_DEV_AES_MAJOR, 0));

	platform_driver_unregister(&nx_aes_driver);
	platform_device_unregister(nx_aes_device);
	printk("Nexell AES Driver Unloaded\n" );
	mutex_destroy( &aes_access_lock );
}

module_init(nx_aes_init);
module_exit(nx_aes_exit);

MODULE_AUTHOR("ray park <ray@nexell.co.kr>");
MODULE_DESCRIPTION("AES driver for the Nexell");
MODULE_LICENSE("GPL");
