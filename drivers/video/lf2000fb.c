/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * Modified for LeapFrog LF1000/LF2000 compatibility.
 * Dave Milici, LeapFrog Enterprises Inc, <dmilici@leapfrog.com>
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
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>
#include <asm/system_info.h>
#include <linux/major.h>
#include <linux/pm.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>
#include <mach/dpc.h>
#include <mach/vmem.h>
#include <mach/lf2000_lcd.h>
#include <mach/board_revisions.h>

#ifdef CONFIG_FB_NEXELL_LF1000_EXTENSION
#include <linux/lf1000/lf1000fb.h>
#endif

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "fb: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*
 * Current support fb: fb0, fb1
 * fb0 = Primary screen
 * fb1 = Secondary screen
 * fb2 = YUV layer (MLC layer 3)
 */
#define FB_NUM_LAYERS		3
#define IS_YUV_LAYER(x)		(x == 3)

#define	FB_OPS_MMAP			0
#define	FB_SOC_DEV			1

#define	FB_CLEAR_COLOR		(0x00)
#define FB_PALETTE_CLEAR 	(0x80000000)

/*------------------------------------------------------------------------------
 * 	local data and macro
 */
/* frame buffer display controller data */
struct fb_dpc_info {
	int 	module;		/* 0: primary, 1: secondary */
	int 	layer;		/* RGB 0, 1, 2 */
	int		x_res;		/* x resolution */
	int		y_res;		/* y resolution */
	int		x_virt;		/* virtual x resolution */
	int		y_virt;		/* virtual y resolution */
	int		pixelbit;	/* bit per pixel */
	u_int	format;
	u_int	offset;		/* pan buffer offset */
	int		w_mm;		/* width  mm for dpi */
	int		h_mm;		/* hegiht  mm for dpi */
	uint	hs_left;
	uint	hs_right;
	uint    hs_width;
	uint	vs_upper;
	uint	vs_lower;
	uint    vs_width;
	uint	pixelclk;
	uint	varclock;	/* inverse pixel clock per varinfo */
	uint	rotate;
};

struct fb_private {
	struct fb_info	*	fb;
	struct device	*	device;
	u32					palette_buffer[256];
	u32					pseudo_pal[16];
	struct fb_dpc_info	dpc;
	/* framebuffer address */
	unsigned int		pbase;
	u_char *			vbase;
	unsigned int		length;
};

#define	_R_BIT(bpi)	 (16 == bpi ? 5 : 8)
#define	_G_BIT(bpi)	 (16 == bpi ? 6 : 8)
#define	_B_BIT(bpi)	 (16 == bpi ? 5 : 8)
#define	_T_BIT(bpi)	 (32 == bpi ? 8 : 0)

static struct fb_ops nxfb_ops;
struct fb_private *g_fb_private[2] = {0, 0};

/*---------------------------------------------------------------------------------------------
 * 	FB device
 --------------------------------------------------------------------------------------------*/
static void fb_init_layer(struct fb_info *info)
{
	struct fb_private *priv = info->par;
#if FB_SOC_DEV
	int   module = priv->dpc.module;
	int   layer  = priv->dpc.layer;
	u_int pbase  = priv->pbase;
	int   xres   = priv->dpc.x_res;
	int   yres   = priv->dpc.y_res;
	int   pixel  = priv->dpc.pixelbit/8;
	int   format = priv->dpc.format;
	int   enabled = soc_dpc_get_layer_enable(module, layer);

	printk(KERN_INFO "%s: layer=%d, enabled=%d\n", __FUNCTION__, layer, enabled);
	if (layer == CFG_DISP_LAYER_SCREEN) {
		soc_dpc_set_rgb_enable(module, layer, 1);
		return;
	}

	#ifdef CONFIG_FB_NEXELL_CLONE
	if (1 == module) {
		struct fb_private *priv_1 = g_fb_private[0];
		int  bpp = priv->dpc.pixelbit/8;
		uint off = (priv_1->dpc.x_res - priv->dpc.x_res)/2 * bpp;
		xres += off;
	}
	#endif

	if (IS_YUV_LAYER(layer)) {
		soc_dpc_set_vid_enable(module, 0);
		soc_dpc_set_vid_format(module, FOURCC_YV12, xres, yres);
		soc_dpc_set_vid_address(module, pbase, 4096, pbase+2048, 4096, pbase+2048+4096*yres/2, 4096, 0);
		soc_dpc_set_vid_position(module, 0, 0, xres, yres, 0);
		soc_dpc_set_vid_priority(module, 2);
		soc_dpc_set_vid_enable(module, enabled);
		printk(KERN_INFO "%s: %d * %d - %d bpp (phys=%08x virt=0x%08x size=%d)\n",
			info->fix.id, xres, yres, pixel*8, pbase, (u_int)priv->vbase, priv->length);
		return;
	}

	soc_dpc_set_out_disable(module);
	soc_dpc_set_rgb_enable(module, layer, 0);
	soc_dpc_set_rgb_format(module, layer, format, xres, yres, pixel);
	soc_dpc_set_rgb_address(module, layer, pbase, pixel, xres*pixel, 0);
	soc_dpc_set_out_enable(module);
#ifdef CONFIG_ANDROID
	soc_dpc_set_rgb_enable(module, layer, 1);
#else
	soc_dpc_set_rgb_enable(module, layer, enabled);
#endif

#endif
	printk(KERN_INFO "%s: %d * %d - %d bpp (phys=%08x virt=0x%08x size=%d)\n",
		info->fix.id, xres, yres, pixel*8, pbase, (u_int)priv->vbase, priv->length);
}

static void fb_set_layer(struct fb_info *info, int waitvsync, int offset)
{
	struct fb_private *priv = info->par;

	int   module = priv->dpc.module;
	int   layer  = priv->dpc.layer;
	u_int pbase  = priv->pbase + offset;

#if !FB_SOC_DEV
	struct NX_MLC_RegisterSet * MLC_BASE = NULL;
	MLC_BASE = (struct NX_MLC_RegisterSet *)
				IO_ADDRESS(PHY_BASEADDR_MLC + (OFFSET_OF_MLC_MODULE * module));

    if (0 == layer || 1 == layer) {
		MLC_BASE->MLCRGBLAYER[layer].MLCADDRESS  = pbase;	/* set RGB layer base */
		MLC_BASE->MLCRGBLAYER[layer].MLCCONTROL |= (1UL << 4);	/* set dirty flag */
	} else if(2 == layer) {
		MLC_BASE->MLCRGBLAYER2.MLCADDRESS  = pbase;		/* set RGB layer base */
        MLC_BASE->MLCRGBLAYER2.MLCCONTROL |= (1UL << 4);		/* set dirty flag */
    } else
		printk(KERN_ERR "nx-fb: not support %d layer\n", layer);

#else
	int xres  = info->var.xres;
	int yres  = info->var.yres;
	int pixel = info->var.bits_per_pixel/8;

	if (IS_YUV_LAYER(priv->dpc.layer)) {
		int format, sizex, sizey;
		soc_dpc_get_vid_format(module, &format, &sizex, &sizey);
		if(format == FOURCC_YUYV)
			soc_dpc_set_vid_address(module, pbase, xres*2, pbase+2048, xres*2, pbase+2048+xres*2*sizey/2, xres*2, waitvsync);
		else
			soc_dpc_set_vid_address(module, pbase, 4096, pbase+2048, 4096, pbase+2048+4096*sizey/2, 4096, waitvsync);
	}
	else {
		layer = NONSTD_TO_POS(info->var.nonstd);
		soc_dpc_set_rgb_address(module, layer, pbase, pixel, xres*pixel, waitvsync);
	}
#endif
}
/*---------------------------------------------------------------------------------------------
 * 	FB locals
 --------------------------------------------------------------------------------------------*/
static int fb_alloc_memory(struct fb_info *info)
{
	struct fb_private *priv = info->par;
	unsigned int length, size;

	length = info->fix.smem_len;

	if(0 == length)
		return 0;

	#ifdef CONFIG_FB_NEXELL_CLONE
	if (1 == priv->dpc.module) {
		struct fb_private *priv_1 = g_fb_private[0];

		int  bpp   = priv->dpc.pixelbit/8;
		uint x_off = (priv_1->dpc.x_res - priv->dpc.x_res)/2 * bpp;
		uint y_off = (priv_1->dpc.y_res - priv->dpc.y_res)/2 * bpp * priv_1->dpc.x_res;

		priv->length = PAGE_ALIGN(length);
		priv->vbase  = priv_1->vbase + (x_off + y_off);
		priv->pbase  = priv_1->pbase + (x_off + y_off);
		info->screen_base	 = priv->vbase;
		info->fix.smem_start = priv->pbase;

		return priv->vbase ? 0 : -ENOMEM;
	}
	#endif

	DBGOUT("%s: %s layer %d (%d * %d - %d bpp), len:%d, align:%d\n",
		__func__, dev_name(priv->device), priv->dpc.module,
		priv->dpc.x_res, priv->dpc.y_res, priv->dpc.pixelbit, length, PAGE_ALIGN(length));

	/* allocate from system memory */
	priv->length = PAGE_ALIGN(length);
	if(is_rio(system_rev)) {
		/* max vmem block alloc = 16MB block (4K x 4K) */
		for (size = min(length, (4096 << 12)); size > 0 ; length -= size, size = length)
		{
			int ret;
			VM_IMEMORY vm;
			int vmem_alloc(VM_IMEMORY *pmem, int minor, void *listhead);

			memset(&vm, 0, sizeof(vm));
			vm.MemWidth = 4096;
			vm.MemHeight = size / 4096;
			vm.Flags = VMEM_BLOCK_BUFFER;
			vm.HorAlign = 1;
			vm.VerAlign = 1;
			ret = vmem_alloc(&vm, 0, NULL);

			printk(KERN_INFO "%s: vmem ret=%d, phys=%08x, virt=%08x\n", __func__, ret, vm.Address, vm.Virtual);

			if (priv->pbase)
				continue;

			priv->pbase = vm.Address & ~0x20000000UL;
			priv->vbase = (void*)vm.Virtual;
		}
	} else {
	priv->vbase  = dma_alloc_writecombine(
						priv->device,
						priv->length,
						&priv->pbase,
						GFP_KERNEL);
    }


	if(priv->vbase) {
		/* leave bootloader framebuffer memory alone (no memset) */

		info->screen_base	 = priv->vbase;
		info->fix.smem_start = priv->pbase;

		if (IS_YUV_LAYER(priv->dpc.layer))
			info->fix.smem_start |= 0x20000000UL;
	}
	return priv->vbase ? 0 : -ENOMEM;
}

static void fb_free_memory(struct fb_info *info)
{
	struct fb_private *priv = info->par;
	DBGOUT("%s\n", __func__);

	#ifdef CONFIG_FB_NEXELL_CLONE
	if (1 == priv->dpc.module)
		return;
	#endif

	if(priv->vbase)
		dma_free_writecombine(priv->device, priv->length, priv->vbase, priv->pbase);
}

static void fb_init_private(struct fb_info *info, struct fb_plat_data * plat, int layer)
{
	struct fb_private *priv = info->par;
	struct fb_dpc_info *dpc = &priv->dpc;
	int i;

	priv->fb	 = info;
	priv->device = info->device;	/* use to alloc/free fb memory */

	/* clear palette buffer */
	for(i = 0; i < 256; i++)
		priv->palette_buffer[i] = FB_PALETTE_CLEAR;

	/* set hw variables */
	dpc->module 	= plat->module;
	dpc->layer  	= (layer == 2) ? 3 : layer; //plat->layer;
	dpc->x_res 		= plat->x_res;
	dpc->y_res 		= plat->y_res;
	dpc->x_virt		= plat->x_res;
	dpc->y_virt		= plat->y_res * plat->buffers; // shared between RGB layers
	dpc->pixelbit 	= (layer == 2) ? 8 : plat->pixelbit;
	dpc->format		= (layer == 2) ? FOURCC_YV12 : NX_MLC_RGBFMT_A8R8G8B8; //plat->format;
	dpc->offset		= plat->buffers == 1 ? 0 : plat->x_res * plat->y_res * plat->pixelbit / 8;
	dpc->w_mm 		= plat->with_mm;
	dpc->h_mm		= plat->height_mm;
	dpc->hs_left	= plat->hs_left;
	dpc->hs_right	= plat->hs_right;
	dpc->vs_upper	= plat->vs_upper;
	dpc->vs_lower	= plat->vs_lower;
	dpc->pixelclk	= plat->pixelclock;
	dpc->rotate		= 0;

	/* read actual dpc timing values from device */
	soc_dpc_get_timings(dpc->module, &dpc->pixelclk,
			&dpc->hs_width, &dpc->hs_left, &dpc->hs_right,
			&dpc->vs_width, &dpc->vs_upper, &dpc->vs_lower);

	if (g_fb_private[dpc->module] == 0)
		g_fb_private[dpc->module] = priv;
	DBGOUT("%s (module=%d)\n", __func__, dpc->module);
}

static void fb_init_info(struct fb_info *info)
{
	struct fb_private *priv = NULL;
	int x_virt, y_virt, bpp = 0;
	DBGOUT("%s\n", __func__);

	priv   = info->par;					/* get fb_private base */
	bpp    = priv->dpc.pixelbit;
	x_virt = IS_YUV_LAYER(priv->dpc.layer) ? 4096 : priv->dpc.x_res > priv->dpc.x_virt ? priv->dpc.x_res : priv->dpc.x_virt;

//FIXME: sesters patched for 1024 x 600 screen (Rio demo)
	switch(get_lcd_size())
	{
		case LCD_320_240:
		case LCD_480_272:
		case LCD_800_480:
		case LCD_UNKNOWN:
			y_virt = IS_YUV_LAYER(priv->dpc.layer) ?  512 : priv->dpc.y_res > priv->dpc.y_virt ? priv->dpc.y_res : priv->dpc.y_virt;
		break;

		case LCD_1024_600:
			y_virt = IS_YUV_LAYER(priv->dpc.layer) ? 2048 : 4096; //priv->dpc.y_res > priv->dpc.y_virt ? priv->dpc.y_res : priv->dpc.y_virt;
		break;
	}

	/* other variable information */
	info->var.width	    	= priv->dpc.w_mm; 		/* width  mm for dpi */
	info->var.height	    = priv->dpc.h_mm; 		/* height  mm for dpi */
	info->var.left_margin	= priv->dpc.hs_left;
	info->var.right_margin	= priv->dpc.hs_right;
	info->var.upper_margin	= priv->dpc.vs_upper;
	info->var.lower_margin	= priv->dpc.vs_lower;
	info->var.hsync_len		= priv->dpc.hs_width;
	info->var.vsync_len		= priv->dpc.vs_width;
	priv->dpc.varclock		=
	info->var.pixclock		= (1000000000/priv->dpc.pixelclk)*1000;	/* pico second */
	info->var.xres	    	= priv->dpc.x_res;
	info->var.yres	    	= priv->dpc.y_res;
	info->var.bits_per_pixel= priv->dpc.pixelbit;
	info->var.xres_virtual 	= x_virt;
	info->var.yres_virtual 	= y_virt;

	info->var.red.offset  	= _G_BIT(bpp) + _B_BIT(bpp);
	info->var.green.offset  = _B_BIT(bpp);
	info->var.blue.offset   = 0;
	info->var.transp.offset = _T_BIT(bpp) ? (_R_BIT(bpp) + _G_BIT(bpp) + _B_BIT(bpp)) : 0;
	info->var.red.length    = _R_BIT(bpp);
	info->var.green.length  = _G_BIT(bpp);
	info->var.blue.length   = _B_BIT(bpp);
	info->var.transp.length = _T_BIT(bpp);

	/* other palette */
	info->pseudo_palette	= &priv->pseudo_pal;

	/* other fixed, framebuffer memory length */
	info->fix.smem_len    	= IS_YUV_LAYER(priv->dpc.layer) ? 4096 * y_virt : 1 * x_virt * y_virt * (bpp / 8);
	info->fix.line_length 	= IS_YUV_LAYER(priv->dpc.layer) ? 4096 : x_virt * (bpp / 8);
	info->fix.type	    	= IS_YUV_LAYER(priv->dpc.layer) ? FB_TYPE_PLANES : FB_TYPE_PACKED_PIXELS;
	info->var.nonstd	 	= IS_YUV_LAYER(priv->dpc.layer) ? LAYER_FORMAT_YUV420 << LF1000_NONSTD_FORMAT : 0;
	info->var.nonstd		|= IS_YUV_LAYER(priv->dpc.layer) ? 2 << LF1000_NONSTD_PRIORITY : (priv->dpc.layer << LF1000_NONSTD_PRIORITY);
	DBGOUT("res: %d by %d, virtual: %d by %d, length:%d\n",
		priv->dpc.x_res, priv->dpc.y_res, x_virt, y_virt, info->fix.smem_len);
}

static struct fb_info *
fb_alloc_info(int fb, struct device *dev)
{
	struct fb_info * info = NULL;
	char name[256];

	/*
	 * 	allocate : fb_info + sizeof(struct fb_private)
	 */
	info = framebuffer_alloc(sizeof(struct fb_private), dev);
	if (! info) {
		printk(KERN_ERR "fail, unable to allocate frame buffer(%d) info ...\n", fb);
		return NULL;
	}

	/* fb_info members */
	sprintf(name, "%s %d", FB_DEV_NAME, fb);
	strcpy(info->fix.id, name);			/* name filed */

	/* default fixed information */
	info->fix.type	    = FB_TYPE_PACKED_PIXELS;
	info->fix.type_aux	= 0;
	info->fix.xpanstep	= 1;
	info->fix.ypanstep	= 1;	/* for fb_pan_display */
	info->fix.ywrapstep	= 0;
	info->fix.accel	    = FB_ACCEL_NONE;

	/* default variable information */
	info->var.nonstd	   	= 0;
	info->var.activate		= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
	info->var.accel_flags	= 0;
	info->var.vmode	    	= FB_VMODE_NONINTERLACED;

	/* link machind file operation to fb_info's file operation */
	info->fbops	= &nxfb_ops;
	info->flags	= FBINFO_FLAG_DEFAULT;

	/* default colormap: palette */
	if(fb_alloc_cmap(&info->cmap, 256, 0)) {
		printk(KERN_ERR "fail, unable to allocate cmap for frame buffer(%d) ...\n", fb);
		framebuffer_release(info);
		return NULL;
	}
	return info;
}

static void fb_exit_info(struct fb_info *info)
{
	fb_dealloc_cmap(&info->cmap);
	framebuffer_release(info);
}

static int fb_get_var_format(struct fb_var_screeninfo *var)
{
	switch (var->bits_per_pixel) {
		case 1:
		case 2:
		case 4:
		case 8:
			return NX_MLC_RGBFMT_PTR5G6B5; //0x443A << 16;
		case 16:
			if (var->transp.offset == 12) {
				if (var->blue.offset == 8)
					return NX_MLC_RGBFMT_A4B4G4R4;
				return NX_MLC_RGBFMT_A4R4G4B4;
			}
			if (var->blue.offset == 11)
				return NX_MLC_RGBFMT_B5G6R5;
			return NX_MLC_RGBFMT_R5G6B5;
		case 24:
			if (var->blue.offset == 16)
				return NX_MLC_RGBFMT_B8G8R8;
			return NX_MLC_RGBFMT_R8G8B8;
		case 32:
			if (var->transp.length == 0) {
				if (var->blue.offset == 16)
					return NX_MLC_RGBFMT_X8B8G8R8;
				return NX_MLC_RGBFMT_X8R8G8B8;
			}
			if (var->blue.offset == 16)
				return NX_MLC_RGBFMT_A8B8G8R8;
			return NX_MLC_RGBFMT_A8R8G8B8;
	}
	return 0;
}

static int fb_set_var_pixfmt(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct fb_private *priv = info->par;
	int format, width, height, depth;

	DBGOUT("%s (bpp:%d)\n", __func__, var->bits_per_pixel);

	soc_dpc_get_rgb_format(priv->dpc.module, priv->dpc.layer, &format, &width, &height, &depth);

	switch (var->bits_per_pixel) {
		case 1:
		case 2:
		case 4:
			var->red.offset    	= 0;
			var->red.length    	= var->bits_per_pixel;
			var->green         	= var->red;
			var->blue          	= var->red;
			var->transp.offset 	= 0;
			var->transp.length 	= 0;
			break;
		case 8:
			var->red.offset    	= 0;
			var->red.length    	= var->bits_per_pixel;
			var->green         	= var->red;
			var->blue          	= var->red;
			var->transp.offset 	= 0;
			var->transp.length 	= 0;
			break;
		case 16:
			if (format == NX_MLC_RGBFMT_A4R4G4B4 || format == NX_MLC_RGBFMT_A4B4G4R4) {
				/* 16 bpp, 4444 format */
				var->red.length		= 4;
				var->red.offset		= (format == NX_MLC_RGBFMT_A4R4G4B4) ? 8 : 0;
				var->green.length	= 4;
				var->green.offset	= 4;
				var->blue.length	= 4;
				var->blue.offset	= (format == NX_MLC_RGBFMT_A4B4G4R4) ? 8 : 0;
				var->transp.length	= 0;
				var->transp.offset	= 12;
				break;
			}
			/* 16 bpp, 565 format */
			var->red.length		= 5;
			var->red.offset		= 11;
			var->green.length	= 6;
			var->green.offset	= 5;
			var->blue.length	= 5;
			var->blue.offset	= 0;
			var->transp.length	= 0;
			if (format == NX_MLC_RGBFMT_B5G6R5) {
				var->blue.offset = var->red.offset;
				var->red.offset	= 0;
			}
			break;
		case 24:
			/* 24 bpp 888 */
			var->red.length		= 8;
			var->red.offset		= 16;
			var->green.length	= 8;
			var->green.offset	= 8;
			var->blue.length	= 8;
			var->blue.offset	= 0;
			var->transp.length	= 0;
			if (format == NX_MLC_RGBFMT_B8G8R8) {
				var->blue.offset = var->red.offset;
				var->red.offset	= 0;
			}
			break;
		case 32:
			/* 32 bpp 888 */
			var->red.length		= 8; 	//info->var.red.length;
			var->red.offset		= 16; 	//info->var.red.offset;
			var->green.length	= 8; 	//info->var.green.length;
			var->green.offset	= 8; 	//info->var.green.offset;
			var->blue.length	= 8; 	//info->var.blue.length;
			var->blue.offset	= 0; 	//info->var.blue.offset;
			var->transp.length	= 8; 	//info->var.transp.length;
			var->transp.offset	= 24; 	//info->var.transp.offset;
			if (format == NX_MLC_RGBFMT_A8B8G8R8) {
				var->blue.offset = var->red.offset;
				var->red.offset	= 0;
			}
			break;
		default:
			printk(KERN_ERR "error, not support fb bpp (%d)\n", var->bits_per_pixel);
	}

	return 0;
}

static void fb_dbg_var_info(struct fb_var_screeninfo *var)
{
#if (0)
	DBGOUT("%s: var->xres  				= %d\n", __func__, var->xres);
	DBGOUT("%s: var->tres  				= %d\n", __func__, var->yres);
	DBGOUT("%s: var->xres_virtual		= %d\n", __func__, var->xres_virtual);
	DBGOUT("%s: var->yres_virtual  		= %d\n", __func__, var->yres_virtual);
	DBGOUT("%s: var->xoffset       		= %d\n", __func__, var->xoffset);
	DBGOUT("%s: var->yoffset       		= %d\n", __func__, var->yoffset);
	DBGOUT("%s: var->bits_per_pixel		= %d\n", __func__, var->bits_per_pixel);
	DBGOUT("%s: var->grayscale     		= %d\n", __func__, var->grayscale);
	DBGOUT("%s: var->nonstd        		= %d\n", __func__, var->nonstd);
	DBGOUT("%s: var->activate      		= %d\n", __func__, var->activate);
	DBGOUT("%s: var->height        		= %d\n", __func__, var->height);
	DBGOUT("%s: var->width         		= %d\n", __func__, var->width);
	DBGOUT("%s: var->accel_flags   		= %d\n", __func__, var->accel_flags);
	DBGOUT("%s: var->pixclock      		= %d\n", __func__, var->pixclock);
	DBGOUT("%s: var->left_margin   		= %d\n", __func__, var->left_margin);
	DBGOUT("%s: var->right_margin  		= %d\n", __func__, var->right_margin);
	DBGOUT("%s: var->upper_margin  		= %d\n", __func__, var->upper_margin);
	DBGOUT("%s: var->lower_margin  		= %d\n", __func__, var->lower_margin);
	DBGOUT("%s: var->hsync_len     		= %d\n", __func__, var->hsync_len);
	DBGOUT("%s: var->vsync_len    		= %d\n", __func__, var->vsync_len);
	DBGOUT("%s: var->sync          		= %d\n", __func__, var->sync);
	DBGOUT("%s: var->vmode         		= %d\n", __func__, var->vmode);
	DBGOUT("%s: var->rotate        		= %d\n", __func__, var->rotate);
	DBGOUT("%s: var->red.offset    		= %d\n", __func__, var->red.offset);
	DBGOUT("%s: var->red.length  		= %d\n", __func__, var->red.length);
	DBGOUT("%s: var->red.msb_right 		= %d\n", __func__, var->red.msb_right);
	DBGOUT("%s: var->green.offset   	= %d\n", __func__, var->green.offset);
	DBGOUT("%s: var->green.length  		= %d\n", __func__, var->green.length);
	DBGOUT("%s: var->green.msb_right	= %d\n", __func__, var->green.msb_right);
	DBGOUT("%s: var->blue.offset    	= %d\n", __func__, var->blue.offset);
	DBGOUT("%s: var->blue.length  		= %d\n", __func__, var->blue.length);
	DBGOUT("%s: var->blue.msb_right 	= %d\n", __func__, var->blue.msb_right);
	DBGOUT("%s: var->transp.offset  	= %d\n", __func__, var->transp.offset);
	DBGOUT("%s: var->transp.length  	= %d\n", __func__, var->transp.length);
	DBGOUT("%s: var->transp.msb_right 	= %d\n", __func__, var->transp.msb_right);
#endif
}

/*---------------------------------------------------------------------------------------------
 * 	FB ops functions
 --------------------------------------------------------------------------------------------*/
/*
 *	nxfb_ops_check_var:
 *	Get the video params out of 'var'. If a value doesn't fit, round it up,
 *	if it's too big, return -EINVAL.
 *
 */
static int nxfb_ops_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	DBGOUT("%s (bpp:%d)\n", __func__, var->bits_per_pixel);
	// FIXME: supposed to validate pending 'var' info
	// fb_set_var_pixfmt(var, info);
	fb_dbg_var_info(var);
	return 0;
}

/*
 *	nxfb_ops_set_par - Optional function. Alters the hardware state.
 *  @info: frame buffer structure that represents a single frame buffer
 *
 */
static int nxfb_ops_set_par(struct fb_info *info)
{
	struct fb_var_screeninfo *var = &info->var;
	struct fb_private *priv = info->par;
	int layer = priv->dpc.layer;
	int enabled = soc_dpc_get_layer_enable(priv->dpc.module, priv->dpc.layer);
	union lf1000fb_cmd c;
	DBGOUT("%s\n", __func__);

	if (var->rotate != priv->dpc.rotate) {
		soc_dpc_set_lcd_flip(priv->dpc.module, var->rotate);
		priv->dpc.rotate = var->rotate;
	}

	if (var->pixclock != priv->dpc.varclock
			|| var->left_margin  != priv->dpc.hs_left
			|| var->right_margin != priv->dpc.hs_right
			|| var->upper_margin != priv->dpc.vs_upper
			|| var->lower_margin != priv->dpc.vs_lower
			|| var->hsync_len != priv->dpc.hs_width
			|| var->vsync_len != priv->dpc.vs_width) {
		int dotclk = (1000000000/var->pixclock)*1000;
		if (soc_dpc_set_timings(priv->dpc.module, dotclk,
				var->hsync_len, var->left_margin, var->right_margin,
				var->vsync_len, var->upper_margin, var->lower_margin)) {
			priv->dpc.pixelclk = dotclk;
			priv->dpc.varclock = var->pixclock;
			priv->dpc.hs_left  = var->left_margin;
			priv->dpc.hs_right = var->right_margin;
			priv->dpc.vs_upper = var->upper_margin;
			priv->dpc.vs_lower = var->lower_margin;
			priv->dpc.hs_width = var->hsync_len;
			priv->dpc.vs_width = var->vsync_len;
		}
	}

	if (IS_YUV_LAYER(priv->dpc.layer)) {
		priv->dpc.format = (NONSTD_TO_PFOR(var->nonstd) == LAYER_FORMAT_YUV422) ? FOURCC_YUYV : FOURCC_YV12;
		if(priv->dpc.format == FOURCC_YUYV)
			info->fix.line_length = var->xres * 2;
		else
			info->fix.line_length = 4096;
		soc_dpc_set_vid_enable(priv->dpc.module, 0);
		soc_dpc_get_vid_position(priv->dpc.module, &c.position.left, &c.position.top, &c.position.right, &c.position.bottom);
		soc_dpc_set_vid_format(priv->dpc.module, priv->dpc.format, var->xres, var->yres);
		soc_dpc_set_vid_address(priv->dpc.module, priv->pbase, info->fix.line_length, priv->pbase+2048, info->fix.line_length, priv->pbase+2048+info->fix.line_length*var->yres/2, info->fix.line_length, 0);
		soc_dpc_set_vid_position(priv->dpc.module, c.position.left, c.position.top, c.position.left+var->xres, c.position.top+var->yres, 0);
		soc_dpc_set_vid_priority(priv->dpc.module, NONSTD_TO_POS(var->nonstd));
		soc_dpc_set_vid_enable(priv->dpc.module, enabled);
		return 0;
	}

	switch (var->bits_per_pixel) {
		case 32:
		case 24:
		case 16:
			info->fix.visual = FB_VISUAL_TRUECOLOR;
			break;
		case 1:
			info->fix.visual = FB_VISUAL_MONO01;
			break;
		default:
			info->fix.visual = FB_VISUAL_PSEUDOCOLOR;
			break;
	}
	info->fix.line_length = (var->xres * var->bits_per_pixel) / 8;

	/* activate this new configuration */
	fb_set_var_pixfmt(var, info);
	layer = NONSTD_TO_POS(var->nonstd);
	enabled = soc_dpc_get_layer_enable(priv->dpc.module, layer);
	soc_dpc_set_rgb_enable(priv->dpc.module, layer, 0);
	soc_dpc_get_rgb_position(priv->dpc.module, layer, &c.position.left, &c.position.top);
	soc_dpc_set_rgb_format(priv->dpc.module, layer, fb_get_var_format(var), var->xres, var->yres, var->bits_per_pixel/8);
	soc_dpc_set_rgb_position(priv->dpc.module, layer, c.position.left, c.position.top, false);
	soc_dpc_set_rgb_address(priv->dpc.module, layer, priv->pbase, var->bits_per_pixel/8, info->fix.line_length, 0);
	soc_dpc_set_rgb_enable(priv->dpc.module, layer, enabled);
	fb_dbg_var_info(var);

	return 0;
}

/**
 *  nxfb_ops_blank
 *	@blank_mode: the blank mode we want.
 *	@info: frame buffer structure that represents a single frame buffer
 *
 *	Blank the screen if blank_mode != 0, else unblank. Return 0 if
 *	blanking succeeded, != 0 if un-/blanking failed due to e.g. a
 *	video mode which doesn't support it. Implements VESA suspend
 *	and powerdown modes on hardware that supports disabling hsync/vsync:
 *	blank_mode == 2: suspend vsync
 *	blank_mode == 3: suspend hsync
 *	blank_mode == 4: powerdown
 *
 *	Returns negative errno on error, or zero on success.
 *
 */
static int nxfb_ops_blank(int blank_mode, struct fb_info *info)
{
	struct fb_private *priv = info->par;
	DBGOUT("%s (mode=%d)\n", __func__, blank_mode);

	switch (blank_mode) {
	case FB_BLANK_UNBLANK:
		if (IS_YUV_LAYER(priv->dpc.layer))
			soc_dpc_set_vid_enable(priv->dpc.module, 1);
		else
			soc_dpc_set_rgb_enable(priv->dpc.module, NONSTD_TO_POS(info->var.nonstd), 1);
		break;
	case FB_BLANK_NORMAL:
		if (IS_YUV_LAYER(priv->dpc.layer))
			soc_dpc_set_vid_enable(priv->dpc.module, 0);
		else
			soc_dpc_set_rgb_enable(priv->dpc.module, NONSTD_TO_POS(info->var.nonstd), 0);
		break;
	}

	/* en/disable LCD */
	return 0;
}

/**
 *	nxfb_ops_setcolreg
 *	- framebuffer layer request to change palette.
 * 	@regno: The palette index to change.
 * 	@red: The red field for the palette data.
 * 	@green: The green field for the palette data.
 * 	@blue: The blue field for the palette data.
 * 	@trans: The transparency (alpha) field for the palette data.
 * 	@info: The framebuffer being changed.
 */
inline static u_int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static void schedule_palette_update(struct fb_info *info, unsigned int regno, unsigned int val)
{
	unsigned long flags;
	struct fb_private *priv = info->par;

	local_irq_save(flags);
	priv->palette_buffer[regno] = val;
	local_irq_restore(flags);
}

static int nxfb_ops_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	struct fb_private *priv = info->par;
	unsigned int val;

#if 0
	DBGOUT("%s (setcol: regno=%d, r=0x%x, g=0x%x, b=0x%x, t=0x%x)\n",
		__func__, regno, red, green, blue, transp);
#endif

	/* YUV is not RGB palette mode, even if 8bpp */
	if (IS_YUV_LAYER(priv->dpc.layer))
		return 1;

	switch (priv->fb->fix.visual) {
	case FB_VISUAL_TRUECOLOR:
		/* true-colour, use pseuo-palette */
		if (regno < 16) {
			u32 *pal = priv->fb->pseudo_palette;
			val  = chan_to_field(red,   &priv->fb->var.red);
			val |= chan_to_field(green, &priv->fb->var.green);
			val |= chan_to_field(blue,  &priv->fb->var.blue);
			pal[regno] = val;
		}
		break;
	case FB_VISUAL_PSEUDOCOLOR:
		if (regno < 256) {
			/* currently assume RGB 5-6-5 mode */
			val  = ((red   >>  0) & 0xf800);
			val |= ((green >>  5) & 0x07e0);
			val |= ((blue  >> 11) & 0x001f);
			schedule_palette_update(info, regno, val);
		}
		break;
	default:
		printk(KERN_ERR "fail, setcolreg return unknown type\n");
		return 1;   /* unknown type */
	}
	return 0;
}

#if FB_OPS_MMAP
static int nxfb_ops_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	struct fb_private *priv = info->par;
	unsigned long off = (vma->vm_pgoff << PAGE_SHIFT);
	uint length = PAGE_ALIGN(priv->length);
	int ret = -ENXIO;

	DBGOUT("%s vm (start:0x%08x, end:0x%08x, off:0x%08x, len:%d)\n",
		__func__, (u_int)vma->vm_start, (u_int)vma->vm_end,
		(u_int)vma->vm_pgoff, (u_int)(vma->vm_end - vma->vm_start) );

	if (off < priv->length) {
		ret = dma_mmap_writecombine(
					priv->device,
					vma,
					(void*)priv->vbase,
					priv->pbase,
					length);

		DBGOUT("%s: mmap (p:0x%08x, v:0x%08x, l:%d) to (s:0x%08x, e:0x%08x, o:0x%08x, l:%d)\n",
			info->fix.id,(u_int)priv->pbase, (u_int)priv->vbase, priv->length,
			(u_int)vma->vm_start, (u_int)vma->vm_end,
			(u_int)vma->vm_pgoff, (u_int)(vma->vm_end - vma->vm_start));
		DBGOUT("%s: mmap ret(%d)\n", ret);

		return ret;
	}

	return -EINVAL;
}
#endif

int nxfb_ops_cursor(struct fb_info *info, struct fb_cursor *cursor)
{
	/* not implementation */
	return 0;
}

static int nxfd_ops_pan_display(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct fb_private *priv = info->par;
	unsigned int offset = 0;
	unsigned int prebuf = priv->pbase;

#if (1) // pan x,y support
	offset = (var->yoffset * info->fix.line_length) + (var->xoffset * var->bits_per_pixel / 8);
#else
	if (var->yoffset || var->xoffset)
		offset = priv->dpc.offset;
#endif
	priv->pbase = info->fix.smem_start + offset;

	DBGOUT("%s (x:%4d, y:%4d, offs:0x%08x)\n", __func__, var->xoffset, var->yoffset, offset);

	/* change window layer base */
	if (prebuf != priv->pbase)
		fb_set_layer(info, IS_YUV_LAYER(priv->dpc.layer) ? 0 : 1, 0);

#if 0
	if (!soc_dpc_get_layer_enable(priv->dpc.module, priv->dpc.layer)) {
		if (IS_YUV_LAYER(priv->dpc.layer))
			soc_dpc_set_vid_enable(priv->dpc.module, 1);
		else
			soc_dpc_set_rgb_enable(priv->dpc.module, priv->dpc.layer, 1);
	}
#endif

	return 0;
}

#ifdef CONFIG_FB_NEXELL_LF1000_EXTENSION
static int lf1000fb_ioctl(struct fb_info *info, unsigned int cmd,
		unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	struct fb_private *priv = info->par;
	union lf1000fb_cmd c, s;
	int format, width, height, depth, enable;
	int layer = IS_YUV_LAYER(priv->dpc.layer) ? priv->dpc.layer : NONSTD_TO_POS(info->var.nonstd);

	DBGOUT("%s: ioctl=%08X, layer=%d, layer=%d\n", __func__, cmd, priv->dpc.layer, layer);

	switch (cmd) {
		case FBIO_WAITFORVSYNC:
			// soc_dpc_wait_vsync(priv->dpc.module);
			// return vsync status without waiting
			return NX_MLC_GetDirtyFlag(priv->dpc.module, layer);
			break;

		case LF1000FB_IOCSALPHA:
			if (!(_IOC_DIR(cmd) & _IOC_WRITE))
				return -EFAULT;
			if (copy_from_user((void *)&c, argp, sizeof(struct lf1000fb_blend_cmd)))
				return -EFAULT;
			if (IS_YUV_LAYER(priv->dpc.layer))
				soc_dpc_set_vid_color(priv->dpc.module, VID_COL_ALPHA, c.blend.alpha, c.blend.enable);
			else
				soc_dpc_set_rgb_color(priv->dpc.module, layer, RGB_COL_ALPHA, c.blend.alpha, c.blend.enable);
			break;

		case LF1000FB_IOCGALPHA:
			if (!(_IOC_DIR(cmd) & _IOC_READ))
				return -EFAULT;
			if (IS_YUV_LAYER(priv->dpc.layer))
				c.blend.alpha = soc_dpc_get_vid_color(priv->dpc.module, VID_COL_ALPHA);
			else
				c.blend.alpha = soc_dpc_get_rgb_color(priv->dpc.module, layer, RGB_COL_ALPHA);
			if (copy_to_user(argp, (void *)&c, sizeof(struct lf1000fb_blend_cmd)))
				return -EFAULT;
			break;

		case LF1000FB_IOCSPOSTION:
			if (!(_IOC_DIR(cmd) & _IOC_WRITE))
				return -EINVAL;
			if (copy_from_user((void *)&c, argp, sizeof(struct lf1000fb_position_cmd)))
				return -EFAULT;
			if (IS_YUV_LAYER(priv->dpc.layer))
				soc_dpc_set_vid_position(priv->dpc.module, c.position.left, c.position.top, c.position.right, c.position.bottom, false);
			else {
				soc_dpc_set_rgb_position2(priv->dpc.module, layer, c.position.left, c.position.top, c.position.right, c.position.bottom, false);
			}
			break;

		case LF1000FB_IOCGPOSTION:
			if (!(_IOC_DIR(cmd) & _IOC_READ))
				return -EINVAL;
			if (IS_YUV_LAYER(priv->dpc.layer))
				soc_dpc_get_vid_position(priv->dpc.module, &c.position.left, &c.position.top, &c.position.right, &c.position.bottom);
			else {
				soc_dpc_get_rgb_position2(priv->dpc.module, layer, &c.position.left, &c.position.top, &c.position.right, &c.position.bottom);
			}
			if (copy_to_user(argp, (void *)&c, sizeof(struct lf1000fb_position_cmd)))
				return -EFAULT;
			break;

		case LF1000FB_IOCSVIDSCALE:
			if (!(_IOC_DIR(cmd) & _IOC_WRITE))
				return -EINVAL;
			if (copy_from_user((void *)&c, argp, sizeof(struct lf1000fb_vidscale_cmd)))
				return -EFAULT;
			if (!IS_YUV_LAYER(priv->dpc.layer))
				return -EINVAL;
			soc_dpc_get_vid_position(priv->dpc.module, &s.position.left, &s.position.top, &s.position.right, &s.position.bottom);
			soc_dpc_set_vid_format(priv->dpc.module, priv->dpc.format, c.vidscale.sizex, c.vidscale.sizey);
			soc_dpc_set_vid_position(priv->dpc.module, s.position.left, s.position.top, s.position.right, s.position.bottom, false);
			if(priv->dpc.format == FOURCC_YUYV)
				soc_dpc_set_vid_address(priv->dpc.module, priv->pbase, c.vidscale.sizex * 2, priv->pbase+2048, c.vidscale.sizex * 2, priv->pbase+2048+c.vidscale.sizex * 2*c.vidscale.sizey/2, c.vidscale.sizex * 2, 0);
			else
				soc_dpc_set_vid_address(priv->dpc.module, priv->pbase, 4096, priv->pbase+2048, 4096, priv->pbase+2048+4096*c.vidscale.sizey/2, 4096, 0);
			break;

		case LF1000FB_IOCGVIDSCALE:
			if (!(_IOC_DIR(cmd) & _IOC_READ))
				return -EINVAL;
			if (!IS_YUV_LAYER(priv->dpc.layer))
				return -EINVAL;
			soc_dpc_get_vid_format(priv->dpc.module, &format, &c.vidscale.sizex, &c.vidscale.sizey);
			if (copy_to_user(argp, (void *)&c, sizeof(struct lf1000fb_vidscale_cmd)))
				return -EFAULT;
			break;
                /*
		case FBIOGET_VBLANK:
			{
			struct fb_vblank vblank;
			memset(&vblank, 0, sizeof(vblank));
			vblank.flags = FB_VBLANK_HAVE_COUNT;
			vblank.count = soc_dpc_get_vblank(priv->dpc.module);
			if (copy_to_user(argp, (void *)&vblank, sizeof(struct fb_vblank)))
				return -EFAULT;
			}
			break; */

		default:
			return -ENOIOCTLCMD;
	}

	return 0;
}
#endif

static struct fb_ops nxfb_ops = {
	.owner			= THIS_MODULE,
	.fb_check_var	= nxfb_ops_check_var,
	.fb_set_par		= nxfb_ops_set_par,
	.fb_blank		= nxfb_ops_blank,
	.fb_setcolreg	= nxfb_ops_setcolreg,
#if FB_OPS_MMAP
	.fb_mmap		= nxfb_ops_mmap,
#endif
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
	.fb_cursor		= nxfb_ops_cursor,		/* Optional !!! */
	.fb_pan_display	= nxfd_ops_pan_display,
#ifdef CONFIG_FB_NEXELL_LF1000_EXTENSION
	.fb_ioctl		= lf1000fb_ioctl,
#endif
};

/*--------------------------------------------------------------------------------
 * Frame buffer platform_driver functions
 ---------------------------------------------------------------------------------*/
static int nxfb_driver_layer_probe(struct platform_device *pdev, int layer)
{
	struct fb_info    	* info = NULL;
	struct fb_plat_data * plat = pdev->dev.platform_data;
	int ret = 0;
	DBGOUT("%s (name=%s, id=%d, layer=%d)\n", __func__, dev_name(&pdev->dev), pdev->id, layer);

	/*	allocate frame info, private memory */
	info = fb_alloc_info(pdev->id, &pdev->dev);
	if(! info) {
		ret = -ENOMEM;
		goto err_fb;
	}

	/* init private data with platform data */
	fb_init_private(info, plat, layer);

	/* other fb information with private data */
	fb_init_info(info);

	/*	allocate frame buffer memory from here */
#if 1 // share framebuffer memory for RGB layers
	if (layer == 1) {
		struct fb_private *priv = g_fb_private[0];
		info->screen_base	 = priv->vbase;
		info->fix.smem_start = priv->pbase;
		ret = 0;
	}
	else
#endif
		ret = fb_alloc_memory(info);
	if(ret) {
		printk(KERN_ERR "fail, unable to allcate frame buffer (%d)\n", pdev->id);
		goto err_map;
	}

	/*
 	 * 	device_create '/proc/fb0' & fb class
	 * 	register machine file operation to frame buffer file operation
 	 * 	registered_fb[]
 	 * 	(drivers/video/fbmem.c)
 	 */
	ret = register_framebuffer(info);
	if(ret < 0) {
		printk(KERN_ERR "fail, unable to register frame buffer(%d)\n", pdev->id);
		goto err_reg;
	}

	/* register to driver data, use platform_get_drvdata */
	platform_set_drvdata(pdev, info);

	/* set window layer base */
	fb_init_layer(info);

	return ret;

err_reg:
	unregister_framebuffer(info);
err_map:
	fb_free_memory(info);
err_fb:
	fb_exit_info(info);

	return ret;
}

static int __devinit nxfb_driver_probe(struct platform_device *pdev)
{
	int i, r;

	for (i = 0; i < FB_NUM_LAYERS; i++) {
		r = nxfb_driver_layer_probe(pdev, i);
		if (r != 0)
			break;
	}

	return r;
}

static int nxfb_driver_layer_remove(struct platform_device *pdev, int layer)
{
	struct fb_info   * info = platform_get_drvdata(pdev);

	DBGOUT("%s\n", __func__);

	unregister_framebuffer(info);
	fb_free_memory(info);
	fb_exit_info(info);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static int nxfb_driver_remove(struct platform_device *pdev)
{
	int i;

	for (i = 0; i < FB_NUM_LAYERS; i++) {
		nxfb_driver_layer_remove(pdev, i);
	}

	return 0;
}

static int nxfb_driver_suspend(struct platform_device *pdev, pm_message_t state)
{
#if FB_SOC_DEV
	struct fb_info   * info = platform_get_drvdata(pdev);
	struct fb_private* priv = info->par;

	PM_DBGOUT("+%s\n", __func__);

	soc_dpc_set_suspend(priv->dpc.module);

	PM_DBGOUT("-%s\n", __func__);
#endif
	return 0;
}

static int nxfb_driver_resume(struct platform_device *pdev)
{
#if FB_SOC_DEV
	struct fb_info   * info = platform_get_drvdata(pdev);
	struct fb_private* priv = info->par;

	PM_DBGOUT("+%s\n", __func__);

	soc_dpc_set_resume(priv->dpc.module);

	PM_DBGOUT("-%s\n", __func__);
#endif
	return 0;
}

static struct platform_driver fb_plat_driver = {
	.probe		= nxfb_driver_probe,
	.remove		= __devexit_p(nxfb_driver_remove),
	.suspend	= nxfb_driver_suspend,
	.resume		= nxfb_driver_resume,
	.driver		= {
		.name	= FB_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init nxfb_driver_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_register(&fb_plat_driver);
}

static void __exit nxfb_driver_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&fb_plat_driver);
}

module_init(nxfb_driver_init);
module_exit(nxfb_driver_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Framebuffer driver for LF2000");
MODULE_LICENSE("GPL");
