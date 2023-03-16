/* linux/arch/arm/mach-xxx/include/mach/reg_otg.h
 *
 * Copyright (C) 2004 Herbert Poetzl <herbert@13thfloor.at>
 *
 * This include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
*/

#ifndef __ASM_ARCH_REGS_USB_OTG_HS_H
#define __ASM_ARCH_REGS_USB_OTG_HS_H

#include "iomap.h"
#include "nxp3200_irq.h"

#define IRQ_OTG						IRQ_PHY_OTG
#define SOC_PA_OTG					0xC0024000
#define	SOC_VA_OTG					IO_ADDRESS(SOC_PA_OTG)


/* USB2.0 OTG Controller register */
#define SOC_USBOTGREG(x) 			((x) + SOC_VA_OTG)

/* LF2000 USBOTG registers */
#define LF2000_USBOTG_PHYPOR			SOC_USBOTGREG(0x1840)
#define LF2000_USBOTG_INTENB			SOC_USBOTGREG(0x1844)
#define LF2000_USBOTG_INTPEND			SOC_USBOTGREG(0x1848)
#define LF2000_USBOTG_TESTPARM3			SOC_USBOTGREG(0x184C)
#define LF2000_USBOTG_TESTPARM4			SOC_USBOTGREG(0x1850)
#define LF2000_USBOTG_LINKCTL			SOC_USBOTGREG(0x1854)
#define LF2000_USBOTG_TESTPARM7			SOC_USBOTGREG(0x185C)
#define	LF2000_USBOTG_CLKENB			SOC_USBOTGREG(0x1FC0)
#define	LF2000_USBOTG_CLKGEN			SOC_USBOTGREG(0x1FC4)

/*============================================================================================== */
	/* Core Global Registers */
#define S3C_UDC_OTG_GOTGCTL			SOC_USBOTGREG(0x0000)		/* OTG Control & Status */
#define S3C_UDC_OTG_GOTGINT			SOC_USBOTGREG(0x0004)		/* OTG Interrupt */
#define S3C_UDC_OTG_GAHBCFG			SOC_USBOTGREG(0x0008)		/* Core AHB Configuration */
#define S3C_UDC_OTG_GUSBCFG			SOC_USBOTGREG(0x000C)		/* Core USB Configuration */
#define S3C_UDC_OTG_GRSTCTL			SOC_USBOTGREG(0x0010)		/* Core Reset */
#define S3C_UDC_OTG_GINTSTS			SOC_USBOTGREG(0x0014)		/* Core Interrupt */
#define S3C_UDC_OTG_GINTMSK			SOC_USBOTGREG(0x0018)		/* Core Interrupt Mask */
#define S3C_UDC_OTG_GRXSTSR			SOC_USBOTGREG(0x001C)		/* Receive Status Debug Read/Status Read */
#define S3C_UDC_OTG_GRXSTSP			SOC_USBOTGREG(0x0020)		/* Receive Status Debug Pop/Status Pop */
#define S3C_UDC_OTG_GRXFSIZ			SOC_USBOTGREG(0x0024)		/* Receive FIFO Size */
#define S3C_UDC_OTG_GNPTXFSIZ		SOC_USBOTGREG(0x0028)		/* Non-Periodic Transmit FIFO Size */
#define S3C_UDC_OTG_GNPTXSTS		SOC_USBOTGREG(0x002C)		/* Non-Periodic Transmit FIFO/Queue Status */

#define S3C_UDC_OTG_HPTXFSIZ		SOC_USBOTGREG(0x0100)		/* Host Periodic Transmit FIFO Size */
#define S3C_UDC_OTG_DIEPTXF(n)		SOC_USBOTGREG(0x0104 + (n-1)*0x4)/* Device IN EP Transmit FIFO Size Register */

/*============================================================================================== */
/* Host Mode Registers */
/*------------------------------------------------ */
/* Host Global Registers */
#define S3C_UDC_OTG_HCFG			SOC_USBOTGREG(0x0400)		/* Host Configuration */
#define S3C_UDC_OTG_HFIR			SOC_USBOTGREG(0x0404)		/* Host Frame Interval */
#define S3C_UDC_OTG_HFNUM			SOC_USBOTGREG(0x0408)		/* Host Frame Number/Frame Time Remaining */
#define S3C_UDC_OTG_HPTXSTS			SOC_USBOTGREG(0x0410)		/* Host Periodic Transmit FIFO/Queue Status */
#define S3C_UDC_OTG_HAINT			SOC_USBOTGREG(0x0414)		/* Host All Channels Interrupt */
#define S3C_UDC_OTG_HAINTMSK		SOC_USBOTGREG(0x0418)		/* Host All Channels Interrupt Mask */

/*------------------------------------------------ */
/* Host Port Control & Status Registers */
#define S3C_UDC_OTG_HPRT			SOC_USBOTGREG(0x0440)		/* Host Port Control & Status */

/*------------------------------------------------ */
/* Host Channel-Specific Registers */
#define S3C_UDC_OTG_HCCHAR0			SOC_USBOTGREG(0x0500)		/* Host Channel-0 Characteristics */
#define S3C_UDC_OTG_HCSPLT0			SOC_USBOTGREG(0x0504)		/* Host Channel-0 Split Control */
#define S3C_UDC_OTG_HCINT0			SOC_USBOTGREG(0x0508)		/* Host Channel-0 Interrupt */
#define S3C_UDC_OTG_HCINTMSK0		SOC_USBOTGREG(0x050C)		/* Host Channel-0 Interrupt Mask */
#define S3C_UDC_OTG_HCTSIZ0			SOC_USBOTGREG(0x0510)		/* Host Channel-0 Transfer Size */
#define S3C_UDC_OTG_HCDMA0			SOC_USBOTGREG(0x0514)		/* Host Channel-0 DMA Address */


/*============================================================================================== */
/* Device Mode Registers */
/*------------------------------------------------ */
/* Device Global Registers */
#define S3C_UDC_OTG_DCFG			SOC_USBOTGREG(0x0800)		/* Device Configuration */
#define S3C_UDC_OTG_DCTL			SOC_USBOTGREG(0x0804)		/* Device Control */
#define S3C_UDC_OTG_DSTS			SOC_USBOTGREG(0x0808)		/* Device Status */
#define S3C_UDC_OTG_DIEPMSK			SOC_USBOTGREG(0x0810)		/* Device IN Endpoint Common Interrupt Mask */
#define S3C_UDC_OTG_DOEPMSK			SOC_USBOTGREG(0x0814)		/* Device OUT Endpoint Common Interrupt Mask */
#define S3C_UDC_OTG_DAINT			SOC_USBOTGREG(0x0818)		/* Device All Endpoints Interrupt */
#define S3C_UDC_OTG_DAINTMSK		SOC_USBOTGREG(0x081C)		/* Device All Endpoints Interrupt Mask */
#define S3C_UDC_OTG_DTKNQR1			SOC_USBOTGREG(0x0820)		/* Device IN Token Sequence Learning Queue Read 1 */
#define S3C_UDC_OTG_DTKNQR2			SOC_USBOTGREG(0x0824)		/* Device IN Token Sequence Learning Queue Read 2 */
#define S3C_UDC_OTG_DVBUSDIS		SOC_USBOTGREG(0x0828)		/* Device VBUS Discharge Time */
#define S3C_UDC_OTG_DVBUSPULSE		SOC_USBOTGREG(0x082C)		/* Device VBUS Pulsing Time */
#define S3C_UDC_OTG_DTKNQR3			SOC_USBOTGREG(0x0830)		/* Device IN Token Sequence Learning Queue Read 3 */
#define S3C_UDC_OTG_DTKNQR4			SOC_USBOTGREG(0x0834)		/* Device IN Token Sequence Learning Queue Read 4 */

/*------------------------------------------------ */
/* Device Logical IN Endpoint-Specific Registers */
#define S3C_UDC_OTG_DIEPCTL(n)		SOC_USBOTGREG(0x0900 + n*0x20)	/* Device IN Endpoint n Control */
#define S3C_UDC_OTG_DIEPINT(n)		SOC_USBOTGREG(0x0908 + n*0x20)	/* Device IN Endpoint n Interrupt */
#define S3C_UDC_OTG_DIEPTSIZ(n)		SOC_USBOTGREG(0x0910 + n*0x20)	/* Device IN Endpoint n Transfer Size */
#define S3C_UDC_OTG_DIEPDMA(n)		SOC_USBOTGREG(0x0914 + n*0x20)	/* Device IN Endpoint n DMA Address */

/*------------------------------------------------ */
/* Device Logical OUT Endpoint-Specific Registers */
#define S3C_UDC_OTG_DOEPCTL(n)		SOC_USBOTGREG(0x0B00 + n*0x20)	/* Device OUT Endpoint n Control */
#define S3C_UDC_OTG_DOEPINT(n)		SOC_USBOTGREG(0x0B08 + n*0x20)	/* Device OUT Endpoint n Interrupt */
#define S3C_UDC_OTG_DOEPTSIZ(n)		SOC_USBOTGREG(0x0B10 + n*0x20)	/* Device OUT Endpoint n Transfer Size */
#define S3C_UDC_OTG_DOEPDMA(n)		SOC_USBOTGREG(0x0B14 + n*0x20)	/* Device OUT Endpoint n DMA Address */

/*------------------------------------------------ */
/* Endpoint FIFO address */
#define S3C_UDC_OTG_EP0_FIFO		SOC_USBOTGREG(0x3000 + 0x040)
#define S3C_UDC_OTG_EP1_FIFO		SOC_USBOTGREG(0x3000 + 0x080)
#define S3C_UDC_OTG_EP2_FIFO		SOC_USBOTGREG(0x3000 + 0x0C0)
#define S3C_UDC_OTG_EP3_FIFO		SOC_USBOTGREG(0x3000 + 0x100)
#define S3C_UDC_OTG_EP4_FIFO		SOC_USBOTGREG(0x3000 + 0x140)
#define S3C_UDC_OTG_EP5_FIFO		SOC_USBOTGREG(0x3000 + 0x180)
#define S3C_UDC_OTG_EP6_FIFO		SOC_USBOTGREG(0x3000 + 0x1C0)
#define S3C_UDC_OTG_EP7_FIFO		SOC_USBOTGREG(0x3000 + 0x200)

/*===================================================================== */
/*definitions related to CSR setting */

/* S3C_UDC_OTG_GOTGCTL */
#define B_SESSION_VALID				(0x1<<19)
#define A_SESSION_VALID				(0x1<<18)

/* S3C_UDC_OTG_GAHBCFG */
#define PTXFE_HALF					(0<<8)
#define PTXFE_ZERO					(1<<8)
#define NPTXFE_HALF					(0<<7)
#define NPTXFE_ZERO					(1<<7)
#define MODE_SLAVE					(0<<5)
#define MODE_DMA					(1<<5)
#define BURST_SINGLE				(0<<1)
#define BURST_INCR					(1<<1)
#define BURST_INCR4					(3<<1)
#define BURST_INCR8					(5<<1)
#define BURST_INCR16				(7<<1)
#define GBL_INT_UNMASK				(1<<0)
#define GBL_INT_MASK				(0<<0)

/* S3C_UDC_OTG_GRSTCTL */
#define AHB_MASTER_IDLE				(1u<<31)
#define CORE_SOFT_RESET				(0x1<<0)

/* S3C_UDC_OTG_GINTSTS/S3C_UDC_OTG_GINTMSK core interrupt register */
#define INT_RESUME					(1u<<31)
#define INT_DISCONN					(0x1<<29)
#define INT_CONN_ID_STS_CNG			(0x1<<28)
#define INT_OUT_EP					(0x1<<19)
#define INT_IN_EP					(0x1<<18)
#define INT_ENUMDONE				(0x1<<13)
#define INT_RESET					(0x1<<12)
#define INT_SUSPEND					(0x1<<11)
#define INT_EARLY_SUSPEND			(0x1<<10)
#define INT_NP_TX_FIFO_EMPTY		(0x1<<5)
#define INT_RX_FIFO_NOT_EMPTY		(0x1<<4)
#define INT_SOF						(0x1<<3)
#define INT_DEV_MODE				(0x0<<0)
#define INT_HOST_MODE				(0x1<<1)
#define	INT_GOUTNakEff				(0x01<<7)
#define	INT_GINNakEff				(0x01<<6)

#define FULL_SPEED_CONTROL_PKT_SIZE	8
#define FULL_SPEED_BULK_PKT_SIZE	64

#define HIGH_SPEED_CONTROL_PKT_SIZE	64
#define HIGH_SPEED_BULK_PKT_SIZE	512

#ifndef CONFIG_PLAT_S5P64XX
#define RX_FIFO_SIZE				2048
#define NPTX_FIFO_START_ADDR		RX_FIFO_SIZE
#define NPTX_FIFO_SIZE				2048
#define PTX_FIFO_SIZE				2048
#else
#define RX_FIFO_SIZE				1024
#define NPTX_FIFO_START_ADDR		RX_FIFO_SIZE
#define NPTX_FIFO_SIZE				256
#define PTX_FIFO_SIZE				256
#endif

#define DEPCTL_TXFNUM_0				(0x0<<22)
#define DEPCTL_TXFNUM_1				(0x1<<22)
#define DEPCTL_TXFNUM_2				(0x2<<22)
#define DEPCTL_TXFNUM_3				(0x3<<22)
#define DEPCTL_TXFNUM_4				(0x4<<22)


/* Enumeration speed */
#define USB_HIGH_30_60MHZ			(0x0<<1)
#define USB_FULL_30_60MHZ			(0x1<<1)
#define USB_LOW_6MHZ				(0x2<<1)
#define USB_FULL_48MHZ				(0x3<<1)

/* S3C_UDC_OTG_GRXSTSP STATUS */
#define OUT_PKT_RECEIVED			(0x2<<17)
#define OUT_TRANSFER_COMPLELTED		(0x3<<17)
#define SETUP_TRANSACTION_COMPLETED	(0x4<<17)
#define SETUP_PKT_RECEIVED			(0x6<<17)
#define GLOBAL_OUT_NAK				(0x1<<17)


/* S3C_UDC_OTG_DCTL device control register */
#define NORMAL_OPERATION			(0x1<<0)
#define SOFT_DISCONNECT				(0x1<<1)
#define TEST_CONTROL_MASK			(0x7<<4)
#define TEST_J_MODE					(0x1<<4)
#define TEST_K_MODE					(0x2<<4)
#define TEST_SE0_NAK_MODE			(0x3<<4)
#define TEST_PACKET_MODE			(0x4<<4)
#define TEST_FORCE_ENABLE_MODE		(0x5<<4)

/* S3C_UDC_OTG_DAINT device all endpoint interrupt register */
#define DAINT_OUT_BIT				(16)
#define DAINT_MASK					(0xFFFF)



/* S3C_UDC_OTG_DIEPCTL0/DOEPCTL0 device control IN/OUT endpoint 0 control register */
#define DEPCTL_EPENA				(0x1<<31)
#define DEPCTL_EPDIS				(0x1<<30)
#define DEPCTL_SETD1PID				(0x1<<29)
#define DEPCTL_SETD0PID				(0x1<<28)
#define DEPCTL_SNAK					(0x1<<27)
#define DEPCTL_CNAK					(0x1<<26)
#define DEPCTL_STALL				(0x1<<21)
#define DEPCTL_TYPE_BIT				(18)
#define DEPCTL_TYPE_MASK			(0x3<<18)
#define DEPCTL_CTRL_TYPE			(0x0<<18)
#define DEPCTL_ISO_TYPE				(0x1<<18)
#define DEPCTL_BULK_TYPE			(0x2<<18)
#define DEPCTL_INTR_TYPE			(0x3<<18)
#define DEPCTL_USBACTEP				(0x1<<15)
#define DEPCTL_NEXT_EP_BIT			(11)
#define DEPCTL_MPS_BIT				(0)
#define DEPCTL_MPS_MASK				(0x7FF)

#define DEPCTL0_MPS_64				(0x0<<0)
#define DEPCTL0_MPS_32				(0x1<<0)
#define DEPCTL0_MPS_16				(0x2<<0)
#define DEPCTL0_MPS_8				(0x3<<0)
#define DEPCTL_MPS_BULK_512			(512<<0)
#define DEPCTL_MPS_INT_MPS_16		(16<<0)

#define DIEPCTL0_NEXT_EP_BIT		(11)

/* S3C_UDC_OTG_DIEPCTLn/DOEPCTLn device control IN/OUT endpoint n control register */

/* S3C_UDC_OTG_DIEPMSK/DOEPMSK device IN/OUT endpoint common interrupt mask register */
/* S3C_UDC_OTG_DIEPINTn/DOEPINTn device IN/OUT endpoint interrupt register */
#define BACK2BACK_SETUP_RECEIVED		(0x1<<6)
#define INTKNEPMIS						(0x1<<5)
#define INTKN_TXFEMP					(0x1<<4)
#define NON_ISO_IN_EP_TIMEOUT			(0x1<<3)
#define CTRL_OUT_EP_SETUP_PHASE_DONE	(0x1<<3)
#define AHB_ERROR						(0x1<<2)
#define EPDISBLD						(0x1<<1)
#define TRANSFER_DONE					(0x1<<0)

/*DIEPTSIZ0 / DOEPTSIZ0 */

/* DEPTSIZ common bit */
#define DEPTSIZ_PKT_CNT_BIT 			(19)
#define DEPTSIZ_XFER_SIZE_BIT			(0)

#define	DEPTSIZ_SETUP_PKCNT_1			(1<<29)
#define	DEPTSIZ_SETUP_PKCNT_2			(2<<29)
#define	DEPTSIZ_SETUP_PKCNT_3			(3<<29)

/* S3C_UDC_OTG_DSTS */
#define DSTS_SOFFN				(0x1FF<<8)
#define DSTS_ERRTICERR				(1<<3)
#define DSTS_ENUMSPD				(3<<1)
#define DSTS_SUSPSTS				(1<<0)

#endif
