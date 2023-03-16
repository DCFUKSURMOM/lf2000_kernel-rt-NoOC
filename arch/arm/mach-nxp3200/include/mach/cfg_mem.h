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
 *	   2009/05/13 first implementation
 ------------------------------------------------------------------------------*/
#ifndef __CFG_MEM_H__
#define __CFG_MEM_H__

/*------------------------------------------------------------------------------
 * 	Linux dram memory map
 *
 *	|					|
 *	|	Linux Kernel	|
 *	|	Code			|
 *	|					|
 *	|	Linux Kernel	|
 *	|	PTS				|
 *	-------------------------	0x80208000	: Linux PTS and Kernel Code
 *	|	Linux Kernel	|
 *	|	Params			|
 *	-------------------------	0x80200100	: Linux Params
 *	|   suspend mode	|
 *	|	back up data	|
 *	=========================	0x80200000	: Linux Kernel TEXT_BASE
 *  |                   |
 *  |   U-Boot Code     |
 *  |                   |
 *  -------------------------   0x80100000  : U-Boot TEXT_BASE
 *  |                   |            		: U-Boot initialzie heap size
 *  | U-Boot Heap 		|					: CONFIG_SYS_MALLOC_LEN (512K)
 *  | 	(BLD Stack)    	|
 *  |                   |
 *  -------------------------   0x80080000  : U-Boot Stack
 *  | U-Boot Stack      |					: CONFIG_STACKSIZE (256k)
 *  |	(BLD Code)    	|
 *  .........................   0x80010000  : BLD TEXT_BASE
 *  |   (BLD PTS)		|       (64K)
 *  .........................   0x80004000  : BLD page table (PTS)
 *  |   Nand ECC        |       (16K ~ 32K)
 *  |   or vector table |
 *  =========================   0x80000000  : Memory Base
 *
 ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * 	 On-Boot/U-Boot Loader zone
 */
#define CFG_BLD_VECT_TABLE_BASE			0x80000000	/* 0K ~ 16K */
#define	CFG_BLD_VECT_TABLE_SIZE			0x00004000	/* 16K */

#define	CFG_BLD_PAGE_TABLE_BASE			0x80004000	/* 16K ~ 32K (must be aligned wiht 16k) */
#define	CFG_BLD_PAGE_TABLE_SIZE			0x00008000	/* 32K */
#define	CFG_BLD_BOOT_STACK_TOP			0x80100000	/* Use under the stacp_top 0x5000~0x100000 (704) */

/*------------------------------------------------------------------------------
 * 	 Kernel / Suspend zone
 */
#define	CFG_KERNEL_TEXT_BASE			0x80200000
#define	CFG_SLEEP_DATA_BASE 			0x80200000	/* max 256 byte */

#if defined(CONFIG_PLAT_NXP3200_RIO)

/*------------------------------------------------------------------------------
 * 	 System memory map
 */
#define	CFG_MEM_VIR_SYSTEM_BASE			0x80200000	/* System, must be at an evne 2MB boundary (head.S) */
#define	CFG_MEM_PHY_SYSTEM_BASE			0x80200000	/* System, must be at an evne 2MB boundary (head.S) */
#define	CFG_MEM_PHY_SYSTEM_SIZE			(162<<20)	/* 34MB + 128MB */

/*------------------------------------------------------------------------------
 *   DMA zone
 */
#define	CFG_MEM_PHY_DMAZONE_SIZE 		(4<<20)		/* 4MB DMA zone */


/*------------------------------------------------------------------------------
 * 	 Reserved memory map
 */
#define	CFG_MEM_VIR_LINEAR_BASE			0xE0000000	/* OGL heap */
#define	CFG_MEM_PHY_LINEAR_BASE			(CFG_MEM_PHY_SYSTEM_BASE + CFG_MEM_PHY_SYSTEM_SIZE + CFG_MEM_PHY_DMAZONE_SIZE)	/* System / DMA */
#define	CFG_MEM_PHY_LINEAR_SIZE			(64<<20)	/* 64MB */

#define	CFG_MEM_VIR_BLOCK_BASE			(CFG_MEM_VIR_LINEAR_BASE + CFG_MEM_PHY_LINEAR_SIZE)	/* For video */
#define	CFG_MEM_PHY_BLOCK_BASE			(CFG_MEM_PHY_LINEAR_BASE + CFG_MEM_PHY_LINEAR_SIZE)	/* For video */
#define	CFG_MEM_PHY_BLOCK_SIZE			(24<<20)	/* 24MB */

#elif defined(CONFIG_PLAT_NXP3200_L2K) || defined(CONFIG_PLAT_NXP3200_M2K) || defined(CONFIG_PLAT_NXP3200_VALENCIA_CIP)

/*------------------------------------------------------------------------------
 * 	 System memory map
 */
#define	CFG_MEM_VIR_SYSTEM_BASE			0x80200000	/* System, must be at an evne 2MB boundary (head.S) */
#define	CFG_MEM_PHY_SYSTEM_BASE			0x80200000	/* System, must be at an evne 2MB boundary (head.S) */
#define	CFG_MEM_PHY_SYSTEM_SIZE			(62<<20)	/* 62MB */

/*------------------------------------------------------------------------------
 *   DMA zone
 */
#define	CFG_MEM_PHY_DMAZONE_SIZE 		(8<<20)		/* 8MB DMA zone */

/*------------------------------------------------------------------------------
 * 	 Reserved memory map
 */
#define	CFG_MEM_VIR_LINEAR_BASE			0xE0000000	/* System / DMA */
#define	CFG_MEM_PHY_LINEAR_BASE			(CFG_MEM_PHY_SYSTEM_BASE + CFG_MEM_PHY_SYSTEM_SIZE + CFG_MEM_PHY_DMAZONE_SIZE)	/* System / DMA */
#define	CFG_MEM_PHY_LINEAR_SIZE			(48<<20)	/* 48MB */

#define	CFG_MEM_VIR_BLOCK_BASE			(CFG_MEM_VIR_LINEAR_BASE + CFG_MEM_PHY_LINEAR_SIZE)	/* For video */
#define	CFG_MEM_PHY_BLOCK_BASE			(CFG_MEM_PHY_LINEAR_BASE + CFG_MEM_PHY_LINEAR_SIZE)	/* For video */
#define	CFG_MEM_PHY_BLOCK_SIZE			(8<<20)		/* 8MB */

#else /* L2K, M2K, etc */

/*------------------------------------------------------------------------------
 * 	 System memory map
 */
#define	CFG_MEM_VIR_SYSTEM_BASE			0x80200000	/* System, must be at an evne 2MB boundary (head.S) */
#define	CFG_MEM_PHY_SYSTEM_BASE			0x80200000	/* System, must be at an evne 2MB boundary (head.S) */
#define	CFG_MEM_PHY_SYSTEM_SIZE			(62<<20)	/* 62MB */

/*------------------------------------------------------------------------------
 *   DMA zone
 */
#undef CFG_MEM_PHY_DMAZONE_SIZE			/* DMA shared with contiguous linear region below */

/*------------------------------------------------------------------------------
 * 	 Reserved memory map
 */
#define	CFG_MEM_VIR_LINEAR_BASE			(CFG_MEM_VIR_SYSTEM_BASE + CFG_MEM_PHY_SYSTEM_SIZE)	/* System / DMA */
#define	CFG_MEM_PHY_LINEAR_BASE			(CFG_MEM_PHY_SYSTEM_BASE + CFG_MEM_PHY_SYSTEM_SIZE)	/* System / DMA */
#define	CFG_MEM_PHY_LINEAR_SIZE			(48<<20)	/* 48MB */

#define	CFG_MEM_VIR_BLOCK_BASE			0xE0000000	/* For video */
#define	CFG_MEM_PHY_BLOCK_BASE			(CFG_MEM_PHY_LINEAR_BASE + CFG_MEM_PHY_LINEAR_SIZE)	/* For video */
#define	CFG_MEM_PHY_BLOCK_SIZE			(16<<20)	/* 16MB */

#endif

/*------------------------------------------------------------------------------
 * 	NOTE> Linux kernel memory map
 *
 *  =========================
 *  |                   |
 *  | 	Reserved       	|		Length	 = 	CFG_MEM_PHY_BLOCK_SIZE
 *  |	for 2D			|				    NOTE>
 *  |                   |					when not defined CFG_MEM_PHY_BLOCK_SIZE, 2D reserved length is 'zero',
 *  |                   |				 	and 2D allocator will be allocate from '1D reserved region')
 *  |                   |
 *  =========================	Physical = 	CFG_MEM_PHY_BLOCK_BASE,
 *  |                   |		Virtual  = 	CFG_MEM_VIR_BLOCK_BASE
 *  |                   |
 *  | 	Reserved       	|		Length	 = 	CFG_MEM_PHY_LINEAR_SIZE
 *  |	for 1D			|				    NOTE>
 *  |                   |					when not defined CFG_MEM_PHY_LINEAR_SIZE, 1D reserved length is 'zero',
 *  |                   |				 	and 1D allocator will be allocate from 'kernel DMA zone')
 *  |                   |
 *  =========================	Physical = 	CFG_MEM_PHY_LINEAR_BASE
 *  |                   |		Virtual  = 	CFG_MEM_VIR_LINEAR_BASE
 *  |                   |
 *  | 	Kernel       	|		Length	 = 	CFG_MEM_PHY_DMAZONE_SIZE or CFG_MEM_PHY_LINEAR_SIZE
 *  |	DMA zone		| 					NOTE>
 *  |                   |					if defined CFG_MEM_PHY_DMAZONE_SIZE 'DMA zone length' is CFG_MEM_PHY_DMAZONE_SIZE,
 *  |                   |				 	else 'DMA zone length' is CFG_MEM_PHY_LINEAR_SIZE
 *  -------------------------
 *  |                   |
 *  | 	Kernel       	|
 *  |	Normal zone		|		Length	 = 	CFG_MEM_PHY_SYSTEM_SIZE
 *  |                   |
 *  |                   |
 *  =========================   Physical = 	CFG_MEM_PHY_SYSTEM_BASE, not use CFG_MEM_VIR_SYSTEM_BASE
 *								Virtual	 = 	0xC0000000

 ------------------------------------------------------------------------------*/

#endif /* __CFG_MEM_H__ */
