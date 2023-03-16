
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
#include <linux/version.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <linux/vmalloc.h>
#include <asm/pgtable.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/map_desc.h>
#include <mach/memory.h>

#if (0)
#define DBGOUT(msg...)		{ printk("cpu: " msg); }
#define _IOMAP()		{							\
	int i = 0;										\
	for (; i<ARRAY_SIZE(cpu_iomap_desc); i++) 		\
		printk(KERN_INFO "cpu: iomap[%2d]: p 0x%08x -> v 0x%08x len=0x%x\n", i,	\
			(u_int)(cpu_iomap_desc[i].pfn<<12),		\
			(u_int)(cpu_iomap_desc[i].virtual),		\
			(u_int)(cpu_iomap_desc[i].length));		\
	}
#else
#define DBGOUT(msg...)		do {} while (0)
#define _IOMAP()
#endif

/* extern for cpu */
extern void 	 		cpu_base_init(void);	/* Not init section for suspend */
extern void __init 		cpu_init_irq(void);
extern void __init 		cpu_soc_init(void);
extern void __init 		cpu_device(void);
extern struct sys_timer cpu_sys_timer;

/* extern for board */
extern void 	 		board_init(void);		/* Not init section for suspend */
extern void __init 		board_device(void);

/*------------------------------------------------------------------------------
 * 	cpu initialize and io/memory map.
 * 	procedure: fixup -> map_io -> init_irq -> timer init -> init_machine
 */
static void __init cpu_fixup(
	struct tag *tags, char **cmdline, struct meminfo *mi
	)
{
	DBGOUT("%s\n", __func__);
	/*
	 * system momory  = system_size + linear_size
	 */
    mi->nr_banks     	= 1;
	mi->bank[0].start 	= CFG_MEM_PHY_SYSTEM_BASE;
    mi->bank[0].size	= CFG_MEM_PHY_SYSTEM_SIZE + CFG_MEM_PHY_DMAZONE_SIZE;

    #if KERNEL_VERSION(2,6,35) > LINUX_VERSION_CODE
    mi->bank[0].node  	= 0;
	#endif
}

extern void vmem_initialize_map_io(void);

static void __init cpu_map_io(void)
{
	/* debug */
	_IOMAP();

	/* initialize memory space outside of Linux */
	vmem_initialize_map_io();

	/* make iotable */
	iotable_init(cpu_iomap_desc, ARRAY_SIZE(cpu_iomap_desc));
	init_consistent_dma_size(CONSISTENT_DMA_SIZE);

	/* init cpu */
	cpu_base_init();
}

static void __init cpu_init_machine(void)
{
	DBGOUT("%s\n", __func__);

	board_init();
	cpu_soc_init();

	/*
	 * register platform device
	 */
	cpu_device();
	board_device();
}

#if KERNEL_VERSION(2,6,37) > LINUX_VERSION_CODE
void __init cpu_adjust_zones(int node, unsigned long *zone_size, unsigned long *zhole_size)
#else
void __init cpu_adjust_zones(unsigned long *zone_size, unsigned long *zhole_size)
#endif
{
 	unsigned int sz = CFG_MEM_PHY_DMAZONE_SIZE >> PAGE_SHIFT;

	zone_size [1] = zone_size[0] - sz;	/* Normal zone */
	zone_size [0] = sz;			/* DMA zone */
	zhole_size[1] = zhole_size[0];
	zhole_size[0] = 0;
}

/*------------------------------------------------------------------------------
 * Maintainer: Nexell Co., Ltd.
 */
#include <mach/iomap.h>
extern void lf2000_restart(char mode, const char *cmd);

MACHINE_START(NXP3200, CFG_SYS_CPU_NAME)
    #if KERNEL_VERSION(2,6,37) > LINUX_VERSION_CODE
	.phys_io	=  (__PB_IO_MAP_REGS_PHYS),
	.io_pg_offst	=  (__PB_IO_MAP_REGS_VIRT >> 18) & 0xfffc,
	#endif
	.atag_offset	=  0x100,
	.fixup		=  cpu_fixup,
	.map_io		=  cpu_map_io,
	.init_irq	=  cpu_init_irq,
	.timer		= &cpu_sys_timer,
	.init_machine	=  cpu_init_machine,
	.restart	=  lf2000_restart,
MACHINE_END
