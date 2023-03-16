/* arch/arm/mach-s3c2410/include/mach/regs-iic.h
 *
 * Copyright (c) 2004 Simtec Electronics <linux@simtec.co.uk>
 *		http://www.simtec.co.uk/products/SWLINUX/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * S3C2410 I2C Controller
*/

#ifndef __ASM_ARCH_REGS_IIC_H
#define __ASM_ARCH_REGS_IIC_H __FILE__

/* taken from the Samsung I2C driver */
/* see s3c2410x user guide, v1.1, section 9 (p447) for more info */

#define NXP3200_IICREG(x) (x)

#define NXP3200_IICCON    	NXP3200_IICREG(0x00)
#define NXP3200_IICSTAT   	NXP3200_IICREG(0x04)
#define NXP3200_IICADD    	NXP3200_IICREG(0x08)
#define NXP3200_IICDS     	NXP3200_IICREG(0x0C)
//#define S3C2440_IICLC	  	NXP3200_IICREG(0x10)
#define NXP3200_IICQCNTMAX	NXP3200_IICREG(0x10)
#define NXP3200_IICIRQPEND	NXP3200_IICREG(0x24)
#define NXP3200_IICCLKENB	NXP3200_IICREG(0x100)

#define NXP3200_IICCON_ACKEN		(1<<7)
#define NXP3200_IICCON_TXDIV_16		(0<<6)
#define NXP3200_IICCON_TXDIV_256	(1<<6)
#define NXP3200_IICCON_IRQEN		(1<<5)
//#define NXP3200_IICCON_IRQPEND		(1<<4)
#define NXP3200_IICCON_SCALE(x)		((x)&15)
#define NXP3200_IICCON_SCALEMASK	(0xf)

#define NXP3200_IICSTAT_ST_ENB		(1<<12)

#define NXP3200_IICSTAT_MASTER_RX	(2<<6)
#define NXP3200_IICSTAT_MASTER_TX	(3<<6)
#define NXP3200_IICSTAT_SLAVE_RX	(0<<6)
#define NXP3200_IICSTAT_SLAVE_TX	(1<<6)
#define NXP3200_IICSTAT_MODEMASK	(3<<6)

#define NXP3200_IICSTAT_START		(1<<5)
#define NXP3200_IICSTAT_BUSBUSY		(1<<5)
#define NXP3200_IICSTAT_TXRXEN		(1<<4)
#define NXP3200_IICSTAT_ARBITR		(1<<3)
#define NXP3200_IICSTAT_ASSLAVE		(1<<10)	/* bit 2 on Samsung */
#define NXP3200_IICSTAT_ADDR0		(1<<9)  /* bit 1 on Samsung */
#define NXP3200_IICSTAT_LASTBIT		(1<<0)

#define NXP3200_IICIRQPEND_HOLD		(1<<1)
#define NXP3200_IICIRQPEND_PEND		(1<<0)

/* not available on NXP3200 */
#if 0
#define S3C2410_IICLC_SDA_DELAY0	(0 << 0)
#define S3C2410_IICLC_SDA_DELAY5	(1 << 0)
#define S3C2410_IICLC_SDA_DELAY10	(2 << 0)
#define S3C2410_IICLC_SDA_DELAY15	(3 << 0)
#define S3C2410_IICLC_SDA_DELAY_MASK	(3 << 0)

#define S3C2410_IICLC_FILTER_ON		(1<<2)
#endif

#define NXP3200_IICCLKENB_PCLKMODE	(1<<3)

#endif /* __ASM_ARCH_REGS_IIC_H */
