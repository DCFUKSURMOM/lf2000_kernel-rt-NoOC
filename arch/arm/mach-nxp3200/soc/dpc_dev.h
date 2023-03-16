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
#ifndef __DPC_DEV_H__
#define __DPC_DEV_H__

/*------------------------------------------------------------------------------
 * Macro:
 */
/* Layer number */
#define	RGB_0_LAYER				0	/* 0: RGB0 layer number */
#define	RGB_1_LAYER				1	/* 1: RGB1 layer number */
#define	RGB_2_LAYER				2	/* 2: RGB2 layer number */
#define	VIDEO_LAYER				3	/* 3: video layer number */

#define	RGB_LAYERS				2	/* number of RGB layer  : 0 = RGB0, 1 =RGB1 */
#define	VID_LAYERS				1	/* number of Video layer: 3 = VIDEO */
#define	ALL_LAYERS				(RGB_LAYERS + VID_LAYERS)

/* Device status */
#define	STAT_ENABLE				0
#define	STAT_DISABLE			1
#define	STAT_RESUME				2
#define	STAT_SUSPEND			4

/* others */
#define	VSYNC_WAIT_TIME			60	/* 60 msec */

/* For math func */
#define	PI 						3.141592
#define	DEGREE_RADIAN(dg)		(dg*PI/180)

typedef struct NX_MLC_RegisterSet	MLC_REG;
typedef struct NX_DPC_RegisterSet	DPC_REG;

/*------------------------------------------------------------------------------
 * struct:
 */
struct rgb_data {
	unsigned int type;
	unsigned int format;
	int			 srcw;
	int			 srch;
	int			 pixelbyte;
	/* color */
	struct {
	unsigned int	backgcolor;
	unsigned int 	transcolor;
	unsigned int	invertcolor;
	unsigned int	alpha;
	} color;
};

struct vid_data{
	unsigned int type;
	unsigned int fourcc;
	int			 srcw;
	int			 srch;
	int			 priority;
	unsigned int h_filter;
	unsigned int v_filter;
	int			 lscreen;	/* screen layer */
	/* color */
	struct {
		int		alpha;		/* Default= 15,    0 <= Range <= 16		*/
		int		bright;		/* Default= 0,  -128 <= Range <= 128	*/
		int		contrast;	/* Default= 0,     0 <= Range <= 8		*/
		double	hue;		/* Default= 0,     0 <= Range <= 360	*/
		double	saturation;	/* Default= 0,  -100 <= Range <= 100	*/
		int		satura;
		int		gamma;
	} color;
};

struct dpc_reg {
	unsigned int	  *	mlc_base;		/* MLC register base */
	unsigned int	  *	dpc_base;		/* DPC register base */
	unsigned int	  * mlc_save;		/* MLC register save ptr */
	unsigned int	  * dpc_save;		/* MLC register save ptr */
};

struct dpc_sync_ts {
	long long innr;
	long long sync;
	long long wait;
};

struct dpc_info {
	char			  * name;
	unsigned int		status;
	int					irq;
	wait_queue_head_t   wait_q;
	unsigned int 		wait_time;
	int 				cond;
	int					x_res;
	int					y_res;
	unsigned int		bgcolor;
	void			  *	ldata[ALL_LAYERS];	/* 0: RGB0, 1:RGB1, 3:VIDEO */
	struct dpc_reg	    regs;				/* dpc registers */
	struct dpc_blu_ops* bluops;				/* device operations */
	struct dpc_lcd_ops* lcdops;
	struct dpc_dpc_ops* dpcops;
	struct dpc_sync_ts	times;
	uint				h_fp;
	uint				h_bp;
	uint				h_sw;
	uint				v_fp;
	uint				v_bp;
	uint				v_sw;
	uint				pixelclock;
	uint				duration;				/* unint is usec */
	uint				flipped;
	char * 				lcd_mfg;
	int					lcd_type;
	int					irq_count;
	int					clk_out_inv;
	int					clk_gen_div;
	int					clk_pad_sel;
};

/*------------------------------------------------------------------------------
 * to store layer RGB data
 */
static struct rgb_data rgb_data[NUMBER_OF_MLC_MODULE][RGB_LAYERS] = { { {0, }, }, };

/*------------------------------------------------------------------------------
 * to store layer VIDEO data
 */
static struct vid_data vid_data[NUMBER_OF_MLC_MODULE] = {
	{
		.priority 		=  CFG_DISP_LAYER_VIDEO_PRIORITY,
		.lscreen  		=  CFG_DISP_LAYER_SCREEN,
		.h_filter		=  0,	/* default horizontal filter */
		.v_filter		=  1,	/* default vertical filter   */
		.color			=
		{
			.alpha		= 15,
			.bright	 	=  0,
			.contrast	=  0,
			.hue		=  0,
			.saturation =  1,
			.satura	 	=  0,
		},
	},
#if defined (CONFIG_FB_NEXELL_SECONDARY) || defined (CONFIG_FB_NEXELL_DUAL)
	{
		.priority 	= CFG_DISP_LAYER_VIDEO_PRIORITY,
		.lscreen  	= CFG_DISP_LAYER_SCREEN,
		.color		= {
			.alpha		= 15,
			.bright	 	=  0,
			.contrast	=  0,
			.hue		=  0,
			.saturation =  1,
			.satura	 	=  0,
		},
	},
#endif
};

/*------------------------------------------------------------------------------
 * Display device informatioin
 */
static struct dpc_info dpc_dev_320_240[NUMBER_OF_DPC_MODULE] = {
	 {
	 	.name 		= "1st dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_P,
	 	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_320_240,
	 	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_320_240,
	 	.bgcolor	= CFG_DISP_BACK_GROUND_COLOR_320_240,
#if !defined(CFG_DISP_PRI_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_PRI_HSYNC_FRONT_PORCH_320_240,
		.h_bp		= CFG_DISP_PRI_HSYNC_BACK_PORCH_320_240,
		.h_sw		= CFG_DISP_PRI_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_PRI_VSYNC_FRONT_PORCH_320_240,
		.v_bp		= CFG_DISP_PRI_VSYNC_BACK_PORCH_320_240,
		.v_sw		= CFG_DISP_PRI_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_PRI_PIXEL_CLOCK_320_240,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
		.clk_out_inv = CFG_DISP_PRI_OUT_CLK_INVERT_320_240,
		.clk_gen_div = CFG_DISP_PRI_CLKGEN0_DIV_320_240,
		.clk_pad_sel = CFG_DISP_PRI_PADCLKSEL_320_240,
	 },
#if defined (CONFIG_FB_NEXELL_SECONDARY) || defined (CONFIG_FB_NEXELL_DUAL)
	 {
	 	.name 		= "2nd dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_S,
	 	.x_res		= CFG_DISP_SEC_RESOL_WIDTH,
	 	.y_res		= CFG_DISP_SEC_RESOL_HEIGHT,
	 	.bgcolor	= CFG_DISP_SEC_BACK_GROUND_COLOR,
#if !defined(CFG_DISP_SEC_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_SEC_HSYNC_FRONT_PORCH,
		.h_bp		= CFG_DISP_SEC_HSYNC_BACK_PORCH,
		.h_sw		= CFG_DISP_SEC_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_SEC_VSYNC_FRONT_PORCH,
		.v_bp		= CFG_DISP_SEC_VSYNC_BACK_PORCH,
		.v_sw		= CFG_DISP_SEC_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_SEC_PIXEL_CLOCK,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
	 },
#endif
};

static struct dpc_info dpc_dev_480_272[NUMBER_OF_DPC_MODULE] = {
	 {
	 	.name 		= "1st dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_P,
	 	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_480_272,
	 	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_480_272,
	 	.bgcolor	= CFG_DISP_BACK_GROUND_COLOR_480_272,
#if !defined(CFG_DISP_PRI_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_PRI_HSYNC_FRONT_PORCH_480_272,
		.h_bp		= CFG_DISP_PRI_HSYNC_BACK_PORCH_480_272,
		.h_sw		= CFG_DISP_PRI_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_PRI_VSYNC_FRONT_PORCH_480_272,
		.v_bp		= CFG_DISP_PRI_VSYNC_BACK_PORCH_480_272,
		.v_sw		= CFG_DISP_PRI_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_PRI_PIXEL_CLOCK_480_272,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
		.clk_out_inv = CFG_DISP_PRI_OUT_CLK_INVERT_480_272,
		.clk_gen_div = CFG_DISP_PRI_CLKGEN0_DIV_480_272,
		.clk_pad_sel = CFG_DISP_PRI_PADCLKSEL_480_272,
	 },
#if defined (CONFIG_FB_NEXELL_SECONDARY) || defined (CONFIG_FB_NEXELL_DUAL)
	 {
	 	.name 		= "2nd dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_S,
	 	.x_res		= CFG_DISP_SEC_RESOL_WIDTH,
	 	.y_res		= CFG_DISP_SEC_RESOL_HEIGHT,
	 	.bgcolor	= CFG_DISP_SEC_BACK_GROUND_COLOR,
#if !defined(CFG_DISP_SEC_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_SEC_HSYNC_FRONT_PORCH,
		.h_bp		= CFG_DISP_SEC_HSYNC_BACK_PORCH,
		.h_sw		= CFG_DISP_SEC_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_SEC_VSYNC_FRONT_PORCH,
		.v_bp		= CFG_DISP_SEC_VSYNC_BACK_PORCH,
		.v_sw		= CFG_DISP_SEC_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_SEC_PIXEL_CLOCK,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
	 },
#endif
};

static struct dpc_info dpc_dev_valencia_800_480[NUMBER_OF_DPC_MODULE] = {
	 {
	 	.name 		= "1st dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_P,
	 	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_800_480,
	 	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_800_480,
	 	.bgcolor	= CFG_DISP_BACK_GROUND_COLOR_800_480,
#if !defined(CFG_DISP_PRI_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_PRI_HSYNC_FRONT_PORCH_VALENCIA_800_480,
		.h_bp		= CFG_DISP_PRI_HSYNC_BACK_PORCH_VALENCIA_800_480,
		.h_sw		= CFG_DISP_PRI_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_PRI_VSYNC_FRONT_PORCH_VALENCIA_800_480,
		.v_bp		= CFG_DISP_PRI_VSYNC_BACK_PORCH_VALENCIA_800_480,
		.v_sw		= CFG_DISP_PRI_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_PRI_PIXEL_CLOCK_VALENCIA_800_480,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
		.clk_out_inv = CFG_DISP_PRI_OUT_CLK_INVERT_VALENCIA_800_480,
		.clk_gen_div = CFG_DISP_PRI_CLKGEN0_DIV_VALENCIA_800_480,
		.clk_pad_sel = CFG_DISP_PRI_PADCLKSEL_800_480,
	 },
#if defined (CONFIG_FB_NEXELL_SECONDARY) || defined (CONFIG_FB_NEXELL_DUAL)
	 {
	 	.name 		= "2nd dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_S,
	 	.x_res		= CFG_DISP_SEC_RESOL_WIDTH,
	 	.y_res		= CFG_DISP_SEC_RESOL_HEIGHT,
	 	.bgcolor	= CFG_DISP_SEC_BACK_GROUND_COLOR,
#if !defined(CFG_DISP_SEC_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_SEC_HSYNC_FRONT_PORCH,
		.h_bp		= CFG_DISP_SEC_HSYNC_BACK_PORCH,
		.h_sw		= CFG_DISP_SEC_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_SEC_VSYNC_FRONT_PORCH,
		.v_bp		= CFG_DISP_SEC_VSYNC_BACK_PORCH,
		.v_sw		= CFG_DISP_SEC_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_SEC_PIXEL_CLOCK,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
	 },
#endif
};

static struct dpc_info dpc_dev_knd_800_480[NUMBER_OF_DPC_MODULE] = {
	 {
	 	.name 		= "1st dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_P,
	 	.x_res		= 800,
	 	.y_res		= 480,
	 	.bgcolor	= CFG_DISP_BACK_GROUND_COLOR_800_480,
#if !defined(CFG_DISP_PRI_DPC_EXTCONFIG)
		.h_fp		= 40,
		.h_bp		= 88,
		.h_sw		= 128,
		.v_fp		= 13,
		.v_bp		= 45,
		.v_sw		= 3,
		.pixelclock = CFG_DISP_PRI_PIXEL_CLOCK_VALENCIA_800_480,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
		.clk_out_inv = CFALSE,
		.clk_gen_div = CFG_DISP_PRI_CLKGEN0_DIV_VALENCIA_800_480,
		.clk_pad_sel = CFG_DISP_PRI_PADCLKSEL_800_480,
	 },
#if defined (CONFIG_FB_NEXELL_SECONDARY) || defined (CONFIG_FB_NEXELL_DUAL)
	 {
	 	.name 		= "2nd dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_S,
	 	.x_res		= CFG_DISP_SEC_RESOL_WIDTH,
	 	.y_res		= CFG_DISP_SEC_RESOL_HEIGHT,
	 	.bgcolor	= CFG_DISP_SEC_BACK_GROUND_COLOR,
#if !defined(CFG_DISP_SEC_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_SEC_HSYNC_FRONT_PORCH,
		.h_bp		= CFG_DISP_SEC_HSYNC_BACK_PORCH,
		.h_sw		= CFG_DISP_SEC_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_SEC_VSYNC_FRONT_PORCH,
		.v_bp		= CFG_DISP_SEC_VSYNC_BACK_PORCH,
		.v_sw		= CFG_DISP_SEC_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_SEC_PIXEL_CLOCK,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
	 },
#endif
};

static struct dpc_info dpc_dev_1024_600[NUMBER_OF_DPC_MODULE] = {
	 {
	 	.name 		= "1st dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_P,
	 	.x_res		= 1024,
	 	.y_res		= 600,
	 	.bgcolor	= CFG_DISP_BACK_GROUND_COLOR_800_480,
#if !defined(CFG_DISP_PRI_DPC_EXTCONFIG)
		.h_fp		= 160,
		.h_bp		= 160,
		.h_sw		= CFG_DISP_PRI_HSYNC_SYNC_WIDTH,
		.v_fp		= 12,
		.v_bp		= 23,
		.v_sw		= CFG_DISP_PRI_VSYNC_SYNC_WIDTH,
		.pixelclock = 44000000,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
		.clk_out_inv = CFALSE,
		.clk_gen_div = 7,
		.clk_pad_sel = CFG_DISP_PRI_PADCLKSEL_800_480,
	 },
#if defined (CONFIG_FB_NEXELL_SECONDARY) || defined (CONFIG_FB_NEXELL_DUAL)
	 {
	 	.name 		= "2nd dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_S,
	 	.x_res		= CFG_DISP_SEC_RESOL_WIDTH,
	 	.y_res		= CFG_DISP_SEC_RESOL_HEIGHT,
	 	.bgcolor	= CFG_DISP_SEC_BACK_GROUND_COLOR,
#if !defined(CFG_DISP_SEC_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_SEC_HSYNC_FRONT_PORCH,
		.h_bp		= CFG_DISP_SEC_HSYNC_BACK_PORCH,
		.h_sw		= CFG_DISP_SEC_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_SEC_VSYNC_FRONT_PORCH,
		.v_bp		= CFG_DISP_SEC_VSYNC_BACK_PORCH,
		.v_sw		= CFG_DISP_SEC_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_SEC_PIXEL_CLOCK,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
	 },
#endif
};

static struct dpc_info dpc_dev_vtk_800_480[NUMBER_OF_DPC_MODULE] = {
	 {
	 	.name 		= "1st dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_P,
	 	.x_res		= CFG_DISP_PRI_RESOL_WIDTH_800_480,
	 	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT_800_480,
	 	.bgcolor	= CFG_DISP_BACK_GROUND_COLOR_800_480,
#if !defined(CFG_DISP_PRI_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_PRI_HSYNC_FRONT_PORCH_VTK_800_480,
		.h_bp		= CFG_DISP_PRI_HSYNC_BACK_PORCH_VTK_800_480,
		.h_sw		= CFG_DISP_PRI_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_PRI_VSYNC_FRONT_PORCH_VTK_800_480,
		.v_bp		= CFG_DISP_PRI_VSYNC_BACK_PORCH_VTK_800_480,
		.v_sw		= CFG_DISP_PRI_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_PRI_PIXEL_CLOCK_VTK_800_480,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
		.clk_out_inv = CFG_DISP_PRI_OUT_CLK_INVERT_VTK_800_480,
		.clk_gen_div = CFG_DISP_PRI_CLKGEN0_DIV_VTK_800_480,
		.clk_pad_sel = CFG_DISP_PRI_PADCLKSEL_800_480,
	 },
#if defined (CONFIG_FB_NEXELL_SECONDARY) || defined (CONFIG_FB_NEXELL_DUAL)
	 {
	 	.name 		= "2nd dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_S,
	 	.x_res		= CFG_DISP_SEC_RESOL_WIDTH,
	 	.y_res		= CFG_DISP_SEC_RESOL_HEIGHT,
	 	.bgcolor	= CFG_DISP_SEC_BACK_GROUND_COLOR,
#if !defined(CFG_DISP_SEC_DPC_EXTCONFIG)
		.h_fp		= CFG_DISP_SEC_HSYNC_FRONT_PORCH,
		.h_bp		= CFG_DISP_SEC_HSYNC_BACK_PORCH,
		.h_sw		= CFG_DISP_SEC_HSYNC_SYNC_WIDTH,
		.v_fp		= CFG_DISP_SEC_VSYNC_FRONT_PORCH,
		.v_bp		= CFG_DISP_SEC_VSYNC_BACK_PORCH,
		.v_sw		= CFG_DISP_SEC_VSYNC_SYNC_WIDTH,
		.pixelclock = CFG_DISP_SEC_PIXEL_CLOCK,
#else
		.h_fp		= 1,
		.h_bp		= 1,
		.h_sw		= 1,
		.v_fp		= 1,
		.v_bp		= 1,
		.v_sw		= 1,
		.pixelclock = 1,
#endif
	 },
#endif
};

/*----------------------------------------------------------------------------*/
/*
 * validate dpc_dev pointer assignment prior to use
 */
static struct dpc_info*		dpc_dev = NULL;

#define	DPC_INFO(_m)		((struct dpc_info *)&dpc_dev[_m])
#define	VID_DATA(_m)		((struct vid_data *)dpc_dev[_m].ldata[RGB_LAYERS])
#define	RGB_DATA(_m, _l)	((struct rgb_data *)dpc_dev[_m].ldata[_l])

#endif	/* __DPC_DEV_H__ */
