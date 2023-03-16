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
#ifndef __DEVICES_H__
#define __DEVICES_H__

/* cpu */
#define UART_DEV_NAME 			"nx-uart"
#if defined(CONFIG_FB_LF2000)
#define	FB_DEV_NAME				"lf2000-fb"
#else
#define	FB_DEV_NAME				"nx-fb"
#endif
#define	EHCI_DEV_NAME			"nx-ehci"
#define	OHCI_DEV_NAME			"nx-ohci"
#define	OTG_GADGET_DEV_NAME		"lf2000-hsotg"
#define	I2C_DEV_NAME			"nx-i2c"
#define	EHCI_DEV_NAME			"nx-ehci"
#define	OHCI_DEV_NAME			"nx-ohci"
#define	NAND_DEV_NAME			"nx-nand"
#define	KEYPAD_DEV_NAME			"nx-keypad"
#define	TS_DEV_NAME				"nx-ts"			/* Touch */
#define	RTC_DEV_NAME			"lf2000-rtc"
#define	SDHC_DEV_NAME			"nx-sdhc"
#define	V4L2_DEV_NAME			"nx-v4l2"		/* with vip */
#define	I2S_DEV_NAME			"nx-i2s"
#define	PCM_DEV_NAME			"nx-pcm"
#define IDCT_DEV_NAME			"lf2000-idct"
#define ADC_DEV_NAME			"nx-adc"
#define SYSFS_DEV_NAME			"lf2000-regs"
#define SPI_DEV_NAME			"lf2000-spi"

/* graphic */
#define VMEM_DEV_MAJOR			240
#define VMEM_DEV_NAME			"vmem"

#define OGL_DEV_MAJOR			249
#define OGL_DEV_NAME			"ogl_vr5"

#define DPC_DEV_MAJOR			241
#define DPC_DEV_NAME			"dpc"

#define VIP_DEV_MAJOR			261
#define	VIP_DEV_NAME			"vip"

/* board */
#define DM9000_DEV_NAME 		"dm9000"

/* misc */

/*--------------------------------------------------------------------
 *	Frame buffer platform data
 */
struct fb_plat_data {
	int 	module;		/* 0: primary, 1: secondary */
	int 	layer;		/* RGB 0, 1, 2 */
	int		x_res;		/* x resolution */
	int		y_res;		/* y resolution */
	int		pixelbit;	/* bit per pixel */
	u_int	format;		/* RGB format */
	int		buffers;	/* set 2 when support pan, default 1 */
	int		with_mm;	/* with (mm), default 0 */
	int		height_mm;	/* height (mm), default 0 */
	/* for pixel clock, default 0 */
	u_int	hs_left;	/* back porch + sync width */
	u_int	hs_right;	/* front porch */
	u_int	vs_upper;	/* back porch + sync width */
	u_int	vs_lower;	/* front porch */
	u_int	pixelclock;
};

/*--------------------------------------------------------------------
 *	I2C platform data
 */
struct i2c_plat_data {
	int 	port;
	int 	irq;
	u_int	logical_io_scl;	/* logical scl pad */
	u_int	logical_io_sda;	/* logical scl pad */
	u_int	clksrc;		/* GPIO: 0, I2C hw: 16 or 256 */
	u_int	clkscale;
	u_int	cntmax;		/* Quarter period count max register */
	int	polling;	/* wait ack method */
	int		bus_num;
	unsigned int	flags;
	unsigned int	slave_addr;
	unsigned long	frequency;
	unsigned int	sda_delay;
	void	(*cfg_gpio)(struct platform_device *dev);
};

/*--------------------------------------------------------------------
 *	SDHC platform data
 */
struct sdhc_plat_data {
	int logical_io_detect;
	int logical_io_wprotect;
	int dma_index;
};

/*--------------------------------------------------------------------
 *	Touch platform data
 */
struct adc_ts_plat_data {
	struct {
		int		logical_xm_p;		/* xmon_p */
		int		logical_xm_n;		/* xmon_n */
		int		logical_ym_p;		/* ymon_p */
		int		logical_ym_n;		/* ymon_n */
		int		logical_detect;		/* detect pen down */
		int		logical_control;	/* detect pen control */
		int 	delay;		/* x, y detect delay time (msec) */
	} io;
	struct {
		int		x_ch;		/* adc x channel */
		int		y_ch;		/* adc y channel */
		int 	x_min;
		int 	x_max;
		int 	y_min;
		int 	y_max;
		int		pointercal[7]; /* calibration value (tslib) */
	} adc;
};

/*--------------------------------------------------------------------
 *	Keypad platform data
 */
struct key_plat_data {
	int				bt_count;
	unsigned int  *	bt_io;
	unsigned int  *	bt_code;
};

/*--------------------------------------------------------------------
 *	VMem platform data
 */
#if defined(CONFIG_GRP_NEXELL_VMEM_KILLER)
struct vmem_plat_data {
	int		resv_task_num;
	char **	resv_task_name;
};
#endif

/*--------------------------------------------------------------------
 *	I2S platform data
 */
struct i2s_plat_data {
	int 	master;			/* 1=master, 0=slave */
	int		sync_bit;
	int		pcmout_w;
	int		pcmin_w;
	int		trans_mode;		/* 0 = I2S, 2 = Left-Justified, 3 = Right-Justified  */
	int		sample_rate;	/* 48000, 44100 */

	int 	clk_src0;		/* source clock 0  select */
	int 	clk_div0;		/* clock 0 dividor */
	int 	clk_inv0;		/* clock 0 invertor */
	int 	clk_src1;		/* source clock 1 select */
	int 	clk_div1;		/* clock 1 dividor */
	int 	clk_inv1;		/* clock 1 invertor */

	u_int	clk_inv;		/* clock out invert */
};

#endif	/* __DEVICES_H__ */

