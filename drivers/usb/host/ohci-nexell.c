/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <linux/device.h>
#include <linux/signal.h>
#include <linux/platform_device.h>
#include <linux/pm.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>

/* Debugging stuff */
#if (0)
#define DBGOUT(msg...)	{ printk(KERN_INFO "ohci: " msg); }
#else
#define DBGOUT(msg...)
#endif

static bool usb_hc_init = false;

static void start_usb_hc(struct device *dev)
{
	u_int UHC_BASE = IO_ADDRESS(PHY_BASEADDR_OHCI);
	u_int OTG_BASE = IO_ADDRESS(0xC0024000);

	if(true == usb_hc_init)
		return;

	DBGOUT("%s\n", __func__);

	// OTG phy clock
	*(volatile unsigned int *)(OTG_BASE + 0x1FC0) = 0x0000000B;	// <-- CLK MODE
	*(volatile unsigned int *)(OTG_BASE + 0x1FC4) = 0x0000000C; // <-- CLKGEN DIV
	*(volatile unsigned int *)(OTG_BASE + 0x1FC0) = 0x0000000F; // <-- CLKGEN ENB
	*(volatile unsigned int *)(OTG_BASE + 0x1868) = 0x00000002; // <-- CLK MODE
	*(volatile unsigned int *)(OTG_BASE + 0x1840) = 0x00000230; // <-- CLKGEN DIV

	// OHCI clock
	*(volatile unsigned int *)(UHC_BASE + 0x3C4) =
											0<<15 |	// Specifies the direction of the PADVCLK pad. You have to set this bit when CLKSRCSEL0/1 is 3 or 4. 0 : Enable(Output)                    1 : Reserved
											3<<5 |	// Specifies divider value of the source clock.
													//	Divider value = CLKDIV0 + 1
													//	Clock Divisor. For divide - by - N, enter an [N - 1] value.
													//	000000 ~ 111111(N-1) : Divide Value (N) = Divide by 1 to 64
													//	Ex) For divide-by-2 : [0001b]
											1<<2 |	// Clock Source Selection
													//	000 : PLL0				001 : PLL1
													//	010 : Reserved			011 : Ext Clock
													//	100 ~ 111 : Reserved
											0<<0;	// Specifies whether to invert the clock output.
													// 0 : Normal (Falling Edge)	1 : Invert (Rising Edge)

	*(volatile unsigned int *)(UHC_BASE + 0x3C0) =
											1<<3 |	// PCLK Operating Mode 0 : Clock is enabled only when CPU accesses 1 : Always
											1<<2 |	// Clock Generation Enable 0 : Disable	1 : Enable
											3<<0;	// USBD Clock Enable
													// 00 : Disable		01 : Reserved		10 : Dynamic	11 : Always

	*(volatile unsigned int *)(UHC_BASE + 0x080) = 0x18;

	msleep(1);

	printk(KERN_INFO "%s: usb host OHCI version 0x%08x\n",
		OHCI_DEV_NAME, *(volatile unsigned int *)UHC_BASE);

	usb_hc_init = true;
}

static void stop_usb_hc(struct device *dev)
{
	DBGOUT("%s\n", __func__);
	usb_hc_init = false;
}


/**
 * usb_hcd_nx_probe - initialize nexell-based HCDs
 * Context: !in_interrupt()
 *
 * Allocates basic resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 *
 */
static int usb_hcd_nx_probe (const struct hc_driver *driver, struct platform_device *pdev)
{
	int retval;
	struct usb_hcd *hcd;

	DBGOUT("%s\n", __func__);

	/* allocate memory for usb_hcd struct. defined usb/ocre/hcd.c */
	hcd = usb_create_hcd (driver, &pdev->dev, EHCI_DEV_NAME);
	if (!hcd)
		return -ENOMEM;

	hcd->rsrc_start = pdev->resource[0].start;
	hcd->rsrc_len   = pdev->resource[0].end - pdev->resource[0].start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, pdev->name)) {
		pr_debug("request_mem_region failed");
		retval = -EBUSY;
		goto err_put;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		pr_debug("ioremap failed");
		retval = -ENOMEM;
		goto err_ioremap;
	}

	start_usb_hc(&pdev->dev);

	ohci_hcd_init(hcd_to_ohci(hcd));

	retval = usb_add_hcd(hcd, pdev->resource[1].start, IRQF_DISABLED);

	if (retval != 0)
		goto err_ioremap;

	return 0;

err_ioremap:
	stop_usb_hc(&pdev->dev);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);

err_put:
	usb_put_hcd(hcd);

	return retval;
}


/**
 * usb_hcd_nx_remove - shutdown processing for nexell-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_hcd_nx_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 *
 */
static void usb_hcd_nx_remove (struct usb_hcd *hcd, struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);

	usb_remove_hcd(hcd);

	stop_usb_hc(&pdev->dev);

	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

static int __devinit ohci_hc_start (struct usb_hcd *hcd)
{
	struct ohci_hcd	*ohci = hcd_to_ohci (hcd);
	int		ret;

	ohci_dbg (ohci, "ohci_hc_start, ohci:%p", ohci);

	/* The value of NDP in roothub_a is incorrect on this hardware */
	ohci->num_ports = 3;

	if ((ret = ohci_init(ohci)) < 0)
		return ret;

	if ((ret = ohci_run (ohci)) < 0) {
		err ("can't start %s", hcd->self.bus_name);
		ohci_stop (hcd);
		return ret;
	}

	return 0;
}

static const struct hc_driver ohci_hc_driver = {
	.description =		hcd_name,
	.product_desc =		"Nexell OHCI ",
	.hcd_priv_size =	sizeof(struct ohci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq =			ohci_irq,
	.flags =		HCD_USB11 | HCD_MEMORY,

	/*
	 * basic lifecycle operations
	 */
	.start =		ohci_hc_start,
	.stop =			ohci_stop,
	.shutdown =		ohci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue =		ohci_urb_enqueue,
	.urb_dequeue =		ohci_urb_dequeue,
	.endpoint_disable =	ohci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number =	ohci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data =	ohci_hub_status_data,
	.hub_control =		ohci_hub_control,
#ifdef  CONFIG_PM
	.bus_suspend =		ohci_bus_suspend,
	.bus_resume =		ohci_bus_resume,
#endif
	.start_port_reset =	ohci_start_port_reset,
};


static int ohci_nx_drv_probe(struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);

	return usb_hcd_nx_probe(&ohci_hc_driver, pdev);
}

static int ohci_nx_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	DBGOUT("%s\n", __func__);

	usb_hcd_nx_remove(hcd, pdev);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

#ifdef	CONFIG_PM
static int ohci_nx_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);

	PM_DBGOUT("+%s\n", __func__);

	if (time_before(jiffies, ohci->next_statechange))
		msleep(5);
	ohci->next_statechange = jiffies;

	stop_usb_hc(&pdev->dev);

	hcd->state = HC_STATE_SUSPENDED;

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

static int ohci_nx_drv_resume(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);

	PM_DBGOUT("+%s\n", __func__);

	if (time_before(jiffies, ohci->next_statechange))
		msleep(5);

	ohci->next_statechange = jiffies;

	start_usb_hc(&pdev->dev);

	ohci_finish_controller_resume(hcd);

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}
#endif


static struct platform_driver ohci_hcd_nexell_driver = {
	.probe		= ohci_nx_drv_probe,
	.remove		= ohci_nx_drv_remove,
	.shutdown	= usb_hcd_platform_shutdown,
#ifdef CONFIG_PM
	.suspend	= ohci_nx_drv_suspend,
	.resume		= ohci_nx_drv_resume,
#endif
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= OHCI_DEV_NAME,
	},
};

MODULE_ALIAS("platform:nexell-ohci");


