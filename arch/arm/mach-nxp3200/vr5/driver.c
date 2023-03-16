#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/slab.h>	  // kmalloc, kfree
#include <linux/proc_fs.h>

#include <linux/types.h>
#include <linux/cdev.h>   // struct cdev
#include <linux/device.h> // struct class

#include <linux/interrupt.h>

#include <asm/io.h>

/* mach-??? headers */
#include <mach/devices.h>

#include "driver.h"

//+-----------------------------------------------------------------------
#define GPL_DRIVER		1

int debug_level = 2;
module_param(debug_level, int, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP);
MODULE_PARM_DESC(debug_level, "debug level number");

int major = 0;
module_param(major, int, S_IRUGO);
MODULE_PARM_DESC(major, "device major number");

//static 
typedef struct 
{
	struct cdev		m_cdev   ;
	struct class *	m_class  ;
	volatile u32 *	m_iobase ;
	int	            m_irqnum ;
} device_object_t;

static device_object_t device_object;

//+-----------------------------------------------------------------------
//+		file operations
//+-----------------------------------------------------------------------
static int ogl_open(struct inode * inode, struct file * filp)
{
	if(filp->f_flags & O_SYNC){
		return 0;
	}
	{
		CONTEXT_ENTRY * context = __drv_create_context();
		if(!context){
			printk("ogl_open failed!\n");
			return -ENOMEM;
		}
		//printk("ogl_open : driver opened.\n");
		filp->private_data = (void *)context;
	}
	return 0;
}

static int ogl_release(struct inode * inode, struct file * filp)
{
	if(filp->f_flags & O_SYNC){
		__drv_close_proc();
		return 0;
	}
	{
		CONTEXT_ENTRY * context = (CONTEXT_ENTRY *)filp->private_data;
		if(!context)
		{
			printk("ogl_release failed!\n");
		}else{
			__drv_destroy_context(context);
			//printk("ogl_release : driver closed.\n");
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------
// ioctl command vectors
typedef	int (*IOCTL_FUNC)(CONTEXT_ENTRY *, unsigned int);

static const IOCTL_FUNC	g_ioctl[]={
	(IOCTL_FUNC)__drv_ioctl_set_version,
	(IOCTL_FUNC)__drv_ioctl_push  ,
	(IOCTL_FUNC)__drv_ioctl_wait  ,
	(IOCTL_FUNC)__drv_ioctl_markswap,
	//(IOCTL_FUNC)__drv_ioctl_free  ,
	//(IOCTL_FUNC)__drv_ioctl_finish,
};

#ifdef KERNEL_VERSION_BELOW_2_6_35
static int ogl_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg)
#else
static long ogl_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
#endif
{
	int cmd_idx = cmd&0xFF;
	CONTEXT_ENTRY * context = (CONTEXT_ENTRY *)filp->private_data;
	if( cmd_idx >= (sizeof(g_ioctl)/sizeof(g_ioctl[0])) )
	{
		printk("ioctl(%d) is not implemented\n", cmd );
		return -1;
	}
	// run io control
	return g_ioctl[cmd&0xFF](context, (unsigned int)arg);
}

static struct file_operations driver_fops = {
		.owner   = THIS_MODULE,
		.open    = ogl_open,
		.release = ogl_release,
#ifdef KERNEL_VERSION_BELOW_2_6_35
		.ioctl   = ogl_ioctl,
#else
		.unlocked_ioctl = ogl_ioctl,
#endif
	};

//+-----------------------------------------------------------------------
//+		platform driver interfaces
//+-----------------------------------------------------------------------
static irqreturn_t ogl_irq_handler(int port, void * dev_id){
	
	__drv_irq_handler();
	return IRQ_HANDLED;
}

static int	ogl_plat_probe(struct platform_device * pdev)
{
	int err=0;
	int irqnum;
	struct resource * res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(res)
	{
		size_t size = res->end - res->start;

		// res->start is physical i/o address.
		if(!request_mem_region(res->start, size, OGL_DEV_NAME)){
			printk("%s : Failed to request mem region.\n", __FUNCTION__);
			return -ENOMEM;
		}
		device_object.m_iobase = (u32 *)ioremap_nocache(res->start, size);
	}else{
		printk("ogl_plat_probe : Can't find driver resource.\n");
		return -ENOMEM;
	}

	irqnum = platform_get_irq(pdev, 0);
	if(irqnum < 0){
		printk("%s : Failed to get irq.\n", __FUNCTION__);
		return -ENODEV;
	}
		
	if((err = request_irq(irqnum, ogl_irq_handler, IRQF_DISABLED, OGL_DEV_NAME, &device_object)) < 0)
	{
		printk("%s : request_irq has been failed. irq(%d), %d\n", __FUNCTION__, irqnum, err);
		return err;
	}
		
	if(!__drv_init(device_object.m_iobase)){
		printk("%s : Failed to __drv_init.\n", __FUNCTION__);
		return -ENODEV;
	}
	device_object.m_irqnum = irqnum;
	return 0;
}

static int	ogl_plat_remove(struct platform_device * pdev)
{
	struct resource * res;

	if( ! __drv_exit(device_object.m_iobase) )
	{
		return -ENODEV;
	}

	free_irq(device_object.m_irqnum, &device_object);

	res = platform_get_resource(pdev, IORESOURCE_IO, 0);
	if(res)
	{
		size_t size = res->end - res->start;
		release_mem_region(res->start, size);
		iounmap(device_object.m_iobase);
	}
	memset(&device_object, 0, sizeof(device_object));
	return 0;
}

static int	ogl_plat_suspend(struct platform_device * pdev, pm_message_t state)
{
	if( ! __drv_suspend() ){ return -ENODEV; }
	return 0;
}

static int	ogl_plat_resume(struct platform_device * pdev)
{
	if( ! __drv_resume() ){ return -ENODEV; }	
	return 0;
}

static struct platform_driver ogl_platform_driver = {
	.probe		= ogl_plat_probe,
	.remove		= ogl_plat_remove,
	.suspend	= ogl_plat_suspend,
	.resume		= ogl_plat_resume,
	.driver		= {
		.owner = THIS_MODULE,
		.name  = OGL_DEV_NAME,
	}
};

//+-----------------------------------------------------------------------
static int driver_class_init(device_object_t * device)
{
	int err;
	dev_t dev = 0;

	if(major == 0)
	{
		err = alloc_chrdev_region(&dev, 0, 1, OGL_DEV_NAME);
		major = MAJOR(dev);
	}
	else
	{
		dev = MKDEV(major, 0);
		err = register_chrdev_region(dev, 1, OGL_DEV_NAME);
	}

	if(err == 0)
	{
		// register char device
		cdev_init(&device->m_cdev, &driver_fops);
		device->m_cdev.owner = THIS_MODULE;
		device->m_cdev.ops = &driver_fops;
		
		if((err = cdev_add(&device->m_cdev, dev, 1)) == 0)
		{
#if GPL_DRIVER
			device->m_class = class_create(THIS_MODULE, OGL_DEV_NAME);
			if(IS_ERR(device->m_class))
			{
				err = PTR_ERR(device->m_class);
			}
			else
			{
				struct device * mdev = device_create(device->m_class, NULL, dev, NULL, OGL_DEV_NAME);
				if(!IS_ERR(mdev))
				{
					//printk("driver_ogl_init : major(%d) with device_class\n", major);
					__drv_loginfo();
					return 0;
				}
				err = PTR_ERR(mdev);
			}
			cdev_del(&device->m_cdev);
#endif
		}
		else
		{
			unregister_chrdev_region(dev,1);
			return err; 
		}
	}
	//printk("driver_ogl_init : major(%d)\n", major);
	__drv_loginfo();
	return err;
}

static void  driver_class_exit(device_object_t * device)
{
	dev_t dev = MKDEV(major, 0);
	
#if GPL_DRIVER
	device_destroy(device->m_class, dev);
	class_destroy(device->m_class);
#endif

	// unregister char device
	cdev_del(&device->m_cdev);
	unregister_chrdev_region(dev, 1);
	//printk("driver_ogl_exit : major(%d)\n", major);
	__drv_loginfo();
}

#define CONFIG_PROC_FS

//------------------------------------------------------------------------------
//     proc file-system
//------------------------------------------------------------------------------
#ifdef CONFIG_PROC_FS
#define DEVICE_PROC_PATH   OGL_DEV_NAME

static int read_ogl_proc(char * buf, char ** start, off_t offset, int count, int * eof, void * data)
{
	int len = 0;
	__drv_loginfo();	
	*eof = 1;
	return len;
}
#endif


static int __init driver_ogl_init(void)
{
	int err = platform_driver_register(&ogl_platform_driver);
	if(!err) 
	{
		if(driver_class_init(&device_object) != 0)
			goto init_error;
	}

#ifdef CONFIG_PROC_FS
	create_proc_read_entry(DEVICE_PROC_PATH, 0, NULL, read_ogl_proc, NULL);
#endif

	return 0;

init_error:
	if(device_object.m_class)
		driver_class_exit(&device_object);
	return -1;
}

static void __exit driver_ogl_exit(void)
{
	driver_class_exit(&device_object);

#ifdef CONFIG_PROC_FS
	remove_proc_entry(DEVICE_PROC_PATH, NULL);
#endif


	platform_driver_unregister(&ogl_platform_driver);
}

module_init(driver_ogl_init);
module_exit(driver_ogl_exit);

MODULE_AUTHOR("billy");
MODULE_LICENSE("GPL");
