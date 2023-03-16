/*
 * LF1000 / LF2000 IDCT Macro Block Decoder driver
 *
 * main.c -- Main driver functionality.
 *
 * Dave Milici <dmilici@leapfrog.com>
 */

#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <mach/devices.h>

/* Register offsets */
#define IDCT_BUF_DATA           (0x00)
#define IDCT_CONTROL            (0x80)
#define IDCT_INT_ENB            (0x84)
#define IDCT_INT_PEND           (0x88)
#define IDCT_CLK_ENB            (0x7C0)

struct idct_device {
	void __iomem		*mem;
	struct cdev		cdev;
	dev_t			dev;

	struct resource		*res;
	unsigned int		irq;

#if CONFIG_LF2000_IDCT_DEBUGFS
	struct dentry		*debug;
#endif
};

#if CONFIG_LF2000_IDCT_DEBUGFS
static void idct_reg(struct seq_file *s, const char *nm, u32 reg)
{
	struct idct_device *dev = s->private;

	seq_printf(s, "%10s:\t0x%08X\n", nm, readl(dev->mem + reg));
}

static int idct_show_registers(struct seq_file *s, void *v)
{
	idct_reg(s, "BUF_DATA", IDCT_BUF_DATA);
	idct_reg(s, "BUF_DATA", IDCT_BUF_DATA + 0x04);
	idct_reg(s, "BUF_DATA", IDCT_BUF_DATA + 0x08);
	idct_reg(s, "BUF_DATA", IDCT_BUF_DATA + 0x0C);
	idct_reg(s, "BUF_DATA", IDCT_BUF_DATA + 0x10);
	idct_reg(s, "BUF_DATA", IDCT_BUF_DATA + 0x14);
	idct_reg(s, "BUF_DATA", IDCT_BUF_DATA + 0x18);
	idct_reg(s, "BUF_DATA", IDCT_BUF_DATA + 0x1C);
	idct_reg(s, "CONTROL",	IDCT_CONTROL);
	idct_reg(s, "INT_ENB",	IDCT_INT_ENB);
	idct_reg(s, "INT_PEND", IDCT_INT_PEND);
	idct_reg(s, "CLK_ENB",	IDCT_CLK_ENB);

	return 0;
}

static int lf2000_idct_regs_open(struct inode *inode, struct file *file)
{
	return single_open(file, idct_show_registers, inode->i_private);
}

static const struct file_operations lf2000_idct_regs_fops = {
	.owner          = THIS_MODULE,
	.open           = lf2000_idct_regs_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};
#endif

/*******************************
 * character device operations *
 *******************************/

static int idct_open(struct inode *inode, struct file *filp)
{
	struct idct_device *idct = container_of(inode->i_cdev, struct idct_device, cdev);

	filp->private_data = idct; 
	return 0;
}

static void idct_vma_open(struct vm_area_struct *vma)
{
}

static void idct_vma_close(struct vm_area_struct *vma)
{
}

static struct vm_operations_struct idct_vm_ops = {
	.open  = idct_vma_open,
	.close = idct_vma_close,
};

static int idct_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret;
	struct idct_device *idct = filp->private_data;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_ops = &idct_vm_ops;
	vma->vm_flags |= VM_IO;

	ret = io_remap_pfn_range(vma,
				 vma->vm_start, 
				 idct->res->start>>PAGE_SHIFT,
				 vma->vm_end - vma->vm_start, 
				 vma->vm_page_prot);
	if(ret < 0)
		return -EAGAIN;

	idct_vma_open(vma);
	return 0;
}

static struct file_operations idct_fops = {
	.owner = THIS_MODULE,
	.open  = idct_open,
	.mmap = idct_mmap,
};

static irqreturn_t idct_isr(int irq, void *data)
{
	struct idct_device *idct = data;

	printk(KERN_EMERG "%s\n", __FUNCTION__);

	writel(0x1, idct->mem + IDCT_INT_PEND);

	return IRQ_HANDLED;
}

static void idct_clock_enable(struct idct_device *idct, const bool enable)
{
	u32 reg = readl(idct->mem + IDCT_CLK_ENB) & ~(0x3);

	if(enable)
		reg |= 0x2;

	writel(reg, idct->mem + IDCT_CLK_ENB);
}

/*********************
 *  module functions *
 *********************/

static int lf2000_idct_probe(struct platform_device *pdev)
{
	int ret;
	unsigned int irq;
	struct resource *res;
	struct idct_device *idct;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	irq = platform_get_irq(pdev, 0);
	if(!res || (int)irq < 0) {
		dev_err(&pdev->dev, "failed to get resource\n");
		return -ENXIO;
	}

	idct = kzalloc(sizeof(*idct), GFP_KERNEL);
	if(!idct)
	{
		dev_err(&pdev->dev, "failed to allocate\n");
		return -ENOMEM;
	}
	platform_set_drvdata(pdev, idct);
	idct->irq	= irq;
	idct->res	= res;

	ret = platform_device_add_data(pdev, idct, sizeof(idct));
	if(ret < 0)
	{
		dev_err(&pdev->dev, "failed to set data.");
		goto fail_alloc;
	}

	if(!request_mem_region(res->start, resource_size(res), res->name)) {
		dev_err(&pdev->dev, "failed to map region.");
		ret = -EBUSY;
		goto fail_alloc;
	}

	idct->mem = ioremap_nocache(res->start, resource_size(res));
	if(idct->mem == NULL) {
		dev_err(&pdev->dev, "failed to ioremap\n");
		ret = -ENOMEM;
		goto fail_remap;
	}
#if 0
	ret = request_irq(idct->irq, idct_isr, 0, "idct", idct);
	if(ret) {
		dev_err(&pdev->dev, "failed to register interrupt\n");
		goto fail_irq;
	}
#endif
	ret = alloc_chrdev_region(&idct->dev, 0, 1, "idct");
	if(ret < 0) {
		dev_err(&pdev->dev, "failed to get a device\n");
		goto fail_dev;
	}

	cdev_init(&idct->cdev, &idct_fops);
	idct->cdev.owner = THIS_MODULE;
	idct->cdev.ops = &idct_fops;
	ret = cdev_add(&idct->cdev, idct->dev, 1);
	if(ret < 0) {
		dev_err(&pdev->dev, "failed to create character device\n");
		goto fail_add;
	}

#if CONFIG_LF2000_IDCT_DEBUGFS
	idct->debug = debugfs_create_dir("lf2000-idct", NULL);
	if(idct->debug)
		debugfs_create_file("registers", S_IRUSR, idct->debug, idct,
				&lf2000_idct_regs_fops);
#endif

	idct_clock_enable(idct, true);

	return 0;

fail_add:
	unregister_chrdev_region(idct->dev, 1);
fail_dev:
#if 0
	free_irq(idct->irq, idct);
#endif
fail_irq:
	iounmap(idct->mem);
fail_remap:
	release_mem_region(res->start, resource_size(res));
fail_alloc:
	kfree(idct);

	return ret;
}

static int lf2000_idct_remove(struct platform_device *pdev)
{
	struct idct_device *idct = platform_get_drvdata(pdev);
	platform_set_drvdata(pdev, NULL);

	idct_clock_enable(idct, false);

#ifdef CONFIG_LF2000_IDCT_DEBUG
	debugfs_remove_recursive(idct->debug);
#endif

	cdev_del(&idct->cdev);
	unregister_chrdev_region(idct->dev, 1);
#if 0
	free_irq(idct->irq, idct);
#endif

	iounmap(idct->mem);

	release_mem_region(idct->res->start, resource_size(idct->res));

	kfree(idct);

	return 0;
}

static struct platform_driver lf2000_idct_driver = {
	.probe		= lf2000_idct_probe,
	.remove		= lf2000_idct_remove,
	.driver		= {
		.name	= IDCT_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init idct_init(void)
{
	return platform_driver_register(&lf2000_idct_driver);
}

static void __exit idct_cleanup(void)
{
	platform_driver_unregister(&lf2000_idct_driver);
}

module_init(idct_init);
module_exit(idct_cleanup);
MODULE_LICENSE("GPL");
