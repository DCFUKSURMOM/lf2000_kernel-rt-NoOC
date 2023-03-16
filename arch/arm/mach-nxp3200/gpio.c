/*
 * (C) Copyright 2011
 * Daniel Lazzari Jr, LeapFrog Inc, <dlazzari@leapfrog.com>
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
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/lf2000/gpio.h>

//For now, use the Nexell supplied functions to hook up
//the framework functionality
#include "prototype/module/nx_gpio.h"
#include <mach/iomap.h>

static DEFINE_SPINLOCK(gpio_lock);

extern unsigned lf2000_gpio_l2p(struct gpio_chip* chip, unsigned offset);

static int lf2000_gpio_get_value (struct gpio_chip *chip, unsigned int offset)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	return (int)NX_GPIO_GetInputValue(mod, pin);
}

static void lf2000_gpio_set_value (struct gpio_chip *chip,
                                   unsigned int offset, int value)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	spin_lock(&gpio_lock);
	NX_GPIO_SetOutputValue(mod, pin, value);
	spin_unlock(&gpio_lock);
}

static int lf2000_gpio_direction_input (struct gpio_chip *chip, unsigned int offset)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	spin_lock(&gpio_lock);
	NX_GPIO_SetOutputEnable(mod, pin, 0);
	spin_unlock(&gpio_lock);
	return 0;
}

static int lf2000_gpio_direction_output (struct gpio_chip *chip,
                                         unsigned int offset, int value)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	spin_lock(&gpio_lock);
	NX_GPIO_SetOutputEnable(mod, pin, 1);
	NX_GPIO_SetOutputValue(mod, pin, value);
	spin_unlock(&gpio_lock);
	return 0;
}

static int lf2000_gpio_to_irq (struct gpio_chip *chip, unsigned int offset)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	return (pin + IRQ_GPIO_START + (mod<<5)); //Must match gpio_irq_handler
}

/*******************************************************************************
 * GPIO Framework extension
 */

static int lf2000_gpio_get_fn(struct gpio_chip* chip, unsigned offset)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	return NX_GPIO_GetPadFunction(mod, pin);
}

static int lf2000_gpio_set_fn(struct gpio_chip* chip, unsigned offset, unsigned function)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	NX_GPIO_SetPadFunction(mod, pin, function);
	return 0;
}

static int lf2000_gpio_get_pu(struct gpio_chip* chip, unsigned offset)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	return NX_GPIO_GetPullUpEnable(mod, pin);
}

static int lf2000_gpio_set_pu(struct gpio_chip* chip, unsigned offset, unsigned value)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	NX_GPIO_SetPullUpEnable(mod, pin, value);
	return 0;
}

#define LF2000_GPIO_CURRENT_BASE 0xC001E100

static int lf2000_gpio_get_cur(struct gpio_chip* chip, unsigned offset)
{
	unsigned int mod, pin, reg;
	unsigned long *ptr;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	
	reg = LF2000_GPIO_CURRENT_BASE + (mod * 8) + (pin > 15);
	ptr = (unsigned long*)IO_ADDRESS(reg);
	return (*ptr & ( 0x3 << (pin * 2) ) ) >> (pin * 2);
}

static int lf2000_gpio_set_cur(struct gpio_chip* chip, unsigned offset, unsigned curr)
{
	unsigned int mod, pin, reg, val, mask;
	unsigned long *ptr;
	mod  = LF2000_GPIO_PHYS_PORT(offset);
	pin  = offset & LF2000_GPIO_PIN_MASK;
	mask = ~(3 << (pin * 2) );
	reg  = LF2000_GPIO_CURRENT_BASE + (mod * 8) + (pin > 15);

	ptr  = (unsigned long*)IO_ADDRESS(reg);
	val  = (*ptr & mask) | (curr << (pin * 2) );
	*ptr = val;
	return 0;
}

/* We use 2 chips so that we can have lots of room for logical pins
 * and not reserve pins we aren't actually using. Note that the
 * logical chip is just a virtual mapping of the physical pins
 */
static struct gpio_chip lf2000_virtual_gpiochip = {
	.label			= "lf2000_virtual_gpio",
	.to_phys		= lf2000_gpio_l2p,
	.base			= 0,
	.ngpio			= GPIO_NUMBER_VALUES,
	.is_virtual		= 1,
};

static struct gpio_chip lf2000_physical_gpiochip = {
	.label			= "lf2000_physical_gpio",
	.direction_input	= lf2000_gpio_direction_input,
	.get			= lf2000_gpio_get_value,
	.direction_output	= lf2000_gpio_direction_output,
	.set			= lf2000_gpio_set_value,
	.to_irq			= lf2000_gpio_to_irq,
	.set_function		= lf2000_gpio_set_fn,
	.get_function		= lf2000_gpio_get_fn,
	.set_pullup		= lf2000_gpio_set_pu,
	.get_pullup		= lf2000_gpio_get_pu,
	.set_current		= lf2000_gpio_set_cur,
	.get_current		= lf2000_gpio_get_cur,
	.base			= LF2000_GPIO_PHYS,
	.ngpio			= 128,
	.can_sleep		= 0,
};

extern void lf2000_gpio_init_map(void);
extern void lf2000_gpio_check_didjfi(void); 

void __init lf2000_gpio_init(void)
{
	spin_lock_init(&gpio_lock);
	lf2000_gpio_init_map();
	gpiochip_add(&lf2000_virtual_gpiochip);
	gpiochip_add(&lf2000_physical_gpiochip);
	lf2000_gpio_check_didjfi();
}



