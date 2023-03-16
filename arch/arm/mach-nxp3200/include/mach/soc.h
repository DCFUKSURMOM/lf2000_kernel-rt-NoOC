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
#ifndef __SOC_H__
#define __SOC_H__

/*------------------------------------------------------------------------------
 * 	Gpio SoC interface
 */
void 	soc_gpio_set_io_func(uint io, uint func);
uint 	soc_gpio_get_io_func(uint io);
void 	soc_gpio_set_io_dir(uint io, int out);
int 	soc_gpio_get_io_dir(uint io);
void 	soc_gpio_set_io_pullup(uint io, int up);
int 	soc_gpio_get_io_pullup(uint io);
void 	soc_gpio_set_out_value(uint io, int high);
int 	soc_gpio_get_out_value(uint io);
int 	soc_gpio_get_in_value(uint io);
void 	soc_gpio_set_int_enable(uint io, int on);
int 	soc_gpio_get_int_enable(uint io);
void 	soc_gpio_set_int_mode(uint io, uint mode);
int 	soc_gpio_get_int_mode(uint io);
int 	soc_gpio_get_int_pend(uint io);
void 	soc_gpio_clr_int_pend(uint io);

void 	soc_alv_set_det_enable(uint io, int on);
int 	soc_alv_get_det_enable(uint io);
void 	soc_alv_set_det_mode(uint io, uint mode, int on);
int 	soc_alv_get_det_mode(uint io, uint mode);

/*------------------------------------------------------------------------------
 * 	DMA SoC interface
 */
#define	DMA_TRANS_MEM2IO	0x1
#define	DMA_TRANS_IO2MEM	0x2
#define	DMA_TRANS_MEM2MEM	0x3
#define	DMA_TRANS_MEM2SIO	0x4
#define	DMA_TRANS_SIO2MEM	0x5

#define	DMA_MODE_NORMAL		(0)
#define	DMA_MODE_BUFFER		(1)

struct dma_trans {
	int	     channel;
	unsigned int tr_type;
	unsigned int srcbase;
	unsigned int dstbase;
	unsigned int length;
	unsigned int requestid;
	unsigned int bitwidth;
};

struct dma_trans * 	soc_dma_request (int channel, bool dynamic);
int				 	soc_dma_release (struct dma_trans *tr);
int				 	soc_dma_transfer(struct dma_trans *tr);
int 				soc_dma_set_mode(struct dma_trans *tr, int mode);
unsigned int 		soc_dma_get_mode(struct dma_trans *tr);
int				 	soc_dma_trans_stop(struct dma_trans *tr);
unsigned int	 	soc_dma_get_length(struct dma_trans *tr);
int				 	soc_dma_check_run(struct dma_trans *tr);
int				 	soc_dma_wait_ready(struct dma_trans *tr, uint timeout);

/*------------------------------------------------------------------------------
 * 	ADC SoC interface
 */
void 	soc_adc_detach(void);
void 	soc_adc_attach(void);
u_short	soc_adc_read(int ch, uint timeout);

void	soc_adc_suspend(void);
void	soc_adc_resume (void);

/*------------------------------------------------------------------------------
 * 	PWM SoC interface
 */
void 	soc_pwm_set_frequency(int ch, uint freq, uint duty);
void 	soc_pwm_get_frequency(int ch, uint *freq, uint *duty);
void 	soc_pwm_set_suspend_ch(int ch, uint freq, uint duty);
void 	soc_pwm_set_resume_ch (int ch);
void 	soc_pwm_set_suspend(void);
void 	soc_pwm_set_resume (void);

/*------------------------------------------------------------------------------
 * 	DPC SoC interface (refer to "mach/dpc.h")
 */

/* Display Backligt unit device */
struct dpc_blu_ops {
	void (*enable)	(uint dev);
	void (*disable)	(uint dev);
	void (*suspend)	(uint dev);
	void (*resume)	(uint dev);
};

/* Display LCD device */
struct dpc_lcd_ops {
	void (*enable)		(uint dev);
	void (*disable)		(uint dev);
	void (*suspend)		(uint dev);
	void (*resume)		(uint dev);
	int  (*setformat)	(uint dev, uint format, uint type, uint opt);
	int  (*getformat)	(uint dev, uint *format, uint *type);
};

/* Display DPC device */
struct dpc_dpc_ops {
	void (*enable)		(u_int dev);
	void (*disable)		(u_int dev);
	void (*suspend)		(u_int dev);
	void (*resume)		(u_int dev);
};

#define	VID_COL_ALPHA		(1)
#define	VID_COL_BRIGHT		(2)
#define	VID_COL_HUE			(3)
#define	VID_COL_CONTRAST	(4)
#define	VID_COL_SATURATION	(5)
#define	VID_COL_GAMMA		(6)

#define	RGB_COL_ALPHA		(1)
#define	RGB_COL_TRANSP		(2)
#define	RGB_COL_INVERT		(3)

/* DISPLAY */
uint 	soc_dpc_get_mode(void);
void 	soc_dpc_get_resolution(int module, int *x_res, int *y_res);
void 	soc_dpc_set_bgcolor(int module, uint color);
uint 	soc_dpc_get_bgcolor(int module);
void 	soc_dpc_wait_vsync(int module);
void 	soc_dpc_set_out_enable(int module);
void 	soc_dpc_set_out_disable(int module);
void 	soc_dpc_set_suspend(int module);
void 	soc_dpc_set_resume(int module);
/* LAYER */
void 	soc_dpc_set_layer_enable(int module, int layer, int enable);
int 	soc_dpc_get_layer_enable(int module, int layer);
void 	soc_dpc_wait_dirtyclear(int module, int layer);
/* VIDEO */
int 	soc_dpc_set_vid_format(int module, uint fourcc, int srcw, int srch);
void 	soc_dpc_get_vid_format(int module, uint *fourcc, int *srcw, int *srch);
void 	soc_dpc_set_vid_address(int module, uint lu_a, uint lu_s, uint cb_a, uint cb_s,
								uint cr_a,	uint cr_s, int waitvsync);
void 	soc_dpc_get_vid_address(int module, u_int*lu_a, u_int*lu_s, u_int*cb_a, u_int*cb_s,
								u_int*cr_a,	u_int*cr_s);
int 	soc_dpc_set_vid_position(int module, int left, int top, int right, int bottom, int waitvsync);
int 	soc_dpc_get_vid_position(int module, int*left, int*top, int*right, int*bottom);
void 	soc_dpc_set_vid_enable(int module, int enable);
void 	soc_dpc_set_vid_priority(int module, int prior);
int  	soc_dpc_get_vid_priority(int module);
void 	soc_dpc_set_vid_colorkey(int module, uint color, int enable);
uint  	soc_dpc_get_vid_colorkey(int module);
void 	soc_dpc_set_vid_color(int module, uint type, uint color, int enable);
uint  	soc_dpc_get_vid_color(int module, uint type);
void 	soc_dpc_set_vid_hfilter(int module, int enable);
uint 	soc_dpc_get_vid_hfilter(int module);
void 	soc_dpc_set_vid_vfilter(int module, int enable);
uint 	soc_dpc_get_vid_vfilter(int module);
/* RGB */
int  	soc_dpc_set_rgb_format(int module, int layer, uint format, int width, int height, int pixelbyte);
void    soc_dpc_get_rgb_format(int module, int layer, uint *format, int *width, int *height, int *pixelbyte);
int 	soc_dpc_set_rgb_position(int module, int layer, int startx, int starty, int waitvsync);
int 	soc_dpc_set_rgb_position2(int module, int layer, int left, int top, int right, int bottom, int waitvsync);
int 	soc_dpc_get_rgb_position(int module, int layer, int*startx, int*starty);
int 	soc_dpc_get_rgb_position2(int module, int layer, int* left, int* top, int* right, int* bottom);
int 	soc_dpc_set_rgb_invalidzone(int module, int layer, int num, int left, int top,
									int right, int bottom, int enable, int waitvsync);
void 	soc_dpc_set_rgb_address(int module, int layer, uint addr,
								uint hstride, uint vstride, int waitvsync);
void 	soc_dpc_get_rgb_address(int module, int layer, u_int*addr,
								u_int*hstride, u_int*vstride);
void 	soc_dpc_set_rgb_enable(int module, int layer, int enable);
void  	soc_dpc_set_rgb_color(int module, int layer, uint type, uint color, int enable);
uint 	soc_dpc_get_rgb_color(int module, int layer, uint type);
/* EXT LCD */
int		soc_dpc_set_lcd_out(int module, uint format, uint type);
int		soc_dpc_get_lcd_out(int module, uint *format, uint *type);
/* External device */
int 	soc_dpc_register_blu(uint dev, struct dpc_blu_ops *ops);
int 	soc_dpc_register_lcd(uint dev, struct dpc_lcd_ops *ops);
int 	soc_dpc_register_dpc(u_int dev, struct dpc_dpc_ops *ops);

int		soc_dpc_set_lcd_flip(int module, int flip);
int		soc_dpc_get_lcd_flip(int module);

int		soc_dpc_set_timings(int module, int clk, int hsw, int hfp, int hbp, int vsw, int vfp, int vbp);
int		soc_dpc_get_timings(int module, int* clk, int* hsw, int* hfp, int* hbp, int* vsw, int* vfp, int* vbp);
int		soc_dpc_get_vblank(int module);

/*------------------------------------------------------------------------------
 * 	WDT(WatchDogTime) SoC interface
 */
int 	soc_wdt_start(uint msec);
void 	soc_wdt_stop(void);
void 	soc_wdt_clear(void);
uint 	soc_wdt_remain(void);

#endif	/* __SOC_H__ */

