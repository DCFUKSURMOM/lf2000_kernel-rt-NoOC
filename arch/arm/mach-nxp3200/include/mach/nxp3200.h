/*------------------------------------------------------------------------------
 *
 *	Copyright (C) 2005 Nexell Co., Ltd All Rights Reserved
 *	Nexell Co. Proprietary & Confidential
 *
 *	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 *  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 *  FOR A PARTICULAR PURPOSE.
 *
 *	Module     : System memory config
 *	Description:
 *	Author     : Platform Team
 *	Export     :
 *	History    :
 *	   2011/02/xx first implementation
 ------------------------------------------------------------------------------*/
#ifndef __NXP3200_H__
#define __NXP3200_H__

/*------------------------------------------------------------------------------
 * Physical Base Address
 */
#define PHY_BASEADDR_DMA			(0xC0000000)
#define PHY_BASEADDR_INTC           (0xC0001000)
#define PHY_BASEADDR_TIMER          (0xC0003000)
#define PHY_BASEADDR_VIP0           (0xC0005000)
#define PHY_BASEADDR_DPC            (0xC000607C)
#define PHY_BASEADDR_MLC            (0xC0008000)
#define PHY_BASEADDR_ROTATOR        (0xC0009000)
#define PHY_BASEADDR_ADC            (0xC000A000)
#define PHY_BASEADDR_MPEGTSIF       (0xC000E000)
#define PHY_BASEADDR_SSPSPI         (0xC000F000)
#define PHY_BASEADDR_CSC            (0xC0012000)
#define PHY_BASEADDR_SDHC           (0xC0013000)
#define PHY_BASEADDR_GPIO           (0xC0014000)
#define PHY_BASEADDR_PWM            (0xC0018000)
#define PHY_BASEADDR_AUDIO          (0xC001B000)
#define PHY_BASEADDR_I2C0           (0xC001C000)
#define PHY_BASEADDR_I2C1           (0xC001D000)
#define PHY_BASEADDR_CLKPWR         (0xC001E000)
#define PHY_BASEADDR_IDCT           (0xC001F000)
#define PHY_BASEADDR_OHCI           (0xC0020000)
#define PHY_BASEADDR_AES            (0xC0022000)
#define PHY_BASEADDR_OTG            (0xC0024000)
#define PHY_BASEADDR_DEINTERLACE	(0xC0028000)
#define PHY_BASEADDR_MCUD           (0xC0029000)
#define PHY_BASEADDR_MCUS           (0xC002B000)
#define PHY_BASEADDR_UART0          (0xC002C000)
#define PHY_BASEADDR_UART1          (0xC002C080)
#define PHY_BASEADDR_UART2          (0xC002D000)
#define PHY_BASEADDR_UART3          (0xC002D080)
#define PHY_BASEADDR_UART4          (0xC002E000)
#define PHY_BASEADDR_UART5          (0xC002E080)
#define PHY_BASEADDR_ALIVE          (0xC0032000)
#define PHY_BASEADDR_RTC            (0xC0032400)
#define PHY_BASEADDR_GRP3D          (0xC0034000)
#define PHY_BASEADDR_ECID         	(0xC003F000)

#define PHY_BASEADDR_NAND_MODULE	(PHY_BASEADDR_MCUS + 0x0000007C)	/* NAND controller */
#define PHY_BASEADDR_NAND_STATIC	(0x2C000000)	/* STATIC(0x00000000) + 0x2C000000 */

/*------------------------------------------------------------------------------
 * Interrupt
 */
#include "nxp3200_irq.h"

/*------------------------------------------------------------------------------
 * prototype header
 */
#ifndef __ASSEMBLY__

#include <nx_adc.h>
#include <nx_alive.h>
#include <nx_audio.h>
#include <nx_clkpwr.h>
#include <nx_csc.h>
#include <nx_deinterlace.h>
#include <nx_dma.h>
#include <nx_dpc.h>
#include <nx_ecid.h>
#include <nx_gpio.h>
#include <nx_i2c.h>
#include <nx_intc.h>
#include <nx_mcud.h>
#include <nx_mcus.h>
#include <nx_mlc.h>
#include <nx_mpegtsif.h>
#include <nx_ohci.h>
#include <nx_pwm.h>
#include <nx_rotator.h>
#include <nx_rtc.h>
#include <nx_sdhc.h>
#include <nx_sspspi.h>
#include <nx_timer.h>
#include <nx_uart.h>
#include <nx_vip.h>

#endif /* 	__ASSEMBLY__ */
#endif /*	__NXP3200_H__ */
