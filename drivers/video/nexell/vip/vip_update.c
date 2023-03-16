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
#include "vip_update.h"

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
typedef struct __VIP_REG_UPDATE {
	CBOOL	VSYNC_FLAG;
	U32		FLAG;
	CBOOL	INTERLACE;
	U16 	CLIPLEFT;
	U16 	CLIPRIGHT;
	U16 	CLIPTOP;
	U16 	CLIPBOTTOM;
	U32 	CLIP_LU_SADDR;
	U32 	CLIP_LU_EADDR;
	U32 	CLIP_CR_SADDR;
	U32 	CLIP_CR_EADDR;
	U32 	CLIP_CB_SADDR;
	U32 	CLIP_CB_EADDR;
	U32 	CLIP_ROTFLIP;
	U16		DECI_TARGETW;
	U16		DECI_TARGETH;
	U16		DECI_DELTAW;
	U16		DECI_DELTAH;
	S16		DECI_CLEARW;
	S16		DECI_CLEARH;
	U32 	DECI_LU_SADDR;
	U32 	DECI_LU_EADDR;
	U32 	DECI_CB_SADDR;
	U32 	DECI_CB_EADDR;
	U32 	DECI_CR_SADDR;
	U32 	DECI_CR_EADDR;
	U32 	DECI_ROTFLIP;
} VIP_REG_UPDATE;

static VIP_REG_UPDATE	REG_UPDATE[2] = { {0, }, };

//------------------------------------------------------------------------------
void	VIP_UPDATE_SetScanMode(int Module, CBOOL bInterlace)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];
	RegVal->INTERLACE = bInterlace;
}

//------------------------------------------------------------------------------
void	VIP_UPDATE_SetFlag(int Module, U32 Flag)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];
	RegVal->FLAG |= Flag;
}

//------------------------------------------------------------------------------
#if defined (NX_DEBUG)
static CBOOL	__vip_updatefield = 1;
#endif

CBOOL	VIP_UPDATE_OnInterrupt(int Module)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];
	const    U32 LASTFIELD = 1U<<5;
	register U32 flag;

	U32 offset = (Module * VIP_REG_OFFS);

	#if defined(NX_DEBUG)
	if (CTRUE == RegVal->INTERLACE) {
		if (__vip_updatefield) {
			if (0 == (REG_VIP_SYNCCTRL(Module) & LASTFIELD))	// Skip odd field
				return CFALSE;
		} else {
			if (REG_VIP_SYNCCTRL(Module) & LASTFIELD)			// Skip even field
				return CFALSE;
		}
	}
	#else
	if (CTRUE == RegVal->INTERLACE) {
		//if (REG_VIP_SYNCCTRL(Module) & LASTFIELD)			// Skip even field
		if (0 == (REG_VIP_SYNCCTRL(Module) & LASTFIELD))	// Skip odd field
			return CFALSE;
	}
	#endif

	flag = RegVal->FLAG;

	/* Clipper */
	if (flag & VIP_REG_UPDATE_FLAG_CLIPPER) {
		if (flag & VIP_REG_UPDATE_FLAG_CLIPREGION) {
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x208), (U16)RegVal->CLIPLEFT);	// REG_VIP_CLIP_LEFT
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x20A), (U16)RegVal->CLIPRIGHT);	// REG_VIP_CLIP_RIGHT
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x20C), (U16)RegVal->CLIPTOP);		// REG_VIP_CLIP_TOP
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x20E), (U16)RegVal->CLIPBOTTOM);	// REG_VIP_CLIP_BOTTOM
		}

		if (flag & VIP_REG_UPDATE_FLAG_CLIPLINEARADDR) {
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x264), (U16)(RegVal->CLIP_LU_SADDR >> 16));		// REG_VIP_CLIP_BASEADDRH
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x266), (U16)(RegVal->CLIP_LU_SADDR & 0xFFFFUL));	// REG_VIP_CLIP_BASEADDRL
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x268), (U16)(RegVal->CLIP_LU_EADDR >> 16));		// REG_VIP_CLIP_STRIDEH
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x26A), (U16)(RegVal->CLIP_LU_EADDR & 0xFFFFUL));	// REG_VIP_CLIP_STRIDEL
		}

		if (flag & VIP_REG_UPDATE_FLAG_CLIPBLOCKADDR) {
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x23E), (U16)(RegVal->CLIP_LU_SADDR >> 24));	// REG_VIP_CLIP_LUSEG
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x248), (U16)(RegVal->CLIP_LU_SADDR));			// REG_VIP_CLIP_LULEFT
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x254), (U16)(RegVal->CLIP_LU_SADDR >> 12));	// REG_VIP_CLIP_LUTOP
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x24E), (U16)(RegVal->CLIP_LU_EADDR));			// REG_VIP_CLIP_LURIGHT
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x25A), (U16)(RegVal->CLIP_LU_EADDR >> 12));	// REG_VIP_CLIP_LUBOTTOM

			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x240), (U16)(RegVal->CLIP_CB_SADDR >> 24));	// REG_VIP_CLIP_CRSEG
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x24A), (U16)(RegVal->CLIP_CR_SADDR));			// REG_VIP_CLIP_CRLEFT
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x256), (U16)(RegVal->CLIP_CR_SADDR >> 12));	// REG_VIP_CLIP_CRTOP
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x250), (U16)(RegVal->CLIP_CR_EADDR));			// REG_VIP_CLIP_CRRIGHT
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x25C), (U16)(RegVal->CLIP_CR_EADDR >> 12));	// REG_VIP_CLIP_CRBOTTOM

			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x242), (U16)(RegVal->CLIP_CR_SADDR >> 24));	// REG_VIP_CLIP_CBSEG
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x24C), (U16)(RegVal->CLIP_CB_SADDR));			// REG_VIP_CLIP_CBLEFT
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x258), (U16)(RegVal->CLIP_CB_SADDR >> 12));	// REG_VIP_CLIP_CBTOP
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x252), (U16)(RegVal->CLIP_CB_EADDR));			// REG_VIP_CLIP_CBRIGHT
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x25E), (U16)(RegVal->CLIP_CB_EADDR >> 12));	// REG_VIP_CLIP_CBBOTTOM
		}

		if (flag & VIP_REG_UPDATE_FLAG_CLIPRHV) {
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x246), (U16)RegVal->CLIP_ROTFLIP);			// REG_VIP_CLIP_ROTFLIP
		}
	}

	/* Decimator */
#if (1)
	if (flag & VIP_REG_UPDATE_FLAG_DECIMATOR)	{
		if (flag & VIP_REG_UPDATE_FLAG_DECISIZE) {
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x210), (U16)RegVal->DECI_TARGETW);   	// REG_VIP_DECI_TARGETW
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x212), (U16)RegVal->DECI_TARGETH);	// REG_VIP_DECI_TARGETH
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x214), (U16)RegVal->DECI_DELTAW);   	// REG_VIP_DECI_DELTAW
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x216), (U16)RegVal->DECI_DELTAH);		// REG_VIP_DECI_DELTAH
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x218), (U16)RegVal->DECI_CLEARW);   	// REG_VIP_DECI_CLEARW
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x21A), (U16)RegVal->DECI_CLEARH);		// REG_VIP_DECI_CLEARH

		}

		if (flag & VIP_REG_UPDATE_FLAG_DECIBLOCKADDR) {
			REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x21C), (U16)(RegVal->DECI_LU_SADDR >> 24));	// REG_VIP_DECI_LUSEG
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x226), (U16)(RegVal->DECI_LU_SADDR));       	// REG_VIP_DECI_LULEFT
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x232), (U16)(RegVal->DECI_LU_SADDR >> 12)); 	// REG_VIP_DECI_LUTOP
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x22C), (U16)(RegVal->DECI_LU_EADDR));       	// REG_VIP_DECI_LURIGHT
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x238), (U16)(RegVal->DECI_LU_EADDR >> 12)); 	// REG_VIP_DECI_LUBOTTOM

            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x220), (U16)(RegVal->DECI_CB_SADDR >> 24)); 	// REG_VIP_DECI_CBSEG
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x228), (U16)(RegVal->DECI_CR_SADDR));       	// REG_VIP_DECI_CRLEFT
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x234), (U16)(RegVal->DECI_CR_SADDR >> 12)); 	// REG_VIP_DECI_CRTOP
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x22E), (U16)(RegVal->DECI_CR_EADDR));       	// REG_VIP_DECI_CRRIGHT
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x23A), (U16)(RegVal->DECI_CR_EADDR >> 12)); 	// REG_VIP_DECI_CRBOTTOM

            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x21E), (U16)(RegVal->DECI_CR_SADDR >> 24)); 	// REG_VIP_DECI_CRSEG
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x22A), (U16)(RegVal->DECI_CB_SADDR));       	// REG_VIP_DECI_CBLEFT
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x236), (U16)(RegVal->DECI_CB_SADDR >> 12)); 	// REG_VIP_DECI_CBTOP
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x230), (U16)(RegVal->DECI_CB_EADDR));       	// REG_VIP_DECI_CBRIGHT
            REG_VIP_WRITE16((VIP_BASEADDR + offset + 0x23C), (U16)(RegVal->DECI_CB_EADDR >> 12)); 	// REG_VIP_DECI_CBBOTTOM
		}

		if (flag & VIP_REG_UPDATE_FLAG_DECIRHV)
			REG_VIP_DECI_ROTFLIP(Module)	= (U16)RegVal->DECI_ROTFLIP;
	}
#endif

	RegVal->FLAG = 0;
	RegVal->VSYNC_FLAG = CTRUE;

	return CTRUE;
}

//------------------------------------------------------------------------------
void	VIP_UPDATE_SetClipRegion(int Module, U32 Left, U32 Top, U32 Right, U32 Bottom)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];

	NX_ASSERT(4096 > Left);
	NX_ASSERT(2048 > Top);
	NX_ASSERT(4096 > Right);
	NX_ASSERT(2048 > Bottom);
	NX_ASSERT(Left < Right);
	NX_ASSERT(Top < Bottom);

	RegVal->CLIPLEFT 	= (U16)Left;
	RegVal->CLIPRIGHT	= (U16)Right;
	RegVal->CLIPTOP		= (U16)Top;
	RegVal->CLIPBOTTOM	= (U16)Bottom;
}

//------------------------------------------------------------------------------
void	VIP_UPDATE_SetClipperBlockAddr(int Module, U32 Format,
				CBOOL bRotation, U32 Width, U32 Height, U32 LuAddr, U32 CbAddr, U32 CrAddr)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];

	NX_ASSERT(3 > Format);
	NX_ASSERT(((CFALSE == bRotation) && (
				((0 == (Width  % 8)) && (NX_VIP_FORMAT_444 == Format)) ||
				((0 == (Width  % 16)) && (NX_VIP_FORMAT_422 == Format)) ||
				((0 == (Width  % 16)) && (NX_VIP_FORMAT_420 == Format))))
				||
				((CTRUE  == bRotation) && (
				((0 == (Height % 8)) && (NX_VIP_FORMAT_444 == Format)) ||
				((0 == (Height % 16)) && (NX_VIP_FORMAT_422 == Format)) ||
				((0 == (Height % 16)) && (NX_VIP_FORMAT_420 == Format)))));
	NX_ASSERT((CTRUE  == bRotation) || (0 == (((LuAddr & 0x00000FFF)>> 0) % 8)));
	NX_ASSERT((CFALSE == bRotation) || (0 == (((LuAddr & 0x00FFF000)>>12) % 8)));
//	NX_ASSERT(4096 > (((LuAddr & 0x00000FFF)>> 0) + Width));
//	NX_ASSERT(4096 > (((LuAddr & 0x00FFF000)>>12) + Height));
	RegVal->CLIP_LU_SADDR	= LuAddr;
	RegVal->CLIP_LU_EADDR	= LuAddr + (Height<<12) + Width;

	if (NX_VIP_FORMAT_420 == Format) {
		NX_ASSERT(0 == (Width&1));
		NX_ASSERT(0 == (Height&1));
		Width 	>>= 1;
		Height 	>>= 1;
	} else if (NX_VIP_FORMAT_422 == Format)	{
		NX_ASSERT(0 == (Width&1));
		Width 	>>= 1;
	}

	NX_ASSERT((CTRUE  == bRotation) || (0 == (((CbAddr & 0x00000FFF)>> 0) % 8)));
	NX_ASSERT((CFALSE == bRotation) || (0 == (((CbAddr & 0x00FFF000)>>12) % 8)));
//	NX_ASSERT(4096 > (((CbAddr & 0x00000FFF)>> 0) + Width));
//	NX_ASSERT(4096 > (((CbAddr & 0x00FFF000)>>12) + Height));

	RegVal->CLIP_CB_SADDR	= CbAddr;
	RegVal->CLIP_CB_EADDR	= CbAddr + (Height<<12) + Width;

	NX_ASSERT((CTRUE  == bRotation) || (0 == (((CrAddr & 0x00000FFF)>> 0) % 8)));
	NX_ASSERT((CFALSE == bRotation) || (0 == (((CrAddr & 0x00FFF000)>>12) % 8)));

//	NX_ASSERT(4096 > (((CrAddr & 0x00000FFF)>> 0) + Width));
//	NX_ASSERT(4096 > (((CrAddr & 0x00FFF000)>>12) + Height));

	RegVal->CLIP_CR_SADDR	= CrAddr;
	RegVal->CLIP_CR_EADDR	= CrAddr + (Height<<12) + Width;
}

//------------------------------------------------------------------------------
void	VIP_UPDATE_SetClipperLinearAddr(int Module, U32 BaseAddr, U32 Stride)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];

	NX_ASSERT(0 == (BaseAddr % 8));
	NX_ASSERT(0 == (Stride % 1));

	RegVal->CLIP_LU_SADDR	= BaseAddr;
	RegVal->CLIP_LU_EADDR	= Stride;
}

//------------------------------------------------------------------------------
void	VIP_UPDATE_SetClipperRHV(int Module, CBOOL bRotation, CBOOL bHFlip, CBOOL bVFlip)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];

	const U32 ROTATION	 = 1UL<<2;
	const U32 VFLIP		 = 1UL<<1;
	const U32 HFLIP		 = 1UL<<0;

	RegVal->CLIP_ROTFLIP = ((bRotation) ? ROTATION : 0) | ((bVFlip) ? VFLIP : 0) | ((bHFlip) ? HFLIP : 0);
}

//------------------------------------------------------------------------------
void	VIP_UPDATE_SetDecimation(int Module, U32 SrcWidth, U32 SrcHeight, U32 DstWidth, U32 DstHeight)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];

	NX_ASSERT(4096 > SrcWidth);
	NX_ASSERT(2048 > SrcHeight);
	NX_ASSERT(DstWidth <= SrcWidth);
	NX_ASSERT(DstHeight <= SrcHeight);

	RegVal->DECI_TARGETW = (U16)DstWidth;
	RegVal->DECI_TARGETH = (U16)DstHeight;
	RegVal->DECI_DELTAW  = (U16)(SrcWidth - DstWidth);
	RegVal->DECI_DELTAH  = (U16)(SrcHeight - DstHeight);
	RegVal->DECI_CLEARW  = (S16)((DstWidth<<1) - SrcWidth);
	RegVal->DECI_CLEARH  = (S16)((DstHeight<<1) - SrcHeight);
}

//------------------------------------------------------------------------------
void 	VIP_UPDATE_SetDecimatorBlockAddr(int Module, U32 Format, CBOOL bRotation,
					U32 Width, U32 Height, U32 LuAddr, U32 CbAddr, U32 CrAddr)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];

	NX_ASSERT(3 > Format);
	NX_ASSERT(((CFALSE == bRotation) && (
				((0 == (Width  % 8)) && (NX_VIP_FORMAT_444 == Format)) ||
				((0 == (Width  % 16)) && (NX_VIP_FORMAT_422 == Format)) ||
				((0 == (Width  % 16)) && (NX_VIP_FORMAT_420 == Format))))
				||
				((CTRUE  == bRotation) && (
				((0 == (Height % 8)) && (NX_VIP_FORMAT_444 == Format)) ||
				((0 == (Height % 16)) && (NX_VIP_FORMAT_422 == Format)) ||
				((0 == (Height % 16)) && (NX_VIP_FORMAT_420 == Format)))));
	NX_ASSERT((CTRUE  == bRotation) || (0 == (((LuAddr & 0x00000FFF)>> 0) % 8)));
	NX_ASSERT((CFALSE == bRotation) || (0 == (((LuAddr & 0x00FFF000)>>12) % 8)));
	NX_ASSERT(4096 > (((LuAddr & 0x00000FFF)>> 0) + Width));
	NX_ASSERT(4096 > (((LuAddr & 0x00FFF000)>>12) + Height));

	RegVal->DECI_LU_SADDR	= LuAddr;
	RegVal->DECI_LU_EADDR	= LuAddr + (Height<<12) + Width;

	if (NX_VIP_FORMAT_420 == Format) {
		NX_ASSERT(0 == (Width&1));
		NX_ASSERT(0 == (Height&1));
		Width 	>>= 1;
		Height 	>>= 1;
	} else if (NX_VIP_FORMAT_422 == Format)	{
		NX_ASSERT(0 == (Width&1));
		Width 	>>= 1;
	}

	NX_ASSERT((CTRUE  == bRotation) || (0 == (((CbAddr & 0x00000FFF)>> 0) % 8)));
	NX_ASSERT((CFALSE == bRotation) || (0 == (((CbAddr & 0x00FFF000)>>12) % 8)));
	NX_ASSERT(4096 > (((CbAddr & 0x00000FFF)>> 0) + Width));
	NX_ASSERT(4096 > (((CbAddr & 0x00FFF000)>>12) + Height));

	RegVal->DECI_CB_SADDR	= CbAddr;
	RegVal->DECI_CB_EADDR	= CbAddr + (Height<<12) + Width;

	NX_ASSERT((CTRUE  == bRotation) || (0 == (((CrAddr & 0x00000FFF)>> 0) % 8)));
	NX_ASSERT((CFALSE == bRotation) || (0 == (((CrAddr & 0x00FFF000)>>12) % 8)));
	NX_ASSERT(4096 > (((CrAddr & 0x00000FFF)>> 0) + Width));
	NX_ASSERT(4096 > (((CrAddr & 0x00FFF000)>>12) + Height));

	RegVal->DECI_CR_SADDR	= CrAddr;
	RegVal->DECI_CR_EADDR	= CrAddr + (Height<<12) + Width;
}

//------------------------------------------------------------------------------
void	VIP_UPDATE_SetDecimatorRHV(int Module, CBOOL bRotation, CBOOL bHFlip, CBOOL bVFlip)
{
	VIP_REG_UPDATE * RegVal = &REG_UPDATE[Module];
	const U32 ROTATION	 = 1UL<<2;
	const U32 VFLIP		 = 1UL<<1;
	const U32 HFLIP		 = 1UL<<0;

	RegVal->DECI_ROTFLIP = ((bRotation) ? ROTATION : 0) | ((bVFlip) ? VFLIP : 0) | ((bHFlip) ? HFLIP : 0);
}

