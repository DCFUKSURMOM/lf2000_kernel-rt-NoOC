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

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

#include <linux/dma-mapping.h>
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>
#include <mach/spi.h>
 
#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "nx-spi: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 	\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",	\
			__FILE__, __FUNCTION__, __LINE__),			\
		printk(KERN_ERR msg); }

#define CFG_SPI_CLK_FREQ		CFG_SYS_PLL1_FREQ
#define CFG_SPI_CLK_INV			CTRUE
#define CFG_SPI_BIT_WIDTH		8


#define	SPI_CLOCK_MAX			(CFG_SPI_CLK_FREQ / CFG_SPI_CLOCK_DIVISOR /  4)
#define	SPI_CLOCK_MIN			(CFG_SPI_CLK_FREQ / CFG_SPI_CLOCK_DIVISOR / 32)
#define	SPI_CLOCK_CURR			(CFG_SPI_CLK_FREQ / CFG_SPI_CLOCK_DIVISOR / CFG_SPI_CLOCK_DIVCOUNT)

#define MAX_SPI_CS_NUM			2

/*----------------------------------------------------------------------------*/
struct spi_bus_info {
	int 				port;
	int 				irqno;
	int					io_clk;					/* clock gpio */
	int					io_rx;					/* tx gpio */
	int					io_tx;					/* rx gpio */
	struct spi_io_cs  	io_cs[MAX_SPI_CS_NUM];
	unsigned int		max_clk;
	unsigned int		min_clk;
};

struct spi_cs_info {
	unsigned int	speed_hz;
	unsigned int	bits_word;
	unsigned int	cs_mode;
};

struct spi_param {
	struct work_struct			work;
	struct workqueue_struct *	work_queue;
	spinlock_t					lock;
	struct list_head			msg_queue;
	struct spi_master 		*	master;
	struct spi_bus_info    		bus;
	struct spi_cs_info			cs_info[MAX_SPI_CS_NUM];
	int							cs_num;
	unsigned int				tr_mode;
	unsigned char			*	tx_buf;
	unsigned char			*	rx_buf;
	int							tr_count;

	struct device *txdev;
	struct device *rxdev;

	struct dma_trans 		* 	dmatxtr;
	struct dma_trans 		* 	dmarxtr;
	
	dma_addr_t waddr;
	dma_addr_t raddr;
	u8 *dma_writeBuffer;
	u8 *dma_readBuffer;
	
	wait_queue_head_t			wait_q;
	unsigned int 				cond;
	long						timeout;
};

/*------------------------------------------------------------------------------
 * 	SPI local functions
 */
static void do_spi_workqueue(struct work_struct *work);
static irqreturn_t do_spi_irqhandler(int irqno, void *dev_id);
static irqreturn_t do_dmatx_irqhandler(int irqno, void *dev_id);
static irqreturn_t do_dmarx_irqhandler(int irqno, void *dev_id);

static int	init_spi_device(struct spi_param *par, struct nxp3200_spi_plat_data * plat)
{
	unsigned int gpio, func;
	int port = plat->port;
	int i = 0, ret = 0;
	
	/* set spi params */
	par->bus.port	 = plat->port;
	par->bus.irqno   = plat->irq;
	par->bus.io_clk  = plat->io_clk;
	par->bus.io_rx	 = plat->io_rx;
	par->bus.io_tx   = plat->io_tx;
	par->bus.max_clk = SPI_CLOCK_MAX;
	par->bus.min_clk = SPI_CLOCK_MIN;
	par->cs_num		 = plat->cs_num;
	/* Set SPI DMA */

	par->dmatxtr		=	soc_dma_request(plat->dma_tx_channel,0);
	if(NULL == par->dmatxtr )
	{
		printk("Error tx request\n");
		return -1;
	}
	par->dmarxtr		=	soc_dma_request(plat->dma_rx_channel,0);
	if(NULL == par->dmarxtr )
	{
		printk("Error rx request\n");
		return -1;
	}
	
	par->dma_writeBuffer = dma_alloc_writecombine(par->txdev,NX_DMA_MAX_TRANSFER_SIZE,&par->waddr,(GFP_KERNEL));
	if(NULL == par->dma_writeBuffer )
	{
		printk("Error alloc Writebuffer\n");
		return -1;
	}

	par->dma_readBuffer = dma_alloc_writecombine(par->rxdev,NX_DMA_MAX_TRANSFER_SIZE,&par->raddr,(GFP_KERNEL));
	if(NULL == par->dma_readBuffer )
	{
		printk("Error alloc Readbuffer\n");
		return -1;
	}
		
	/* set spi gpio */
	soc_gpio_set_io_func(par->bus.io_clk, NX_GPIO_PADFUNC_1);
	soc_gpio_set_io_func(par->bus.io_rx , NX_GPIO_PADFUNC_1);
	soc_gpio_set_io_func(par->bus.io_tx , NX_GPIO_PADFUNC_1);
	soc_gpio_set_io_pullup(par->bus.io_rx, 1);
	soc_gpio_set_io_pullup(par->bus.io_tx, 1);
	for (i = 0; par->cs_num > i; i++) {
		gpio = par->bus.io_cs[i].pad  = (&plat->io_cs[i])->pad;
		func = par->bus.io_cs[i].func = (&plat->io_cs[i])->func;
		DBGOUT("cs[%d], io=%3d, mode=0x%x\n", i, gpio, func);

		soc_gpio_set_io_func(gpio, func);
		if (func == NX_GPIO_PADFUNC_GPIO) {
			soc_gpio_set_io_dir(gpio, 1);
			soc_gpio_set_out_value(gpio, 1);
		}
		soc_gpio_set_io_pullup(gpio, 1);
	}
 
	NX_SSPSPI_Initialize();
	NX_SSPSPI_SetBaseAddress(port,IO_ADDRESS(NX_SSPSPI_GetPhysicalAddress(port)));
	NX_SSPSPI_OpenModule(port);

	NX_SSPSPI_SetClockPClkMode(port, NX_PCLKMODE_ALWAYS);
	NX_SSPSPI_SetClockSource(port, 0, plat->clk_src);	

	NX_SSPSPI_SetClockDivisor(port, 0, plat->clk_divisor); 	
	NX_SSPSPI_SetClockDivisorEnable(port, CTRUE);

	NX_SSPSPI_SetEnable(port, CFALSE);
	NX_SSPSPI_SetExternalClockSource(port, CFALSE);	
	NX_SSPSPI_SetDividerCount(port, plat->clk_divcount);
	
	NX_SSPSPI_SetBitWidth(port, 16);
	NX_SSPSPI_SetDMATransferMode(port, CFALSE);

	NX_SSPSPI_SetRxBurstEnable(port, CFALSE );

	// SET CON1
	NX_SSPSPI_SetProtocol(port, NX_SSPSPI_PROTOCOL_SPI);
	NX_SSPSPI_SetSPIFormat(port, plat->format);
	NX_SSPSPI_SetClockPolarityInvert( port, CFALSE);
	NX_SSPSPI_SetSlaveMode(port, CFALSE);
	NX_SSPSPI_SetByteSwap(port, CFALSE);
	
	NX_SSPSPI_SetInterruptEnableAll(port, CFALSE);
	NX_SSPSPI_ClearInterruptPendingAll(port);

	/* SPI resource */
	INIT_WORK(&par->work, do_spi_workqueue);
	INIT_LIST_HEAD(&par->msg_queue);
	spin_lock_init(&par->lock);

	par->work_queue = create_singlethread_workqueue(SPI_DEV_NAME);
	if (NULL == par->work_queue)
		return -ENOMEM;

	ret = request_irq(par->bus.irqno, do_spi_irqhandler, IRQF_DISABLED, I2C_DEV_NAME, par);
	if (ret) {
		ERROUT("fail, spi:%d request irq %d ...\n", port, par->bus.irqno);
		return ret;
	}

	ret = request_irq(PB_DMA_IRQ(par->dmatxtr->channel), do_dmatx_irqhandler, IRQF_DISABLED, I2C_DEV_NAME, par);
	if (ret) {
		ERROUT("fail, spi dma Tx channel:%d request irq %d ...\n",par->dmatxtr->channel, PB_DMA_IRQ(par->dmatxtr->channel));
		return ret;
	}
	ret = request_irq(PB_DMA_IRQ(par->dmarxtr->channel), do_dmarx_irqhandler, IRQF_DISABLED, I2C_DEV_NAME, par);
	if (ret) {
		ERROUT("fail, spi dma Rx channel :%d request irq %d ...\n", par->dmarxtr->channel,PB_DMA_IRQ(par->dmatxtr->channel));
		return ret;
	}

	init_waitqueue_head(&par->wait_q);
	par->timeout = 1000;
	
	printk(KERN_INFO "spi%d: irq %d, slave cs %d \n", port, par->bus.irqno, par->cs_num);
	printk(KERN_INFO "spi%d: %lu hz [%lu hz ~ %lu hz, pll:%lu] \n",
		port, SPI_CLOCK_CURR, SPI_CLOCK_MIN, SPI_CLOCK_MAX, CFG_SPI_CLK_FREQ);

	return ret;
}

static void	exit_spi_device(struct spi_param *par)
{
	int port = par->bus.port;

	DBGOUT("%s\n", __func__);

	if (par->work_queue) {
		flush_workqueue(par->work_queue);
		destroy_workqueue(par->work_queue);
	}

	if (par->bus.irqno)
		free_irq(par->bus.irqno, par);

	if (PB_DMA_IRQ(par->dmatxtr->channel))
		free_irq(PB_DMA_IRQ(par->dmatxtr->channel), par);
		
	if (PB_DMA_IRQ(par->dmarxtr->channel))
		free_irq(PB_DMA_IRQ(par->dmarxtr->channel), par);
		
	/* SPI prototype */
	NX_SSPSPI_SetEnable(port, CFALSE);
	NX_SSPSPI_SetClockDivisorEnable(port, CFALSE);
	NX_SSPSPI_SetClockPClkMode(port, NX_PCLKMODE_DYNAMIC);
}

static int set_spi_device(struct spi_param *par, int chip_select, unsigned int speed, int bpw)
{
	struct spi_cs_info *csi = &par->cs_info[chip_select];

	unsigned int mode = par->tr_mode;
	int port = par->bus.port;

	DBGOUT("%s: speed=%d, bits/w=%d, mode=0x%x\n", __func__, speed, bpw, mode);

	if ( !( (speed && (csi->speed_hz  != speed)) ||
		    (bpw   && (csi->bits_word != bpw  )) ||
		    (mode  && (csi->cs_mode	  != mode )) )) {
		return 0;
	}

	if (mode & SPI_3WIRE) {
		printk(KERN_ERR "Not support 3-wire spi mode\n");
		return -EINVAL;
	}

	NX_SSPSPI_SetEnable(port, CFALSE);
	NX_SSPSPI_SetClockDivisorEnable(port, CFALSE);

	if (mode && (csi->cs_mode != mode)) {
		NX_SSPSPI_SetSPIFormat(port, (mode & SPI_CPHA) ? NX_SSPSPI_FORMAT_B : NX_SSPSPI_FORMAT_A);
		NX_SSPSPI_SetClockPolarityInvert(port, (mode & SPI_CPOL) ? CFALSE : CTRUE);
	}

	if (bpw && (csi->bits_word != bpw))
		NX_SSPSPI_SetBitWidth(port, bpw);

	if (speed && (csi->speed_hz != speed)) {
		unsigned int div = CFG_SPI_CLK_FREQ/CFG_SPI_CLOCK_DIVISOR/speed;
		NX_SSPSPI_SetClockSource(port, 0, CFG_SPI_CLK_SRC);
		NX_SSPSPI_SetClockDivisor(port, 0, CFG_SPI_CLOCK_DIVISOR);
 		NX_SSPSPI_SetDividerCount(port, div);
	}

	NX_SSPSPI_SetClockDivisorEnable(port, CTRUE);
	NX_SSPSPI_SetEnable(port, CTRUE);

	/* save spi setup value */
	csi->speed_hz  = speed;
	csi->bits_word = bpw;
	par->tr_mode   = csi->cs_mode;

	printk(KERN_INFO "%s.%d: %8d hz [%lu hz ~ %lu hz, pll:%lu] \n",
		SPI_DEV_NAME, port, speed, SPI_CLOCK_MIN, SPI_CLOCK_MAX, CFG_SPI_CLK_FREQ);
	return 0;
}

static void set_spi_chip_select(struct spi_param *par, int chip_select, int enable)
{
	struct spi_io_cs   *io  = &par->bus.io_cs[chip_select];
	struct spi_cs_info *csi = &par->cs_info[chip_select];

	unsigned int gpio = io->pad;
	unsigned int func = io->func;
	unsigned int mode = csi->cs_mode;

	DBGOUT("%s: cs=%d, gpio=%3u, func=0x%x, enable=%d\n",
		__func__, chip_select, gpio, func, enable);

	if (func == NX_GPIO_PADFUNC_GPIO) {
		if (mode & SPI_CS_HIGH)
			soc_gpio_set_out_value(gpio, enable ? 1 : 0);
			
		else
			soc_gpio_set_out_value(gpio, enable ? 0 : 1);
			
	}
}

static inline int spi_wait_ready(struct spi_param *par, int tx)
{
	int port = par->bus.port;

	if (tx)
		while (CFALSE == NX_SSPSPI_IsTxFIFOEmpty(port)) { ; }
	else
		while (CTRUE  == NX_SSPSPI_IsRxFIFOEmpty(port)) { ; }

	return 0;
}

static inline int spi_readnwrite(struct spi_device *spi, const u8 *tx_buf, u8 *rx_buf, int count)
{
	struct spi_param *par = spi_master_get_devdata(spi->master);
	int port = par->bus.port;
	int ret;
	int i;
	
//	NX_ASSERT(64 >= count);
	NX_ASSERT(CFALSE == NX_SSPSPI_GetEnable(port));

	NX_SSPSPI_SetDMATransferMode(port, CFALSE);


 #if 1//DMA Transfer Mode

	NX_SSPSPI_ResetFIFO(port );
	NX_SSPSPI_SetDMATransferMode(port, CTRUE);
	NX_SSPSPI_SetBitWidth(port, 8);

	if(tx_buf)
		memcpy(par->dma_writeBuffer,tx_buf,count);
	else
		memset(par->dma_writeBuffer, 0, count);

	par->dmatxtr->tr_type = DMA_TRANS_MEM2IO;
	par->dmatxtr->srcbase = par->waddr;
	par->dmatxtr->dstbase = NX_SSPSPI_GetPhysicalAddress(port);
	par->dmatxtr->requestid = NX_SSPSPI_GetDMAIndex_Tx(port);
	par->dmatxtr->bitwidth = NX_SSPSPI_GetDMABusWidth(port);
	par->dmatxtr->length = count;

	par->dmarxtr->tr_type = DMA_TRANS_IO2MEM;
	par->dmarxtr->srcbase = NX_SSPSPI_GetPhysicalAddress(port);
	par->dmarxtr->dstbase = par->raddr;
	par->dmarxtr->requestid = NX_SSPSPI_GetDMAIndex_Rx(port);
	par->dmarxtr->bitwidth = NX_SSPSPI_GetDMABusWidth(port);
	par->dmarxtr->length = count;

	par->cond = 0;
			
	ret = soc_dma_transfer(par->dmatxtr);
	if(ret > 0)
	{
		printk("DMA Transfer Error  %s :(%d) \n",__func__,__LINE__);	
		return -1;
	}
	ret	= soc_dma_transfer(par->dmarxtr);
	if(ret > 0)
	{
		printk("DMA Transfer Error  %s :(%d) \n",__func__,__LINE__);	
		return -1;
	}
	NX_SSPSPI_SetEnable(port, CTRUE);
//	set_spi_chip_select(par, spi->chip_select, 1);
			
			
 	ret = wait_event_interruptible_timeout(par->wait_q, par->cond, par->timeout);
 	if(0 == par->cond)
	{
		printk("DMA Transfer Error  %s :(%d) ret : %x \n",__func__,__LINE__,ret);	
		set_spi_chip_select(par, spi->chip_select, 0);
		return -1;
	}

	NX_SSPSPI_SetEnable(port, CFALSE);
//	set_spi_chip_select(par, spi->chip_select, 0);
	
	if(rx_buf)
		memcpy(rx_buf,par->dma_readBuffer,count);

	//NX_SSPSPI_SetDMATransferMode(port, CFALSE);
#endif
#if 0 //Polling mode 

	for (i = 0; count > i; i++)	
	{
		printk("tx_buf : %d %d %d \n",i,tx_buf[i],port);
			NX_SSPSPI_PutByte(port, tx_buf[i]);
		if (0 > spi_wait_ready(par, 1)) {
			dev_err(&spi->dev, "TXS timed out\n");
			return -1;
			}
			tx_buf[i] = NX_SSPSPI_GetByte(port);
			spi_wait_ready(par, 0);
			printk("tx_buf : %d %d\n",i,tx_buf[i]);
	}
	//NX_SSPSPI_SetEnable(port, CTRUE);

	if (0 > spi_wait_ready(par, 1)) {
		dev_err(&spi->dev, "TXS timed out\n");
		return -1;
	}
#endif
	return 1;
}

static unsigned int trans_spi_data(struct spi_device *spi, struct spi_transfer *tr)
{
	int count, ret;
	const u8 *tx;
	u8 *rx;

	tx    = tr->tx_buf;
	rx    = tr->rx_buf;

	/* max transfer size limited by DMA engine */
	count = (tr->len < NX_DMA_MAX_TRANSFER_SIZE) ? tr->len : NX_DMA_MAX_TRANSFER_SIZE;

	ret = spi_readnwrite(spi, tx, rx, count);

	if (0 > ret)
		goto out;

out:
	return count;
}

static int setup_spi_trans(struct spi_device *spi, struct spi_transfer *tr)
{
	struct spi_param *par = NULL;
	unsigned int speed = spi->max_speed_hz;
	unsigned int bpw   = spi->bits_per_word;
	int chip_select	   = spi->chip_select;

	par = spi_master_get_devdata(spi->master);

	if ((tr != NULL) && tr->speed_hz)
		speed = tr->speed_hz;

	if ((tr != NULL) && tr->bits_per_word)
		bpw = tr->bits_per_word;

	return set_spi_device(par, chip_select, speed, bpw);
}



static irqreturn_t do_dmatx_irqhandler(int irqno, void *dev_id)
{	
	struct spi_param *par = dev_id;
	DBGOUT("------------ %s-------------\n",__func__);
	NX_SSPSPI_ClearInterruptPendingAll(par->bus.port);

	return IRQ_HANDLED;
}
static irqreturn_t do_dmarx_irqhandler(int irqno, void *dev_id)
{
	struct spi_param *par = dev_id;
	DBGOUT("------------ %s-------------\n",__func__);
	par->cond= 1;
	wake_up(&par->wait_q);
	
	NX_SSPSPI_ClearInterruptPendingAll(par->bus.port);
	
	return IRQ_HANDLED;
}

static irqreturn_t do_spi_irqhandler(int irqno, void *dev_id)
{
	DBGOUT("------------ %s-------------\n",__func__);
	return IRQ_HANDLED;
}

static void do_spi_workqueue(struct work_struct *work)
{
	struct spi_param *par = container_of(work, struct spi_param, work);

	spin_lock_irq(&par->lock);

	while (! list_empty(&par->msg_queue)) {
		struct spi_message 	*msg;
		struct spi_device 	*spi;
		struct spi_transfer *tr = NULL;
		int chip_select;
		int cs_active   = 0;
		int status      = 0;
				
		msg = container_of(par->msg_queue.next, struct spi_message, queue);
		list_del_init(&msg->queue);
		spin_unlock_irq(&par->lock);

		spi = msg->spi;
		chip_select = spi->chip_select;

		/* Load defaults */
		status = setup_spi_trans(spi, NULL);
		if (0 > status)
			goto msg_done;

		list_for_each_entry(tr, &msg->transfers, transfer_list) {
			
			if (tr->speed_hz || tr->bits_per_word) {
				status = setup_spi_trans(spi, tr);
				if (0 > status)
					break;
			}
			
			if (! cs_active) {
				set_spi_chip_select(par, chip_select, 1);
				cs_active = 1;
			}
			

			if (tr->len)
				msg->actual_length += trans_spi_data(spi, tr);

			if (tr->delay_usecs)
				udelay(tr->delay_usecs);

			if (tr->cs_change) {
				set_spi_chip_select(par, chip_select, 0);
				cs_active = 0;
			}
		}

msg_done:
		if (cs_active)
			set_spi_chip_select(par, chip_select, 0);

		msg->status = status;
		msg->complete(msg->context);

		spin_lock_irq(&par->lock);
	}

	spin_unlock_irq(&par->lock);
}

/*------------------------------------------------------------------------------
 * 	SPI master functions
 */
static int nx_spi_setup(struct spi_device *spi)
{
	struct spi_param   *par = NULL;
	struct spi_cs_info *csi = NULL;

	par = spi_master_get_devdata(spi->master);
	csi  = &par->cs_info[spi->chip_select];

	DBGOUT("setup spi%d.%d, mode=0x%x, %s%s%s%s, %u bits/w, %u Hz max \n",
			par->bus.port, spi->chip_select,
			(spi->mode & (SPI_CPOL | SPI_CPHA)),
			(spi->mode & SPI_CS_HIGH) ? "cs_high, " : "",
			(spi->mode & SPI_LSB_FIRST) ? "lsb, " : "",
			(spi->mode & SPI_3WIRE) ? "3wire, " : "",
			(spi->mode & SPI_LOOP) ? "loopback, " : "",
			spi->bits_per_word, spi->max_speed_hz);

	if (spi->bits_per_word > 16 || 1 > spi->bits_per_word ) {
		ERROUT("invalid bits_per_word=%d\n", spi->bits_per_word);
		return -EINVAL;
	}

	if (spi->max_speed_hz == 0)
		spi->max_speed_hz = par->bus.max_clk;

	if (spi->max_speed_hz > par->bus.max_clk)
		spi->max_speed_hz = par->bus.max_clk;

	if (spi->max_speed_hz < par->bus.min_clk)
		spi->max_speed_hz = par->bus.min_clk;

	csi->cs_mode = spi->mode;

	/*
	 * NOTE: cannot change speed and other hw settings immediately,
	 *       otherwise sharing of spi bus is not possible,
	 *       so do not call setupxfer(spi, NULL) here
	 */
	return 0;
}

static int nx_spi_transfer(struct spi_device *spi, struct spi_message *msg)
{
	struct spi_param 	*par;
	struct spi_transfer *tr = NULL;
	unsigned long flags;

	msg->actual_length = 0;
	msg->status 	   = 0;

	/* reject invalid messages and transfers */
	if (list_empty(&msg->transfers) || !msg->complete)
		return -EINVAL;

	par = spi_master_get_devdata(spi->master);

	list_for_each_entry(tr, &msg->transfers, transfer_list) {
		unsigned int bits_per_word = spi->bits_per_word;
			
	if (tr->tx_buf == NULL && tr->rx_buf == NULL && tr->len) {
			ERROUT("missing rx and tx buf\n");
			goto err_trans;
		}

		if ((tr != NULL) && tr->bits_per_word)
			bits_per_word = tr->bits_per_word;

		if ((bits_per_word > 16) || (1 > bits_per_word)) {
			ERROUT("invalid transfer %d bits\n", bits_per_word);
			goto err_trans;
		}

		if ((tr != NULL) && tr->speed_hz &&
		   (par->bus.min_clk > tr->speed_hz || tr->speed_hz > par->bus.max_clk)) {
			ERROUT("over speed %d HZ min (%d Hz) max(%d Hz)\n",
				tr->speed_hz, par->bus.min_clk, par->bus.max_clk);
			goto err_trans;
		}
	}

	spin_lock_irqsave(&par->lock, flags);
	list_add_tail(&msg->queue, &par->msg_queue);
	queue_work(par->work_queue, &par->work);
	spin_unlock_irqrestore(&par->lock, flags);

	return 0;

err_trans:
	/* Message rejected and not queued */
	msg->status = -EINVAL;
	if (msg->complete)
		msg->complete(msg->context);

	return -EINVAL;
}

static int nx_spi_drv_probe(struct platform_device *pdev)
{
	struct spi_master    * master;
	struct spi_param     * par;
	struct nxp3200_spi_plat_data * plat = pdev->dev.platform_data;
	int ret = 0;
	
	DBGOUT("%s\n", __func__);

	master = spi_alloc_master(&pdev->dev, sizeof *par);
	if (master == NULL) {
		printk(KERN_ERR "fail, %s master allocation failed ...\n", pdev->name);
		return -ENOMEM;
	}
	par = spi_master_get_devdata(master);
	par->master	= master;

	/* init spi data struct */
	ret = init_spi_device(par, plat);
	if (ret)
		goto err_mem;

	master->bus_num 		= par->bus.port;
	master->mode_bits 		= SPI_CPOL | SPI_CPHA | SPI_CS_HIGH | SPI_LSB_FIRST;
	master->setup 			= nx_spi_setup;
	master->transfer 		= nx_spi_transfer;
	master->num_chipselect 	= par->cs_num;

	ret = spi_register_master(master);
	if (0 > ret)
		goto err_out;

	platform_set_drvdata(pdev, master);

	return ret;

err_out:
	spi_master_put(master);

err_mem:
	kfree(master);
	return ret;
}

static int nx_spi_drv_remove(struct platform_device *pdev)
{
	struct spi_master *master;
	struct spi_param  *par;

	master = dev_get_drvdata(&pdev->dev);
	par    = spi_master_get_devdata(master);

	exit_spi_device(par);

	dma_free_coherent(par->txdev,NX_DMA_MAX_TRANSFER_SIZE,par->dma_writeBuffer,par->waddr);
	dma_free_coherent(par->rxdev,NX_DMA_MAX_TRANSFER_SIZE,par->dma_readBuffer,par->raddr);

	soc_dma_release(par->dmatxtr);
	soc_dma_release(par->dmarxtr);

	spi_unregister_master(master);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver spi_plat_driver = {
	.probe		= nx_spi_drv_probe,
	.remove		= nx_spi_drv_remove,
	.driver 	= {
		.owner	= THIS_MODULE,
		.name	= SPI_DEV_NAME,
	},
};

static int __init nx_spi_drv_init(void)
{
	return platform_driver_register(&spi_plat_driver);
}

static void __exit nx_spi_drv_exit(void)
{
	platform_driver_unregister(&spi_plat_driver);
}

module_init(nx_spi_drv_init);
module_exit(nx_spi_drv_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("SPI driver for the Nexell");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:nx-spi");
