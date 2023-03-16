/* linux/drivers/i2c/busses/i2c-nexell_proto.c
 *
 * Scott Esters <sesters@leapfrog.com>
 * Copyright (C) 2012 LeapFrog Enterprises
 *
 * NXP3200 I2C Controller, patterned after the S3C2410 I2C Controller driver
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

#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of_i2c.h>
#include <linux/of_gpio.h>

#include <asm/irq.h>

// sesters: remove Samsung-specific code as part of conversion from S3C24XX to NXP3200
#undef S3C24XX_I2C_DRIVER

#ifdef S3C24XX_I2C_DRIVER
#include <plat/regs-iic.h>
#include <plat/iic.h>
#else
#include <asm/system_info.h>
#include <linux/lf2000/gpio.h>
#include <mach/regs-iic.h>
#include <mach/devices.h>
#include <mach/nxp3200.h>
#include <mach/soc.h>
#include <mach/iomap.h>
#include <mach/board_revisions.h>

#endif

/* i2c controller state */

enum nxp3200_i2c_state {
	STATE_IDLE,
	STATE_START,
	STATE_READ,
	STATE_WRITE,
	STATE_STOP
};

#ifdef S3C24XX_I2C_DRIVER
enum s3c24xx_i2c_type {
	TYPE_S3C2410,
	TYPE_S3C2440,
};
#endif

struct nxp3200_i2c {
	spinlock_t		lock;
	wait_queue_head_t	wait;
	unsigned int		suspended:1;

	struct i2c_msg		*msg;
	unsigned int		msg_num;
	unsigned int		msg_idx;
	unsigned int		msg_ptr;

	unsigned int		tx_setup;
	unsigned int		irq;

	enum nxp3200_i2c_state	state;
	unsigned long		clkrate;

	void __iomem		*regs;
	struct clk		*clk;
	struct device		*dev;
	struct resource		*ioarea;
	struct i2c_adapter	adap;

	struct i2c_plat_data	*pdata;
	int			gpios[2];
#ifdef CONFIG_CPU_FREQ
	struct notifier_block	freq_transition;
#endif
};

#ifndef S3C24XX_I2C_DRIVER
static void i2c_dump_gpio(unsigned offset)
{
	unsigned int mod, pin;
	mod = LF2000_GPIO_PHYS_PORT(offset);
	pin = offset & LF2000_GPIO_PIN_MASK;
	printk(KERN_INFO "port:0x%X, pin:0x%X, function:0x%X, direction:0x%X, pullup:0x%X, value:0x%X\n",
		mod, pin,
		NX_GPIO_GetPadFunction(mod,pin),
		NX_GPIO_GetOutputEnable(mod,pin),
		NX_GPIO_GetPullUpEnable(mod, pin),
		NX_GPIO_GetInputValue(mod, pin)
		);
}

static void i2c_dump_registers(struct nxp3200_i2c *i2c, char *function, int line)
{
	printk(KERN_INFO "%s.%d register dump  port:%d, regs:0x%p\n",
		function, line, i2c->pdata->port, i2c->regs);
	printk(KERN_INFO "NXP3200_IICCON      0x00:0x%8.8X\n",readl(i2c->regs + NXP3200_IICCON));
	printk(KERN_INFO "NXP3200_IICSTAT     0x04:0x%8.8X\n",readl(i2c->regs + NXP3200_IICSTAT));
	printk(KERN_INFO "NXP3200_IICADD      0x08:0x%8.8X\n",readl(i2c->regs + NXP3200_IICADD));
	printk(KERN_INFO "NXP3200_IICDS       0x0c:0x%8.8X\n",readl(i2c->regs + NXP3200_IICDS));
	printk(KERN_INFO "NXP3200_IICIRQPEND  0x24:0x%8.8X\n",readl(i2c->regs + NXP3200_IICIRQPEND));
	printk(KERN_INFO "NXP3200_IICCLKENB  0x100:0x%8.8X\n",readl(i2c->regs + NXP3200_IICCLKENB));
}

extern unsigned lf2000_gpio_l2p(struct gpio_chip* chip, unsigned offset);

static void	i2c_set_io_func(struct nxp3200_i2c * i2c)
{
	int	  func = NX_GPIO_PADFUNC_1;
	u_int scl  = lf2000_gpio_l2p(NULL,i2c->pdata->logical_io_scl) - LF2000_GPIO_PHYS;
	u_int sda  = lf2000_gpio_l2p(NULL,i2c->pdata->logical_io_sda) - LF2000_GPIO_PHYS;

	dev_dbg(i2c->dev,"%s (scl:%d, sda:%d, func:%d)\n", __func__, scl, sda, func);

	/*  set scl pad */
	if (func != soc_gpio_get_io_func(scl)) {
		soc_gpio_set_io_func(scl, func);
		soc_gpio_set_io_pullup(scl, 1);
	}

	/*  set sda pad */
	if (func != soc_gpio_get_io_func(sda)) {
		soc_gpio_set_io_func(sda, func);
		soc_gpio_set_io_pullup(sda, 1);
	}
}

static inline void	i2c_set_clock(struct nxp3200_i2c *i2c, bool enable)
{
	uint	tmp;

	dev_dbg(i2c->dev, "%s(port:%d %s)\n",
		__func__, i2c->pdata->port, enable ? "on" : "off");

	tmp = readl(i2c->regs + NXP3200_IICCLKENB);

	if (enable) {
		writel(tmp | NXP3200_IICCLKENB_PCLKMODE, i2c->regs + NXP3200_IICCLKENB);
	} else {
		writel(tmp & ~NXP3200_IICCLKENB_PCLKMODE, i2c->regs + NXP3200_IICCLKENB);
	}
	//i2c_dump_registers(i2c, __FUNCTION__, __LINE__);
}

static int i2c_init_func(struct nxp3200_i2c *i2c)
{

	/* RIO wants to run Neonode I2C channel at 100k BPS */
	if (i2c->pdata->port == 0) {
		switch (system_rev) {
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
			/* On Neonode port */
			i2c->pdata->clkscale = CFG_I2C0_CLOCK_SCALER_RIO;
			/* max Neonode port speed is 100 KHz */
			i2c->pdata->frequency = 100000;
			break;
		default:
			break;
		}
	}

	//sesters debugging
	printk(KERN_INFO "%s.%s.%d  i2c->pdata->port:%d, i2c->pdata->clkscale:%d, i2c->pdata->clksrc:%d, i2c->pdata->cntmax:%d",
		__FILE__, __FUNCTION__, __LINE__, i2c->pdata->port, i2c->pdata->clkscale, i2c->pdata->clksrc, i2c->pdata->cntmax);

	return 0;
}
#endif

#ifdef S3C24XX_I2C_DRIVER
/* default platform data removed, dev should always carry data. */

/* s3c24xx_i2c_is2440()
 *
 * return true is this is an s3c2440
*/

static inline int s3c24xx_i2c_is2440(struct s3c24xx_i2c *i2c)
{
	struct platform_device *pdev = to_platform_device(i2c->dev);
	enum s3c24xx_i2c_type type;

#ifdef CONFIG_OF
	if (i2c->dev->of_node)
		return of_device_is_compatible(i2c->dev->of_node,
				"samsung,s3c2440-i2c");
#endif

	type = platform_get_device_id(pdev)->driver_data;
	return type == TYPE_S3C2440;
}
#endif

/* nxp3200_i2c_master_complete
 *
 * complete the message and wake up the caller, using the given return code,
 * or zero to mean ok.
*/

static inline void nxp3200_i2c_master_complete(struct nxp3200_i2c *i2c, int ret)
{
	dev_dbg(i2c->dev, "master_complete %d\n", ret);

	i2c->msg_ptr = 0;
	i2c->msg = NULL;
	i2c->msg_idx++;
	i2c->msg_num = 0;
	if (ret)
		i2c->msg_idx = ret;

	wake_up(&i2c->wait);
}

static inline void nxp3200_i2c_disable_ack(struct nxp3200_i2c *i2c)
{
	unsigned long tmp;

	tmp = readl(i2c->regs + NXP3200_IICCON);
	writel(tmp & ~NXP3200_IICCON_ACKEN, i2c->regs + NXP3200_IICCON);
}

static inline void nxp3200_i2c_enable_ack(struct nxp3200_i2c *i2c)
{
	unsigned long tmp;

	tmp = readl(i2c->regs + NXP3200_IICCON);
	writel(tmp | NXP3200_IICCON_ACKEN, i2c->regs + NXP3200_IICCON);
}

/* irq enable/disable functions */

static inline void nxp3200_i2c_disable_irq(struct nxp3200_i2c *i2c)
{
	unsigned long tmp;

	tmp = readl(i2c->regs + NXP3200_IICCON);
	writel(tmp & ~NXP3200_IICCON_IRQEN, i2c->regs + NXP3200_IICCON);
}

static inline void nxp3200_i2c_enable_irq(struct nxp3200_i2c *i2c)
{
	unsigned long tmp;

	tmp = readl(i2c->regs + NXP3200_IICCON);
	writel(tmp | NXP3200_IICCON_IRQEN, i2c->regs + NXP3200_IICCON);
}

static inline void nxp3200_i2c_ClearOperationHold(struct nxp3200_i2c *i2c)
{
	writel(NXP3200_IICIRQPEND_HOLD, i2c->regs + NXP3200_IICIRQPEND);
}

static inline void nxp3200_i2c_ClearInterruptPendingAll(struct nxp3200_i2c *i2c)
{
	writel(NXP3200_IICIRQPEND_PEND, i2c->regs + NXP3200_IICIRQPEND);
}

/* nxp3200_i2c_message_start
 *
 * put the start of a message onto the bus
*/

static void nxp3200_i2c_message_start(struct nxp3200_i2c *i2c,
				      struct i2c_msg *msg)
{
	unsigned int addr = (msg->addr & 0x7f) << 1;
	unsigned long stat, temp;
	unsigned long iiccon;

#ifndef S3C24XX_I2C_DRIVER
	stat = readl(i2c->regs + NXP3200_IICSTAT) & 0x1F0F;
#else
	stat = 0;
#endif
	if (msg->flags & I2C_M_RD) {
		stat |= NXP3200_IICSTAT_MASTER_RX;
		//nxp3200_i2c_disable_ack(i2c);
		nxp3200_i2c_enable_ack(i2c);
		addr |= 1;
	} else {
		stat |= NXP3200_IICSTAT_MASTER_TX;
		nxp3200_i2c_enable_ack(i2c);
	}

	if (msg->flags & I2C_M_REV_DIR_ADDR)
		addr ^= 1;

	/* clear irq condition */
	nxp3200_i2c_ClearInterruptPendingAll(i2c);

	//iiccon = readl(i2c->regs + NXP3200_IICCON);
	// sesters: try to clear busy condition, leave commented out
	writel(0x1010, i2c->regs + NXP3200_IICSTAT);

//	i2c_dump_registers(i2c, __FUNCTION__, __LINE__);
	writel(addr, i2c->regs + NXP3200_IICDS);


	stat |=  (NXP3200_IICSTAT_TXRXEN | NXP3200_IICSTAT_ST_ENB);

	/* delay here to ensure the data byte has gotten onto the bus
	 * before the transaction is started */
	/* enable start condition */
	stat |= NXP3200_IICSTAT_START;
	writel(stat, i2c->regs + NXP3200_IICSTAT);

	ndelay(i2c->tx_setup);

	/* sesters: allow data to start */
	nxp3200_i2c_ClearOperationHold(i2c);

	dev_dbg(i2c->dev, "START: %08lx to IICSTAT, %02x to DS\n", stat, addr);
}

static inline void nxp3200_i2c_stop(struct nxp3200_i2c *i2c, int ret)
{
// sesters: test using Samsung 'STOP' sequence


#ifndef S3C24XX_I2C_DRIVER

	unsigned long iicstat = (i2c->msg->flags & I2C_M_RD) ? NXP3200_IICSTAT_MASTER_RX : NXP3200_IICSTAT_MASTER_TX;

	//sesters testing stop bit

	//unsigned long iicstat = readl(i2c->regs + NXP3200_IICSTAT) & 0x1F0F;
#else
	unsigned long iicstat = readl(i2c->regs + NXP3200_IICSTAT);
#endif

	dev_info(i2c->dev, "%s.%d i2c->state:%d STOP\n",
			__FUNCTION__, __LINE__, i2c->state);

	/* stop the transfer */
#ifndef S3C24XX_I2C_DRIVER

	iicstat &= ~NXP3200_IICSTAT_START;
// sesters: ensure ST_ENB set
	iicstat |= NXP3200_IICSTAT_ST_ENB;
#else
	iicstat &= ~NXP3200_IICSTAT_START;
#endif
	// sesters: just for testing
	//iicstat = readl(i2c->regs + NXP3200_IICSTAT) & 0x1F0F;
	//iicstat |= 0x10D0;
	writel(iicstat, i2c->regs + NXP3200_IICSTAT);

	i2c->state = STATE_STOP;

	//sesters try to clear bus busy condition
	nxp3200_i2c_disable_ack(i2c);
	// sesters testing stop
	// nxp3200_i2c_master_complete(i2c, ret);
	nxp3200_i2c_disable_irq(i2c);

#ifndef S3C24XX_I2C_DRIVER
	/* sesters: generate stop */
	nxp3200_i2c_ClearInterruptPendingAll(i2c);
	nxp3200_i2c_ClearOperationHold(i2c);
//	i2c_dump_registers(i2c, __FUNCTION__, __LINE__);
	nxp3200_i2c_master_complete(i2c, ret);
#endif
}

/* helper functions to determine the current state in the set of
 * messages we are sending */

/* is_lastmsg()
 *
 * returns TRUE if the current message is the last in the set
*/

static inline int is_lastmsg(struct nxp3200_i2c *i2c)
{
	return i2c->msg_idx >= (i2c->msg_num - 1);
}

/* is_msglast
 *
 * returns TRUE if we this is the last byte in the current message
*/

static inline int is_msglast(struct nxp3200_i2c *i2c)
{
	return i2c->msg_ptr == i2c->msg->len-1;
}

/* is_msgend
 *
 * returns TRUE if we reached the end of the current message
*/

static inline int is_msgend(struct nxp3200_i2c *i2c)
{
	return i2c->msg_ptr >= i2c->msg->len;
}

/* i2c_nxp3200_irq_nextbyte
 *
 * process an interrupt and work out what to do
 */

static int i2c_nxp3200_irq_nextbyte(struct nxp3200_i2c *i2c, unsigned long iicstat)
{
	unsigned long tmp;
	unsigned char byte;
	int ret = 0;

	switch (i2c->state) {

	case STATE_IDLE:
		dev_err(i2c->dev, "%s: called in STATE_IDLE\n", __func__);
		goto out;

	case STATE_STOP:
		dev_err(i2c->dev, "%s: called in STATE_STOP\n", __func__);
		nxp3200_i2c_disable_irq(i2c);
		goto out_ack;

	case STATE_START:
		/* last thing we did was send a start condition on the
		 * bus, or started a new i2c message
		 */

		if (iicstat & NXP3200_IICSTAT_LASTBIT &&
		    !(i2c->msg->flags & I2C_M_IGNORE_NAK)) {
			/* ack was not received... */

			dev_info(i2c->dev, "ack was not received\n");
			nxp3200_i2c_stop(i2c, -ENXIO);
			goto out_ack;
		}

		if (i2c->msg->flags & I2C_M_RD)
			i2c->state = STATE_READ;
		else
			i2c->state = STATE_WRITE;

		/* terminate the transfer if there is nothing to do
		 * as this is used by the i2c probe to find devices. */

		if (is_lastmsg(i2c) && i2c->msg->len == 0) {
			nxp3200_i2c_stop(i2c, 0);
			goto out_ack;
		}

		if (i2c->state == STATE_READ)
			goto prepare_read;

		/* fall through to the write state, as we will need to
		 * send a byte as well */

	case STATE_WRITE:
		/* we are writing data to the device... check for the
		 * end of the message, and if so, work out what to do
		 */

		if (!(i2c->msg->flags & I2C_M_IGNORE_NAK)) {
			if (iicstat & NXP3200_IICSTAT_LASTBIT) {
				dev_info(i2c->dev, "WRITE: No Ack\n");
				printk(KERN_INFO "%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

				nxp3200_i2c_stop(i2c, -ECONNREFUSED);
				goto out_ack;
			}
		}

 retry_write:

		if (!is_msgend(i2c)) {
			//nxp3200_i2c_disable_ack(i2c);
			byte = i2c->msg->buf[i2c->msg_ptr++];
			writel(byte, i2c->regs + NXP3200_IICDS);

			/* delay after writing the byte to allow the
			 * data setup time on the bus, as writing the
			 * data to the register causes the first bit
			 * to appear on SDA, and SCL will change as
			 * soon as the interrupt is acknowledged */

			ndelay(i2c->tx_setup);
			/* sesters: allow data to start */
			//sesters: comment out, maybe break it
			//nxp3200_i2c_ClearOperationHold(i2c);

		} else if (!is_lastmsg(i2c)) {
			/* we need to go to the next i2c message */

			dev_dbg(i2c->dev, "WRITE: Next Message\n");

			i2c->msg_ptr = 0;
			i2c->msg_idx++;
			i2c->msg++;

			/* check to see if we need to do another message */
			if (i2c->msg->flags & I2C_M_NOSTART) {

				printk(KERN_INFO "%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);
				if (i2c->msg->flags & I2C_M_RD) {
					/* cannot do this, the controller
					 * forces us to send a new START
					 * when we change direction */

					printk(KERN_INFO "%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);
					nxp3200_i2c_stop(i2c, -EINVAL);
				}
				printk(KERN_INFO "%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);

				goto retry_write;
			} else {
				printk(KERN_INFO "%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);
				/* send the new start */
				nxp3200_i2c_message_start(i2c, i2c->msg);
				i2c->state = STATE_START;
			}

		} else {
			/* send stop */
			nxp3200_i2c_stop(i2c, 0);
		}
		break;

	case STATE_READ:
		/* we have a byte of data in the data register, do
		 * something with it, and then work out whether we are
		 * going to do any more read/write
		 */

		byte = readl(i2c->regs + NXP3200_IICDS) & 0xFF;
		i2c->msg->buf[i2c->msg_ptr++] = byte;

 prepare_read:
		if (is_msglast(i2c)) {
			/* last byte of buffer */

			if (is_lastmsg(i2c))
				nxp3200_i2c_disable_ack(i2c);

		} else if (is_msgend(i2c)) {
			/* ok, we've read the entire buffer, see if there
			 * is anything else we need to do */

			if (is_lastmsg(i2c)) {
				/* last message, send stop and complete */
				dev_dbg(i2c->dev, "READ: Send Stop\n");

				nxp3200_i2c_stop(i2c, 0);
			} else {
				/* go to the next transfer */
				dev_dbg(i2c->dev, "READ: Next Transfer\n");

				i2c->msg_ptr = 0;
				i2c->msg_idx++;
				i2c->msg++;
			}
		}

		break;
	}

	/* acknowledge the IRQ and get back on with the work */

 out_ack:
#ifdef S3C24XX_I2C_DRIVER
	tmp = readl(i2c->regs + NXP3200_IICCON);
	tmp &= ~NXP3200_IICCON_IRQPEND;
	writel(tmp, i2c->regs + NXP3200_IICCON);
#else
	nxp3200_i2c_ClearInterruptPendingAll(i2c);
	nxp3200_i2c_ClearOperationHold(i2c);
#endif
 out:
	return ret;
}

/* nxp3200_i2c_irq
 *
 * top level IRQ servicing routine
*/

static irqreturn_t nxp3200_i2c_irq(int irqno, void *dev_id)
{
	struct nxp3200_i2c *i2c = dev_id;
	unsigned long status;
	unsigned long tmp;

	status = readl(i2c->regs + NXP3200_IICSTAT);

	if (status & NXP3200_IICSTAT_ARBITR) {
		/* deal with arbitration loss */
		dev_err(i2c->dev, "deal with arbitration loss\n");
	}

	if (i2c->state == STATE_IDLE) {
		dev_dbg(i2c->dev, "IRQ: error i2c->state == IDLE\n");

#ifdef S3C24XX_I2C_DRIVER
	tmp = readl(i2c->regs + NXP3200_IICCON);
	tmp &= ~NXP3200_IICCON_IRQPEND;
	writel(tmp, i2c->regs + NXP3200_IICCON);
#else
	nxp3200_i2c_ClearInterruptPendingAll(i2c);
#endif
		goto out;
	}

	/* pretty much this leaves us with the fact that we've
	 * transmitted or received whatever byte we last sent */

	i2c_nxp3200_irq_nextbyte(i2c, status);

 out:
	return IRQ_HANDLED;
}


/* nxp3200_i2c_set_master
 *
 * get the i2c bus for a master transaction
*/

static int nxp3200_i2c_set_master(struct nxp3200_i2c *i2c)
{
	unsigned long iicstat;
	int timeout = 400;

	while (timeout-- > 0) {
		iicstat = readl(i2c->regs + NXP3200_IICSTAT);

		if (!(iicstat & NXP3200_IICSTAT_BUSBUSY))
			return 0;

		msleep(1);
	}

	return -ETIMEDOUT;
}

/* nxp3200_i2c_doxfer
 *
 * this starts an i2c transfer
*/

static int nxp3200_i2c_doxfer(struct nxp3200_i2c *i2c,
			      struct i2c_msg *msgs, int num)
{
	unsigned long iicstat, timeout;
	int spins = 20;
	int ret;

	if (i2c->suspended)
		return -EIO;

	ret = nxp3200_i2c_set_master(i2c);
	if (ret != 0) {
		dev_err(i2c->dev, "cannot get bus (error %d)\n", ret);
		ret = -EAGAIN;
		goto out;
	}

	spin_lock_irq(&i2c->lock);

	i2c->msg     = msgs;
	i2c->msg_num = num;
	i2c->msg_ptr = 0;
	i2c->msg_idx = 0;
	i2c->state   = STATE_START;

	nxp3200_i2c_enable_irq(i2c);
	nxp3200_i2c_message_start(i2c, msgs);
	spin_unlock_irq(&i2c->lock);

	timeout = wait_event_timeout(i2c->wait, i2c->msg_num == 0, HZ * 5);

	ret = i2c->msg_idx;

	/* having these next two as dev_err() makes life very
	 * noisy when doing an i2cdetect */

	if (timeout == 0)
		dev_dbg(i2c->dev, "timeout\n");
	else if (ret != num)
		dev_dbg(i2c->dev, "incomplete xfer (%d)\n", ret);

	/* ensure the stop has been through the bus */

	dev_dbg(i2c->dev, "waiting for bus idle\n");

	/* first, try busy waiting briefly */
	do {
		cpu_relax();
		iicstat = readl(i2c->regs + NXP3200_IICSTAT);
	} while ((iicstat & NXP3200_IICSTAT_START) && --spins);

	/* if that timed out sleep */
	if (!spins) {
		msleep(1);
		iicstat = readl(i2c->regs + NXP3200_IICSTAT);
	}

	if (iicstat & NXP3200_IICSTAT_START) {
		dev_warn(i2c->dev, "timeout waiting for bus idle\n");
		//sesters: just for debugging
//		i2c_dump_registers(i2c, __FUNCTION__, __LINE__);
	}
 out:
	return ret;
}

/* nxp3200_i2c_xfer
 *
 * first port of call from the i2c bus code when an message needs
 * transferring across the i2c bus.
*/

static int nxp3200_i2c_xfer(struct i2c_adapter *adap,
			struct i2c_msg *msgs, int num)
{
	struct nxp3200_i2c *i2c = (struct nxp3200_i2c *)adap->algo_data;
	int retry;
	int ret;

#ifdef S3C24XX_I2C_DRIVER
	pm_runtime_get_sync(&adap->dev);
	clk_enable(i2c->clk);
#else
	//i2c_set_io_func(i2c);
	i2c_set_clock(i2c, CTRUE);
#endif
	for (retry = 0; retry < adap->retries; retry++) {

		ret = nxp3200_i2c_doxfer(i2c, msgs, num);

		if (ret != -EAGAIN) {
#ifdef S3C24XX_I2C_DRIVER
			clk_disable(i2c->clk);
#else
			i2c_set_clock(i2c, CFALSE);
#endif
			pm_runtime_put_sync(&adap->dev);
			return ret;
		}

		dev_info(i2c->dev, "Retrying transmission (%d)\n", retry);

		udelay(100);
	}
#ifdef S3C24XX_I2C_DRIVER
	clk_disable(i2c->clk);
#else
	i2c_set_clock(i2c, CFALSE);
#endif
	pm_runtime_put_sync(&adap->dev);
	return -EREMOTEIO;
}

/* declare our i2c functionality */
static u32 nxp3200_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL | I2C_FUNC_PROTOCOL_MANGLING;
}

/* i2c bus registration info */

static const struct i2c_algorithm nxp3200_i2c_algorithm = {
	.master_xfer		= nxp3200_i2c_xfer,
	.functionality		= nxp3200_i2c_func,
};

/* nxp3200_i2c_calcdivisor
 *
 * return the divisor settings for a given frequency
*/

static int nxp3200_i2c_calcdivisor(unsigned long clkin, unsigned int wanted,
				   unsigned int *div1, unsigned int *divs)
{
	unsigned int calc_divs = clkin / wanted;
	unsigned int calc_div1;

	if (calc_divs > (16*16))
		calc_div1 = 256;
	else
		calc_div1 = 16;

	calc_divs += calc_div1-1;
	calc_divs /= calc_div1;

	if (calc_divs == 0)
		calc_divs = 1;
	if (calc_divs > 17)
		calc_divs = 17;

	*divs = calc_divs;
	*div1 = calc_div1;

	return clkin / (calc_divs * calc_div1);
}

/* nxp3200_i2c_clockrate
 *
 * work out a divisor for the user requested frequency setting,
 * either by the requested frequency, or scanning the acceptable
 * range of frequencies until something is found
*/

static int nxp3200_i2c_clockrate(struct nxp3200_i2c *i2c, unsigned int *got)
{
	struct i2c_plat_data *pdata = i2c->pdata;
	unsigned long clkin = cpu_get_clock_hz(CFG_SYS_CLKSRC_PCLK);
	unsigned int divs, div1;
	unsigned long target_frequency;
	u32 iiccon;
	int freq;

	i2c->clkrate = clkin;
	clkin /= 1000;		/* clkin now in KHz */

	dev_info(i2c->dev, "pdata desired frequency %lu\n", pdata->frequency);

	target_frequency = pdata->frequency ? pdata->frequency : 100000;

	target_frequency /= 1000; /* Target frequency now in KHz */

	freq = nxp3200_i2c_calcdivisor(clkin, target_frequency, &div1, &divs);

	if (freq > target_frequency) {
		dev_err(i2c->dev,
			"Unable to achieve desired frequency %luKHz."	\
			" Lowest achievable %dKHz\n", target_frequency, freq);
		return -EINVAL;
	}

	*got = freq;

	i2c->pdata->clkscale = divs;
	i2c->pdata->clksrc = (div1 == 256) ? 1 : 0;

	iiccon = readl(i2c->regs + NXP3200_IICCON);
	iiccon &= ~(NXP3200_IICCON_SCALEMASK | NXP3200_IICCON_TXDIV_256);
	iiccon |= (divs-1);

	if (div1 == 256)
		iiccon |= NXP3200_IICCON_TXDIV_256;

	writel(iiccon, i2c->regs + NXP3200_IICCON);

	dev_dbg(i2c->dev, "freq:%d, div1:%d, divs:%d, iiccon:0x%8.8X\n", freq, div1, divs, iiccon);

#ifdef S3C24XX_I2C_DRIVER
	if (s3c24xx_i2c_is2440(i2c)) {
		unsigned long sda_delay;

		if (pdata->sda_delay) {
			sda_delay = clkin * pdata->sda_delay;
			sda_delay = DIV_ROUND_UP(sda_delay, 1000000);
			sda_delay = DIV_ROUND_UP(sda_delay, 5);
			if (sda_delay > 3)
				sda_delay = 3;
			sda_delay |= NXP3200_IICLC_FILTER_ON;
		} else
			sda_delay = 0;

		dev_dbg(i2c->dev, "IICLC=%08lx\n", sda_delay);
		writel(sda_delay, i2c->regs + S3C2440_IICLC);
	}
#endif

	return 0;
}

#ifdef CONFIG_CPU_FREQ

#define freq_to_i2c(_n) container_of(_n, struct nxp3200_i2c, freq_transition)

static int nxp3200_i2c_cpufreq_transition(struct notifier_block *nb,
					  unsigned long val, void *data)
{
	struct nxp3200_i2c *i2c = freq_to_i2c(nb);
	unsigned long flags;
	unsigned int got;
	int delta_f;
	int ret;

	delta_f = clk_get_rate(i2c->clk) - i2c->clkrate;

	/* if we're post-change and the input clock has slowed down
	 * or at pre-change and the clock is about to speed up, then
	 * adjust our clock rate. <0 is slow, >0 speedup.
	 */

	if ((val == CPUFREQ_POSTCHANGE && delta_f < 0) ||
	    (val == CPUFREQ_PRECHANGE && delta_f > 0)) {
		spin_lock_irqsave(&i2c->lock, flags);
		ret = nxp3200_i2c_clockrate(i2c, &got);
		spin_unlock_irqrestore(&i2c->lock, flags);

		if (ret < 0)
			dev_err(i2c->dev, "cannot find frequency\n");
		else
			dev_info(i2c->dev, "setting freq %d\n", got);
	}

	return 0;
}

static inline int nxp3200_i2c_register_cpufreq(struct nxp3200_i2c *i2c)
{
	i2c->freq_transition.notifier_call = nxp3200_i2c_cpufreq_transition;

	return cpufreq_register_notifier(&i2c->freq_transition,
					 CPUFREQ_TRANSITION_NOTIFIER);
}

static inline void nxp3200_i2c_deregister_cpufreq(struct nxp3200_i2c *i2c)
{
	cpufreq_unregister_notifier(&i2c->freq_transition,
				    CPUFREQ_TRANSITION_NOTIFIER);
}

#else
static inline int nxp3200_i2c_register_cpufreq(struct nxp3200_i2c *i2c)
{
	return 0;
}

static inline void nxp3200_i2c_deregister_cpufreq(struct nxp3200_i2c *i2c)
{
}
#endif

#ifdef CONFIG_OF
static int nxp3200_i2c_parse_dt_gpio(struct nxp3200_i2c *i2c)
{
	int idx, gpio, ret;

	for (idx = 0; idx < 2; idx++) {
		gpio = of_get_gpio(i2c->dev->of_node, idx);
		if (!gpio_is_valid(gpio)) {
			dev_err(i2c->dev, "invalid gpio[%d]: %d\n", idx, gpio);
			goto free_gpio;
		}

		ret = gpio_request(gpio, "i2c-bus");
		if (ret) {
			dev_err(i2c->dev, "gpio [%d] request failed\n", gpio);
			goto free_gpio;
		}
	}
	return 0;

free_gpio:
	while (--idx >= 0)
		gpio_free(i2c->gpios[idx]);
	return -EINVAL;
}

static void nxp3200_i2c_dt_gpio_free(struct nxp3200_i2c *i2c)
{
	unsigned int idx;
	for (idx = 0; idx < 2; idx++)
		gpio_free(i2c->gpios[idx]);
}
#else
static int nxp3200_i2c_parse_dt_gpio(struct nxp3200_i2c *i2c)
{
	return 0;
}

static void nxp3200_i2c_dt_gpio_free(struct nxp3200_i2c *i2c)
{
}
#endif


/* nxp3200_i2c_init
 *
 * initialise the controller, set the IO lines and frequency
*/

static int nxp3200_i2c_init(struct nxp3200_i2c *i2c)
{
	unsigned long iicon = NXP3200_IICCON_IRQEN | NXP3200_IICCON_ACKEN;
	unsigned int freq;

	printk(KERN_INFO "%s.%s.%d i2c->regs:0x%8.8X\n", __FILE__, __FUNCTION__, __LINE__, i2c->regs);

	/* initialize the gpio */

#ifndef S3C24XX_I2C_DRIVER
		i2c_init_func(i2c);
#else
	if (i2c->pdata->cfg_gpio) {
		pdata->cfg_gpio(to_platform_device(i2c->dev));
	} else {
		if (nxp3200_i2c_parse_dt_gpio(i2c))
			return -EINVAL;
	}
#endif

	/* write slave address */

	writel(i2c->pdata->slave_addr, i2c->regs + NXP3200_IICADD);

	dev_dbg(i2c->dev, "slave address 0x%02x\n", i2c->pdata->slave_addr);

	writel(iicon, i2c->regs + NXP3200_IICCON);


	/* we need to work out the divisors for the clock... */

	if (nxp3200_i2c_clockrate(i2c, &freq) != 0) {
		writel(0, i2c->regs + NXP3200_IICCON);
		dev_err(i2c->dev, "cannot meet bus frequency required\n");
		return -EINVAL;
	}

	/* Set QCNT_MAX for NXP3200 */
	writel(i2c->pdata->cntmax ,i2c->regs + NXP3200_IICQCNTMAX);


	/* todo - check that the i2c lines aren't being dragged anywhere */
	/* stop the transfer */
	// sesters: Nexell brute force try
	writel(0x1010, i2c->regs + NXP3200_IICSTAT);
	nxp3200_i2c_ClearOperationHold(i2c);
	//nxp3200_i2c_stop(i2c, 0);

	dev_dbg(i2c->dev, "bus frequency set to %d KHz\n", freq);
	dev_dbg(i2c->dev, "NXP3200_IICCON=0x%02lx\n", iicon);

	return 0;
}

#ifdef CONFIG_OF
/* nxp3200_i2c_parse_dt
 *
 * Parse the device tree node and retreive the platform data.
*/

static void
nxp3200_i2c_parse_dt(struct device_node *np, struct nxp3200_i2c *i2c)
{
	struct i2c_plat_data *pdata = i2c->pdata;

	if (!np)
		return;

	pdata->bus_num = -1; /* i2c bus number is dynamically assigned */
	of_property_read_u32(np, "samsung,i2c-sda-delay", &pdata->sda_delay);
	of_property_read_u32(np, "samsung,i2c-slave-addr", &pdata->slave_addr);
	of_property_read_u32(np, "samsung,i2c-max-bus-freq",
				(u32 *)&pdata->frequency);
}
#else
static void
nxp3200_i2c_parse_dt(struct device_node *np, struct nxp3200_i2c *i2c)
{
	printk("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);
	return;
}
#endif

/* nxp3200_i2c_probe
 *
 * called by the bus driver when a suitable device is found
*/

static int nxp3200_i2c_probe(struct platform_device *pdev)
{
	struct nxp3200_i2c *i2c;
	struct i2c_plat_data *pdata = NULL;
	struct resource *res;
	int ret;

	if (!pdev->dev.of_node) {
		pdata = pdev->dev.platform_data;
		if (!pdata) {
			dev_err(&pdev->dev, "no platform data\n");
			return -EINVAL;
		}
	}

	msleep(2000);

	i2c = devm_kzalloc(&pdev->dev, sizeof(struct nxp3200_i2c), GFP_KERNEL);
	if (!i2c) {
		dev_err(&pdev->dev, "no memory for state\n");
		return -ENOMEM;
	}

	i2c->pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (!i2c->pdata) {
		ret = -ENOMEM;
		goto err_noclk;
	}

	if (pdata)
		memcpy(i2c->pdata, pdata, sizeof(*pdata));
	else
		nxp3200_i2c_parse_dt(pdev->dev.of_node, i2c);

	strlcpy(i2c->adap.name, "nxp3200-i2c", sizeof(i2c->adap.name));
	i2c->adap.owner   = THIS_MODULE;
	i2c->adap.algo    = &nxp3200_i2c_algorithm;
	i2c->adap.retries = 2;
	i2c->adap.class   = I2C_CLASS_HWMON | I2C_CLASS_SPD;
	i2c->tx_setup     = 50;


	/* initialize Nexell I2C bus library */
	NX_I2C_SetBaseAddress(i2c->pdata->port, (U32)IO_ADDRESS(NX_I2C_GetPhysicalAddress(i2c->pdata->port)));
	i2c->regs = (U32)IO_ADDRESS(NX_I2C_GetPhysicalAddress(i2c->pdata->port));
	printk(KERN_INFO "%s.%s.%d  i2c->regs:0x%8.8X\n", __FILE__, __FUNCTION__, __LINE__, i2c->regs);

	spin_lock_init(&i2c->lock);
	init_waitqueue_head(&i2c->wait);

#ifdef S3C24XX_I2C_DRIVER
	/* find the clock and enable it */

	i2c->dev = &pdev->dev;
	i2c->clk = clk_get(&pdev->dev, "i2c");
	if (IS_ERR(i2c->clk)) {
		dev_err(&pdev->dev, "cannot get clock\n");
		ret = -ENOENT;
		goto err_noclk;
	}


	dev_dbg(&pdev->dev, "clock source %p\n", i2c->clk);
	clk_enable(i2c->clk);
#else
	i2c_set_io_func(i2c);
	i2c_set_clock(i2c, CTRUE);
#endif

	/* map the registers */

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "cannot find IO resource\n");
		ret = -ENOENT;
		goto err_clk;
	}

	i2c->ioarea = request_mem_region(res->start, resource_size(res),
					 pdev->name);

	if (i2c->ioarea == NULL) {
		dev_err(&pdev->dev, "cannot request IO\n");
		ret = -ENXIO;
		goto err_clk;
	}

	i2c->regs = ioremap(res->start, resource_size(res));

	if (i2c->regs == NULL) {
		dev_err(&pdev->dev, "cannot map IO\n");
		ret = -ENXIO;
		goto err_ioarea;
	}

	dev_dbg(&pdev->dev, "registers %p (%p, %p)\n",
		i2c->regs, i2c->ioarea, res);

	/* setup info block for the i2c core */

	i2c->adap.algo_data = i2c;
	i2c->adap.dev.parent = &pdev->dev;

	/* initialize the i2c controller */

	ret = nxp3200_i2c_init(i2c);
	if (ret != 0)
		goto err_iomap;

	/* find the IRQ for this unit (note, this relies on the init call to
	 * ensure no current IRQs pending
	 */

	i2c->irq = ret = platform_get_irq(pdev, 0);
	if (ret <= 0) {
		dev_err(&pdev->dev, "cannot find IRQ\n");
		goto err_iomap;
	}

	ret = request_irq(i2c->irq, nxp3200_i2c_irq, 0,
			  dev_name(&pdev->dev), i2c);

	if (ret != 0) {
		dev_err(&pdev->dev, "cannot claim IRQ %d\n", i2c->irq);
		goto err_iomap;
	}

	ret = nxp3200_i2c_register_cpufreq(i2c);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to register cpufreq notifier\n");
		goto err_irq;
	}

	/* Note, previous versions of the driver used i2c_add_adapter()
	 * to add the bus at any number. We now pass the bus number via
	 * the platform data, so if unset it will now default to always
	 * being bus 0.
	 */

	i2c->adap.nr = i2c->pdata->port;
	i2c->adap.dev.of_node = pdev->dev.of_node;

	ret = i2c_add_numbered_adapter(&i2c->adap);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to add bus to i2c core\n");
		goto err_cpufreq;
	}

	of_i2c_register_devices(&i2c->adap);
	platform_set_drvdata(pdev, i2c);

	// sesters: remove while debugging
#if 0
	pm_runtime_enable(&pdev->dev);
	pm_runtime_enable(&i2c->adap.dev);
#endif

	/* sesters: testing, make high level */
// sesters: just testing
	//writel(0xFF, i2c->regs + NXP3200_IICDS);

	dev_dbg(&pdev->dev, "%s: I2C adapter\n", dev_name(&i2c->adap.dev));

//	i2c_dump_registers(i2c, __FUNCTION__, __LINE__);

#ifdef S3C24XX_I2C_DRIVER
	clk_disable(i2c->clk);
#else
	i2c_set_clock(i2c, CFALSE);
#endif

	return 0;

 err_cpufreq:
	nxp3200_i2c_deregister_cpufreq(i2c);

 err_irq:
	free_irq(i2c->irq, i2c);

 err_iomap:
	iounmap(i2c->regs);

 err_ioarea:
	release_resource(i2c->ioarea);
	kfree(i2c->ioarea);

 err_clk:
#ifdef S3C24XX_I2C_DRIVER
	clk_disable(i2c->clk);
#else
	i2c_set_clock(i2c, CFALSE);
#endif

 err_noclk:
	return ret;
}

/* nxp3200_i2c_remove
 *
 * called when device is removed from the bus
*/

static int nxp3200_i2c_remove(struct platform_device *pdev)
{
	struct nxp3200_i2c *i2c = platform_get_drvdata(pdev);

#ifdef S3C24XX_I2C_DRIVER
	pm_runtime_disable(&i2c->adap.dev);
	pm_runtime_disable(&pdev->dev);
#endif
	nxp3200_i2c_deregister_cpufreq(i2c);


	i2c_del_adapter(&i2c->adap);
	free_irq(i2c->irq, i2c);

	iounmap(i2c->regs);

	release_resource(i2c->ioarea);
	kfree(i2c->ioarea);

	return 0;
}

#ifdef CONFIG_PM
static int nxp3200_i2c_suspend_noirq(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct nxp3200_i2c *i2c = platform_get_drvdata(pdev);

	i2c->suspended = 1;

	return 0;
}

static int nxp3200_i2c_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct nxp3200_i2c *i2c = platform_get_drvdata(pdev);

	i2c->suspended = 0;
#ifdef S3C24XX_I2C_DRIVER
	clk_enable(i2c->clk);
#else
	//i2c_set_io_func(i2c);
	i2c_set_clock(i2c, CTRUE);
#endif
	nxp3200_i2c_init(i2c);
#ifdef S3C24XX_I2C_DRIVER
	clk_disable(i2c->clk);
#else
	i2c_set_clock(i2c, CFALSE);
#endif

	return 0;
}

static const struct dev_pm_ops nxp3200_i2c_dev_pm_ops = {
	.suspend_noirq = nxp3200_i2c_suspend_noirq,
	.resume = nxp3200_i2c_resume,
};

#define NXP3200_DEV_PM_OPS (&nxp3200_i2c_dev_pm_ops)
#else
#define NXP3200_DEV_PM_OPS NULL
#endif

#ifdef S3C24XX_I2C_DRIVER
/* device driver for platform bus bits */

static struct platform_device_id s3c24xx_driver_ids[] = {
	{
		.name		= "s3c2410-i2c",
		.driver_data	= TYPE_S3C2410,
	}, {
		.name		= "s3c2440-i2c",
		.driver_data	= TYPE_S3C2440,
	}, { },
};
MODULE_DEVICE_TABLE(platform, s3c24xx_driver_ids);
#endif

#ifdef CONFIG_OF
#ifdef S3C24XX_I2C_DRIVER
static const struct of_device_id s3c24xx_i2c_match[] = {
	{ .compatible = "samsung,s3c2410-i2c" },
	{ .compatible = "samsung,s3c2440-i2c" },
	{},
};
MODULE_DEVICE_TABLE(of, s3c24xx_i2c_match);
#endif
#else
#define nxp3200_i2c_match NULL
#endif

static struct platform_driver nxp3200_i2c_driver = {
	.probe		= nxp3200_i2c_probe,
	.remove		= nxp3200_i2c_remove,
#ifdef S3C24XX_I2C_DRIVER
	.id_table	= s3c24xx_driver_ids,
#endif
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= I2C_DEV_NAME,
		.pm	= NXP3200_DEV_PM_OPS,
		.of_match_table = nxp3200_i2c_match,
	},
};

static int __init i2c_adap_nxp3200_init(void)
{
	return platform_driver_register(&nxp3200_i2c_driver);
}
subsys_initcall(i2c_adap_nxp3200_init);

static void __exit i2c_adap_nxp3200_exit(void)
{
	platform_driver_unregister(&nxp3200_i2c_driver);
}
module_exit(i2c_adap_nxp3200_exit);

MODULE_DESCRIPTION("NXP3200 I2C Bus driver");
MODULE_AUTHOR("Scott Esters, <sesters@leapfrog.com>");
MODULE_LICENSE("GPL");
