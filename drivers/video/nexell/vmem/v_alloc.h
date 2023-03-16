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
#include <linux/slab.h>

#include <mach/platform.h>

#if (1)
#define	KNL_BREAK		NX_BREAK
#define KNL_ASSERT		NX_ASSERT
#else
#define KNL_ASSERT(expr)
#define KNL_BREAK
#endif

#define	KNL_Print(msg...)	{ printk(KERN_INFO msg); }

//------------------------------------------------------------------------------
#define KNL_MALLOC(size)	kzalloc(size, GFP_KERNEL)
#define KNL_FREE(ptr)		kfree(ptr)
#define KNL_BOOL           	int
#define KNL_NULL			0
#define KNL_TRUE			1
#define KNL_FALSE			0

//--------------------------------------------------------------------------
//
//	1D memory manager
//
//	exports
//		- MCB2DNODE
//		- MCB2D_Initialize
//		- MCB2D_Finalize
//		- MCB2D_Malloc
//		- MCB2D_Free
//		- MCB2D_ShowNodes
//		- MCB2D_AvailSpace
//
//------------------------------------------------------------------------------
#define MCBFLAG_USED		(1)

typedef struct __MCB1DNODE {
	unsigned int uiPhysAddr;		// physical address
	unsigned int uiVirtAddr;		// physical address
	unsigned int uiSize;		// 실제 할당된 aligned size
	int          Flags;

	struct __MCB1DNODE * pPrev;
	struct __MCB1DNODE * pNext;
	struct __MCB1DHEAP * pHeap;
} MCB1DNODE;

typedef struct __MCB1DHEAP {
	unsigned int uiHeap;
	unsigned int uiPhysBase;		// physical address
	unsigned int uiVirtBase;		// Virtual  address
	unsigned int uiPoolSize;		//
	unsigned int uiRemainSize;		// 정해진 메모리 할당 사이즈 중 할당되지 않고 남아있는 양

	MCB1DNODE 	   	   * pNode;
	struct __MCB1DHEAP * pNext;
} MCB1DHEAP;

extern void 		MCB1D_Initialize  	(unsigned int *physical, unsigned int *virtual, unsigned int *length, int arraysize);
extern void 		MCB1D_Finalize		(void);
extern MCB1DNODE *	MCB1D_Malloc		(int size, int align);
extern void			MCB1D_Free  		(MCB1DNODE *pNode);
extern void 		MCB1D_ShowStatus	(void);

//--------------------------------------------------------------------------
//
//	2D memory manager
//
//	exports
//		- MCB2DNODE
//		- MCB2D_Initialize
//		- MCB2D_Finalize
//		- MCB2D_Malloc
//		- MCB2D_Free
//		- MCB2D_ShowNodes
//		- MCB2D_AvailSpace
//
//------------------------------------------------------------------------------
typedef struct __MCB2DNODE {
	unsigned int 	 	 uiX;
	unsigned int	 	 uiY;
	unsigned int	  	 uiWidth;
	unsigned int	 	 uiHeight;

	unsigned int	 	 bSolid;
	struct __MCB2DNODE * pParent;
	struct __MCB2DNODE * pChild1;
	struct __MCB2DNODE * pChild2;
} MCB2DNODE;

extern void			MCB2D_Initialize(MCB2DNODE * pMCB2D, unsigned int width, unsigned int  height,
							unsigned int x, unsigned int y,	MCB2DNODE * pParent);
extern void 		MCB2D_Finalize  (MCB2DNODE * pMCB2D);
extern MCB2DNODE * 	MCB2D_Malloc	(MCB2DNODE * pMCB2D, unsigned int width, unsigned int height, unsigned int align_x, unsigned int align_y);
extern void			MCB2D_Free		(MCB2DNODE * pMCB2D);
extern void 		MCB2D_ShowNodes (MCB2DNODE * pMCB2D, int (*pLogFunc)(const char *, ...), int Depth);
//unsigned int		MCB2D_AvailSpace(MCB2DNODE * pMCB2D);