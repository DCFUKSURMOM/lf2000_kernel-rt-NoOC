/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
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
#ifndef __VMEM_H__
#define __VMEM_H__

#include "fourcc.h"

/* memory data struct */
typedef struct {
	void		*	MemPoint;
	unsigned int	Flags;
	unsigned short 	HorAlign;
	unsigned short 	VerAlign;
	unsigned int 	MemWidth;
	unsigned int 	MemHeight;
	unsigned int 	Address;	/* physical address */
	unsigned int 	Virtual;	/* virtual  address */
} VM_IMEMORY;

/* video memory data struct */
typedef struct {
	VM_IMEMORY	LuMemory;
	VM_IMEMORY	CbMemory;
	VM_IMEMORY	CrMemory;
} VM_VMEMORY;

/* base memory info */
typedef struct {
	/* Linear memory base info */
	unsigned int LinPhyBase;
	unsigned int LinPhySize;
	unsigned int LinVirBase;
	unsigned int LinVirSize;
	/* Block memory base info */
	unsigned int BlkPhyBase;
	unsigned int BlkPhySize;
	unsigned int BlkVirBase;
	unsigned int BlkVirSize;
} VM_MEMBASE;

/* Alloc type */
#define VMEM_LINEAR_BUFFER   		0x00000000		/* Linear memory region */
#define VMEM_BLOCK_BUFFER    		0x00000001		/* Block  memory region */
#define VMEM_NOT_SEPARATED	   		0x00000010		/* Y,Cb,Cr memory region is sticking each other. */

/* Error code	*/
#define ERR_VMEM_NO		        ( 0)
#define ERR_VMEM_ERROR		 	(-1)
#define ERR_VMEM_ALLOC	        (-2)
#define ERR_VMEM_INVALID	    (-3)

/*-----------------------------------------------------------------------------
 *	IOCTL CODE
 */
#include "ioc_magic.h"

enum {
	IOCTL_VMEM_STATUS	= _IO(IOC_NX_MAGIC, 1),
	IOCTL_VMEM_RESET	= _IO(IOC_NX_MAGIC, 2),
	IOCTL_VMEM_INFO		= _IO(IOC_NX_MAGIC, 3),
	IOCTL_VMEM_ALLOC	= _IO(IOC_NX_MAGIC, 4),
	IOCTL_VMEM_FREE		= _IO(IOC_NX_MAGIC, 5),
	IOCTL_VMEM_VALLOC	= _IO(IOC_NX_MAGIC, 6),
	IOCTL_VMEM_VFREE	= _IO(IOC_NX_MAGIC, 7),
	IOCTL_VMEM_MAP		= _IO(IOC_NX_MAGIC, 8),	/* virtual mapping */
};

#endif /* __VMEM_H__ */
