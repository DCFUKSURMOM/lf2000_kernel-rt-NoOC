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

#ifndef __VIP_UPDATE_H__
#define __VIP_UPDATE_H__

#include <linux/kernel.h>
#include <mach/platform.h>

//------------------------------------------------------------------------------
// VIP Register definition
//------------------------------------------------------------------------------
#define	VIP_BASEADDR				IO_ADDRESS(PHY_BASEADDR_VIP_MODULE)
#define	VIP_REG_OFFS				OFFSET_OF_VIP_MODULE

#if (0)
#define	REG_VIP_WRITE16(a, v)		(*(volatile unsigned short*)(a) = v)
#else
#define	REG_VIP_WRITE16(a, v)		WriteIOW((volatile unsigned short*)(a), v)
#endif

#define REG_VIP_CONFIG(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x000)) // VIP Configuration Register
#define REG_VIP_HVINT(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x002)) // VIP Interrupt Control Register
#define REG_VIP_SYNCCTRL(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x004)) // VIP Sync Control Register
#define REG_VIP_SYNCMON(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x006)) // VIP Sync Monitor Register
#define REG_VIP_VBEGIN(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x008)) // VIP Vertical Sync Start Register
#define REG_VIP_VEND(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x00A)) // VIP Vertical Sync End Register
#define REG_VIP_HBEGIN(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x00C)) // VIP Horizontal Sync Start Register
#define REG_VIP_HEND(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x00E)) // VIP Horizontal Sync End Register
#define REG_VIP_FIFOCTRL(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x010)) // VIP FIFO Control Register
#define REG_VIP_HCOUNT(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x012)) // VIP Horizontal Counter Register
#define REG_VIP_VCOUNT(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x014)) // VIP Vertical Counter Register
#define REG_VIP_CDENB(dev)			(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x200)) // VIP Clipper & Decimator Enable Register
#define REG_VIP_DONEINT(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x202)) // VIP Done Interrupt Register
#define REG_VIP_IMGWIDTH(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x204)) // VIP Image Width Register
#define REG_VIP_IMGHEIGHT(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x206)) // VIP Image Height Register
#define REG_VIP_CLIP_LEFT(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x208)) // VIP Clipper Left Register
#define REG_VIP_CLIP_RIGHT(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x20A)) // VIP Clipper Right Register
#define REG_VIP_CLIP_TOP(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x20C)) // VIP Clipper Top Register
#define REG_VIP_CLIP_BOTTOM(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x20E)) // VIP Clipper Bottom Register
#define REG_VIP_DECI_TARGETW(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x210)) // VIP Decimator Target Width Register
#define REG_VIP_DECI_TARGETH(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x212)) // VIP Decimator Target Height Register
#define REG_VIP_DECI_DELTAW(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x214)) // VIP Decimator Delta Width Register
#define REG_VIP_DECI_DELTAH(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x216)) // VIP Decimator Delta Height Register
#define REG_VIP_DECI_CLEARW(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x218)) // VIP Decimator Clear Width Register
#define REG_VIP_DECI_CLEARH(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x21A)) // VIP Decimator Clear Height Register
#define REG_VIP_DECI_LUSEG(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x21C)) // VIP Decimator Lu Segment Register
#define REG_VIP_DECI_CRSEG(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x21E)) // VIP Decimator Cr Segment Register
#define REG_VIP_DECI_CBSEG(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x220)) // VIP Decimator Cb Segment Register
#define REG_VIP_DECI_FORMAT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x222)) // VIP Decimator Format Register
#define REG_VIP_DECI_ROTFLIP(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x224)) // VIP Decimator Rotation & Flip Register
#define REG_VIP_DECI_LULEFT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x226)) // VIP Decimator Lu Left Register
#define REG_VIP_DECI_CRLEFT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x228)) // VIP Decimator Cr Left Register
#define REG_VIP_DECI_CBLEFT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x22A)) // VIP Decimator Cb Left Register
#define REG_VIP_DECI_LURIGHT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x22C)) // VIP Decimator Lu Right Register
#define REG_VIP_DECI_CRRIGHT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x22E)) // VIP Decimator Cr Right Register
#define REG_VIP_DECI_CBRIGHT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x230)) // VIP Decimator Cb Right Register
#define REG_VIP_DECI_LUTOP(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x232)) // VIP Decimator Lu Top Register
#define REG_VIP_DECI_CRTOP(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x234)) // VIP Decimator Cr Top Register
#define REG_VIP_DECI_CBTOP(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x236)) // VIP Decimator Cb Top Register
#define REG_VIP_DECI_LUBOTTOM(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x238)) // VIP Decimator Lu Bottom Register
#define REG_VIP_DECI_CRBOTTOM(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x23A)) // VIP Decimator Cr Bottom Register
#define REG_VIP_DECI_CBBOTTOM(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x23C)) // VIP Decimator Cb Bottom Register
#define REG_VIP_CLIP_LUSEG(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x23E)) // VIP Clipper Lu Segment Register
#define REG_VIP_CLIP_CRSEG(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x240)) // VIP Clipper Cr Segment Register
#define REG_VIP_CLIP_CBSEG(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x242)) // VIP Clipper Cb Segment Register
#define REG_VIP_CLIP_FORMAT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x244)) // VIP Clipper Format Register
#define REG_VIP_CLIP_ROTFLIP(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x246)) // VIP Clipper Rotation & Flip Register
#define REG_VIP_CLIP_LULEFT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x248)) // VIP Clipper Lu Left Register
#define REG_VIP_CLIP_CRLEFT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x24A)) // VIP Clipper Cr Left Register
#define REG_VIP_CLIP_CBLEFT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x24C)) // VIP Clipper Cb Left Register
#define REG_VIP_CLIP_LURIGHT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x24E)) // VIP Clipper Lu Right Register
#define REG_VIP_CLIP_CRRIGHT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x250)) // VIP Clipper Cr Right Register
#define REG_VIP_CLIP_CBRIGHT(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x252)) // VIP Clipper Cb Right Register
#define REG_VIP_CLIP_LUTOP(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x254)) // VIP Clipper Lu Top Register
#define REG_VIP_CLIP_CRTOP(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x256)) // VIP Clipper Cr Top Register
#define REG_VIP_CLIP_CBTOP(dev)		(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x258)) // VIP Clipper Cb Top Register
#define REG_VIP_CLIP_LUBOTTOM(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x25A)) // VIP Clipper Lu Bottom Register
#define REG_VIP_CLIP_CRBOTTOM(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x25C)) // VIP Clipper Cr Bottom Register
#define REG_VIP_CLIP_CBBOTTOM(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x25E)) // VIP Clipper Cb Bottom Register
#define REG_VIP_VIP_SCANMODE(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x260)) // VIP Clipper & Decimator Scan Mode Register
#define REG_VIP_CLIP_YUYVENB(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x262)) // VIP Clipper Linear YUYV Enable Register
#define REG_VIP_CLIP_BASEADDRH(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x264)) // VIP Clipper Linear Base Address High Register
#define REG_VIP_CLIP_BASEADDRL(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x266)) // VIP Clipper Linear Base Address Low Register
#define REG_VIP_CLIP_STRIDEH(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x268)) // VIP Clipper Linear Stride High Register
#define REG_VIP_CLIP_STRIDEL(dev)	(*(volatile U16 *)(VIP_BASEADDR + (dev * VIP_REG_OFFS)+ 0x26A)) // VIP Clipper Linear Stride Low Register

//------------------------------------------------------------------------------
// VIP Interrupt Update Flag
//------------------------------------------------------------------------------
#define VIP_REG_UPDATE_FLAG_CLIPREGION		(1<< 0)
#define VIP_REG_UPDATE_FLAG_CLIPBLOCKADDR	(1<< 1)
#define VIP_REG_UPDATE_FLAG_CLIPLINEARADDR	(1<< 2)
#define VIP_REG_UPDATE_FLAG_CLIPRHV			(1<< 3)
#define VIP_REG_UPDATE_FLAG_CLIPPER			(0x000000FF)

#define VIP_REG_UPDATE_FLAG_DECISIZE		(1<< 8)
#define VIP_REG_UPDATE_FLAG_DECIBLOCKADDR	(1<< 9)
#define VIP_REG_UPDATE_FLAG_DECIRHV			(1<<11)
#define VIP_REG_UPDATE_FLAG_DECIMATOR		(0x0000FF00)

CBOOL	VIP_UPDATE_OnInterrupt(int Module);
void	VIP_UPDATE_WaitForInterrupt(int Module);
void	VIP_UPDATE_SetScanMode(int Module, CBOOL bInterlace);
void	VIP_UPDATE_SetFlag(int Module, U32 Flag);
void	VIP_UPDATE_SetClipRegion(int Module, U32 Left, U32 Top, U32 Right, U32 Bottom);
void	VIP_UPDATE_SetClipperBlockAddr(int Module, U32 Format, CBOOL bRotation,
					U32 Width, U32 Height, U32 LuAddr, U32 CbAddr, U32 CrAddr);
void	VIP_UPDATE_SetClipperLinearAddr(int Module, U32 BaseAddr, U32 Stride);
void	VIP_UPDATE_SetClipperRHV(int Module, CBOOL bRotation,	CBOOL bHFlip, CBOOL bVFlip);
void	VIP_UPDATE_SetDecimation(int Module, U32 SrcWidth, U32 SrcHeight, U32 DstWidth, U32 DstHeight);
void 	VIP_UPDATE_SetDecimatorBlockAddr(int Module, U32 Format, CBOOL bRotation,
					 U32 Width, U32 Height, U32 LuAddr, U32 CbAddr, U32 CrAddr);
void	VIP_UPDATE_SetDecimatorRHV(int Module, CBOOL bRotation, CBOOL bHFlip, CBOOL bVFlip);

#endif	// __VIP_UPDATE_H__

