/*
 * (C) Copyright 2010
 * Nexell Co
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
#include "v_alloc.h"

#ifdef __cplusplus
#define KNL_ASSERT_VALIDRECT(rect)									\
			KNL_ASSERT( IsPowerOf2((rect)->uiWidth) );				\
			KNL_ASSERT( IsPowerOf2((rect)->uiHeight) );            \
			KNL_ASSERT( 0 == ((rect)->m_nX%(rect)->uiWidth ) );    \
			KNL_ASSERT( 0 == ((rect)->m_nY%(rect)->uiHeight) );
#else
#define KNL_ASSERT_VALIDRECT(rect)
#endif

static void			__SplitToPowerOf2( MCB2DNODE * );
static MCB2DNODE *	__Split( MCB2DNODE *, unsigned int, unsigned int );
static int			__IsFreeNode( MCB2DNODE * pMCB2D );
static void			__GetClosest( MCB2DNODE *pMCB2D,
						unsigned int width, unsigned int height, unsigned int align_x, unsigned int align_y,
						unsigned int *pLeastWaste, MCB2DNODE **ppBestFit );

//--------------------------------------------------------------------------
//
//	2D memory manager
//
//	exports
//		- MCB2D_Initialize
//		- MCB2D_Finalize
//		- MCB2D_Malloc
//		- MCB2D_Free
//		- MCB2D_ShowNodes
//		- MCB2D_AvailSpace
//
//------------------------------------------------------------------------------
void		MCB2D_Initialize( MCB2DNODE * pMCB2D, unsigned int width, unsigned int  height,
							unsigned int x, unsigned int y,	MCB2DNODE * pParent )
{
	KNL_ASSERT( 0 < width  );
	KNL_ASSERT( 0 < height );

	if (! pMCB2D)
		return;

	//	Creating initial Node2d
	pMCB2D->bSolid = KNL_FALSE;

	pMCB2D->uiX = x;
	pMCB2D->uiY = y;
	pMCB2D->uiWidth  = width;
	pMCB2D->uiHeight = height;

	pMCB2D->pParent = pParent;
	pMCB2D->pChild1 = (MCB2DNODE *)KNL_NULL;
	pMCB2D->pChild2 = (MCB2DNODE *)KNL_NULL;
	if ( ! pParent ){ __SplitToPowerOf2( pMCB2D ); }
}

void MCB2D_Finalize  (MCB2DNODE * pMCB2D)
{
	if ( ! pMCB2D ){ return; }

	if ( pMCB2D->pChild1 )
	{
		MCB2D_Finalize(pMCB2D->pChild1);
		KNL_FREE(pMCB2D->pChild1);
	}

	if ( pMCB2D->pChild2 )
	{
		MCB2D_Finalize(pMCB2D->pChild2);
		KNL_FREE(pMCB2D->pChild2);
	}

	pMCB2D->bSolid  = KNL_FALSE;
	pMCB2D->pChild1 = (MCB2DNODE *)KNL_NULL;
	pMCB2D->pChild2 = (MCB2DNODE *)KNL_NULL;

	if ( ! pMCB2D->pParent ){ __SplitToPowerOf2( pMCB2D ); }
}

MCB2DNODE * MCB2D_Malloc( MCB2DNODE * pMCB2D, unsigned int width, unsigned int height, unsigned int align_x, unsigned int align_y )
{
	unsigned int waste;
	MCB2DNODE * pNode = (MCB2DNODE *)KNL_NULL;

	if (0 == align_x) align_x = 1;
	if (0 == align_y) align_y = 1;

	//width = ( width + pMCB2D->uiAlign - 1 ) & ( - (int)pMCB2D->uiAlign );	// Round up to alignment

	if ( ! pMCB2D ){ return (MCB2DNODE *)KNL_NULL; }

	// Find the best node to contain the new area
	// Attempting to allocate (widthxheight) space
	__GetClosest( pMCB2D, width, height, align_x, align_y, &waste, &pNode );
	if ( pNode )
	{
		// Split the node into the required space & remainder piece(s)
		pNode = __Split( pNode, width, height );
		// Succesfully allocated pNode (m_nX,m_nY,uiWidth,uiHeight)
		KNL_ASSERT( CNULL != pNode );
		// node가 사용중임을 표시해둔다.
		pNode->bSolid = KNL_TRUE;
	//	KNL_ASSERT( CNULL == pNode->pChild1 );
	//	KNL_ASSERT( CNULL == pNode->pChild2 );
	}
	else
	{
		// Failed to allocate Node2d !!
	}
	return pNode;
}

void		MCB2D_Free(MCB2DNODE * pMCB2D)
{
	if ( ! pMCB2D ){ return; }

	if ( pMCB2D->bSolid )
	{
	//	KNL_ASSERT( pMCB2D->pChild1 == KNL_NULL );
	//	KNL_ASSERT( pMCB2D->pChild2 == KNL_NULL );
		pMCB2D->bSolid = KNL_FALSE;
	}
	else
	{
	//	KNL_ASSERT( pMCB2D->pChild1 );
	//	KNL_ASSERT( pMCB2D->pChild2 );
		// We are being told to delete both child nodes as they are now free
		if ( __IsFreeNode(pMCB2D->pChild1) && __IsFreeNode(pMCB2D->pChild2) )
		{
			KNL_FREE(pMCB2D->pChild1);
			KNL_FREE(pMCB2D->pChild2);
			pMCB2D->pChild1 = pMCB2D->pChild2 = (MCB2DNODE *)KNL_NULL;
		}
	}
	if ( pMCB2D->pParent )
	{
		if ( __IsFreeNode(pMCB2D->pParent) )
		{
			// Now this & sibling are free so we can erase both & coalesce
			MCB2D_Free(pMCB2D->pParent);
		}
		else
		{
			__SplitToPowerOf2(pMCB2D);
		}
	}
}

void MCB2D_ShowNodes (MCB2DNODE * pMCB2D, int (*pLogFunc)(const char *, ...), int Depth )
{
	int  i;

	if ( ! pMCB2D )
		return;

	for(i = 0; i < Depth; i++)
		pLogFunc("|");

	pLogFunc("+%s(0x%08x, y:%d~%d, x:%d~%d)\n",
	 	pMCB2D->bSolid ? "*" : "-",
	 	(int)(pMCB2D->uiX+pMCB2D->uiY*4096),
	 	(int)pMCB2D->uiY, (int)pMCB2D->uiY + (int)pMCB2D->uiHeight,
	 	(int)pMCB2D->uiX, (int)pMCB2D->uiX + (int)pMCB2D->uiWidth);

	if ( pMCB2D->pChild1 ) MCB2D_ShowNodes (pMCB2D->pChild1, pLogFunc, Depth+1 );
	if ( pMCB2D->pChild2 ) MCB2D_ShowNodes (pMCB2D->pChild2, pLogFunc, Depth+1 );
}

/*
unsigned int	MCB2D_AvailSpace(MCB2DNODE * pMCB2D)
{
	if ( ! pMCB2D ){ return 0; }
	if ( pMCB2D->pChild2 )
		return MCB2D_AvailSpace(pMCB2D->pChild1) + MCB2D_AvailSpace(pMCB2D->pChild2);
	else if ( pMCB2D->pChild1 != (MCB2DNODE *)KNL_NULL )
		return 0;
	else
		return pMCB2D->uiWidth * pMCB2D->uiHeight;
}
*/

//------------------------------------------------------------------------------
// implementation
//		- __SplitHorizontal
//		- __SplitVertical
//		- __Split
//		- __GetClosest
//		- __SplitPowerOf2Horizontal
//		- __SplitPowerOf2Vertical
//		- __SplitToPowerOf2
//		- __IsFreeNode
//		- __IsPowerOf2
//------------------------------------------------------------------------------
static MCB2DNODE * __SplitHorizontal( MCB2DNODE * pMCB2D, unsigned int width, unsigned int height )
{
	// 좌측상단의 width*height를 제외한 나머지 영역에서
	// 가장큰 2의 승수꼴 사각형을 잘라낸다.
	unsigned int newsize;

	for( newsize=1; width+newsize <= pMCB2D->uiWidth; newsize<<=1 ){ /* do nothing */ }

	newsize>>=1;
//	KNL_ASSERT( 0 < newsize );
//	KNL_ASSERT( 0 == ( (m_nX+(uiWidth-newsize)) % newsize ) );

	// Alloc the child1
	pMCB2D->pChild1 = (MCB2DNODE *)KNL_MALLOC(sizeof(MCB2DNODE));
	MCB2D_Initialize( pMCB2D->pChild1, (pMCB2D->uiWidth - newsize), pMCB2D->uiHeight,
					pMCB2D->uiX, pMCB2D->uiY, pMCB2D );

	// Alloc the child2
	pMCB2D->pChild2 = (MCB2DNODE *)KNL_MALLOC(sizeof(MCB2DNODE));
	MCB2D_Initialize( pMCB2D->pChild2, newsize, pMCB2D->uiHeight,
					pMCB2D->uiX + (pMCB2D->uiWidth - newsize), pMCB2D->uiY, pMCB2D );

	__SplitToPowerOf2(pMCB2D->pChild2);
	return __Split( pMCB2D->pChild1, width, height );
}

static MCB2DNODE * __SplitVertical ( MCB2DNODE * pMCB2D, unsigned int width, unsigned int height )
{
	// 좌측상단의 width*height를 제외한 나머지 영역에서
	// 가장큰 2의 승수꼴 사각형을 잘라낸다.
	unsigned int newsize;
	for( newsize=1; height+newsize <= pMCB2D->uiHeight; newsize<<=1 ){ /* do nothing */ }

	newsize>>=1;
//	KNL_ASSERT( 0 < newsize );
//	KNL_ASSERT( 0 == ( (m_nY+(uiHeight-newsize)) % newsize ) );

	// Alloc the child1
	pMCB2D->pChild1 = (MCB2DNODE *)KNL_MALLOC(sizeof(MCB2DNODE));
	MCB2D_Initialize( pMCB2D->pChild1, pMCB2D->uiWidth, (pMCB2D->uiHeight - newsize),
					pMCB2D->uiX, pMCB2D->uiY, pMCB2D );

	// Alloc the child2
	pMCB2D->pChild2 = (MCB2DNODE *)KNL_MALLOC(sizeof(MCB2DNODE));
	MCB2D_Initialize( pMCB2D->pChild2, pMCB2D->uiWidth, newsize,
					pMCB2D->uiX, pMCB2D->uiY + (pMCB2D->uiHeight - newsize), pMCB2D );

	__SplitToPowerOf2(pMCB2D->pChild2);
	return __Split( pMCB2D->pChild1, width, height );
}

static MCB2DNODE * __Split( MCB2DNODE * pMCB2D, unsigned int width, unsigned int height )
{
	unsigned int dw = pMCB2D->uiWidth  - width ;
	unsigned int dh = pMCB2D->uiHeight - height;

	// Splitting Node2d (m_nWidthxm_nHeight) to give (widthxheight) space
//	KNL_ASSERT( ! pMCB2D->m_Solid );
//	KNL_ASSERT( width <= pMCB2D->uiWidth );
//	KNL_ASSERT( height <= pMCB2D->uiHeight );

	if ( dh > 0 || dw > 0 )
	{
		int bDivide = (dw > dh) ? KNL_TRUE : KNL_FALSE;

		if ( bDivide )	{ return __SplitHorizontal( pMCB2D, width, height ); }
		else 			{ return __SplitVertical( pMCB2D, width, height ); }
	}
	return pMCB2D;
}

static void		__GetClosest( MCB2DNODE *pMCB2D,
			unsigned int width, unsigned int height, unsigned int align_x, unsigned int align_y,
			unsigned int *pLeastWaste, MCB2DNODE **ppBestFit )
{
	if ( pMCB2D->bSolid )
		return; // this node is in use
	if ( pMCB2D->uiWidth < width || pMCB2D->uiHeight < height )
		return; // this one (and any children) are too small
	if ( pMCB2D->pChild2 ) // This is a parent node, just check child nodes for better fit
	{
//		KNL_ASSERT( CNULL != pMCB2D->pChild1 );
		__GetClosest( pMCB2D->pChild1, width, height, align_x, align_y, pLeastWaste, ppBestFit );

		if ( 0 == pLeastWaste ){ return; }
		__GetClosest( pMCB2D->pChild2, width, height, align_x, align_y, pLeastWaste, ppBestFit );
		//if ( 0 == pLeastWaste ){ return; }
	}
	else
	{
		KNL_ASSERT_VALIDRECT( this );
		if ( 0 == (pMCB2D->uiX % align_x) && 0 == (pMCB2D->uiY % align_y) )
		{
			// This node is free & big enough - check for fit
			unsigned int waste = pMCB2D->uiWidth * pMCB2D->uiHeight - width * height;
			if ( *ppBestFit == (MCB2DNODE *)KNL_NULL || waste < *pLeastWaste )
			{
				*pLeastWaste = waste;
				*ppBestFit   = pMCB2D;
			}
		}
	}
}

static int	__SplitPowerOf2Horizontal ( MCB2DNODE * pMCB2D )
{
	unsigned int newsize;

	// 사각형의 위치값(m_nY)을 나머지 없이 나눌수 있는 최대 2의 승수를 구함.
    for( newsize=1; (0 == (pMCB2D->uiX & newsize)) && (newsize<pMCB2D->uiWidth); newsize<<=1 ){ /* do nothing */ }

    // 구해진 값이 현재 크기(uiHeight)보다 작다면 쪼개야 한다.
    if ( newsize < pMCB2D->uiWidth )
	{
		// Alloc the child1
		pMCB2D->pChild1 = (MCB2DNODE *)KNL_MALLOC(sizeof(MCB2DNODE));
		MCB2D_Initialize( pMCB2D->pChild1, newsize, pMCB2D->uiHeight,
						pMCB2D->uiX, pMCB2D->uiY, pMCB2D );

		// Alloc the child2
		pMCB2D->pChild2 = (MCB2DNODE *)KNL_MALLOC(sizeof(MCB2DNODE));
		MCB2D_Initialize( pMCB2D->pChild2, pMCB2D->uiWidth - newsize, pMCB2D->uiHeight,
						pMCB2D->uiX + newsize, pMCB2D->uiY, pMCB2D );

		__SplitToPowerOf2(pMCB2D->pChild1);
		__SplitToPowerOf2(pMCB2D->pChild2);
		return KNL_TRUE;
	}
	return KNL_FALSE;
}

static int	__SplitPowerOf2Vertical ( MCB2DNODE * pMCB2D )
{
	unsigned int newsize;

    // 사각형의 위치값(m_nY)을 나머지 없이 나눌수 있는 최대 2의 승수를 구함.
    for( newsize=1;(0 == (pMCB2D->uiY & newsize)) && (newsize<pMCB2D->uiHeight); newsize<<=1 ){ /* do nothing */ }

    // 구해진 값이 현재 크기(uiHeight)보다 작다면 쪼개야 한다.
    if ( newsize < pMCB2D->uiHeight )
	{
		pMCB2D->pChild1 = (MCB2DNODE *)KNL_MALLOC(sizeof(MCB2DNODE));
		MCB2D_Initialize( pMCB2D->pChild1, pMCB2D->uiWidth, newsize,
						pMCB2D->uiX, pMCB2D->uiY, pMCB2D );

		pMCB2D->pChild2 = (MCB2DNODE *)KNL_MALLOC(sizeof(MCB2DNODE));
		MCB2D_Initialize( pMCB2D->pChild2, pMCB2D->uiWidth, pMCB2D->uiHeight - newsize,
						pMCB2D->uiX, pMCB2D->uiY+newsize, pMCB2D );

		__SplitToPowerOf2(pMCB2D->pChild1);
		__SplitToPowerOf2(pMCB2D->pChild2);
		return KNL_TRUE;
	}
	return KNL_FALSE;
}

static void		__SplitToPowerOf2 ( MCB2DNODE * pMCB2D )
{
	int	bDivide;

	KNL_ASSERT( 0 < pMCB2D->uiWidth );
	KNL_ASSERT( 0 < pMCB2D->uiHeight );

	bDivide = ( pMCB2D->uiWidth >= pMCB2D->uiHeight ) ? KNL_TRUE : KNL_FALSE; // 큰변을 쪼갠다
	if ( bDivide )
	{
		if ( __SplitPowerOf2Horizontal(pMCB2D) ){ return; }
		if ( __SplitPowerOf2Vertical(pMCB2D) ){ return; }
	}
	else
	{
		if ( __SplitPowerOf2Vertical(pMCB2D) ){ return; }
		if ( __SplitPowerOf2Horizontal(pMCB2D) ){ return; }
	}
}

static int	__IsFreeNode( MCB2DNODE * pMCB2D )
{
	if ( pMCB2D->bSolid )
		return KNL_FALSE;

	if ( pMCB2D->pChild1 )
	{
		if ( !__IsFreeNode(pMCB2D->pChild1) )
			return KNL_FALSE;
	}

	if ( pMCB2D->pChild2 )
	{
		if ( !__IsFreeNode(pMCB2D->pChild2) )
			return KNL_FALSE;
	}
	return KNL_TRUE;
}

/*
static int __IsPowerOf2( unsigned int value )
{
	return ( (value & (value-1)) == 0 );
}
*/
