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
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/vip.h>
#include <mach/fourcc.h>
#include <mach/soc.h>

#include "vip_drv.h"
#include "vip_update.h"

#define	DUMG_REGISTER		0

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "vip: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#if (0)
#define ERROUT(msg...)		{ 	\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",	\
			__FILE__, __FUNCTION__, __LINE__),			\
		printk(KERN_ERR msg); }
#else
#define ERROUT(msg...)		do {} while (0)
#endif

static DEFINE_MUTEX(dev_lock);
#define	DEV_LOCK()			mutex_lock(&dev_lock)
#define	DEV_UNLOCK()		mutex_unlock(&dev_lock)
/*------------------------------------------------------------------------------
 * 	VIP hw data
 */

#define	DEV_CLIPPER			0
#define	DEV_DECIMATOR		1

struct vip_stat {
	int			v_type;
	int			h_cnt;
	int			v_cnt;
	long long	timestamp;
};

struct vip_info {
	int					load;		/* driver load status */
	int					module;		/* vip device module */
	int					port;		/* 0, 1 */
	int					device;		/* 0 = Clipper, 1 = Decimator */
	unsigned int		format;		/* fourcc -> format */
	int					clip_l;		/* clip left */
	int					clip_t;		/* clip top */
	int					clip_r;		/* clip right */
	int					clip_b;		/* clip bottom */
	int					scale_w;	/* decimator width */
	int					scale_h;	/* decimator height */
	unsigned int		rotate;		/* rotate flag */
	int					width;		/* vip out widht */
	int					height;		/* vip out height */

	int					ext_sync;	/* vip hw sync */
	int					clk_inv;

	/* vip out data */
	struct vip_prop		prop;
	struct vip_stat		stat;

	/* device status */
	unsigned int 		dev_stat;
	struct mutex 		lock;
	unsigned int 		users;

	/* video decoder, port = 0,1 <- decoder 0, 1 */
	struct vip_dec_ops* dec_list[VIP_MAX_PORT][VIP_MAX_DECODER];
	struct vip_dec_ops* decoder;			/* current decoder pointer */
	int					dec_index;			/* current decoder index */

	/* interrupt */
	int					irq_nr;
	wait_queue_head_t	wait_q;
	long				timeout;
	unsigned int		cond;
};

struct vip_info	vip_info[VIP_MAX_MODULE] = { { 0, }, };

#define	STATUS_CLEAR		(0)
#define	STATUS_INIT			(1<<0)
#define	STATUS_RUN			(1<<1)
#define	STATUS_PREPARE		(1<<2)
#define	STATUS_IRQ			(1<<3)
#define	STATUS_SUSPEND		(1<<4)

/* ( 0:VSYNC, 1:HSYNC, 2:Clipper & Decimator Done ). */
#define VIP_INT_SOURCE		NX_VIP_INT_VSYNC	/* NX_VIP_INT_VSYNC, NX_VIP_INT_DONE */

static void   dump_register(int module);
static struct NX_VIP_RegisterSet save_reg[VIP_MAX_MODULE];	/* For sleep mode */

/*----------------------------------------------------------------------------*/
int register_vip_decoder(struct vip_dec_ops* dec_ops, int module, int port)
{
	struct vip_info *info = &vip_info[module];
	int i = 0;

	if (module >= VIP_MAX_MODULE)
		return -1;

	if (! info->load) {
		printk(KERN_ERR "vip.%d is not load ...\n", module);
		return -1;
	}

	for (i = 0; VIP_MAX_DECODER > i; i++) {
		if (NULL == info->dec_list[port][i]) {
			info->dec_list[port][i] = dec_ops;
			break;
		}
	}

	if (VIP_MAX_DECODER == i) {
		printk(KERN_ERR "vip.%d decoder is full ...\n", module);
		return -1;
	}

	printk(KERN_INFO "vip-%d.%d: register decoder nr = %d\n", module, port, i);
	return 0;
}
EXPORT_SYMBOL(register_vip_decoder);

void unregister_vip_decoder(struct vip_dec_ops* dec_ops, int module, int port)
{
	struct vip_info *info = &vip_info[module];
	int i = 0;

	if (module >= VIP_MAX_MODULE)
		return;

	for (i = 0; VIP_MAX_DECODER > i; i++) {
		if (info->dec_list[port][i] == dec_ops) {
			info->dec_list[port][i] = NULL;
			break;
		}
	}
	printk(KERN_INFO "vip-%d.%d: unregister decoder nr = %d\n", module, port, i);
}
EXPORT_SYMBOL(unregister_vip_decoder);

/*------------------------------------------------------------------------------
 * 	VIP hw control
 */
static int verify_param(struct vip_info *info, struct vip_sync *sync, struct vip_param *param)
{
	int   left, top, right, bottom;
	int   clp_w, clp_h, scl_w, scl_h;
	int   out_w, out_h;
	u_int format;

	DBGOUT("%s: module=%d\n", __func__, info->module);

	left  	= param->left;
	top  	= param->top;
	right  	= param->right;
	bottom  = param->bottom;

	/*--------------------------------------------------------------------------
	 * vip image zone (clipper/decimator)
	 */
	left   = (left + 15) & ~0xF;	/* multiple of 16 */
	right  = (right) & ~0xF;		/* multiple of 16 */

	clp_w  = (right - left);
	clp_h  = (bottom - top);
	clp_h /= sync->interlace ? 2 : 1;

	if (clp_w > sync->h_active || clp_h > sync->v_active) {
		printk(KERN_ERR "%s: fail, clip=%d,%d must be less than sync=%d,%d\n",
			__func__, clp_w, clp_h, sync->h_active, sync->v_active);
		return -1;
	}

	scl_w = (param->scale_w) & ~0xF;	/* multiple of 16 */
	scl_h = (param->scale_h);

	if ( DEV_DECIMATOR == param->device &&
		(scl_w > clp_w || scl_h > clp_h) ) {
		printk(KERN_ERR "%s: fail, scale=%d,%d must be less than clip=%d,%d\n",
			__func__, scl_w, scl_h, clp_w, clp_h);
		return -1;
	}

	out_w = (DEV_DECIMATOR == param->device) ? scl_w : clp_w;
	out_h = (DEV_DECIMATOR == param->device) ? scl_h : clp_h * (sync->interlace ? 2 : 1);

	/*--------------------------------------------------------------------------
	 * fourcc
	 */
	if ( param->fourcc == FOURCC_YUYV &&
		(param->rotate || param->device == DEV_DECIMATOR) )  {
		printk(KERN_ERR "%s: fail, yuyv isn't support rotate/decimator...\n", __func__);
		return -1;
	}

	switch (param->fourcc) {
	case FOURCC_MVS0:	format = NX_VIP_FORMAT_420; 	break;
	case FOURCC_MVS2:	format = NX_VIP_FORMAT_422; 	break;
	case FOURCC_MVS4:	format = NX_VIP_FORMAT_444;		break;
	case FOURCC_YUYV:	format = NX_VIP_FORMAT_YUYV;	break;
	default:
		DBGOUT("%s: fail, not support fourcc=%c%c%c%c\n",
			 __func__, (param->fourcc)&0xFF, (param->fourcc>>8)&0xFF,
			(param->fourcc>>16)&0xFF, (param->fourcc>>24)&0xFF );
		return -1;
	}

	/*--------------------------------------------------------------------------
	 * rotate
	 */
	if (param->rotate & VIP_ROTATE_ENABLE) {
		out_w = out_w + out_h;
		out_h = out_w - out_h;
		out_w = out_w - out_h;
	}

	/*--------------------------------------------------------------------------
	 * set vip info
	 */
	info->device 	= param->device;	/* 0: Clipper, 1: Decimator */
	info->format	= format;

	info->clip_l  	= left;
	info->clip_t  	= top	 / (sync->interlace ? 2 : 1);
	info->clip_r  	= right;
	info->clip_b  	= bottom / (sync->interlace ? 2 : 1);
	info->scale_w 	= scl_w;
	info->scale_h 	= scl_h;

	info->rotate 	= param->rotate;

	info->width  	= out_w;
	info->height 	= out_h;
	info->timeout   = msecs_to_jiffies(sync->framepersec + 200);

	/* aligned size */
	param->left  	= left;
	param->top  	= top;
	param->right  	= right;
	param->bottom  	= bottom;
	param->scale_w 	= scl_w;
	param->scale_h 	= scl_h;

	DBGOUT("--- VIP information ---\n");
	DBGOUT("port	= %d \n", info->port);
	DBGOUT("device	= %s \n", DEV_DECIMATOR == info->device ? "DEC" : "CLP");
	DBGOUT("scan    = %s \n", sync->interlace ? "Interlace" : "Progrssive");
	DBGOUT("fourcc	= %c%c%c%c, 0x%x \n", param->fourcc&0xFF,(param->fourcc>>8)&0xFF,
		(param->fourcc>>16)&0xFF,(param->fourcc>>24)&0xFF, info->format);
	DBGOUT("clip_l	= %d \n", info->clip_l);
	DBGOUT("clip_t	= %d \n", info->clip_t);
	DBGOUT("clip_r	= %d \n", info->clip_r);
	DBGOUT("clip_b	= %d \n", info->clip_b);
	DBGOUT("scale_w	= %d \n", info->scale_w);
	DBGOUT("scale_h	= %d \n", info->scale_h);
	DBGOUT("rotate	= %s \n", info->rotate & VIP_ROTATE_ENABLE ? "ON" : "OFF");
	DBGOUT("v_flip	= %s \n", info->rotate & VIP_H_FLIP_ENABLE ? "ON" : "OFF");
	DBGOUT("h_flip	= %s \n", info->rotate & VIP_V_FLIP_ENABLE ? "ON" : "OFF");
	DBGOUT("out_w	= %d \n", info->width);
	DBGOUT("out_h	= %d \n", info->height);
	DBGOUT("v_type	= %d, %s \n", sync->video_type, sync->video_type == 0 ? "NTSC" :
							  (sync->video_type == 1?"PAL":"OTHERS"));
	DBGOUT("------------------------------\n");
	return 0;
}

static int get_property(struct vip_info *info, struct vip_sync *sync)
{
	struct vip_prop    * prop = &info->prop;
	struct vip_dec_ops * pdec = info->decoder;
	struct vip_sync      tsync;

	int port  = info->port;
	int index = info->dec_index;

	pdec = info->dec_list[port][index];
	if (! pdec) {
		printk(KERN_ERR "no attached decoder, vip-%d.%d index %d \n",
			info->module, info->port, info->dec_index);
		return -1;
	}

	if (! sync)
		sync = &tsync;

	/* clear */
	memset(sync, 0, sizeof(struct vip_sync));

	/* decoder sync info */
	pdec->get_sync(sync);

	/* set sync info */
	prop->interlace  = sync->interlace;
	prop->video_type = sync->video_type;
	prop->h_active   = sync->h_active;
	prop->v_active   = sync->v_active;
	prop->fps 	     = sync->framepersec;

	info->ext_sync	 = sync->external_sync;
	info->clk_inv	 = sync->clock_invert;
	info->decoder    = pdec;

	return 0;
}

static void clock_on(int module)
{
	struct vip_info * info = &vip_info[module];

	DBGOUT("%s, module=%d\n", __func__, module);
	NX_VIP_SetClockPClkMode(module, NX_PCLKMODE_ALWAYS);
	NX_VIP_SetClockBClkMode(module, NX_BCLKMODE_DYNAMIC);
	NX_VIP_SetClockSource(module, 0, info->clk_inv ? NX_VIP_CLKSRC_INVICLKIN : NX_VIP_CLKSRC_ICLKIN);
	NX_VIP_SetClockDivisor(module, 0, 1);
	NX_VIP_SetClockDivisorEnable(module, CTRUE);
	NX_VIP_SetVIPEnable(module, CFALSE, CFALSE, CFALSE, CFALSE);
}

static void clock_off(int module)
{
	DBGOUT("%s, module=%d\n", __func__, module);
	NX_VIP_SetVIPEnable(module, CFALSE, CFALSE, CFALSE, CFALSE);	// VIP, SEP, CLIP, DECI
	NX_VIP_SetInterruptEnableAll(module, CFALSE);
	NX_VIP_ClearInterruptPendingAll(module);
	NX_VIP_SetClockPClkMode(module, NX_PCLKMODE_DYNAMIC);
	NX_VIP_SetClockBClkMode(module, NX_BCLKMODE_DISABLE);
	NX_VIP_SetClockDivisorEnable(module, CFALSE);
}

static void setup_io_func(int module, int port)
{
#if defined (CONFIG_ARCH_NXP2120)
	/* VCLK, VID0 ~ 7 */

	/* VID = VIP 0 = module 0: port 0 */
	/* VID = VIP 0 = module 1: port 0 */
	const u_int port_0[][2] = {
		{ PAD_GPIO_C +  7, NX_GPIO_PADFUNC_1 },
		{ PAD_GPIO_B + 31, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_C +  0, NX_GPIO_PADFUNC_1 },
		{ PAD_GPIO_C +  1, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_C +  2, NX_GPIO_PADFUNC_1 },
		{ PAD_GPIO_C +  3, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_C +  4, NX_GPIO_PADFUNC_1 },
		{ PAD_GPIO_C +  5, NX_GPIO_PADFUNC_1 }, { PAD_GPIO_C +  6, NX_GPIO_PADFUNC_1 },
	};

	/* VID_B = VIP 1 = module 0: port 1 */
	const u_int port_1[][2] = {
		{ PAD_GPIO_B +  2, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_A + 16, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_A + 17, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_A + 18, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_A + 19, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_A + 20, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_A + 21, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_A + 22, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_A + 23, NX_GPIO_PADFUNC_2 },
	};

	/* VID_B2 = VIP 2 = module 1: port 1 */
	const u_int port_2[][2] = {
		{ PAD_GPIO_A + 31, NX_GPIO_PADFUNC_3 },
		{ PAD_GPIO_B + 13, NX_GPIO_PADFUNC_3 }, { PAD_GPIO_B + 14, NX_GPIO_PADFUNC_3 },
		{ PAD_GPIO_B + 15, NX_GPIO_PADFUNC_3 }, { PAD_GPIO_B + 16, NX_GPIO_PADFUNC_3 },
		{ PAD_GPIO_B + 17, NX_GPIO_PADFUNC_3 }, { PAD_GPIO_B + 18, NX_GPIO_PADFUNC_3 },
		{ PAD_GPIO_A + 29, NX_GPIO_PADFUNC_3 }, { PAD_GPIO_A + 30, NX_GPIO_PADFUNC_3 },
	};

	u_int *pad = NULL;
	int mod, i, len;

	mod = (module << 1) + port;
	len = sizeof(port_0)/sizeof(port_0[0]);

	DBGOUT("%s, module=%d, port=%d, mod=%d\n", __func__, module, port, mod);

	switch (mod) {
	case 0:	/* port 0 = module 0: port 0 */
	case 2:	/* port 0 = module 1: port 0 */
			pad = (u_int*)port_0;	break;

	case 1:	/* port 1 = module 0: port 1 */
			pad = (u_int*)port_1;	break;

	case 3:	/* port 2 = module 1: port 1 */
			pad = (u_int*)port_2;	break;
	default:
		printk(KERN_ERR "not support mode, module=%d, port=%d\n", module, port);
		return;
	}

	/* set pad function */
	for (i = 0; len > i; i++) {
		u_int io = *pad++;
		u_int fn = *pad++;
		DBGOUT("%s, io=%d, fn=%d\n", __func__, io, fn);
		soc_gpio_set_io_func(io, fn);
	}
#endif

#if defined (CONFIG_ARCH_NXP3200)

	/* VIP0:0 = VCLK, VID0 ~ 7 */
	const u_int port_0[][2] = {
		/* VCLK */
		{ PAD_GPIO_C +  0, NX_GPIO_PADFUNC_2 },
		/* DATA */
		{ PAD_GPIO_C +  8, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_C +  9, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_C + 10, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_C + 11, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_C + 12, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_C + 13, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_C + 14, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_C +  1, NX_GPIO_PADFUNC_2 },
	};

	/* VIP0:1 =  VCLK_B, VID_B0 ~ 7, VHSYNC_B, VVSYNC_B, VDVALID_B  */
	const u_int port_1[][2] = {
		/* VCLK */
		{ PAD_GPIO_C + 22, NX_GPIO_PADFUNC_2 },
		/* DATA */
		{ PAD_GPIO_C + 25, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_C + 26, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_C + 27, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_C + 28, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_C + 29, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_C + 30, NX_GPIO_PADFUNC_2 },
		{ PAD_GPIO_C + 31, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_D +  0, NX_GPIO_PADFUNC_2 },
		/* HSYNC, VSYNC */
		{ PAD_GPIO_C + 24, NX_GPIO_PADFUNC_2 }, { PAD_GPIO_C + 21, NX_GPIO_PADFUNC_2 },
		/* DVALID */
		{ PAD_GPIO_C + 23, NX_GPIO_PADFUNC_2 },
	};

	u_int *pad = NULL;
	int mod, i, len;

	mod = (module << 1) + port;
	len = sizeof(port_0)/sizeof(port_0[0]);

	DBGOUT("%s, module=%d, port=%d, mod=%d\n", __func__, module, port, mod);

	switch (mod) {
	case 0:	/* port 0 = module 0: port 0 */
			pad = (u_int*)port_0;
			len = sizeof(port_0)/sizeof(port_0[0]);
			break;

	case 1:	/* port 1 = module 0: port 1 */
			pad = (u_int*)port_1;
			len = sizeof(port_1)/sizeof(port_1[0]);
			break;
	default:
		printk(KERN_ERR "not support mode, module=%d, port=%d\n", module, port);
		return;
	}

	/* set pad function */
	for (i = 0; len > i; i++) {
		u_int io = *pad++;
		u_int fn = *pad++;
		DBGOUT("%s, io=%d, fn=%d\n", __func__, io, fn);
		soc_gpio_set_io_dir (io, 0);
		soc_gpio_set_io_func(io, fn);
	}
#endif
}

static void power_save(void * reg, int module, int down)
{
	struct NX_VIP_RegisterSet *pREG = (struct NX_VIP_RegisterSet *)NX_VIP_GetBaseAddress(module);
	struct NX_VIP_RegisterSet *pSRC = NULL;
	struct NX_VIP_RegisterSet *pDST = NULL;

	if (down) {
		pDST = (struct NX_VIP_RegisterSet *)reg;
		pSRC = pREG;
	} else {
		pDST = pREG;
		pSRC = (struct NX_VIP_RegisterSet *)reg;
	}

	pDST->VIP_CONFIG		= pSRC->VIP_CONFIG;
	pDST->VIP_HVINT			= pSRC->VIP_HVINT;
	pDST->VIP_SYNCCTRL		= pSRC->VIP_SYNCCTRL;
	pDST->VIP_SYNCMON		= pSRC->VIP_SYNCMON;
	pDST->VIP_VBEGIN		= pSRC->VIP_VBEGIN;
	pDST->VIP_VEND			= pSRC->VIP_VEND;
	pDST->VIP_HBEGIN		= pSRC->VIP_HBEGIN;
	pDST->VIP_HEND			= pSRC->VIP_HEND;
	pDST->VIP_FIFOCTRL		= pSRC->VIP_FIFOCTRL;
	pDST->VIP_HCOUNT		= pSRC->VIP_HCOUNT;
	pDST->VIP_VCOUNT		= pSRC->VIP_VCOUNT;
	pDST->VIP_CDENB			= pSRC->VIP_CDENB;
	pDST->VIP_ODINT			= pSRC->VIP_ODINT;
	pDST->VIP_IMGWIDTH		= pSRC->VIP_IMGWIDTH;
	pDST->VIP_IMGHEIGHT		= pSRC->VIP_IMGHEIGHT;
	pDST->CLIP_LEFT			= pSRC->CLIP_LEFT;
	pDST->CLIP_RIGHT		= pSRC->CLIP_RIGHT;
	pDST->CLIP_TOP			= pSRC->CLIP_TOP;
	pDST->CLIP_BOTTOM		= pSRC->CLIP_BOTTOM;
	pDST->DECI_TARGETW		= pSRC->DECI_TARGETW;
	pDST->DECI_TARGETH		= pSRC->DECI_TARGETH;
	pDST->DECI_DELTAW		= pSRC->DECI_DELTAW;
	pDST->DECI_DELTAH		= pSRC->DECI_DELTAH;
	pDST->DECI_CLEARW		= pSRC->DECI_CLEARW;
	pDST->DECI_CLEARH		= pSRC->DECI_CLEARH;
	pDST->DECI_LUSEG		= pSRC->DECI_LUSEG;
	pDST->DECI_CRSEG		= pSRC->DECI_CRSEG;
	pDST->DECI_CBSEG		= pSRC->DECI_CBSEG;
	pDST->DECI_FORMAT		= pSRC->DECI_FORMAT;
	pDST->DECI_ROTFLIP		= pSRC->DECI_ROTFLIP;
	pDST->DECI_LULEFT		= pSRC->DECI_LULEFT;
	pDST->DECI_CRLEFT		= pSRC->DECI_CRLEFT;
	pDST->DECI_CBLEFT		= pSRC->DECI_CBLEFT;
	pDST->DECI_LURIGHT		= pSRC->DECI_LURIGHT;
	pDST->DECI_CRRIGHT		= pSRC->DECI_CRRIGHT;
	pDST->DECI_CBRIGHT		= pSRC->DECI_CBRIGHT;
	pDST->DECI_LUTOP		= pSRC->DECI_LUTOP;
	pDST->DECI_CRTOP		= pSRC->DECI_CRTOP;
	pDST->DECI_CBTOP		= pSRC->DECI_CBTOP;
	pDST->DECI_LUBOTTOM		= pSRC->DECI_LUBOTTOM;
	pDST->DECI_CRBOTTOM		= pSRC->DECI_CRBOTTOM;
	pDST->DECI_CBBOTTOM		= pSRC->DECI_CBBOTTOM;
	pDST->CLIP_LUSEG		= pSRC->CLIP_LUSEG;
	pDST->CLIP_CRSEG		= pSRC->CLIP_CRSEG;
	pDST->CLIP_CBSEG		= pSRC->CLIP_CBSEG;
	pDST->CLIP_FORMAT		= pSRC->CLIP_FORMAT;
	pDST->CLIP_ROTFLIP		= pSRC->CLIP_ROTFLIP;
	pDST->CLIP_LULEFT		= pSRC->CLIP_LULEFT;
	pDST->CLIP_CRLEFT		= pSRC->CLIP_CRLEFT;
	pDST->CLIP_CBLEFT		= pSRC->CLIP_CBLEFT;
	pDST->CLIP_LURIGHT		= pSRC->CLIP_LURIGHT;
	pDST->CLIP_CRRIGHT		= pSRC->CLIP_CRRIGHT;
	pDST->CLIP_CBRIGHT		= pSRC->CLIP_CBRIGHT;
	pDST->CLIP_LUTOP		= pSRC->CLIP_LUTOP;
	pDST->CLIP_CRTOP		= pSRC->CLIP_CRTOP;
	pDST->CLIP_CBTOP		= pSRC->CLIP_CBTOP;
	pDST->CLIP_LUBOTTOM		= pSRC->CLIP_LUBOTTOM;
	pDST->CLIP_CRBOTTOM		= pSRC->CLIP_CRBOTTOM;
	pDST->CLIP_CBBOTTOM		= pSRC->CLIP_CBBOTTOM;
	pDST->VIP_SCANMODE		= pSRC->VIP_SCANMODE;
	pDST->CLIP_YUYVENB		= pSRC->CLIP_YUYVENB;
	pDST->CLIP_BASEADDRH	= pSRC->CLIP_BASEADDRH;
	pDST->CLIP_BASEADDRL	= pSRC->CLIP_BASEADDRL;
	pDST->CLIP_STRIDEH		= pSRC->CLIP_STRIDEH;
	pDST->CLIP_STRIDEL		= pSRC->CLIP_STRIDEL;
	pDST->VIP_VIP1			= pSRC->VIP_VIP1;

	pDST->VIPCLKGEN[0][0]	= pSRC->VIPCLKGEN[0][0];
	pDST->VIPCLKGEN[0][1]	= pSRC->VIPCLKGEN[0][1];
	pDST->VIPCLKGEN[1][0]	= pSRC->VIPCLKGEN[1][0];
	pDST->VIPCLKGEN[1][1]	= pSRC->VIPCLKGEN[1][1];
	pDST->VIPCLKENB			= pSRC->VIPCLKENB;
}

static int init_device(int module, struct vip_param *param)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_prop    * prop = &info->prop;
	struct vip_sync      sync;

	int	h_aw, h_fp, h_sw, h_bp;
	int v_ah, v_fp, v_sw, v_bp;
	int ret = 0;

	DBGOUT("%s: module=%d, status=0x%x\n", __func__, module, info->dev_stat);

	if ( (STATUS_INIT & info->dev_stat) ||
		 (STATUS_RUN  & info->dev_stat) ) {
		printk(KERN_ERR "%s: fail, invalid status, current status=%s ...\n",
			__func__, ((STATUS_INIT & info->dev_stat) ? "inited":"running") );
		return -1;
	}

	/* sync */
	ret = get_property(info, &sync);
	if (0 > ret)
		return ret;

	/* check parameters */
	ret = verify_param(info, &sync, param);
	if (0 > ret)
		return ret;

	/* set vip pad */
	setup_io_func(module, info->port);

	h_aw = sync.h_active;
	h_fp = sync.h_frontporch;
	h_sw = sync.h_syncwidth;
	h_bp = sync.h_backporch;
	v_ah = sync.v_active;
	v_fp = sync.v_frontporch;
	v_sw = sync.v_syncwidth;
	v_bp = sync.v_backporch;

	/* clock enable */
	clock_on(module);

	/* set hw */
	NX_VIP_SetHVSync(
					module,
					CFALSE,	// bExtSync
					h_aw * 2,	// Active width
					#if defined (CONFIG_VIP_DECODER_TVP5150) || \
						defined (CONFIG_VIP_DECODER_TVP5150_MODULE)
					v_ah + 12,	// Active height
					#else
					v_ah,		// Active height
					#endif
					h_sw * 2,	// Horizontal sync width
					h_fp * 2,	// Horizontal front porch
					h_bp * 2,	// Horizontal back porch
					v_sw,		// Vertical sync width
					v_fp,		// Vertical front porch
					v_bp);		// Vertical back porch


	NX_VIP_SetDataMode(module, sync.data_order, 8);	/* OV7725 order is 2:Y0CBY1CR */
	NX_VIP_SetFieldMode(module,
						sync.ext_field_sig ? CTRUE : CFALSE,
						(NX_VIP_FIELDSEL)sync.ext_field_sel,
						prop->interlace,
						sync.field_invert ? CTRUE : CFALSE);
	NX_VIP_SetDValidMode(module,
						sync.ext_dvalid_sig ? CTRUE : CFALSE,
						sync.ext_dvalid_pol ? CTRUE : CFALSE);
	NX_VIP_SetFIFOResetMode(module, NX_VIP_FIFORESET_FRAMEEND);

	#if defined (CONFIG_VIP_DECODER_TVP5150) || \
		defined (CONFIG_VIP_DECODER_TVP5150_MODULE)
	NX_VIP_SetClipRegion(module, info->clip_l, info->clip_t + 12, info->clip_r, info->clip_b + 12);
	#else
	NX_VIP_SetClipRegion(module, info->clip_l, info->clip_t, info->clip_r, info->clip_b);
	#endif
	NX_VIP_SetClipperFormat(module, (NX_VIP_FORMAT)info->format,
							info->rotate & VIP_ROTATE_ENABLE,
							info->rotate & VIP_H_FLIP_ENABLE,
							info->rotate & VIP_V_FLIP_ENABLE );

	if (DEV_DECIMATOR == param->device) {
		int clip_w = info->clip_r - info->clip_l;
		int clip_h = info->clip_b - info->clip_t;
		NX_VIP_SetDecimation(module, clip_w, clip_h, info->scale_w, info->scale_h);
		NX_VIP_SetDecimatorFormat(module, (NX_VIP_FORMAT)info->format,
							info->rotate & VIP_ROTATE_ENABLE,
							info->rotate & VIP_H_FLIP_ENABLE,
							info->rotate & VIP_V_FLIP_ENABLE );
	}

	NX_VIP_SetInputPort(module, (NX_VIP_INPUTPORT)info->port);

	DBGOUT("VIP: h_aw=%3d, h_fp=%3d, h_sw=%3d, h_bp=%3d\n", h_aw, h_fp, h_sw, h_bp);
	DBGOUT("VIP: v_ah=%3d, v_fp=%3d, v_sw=%3d, v_bp=%3d\n", v_ah, v_fp, v_sw, v_bp);

	if (info->decoder && info->decoder->init)
		info->decoder->init();

	/* change status */
	info->dev_stat = STATUS_INIT;
	return ret;
}

static int run_device(int module)
{
	struct vip_info * info = &vip_info[module];
	struct vip_prop * prop = &info->prop;
	int  ret = 0;

	DBGOUT("%s: module=%d, status=0x%x\n", __func__, module, info->dev_stat);
	if (! (STATUS_PREPARE & info->dev_stat)) {
		printk(KERN_ERR "%s: fail, not prepared status=0x%x ...\n", __func__, info->dev_stat);
		return -1;
	}

	/* change status */
	info->dev_stat |= STATUS_RUN;

	/* set update register */
	VIP_UPDATE_SetScanMode(module, prop->interlace ? CTRUE : CFALSE);

	NX_VIP_SetInterruptEnableAll(module, CFALSE);
	NX_VIP_ClearInterruptPendingAll(module);
	NX_VIP_SetInterruptEnable(module, VIP_INT_SOURCE, CTRUE);

	if (DEV_DECIMATOR == info->device)
		NX_VIP_SetVIPEnable(module, CTRUE, CTRUE, CFALSE, CTRUE); 	// VIP, SEP, CLIP, DECI
	else
		NX_VIP_SetVIPEnable(module, CTRUE, CTRUE, CTRUE, CFALSE); 	// VIP, SEP, CLIP, DECI

	/* wait irq */
	wait_event_interruptible_timeout(info->wait_q, info->cond, info->timeout);
	info->cond ? (info->cond = 0, ret = 0) : (ret = -1);
	if (0 > ret) {
		dump_register(module);
		printk(KERN_ERR "NO IRQ 0, wait=%d ms\n", jiffies_to_msecs(info->timeout));
	}

	/* set update register */
	if (DEV_DECIMATOR == info->device)
		VIP_UPDATE_SetFlag(module, VIP_REG_UPDATE_FLAG_DECIBLOCKADDR);
	else
		VIP_UPDATE_SetFlag(module, VIP_REG_UPDATE_FLAG_CLIPBLOCKADDR);

	/* wait irq */
	wait_event_interruptible_timeout(info->wait_q, info->cond, info->timeout);
	info->cond ? (info->cond = 0, ret = 0) : (ret = -1);
	if (0 > ret) {
		dump_register(module);
		printk(KERN_ERR "NO IRQ 1, wait=%d ms\n", jiffies_to_msecs(info->timeout));
	}

	return ret;
}

static void stop_device(int module)
{
	struct vip_info * info = &vip_info[module];
	volatile int wait = 0;

	DBGOUT("%s: module=%d, status=0x%x\n", __func__, module, info->dev_stat);

	info->dev_stat &= ~STATUS_RUN;
	info->dev_stat &= ~STATUS_PREPARE;

	while ( 1000 >= wait++ &&
			(STATUS_IRQ & info->dev_stat))
		msleep(1);

	NX_VIP_SetVIPEnable(module, CFALSE, CFALSE, CFALSE, CFALSE); 	// VIP, SEP, CLIP, DECI
	NX_VIP_SetInterruptEnableAll(module, CFALSE);
	NX_VIP_ClearInterruptPendingAll(module);
}

static void exit_device(int module)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = info->decoder;

	DBGOUT("%s: module=%d, status=0x%x\n", __func__, module, info->dev_stat);

	if (STATUS_CLEAR == info->dev_stat)
		return;

	if (STATUS_RUN == info->dev_stat)
		stop_device(module);

	if (pdec && pdec->exit)
		pdec->exit();

	clock_off(module);

	/* change status */
	info->dev_stat = STATUS_CLEAR;
}

static int prepare_buffer(int module, struct vip_buffer *buff)
{
	struct vip_info * info = &vip_info[module];

	DBGOUT("%s: module=%d, %s, lu_a=0x%08x (%4d), cb_a=0x%08x(%4d), cr_a=0x%08x(%d)\n",
		__func__, module, DEV_DECIMATOR == info->device ? "DEC":"CLP",
		buff->lu_a, buff->lu_s, buff->cb_a, buff->cb_s, buff->cr_a, buff->cr_s);

	if (STATUS_RUN & info->dev_stat) {
		printk(KERN_ERR "%s: fail, invalid status, current running ...\n", __func__);
		return -1;
	}

	if (DEV_DECIMATOR == info->device) {
		NX_VIP_SetDecimatorAddr(module, (NX_VIP_FORMAT)info->format,
				info->width, info->height, buff->lu_a, buff->cb_a, buff->cr_a);

		VIP_UPDATE_SetDecimatorBlockAddr(module, info->format, CFALSE,
				info->width, info->height, buff->lu_a, buff->cb_a, buff->cr_a);
		VIP_UPDATE_SetFlag(module, VIP_REG_UPDATE_FLAG_DECIBLOCKADDR);

	} else {
		if (NX_VIP_FORMAT_YUYV == (NX_VIP_FORMAT)info->format) {
			NX_VIP_SetClipperAddrYUYV(module, buff->lu_a, buff->lu_s);

			VIP_UPDATE_SetClipperLinearAddr(module, buff->lu_a, buff->lu_s);
			VIP_UPDATE_SetFlag(module, VIP_REG_UPDATE_FLAG_CLIPLINEARADDR);

		} else {
			NX_VIP_SetClipperAddr(module, (NX_VIP_FORMAT)info->format,
					info->width, info->height, buff->lu_a, buff->cb_a, buff->cr_a);

			VIP_UPDATE_SetClipperBlockAddr(module, info->format,
					info->rotate & VIP_ROTATE_ENABLE ? CTRUE: CFALSE,
					info->width, info->height,
					buff->lu_a, buff->cb_a, buff->cr_a);
			VIP_UPDATE_SetFlag(module, VIP_REG_UPDATE_FLAG_CLIPBLOCKADDR);
		}
	}

	/* change status */
	info->dev_stat |= STATUS_PREPARE;
	return CTRUE;
}

static int update_buffer(int module, struct vip_buffer *buff)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = info->decoder;
	int  ret = 0;

	DBGOUT("%s: %s, lu_a=0x%08X, cb_a=0x%08X, cr_a=0x%08X\n",
		__func__, DEV_DECIMATOR == info->device ? "DEC":"CLP",
		buff->lu_a, buff->cb_a, buff->cr_a);

	if (DEV_DECIMATOR == info->device) {
		VIP_UPDATE_SetDecimatorBlockAddr(module, info->format, CFALSE,
				info->width, info->height, buff->lu_a, buff->cb_a, buff->cr_a);
		VIP_UPDATE_SetFlag(module, VIP_REG_UPDATE_FLAG_DECIBLOCKADDR);
	} else {
		if (NX_VIP_FORMAT_YUYV == (NX_VIP_FORMAT)info->format) {
			VIP_UPDATE_SetClipperLinearAddr(module, buff->lu_a, buff->lu_s);
			VIP_UPDATE_SetFlag(module, VIP_REG_UPDATE_FLAG_CLIPLINEARADDR);
		} else {
			VIP_UPDATE_SetClipperBlockAddr(module, info->format,
					info->rotate & VIP_ROTATE_ENABLE ? CTRUE: CFALSE,
					info->width, info->height,
					buff->lu_a, buff->cb_a, buff->cr_a);
			VIP_UPDATE_SetFlag(module, VIP_REG_UPDATE_FLAG_CLIPBLOCKADDR);
		}
	}

	wait_event_interruptible_timeout(info->wait_q, info->cond, info->timeout);
	if (! info->cond) {
		ret = -1;
		printk(KERN_ERR "NO IRQ, wait=%d ms, hc=%d, vc=%d\n",
			jiffies_to_msecs(info->timeout), REG_VIP_HCOUNT(module), REG_VIP_VCOUNT(module));
		/* set timestmap */
		info->stat.timestamp += jiffies_to_msecs(info->timeout);
	}
	info->cond = 0;

	/* set video status. */
	buff->timestamp  = info->stat.timestamp;
	buff->h_count    = (info->stat.h_cnt  - (info->ext_sync ? 4 : 0)) >> 1;
	buff->v_count    = (info->stat.v_cnt);
	buff->video_type = 0;

	/* set video type changing. */
	if (0 > ret) {
		buff->video_type = -1;
	} else {
		if (pdec->det_type) {
			buff->video_type = pdec->det_type(buff->h_count, buff->v_count);
			if (-1 != buff->video_type && buff->video_type != info->prop.video_type) {
				printk(KERN_INFO "WARN: changed VIP video type, %d -> %d\n",
					info->prop.video_type, buff->video_type);
			}
		}
	}

	DBGOUT("HC=%4d, VC=%4d\n", buff->h_count, buff->v_count);
	return ret;
}

static int wait_device(int module)
{
	struct vip_info * info = &vip_info[module];
	int  ret = 0;

	if (! (STATUS_RUN & info->dev_stat)) {
		printk(KERN_ERR "%s: fail, invalid status, not run ...\n", __func__);
		return -1;
	}

	wait_event_interruptible_timeout(info->wait_q, info->cond, info->timeout);
	if (! info->cond) {
		ret = -1;
		printk(KERN_ERR "NO IRQ, wait=%d ms\n", jiffies_to_msecs(info->timeout));
		info->stat.timestamp += jiffies_to_msecs(info->timeout);
	}

	info->cond = 0;
	return ret;
}

static int get_video_prop(int module, struct vip_prop * prop)
{
	struct vip_info * info = &vip_info[module];

	if (0 > get_property(info, NULL))
		return -1;

	prop->interlace  = info->prop.interlace;
	prop->video_type = info->prop.video_type;
	prop->h_active   = info->prop.h_active;
	prop->v_active   = info->prop.v_active;
	prop->fps 	     = info->prop.fps;

	DBGOUT("%s\n", __func__);
	DBGOUT("Scan mode  : %s 		  \n", prop->interlace ? "Interlace" : "Progress");
	DBGOUT("Active w/h : w=%4d, h=%4d \n", prop->h_active, prop->v_active);
	DBGOUT("FPS        : %d 		  \n", prop->fps);
	return 0;
}

static int set_input_port(int module, int port)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = NULL;
	int index = info->dec_index;

	DBGOUT("%s, module=%d, port=%d \n", __func__, module, port);
	if (port > VIP_MAX_PORT) {
		printk(KERN_ERR "%s, not support port %d (max %d)\n",
			__func__, port, VIP_MAX_PORT);
		return -1;
	}

	if (STATUS_RUN  & info->dev_stat) {
		printk(KERN_ERR "%s: fail, invalid status, current running ...\n", __func__);
		return -1;
	}

	pdec = info->dec_list[port][index];
	if (! pdec) {
		ERROUT("no attached decoder, vip-%d.%d index %d \n",
			info->module, port, info->dec_index);
		return -1;
	}

	info->port    = port;
	info->decoder = pdec;

	return get_property(info, NULL);
}

static int get_input_port(int module)
{
	struct vip_info * info = &vip_info[module];
	return info->port;
}

static int get_dec_vtype_nr(int module)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = info->decoder;
	int ret = 0;

	if (! pdec) {
		ERROUT("no attached decoder, vip-%d.%d index %d \n",
			info->module, info->port, info->dec_index);
		return -1;
	}

	if (pdec->get_type_nr)
		ret = pdec->get_type_nr();

	return (ret ? ret : 1);
}

static int set_dec_vtype(int module, int type)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = info->decoder;
	int ret = 0;

	DBGOUT("%s, module=%d, type=%d \n", __func__, module, type);
	if (STATUS_RUN  & info->dev_stat) {
		printk(KERN_ERR "%s: fail, invalid status, current running ...\n", __func__);
		return -1;
	}

	if (! pdec) {
		ERROUT("no attached decoder, vip-%d.%d index %d \n",
			info->module, info->port, info->dec_index);
		return -1;
	}

	if (pdec->set_type)
		ret = pdec->set_type(type);

	return ret;
}

static int get_dec_vtype(int module)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = info->decoder;
	int ret = 0;

	if (! pdec) {
		ERROUT("no attached decoder, vip-%d.%d index %d \n",
			info->module, info->port, info->dec_index);
		return -1;
	}

	if (pdec->get_type)
		ret = pdec->get_type();

	return ret;
}

static int get_dec_vprop(int module, int type, struct vip_prop *prop)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = info->decoder;
	int ret = 0;

	DBGOUT("%s, module=%d \n", __func__, module);

	pdec = info->decoder;
	if (! pdec) {
		ERROUT("no attached decoder, vip-%d.%d index %d \n",
			info->module, info->port, info->dec_index);
		return -1;
	}

	memset(prop, 0, sizeof(struct vip_prop));

	if (pdec->get_prop)
		ret = pdec->get_prop(type, prop);

	return ret;
}

static int get_dec_index_nr(int module)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = NULL;
	int port = info->port;
	int num  = 0, i = 0;

	for ( ; VIP_MAX_DECODER > i; i++) {
		pdec = info->dec_list[port][i];
		if (pdec)
			num++;
	}
	return num;
}

static int set_dec_index(int module, int index)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = NULL;
	int port = info->port;

	DBGOUT("%s, module=%d, port=%d, index=%d \n", __func__, module, port, index);

	if (STATUS_RUN  & info->dev_stat) {
		printk(KERN_ERR "%s: fail, invalid status, current running ...\n", __func__);
		return -1;
	}

	pdec = info->dec_list[port][index];
	if (! pdec) {
		ERROUT("no attached decoder, vip-%d.%d index %d \n",
			info->module, port, index);
		return -1;
	}

	info->dec_index = index;
	info->decoder	= pdec;

	return get_property(info, NULL);
}

static int get_dec_index(int module)
{
	struct vip_info * info = &vip_info[module];
	return info->dec_index;
}

static int set_dec_resol(int module, int width, int height, int fps)
{
	struct vip_info    * info = &vip_info[module];
	struct vip_dec_ops * pdec = info->decoder;

	DBGOUT("%s, module=%d \n", __func__, module);

	if (STATUS_RUN  & info->dev_stat) {
		printk(KERN_ERR "%s: fail, invalid status, current running ...\n", __func__);
		return -1;
	}

	pdec = info->decoder;
	if (! pdec) {
		ERROUT("no attached decoder, vip-%d.%d index %d \n",
			info->module, info->port, info->dec_index);
		return -1;
	}

	if (pdec->set_resol) {
		int ret = pdec->set_resol(width, height, fps);
		if (0 > ret)
			return ret;
	}

	return get_property(info, NULL);
}

static void dump_register(int module)
{
#if (DUMG_REGISTER)
	struct NX_VIP_RegisterSet *pREG =
		(struct NX_VIP_RegisterSet*)NX_VIP_GetBaseAddress(module);

  	DBGOUT(" VIP_CONFIG		= 0x%04x\r\n", pREG->VIP_CONFIG);
  	DBGOUT(" VIP_HVINT		= 0x%04x\r\n", pREG->VIP_HVINT);
  	DBGOUT(" VIP_SYNCCTRL	= 0x%04x\r\n", pREG->VIP_SYNCCTRL);
  	DBGOUT(" VIP_SYNCMON	= 0x%04x\r\n", pREG->VIP_SYNCMON);
  	DBGOUT(" VIP_VBEGIN		= 0x%04x\r\n", pREG->VIP_VBEGIN);
  	DBGOUT(" VIP_VEND		= 0x%04x\r\n", pREG->VIP_VEND);
  	DBGOUT(" VIP_HBEGIN		= 0x%04x\r\n", pREG->VIP_HBEGIN);
  	DBGOUT(" VIP_HEND		= 0x%04x\r\n", pREG->VIP_HEND);
  	DBGOUT(" VIP_FIFOCTRL	= 0x%04x\r\n", pREG->VIP_FIFOCTRL);
  	DBGOUT(" VIP_HCOUNT		= 0x%04x\r\n", pREG->VIP_HCOUNT);
  	DBGOUT(" VIP_VCOUNT		= 0x%04x\r\n", pREG->VIP_VCOUNT);
   	DBGOUT(" VIP_CDENB		= 0x%04x\r\n", pREG->VIP_CDENB);
	DBGOUT(" VIP_ODINT		= 0x%04x\r\n", pREG->VIP_ODINT);
	DBGOUT(" VIP_IMGWIDTH	= 0x%04x\r\n", pREG->VIP_IMGWIDTH);
	DBGOUT(" VIP_IMGHEIGHT 	= 0x%04x\r\n", pREG->VIP_IMGHEIGHT);
	DBGOUT(" CLIP_LEFT		= 0x%04x\r\n", pREG->CLIP_LEFT);
	DBGOUT(" CLIP_RIGHT		= 0x%04x\r\n", pREG->CLIP_RIGHT);
	DBGOUT(" CLIP_TOP		= 0x%04x\r\n", pREG->CLIP_TOP);
	DBGOUT(" CLIP_BOTTOM	= 0x%04x\r\n", pREG->CLIP_BOTTOM);
	DBGOUT(" DECI_TARGETW	= 0x%04x\r\n", pREG->DECI_TARGETW);
	DBGOUT(" DECI_TARGETH	= 0x%04x\r\n", pREG->DECI_TARGETH);
	DBGOUT(" DECI_DELTAW	= 0x%04x\r\n", pREG->DECI_DELTAW);
	DBGOUT(" DECI_DELTAH	= 0x%04x\r\n", pREG->DECI_DELTAH);
	DBGOUT(" DECI_CLEARW	= 0x%04x\r\n", pREG->DECI_CLEARW);
	DBGOUT(" DECI_CLEARH	= 0x%04x\r\n", pREG->DECI_CLEARH);
	DBGOUT(" DECI_LUSEG		= 0x%04x\r\n", pREG->DECI_LUSEG);
	DBGOUT(" DECI_CRSEG		= 0x%04x\r\n", pREG->DECI_CRSEG);
	DBGOUT(" DECI_CBSEG		= 0x%04x\r\n", pREG->DECI_CBSEG);
	DBGOUT(" DECI_FORMAT	= 0x%04x\r\n", pREG->DECI_FORMAT);
	DBGOUT(" DECI_ROTFLIP	= 0x%04x\r\n", pREG->DECI_ROTFLIP);
	DBGOUT(" DECI_LULEFT	= 0x%04x\r\n", pREG->DECI_LULEFT);
	DBGOUT(" DECI_CRLEFT	= 0x%04x\r\n", pREG->DECI_CRLEFT);
	DBGOUT(" DECI_CBLEFT	= 0x%04x\r\n", pREG->DECI_CBLEFT);
	DBGOUT(" DECI_LURIGHT	= 0x%04x\r\n", pREG->DECI_LURIGHT);
	DBGOUT(" DECI_CRRIGHT	= 0x%04x\r\n", pREG->DECI_CRRIGHT);
	DBGOUT(" DECI_CBRIGHT	= 0x%04x\r\n", pREG->DECI_CBRIGHT);
	DBGOUT(" DECI_LUTOP		= 0x%04x\r\n", pREG->DECI_LUTOP);
	DBGOUT(" DECI_CRTOP		= 0x%04x\r\n", pREG->DECI_CRTOP);
	DBGOUT(" DECI_CBTOP		= 0x%04x\r\n", pREG->DECI_CBTOP);
	DBGOUT(" DECI_LUBOTTOM 	= 0x%04x\r\n", pREG->DECI_LUBOTTOM);
	DBGOUT(" DECI_CRBOTTOM 	= 0x%04x\r\n", pREG->DECI_CRBOTTOM);
	DBGOUT(" DECI_CBBOTTOM 	= 0x%04x\r\n", pREG->DECI_CBBOTTOM);
	DBGOUT(" CLIP_LUSEG		= 0x%04x\r\n", pREG->CLIP_LUSEG);
	DBGOUT(" CLIP_CRSEG		= 0x%04x\r\n", pREG->CLIP_CRSEG);
	DBGOUT(" CLIP_CBSEG		= 0x%04x\r\n", pREG->CLIP_CBSEG);
	DBGOUT(" CLIP_FORMAT	= 0x%04x\r\n", pREG->CLIP_FORMAT);
	DBGOUT(" CLIP_ROTFLIP	= 0x%04x\r\n", pREG->CLIP_ROTFLIP);
	DBGOUT(" CLIP_LULEFT	= 0x%04x\r\n", pREG->CLIP_LULEFT);
	DBGOUT(" CLIP_CRLEFT	= 0x%04x\r\n", pREG->CLIP_CRLEFT);
	DBGOUT(" CLIP_CBLEFT	= 0x%04x\r\n", pREG->CLIP_CBLEFT);
	DBGOUT(" CLIP_LURIGHT	= 0x%04x\r\n", pREG->CLIP_LURIGHT);
	DBGOUT(" CLIP_CRRIGHT	= 0x%04x\r\n", pREG->CLIP_CRRIGHT);
	DBGOUT(" CLIP_CBRIGHT	= 0x%04x\r\n", pREG->CLIP_CBRIGHT);
	DBGOUT(" CLIP_LUTOP		= 0x%04x\r\n", pREG->CLIP_LUTOP);
	DBGOUT(" CLIP_CRTOP		= 0x%04x\r\n", pREG->CLIP_CRTOP);
	DBGOUT(" CLIP_CBTOP		= 0x%04x\r\n", pREG->CLIP_CBTOP);
	DBGOUT(" CLIP_LUBOTTOM	= 0x%04x\r\n", pREG->CLIP_LUBOTTOM);
	DBGOUT(" CLIP_CRBOTTOM	= 0x%04x\r\n", pREG->CLIP_CRBOTTOM);
	DBGOUT(" CLIP_CBBOTTOM	= 0x%04x\r\n", pREG->CLIP_CBBOTTOM);
	DBGOUT(" VIP_SCANMODE	= 0x%04x\r\n", pREG->VIP_SCANMODE);
	DBGOUT(" CLIP_YUYVENB	= 0x%04x\r\n", pREG->CLIP_YUYVENB);
	DBGOUT(" CLIP_BASEADDRH	= 0x%04x\r\n", pREG->CLIP_BASEADDRH);
	DBGOUT(" CLIP_BASEADDRL	= 0x%04x\r\n", pREG->CLIP_BASEADDRL);
	DBGOUT(" CLIP_STRIDEH	= 0x%04x\r\n", pREG->CLIP_STRIDEH);
	DBGOUT(" CLIP_STRIDEL	= 0x%04x\r\n", pREG->CLIP_STRIDEL);
	DBGOUT(" VIPCLKENB		= 0x%04x\r\n", pREG->VIPCLKENB);
	DBGOUT(" VIPCLKGEN[0][0]= 0x%04x\r\n", pREG->VIPCLKGEN[0][0]);
	DBGOUT(" VIPCLKGEN[0][1]= 0x%04x\r\n", pREG->VIPCLKGEN[0][1]);
	DBGOUT(" VIPCLKGEN[1][0]= 0x%04x\r\n", pREG->VIPCLKGEN[1][0]);
	DBGOUT(" VIPCLKGEN[1][1]= 0x%04x\r\n", pREG->VIPCLKGEN[1][1]);
#endif
}

/*------------------------------------------------------------------------------
 * 	VIP interrupt
 */
static irqreturn_t do_irq_handler(int irq, void *desc)
{
	struct vip_info   * info   = desc;
	struct vip_stat * stat = &info->stat;
	int module = info->module;
	struct timeval tv;

	DBGOUT("%s, module=%d, pnd=%d, status=0x%x\n",
		__func__, module, NX_VIP_GetInterruptPending(module, VIP_INT_SOURCE), info->dev_stat);

	NX_VIP_ClearInterruptPending(module, VIP_INT_SOURCE);

	/* Odd or Even field */
	if (CTRUE == VIP_UPDATE_OnInterrupt(module)) {

		stat->h_cnt = REG_VIP_HCOUNT(module);
		stat->v_cnt = REG_VIP_VCOUNT(module);

		do_gettimeofday(&tv);
		stat->timestamp = tv.tv_sec * 1000 + tv.tv_usec/1000;

		info->dev_stat |= STATUS_IRQ;
		if (! (STATUS_RUN & info->dev_stat) ||
			(STATUS_SUSPEND & info->dev_stat) ) {
			info->dev_stat &= ~STATUS_IRQ;
			NX_VIP_SetInterruptEnableAll(module, CFALSE);
			NX_VIP_SetVIPEnable(module, CFALSE, CFALSE, CFALSE, CFALSE);
		}

		info->cond = 1;
		wake_up(&info->wait_q);
	}

	return IRQ_HANDLED;
}

static int init_interrupt(int module)
{
	struct vip_info * info = &vip_info[module];
	int irq = NX_VIP_GetInterruptNumber(module);
	int ret = 0;

	DBGOUT("%s, module=%d, irq=%d (src:%d)\n", __func__, module, irq, VIP_INT_SOURCE);

	ret = request_irq(irq, &do_irq_handler, IRQF_DISABLED, VIP_DEV_NAME, info);
	if (ret) {
		info->irq_nr = -1;
		printk(KERN_ERR "request interrupt=%d\n", irq);
		return -1;
	}

	info->irq_nr = irq;
	init_waitqueue_head(&info->wait_q);

	/* Set HW */
	NX_VIP_SetInterruptEnableAll(module, CFALSE);
	NX_VIP_ClearInterruptPendingAll(module);
	NX_VIP_SetInterruptEnable(module, VIP_INT_SOURCE, CTRUE);

	return 0;
}

static void deinit_interrupt(int module)
{
	struct vip_info * info = &vip_info[module];

	DBGOUT("%s, module=%d, irq=%d\n", __func__, module, info->irq_nr);

	NX_VIP_SetInterruptEnableAll(module, CFALSE);
	NX_VIP_ClearInterruptPendingAll(module);

	if (info->irq_nr >= 0) {
		free_irq(info->irq_nr, info);
		info->irq_nr = -1;
	}
}


/*------------------------------------------------------------------------------
 * 	VIP ops
 */
static int nx_vip_ops_open(struct inode *inode, struct file *flip)
{
	struct vip_info    * info = NULL;
	struct vip_dec_ops * pdec = NULL;

	int module = MINOR(inode->i_rdev);
	int i = 0, n = 0;
	int ret = 0;

	info = &vip_info[module];
	DBGOUT("%s, module=%d, status=0x%x\n", __func__, module, info->dev_stat);

	if (0 == info->users) {

		/* find decoder */
		for (i = 0; VIP_MAX_PORT > i && ! pdec; i++) {
			for (n = 0; VIP_MAX_DECODER > n; n++) {
				pdec = info->dec_list[i][n];
				if (pdec) {
					info->port = i, info->dec_index = n;
					break;
				}
			}
		}

		if (! pdec) {
			info->port = 0, info->dec_index = 0;
			printk(KERN_INFO "%s, WARN, vip %d.%d no attached decoder %d \n",
				__func__, module, 0, 0);
		}

		info->decoder = pdec;
		if (pdec)
			get_property(info, NULL);

		ret = init_interrupt(module);
		if (0 == ret)
			clock_on(module);

		/* clear status */
		info->dev_stat = STATUS_CLEAR;
	}

	if (0 == ret)
		info->users++;

	return ret;
}

static int nx_vip_ops_release(struct inode *inode, struct file *flip)
{
	struct vip_info * info = NULL;
	int module = MINOR(inode->i_rdev);

	info = &vip_info[module];
	DBGOUT("%s, module=%d, status=0x%x\n", __func__, module, info->dev_stat);

	if (1 == info->users) {
		exit_device(module);
		deinit_interrupt(module);
	}

	info->users--;
	if (0 > info->users)
		info->users = 0;

	return 0;
}

static long nx_vip_ops_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
{
	struct vip_info * info = NULL;
	int module = (int)MINOR(file->f_path.dentry->d_inode->i_rdev);
	bool ret = false;

	DBGOUT("%s (module=%d, cmd=0x%x, type=%c, nr=%d) \n",
		__func__, module, cmd, _IOC_TYPE(cmd), _IOC_NR(cmd));

	info = &vip_info[module];
	DEV_LOCK();

	switch (cmd) {
	case IOCTL_VIP_DEV_INIT:
		{
			struct vip_param param;
			if (copy_from_user(&param, (const void __user *)arg, sizeof(param)))
				break;

			if (0 > init_device(module, &param))
				break;

			if(copy_to_user((void __user *)arg, (const void *)&param, sizeof(param)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEV_EXIT:
		{
			exit_device(module);
			ret = true;
		}
		break;

	case IOCTL_VIP_DEV_RUN:
		{
			run_device(module);
			ret = true;
		}
		break;

	case IOCTL_VIP_DEV_STOP:
		{
			stop_device(module);
			ret = true;
		}
		break;

	case IOCTL_VIP_DEV_WAIT:
		{
			wait_device(module);
			ret = true;
		}
		break;

	case IOCTL_VIP_BUF_PREPARE:
		{
			struct vip_buffer buff;
			if (copy_from_user(&buff, (const void __user *)arg, sizeof(buff)))
				break;

			if (0 > prepare_buffer(module, &buff))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_BUF_UPDATE:
		{
			struct vip_buffer buff;
			if (copy_from_user(&buff, (const void __user *)arg, sizeof(buff)))
				break;

			if (0 > update_buffer(module, &buff))
				break;

			if (copy_to_user((void __user *)arg, (const void *)&buff, sizeof(buff)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_VID_GET_PROP:
		{
			struct vip_prop prop;

			if (0 > get_video_prop(module, &prop))
				break;

			if (copy_to_user((void __user *)arg, (const void *)&prop, sizeof(prop)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_VID_SET_PORT:
		{
			int port;
			if (copy_from_user(&port, (const void __user *)arg, sizeof(port)))
				break;

			if (0 > set_input_port(module, port))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_VID_GET_PORT:
		{
			int port = get_input_port(module);
			if (copy_to_user((void __user *)arg, (const void *)&port, sizeof(port)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEC_ALL_TYPE:
		{
			int num = get_dec_vtype_nr(module);
			if (0 > num)
				break;

			if (copy_to_user((void __user *)arg, (const void *)&num, sizeof(num)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEC_SET_TYPE:
		{
			int type;
			if (copy_from_user(&type, (const void __user *)arg, sizeof(type)))
				break;

			if (0 > set_dec_vtype(module, type))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEC_GET_TYPE:
		{
			int type = get_dec_vtype(module);
			if (copy_to_user((void __user *)arg, (const void *)&type, sizeof(type)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEC_GET_PROP:
		{
			u_int buf[6];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				break;
			/*
			 * 	buf[0]	: type
			 *  struct vip_prop
			 * 	buf[1]	: interlace
			 * 	buf[2]	: video_type
			 * 	buf[3]	: h_active
			 * 	buf[4]	: v_active
			 * 	buf[5]	: fps
			 */

			if (0 > get_dec_vprop(module, buf[0], (struct vip_prop*)&buf[1]))
				break;

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEC_ALL_NUM:
		{
			int num = get_dec_index_nr(module);
			if (copy_to_user((void __user *)arg, (const void *)&num, sizeof(num)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEC_SET_NUM:
		{
			int index;
			if (copy_from_user(&index, (const void __user *)arg, sizeof(index)))
				break;

			if (0 > set_dec_index(module, index))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEC_GET_NUM:
		{
			int index = get_dec_index(module);
			if (copy_to_user((void __user *)arg, (const void *)&index, sizeof(index)))
				break;
			else
				ret = true;
		}
		break;

	case IOCTL_VIP_DEC_SET_RESOL:
		{
			u_int buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				break;
			/*
			 * 	buf[0]	: width
			 * 	buf[1]	: height
			 * 	buf[2]	: fps
			 */

			if (0 > set_dec_resol(module, buf[0], buf[1], buf[2]))
				break;
			else
				ret = true;
		}
		break;

	default:
		break;
	}

	DBGOUT("%s (module=%d, cmd=0x%x, type=%c, nr=%d, ret=%s) \n\n",
		__func__, module, cmd, _IOC_TYPE(cmd), _IOC_NR(cmd), ret?"done":"fail");

	DEV_UNLOCK();
	return ret;
}

struct file_operations nx_vip_ops = {
	.owner 			= THIS_MODULE,
	.open 			= nx_vip_ops_open,
	.release		= nx_vip_ops_release,
	.unlocked_ioctl = nx_vip_ops_ioctl,
};

/*--------------------------------------------------------------------------------
 * VIP platform_driver functions
 ---------------------------------------------------------------------------------*/
struct class *vip_class;

static int __init nx_vip_drv_probe(struct platform_device *pdev)
{
	struct vip_info *info = NULL;
	int module = pdev->id;

	DBGOUT("%s (name=%s, id=%d)\n", __func__, dev_name(&pdev->dev), module);

	info = &vip_info[module];

	/* clear */
	memset(info, 0, sizeof(*info));

	/* init resource */
	mutex_init(&info->lock);

	/* set prototype base */
	NX_VIP_SetBaseAddress(module, (U32)IO_ADDRESS(NX_VIP_GetPhysicalAddress(module)));

	/* set platform data */
	info->load	 = 1;
	info->module = module;

	printk(KERN_INFO "%s: register major:%d, minor:%d\n", VIP_DEV_NAME, VIP_DEV_MAJOR, module);
	return 0;
}

static int nx_vip_drv_remove(struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_vip_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct vip_info    * info = NULL;
	struct vip_dec_ops * pdec = NULL;
	int module = pdev->id;
	int i, n;

	volatile int wait = 0;

	PM_DBGOUT("%s\n", __func__);

	info = &vip_info[module];

	info->dev_stat |= STATUS_SUSPEND;

	while ( 1000 >= wait++ &&
			(STATUS_IRQ & info->dev_stat))
		msleep(1);

	power_save(&save_reg[module], module, 1);

	for (i = 0;  VIP_MAX_PORT > i; i++) {
		for (n = 0; VIP_MAX_DECODER > n; n++) {
			pdec = info->dec_list[i][n];
			if (pdec && pdec->suspend)
				pdec->suspend();
		}
	}

	clock_off(module);
	return 0;
}

static int nx_vip_drv_resume(struct platform_device *pdev)
{
	struct vip_info    * info = NULL;
	struct vip_dec_ops * pdec = NULL;
	int module = pdev->id;
	int port, i, n;

	PM_DBGOUT("%s\n", __func__);

	info = &vip_info[module];
	port = info->port;

	setup_io_func(module, port);

	for (i = 0;  VIP_MAX_PORT > i; i++) {
		for (n = 0; VIP_MAX_DECODER > n; n++) {
			pdec = info->dec_list[i][n];
			if (pdec && pdec->resume)
				pdec->resume();
		}
	}

	power_save(&save_reg[module], module, 0);

	info->dev_stat &= ~STATUS_SUSPEND;

	if (STATUS_RUN & info->dev_stat) {

		NX_VIP_SetInterruptEnableAll(module, CFALSE);
		NX_VIP_ClearInterruptPendingAll(module);
		NX_VIP_SetInterruptEnable(module, VIP_INT_SOURCE, CTRUE);

		if (DEV_DECIMATOR == info->device)
			NX_VIP_SetVIPEnable(module, CTRUE, CTRUE, CFALSE, CTRUE); 	// VIP, SEP, CLIP, DECI
		else
			NX_VIP_SetVIPEnable(module, CTRUE, CTRUE, CTRUE, CFALSE); 	// VIP, SEP, CLIP, DECI

		clock_on(module);
	}

	return 0;
}

static struct platform_driver vip_plat_driver = {
	.probe		= nx_vip_drv_probe,
	.remove		= nx_vip_drv_remove,
	.suspend	= nx_vip_drv_suspend,
	.resume		= nx_vip_drv_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= VIP_DEV_NAME,
	},
};

static int __init nx_vip_drv_init(void)
{
	int ret = platform_driver_register(&vip_plat_driver);
	int i = 0;

	if (0 > ret)
		return ret;

	ret = register_chrdev(VIP_DEV_MAJOR, "VIP (Video Input Processor)", &nx_vip_ops);
	if (0 > ret) {
		printk(KERN_ERR "fail, register device (%s, major:%d)\n",
			VIP_DEV_NAME, VIP_DEV_MAJOR);
		return ret;
	}

	vip_class = class_create(THIS_MODULE, "vip");

	for (i = 0; NX_VIP_GetNumberOfModule() > i; i++)
		device_create(vip_class, NULL, MKDEV(VIP_DEV_MAJOR, i), NULL, "vip%u", i);

	return ret;
}

static void __exit nx_vip_drv_exit(void)
{
	int i = 0;
	DBGOUT("%s\n", __func__);

	for (i = 0; NX_VIP_GetNumberOfModule() > i; i++)
		device_destroy(vip_class, MKDEV(VIP_DEV_MAJOR, i));

	class_destroy(vip_class);

	unregister_chrdev(VIP_DEV_MAJOR, "VIP (Video Input Processor)");
	platform_driver_unregister(&vip_plat_driver);
}

module_init(nx_vip_drv_init);
module_exit(nx_vip_drv_exit);

MODULE_AUTHOR("jhkim <jhkin@nexell.co.kr>");
MODULE_DESCRIPTION("VIP (Video Input Processor) driver for the Nexell");
MODULE_LICENSE("GPL");

