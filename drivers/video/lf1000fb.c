/*
 * drivers/video/lf1000fb.c
 *
 * Framebuffer support for the LF1000
 *
 * Copyright 2012 LeapFrog Enterprises Inc.
 *
 * Parts of this are based on the CL7700 FB driver which is (C) 2009 CoreLogic 
 * Inc. and written by Sungdae Choi.  It's also inspired by pxafb.c.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/fb.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/lf1000/lf1000fb.h>
#include <mach/platform.h>
//#include <mach/screen.h>
#include <linux/lf1000/screen.h>
#include <mach/gpio.h>
#include <asm/uaccess.h>
#include <asm/cputype.h>
//#include <plat/hardware.h>

/* nexell soc headers */
#include <mach/soc.h>

#define DRIVER_NAME			"nx-fb" //"lf1000-fb"
#define RESSIZE(res)		(((res)->end - (res)->start))
#define PALETTE_CLEAR		0x80000000

static struct resource lf1000_fb_resources[] = {
	[0] = {	/* frame buffer memory */
		.start		= 0x8E800000, 	//LF1000_FB_START_ADDR,
		.end		= 0x8FC00000, 	//LF1000_FB_START_ADDR+LF1000_FB_SIZE,
		.flags		= IORESOURCE_MEM,
	},
	[1] = { /* MLC registers */
		.start		= 0xC0008000,	//LF1000_MLC_BASE,
		.end		= 0xC00087CF,	//LF1000_MLC_END,
		.flags		= IORESOURCE_MEM,
	},
#ifdef USE_IRQ
	[2] = { /* DPC VSYNC */
		.start		= LF1000_DPC_IRQ,
		.end		= LF1000_DPC_IRQ,
		.flags		= IORESOURCE_IRQ,
	},
#endif
};

static struct lf1000_screen_info screen	= {
	.name = "ILI6480G2",
	.xres	= 800, //480,
	.yres	= 480, //272,
	.clk_hz = 6400000,
	.hsw	= 2,
	.hfp	= 5,
	.hbp	= 40,
	.vsw	= 1,
	.vfp	= 8,
	.vbp	= 8,
};

struct lf1000_screen_info *lf1000_get_screen_info(void)
{
	return &screen;
}

/*
 * Settings
 */

#define LF1000_FB_NUM_BUFFERS	3	/* buffers per layer */

/* The YUV layer is always last.  The other layers are RGB. */
#define IS_YUV_LAYER(l)		(l->index == l->parent->num_layers-1)

struct lf1000fb_layer {
	u8			index;
	struct lf1000fb_info	*parent;
	struct fb_info		*fbinfo;

	bool			enabled;
	bool			vflip;
	u8			format;
	u32			vstride;
	u8			hstride;

	unsigned int pseudo_pal[16];

	u32			hsrc;
	u32			vsrc;
};

struct lf1000fb_info {
	struct platform_device		*pdev;
	struct resource			*fbres;
	struct resource			*mlcres;
	struct lf1000_screen_info	*screen;

	void __iomem			*fbmem;
	void __iomem			*mlc;
	int 				irq;
	unsigned int			nirq;

	struct fb_info			**fbs;
	unsigned			num_layers;

	struct dentry			*debug;

	bool					prisec;
	void __iomem			*mlcreg;
};

/* controller registers */

#define MLCCONTROLT	0x00
#define MLCSCREENSIZE	0x04
#define MLCBGCOLOR	0x08

#define MLCCLKENB 		0x03C0
#define MLCSECONDARY	0x0400

/* layer registers */

#define RGBREGLEN	0x34

#define RGBLEFTRIGHT	0x00
#define RGBTOPBOTTOM	0x04
#define RGBCONTROL	0x18
#define RGBHSTRIDE	0x1C
#define RGBVSTRIDE	0x20
#define RGBTPCOLOR	0x24
#define RGBADDRESS	0x2C
#define RGBPALETTE	0x30

#define YUVLEFTRIGHT	0x00
#define YUVTOPBOTTOM	0x04
#define YUVCONTROL	0x08
#define YUVVSTRIDE	0x0C
#define YUVTPCOLOR	0x10
#define YUVADDRESS	0x18
#define YUVADDRESSCB	0x1C
#define YUVADDRESSCR	0x20
#define YUVVSTRIDECB	0x24
#define YUVVSTRIDECR	0x28
#define YUVHSCALE	0x2C
#define YUVVSCALE	0x30

static void __iomem *mlc_reg(struct lf1000fb_layer *layer, u32 reg)
{
	return layer->parent->mlcreg + layer->index*RGBREGLEN + reg + 0xC;
}

static void mlc_select(struct lf1000fb_layer *layer, bool secondary)
{
	layer->parent->prisec = secondary;
	layer->parent->mlcreg = (secondary) ? layer->parent->mlc + MLCSECONDARY : layer->parent->mlc;
}

static inline u32 get_fb_size(struct lf1000fb_info *info,
		struct lf1000fb_layer *layer)
{
	if (IS_YUV_LAYER(layer))
		return PAGE_ALIGN(info->screen->yres * 4096 *
				LF1000_FB_NUM_BUFFERS);

	/* at a minimum, support 640x480 video */
	if (info->screen->xres < 640 || info->screen->yres < 480)
		return PAGE_ALIGN(640*480*4*LF1000_FB_NUM_BUFFERS);
	return PAGE_ALIGN(info->screen->xres * info->screen->yres * 4 *
			LF1000_FB_NUM_BUFFERS);
}

/* debugfs */

static void layer_show_reg(struct seq_file *s, const char *nm, u32 reg)
{
	struct lf1000fb_layer *layer = s->private;

	seq_printf(s, "%10s:\t0x%08X\n", nm, readl(mlc_reg(layer, reg)));
}

static int lf1000_layer_regs_show(struct seq_file *s, void *v)
{
	struct lf1000fb_layer *layer = s->private;
	int n;

	for (n = 0; n < 2; n++)
	{
		mlc_select(layer, n);

		if (IS_YUV_LAYER(layer)) {
			layer_show_reg(s, "LEFTRIGHT", YUVLEFTRIGHT);
			layer_show_reg(s, "TOPBOTTOM", YUVTOPBOTTOM);
			layer_show_reg(s, "CONTROL", YUVCONTROL);
			layer_show_reg(s, "VSTRIDE", YUVVSTRIDE);
			layer_show_reg(s, "TPCOLOR", YUVTPCOLOR);
			layer_show_reg(s, "ADDRESS", YUVADDRESS);
			layer_show_reg(s, "ADDRESSCB", YUVADDRESSCB);
			layer_show_reg(s, "ADDRESSCR", YUVADDRESSCR);
			layer_show_reg(s, "VSTRIDECB", YUVVSTRIDECB);
			layer_show_reg(s, "VSTRIDECR", YUVVSTRIDECR);
			layer_show_reg(s, "HSCALE", YUVHSCALE);
			layer_show_reg(s, "VSCALE", YUVVSCALE);
		} else {
			layer_show_reg(s, "LEFTRIGHT", RGBLEFTRIGHT);
			layer_show_reg(s, "TOPBOTTOM", RGBTOPBOTTOM);
			layer_show_reg(s, "CONTROL", RGBCONTROL);
			layer_show_reg(s, "HSTRIDE", RGBHSTRIDE);
			layer_show_reg(s, "VSTRIDE", RGBVSTRIDE);
			layer_show_reg(s, "TPCOLOR", RGBTPCOLOR);
			layer_show_reg(s, "ADDRESS", RGBADDRESS);
			layer_show_reg(s, "PALETTE", RGBPALETTE);
		}
	}
	mlc_select(layer, 0);

	return 0;
}

static int lf1000_layer_regs_open(struct inode *inode, struct file *file)
{
	return single_open(file, lf1000_layer_regs_show, inode->i_private);
}

static const struct file_operations lf1000_layer_regs_fops = {
	.owner		= THIS_MODULE,
	.open		= lf1000_layer_regs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void mlc_show_reg(struct seq_file *s, const char *nm, u32 reg)
{
	struct lf1000fb_info *info = s->private;
	void __iomem *addr = info->mlc + reg;

	seq_printf(s, "%10s:\t0x%08X\n", nm, readl(addr));
}

static int lf1000_mlc_regs_show(struct seq_file *s, void *v)
{
	mlc_show_reg(s, "CONTROLT", MLCCONTROLT);
	mlc_show_reg(s, "SCREENSIZE", MLCSCREENSIZE);
	mlc_show_reg(s, "BGCOLOR", MLCBGCOLOR);

	return 0;
}

static int lf1000_mlc_regs_open(struct inode *inode, struct file *file)
{
	return single_open(file, lf1000_mlc_regs_show, inode->i_private);
}

static const struct file_operations lf1000_mlc_regs_fops = {
	.owner		= THIS_MODULE,
	.open		= lf1000_mlc_regs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/* hardware routines */

static void mlc_set_dirty(struct lf1000fb_layer *fbi)
{
	void __iomem *control;
	u32 tmp;

	if (IS_YUV_LAYER(fbi))
		control = mlc_reg(fbi, YUVCONTROL);
	else
		control = mlc_reg(fbi, RGBCONTROL);

	tmp = readl(control);
	writel(tmp | (1<<4), control);
}

static bool mlc_is_dirty(struct lf1000fb_layer *fbi)
{
	void __iomem *control;
	
	if (IS_YUV_LAYER(fbi))
		control = mlc_reg(fbi, YUVCONTROL);
	else
		control = mlc_reg(fbi, RGBCONTROL);

	return !!(readl(control) & (1<<4));
}

static u32 linear_to_xy(u32 addr)
{
	u32 segment = addr >> 24;
	u32 offset = addr & 0xFFFFFF;
	u32 y = offset >> 12;
	u32 x = offset & 0xFFF;

	return 0x20000000 | (segment << 24) | (y << 12) | (x << 0);
}

static void mlc_set_address(struct lf1000fb_layer *fbi, u32 address)
{
	if (fbi->index >= fbi->parent->num_layers)
		return;

	if (IS_YUV_LAYER(fbi)) {
		if (NONSTD_TO_PFOR(fbi->fbinfo->var.nonstd) ==
				LAYER_FORMAT_YUV420) {
			const u32 pitch = 4096;

			writel(linear_to_xy(address),
					mlc_reg(fbi, YUVADDRESS));
			writel(linear_to_xy(address + pitch/2),
					mlc_reg(fbi, YUVADDRESSCB));
			writel(linear_to_xy(address + pitch/2 +
						pitch*fbi->fbinfo->var.yres/2),
					mlc_reg(fbi, YUVADDRESSCR));
		}
	} else {
		writel(address, mlc_reg(fbi, RGBADDRESS));
	}
}

static void set_invert(struct lf1000fb_layer *fbi, u8 en)
{
	u32 val;

	val = readl(mlc_reg(fbi, RGBCONTROL));
	if (en)
		val |= (1<<1);
	else
		val &= ~(1<<1);

	writel(val, mlc_reg(fbi, RGBCONTROL));
}

static void set_blend(struct lf1000fb_layer *fbi, u8 en, u8 alpha)
{
	void __iomem *control;
	void __iomem *tpcolor;
	u32 val;

	if (IS_YUV_LAYER(fbi)) {
		control = mlc_reg(fbi, YUVCONTROL);
		tpcolor = mlc_reg(fbi, YUVTPCOLOR);
	} else {
		control = mlc_reg(fbi, RGBCONTROL);
		tpcolor = mlc_reg(fbi, RGBTPCOLOR);
	}

	val = readl(tpcolor) & ~(0xF<<28);
	writel(val | ((alpha & 0xF)<<28), tpcolor);

	val = readl(control);
	if (en)
		writel(val | (1<<2), control);
	else
		writel(val & ~(1<<2), control);

	mlc_set_dirty(fbi);
}

static void get_blend(struct lf1000fb_layer *fbi, u8 *en, u8 *alpha)
{
	void __iomem *control;
	void __iomem *tpcolor;

	if (IS_YUV_LAYER(fbi)) {
		control = mlc_reg(fbi, YUVCONTROL);
		tpcolor = mlc_reg(fbi, YUVTPCOLOR);
	} else {
		control = mlc_reg(fbi, RGBCONTROL);
		tpcolor = mlc_reg(fbi, RGBTPCOLOR);
	}

	*alpha = (readl(tpcolor)>>28) & 0xF;
	*en = !!(readl(control) & (1<<2));
}

static void mlc_set_locksize(struct lf1000fb_layer *fbi, u8 size)
{
	void __iomem *control;
	u32 val;

	if (IS_YUV_LAYER(fbi))
		control = mlc_reg(fbi, YUVCONTROL);
	else
		control = mlc_reg(fbi, RGBCONTROL);

	val = readl(control) & ~(3<<12);
	writel(val | (size & 3)<<12, control);
}

/* Sets the position ("priority") of the YUV layer.  Position 0 places the
 * layer on the top of the "stack". */
static u8 mlc_set_yuv_position(struct lf1000fb_layer *fbi, u8 pos)
{
	void __iomem *mlc = fbi->parent->mlcreg;
	u32 tmp;

	if (!IS_YUV_LAYER(fbi))
		return pos;

	if (pos > fbi->parent->num_layers - 1)
		pos = fbi->parent->num_layers - 1;

	tmp = readl(mlc + MLCCONTROLT) & ~(0x3<<8);
	writel(tmp|(1<<3)|((pos & 0x3)<<8), mlc + MLCCONTROLT);

	return pos;
}

/* FB operations */

static void mlc_set_rgb_format(struct fb_info *fbinfo, u16 format)
{
	struct lf1000fb_layer *fbi = fbinfo->par;
	struct fb_var_screeninfo *var = &fbinfo->var;
	u8 i = fbi->index;
	u32 tmp;
	u8 bpp = var->bits_per_pixel>>3;
	u32 hstride = bpp;
	u32 vstride = bpp*var->xres;

	if (IS_YUV_LAYER(fbi))
		return;

	if (NONSTD_TO_PLANAR(var->nonstd))
		vstride = 4096;
	else
		vstride = bpp*var->xres;

	dev_dbg(&fbi->parent->pdev->dev,
			"layer%d: setting BPP=%d, format=0x%X\n", i,
			var->bits_per_pixel, format);

	fbi->vstride = vstride;
	fbi->hstride = hstride;

	if (fbi->vflip) {
		vstride = 0 - vstride;
		hstride = 0 - hstride;
	}
	writel(hstride, mlc_reg(fbi, RGBHSTRIDE));
	writel(vstride, mlc_reg(fbi, RGBVSTRIDE));

	tmp = readl(mlc_reg(fbi, RGBCONTROL)) & ~(0xFFFF<<16);
	writel(tmp | (format<<16), mlc_reg(fbi, RGBCONTROL));

	fbi->format = LAYER_FORMAT_RGB;
}

static void mlc_set_yuv_format(struct lf1000fb_layer *layer, u16 format,
		u8 bpp, u16 width)
{
	u32 tmp;
	u32 stride = bpp*width;

	if (!IS_YUV_LAYER(layer))
		return;

	layer->vstride = stride;
	layer->hstride = bpp;

	writel(stride, mlc_reg(layer, YUVVSTRIDE));
	writel(stride, mlc_reg(layer, YUVVSTRIDECB));
	writel(stride, mlc_reg(layer, YUVVSTRIDECR));

	tmp = readl(mlc_reg(layer, YUVCONTROL));
       	tmp &= ~(0xFFFF<<16);
	writel(tmp | (format<<16), mlc_reg(layer, YUVCONTROL));
}

#define LF1000_FB_FORMAT_YUV420		0x0000
#define LF1000_FB_FORMAT_YUV422		0x0002

#define LF1000_FB_FORMAT_R5G6B5		0x4432
#define LF1000_FB_FORMAT_B5G6R5		0xC432
#define LF1000_FB_FORMAT_X1R5G5B5	0x4342
#define LF1000_FB_FORMAT_X1B5G5R5	0xC342
#define LF1000_FB_FORMAT_X4R4G4B4	0x4211
#define LF1000_FB_FORMAT_X4B4G4R4	0xC211
#define LF1000_FB_FORMAT_X8R3G3B2	0x4120
#define LF1000_FB_FORMAT_X8B3G3R2	0xC120
#define LF1000_FB_FORMAT_A1R5G5B5	0x3342
#define LF1000_FB_FORMAT_A1B5G5R5	0xB342
#define LF1000_FB_FORMAT_A4R4G4B4	0x2211
#define LF1000_FB_FORMAT_A4B4G4R4 	0xA211
#define LF1000_FB_FORMAT_A8R3G3B2	0x1120
#define LF1000_FB_FORMAT_A8B3G3R2	0x9120
#define LF1000_FB_FORMAT_R8G8B8		0x4653
#define LF1000_FB_FORMAT_B8G8R8		0xC653
#define LF1000_FB_FORMAT_X8R8G8B8	0x4653
#define LF1000_FB_FORMAT_X8B8G8R8 	0xC653
#define LF1000_FB_FORMAT_A8R8G8B8	0x0653
#define LF1000_FB_FORMAT_A8B8G8R8	0x8653
#define LF1000_FB_FORMAT_PTR5G6B5	0x443A

static int set_yuv(struct fb_info *fbi, struct fb_var_screeninfo *var,
		bool apply)
{
	struct lf1000fb_layer *layer = fbi->par;
	u8 pf = NONSTD_TO_PFOR(var->nonstd);

	switch (pf) {
		case LAYER_FORMAT_YUV420:
			if (apply) {
				mlc_set_yuv_format(layer,
						LF1000_FB_FORMAT_YUV420,
						1, 4096);
				layer->format = pf;
			}
			break;
		case LAYER_FORMAT_YUV422:
			if (apply) {
				mlc_set_yuv_format(layer,
						LF1000_FB_FORMAT_YUV422,
						2, fbi->var.xres);
				layer->format = pf;
			}
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

static u16 set_rgb332(struct fb_var_screeninfo *var)
{
	var->red.length = 3;
	var->green.length = 3;
	var->blue.length = 2;

	var->red.offset = 5;
	var->green.offset = 2;
	var->blue.offset = 0;

	if (var->transp.length == 0)
		return LF1000_FB_FORMAT_X8R3G3B2;

	var->transp.length = 8;
	var->transp.offset = 8;

	return LF1000_FB_FORMAT_A8R3G3B2;
}

static u16 set_bgr332(struct fb_var_screeninfo *var)
{
	var->red.length = 2;
	var->green.length = 3;
	var->blue.length = 3;

	var->red.offset = 0;
	var->green.offset = 2;
	var->blue.offset = 5;

	if (var->transp.length == 0)
		return LF1000_FB_FORMAT_X8B3G3R2;

	var->transp.length = 8;
	var->transp.offset = 8;

	return LF1000_FB_FORMAT_A8B3G3R2;
}

static u16 set_rgb444(struct fb_var_screeninfo *var)
{
	var->red.length = 4;
	var->green.length = 4;
	var->blue.length = 4;

	var->red.offset = 8;
	var->green.offset = 4;
	var->blue.offset = 0;

	if (var->transp.length == 0)
		return LF1000_FB_FORMAT_X4R4G4B4;

	var->transp.length = 4;
	var->transp.offset = 12;

	return LF1000_FB_FORMAT_A4R4G4B4;
}

static u16 set_bgr444(struct fb_var_screeninfo *var)
{
	var->red.length = 4;
	var->green.length = 4;
	var->blue.length = 4;

	var->red.offset = 0;
	var->green.offset = 4;
	var->blue.offset = 8;

	if (var->transp.length == 0)
		return LF1000_FB_FORMAT_X4B4G4R4;

	var->transp.length = 4;
	var->transp.offset = 12;

	return LF1000_FB_FORMAT_A4B4G4R4;
}

static u16 set_rgb555(struct fb_var_screeninfo *var)
{
	var->red.length = 5;
	var->green.length = 5;
	var->blue.length = 5;

	var->red.offset = 10;
	var->green.offset = 5;
	var->blue.offset = 0;

	if (var->transp.length == 0)
		return LF1000_FB_FORMAT_X1R5G5B5;

	var->transp.length = 1;
	var->transp.offset = 15;

	return LF1000_FB_FORMAT_A1R5G5B5;
}

static u16 set_bgr555(struct fb_var_screeninfo *var)
{
	var->red.length = 5;
	var->green.length = 5;
	var->blue.length = 5;

	var->red.offset = 0;
	var->green.offset = 5;
	var->blue.offset = 10;

	if (var->transp.length == 0)
		return LF1000_FB_FORMAT_X1B5G5R5;

	var->transp.length = 1;
	var->transp.offset = 15;

	return LF1000_FB_FORMAT_A1B5G5R5;
}

static u16 set_rgb565(struct fb_var_screeninfo *var)
{
	var->red.length = 5;
	var->green.length = 6;
	var->blue.length = 5;

	var->red.offset = 11;
	var->green.offset = 5;
	var->blue.offset = 0;

	var->transp.length = 0;
	var->transp.offset = 0;

	return LF1000_FB_FORMAT_R5G6B5;
}

static u16 set_bgr565(struct fb_var_screeninfo *var)
{
	var->red.length = 5;
	var->green.length = 6;
	var->blue.length = 5;

	var->red.offset = 0;
	var->green.offset = 5;
	var->blue.offset = 11;

	var->transp.length = 0;
	var->transp.offset = 0;

	return LF1000_FB_FORMAT_B5G6R5;
}

static u16 set_rgb888(struct fb_var_screeninfo *var)
{
	var->red.length = 8;
	var->green.length = 8;
	var->blue.length = 8;

	var->red.offset = 16;
	var->green.offset = 8;
	var->blue.offset = 0;

	if (var->bits_per_pixel == 32) {
		if (var->transp.length == 0)
			return LF1000_FB_FORMAT_X8R8G8B8;

		var->transp.length = 8;
		var->transp.offset = 24;
		return LF1000_FB_FORMAT_A8R8G8B8;
	}

	var->transp.length = 0;
	var->transp.offset = 0;

	return LF1000_FB_FORMAT_R8G8B8;
}

static u16 set_bgr888(struct fb_var_screeninfo *var)
{
	var->red.length = 8;
	var->green.length = 8;
	var->blue.length = 8;

	var->red.offset = 0;
	var->green.offset = 8;
	var->blue.offset = 16;

	if (var->bits_per_pixel == 32) {
		if (var->transp.length == 0)
			return LF1000_FB_FORMAT_X8B8G8R8;
		
		var->transp.length = 8;
		var->transp.offset = 24;
		return LF1000_FB_FORMAT_A8B8G8R8;
	}

	var->transp.length = 0;
	var->transp.offset = 0;

	return LF1000_FB_FORMAT_B8G8R8;
}

static int set_rgb(struct fb_info *fbi, struct fb_var_screeninfo *var,
		bool apply)
{
	struct lf1000fb_layer *layer = fbi->par;
	struct platform_device *pdev = layer->parent->pdev;
	u16 format = 0;
	u32 visual = FB_VISUAL_PSEUDOCOLOR;

	switch (var->bits_per_pixel) {
		case 1:
		case 2:
		case 4:
		case 8:
			var->red.length = var->bits_per_pixel;
			var->green.length = var->bits_per_pixel;
			var->blue.length = var->bits_per_pixel;

			format = LF1000_FB_FORMAT_PTR5G6B5;
			break;

		case 16:
			if (var->blue.offset == 0 &&
			    var->green.offset == 5 &&
			    var->red.offset == 11) {
				format = set_rgb565(var);
			} else if (var->red.offset == 0 &&
				   var->green.offset == 5 &&
				   var->blue.offset == 11) {
				format = set_bgr565(var);
			} else if (var->red.offset == 8 &&
				   var->green.offset == 4 &&
				   var->blue.offset == 0) {
				format = set_rgb444(var);
			} else if (var->red.offset == 0 &&
				   var->green.offset == 4 &&
				   var->blue.offset == 8) {
				format = set_bgr444(var);
			} else if (var->red.offset == 5 &&
				   var->green.offset == 2 &&
				   var->blue.offset == 0) {
				format = set_rgb332(var);
			} else if (var->blue.offset == 5 &&
				   var->green.offset == 2 &&
				   var->red.offset == 0) {
				format = set_bgr332(var);
			} else if (var->blue.offset == 0 &&
				   var->green.offset == 5 &&
				   var->red.offset == 10) {
				format = set_rgb555(var);
			} else if (var->blue.offset == 10 &&
				   var->green.offset == 5 &&
				   var->red.offset == 0) {
				format = set_bgr555(var);
			} else {
				dev_info(&pdev->dev, "%s guessing RGB565\n",
						__FUNCTION__);
				format = set_rgb565(var);
			}
			break;

		case 24:
			if (var->blue.offset == 0 &&
			    var->green.offset == 8 &&
			    var->red.offset == 16) {
				format = set_rgb888(var); 
			} else if (var->red.offset == 0 &&
				   var->green.offset == 8 &&
				   var->blue.offset == 16) {
				format = set_bgr888(var);
			} else {
				dev_info(&pdev->dev, "%s guessing RGB888\n",
						__FUNCTION__);
				format = set_rgb888(var);
			}

			visual = FB_VISUAL_TRUECOLOR;
			break;

		case 32:
			if (var->red.offset == 16 &&
			    var->green.offset == 8 &&
			    var->blue.offset == 0) {
				format = set_rgb888(var);
			} else if (var->red.offset == 0 &&
				   var->green.offset == 8 &&
				   var->blue.offset == 16) {
				format = set_bgr888(var);
			} else {
				dev_info(&pdev->dev, "%s guessing %cRGB888\n",
					__FUNCTION__,
					var->transp.length ? 'A' : 'X');
				format = set_rgb888(var);
			}

			visual = FB_VISUAL_TRUECOLOR;
			break;

		default:
			return -EINVAL;
	}	

	if (apply) {
		mlc_set_rgb_format(fbi, format);
		fbi->fix.visual = visual;
	}

	return 0;
}

/* The YUV layer has a scaler so we can configure it for desired resolutions */
static int set_video_scaler(struct lf1000fb_layer *fbi, u32 xres, u32 yres,
		bool apply)
{
	struct fb_var_screeninfo *var = &fbi->fbinfo->var;
	u32 hscale, vscale, hround, vround;
	
	if (!IS_YUV_LAYER(fbi))
		return -EINVAL;

	fbi->hsrc = xres;
	fbi->vsrc = yres;

	/* Enable adjusted ratio with bilinear filter for upscaling or just
	 * downscale.  The height scales independant of the width. 
	 * Use rounding of scaler ratio to support get_video_scaler. */

	if (xres < var->xres) {
		hscale = ((xres-1)<<12)/(var->xres-1);
		hround = hscale & 1;
		hscale >>= 1;
		hscale += hround;
		writel((1<<28) | hscale, mlc_reg(fbi, YUVHSCALE));
	} else {
		if (xres > var->xres_virtual/2)
			return -EINVAL;
		writel((xres<<11)/var->xres, mlc_reg(fbi, YUVHSCALE));
	}

	if (yres < var->yres) {
		vscale = ((yres-1)<<12)/(var->yres-1);
		vround = vscale & 1;
		vscale >>= 1;
		vscale += vround;
		writel((1<<28) | vscale, mlc_reg(fbi, YUVVSCALE));
	} else {
		if (yres > var->yres_virtual)
			return -EINVAL;
		writel((yres<<11)/var->yres, mlc_reg(fbi, YUVVSCALE));
	}

	if (apply)
		mlc_set_dirty(fbi);
	return 0;
}

static int get_video_scaler(struct lf1000fb_layer *fbi, u32 *xres, u32 *yres)
{
	struct fb_var_screeninfo *var = &fbi->fbinfo->var;
	u32 val;

	if (!IS_YUV_LAYER(fbi))
		return -EINVAL;
	
	val = readl(mlc_reg(fbi, YUVHSCALE));
	if (val & (1<<28))
		*xres = (((val & ~(1<<28)) * (var->xres - 1))>>11) + 1;
	else
		*xres = ((val * var->xres) >> 11);
		
	val = readl(mlc_reg(fbi, YUVVSCALE));
	if (val & (1<<28))
		*yres = (((val & ~(1<<28)) * (var->yres - 1))>>11) + 1;
	else
		*yres = ((val * var->yres) >> 11);

	return 0;
}

static void schedule_palette_update(struct lf1000fb_layer *fbi,
		unsigned int regno, unsigned int val)
{
	if (IS_YUV_LAYER(fbi))
		return;

	writel((regno<<24)|((u16)val), mlc_reg(fbi, RGBPALETTE));
	mlc_set_dirty(fbi);
}

static unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	return ((chan & 0xffff) >> (16 - bf->length)) << bf->offset;
}

static int lf1000fb_setcolreg(unsigned regno, unsigned red, unsigned green,
		unsigned blue, unsigned transp, struct fb_info *info)
{
	struct lf1000fb_layer *fbi = info->par;

	if (fbi->index > fbi->parent->num_layers)
		return -EINVAL;

	if (info->fix.visual == FB_VISUAL_TRUECOLOR && regno < 16) {
		u16 val;
		val = chan_to_field(red, &info->var.red);
		val |= chan_to_field(green, &info->var.green);
		val |= chan_to_field(blue, &info->var.blue);
		
		fbi->pseudo_pal[regno] = val;

		return 0;
	}
	
	if (info->fix.visual == FB_VISUAL_PSEUDOCOLOR && regno < 256) {
		u32 col = 0;

		/* pack R,G,B values into RGB565 palette format */
		col |= (red >> (8+3)) << 11;
		col |= (green >> (8+2)) << 5;
		col |= (blue >> (8+3)) << 0;
		
		schedule_palette_update(fbi, regno, col);

		return 0;
	}
	
	return -EINVAL;
}

static int lf1000fb_check_var(struct fb_var_screeninfo *var,
		struct fb_info *info)
{
	struct lf1000fb_layer *layer = info->par;
	u8 format;

	if (layer->index >= layer->parent->num_layers)
		return -EINVAL;

	/* YUV formats are only allowed on the YUV layer */
	format = NONSTD_TO_PFOR(var->nonstd);
	if ((format == LAYER_FORMAT_YUV420 || format == LAYER_FORMAT_YUV422) &&
			!IS_YUV_LAYER(layer))
		return -EINVAL;

	/* a hardware bug prevents us from supporting rotation/flipping in
	 * other than 32-bit mode */
	if (var->rotate && var->bits_per_pixel != 32)
		return -EINVAL;

	if (IS_YUV_LAYER(layer))
		return set_yuv(info, var, 0);
	else
		return set_rgb(info, var, 0);
}

static void set_position(struct lf1000fb_layer *fbi, s32 left,
		s32 top, s32 right, s32 bottom, bool apply)
{
	writel((((left & 0xFFF)<<16) | ((right - 1) & 0xFFF)),
			mlc_reg(fbi, RGBLEFTRIGHT));

	writel((((top & 0xFFF)<<16) | ((bottom - 1) & 0xFFF)),
			mlc_reg(fbi, RGBTOPBOTTOM));

	if (apply)
		mlc_set_dirty(fbi);
}

static void get_position(struct lf1000fb_layer *fbi, s32 *left,
		s32 *top, s32 *right, s32 *bottom)
{
	u32 val;

	val = readl(mlc_reg(fbi, RGBLEFTRIGHT));
	*left = (val>>16) & 0xFFF;
	*right = (val & 0xFFF) + 1;

	val = readl(mlc_reg(fbi, RGBTOPBOTTOM));
	*top = (val>>16) & 0xFFF;
	*bottom = (val & 0xFFF) + 1;
}

static void set_resolution(struct lf1000fb_layer *fbi)
{
	s32 left, right, top, bottom;
	struct fb_var_screeninfo *var = &fbi->fbinfo->var;

	get_position(fbi, &left, &top, &right, &bottom);
	set_position(fbi, left, top, left + var->xres, top + var->yres, 0);
}

static void lf1000fb_set_vflip(struct lf1000fb_layer *layer, bool flip)
{
	u32 addr;

	if (layer->vflip == flip)
		return;

	addr = readl(mlc_reg(layer, RGBADDRESS));
	
	if (flip) { /* normal to flipped */
		writel(addr + layer->vstride * (layer->fbinfo->var.yres),
				mlc_reg(layer, RGBADDRESS));
		writel(0 - layer->vstride, mlc_reg(layer, RGBVSTRIDE));
		set_invert(layer, 1);
	} else { /* flipped to normal */
		writel(addr - layer->vstride * (layer->fbinfo->var.yres),
				mlc_reg(layer, RGBADDRESS));
		writel(layer->vstride, mlc_reg(layer, RGBVSTRIDE));
		set_invert(layer, 0);
	}

	layer->vflip = flip;
}

static void lf1000fb_rotate(struct fb_info *info, int rotate)
{
	struct lf1000fb_layer *layer = info->par;

	if (IS_YUV_LAYER(layer) || layer->index >= layer->parent->num_layers)
		return;

	switch (rotate) {
		case 0:
			lf1000fb_set_vflip(layer, 0);
			layer->fbinfo->var.rotate = rotate;
			break;
		case 180:
			lf1000fb_set_vflip(layer, 1);
			layer->fbinfo->var.rotate = rotate;
			break;
	}
}

static void lf1000fb_set_type(struct lf1000fb_layer *layer, bool planar)
{
	struct fb_info *info = layer->fbinfo;
	struct fb_var_screeninfo *var = &info->var;
	u32 addr = readl(mlc_reg(layer, RGBADDRESS));

	if (planar) {
		info->fix.line_length = 4096;
		info->fix.type = FB_TYPE_PLANES;
		info->fix.smem_start |= 0x20000000;
		mlc_set_address(layer, addr | 0x20000000);
	} else {
		info->fix.type = FB_TYPE_PACKED_PIXELS;
		info->fix.smem_start &= ~0x20000000;
		writel(addr & ~0x20000000, mlc_reg(layer, RGBADDRESS));
		info->fix.line_length = var->xres * (var->bits_per_pixel>>3);
	}
}

static int lf1000fb_set_par(struct fb_info *info)
{
	struct lf1000fb_layer *layer = info->par;
	struct fb_var_screeninfo *var = &info->var;
	int ret = 0;

	if (layer->index >= layer->parent->num_layers)
		return -EINVAL;

	set_resolution(layer);

	/* a hardware bug prevents us from supporting rotation/flipping in
	 * other than 32-bit mode */
	if (var->bits_per_pixel == 32)
		lf1000fb_rotate(info, var->rotate);
	
	if (IS_YUV_LAYER(layer)) {
		info->fix.line_length = 4096;
		ret = set_yuv(info, var, 1);
	} else {
		ret = set_rgb(info, var, 1);
	}

	if (ret)
		goto out_set;

	if (IS_YUV_LAYER(layer)) {
		u8 pos;
		
		pos = mlc_set_yuv_position(layer, NONSTD_TO_POS(var->nonstd));
		var->nonstd &=
			~(LF1000_NONSTD_PRIORITY_MASK<<LF1000_NONSTD_PRIORITY);
		var->nonstd |= (pos<<LF1000_NONSTD_PRIORITY);
	} else {
		lf1000fb_set_type(layer, NONSTD_TO_PLANAR(var->nonstd));
		var->nonstd &=
			~(LF1000_NONSTD_PRIORITY_MASK<<LF1000_NONSTD_PRIORITY);
	}

#ifdef USE_TVOUT
	/* clone secondary MLC for TV out */
	if (gpio_have_tvout()) {
		mlc_select(layer, 1);
		set_resolution(layer);
		if (IS_YUV_LAYER(layer))
			set_yuv(info, var, 1);
		else
			set_rgb(info, var, 1);
		if (IS_YUV_LAYER(layer))
			mlc_set_yuv_position(layer, NONSTD_TO_POS(var->nonstd));
		else
			lf1000fb_set_type(layer, NONSTD_TO_PLANAR(var->nonstd));
		mlc_set_dirty(layer);
		mlc_select(layer, 0);
	}
#endif

out_set:
	mlc_set_dirty(layer);
	return ret;
}

static int lf1000fb_blank(int blank, struct fb_info *info)
{
	struct lf1000fb_layer *layer = info->par;
	void __iomem *control;
	u32 reg;
	
	if (layer->index >= layer->parent->num_layers)
		return -EINVAL;

	if (IS_YUV_LAYER(layer))
		control = mlc_reg(layer, YUVCONTROL);
	else
		control = mlc_reg(layer, RGBCONTROL);

	reg = readl(control);

	switch (blank) {
		case FB_BLANK_NORMAL:
			/* make MLC background color black */
			writel(0x000000, layer->parent->mlcreg + MLCBGCOLOR);
			writel((1<<3) | readl(layer->parent->mlcreg + MLCCONTROLT), layer->parent->mlcreg + MLCCONTROLT);
		case FB_BLANK_VSYNC_SUSPEND:
		case FB_BLANK_HSYNC_SUSPEND:
		case FB_BLANK_POWERDOWN:
			/* disable */
			reg &= ~(1<<5);
			reg |= (1<<4);
			writel(reg, control);
			do {
				reg = readl(control);
			} while (reg & (1<<4));

			/* enable sleep */
			reg &= ~(1<<14);
			writel(reg, control);

			/* palette off */
			reg &= ~(1<<15);
			writel(reg, control);

			layer->enabled = 0;
			mlc_set_dirty(layer);
			break;

		case FB_BLANK_UNBLANK:
			/* palette on */
			reg |= (1<<15);
			writel(reg, control);

			/* disable sleep */
			reg |= (1<<14);
			writel(reg, control);

			/* enable */
			reg |= (1<<5);
			writel(reg, control);
			
			layer->enabled = 1;
			mlc_set_dirty(layer);
			break;
	}

#ifdef USE_TVOUT
	/* clone secondary MLC for TV out */
	if (gpio_have_tvout()) {
		mlc_select(layer, 1);
		writel(reg, control + MLCSECONDARY);
		mlc_set_dirty(layer);
		mlc_select(layer, 0);
	}
#endif

	return 0;
}

static int lf1000fb_pan_display(struct fb_var_screeninfo *var,
		struct fb_info *info)
{
	struct lf1000fb_layer *layer = info->par;
	u32 address;
	u32 offset;

	if (layer->index >= layer->parent->num_layers)
		return -EINVAL;
	
	address = layer->fbinfo->fix.smem_start;

 	if (IS_YUV_LAYER(layer)) {
		offset = var->xoffset + layer->vstride * var->yoffset;
	} else {	
		offset = var->xoffset + layer->vstride * var->yoffset;
		if (layer->vflip)
			address += layer->vstride * (layer->fbinfo->var.yres);
	}

	if (address >= layer->fbinfo->fix.smem_start +
			layer->fbinfo->fix.smem_len)
		return -EINVAL;

	layer->fbinfo->var.xoffset = var->xoffset;
	layer->fbinfo->var.yoffset = var->yoffset;
	mlc_set_address(layer, address+offset);
	mlc_set_dirty(layer);

#ifdef USE_TVOUT
	/* clone secondary MLC for TV out */
	if (gpio_have_tvout()) {
		mlc_select(layer, 1);
		mlc_set_address(layer, address+offset);
		mlc_set_dirty(layer);
		mlc_select(layer, 0);
	}
#endif

	return 0;
}

/* This SoC has a few few features that don't fit into the standard FB API so
 * we extend that here as needed. */
static int lf1000fb_ioctl(struct fb_info *info, unsigned int cmd,
		unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	union lf1000fb_cmd c;
	struct lf1000fb_layer *fbi = info->par;

	if (fbi->index >= fbi->parent->num_layers)
		return -EINVAL;

	switch (cmd) {
		case FBIOGET_VBLANK:
			{
			struct fb_vblank vblank;
			struct lf1000fb_info *fb_info = fbi->parent;
			vblank.flags = FB_VBLANK_HAVE_COUNT;
			vblank.count = fb_info->nirq;
			vblank.hcount = 0;
			vblank.vcount = 0;
			if (copy_to_user(argp, (void *)&vblank,
					sizeof(struct fb_vblank)))
				return -EFAULT;
			}
			break;

		case FBIO_WAITFORVSYNC:
#ifdef USE_TVOUT
			if (gpio_have_tvout()) {
				int r;
				mlc_select(fbi, 1);
				r = mlc_is_dirty(fbi);
				mlc_select(fbi, 0);
				return r;
			}
#endif
			return mlc_is_dirty(fbi);

		case LF1000FB_IOCSALPHA:
			if (!(_IOC_DIR(cmd) & _IOC_WRITE))
				return -EFAULT;
			if (copy_from_user((void *)&c, argp,
					sizeof(struct lf1000fb_blend_cmd)))
				return -EFAULT;
			set_blend(fbi, c.blend.enable, c.blend.alpha);
#ifdef USE_TVOUT
			if (gpio_have_tvout()) {
				mlc_select(fbi, 1);
				set_blend(fbi, c.blend.enable, c.blend.alpha);
				mlc_select(fbi, 0);
			}
#endif
			break;

		case LF1000FB_IOCGALPHA:
			if (!(_IOC_DIR(cmd) & _IOC_READ))
				return -EFAULT;
			get_blend(fbi, &c.blend.enable, &c.blend.alpha);
			if (copy_to_user(argp, (void *)&c,
					sizeof(struct lf1000fb_blend_cmd)))
				return -EFAULT;
			break;

		case LF1000FB_IOCSPOSTION:
			if (!(_IOC_DIR(cmd) & _IOC_WRITE))
				return -EINVAL;
			if (copy_from_user((void *)&c, argp,
					sizeof(struct lf1000fb_position_cmd)))
				return -EFAULT;
			set_position(fbi, c.position.left, c.position.top,
					c.position.right, c.position.bottom,
					c.position.apply);
#ifdef USE_TVOUT
			if (gpio_have_tvout()) {
				mlc_select(fbi, 1);
				set_position(fbi, c.position.left, c.position.top,
						c.position.right, c.position.bottom,
						c.position.apply);
				mlc_select(fbi, 0);
			}
#endif
			break;

		case LF1000FB_IOCGPOSTION:
			if (!(_IOC_DIR(cmd) & _IOC_READ))
				return -EINVAL;
			get_position(fbi, &c.position.left, &c.position.top,
					&c.position.right, &c.position.bottom);
			if (copy_to_user(argp, (void *)&c,
					sizeof(struct lf1000fb_position_cmd)))
				return -EFAULT;
			break;

		case LF1000FB_IOCSVIDSCALE:
			if (!(_IOC_DIR(cmd) & _IOC_WRITE))
				return -EINVAL;
			if (copy_from_user((void *)&c, argp,
					sizeof(struct lf1000fb_vidscale_cmd)))
				return -EFAULT;
			if (set_video_scaler(fbi, c.vidscale.sizex,
					c.vidscale.sizey, c.vidscale.apply))
				return -EINVAL;
#ifdef USE_TVOUT
			if (gpio_have_tvout()) {
				mlc_select(fbi, 1);
				set_video_scaler(fbi, c.vidscale.sizex,
						c.vidscale.sizey, c.vidscale.apply);
				mlc_select(fbi, 0);
			}
#endif
			break;

		case LF1000FB_IOCGVIDSCALE:
			if (!(_IOC_DIR(cmd) & _IOC_READ))
				return -EINVAL;
			if (get_video_scaler(fbi, &c.vidscale.sizex,
					&c.vidscale.sizey))
				return -EINVAL;
			if (copy_to_user(argp, (void *)&c,
					sizeof(struct lf1000fb_vidscale_cmd)))
				return -EFAULT;
			break;

		default:
			return -ENOIOCTLCMD;
	}

	return 0;
}

static struct fb_ops lf1000fb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= lf1000fb_setcolreg,
	.fb_check_var	= lf1000fb_check_var,
	.fb_set_par	= lf1000fb_set_par,
	.fb_pan_display	= lf1000fb_pan_display,
	.fb_rotate	= lf1000fb_rotate,
	.fb_blank	= lf1000fb_blank,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
	.fb_ioctl	= lf1000fb_ioctl,
};

#ifdef USE_IRQ
static irqreturn_t lf1000fb_irq(int irq, void *dev_id)
{
	struct lf1000fb_info *info = dev_id;

	if (lf1000_dpc_int_pending()) {
		info->nirq++;
		lf1000_dpc_clear_int();
	}

	return IRQ_HANDLED;
}
#endif

/*
 * platform device
 */

static int lf1000fb_map_resource(struct lf1000fb_info *info,
		struct resource **res, void __iomem **mem,
		u8 index, const char *name)
{
#if 0
	*res = platform_get_resource(info->pdev, IORESOURCE_MEM, index);
#else
	*res = &lf1000_fb_resources[index];
#endif
	if (!res) {
		dev_err(&info->pdev->dev, "can't get %s resource\n", name);
		return -ENXIO;
	}

	if (!request_mem_region((*res)->start, RESSIZE(*res), name)) {
		dev_err(&info->pdev->dev, "can't request %s memory\n", name);
		release_resource(*res);
		*res = NULL;
		return -EBUSY;
	}

	*mem = ioremap_nocache((*res)->start, RESSIZE(*res));
	if(!mem) {
		dev_err(&info->pdev->dev, "can't remap %s memory\n", name);
		release_mem_region((*res)->start, RESSIZE(*res));
		return -ENOMEM;
	}

	return 0;
}

static void lf1000fb_unmap_resource(struct resource **res, void __iomem **mem)
{
	if (*mem) {
		iounmap(*mem);
		*mem = NULL;
	}

	if (*res) {
		release_mem_region((*res)->start, RESSIZE(*res));
		release_resource(*res);
		*res = NULL;
	}
}

static void __init init_debugfs(struct lf1000fb_layer *layer)
{
	struct dentry *dir;
	char name[4];

	snprintf(name, 4, "fb%u", layer->index);
	dir = debugfs_create_dir(name, layer->parent->debug);
	if (!dir || IS_ERR(dir))
		return;

	debugfs_create_file("registers", S_IRUSR, dir, layer,
			&lf1000_layer_regs_fops);
	debugfs_create_bool("enabled", S_IRUSR, dir, (u32 *)&layer->enabled);
	debugfs_create_bool("vflip", S_IRUSR, dir, (u32 *)&layer->vflip);
	debugfs_create_u8("format", S_IRUSR, dir, (u8 *)&layer->format);
	debugfs_create_u32("vstride", S_IRUSR, dir, &layer->vstride);
	debugfs_create_u8("hstride", S_IRUSR, dir, &layer->hstride);
}

static int __init lf1000fb_probe_layer(struct lf1000fb_info *info, u8 index)
{
	int ret;
	struct fb_info *fbi;
	struct lf1000fb_layer *layer;
	u32 fb_size;

	if (index >= info->num_layers)
		return -EINVAL;

	fbi = framebuffer_alloc(sizeof(struct lf1000fb_layer),
			&info->pdev->dev);
	if (fbi == NULL) {
		dev_err(&info->pdev->dev, "can't allocate layer %d\n", index);
		return -ENOMEM;
	}

	layer = fbi->par;
	layer->index = index;
	layer->parent = info;
	layer->fbinfo = fbi;
	layer->vflip = 0;
	fbi->pseudo_palette = layer->pseudo_pal;
	fb_size = get_fb_size(info, layer);

	memset(layer->pseudo_pal, PALETTE_CLEAR,
			ARRAY_SIZE(layer->pseudo_pal));

	info->fbs[index] = fbi;

	layer->fbinfo->fix.smem_start = info->fbres->start;
	layer->fbinfo->fix.smem_len = RESSIZE(info->fbres);
	layer->fbinfo->fix.accel = FB_ACCEL_NONE;
	layer->fbinfo->fix.ywrapstep = 0;
	snprintf(&layer->fbinfo->fix.id[0], 16, DRIVER_NAME "%d", index);

	fbi->var.xres		= info->screen->xres;
	fbi->var.yres		= info->screen->yres;
	fbi->var.xoffset	= 0;
	fbi->var.yoffset	= 0;

#if 0
	/* Preserve top layer with bootloader settings */
	if (index == 0) {
		u32 left, top, right, bottom;
		get_position(layer, &left, &top, &right, &bottom);
		fbi->var.xres	= right - left;
		fbi->var.yres	= bottom - top;
	}
#endif

	dev_info(&info->pdev->dev, "layer=%d, xyres=%dx%d\n",
			index, fbi->var.xres, fbi->var.yres);

	if (IS_YUV_LAYER(layer)) {
		fbi->var.nonstd =
			((info->num_layers - 1)<<LF1000_NONSTD_PRIORITY)|
			(LAYER_FORMAT_YUV420<<LF1000_NONSTD_FORMAT);
		layer->fbinfo->fix.type = FB_TYPE_PLANES;
		layer->fbinfo->fix.smem_start |= 0x20000000;
		layer->fbinfo->fix.xpanstep = 1;
		layer->fbinfo->fix.ypanstep = 1;
		layer->fbinfo->fix.line_length = 4096;
		fbi->var.xres_virtual = 4096;
		fbi->var.yres_virtual = layer->fbinfo->fix.smem_len / 4096;
		mlc_set_yuv_position(layer, info->num_layers - 1);
		set_video_scaler(layer, fbi->var.xres, fbi->var.yres, 0);
	} else {
		fbi->var.nonstd = (LAYER_FORMAT_RGB<<LF1000_NONSTD_FORMAT);
		layer->fbinfo->fix.type = FB_TYPE_PACKED_PIXELS;
		layer->fbinfo->fix.xpanstep = 1;
		layer->fbinfo->fix.ypanstep = 1;
		fbi->var.xres_virtual = 4096;
		fbi->var.yres_virtual = layer->fbinfo->fix.smem_len / 4096;

		/* use ABGR888 initially */
		fbi->var.bits_per_pixel	= 32;
		fbi->var.transp.length	= 8;
		fbi->var.transp.offset	= 24;
		set_bgr888(&fbi->var);
	}

	mlc_set_address(layer, info->fbres->start);
	mlc_set_locksize(layer, 0);

	fbi->var.activate = FB_ACTIVATE_FORCE | FB_ACTIVATE_NOW;

	fbi->fbops = &lf1000fb_ops;
	fbi->flags = FBINFO_DEFAULT;
	fbi->node = -1;

	fbi->screen_size = RESSIZE(info->fbres);
	fbi->screen_base = info->fbmem;

	ret = fb_alloc_cmap(&fbi->cmap, 256, 0);
	if (ret) {
		dev_err(&info->pdev->dev, "failed to allocate FB%d cmap\n",
				index);
		goto out_layer;
	}

#ifdef USE_TVOUT
	/* setup secondary MLC to clone primary MLC for TV out */
	if (gpio_have_tvout()) {
		mlc_select(layer, 1);
		mlc_set_address(layer, info->fbres->start);
		set_position(layer, 0, 0, fbi->var.xres, fbi->var.yres, 1);
		mlc_select(layer, 0);
	}
#endif

	lf1000fb_set_par(fbi); /* set up the hardware */

	ret = register_framebuffer(fbi);
	if (ret) {
		dev_err(&info->pdev->dev, "failed to register FB%d device\n",
				index);
		goto out_layer;
	}

	/* Only top layer should be visible */
	if (index > 0)
		lf1000fb_blank(FB_BLANK_POWERDOWN, fbi);
	else
		lf1000fb_blank(FB_BLANK_UNBLANK, fbi);

	if (info->debug)
		init_debugfs(layer);

	return 0;

out_layer:
	framebuffer_release(fbi);
	info->fbs[index] = NULL;

	return ret;
}

static int __init lf1000fb_probe(struct platform_device *pdev)
{
	struct lf1000fb_info *info = NULL;
	struct fb_info *fbinfo;
	int i;
	u32 reg;
	struct dentry *dir;
	int ret = 0;

	fbinfo = framebuffer_alloc(sizeof(struct lf1000fb_info), &pdev->dev);
	if (!fbinfo) {
		dev_err(&pdev->dev, "can't allocate FB\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, info);

	info = fbinfo->par;
	info->pdev = pdev;
	info->screen = lf1000_get_screen_info();
	
	info->num_layers = 3;

	dev_info(&info->pdev->dev, "layers=%d, screen=%dx%d\n",
			info->num_layers, info->screen->xres, info->screen->yres);

	info->fbs = kzalloc(sizeof(struct fb_info)*info->num_layers,
			GFP_KERNEL);
	if (!info->fbs) {
		dev_err(&pdev->dev, "can't allocate layer data\n");
		goto out_layers;
	}

#ifdef USE_IRQ
	info->irq = platform_get_irq(pdev, 0);
	if (info->irq < 0) {
		dev_err(&pdev->dev, "failed to get an IRQ number\n");
		ret = info->irq;
		goto out_irq;
	}

	ret = request_irq(info->irq, lf1000fb_irq, IRQF_SHARED,
			DRIVER_NAME, info);
	if (ret) {
		dev_err(&pdev->dev, "can't get DPC IRQ\n");
		ret = -ENOENT;
		goto out_irq;
	}
	dev_info(&pdev->dev, "got IRQ %d\n", info->irq);
#endif

	ret = lf1000fb_map_resource(info, &info->fbres, &info->fbmem, 0,
			DRIVER_NAME);
	if (ret)
		goto out_fbmem;

	ret = lf1000fb_map_resource(info, &info->mlcres, &info->mlc, 1,
			DRIVER_NAME "-mlc");
	if (ret) {
		lf1000fb_unmap_resource(&info->fbres, &info->fbmem);
		goto out_mlc;
	}

	dir = debugfs_create_dir(DRIVER_NAME, NULL);
	if (!dir || IS_ERR(dir))
		info->debug = NULL;
	else {
		info->debug = dir;
		debugfs_create_u32("nirq", S_IRUSR, dir, (u32 *)&info->nirq);
		debugfs_create_file("registers", S_IRUSR, dir, info,
			&lf1000_mlc_regs_fops);
	}

	/* set primary MLC register base address */
	info->mlcreg = info->mlc;
	info->prisec = false;

	// enable MLC clock
	writel(0xF, info->mlc + MLCCLKENB);

	/* set MLC screen size */
	writel((info->screen->xres-1) | ((info->screen->yres-1)<<16),
	       info->mlc + MLCSCREENSIZE);

	for (i = 0; i < info->num_layers; i++) {
		dev_dbg(&info->pdev->dev, "adding FB%d\n", i);
		ret = lf1000fb_probe_layer(info, i);
		if (ret)
			goto out_fb;
	}

	/* apply changes and make sure MLC is enabled */
	reg = readl(info->mlc + MLCCONTROLT);
	writel(reg | (1<<3)|(1<<1), info->mlc + MLCCONTROLT);

#ifdef USE_TVOUT
	/* clone secondary MLC for TV out dual display */
	if (gpio_have_tvout()) {
		reg = readl(info->mlc + MLCCLKENB);
		writel(reg, info->mlc + MLCCLKENB + MLCSECONDARY);
		reg = readl(info->mlc + MLCSCREENSIZE);
		writel(reg, info->mlc + MLCSCREENSIZE + MLCSECONDARY);
		reg = readl(info->mlc + MLCBGCOLOR);
		writel(reg, info->mlc + MLCBGCOLOR + MLCSECONDARY);
		reg = readl(info->mlc + MLCCONTROLT);
		writel(reg, info->mlc + MLCCONTROLT + MLCSECONDARY);
	}
#endif

#ifdef USE_IRQ
	info->nirq = 0;
	lf1000_dpc_enable_int(1);
#endif

	// Enable LCD output
	soc_dpc_set_out_enable(0);

	return 0;

out_fb:
	lf1000fb_unmap_resource(&info->mlcres, &info->mlc);
out_mlc:
	lf1000fb_unmap_resource(&info->fbres, &info->fbmem);
out_fbmem:
out_irq:
	kfree(info->fbs);
out_layers:
	framebuffer_release(fbinfo);
	platform_set_drvdata(pdev, NULL);

	return ret;
}

static int lf1000fb_remove(struct platform_device *pdev)
{
	struct fb_info *fbinfo = platform_get_drvdata(pdev);
	struct lf1000fb_info *info = fbinfo->par;
	struct lf1000fb_layer *layer;
	int i;

	if (info->debug)
		debugfs_remove_recursive(info->debug);

	for (i = 0; i < info->num_layers; i++) {
		if (info->fbs[i]) {
			dev_dbg(&info->pdev->dev, "removing FB%d\n", i);
			layer = (info->fbs[i])->par;
			fb_dealloc_cmap(&layer->fbinfo->cmap);
			unregister_framebuffer(layer->fbinfo);
			framebuffer_release(layer->fbinfo);
		}
	}

	lf1000fb_unmap_resource(&info->mlcres, &info->mlc);
	lf1000fb_unmap_resource(&info->fbres, &info->fbmem);

	kfree(info->fbs);
	framebuffer_release(fbinfo);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver lf1000fb_driver = {
	.probe		= lf1000fb_probe,
	.remove		= lf1000fb_remove,
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
};

static int lf1000fb_init(void)
{
	return platform_driver_register(&lf1000fb_driver);
}

static void __exit lf1000fb_exit(void)
{
	platform_driver_unregister(&lf1000fb_driver);
}

module_init(lf1000fb_init);
module_exit(lf1000fb_exit);

MODULE_DESCRIPTION("LF1000 Frame Buffer Driver");
MODULE_AUTHOR("LeapFrog");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:lf1000-fb");
