/* Common helper macros for the drivers.
 *
 * (C) Copyright 2012
 * LeapFrog Enterprises, Inc
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef LF1000_COMMON_H
#define LF1000_COMMON_H

/* bit masking */
#define BIT_SET(v,b)	(v |= (1<<(b)))
#define BIT_CLR(v,b)	(v &= ~(1<<(b)))
#define IS_SET(v,b)	(v & (1<<(b)))
#define IS_CLR(v,b)	!(v & (1<<(b)))
#define BIT_MASK_ONES(b) ((1<<(b))-1)

/*
 * make sure bootloaders have basic kernel-style types
 */
#ifndef u8
#define u8	unsigned char
#endif
#ifndef u16
#define u16     unsigned short
#endif
#ifndef u32
#define u32     unsigned int
#endif

#ifndef NULL
#define NULL	0
#endif

/* 
 * register helpers for bootloaders and the like
 */
#define REG8(addr)  *((volatile u8 *)(addr))
#define REG16(addr) *((volatile u16 *)(addr))
#define REG32(addr) *((volatile u32 *)(addr))

#endif
