/*
 * (C) Copyright 2012 LeapFrog Enterprises, Inc
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

#include <linux/device.h>
#include <linux/string.h>
#include <linux/export.h>
#include <linux/gpio.h>
#include <mach/adc.h>
#include <mach/soc.h>
#include <mach/board_revisions.h>

#include <asm/system.h>	/* system_rev global */
#include <mach/platform_id.h>
#include <mach/lf2000_lcd.h>
#include <linux/lf2000/gpio.h>


#define ADC_LCD_CHAN		0		/* adc channel */
#define ADC_DELTA_SENSE		40		/* per sense resistor */
#define ADC_TIMEOUT			1000	/* msec */
#define ADC_MIN_SENSE		10		/* bad GP sense resistor */

#define NUM_LCD_ENTRIES		20

struct lcd_entry_t {
	int adc;				/* ADC sense */
	const char* name;		/* ID string */
};

static struct lcd_entry_t lcd_table[NUM_LCD_ENTRIES] = {
	{	10, 	"GoWorld-1",	},
	{	48, 	"GoWorld-2",	},
	{	85, 	"GoWorld-3",	},
	{	123,	"GoWorld-4",	},
	{	164,	"GiantPlus-1",	},
	{	202,	"GiantPlus-2",	},
	{	242, 	"GiantPlus-3",	},
	{	279, 	"GiantPlus-4",	},
	{	314,	"PowerTip-1",	},
	{	352,	"PowerTip-2",	},
	{	392,	"PowerTip-3",	},
	{	433,	"PowerTip-4",	},
	{	475,	"K&D-1",	},
	{	517,	"K&D-2",	},
	{	554,	"K&D-3",	},
	{	592,	"K&D-4",	},
	{	633,	"Haier-1",	},
	{	672,	"Haier-2",	},
	{	710,	"Haier-3",	},
	{	750,	"Haier-4",	}
};

/*
 * determine if system can be powered from USB device
 */
bool have_usb_power_option(void)
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
			return(1);
			break;

		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_VTK:
		case LF2000_BOARD_UNKNOWN:
		default:
			break;
	}
	return(0);
}

EXPORT_SYMBOL(have_usb_power_option);

enum lf2000_leapfrog_platform get_leapfrog_platform(void)
{
	switch(system_rev)
	{
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_LUCY_CIP:
			return LUCY;
		
		case LF2000_BOARD_VALENCIA:
		case LF2000_BOARD_VALENCIA_EP:
		// case LF2000_BOARD_VALENCIA_EP_8:
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
			return VALENCIA;

		case LF2000_BOARD_VALENCIA_EP_8:
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
			return RIO;
		default:
			return UNKNOWN;
	}
	return UNKNOWN;
}
EXPORT_SYMBOL(get_leapfrog_platform);

bool is_board_lucy(void)
{
	switch(system_rev)
	{
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_LUCY_CIP:
			return 1;

		default:
			break;
	}
	return 0;
}
EXPORT_SYMBOL(is_board_lucy);

static ssize_t sysfs_show_board_id(struct device *dev,
                                   struct device_attribute* attr,
                                   char* buf)
{
	return sprintf(buf, "0x%X\n", system_rev);
}

static DEVICE_ATTR(system_rev, 0444, sysfs_show_board_id, NULL);

/*
 * Return screen type
 *  Note revision suffix in name
 */
inline int get_median3(int a, int b, int c)
{
	if (a<b) {
		// a X Y   or   c a b
		if (a<c)
			return b<c ? b : c;
		else
			// 2 0 1
			return a;
	}
	else {
		// b X Y   or   c b a
		if (b<c)
			return a<c ? a : c;
		else
			return b;
	}
}

enum camera_type_t get_rear_camera_type(void)
{
	enum camera_type_t type = CAMERA_NONE;

	switch(system_rev)
	{
		case LF2000_BOARD_VTK:
			type = CAMERA_SR300PC10;
			break;

		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_LUCY_CIP:
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
		case LF2000_BOARD_VALENCIA:
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_CIP:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
			type = CAMERA_HI253;
			break;
	}
	return type;
}
EXPORT_SYMBOL(get_rear_camera_type);

int get_rear_camera_i2c_adapter_id(void)
{
	int id = -1;

	switch(system_rev)
	{
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
			id = 0;
			break;
		case LF2000_BOARD_VTK:
		case LF2000_BOARD_VALENCIA:
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_CIP:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
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
			id = 1;
			break;
	}
	return id;
}
EXPORT_SYMBOL(get_rear_camera_i2c_adapter_id);

enum camera_type_t get_front_camera_type(void)
{
	enum camera_type_t type = CAMERA_NONE;

	switch(system_rev)
	{
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_CIP:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
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
			type = CAMERA_HI253;
			break;
	}
	return type;
}
EXPORT_SYMBOL(get_front_camera_type);

int get_front_camera_i2c_adapter_id(void)
{
	int id = -1;

	switch(system_rev)
	{
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
			id = 1;
			break;
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_CIP:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
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
			id = 0;
			break;
	}
	return id;
}
EXPORT_SYMBOL(get_front_camera_i2c_adapter_id);

unsigned int get_camera_enable_delay_ms(void)
{
	unsigned int delay = 10;

	/*
	 * Datasheet calls for 10 ms.  Valencia FEP has an addtional
	 * hardware delay of 50 ms on CHIP_ENABLEB.
	 */
	switch(system_rev)
	{
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_CIP:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
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
			delay = 60;
			break;
	}
	return delay;
}
EXPORT_SYMBOL(get_camera_enable_delay_ms);

static const char* lcd_mfg = NULL;
const char* get_lcd_mfg(void)
{
	unsigned short lcd_sense[4] = {0, 0, 0, 0};
	unsigned short lcd_index = 0;
	unsigned short lcd_start = 0, i;

	soc_adc_attach();
	lcd_sense[0] = soc_adc_read(ADC_LCD_CHAN, ADC_TIMEOUT);
	lcd_sense[1] = soc_adc_read(ADC_LCD_CHAN, ADC_TIMEOUT);
	lcd_sense[2] = soc_adc_read(ADC_LCD_CHAN, ADC_TIMEOUT);
	lcd_sense[3] = soc_adc_read(ADC_LCD_CHAN, ADC_TIMEOUT);
	lcd_sense[0] = get_median3(lcd_sense[1], lcd_sense[2], lcd_sense[3]);
	soc_adc_detach();

	/* ADC table entries are 10-bit per spec */
	lcd_sense[0] >>= 2;
	lcd_start = lcd_sense[0] / ADC_DELTA_SENSE;
	/* scan ADC table from approximate start index */
	if (lcd_sense[0] > lcd_table[lcd_start].adc) {
		for (i = lcd_start; i < NUM_LCD_ENTRIES; i++)
			if (lcd_sense[0] <= lcd_table[i].adc + (ADC_DELTA_SENSE >> 1))
				break;
	} else {
		for (i = lcd_start; i > 0; i--)
			if (lcd_sense[0] >= lcd_table[i].adc - (ADC_DELTA_SENSE >> 1))
				break;
	}
	lcd_index = i;
	if (lcd_index < NUM_LCD_ENTRIES)
		return lcd_mfg = lcd_table[lcd_index].name;
	return lcd_mfg = "Unknown";
}
EXPORT_SYMBOL(get_lcd_mfg);

static ssize_t sysfs_show_lcd_mfg(struct device *dev,
				  struct device_attribute *attr,
				  char* buf)
{
	if (lcd_mfg == NULL)
		lcd_mfg = get_lcd_mfg();
	return sprintf(buf, "%s\n", lcd_mfg);
}

static DEVICE_ATTR(lcd_mfg, 0444, sysfs_show_lcd_mfg, NULL);

static ssize_t sysfs_show_lcd_mfg_get(struct device *dev,
				  struct device_attribute *attr,
				  char* buf)
{
	return sprintf(buf, "%s\n", get_lcd_mfg());
}

static DEVICE_ATTR(lcd_mfg_get, 0444, sysfs_show_lcd_mfg_get, NULL);

enum lf2000_lcd_size get_lcd_size(void)
{
	switch(system_rev)
	{
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_LUCY_CIP:
			return LCD_320_240;

		case LF2000_BOARD_VALENCIA:
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_CIP:
			return LCD_480_272;

		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VTK:
			return LCD_800_480;

		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
			return LCD_1024_600;

		default:
			return LCD_UNKNOWN;
	}
	return LCD_UNKNOWN;
}

EXPORT_SYMBOL(get_lcd_size);

int get_lcd_type(void)
{
	switch (get_leapfrog_platform())
	{
		case LUCY:
			if (strstr(lcd_mfg, "GoWorld") || strstr(lcd_mfg, "PowerTip"))
				return LCD_HX8238;
			else if (strstr(lcd_mfg, "GiantPlus"))
				return LCD_ILI9322;
			break;
		case RIO:
			return LCD_ILI6480G2;
		case VALENCIA:
			return LCD_ILI6480G2;
		case UNKNOWN:
			break;
	}
	return -1;
}
EXPORT_SYMBOL(get_lcd_type);

static ssize_t sysfs_show_lcd_type(struct device *dev,
				  struct device_attribute *attr,
				  char* buf)
{
	switch(get_lcd_type())
	{
		case LCD_HX8238:
			return sprintf(buf, "HX8238\n");
		case LCD_ILI9322:
			return sprintf(buf, "ILI9322\n");
		case LCD_ILI6480G2:
			return sprintf(buf, "ILI6480G2\n");
	}
	return sprintf(buf, "UNKNOWN\n");
}

static DEVICE_ATTR(lcd_type, 0444, sysfs_show_lcd_type, NULL);

static ssize_t sysfs_show_lcd_size(struct device *dev,
				  struct device_attribute *attr,
				  char* buf)
{
	switch(get_lcd_size())
	{
		case LCD_320_240:
			return sprintf(buf, "320x240\n");
		case LCD_480_272:
			return sprintf(buf, "480x272\n");
		case LCD_800_480:
			return sprintf(buf, "800x480\n");
		case LCD_1024_600:
			return sprintf(buf, "1024x600\n");
		case LCD_UNKNOWN:
			break;
	}
	return sprintf(buf, "UNKNOWN\n");
}

static DEVICE_ATTR(lcd_size, 0444, sysfs_show_lcd_size, NULL);

static ssize_t sysfs_show_platform(struct device *dev,
                                   struct device_attribute *attr,
                                   char* buf)
{
	switch(get_leapfrog_platform())
	{
		case LUCY:
			return sprintf(buf, "LUCY\n");
		case RIO:
			return sprintf(buf, "RIO\n");
		case VALENCIA:
			return sprintf(buf, "VALENCIA\n");
		case UNKNOWN:
			break;
	}
	return sprintf(buf, "UNKNOWN\n");
}

static DEVICE_ATTR(platform, 0444, sysfs_show_platform, NULL);

static ssize_t sysfs_show_platform_family(struct device *dev,
                                          struct device_attribute *attr,
                                          char *buf)
{
	switch(get_leapfrog_platform())
	{
		case LUCY:
			return sprintf(buf, "LEX\n");
		case VALENCIA:
			return sprintf(buf, "LPAD\n");
		case RIO:
			return sprintf(buf, "RIO\n");
		case UNKNOWN:
			break;
	}
	return sprintf(buf, "UNKNOWN\n");
}

static DEVICE_ATTR(platform_family, 0444, sysfs_show_platform_family, NULL);

#if defined(CONFIG_BCM4319)
void bcm_wlan_power_off(int param);
void bcm_wlan_power_on(int param);
static ssize_t sysfs_show_wifi_power(struct device *dev,
                                     struct device_attribute *attr,
                                     char *buf)
{
	return sprintf(buf, "%d\n", gpio_get_value(WIFI_RESET));
}

static ssize_t sysfs_store_wifi_power(struct device *dev,
                                      struct device_attribute *attr,
			              const char *buf,
			              size_t count)
{
	int powered;
	if(sscanf(buf, "%d", &powered) != 1)
		return -EINVAL;
	if(powered)
		bcm_wlan_power_on(1);
	else
		bcm_wlan_power_off(1);
	return count;
}

static DEVICE_ATTR(wifi_power, 0666, sysfs_show_wifi_power, sysfs_store_wifi_power);
#endif //CONFIG_BCM4319

static struct bus_type board_subsys = {
	.name		= "board",
	.dev_name	= "board",
};

static int __init init_board_sysfs(void)
{
	int error = subsys_system_register(&board_subsys, NULL);

	error	      = device_create_file(board_subsys.dev_root, &dev_attr_lcd_mfg);
	if(!error)
		error = device_create_file(board_subsys.dev_root, &dev_attr_lcd_mfg_get);
	if(!error)
		error = device_create_file(board_subsys.dev_root, &dev_attr_lcd_type);
	if(!error)
		error = device_create_file(board_subsys.dev_root, &dev_attr_lcd_size);
	if(!error)
		error = device_create_file(board_subsys.dev_root, &dev_attr_platform);
	if(!error)
		error = device_create_file(board_subsys.dev_root, &dev_attr_platform_family);
	if(!error)
		error = device_create_file(board_subsys.dev_root, &dev_attr_system_rev);
#if defined(CONFIG_BCM4319)
	if(!error)
		error = device_create_file(board_subsys.dev_root, &dev_attr_wifi_power);
#endif //CONFIG_BCM4319

	return error;
}

device_initcall(init_board_sysfs);

