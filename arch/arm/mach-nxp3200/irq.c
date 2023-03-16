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
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>

#include <asm/mach/irq.h>

/* nexell soc headers */
#include <mach/platform.h>

#if (0)
#define IRQ_DBGOUT(msg...)		{ printk(KERN_INFO "irq: " msg); }
#else
#define IRQ_DBGOUT(msg...)		do {} while (0)
#endif	/* irq debug */

#if (0)
#define PIO_DBGOUT(msg...)		{ printk(KERN_INFO "pio: " msg); }
#else
#define PIO_DBGOUT(msg...)		do {} while (0)
#endif 	/* gpio irq debug */

#if (0)
#define DMA_DBGOUT(msg...)		{ printk(KERN_INFO "dma: " msg); }
#else
#define DMA_DBGOUT(msg...)		do {} while (0)
#endif 	/* dma irq debug */

#if (0)
#define ALV_DBGOUT(msg...)		{ printk(KERN_INFO "alv: " msg); }
#else
#define ALV_DBGOUT(msg...)		do {} while (0)
#endif 	/* Alive irq debug */


/* local */
static void __init gpio_init_irq(void);
static void __init dma_init_irq(void);
static void __init alive_init_irq(void);

/*----------------------------------------------------------------------------
 *  irq handler
 *  start  -> enable
 *  do IRQ -> mask_ack -> handler -> unmask->
 *  do IRQ -> mask_ack -> handler -> unmask-> ...
 *  end    -> disable
 ----------------------------------------------------------------------------*/
#define	REG_INTBASE	IO_ADDRESS(PHY_BASEADDR_INTC_MODULE)

#define REG_INTMODE_0		0x08	/* Interrupt Mode Register */
#define REG_INTMODE_1		0x0C 	/* Interrupt Mode Register */
#define REG_INTMASK_0		0x10	/* Interrupt Mask Register */
#define REG_INTMASK_1		0x14 	/* Interrupt Mask Register */
#define REG_PRIORDER		0x18 	/* Priority Order Register */
#define REG_INTPEND_0		0x20	/* Interrupt Pending Register */
#define REG_INTPEND_1		0x24 	/* Interrupt Pending Register */

inline static void	soc_int_set_enable(int num)
{
	volatile u_int *reg = (u_int*)(REG_INTBASE + REG_INTMASK_0);
	 reg += num >> 5;
	*reg &= ~(1 << (num & 0x1f));
}

inline static void	soc_int_set_disable(int num)
{
	volatile u_int *reg = (u_int*)(REG_INTBASE + REG_INTMASK_0);
	 reg += num >> 5;
	*reg |= (1 << (num & 0x1f));
}

inline static void	soc_int_clr_pend(int num)
{
	volatile u_int *reg = (u_int*)(REG_INTBASE + REG_INTPEND_0);
	 reg += num >> 5;
	*reg  = 1UL << (num & 0x1f);
}

inline static int	soc_int_get_pend(int num)
{
	volatile u_int *reg = (u_int*)(REG_INTBASE + REG_INTPEND_0);
	reg += num >> 5;
	return (*reg >> num) & 1UL;
}

/*------------------------------------------------------------------------*/
static void cpu_irq_enable(struct irq_data *d)
{
	IRQ_DBGOUT("enable(cpu_irq:%d)\n", d->irq);
#if (1)
	NX_INTC_ClearInterruptPending(d->irq);
	NX_INTC_SetInterruptEnable(d->irq, CTRUE);
	NX_INTC_GetInterruptPending(d->irq);
#else
	soc_int_clr_pend(d->irq); 		/* intc : clear irq pend */
	soc_int_set_enable(d->irq);		/* intc : enable irq */
	soc_int_get_pend(d->irq);		/* Guarantee that irq Pending is cleared */
#endif
}

static void cpu_irq_disable(struct irq_data *d)
{
	IRQ_DBGOUT("disable(cpu_irq:%d)\n", d->irq);
#if (1)
	NX_INTC_ClearInterruptPending(d->irq);
	NX_INTC_SetInterruptEnable(d->irq, CFALSE);
	NX_INTC_GetInterruptPending(d->irq);
#else
	soc_int_clr_pend(d->irq);	 	/* intc : clear irq pend */
	soc_int_set_disable(d->irq);		/* intc : disable irq */
	soc_int_get_pend(d->irq);		/* Guarantee that irq Pending is cleared */
#endif
}

/* disable irq: set mask bit & clear irq pend bit */
static void cpu_irq_mask_ack(struct irq_data *d)
{
	IRQ_DBGOUT("mask_ack(irq:%d)\n", d->irq);
#if (1)
	NX_INTC_ClearInterruptPending(d->irq);
	NX_INTC_SetInterruptEnable(d->irq, CFALSE);
	NX_INTC_GetInterruptPending(d->irq);
#else
	soc_int_clr_pend(d->irq);		/* intc : clear irq pend */
	soc_int_set_disable(d->irq);		/* intc : disable irq */
	soc_int_get_pend(d->irq);		/* Guarantee that irq Pending is cleared */
#endif
}

/* enable irq: set unmask bit & clear irq pend bit */
static void cpu_irq_unmask(struct irq_data *d)
{
	IRQ_DBGOUT("unmask(irq:%d)\n", d->irq);
#if (1)
	NX_INTC_ClearInterruptPending(d->irq);
	NX_INTC_SetInterruptEnable(d->irq, CTRUE);
	NX_INTC_GetInterruptPending(d->irq);
#else
	soc_int_clr_pend(d->irq);		/* intc : clear irq pend */
	soc_int_set_enable(d->irq);		/* intc : enable irq */
	soc_int_get_pend(d->irq);		/* Guarantee that irq Pending is cleared */
#endif
}

/* interrupt controller irq control */
static struct irq_chip cpu_irq_chip = {
	.name		= "IRQ",
	.irq_enable	= cpu_irq_enable,
	.irq_disable	= cpu_irq_disable,
	.irq_mask_ack	= cpu_irq_mask_ack,
	.irq_unmask	= cpu_irq_unmask,
	.irq_mask	= cpu_irq_disable,
};

void __init cpu_init_irq(void)
{
	int i = 0;
	IRQ_DBGOUT("%s\n", __func__);

	/* setup physical interrupt */
	for (i = 0; IRQ_PHY_MAX_COUNT > i; i++) {
		irq_set_chip_and_handler(i, &cpu_irq_chip, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);
	}
	gpio_init_irq();
	dma_init_irq();
	alive_init_irq();
}

/*----------------------------------------------------------------------------
 *  GPIO irq chain handler
 *  start  -> enable
 *  do IRQ -> gpio handler -> mask_ack -> handler -> unmask->
 *  do IRQ -> gpio handler -> mask_ack -> handler -> unmask->  ...
 *  end    -> disable
 ----------------------------------------------------------------------------*/
static void gpio_irq_enable(struct irq_data *d)
{
	int irq = IRQ_PHY_GPIO;
	int mod = (d->irq - IRQ_GPIO_START) >> 5;
	int bit = (d->irq - IRQ_GPIO_START) & 0x1F;

	PIO_DBGOUT("enable(gpio_irq:%d, grp:%d, bit:%d)\n", d->irq, mod, bit);

	NX_GPIO_ClearInterruptPending(mod, bit);		/* ack   : gpio clear pend */
	NX_INTC_ClearInterruptPending(irq);				/* ack   : intc clear irq pend */

	NX_GPIO_SetInterruptEnable(mod, bit, CTRUE);	/* unmask: gpio enable irq */

	NX_INTC_GetInterruptPending(irq);				/* Guarantee that irq Pending is cleared */
}

static void gpio_irq_disable(struct irq_data *d)
{
	int irq = IRQ_PHY_GPIO;
	int mod = (d->irq - IRQ_GPIO_START) >> 5;
	int bit = (d->irq - IRQ_GPIO_START) & 0x1F;

	PIO_DBGOUT("disable(gpio_irq:%d, grp:%d, bit:%d)\n", d->irq, mod, bit);

	NX_GPIO_ClearInterruptPending(mod, bit);		/* ack   : gpio clear pend */
	NX_INTC_ClearInterruptPending(irq);				/* ack : intc clear irq pend */

	NX_GPIO_SetInterruptEnable(mod, bit, CFALSE);	/* mask: gpio disalbe irq */

	NX_INTC_GetInterruptPending(irq);				/* Guarantee that irq Pending is cleared */
}

/* disable irq: set mask bit & clear irq pend bit */
static void gpio_irq_mask_ack(struct irq_data *d)
{
	int irq = IRQ_PHY_GPIO;
	int mod = (d->irq - IRQ_GPIO_START) >> 5;
	int bit = (d->irq - IRQ_GPIO_START) & 0x1F;

	PIO_DBGOUT("mask_ack(gpio_irq:%d, grp:%d, bit:%d)\n", d->irq, mod, bit);

	NX_GPIO_ClearInterruptPending(mod, bit);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);				/* ack : intc clear irq pend */

	NX_GPIO_SetInterruptEnable(mod, bit, CFALSE);	/* mask: gpio enable irq */

	NX_INTC_GetInterruptPending(irq);				/* check : Guarantee that irq Pending is cleared */
}

/* enable irq: set unmask bit & clear irq pend bit */
static void gpio_irq_unmask(struct irq_data *d)
{
	int irq = IRQ_PHY_GPIO;
	int mod = (d->irq - IRQ_GPIO_START) >> 5;
	int bit = (d->irq - IRQ_GPIO_START) & 0x1F;

	PIO_DBGOUT("unmask(gpio_irq:%d, grp:%d, bit:%d)\n", d->irq, mod, bit);

	NX_GPIO_ClearInterruptPending(mod, bit);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);				/* ack : intc clear irq pend */

	NX_GPIO_SetInterruptEnable(mod, bit, CTRUE);	/* unmask: gpio enable irq */

	NX_INTC_GetInterruptPending(irq);				/* check : Guarantee that irq Pending is cleared */
}

static int gpio_irq_set_type(struct irq_data *d, unsigned int flow_type)
{
	int mod = (d->irq - IRQ_GPIO_START) >> 5;
	int bit = (d->irq - IRQ_GPIO_START) & 0x1F;
	NX_GPIO_INTMODE desired_mode = 0;
	
	switch(flow_type)
	{
		case IRQF_TRIGGER_NONE:
			return 0;	//There is no hw equivelant, so ignore
		case IRQF_TRIGGER_RISING:
			desired_mode = NX_GPIO_INTMODE_RISINGEDGE;
			break;
		case IRQF_TRIGGER_FALLING:
			desired_mode = NX_GPIO_INTMODE_FALLINGEDGE;
			break;
		case IRQF_TRIGGER_HIGH:
			desired_mode = NX_GPIO_INTMODE_HIGHLEVEL;
			break;
		case IRQF_TRIGGER_LOW:
			desired_mode = NX_GPIO_INTMODE_LOWLEVEL;
			break;
		case IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING:
			desired_mode = NX_GPIO_INTMODE_RISINGNFALLINGEDGE;
			break;
		default:
			return -EINVAL;
	}
	NX_GPIO_SetInterruptMode(mod, bit, desired_mode);
	return 0;
}

static struct irq_chip gpio_irq_chip = {
	.name			= "Gpio IRQ",
	.irq_enable		= gpio_irq_enable,
	.irq_disable	= gpio_irq_disable,
	.irq_mask_ack	= gpio_irq_mask_ack,
	.irq_unmask		= gpio_irq_unmask,
	.irq_set_type	= gpio_irq_set_type,
};

static void gpio_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	int  mod = 0, bit = 0, irqnum = 0;
	int  ret = -1;
	uint pnd =  0;

	struct irq_desc *irqdesc;
	unsigned long flags  = 0;
	unsigned long detmod = 0;

	PIO_DBGOUT("gpio_irq_handle(irq:%d)\n", irq);

	for (mod = 0; NX_GPIO_GetNumberOfModule() > mod; mod++)	{

		bit = NX_GPIO_GetInterruptPendingNumber(mod);		/* masked pend bit */
		pnd = *(U32*)(NX_GPIO_GetBaseAddress(mod) + 0x14);	/* pend bit */

		if (-1 == bit && ! pnd)
			continue;

		if (-1 == bit && pnd) {
			for(bit = 0 ; 32 > bit ; bit++ )
				if(0 != (pnd & (1UL<<bit)))
					break;
		}

		irqnum  = (bit + IRQ_GPIO_START + (mod<<5));
		irqdesc = irq_desc + irqnum;

		if (irqdesc && irqdesc->action)	{

			flags  = irqdesc->action->flags;
			detmod = NX_GPIO_GetInterruptMode(mod, bit);

			/* disable shared irq when level detect */
			if (!(detmod & 0x2))
				irqdesc->action->flags |= IRQF_DISABLED;

			generic_handle_irq(irqnum);

			irqdesc->action->flags = flags;
		} else {
			printk(KERN_ERR "Fail, not registered gpio irq:%d, mod:%d, bit:%d \n",
				irq, mod, bit);
		}

		/* success & exit */
		ret = 0;
		break;
	}

	PIO_DBGOUT("handle(irq:%d-irqnum:%d), ret(%d)\n", irq, irqnum, ret);

	if (-1 == ret) {
		printk(KERN_ERR "Unknown pio irq(A:0x%x-0x%x, B:0x%x-0x%x, C:0x%x-0x%x)\n",
			*(U32*)(NX_GPIO_GetBaseAddress(0) + 0x14), NX_GPIO_GetInterruptEnable32(0),
			*(U32*)(NX_GPIO_GetBaseAddress(1) + 0x14), NX_GPIO_GetInterruptEnable32(1),
			*(U32*)(NX_GPIO_GetBaseAddress(2) + 0x14), NX_GPIO_GetInterruptEnable32(2));

		for (mod = 0; NX_GPIO_GetNumberOfModule() > mod; mod++)
			NX_GPIO_ClearInterruptPendingAll(mod);

		NX_INTC_GetInterruptPending(IRQ_PHY_GPIO);			/* check : Guarantee that irq Pending is cleared */
	}
}


/*
 * This lock class tells lockdep that GPIO irqs are in a different
 * category than their parents, so it won't report false recursion.
 */

static struct lock_class_key gpio_lock_class;

static void __init gpio_init_irq(void)
{
	int i = 0, irq = IRQ_PHY_GPIO;
	
	/* IRQ chips that handle edge irqs need to have an ack method */
	gpio_irq_chip.irq_ack = dummy_irq_chip.irq_ack;

	/* regist virtual gpio irq info */
	for (i = IRQ_GPIO_START; IRQ_GPIO_END > i; i++) {
		irq_set_lockdep_class(i, &gpio_lock_class);

		irq_set_chip_and_handler(i, &gpio_irq_chip, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);
	}

	/* register physical gpio irq handler, shared irq handler*/
	irq_set_chained_handler(irq, gpio_irq_handler);

	NX_INTC_ClearInterruptPending(irq);
	NX_INTC_SetInterruptEnable(irq, CTRUE);
}


/*----------------------------------------------------------------------------
 *  DMA irq chain handler
 *  start  -> enable
 *  do IRQ -> dma handler -> mask_ack -> handler -> unmask->
 *  do IRQ -> dma handler -> mask_ack -> handler -> unmask->  ...
 *  end    -> disable
 ----------------------------------------------------------------------------*/
static void dma_irq_enable(struct irq_data *d)
{
	int irq = IRQ_PHY_DMA;
	int ch  = (d->irq - IRQ_DMA_START);

	DMA_DBGOUT("enable(dma_irq:%d, ch:%d)\n", d->irq, ch);

	NX_DMA_ClearInterruptPendingAll(ch);		/* ack   : dma clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* ack   : intc clear irq pend */

	NX_DMA_SetInterruptEnableAll(ch, CTRUE);	/* unmask: enable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

static void dma_irq_disable(struct irq_data *d)
{
	int irq = IRQ_PHY_DMA;
	int ch  = (d->irq - IRQ_DMA_START);

	DMA_DBGOUT("disable(dma_irq:%d, ch:%d)\n", d->irq, ch);

	NX_DMA_ClearInterruptPendingAll(ch);		/* ack  : dma clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* ack  : intc clear irq pend */

	NX_DMA_SetInterruptEnableAll(ch, CFALSE);	/* mask : dma disable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

/* disable irq: set mask bit & clear irq pend bit */
static void dma_irq_mask_ack(struct irq_data *d)
{
	int irq = IRQ_PHY_DMA;
	int ch  = (d->irq - IRQ_DMA_START);

	DMA_DBGOUT("mask_ack(dma_irq:%d, ch:%d)\n", d->irq, ch);

	NX_DMA_ClearInterruptPendingAll(ch);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* intc : clear irq pend */

	NX_DMA_SetInterruptEnableAll(ch, CFALSE);	/* mask: dma disable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

/* enable irq: set unmask bit & clear irq pend bit */
static void dma_irq_unmask(struct irq_data *d)
{
	int irq = IRQ_PHY_DMA;
	int ch  = (d->irq - IRQ_DMA_START);

	DMA_DBGOUT("unmask(dma_irq:%d, ch:%d)\n", d->irq, ch);

	NX_DMA_ClearInterruptPendingAll(ch);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* intc : clear irq pend */

	NX_DMA_SetInterruptEnableAll(ch, CTRUE);	/* unmask: dma enable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

static struct irq_chip dma_irq_chip = {
	.name		= "DMA IRQ",
	.irq_enable	= dma_irq_enable,
	.irq_disable	= dma_irq_disable,
	.irq_mask_ack	= dma_irq_mask_ack,
	.irq_unmask	= dma_irq_unmask,
};

static void dma_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	int  ch = 0, irqnum = 0;
	int  ret = -1;

	struct irq_desc *irqdesc;

	for (ch = 0; NX_DMA_GetNumberOfModule() > ch; ch++) {

		if (NX_DMA_GetInterruptPendingAll(ch)) {

			irqnum  = (ch + IRQ_DMA_START);
			irqdesc = irq_desc + irqnum;

			if (irqdesc && irqdesc->action)
				irqdesc->handle_irq(irqnum, irqdesc);
			else
				printk(KERN_ERR "Error, unknown dma irq(%d) \n", irq);

			/* success & exit */
			ret = 0;
			break;
		}
	}

	DMA_DBGOUT("dma_irq_handle(irq:%d-irqnum:%d), ret(%d)\n", irq, irqnum, ret);

	if (-1 == ret) {
		printk(KERN_ERR "Unknown dma irq(%d)\r\n", irq);

		for (ch = 0; NX_DMA_GetNumberOfModule() > ch; ch++)
			NX_DMA_ClearInterruptPendingAll(ch);

		NX_INTC_ClearInterruptPending(IRQ_PHY_DMA);		/* ack   : intc clear irq pend */
		NX_INTC_GetInterruptPending(IRQ_PHY_DMA);		/* Guarantee that irq Pending is cleared */
	}
}

static struct lock_class_key dma_lock_class;

static void __init dma_init_irq(void)
{
	int i = 0, irq = IRQ_PHY_DMA;

	/* register virtual dma irq info */
	for (i = IRQ_DMA_START; IRQ_DMA_END > i; i++) {
		irq_set_lockdep_class(i, &dma_lock_class);
		irq_set_chip_and_handler(i, &dma_irq_chip, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);
	}

	/* register physical dma irq handler, shared irq handler*/
	irq_set_chained_handler(irq, dma_irq_handler);

	NX_INTC_ClearInterruptPending(irq);
	NX_INTC_SetInterruptEnable(irq, CTRUE);
}

/*----------------------------------------------------------------------------
 *  ALIVE irq chain handler
 *  start  -> enable
 *  do IRQ -> alive handler -> mask_ack -> handler -> unmask->
 *  do IRQ -> alive handler -> mask_ack -> handler -> unmask->  ...
 *  end    -> disable
 ----------------------------------------------------------------------------*/
static void alive_irq_enable(struct irq_data *d)
{
	int irq = IRQ_PHY_ALIVE;
	int bit = (d->irq - IRQ_ALIVE_START);

	ALV_DBGOUT("alive_irq_enable(alive_irq:%d, ch:%d)\n", d->irq, bit);

	NX_ALIVE_ClearInterruptPending(bit);		/* ack   : alive clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* ack   : intc clear irq pend */

	NX_ALIVE_SetInterruptEnable(bit, CTRUE);	/* unmask: enable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

static void alive_irq_disable(struct irq_data *d)
{
	int bit = (d->irq - IRQ_ALIVE_START);
	int irq = IRQ_PHY_ALIVE;

	ALV_DBGOUT("alive_irq_disable(irq:%d, ch:%d)\n", d->irq, bit);

	NX_ALIVE_ClearInterruptPending(bit);		/* ack  : alive clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* ack  : intc clear irq pend */

	NX_ALIVE_SetInterruptEnable(bit, CFALSE);	/* mask : alive disable irq */

	NX_INTC_GetInterruptPending(irq);		/* Guarantee that irq Pending is cleared */
}

/* disable irq: set mask bit & clear irq pend bit */
static void alive_irq_mask_ack(struct irq_data *d)
{
	int irq = IRQ_PHY_ALIVE;
	int bit = (d->irq - IRQ_ALIVE_START);

	ALV_DBGOUT("alive_irq_mask_ack(irq:%d, ch:%d)\n", d->irq, bit);

	NX_ALIVE_ClearInterruptPending(bit);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);		/* intc : clear irq pend */

	NX_ALIVE_SetInterruptEnable(bit, CFALSE);	/* mask: alive disable irq */

	NX_INTC_GetInterruptPending(irq);		/* Guarantee that irq Pending is cleared */
}

/* enable irq: set unmask bit & clear irq pend bit */
static void alive_irq_unmask(struct irq_data *d)
{
	int irq = IRQ_PHY_ALIVE;
	int bit = (d->irq - IRQ_ALIVE_START);

	ALV_DBGOUT("alive_irq_unmask(irq:%d, ch:%d)\n", d->irq, bit);

	NX_ALIVE_ClearInterruptPending(bit);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);		/* intc : clear irq pend */

	NX_ALIVE_SetInterruptEnable(bit, CTRUE);	/* unmask: alive enable irq */

	NX_INTC_GetInterruptPending(irq);		/* Guarantee that irq Pending is cleared */
}

static struct irq_chip alive_irq_chip = {
	.name		= "Alive IRQ",
	.irq_enable	= alive_irq_enable,
	.irq_disable	= alive_irq_disable,
	.irq_mask_ack	= alive_irq_mask_ack,
	.irq_unmask	= alive_irq_unmask,
};

static void alive_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	int  bit = 0, irqnum = 0;
	int  ret = -1;

	struct irq_desc *irqdesc;

	bit = NX_ALIVE_GetInterruptPendingNumber();

	if (bit != -1) {

		irqnum  = (bit + IRQ_ALIVE_START);
		irqdesc = irq_desc + irqnum;

		if (irqdesc && irqdesc->action)
			irqdesc->handle_irq(irqnum, irqdesc);
		else
			printk(KERN_ERR "Error, unknown alive irq(%d) \n", irqnum);

		/* success */
		ret = 0;
	}

	ALV_DBGOUT("alive_irq_handler(irq:%d-irqnum:%d), ret(%d)\n", irq, irqnum, ret);

	if (-1 == ret) {
		printk(KERN_ERR "Unknown alive irq(%d)\r\n", irq);
		/* check : Guarantee that irq Pending is cleared */
		NX_INTC_GetInterruptPending(IRQ_PHY_ALIVE);
	}
}

static void __init alive_init_irq(void)
{
	int i = 0, irq = IRQ_PHY_ALIVE;

	/* register virtual alive irq info */
	for (i = IRQ_ALIVE_START; IRQ_ALIVE_END > i; i++) {
		irq_set_chip_and_handler(i, &alive_irq_chip, handle_level_irq);
		set_irq_flags (i, IRQF_VALID);
	}

	/* register physical alive irq handler, shared irq handler*/
	irq_set_chained_handler(irq, alive_irq_handler);

	NX_INTC_ClearInterruptPending(irq);
	NX_INTC_SetInterruptEnable(irq, CTRUE);
}
