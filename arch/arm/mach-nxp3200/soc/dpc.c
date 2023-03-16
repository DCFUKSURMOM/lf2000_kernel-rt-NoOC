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
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm-generic/gpio.h>
#include <linux/lf2000/gpio.h>
#include <asm/system_info.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/platform_id.h>
#include <mach/devices.h>
#include <mach/soc.h>
#include <mach/dpc.h>
#include <mach/lf2000_lcd.h>
#include <mach/board_revisions.h>

#include "dpc_dev.h"

extern u_int cpu_get_clock_hz(cfg_sys_clksrc clk);

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "dpc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ printk(KERN_ERR "dpc: " msg); }

#define	DUMP_REG			(0)
#define	CHECK_TIMESTAMP		(1)

#if DUMP_REG
static void dbg_reg_dpc(int dev);
static void dbg_reg_mlc(int dev);
#endif

#if (CHECK_TIMESTAMP)
#define	SET_SEC_VAL(ss)		(ss * 1000*1000)
#define	GET_SEC_VAL(ls)		(ls / 1000*1000)

#define	GET_REAL_TIME(lt) {					\
		struct timeval tv;					\
		do_gettimeofday(&tv);				\
		NX_ASSERT(1000*1000 > tv.tv_usec);	\
		lt  = 0;							\
		lt  = SET_SEC_VAL(tv.tv_sec);		\
		lt += tv.tv_usec;					\
	}
#else
#define	GET_REAL_TIME(lt) 	(0)
#endif

/*----------------------------------------------------------------------------*/
static MLC_REG	mlc_reg_save[NUMBER_OF_MLC_MODULE];
static DPC_REG	dpc_reg_save[NUMBER_OF_DPC_MODULE];

/*------------------------------------------------------------------------------
 *  Local functions
 */

/* 12345'678'[8] -> 12345 [5], 123456'78'[8] -> 123456[6] */
inline static u_short R8G8B8toR5G6B5(uint RGB)
{
	u_char	R = (u_char)((RGB>>16) & 0xff);
	u_char	G = (u_char)((RGB>>8 ) & 0xff);
	u_char	B = (u_char)((RGB>>0 ) & 0xff);

	u_short R5G6B5 = ((R & 0xF8)<<8) | ((G & 0xFC)<<3) | ((B & 0xF8)>>3);
	DBGOUT(" RGB888:0x%08x -> RGB565:0x%08x\n", RGB, R5G6B5);
	return R5G6B5;
}

/* 12345 [5] -> 12345'123'[8], 123456[6] -> 123456'12'[8] */
inline static uint R5G6B5toR8G8B8(u_short RGB)
{
	u_char R5  = (RGB >> 11) & 0x1f;
	u_char G6  = (RGB >> 5 ) & 0x3f;
	u_char B5  = (RGB >> 0 ) & 0x1f;
	u_char R8  = ((R5 << 3) & 0xf8) | ((R5 >> 2) & 0x7);
	u_char G8  = ((G6 << 2) & 0xfc) | ((G6 >> 4) & 0x3);
	u_char B8  = ((B5 << 3) & 0xf8) | ((B5 >> 2) & 0x7);

	uint R8B8G8 = (R8 << 16) | (G8 << 8) | (B8);
	DBGOUT(" RGB565:0x%08x -> RGB888:0x%08x\n", RGB, R8B8G8);
	return R8B8G8;
}

/* 123'45678'[8] -> 123[3], 12'345678'[8] -> 12 [2] */
inline static u_char R8G8B8toR3G3B2(uint RGB)
{
	u_char	R = (u_char)((RGB>>16) & 0xff);
	u_char	G = (u_char)((RGB>>8 ) & 0xff);
	u_char	B = (u_char)((RGB>>0 ) & 0xff);

	u_char R3G3B2 = ( (R & 0xE0) | ((G & 0xE0)>>3) | ((B & 0xC0)>>6) );
	DBGOUT(" RGB888:0x%08x -> RGB332:0x%08x\n", RGB, R3G3B2);
	return R3G3B2;
}

/* 123[3] -> 123'123'12' [8], 12 [2] -> 12'12'12'12'[8] */
inline static uint R3G3B2toR8G8B8(u_char RGB)
{
	u_char R3  = (RGB >> 5) & 0x7;
	u_char G3  = (RGB >> 2) & 0x7;
	u_char B2  = (RGB >> 0) & 0x3;

	u_char R8  = ((R3 << 5) | (R3 << 2) | (R3 >> 1) );
	u_char G8  = ((G3 << 5) | (G3 << 2) | (G3 >> 1) );
	u_char B8  = ((B2 << 6) | (B2 << 4) | (B2 << 2) | B2 );

	uint R8B8G8 = (R8 << 16) | (G8 << 8) | (B8);
	DBGOUT(" RGB332:0x%08x -> RGB888:0x%08x\n", RGB, R8B8G8);
	return R8B8G8;
}

inline static double ksin(double radian)
{
	int    i = 1;
	double d = 0.0;
	double s = radian;

	while (1) {
		d += s;
		s *= -radian * radian/(2 * i * (2 * i + 1));
		i++;
		if (s < 0.000000000000001 && s > -0.000000000000001)
			break;
	}
	return d;
}

inline static double kcos(double radian)
{
	int    i = 1;
	double d = 0.0;
	double s = 1.0;

	while (1) {
		d += s;
		s *= -radian * radian/(2 * i * (2 * i - 1));
		i++;

		if (s < 0.000000000000001 && s > -0.000000000000001)
			break;
	}
	return d;
}

void set_dpc(struct dpc_info ** dpc, int module);

unsigned int get_pri_out_clk_invert(void)
{
	struct dpc_info *dpc = NULL;
	set_dpc(&dpc, 0);
	return dpc->clk_out_inv;
}

unsigned int get_pri_clkgen0_div(void)
{
	struct dpc_info *dpc = NULL;
	set_dpc(&dpc, 0);
	/* calculate nominal dot clock divider since PLLs vary */
	dpc->clk_gen_div = cpu_get_clock_hz((cfg_sys_clksrc)CFG_DISP_PRI_CLKGEN0_SOURCE) / dpc->pixelclock;
	if (dpc->clk_gen_div & 1)
		dpc->clk_gen_div--;
	printk(KERN_INFO "%s: pll=%d, clk=%d, div=%d\n", __func__, cpu_get_clock_hz(CFG_DISP_PRI_CLKGEN0_SOURCE), dpc->pixelclock, dpc->clk_gen_div);
	return dpc->clk_gen_div;
}

unsigned int get_disp_pri_padclksel(void)
{
	struct dpc_info *dpc = NULL;
	set_dpc(&dpc, 0);
	return dpc->clk_pad_sel;
}

struct dpc_info* validate_dpc_dev(void)
{
	if (dpc_dev != NULL)
		return dpc_dev;

	switch(get_lcd_size()) {
	case LCD_1024_600:
		dpc_dev = dpc_dev_1024_600;
		break;
	case LCD_800_480:
		switch(system_rev) {
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_RIO_KND_800_480:
			dpc_dev = dpc_dev_knd_800_480;
			break;
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
			dpc_dev = dpc_dev_valencia_800_480;
			break;
		default:
			dpc_dev = dpc_dev_vtk_800_480;
			break;
		}
		break;
	case LCD_480_272:
		dpc_dev = dpc_dev_480_272;
		break;
	case LCD_320_240:
	case LCD_UNKNOWN:
	default:
		dpc_dev = dpc_dev_320_240;
		break;
	}
	return dpc_dev;
}

void set_dpc(struct dpc_info ** dpc, int module)
{
	validate_dpc_dev();
	*dpc = DPC_INFO(module);
}

void set_vid(struct vid_data ** vid, int module)
{
	validate_dpc_dev();
	*vid = VID_DATA(module);
}

void set_rgb(struct rgb_data ** rgb, int module, int layer)
{
	validate_dpc_dev();
	*rgb = RGB_DATA(module, layer);
}


extern unsigned lf2000_gpio_l2p( struct gpio_chip* chip, unsigned offset );

/* set_spi_gpio_pin()  set LCD spi channel and gpio port/pin at runtime
 * return 1 on success, otherwise return 0
 */
int set_spi_gpio_pin(unsigned int * spi_channel, int * gpio_port, int * gpio_pin)
{
	switch (lf2000_gpio_l2p(NULL, LCD_SPI)) {
	case LF2000_GPIO_PORT_B | 12:
		*gpio_port   = 1;
		*gpio_pin    = 12;
		*spi_channel = 0;
		break;
	case LF2000_GPIO_PORT_C | 3:
		*gpio_port   = 2;
		*gpio_pin    = 3;
		*spi_channel = 1;
		break;
	default:
	case LF2000_GPIO_PORT_NONE:
		printk(KERN_INFO "%s:%s.%d  LCD_SPI not set in gpio_map[]\n",
			__FILE__, __func__, __LINE__);
		return 0;
	}
	return 1;
}

inline static void dpc_set_irq_enable(int module)
{
	struct dpc_info *dpc = NULL;
	int irq;

	set_dpc(&dpc, module);

	irq = dpc->irq;

	DBGOUT("%s (m:%d, irq:%d)\n", __func__, module, irq);

	NX_DPC_SetInterruptEnableAll(irq, CTRUE);
	enable_irq(irq);
}

inline static void dpc_set_irq_disable(int module)
{
	struct dpc_info *dpc = NULL;
	int irq;

	set_dpc(&dpc, module);

	irq = dpc->irq;

	DBGOUT("%s (m:%d, irq:%d)\n", __func__, module, irq);

	disable_irq(irq);
	NX_DPC_SetInterruptEnableAll(irq, CFALSE);
}

static void spi_lcd_init(void)
{
	int spi_channel;
	int gpio_port;
	int gpio_pin;

	struct dpc_info *dpc = NULL;

	/* special SPI port handling for 24 bit frame */
	set_dpc(&dpc, 0);
	if (!set_spi_gpio_pin(&spi_channel, &gpio_port, &gpio_pin))
		return;

	do {
		dpc->lcd_mfg = (char*)get_lcd_mfg();
	} while (strcmp(get_lcd_mfg(), dpc->lcd_mfg) != 0);
	dpc->lcd_type = get_lcd_type();

	printk(KERN_INFO "%s: spi_channel: %d, gpio_port:%d, gpio_pin:%d, lcd: %s, type: %d\n",
		__func__, spi_channel, gpio_port, gpio_pin, dpc->lcd_mfg, dpc->lcd_type);

	// SPI init module
	NX_SSPSPI_Initialize();
	NX_SSPSPI_SetBaseAddress(spi_channel,
			(U32)IO_ADDRESS(NX_SSPSPI_GetPhysicalAddress(spi_channel)));
	NX_SSPSPI_OpenModule(spi_channel);

	// SPI init setup
	NX_SSPSPI_SetDividerCount(spi_channel, 4);		// prescale divider
	if (dpc->lcd_type == LCD_HX8238)
		NX_SSPSPI_SetBitWidth(spi_channel, 8);		// 8-bit
	else
		NX_SSPSPI_SetBitWidth(spi_channel, 16);		// 16-bit
	NX_SSPSPI_SetEnable(spi_channel, CTRUE);
	NX_SSPSPI_SetByteSwap(spi_channel, CFALSE);		// no byte swap
	NX_SSPSPI_SetProtocol(spi_channel, NX_SSPSPI_PROTOCOL_SPI);// SPI
	NX_SSPSPI_SetSPIFormat(spi_channel, NX_SSPSPI_FORMAT_A);// format
	NX_SSPSPI_SetSlaveMode(spi_channel, CFALSE);		// master
	NX_SSPSPI_SetClockPolarityInvert(spi_channel, CFALSE);	// normal polarity

	// SPI enable
	NX_SSPSPI_SetEnable(spi_channel, CTRUE);
	NX_SSPSPI_ResetFIFO(spi_channel);

	NX_SSPSPI_SetClockPClkMode(spi_channel, NX_PCLKMODE_ALWAYS);
	NX_SSPSPI_SetClockSource(spi_channel, 0, 1);		// PLL1
	NX_SSPSPI_SetClockDivisor(spi_channel, 0, 0xE); 	// divisor = ?
	NX_SSPSPI_SetClockDivisorEnable(spi_channel, CTRUE);

	if (dpc->lcd_type == LCD_HX8238) {
		// SPI CS needs manual assertion for 24-bit transfers
		NX_GPIO_SetPadFunction (gpio_port, gpio_pin, NX_GPIO_PADFUNC_GPIO);
		NX_GPIO_SetOutputEnable(gpio_port, gpio_pin, CTRUE);
		NX_GPIO_SetOutputValue (gpio_port, gpio_pin, 1);
	}
	else {
		// SPI CS auto assertion
		NX_GPIO_SetPadFunction (gpio_port, gpio_pin, NX_GPIO_PADFUNC_1);
	}
}

static void spi_lcd_write(u8 reg, u8 val)
{
	int	spi_channel, gpio_port, gpio_pin;

	if (!set_spi_gpio_pin(&spi_channel, &gpio_port, &gpio_pin))
		return;

	// SPI LCD 16-bit register write for ILI6480G2
	NX_SSPSPI_PutHalfWord(spi_channel, (reg<<10) | (0<<9) | val);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(1);
}

#if DUMP_REG
static u16 spi_lcd_read(u8 reg)
{
	int	spi_channel, gpio_port, gpio_pin;
	u16 val = 0;

	if (!set_spi_gpio_pin(&spi_channel, &gpio_port, &gpio_pin))
		return 0;

	// SPI LCD 16-bit register read for ILI6480G2
	NX_SSPSPI_PutHalfWord(spi_channel, (reg<<10) | (1<<9) | (1<<8) | 0xFF);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(1);
	while (NX_SSPSPI_IsRxFIFOEmpty(spi_channel))
		udelay(1);
	val = NX_SSPSPI_GetHalfWord(spi_channel);

	DBGOUT("%s: %02X, %04X\n", __func__, reg, val);
	return val;
}
#endif

static void spi_lcd_write16(u8 reg, u8 val)
{
	int	spi_channel, gpio_port, gpio_pin;

	DBGOUT("%s: %02X, %02X\n", __func__, reg, val);

	if (!set_spi_gpio_pin(&spi_channel, &gpio_port, &gpio_pin))
		return;

	// SPI LCD 16-bit register write for ILI9322
	NX_SSPSPI_PutHalfWord(spi_channel, (reg<<8) | val);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(1);
}

#if DUMP_REG
static u16 spi_lcd_read16(u8 reg)
{
	int	spi_channel, gpio_port, gpio_pin;
	u16 val = 0;

	DBGOUT("%s: %02X, %02X\n", __func__, reg, val);

	if (!set_spi_gpio_pin(&spi_channel, &gpio_port, &gpio_pin))
		return val;

	// SPI LCD 16-bit register read for ILI9322
	spi_lcd_write16((1<<7) | reg, 0x00);
	while (NX_SSPSPI_IsRxFIFOEmpty(spi_channel))
		udelay(1);
	val = NX_SSPSPI_GetHalfWord(spi_channel);

	DBGOUT("%s: %02X, %04X\n", __func__, reg, val);
	return val;
}
#endif

static void spi_lcd_write24(u8 reg, u16 val)
{
	int	spi_channel, gpio_port, gpio_pin;

	DBGOUT("%s: %02X, %02X\n", __func__, reg, val);

	if (!set_spi_gpio_pin(&spi_channel, &gpio_port, &gpio_pin))
		return;

	// SPI LCD 24-bit register write for HX8238
	NX_GPIO_SetOutputValue (gpio_port, gpio_pin, 0);

	NX_SSPSPI_PutByte(spi_channel, 0x70);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(0);
	while (NX_SSPSPI_IsRxFIFOEmpty(spi_channel))
		udelay(0);
	NX_SSPSPI_GetByte(spi_channel);
	NX_SSPSPI_PutByte(spi_channel, 0x00);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(0);
	while (NX_SSPSPI_IsRxFIFOEmpty(spi_channel))
		udelay(0);
	NX_SSPSPI_GetByte(spi_channel);
	NX_SSPSPI_PutByte(spi_channel, reg);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(0);
	while (NX_SSPSPI_IsRxFIFOEmpty(spi_channel))
		udelay(0);
	NX_SSPSPI_GetByte(spi_channel);
	while (!NX_SSPSPI_IsTxShiftEmpty(spi_channel))
		udelay(0);

	NX_GPIO_SetOutputValue (gpio_port, gpio_pin, 1);
	udelay(0);
	NX_GPIO_SetOutputValue (gpio_port, gpio_pin, 0);

	NX_SSPSPI_PutByte(spi_channel, 0x72);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(0);
	while (NX_SSPSPI_IsRxFIFOEmpty(spi_channel))
		udelay(0);
	NX_SSPSPI_GetByte(spi_channel);
	NX_SSPSPI_PutByte(spi_channel, val >> 8);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(0);
	while (NX_SSPSPI_IsRxFIFOEmpty(spi_channel))
		udelay(0);
	NX_SSPSPI_GetByte(spi_channel);
	NX_SSPSPI_PutByte(spi_channel, val & 0xFF);
	while (!NX_SSPSPI_IsTxFIFOEmpty(spi_channel))
		udelay(0);
	while (NX_SSPSPI_IsRxFIFOEmpty(spi_channel))
		udelay(0);
	NX_SSPSPI_GetByte(spi_channel);
	while (!NX_SSPSPI_IsTxShiftEmpty(spi_channel))
		udelay(0);

	NX_GPIO_SetOutputValue (gpio_port, gpio_pin, 1);
}

static void spi_lcd_dump16(void)
{
#if DUMP_REG
	int i;

	for (i = 0x00; i < 0x18; i++)
		printk(KERN_INFO "%02X: %04X\n", i, spi_lcd_read16(i));
	printk(KERN_INFO "%02X: %04X\n", 0x30, spi_lcd_read16(0x30));
#endif
}

static void spi_lcd_dump_6482(void)
{
#if DUMP_REG
	int i;

	// ILI6480G2 / ILI6482 register dump
	for (i = 0; i < 24; i++)
		printk(KERN_INFO "%02X: %04X\n", i, spi_lcd_read(i));
	printk(KERN_INFO "%02X: %04X\n", 33, spi_lcd_read(33));
	printk(KERN_INFO "%02X: %04X\n", 37, spi_lcd_read(37));
#endif
}

static void spi_lcd_setup(void)
{
	struct dpc_info *dpc;

	DBGOUT("%s\n", __func__);

	set_dpc(&dpc, 0);

	if (dpc->lcd_type == LCD_HX8238) {
		// HX8238
		spi_lcd_write24(0x04, 0x0447);	// format
		spi_lcd_write24(0x05, 0xBCC4);	// sync
		spi_lcd_write24(0x01, 0x313F);	// flip
	}
	else if (dpc->lcd_type == LCD_ILI9322) {
		// ILI9322
		spi_lcd_dump16();
		spi_lcd_write16(0x06, 0x5C);	// flip
		spi_lcd_write16(0x05, 0x22);	// power
	}
	else if (dpc->lcd_type == LCD_ILI6480G2) {
		// ILI6480G2
		spi_lcd_dump_6482();
		spi_lcd_write(0x01, 0x1C);		// flip
	}
}

int	soc_dpc_set_lcd_flip(int module, int flip)
{
	int	spi_channel, gpio_port, gpio_pin;
	struct dpc_info *dpc;

	set_dpc(&dpc, module);
	if (!set_spi_gpio_pin(&spi_channel, &gpio_port, &gpio_pin))
		return flip;

	DBGOUT("%s (mod:%d, flip:%d)\n", __func__, module, flip);

	if (strcmp(get_lcd_mfg(), dpc->lcd_mfg) != 0) {
		NX_SSPSPI_SetEnable(spi_channel, CFALSE);
		spi_lcd_init();
		spi_lcd_setup();
	}

	if (dpc->lcd_type == LCD_HX8238)
		spi_lcd_write24(0x01, (flip) ? 0x733F : 0x313F);
	else if (dpc->lcd_type == LCD_ILI9322)
		spi_lcd_write16(0x06, (flip) ? 0x5F : 0x5C);
	else if (dpc->lcd_type == LCD_ILI6480G2)
		spi_lcd_write(0x01, (flip) ? 0x1F : 0x1C);

	return dpc->flipped = flip;
}

int	soc_dpc_get_lcd_flip(int module)
{
	struct dpc_info *dpc;

	set_dpc(&dpc, module);

	DBGOUT("%s (mod:%d, flip:%d)\n", __func__, module, dpc->flipped);
	return dpc->flipped;
}

static void dpc_set_1st_on(void)
{
	int module = 0;
	struct dpc_info *dpc;
	struct vid_data *vid;

	NX_DPC_DITHER RDither, GDither, BDither;
	uint  DPCFmt = CFG_DISP_PRI_OUT_FORMAT;
	bool  bESync = CFALSE;
	uint  XResol;
	uint  YResol;
	int   layer, delay;

	uint  bgcolor;
	int   priority;

	set_dpc(&dpc, module);
	set_vid(&vid, module);

	bgcolor  = dpc->bgcolor;
	priority = vid->priority;

	DBGOUT("%s (m:%d)\n", __func__, module);
	XResol = dpc->x_res;
	YResol = dpc->y_res;

	// Horizontal Up scale
	// Note: Only secondary DPC can scale up of horizontal width.
	if (module == 1)
		NX_DPC_SetHorizontalUpScaler(module, CFALSE, 2, 2);

	/* Set DPC */
   	if (((uint)NX_DPC_FORMAT_RGB555   == DPCFmt) ||
		((uint)NX_DPC_FORMAT_MRGB555A == DPCFmt) ||
		((uint)NX_DPC_FORMAT_MRGB555B == DPCFmt) ) {
		RDither = GDither = BDither = NX_DPC_DITHER_5BIT;
	} else if (((uint)NX_DPC_FORMAT_RGB565  == DPCFmt) ||
			   ((uint)NX_DPC_FORMAT_MRGB565 == DPCFmt) ) {
		RDither = BDither = NX_DPC_DITHER_5BIT;
		GDither           = NX_DPC_DITHER_6BIT;
	} else if (((uint)NX_DPC_FORMAT_RGB666  == DPCFmt) ||
			   ((uint)NX_DPC_FORMAT_MRGB666 == DPCFmt) ) {
		RDither = GDither = BDither = NX_DPC_DITHER_6BIT;
	} else {
		RDither = GDither = BDither = NX_DPC_DITHER_BYPASS;
	}

	// VCLK2 : CLKGEN0
	NX_DPC_SetClockSource  (module, 0, CFG_DISP_PRI_CLKGEN0_SOURCE);	// CLKSRCSEL
	NX_DPC_SetClockDivisor (module, 0, get_pri_clkgen0_div());			// CLKDIV
	NX_DPC_SetClockOutDelay(module, 0, CFG_DISP_PRI_CLKGEN0_DELAY); 	// OUTCLKDELAY
	DBGOUT("%s: CLKGEN0 %d, %d, %d\n", __FUNCTION__, CFG_DISP_PRI_CLKGEN0_SOURCE, get_pri_clkgen0_div(), CFG_DISP_PRI_CLKGEN0_DELAY);

	// VCLK : CLKGEN1
	NX_DPC_SetClockSource  (module, 1, CFG_DISP_PRI_CLKGEN1_SOURCE);	// CLKSRCSEL  : CLKGEN0's out
	NX_DPC_SetClockDivisor (module, 1, CFG_DISP_PRI_CLKGEN1_DIV);		// CLKDIV
	NX_DPC_SetClockOutDelay(module, 1, CFG_DISP_PRI_CLKGEN1_DELAY); 	// OUTCLKDELAY
	DBGOUT("%s: CLKGEN1 %d, %d, %d\n", __FUNCTION__, CFG_DISP_PRI_CLKGEN1_SOURCE, CFG_DISP_PRI_CLKGEN1_DIV, CFG_DISP_PRI_CLKGEN1_DELAY);

	NX_DPC_SetMode(module,
			(NX_DPC_FORMAT)DPCFmt,						// FORMAT
			CFALSE,										// SCANMODE
     	    CFG_DISP_PRI_OUT_POL_INVERT,   				// POLFIELD
            CFG_DISP_PRI_OUT_RGB, 						// RGBMODE
            CFALSE,       								// SWAPRB
	        (NX_DPC_YCORDER)CFG_DISP_PRI_OUT_YCORDER,	// YCORDER
	        bESync,										// YCCLIP
    	    bESync,  									// Embedded sync, Note> have to set to CTRUE for ITU-R BT.656
        	(NX_DPC_PADCLK)get_disp_pri_padclksel(),		// PADCLKSEL
           	get_pri_out_clk_invert(),					// PADCLKINV
			CFG_DISP_PRI_OUT_DUAL_VIEW);

	NX_DPC_SetHSync(module,
			XResol,
			dpc->h_sw,
			dpc->h_fp,
			dpc->h_bp,
			CFG_DISP_PRI_HSYNC_ACTIVE_HIGH);

	NX_DPC_SetVSync(module,
			YResol,
			dpc->v_sw,
			dpc->v_fp,
			dpc->v_bp,
			CFG_DISP_PRI_VSYNC_ACTIVE_HIGH,
			CFG_DISP_PRI_EVSYNC_ACTIVE_HEIGHT,
			CFG_DISP_PRI_EVSYNC_SYNC_WIDTH,
			CFG_DISP_PRI_EVSYNC_FRONT_PORCH,
			CFG_DISP_PRI_EVSYNC_BACK_PORCH);

	NX_DPC_SetVSyncOffset(module,
			CFG_DISP_PRI_VSYNC_START_OFFSET,
			CFG_DISP_PRI_VSYNC_END_OFFSET,
			CFG_DISP_PRI_EVSYNC_START_OFFSET,
			CFG_DISP_PRI_EVSYNC_END_OFFSET);

	NX_DPC_SetDelay(module,
		  	CFG_DISP_PRI_SYNC_DELAY_RGB_PVD,	// DELAYRGB
       	  	CFG_DISP_PRI_SYNC_DELAY_HS_CP1,		// DELAYHS_CP1
        	CFG_DISP_PRI_SYNC_DELAY_VS_FRAM,	// DELAYVS_FRAM
	        CFG_DISP_PRI_SYNC_DELAY_DE_CP2);	// DELAYDE_CP2

   	NX_DPC_SetDither(module, RDither, GDither, BDither);

	/* Set MLC */
	NX_MLC_SetClockPClkMode(module, NX_PCLKMODE_DYNAMIC);
	NX_MLC_SetClockBClkMode(module, NX_BCLKMODE_DYNAMIC);
	NX_MLC_SetLayerPriority(module, (NX_MLC_PRIORITY)priority);
	NX_MLC_SetBackground(module, bgcolor);
	NX_MLC_SetFieldEnable(module, CFG_DISP_PRI_MLC_INTERLACE);
	NX_MLC_SetScreenSize(module, XResol, YResol);
	NX_MLC_SetRGBLayerGamaTablePowerMode(module, CFALSE, CFALSE, CFALSE);
	NX_MLC_SetRGBLayerGamaTableSleepMode(module, CTRUE, CTRUE, CTRUE);
	NX_MLC_SetRGBLayerGammaEnable(module, CFALSE);
	NX_MLC_SetDitherEnableWhenUsingGamma(module, CFALSE);
	NX_MLC_SetGammaPriority(module, CFALSE);
    NX_MLC_SetTopPowerMode(module, CTRUE);
    NX_MLC_SetTopSleepMode(module, CFALSE);
	NX_MLC_SetMLCEnable(module, CTRUE);

	// Set layer dirty flag
	for (layer = 0; RGB_LAYERS > layer ; layer++) {
		NX_MLC_SetLockSize(module, layer, CFG_DISP_PRI_MLC_LOCKSIZE);
		NX_MLC_SetDirtyFlag(module, layer);
	}

	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
	NX_MLC_SetTopDirtyFlag(module);

	/* DPC ON */
	NX_DPC_SetDPCEnable(module, CTRUE);
	NX_DPC_SetClockDivisorEnable(module, CTRUE);	// CLKENB : Provides internal operating clock.

	/* some delay */
	for (delay = 0; 100 > 100; delay++) { ; }
}

static void dpc_set_2st_on(void)
{
	int module = 1;
	struct dpc_info *dpc;
	struct vid_data *vid;

	NX_DPC_DITHER RDither, GDither, BDither;
	uint  DPCFmt = CFG_DISP_SEC_OUT_FORMAT;
	bool  bESync = CFALSE;
    uint  XResol = CFG_DISP_SEC_RESOL_WIDTH;
    uint  YResol = CFG_DISP_SEC_RESOL_HEIGHT;
	uint  YActiv = YResol/2;
	int   layer;

	uint  bgcolor;
	int   priority;

	set_dpc(&dpc, module);
	set_vid(&vid, module);

	bgcolor  = dpc->bgcolor;
	priority = vid->priority;

	DBGOUT("%s (m:%d)\n", __func__, module);

	/* Set DPC */
   	if (((uint)NX_DPC_FORMAT_RGB555   == DPCFmt) ||
		((uint)NX_DPC_FORMAT_MRGB555A == DPCFmt) ||
		((uint)NX_DPC_FORMAT_MRGB555B == DPCFmt) ) {
		RDither = GDither = BDither = NX_DPC_DITHER_5BIT;
	} else if (((uint)NX_DPC_FORMAT_RGB565  == DPCFmt) ||
			   ((uint)NX_DPC_FORMAT_MRGB565 == DPCFmt) ) {
		RDither = BDither = NX_DPC_DITHER_5BIT;
		GDither           = NX_DPC_DITHER_6BIT;
	} else if (((uint)NX_DPC_FORMAT_RGB666  == DPCFmt) ||
			   ((uint)NX_DPC_FORMAT_MRGB666 == DPCFmt) ) {
		RDither = GDither = BDither = NX_DPC_DITHER_6BIT;
	} else {
		RDither = GDither = BDither = NX_DPC_DITHER_BYPASS;
	}

	//	SECONDARY DPC Contoller
	NX_DPC_SetClockPClkMode(module, NX_PCLKMODE_DYNAMIC);
	NX_DPC_SetDPCEnable(module, CFALSE);
	NX_DPC_SetClockDivisorEnable(module, CFALSE);

	#if (CFG_DISP_SEC_SCALE_UP_ENABLE == CTRUE)
		if(Width > CFG_DISP_PRI_RESOL_WIDTH)
			NX_DPC_SetHorizontalUpScaler(module, CTRUE, CFG_DISP_PRI_RESOL_WIDTH, Width);
		else
			NX_DPC_SetHorizontalUpScaler(module, CFALSE, 2, 2);
	#else
		NX_DPC_SetHorizontalUpScaler(module, CFALSE, 2, 2);
	#endif

	// Internal Encoder Mode
    // VCLK2 : CLKGEN0
    NX_DPC_SetClockSource  (module, 0, CFG_DISP_SEC_CLKGEN0_SOURCE);	// CLKSRCSEL
    NX_DPC_SetClockDivisor (module, 0, CFG_DISP_SEC_CLKGEN0_DIV);		// CLKDIV
    NX_DPC_SetClockOutDelay(module, 0, CFG_DISP_SEC_CLKGEN0_DELAY); 	// OUTCLKDELAY

    // VCLK : CLKGEN1
    NX_DPC_SetClockSource  (module, 1, CFG_DISP_SEC_CLKGEN1_SOURCE);	// CLKSRCSEL  : CLKGEN0's out
    NX_DPC_SetClockDivisor (module, 1, CFG_DISP_SEC_CLKGEN1_DIV);		// CLKDIV
    NX_DPC_SetClockOutDelay(module, 1, CFG_DISP_SEC_CLKGEN1_DELAY);	// OUTCLKDELAY

   	NX_DPC_SetMode(module,
			(NX_DPC_FORMAT)DPCFmt,						// FORMAT
   	        CFG_DISP_SEC_OUT_INTERLACE,					// SCANMODE
   	        CFG_DISP_SEC_OUT_POL_INVERT,				// POLFIELD
   	        CFG_DISP_SEC_OUT_RGB, 						// RGBMODE
   	        CFALSE,       								// SWAPRB
   	        (NX_DPC_YCORDER)CFG_DISP_SEC_OUT_YCORDER,	// YCORDER
   	        (bESync) ? CTRUE : CFALSE,					// YCCLIP
   	        bESync,  									// Embedded sync
   	        (NX_DPC_PADCLK)get_disp_pri_padclksel(),		// PADCLKSEL
   	        CFG_DISP_SEC_OUT_CLK_INVERT,				// PADCLKINV
			CFG_DISP_SEC_OUT_DUAL_VIEW);

   	NX_DPC_SetHSync(module,
			XResol,
			CFG_DISP_SEC_HSYNC_SYNC_WIDTH,
			CFG_DISP_SEC_HSYNC_FRONT_PORCH,
			CFG_DISP_SEC_HSYNC_BACK_PORCH,
			CFG_DISP_SEC_HSYNC_ACTIVE_HIGH);

   	NX_DPC_SetVSync(module,
			YActiv,
			CFG_DISP_SEC_VSYNC_SYNC_WIDTH,
			CFG_DISP_SEC_VSYNC_FRONT_PORCH,
			CFG_DISP_SEC_VSYNC_BACK_PORCH,
			CFG_DISP_SEC_VSYNC_ACTIVE_HIGH,
    		CFG_DISP_SEC_EVSYNC_ACTIVE_HEIGHT,
			CFG_DISP_SEC_EVSYNC_SYNC_WIDTH,
			CFG_DISP_SEC_EVSYNC_FRONT_PORCH,
			CFG_DISP_SEC_EVSYNC_BACK_PORCH);

	NX_DPC_SetVSyncOffset(module,
			CFG_DISP_SEC_VSYNC_START_OFFSET,
			CFG_DISP_SEC_VSYNC_END_OFFSET,
			CFG_DISP_SEC_EVSYNC_START_OFFSET,
			CFG_DISP_SEC_EVSYNC_END_OFFSET);

	NX_DPC_SetDelay(module,
		  	CFG_DISP_SEC_SYNC_DELAY_RGB_PVD,	// DELAYRGB
       	  	CFG_DISP_SEC_SYNC_DELAY_HS_CP1,		// DELAYHS_CP1
        	CFG_DISP_SEC_SYNC_DELAY_VS_FRAM,	// DELAYVS_FRAM
	        CFG_DISP_SEC_SYNC_DELAY_DE_CP2);	// DELAYDE_CP2

   	NX_DPC_SetDither(module, RDither, GDither, BDither);

	//--------------------------------------------------------------------------
	/* Set MLC */
	NX_MLC_SetClockPClkMode(module, NX_PCLKMODE_DYNAMIC);
	NX_MLC_SetClockBClkMode(module, NX_BCLKMODE_DYNAMIC);
	NX_MLC_SetLayerPriority(module, (NX_MLC_PRIORITY)priority);
	NX_MLC_SetBackground(module, bgcolor);
	NX_MLC_SetFieldEnable(module, CFG_DISP_SEC_MLC_INTERLACE);
	NX_MLC_SetScreenSize(module, XResol, YResol);
	NX_MLC_SetRGBLayerGamaTablePowerMode(module, CFALSE, CFALSE, CFALSE);
	NX_MLC_SetRGBLayerGamaTableSleepMode(module, CTRUE, CTRUE, CTRUE);
	NX_MLC_SetRGBLayerGammaEnable(module, CFALSE);
	NX_MLC_SetDitherEnableWhenUsingGamma(module, CFALSE);
	NX_MLC_SetGammaPriority(module, CFALSE);
    NX_MLC_SetTopPowerMode(module, CTRUE);
    NX_MLC_SetTopSleepMode(module, CFALSE);
	NX_MLC_SetMLCEnable(module, CTRUE);

	// Set layer dirty flag
	for (layer = 0; RGB_LAYERS > layer ; layer++) {
		NX_MLC_SetLockSize(module, layer, CFG_DISP_SEC_MLC_LOCKSIZE);
		NX_MLC_SetDirtyFlag(module, layer);
	}

	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
	NX_MLC_SetTopDirtyFlag(module);

	//--------------------------------------------------------------------------
	NX_DPC_SetENCEnable(module, CTRUE);
    mdelay(100);
 	NX_DPC_SetClockDivisorEnable(module, CTRUE);
	mdelay(100);
    NX_DPC_SetENCEnable(module, CFALSE);
    mdelay(100);
    NX_DPC_SetClockDivisorEnable(module, CFALSE);
    mdelay(100);
    NX_DPC_SetENCEnable(module, CTRUE);

	NX_DPC_SetVideoEncoderPowerDown(module, CTRUE);
    NX_DPC_SetVideoEncoderMode(module, (NX_DPC_VBS)CFG_DISP_SEC_ENC_OUTMODE, CFG_DISP_SEC_ENC_PEDESTAL);
    NX_DPC_SetVideoEncoderFSCAdjust(module, 0 );
    NX_DPC_SetVideoEncoderBandwidth(module,
    		(NX_DPC_BANDWIDTH)CFG_DISP_SEC_ENC_Y_BANDWIDTH,
			(NX_DPC_BANDWIDTH)CFG_DISP_SEC_ENC_C_BANDWIDTH);
    NX_DPC_SetVideoEncoderColorControl(module, 0, 0, 0, 0, 0 );
    NX_DPC_SetVideoEncoderTiming(module,
			CFG_DISP_SEC_HSYNC_START,
            CFG_DISP_SEC_HSYNC_END,
            CFG_DISP_SEC_VSYNC_START,
            CFG_DISP_SEC_VSYNC_END);
    NX_DPC_SetVideoEncoderPowerDown(module, CFALSE);
    NX_DPC_SetENCEnable(module, CTRUE);

	//--------------------------------------------------------------------------
	/* DPC ON */
	NX_DPC_SetDPCEnable(module, CTRUE);
    NX_DPC_SetClockDivisorEnable(module, CTRUE); // CLKENB : Provides internal operating clock.
}

static void dpc_set_1st_off(void)
{
	int layer;
	int wait   = 60;
	int module = 0;

	DBGOUT("%s (m:%d)\n", __func__, module);

	/*	MLC layer OFF */
	for (layer = 0; RGB_LAYERS > layer; layer++) {
		NX_MLC_SetLayerEnable(module, layer, (layer == CFG_DISP_LAYER_SCREEN) ? CTRUE : CFALSE);
		NX_MLC_SetDirtyFlag(module, layer);
	}

	NX_MLC_SetLayerEnable(module, VIDEO_LAYER, CFALSE);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);

   	NX_MLC_SetTopPowerMode(module, CFALSE);
   	NX_MLC_SetTopSleepMode(module, CTRUE);
	NX_MLC_SetMLCEnable(module, CFALSE);
	NX_MLC_SetTopDirtyFlag(module);

	/* disable interrupt */
	dpc_set_irq_disable(module);

	/* DPC */
	NX_DPC_SetDPCEnable(module, CFALSE);
	NX_DPC_SetDirtyFlag(module);

	while (wait-- > 0 && CTRUE == NX_DPC_GetDirtyFlag(module))
		mdelay(1);

	NX_DPC_SetClockPClkMode(module, NX_PCLKMODE_DYNAMIC);
	NX_DPC_SetClockDivisorEnable(module, CFALSE);

	/* enable interrupt */
	dpc_set_irq_enable(module);
}

static void dpc_set_2st_off(void)
{
	int layer;
	int wait   = 60;
	int module = 1;

	DBGOUT("%s (m:%d)\n", __func__, module);

	/*	MLC layer OFF */
	for (layer = 0; RGB_LAYERS > layer; layer++) {
		NX_MLC_SetLayerEnable(module, layer, CFALSE);
		NX_MLC_SetDirtyFlag(module, layer);
	}

	NX_MLC_SetLayerEnable(module, VIDEO_LAYER, CFALSE);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);

   	NX_MLC_SetTopPowerMode(module, CFALSE);
   	NX_MLC_SetTopSleepMode(module, CTRUE);
	NX_MLC_SetMLCEnable(module, CFALSE);
	NX_MLC_SetTopDirtyFlag(module);

	/* disable interrupt */
	dpc_set_irq_disable(module);

	NX_DPC_ClearInterruptPendingAll(module);
	NX_DPC_SetInterruptEnableAll(module, CTRUE);

	/* wait vertical sync */
	while (wait-- > 0 && CFALSE == NX_DPC_GetInterruptPendingAll(module))
		mdelay(1);

	NX_DPC_SetDPCEnable(module, CFALSE);
	NX_DPC_SetClockPClkMode(module, NX_PCLKMODE_DYNAMIC);
	NX_DPC_SetClockDivisorEnable(module, CFALSE);

	/* enable interrupt */
	dpc_set_irq_enable(module);
}

static void dpc_set_dev_enable(int module)
{
	(0 == module) ? dpc_set_1st_on() : dpc_set_2st_on();
}

static void dpc_set_dev_disable(int module)
{
	(0 == module) ? dpc_set_1st_off() : dpc_set_2st_off();
}

static void dpc_set_vid_alpha(int module, int depth, bool on)
{
	if (depth <= 0 )	depth = 0;
	if (depth >= 15)	depth = 15;

	NX_MLC_SetAlphaBlending(module, VIDEO_LAYER, (on?CTRUE:CFALSE), depth);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
}

static void dpc_set_vid_luenhance(int module, int bright, int contra)
{
	DBGOUT("%s (module:%d, contrast:%d, bright:%d)\n", __func__, module, contra, bright);

	if (contra >=    7) contra =    7;
	if (contra <=    0)	contra =    0;
	if (bright >=  127) bright =  127;
	if (bright <= -128) bright = -128;

	NX_MLC_SetVideoLayerLumaEnhance(module, (uint)contra, (S32)bright);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
}

static void dpc_set_vid_cbrenhance(int module, int cba, int cbb, int cra, int crb)
{
	int i;

	DBGOUT("%s (module:%d, cba:%d, cbb:%d, cra:%d, crb:%d)\n",
		__func__, module, cba, cbb, cra, crb);

	if (cba > 127) cba=127; if (cba <= -128) cba=-128;
	if (cbb > 127) cbb=127; if (cbb <= -128) cbb=-128;
	if (cra > 127) cra=127; if (cra <= -128) cra=-128;
	if (crb > 127) crb=127; if (crb <= -128) crb=-128;

	for (i=1; 5 > i; i++)
		NX_MLC_SetVideoLayerChromaEnhance(module, i, cba, cbb, cra, crb);

	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
}

/*------------------------------------------------------------------------------
 * IRQ handler
 */
static irqreturn_t	dpc_irq_handler(int irq, void *desc)
{
	struct dpc_info *dpc = desc;

	dpc->cond = 1;
	dpc->irq_count++;
	wake_up_interruptible(&dpc->wait_q);
	NX_DPC_ClearInterruptPendingAll(irq);
	GET_REAL_TIME(dpc->times.sync);

	/* check irq duration */
#if (0)
	{
		static long long pre_ts = 0;
		long long new_ts = dpc->times.sync;
		if (pre_ts && (new_ts - pre_ts) > (dpc->duration + 2*1000))
			printk(KERN_ERR "long sync: %06llu us (druation=%06u us)\n",
				(new_ts - pre_ts), dpc->duration);
		pre_ts = new_ts;
	}
#endif
	return IRQ_HANDLED;
}

inline static int dpc_wait_vsync(int module)
{
	struct dpc_info *dpc;
	int ret = -1;

	set_dpc(&dpc, module);

	if (NX_DPC_GetDPCEnable(module)) {
		wait_queue_head_t  *wq = &dpc->wait_q;
		struct dpc_sync_ts *ts = &dpc->times;

		struct task_struct * tsk = current;
		struct sched_param param = { .sched_priority = 1 };
		int priority = tsk->rt_priority;
		int policy   = tsk->policy & 0xF;

		dpc->cond = 0;	/* clear condition */

		/* change schedule */
		sched_setscheduler(tsk, SCHED_FIFO, &param);

		GET_REAL_TIME(ts->innr);
		ret = wait_event_interruptible_timeout(*wq, dpc->cond, dpc->wait_time);
		GET_REAL_TIME(ts->wait);
		if (0 == dpc->cond || 0 == ret) {
			printk(KERN_ERR "fail, wait vsync dev:%d, time:%s, cnd:%d, wait:%06llu (us)\n",
				module, !ret?"out":"remain", dpc->cond, (ts->wait - ts->innr));
			if (dpc->cond)
				printk(KERN_ERR "start~sync:%06llu, sync~end:%06llu, start~end:%06llu (us)\n",
					(ts->sync - ts->innr), (ts->wait - ts->sync), (ts->wait - ts->innr));
		}
		/* recover schedule */
		param.sched_priority = priority;
		sched_setscheduler(tsk, policy, &param);
	}
	return ret;	/* 0: success, -1: fail */
}

inline static int dpc_wait_dirtyclear(int module, int layer)
{
	return dpc_wait_vsync(module);
}

/*----------------------------------------------------------------------------*/
void soc_dpc_init(void)
{
	struct dpc_info *dpc;
	int num, ret = 0;
	int i, l;

	set_dpc(&dpc, 0);
	num = NX_MLC_GetNumberOfModule();

	for (i = 0; num > i; i++, dpc++) {
		uint hfp = 0, hbp = 0, hsw = 0, xres = 0;
		uint vfp = 0, vbp = 0, vsw = 0, yres = 0;
		uint pck = 0, fps = 0, dur = 0;

		if (NULL == dpc->name)
			continue;

		/* set prototype base */
		NX_MLC_SetBaseAddress(i, (U32)IO_ADDRESS(NX_MLC_GetPhysicalAddress(i)));
		NX_DPC_SetBaseAddress(i, (U32)IO_ADDRESS(NX_DPC_GetPhysicalAddress(i)));
		NX_MLC_OpenModule(i);
		NX_DPC_OpenModule(i);

		/* set module's base */
		dpc->regs.mlc_base = (uint*)NX_MLC_GetBaseAddress(i);
		dpc->regs.dpc_base = (uint*)NX_DPC_GetBaseAddress(i);
		dpc->regs.mlc_save = (uint*)&mlc_reg_save[0];
		dpc->regs.dpc_save = (uint*)&dpc_reg_save[0];

		xres = dpc->x_res, hfp = dpc->h_fp, hbp = dpc->h_bp, hsw = dpc->h_sw;
		yres = dpc->y_res, vfp = dpc->v_fp,	vbp = dpc->v_bp, vsw = dpc->v_sw;

		/* calculate effective pixel clock per PLL divider */
		if (0 == i)
			dpc->pixelclock  = cpu_get_clock_hz((cfg_sys_clksrc)CFG_DISP_PRI_CLKGEN0_SOURCE) / (get_pri_clkgen0_div());

		pck  = dpc->pixelclock;
		fps	 = pck ? (pck)/((xres+hfp+hbp+hsw) * (yres+vfp+vbp+vsw)) : 60;
		fps *= 100; /* no pre-multiply overflow */
		dur  = (1000 * 100 * 10)/fps;

		/* adjust blank timings if not near 60 Hz */
		if (pck && (fps < 5900 || fps > 6100)) {
			uint dx = dpc->pixelclock / (60 * (yres+vfp+vbp+vsw));
			int  dy = dx - (xres+hfp+hbp+hsw);
			if (hbp + dy > 0)
				hbp += dy;
			else
				hbp = 1;
			dpc->h_bp = hbp;
			/* recalculate vertical refresh with adjusted sync */
			fps	 = (pck)/((xres+hfp+hbp+hsw) * (yres+vfp+vbp+vsw));
			fps *= 100;
			dur  = (1000 * 100 * 10)/fps;
		}

		/* set module's layer data */
		for (l = 0; ALL_LAYERS > l; l++) {
			if (RGB_LAYERS == l)
				dpc->ldata[l] = &vid_data[i];		/* Video layer */
			else
				dpc->ldata[l] = &rgb_data[i][l];	/* RGB layer */
		}

		/* register irq */
		init_waitqueue_head(&dpc->wait_q);
		ret = request_irq(dpc->irq, &dpc_irq_handler, IRQF_DISABLED, dpc->name, dpc);
		if (ret) {
			printk(KERN_ERR "fail, dpc[%d] request irq %d ...\n", i, dpc->irq);
			continue;
		}
		dpc->wait_time = msecs_to_jiffies(dur/10) + 1;	/* +1 jiffies */
		dpc->duration  = (dur * 100);					/* usec */

		/* enable source irq */
		NX_DPC_SetInterruptEnableAll(i, CTRUE);

		printk(KERN_INFO"dpc%d: irq=%d, fps=%d.%d, duration=%d.%dms, %djiffies (%u, %d+%d+%d, %d+%d+%d)\n",
			i, dpc->irq, fps/100, fps%100, dpc->duration/1000, dpc->duration%1000,
			dpc->wait_time, pck, hfp, xres, hbp+hsw, vfp, yres, vbp+vsw);
	}

#if defined(CONFIG_PLAT_NXP3200_M2K) || \
	defined(CONFIG_PLAT_NXP3200_L2K) || \
	defined(CONFIG_PLAT_NXP3200_VALENCIA_CIP) || \
	defined(CONFIG_PLAT_NXP3200_FAVELA) || \
	defined(CONFIG_PLAT_NXP3200_RIO) || \
	defined(CONFIG_PLAT_NXP3200_BID)
	/* init LCD via SPI */
	spi_lcd_init();
	spi_lcd_setup();
	soc_dpc_set_layer_enable(0, CFG_DISP_LAYER_SCREEN, 1);
#endif
}

void soc_dpc_exit(void)
{
	struct dpc_info *dpc;
	int i, num;

	set_dpc(&dpc, 0);
	num = NX_MLC_GetNumberOfModule();
	for (i = 0; num > i; i++, dpc++)
		free_irq(dpc->irq, NULL);
}

/*------------------------------------------------------------------------------
 * Display output control
 */
int soc_dpc_set_timings(int module, int clk, int hsw, int hfp, int hbp, int vsw, int vfp, int vbp)
{
	int clksrc, divisor;
	struct dpc_info *dpc;
	DBGOUT("%s (m:%d)\n", __func__, module);
	set_dpc(&dpc, module);

	if (clk == 0)
		return 0;

	clksrc = cpu_get_clock_hz(CFG_DISP_PRI_CLKGEN0_SOURCE);
	divisor = clksrc / clk;
	if (divisor & 1)
		divisor--;
	printk(KERN_INFO "%s src=%d, clk=%d, div=%d\n", __func__, clksrc, clk, divisor);
	if (divisor == 0)
		return 0;

	NX_DPC_SetDPCEnable(module, CFALSE);
	NX_DPC_SetClockDivisorEnable(module, CFALSE);
	NX_DPC_SetClockDivisor(module, 0, divisor);
	NX_DPC_SetHSync(module, dpc->x_res, hsw, hfp, hbp,
			CFG_DISP_PRI_HSYNC_ACTIVE_HIGH);
	NX_DPC_SetVSync(module, dpc->y_res, vsw, vfp, vbp,
			CFG_DISP_PRI_VSYNC_ACTIVE_HIGH,
			CFG_DISP_PRI_EVSYNC_ACTIVE_HEIGHT,
			CFG_DISP_PRI_EVSYNC_SYNC_WIDTH,
			CFG_DISP_PRI_EVSYNC_FRONT_PORCH,
			CFG_DISP_PRI_EVSYNC_BACK_PORCH);
	NX_DPC_SetClockDivisorEnable(module, CTRUE);
	NX_DPC_SetDPCEnable(module, CTRUE);
	NX_DPC_SetDirtyFlag(module);

	return 1;
}

int soc_dpc_get_timings(int module, int* clk, int* hsw, int* hfp, int* hbp, int* vsw, int* vfp, int* vbp)
{
	struct dpc_info *dpc;
	DBGOUT("%s (m:%d)\n", __func__, module);
	set_dpc(&dpc, module);

	*clk = dpc->pixelclock;
	*hsw = dpc->h_sw;
	*hfp = dpc->h_fp;
	*hbp = dpc->h_bp;
	*vsw = dpc->v_sw;
	*vfp = dpc->v_fp;
	*vbp = dpc->v_bp;

	return 1;
}

int	soc_dpc_get_vblank(int module)
{
	struct dpc_info *dpc;
	DBGOUT("%s (m:%d)\n", __func__, module);
	set_dpc(&dpc, module);
	return dpc->irq_count;
}

uint soc_dpc_get_mode(void)
{
	int mode = CFG_DISP_MAIN_SCREEN;

	/*
	 * 0 : 1st main screen
	 * 1 : 2st main screen
	 * 2 : clone mode
	 * 3 : multi monitor
	 */
	DBGOUT("%s (mode=%d)\n", __func__, mode);
	return mode;
}

void soc_dpc_get_resolution(int module, int *x_res, int *y_res)
{
	struct dpc_info *dpc;
	DBGOUT("%s (m:%d)\n", __func__, module);
	set_dpc(&dpc, module);
	*x_res = dpc->x_res;
	*y_res = dpc->y_res;
}

void soc_dpc_set_bgcolor(int module, uint color)
{
	struct dpc_info *dpc;

	set_dpc(&dpc, module);
	DBGOUT("%s (m:%d, c:0x%x)\n", __func__, module, color);

	dpc->bgcolor = color;
	NX_MLC_SetBackground(module, color & 0x00FFFFFF);
	NX_MLC_SetTopDirtyFlag(module);
}

uint soc_dpc_get_bgcolor(int module)
{
	struct dpc_info *dpc;
	set_dpc(&dpc, module);
	DBGOUT("%s (m:%d)\n", __func__, module);
	return (uint)dpc->bgcolor;
}

void soc_dpc_wait_vsync(int module)
{
	dpc_wait_vsync(module);
}

void soc_dpc_set_out_enable(int module)
{
	struct dpc_info *dpc;

	set_dpc(&dpc, module);
	DBGOUT("%s (m:%d)\n", __func__, module);
	/* check status */
	if (CTRUE == NX_DPC_GetDPCEnable(module)) {
		printk("dpc:%d is already enabled ...\n", module);
		return;
	}

	/*
	 * step: display -> lcd -> backlight
	 */
	dpc_set_dev_enable(module);

	if (dpc->lcdops && dpc->lcdops->enable)
		dpc->lcdops->enable(module);

	if (dpc->bluops && dpc->bluops->enable)
		dpc->bluops->enable(module);

	/* change status */
	dpc->status	= STAT_ENABLE;
}

void soc_dpc_set_out_disable(int module)
{
	struct dpc_info *dpc;

	set_dpc(&dpc, module);
	DBGOUT("%s (m:%d)\n", __func__, module);

	/* check status */
	if (CFALSE == NX_DPC_GetDPCEnable(module))
		return;

	/*
	 * step: backlight -> display -> lcd
	 */
	if (dpc->bluops && dpc->bluops->disable)
		dpc->bluops->disable(module);

	dpc_set_dev_disable(module);

	if (dpc->lcdops && dpc->lcdops->disable)
		dpc->lcdops->disable(module);

	/* change status */
	dpc->status	= STAT_DISABLE;
}

void soc_dpc_set_suspend(int module)
{
	struct dpc_info *dpc;
	volatile uint *mlc_a;
	volatile uint *dpc_a;

	uint mlc_s = NX_MLC_GetSizeOfRegisterSet();
	uint dpc_s = NX_DPC_GetSizeOfRegisterSet();

	set_dpc(&dpc, module);
	PM_DBGOUT("+%s (m:%d)\n", __func__, module);

	mlc_a = dpc->regs.mlc_base;
	dpc_a = dpc->regs.dpc_base;

#if DUMP_REG
	dbg_reg_dpc(module);
	dbg_reg_mlc(module);
#endif

	/* save MLC: save layer enable status */
	memcpy((void*)dpc->regs.mlc_save, (const void*)mlc_a, mlc_s);

	/*
	 * step: backlight -> display -> lcd
	 */
	if (dpc->bluops && dpc->bluops->suspend)
		dpc->bluops->suspend(module);

	dpc_set_dev_disable(module);

	if (dpc->lcdops && dpc->lcdops->suspend)
		dpc->lcdops->suspend(module);

	/* save DPC: save clock disable status */
	memcpy((void*)dpc->regs.dpc_save, (const void*)dpc_a, dpc_s);

	/* change status */
	dpc->status |= STAT_SUSPEND;
	dpc->status &= ~STAT_RESUME;

	PM_DBGOUT("-%s (m:%d)\n", __func__, module);
}

void soc_dpc_set_resume(int module)
{
	struct dpc_info *dpc;
	volatile uint *mlc_a;
	volatile uint *dpc_a;

	uint mlc_s = NX_MLC_GetSizeOfRegisterSet();
	uint dpc_s = NX_DPC_GetSizeOfRegisterSet();

	int i = 0, wait = 60;

	set_dpc(&dpc, module);
	mlc_a = dpc->regs.mlc_base;
	dpc_a = dpc->regs.dpc_base;

	PM_DBGOUT("+%s (m:%d)\n", __func__, module);

	/* restore register */
	for (i = 0; mlc_s/4 > i; i++)
		WriteIODW((mlc_a+i), dpc->regs.mlc_save[i]);

	for (i = 0; dpc_s/4 > i; i++)
		WriteIODW((dpc_a+i), dpc->regs.dpc_save[i]);

	/*
	 * step: display -> lcd -> backlight
	 */
	NX_MLC_SetTopPowerMode(module, CTRUE);
   	NX_MLC_SetTopSleepMode(module, CFALSE);
	NX_MLC_SetMLCEnable(module, CTRUE);

	for (i = 0; RGB_LAYERS > i; i++)
		NX_MLC_SetDirtyFlag(module, i);

	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);

	NX_MLC_SetTopDirtyFlag(module);

	/* disable interrupt */
	dpc_set_irq_disable(module);

	NX_DPC_ClearInterruptPendingAll(module);
	NX_DPC_SetInterruptEnableAll(module, CTRUE);
	NX_DPC_SetClockDivisorEnable(module, CTRUE);
	NX_DPC_SetDPCEnable(module, CTRUE);

	while (wait-- > 0 &&
		   CFALSE == NX_DPC_GetInterruptPendingAll(module))
		mdelay(1);

	if (dpc->lcdops && dpc->lcdops->enable)
		dpc->lcdops->resume(module);

	if (dpc->bluops && dpc->bluops->enable)
		dpc->bluops->resume(module);

	/* enable interrupt */
	dpc_set_irq_enable(module);

	/* change status */
	dpc->status &= ~STAT_SUSPEND;
	dpc->status |= STAT_RESUME;

#if DUMP_REG
	dbg_reg_dpc(module);
	dbg_reg_mlc(module);
#endif

	PM_DBGOUT("-%s (m:%d)\n", __func__, module);
}

/*------------------------------------------------------------------------------
 * Display layer "RGB and VIDEO" control
 */
void soc_dpc_set_layer_enable(int module, int layer, int enable)
{
	CBOOL hl, hc, vl, vc;

	DBGOUT("%s (m:%d, l:%d)\n", __func__, module, layer);
	if (VIDEO_LAYER == layer) {
		/* Filter */
		if (enable) {
        	NX_MLC_SetVideoLayerLineBufferPowerMode(module, CTRUE);
   	 		NX_MLC_SetVideoLayerLineBufferSleepMode(module, CFALSE);
 			NX_MLC_SetLayerEnable(module, layer, CTRUE);
			NX_MLC_SetDirtyFlag(module, layer);
		} else {
	 		NX_MLC_SetLayerEnable(module, layer, CFALSE);
			NX_MLC_SetDirtyFlag(module, layer);
#if 0
			dpc_wait_dirtyclear(module, layer);

        	NX_MLC_GetVideoLayerScaleFilter(module, &hl, &hc, &vl, &vc);
			if (hl | hc | vl | vc)
				NX_MLC_SetVideoLayerScaleFilter(module, 0, 0, 0, 0);
	     	NX_MLC_SetVideoLayerLineBufferPowerMode(module, CFALSE);
   			NX_MLC_SetVideoLayerLineBufferSleepMode(module, CTRUE);
			NX_MLC_SetDirtyFlag(module, layer);
			dpc_wait_dirtyclear(module, layer);
#endif
		}
	/* RGB layer */
	} else {
 		NX_MLC_SetLayerEnable(module, layer, (enable ? CTRUE : CFALSE));
		NX_MLC_SetDirtyFlag(module, layer);
	}
}

int soc_dpc_get_layer_enable(int module, int layer)
{
	DBGOUT("%s (m:%d, l:%d)\n", __func__, module, layer);
	return NX_MLC_GetLayerEnable(module, layer) ? 1 : 0;
}

void soc_dpc_wait_dirtyclear(int module, int layer)
{
	DBGOUT("%s (m:%d, l:%d)\n", __func__, module, layer);
	dpc_wait_dirtyclear(module, layer);
}

/*------------------------------------------------------------------------------
 * Display layer "VIDEO" control
 */
int soc_dpc_set_vid_format(int module, uint fourcc, int srcw, int srch)
{
	struct vid_data *vid;
	uint format;

	set_vid(&vid, module);
	DBGOUT("%s (m:%d)\n", __func__, module);
	switch (fourcc) {
	case FOURCC_MVS0:
	case FOURCC_YV12: format = NX_MLC_YUVFMT_420;
		break;
	case FOURCC_MVS2:
	case FOURCC_MVN2: format = NX_MLC_YUVFMT_422;
		break;
	case FOURCC_MVS4: format = NX_MLC_YUVFMT_444;
		break;
	case FOURCC_YUY2:
	case FOURCC_YUYV: format = NX_MLC_YUVFMT_YUYV;
		break;
	default:
		printk(KERN_ERR "fail, not support video format (0x%08x)...\n", fourcc);
		return (-1);
	}

	if (0>= srcw || 0 >= srch) {
		printk(KERN_ERR "fail, (%d) invalid source size (%d * %d) \n",
			module, srcw, srch);
		return (-1);
	}

	/* set video data. */
	vid->fourcc = fourcc;
	vid->srcw   = srcw;
	vid->srch   = srch;

	NX_MLC_SetFormatYUV(module, (NX_MLC_YUVFMT)format);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
	return 0;
}

void soc_dpc_get_vid_format(int module, uint *fourcc, int *srcw, int *srch)
{
	struct vid_data *vid;

	set_vid(&vid, module);
	DBGOUT("%s (m:%d)\n", __func__, module);
	*fourcc = vid->fourcc;
	*srcw   = vid->srcw;
	*srch   = vid->srch;
}

void soc_dpc_set_vid_address(
	int module, uint lu_a, uint lu_s, uint cb_a, uint cb_s,
	uint cr_a,	uint cr_s, int waitvsync
	)
{
	struct vid_data *vid;
	int  layer = VIDEO_LAYER;

	set_vid(&vid, module);
	DBGOUT("%s (m:%d, lua:0x%x, cba:0x%x, cra:0x%x - lus:%d, cbs:%d, crs:%d, vs:%d)\n",
		__func__, module, lu_a, cb_a, cr_a, lu_s, cb_s, cr_s, waitvsync);

	/* set prototype */
	if (FOURCC_YUYV == vid->fourcc) {
		NX_MLC_SetVideoLayerAddressYUYV(module, lu_a, lu_s);
	} else {
		NX_MLC_SetVideoLayerStride (module, lu_s, cb_s, cr_s);
		NX_MLC_SetVideoLayerAddress(module, lu_a, cb_a, cr_a);
	}

	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		dpc_wait_dirtyclear(module, layer);
}

void 	soc_dpc_get_vid_address(int module, u_int*lu_a, u_int*lu_s, u_int*cb_a, u_int*cb_s,
								u_int*cr_a,	u_int*cr_s)
{
	register struct NX_MLC_RegisterSet *pMLC =
		(struct NX_MLC_RegisterSet*)IO_ADDRESS(NX_MLC_GetPhysicalAddress(module));
	struct vid_data *vid;

	set_vid(&vid, module);
	if (FOURCC_YUYV == vid->fourcc) {
		*cb_a = 0, *cb_s = 0, *cr_a = 0, *cr_s = 0;
		*lu_a = ReadIODW(&pMLC->MLCVIDEOLAYER.MLCADDRESS);
		*lu_s = ReadIODW(&pMLC->MLCVIDEOLAYER.MLCVSTRIDE);
	} else {
		*lu_a =	ReadIODW(&pMLC->MLCVIDEOLAYER.MLCADDRESS);
		*cb_a =	ReadIODW(&pMLC->MLCVIDEOLAYER.MLCADDRESSCB);
		*cr_a =	ReadIODW(&pMLC->MLCVIDEOLAYER.MLCADDRESSCR);
		*lu_s = ReadIODW(&pMLC->MLCVIDEOLAYER.MLCVSTRIDE);
		*cb_s = ReadIODW((volatile U32 *)&pMLC->MLCVIDEOLAYER.MLCVSTRIDECB);
		*cr_s = ReadIODW((volatile U32 *)&pMLC->MLCVIDEOLAYER.MLCVSTRIDECR);
	}
}

int soc_dpc_set_vid_position(
	int module, int left, int top, int right, int bottom, int waitvsync
	)
{
	struct vid_data *vid;
	int layer = VIDEO_LAYER;
	int srcw = 0, srch = 0;
	int dstw = 0, dsth = 0;
	int hf = 0, vf = 1;

	set_vid(&vid, module);
	srcw = vid->srcw;
	srch = vid->srch;
	dstw = right - left;
	dsth = bottom - top;

	if (! vid->fourcc) {
		printk(KERN_ERR "fail, not set video format for %d module ...\n", module);
		return (-1);
	}
	DBGOUT("%s (m:%d, l:%d, t:%d, r:%d, b:%d, vs:%d)\n",
		__func__, module, left, top, right, bottom, waitvsync);

	/* check position factor */
	if (left >= right ) right  = left+1;
	if (top  >= bottom) bottom = top +1;
	if (0    >= right )	left  -= (right -0),  right  = 1;
	if (0    >= bottom)	top   -= (bottom-0), bottom = 1;

	/* check scale factor */
	if (dstw >= 2048) dstw = 2048;
	if (dsth >= 2048) dsth = 2048;

	/* set filter factor */
	if (srcw == dstw && srch == dsth)
		hf = 0, vf = 0;

	if (dstw > srcw)
		hf = 1, vf = 1;

	vid->h_filter = hf;
	vid->v_filter = vf;

	/* set scale */
	NX_MLC_SetVideoLayerScale(module, srcw, srch, dstw, dsth, (CBOOL)hf, (CBOOL)hf, (CBOOL)vf, (CBOOL)vf);
	NX_MLC_SetPosition(module, layer, left, top, right-1, bottom-1);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		dpc_wait_dirtyclear(module, layer);

	return 0;
}

int soc_dpc_get_vid_position(
	int module, int *left, int *top, int *right, int *bottom
	)
{
	register struct NX_MLC_RegisterSet *pMLC =
		(struct NX_MLC_RegisterSet*)IO_ADDRESS(NX_MLC_GetPhysicalAddress(module));

	U32 lr = ReadIODW(&pMLC->MLCVIDEOLAYER.MLCLEFTRIGHT);
	U32 tb = ReadIODW(&pMLC->MLCVIDEOLAYER.MLCTOPBOTTOM);

	*left   = ((lr >> 16) & 0xFFFUL);
	*right  = ((lr >>  0) & 0xFFFUL) + 1;
	*top    = ((tb >> 16) & 0xFFFUL);
	*bottom = ((tb >>  0) & 0xFFFUL) + 1;

	return 0;
}

void soc_dpc_set_vid_enable(int module, int enable)
{
	DBGOUT("%s (m:%d, e:%d)\n", __func__, module, enable);
	soc_dpc_set_layer_enable(module, VIDEO_LAYER, enable);
}

void soc_dpc_set_vid_priority(int module, int prior)
{
	struct vid_data *vid;

	set_vid(&vid, module);
	DBGOUT("%s (m:%d, p:%d)\n", __func__, module, prior);
	switch (prior) {
	case 0:
		NX_MLC_SetLayerPriority(module, NX_MLC_PRIORITY_VIDEOFIRST);	// PRIORITY-video>0>1>2
		break;
	case 1:
		NX_MLC_SetLayerPriority(module, NX_MLC_PRIORITY_VIDEOSECOND); // PRIORITY-0>video>1>2
		break;
	case 2:
		NX_MLC_SetLayerPriority(module, NX_MLC_PRIORITY_VIDEOTHIRD);	// PRIORITY-0>1>video>2
		break;
	case 3:
		NX_MLC_SetLayerPriority(module, NX_MLC_PRIORITY_VIDEOFOURTH); // PRIORITY-0>1>2>video
		break;
	default: printk(KERN_ERR "Not support video priority num(0~3), (%d) \n", prior);
		return;
	}

	vid->priority = prior;
	NX_MLC_SetTopDirtyFlag(module);
}

int  soc_dpc_get_vid_priority(int module)
{
	struct vid_data *vid;

	set_vid(&vid, module);
	DBGOUT("%s (m:%d)\n", __func__, module);

	return (int)vid->priority;
}

void  soc_dpc_set_vid_colorkey(int module, uint color, int enable)
{
	struct vid_data *vid;
	set_vid(&vid, module);
	DBGOUT("%s (m:%d, c:0x%x, e:%d)\n", __func__, module, color, enable);
	soc_dpc_set_rgb_color(module, vid->lscreen, RGB_COL_TRANSP, color, enable);
}

uint soc_dpc_get_vid_colorkey(int module)
{
	struct vid_data *vid;

	set_vid(&vid, module);
	DBGOUT("%s (m:%d)\n", __func__, module);
	return soc_dpc_get_rgb_color(module, vid->lscreen, RGB_COL_TRANSP);
}

void soc_dpc_set_vid_color(int module, uint type, uint color, int enable)
{
	struct vid_data *vid;
	int sv,	cv;
	double ang;

	set_vid(&vid, module);
	DBGOUT("%s (m:%d, t:0x%x, c:0x%x, e:%d)\n", __func__, module, type, color, enable);
	switch (type) {
	case VID_COL_ALPHA:
		{
			vid->color.alpha = (enable ? color : 15);		// Default 15
			dpc_set_vid_alpha(module, vid->color.alpha, enable);
		}
		break;

	case VID_COL_BRIGHT:
		{
			vid->color.bright   = (enable ? color : 0);
			vid->color.contrast = (enable?vid->color.contrast:0);
			dpc_set_vid_luenhance(module, (int)vid->color.bright, (int)vid->color.contrast);
		}
		break;

	case VID_COL_CONTRAST:
		{
			vid->color.contrast = (enable ? color : 0);
			vid->color.bright   = (enable ? vid->color.bright : 0);
			dpc_set_vid_luenhance(module, (int)vid->color.bright, (int)vid->color.contrast);
		}
		break;

	case VID_COL_HUE:
		{
 			if ((int)color <=   0) color = 0;
 			if ((int)color >= 360) color = 360;

 			vid->color.hue 		  = (enable ? (int)color : 0);
 			vid->color.saturation = (enable ? vid->color.saturation : 1);

			ang = DEGREE_RADIAN(vid->color.hue);

   			sv = (ksin(ang) * 64 * vid->color.saturation);
   			cv = (kcos(ang) * 64 * vid->color.saturation);

			DBGOUT("hue(h:0x%08lx, s:0x%08lx, cv:0x%08x, sv:0x%08x)\n",
				(ulong)vid->color.hue, (ulong)vid->color.saturation, cv, sv);

   			dpc_set_vid_cbrenhance(module, cv, -sv, sv, cv);
   		}
		break;

	case VID_COL_SATURATION:
		{
	 		if ((int)color <= -100) color = -100;
			if ((int)color >=  100) color =  100;

			vid->color.hue 		  = (enable ? vid->color.hue : 0);
			vid->color.saturation = (enable ? 1 + (0.01 * (int)color) : 1);
			vid->color.satura 	  = (enable ? (int)color : 0);

			ang = (DEGREE_RADIAN(vid->color.hue));

   			sv  = (ksin(ang) * 64 * vid->color.saturation);
   			cv  = (kcos(ang) * 64 * vid->color.saturation);

			DBGOUT("saturation(h:0x%08lx, s:0x%08lx, cv:0x%08x, sv:0x%08x)\n",
				(ulong)vid->color.hue, (ulong)vid->color.saturation, cv, sv);

			dpc_set_vid_cbrenhance(module, cv, -sv, sv, cv);
		}
		break;

	case VID_COL_GAMMA:
		{
			vid->color.gamma = color;
		}
		break;

	default:
		break;
	}
}

uint  soc_dpc_get_vid_color(int module, uint type)
{
	struct vid_data *vid;

	set_vid(&vid, module);
	DBGOUT("%s (m:%d, t:%d)\n", __func__, module, type);
	switch (type) {
	case VID_COL_ALPHA:			return (uint)vid->color.alpha;
	case VID_COL_BRIGHT:		return (uint)vid->color.bright;
	case VID_COL_CONTRAST:		return (uint)vid->color.contrast;
	case VID_COL_HUE:			return (uint)vid->color.hue;
	case VID_COL_SATURATION:	return (uint)vid->color.satura;
	case VID_COL_GAMMA:			return (uint)vid->color.gamma;
	default: 					break;
	}
	return (-1);
}

void soc_dpc_set_vid_hfilter(int module, int enable)
{
	struct vid_data *vid;
	CBOOL hl, hc, vl, vc;

	set_vid(&vid, module);
	hl = hc = vid->h_filter = enable;
	vl = vc = vid->v_filter;

	NX_MLC_SetVideoLayerScaleFilter(module, hl, hc, vl, vc);
}

uint soc_dpc_get_vid_hfilter(int module)
{
	CBOOL hl, hc, vl, vc;

	NX_MLC_GetVideoLayerScaleFilter(module, &hl, &hc, &vl, &vc);

	if (hl != hc) {
		printk(KERN_INFO "%s: WARN %d horizontal filter Lu=%s, Ch=%s \r\n",
			__func__, module, hl?"On":"Off", hc?"On":"Off" );
	}
	return (uint)hl;
}

void soc_dpc_set_vid_vfilter(int module, int enable)
{
	struct vid_data *vid;
	CBOOL hl, hc, vl, vc;

	set_vid(&vid, module);
	hl = hc = vid->h_filter;
	vl = vc = vid->v_filter = enable;

	NX_MLC_SetVideoLayerScaleFilter(module, hl, hc, vl, vc);
}

uint soc_dpc_get_vid_vfilter(int module)
{
	CBOOL hl, hc, vl, vc;

	NX_MLC_GetVideoLayerScaleFilter(module, &hl, &hc, &vl, &vc);

	if (hl != hc) {
		printk(KERN_INFO "%s: WARN %d vertical filter Lu=%s, Ch=%s \r\n",
			__func__, module, vl?"On":"Off", vc?"On":"Off");
	}
	return (uint)vl;
}

/*------------------------------------------------------------------------------
 * Display layer "RGB" control
 */
int  soc_dpc_set_rgb_format(
		int module, int layer, uint format, int width, int height, int pixelbyte
		)
{
	struct rgb_data *rgb;

	set_rgb(&rgb, module, layer);
	DBGOUT("%s (m:%d, l:%d, f:0x%x, w:%d, h:%d, b:%d)\n",
		__func__, module, layer, format, width, height, pixelbyte);

	if (NX_MLC_GetLayerEnable(module, layer)) {
		printk(KERN_ERR "dpc:%d layer:%d is running ...\n", module, layer);
		return (-1);
	}

	rgb->format    = format;
	rgb->srcw	   = width;
	rgb->srch      = height;
	rgb->pixelbyte = pixelbyte;

	NX_MLC_SetAlphaBlending(module, layer, CFALSE, rgb->color.alpha);
	NX_MLC_SetTransparency(module, layer, CFALSE, rgb->color.transcolor);
	NX_MLC_SetColorInversion(module, layer, CFALSE, rgb->color.invertcolor);
	NX_MLC_SetFormatRGB(module, layer, (NX_MLC_RGBFMT)format);
	NX_MLC_SetRGBLayerInvalidPosition(module, layer, 0, 0, 0, 0, 0, CFALSE);
	NX_MLC_SetRGBLayerInvalidPosition(module, layer, 1, 0, 0, 0, 0, CFALSE);

	if (width && height)
		NX_MLC_SetPosition(module, layer, 0, 0, width-1, height-1);

	NX_MLC_SetDirtyFlag(module, layer);
	return 0;
}

void  soc_dpc_get_rgb_format(
		int module, int layer, uint *format, int *width, int *height, int *pixelbyte
		)
{
	struct rgb_data *rgb;

	set_rgb(&rgb, module, layer);
	DBGOUT("%s (m:%d, l:%d)\n", __func__, module, layer);

	*format    = rgb->format;
	*width	   = rgb->srcw;
	*height    = rgb->srch;
	*pixelbyte = rgb->pixelbyte;
}

int soc_dpc_set_rgb_position(int module, int layer, int startx, int starty, int waitvsync)
{
	struct rgb_data *rgb;

	int left   = startx;
	int top    = starty;
	int right;
	int bottom;

	set_rgb(&rgb, module, layer);
	right  = startx + rgb->srcw;
	bottom = starty + rgb->srch;


	if (! rgb->format) {
		printk(KERN_ERR "fail, not set rgb format for %d module's %d rgb ...\n",
			module, layer);
		return -1;
	}

	DBGOUT("%s (m:%d, l:%d, - l:%d, t:%d, r:%d, b:%d, w:%d)\n",
		__func__, module, layer, left, top, right, bottom, waitvsync);

	NX_MLC_SetPosition(module, layer, left, top, right-1, bottom-1);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		dpc_wait_dirtyclear(module, layer);
	return 0;
}

int soc_dpc_set_rgb_position2(int module, int layer, int left, int top, int right, int bottom, int waitvsync)
{
	struct rgb_data *rgb;

	set_rgb(&rgb, module, layer);
	if (! rgb->format) {
		printk(KERN_ERR "fail, not set rgb format for %d module's %d rgb ...\n",
			module, layer);
		return -1;
	}

	DBGOUT("%s (m:%d, l:%d, - l:%d, t:%d, r:%d, b:%d, w:%d)\n",
		__func__, module, layer, left, top, right, bottom, waitvsync);

	NX_MLC_SetPosition(module, layer, left, top, right-1, bottom-1);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		dpc_wait_dirtyclear(module, layer);
	return 0;
}

int soc_dpc_get_rgb_position(
	int module, int layer, int*startx, int*starty
	)
{
	register struct NX_MLC_RegisterSet *pMLC =
		(struct NX_MLC_RegisterSet*)IO_ADDRESS(NX_MLC_GetPhysicalAddress(module));

	U32 lr = ReadIODW(&pMLC->MLCRGBLAYER[layer].MLCLEFTRIGHT);
	U32 tb = ReadIODW(&pMLC->MLCRGBLAYER[layer].MLCTOPBOTTOM);

	*startx   = ((lr >> 16) & 0xFFFUL);
	*starty   = ((tb >> 16) & 0xFFFUL);

	return 0;
}

int soc_dpc_get_rgb_position2(int module, int layer, int* left, int* top, int* right, int* bottom)
{
	register struct NX_MLC_RegisterSet *pMLC =
		(struct NX_MLC_RegisterSet*)IO_ADDRESS(NX_MLC_GetPhysicalAddress(module));

	U32 lr = ReadIODW(&pMLC->MLCRGBLAYER[layer].MLCLEFTRIGHT);
	U32 tb = ReadIODW(&pMLC->MLCRGBLAYER[layer].MLCTOPBOTTOM);

	*left   = ((lr >> 16) & 0xFFFUL);
	*right  = ((lr >>  0) & 0xFFFUL) + 1;
	*top    = ((tb >> 16) & 0xFFFUL);
	*bottom = ((tb >>  0) & 0xFFFUL) + 1;

	return 0;
}

int soc_dpc_set_rgb_invalidzone(
	int module, int layer, int num, int left, int top,
	int right, int bottom, int enable, int waitvsync
	)
{
	DBGOUT("%s (m:%d, l:%d, n:%d - l:%d, t:%d, r:%d, b:%d, e:%d, w:%d)\n",
		__func__, module, layer, num, left, top, right, bottom, enable, waitvsync);

	NX_MLC_SetRGBLayerInvalidPosition(module, layer, num, left, top, right, bottom, enable);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		dpc_wait_dirtyclear(module, layer);

	return 0;
}

void soc_dpc_set_rgb_address(
	int module, int layer, uint addr, uint hstride, uint vstride, int waitvsync
	)
{
	DBGOUT("%s (m:%d, l:%d, a:0x%x, h:%d, v:%d, w:%d)\n",
		__func__, module, layer, addr, hstride, vstride, waitvsync);

	NX_MLC_SetRGBLayerStride (module, layer, hstride, vstride);
	NX_MLC_SetRGBLayerAddress(module, layer, addr);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		dpc_wait_dirtyclear(module, layer);
}

void 	soc_dpc_get_rgb_address(int module, int layer, u_int*addr,
								u_int*hstride, u_int*vstride)
{
	register struct NX_MLC_RegisterSet *pMLC =
		(struct NX_MLC_RegisterSet*)IO_ADDRESS(NX_MLC_GetPhysicalAddress(module));

	*hstride = ReadIODW((volatile U32 *)&pMLC->MLCRGBLAYER[layer].MLCHSTRIDE);
	*vstride = ReadIODW((volatile U32 *)&pMLC->MLCRGBLAYER[layer].MLCVSTRIDE);
	*addr    = ReadIODW((volatile U32 *)&pMLC->MLCRGBLAYER[layer].MLCADDRESS);
}

void soc_dpc_set_rgb_enable(int module, int layer, int enable)
{
	DBGOUT("%s (m:%d, l:%d, e:%d)\n", __func__, module, layer, enable);

	NX_MLC_SetLayerEnable(module, layer, (enable ? CTRUE : CFALSE));
	NX_MLC_SetDirtyFlag(module, layer);
}

void  soc_dpc_set_rgb_color(int module, int layer, uint type, uint color, int enable)
{
	struct rgb_data *rgb;

	set_rgb(&rgb, module, layer);
	DBGOUT("%s (m:%d, l:%d, t:%d, c:0x%x, e:%d)\n",
		 __func__, module, layer, type, color, enable);

	switch (type) {
	case RGB_COL_ALPHA:
		{
			if (color <= 0 )	color = 0;
			if (color >= 15)	color = 15;

			rgb->color.alpha = (enable?color:15);

			NX_MLC_SetAlphaBlending(module, layer, (enable ? CTRUE : CFALSE), color);
			NX_MLC_SetDirtyFlag(module, layer);
		}
		break;

	case RGB_COL_TRANSP:
		{
			if (1 == rgb->pixelbyte)
				color = R8G8B8toR3G3B2((uint)color), color = R3G3B2toR8G8B8((u_char)color);
			if (2 == rgb->pixelbyte)
				color = R8G8B8toR5G6B5((uint)color), color = R5G6B5toR8G8B8((u_short)color);

			rgb->color.transcolor = (enable?color:0);

			NX_MLC_SetTransparency(module, layer, (enable ? CTRUE : CFALSE), (color & 0x00FFFFFF));
			NX_MLC_SetDirtyFlag(module, layer);
		}
		break;

	case RGB_COL_INVERT:
		{
			if (1 == rgb->pixelbyte)
				color = R8G8B8toR3G3B2((uint)color), color = R3G3B2toR8G8B8((u_char)color);
			if (2 == rgb->pixelbyte)
				color = R8G8B8toR5G6B5((uint)color), color = R5G6B5toR8G8B8((u_short)color);

			rgb->color.invertcolor = (enable?color:0);

			NX_MLC_SetColorInversion(module, layer, (enable ? CTRUE : CFALSE), (color & 0x00FFFFFF));
			NX_MLC_SetDirtyFlag(module, layer);
		}
		break;

	default:
		break;
	}
}

uint  soc_dpc_get_rgb_color(int module, int layer, uint type)
{
	struct rgb_data *rgb;

	set_rgb(&rgb, module, layer);
	DBGOUT("%s (m:%d, l:%d, t:%d)\n", __func__, module, layer, type);

	switch (type) {
	case RGB_COL_ALPHA:		return (uint)rgb->color.alpha;
	case RGB_COL_TRANSP:	return (uint)rgb->color.transcolor;
	case RGB_COL_INVERT:	return (uint)rgb->color.invertcolor;
	default: 				break;
	}
	return 0;
}


/*------------------------------------------------------------------------------
 * External LCD device
 */
int	soc_dpc_set_lcd_out(int module, uint format, uint type)
{
	struct dpc_info *dpc;
	int ret = -1;

	set_dpc(&dpc, module);
	DBGOUT("%s (m:%d, f:%x, t:%x)\n", __func__, module, format, type);
	if (dpc->lcdops && dpc->lcdops->setformat)
		ret = dpc->lcdops->setformat(module, format, type, 0);
	return ret;
}

int	soc_dpc_get_lcd_out(int module, uint *format, uint *type)
{
	struct dpc_info *dpc;
	int ret = -1;

	set_dpc(&dpc, module);
	DBGOUT("%s (m:%d)\n", __func__, module);
	if (dpc->lcdops && dpc->lcdops->getformat)
		ret = dpc->lcdops->getformat(module, format, type);
	return ret;
}

/*------------------------------------------------------------------------------
 *	External Backlight Unit
 */
int soc_dpc_register_blu(uint dev, struct dpc_blu_ops *ops)
{
	struct dpc_info *dpc;

	set_dpc(&dpc, dev);
	DBGOUT("%s (m:%d, ops:0x%x)\n", __func__, dev, (uint)ops);
	if (dpc->bluops) {
		printk(KERN_ERR "fail, module(%d) backlight unit ops exist ...\n", dev);
		return (-1);
	}
	dpc->bluops = ops;
	return 0;
}

/*------------------------------------------------------------------------------
 *	External LCD Unit
 */
int soc_dpc_register_lcd(uint dev, struct dpc_lcd_ops *ops)
{
	struct dpc_info *dpc;

	set_dpc(&dpc, dev);
	DBGOUT("%s (m:%d, ops:0x%x)\n", __func__, dev, (uint)ops);
	if (dpc->lcdops) {
		printk(KERN_ERR "fail, module(%d) LCD unit ops exist ...\n", dev);
		return (-1);
	}
	dpc->lcdops = ops;
	return 0;
}

/*------------------------------------------------------------------------------
 *	External DPC Unit
 */
int soc_dpc_register_dpc(uint dev, struct dpc_dpc_ops *ops)
{
	struct dpc_info *dpc;

	set_dpc(&dpc, dev);
	DBGOUT("%s (m:%d, ops:0x%x)\n", __func__, dev, (uint)ops);

	if (dpc->dpcops) {
		printk(KERN_ERR "fail, module(%d) DPC unit ops exist ...\n", dev);
		return (-1);
	}
	dpc->dpcops = ops;
	return 0;
}

#if DUMP_REG
static void dbg_reg_dpc(int dev)
{
/*
	U32 DPCBASE = IO_ADDRESS(PHY_BASEADDR_DPC + (0x400*dev));
	printk("==========================================================\n");
	printk(" DPCHTOTAL				= 0x%04x\n", *(U16*)(DPCBASE + 0x07C));
	printk(" DPCHSWIDTH				= 0x%04x\n", *(U16*)(DPCBASE + 0x07E));
	printk(" DPCHASTART				= 0x%04x\n", *(U16*)(DPCBASE + 0x080));
	printk(" DPCHAEND				= 0x%04x\n", *(U16*)(DPCBASE + 0x082));
	printk(" DPCVTOTAL				= 0x%04x\n", *(U16*)(DPCBASE + 0x084));
	printk(" DPCVSWIDTH				= 0x%04x\n", *(U16*)(DPCBASE + 0x086));
	printk(" DPCVASTART				= 0x%04x\n", *(U16*)(DPCBASE + 0x088));
	printk(" DPCVAEND				= 0x%04x\n", *(U16*)(DPCBASE + 0x08A));
	printk(" DPCCTRL1				= 0x%04x\n", *(U16*)(DPCBASE + 0x08E));
	printk(" DPCEVTOTAL				= 0x%04x\n", *(U16*)(DPCBASE + 0x090));
	printk(" DPCEVSWIDTH			= 0x%04x\n", *(U16*)(DPCBASE + 0x092));
	printk(" DPCEVASTART			= 0x%04x\n", *(U16*)(DPCBASE + 0x094));
	printk(" DPCEVAEND				= 0x%04x\n", *(U16*)(DPCBASE + 0x096));
	printk(" DPCCTRL2				= 0x%04x\n", *(U16*)(DPCBASE + 0x098));
	printk(" DPCVSEOFFSET			= 0x%04x\n", *(U16*)(DPCBASE + 0x09A));
	printk(" DPCVSSOFFSET			= 0x%04x\n", *(U16*)(DPCBASE + 0x09C));
	printk(" DPCEVSEOFFSET			= 0x%04x\n", *(U16*)(DPCBASE + 0x09E));
	printk(" DPCEVSSOFFSET			= 0x%04x\n", *(U16*)(DPCBASE + 0x0A0));
	printk(" DPCDELAY0				= 0x%04x\n", *(U16*)(DPCBASE + 0x0A2));
	printk(" DPCUPSCALECON0 		= 0x%04x\n", *(U16*)(DPCBASE + 0x0A4));
	printk(" DPCUPSCALECON1 		= 0x%04x\n", *(U16*)(DPCBASE + 0x0A6));
	printk(" DPCUPSCALECON2 		= 0x%04x\n", *(U16*)(DPCBASE + 0x0A8));
	printk(" DPCCLKENB       		= 0x%08x\n", *(U32*)(DPCBASE + 0x1C0));
	printk(" DPCCLKGEN[0][0] 		= 0x%08x\n", *(U32*)(DPCBASE + 0x1C4));
	printk(" DPCCLKGEN[0][1] 		= 0x%08x\n", *(U32*)(DPCBASE + 0x1C8));
	printk(" DPCCLKGEN[1][0] 		= 0x%08x\n", *(U32*)(DPCBASE + 0x1CC));
	printk(" DPCCLKGEN[1][1] 		= 0x%08x\n", *(U32*)(DPCBASE + 0x1D0));
	printk(" DPCCTRL0				= 0x%04x\n", *(U16*)(DPCBASE + 0x08C));
	printk("==========================================================\n");
*/
}

static void dbg_reg_mlc(int dev)
{
	struct NX_MLC_RegisterSet *MLC =
		(struct NX_MLC_RegisterSet*)IO_ADDRESS(NX_MLC_GetPhysicalAddress(dev));

	printk("==========================================================\n");
	printk("MLCBASE			    	    =0x%08x -> 0x%08x\n", (uint)NX_MLC_GetPhysicalAddress(dev), (uint)MLC);
	printk("MLCCONTROLT			    	=0x%08x\n", MLC->MLCCONTROLT);
	printk("MLCSCREENSIZE		    	=0x%08x\n", MLC->MLCSCREENSIZE);
	printk("MLCBGCOLOR		      		=0x%08x\n", MLC->MLCBGCOLOR);

	// MLCRGBLAYER[0]
	printk("RGB_0 MLCLEFTRIGHT	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCLEFTRIGHT);
	printk("RGB_0 MLCTOPBOTTOM 	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCTOPBOTTOM);
	printk("RGB_0 MLCINVALIDLEFTRIGHT0  =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVALIDLEFTRIGHT0);
	printk("RGB_0 MLCINVALIDTOPBOTTOM0  =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVALIDTOPBOTTOM0);
	printk("RGB_0 MLCINVALIDLEFTRIGHT1  =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVALIDLEFTRIGHT1);
	printk("RGB_0 MLCINVALIDTOPBOTTOM1  =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVALIDTOPBOTTOM1);
	printk("RGB_0 MLCCONTROL	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCCONTROL);
	printk("RGB_0 MLCHSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCHSTRIDE);
	printk("RGB_0 MLCVSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCVSTRIDE);
	printk("RGB_0 MLCTPCOLOR	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCTPCOLOR);
	printk("RGB_0 MLCINVCOLOR		    =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVCOLOR);
	printk("RGB_0 MLCADDRESS	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCADDRESS);

	// MLCRGBLAYER[1]
	printk("RGB_1 MLCLEFTRIGHT	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCLEFTRIGHT);
	printk("RGB_1 MLCTOPBOTTOM 	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCTOPBOTTOM);
	printk("RGB_1 MLCINVALIDLEFTRIGHT0  =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVALIDLEFTRIGHT0);
	printk("RGB_1 MLCINVALIDTOPBOTTOM0  =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVALIDTOPBOTTOM0);
	printk("RGB_1 MLCINVALIDLEFTRIGHT1  =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVALIDLEFTRIGHT1);
	printk("RGB_1 MLCINVALIDTOPBOTTOM1  =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVALIDTOPBOTTOM1);
	printk("RGB_1 MLCCONTROL	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCCONTROL);
	printk("RGB_1 MLCHSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCHSTRIDE);
	printk("RGB_1 MLCVSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCVSTRIDE);
	printk("RGB_1 MLCTPCOLOR	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCTPCOLOR);
	printk("RGB_1 MLCINVCOLOR		    =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVCOLOR);
	printk("RGB_1 MLCADDRESS	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCADDRESS);

	// MLCVIDEOLAYER
	printk("VIDEO MLCLEFTRIGHT          =0x%08x\n", MLC->MLCVIDEOLAYER.MLCLEFTRIGHT);
	printk("VIDEO MLCTOPBOTTOM			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCTOPBOTTOM);
	printk("VIDEO MLCCONTROL	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCCONTROL);
	printk("VIDEO MLCVSTRIDE	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCVSTRIDE);
	printk("VIDEO MLCTPCOLOR	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCTPCOLOR);
	printk("VIDEO MLCADDRESS	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCADDRESS);
	printk("VIDEO MLCADDRESSCB	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCADDRESSCB);
	printk("VIDEO MLCADDRESSCR	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCADDRESSCR);
	printk("VIDEO MLCVSTRIDECB	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCVSTRIDECB);
	printk("VIDEO MLCVSTRIDECR	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCVSTRIDECR);
	printk("VIDEO MLCHSCALE	            =0x%08x\n", MLC->MLCVIDEOLAYER.MLCHSCALE);
	printk("VIDEO MLCVSCALE	            =0x%08x\n", MLC->MLCVIDEOLAYER.MLCVSCALE);
	printk("VIDEO MLCLUENH	            =0x%08x\n", MLC->MLCVIDEOLAYER.MLCLUENH);
	printk("VIDEO MLCCHENH[0]			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCCHENH[0]);
	printk("VIDEO MLCCHENH[1]			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCCHENH[1]);
	printk("VIDEO MLCCHENH[2]			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCCHENH[2]);
	printk("VIDEO MLCCHENH[3]			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCCHENH[3]);

	// MLCRGBLAYER2
	printk("RGB_2 MLCLEFTRIGHT	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCLEFTRIGHT);
	printk("RGB_2 MLCTOPBOTTOM 	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCTOPBOTTOM);
	printk("RGB_2 MLCINVALIDLEFTRIGHT0  =0x%08x\n", MLC->MLCRGBLAYER2.MLCINVALIDLEFTRIGHT0);
	printk("RGB_2 MLCINVALIDTOPBOTTOM0  =0x%08x\n", MLC->MLCRGBLAYER2.MLCINVALIDTOPBOTTOM0);
	printk("RGB_2 MLCINVALIDLEFTRIGHT1  =0x%08x\n", MLC->MLCRGBLAYER2.MLCINVALIDLEFTRIGHT1);
	printk("RGB_2 MLCINVALIDTOPBOTTOM1	=0x%08x\n", MLC->MLCRGBLAYER2.MLCINVALIDTOPBOTTOM1);
	printk("RGB_2 MLCCONTROL	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCCONTROL);
	printk("RGB_2 MLCHSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCHSTRIDE);
	printk("RGB_2 MLCVSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCVSTRIDE);
	printk("RGB_2 MLCTPCOLOR	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCTPCOLOR);
	printk("RGB_2 MLCINVCOLOR 	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCINVCOLOR);
	printk("RGB_2 MLCADDRESS	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCADDRESS);

	printk("MLCGAMMACONT	        	=0x%08x\n", MLC->MLCGAMMACONT);
	printk("MLCRGAMMATABLEWRITE			=0x%08x\n", MLC->MLCRGAMMATABLEWRITE);
	printk("MLCGGAMMATABLEWRITE			=0x%08x\n", MLC->MLCGGAMMATABLEWRITE);
	printk("MLCBGAMMATABLEWRITE			=0x%08x\n", MLC->MLCBGAMMATABLEWRITE);
	printk("MLCCLKENB;              	=0x%08x\n", MLC->MLCCLKENB);
	printk("==========================================================\n");
}
#endif

/*----------------------------------------------------------------------------*/
/* DISPLAY */
EXPORT_SYMBOL(soc_dpc_get_mode);
EXPORT_SYMBOL(soc_dpc_get_resolution);
EXPORT_SYMBOL(soc_dpc_set_bgcolor);
EXPORT_SYMBOL(soc_dpc_get_bgcolor);
EXPORT_SYMBOL(soc_dpc_wait_vsync);
EXPORT_SYMBOL(soc_dpc_set_out_enable);
EXPORT_SYMBOL(soc_dpc_set_out_disable);
EXPORT_SYMBOL(soc_dpc_set_suspend);
EXPORT_SYMBOL(soc_dpc_set_resume);
/* LAYER */
EXPORT_SYMBOL(soc_dpc_set_layer_enable);
EXPORT_SYMBOL(soc_dpc_get_layer_enable);
EXPORT_SYMBOL(soc_dpc_wait_dirtyclear);
/* VIDEO */
EXPORT_SYMBOL(soc_dpc_set_vid_format);
EXPORT_SYMBOL(soc_dpc_get_vid_format);
EXPORT_SYMBOL(soc_dpc_set_vid_address);
EXPORT_SYMBOL(soc_dpc_get_vid_address);
EXPORT_SYMBOL(soc_dpc_set_vid_position);
EXPORT_SYMBOL(soc_dpc_get_vid_position);
EXPORT_SYMBOL(soc_dpc_set_vid_enable);
EXPORT_SYMBOL(soc_dpc_set_vid_priority);
EXPORT_SYMBOL(soc_dpc_get_vid_priority);
EXPORT_SYMBOL(soc_dpc_set_vid_colorkey);
EXPORT_SYMBOL(soc_dpc_get_vid_colorkey);
EXPORT_SYMBOL(soc_dpc_set_vid_color);
EXPORT_SYMBOL(soc_dpc_get_vid_color);
EXPORT_SYMBOL(soc_dpc_set_vid_hfilter);
EXPORT_SYMBOL(soc_dpc_get_vid_hfilter);
EXPORT_SYMBOL(soc_dpc_set_vid_vfilter);
EXPORT_SYMBOL(soc_dpc_get_vid_vfilter);
/* RGB */
EXPORT_SYMBOL(soc_dpc_set_rgb_format);
EXPORT_SYMBOL(soc_dpc_get_rgb_format);
EXPORT_SYMBOL(soc_dpc_set_rgb_position);
EXPORT_SYMBOL(soc_dpc_get_rgb_position);
EXPORT_SYMBOL(soc_dpc_set_rgb_invalidzone);
EXPORT_SYMBOL(soc_dpc_set_rgb_address);
EXPORT_SYMBOL(soc_dpc_get_rgb_address);
EXPORT_SYMBOL(soc_dpc_set_rgb_enable);
EXPORT_SYMBOL(soc_dpc_set_rgb_color);
EXPORT_SYMBOL(soc_dpc_get_rgb_color);
/* EXT LCD */
EXPORT_SYMBOL(soc_dpc_set_lcd_out);
EXPORT_SYMBOL(soc_dpc_get_lcd_out);
/* External device */
EXPORT_SYMBOL(soc_dpc_register_blu);
EXPORT_SYMBOL(soc_dpc_register_lcd);
EXPORT_SYMBOL(soc_dpc_register_dpc);

EXPORT_SYMBOL(soc_dpc_set_lcd_flip);
EXPORT_SYMBOL(soc_dpc_get_lcd_flip);
