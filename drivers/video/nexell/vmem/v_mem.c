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
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/mach/map.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/vmem.h>

#include "v_alloc.h"

#if (0)
#define DBGOUT(msg...)	{ printk(KERN_INFO "vmem: " msg); }
#else
#define DBGOUT(msg...)	do {} while (0)
#endif

#define ERROUT(msg...)		{ 	\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",	\
			__FILE__, __FUNCTION__, __LINE__),			\
		printk(KERN_ERR msg); }

#define	ACTIVE_PROCESS	"system_server"

/*-------------------------------------------------------------------------*/
#define	_2D_BASE_ALIGN		( 1 * 1024 * 1024)	/* 2D base address align */
#define	_2D_SIZE_ALIGN		(16 * 1024 * 1024)	/* 2D size align */
#define	_2D_SIZE_MIN		( 4 * 1024 * 1024)	/* 2D min size */
#define	_2D_STRIDE			(4096)				/* 0x1000 */
#define	_2D_ZONE_OFFS		(0x20000000)
#define	KILL_WAIT_TIME		(1000)	/* ms */

#define	GET_PID()			task_tgid_vnr(get_current())
#define	GET_TID()			task_pid_nr(get_current())

/*--------------------------------------------------------------------------
 *	Set block memory map table
 --------------------------------------------------------------------------*/
struct PART_INFO {
	unsigned int phybase;
	unsigned int virbase;
	unsigned int length;
};

static struct PART_INFO __n1d_parts[] = {
#if (0 == MEM_DMAZONE_EQ_LINEAR)
	{
		.phybase = CFG_MEM_PHY_LINEAR_BASE,
		.virbase = CFG_MEM_VIR_LINEAR_BASE,
		.length  = CFG_MEM_PHY_LINEAR_SIZE,
	},
#endif
};

static struct PART_INFO __n2d_parts[] = {
#if defined(CFG_MEM_PHY_BLOCK_BASE)
	{
		.phybase = CFG_MEM_PHY_BLOCK_BASE,
		.virbase = CFG_MEM_VIR_BLOCK_BASE,
		.length  = CFG_MEM_PHY_BLOCK_SIZE,
	},
#endif
};

#define NUM_OF_1D_PARTS		(sizeof(__n1d_parts)/sizeof(__n1d_parts[0]))	/* total partition number */
#define NUM_OF_2D_PARTS		(sizeof(__n2d_parts)/sizeof(__n2d_parts[0]))	/* total partition number */

/* lock */
static DEFINE_MUTEX			(_mlock_);	/* allocator lock */
static spinlock_t			(_slock_);	/* list lock */

#define	VMEM_LOCK()			{ mutex_lock(&_mlock_); }
#define	VMEM_UNLOCK()		{ if (mutex_is_locked(&_mlock_)) mutex_unlock(&_mlock_); }

#define	LIST_LOCK_INIT()	{ spin_lock_init(&_slock_); }
#define	LIST_LOCK()			{ spin_lock(&_slock_); }
#define	LIST_UNLOCK()		{ spin_unlock(&_slock_); }

#define	LIST_ADD(x, l)		{ list_add(x, l); }
#define	LIST_DEL(x)			{ list_del(x); }
#define	LIST_LOCK_ADD(x, l)	{ spin_lock(&_slock_); list_add(x, l); spin_unlock(&_slock_); }
#define	LIST_LOCK_DEL(x)	{ spin_lock(&_slock_); list_del(x);    spin_unlock(&_slock_); }

#if defined(CFG_MEM_PHY_BLOCK_BASE)
struct list_head 	__n2d_parts_list[NUM_OF_2D_PARTS];	/* root node */
#else
struct list_head 	__n2d_parts_list[1];	/* root node */
#endif
struct device *		__pdevptr = NULL;
static u64  		__dmamask = DMA_BIT_MASK(32);

/*--------------------------------------------------------------------------------
 *	__list_top_node
 *		|
 *	__list_drv_node.head / __list_drv_node.node
 *		|						|
 *		|					driver list -> driver list -> driver list....
 *		|
 *	file VM_LIST->head  / VM_LIST->node
 *		|						|
 *		|					user list -> user list -> user list....
 *		|
 *	file VM_LIST->head  / VM_LIST->node
 *		|						|
 *		|					user list -> user list -> user list....
 *		|
 *
 ---------------------------------------------------------------------------------*/
typedef struct {
	MCB2DNODE			node2d;
	unsigned int 		segbase;
	unsigned int 		width;
	unsigned int 		height;
	unsigned int 		reserved;
	struct list_head 	list;
} MCB2DBASE;

struct VM_NODE {
	unsigned int	  tid;		/* thread id */
	void			* handle;
	unsigned int 	  flags;
	unsigned int 	  physical;
	unsigned int 	  virtual;
	unsigned int	  width;
	unsigned int	  height;
	struct list_head  list;
};

#define	TASK_NODE_RESV		(1<< 0)
#define	TASK_NODE_KILL		(1<< 1)

#define	ALLOC_SYS_MEM		(1<<31)
#define ALLOC_DMA_MEM		CONFIG_GRP_NEXELL_VMEM_ALLOC_DMA_MAX

struct VM_LIST {
	struct list_head node;		/* private node list */
	struct list_head head;		/* all node list */
	unsigned int	 pid;		/* process id */
	unsigned int	 flags;
	unsigned int	 total_1d;
	unsigned int	 total_2d;
};

static LIST_HEAD		(__list_top_node);
static struct VM_LIST 	(__list_drv_node);
static volatile int 	__task_kill_on = 0;

/*---------------------------------------------------------------------------------*/
struct vmem_device {
	struct vmem_plat_data *plat;
};

struct vmem_device	__vmemdev;

/*---------------------------------------------------------------------------------*/
#define K(x) ((x) << (PAGE_SHIFT-10))
static void show_dmazone(void)
{
	struct zone *zone;
	for_each_populated_zone(zone) {
 		unsigned long nr[MAX_ORDER], flags, order, total = 0;
		printk("%s: ", zone->name);
		spin_lock_irqsave(&zone->lock, flags);
		for (order = 0; order < MAX_ORDER; order++) {
			nr[order] = zone->free_area[order].nr_free;
			total += nr[order] << order;
		}
		spin_unlock_irqrestore(&zone->lock, flags);
		for (order = 0; order < MAX_ORDER; order++)
			printk("%lu*%lukB ", nr[order], K(1UL) << order);
		printk("= %lukB\n", K(total));
	}
}

/*
 * cat /sys/devices/platform/vmem.0/vmem
 */
static ssize_t vmem_show_attr(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct list_head *lhead = NULL, *list = NULL;
	struct VM_LIST   *vlist = NULL;
	struct VM_NODE	 *pnode = NULL;
	uint total = 0, size = 0;

	LIST_LOCK();

	printk(KERN_INFO "\n");
	list_for_each(lhead, &__list_top_node) {
		vlist = list_entry(lhead, struct VM_LIST, head);
		list_for_each(list, &vlist->node) {
			pnode  = list_entry(list, struct VM_NODE, list);
			size   = pnode->width * pnode->height;
			total += PAGE_ALIGN(size);

			printk(KERN_INFO "[%4d] *h:0x%08x, a:0x%08x, w:%8d, h:%6d [%s:%s] \n",
				pnode->tid, (u_int)pnode->handle, pnode->physical,
				pnode->width, pnode->height,
				pnode->flags & VMEM_BLOCK_BUFFER ? "2D":"1D",
				pnode->flags & ALLOC_SYS_MEM ? "S":"R");
		}
		printk(KERN_INFO "[%4d] Total 1D : %8d, 2D : %8d, %s\n",
				vlist->pid, vlist->total_1d, vlist->total_2d,
				find_task_by_vpid(vlist->pid)->comm);
	}

	if (NUM_OF_1D_PARTS)
		MCB1D_ShowStatus();

	show_dmazone();

	printk(KERN_INFO "All Total = %8d\n", total);

	LIST_UNLOCK();
	return 1;
}

#if defined(CONFIG_GRP_NEXELL_VMEM_KILLER)

static int vmem_killer(int type2d)
{
	struct task_struct *task  = NULL;
	struct list_head   *lhead = NULL;
	struct VM_LIST *vlist = NULL;
	int task_equl =  0;
	int task_pid  = -1;

#if defined(CONFIG_GRP_NEXELL_VMEM_KILL_LIST_OGL)
 	int (*ext_func)(void) = NULL;
	extern int KnlGetPrimCandidatePIDToRemove(void);

 	ext_func = symbol_get(KnlGetPrimCandidatePIDToRemove);
 	if (ext_func)
		task_pid = ext_func();
	printk(KERN_INFO "%s: get kill task pid %4d from OGL ...\n", __func__, task_pid);
#endif

	LIST_LOCK();

	/* search from previous list */
	list_for_each_prev(lhead, &__list_top_node) {

		vlist = list_entry(lhead, struct VM_LIST, head);

		printk(KERN_INFO "%s: Check %4d (resv:%s, killed:%s), %s\n",
			__func__, vlist->pid, vlist->flags&TASK_NODE_RESV?"O":"X",
			vlist->flags&TASK_NODE_KILL?"O":"X", find_task_by_vpid(vlist->pid)->comm);

		if (TASK_NODE_KILL & vlist->flags)
			break;

		if (TASK_NODE_RESV & vlist->flags) {
			task_pid = (task_pid == vlist->pid ? -1 : task_pid);
			continue;
		}

		/* check 1d used */
		if (!type2d && !vlist->total_1d && NUM_OF_2D_PARTS)
			continue;

		/* check 2d used */
		if (type2d && !vlist->total_2d)
		if (NUM_OF_2D_PARTS || !vlist->total_1d)
			continue;

 		if (task_pid > 0 && task_pid != vlist->pid)
 			continue;

		task_equl = (vlist->pid == GET_PID());
		task_pid  = (vlist->pid);

		/* if disabled, kill own process */
		#if (0)
		if (task_equl)
			break;
		#endif

		vlist->flags |= TASK_NODE_KILL;
		task = find_task_by_vpid(task_pid);
		break;
	}
	LIST_UNLOCK();

	if (task) {
		int wait = 0;
		printk(KERN_INFO "%s: Send kill signal %4d, %s\n", __func__, task_pid, task->comm);

		force_sig(SIGTERM, task);	// send_sig(SIGKILL, task, 1);

		/* wait for task kill */
		if (! task_equl) {
			printk(KERN_INFO "%s: Killing %s, wait %d ms...\n",
				__func__, task->comm, KILL_WAIT_TIME);
			__task_kill_on = 1;
			while (KILL_WAIT_TIME > wait && __task_kill_on) {
				wait++;
				msleep(1);
			}
			printk(KERN_INFO "%s: %s kill %s  ...\n",
				__func__, __task_kill_on?"Fail":"Done", task->comm);
			return __task_kill_on ? -1: 0;
		}
		printk(KERN_INFO "%s: Exit kill task \n", __func__);
	}
	printk(KERN_ERR "%s: Not found kill task ...\n", __func__);
	return -1;
}
#endif

/*---------------------------------------------------------------------------------*/
/* called as part of MACHINE_START() -> map_io */
void vmem_1dn_init_map_io(void)
{
	struct map_desc   desc[NUM_OF_1D_PARTS];
	struct PART_INFO *part = NULL;
	unsigned int phys[NUM_OF_1D_PARTS];
	unsigned int virt[NUM_OF_1D_PARTS];
	unsigned int size[NUM_OF_1D_PARTS];
	int i = 0;

	for(i = 0; NUM_OF_1D_PARTS > i; i++) {
		part  = &__n1d_parts[i];
		phys[i] = part->phybase;
		virt[i] = part->virbase;
		size[i] = part->length;

		desc[i].virtual = part->virbase;
		desc[i].pfn	 	= __phys_to_pfn(part->phybase);
		desc[i].length  = part->length;
		desc[i].type    = MT_MEMORY_NONCACHED;	/* MT_DEVICE, MT_MEMORY_NONCACHED */

		iotable_init(&desc[i], i+1);
		printk(KERN_INFO "%s: 1D map[%d]: p 0x%08x -> v 0x%08x len=%dM\n",
			VMEM_DEV_NAME, i, (u_int)(desc[i].pfn<<12), (u_int)(desc[i].virtual),
			(u_int)(desc[i].length/SZ_1M));
	}
}

static int vmem_1dn_init(void)
{
	struct map_desc   desc[NUM_OF_1D_PARTS];
	struct PART_INFO *part = NULL;
	unsigned int phys[NUM_OF_1D_PARTS];
	unsigned int virt[NUM_OF_1D_PARTS];
	unsigned int size[NUM_OF_1D_PARTS];
	int i = 0;

	for(i = 0; NUM_OF_1D_PARTS > i; i++) {
		part  = &__n1d_parts[i];
		phys[i] = part->phybase;
		virt[i] = part->virbase;
		size[i] = part->length;
	}

	if (NUM_OF_1D_PARTS)
		MCB1D_Initialize(phys, virt, size, NUM_OF_1D_PARTS);
	else
		printk(KERN_INFO "%s: 1D map[1]: dma zone len=%dM\n",
			VMEM_DEV_NAME, CFG_MEM_PHY_DMAZONE_SIZE/SZ_1M);

	return 0;
}

static void vmem_1dn_exit(void)
{
	if (NUM_OF_1D_PARTS)
		MCB1D_Finalize();
}

/* called as part of MACHINE_START() -> map_io */
static void __init vmem_2dn_init_map_io(void)
{
#if defined(CFG_MEM_PHY_BLOCK_BASE)
	struct map_desc   desc[NUM_OF_2D_PARTS];
	int i = 0;

	DBGOUT("%s (%d)\n", __func__, NUM_OF_2D_PARTS);

	/* virtual mapping */
	for(i = 0; NUM_OF_2D_PARTS > i; i++) {
		INIT_LIST_HEAD(&__n2d_parts_list[i]);

		desc[i].virtual = __n2d_parts[i].virbase;
		desc[i].pfn	 	= __phys_to_pfn(__n2d_parts[i].phybase);
		desc[i].length  = __n2d_parts[i].length;
		desc[i].type    = MT_MEMORY_NONCACHED;	/* MT_DEVICE, MT_MEMORY_NONCACHED */

		iotable_init(&desc[i], i+1);
		printk(KERN_INFO "%s: 2D map[%d]: p 0x%08x -> v 0x%08x len=%dM\n",
			VMEM_DEV_NAME, i, (u_int)(desc[i].pfn<<12), (u_int)(desc[i].virtual),
			(u_int)(desc[i].length/SZ_1M));

	}
#else
    struct list_head *lhead = &__n2d_parts_list[0];
    INIT_LIST_HEAD(lhead);
#endif
}

static int vmem_2dn_init(void)
{
#if defined(CFG_MEM_PHY_BLOCK_BASE)
	struct PART_INFO *part = NULL;
	struct list_head *lhead = NULL;
	int i = 0;

	DBGOUT("%s (%d)\n", __func__, NUM_OF_2D_PARTS);

	for(i = 0; NUM_OF_2D_PARTS > i; i++)	{
		uint phybase = 0, segment = 0, restlen = 0;

		part  = &__n2d_parts[i];
		lhead = &__n2d_parts_list[i];

		NX_ASSERT(!(part->phybase & (_2D_BASE_ALIGN - 1)));
		NX_ASSERT(part->length >= _2D_SIZE_MIN);
		INIT_LIST_HEAD(lhead);

		phybase = part->phybase;
		restlen = (phybase + part->length - (phybase & ~(_2D_SIZE_ALIGN - 1)));
		DBGOUT("[%d] base:0x%08x, size:0x%08x, ext:0x%08x \n",
			i, phybase, part->length, restlen);

		/* 1 segment */
		if (phybase == (phybase + (part->length & ~(_2D_SIZE_ALIGN - 1))) )
			segment = (phybase + part->length - (phybase & ~(_2D_SIZE_ALIGN - 1)));
		else
			segment = _2D_SIZE_ALIGN;

		/* create root node each segment */
		while(1) {
			MCB2DBASE * base2d = kzalloc(sizeof(MCB2DBASE), GFP_KERNEL);
			if (! base2d) {
				ERROUT("error, kmalloc for root node[%d], base2d=0x%x, height=0x%x\n",
					i, phybase, segment/_2D_STRIDE);
				return -ENOMEM;
			}

			base2d->segbase  = (phybase & ~(_2D_SIZE_ALIGN - 1));	// align 16Mbyte
			base2d->width    = _2D_STRIDE;
			base2d->height   = segment/_2D_STRIDE;
			base2d->reserved = (phybase - (phybase & ~(_2D_SIZE_ALIGN - 1)))/_2D_STRIDE;
			DBGOUT("seg base:0x%08x, size:0x%08x, w:%6d, h:%6d, r:%4d \n",
				base2d->segbase, segment, base2d->width, base2d->height, base2d->reserved);

			MCB2D_Initialize(&base2d->node2d, base2d->width, base2d->height, 0, 0, NULL);

			/* pre-allocate reserved area. */
			if (base2d->reserved)
				MCB2D_Malloc(&base2d->node2d, _2D_STRIDE, base2d->reserved, 0, 0);

			LIST_LOCK_ADD(&base2d->list, lhead);
			if (! (restlen -= segment))
				break;

			segment = (restlen & ~(_2D_SIZE_ALIGN - 1)) ? _2D_SIZE_ALIGN : restlen;
			phybase = ((phybase + _2D_SIZE_ALIGN) & ~(_2D_SIZE_ALIGN - 1));
		}
	}
#else
    struct list_head *lhead = &__n2d_parts_list[0];
    INIT_LIST_HEAD(lhead);
#endif
	return 0;
}

static void vmem_2dn_exit(void)
{
	struct list_head *lhead = NULL, *list = NULL;
	MCB2DBASE *base2d = NULL;
	int i;

	DBGOUT("%s\n", __func__);
	for( i = 0; sizeof(__n2d_parts_list)/sizeof(__n2d_parts_list[0]) > i; i++ )	{
		lhead = &__n2d_parts_list[i];
		list_for_each_prev(list, lhead) {
			base2d = list_entry(list, MCB2DBASE, list);
			if (base2d) {
				DBGOUT("[%d] seg base:0x%08x, w:%6d, h:%6d, r:%4d \n",
					i, base2d->segbase, base2d->width, base2d->height, base2d->reserved);
				MCB2D_Finalize(&base2d->node2d);
				LIST_LOCK_DEL(&base2d->list);
				kfree((void*)base2d);
			}
		}
	}
}

static int vmem_1dn_alloc(struct VM_NODE *pnode, VM_IMEMORY *pmem, int node)
{
	struct device *dev = __pdevptr;
	unsigned long size = pmem->MemWidth * pmem->MemHeight;
	MCB1DNODE * node1d = NULL;
	void	  * handle = NULL;
	unsigned int dma_max = ALLOC_DMA_MEM;

	VMEM_LOCK();

	if (NUM_OF_1D_PARTS) {
		if (dma_max >= size)
			handle = dma_alloc_writecombine(dev, size, &pmem->Address, (GFP_KERNEL | GFP_DMA));
		if (NULL == handle)
			node1d = MCB1D_Malloc(size, pmem->HorAlign);
	}

	if (node1d)
		handle = node1d;

	/* from dma zone */
	if (NULL == handle) {
		size = PAGE_ALIGN(size);
		handle = dma_alloc_writecombine(dev, size, &pmem->Address, (GFP_KERNEL | GFP_DMA));
	}

	/* retry from nomal zone */
	if (NULL == handle) {
		handle = dma_alloc_writecombine(dev, size, &pmem->Address, GFP_KERNEL);
		if (NULL == handle) {
			/*ERROUT("[%4d] Fail, allocate 1D memory (%d by %d) !!!\n",
				GET_TID(), (int)pmem->MemWidth, (int)pmem->MemHeight);

			if (NUM_OF_1D_PARTS)
				MCB1D_ShowStatus();
			show_dmazone();*/

			VMEM_UNLOCK();
			return -ENOMEM;
		}
	}

	pmem->MemPoint = pnode;
	if (node1d) {
		pmem->Address = node1d->uiPhysAddr;
		pmem->Virtual = node1d->uiVirtAddr;
	} else {
		pmem->Virtual = (uint)handle;
	}

	pnode->tid      = GET_TID();	/* set thread id */
	pnode->handle   = handle;
	pnode->physical = pmem->Address;
	pnode->virtual  = pmem->Virtual;
	pnode->width    = pmem->MemWidth;
	pnode->height   = pmem->MemHeight;
	pnode->flags    = pmem->Flags | (node1d ? 0 : ALLOC_SYS_MEM);

	VMEM_UNLOCK();
	return 0;
}

static int vmem_2dn_alloc(struct VM_NODE *pnode, VM_IMEMORY *pmem, int node)
{
#if defined(CFG_MEM_PHY_BLOCK_BASE)
	struct list_head *lhead = &__n2d_parts_list[node];
	struct list_head *list  = NULL;
	MCB2DBASE * base2d = NULL;
	MCB2DNODE * node2d = NULL;

	VMEM_LOCK();

	list_for_each_prev(list, lhead) {
		base2d = list_entry(list, MCB2DBASE, list);
		if (base2d) {
			DBGOUT("[node:%d] seg base:0x%08x, w:%6d, h:%6d, r:%4d \n",
				node, base2d->segbase, base2d->width, base2d->height, base2d->reserved);
			node2d = MCB2D_Malloc(&base2d->node2d, pmem->MemWidth, pmem->MemHeight,
								pmem->HorAlign, pmem->VerAlign );
			if (node2d)
				break;
		}
	}

	if (NULL == node2d) {
		ERROUT("[%4d] Fail, allocate 2D memory (%d by %d) !!!\n",
			GET_TID(), (int)pmem->MemWidth, (int)pmem->MemHeight);
		VMEM_UNLOCK();
		return  -ENOMEM;
	}

	pmem->MemPoint = pnode;
	pmem->Address  = (_2D_ZONE_OFFS | base2d->segbase) + (node2d->uiY<<12) + node2d->uiX;
	pmem->Virtual  = __n2d_parts[node].virbase +
					(pmem->Address - __n2d_parts[node].phybase - _2D_ZONE_OFFS);

	pnode->tid      = GET_TID();	/* set thread id */
	pnode->handle   = node2d;
	pnode->physical = pmem->Address;
	pnode->virtual  = pmem->Virtual;
	pnode->width    = pmem->MemWidth;
	pnode->height   = pmem->MemHeight;
	pnode->flags    = pmem->Flags;

	VMEM_UNLOCK();
	return 0;
#else
	struct list_head *lhead = &__n2d_parts_list[0];
	struct list_head *list  = NULL;
	MCB2DBASE * base2d = NULL;
	MCB2DNODE * node2d = NULL;
    MCB1DNODE * node1d = NULL;
    int segbase;
    int segbase_y;

	VMEM_LOCK();

	list_for_each_prev(list, lhead) {
		base2d = list_entry(list, MCB2DBASE, list);
		if (base2d) {
			DBGOUT("[node:%d] seg base:0x%08x, w:%6d, h:%6d, r:%4d \n",
				node, base2d->segbase, base2d->width, base2d->height, base2d->reserved);
			node2d = MCB2D_Malloc(&base2d->node2d, pmem->MemWidth, pmem->MemHeight,
								pmem->HorAlign, pmem->VerAlign );
			if (node2d)
				break;
		}
	}

	if (NULL == node2d) {
		base2d = kzalloc(sizeof(MCB2DBASE), GFP_KERNEL);
		if (! base2d) {
			ERROUT("error, kmalloc for root node, width=0x%x, height=0x%x\n",
				    pmem->MemWidth, pmem->MemHeight );
		    VMEM_UNLOCK();
			return -ENOMEM;
		}
		base2d->width    = _2D_STRIDE;
		base2d->height   = _2D_SIZE_MIN/_2D_STRIDE;
		base2d->reserved = 0;
        while( base2d->height < pmem->MemHeight ){ base2d->height <<= 1; }
        node1d = MCB1D_Malloc(base2d->width * base2d->height, _2D_SIZE_MIN);
        if( !node1d )
        {
			ERROUT("error, 2D heap alloc, width=0x%x, height=0x%x\n",
				    base2d->width, base2d->height );
            kfree((void*)base2d);
            VMEM_UNLOCK();
			return -ENOMEM;
        }
		base2d->segbase  = (unsigned int)node1d;
		DBGOUT("alloc new 2d heap : seg base:0x%08x, w:%6d, h:%6d, r:%4d \n",
			node1d->uiPhysAddr, base2d->width, base2d->height, base2d->reserved);

		MCB2D_Initialize(&base2d->node2d, base2d->width, base2d->height, 0, 0, NULL);

        lhead = &__n2d_parts_list[0];
		LIST_LOCK_ADD(&base2d->list, lhead);

		node2d = MCB2D_Malloc(&base2d->node2d, pmem->MemWidth, pmem->MemHeight,
							pmem->HorAlign, pmem->VerAlign );
		if (!node2d)
        {
    		ERROUT("[%4d] Fail, allocate 2D memory (%d by %d) !!!\n",
    			GET_TID(), (int)pmem->MemWidth, (int)pmem->MemHeight);
    		VMEM_UNLOCK();
    		return  -ENOMEM;
    	}
	}

	node1d = (MCB1DNODE *)base2d->segbase;

	segbase   = (node1d->uiPhysAddr & (~(_2D_SIZE_ALIGN - 1)));
	segbase_y = (node1d->uiPhysAddr - segbase)/_2D_STRIDE;

	pmem->MemPoint = pnode;
	pmem->Address  = (_2D_ZONE_OFFS | segbase) + ((segbase_y+node2d->uiY)<<12) + node2d->uiX;
	pmem->Virtual  = node1d->uiVirtAddr +
					(pmem->Address - node1d->uiPhysAddr - _2D_ZONE_OFFS);
	pnode->tid      = GET_TID();	/* set thread id */
	pnode->handle   = node2d;
	pnode->physical = pmem->Address;
	pnode->virtual  = pmem->Virtual;
	pnode->width    = pmem->MemWidth;
	pnode->height   = pmem->MemHeight;
	pnode->flags    = pmem->Flags;

	VMEM_UNLOCK();
	return 0;
#endif
}

static int vmem_1dn_free(struct VM_NODE *pnode)
{
	struct device *dev = __pdevptr;
	unsigned long size = PAGE_ALIGN(pnode->width * pnode->height);
	unsigned int use1d = (pnode->flags & ALLOC_SYS_MEM) ? 0 : 1;

	VMEM_LOCK();

	if (NUM_OF_1D_PARTS && use1d)
		MCB1D_Free((MCB1DNODE*)pnode->handle);
	else
		dma_free_coherent(dev, size, pnode->handle, pnode->physical);

	VMEM_UNLOCK();
	return 0;
}

static int vmem_2dn_free(struct VM_NODE *pnode)
{
	VMEM_LOCK();
	MCB2D_Free((MCB2DNODE*)pnode->handle);

#if !defined(CFG_MEM_PHY_BLOCK_BASE)
    {
    	struct list_head *lhead = &__n2d_parts_list[0];
    	struct list_head *list  = NULL;
    	MCB2DBASE * base2d = NULL;
        MCB1DNODE * node1d = NULL;
    	list_for_each_prev(list, lhead) {
    		base2d = list_entry(list, MCB2DBASE, list);
    		if (base2d && (!base2d->node2d.pChild1) && (!base2d->node2d.pChild2) && (!base2d->node2d.bSolid) )
    		{
    		    node1d = (MCB1DNODE *)base2d->segbase;
        		DBGOUT("free 2d heap : seg base:0x%08x, w:%6d, h:%6d, r:%4d \n",
        			node1d->uiPhysAddr, base2d->width, base2d->height, base2d->reserved);
                MCB1D_Free(node1d);
                LIST_LOCK_DEL( &base2d->list );
                kfree((void*)base2d);
                break;
    		}
    	}
    }
#endif
	VMEM_UNLOCK();

	return 0;
}

/*
 * called by MACHINE_START() -> map_io to reserve memory.
 */
void __init vmem_initialize_map_io(void)
{
	vmem_1dn_init_map_io();
	vmem_2dn_init_map_io();
}

static int vmem_initialize(void)
{
	int ret = 0;
	struct VM_LIST *vlist = &__list_drv_node;

	memset(vlist, 0, sizeof(struct VM_LIST));
	/*
	 * can't kill driver node
	 */
	vlist->flags = TASK_NODE_RESV;

	/* init driver list */
	INIT_LIST_HEAD(&vlist->head);
	INIT_LIST_HEAD(&vlist->node);

	/* add driver list to top */
	LIST_LOCK_INIT();
	LIST_LOCK_ADD (&vlist->head, &__list_top_node);

	ret = vmem_1dn_init();
	if (0 > ret)
		return ret;

	ret = vmem_2dn_init();
	if (0 > ret)
		return ret;

	return 0;
}

static void vmem_finialize(void)
{
	vmem_1dn_exit();
	vmem_2dn_exit();
}

/*---------------------------------------------------------------------------------*/
int vmem_alloc(VM_IMEMORY *pmem, int minor, void *listhead)
{
	struct list_head *lhead = (NULL == listhead ? &__list_drv_node : listhead);
	struct VM_LIST   *vlist = (struct VM_LIST*)lhead;
	struct VM_NODE   *pnode = NULL;

	unsigned int length = 0;
	int ret = 0, node = 0, type2d = 0;

	DBGOUT("%s (minor:%d, listhead:0x%08x)\n", __func__, minor, (uint)listhead);
	NX_ASSERT(pmem);
	NX_ASSERT(0 == minor);

	pmem->MemPoint = NULL;
	type2d = (pmem->Flags & VMEM_BLOCK_BUFFER ? 1 : 0);
	length = (pmem->MemWidth * pmem->MemHeight);
	node   = minor;

	/* when linear cb/cr (YUYV) */
	if (! length)
		return 0;

	pnode = kzalloc(sizeof(struct VM_NODE), GFP_KERNEL);
	if (! pnode) {
		ERROUT("Fail, kmalloc (%d) !!!\n", sizeof(struct VM_NODE));
		return -ENOMEM;
	}

	if (type2d)
		ret = vmem_2dn_alloc(pnode, pmem, node);
	else
		ret = vmem_1dn_alloc(pnode, pmem, node);

	if (0 > ret) {
		kfree(pnode);
#if defined(CONFIG_GRP_NEXELL_VMEM_KILLER)
		if (0 == vmem_killer(type2d))
			ret = vmem_alloc(pmem, minor, listhead);
#endif
		return ret;
	}

	if (type2d)
		vlist->total_2d += length;
	else
		vlist->total_1d += length;

	/* add node to list */
	LIST_LOCK_ADD(&pnode->list, lhead);

	DBGOUT("[%4d] +h:0x%08x, w:%8d, h:%6d, a:0x%08x, v:0x%08x [%s]\n",
		pnode->tid, (u_int)pnode->handle, pnode->width, pnode->height,
		pnode->physical, pnode->virtual, type2d?"2D":"1D");
	DBGOUT("done:%s (minor:%d)\n", __func__, node);
	return ret;
}

int vmem_free(VM_IMEMORY *pmem, void *listhead)
{
	struct VM_LIST *vlist = (NULL == listhead ? &__list_drv_node : listhead);
	struct VM_NODE *pnode = NULL;

	unsigned int length = 0;
	int type2d = 0;

	DBGOUT("%s (listhead:0x%08x)\n", __func__, (uint)listhead);
	NX_ASSERT(pmem)

	if (! pmem->MemPoint ||
		! pmem->MemWidth || ! pmem->MemHeight)
		return 0;

	pnode  = (struct VM_NODE *)pmem->MemPoint;
	type2d = (pnode->flags & VMEM_BLOCK_BUFFER ? 1 : 0);
	length = (pmem->MemWidth * pmem->MemHeight);

	DBGOUT("[%4d] -h:0x%08x, w:%8d, h:%6d, a:0x%08x, v:0x%08x [%s]\n",
		pnode->tid, (u_int)pnode->handle, pnode->width, pnode->height,
		pnode->physical, pnode->virtual, type2d?"2D":"1D");

	if (type2d)
		vmem_2dn_free(pnode);
	else
		vmem_1dn_free(pnode);

	if (type2d)
		vlist->total_2d -= length;
	else
		vlist->total_1d -= length;

	if (0 > vlist->total_2d) vlist->total_2d = 0;
	if (0 > vlist->total_1d) vlist->total_1d = 0;

	/* delete node from list */
	LIST_LOCK_DEL(&pnode->list);
	kfree(pnode);

	/* clear */
	pmem->Address  = 0;
	pmem->MemPoint = NULL;

	DBGOUT("%s, done\n", __func__);
	return 0;
}

unsigned int vmem_get_base(int minor, int type2d,
				unsigned int *phys, unsigned int *virt, unsigned int *size)
{
	int	node = minor;
	NX_ASSERT(0 == minor);

	if (type2d && NUM_OF_2D_PARTS) {
		if (phys) *phys = __n2d_parts[node].phybase;
		if (virt) *virt = __n2d_parts[node].virbase;
		if (size) *size = __n2d_parts[node].length;
	}

	if (NUM_OF_1D_PARTS) {
		if (phys) *phys = __n1d_parts[node].phybase;
		if (virt) *virt = __n1d_parts[node].virbase;
		if (size) *size = __n1d_parts[node].length;
	} else {
		if (phys) *phys = 0;
		if (virt) *virt = 0;
		if (size) *size = 0;
	}
	return 0;
}

EXPORT_SYMBOL_GPL(vmem_alloc);
EXPORT_SYMBOL_GPL(vmem_free);
EXPORT_SYMBOL_GPL(vmem_get_base);

/*---------------------------------------------------------------------------------*/
void vmem_reset(unsigned int type, int minor, void *listhead)
{
	struct list_head *lhead = NULL, *list = NULL;
	struct VM_LIST   *vlist = NULL;
	struct VM_NODE	 *pnode = NULL;
	int type2d = 0;

	NX_ASSERT(0 == minor);
	LIST_LOCK();

	list_for_each(lhead, &__list_top_node) {
		vlist = list_entry(lhead, struct VM_LIST, head);
		list_for_each(list, &vlist->node) {
			pnode  = list_entry(list, struct VM_NODE, list);
			type2d = (pnode->flags & VMEM_BLOCK_BUFFER ? 1 : 0);
			DBGOUT("[%4d] -h:0x%08x, w:%8d, h:%6d [%s]\n",
				pnode->tid, (u_int)pnode->handle, pnode->width, pnode->height, type2d?"2D":"1D");

			if (type2d)
				vmem_2dn_free(pnode);
			else
				vmem_1dn_free(pnode);

			list = list->prev;
			LIST_DEL(&pnode->list);
			kfree(pnode);
		}
		LIST_DEL(&vlist->head);
		kfree(vlist);
	}

	LIST_UNLOCK();
}

/*--------------------------------------------------------------------------------
 * file_operations
 ---------------------------------------------------------------------------------*/
static int nx_vmem_open(struct inode *inode, struct file *filp)
{
	int minor = MINOR(inode->i_rdev);
	struct VM_LIST *vlist = NULL;

	DBGOUT("%s (minor:%d, parts:%d)\n", __func__, minor, NUM_OF_2D_PARTS);
	if (minor) {
		ERROUT("open, /dev/vmem'%d' not support ...\n", minor);
		return -EINVAL;
	}

	vlist = kzalloc(sizeof(struct VM_LIST), GFP_KERNEL);
	if (NULL == vlist)
		return -ENOMEM;

	/* set process id */
	vlist->pid = GET_PID();

#if defined(CONFIG_GRP_NEXELL_VMEM_KILLER)
	{
		struct vmem_device *vdev = &__vmemdev;
		char **name = (char**)vdev->plat->resv_task_name;
		int i = 0, r = 0, num = vdev->plat->resv_task_num;
		for ( ; num > i && !r; i++, name++) {
			char *rc = find_task_by_vpid(GET_PID())->comm;
			int l1 = strlen(rc), l2 = strlen(*name);
			for ( ; l1 > 0; l1--, rc++) {
				rc = strchr(rc, *name[0]);
				if (NULL == rc) break;
				if (! strncmp(rc, *name, l2)) {
					r = vlist->flags = TASK_NODE_RESV;
					break;
				}
			}
		}
		DBGOUT("%s: task = %s, resv [%s] \n", __func__,
			find_task_by_vpid(GET_PID())->comm, vlist->flags&TASK_NODE_RESV?"O":"X");
	}
#endif

	INIT_LIST_HEAD(&vlist->head);
	INIT_LIST_HEAD(&vlist->node);
	LIST_LOCK_ADD (&vlist->head, &__list_top_node);

	filp->private_data = (void*)vlist;
	DBGOUT("%s done (private_data:0x%p)\n", __func__, filp->private_data);

	/*
	printk("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printk("%s [%4d] private=0x%08x, R:%s %s\n",
		__func__, GET_PID(), (uint)filp->private_data,
		vlist->flags&TASK_NODE_RESV?"O":"X", find_task_by_vpid(GET_PID())->comm);
	printk("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	*/
	return 0;
}

static int nx_vmem_release(struct inode *inode, struct file *filp)
{
	struct list_head *list  = NULL;
	struct VM_LIST   *vlist = NULL;
	struct VM_NODE   *pnode = NULL;
	int type2d = 0;

	DBGOUT("%s (minor:%d)\n", __func__, MINOR(inode->i_rdev));
	NX_ASSERT(0 == MINOR(inode->i_rdev));

	vlist = (struct VM_LIST*)filp->private_data;
	if (! vlist)
		return -EBADF;	/* Bad file number */

	list_for_each(list, &vlist->node) {
		pnode  = list_entry(list, struct VM_NODE, list);
		type2d = (pnode->flags & VMEM_BLOCK_BUFFER ? 1 : 0);

		DBGOUT("[%4d] -h:0x%08x, w:%8d, h:%6d [%s]\n",
			pnode->tid, (u_int)pnode->handle, pnode->width, pnode->height, type2d?"2D":"1D");

		if (type2d)
			vmem_2dn_free(pnode);
		else
			vmem_1dn_free(pnode);

		list = list->prev;
		LIST_LOCK_DEL(&pnode->list);
		kfree(pnode);
	}

	if (TASK_NODE_KILL & vlist->flags)
		__task_kill_on = 0;

	LIST_LOCK_DEL(&vlist->head);
	kfree(vlist);

	/*
	printk("---------------------------------------------------------\n");
	printk("%s [%4d] private=0x%08x, R:%s %s\n",
		__func__, GET_PID(), (uint)filp->private_data,
		vlist->flags&TASK_NODE_RESV?"O":"X", find_task_by_vpid(GET_PID())->comm);
	printk("---------------------------------------------------------\n");
	*/
	return 0;
}

static long nx_vmem_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{
	int   ret   = 0;
	int   minor = (int)MINOR(filp->f_path.dentry->d_inode->i_rdev);
	void *lhead = filp->private_data;

	DBGOUT("%s (minor:%d, ioctl:%d)\n", __func__, minor, _IOC_NR(cmd));
	NX_ASSERT(0 == minor);

	switch(cmd)	{
		case IOCTL_VMEM_STATUS:
			vmem_show_attr(NULL, NULL, NULL);
			break;

		case IOCTL_VMEM_RESET:
			{
				unsigned int type;
				if (copy_from_user((void*)&type, (const void*)arg, sizeof(unsigned int)))
					break;
				vmem_reset(type, minor, lhead);
			}
			break;

		case IOCTL_VMEM_INFO:
			{
				VM_MEMBASE base;
				if (copy_from_user((void*)&base, (const void*)arg, sizeof(VM_MEMBASE)))
					break;

				base.LinPhyBase = 0,
				base.LinPhySize = 0;
				base.LinVirBase = 0;
				base.LinVirSize = 0;

				base.BlkPhyBase = 0;
				base.BlkPhySize = 0;
				base.BlkVirBase = 0;
				base.BlkVirSize = 0;

				if (NUM_OF_1D_PARTS > minor) {
					base.LinPhyBase = __n1d_parts[minor].phybase;
					base.LinPhySize = __n1d_parts[minor].length;
					base.LinVirBase = __n1d_parts[minor].virbase;
					base.LinVirSize = __n1d_parts[minor].length;
				}

#if defined(CFG_MEM_PHY_BLOCK_BASE)
				if (NUM_OF_2D_PARTS > minor) {
					base.BlkPhyBase = __n2d_parts[minor].phybase;
					base.BlkPhySize = __n2d_parts[minor].length;
					base.BlkVirBase = __n2d_parts[minor].virbase;
					base.BlkVirSize = __n2d_parts[minor].length;
				}
#else
				if (NUM_OF_1D_PARTS > minor) {
					base.BlkPhyBase = __n1d_parts[minor].phybase;
					base.BlkPhySize = __n1d_parts[minor].length;
					base.BlkVirBase = __n1d_parts[minor].virbase;
					base.BlkVirSize = __n1d_parts[minor].length;
				}
#endif
				if (copy_to_user((void*)arg, (const void*)&base, sizeof(VM_MEMBASE)))
					break;
			}
			break;

		case IOCTL_VMEM_ALLOC:
			{
				VM_IMEMORY imem = { 0, };
				if (copy_from_user((void*)&imem, (const void*)arg, sizeof(VM_IMEMORY)))
					break;

				ret = vmem_alloc(&imem, minor, lhead);

				if (ERR_VMEM_NO == ret) {
					if (copy_to_user((void*)arg, (const void*)&imem, sizeof(VM_IMEMORY))) {
						vmem_free(&imem, lhead);
 						ret = ERR_VMEM_INVALID;
						break;
					}
				}
			}
			break;

		case IOCTL_VMEM_FREE:
			{
				VM_IMEMORY imem;
				if (copy_from_user((void*)&imem, (const void*)arg, sizeof(VM_IMEMORY)))
					break;
				vmem_free((VM_IMEMORY*)&imem, lhead);
			}
			break;

		case IOCTL_VMEM_VALLOC:
			{
				VM_VMEMORY vmem;
				if (copy_from_user((void*)&vmem, (const void*)arg, sizeof(VM_VMEMORY)))
 					break;

				/* allocate Lu */
				ret = vmem_alloc(&vmem.LuMemory, minor, lhead);
				if (0 > ret)
 					break;

				/* allocate Cb */
				ret  = vmem_alloc(&vmem.CbMemory, minor, lhead);
				if (0 > ret) {
 					vmem_free(&vmem.LuMemory, lhead);
 					break;
 				}

 				/* allocate Cr */
				ret  = vmem_alloc(&vmem.CrMemory, minor, lhead);
				if (0 > ret) {
 					vmem_free(&vmem.LuMemory, lhead);
 					vmem_free(&vmem.CbMemory, lhead);
 					break;
 				}

				if (copy_to_user((void*)arg, (const void*)&vmem, sizeof(VM_VMEMORY))) {
					vmem_free(&vmem.LuMemory, lhead);
 					vmem_free(&vmem.CbMemory, lhead);
 					vmem_free(&vmem.CrMemory, lhead);
 					ret = -EINVAL;
					break;
				}
			}
			break;

		case IOCTL_VMEM_VFREE:
			{
				VM_VMEMORY  vmem;
				if (copy_from_user((void*)&vmem, (const void*)arg, sizeof(VM_VMEMORY)))
					break;

				vmem_free(&vmem.LuMemory, lhead);
				vmem_free(&vmem.CbMemory, lhead);
				vmem_free(&vmem.CrMemory, lhead);
			}
			break;

		default:
			ERROUT("Fail, unknown ioctl ...\n");
			ret = -EINVAL;
	}

	DBGOUT("IoCtl (cmd:0x%x, nr:%d, ret:%d) \n\n", cmd, _IOC_NR(cmd), ret);
	return ret;
}

void nx_vmem_vma_open(struct vm_area_struct *vma)
{
}

void nx_vmem_vma_close(struct vm_area_struct *vma)
{
}

static struct vm_operations_struct nx_vmem_vm_ops = {
	.open = nx_vmem_vma_open,
	.close = nx_vmem_vma_close,
};

static int nx_vmem_mmap(struct file *filp, struct vm_area_struct *vma)
{
	if(remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, vma->vm_end - vma->vm_start, vma->vm_page_prot))
		return -EAGAIN;

	vma->vm_ops = &nx_vmem_vm_ops;
	nx_vmem_vma_open(vma);
	return 0;
}

struct file_operations nx_vmem_fops = {
	.owner 			= THIS_MODULE,
	.open 			= nx_vmem_open,
	.release		= nx_vmem_release,
	.mmap			= nx_vmem_mmap,
	.unlocked_ioctl	= nx_vmem_ioctl,
};

/*--------------------------------------------------------------------------------
 * Memory allocator module functions.
 *
 * attribute
 * 	#>cat /sys/devices/platform/vmem.0/vmem
 ---------------------------------------------------------------------------------*/
static DEVICE_ATTR(vmem, 0666, vmem_show_attr, NULL);
static struct attribute *vmem_attrs[] = {
	&dev_attr_vmem.attr,
	NULL
};

static struct attribute_group vmem_attr_group = {
	.attrs = vmem_attrs,
};

struct class *vm_class;

static int nx_vmem_probe(struct platform_device *pdev)
{
	int ret = vmem_initialize();
	if (0 > ret)	{
		ERROUT("Error, memory node init ...\n");
		return ret;
	}

	ret = register_chrdev(VMEM_DEV_MAJOR, "Video Memory Allocator", &nx_vmem_fops);
	if (0 > ret)	{
		ERROUT("Fail, register device (%s, major:%d)\n",
			VMEM_DEV_NAME, VMEM_DEV_MAJOR);
		return ret;
	}

	ret = sysfs_create_group(&pdev->dev.kobj, &vmem_attr_group);
	if (ret) {
		printk(KERN_ERR "Fail, %s create sysfs group ...\n", pdev->name);
		return ret;
	}

	/* for hotplug */
	vm_class = class_create(THIS_MODULE, "vmem");
	device_create(vm_class, NULL, MKDEV(VMEM_DEV_MAJOR, 0), NULL, "vmem");

	/* set device info */
	__pdevptr = &pdev->dev;
	__pdevptr->dma_mask = &__dmamask;
	__pdevptr->coherent_dma_mask = 0xffffffff;
	__vmemdev.plat = pdev->dev.platform_data;

#if defined(CONFIG_GRP_NEXELL_VMEM_KILLER)
	{
		char **name = (char**)__vmemdev.plat->resv_task_name;
		int i = 0, num = __vmemdev.plat->resv_task_num;
		for (;  num > i; i++, name++) {
			printk(KERN_ERR "%s: killer reserve task '%s' \n", VMEM_DEV_NAME, *name);
		}
	}
#endif

	DBGOUT("DONE : %s\n", __func__);
	return ret; /* success */
}

static int nx_vmem_remove(struct platform_device *pdev)
{
	vmem_finialize();

	sysfs_remove_group(&pdev->dev.kobj, &vmem_attr_group);
	device_destroy(vm_class, MKDEV(VMEM_DEV_MAJOR, 0));
	class_destroy (vm_class);

	unregister_chrdev(VMEM_DEV_MAJOR, "Video Memory Allocator");
	return 0;
}

static struct platform_driver vmem_plat_driver = {
	.probe		= nx_vmem_probe,
	.remove		= nx_vmem_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= VMEM_DEV_NAME,
	},
};

static int __init nx_vmem_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_register(&vmem_plat_driver);
}

static void __exit nx_vmem_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&vmem_plat_driver);
}

module_init(nx_vmem_init);
module_exit(nx_vmem_exit);

MODULE_AUTHOR("jhkim <jhkin@nexell.co.kr>");
MODULE_DESCRIPTION("Video Memory Allocator driver for the Nexell");
MODULE_LICENSE("GPL");
