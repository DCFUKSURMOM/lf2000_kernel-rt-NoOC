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
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <media/soc_camera.h>
#include <media/hynix_yac.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/lf2000/gpio.h>
#include <linux/mmc/host.h>
#include <linux/mtd/nand.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <asm/system_info.h>
#include <asm/system_misc.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/lfp100.h>
#include <mach/lf2000_lcd.h>
#include <mach/lf2000_vip.h>
#include <mach/platform_id.h>
#include <mach/soc.h>
#include <mach/spi.h>
#include <mach/board_revisions.h>

/* PCA9555 GPIO Extender */
#include <linux/i2c/pca953x.h>

extern void lf2000_set_scratch_reboot();

/*------------------------------------------------------------------------------
 * Generic device routines for OTG
 */
#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
#include <linux/dm9000.h>

static struct resource dm9000_resource[] = {
	[0] = {
		.start	= CFG_EXT_PHY_BASEADDR_ETHER,
		.end	= CFG_EXT_PHY_BASEADDR_ETHER + 1,		// 3 (8 BIT), 1 (16 BIT)
		.flags	= IORESOURCE_MEM
	},
	[1] = {
		.start	= CFG_EXT_PHY_BASEADDR_ETHER + 4,		// + 0x4 (8 BIT), 4 (16 BIT)
		.end	= CFG_EXT_PHY_BASEADDR_ETHER + 5,		// + 0x4 + 500 (8 BIT), 5 (16 BIT)
		.flags	= IORESOURCE_MEM
	},
	[2] = {
		.start	= CFG_EXT_IRQ_NUM_ETHER,
		.end	= CFG_EXT_IRQ_NUM_ETHER,
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	}
};

static struct dm9000_plat_data dm9000_platdata = {
	.flags		= DM9000_PLATF_16BITONLY,	// DM9000_PLATF_8BITONLY
};

static struct platform_device dm9000_plat_device = {
	.name			= DM9000_DEV_NAME,
	.id				= 0,
	.num_resources	= ARRAY_SIZE(dm9000_resource),
	.resource		= dm9000_resource,
	.dev			= {
		.platform_data	= &dm9000_platdata,
	}
};
#endif	/* CONFIG_DM9000 || CONFIG_DM9000_MODULE */

void clk_disable(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate (struct clk *clk)
{
	return CFG_SYS_PLL0_FREQ;
}
EXPORT_SYMBOL(clk_get_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	return 0;
}
EXPORT_SYMBOL(clk_set_rate);

int clk_enable(struct clk *clk)
{
	return 0;
}
EXPORT_SYMBOL(clk_enable);

struct clk *clk_get(struct device *dev, const char *id)
{
	return dev && strcmp(dev_name(dev), "mb:16") == 0 ? NULL : ERR_PTR(-ENOENT);
}
EXPORT_SYMBOL(clk_get);

void clk_put(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_put);

/*------------------------------------------------------------------------------
 * Frame Buffer platform device
 */
#if defined(CONFIG_FB_NEXELL)

/* fb0 */
#if defined(CONFIG_FB_NEXELL_PRIMARY)
static struct fb_plat_data fb0_platdata_320_240 = {
	.module		= CFG_DISP_MAIN_SCREEN == MAIN_SCREEN_PRI ? 0 : 1,
	.layer		= CFG_DISP_LAYER_SCREEN,
	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_320_240,
	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_320_240,
	.pixelbit	= CFG_DISP_SCREEN_PIXEL_BYTE * 8,
	.format		= CFG_DISP_SCREEN_RGB_FORMAT,
#if (1)
	.buffers	= 8,
#else
	.buffers	= 1,
#endif
	.with_mm	= CFG_DISP_PRI_RESOL_WIDTH_320_240 * 25 / 72,
	.height_mm	= CFG_DISP_PRI_RESOL_HEIGHT_320_240 * 25 / 72,
	.hs_left	= CFG_DISP_PRI_HSYNC_SYNC_WIDTH + CFG_DISP_PRI_HSYNC_BACK_PORCH_320_240,
	.hs_right	= CFG_DISP_PRI_HSYNC_FRONT_PORCH_320_240,
	.vs_upper	= CFG_DISP_PRI_VSYNC_SYNC_WIDTH + CFG_DISP_PRI_VSYNC_BACK_PORCH_320_240,
	.vs_lower	= CFG_DISP_PRI_VSYNC_FRONT_PORCH_320_240,
	.pixelclock	= CFG_DISP_PRI_PIXEL_CLOCK_320_240,
};

static struct platform_device fb0_plat_device_320_240 = {
	.name	= FB_DEV_NAME,
	.id		= 0,	/* device channel */
	.dev    = {
		.coherent_dma_mask 	= 0xffffffffUL,	/* for DMA allocate */
		.platform_data		= &fb0_platdata_320_240
	},
};

static struct fb_plat_data fb0_platdata_480_272 = {
	.module		= CFG_DISP_MAIN_SCREEN == MAIN_SCREEN_PRI ? 0 : 1,
	.layer		= CFG_DISP_LAYER_SCREEN,
	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_480_272,
	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_480_272,
	.pixelbit	= CFG_DISP_SCREEN_PIXEL_BYTE * 8,
	.format		= CFG_DISP_SCREEN_RGB_FORMAT,
#if (1)
	.buffers	= 8,
#else
	.buffers	= 1,
#endif
	.with_mm	= CFG_DISP_PRI_RESOL_WIDTH_480_272 * 25 / 72,
	.height_mm	= CFG_DISP_PRI_RESOL_HEIGHT_480_272 * 25 / 72,
	.hs_left	= CFG_DISP_PRI_HSYNC_SYNC_WIDTH + CFG_DISP_PRI_HSYNC_BACK_PORCH_480_272,
	.hs_right	= CFG_DISP_PRI_HSYNC_FRONT_PORCH_480_272,
	.vs_upper	= CFG_DISP_PRI_VSYNC_SYNC_WIDTH + CFG_DISP_PRI_VSYNC_BACK_PORCH_480_272,
	.vs_lower	= CFG_DISP_PRI_VSYNC_FRONT_PORCH_480_272,
	.pixelclock	= CFG_DISP_PRI_PIXEL_CLOCK_480_272,
};

static struct platform_device fb0_plat_device_480_272 = {
	.name	= FB_DEV_NAME,
	.id		= 0,	/* device channel */
	.dev    = {
		.coherent_dma_mask 	= 0xffffffffUL,	/* for DMA allocate */
		.platform_data		= &fb0_platdata_480_272
	},
};

static struct fb_plat_data fb0_platdata_valencia_800_480 = {
	.module		= CFG_DISP_MAIN_SCREEN == MAIN_SCREEN_PRI ? 0 : 1,
	.layer		= CFG_DISP_LAYER_SCREEN,
	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_800_480,
	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_800_480,
	.pixelbit	= CFG_DISP_SCREEN_PIXEL_BYTE * 8,
	.format		= CFG_DISP_SCREEN_RGB_FORMAT,
#if (1)
	.buffers	= 8,
#else
	.buffers	= 1,
#endif
	.with_mm	= CFG_DISP_PRI_RESOL_WIDTH_800_480 * 25 / 72,
	.height_mm	= CFG_DISP_PRI_RESOL_HEIGHT_800_480 * 25 / 72,
	.hs_left	= CFG_DISP_PRI_HSYNC_SYNC_WIDTH + CFG_DISP_PRI_HSYNC_BACK_PORCH_VALENCIA_800_480,
	.hs_right	= CFG_DISP_PRI_HSYNC_FRONT_PORCH_VALENCIA_800_480,
	.vs_upper	= CFG_DISP_PRI_VSYNC_SYNC_WIDTH + CFG_DISP_PRI_VSYNC_BACK_PORCH_VALENCIA_800_480,
	.vs_lower	= CFG_DISP_PRI_VSYNC_FRONT_PORCH_VALENCIA_800_480,
	.pixelclock	= CFG_DISP_PRI_PIXEL_CLOCK_VALENCIA_800_480,
};

static struct platform_device fb0_plat_device_valencia_800_480 = {
	.name	= FB_DEV_NAME,
	.id		= 0,	/* device channel */
	.dev    = {
		.coherent_dma_mask 	= 0xffffffffUL,	/* for DMA allocate */
		.platform_data		= &fb0_platdata_valencia_800_480
	},
};

static struct fb_plat_data fb0_platdata_knd_800_480 = {
	.module		= CFG_DISP_MAIN_SCREEN == MAIN_SCREEN_PRI ? 0 : 1,
	.layer		= CFG_DISP_LAYER_SCREEN,
	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_800_480,
	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_800_480,
	.pixelbit	= CFG_DISP_SCREEN_PIXEL_BYTE * 8,
	.format		= CFG_DISP_SCREEN_RGB_FORMAT,
#if (1)
	.buffers	= 8,
#else
	.buffers	= 1,
#endif
	.with_mm	= CFG_DISP_PRI_RESOL_WIDTH_800_480 * 25 / 72,
	.height_mm	= CFG_DISP_PRI_RESOL_HEIGHT_800_480 * 25 / 72,
	.hs_left	= 88,
	.hs_right	= 40,
	.vs_upper	= 45,
	.vs_lower	= 13,
	.pixelclock	= CFG_DISP_PRI_PIXEL_CLOCK_VALENCIA_800_480,
};

static struct platform_device fb0_plat_device_knd_800_480 = {
	.name	= FB_DEV_NAME,
	.id		= 0,	/* device channel */
	.dev    = {
		.coherent_dma_mask 	= 0xffffffffUL,	/* for DMA allocate */
		.platform_data		= &fb0_platdata_knd_800_480
	},
};

static struct fb_plat_data fb0_platdata_1024_600 = {
	.module		= CFG_DISP_MAIN_SCREEN == MAIN_SCREEN_PRI ? 0 : 1,
	.layer		= CFG_DISP_LAYER_SCREEN,
	.x_res		= 1024,
	.y_res		= 600,
	.pixelbit	= CFG_DISP_SCREEN_PIXEL_BYTE * 8,
	.format		= CFG_DISP_SCREEN_RGB_FORMAT,
	// Non-rio devices should have 5, but AFAIK there are no production devices with a 1024x600 screen besides rio.
	.buffers	= 8,
	.with_mm	= 1024 * 25 / 72,
	.height_mm	= 600 * 25 / 72,
	.hs_left	= 160,
	.hs_right	= 160,
	.vs_upper	= 23,
	.vs_lower	= 12,
	.pixelclock	= 44000000,
};

static struct platform_device fb0_plat_device_1024_600 = {
	.name	= FB_DEV_NAME,
	.id		= 0,	/* device channel */
	.dev    = {
		.coherent_dma_mask 	= 0xffffffffUL,	/* for DMA allocate */
		.platform_data		= &fb0_platdata_1024_600
	},
};

static struct fb_plat_data fb0_platdata_vtk_800_480 = {
	.module		= CFG_DISP_MAIN_SCREEN == MAIN_SCREEN_PRI ? 0 : 1,
	.layer		= CFG_DISP_LAYER_SCREEN,
	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_800_480,
	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_800_480,
	.pixelbit	= CFG_DISP_SCREEN_PIXEL_BYTE * 8,
	.format		= CFG_DISP_SCREEN_RGB_FORMAT,
#if (1)
	.buffers	= 8,
#else
	.buffers	= 1,
#endif
	.with_mm	= CFG_DISP_PRI_RESOL_WIDTH_800_480 * 25 / 72,
	.height_mm	= CFG_DISP_PRI_RESOL_HEIGHT_800_480 * 25 / 72,
	.hs_left	= CFG_DISP_PRI_HSYNC_SYNC_WIDTH + CFG_DISP_PRI_HSYNC_BACK_PORCH_VTK_800_480,
	.hs_right	= CFG_DISP_PRI_HSYNC_FRONT_PORCH_VTK_800_480,
	.vs_upper	= CFG_DISP_PRI_VSYNC_SYNC_WIDTH + CFG_DISP_PRI_VSYNC_BACK_PORCH_VTK_800_480,
	.vs_lower	= CFG_DISP_PRI_VSYNC_FRONT_PORCH_VTK_800_480,
	.pixelclock	= CFG_DISP_PRI_PIXEL_CLOCK_VTK_800_480,
};

static struct platform_device fb0_plat_device_vtk_800_480 = {
	.name	= FB_DEV_NAME,
	.id		= 0,	/* device channel */
	.dev    = {
		.coherent_dma_mask 	= 0xffffffffUL,	/* for DMA allocate */
		.platform_data		= &fb0_platdata_vtk_800_480
	},
};
#endif

/* fb1 */
#if defined(CONFIG_FB_NEXELL_SECONDARY)
static struct fb_plat_data fb1_platdata = {
	.module		= CFG_DISP_MAIN_SCREEN == MAIN_SCREEN_PRI ? 1 : 0,
	.layer		= CFG_DISP_LAYER_SCREEN,
	.x_res		= CFG_DISP_SEC_RESOL_WIDTH,
	.y_res		= CFG_DISP_SEC_RESOL_HEIGHT,
	.pixelbit	= CFG_DISP_SCREEN_PIXEL_BYTE * 8,
	.format		= CFG_DISP_SCREEN_RGB_FORMAT,
#ifdef CONFIG_ANDROID
	.buffers	= 2,
#else
	.buffers	= 1,
#endif
};

static struct platform_device fb1_plat_device = {
	.name	= FB_DEV_NAME,
	.id		= 1,	/* device channel */
	.dev    = {
		.coherent_dma_mask 	= 0xffffffffUL,	/* for DMA allocate */
		.platform_data		= &fb1_platdata
	},
};
#endif

static struct platform_device *fb_plat_devices_320_240[]  = {
#if defined(CONFIG_FB_NEXELL_PRIMARY)
		&fb0_plat_device_320_240,
#endif
#if defined(CONFIG_FB_NEXELL_SECONDARY)
		&fb1_plat_device,
#endif
};

static struct platform_device *fb_plat_devices_480_272[] = {
#if defined(CONFIG_FB_NEXELL_PRIMARY)
		&fb0_plat_device_480_272,
#endif
#if defined(CONFIG_FB_NEXELL_SECONDARY)
		&fb1_plat_device,
#endif
};

static struct platform_device *fb_plat_devices_800_480[] = {
#if defined(CONFIG_FB_NEXELL_PRIMARY)
		&fb0_plat_device_valencia_800_480,
#endif
#if defined(CONFIG_FB_NEXELL_SECONDARY)
		&fb1_plat_device,
#endif
};

static struct platform_device *fb_plat_devices_1024_600[] = {
#if defined(CONFIG_FB_NEXELL_PRIMARY)
		&fb0_plat_device_1024_600,
#endif
#if defined(CONFIG_FB_NEXELL_SECONDARY)
		&fb1_plat_device,
#endif
};
#endif	/* CONFIG_FB_NEXELL */

/*------------------------------------------------------------------------------
 * V4L2 platform device
 */
#if defined(CONFIG_VIDEO_NEXELL)
static struct v4l2_plat_data v4l2_platdata = {
	.in_port		= 1,
	.dpc_device		= MAIN_SCREEN_PRI,
	.hor_align		= 1,
	.ver_align		= 1,
	.buff_count		= 4,
	.skip_count		= 3,
};

static struct platform_device v4l2_plat_device = {
	.name	= V4L2_DEV_NAME,
	.id		= 0,	/* 0, 1, ... */
	.dev    = {
		.platform_data	= &v4l2_platdata,
	},
};
#endif	/* CONFIG_VIDEO_NEXELL */

/*------------------------------------------------------------------------------
 * VIP platform device
 */
#if defined(CONFIG_GRP_NEXELL_VIP) || defined(CONFIG_GRP_NEXELL_VIP_MODULE)

#if	defined(CONFIG_GRP_NEXELL_VIP_MODULE0)
static struct platform_device vip_plat_device0 = {
	.name	= VIP_DEV_NAME,
	.id		= 0,
};
#endif

static struct platform_device *vip_plat_devices[] __initdata = {
#if	defined(CONFIG_GRP_NEXELL_VIP_MODULE0)
	&vip_plat_device0,
#endif
};
#endif	/* CONFIG_GRP_NEXELL_VIP || CONFIG_GRP_NEXELL_VIP_MODULE */

/*------------------------------------------------------------------------------
 * Hynix YACD511SBDBC [Hi-253] CMOS Image Sensor
 */
#if defined(CONFIG_SOC_CAMERA_SR300PC10)
static const struct lf2000_vip_platform_data lf2000_vip_platdata = {
	/* Incompatible HSYNC - must use ITU-R 656 */
	/* Wired to alternate input port */
	.flags	= LF2000_VIP_PORT | LF2000_VIP_ITU656,

	/* TODO: where are these specified? */
	.hfp	= 7,
	.hsw	= 1,
	.hbp	= 1,
	.vfp	= 0,
	.vsw	= 1,
	.vbp	= 1,
};

static unsigned long lf2k_hi253_query_bus_param(struct soc_camera_link *link)
{
	return SOCAM_DATAWIDTH_8;
}

static void hi253_set_control_pins(struct hynix_yac_camera_info	*info)
{
	if(info->name == "hi253-rear")
	{
		info->RESETB		= REAR_CAM_RESET_L;
		info->CHIP_ENABLEB	= REAR_CAM_ENABLE_L;
		info->resetb_name	= "hi253-rear reset";
		info->enableb_name	= "hi253-rear enable";
		
		switch(system_rev) {
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
		case LF2000_BOARD_VALENCIA_CIP:
			info->CLOCK_ENABLEB = RCAM_CLK_ENA_L;
			info->clock_enableb_name = "hi253-rear clock enable";
			break;
		default:
			break;
		}
	}
	
	if(info->name == "hi253-front")
	{
		info->RESETB		= FRONT_CAM_RESET_L;
		info->CHIP_ENABLEB	= FRONT_CAM_ENABLE_L;
		info->resetb_name	= "hi253-front reset";
		info->enableb_name	= "hi253-front enable";
	
		switch(system_rev) {
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
		case LF2000_BOARD_VALENCIA_CIP:
			info->CLOCK_ENABLEB = FCAM_CLK_ENA_L;
			info->clock_enableb_name = "hi253-front clock enable";
			break;
		default:
			break;
		}
	}
}

static int hi253_power(struct device *dev, int on)
{
	struct soc_camera_link		*icl	= dev->platform_data;
	struct hynix_yac_camera_info	*info	= icl->priv;
	int err;
	
	dev_info(dev, "hynix: %s\n", __FUNCTION__);

	/* For VCLK27 (MCLK) */
	volatile U32 *BASE;
	U32 val;

	BASE = (U32*)(IO_ADDRESS(NX_VIP_GetPhysicalAddress(0)) + 0x14);
	val = *BASE;

	/* MCLK */
	soc_gpio_set_io_func(PAD_GPIO_D + 24, NX_GPIO_PADFUNC_2);

	if(on)
	{
		if(info->attached == false)
		{
			printk(KERN_INFO "hynix: setting control pins \n");
			hi253_set_control_pins(info);
			
			/* First run - configure pins */
			dev_info(dev, "hynix: Configuring pins...\n");
			
			err = gpio_request(info->RESETB, info->resetb_name);
			if(err != 0)
			{
				dev_info(dev, "%s: No RESETB pin.\n", info->name);
			}
			gpio_set_current(info->RESETB, GPIO_CURRENT_2MA);
			gpio_set_function(info->RESETB, GPIO_GPIOFN);
			gpio_direction_output(info->RESETB, 1);
			gpio_set_pullup(info->RESETB, 0);

			err = gpio_request(info->CHIP_ENABLEB, info->enableb_name);
			if(err != 0)
			{
				dev_info(dev, "%s: No CHIP_ENABLEB pin.\n", info->name);
			}
			gpio_set_current(info->CHIP_ENABLEB, GPIO_CURRENT_2MA);
			gpio_set_function(info->CHIP_ENABLEB, GPIO_GPIOFN);
			gpio_direction_output(info->CHIP_ENABLEB, 1);
			gpio_set_pullup(info->CHIP_ENABLEB, 0);
			
			switch(system_rev) {
			case LF2000_BOARD_RIO:
			case LF2000_BOARD_RIO_KND_800_480:
			case LF2000_BOARD_RIO_BETA_1024_600:
			case LF2000_BOARD_RIO_BETA_800_480:
			case LF2000_BOARD_RIO_BETA_1024_600_700_400:
			case LF2000_BOARD_RIO_BETA_800_480_700_400:
			case LF2000_BOARD_RIO_BETA_1024_600_550_275:
			case LF2000_BOARD_RIO_EP_550_275:
			case LF2000_BOARD_RIO_EP_666_333:
			case LF2000_BOARD_RIO_EP_800_333:
			case LF2000_BOARD_RIO_EP_700_400:
			case LF2000_BOARD_RIO_EP_800_400:
			case LF2000_BOARD_RIO_FEP_800_327P67:
			case LF2000_BOARD_RIO_FEP_800_327P666:
			case LF2000_BOARD_VALENCIA_CIP:
				err = gpio_request(info->CLOCK_ENABLEB, info->clock_enableb_name);
				if(err != 0)
				{
					dev_info(dev, "%s: No CLK_ENABLE pin.\n", info->name);
				}
				gpio_set_current(info->CLOCK_ENABLEB, GPIO_CURRENT_2MA);
				gpio_set_function(info->CLOCK_ENABLEB, GPIO_GPIOFN);
				gpio_direction_output(info->CLOCK_ENABLEB, 1);
				gpio_set_pullup(info->CLOCK_ENABLEB, 0);
				break;
			default:
				break;
			}
		}
		
		dev_info(dev, "hynix: Power on \n");
		
		/* CHIP_ENABLEB low */
		gpio_direction_output(info->CHIP_ENABLEB, 0);
		
		//FIXME: This can be 5?
		msleep(10);

		switch(system_rev) {
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
		case LF2000_BOARD_VALENCIA_CIP:
			/* CLOCK_ENABLE low */
			gpio_direction_output(info->CLOCK_ENABLEB, 0);
			break;
		default:
			break;
		}

		/* MCLK on */
		WriteIODW(BASE, (val | 0x00010000));

		/* FIXME: Reduce it to 10? Check with Sam on board specifics */
		msleep(info->enable_delay_ms);

		/* release RESETB */
		if(true == info->reset)
		{
			info->reset = false;

			gpio_direction_output(info->RESETB, 1);
		}
	}
	else
	{
		dev_info(dev, "hynix: Power off...\n");
		/*
		 * Do not apply reset until device removal.
		 * Intermediate calls to .power() affect only ENABLEB and MCLK.
		 */
		if(info->attached == false)
		{
			info->reset = true;

			/* assert RESETB */
			gpio_direction_output(info->RESETB, 0);

			msleep(10);
		}
		
		/* MCLK off */
		WriteIODW(BASE, (val & ~(0x00010000)));

		switch(system_rev) {
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
		case LF2000_BOARD_VALENCIA_CIP:
			/* CLOCK_ENABLEB high */
			gpio_direction_output(info->CLOCK_ENABLEB, 1);
			break;
		default:
			break;
		}

		msleep(5);

		/* CHIP_ENABLEB high */
		gpio_direction_output(info->CHIP_ENABLEB, 1);

		if(info->attached == false)
		{
			/* Last run */
			gpio_free(info->CHIP_ENABLEB);
			gpio_free(info->RESETB);
			
			switch(system_rev) {
			case LF2000_BOARD_RIO:
			case LF2000_BOARD_RIO_KND_800_480:
			case LF2000_BOARD_RIO_BETA_1024_600:
			case LF2000_BOARD_RIO_BETA_800_480:
			case LF2000_BOARD_RIO_BETA_1024_600_700_400:
			case LF2000_BOARD_RIO_BETA_800_480_700_400:
			case LF2000_BOARD_RIO_BETA_1024_600_550_275:
			case LF2000_BOARD_RIO_EP_550_275:
			case LF2000_BOARD_RIO_EP_666_333:
			case LF2000_BOARD_RIO_EP_800_333:
			case LF2000_BOARD_RIO_EP_700_400:
			case LF2000_BOARD_RIO_EP_800_400:
			case LF2000_BOARD_RIO_FEP_800_327P67:
			case LF2000_BOARD_RIO_FEP_800_327P666:
			case LF2000_BOARD_VALENCIA_CIP:
				gpio_free(info->CLOCK_ENABLEB);
				break;
			default:
				break;
			}
		}
	}

	return 0;
}

static struct hynix_yac_camera_info rear_hi253_cam_info = {
	.name	= "hi253-rear",

	/* VCLK27 (GPIO_D[24]) */
	.mclk_hz		= 27000000,

	/* min tVCK = 9.25 ns */
	.host_max_pclk_hz	= 108000000,

	.vsync		= 20,

	/* Incompatible HSYNC - must use ITU-R 656 */
	.flags	= HYNIX_YAC_ITU656,

	/* ITU656 markers required by LF2000 */
	.sof	= 0x80,
	.sol	= 0x80,
	.eof	= 0xb6,
	.eol	= 0x9d,

	/* Initial state */
	.reset		= true,
	.attached	= false,
};

static struct i2c_board_info rear_hi253_i2c_info = {
	/* .type must match driver's i2c_device_id.name */
	I2C_BOARD_INFO("hynix_yac", (0x40 >> 1)),
};

static struct hynix_yac_camera_info front_hi253_cam_info = {
	.name	= "hi253-front",

	/* VCLK27 (GPIO_D[24]) */
	.mclk_hz		= 27000000,

	/* min tVCK = 9.25 ns */
	.host_max_pclk_hz	= 108000000,

	.vsync		= 20,

	/* Incompatible HSYNC - must use ITU-R 656 */
	.flags	= HYNIX_YAC_ITU656,

	/* ITU656 markers required by LF2000 */
	.sof	= 0x80,
	.sol	= 0x80,
	.eof	= 0xb6,
	.eol	= 0x9d,

	/* Initial state */
	.reset		= true,
	.attached	= false,
};

static struct i2c_board_info front_hi253_i2c_info = {
	/* .type must match driver's i2c_device_id.name */
	I2C_BOARD_INFO("hynix_yac", (0x40 >> 1)),
};

static struct soc_camera_link hi253_link[] = {
	{
		.bus_id			= 0,	/* matches VIP platform_device id */
		.board_info		= &rear_hi253_i2c_info,
		.power			= &hi253_power,
		.i2c_adapter_id		= 1,		/* i2c bus number */
		.query_bus_param	= lf2k_hi253_query_bus_param,
		.priv			= &rear_hi253_cam_info,
	},
	{
		.bus_id			= 0,	/* matches VIP platform_device id */
		.board_info		= &front_hi253_i2c_info,
		.power			= &hi253_power,
		.i2c_adapter_id		= 1,		/* i2c bus number */
		.query_bus_param	= lf2k_hi253_query_bus_param,
		.priv			= &front_hi253_cam_info,
	},
};

static struct platform_device lf2k_camera[] = {
	{
		.name	= "soc-camera-pdrv",
		.id	= 0,
		.dev	= {
			.platform_data = &hi253_link[0],
		},
	},
	{
		.name	= "soc-camera-pdrv",
		.id	= 1,
		.dev	= {
			.platform_data = &hi253_link[1],
		},
	},
};
#endif /* CONFIG_SOC_CAMERA_SR300PC10 */

#if defined(CONFIG_TOUCHSCREEN_ZFORCE_I2C) || defined(CONFIG_TOUCHSCREEN_ZFORCE_I2C_MODULE)
static struct i2c_board_info zforce_i2c_info = {
	/* .type must match driver's i2c_device_id.name */
	I2C_BOARD_INFO("zforce", 0x50),
	.platform_data	= USB_OVP,
};

static void set_zforce_irq()
{
	switch(system_rev) {
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
			zforce_i2c_info.irq = gpio_to_irq(TP_INT);
			break;
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_LUCY_CIP:
		case LF2000_BOARD_VALENCIA:
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
		case LF2000_BOARD_VALENCIA_CIP:
		case LF2000_BOARD_VTK:
			zforce_i2c_info.irq = gpio_to_irq(zforce_i2c_info.platform_data);
			break;
 		default:
			printk(KERN_ERR "%s: Invalid Board Revision 0x%X\n", __func__, system_rev);
			break;
	}
}
#endif /* CONFIG_TOUCHSCREEN_ZFORCE_I2C */
/*------------------------------------------------------------------------------
 * SDHC platform device
 */
#if defined(CONFIG_MMC_NEXELL_SDHC) || defined(CONFIG_MMC_NEXELL_SDHC_MODULE)

#if	defined(CONFIG_MMC_NEXELL_SDHC0)
#if defined(CONFIG_MMC_MES)
static struct resource sdhc0_resources[] = {
	[0] = {
		.start	= PHY_BASEADDR_SDHC,
		.end	= PHY_BASEADDR_SDHC + 0x2C8,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_PHY_SDHC0,
		.end	= IRQ_PHY_SDHC0,
		.flags	= IORESOURCE_IRQ,
	},
};
#endif
static struct sdhc_plat_data sdhc0_platdata = {
	.logical_io_detect	= SDHC_DETECT0,
	.logical_io_wprotect	= SDHC_WP0,
	.dma_index		= CFG_DMA_SDHC_0_RW,
};

struct platform_device sdhc0_plat_device = {
#if defined(CONFIG_MMC_MES)
	.name	= "mes-sdhc",
#elif defined(CONFIG_MMC_NEXELL)
	.name	= SDHC_DEV_NAME,
#endif
	.id		= 0,	/* device channel */
	.dev    = {
		.platform_data	= &sdhc0_platdata,
	},
#if defined(CONFIG_MMC_MES)
	.num_resources	= ARRAY_SIZE(sdhc0_resources),
	.resource	= sdhc0_resources,
#endif
};
#endif

#if	defined(CONFIG_MMC_NEXELL_SDHC1)
#if defined(CONFIG_MMC_MES)
static struct resource sdhc1_resources[] = {
	[0] = {
		.start	= PHY_BASEADDR_SDHC + OFFSET_OF_SDHC_MODULE,
		.end	= PHY_BASEADDR_SDHC + OFFSET_OF_SDHC_MODULE + 0x2C8,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_PHY_SDHC1,
		.end	= IRQ_PHY_SDHC1,
		.flags	= IORESOURCE_IRQ,
	},
};
#endif
static struct sdhc_plat_data sdhc1_platdata = {
	.logical_io_detect	= SDHC_DETECT1,
	.logical_io_wprotect	= SDHC_WP1,
	.dma_index		= CFG_DMA_SDHC_1_RW,
};

struct platform_device sdhc1_plat_device = {
#if defined(CONFIG_MMC_MES)
	.name	= "mes-sdhc",
#elif defined(CONFIG_MMC_NEXELL)
	.name	= SDHC_DEV_NAME,
#endif
	.id		= 1,	/* device channel */
	.dev    = {
		.platform_data	= &sdhc1_platdata,
	},
#if defined(CONFIG_MMC_MES)
	.num_resources	= ARRAY_SIZE(sdhc1_resources),
	.resource	= sdhc1_resources,
#endif
};
#endif

static struct platform_device *sdhc_plat_devices[] = {
#if	defined(CONFIG_MMC_NEXELL_SDHC0)
	&sdhc0_plat_device,
#endif
#if	defined(CONFIG_MMC_NEXELL_SDHC1)
	&sdhc1_plat_device,
#endif
};

#endif	/* CONFIG_MMC_NEXELL_SDHC || CONFIG_MMC_NEXELL_SDHC_MODULE */

/*------------------------------------------------------------------------------
 * Touch platform device
 */
#if	defined(CONFIG_TOUCHSCREEN_NEXELL_ADC) || defined(CONFIG_TOUCHSCREEN_NEXELL_ADC_MODULE)
static struct adc_ts_plat_data ts_platdata = {
	.io =  {
		.logical_xm_p		= XMON_P,
		.logical_xm_n		= XMON_N,
		.logical_ym_p		= YMON_P,
		.logical_ym_n		= YMON_N,
		.logical_detect		= PENDOWN_DETECT,
		.logical_control	= PENDOWN_CON,
		.delay		= 4,
	},
	.adc = {
		.x_ch		= CFG_TOUCH_X_ADC_CH,
		.y_ch		= CFG_TOUCH_Y_ADC_CH,
		.x_min	 	= 0,
		.x_max	 	= CFG_DISP_PRI_RESOL_WIDTH_800_480,	/* default 1023 */
		.y_min	 	= 0,
		.y_max	 	= CFG_DISP_PRI_RESOL_HEIGHT_800_480,	/* deafult 1023 */
		.pointercal = CFG_TOUCH_CALIBRATION 		/* CFG_TOUCH_CALIBRATION, */
	},
};

static struct platform_device ts_plat_device = {
	.name	= TS_DEV_NAME,
	.id		= -1,
	.dev    = {
		.platform_data	= &ts_platdata
	},
};
#endif	/* CONFIG_TOUCHSCREEN_NEXELL_ADC || CONFIG_TOUCHSCREEN_NEXELL_ADC_MODULE */

/*------------------------------------------------------------------------------
 * Keypad platform device
 */
#if defined(CONFIG_KEYBOARD_NEXELL_KEY) || defined(CONFIG_KEYBOARD_NEXELL_KEY_MODULE)

#include <linux/input.h>

static unsigned int  button_gpio[] = CFG_KEYPAD_KEY_BUTTON;
static unsigned int  button_code[] = CFG_KEYPAD_KEY_CODE;

struct key_plat_data key_platdata = {
	.bt_count	= ARRAY_SIZE(button_gpio),
	.bt_io		= button_gpio,
	.bt_code	= button_code,
};

static struct platform_device key_plat_device = {
	.name	= KEYPAD_DEV_NAME,
	.id		= -1,
	.dev    = {
		.platform_data	= &key_platdata
	},
};
#endif	/* CONFIG_KEYBOARD_NEXELL_KEY || CONFIG_KEYBOARD_NEXELL_KEY_MODULE */

/*------------------------------------------------------------------------------
 * Graphic VMEM platform device
 */
#if defined(CONFIG_GRP_NEXELL_VMEM)

#if defined(CONFIG_GRP_NEXELL_VMEM_KILLER)
static char * task_name[] = CFG_VMEM_KILLER_RESERVE_TASK;

struct vmem_plat_data vmem_platdata = {
	.resv_task_num	= ARRAY_SIZE(task_name),
	.resv_task_name	= task_name,
};
#endif

static struct platform_device vmem_plat_device = {
	.name	= VMEM_DEV_NAME,
	.id		= 0,
#if defined(CONFIG_GRP_NEXELL_VMEM_KILLER)
	.dev    = {
		.platform_data	= &vmem_platdata
	},
#endif
};
#endif 	/* CONFIG_GRP_NEXELL_VMEM && CONFIG_GRP_NEXELL_VMEM_KILLER */

/*------------------------------------------------------------------------------
 * SPI platform device
 */
#if defined(CONFIG_SPI_NEXELL) || defined(CONFIG_SPI_NEXELL_MODULE)

static struct spi_io_cs spi_board_cs [] = {
	{ CFG_PIO_SPI0_FRAME, NX_GPIO_PADFUNC_GPIO },	/* chip select 0 */
};

static struct nxp3200_spi_plat_data spi_platdata = {
	.port		= 0,
	.irq		= IRQ_PHY_SSPSPI0,
	.io_clk		= CFG_PIO_SPI0_CLOCK,
	.io_rx 		= CFG_PIO_SPI0_RX,
	.io_tx	 	= CFG_PIO_SPI0_TX,
	.io_cs		= spi_board_cs,
	.cs_num 	= ARRAY_SIZE(spi_board_cs),	/* Note> must be more than ARRAY_SIZE(spi_plat_board) */
	.format		= NX_SSPSPI_FORMAT_A,
	
	.clk_src	= CFG_SPI_CLK_SRC,
	.clk_divisor	= CFG_SPI_CLOCK_DIVISOR,
	.clk_divcount	= CFG_SPI_CLOCK_DIVCOUNT,
	
	.dma_tx_channel	= CFG_DMA_SPI_TX,
	.dma_rx_channel = CFG_DMA_SPI_RX,
};

static struct platform_device spi_plat_device = {
	.name	= SPI_DEV_NAME,
	.id	= 0,
	.dev    = {
		.platform_data	= &spi_platdata
	},
};
#endif	/* CONFIG_SPI_NEXELL || CONFIG_SPI_NEXELL_MODULE */

#if defined(CONFIG_SPI_SPIDEV) || defined(CONFIG_SPI_SPIDEV_MODULE)

#include <linux/spi/spi.h>

static struct spi_board_info spi_plat_board[] = {
	{
		.modalias 		 = "spidev",	/* fixup */
		.max_speed_hz 	 = 3125000,     /* max spi clock (SCK) speed in HZ */
		.bus_num 		 = 0,			/* Note> set bus num, must be smaller than ARRAY_SIZE(spi_plat_device) */
		.chip_select 	 = 0,			/* Note> set chip select num, must be smaller than spi cs_num */
	},
};
#endif

/*------------------------------------------------------------------------------
 * ASoC (CS42L52) platform device
 */
#if defined(CONFIG_SND_CODEC_CS42L52) || defined(CONFIG_SND_CODEC_CS42L52_MODULE)

#define	CS42L52_I2C_BUS		(0)

static struct i2c_board_info  cs42l52_i2c_bdi = {
	.type	= "cs42l52",
	.addr	= (0x94>>1),		// 0x4A (7BIT)
};
#endif

/*------------------------------------------------------------------------------
 * ASoC (LFP100) platform device
 */
#if defined(CONFIG_SOC_LFP100)
static struct platform_device lfp100_plat_device = {
	.name	= "lfp100-chip",
	.id		= -1,
};
#endif

#if defined(CONFIG_SND_CODEC_LFP100) || defined(CONFIG_SND_CODEC_LFP100_MODULE)

static struct i2c_board_info lfp100_i2c_codec_bdi = {
	.type	= "lfp100",
	.addr	= LFP100_ADDR,
};

static struct platform_device lfp100_asoc_device = {
	.name	= "lfp100-asoc",
	.id		= -1,
};
#endif

#if defined(CONFIG_SENSORS_LF2000)
static struct platform_device power_plat_device = {
	.name	= "lf2000-power",
	.id		= -1,
};
#endif

#if defined(CONFIG_BACKLIGHT_LF2000)
static struct platform_device backlight_plat_device = {
	.name	= "lf2000-bl",
	.id		= -1,
};
#endif

/*------------------------------------------------------------------------------
 * Acclerometer platform device
 */
#if defined(CONFIG_INPUT_LF2000_ACLMTR)
static struct platform_device aclmtr_plat_device = {
	.name	= "lf2000-aclmtr",
	.id		= -1,
};
#endif

/*------------------------------------------------------------------------------
 * Three-Axis sensor(LIS3XXX) platform device
 */
#if defined(CONFIG_SENSORS_LIS3_I2C) || defined(CONFIG_SENSORS_LIS3_I2C_MODULE)

#include <linux/lis3lv02d.h>

static struct lis3lv02d_platform_data lis3_platdata = {
	.click_flags 	= LIS3_CLICK_SINGLE_X | LIS3_CLICK_SINGLE_Y | LIS3_CLICK_SINGLE_Z,	/* reserved in LIS302DL */
	.irq_cfg		= LIS3_IRQ1_CLICK | LIS3_IRQ2_CLICK,

	.wakeup_flags	= LIS3_WAKEUP_X_LO | LIS3_WAKEUP_X_HI |
			  		  LIS3_WAKEUP_Y_LO | LIS3_WAKEUP_Y_HI |
			  		  LIS3_WAKEUP_Z_LO | LIS3_WAKEUP_Z_HI,
	.wakeup_thresh	= 10,
	.click_thresh_x = 10,	/* reserved in LIS302DL */
	.click_thresh_y = 10,	/* reserved in LIS302DL */
	.click_thresh_z = 10,	/* reserved in LIS302DL */
};

#define	LIS3_I2C_BUS		0

static struct i2c_board_info __initdata lis3_i2c_boardinfo = {
	.type			= "lis3lv02d",
	.addr			= 0x3A>>1,
	.platform_data	= &lis3_platdata,
	.irq			= PB_PIO_IRQ(PAD_GPIO_C + 10)
};
#endif

#if defined(CONFIG_BCM4319)
static struct bcm4319_platform_data {
	const unsigned			WIFI_RST_L;
	const struct platform_device	*sdhc;
};

static const struct bcm4319_platform_data bcm4319_data = {
	.WIFI_RST_L	= WIFI_RESET,
	.sdhc		= &sdhc1_plat_device,
};

void bcm_wlan_power_off(int param)
{
	struct mmc_host *host = platform_get_drvdata(bcm4319_data.sdhc);

	gpio_direction_output(bcm4319_data.WIFI_RST_L, 0);

	mmc_detect_change(host, 0);
	flush_delayed_work_sync(&host->detect);
}
EXPORT_SYMBOL(bcm_wlan_power_off);

void bcm_wlan_power_on(int param)
{
	struct mmc_host *host = platform_get_drvdata(bcm4319_data.sdhc);

	gpio_direction_output(bcm4319_data.WIFI_RST_L, 1);

	mmc_detect_change(host, 0);
	flush_delayed_work_sync(&host->detect);
}
EXPORT_SYMBOL(bcm_wlan_power_on);
#endif /* CONFIG_BCM4319 */
extern int lfp100_dump_registers(void);

void lf2000_poweroff(void)
{
	//printk(KERN_ALERT "%s\n", __FUNCTION__);

#if defined(CONFIG_SOC_LFP100)
	if(lfp100_is_charging_active())
	{
		//printk(KERN_ALERT "Go in standby \n");
		lfp100_set_power_standby();
	}
	else
	{
		//printk(KERN_ALERT "PMIC Poweroff \n");
		lfp100_set_power_off();
	}
#endif
}
EXPORT_SYMBOL(lf2000_poweroff);

/* (char str, const char *cmd) */
void lf2000_restart(char mode, const char *cmd)
{
	local_irq_disable();
	local_fiq_disable();

	switch (mode) {
	case 's':
		/* hop to zero to restart system */
		soft_restart(0);
		break;
	case 'h':
	default:
		printk(KERN_INFO "%s\n", __FUNCTION__);
		lf2000_set_scratch_reboot();
		/* use LF2000 software reset */
		NX_CLKPWR_SetSoftwareResetEnable(1);
		NX_CLKPWR_DoSoftwareReset();
		break;
	}
}

/*------------------------------------------------------------------------------
 * register board platform devices
 */
void __init board_device(void)
{
	u_int clkspd;
	printk(KERN_INFO "%s\n", __FUNCTION__);
	/* register platform poweroff and restart handlers */
	pm_power_off = lf2000_poweroff;

#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
	printk("plat: add device dm9000 net\n");
	platform_device_register(&dm9000_plat_device);
#endif

#if defined(CONFIG_FB_NEXELL)
	printk("plat: add device frame buffer\n");
	switch(get_lcd_size()) {
        case LCD_UNKNOWN:
		printk("unknown LCD size:%d, using 320 x 240\n", get_lcd_size());
		platform_add_devices(fb_plat_devices_320_240,
				     ARRAY_SIZE(fb_plat_devices_320_240));
		break;
        case LCD_320_240:    
		printk(" LCD 320 x 240\n");
		platform_add_devices(fb_plat_devices_320_240,
				     ARRAY_SIZE(fb_plat_devices_320_240));
		break;

        case LCD_480_272:
		printk(" LCD 480 x 272\n");
		platform_add_devices(fb_plat_devices_480_272,
				     ARRAY_SIZE(fb_plat_devices_480_272));
		break;

        case LCD_800_480:
		printk(" LCD 800 x 480\n");
		switch(system_rev) {
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
			fb_plat_devices_800_480[0] = &fb0_plat_device_knd_800_480;
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
			platform_add_devices(fb_plat_devices_800_480,
					     ARRAY_SIZE(fb_plat_devices_800_480));
			break;
		default:
			fb_plat_devices_800_480[0] = &fb0_plat_device_vtk_800_480;
			platform_add_devices(fb_plat_devices_800_480,
					     ARRAY_SIZE(fb_plat_devices_800_480));
			break;
		}
		break;

		case LCD_1024_600:
		printk(" LCD 1024 x 600\n");
		platform_add_devices(fb_plat_devices_1024_600,
				     ARRAY_SIZE(fb_plat_devices_1024_600));
		break;
	}
#endif

#if defined(CONFIG_VIDEO_NEXELL)
	printk("plat: add device v4l2 camera\n");
	platform_device_register(&v4l2_plat_device);
#endif

#if defined(CONFIG_GRP_NEXELL_VIP) || defined(CONFIG_GRP_NEXELL_VIP_MODULE)
	printk("plat: add device vip\n");
	platform_add_devices(vip_plat_devices, ARRAY_SIZE(vip_plat_devices));
#endif

#if defined(CONFIG_MMC_NEXELL_SDHC) || defined(CONFIG_MMC_NEXELL_SDHC_MODULE)
	printk("plat: add device sdhc (array:%d) \n", ARRAY_SIZE(sdhc_plat_devices));
	platform_add_devices(sdhc_plat_devices, ARRAY_SIZE(sdhc_plat_devices));
#endif

#if defined(CONFIG_TOUCHSCREEN_NEXELL_ADC) || defined(CONFIG_TOUCHSCREEN_NEXELL_ADC_MODULE)
	printk("plat: add device touchscreen\n");
	platform_device_register(&ts_plat_device);
#endif

#if defined(CONFIG_KEYBOARD_NEXELL_KEY) || defined(CONFIG_KEYBOARD_NEXELL_KEY_MODULE)
	printk("plat: add device keypad\n");
	platform_device_register(&key_plat_device);
#endif

#if defined(CONFIG_INPUT_LF2000_ACLMTR) || defined(CONFIG_INPUT_LF2000_ACLMTR_MODULE)
	printk("plat: add device aclmtr\n");
	platform_device_register(&aclmtr_plat_device);
#endif

#if defined(CONFIG_GRP_NEXELL_VMEM)
	printk("plat: add device graphic memory allocator\n");
	platform_device_register(&vmem_plat_device);
#endif

#if defined(CONFIG_SPI_NEXELL) || defined(CONFIG_SPI_NEXELL_MODULE)
	printk("plat: add device spi\n");
	clkspd = cpu_get_clock_hz(spi_platdata.clk_src);
	spi_platdata.clk_divcount = (clkspd / spi_platdata.clk_divisor / 25000000) + 1;
	if(spi_platdata.clk_divcount < 5)
		spi_platdata.clk_divcount = 5;
	platform_device_register(&spi_plat_device);
#endif

#if defined(CONFIG_SPI_SPIDEV) || defined(CONFIG_SPI_SPIDEV_MODULE)
	spi_register_board_info(spi_plat_board, ARRAY_SIZE(spi_plat_board));
	printk("plat: register spidev\n");
#endif

#if defined(CONFIG_SND_CODEC_CS42L52) || defined(CONFIG_SND_CODEC_CS42L52_MODULE)
	printk("plat: register asoc-cs42l52\n");
	i2c_register_board_info(CS42L52_I2C_BUS, &cs42l52_i2c_bdi, 1);
#endif

#if defined(CONFIG_SOC_LFP100)
	printk("plat: register lfp100 chip\n");
	platform_device_register(&lfp100_plat_device);
#endif

#if defined(CONFIG_SND_CODEC_LFP100) || defined(CONFIG_SND_CODEC_LFP100_MODULE)
	printk("plat: register asoc-lfp100\n");
	if (is_board_lucy())
		i2c_register_board_info(LFP100_I2C_ADAPTER_1, &lfp100_i2c_codec_bdi, 1);
	else
		i2c_register_board_info(LFP100_I2C_ADAPTER_0, &lfp100_i2c_codec_bdi, 1);
	platform_device_register(&lfp100_asoc_device);
#endif

#if defined(CONFIG_SENSORS_LF2000)
	printk("plat: register power monitor\n");
	platform_device_register(&power_plat_device);
#endif

#if defined(CONFIG_BACKLIGHT_LF2000)
	printk("plat: register backlight\n");
	platform_device_register(&backlight_plat_device);
#endif

#if defined(CONFIG_ANDROID_PMEM)
	printk("plat: add android pmem\n");
	platform_device_register(&pmem_plat_device);
#endif

#if defined(CONFIG_SOC_CAMERA_SR300PC10) || defined(CONFIG_SOC_CAMERA_SR300PC10_MODULE)
	printk("plat: register hi253\n");
	//lf2000_set_vip_info(&lf2000_vip_platform_data);
	
	if(get_rear_camera_type() != CAMERA_NONE)
	{
		struct soc_camera_link *link = lf2k_camera[0].dev.platform_data;
		struct hynix_yac_camera_info *info = link->priv;

		link->i2c_adapter_id = get_rear_camera_i2c_adapter_id();
		info->enable_delay_ms = get_camera_enable_delay_ms();
		//printk(KERN_INFO "%ld\n", info->flags);	
		platform_device_register(&lf2k_camera[0]);
	}
	if(get_front_camera_type() != CAMERA_NONE)
	{
		struct soc_camera_link *link = lf2k_camera[1].dev.platform_data;
		struct hynix_yac_camera_info *info = link->priv;

		link->i2c_adapter_id = get_front_camera_i2c_adapter_id();
		info->enable_delay_ms = get_camera_enable_delay_ms();
		//printk(KERN_INFO "%ld\n", info->flags);	
		platform_device_register(&lf2k_camera[1]);
	}
#endif

#if defined(CONFIG_BCM4319)
	gpio_request(bcm4319_data.WIFI_RST_L, "Wi-Fi reset");
#endif

#if defined(CONFIG_TOUCHSCREEN_ZFORCE_I2C) || defined(CONFIG_TOUCHSCREEN_ZFORCE_I2C_MODULE)
//zforce_i2c_info.irq = gpio_to_irq(USB_OVP);
//gpio_request_one(zforce_i2c_info.platform_data, GPIOF_IN, "zForce irq");
	set_zforce_irq();	
	i2c_register_board_info(0, &zforce_i2c_info, 1);
#endif /* CONFIG_TOUCHSCREEN_ZFORCE_I2C */
}

/*------------------------------------------------------------------------------
 * Serial platform device
 */
#if defined(CONFIG_SERIAL_NEXELL_UART)

#if	defined(CONFIG_SERIAL_NEXELL_UART_PORT0)
static struct platform_device uart_plat_device0 = {
	.name	= UART_DEV_NAME,
	.id		= 0,
};
#endif
#if	defined(CONFIG_SERIAL_NEXELL_UART_PORT1)
static struct platform_device uart_plat_device1 = {
	.name	= UART_DEV_NAME,
	.id		= 1,
};
#endif

static struct platform_device *uart_plat_devices[] __initdata = {
#if	defined(CONFIG_SERIAL_NEXELL_UART_PORT0)
	&uart_plat_device0,
#endif
#if	defined(CONFIG_SERIAL_NEXELL_UART_PORT1)
	&uart_plat_device1,
#endif
};
#endif /* CONFIG_SERIAL_NEXELL_UART */

/*------------------------------------------------------------------------------
 * I2C Bus platform device
 */
#if defined(CONFIG_I2C_NEXELL) || defined(CONFIG_I2C_NEXELL_PROTO)

#if	defined(CONFIG_I2C_NEXELL_PORT0) || defined (CONFIG_I2C_NEXELL_PROTO_PORT0)

static struct resource i2c0_resource[] = {
	[0] = DEFINE_RES_MEM(PHY_BASEADDR_I2C0_MODULE, SZ_4K),
	[1] = DEFINE_RES_IRQ(IRQ_PHY_I2C0),
};

static struct i2c_plat_data i2c0_platdata = {
	.port		= 0,
	.irq		= IRQ_PHY_I2C0,
	.logical_io_scl	= I2C_SCL0,
	.logical_io_sda	= I2C_SDA0,
	.clksrc 	= CFG_I2C0_CLOCK_SOURCE,
	.clkscale 	= CFG_I2C0_CLOCK_SCALER_GENERIC,
	.cntmax 	= CFG_I2C0_CLOCK_DELAY,
	.flags		= 0,
	.slave_addr	= 0x10,
	.frequency	= 400*1000,
	.sda_delay	= 100,
	.cfg_gpio	= NULL,
	.bus_num	= 0,
};

static struct platform_device i2c_plat_device0 = {
	.name	= I2C_DEV_NAME,
	.id		= 0,
	.num_resources	= ARRAY_SIZE(i2c0_resource),
	.resource	= i2c0_resource,
	.dev    = {
		.platform_data	= &i2c0_platdata
	},
};
#endif
#if	defined(CONFIG_I2C_NEXELL_PORT1) || defined (CONFIG_I2C_NEXELL_PROTO_PORT1)
static struct resource i2c1_resource[] = {
	[0] = DEFINE_RES_MEM(PHY_BASEADDR_I2C1_MODULE, SZ_4K),
	[1] = DEFINE_RES_IRQ(IRQ_PHY_I2C1),
};

static struct i2c_plat_data i2c1_platdata = {
	.port		= 1,
	.irq		= IRQ_PHY_I2C1,
	.logical_io_scl	= I2C_SCL1,
	.logical_io_sda	= I2C_SDA1,
	.clksrc 	= CFG_I2C1_CLOCK_SOURCE,
	.clkscale 	= CFG_I2C1_CLOCK_SCALER,
	.cntmax 	= CFG_I2C1_CLOCK_DELAY,
	.flags		= 0,
	.slave_addr	= 0x10,
	.frequency	= 400*1000,
	.sda_delay	= 100,
	.cfg_gpio	= NULL,
	.bus_num	= 1,
};

static struct platform_device i2c_plat_device1 = {
	.name	= I2C_DEV_NAME,
	.id		= 1,
	.num_resources	= ARRAY_SIZE(i2c1_resource),
	.resource	= i2c1_resource,
	.dev    = {
		.platform_data	= &i2c1_platdata
	},
};
#endif

static struct platform_device *i2c_plat_devices[] __initdata = {
#if	defined(CONFIG_I2C_NEXELL_PORT0) || defined(CONFIG_I2C_NEXELL_PROTO_PORT0)
	&i2c_plat_device0,
#endif
#if	defined(CONFIG_I2C_NEXELL_PORT1) || defined(CONFIG_I2C_NEXELL_PROTO_PORT1)
	&i2c_plat_device1,
#endif
};

#endif	/* CONFIG_I2C_NEXELL */

/*------------------------------------------------------------------------------
 * Alsa sound platform device (I2S-PCM)
 */
#if defined(CONFIG_SND_NEXELL_SOC_I2S) || defined(CONFIG_SND_NEXELL_SOC_I2S_MODULE)

struct i2s_plat_data i2s_platdata = {
	.master			= CFG_AUDIO_I2S_MASTER_MODE,
	.sync_bit		= CFG_AUDIO_I2S_SYNC_PERIOD,
	.pcmout_w		= CFG_AUDIO_I2S_PCM_OUT_WIDTH,
	.pcmin_w		= CFG_AUDIO_I2S_PCM_IN_WIDTH,
	.trans_mode		= CFG_AUDIO_I2S_TRANS_MODE,
	.sample_rate	= CFG_AUDIO_I2S_SAMPLE_RATES_32000,

	.clk_src0		= CFG_AUDIO_I2S_CLK_SRC_0,
	.clk_div0		= CFG_AUDIO_I2S_CLK_DIV_0_32000,
	.clk_inv0		= CFG_AUDIO_I2S_CLK_INV_0_32000,
	.clk_src1		= CFG_AUDIO_I2S_CLK_SRC_1,
	.clk_div1		= CFG_AUDIO_I2S_CLK_DIV_1,
	.clk_inv1		= CFG_AUDIO_I2S_CLK_INV_1,

/*
 * Set .clk_inv at run-time based on Audio chip in sound/soc/nexell/nexell-i2s.c
 * Note that LFP100 / LFP200 expects inverted clock
 */
	.clk_inv 		= CTRUE,	// invert for LFP100 / LFP200
};

static struct platform_device i2s_plat_device = {
	.name	= I2S_DEV_NAME,
	.id		= -1,
	.dev    = {
		.platform_data	= &i2s_platdata
	},
};
#endif /* CONFIG_SND_NEXELL_SOC_I2S || CONFIG_SND_NEXELL_SOC_I2S_MODULE */

#if defined(CONFIG_SND_NEXELL_SOC) || defined(CONFIG_SND_NEXELL_SOC_MODULE)
static struct platform_device pcm_plat_device = {
	.name	= PCM_DEV_NAME,
	.id		= -1,
};
#endif /* CONFIG_SND_NEXELL_SOC || CONFIG_SND_NEXELL_SOC_MODULE */

/*------------------------------------------------------------------------------
 * RTC (Real Time Clock) platform device
 */
#if defined(CONFIG_RTC_DRV_LF2000)
static struct resource rtc_resources[] = {
	[0] = {
		.start	= PHY_BASEADDR_RTC,
		.end	= PHY_BASEADDR_RTC + 0x00000020,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_PHY_RTC,
		.end	= IRQ_PHY_RTC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device rtc_plat_device = {
	.name	= RTC_DEV_NAME,
	.id		= 0,
	.num_resources	= ARRAY_SIZE(rtc_resources),
	.resource	= rtc_resources,
};
#endif	/* CONFIG_RTC_DRV_LF2000 */

/*------------------------------------------------------------------------------
 * Graphic DPC platform device
 */
#if defined(CONFIG_GRP_NEXELL_DPC)
static struct platform_device dpc_plat_device = {
	.name	= DPC_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_GRP_NEXELL_DPC */

/*------------------------------------------------------------------------------
 * USB OHCI platform device
 */
#if defined(CONFIG_USB_OHCI_HCD) ||	defined(CONFIG_USB_OHCI_HCD_MODULE)

static struct resource ohci_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_OHCI,
		.end    = PHY_BASEADDR_OHCI + 0x00000400,
		.flags  = IORESOURCE_IO,
	},
	[1] = {
		.start  = IRQ_PHY_OHCI,
		.end    = IRQ_PHY_OHCI,
		.flags  = IORESOURCE_IRQ,
	},
};

static u64 ohci_dmamask = 0xffffffffUL;

static struct platform_device ohci_plat_device = {
	.name	= OHCI_DEV_NAME,
	.id		= 0,
	.dev    = {
		.dma_mask = &ohci_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	},
	.num_resources  = ARRAY_SIZE(ohci_resources),
	.resource       = ohci_resources,
};

#endif	/* CONFIG_USB_OHCI_HCD || CONFIG_USB_OHCI_HCD_MODULE */


/*------------------------------------------------------------------------------
 * USB OTG Host or Gadget
 */
#if defined(CONFIG_USB_NEXELL_OTG_HOST) || defined(CONFIG_USB_NEXELL_OTG_HOST_MODULE) ||	\
	defined(CONFIG_USB_NEXELL_OTGD) || defined(CONFIG_USB_NEXELL_OTGD_MODULE)     ||	\
	defined(CONFIG_USB_LF2000_OTGD) || defined(CONFIG_USB_LF2000_OTGD_MODULE)     ||	\
	defined(CONFIG_USB_DWC_OTG) || defined(CONFIG_USB_DWC_OTG_MODULE)

#include <linux/delay.h>
#include <asm/io.h>
#include <mach/reg_otg.h>

void otg_clk_enable(void)
{
	volatile int i;
	writel(0x0B, (void __iomem *)LF2000_USBOTG_CLKENB);
	writel(0x0C, (void __iomem *)LF2000_USBOTG_CLKGEN);
	writel(0x0F, (void __iomem *)LF2000_USBOTG_CLKENB);
	for (i = 10; i > 0; i-- )
		/* noop */ ;
}
EXPORT_SYMBOL(otg_clk_enable);

void otg_clk_disable(void)
{
}
EXPORT_SYMBOL(otg_clk_disable);

/* Initializes OTG Phy. */
void otg_phy_init(void)
{
	/* Configure USB as Device */
	volatile int i;
	writel(0x0001, (void __iomem *)LF2000_USBOTG_TESTPARM7); /* TESTPARM7 = 0x1 */
#if defined(CONFIG_USB_LF2000_OTGD) || defined(CONFIG_USB_LF2000_OTGD_MODULE)
	writel(0x0430, (void __iomem *)LF2000_USBOTG_TESTPARM4); /* TESTPARM4 = 0x430 */
#else
	writel(0x0410, (void __iomem *)LF2000_USBOTG_TESTPARM4); /* TESTPARM4 = 0x410 */
#endif
	writel(0x0237, (void __iomem *)LF2000_USBOTG_PHYPOR);	 /* PHY POR on (1->0)(auto clear) */
	udelay(230);                  		 /* 220us delay need. 230us delayed */
	for (i = 600; i > 0; i--)
		/* noop */ ;
	writel(0x4000, (void __iomem *)LF2000_USBOTG_LINKCTL);	/* LINK - prst_n (0->1)(auto clear) */
	for(i = 6; i>0; i--)
		/* noop */ ;

	writel(0xC000, (void __iomem *)LF2000_USBOTG_LINKCTL);	/* LINK - hreset_n (0->1)(auto clear) */
}
EXPORT_SYMBOL(otg_phy_init);

void otg_phy_off(void)
{
}
EXPORT_SYMBOL(otg_phy_off);

#endif /* CONFIG_USB_NEXELL_OTG_HOST || CONFIG_USB_NEXELL_OTGD || CONFIG_USB_LF2000_OTG */

/*------------------------------------------------------------------------------
 * USB Gadget platform device
 */
#if defined(CONFIG_USB_NEXELL_OTGD) || defined(CONFIG_USB_NEXELL_OTGD_MODULE) ||	\
	defined(CONFIG_USB_LF2000_OTGD) || defined(CONFIG_USB_LF2000_OTGD_MODULE) ||	\
	defined(CONFIG_USB_DWC_OTG) || defined (CONFIG_USB_DWC_OTG_MODULE)

#include <linux/usb/ch9.h>
/* USB Control request data struct must be located here for DMA transfer */
struct usb_ctrlrequest usb_ctrl __attribute__((aligned(8)));
EXPORT_SYMBOL(usb_ctrl);

static struct resource otg_gadget_resources[] = {
	[0] = {
		.start  = SOC_PA_OTG,
		.end    = SOC_PA_OTG + SZ_16K - 1,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = IRQ_OTG,
		.end    = IRQ_OTG,
		.flags  = IORESOURCE_IRQ,
	},
};

static u64 otg_gadget_dmamask = 0xffffffffUL;

static struct platform_device otg_gadget_plat_device = {
	.name	= OTG_GADGET_DEV_NAME,
	.id		= -1,
	.dev    = {
		.dma_mask = &otg_gadget_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	},
	.num_resources  = ARRAY_SIZE(otg_gadget_resources),
	.resource       = otg_gadget_resources,
};

#endif	/* CONFIG_USB_NEXELL_OTGD || CONFIG_USB_LF2000_OTGD */

/*------------------------------------------------------------------------------
 * Graphic OpenGL|ES platform device
 */
static struct resource ogl_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_GRP3D,
		.end    = PHY_BASEADDR_GRP3D + 0x00001000,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start  = IRQ_PHY_GRP3D,
		.end    = IRQ_PHY_GRP3D,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device ogl_plat_device = {
	.name	= OGL_DEV_NAME,
	.id		= 0,
	.num_resources  = ARRAY_SIZE(ogl_resources),
	.resource       = ogl_resources,
};

/*------------------------------------------------------------------------------
 * MISC GPIO platform device
 */
#if defined(CONFIG_MISC_NEXELL_GPIO)
static struct platform_device gpio_plat_device = {
	.name	= GPIO_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_MISC_NEXELL_GPIO */

#if defined(CONFIG_MTD_NAND)
	static struct platform_nand_data lf2000_platform_nand_data = {
			/* first the struct platform_nand_chip */
		{
			.nr_chips = 1
		},
			/* then the platform_nand_ctrl */
		{
			.priv = NULL
		}
	};
	static struct resource lf2000_nand_resource = {
		.start			= 0x2C000000,
		.end			= 0x2C000018,
		.flags			= IORESOURCE_MEM,
	};
	static struct platform_device lf2000_nand_plat_device = {
		.name			= "lf2000-nand",
		.id				= -1,
		.num_resources	= 1,
		.resource		= &lf2000_nand_resource,
		.dev =  {
					.platform_data = &lf2000_platform_nand_data,
					.coherent_dma_mask = DMA_BIT_MASK(32),
					.dma_mask = &lf2000_nand_plat_device.dev.coherent_dma_mask,
				}
	};
#endif

/*------------------------------------------------------------------------------
 * VIP platform device
 */
#if defined(CONFIG_VIDEO_LF2000) || defined(CONFIG_VIDEO_LF2000_MODULE)
static struct resource vip_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_VIP0,
		.end    = PHY_BASEADDR_VIP0 + 0x000007CC,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = IRQ_PHY_VIP,
		.end    = IRQ_PHY_VIP,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device vip_plat_device = {
	.name	= VIP_DEV_NAME,
	.id		= 0,		/* matches soc_camera_link bus_id */
	.num_resources  = ARRAY_SIZE(vip_resources),
	.resource       = vip_resources,
	.dev    = {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.dma_mask = &vip_plat_device.dev.coherent_dma_mask,
		.platform_data = &lf2000_vip_platdata,
	},
};

void lf2000_set_vip_info(const struct lf2000_vip_platform_data *pdata)
{
	printk(KERN_INFO "%s %ld\n", __FUNCTION__, pdata->flags);	
	vip_plat_device.dev.platform_data = pdata;
}
#endif	/* CONFIG_VIDEO_LF2000 */

/*------------------------------------------------------------------------------
 * ADC driver to expose ADC channels
 */

static struct resource adc_resources[] = {
	[0] = {
		.start	= PHY_BASEADDR_ADC,
		.end	= PHY_BASEADDR_ADC + 0x00000040,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_PHY_ADC,
		.end	= IRQ_PHY_ADC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device adc_plat_device = {
	.name		= ADC_DEV_NAME,
	.id		= 0,
	.num_resources	= ARRAY_SIZE(adc_resources),
	.resource	= adc_resources,
};


/*------------------------------------------------------------------------------
 * PCA9555 GPIO Extension board
 */

#if defined(CONFIG_GPIO_PCA953X)
static int pca9555_setup(struct i2c_client *client, unsigned gpio,
				unsigned ngpio, void *context)
{
	//TODO: Need to probe board here?
	return 0;
}

static struct pca953x_platform_data pca9555_data = {
	.gpio_base	= 0xC00, //0b110000000000
	.irq_base	= IRQ_GPIOEX_START,
	.setup		= pca9555_setup,
};

static struct i2c_board_info pca9555_board_info = {
	.type		= "pca9555",
	.addr		= 0x27,
	.platform_data	= &pca9555_data,
	//.irq		= gpio_to_irq(LF2000_GPIO_PORT_A | 9),
};
#endif

extern void lf2000_gpio_init(void);
extern void soc_sysfs_init(void);
 
/*------------------------------------------------------------------------------
 * GPIO Button driver
 */
//#if defined(CONFIG_KEYBOARD_GPIO)
extern struct platform_device lf2000_gpio_buttons;
//#endif

/*------------------------------------------------------------------------------
 * SOC AES Device
 */
#if defined(CONFIG_CRYPTO_DEV_NXP3200)
static struct resource aes_resource = {
	.start		= PHY_BASEADDR_AES,
	.end		= PHY_BASEADDR_AES + 0x40,
	.flags		= IORESOURCE_MEM,
};

static struct platform_device aes_plat_device = {
	.name		= "nx_aes",
	.id		= 0,
	.num_resources	= 1,
	.resource	= &aes_resource,
};
#endif
/*------------------------------------------------------------------------------
 * IDCT platform device
 */
#if defined(CONFIG_LF2000_IDCT) || defined(CONFIG_LF2000_IDCT_MODULE)
static struct resource idct_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_IDCT,
		.end    = PHY_BASEADDR_IDCT + 0x000007C4,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = IRQ_PHY_IDCT,
		.end    = IRQ_PHY_IDCT,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device idct_plat_device = {
	.name	= IDCT_DEV_NAME,
	.id		= -1,
	.num_resources  = ARRAY_SIZE(idct_resources),
	.resource       = idct_resources,
};
#endif	/* CONFIG_LF2000_IDCT */

/*------------------------------------------------------------------------------
 * CSC (Color Space Converter) platform device
 */
#if defined(CONFIG_GRP_LF2000_CSC) || defined(CONFIG_GRP_LF2000_CSC_MODULE)
static struct resource csc_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_CSC,
		.end    = PHY_BASEADDR_CSC + 0x000007C4,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = IRQ_PHY_CSC,
		.end    = IRQ_PHY_CSC,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device csc_plat_device = {
	.name	= "lf2000-csc", //CSC_DEV_NAME,
	.id		= -1,
	.num_resources  = ARRAY_SIZE(csc_resources),
	.resource       = csc_resources,
};
#endif	/* CONFIG_LF2000_CSC */

/*------------------------------------------------------------------------------
 * Rotator platform device
 */
#if defined(CONFIG_GRP_LF2000_ROTATOR) || defined(CONFIG_GRP_LF2000_ROTATOR_MODULE)
static struct resource rotator_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_ROTATOR,
		.end    = PHY_BASEADDR_ROTATOR + 0x000007C4,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = IRQ_PHY_ROTATOR,
		.end    = IRQ_PHY_ROTATOR,
		.flags  = IORESOURCE_IRQ,
	},
};

static struct platform_device rotator_plat_device = {
	.name	= "lf2000-rotator", //ROTATOR_DEV_NAME,
	.id		= -1,
	.num_resources  = ARRAY_SIZE(rotator_resources),
	.resource       = rotator_resources,
};
#endif	/* CONFIG_LF2000_ROTATOR */

/*------------------------------------------------------------------------------
 * Alive register platform device
 */

static struct resource alive_resource = {
		.start	= PHY_BASEADDR_ALIVE,
		.end	= PHY_BASEADDR_ALIVE + 0x200,
		.flags	= IORESOURCE_MEM,
};

static struct platform_device alive_device = {
	.name	= "lf2000-alive",
	.id	= -1,
	.num_resources	= 1,
	.resource	= &alive_resource,
};

/*------------------------------------------------------------------------------
 * CPU sysfs virtual device -- allow access to SOC registers
 */

static struct platform_device sysfs_plat_device = {
	.name	= SYSFS_DEV_NAME,
	.id	= -1,
};

/*------------------------------------------------------------------------------
 * register cpu platform devices
 */
void __init cpu_device(void)
{
#if defined(CONFIG_GENERIC_GPIO)
	printk("initialize gpio chips\n");
	lf2000_gpio_init();
#endif

	printk("plat: add device graphic opengl|es\n");
	platform_device_register(&ogl_plat_device);

	printk("plat: add device alive registers\n");
	platform_device_register(&alive_device);

#if defined(CONFIG_SERIAL_NEXELL_UART)
	printk("plat: add device serial (array:%d)\n", ARRAY_SIZE(uart_plat_devices));
	platform_add_devices(uart_plat_devices, ARRAY_SIZE(uart_plat_devices));
#endif

#if defined(CONFIG_I2C_NEXELL) || defined(CONFIG_I2C_NEXELL_PROTO)
	printk("plat: add device i2c bus (array:%d) \n", ARRAY_SIZE(i2c_plat_devices));
	platform_add_devices(i2c_plat_devices, ARRAY_SIZE(i2c_plat_devices));
#endif

#if defined(CONFIG_GPIO_PCA953X)
	printk("plat: add device GPIO PCA953X extender \n");
	pca9555_board_info.irq = gpio_to_irq(LF2000_GPIO_PORT_A | 9);
	i2c_register_board_info(1, &pca9555_board_info, 1);
#endif

	printk("plat: add device ADC  \n");
	platform_device_register(&adc_plat_device);

#if defined(CONFIG_SND_NEXELL_SOC_I2S) ||	defined(CONFIG_SND_NEXELL_SOC_I2S_MODULE)
	printk("plat: add device asoc-i2s \n");
	platform_device_register(&i2s_plat_device);
#endif

#if defined(CONFIG_SND_NEXELL_SOC) ||	defined(CONFIG_SND_NEXELL_SOC_MODULE)
	printk("plat: add device asoc-pcm \n");
	platform_device_register(&pcm_plat_device);
#endif

#if defined(CONFIG_RTC_DRV_LF2000)
	printk("plat: add device Real Time Clock  \n");
	platform_device_register(&rtc_plat_device);
#endif

#if defined(CONFIG_GRP_NEXELL_DPC)
	printk("plat: add device display control\n");
	platform_device_register(&dpc_plat_device);
#endif

#if defined(CONFIG_USB_OHCI_HCD) ||	defined(CONFIG_USB_OHCI_HCD_MODULE)
	printk("plat: add device usb ohci\n");
	platform_device_register(&ohci_plat_device);
#endif

#if defined(CONFIG_USB_NEXELL_OTGD) || defined(CONFIG_USB_NEXELL_OTGD_MODULE) ||	\
	defined(CONFIG_USB_LF2000_OTGD) || defined(CONFIG_USB_LF2000_OTGD_MODULE) ||	\
	defined(CONFIG_USB_DWC_OTG) || defined(CONFIG_USB_DWC_OTG_MODULE)
	otg_clk_enable();
	otg_phy_init();
	
	printk("plat: add device usb gadget otg\n");
	platform_device_register(&otg_gadget_plat_device);
#endif

#if defined(CONFIG_MISC_NEXELL_GPIO)
	printk("plat: add device misc gpio\n");
	platform_device_register(&gpio_plat_device);
#endif

#if defined(CONFIG_MTD_NAND)
	printk("plat: add device lf2000-nand\n");
	platform_device_register(&lf2000_nand_plat_device);
#endif

#if defined(CONFIG_VIDEO_LF2000) || defined(CONFIG_VIDEO_LF2000_MODULE)
	printk("plat: add device vip\n");
	platform_device_register(&vip_plat_device);
#endif

#if defined(CONFIG_CRYPTO_DEV_NXP3200)
	printk("plat: add device aes\n");
	platform_device_register(&aes_plat_device);
#endif

//#if defined(CONFIG_KEYBOARD_GPIO)
	printk("plat: add device gpio_keyboard\n");
	platform_device_register(&lf2000_gpio_buttons);
//#endif

#if defined(CONFIG_LF2000_IDCT) || defined(CONFIG_LF2000_IDCT_MODULE)
	printk("plat: add device idct\n");
	platform_device_register(&idct_plat_device);
#endif

#if defined(CONFIG_GRP_LF2000_CSC) || defined(CONFIG_GRP_LF2000_CSC_MODULE)
	printk("plat: add device csc\n");
	platform_device_register(&csc_plat_device);
#endif

#if defined(CONFIG_GRP_LF2000_ROTATOR) || defined(CONFIG_GRP_LF2000_ROTATOR_MODULE)
	printk("plat: add device rotator\n");
	platform_device_register(&rotator_plat_device);
#endif
	printk("plat: add device SOC sysfs\n");
	platform_device_register(&sysfs_plat_device);
}
