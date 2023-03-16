//------------------------------------------------------------------------------
//
//	Copyright (C) 2008 MagicEyes Digital Co., Ltd All Rights Reserved
//	MagicEyes Digital Co. Proprietary & Confidential
//
//	MAGICEYES INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
//	FITNESS FOR A PARTICULAR PURPOSE.
//
//	Module     :
//	File       : vr313_memory1d.c
//	Description:
//	Author     : Billy(billy@mesdigital.com)
//	Export     :
//	History    :
//		2011/05/27 Modified by Gamza
//		2009/06/23 Modified a member variable, uiRemainSize, to exactly get a value.
//		2008/10/30 Modified a member variable, uiSize, in a Mem1D structure when we allocated a memory1d from
//				manager1D.
//		2008/10/13 The first distribution for the record.
//------------------------------------------------------------------------------

// #define	USER_TEST

#if	defined(USER_TEST)
#include <stdio.h>
#include <stdlib.h>

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

#define KNL_MALLOC(size)	malloc(size)
#define KNL_FREE(ptr)		free(ptr)
#define KNL_BOOL           	int
#define KNL_NULL			0
#define KNL_TRUE			1
#define KNL_FALSE			0

#define KNL_ASSERT(expr)	if (!(expr)) { printf("ASSERT, %s:%d\n", __FUNCTION__, __LINE__); while (1); }
#define KNL_BREAK
#define	KNL_Print			printf
#else

#include <linux/kernel.h>
#include <linux/slab.h>
#include "v_alloc.h"

#endif

// Make more fragmentations but make largest free block larger.
// #define MAKE_SIZE_GROUP

typedef struct __RESV_MEMORY {
	unsigned int uiPhysBase;
	unsigned int uiVirtBase;
	unsigned int uiLength;
	unsigned int uiFlags;
} RESV_MEMORY;

static RESV_MEMORY  __HeapData[4];
static int 			__HeapCount = 0;
static MCB1DHEAP * 	__pRootHeap = NULL;

/////////////////////////////////////////////////////////
//	Internal Linkage
//		- __HeapBaseAlloc
//		- __HeapBaseFree
//		- __HeapPhysical
//		- __HeapVirtual
//		- __HeapPhysical
//		- __HeapLength
//		- __HeapLinkNode
//		- __HeapMergeLinkNode
//		- __HeapDivideLinkNode
//		- __HeapMalloc
//		- __HeapFree
//		- __ManagerMalloc
//		- __ManagerFree
/////////////////////////////////////////////////////////

static inline unsigned int __HeapBaseAlloc(void)
{
	int i = 0;
	for (; __HeapCount > i; i++) {
		if (0 == __HeapData[i].uiFlags) {
			__HeapData[i].uiFlags = 1;
			return i+1;
		}
	}
	return 0;
}

static inline void __HeapBaseFree(unsigned int Heap)
{
	KNL_ASSERT(__HeapCount > (Heap-1));
	__HeapData[(Heap-1)].uiFlags = 0;
}

static inline unsigned int __HeapPhysical(unsigned int Heap)
{
	KNL_ASSERT(__HeapCount > (Heap-1));
	return __HeapData[(Heap-1)].uiPhysBase;
}

static inline unsigned int __HeapVirtual(unsigned int Heap)
{
	KNL_ASSERT(__HeapCount > (Heap-1));
	return __HeapData[(Heap-1)].uiVirtBase;
}

static inline unsigned int __HeapLength(unsigned int Heap)
{
	KNL_ASSERT(__HeapCount > (Heap-1));
	return __HeapData[(Heap-1)].uiLength;
}

static inline void __HeapLinkNode(MCB1DNODE *pPrev, MCB1DNODE *pNext)
{
	if (pPrev) { pPrev->pNext = pNext; }
	if (pNext) { pNext->pPrev = pPrev; }
}

static MCB1DNODE * __HeapMergeLinkNode(MCB1DHEAP *pHeap, MCB1DNODE *pPrev, MCB1DNODE *pNext)
{
	KNL_ASSERT(pPrev->uiPhysAddr + pPrev->uiSize == pNext->uiPhysAddr);
	KNL_ASSERT(pPrev->uiVirtAddr + pPrev->uiSize == pNext->uiVirtAddr);

	pPrev->uiSize += pNext->uiSize;

	// link node
	__HeapLinkNode(pPrev, pNext->pNext);

	// remove node
	if (pNext)
		KNL_FREE(pNext);

	// return new node
	return pPrev;
}

static MCB1DNODE * __HeapDivideLinkNode(MCB1DHEAP *pHeap, MCB1DNODE *pNode, unsigned int uiSize)
{
	MCB1DNODE * pNextNode;
	pNextNode = (MCB1DNODE *)KNL_MALLOC(sizeof(MCB1DNODE));

	KNL_ASSERT(pNode->uiSize > uiSize);
	KNL_ASSERT(pNextNode);

	*pNextNode = *pNode;

	pNextNode->uiPhysAddr = pNode->uiPhysAddr + uiSize;
	pNextNode->uiVirtAddr = pNode->uiVirtAddr + uiSize;
	pNextNode->uiSize	  = pNode->uiSize - uiSize;

	pNode->uiSize = uiSize;

	// link node
	__HeapLinkNode(pNextNode, pNode->pNext);
	__HeapLinkNode(pNode, pNextNode);

	return pNode;
}

static KNL_BOOL __HeapMalloc(MCB1DHEAP *pHeap, int Align, int nByteSize, MCB1DNODE **ppNode)
{
	if (0 == pHeap->uiHeap) {
		MCB1DNODE *pNode  = NULL;
		unsigned int Heap = __HeapBaseAlloc();

		/* No partition, allocate failed */
		if (0 == Heap) {
			pHeap->uiHeap	    = 0;
			pHeap->uiPhysBase 	= 0;
			pHeap->uiVirtBase 	= 0;
			pHeap->uiPoolSize   = 0;
			pHeap->uiRemainSize = 0;
			pHeap->pNode		= NULL;
			return KNL_FALSE;
		}
		pHeap->uiHeap	    = Heap;
		pHeap->uiPhysBase 	= __HeapPhysical(Heap);
		pHeap->uiVirtBase 	= __HeapVirtual (Heap);
		pHeap->uiPoolSize   = __HeapLength  (Heap);
		pHeap->uiRemainSize = pHeap->uiPoolSize;

		pNode = (MCB1DNODE *)KNL_MALLOC(sizeof(MCB1DNODE));
		KNL_ASSERT(pNode);	// No system memory

		pNode->uiPhysAddr = pHeap->uiPhysBase;
		pNode->uiVirtAddr = pHeap->uiVirtBase;
		pNode->uiSize 	  = pHeap->uiPoolSize;
		pNode->Flags	  = 0;
		pNode->pHeap      = pHeap;
		pNode->pNext	  = NULL;
		pNode->pPrev	  = NULL;

		pHeap->pNode = pNode;
	}

	/*
	 *	Allocate memory
	 */
	if ((int)pHeap->uiRemainSize >= nByteSize)	{
		// 일단 임시구현!
		int nMaxDiff = pHeap->uiPoolSize;
		int nCurDiff0;
		int nCurDiff1;
		MCB1DNODE * pBestFit = NULL;
		MCB1DNODE * pNode = pHeap->pNode;

		while (pNode) {
			if (!(pNode->Flags & MCBFLAG_USED) && (int)pNode->uiSize >= nByteSize) {
#ifdef MAKE_SIZE_GROUP
				unsigned int allocbase = (pNode->uiPhysAddr + pNode->uiSize - nByteSize) & (~( Align - 1 ));
#else
				unsigned int allocbase = (pNode->uiPhysAddr + Align	- 1) & (~( Align - 1 ));
#endif
				nCurDiff0 = (allocbase - pNode->uiPhysAddr);
				nCurDiff1 = (pNode->uiSize) - (nCurDiff0 + nByteSize);

				if ((nCurDiff0 >= 0) && (nCurDiff1 >= 0) && ((nCurDiff0 + nCurDiff1) < nMaxDiff)) {
					nMaxDiff = nCurDiff0 + nCurDiff1;
					pBestFit = pNode;
					if (0 == nMaxDiff)
						break;
				}
			}
			pNode = pNode->pNext;
		}

		if(pBestFit) {
#ifdef MAKE_SIZE_GROUP
			unsigned int allocbase = (pBestFit->uiPhysAddr + pBestFit->uiSize - nByteSize) & (~( Align - 1 ));
#else
			unsigned int allocbase = (pBestFit->uiPhysAddr + Align - 1) & (~( Align - 1 ));
#endif
			nCurDiff0 = (allocbase-pBestFit->uiPhysAddr);
			nCurDiff1 = (pBestFit->uiSize) - (nCurDiff0 + nByteSize);
			if (nCurDiff0) {
				pBestFit = __HeapDivideLinkNode(pHeap, pBestFit, nCurDiff0 );
				if(! pBestFit)
					return KNL_FALSE;
				pBestFit = pBestFit->pNext;
			}

			if (nCurDiff1)
				pBestFit = __HeapDivideLinkNode(pHeap, pBestFit, nByteSize );

			pBestFit->Flags |= MCBFLAG_USED;
			pHeap->uiRemainSize -= nByteSize;
		}

		*ppNode = pBestFit;
		return (KNL_BOOL)(*ppNode != NULL);
	}

	return KNL_FALSE;
}

static void __HeapFree(MCB1DHEAP *pHeap, MCB1DNODE **ppNode)
{
	MCB1DNODE * pPrev = (*ppNode)->pPrev;
	MCB1DNODE * pNext = (*ppNode)->pNext;

	if ((*ppNode)->Flags)
		pHeap->uiRemainSize += (*ppNode)->uiSize;

	if (pPrev) {
		if (!(pPrev->Flags & MCBFLAG_USED))
			*ppNode = __HeapMergeLinkNode(pHeap, pPrev, *ppNode);
	}

	if (pNext) {
		if (!(pNext->Flags & MCBFLAG_USED))
			*ppNode = __HeapMergeLinkNode(pHeap, *ppNode, pNext);
	}

	(*ppNode)->Flags &= ~MCBFLAG_USED;

	if (((*ppNode)->uiSize == pHeap->uiPoolSize) || (pHeap->uiRemainSize == pHeap->uiPoolSize)) {
		__HeapBaseFree(pHeap->uiHeap);

		pHeap->uiHeap       = 0;
		pHeap->uiPhysBase	= 0;
		pHeap->uiVirtBase	= 0;
		pHeap->uiPoolSize   = 0;
		pHeap->uiRemainSize = 0;

		if(pHeap->pNode) {
			KNL_FREE(pHeap->pNode);
			pHeap->pNode = NULL;
		}
	}
}

static MCB1DNODE * __ManagerMalloc(int nByteSize, int Align, MCB1DHEAP *pRootHeap)
{
	MCB1DHEAP * pHeap = pRootHeap;

#ifdef MAKE_SIZE_GROUP
	if (nByteSize > Align) {
		for (Align=1; nByteSize > (1<<Align); Align++){ ; }
		Align = (1<<Align);
	}
#endif

	while (pHeap) {

		MCB1DNODE * pMCB1D = NULL;
		if (__HeapMalloc(pHeap, Align, nByteSize, &pMCB1D))
			return pMCB1D;

		/* Next container */
		if (pHeap->uiHeap && ! pHeap->pNext) {
			MCB1DHEAP * pNewHeap = (MCB1DHEAP *)KNL_MALLOC(sizeof(MCB1DHEAP));
			KNL_ASSERT(pNewHeap);

			pNewHeap->uiHeap       = 0;
			pNewHeap->uiPhysBase   = 0;
			pNewHeap->uiVirtBase   = 0;
			pNewHeap->uiRemainSize = 0;
			pNewHeap->pNext		   = NULL;
			pNewHeap->pNode		   = NULL;

			pHeap->pNext = pNewHeap;
		}
		pHeap = pHeap->pNext;
	}
	return NULL;
}

static void __ManagerFree(MCB1DNODE * pMCB1D)
{
	MCB1DHEAP* pHeap;

	KNL_ASSERT(pMCB1D);
	KNL_ASSERT(pMCB1D->pHeap);
	KNL_ASSERT(pMCB1D->Flags & MCBFLAG_USED);

	pHeap = pMCB1D->pHeap;
	__HeapFree(pHeap, &pMCB1D);

	//if (pHeap->uiRemainSize == pHeap->uiPoolSize) {
	//	__HeapBaseFree(pHeap->uiHeap);
	//	pHeap->uiHeap	    = 0;
	//	pHeap->uiPoolSize   = 0;
	//	pHeap->uiRemainSize = 0;
	//	pHeap->uiPhysBase	    = 0;
	//}
}

/////////////////////////////////////////////////////////
//	External Linkage
//		- MCB1D_Initialize
//		- MCB1D_Finalize
//		- MCB1D_ShowStatus
//		- MCB1D_Malloc
//		- MCB1D_Free
/////////////////////////////////////////////////////////
void MCB1D_Initialize(unsigned int *phyical, unsigned int *virtual, unsigned int *length, int arraysize)
{
	MCB1DHEAP * pHeap = (MCB1DHEAP *)KNL_MALLOC(sizeof(MCB1DHEAP));
	int i = 0;

	KNL_ASSERT(pHeap);

	for (i = 0; arraysize > i; i++) {
		__HeapData[i].uiPhysBase = *phyical++;
		__HeapData[i].uiVirtBase = *virtual++;
		__HeapData[i].uiLength   = *length++;
		__HeapData[i].uiFlags    = 0;
	}
	__HeapCount = arraysize;

	pHeap->uiHeap	    = 0;
	pHeap->uiPhysBase 	= 0;
	pHeap->uiRemainSize = 0;
	pHeap->pNext	    = NULL;
	pHeap->pNode 	 	= NULL;

	__pRootHeap = pHeap;
}

void MCB1D_Finalize(void)
{
	MCB1DHEAP * pHeap = __pRootHeap;
	int i = 0;

	while (pHeap) {
		MCB1DHEAP * pDelHeap = pHeap;
		pHeap = pHeap->pNext;
		KNL_FREE(pDelHeap);
	}
	__pRootHeap = NULL;

	for (i = 0; __HeapCount > i; i++) {
		__HeapData[i].uiPhysBase = 0;
		__HeapData[i].uiVirtBase = 0;
		__HeapData[i].uiLength   = 0;
		__HeapData[i].uiFlags    = 0;
	}
	__HeapCount = 0;
}

extern void MCB1D_ShowStatus(void)
{
	int c_size = 0, c_count = 0, c_maxsize = 0;
	int a_size = 0, a_count = 0, a_maxsize = 0;
	int f_size = 0, f_count = 0, f_maxsize = 0;

	MCB1DHEAP *pHeap = __pRootHeap;

	while (pHeap) {
		MCB1DNODE * pNode = pHeap->pNode;
		while (pNode)
		{
			if (pNode->Flags & MCBFLAG_USED) {
				a_size    += pNode->uiSize;
				a_count   ++;
				if( pNode->uiSize > a_maxsize ){ a_maxsize = pNode->uiSize; }
			} else {
				f_size    += pNode->uiSize;
				f_count   ++;
				if (pNode->uiSize > f_maxsize)
					f_maxsize = pNode->uiSize;
			}
			pNode = pNode->pNext;
		}
		c_size += pHeap->uiPoolSize;
		c_count++;

		if (c_maxsize < pHeap->uiPoolSize)
			c_maxsize = pHeap->uiPoolSize;

		pHeap = pHeap->pNext;
	}
	KNL_Print( "1D status\n" );
	KNL_Print( " total = %9d, heap  = %4d, max = %9d \n", c_size, c_count, c_maxsize);
	KNL_Print( " alloc = %9d, count = %4d, max = %9d \n", a_size, a_count, a_maxsize);
	KNL_Print( " free  = %9d, count = %4d, max = %9d \n", f_size, f_count, f_maxsize);
}

MCB1DNODE * MCB1D_Malloc(int size, int align)
{
	KNL_ASSERT(__pRootHeap);
	KNL_ASSERT(0 == (align & (align-1)));

	return __ManagerMalloc(size, align, __pRootHeap);
}

void		MCB1D_Free(MCB1DNODE *pNode)
{
	KNL_ASSERT(__pRootHeap);
	KNL_ASSERT(pNode);

	__ManagerFree(pNode);
}

#if defined(USER_TEST)
//------------------------------------------------------------------------------
//	configurations
//------------------------------------------------------------------------------
#define USE_RESERVED_MEMORY_BASE0    (0x80000000)
#define USE_RESERVED_MEMORY_SIZE0    (0x01000000)
#define USE_RESERVED_MEMORY_BASE1    (0x60000000)
#define USE_RESERVED_MEMORY_SIZE1    (0x02000000)
//#define USE_RESERVED_MEMORY_PBASE2 0x40000000
//#define USE_RESERVED_MEMORY_SIZE2  (4*0x100000)

//------------------------------------------------------------------------------
//	primitive level allocator ( external memory allocator or reserved memory space )
//------------------------------------------------------------------------------
#define USE_RESERVED_MEMORY
static unsigned int RESERVED_BASE_LIST[] =
{
#ifdef USE_RESERVED_MEMORY_BASE0
	USE_RESERVED_MEMORY_BASE0,
#endif
#ifdef USE_RESERVED_MEMORY_BASE1
	USE_RESERVED_MEMORY_BASE1,
#endif
#ifdef USE_RESERVED_MEMORY_BASE2
	USE_RESERVED_MEMORY_BASE2,
#endif
};

static unsigned int RESERVED_SIZE_LIST[] =
{
#ifdef USE_RESERVED_MEMORY_BASE0
	USE_RESERVED_MEMORY_SIZE0,
#endif
#ifdef USE_RESERVED_MEMORY_BASE1
	USE_RESERVED_MEMORY_SIZE1,
#endif
#ifdef USE_RESERVED_MEMORY_BASE2
	USE_RESERVED_MEMORY_SIZE2,
#endif
};

static unsigned int RESERVED_HEAP_USED[] =
{
#ifdef USE_RESERVED_MEMORY_BASE0
	0,
#endif
#ifdef USE_RESERVED_MEMORY_BASE1
	0,
#endif
#ifdef USE_RESERVED_MEMORY_BASE2
	0,
#endif
};

//------------------------------------------------------------------------------
typedef struct {
	void	  	 * hMCB1D;
	unsigned int   uiPhysBase;
	unsigned int   uiSize;
} MEMORY1D;

#define TEST_COUNT  100
#define TEST(exp)	if(! (exp)){ printf( "*E: %d : assert(%s)\n", __LINE__, #exp ); while(1); }

void main(void)
{
	int i;
	MEMORY1D  test [TEST_COUNT] = { {NULL, }, };
	int       size [TEST_COUNT];
	int       align[TEST_COUNT];
	MCB1DNODE	 *pnode = NULL;

	MCB1D_Initialize(RESERVED_BASE_LIST, RESERVED_BASE_LIST, RESERVED_SIZE_LIST,
					sizeof(RESERVED_HEAP_USED)/sizeof(RESERVED_HEAP_USED[0]));

	//-------------------------------------------------------------
	printf( "[TEST 1] random allocation under initial state\n" );
	for( i=0; i<TEST_COUNT; i++ )
	{
		MEMORY1D* pmem = test+i;
		size [i] = rand()%0x100000;
		align[i] = (1<<(rand()%7));
		pnode = MCB1D_Malloc(size[i], align[i]);
		if(pnode)
		{
			pmem->hMCB1D = (void*)pnode;
			pmem->uiSize = pnode->uiSize;
			pmem->uiPhysBase = pnode->uiPhysAddr;

			TEST( pmem->hMCB1D != NULL );
			TEST( pmem->uiSize >= size[i] );
			TEST( (pmem->uiPhysBase % align[i]) == 0 );
		}
		else
		{
			size [i] = 0;
			align[i] = 0;
		}
	}

	MCB1D_ShowStatus();
	printf( "test end...\n\n\n" );

	//-------------------------------------------------------------
	printf( "[TEST 2] random free \n" );

	for( i=0; i<TEST_COUNT; i++ )
	{
		MEMORY1D* pmem = test+i;
		if (pmem->hMCB1D && (rand() & 1))
		{
			MCB1D_Free((MCB1DNODE *)pmem->hMCB1D);
			pmem->hMCB1D = NULL;
			size [i] = 0;
			align[i] = 0;
		}
	}

	MCB1D_ShowStatus();
	printf( "test end...\n\n\n" );

	//-------------------------------------------------------------
	printf( "[TEST 3] random allocation under fragmented state\n" );

	for( i=0; i<TEST_COUNT; i++ )
	{
		MEMORY1D* pmem = test+i;
		if(NULL == pmem->hMCB1D)
		{
			size [i] = rand()%0x100000;
			align[i] = (1<<(rand()%7));
			pnode = MCB1D_Malloc(size[i], align[i]);
			if(pnode)
			{
				pmem->hMCB1D = (void*)pnode;
				pmem->uiSize = pnode->uiSize;
				pmem->uiPhysBase = pnode->uiPhysAddr;

				TEST( pmem->hMCB1D != NULL );
				TEST( pmem->uiSize >= size[i] );
				TEST( (pmem->uiPhysBase % align[i]) == 0 );
			}
			else
			{
				size [i] = 0;
				align[i] = 0;
			}
		}
	}

	MCB1D_ShowStatus();
	printf( "test end...\n\n\n" );

	//-------------------------------------------------------------
	printf( "[TEST 4] random free/allocation\n" );

	for( i=0; i<TEST_COUNT; i++ )
	{
		MEMORY1D* pmem = test+i;
		if( pmem->hMCB1D!=NULL && (rand() & 1) )
		{
			MCB1D_Free((MCB1DNODE *)pmem->hMCB1D);
			pmem->hMCB1D = NULL;
			size [i] = 0;
			align[i] = 0;
		}
	}
	for( i=0; i<TEST_COUNT; i++ )
	{
		MEMORY1D* pmem = test+i;
		if( pmem->hMCB1D!=NULL )
		{
			MCB1D_Free((MCB1DNODE *)pmem->hMCB1D);
			pmem->hMCB1D = NULL;
			size [i] = 0;
			align[i] = 0;
		}
		if( rand() & 1 )
		{
			size [i] = rand()%0x100000;
			align[i] = (1<<(rand()%7));
			pnode = MCB1D_Malloc(size[i], align[i]);
			if(pnode)
			{
				pmem->hMCB1D = (void*)pnode;
				pmem->uiSize = pnode->uiSize;
				pmem->uiPhysBase = pnode->uiPhysAddr;

				TEST( pmem->hMCB1D != NULL );
				TEST( pmem->uiSize >= size[i] );
				TEST( (pmem->uiPhysBase % align[i]) == 0 );
			}
			else
			{
				size [i] = 0;
				align[i] = 0;
			}
		}
	}

	MCB1D_ShowStatus();
	printf( "test end...\n\n\n" );

	//-------------------------------------------------------------
	printf( "[TEST 5] free all\n" );

	for( i=0; i<TEST_COUNT; i++ )
	{
		MEMORY1D* pmem = test+i;
		if( pmem->hMCB1D!=NULL )
		{
			MCB1D_Free((MCB1DNODE *)pmem->hMCB1D);
			pmem->hMCB1D = NULL;
			size [i] = 0;
			align[i] = 0;
		}
	}

	MCB1D_ShowStatus();
	printf( "test end...\n\n\n" );

	//-------------------------------------------------------------
	printf( "[TEST 6] random full allocation under initial state\n" );
	for( i=0; ; i++ )
	{
		MEMORY1D* pmem = test+i;
		size [i] = rand()%0x200000;
		align[i] = (1<<(rand()%7));
		pnode = MCB1D_Malloc(size[i], align[i]);
		if(pnode)
		{
			pmem->hMCB1D = (void*)pnode;
			pmem->uiSize = pnode->uiSize;
			pmem->uiPhysBase = pnode->uiPhysAddr;

			TEST( pmem->hMCB1D != NULL );
			TEST( pmem->uiSize >= size[i] );
			TEST( (pmem->uiPhysBase % align[i]) == 0 );
		}
		else
		{
			break;
		}
	}

	MCB1D_ShowStatus();
	printf( "test end...\n\n\n" );

	//-------------------------------------------------------------
	MCB1D_Finalize();
}

#endif