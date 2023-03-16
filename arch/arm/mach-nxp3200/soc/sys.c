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
#include <linux/init.h>
#include <linux/types.h>
#include <linux/delay.h>

#if (0)
#define DBGOUT(msg...)		{ printk("sys : " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif
/*------------------------------------------------------------------------------
 * refer to "arch/arm/include/asm/call.S
 */
#define	__NR_cach_flush		223

extern long sys_call_table[];
extern asmlinkage long sys_ni_syscall(void);

asmlinkage long (*sys_org_syscall)(unsigned int, unsigned int, unsigned int);
asmlinkage long	sys_cacheflush (unsigned int start, unsigned int end, unsigned int opt);

/*----------------------------------------------------------------------------*/
void soc_sys_call(void)
{
	DBGOUT("%s\n", __func__);

	sys_org_syscall = (void*)sys_call_table[__NR_cach_flush];
	if (sys_org_syscall != (void*)sys_ni_syscall) {
		printk(KERN_ERR "%s: Fail register new syscall to sys[%d]\n",
			__func__, __NR_cach_flush);
		return;
	}
	printk(KERN_INFO "sys : register new syscall to sys[%d]\n",
			__NR_cach_flush);

	sys_call_table[__NR_cach_flush] = (long)sys_cacheflush;
}

/*----------------------------------------------------------------------------*/
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <linux/delay.h>

/* sys_cacheflush -- flush (part of) the processor cache.  */
asmlinkage long
sys_cacheflush (unsigned int start, unsigned int end, unsigned int opt)
{
	u_long flags;
	DBGOUT("%s: sa=0x%08x, ea=0x%08x, op=0x%08x)\n", __func__, start, end, opt);

	local_irq_save(flags);

	flush_cache_all();

	local_irq_restore(flags);
	return 0;
}


