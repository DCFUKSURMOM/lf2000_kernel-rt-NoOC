#include <linux/slab.h>
#include <linux/kernel.h>
#include "nx_type.h"

static int AllocCount = 0;

void *NxMemAlloc( unsigned int size )
{
	void *ptr;
	AllocCount++;
	ptr = kmalloc(size, GFP_KERNEL);
//	printk("[AllocCount A = %03d, 0x%08x]\n", AllocCount, (unsigned int)ptr);
	return ptr;
}

void *NxMemCalloc(unsigned int num, unsigned int typeSize)
{
	void *ptr;
	AllocCount++;
	ptr = kmalloc(num*typeSize, GFP_KERNEL);
	if( ptr ){
		memset( ptr, 0, num*typeSize );
	}
	return ptr;
}

void NxMemFree( void *ptr )
{
	if( 0 != ptr ){
//		printk("[AllocCount F = %03d, 0x%08x]\n", AllocCount, (unsigned int)ptr);
		AllocCount --;
		kfree( ptr );
	}
}


void NxMemSet( void *ptr, int data, unsigned int size )
{
	memset( ptr, data, size );
}

void NxMemCpy( void *dst, void *src, unsigned int size )
{
	memcpy( dst, src, size);
}
