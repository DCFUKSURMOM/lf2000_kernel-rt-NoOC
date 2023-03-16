/*
 * arch/arm/mach-nxp3200/include/mach/board_revisions.h
 *
 * LeapFrog board Revisions
 *
 * Scott Esters <sesters@leapfrog.com>
 *
 * This program is free software; you can redistributte it and/or modify
 * it under the terms of the GNU Gneral Public License as published by
 * the Free Software Foundation
 */

#ifndef BOARD_REVISIONS_H
#define BOARD_REVISIONS_H

/* This is a list of board types that can be detected at runtime to deal with
 * hardware quirks. This is passed to the kernel as the ATAG_REVISION value.
 * The kernel holds this value in the global 'system_rev'.
 */

/* removed LF1000 defines */
#if 0
/* LF1000 Development boards and original Didj Form Factor (alpha) board. */
#define LF1000_BOARD_DEV		0x00

/* Original Didj 08 (Legacy Rev A) */
#define LF1000_BOARD_DIDJ		0x03

/* Didj 09, 2GB MLC Flash, 64KB Boot Flash */
#define LF1000_BOARD_DIDJ_09		0x04

/* Acorn, 8GB SLC Flash, 64MB SDRAM, 512KB Boot Flash, TV Out, RTC SuperCap */
#define LF1000_BOARD_ACORN		0x05


/*
 * Emerald / Leapster3 Boards
 */

/* Leapster Explorer POP, 8GB SLC Flash, 64MB SDRAM,
 * 512KB Boot Flash, TV Out, No RTC SuperCap */
#define LF1000_BOARD_EMERALD_POP	0x01

/* Leapster Explorer, 8GB SLC Flash, 64MB SDRAM,
 * 512KB Boot Flash, No TV Out, No RTC SuperCap */
#define LF1000_BOARD_EMERALD_NOTV_NOCAP	0x02

/* Leapster Explorer, 8GB SLC Flash, 64MB SDRAM,
 * 512KB Boot Flash, TV Out, No RTC SuperCap */
#define LF1000_BOARD_EMERALD_TV_NOCAP	0x06

/* Leapster Explorer, 8GB SLC Flash, 64MB SDRAM,
 * 512KB Boot Flash, No TV Out, RTC SuperCap */
#define LF1000_BOARD_EMERALD_NOTV_CAP	0x07

/* Leapster Explorer 8GB SLC Flash, 64MB Samsung
 * SDRAM, 512KB Boot Flash, No TV Out, No RTC SuperCap */
#define LF1000_BOARD_EMERALD_SAMSUNG	0x0A


/*
 * K2 Boards
 */

/* K2 Base, 8GB SLC Flash, 64MB SDRAM, 512KB Boot Flash */
#define	LF1000_BOARD_K2			0x10


/*
 * Madrid Boards
 */

/* Madrid, 5" LCD 2GB MLC Flash + SD Controller, 64MB SDRAM,
 *512KB Boot Flash, No TV Out, No RTC SuperCap, Accelerometer,
 * Vibration motor, USB Camera SDIO WIFI  */
#define LF1000_BOARD_MADRID		0x0B

/* Madrid, 5" LCD 2GB MLC Flash + SD Controller, 64MB SDRAM,
 *512KB Boot Flash, TV Out, No RTC SuperCap, Accelerometer,
 * Vibration motor, USB Camera SDIO WIFI  */
#define LF1000_BOARD_MADRID_POP		0x0C

/* Madrid, 5" LCD 2GB MLC Flash + SD Controller, 64MB SDRAM,
 *512KB Boot Flash, No TV Out, No RTC SuperCap, Accelerometer,
 * Vibration motor, USB Camera SDIO WIFI, LFP100  */
#define LF1000_BOARD_MADRID_LFP100	0x0D
#endif

/*
 * LF2000 Boards
 */

#define LF2000_BOARD_UNKNOWN			0x200
#define LF2000_BOARD_VTK			0x201
//#define LF2000_BOARD_E2K			0x202	// unsupported
//#define LF2000_BOARD_E2K_SD			0x203	// unsupported
//#define LF2000_BOARD_M2K			0x204	// unsupported
#define LF2000_BOARD_LUCY			0x205
#define LF2000_BOARD_VALENCIA			0x206
#define LF2000_BOARD_VALENCIA_EP		0x207
#define LF2000_BOARD_VALENCIA_FEP		0x208
#define LF2000_BOARD_LUCY_PP			0x209
#define LF2000_BOARD_VALENCIA_EP_800_480	0x20A
#define LF2000_BOARD_VALENCIA_EP_800_480_8	0x20B
#define LF2000_BOARD_VALENCIA_EP_8		0x20C
#define LF2000_BOARD_VALENCIA_FEP_8		0x20D
#define LF2000_BOARD_VALENCIA_FEP_800_480	0x20E
#define LF2000_BOARD_VALENCIA_FEP_800_480_8	0x20F
#define LF2000_BOARD_VALENCIA_KND_800_480	0x210
#define LF2000_BOARD_VALENCIA_KND_800_480_8	0x211
#define LF2000_BOARD_VALENCIA_KND_1024_600	0x212

#define LF2000_BOARD_VALENCIA_KND_1024_600_8	0x217

#define LF2000_BOARD_LUCY_CIP			0x300

#define LF2000_BOARD_VALENCIA_CIP		0x310

#define LF2000_BOARD_RIO			0x320
#define LF2000_BOARD_RIO_KND_800_480		0x321

#define LF2000_BOARD_RIO_BETA_1024_600		0x322
#define LF2000_BOARD_RIO_BETA_800_480		0x323

#define LF2000_BOARD_RIO_BETA_1024_600_700_400	0x324
#define LF2000_BOARD_RIO_BETA_800_480_700_400	0x325

#define LF2000_BOARD_RIO_EP_800_333		0x326
#define LF2000_BOARD_RIO_EP_700_400		0x327
#define	LF2000_BOARD_RIO_EP_800_400		0x328
#define LF2000_BOARD_RIO_EP_550_275		0x329

#define LF2000_BOARD_RIO_BETA_1024_600_550_275	0x32A

/*
 * Board ID 0x32B reused so that FEP units will have PLL1 running at 327.667 MHz
 * #define LF2000_BOARD_RIO_FEP_800_333		0x32B
 */

#define LF2000_BOARD_RIO_FEP_800_327P666	0x32B

#define LF2000_BOARD_RIO_EP_666_333		0x32C

#define LF2000_BOARD_RIO_FEP_800_327P67		0x32D

static inline bool is_rio(int sysid) {
    return (sysid >= LF2000_BOARD_RIO);
}

#endif /* BOARD_REVISIONS_H */
