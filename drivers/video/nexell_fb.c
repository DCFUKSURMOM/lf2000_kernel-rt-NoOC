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
#include <linux/major.h>
#include <linux/pm.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>
#include <mach/dpc.h>

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "fb: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*
 * Current support fb: fb0, fb1
 * fb0 = Primary screen
 * fb1 = Secondary screen
 */

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
	uint	vs_upper;
	uint	vs_lower;
	uint	pixelclk;
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
struct fb_private *g_fb_private[2];

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

	#ifdef CONFIG_FB_NEXELL_CLONE
	if (1 == module) {
		struct fb_private *priv_1 = g_fb_private[0];
		int  bpp = priv->dpc.pixelbit/8;
		uint off = (priv_1->dpc.x_res - priv->dpc.x_res)/2 * bpp;
		xres += off;
	}
	#endif

	#if defined(CONFIG_LOGO_NEXELL_COPY)
	{
	u_int srcfb = 0, dstfb = (u_int)priv->vbase;
	u_int hs, vs, size = (xres * yres * pixel);

	soc_dpc_get_rgb_address(module, layer, &srcfb, &hs, &vs);
	if (srcfb) {
		srcfb = (u_int)ioremap(srcfb, size);
		memcpy ((void *)dstfb, (const void *)srcfb, size);
		iounmap((void __iomem *)srcfb);
	}
	}
	#endif

	#if defined(CONFIG_BACKLIGHT_PWM)
	/* generic pwm backlight */
	soc_dpc_set_rgb_address(module, layer, pbase, pixel, xres*pixel, 0);
	#else
	{
	/* backlight depend on board */
	int format = priv->dpc.format;
	soc_dpc_set_out_disable(module);
	soc_dpc_set_rgb_enable(module, layer, 0);
	soc_dpc_set_rgb_format(module, layer, format, xres, yres, pixel);
	soc_dpc_set_rgb_address(module, layer, pbase, pixel, xres*pixel, 0);
	soc_dpc_set_out_enable(module);
	soc_dpc_set_rgb_enable(module, layer, 1);
	}
	#endif

#endif	/* FB_SOC_DEV */

	printk(KERN_INFO "%s: %d * %d - %d bpp (phys=%08x virt=0x%08x size=%d)\n",
		info->fix.id, xres, yres, pixel*8, pbase, (u_int)priv->vbase, priv->length);
}

static void fb_set_layer(struct fb_info *info, int waitvsync)
{
	struct fb_private *priv = info->par;

	int   module = priv->dpc.module;
	int   layer  = priv->dpc.layer;
	u_int pbase  = priv->pbase;

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
	int xres  = priv->dpc.x_res;
	int pixel = priv->dpc.pixelbit/8;

	soc_dpc_set_rgb_address(module, layer, pbase, pixel, xres*pixel, waitvsync);
#endif
}
/*---------------------------------------------------------------------------------------------
 * 	FB locals
 --------------------------------------------------------------------------------------------*/
static int __init fb_alloc_memory(struct fb_info *info)
{
	struct fb_private *priv = info->par;
	unsigned int length;

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
	priv->vbase  = dma_alloc_writecombine(
						priv->device,
						priv->length,
						&priv->pbase,
						GFP_KERNEL);

	if(priv->vbase) {
		memset(priv->vbase, FB_CLEAR_COLOR, priv->length);

		info->screen_base	 = priv->vbase;
		info->fix.smem_start = priv->pbase;
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

static void fb_init_private(struct fb_info *info, struct fb_plat_data * plat)
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
	dpc->layer  	= plat->layer;
	dpc->x_res 		= plat->x_res;
	dpc->y_res 		= plat->y_res;
	dpc->x_virt		= plat->x_res;
	dpc->y_virt		= plat->y_res * plat->buffers;
	dpc->pixelbit 	= plat->pixelbit;
	dpc->format		= plat->format;
	dpc->offset		= plat->buffers == 1 ? 0 : plat->x_res * plat->y_res * plat->pixelbit / 8;
	dpc->w_mm 		= plat->with_mm;
	dpc->h_mm		= plat->height_mm;
	dpc->hs_left	= plat->hs_left;
	dpc->hs_right	= plat->hs_right;
	dpc->vs_upper	= plat->vs_upper;
	dpc->vs_lower	= plat->vs_lower;
	dpc->pixelclk	= plat->pixelclock;

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
	x_virt = priv->dpc.x_res > priv->dpc.x_virt ? priv->dpc.x_res : priv->dpc.x_virt;
	y_virt = priv->dpc.y_res > priv->dpc.y_virt ? priv->dpc.y_res : priv->dpc.y_virt;

	if (! priv->dpc.pixelclk)
		priv->dpc.pixelclk = 27000000;

	/* other variable information */
	info->var.width	    	= priv->dpc.w_mm; 		/* width  mm for dpi */
	info->var.height	    = priv->dpc.h_mm; 		/* height  mm for dpi */
	info->var.left_margin	= priv->dpc.hs_left;
	info->var.right_margin	= priv->dpc.hs_right;
	info->var.upper_margin	= priv->dpc.vs_upper;
	info->var.lower_margin	= priv->dpc.vs_lower;
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
	info->fix.smem_len    	= x_virt * y_virt * (bpp / 8);
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
	info->fix.xpanstep	= 0;
	info->fix.ypanstep	= 1;	/* for fb_pan_display */
	info->fix.ywrapstep	= 0;
	info->fix.accel	    = FB_ACCEL_NONE;

	/* default variable information */
	info->var.nonstd	   	= 0;
	info->var.activate		= FB_ACTIVATE_NOW;
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

static int fb_set_var_pixfmt(struct fb_var_screeninfo *var, struct fb_info *info)
{
	DBGOUT("%s (bpp:%d)\n", __func__, var->bits_per_pixel);

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
			/* 16 bpp, 565 format */
			var->red.length		= 5;
			var->red.offset		= 11;
			var->green.length	= 6;
			var->green.offset	= 5;
			var->blue.length	= 5;
			var->blue.offset	= 0;
			var->transp.length	= 0;
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
			break;
		case 32:
			/* 32 bpp 888 */
			var->red.length		= info->var.red.length;
			var->red.offset		= info->var.red.offset;
			var->green.length	= info->var.green.length;
			var->green.offset	= info->var.green.offset;
			var->blue.length	= info->var.blue.length;
			var->blue.offset	= info->var.blue.offset;
			var->transp.length	= info->var.transp.length;
			var->transp.offset	= info->var.transp.offset;
			break;
		default:
			printk(KERN_ERR "error, not support fb bpp (%d)\n", var->bits_per_pixel);
	}

	return 0;
}

static void fb_dbg_var_info(struct fb_var_screeninfo *var)
{
#if (1)
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
	fb_set_var_pixfmt(var, info);
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
	DBGOUT("%s\n", __func__);

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
	DBGOUT("%s (mode=%d)\n", __func__, blank_mode);

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

	DBGOUT("%s (setcol: regno=%d, r=0x%x, g=0x%x, b=0x%x, t=0x%x)\n",
		__func__, regno, red, green, blue, transp);

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
	unsigned int align  = priv->dpc.x_res * priv->dpc.y_res * priv->dpc.pixelbit/8;

#if (0)
	offset = (var->yoffset * info->fix.line_length) + (var->xoffset * var->bits_per_pixel / 8);
#else
	if (offset % align)
		offset = (offset + align - 1)/align * align;
#endif
	priv->pbase = info->fix.smem_start + offset;

	DBGOUT("%s (x:%4d, y:%4d, offs:0x%08x)\n", __func__, var->xoffset, var->yoffset, offset);

	/* change window layer base */
	if (prebuf != priv->pbase)
		fb_set_layer(info, 1);

	return 0;
}

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
};

/*--------------------------------------------------------------------------------
 * Frame buffer platform_driver functions
 ---------------------------------------------------------------------------------*/
static int __init nxfb_driver_probe(struct platform_device *pdev)
{
	struct fb_info    	* info = NULL;
	struct fb_plat_data * plat = pdev->dev.platform_data;
	int ret = 0;
	DBGOUT("%s (name=%s, id=%d)\n", __func__, dev_name(&pdev->dev), pdev->id);

	/*	allocate frame info, private memory */
	info = fb_alloc_info(pdev->id, &pdev->dev);
	if(! info) {
		ret = -ENOMEM;
		goto err_fb;
	}

	/* init private data with platform data */
	fb_init_private(info, plat);

	/* other fb information with private data */
	fb_init_info(info);

	/*	allocate frame buffer memory from here */
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

static int nxfb_driver_remove(struct platform_device *pdev)
{
	struct fb_info   * info = platform_get_drvdata(pdev);

	DBGOUT("%s\n", __func__);

	unregister_framebuffer(info);
	fb_free_memory(info);
	fb_exit_info(info);

	platform_set_drvdata(pdev, NULL);
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
MODULE_DESCRIPTION("Framebuffer driver for the Nexell");
MODULE_LICENSE("GPL");
