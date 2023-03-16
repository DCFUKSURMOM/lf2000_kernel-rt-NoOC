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
#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

#include <mach/cfg_mem.h>

#ifndef CFG_MEM_PHY_DMAZONE_SIZE
#define	CFG_MEM_PHY_DMAZONE_SIZE		CFG_MEM_PHY_LINEAR_SIZE
#define	MEM_DMAZONE_EQ_LINEAR			(1)
#else
#define	MEM_DMAZONE_EQ_LINEAR			(0)
#endif

/*
 * Physical DRAM offset.
 */
#define PHYS_OFFSET				CFG_MEM_PHY_SYSTEM_BASE

/*
 * DMA allocate size (func: dma_alloc_writecombine)
 */
#define CONSISTENT_DMA_SIZE		CFG_MEM_PHY_DMAZONE_SIZE

#ifndef __ASSEMBLY__
/*
 * DMA zone
 */
#include <linux/version.h>
#if KERNEL_VERSION(2,6,37) > LINUX_VERSION_CODE
void cpu_adjust_zones(int node, unsigned long *size, unsigned long *holes);
#define arch_adjust_zones(node, size, holes) 	cpu_adjust_zones(node, size, holes)
#else
void cpu_adjust_zones(unsigned long *size, unsigned long *holes);
#define arch_adjust_zones(size, holes) 			cpu_adjust_zones(size, holes)
#endif

#endif /* __ASSEMBLY__ */

#define ISA_DMA_THRESHOLD   (PHYS_OFFSET + CFG_MEM_PHY_DMAZONE_SIZE - 1)
/* #define MAX_DMA_ADDRESS     (PAGE_OFFSET + CFG_MEM_PHY_DMAZONE_SIZE) */

#endif	/*  __ASM_ARCH_MEMORY_H */

