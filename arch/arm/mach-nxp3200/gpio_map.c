/*
 * (C) Copyright 2011
 * Daniel Lazzari Jr, LeapFrog Inc, <dlazzari@leapfrog.com>
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

#include <linux/export.h>
#include <linux/gpio.h>
#include <linux/lf2000/gpio.h>
#include <asm/system.h>

#include <mach/platform.h>
#include <mach/board_revisions.h>

int gpio_map[GPIO_NUMBER_VALUES];

/*
 * GPIO entry map
 * Sort entries by GPIO port / pin, accounting for all GPIO pins
 * Mark unused GPIO pins with LF2000_GPIO_PORT_NONE for housekeeping
 */


static void init_lucy(void)
{
	printk(KERN_WARNING "%s: Lucy GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0    - pin  A0 */
	/* PVD1    - pin  A1 */
	/* PVD2    - pin  A2 */
	/* PVD3    - pin  A3 */
	/* PVD4    - pin  A4 */
	/* PVD5    - pin  A5 */
	/* PVD6    - pin  A6 */
	/* PVD7    - pin  A7 */

	/* TX0     - pin  A8 */
	gpio_map[REAR_CAM_ENABLE_L]	= LF2000_GPIO_PORT_A | 9;
	gpio_map[SHOULDER_RIGHT]	= LF2000_GPIO_PORT_A | 10;
	gpio_map[HEADPHONE_JACK]	= LF2000_GPIO_PORT_A | 11;
	gpio_map[BUTTON_PAUSE]		= LF2000_GPIO_PORT_A | 12;
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_PORT_A | 13;
	gpio_map[BUTTON_HINT]		= LF2000_GPIO_PORT_A | 14;
	/* N/C     - pin A15 */

	/* TP602   - pin A16 */
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_PORT_A | 17;
	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_A | 18;
	/* N/C     - pin A19 */
	/* N/C     - pin A20 */
	/* I2SDOUT - pin A21 */
	/* I2SBCLK - pin A22 */
	/* I2SDIN  - pin A23 */

	/* I2SSYNC - pin A24 */
	/* I2SMCLK - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	gpio_map[BUTTON_B]		= LF2000_GPIO_PORT_A | 30;
	gpio_map[POWER_ON]		= LF2000_GPIO_PORT_A | 31;

	/* GPIO Port B */
	/* SDCLK0  - pin  B0 */
	/* SDCMD0  - pin  B1 */
	/* SDDAT0  - pin  B2 */
	/* SDDAT1  - pin  B3 */
	/* SDDAT2  - pin  B4 */
	/* SDDAT3  - pin  B5 */
	/* N/C     - pin  B6 */
	/* N/C     - pin  B7 */

	/* N/C     - pin  B8 */
	/* N/C     - pin  B9 */
	/* N/C     - pin B10 */
	/* N/C     - pin B11 */
	gpio_map[LCD_SPI]		=  LF2000_GPIO_PORT_B | 12;
	/* SPI CLK - pin B13 */
	/* SPI SDO - pin B14 */
	/* SPI SDI - pin B15 */ 

	/* PVD8    - pin B16 */
	/* PVD9    - pin B17 */
	/* PVD10   - pin B18 */
	/* PVD11   - pin B19 */
	/* PVD12   - pin B20 */
	/* PVD13   - pin B21 */
	/* PVD14   - pin B22 */
	/* PVD15   - pin B23 */

	/* PVD16   - pin B24 */
	/* PVD17   - pin B25 */
	/* PVD18   - pin B26 */
	/* PVD19   - pin B27 */
	/* PVD20   - pin B28 */
	/* PVD21   - pin B29 */
	/* PVD22   - pin B30 */
	/* PVD23   - pin B31 */

	/* GPIO Port C */
	gpio_map[LFP100_INT]		= LF2000_GPIO_PORT_C | 0;
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_PORT_C | 1;
	gpio_map[RT_BATTPACK_DET_L]	= LF2000_GPIO_PORT_C | 2;
	gpio_map[ACCEL_INT]		= LF2000_GPIO_PORT_C | 3;
	gpio_map[REAR_CAM_RESET_L]	= LF2000_GPIO_PORT_C | 4;
	gpio_map[CHG_DISABLE]		= LF2000_GPIO_PORT_C | 5;
	gpio_map[DCIN_OVP]		= LF2000_GPIO_PORT_C | 6;
	gpio_map[BUTTON_A]		= LF2000_GPIO_PORT_C | 7;

	gpio_map[SHOULDER_LEFT]		= LF2000_GPIO_PORT_C | 8;
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_PORT_C | 9;
	gpio_map[BUTTON_HOME]		= LF2000_GPIO_PORT_C | 10;
	gpio_map[LED_ENA]		= LF2000_GPIO_PORT_C | 11;
	gpio_map[USBD_ID_SENSE]		= LF2000_GPIO_PORT_C | 12;
	gpio_map[DPAD_UP]		= LF2000_GPIO_PORT_C | 13;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_PORT_C | 14;
	gpio_map[TOUCHSCREEN_Y2]	= LF2000_GPIO_PORT_C | 15;

	gpio_map[TOUCHSCREEN_X2]	= LF2000_GPIO_PORT_C | 16;
	gpio_map[TOUCHSCREEN_Y1]	= LF2000_GPIO_PORT_C | 17;
	gpio_map[TOUCHSCREEN_X1]	= LF2000_GPIO_PORT_C | 18;
	/* N/C      - pin C19 */
	gpio_map[NAND_CHIP_SELECT] = LF2000_GPIO_PORT_C | 20;
	/* VVSYNC_B - pin C21 */
	/* VCLKB    - pin C22 */
	/* VDVALID  - pin C23 */

	/* VHSYNC_B - pin C24 */
	/* VID_B0   - pin C25 */
	/* VID_B1   - pin C26 */
	/* VID_B2   - pin C27 */
	/* VID_B3   - pin C28 */
	/* VID_B4   - pin C29 */
	/* VID_B5   - pin C30 */
	/* VID_B6   - pin C31 */

	/* GPIO Port D */

	/* VID_B7   - pin  D0 */
	/* SD8      - pin  D1 */
	/* SD9      - pin  D2 */
	/* SD10     - pin  D3 */
	/* SD11     - pin  D4 */
	/* SD12     - pin  D5 */
	/* SD13     - pin  D6 */
	/* SD14     - pin  D7 */

	/* SD15     - pin  D8 */
	/* SA8      - pin  D9 */
	/* SA9      - pin D10 */
	/* SA10     - pin D11 */
	/* SA11     - pin D12 */
	/* SA12     - pin D13 */
	/* SA13     - pin D14 */
	/* SA14     - pin D15 */

	/* SA15     - pin D16 */
	/* SA16     - pin D17 */
	/* SA17     - pin D18 */
	/* SA18     - pin D19 */
	/* PVCLK    - pin D20 */
	/* PDE      - pin D21 */
	/* PHSYNC   - pin D22 */
	/* PVSYNC   - pin D23 */

	/* VCLK27   - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C -  pin 0 */
	/* N/C -  pin 1 */
	/* N/C -  pin 2 */
	/* N/C -  pin 3 */
	/* N/C -  pin 4 */
	/* N/C -  pin 5 */
	/* N/C -  pin 6 */
	/* N/C -  pin 7 */

}

static void init_lucy_cip(void)
{
	printk(KERN_WARNING "%s: Lucy CIP GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0    - pin  A0 */
	/* PVD1    - pin  A1 */
	/* PVD2    - pin  A2 */
	/* PVD3    - pin  A3 */
	/* PVD4    - pin  A4 */
	/* PVD5    - pin  A5 */
	/* PVD6    - pin  A6 */
	/* PVD7    - pin  A7 */

	/* TX0     - pin  A8 */
	gpio_map[REAR_CAM_ENABLE_L]	= LF2000_GPIO_PORT_A | 9;
	gpio_map[SHOULDER_RIGHT]	= LF2000_GPIO_PORT_A | 10;
	gpio_map[HEADPHONE_JACK]	= LF2000_GPIO_PORT_A | 11;
	gpio_map[BUTTON_PAUSE]		= LF2000_GPIO_PORT_A | 12;
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_PORT_A | 13;
	gpio_map[BUTTON_HINT]		= LF2000_GPIO_PORT_A | 14;
	/* N/C     - pin A15 */

	/* TP602   - pin A16 */
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_PORT_A | 17;
	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_A | 18;
	/* N/C     - pin A19 */
	/* N/C     - pin A20 */
	/* I2SDOUT - pin A21 */
	/* I2SBCLK - pin A22 */
	/* I2SDIN  - pin A23 */

	/* I2SSYNC - pin A24 */
	/* I2SMCLK - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	gpio_map[BUTTON_B]		= LF2000_GPIO_PORT_A | 30;
	gpio_map[POWER_ON]		= LF2000_GPIO_PORT_A | 31;

	/* GPIO Port B */
	/* SDCLK0  - pin  B0 */
	/* SDCMD0  - pin  B1 */
	/* SDDAT0  - pin  B2 */
	/* SDDAT1  - pin  B3 */
	/* SDDAT2  - pin  B4 */
	/* SDDAT3  - pin  B5 */
	/* N/C     - pin  B6 */
	/* N/C     - pin  B7 */

	/* N/C     - pin  B8 */
	/* N/C     - pin  B9 */
	/* N/C     - pin B10 */
	/* N/C     - pin B11 */
	gpio_map[LCD_SPI]		=  LF2000_GPIO_PORT_B | 12;
	/* SPI CLK - pin B13 */
	/* SPI SDO - pin B14 */
	/* SPI SDI - pin B15 */

	/* PVD8    - pin B16 */
	/* PVD9    - pin B17 */
	/* PVD10   - pin B18 */
	/* PVD11   - pin B19 */
	/* PVD12   - pin B20 */
	/* PVD13   - pin B21 */
	/* PVD14   - pin B22 */
	/* PVD15   - pin B23 */

	/* PVD16   - pin B24 */
	/* PVD17   - pin B25 */
	/* PVD18   - pin B26 */
	/* PVD19   - pin B27 */
	/* PVD20   - pin B28 */
	/* PVD21   - pin B29 */
	/* PVD22   - pin B30 */
	/* PVD23   - pin B31 */

	/* GPIO Port C */
	gpio_map[LFP100_INT]		= LF2000_GPIO_PORT_C | 0;
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_PORT_C | 1;
	gpio_map[RT_BATTPACK_DET_L]	= LF2000_GPIO_PORT_C | 2;
	gpio_map[ACCEL_INT]		= LF2000_GPIO_PORT_C | 3;
	gpio_map[REAR_CAM_RESET_L]	= LF2000_GPIO_PORT_C | 4;
	gpio_map[CHG_DISABLE]		= LF2000_GPIO_PORT_C | 5;
	gpio_map[DCIN_OVP]		= LF2000_GPIO_PORT_C | 6;
	gpio_map[BUTTON_A]		= LF2000_GPIO_PORT_C | 7;

	gpio_map[SHOULDER_LEFT]		= LF2000_GPIO_PORT_C | 8;
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_PORT_C | 9;
	gpio_map[BUTTON_HOME]		= LF2000_GPIO_PORT_C | 10;
	gpio_map[LED_ENA]		= LF2000_GPIO_PORT_C | 11;
	gpio_map[USBD_ID_SENSE]		= LF2000_GPIO_PORT_C | 12;
	gpio_map[DPAD_UP]		= LF2000_GPIO_PORT_C | 13;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_PORT_C | 14;
	gpio_map[TOUCHSCREEN_Y2]	= LF2000_GPIO_PORT_C | 15;

	gpio_map[TOUCHSCREEN_X2]	= LF2000_GPIO_PORT_C | 16;
	gpio_map[TOUCHSCREEN_Y1]	= LF2000_GPIO_PORT_C | 17;
	gpio_map[TOUCHSCREEN_X1]	= LF2000_GPIO_PORT_C | 18;
	/* N/C      - pin C19 */
	gpio_map[NAND_CHIP_SELECT] = LF2000_GPIO_PORT_C | 20;
	/* VVSYNC_B - pin C21 */
	/* VCLKB    - pin C22 */
	/* VDVALID  - pin C23 */

	/* VHSYNC_B - pin C24 */
	/* VID_B0   - pin C25 */
	/* VID_B1   - pin C26 */
	/* VID_B2   - pin C27 */
	/* VID_B3   - pin C28 */
	/* VID_B4   - pin C29 */
	/* VID_B5   - pin C30 */
	/* VID_B6   - pin C31 */

	/* GPIO Port D */

	/* VID_B7   - pin  D0 */
	/* SD8      - pin  D1 */
	/* SD9      - pin  D2 */
	/* SD10     - pin  D3 */
	/* SD11     - pin  D4 */
	/* SD12     - pin  D5 */
	/* SD13     - pin  D6 */
	/* SD14     - pin  D7 */

	/* SD15     - pin  D8 */
	/* SA8      - pin  D9 */
	/* SA9      - pin D10 */
	/* SA10     - pin D11 */
	/* SA11     - pin D12 */
	/* SA12     - pin D13 */
	/* SA13     - pin D14 */
	/* SA14     - pin D15 */

	/* SA15     - pin D16 */
	/* SA16     - pin D17 */
	/* SA17     - pin D18 */
	/* SA18     - pin D19 */
	/* PVCLK    - pin D20 */
	/* PDE      - pin D21 */
	/* PHSYNC   - pin D22 */
	/* PVSYNC   - pin D23 */

	/* VCLK27   - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C -  pin 0 */
	/* N/C -  pin 1 */
	/* N/C -  pin 2 */
	/* N/C -  pin 3 */
	/* N/C -  pin 4 */
	/* N/C -  pin 5 */
	/* N/C -  pin 6 */
	/* N/C -  pin 7 */

}

static void init_rio(void)
{
	printk(KERN_WARNING "%s: Rio GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0      - pin  A0 */
	/* PVD1      - pin  A1 */
	/* PVD2      - pin  A2 */
	/* PVD3      - pin  A3 */
	/* PVD4      - pin  A4 */
	/* PVD5      - pin  A5 */
	/* PVD6      - pin  A6 */
	/* PVD7      - pin  A7 */

	/* TX0       - pin  A8 */
	/* N/C       - pin  A9 */
	gpio_map[DPAD_UP]		= LF2000_GPIO_PORT_A | 10;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_PORT_A | 11;
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_PORT_A | 12;
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_PORT_A | 13;
	gpio_map[BUTTON_HOME]		= LF2000_GPIO_PORT_A | 14;	/* schematic calls this HOME_BTN_L */
	gpio_map[MAGNETO_DRDY]		= LF2000_GPIO_PORT_A | 15;

	/* N/C       - pin A16 */
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_PORT_A | 17;
	/* N/C       - pin A18 */
	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_A | 19;
	/* N/C       - pin A20 */
	/* I2SDOUT   - pin A21 */
	/* I2SBCLK   - pin A22 */
	/* I2SDIN    - pin A23 */

	/* I2SSYNC   - pin A24 */
	/* I2SMCLK   - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	gpio_map[LED_ENA]		= LF2000_GPIO_PORT_A | 30;
	gpio_map[TP_INT]		= LF2000_GPIO_PORT_A | 31;

	/* GPIO Port B */
	/* SDCLK0    - pin  B0 */
	/* SDCMD0    - pin  B1 */
	/* SDDAT0[0] - pin  B2 */
	/* SDDAT0[1] - pin  B3 */
	/* SDDAT0[2] - pin  B4 */
	/* SDDAT0[3] - pin  B5 */
	/* SDCLK1    - pin  B6 */
	/* SDCMD1    - pin  B7 */

	/* SDDAT1[0] - pin  B8 */
	/* SDDAT1[1] - pin  B9 */
	/* SDDAT1[2] - pin B10 */
	/* SDDAT1[3] - pin B11 */
	/* SPI nCS   - pin B12 */
	/* SPI CLK   - pin B13 */
	/* SPI SDO   - pin B14 */
	/* SPI SDI   - pin B15 */ 

	/* PVD8      - pin B16 */
	/* PVD9      - pin B17 */
	/* PVD10     - pin B18 */
	/* PVD11     - pin B19 */
	/* PVD12     - pin B20 */
	/* PVD13     - pin B21 */
	/* PVD14     - pin B22 */
	/* PVD15     - pin B23 */

	/* PVD16     - pin B24 */
	/* PVD17     - pin B25 */
	/* PVD18     - pin B26 */
	/* PVD19     - pin B27 */
	/* PVD20     - pin B28 */
	/* PVD21     - pin B29 */
	/* PVD22     - pin B30 */
	/* PVD23     - pin B31 */

	/* GPIO Port C */
	/* VCLK      - pin C0  */
	/* VID[7]    - pin C1  */
	gpio_map[USB_POWER_FLT_L]	= LF2000_GPIO_PORT_C | 2;
	gpio_map[LCD_SPI]		= LF2000_GPIO_PORT_C | 3;
	/* SPI CLK   - pin C4  */
	gpio_map[MAGNETO_INT]		= LF2000_GPIO_PORT_C | 5;
	/* SPI SDI   - pin C6  */
	gpio_map[LFP100_INT]		= LF2000_GPIO_PORT_C | 7;

	/* VID[0]    - pin C8  */
	/* VID[1]    - pin C9  */
	/* VID[2]    - pin C10 */
	/* VID[3]    - pin C11 */
	/* VID[4]    - pin C12 */
	/* VID[5]    - pin C13 */
	/* VID[6]    - pin C14 */
	gpio_map[TOUCHSCREEN_Y2]	= LF2000_GPIO_PORT_C | 15;

	gpio_map[TOUCHSCREEN_X2]	= LF2000_GPIO_PORT_C | 16;
	gpio_map[TOUCHSCREEN_Y1]	= LF2000_GPIO_PORT_C | 17;
	gpio_map[TOUCHSCREEN_X1]	= LF2000_GPIO_PORT_C | 18;
	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_C | 19;
	gpio_map[NAND_CHIP_SELECT] = LF2000_GPIO_PORT_C | 20;
	gpio_map[REAR_CAM_ENABLE_L]	= LF2000_GPIO_PORT_C | 21;
	/* VCLKB      - pin C22 */
	gpio_map[REAR_CAM_RESET_L]	= LF2000_GPIO_PORT_C | 23;

	gpio_map[RCAM_CLK_ENA_L]	= LF2000_GPIO_PORT_C | 24;
	/* VID_B0     - pin C25 */
	/* VID_B1     - pin C26 */
	/* VID_B2     - pin C27 */
	/* VID_B3     - pin C28 */
	/* VID_B4     - pin C29 */
	/* VID_B5     - pin C30 */
	/* VID_B6     - pin C31 */

	/* GPIO Port D */

	/* VID_B7     - pin  D0 */
	/* SD8        - pin  D1 */
	/* SD9        - pin  D2 */
	/* SD10       - pin  D3 */
	/* SD11       - pin  D4 */
	/* SD12       - pin  D5 */
	/* SD13       - pin  D6 */
	/* SD14       - pin  D7 */

	/* SD15       - pin  D8 */
	gpio_map[ACCEL_INT]		= LF2000_GPIO_PORT_D | 9;
	/* N/C        - pin  D10*/
	gpio_map[WIFI_RESET]		= LF2000_GPIO_PORT_D | 11;	/* schematic calls this HOME_BTN_L */
	gpio_map[HEADPHONE_JACK]	= LF2000_GPIO_PORT_D | 12;
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_PORT_D | 13;
	gpio_map[FRONT_CAM_RESET_L]	= LF2000_GPIO_PORT_D | 14;
	gpio_map[FRONT_CAM_ENABLE_L]	= LF2000_GPIO_PORT_D | 15;

	/* SA15       - pin D16 */
	/* SA16       - pin D17 */
	gpio_map[FCAM_CLK_ENA_L]	= LF2000_GPIO_PORT_D | 18;
	gpio_map[USB_CHG_DETECT]	= LF2000_GPIO_PORT_D | 19;
	/* PVCLK      - pin D20 */
	/* PDE        - pin D21 */
	/* PHSYNC     - pin D22 */
	/* PVSYNC     - pin D23 */

	/* VCLK27     - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C        -  pin 0 */
	/* N/C        -  pin 1 */
	/* N/C        -  pin 2 */
	/* N/C        -  pin 3 */
	/* N/C        -  pin 4 */
	/* N/C        -  pin 5 */
	/* N/C        -  pin 6 */
	/* N/C        -  pin 7 */
}

static void init_rio_beta(void)
{
	printk(KERN_WARNING "%s: Rio Beta GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0      - pin  A0 */
	/* PVD1      - pin  A1 */
	/* PVD2      - pin  A2 */
	/* PVD3      - pin  A3 */
	/* PVD4      - pin  A4 */
	/* PVD5      - pin  A5 */
	/* PVD6      - pin  A6 */
	/* PVD7      - pin  A7 */

	/* TX0       - pin  A8 */
	/* N/C       - pin  A9 */
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_PORT_A | 10;
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_PORT_A | 11;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_PORT_A | 12;
	gpio_map[DPAD_UP]		= LF2000_GPIO_PORT_A | 13;
	gpio_map[BUTTON_ESC]		= LF2000_GPIO_PORT_A | 14;	/* schematic calls this HOME_BTN_L */
	gpio_map[NEONODE_BSL_RX]	= LF2000_GPIO_PORT_A | 15;

	gpio_map[NEONODE_BSL_TX]	= LF2000_GPIO_PORT_A | 16;
	gpio_map[MAGNETO_DRDY]		= LF2000_GPIO_PORT_A | 17;
	gpio_map[LFP100_INT]		= LF2000_GPIO_PORT_A | 18;
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_PORT_A | 19;
	/* N/C       - pin A20 */
	/* I2SDOUT   - pin A21 */
	/* I2SBCLK   - pin A22 */
	/* I2SDIN    - pin A23 */

	/* I2SSYNC   - pin A24 */
	/* I2SMCLK   - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	gpio_map[LED_ENA]		= LF2000_GPIO_PORT_A | 30;
	gpio_map[TP_INT]		= LF2000_GPIO_PORT_A | 31;

	/* GPIO Port B */
	/* SDCLK0    - pin  B0 */
	/* SDCMD0    - pin  B1 */
	/* SDDAT0[0] - pin  B2 */
	/* SDDAT0[1] - pin  B3 */
	/* SDDAT0[2] - pin  B4 */
	/* SDDAT0[3] - pin  B5 */
	/* SDCLK1    - pin  B6 */
	/* SDCMD1    - pin  B7 */

	/* SDDAT1[0] - pin  B8 */
	/* SDDAT1[1] - pin  B9 */
	/* SDDAT1[2] - pin B10 */
	/* SDDAT1[3] - pin B11 */
	/* SPI nCS   - pin B12 */
	/* SPI CLK   - pin B13 */
	/* SPI SDO   - pin B14 */
	/* SPI SDI   - pin B15 */

	/* PVD8      - pin B16 */
	/* PVD9      - pin B17 */
	/* PVD10     - pin B18 */
	/* PVD11     - pin B19 */
	/* PVD12     - pin B20 */
	/* PVD13     - pin B21 */
	/* PVD14     - pin B22 */
	/* PVD15     - pin B23 */

	/* PVD16     - pin B24 */
	/* PVD17     - pin B25 */
	/* PVD18     - pin B26 */
	/* PVD19     - pin B27 */
	/* PVD20     - pin B28 */
	/* PVD21     - pin B29 */
	/* PVD22     - pin B30 */
	/* PVD23     - pin B31 */

	/* GPIO Port C */
	/* VCLK      - pin C0  */
	/* VID[7]    - pin C1  */
	gpio_map[USB_POWER_FLT_L]	= LF2000_GPIO_PORT_C | 2;
	gpio_map[LCD_SPI]		= LF2000_GPIO_PORT_C | 3;
	/* SPI CLK   - pin C4  */
	gpio_map[MAGNETO_INT]		= LF2000_GPIO_PORT_C | 5;
	/* SPI SDI   - pin C6  */
	gpio_map[CHG_FLT]		= LF2000_GPIO_PORT_C | 7;

	/* VID[0]    - pin C8  */
	/* VID[1]    - pin C9  */
	/* VID[2]    - pin C10 */
	/* VID[3]    - pin C11 */
	/* VID[4]    - pin C12 */
	/* VID[5]    - pin C13 */
	/* VID[6]    - pin C14 */
	gpio_map[TOUCHSCREEN_Y2]	= LF2000_GPIO_PORT_C | 15;

	gpio_map[TOUCHSCREEN_X2]	= LF2000_GPIO_PORT_C | 16;
	gpio_map[NEONODE_TEST]		= LF2000_GPIO_PORT_C | 17;
	gpio_map[NEONODE_RST]		= LF2000_GPIO_PORT_C | 18;
	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_C | 19;
	gpio_map[NAND_CHIP_SELECT]	= LF2000_GPIO_PORT_C | 20;
	gpio_map[REAR_CAM_ENABLE_L]	= LF2000_GPIO_PORT_C | 21;
	/* VCLKB      - pin C22 */
	gpio_map[REAR_CAM_RESET_L]	= LF2000_GPIO_PORT_C | 23;

	gpio_map[RCAM_CLK_ENA_L]	= LF2000_GPIO_PORT_C | 24;
	/* VID_B0     - pin C25 */
	/* VID_B1     - pin C26 */
	/* VID_B2     - pin C27 */
	/* VID_B3     - pin C28 */
	/* VID_B4     - pin C29 */
	/* VID_B5     - pin C30 */
	/* VID_B6     - pin C31 */

	/* GPIO Port D */

	/* VID_B7     - pin  D0 */
	/* SD8        - pin  D1 */
	/* SD9        - pin  D2 */
	/* SD10       - pin  D3 */
	/* SD11       - pin  D4 */
	/* SD12       - pin  D5 */
	/* SD13       - pin  D6 */
	/* SD14       - pin  D7 */

	/* SD15       - pin  D8 */
	gpio_map[ACCEL_INT]		= LF2000_GPIO_PORT_D | 9;
	gpio_map[FRONT_CAM_RESET_L]	= LF2000_GPIO_PORT_D | 10;
	gpio_map[WIFI_RESET]		= LF2000_GPIO_PORT_D | 11;
	gpio_map[FRONT_CAM_ENABLE_L]	= LF2000_GPIO_PORT_D | 12;
	gpio_map[FCAM_CLK_ENA_L]	= LF2000_GPIO_PORT_D | 13;
	gpio_map[HEADPHONE_JACK]	= LF2000_GPIO_PORT_D | 14;
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_PORT_D | 15;

	/* SA15       - pin D16 */
	/* SA16       - pin D17 */
	gpio_map[USB_CHG_DETECT]	= LF2000_GPIO_PORT_D | 18;
	gpio_map[USB_OTG_PWR_ENA]	= LF2000_GPIO_PORT_D | 19;
	/* PVCLK      - pin D20 */
	/* PDE        - pin D21 */
	/* PHSYNC     - pin D22 */
	/* PVSYNC     - pin D23 */

	/* VCLK27     - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C        -  pin 0 */
	/* N/C        -  pin 1 */
	/* N/C        -  pin 2 */
	/* N/C        -  pin 3 */
	/* N/C        -  pin 4 */
	/* N/C        -  pin 5 */
	/* N/C        -  pin 6 */
	/* N/C        -  pin 7 */
}

static void init_rio_ep(void)
{
	printk(KERN_WARNING "%s: Rio EP GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0      - pin  A0 */
	/* PVD1      - pin  A1 */
	/* PVD2      - pin  A2 */
	/* PVD3      - pin  A3 */
	/* PVD4      - pin  A4 */
	/* PVD5      - pin  A5 */
	/* PVD6      - pin  A6 */
	/* PVD7      - pin  A7 */

	/* TX0       - pin  A8 */
	/* N/C       - pin  A9 */
	gpio_map[DPAD_UP]		= LF2000_GPIO_PORT_A | 10;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_PORT_A | 11;
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_PORT_A | 12;
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_PORT_A | 13;
	gpio_map[BUTTON_HOME]		= LF2000_GPIO_PORT_A | 14;	/* schematic calls this HOME_BTN_L */
	gpio_map[NEONODE_BSL_RX]	= LF2000_GPIO_PORT_A | 15;

	gpio_map[NEONODE_BSL_TX]	= LF2000_GPIO_PORT_A | 16;
	/* TX2       - pin A17 */
	gpio_map[LFP100_INT]		= LF2000_GPIO_PORT_A | 18;
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_PORT_A | 19;
	/* N/C       - pin A20 */
	/* I2SDOUT   - pin A21 */
	/* I2SBCLK   - pin A22 */
	/* I2SDIN    - pin A23 */

	/* I2SSYNC   - pin A24 */
	/* I2SMCLK   - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	gpio_map[LED_ENA]		= LF2000_GPIO_PORT_A | 30;
	gpio_map[TP_INT]		= LF2000_GPIO_PORT_A | 31;

	/* GPIO Port B */
	/* SDCLK0    - pin  B0 */
	/* SDCMD0    - pin  B1 */
	/* SDDAT0[0] - pin  B2 */
	/* SDDAT0[1] - pin  B3 */
	/* SDDAT0[2] - pin  B4 */
	/* SDDAT0[3] - pin  B5 */
	/* SDCLK1    - pin  B6 */
	/* SDCMD1    - pin  B7 */

	/* SDDAT1[0] - pin  B8 */
	/* SDDAT1[1] - pin  B9 */
	/* SDDAT1[2] - pin B10 */
	/* SDDAT1[3] - pin B11 */
	/* SPI nCS   - pin B12 */
	/* SPI CLK   - pin B13 */
	/* SPI SDO   - pin B14 */
	/* SPI SDI   - pin B15 */

	/* PVD8      - pin B16 */
	/* PVD9      - pin B17 */
	/* PVD10     - pin B18 */
	/* PVD11     - pin B19 */
	/* PVD12     - pin B20 */
	/* PVD13     - pin B21 */
	/* PVD14     - pin B22 */
	/* PVD15     - pin B23 */

	/* PVD16     - pin B24 */
	/* PVD17     - pin B25 */
	/* PVD18     - pin B26 */
	/* PVD19     - pin B27 */
	/* PVD20     - pin B28 */
	/* PVD21     - pin B29 */
	/* PVD22     - pin B30 */
	/* PVD23     - pin B31 */

	/* GPIO Port C */
	/* VCLK      - pin C0  */
	/* VID[7]    - pin C1  */
	gpio_map[USB_POWER_FLT_L]	= LF2000_GPIO_PORT_C | 2;
	gpio_map[LCD_SPI]		= LF2000_GPIO_PORT_C | 3;
	/* SPI CLK   - pin C4  */
	/*           - pin C5  */
	/* SPI SDI   - pin C6  */
	gpio_map[CHG_FLT]		= LF2000_GPIO_PORT_C | 7;

	/* VID[0]    - pin C8  */
	/* VID[1]    - pin C9  */
	/* VID[2]    - pin C10 */
	/* VID[3]    - pin C11 */
	/* VID[4]    - pin C12 */
	/* VID[5]    - pin C13 */
	/* VID[6]    - pin C14 */
	/* nSCS[2]   - pin C15 */

	/* nSCS[3]   - pin C16 */
	gpio_map[NEONODE_TEST]		= LF2000_GPIO_PORT_C | 17;
	gpio_map[NEONODE_RST]		= LF2000_GPIO_PORT_C | 18;
	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_C | 19;
	gpio_map[NAND_CHIP_SELECT]	= LF2000_GPIO_PORT_C | 20;
	gpio_map[REAR_CAM_ENABLE_L]	= LF2000_GPIO_PORT_C | 21;
	/* VCLKB      - pin C22 */
	gpio_map[REAR_CAM_RESET_L]	= LF2000_GPIO_PORT_C | 23;

	gpio_map[RCAM_CLK_ENA_L]	= LF2000_GPIO_PORT_C | 24;
	/* VID_B0     - pin C25 */
	/* VID_B1     - pin C26 */
	/* VID_B2     - pin C27 */
	/* VID_B3     - pin C28 */
	/* VID_B4     - pin C29 */
	/* VID_B5     - pin C30 */
	/* VID_B6     - pin C31 */

	/* GPIO Port D */

	/* VID_B7     - pin  D0 */
	/* SD8        - pin  D1 */
	/* SD9        - pin  D2 */
	/* SD10       - pin  D3 */
	/* SD11       - pin  D4 */
	/* SD12       - pin  D5 */
	/* SD13       - pin  D6 */
	/* SD14       - pin  D7 */

	/* SD15       - pin  D8 */
	gpio_map[ACCEL_INT]		= LF2000_GPIO_PORT_D | 9;
	gpio_map[FRONT_CAM_RESET_L]	= LF2000_GPIO_PORT_D | 10;
	gpio_map[WIFI_RESET]		= LF2000_GPIO_PORT_D | 11;
	gpio_map[FRONT_CAM_ENABLE_L]	= LF2000_GPIO_PORT_D | 12;
	gpio_map[FCAM_CLK_ENA_L]	= LF2000_GPIO_PORT_D | 13;
	gpio_map[HEADPHONE_JACK]	= LF2000_GPIO_PORT_D | 14;
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_PORT_D | 15;

	/* SA15       - pin D16 */
	/* SA16       - pin D17 */
	gpio_map[USB_CHG_DETECT] = LF2000_GPIO_PORT_D | 18;
	/* gpio_map[USB_OTG_PWR_ENA]	= LF2000_GPIO_PORT_D | 19; */
	/* PVCLK      - pin D20 */
	/* PDE        - pin D21 */
	/* PHSYNC     - pin D22 */
	/* PVSYNC     - pin D23 */

	/* VCLK27     - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C        -  pin 0 */
	/* N/C        -  pin 1 */
	/* N/C        -  pin 2 */
	/* N/C        -  pin 3 */
	/* N/C        -  pin 4 */
	/* N/C        -  pin 5 */
	/* N/C        -  pin 6 */
	/* N/C        -  pin 7 */
}

static void init_valencia(void)
{
	printk(KERN_WARNING "%s: Valencia GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0      - pin  A0 */
	/* PVD1      - pin  A1 */
	/* PVD2      - pin  A2 */
	/* PVD3      - pin  A3 */
	/* PVD4      - pin  A4 */
	/* PVD5      - pin  A5 */
	/* PVD6      - pin  A6 */
	/* PVD7      - pin  A7 */

	/* TX0       - pin  A8 */
	/* N/C       - pin  A9 */
	/* N/C       - pin A10 */
	/* N/C       - pin A11 */
	gpio_map[REAR_CAM_RESET_L]	= LF2000_GPIO_PORT_A | 12;
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_PORT_A | 13;
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_PORT_A | 14;
	gpio_map[USBD_ID_SENSE]		= LF2000_GPIO_PORT_A | 15;

	gpio_map[HEADPHONE_JACK]	= LF2000_GPIO_PORT_A | 16;
	gpio_map[USB_OVP]		= LF2000_GPIO_PORT_A | 17;
	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_A | 18;
	gpio_map[ACCEL_INT]		= LF2000_GPIO_PORT_A | 19;
	gpio_map[DCIN_OVP]		= LF2000_GPIO_PORT_A | 20;
	/* I2SDOUT   - pin A21 */
	/* I2SBCLK   - pin A22 */
	/* I2SDIN    - pin A23 */

	/* I2SSYNC   - pin A24 */
	/* I2SMCLK   - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	gpio_map[LED_ENA]		= LF2000_GPIO_PORT_A | 30;
	/* N/C       - pin A31 */

	/* GPIO Port B */
	/* SDCLK0    - pin  B0 */
	/* SDCMD0    - pin  B1 */
	/* SDDAT0[0] - pin  B2 */
	/* SDDAT0[1] - pin  B3 */
	/* SDDAT0[2] - pin  B4 */
	/* SDDAT0[3] - pin  B5 */
	/* SDCLK1    - pin  B6 */
	/* SDCMD1    - pin  B7 */

	/* SDDAT1[0] - pin  B8 */
	/* SDDAT1[1] - pin  B9 */
	/* SDDAT1[2] - pin B10 */
	/* SDDAT1[3] - pin B11 */
	/* SPI nCS   - pin B12 */
	/* SPI CLK   - pin B13 */
	/* SPI SDO   - pin B14 */
	/* SPI SDI   - pin B15 */

	/* PVD8      - pin B16 */
	/* PVD9      - pin B17 */
	/* PVD10     - pin B18 */
	/* PVD11     - pin B19 */
	/* PVD12     - pin B20 */
	/* PVD13     - pin B21 */
	/* PVD14     - pin B22 */
	/* PVD15     - pin B23 */

	/* PVD16     - pin B24 */
	/* PVD17     - pin B25 */
	/* PVD18     - pin B26 */
	/* PVD19     - pin B27 */
	/* PVD20     - pin B28 */
	/* PVD21     - pin B29 */
	/* PVD22     - pin B30 */
	/* PVD23     - pin B31 */

	/* GPIO Port C */
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_PORT_C | 0;
	gpio_map[WIFI_RESET]		= LF2000_GPIO_PORT_C | 1;
	gpio_map[REAR_CAM_ENABLE_L]	= LF2000_GPIO_PORT_C | 2;
	gpio_map[LCD_SPI]		= LF2000_GPIO_PORT_C | 3;
	/* SPI CLK   - pin  C4 */
	/* SPI SDO   - pin  C5 */
	/* SPI SDI   - pin  C6 */
	gpio_map[PWR_LED]		= LF2000_GPIO_PORT_C | 7;

	gpio_map[CHG_DISABLE]		= LF2000_GPIO_PORT_C | 8;
	gpio_map[DPAD_UP]		= LF2000_GPIO_PORT_C | 9;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_PORT_C | 10;
	gpio_map[BUTTON_HOME]		= LF2000_GPIO_PORT_C | 11;	/* schematic calls this HOME_BTN_L */
	gpio_map[RT_BATTPACK_DET_L]	= LF2000_GPIO_PORT_C | 12;
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_PORT_C | 13;
	gpio_map[LFP100_INT]		= LF2000_GPIO_PORT_C | 14;
	gpio_map[TOUCHSCREEN_Y2]	= LF2000_GPIO_PORT_C | 15;

	gpio_map[TOUCHSCREEN_X2]	= LF2000_GPIO_PORT_C | 16;
	gpio_map[TOUCHSCREEN_Y1]	= LF2000_GPIO_PORT_C | 17;
	gpio_map[TOUCHSCREEN_X1]	= LF2000_GPIO_PORT_C | 18;
	/* N/C        - pin C19 */
	gpio_map[NAND_CHIP_SELECT] = LF2000_GPIO_PORT_C | 20;
	/* VVSYNC_B   - pin C21 */
	/* VCLKB      - pin C22 */
	/* VDVALID    - pin C23 */

	/* VHSYNC_B   - pin C24 */
	/* VID_B0     - pin C25 */
	/* VID_B1     - pin C26 */
	/* VID_B2     - pin C27 */
	/* VID_B3     - pin C28 */
	/* VID_B4     - pin C29 */
	/* VID_B5     - pin C30 */
	/* VID_B6     - pin C31 */

	/* GPIO Port D */

	/* VID_B7     - pin  D0 */
	/* SD8        - pin  D1 */
	/* SD9        - pin  D2 */
	/* SD10       - pin  D3 */
	/* SD11       - pin  D4 */
	/* SD12       - pin  D5 */
	/* SD13       - pin  D6 */
	/* SD14       - pin  D7 */

	/* SD15       - pin  D8 */
	/* SA8        - pin  D9 */
	/* SA9        - pin D10 */
	/* SA10       - pin D11 */
	/* SA11       - pin D12 */
	/* SA12       - pin D13 */
	/* SA13       - pin D14 */
	/* SA14       - pin D15 */

	/* SA15       - pin D16 */
	/* SA16       - pin D17 */
	/* SA17       - pin D18 */
	/* SA18       - pin D19 */
	/* PVCLK      - pin D20 */
	/* PDE        - pin D21 */
	/* PHSYNC     - pin D22 */
	/* PVSYNC     - pin D23 */

	/* VCLK27     - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C        -  pin 0 */
	/* N/C        -  pin 1 */
	/* N/C        -  pin 2 */
	/* N/C        -  pin 3 */
	/* N/C        -  pin 4 */
	/* N/C        -  pin 5 */
	/* N/C        -  pin 6 */
	/* N/C        -  pin 7 */
}

static void init_valencia_ep(void)
{
	printk(KERN_WARNING "%s: Valencia EP GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0      - pin  A0 */
	/* PVD1      - pin  A1 */
	/* PVD2      - pin  A2 */
	/* PVD3      - pin  A3 */
	/* PVD4      - pin  A4 */
	/* PVD5      - pin  A5 */
	/* PVD6      - pin  A6 */
	/* PVD7      - pin  A7 */

	/* TX0       - pin  A8 */
	/* N/C       - pin  A9 */
	gpio_map[FRONT_CAM_ENABLE_L]	= LF2000_GPIO_PORT_A | 10;
	gpio_map[FRONT_CAM_RESET_L]	= LF2000_GPIO_PORT_A | 11;
	gpio_map[REAR_CAM_RESET_L]	= LF2000_GPIO_PORT_A | 12;
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_PORT_A | 13;
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_PORT_A | 14;
	gpio_map[USBD_ID_SENSE]		= LF2000_GPIO_PORT_A | 15;

	gpio_map[HEADPHONE_JACK]	= LF2000_GPIO_PORT_A | 16;
	gpio_map[USB_OVP]		= LF2000_GPIO_PORT_A | 17;
	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_A | 18;
	gpio_map[ACCEL_INT]		= LF2000_GPIO_PORT_A | 19;
	gpio_map[DCIN_OVP]		= LF2000_GPIO_PORT_A | 20;
	/* I2SDOUT   - pin A21 */
	/* I2SBCLK   - pin A22 */
	/* I2SDIN    - pin A23 */

	/* I2SSYNC   - pin A24 */
	/* I2SMCLK   - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	gpio_map[LED_ENA]		= LF2000_GPIO_PORT_A | 30;
	gpio_map[TP_DEBUG_NEONODE]		= LF2000_GPIO_PORT_A | 31;

	/* GPIO Port B */
	/* SDCLK0    - pin  B0 */
	/* SDCMD0    - pin  B1 */
	/* SDDAT0[0] - pin  B2 */
	/* SDDAT0[1] - pin  B3 */
	/* SDDAT0[2] - pin  B4 */
	/* SDDAT0[3] - pin  B5 */
	/* SDCLK1    - pin  B6 */
	/* SDCMD1    - pin  B7 */

	/* SDDAT1[0] - pin  B8 */
	/* SDDAT1[1] - pin  B9 */
	/* SDDAT1[2] - pin B10 */
	/* SDDAT1[3] - pin B11 */
	/* SPI nCS   - pin B12 */
	/* SPI CLK   - pin B13 */
	/* SPI SDO   - pin B14 */
	/* SPI SDI   - pin B15 */

	/* PVD8      - pin B16 */
	/* PVD9      - pin B17 */
	/* PVD10     - pin B18 */
	/* PVD11     - pin B19 */
	/* PVD12     - pin B20 */
	/* PVD13     - pin B21 */
	/* PVD14     - pin B22 */
	/* PVD15     - pin B23 */

	/* PVD16     - pin B24 */
	/* PVD17     - pin B25 */
	/* PVD18     - pin B26 */
	/* PVD19     - pin B27 */
	/* PVD20     - pin B28 */
	/* PVD21     - pin B29 */
	/* PVD22     - pin B30 */
	/* PVD23     - pin B31 */

	/* GPIO Port C */
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_PORT_C | 0;
	gpio_map[WIFI_RESET]		= LF2000_GPIO_PORT_C | 1;
	gpio_map[REAR_CAM_ENABLE_L]	= LF2000_GPIO_PORT_C | 2;
	gpio_map[LCD_SPI]		= LF2000_GPIO_PORT_C | 3;
	/* SPI CLK   - pin  C4 */
	/* SPI SDO   - pin  C5 */
	/* SPI SDI   - pin  C6 */
	gpio_map[PWR_LED]		= LF2000_GPIO_PORT_C | 7;

	gpio_map[CHG_DISABLE]		= LF2000_GPIO_PORT_C | 8;
	gpio_map[DPAD_UP]		= LF2000_GPIO_PORT_C | 9;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_PORT_C | 10;
	gpio_map[BUTTON_HOME]		= LF2000_GPIO_PORT_C | 11;	/* schematic calls this HOME_BTN_L */
	gpio_map[RT_BATTPACK_DET_L]	= LF2000_GPIO_PORT_C | 12;
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_PORT_C | 13;
	gpio_map[LFP100_INT]		= LF2000_GPIO_PORT_C | 14;
	gpio_map[TOUCHSCREEN_Y2]	= LF2000_GPIO_PORT_C | 15;

	gpio_map[TOUCHSCREEN_X2]	= LF2000_GPIO_PORT_C | 16;
	gpio_map[TOUCHSCREEN_Y1]	= LF2000_GPIO_PORT_C | 17;
	gpio_map[TOUCHSCREEN_X1]	= LF2000_GPIO_PORT_C | 18;
	/* N/C        - pin C19 */
	gpio_map[NAND_CHIP_SELECT] = LF2000_GPIO_PORT_C | 20;
	/* VVSYNC_B   - pin C21 */
	/* VCLKB      - pin C22 */
	/* VDVALID    - pin C23 */

	/* VHSYNC_B   - pin C24 */
	/* VID_B0     - pin C25 */
	/* VID_B1     - pin C26 */
	/* VID_B2     - pin C27 */
	/* VID_B3     - pin C28 */
	/* VID_B4     - pin C29 */
	/* VID_B5     - pin C30 */
	/* VID_B6     - pin C31 */

	/* GPIO Port D */

	/* VID_B7     - pin  D0 */
	/* SD8        - pin  D1 */
	/* SD9        - pin  D2 */
	/* SD10       - pin  D3 */
	/* SD11       - pin  D4 */
	/* SD12       - pin  D5 */
	/* SD13       - pin  D6 */
	/* SD14       - pin  D7 */

	/* SD15       - pin  D8 */
	/* SA8        - pin  D9 */
	/* SA9        - pin D10 */
	/* SA10       - pin D11 */
	/* SA11       - pin D12 */
	/* SA12       - pin D13 */
	/* SA13       - pin D14 */
	/* SA14       - pin D15 */

	/* SA15       - pin D16 */
	/* SA16       - pin D17 */
	/* SA17       - pin D18 */
	/* SA18       - pin D19 */
	/* PVCLK      - pin D20 */
	/* PDE        - pin D21 */
	/* PHSYNC     - pin D22 */
	/* PVSYNC     - pin D23 */

	/* VCLK27     - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C        -  pin 0 */
	/* N/C        -  pin 1 */
	/* N/C        -  pin 2 */
	/* N/C        -  pin 3 */
	/* N/C        -  pin 4 */
	/* N/C        -  pin 5 */
	/* N/C        -  pin 6 */
	/* N/C        -  pin 7 */
}

static void init_valencia_cip(void)
{
	printk(KERN_WARNING "%s: Valencia CIP GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0      - pin  A0 */
	/* PVD1      - pin  A1 */
	/* PVD2      - pin  A2 */
	/* PVD3      - pin  A3 */
	/* PVD4      - pin  A4 */
	/* PVD5      - pin  A5 */
	/* PVD6      - pin  A6 */
	/* PVD7      - pin  A7 */

	/* TX0       - pin  A8 */
	gpio_map[FCAM_CLK_ENA_L]	= LF2000_GPIO_PORT_A | 9;
	gpio_map[FRONT_CAM_ENABLE_L]	= LF2000_GPIO_PORT_A | 10;
	gpio_map[FRONT_CAM_RESET_L]	= LF2000_GPIO_PORT_A | 11;
	gpio_map[REAR_CAM_RESET_L]	= LF2000_GPIO_PORT_A | 12;
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_PORT_A | 13;
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_PORT_A | 14;
	gpio_map[USBD_ID_SENSE]		= LF2000_GPIO_PORT_A | 15;

	/* N/C       - pin A16 */
	gpio_map[USB_OVP]		= LF2000_GPIO_PORT_A | 17;
	/* N/C       - pin A18 */
	gpio_map[ACCEL_INT]		= LF2000_GPIO_PORT_A | 19;
	/* N/C       - pin A20 */
	/* I2SDOUT   - pin A21 */
	/* I2SBCLK   - pin A22 */
	/* I2SDIN    - pin A23 */

	/* I2SSYNC   - pin A24 */
	/* I2SMCLK   - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	gpio_map[LED_ENA]		= LF2000_GPIO_PORT_A | 30;
	gpio_map[RCAM_CLK_ENA_L]	= LF2000_GPIO_PORT_A | 31;

	/* GPIO Port B */
	/* SDCLK0    - pin  B0 */
	/* SDCMD0    - pin  B1 */
	/* SDDAT0[0] - pin  B2 */
	/* SDDAT0[1] - pin  B3 */
	/* SDDAT0[2] - pin  B4 */
	/* SDDAT0[3] - pin  B5 */
	/* SDCLK1    - pin  B6 */
	/* SDCMD1    - pin  B7 */

	/* SDDAT1[0] - pin  B8 */
	/* SDDAT1[1] - pin  B9 */
	/* SDDAT1[2] - pin B10 */
	/* SDDAT1[3] - pin B11 */
	/* SPI nCS   - pin B12 */
	/* SPI CLK   - pin B13 */
	/* SPI SDO   - pin B14 */
	/* SPI SDI   - pin B15 */

	/* PVD8      - pin B16 */
	/* PVD9      - pin B17 */
	/* PVD10     - pin B18 */
	/* PVD11     - pin B19 */
	/* PVD12     - pin B20 */
	/* PVD13     - pin B21 */
	/* PVD14     - pin B22 */
	/* PVD15     - pin B23 */

	/* PVD16     - pin B24 */
	/* PVD17     - pin B25 */
	/* PVD18     - pin B26 */
	/* PVD19     - pin B27 */
	/* PVD20     - pin B28 */
	/* PVD21     - pin B29 */
	/* PVD22     - pin B30 */
	/* PVD23     - pin B31 */

	/* GPIO Port C */
	/* VCLK      - pin C0  */
	/* VID[7]    - pin C1  */
	gpio_map[REAR_CAM_ENABLE_L]	= LF2000_GPIO_PORT_C | 2;
	gpio_map[LCD_SPI]		= LF2000_GPIO_PORT_C | 3;
	/* SPI CLK   - pin C4  */
	/* SPI SDO   - pin C5  */
	/* SPI SDI   - pin C6  */
	gpio_map[PWR_LED]		= LF2000_GPIO_PORT_C | 7;

	/* VID[0]    - pin C8  */
	/* VID[1]    - pin C9  */
	/* VID[2]    - pin C10 */
	/* VID[3]    - pin C11 */
	/* VID[4]    - pin C12 */
	/* VID[5]    - pin C13 */
	/* VID[6]    - pin C14 */
	gpio_map[TOUCHSCREEN_Y2]	= LF2000_GPIO_PORT_C | 15;

	gpio_map[TOUCHSCREEN_X2]	= LF2000_GPIO_PORT_C | 16;
	gpio_map[TOUCHSCREEN_Y1]	= LF2000_GPIO_PORT_C | 17;
	gpio_map[TOUCHSCREEN_X1]	= LF2000_GPIO_PORT_C | 18;
	/* N/C        - pin C19 */
	gpio_map[NAND_CHIP_SELECT] = LF2000_GPIO_PORT_C | 20;
	gpio_map[HEADPHONE_JACK]	= LF2000_GPIO_PORT_C | 21;
	/* VCLKB      - pin C22 */
	gpio_map[DCIN_OVP]		= LF2000_GPIO_PORT_C | 23;

	gpio_map[CARTRIDGE_DETECT]	= LF2000_GPIO_PORT_C | 24;
	/* VID_B0     - pin C25 */
	/* VID_B1     - pin C26 */
	/* VID_B2     - pin C27 */
	/* VID_B3     - pin C28 */
	/* VID_B4     - pin C29 */
	/* VID_B5     - pin C30 */
	/* VID_B6     - pin C31 */

	/* GPIO Port D */

	/* VID_B7     - pin  D0 */
	/* SD8        - pin  D1 */
	/* SD9        - pin  D2 */
	/* SD10       - pin  D3 */
	/* SD11       - pin  D4 */
	/* SD12       - pin  D5 */
	/* SD13       - pin  D6 */
	/* SD14       - pin  D7 */

	/* SD15       - pin  D8 */
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_PORT_D | 9;
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_PORT_D | 10;
	gpio_map[BUTTON_ESC]		= LF2000_GPIO_PORT_D | 11;	/* schematic calls this HOME_BTN_L */
	gpio_map[RT_BATTPACK_DET_L]	= LF2000_GPIO_PORT_D | 12;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_PORT_D | 13;
	gpio_map[LFP100_INT]		= LF2000_GPIO_PORT_D | 14;
	gpio_map[DPAD_UP]		= LF2000_GPIO_PORT_D | 15;

	/* SA15       - pin D16 */
	/* SA16       - pin D17 */
	gpio_map[CHG_DISABLE]		= LF2000_GPIO_PORT_D | 18;
	gpio_map[WIFI_RESET]		= LF2000_GPIO_PORT_D | 19;
	/* PVCLK      - pin D20 */
	/* PDE        - pin D21 */
	/* PHSYNC     - pin D22 */
	/* PVSYNC     - pin D23 */

	/* VCLK27     - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C        -  pin 0 */
	/* N/C        -  pin 1 */
	/* N/C        -  pin 2 */
	/* N/C        -  pin 3 */
	/* N/C        -  pin 4 */
	/* N/C        -  pin 5 */
	/* N/C        -  pin 6 */
	/* N/C        -  pin 7 */
}

static void init_vtk(void)
{

#define LF2000_GPIO_EXTENDER_BASE 0xC00
	printk(KERN_WARNING "%s: VTK GPIO mapping\n", __func__);

	/* GPIO Port A */
	/* PVD0    - pin  A0 */
	/* PVD1    - pin  A1 */
	/* PVD2    - pin  A2 */
	/* PVD3    - pin  A3 */
	/* PVD4    - pin  A4 */
	/* PVD5    - pin  A5 */
	/* PVD6    - pin  A6 */
	/* PVD7    - pin  A7 */

	/* TX0     - pin  A8 */
	/* N/C     - pin  A9 */
	gpio_map[PENDOWN_DETECT]	= LF2000_GPIO_PORT_A | 10;
	gpio_map[PENDOWN_CON]		= LF2000_GPIO_PORT_A | 11;
	/* N/C     - pin A12 */
	/* N/C     - pin A13 */
	/* N/C     - pin A14 */
	/* N/C     - pin A15 */

	/* N/C     - pin A16 */
	gpio_map[YMON_N]		= LF2000_GPIO_PORT_A | 17;
	gpio_map[YMON_P]		= LF2000_GPIO_PORT_A | 18;
	gpio_map[XMON_N]		= LF2000_GPIO_PORT_A | 19;
	gpio_map[XMON_P]		= LF2000_GPIO_PORT_A | 20;
	/* N/C     - pin A19 */
	/* N/C     - pin A20 */
	/* I2SDOUT - pin A21 */
	/* I2SBCLK - pin A22 */
	/* I2SDIN  - pin A23 */

	/* I2SSYNC - pin A24 */
	/* I2SMCLK - pin A25 */
	gpio_map[I2C_SCL0]		= LF2000_GPIO_PORT_A | 26;
	gpio_map[I2C_SDA0]		= LF2000_GPIO_PORT_A | 27;
	gpio_map[I2C_SCL1]		= LF2000_GPIO_PORT_A | 28;
	gpio_map[I2C_SDA1]		= LF2000_GPIO_PORT_A | 29;
	/* N/C     - pin A30 */
	gpio_map[LCD_PCI_ENB]		= LF2000_GPIO_PORT_A | 31;

	/* GPIO Port B */
	/* SDCLK0  - pin  B0 */
	/* SDCMD0  - pin  B1 */
	/* SDDAT0  - pin  B2 */
	/* SDDAT1  - pin  B3 */
	/* SDDAT2  - pin  B4 */
	/* SDDAT3  - pin  B5 */
	/* N/C     - pin  B6 */
	/* N/C     - pin  B7 */

	/* N/C     - pin  B8 */
	/* N/C     - pin  B9 */
	/* N/C     - pin B10 */
	/* N/C     - pin B11 */
	/* N/C      -pin B12 */
	/* SPI CLK - pin B13 */
	/* SPI SDO - pin B14 */
	/* SPI SDI - pin B15 */

	/* PVD8    - pin B16 */
	/* PVD9    - pin B17 */
	/* PVD10   - pin B18 */
	/* PVD11   - pin B19 */
	/* PVD12   - pin B20 */
	/* PVD13   - pin B21 */
	/* PVD14   - pin B22 */
	/* PVD15   - pin B23 */

	/* PVD16   - pin B24 */
	/* PVD17   - pin B25 */
	/* PVD18   - pin B26 */
	/* PVD19   - pin B27 */
	/* PVD20   - pin B28 */
	/* PVD21   - pin B29 */
	/* PVD22   - pin B30 */
	/* PVD23   - pin B31 */

	/* GPIO Port C */
	/* N/C      - pin C0  */
	/* N/C      - pin C1  */
	/* N/C      - pin C2  */
	/* N/C      - pin C3  */
	/* N/C      - pin C4  */
	/* N/C      - pin C5  */
	/* N/C      - pin C6  */
	/* N/C      - pin C7  */

	/* N/C      - pin C8  */
	/* N/C      - pin C9  */
	/* N/C      - pin C10 */
	/* N/C      - pin C11 */
	/* N/C      - pin C12 */
	/* N/C      - pin C13 */
	/* N/C      - pin C14 */
	/* N/C      - pin C15 */

	/* N/C      - pin C16 */
	/* N/C      - pin C17 */
	/* N/C      - pin C18 */
	/* N/C      - pin C19 */
	gpio_map[NAND_CHIP_SELECT] = LF2000_GPIO_PORT_C | 20;
	/* VVSYNC_B - pin C21 */
	/* VCLKB    - pin C22 */
	/* VDVALID  - pin C23 */

	/* VHSYNC_B - pin C24 */
	/* VID_B0   - pin C25 */
	/* VID_B1   - pin C26 */
	/* VID_B2   - pin C27 */
	/* VID_B3   - pin C28 */
	/* VID_B4   - pin C29 */
	/* VID_B5   - pin C30 */
	/* VID_B6   - pin C31 */

	/* GPIO Port D */

	/* VID_B7   - pin  D0 */
	/* SD8      - pin  D1 */
	/* SD9      - pin  D2 */
	/* SD10     - pin  D3 */
	/* SD11     - pin  D4 */
	/* SD12     - pin  D5 */
	/* SD13     - pin  D6 */
	/* SD14     - pin  D7 */

	/* SD15     - pin  D8 */
	/* SA8      - pin  D9 */
	/* SA9      - pin D10 */
	/* SA10     - pin D11 */
	/* SA11     - pin D12 */
	/* SA12     - pin D13 */
	/* SA13     - pin D14 */
	/* SA14     - pin D15 */

	/* SA15     - pin D16 */
	/* SA16     - pin D17 */
	/* SA17     - pin D18 */
	/* SA18     - pin D19 */
	/* PVCLK    - pin D20 */
	/* PDE      - pin D21 */
	/* PHSYNC   - pin D22 */
	/* PVSYNC   - pin D23 */

	/* VCLK27   - pin D24 */

	/* GPIO Port ALIVE */
	/* N/C -  pin 0 */
	/* N/C -  pin 1 */
	/* N/C -  pin 2 */
	/* N/C -  pin 3 */
	/* N/C -  pin 4 */
	/* N/C -  pin 5 */
	/* N/C -  pin 6 */
	/* N/C -  pin 7 */

	gpio_map[BUTTON_RED]		= LF2000_GPIO_EXTENDER_BASE + 7;
	gpio_map[BUTTON_B]		= LF2000_GPIO_EXTENDER_BASE + 8;
	gpio_map[BUTTON_A]		= LF2000_GPIO_EXTENDER_BASE + 9;
	gpio_map[BUTTON_VOLUMEDOWN]	= LF2000_GPIO_EXTENDER_BASE + 10;
	gpio_map[BUTTON_VOLUMEUP]	= LF2000_GPIO_EXTENDER_BASE + 11;
	gpio_map[DPAD_RIGHT]		= LF2000_GPIO_EXTENDER_BASE + 12;
	gpio_map[DPAD_LEFT]		= LF2000_GPIO_EXTENDER_BASE + 13;
	gpio_map[DPAD_DOWN]		= LF2000_GPIO_EXTENDER_BASE + 14;
	gpio_map[DPAD_UP]		= LF2000_GPIO_EXTENDER_BASE + 15;
}

int lf2000_gpio_map_valid = 0;

void lf2000_gpio_init_map(void)
{
	int i;

	/* mark all gpio mappings as invalid */
	for (i=0; i < (sizeof(gpio_map) / sizeof(gpio_map[0])); i++) {
		gpio_map[i] = LF2000_GPIO_PORT_NONE;
	}
	
	/* remap GPIOs based on Board Revision */
	switch(system_rev) {
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
			init_lucy();
			break;
		case LF2000_BOARD_LUCY_CIP:
			init_lucy_cip();
			break;

		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
			init_rio();
			break;

		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
			init_rio_beta();
			break;

		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
			init_rio_ep();
			break;

		case LF2000_BOARD_VALENCIA:
			init_valencia();
			break;

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
			init_valencia_ep();
			break;

		case LF2000_BOARD_VALENCIA_CIP:
			init_valencia_cip();
			break;

		case LF2000_BOARD_VTK:
			init_vtk();
			break;

 		default:
			printk(KERN_ERR "%s GPIO mapping does not support " \
				"Board Revision = 0x%X\n",
				__func__, system_rev);
#if   defined(CONFIG_PLAT_NXP3200_L2K) | \
      defined(CONFIG_PLAT_NXP3200_BID)
			printk(KERN_WARNING "defaulting to Lucy GPIO mapping\n");
			init_lucy();

#elif defined(CONFIG_PLAT_NXP3200_FAVELA) || \
      defined(CONFIG_PLAT_NXP3200_VALENCIA) || \
      defined(CONFIG_PLAT_NXP3200_M2K)
			printk(KERN_WARNING "defaulting to VALENCIA GPIO mapping\n");
			init_valencia();

#elif defined(CONFIG_PLAT_NXP3200_RIO)
			printk(KERN_WARNING "defaulting to Rio GPIO mapping\n");
			init_rio();

#elif defined(CONFIG_PLAT_NXP3200_RIO_BETA)
			printk(KERN_WARNING "defaulting to Rio Beta GPIO mapping\n");
			init_rio_beta();

#elif defined(CONFIG_PLAT_NXP3200_RIO_EP)
			printk(KERN_WARNING "defaulting to Rio Beta GPIO mapping\n");
			init_rio_ep();

#elif defined(CONFIG_PLAT_NXP3200_VALENCIA_CIP)
			printk(KERN_WARNING "defaulting to VALENCIA CIP GPIO mapping\n");
			init_valencia_cip();

#elif defined(CONFIG_PLAT_NXP3200_VTK)
			printk(KERN_WARNING "defaulting to VTK GPIO mapping\n");
			init_vtk();
#else
#error CONFIG_PLAT not set
#endif
			break;
	}
	lf2000_gpio_map_valid = 1;	/* initialized gpio_map[] */
}

extern unsigned lf2000_gpio_l2p( struct gpio_chip* chip, unsigned offset )
{
	/* ensure GPIO map initialized */
	if (!lf2000_gpio_map_valid)
		lf2000_gpio_init_map();

	/* ensure index is in range */
	if (offset < sizeof(gpio_map)) {
		return gpio_map[offset];
	} else {
		printk(KERN_WARNING "%s.%s:%d offset (%d) out of range\n",
			__FILE__, __func__, __LINE__, offset);
	}
	return LF2000_GPIO_PORT_NONE;
}
EXPORT_SYMBOL(lf2000_gpio_l2p);

extern void lf2000_gpio_check_didjfi(void) 
{
	/* Check to see if an OEM cartridge is inserted. If not, we claim
	CARTRIDGE DETECT as an output pin. This is used to drive the EN pin on
	the didj-fi. We *DON'T* want to do this if an OEM cartridge is here, as
	in that case the CARTRIDGE_DETECT line is shorted to ground and driving it
	high as an output could (theoretically) burn out the pin on the SOC. */
        gpio_request(CARTRIDGE_DETECT,"Didj-Fi EN");
        gpio_set_function(CARTRIDGE_DETECT, 0);
        gpio_direction_input(CARTRIDGE_DETECT);

		if(gpio_get_value(CARTRIDGE_DETECT)) {
		  printk("%s.%s:%d No OEM cart detected; CARTRIDGE_DETECT in didj-fi mode\n",
		     __FILE__, __func__, __LINE__),
		  gpio_direction_output(CARTRIDGE_DETECT, 1);

		} else {
		  printk("%s.%s:%d OEM cart detected; CARTRIDGE_DETECT in normal mode\n",
			__FILE__, __func__, __LINE__);
		  gpio_free(CARTRIDGE_DETECT);
		}
		return;
}
