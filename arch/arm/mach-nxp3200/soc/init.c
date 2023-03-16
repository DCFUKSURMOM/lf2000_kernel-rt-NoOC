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
#include <linux/platform_device.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

/* nexell soc headers */
#include <mach/platform.h>

#if (0)
#define DBGOUT(msg...)		{ printk("soc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

typedef void (init_fnc_t) (void);
/*------------------------------------------------------------------------------
 * extern
 */
extern void soc_sys_call(void);
extern void soc_gpio_init(void);
extern void soc_dma_init(void);
extern void soc_adc_init(void);
extern void soc_dpc_init(void);
extern void soc_pwm_init(void);
extern void soc_wdt_init(void);

init_fnc_t * init_soc[] = {
	soc_gpio_init,
	soc_dma_init,
	soc_adc_init,
	soc_dpc_init,
	soc_pwm_init,
	soc_wdt_init,
	soc_sys_call,
	NULL,
};

/*-----------------------------------------------------------------------------*/
void __init cpu_soc_init(void)
{
	init_fnc_t **fnc_ptr;

	for (fnc_ptr = init_soc; *fnc_ptr; ++fnc_ptr) {
		DBGOUT("soc init func[0x%x]\n", *(uint *)fnc_ptr);
		(*fnc_ptr)();
	}
}
