/* SD/SDIO Host Controller Driver for MagicEyes SoCs
 *
 * Copyright (c) 2010 Leapfrog Enterprises Inc.
 *
 * Scott Esters <sesters@leapfrog.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This host controller supports:
 * - MMC spec 4.2
 * - SD memory card spec 2.0
 * - SDIO card spec 1.10
 * - 1-bit and 4-bit data bus modes
 * - PIO and DMA (PIO not implemented)
 * - up to 50MHz bus clock
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio.h>
#include <linux/io.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>

#include <asm/sizes.h>

#include <asm-generic/gpio.h>
#include <linux/lf2000/gpio.h>

#include <mach/platform.h>
#include <mach/common.h>
#include <mach/soc.h>
#include <mach/devices.h>

#include "mes_sdhc.h"

#if !defined(CONFIG_MMC_NEXELL_SDHC0) && \
    !defined(CONFIG_MMC_NEXELL_SDHC1)
#warning "No host controllers enabled.  Enable at least one."
#endif

#define RESSIZE(res) (((res)->end - (res)->start)+1)

#define MAX_CHANNELS	2

#define COMPLETION_TIMEOUT	(2*1000)
#define DRIVER_NAME		"mes-sdhc"
#define PLL1			1
#define SDIO_CLK_SRC		PLL1
#define	SDIO_CLK_DIV		3
#define DIV_400KHZ	62	/* divider for 400KHz */
#define SDIO_DIV		0	/* divider for full speed */

#define SDIO_GPIO_PORT	GPIO_PORT_B
#define SDIO_GPIO_FUNC	GPIO_ALT1

/* TODO: these are platform resources, move them and/or integrate with the
 * GPIO framework and request them */
static unsigned sdio_pins[MAX_CHANNELS][6] = {
	{
		LF2000_GPIO_PORT_B | 2,  /* Data 0 */
		LF2000_GPIO_PORT_B | 3,  /* Data 1 */
		LF2000_GPIO_PORT_B | 4,  /* Data 2 */
		LF2000_GPIO_PORT_B | 5,  /* Data 3 */
		LF2000_GPIO_PORT_B | 0,  /* Clock */
		LF2000_GPIO_PORT_B | 1,  /* Command */
	},
	{
		LF2000_GPIO_PORT_B | 8,  /* Data 0 */
		LF2000_GPIO_PORT_B | 9,  /* Data 1 */
		LF2000_GPIO_PORT_B | 10, /* Data 2 */
		LF2000_GPIO_PORT_B | 11, /* Data 3 */
		LF2000_GPIO_PORT_B | 6,  /* Clock */
		LF2000_GPIO_PORT_B | 7,  /* Command */
	},
};

extern unsigned int cpu_get_clock_hz(int clk);

struct sdhc_dma_sg {
	int 			count;
	struct scatterlist	*sg;
};

struct mes_sdio_host {
	struct mmc_host		*mmc;
	struct resource 	*mem;
	void __iomem		*base;
	struct platform_device	*pdev;

	u8			channel;
	struct dma_trans	*sd_dma_tr;
	struct sdhc_dma_sg	sd_sg;
	bool			dma_active;
	enum dma_data_direction	dma_dir;
	int			dma_nents;
	int			irq;
	int			div;
	u32			clock_hz;
	u8			bus_width;
	unsigned char		power_mode;
	bool			sdio_irq_en;

	struct mmc_request	*mrq;
	struct mmc_data		*data;

	struct completion	dma_transfer;

	spinlock_t		lock;

	/* debugging */
	struct dentry		*debug;
};

static void mes_regs_show_reg(struct seq_file *s, const char *nm, u32 reg)
{
	struct mes_sdio_host *host = s->private;

	seq_printf(s, "%9s:  0x%08X\n", nm, readl(host->base + reg));
}

static int mes_regs_show(struct seq_file *s, void *v)
{
	struct mes_sdio_host *host = s->private;

	seq_printf(s, "%9s:  %u\n", "CHANNEL", host->mmc->index);
	seq_printf(s, "%9s:  0x%p\n", "ADDRESS", host->base);
	seq_printf(s, "\n");
	mes_regs_show_reg(s, "CTRL", SDI_CTRL);
	mes_regs_show_reg(s, "CLKDIV", SDI_CLKDIV);
	mes_regs_show_reg(s, "CLKENA", SDI_CLKENA);
	mes_regs_show_reg(s, "TMOUT", SDI_TMOUT);
	mes_regs_show_reg(s, "CTYPE", SDI_CTYPE);
	mes_regs_show_reg(s, "BLKSIZ", SDI_BLKSIZ);
	mes_regs_show_reg(s, "BYTCNT", SDI_BYTCNT);
	mes_regs_show_reg(s, "INTMASK", SDI_INTMASK);
	mes_regs_show_reg(s, "CMDARG", SDI_CMDARG);
	mes_regs_show_reg(s, "CMD", SDI_CMD);
	mes_regs_show_reg(s, "RESP0", SDI_RESP0);
	mes_regs_show_reg(s, "RESP1", SDI_RESP1);
	mes_regs_show_reg(s, "RESP2", SDI_RESP2);
	mes_regs_show_reg(s, "RESP3", SDI_RESP3);
	mes_regs_show_reg(s, "MINTSTS", SDI_MINTSTS);
	mes_regs_show_reg(s, "RINTSTS", SDI_RINTSTS);
	mes_regs_show_reg(s, "STATUS", SDI_STATUS);
	mes_regs_show_reg(s, "FIFOTH", SDI_FIFOTH);
	mes_regs_show_reg(s, "TCBCNT", SDI_TCBCNT);
	mes_regs_show_reg(s, "TBBCNT", SDI_TBBCNT);
	mes_regs_show_reg(s, "DAT", SDI_DAT);
	mes_regs_show_reg(s, "SYSCLKENB", SDI_SYSCLKENB);
	mes_regs_show_reg(s, "CLKGEN", SDI_CLKGEN);

	return 0;
}

static void mes_status_show_bit(struct seq_file *s, const char *nm, u32 v)
{
	seq_printf(s, "%10s:\t%d\n", nm, !!v);
}

static void mes_status_show_hex(struct seq_file *s, const char *nm, u32 v)
{
	seq_printf(s, "%10s:\t0x%X\n", nm, v);
}

static int mes_status_show(struct seq_file *s, void *v)
{
	struct mes_sdio_host *host = s->private;
	u32 status = readl(host->base + SDI_STATUS);

	mes_status_show_bit(s, "DMAREQ", status & (1<<DMAREQ));
	mes_status_show_bit(s, "DMAACK", status & (1<<DMAACK));
	mes_status_show_hex(s, "FIFOCOUNT", (status>>FIFOCOUNT) & 0x1F);
	mes_status_show_hex(s, "RSPINDEX", (status>>RSPINDEX) & 0x3F);
	mes_status_show_bit(s, "FSMBUSY", status & (1<<FSMBUSY));
	mes_status_show_bit(s, "DATBUSY", status & (1<<DATBUSY));
	mes_status_show_bit(s, "CPRESENT", status & (1<<CPRESENT));
	mes_status_show_hex(s, "CMDFSM", (status>>CMDFSM) & 0xF);
	mes_status_show_bit(s, "FIFOFULL", status & (1<<FIFOFULL));
	mes_status_show_bit(s, "FIFOEMPTY", status & (1<<FIFOEMPTY));
	mes_status_show_bit(s, "TXWMARK", status & (1<<TXWMARK));
	mes_status_show_bit(s, "RXWMARK", status & (1<<RXWMARK));

	return 0;
}

static int mes_sdio_regs_open(struct inode *inode, struct file *file)
{
	return single_open(file, mes_regs_show, inode->i_private);
}

static int mes_sdio_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, mes_status_show, inode->i_private);
}

static const struct file_operations mes_sdio_regs_fops = {
	.owner		= THIS_MODULE,
	.open		= mes_sdio_regs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const struct file_operations mes_sdio_status_fops = {
	.owner		= THIS_MODULE,
	.open		= mes_sdio_status_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void mes_sdio_init_debugfs(struct mes_sdio_host *host)
{
	struct dentry *dir;
       
	dir = debugfs_create_dir(dev_name(mmc_dev(host->mmc)), NULL);
	if (!dir || IS_ERR(dir)) {
		host->debug = NULL;
		return;
	}

	debugfs_create_file("registers", S_IRUSR, dir, host,
			&mes_sdio_regs_fops);
	debugfs_create_file("status", S_IRUSR, dir, host,
			&mes_sdio_status_fops);

	host->debug = dir;
	
}

/*
 * Hardware helpers.
 */

static void mes_sdio_reset_controller(struct mes_sdio_host *host)
{
	u32 tmp = readl(host->base + SDI_CTRL);

	tmp &= ~((1<<DMARST)|(1<<FIFORST));
	tmp |= (1<<CTRLRST);
	writel(tmp, host->base + SDI_CTRL);

	while (readl(host->base + SDI_CTRL) & (1<<CTRLRST));
	dev_dbg(&host->pdev->dev, "Finished resetting controller\n");
}

static void mes_sdio_set_dma(struct mes_sdio_host *host, bool en)
{
	u32 tmp = readl(host->base + SDI_CTRL) & ~(1<<DMA_ENA);

	writel(tmp | (en<<DMA_ENA), host->base + SDI_CTRL);
}

static void mes_sdio_reset_dma(struct mes_sdio_host *host)
{
	u32 tmp = readl(host->base + SDI_CTRL);

	tmp &= ~((1<<CTRLRST)|(1<<FIFORST));
	tmp |= (1<<DMARST);
	writel(tmp, host->base + SDI_CTRL);

	while (readl(host->base + SDI_CTRL) & (1<<DMARST));
	dev_dbg(&host->pdev->dev, "Finished resetting dma\n");
}

static void mes_sdio_reset_fifo(struct mes_sdio_host *host)
{
	u32 tmp = readl(host->base + SDI_CTRL);

	tmp &= ~((1<<DMARST)|(1<<CTRLRST));
	tmp |= (1<<FIFORST);
	writel(tmp, host->base + SDI_CTRL);

	while (readl(host->base + SDI_CTRL) & (1<<FIFORST));
	dev_dbg(&host->pdev->dev, "Finished resetting fifo\n");
}

/* Set the RX and TX FIFO thresholds.  The FIFOs are 64 bytes (16 words) long
 * and the recommended values are a TX threshold of (length/2) or 8 and an RX
 * threshold of ((length/2)-1) or 7. */
static void mes_sdio_set_fifo_th(struct mes_sdio_host *host)
{
	u32 tmp = readl(host->base + SDI_FIFOTH) & ~((0xF<<RXTH)|(0x3<<TXTH));

	tmp |= (7<<RXTH)|(8<<TXTH);

	writel(tmp, host->base + SDI_FIFOTH);
}

static void mes_sdio_set_width(struct mes_sdio_host *host, u8 width)
{
	u32 tmp = readl(host->base + SDI_CTYPE);

	if (width == MMC_BUS_WIDTH_4)
		tmp |= (1<<WIDTH);
	else
		tmp &= ~(1<<WIDTH);

	writel(tmp, host->base + SDI_CTYPE);
}

static inline void mes_sdio_clear_int_status(struct mes_sdio_host *host)
{
	writel(0xFFFFFFFF, host->base + SDI_RINTSTS);
}

static void mes_sdio_interrupt_enable(struct mes_sdio_host *host)
{
	u32 tmp = readl(host->base + SDI_CTRL);
	tmp |= (1<<INT_ENA)/*|(1<<SEND_IRQ_RESP)*/;
	writel(tmp, host->base + SDI_CTRL);
}

static int check_command_error(struct mes_sdio_host *host, u32 irqm)
{
	if (irqm & (1<<HLEINT)) {
		dev_err(&host->pdev->dev, "HW locked error\n");
		return -EIO;
	}

	if (irqm & (1<<REINT)) {
		dev_err(&host->pdev->dev, "response error\n");
		return -EIO;
	}

	if (irqm & (1<<RCRCINT)) {
		dev_err(&host->pdev->dev, "bad CRC\n");
		return -EILSEQ;
	}

	if (irqm & (1<<RTOINT)) {
		/* This happens when the stack probes for different types of
		 * cards and is expected.  For example, an SD card will time
		 * out when probed as an SDIO card. */
		dev_dbg(&host->pdev->dev, "response timeout\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static int check_data_error(struct mes_sdio_host *host, u32 irqm)
{
	if (irqm & (1<<FRUNINT)) {
		dev_err(&host->pdev->dev, "FIFO error\n");
		return -EIO;
	}
	
	if (irqm & (1<<DRTOINT)) {
		dev_err(&host->pdev->dev, "data read response timeout\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static void mes_sdio_setup_controller(struct mes_sdio_host *host)
{
	dev_dbg(&host->pdev->dev, "%s\n", __FUNCTION__);

	/* Turn off clock output, turn off clock low power mode.  The clock
	 * output will be enabled later by the MMC subsystem. */
	writel(0, host->base + SDI_CLKENA);

	/* use PLL1/3, 147MHz/3 = 49MHz */
	writel((2<<CLKSRCSEL0)|((SDIO_CLK_DIV-1)<<CLKDIV0),
			host->base + SDI_CLKGEN);
	writel((1<<PCLKMODE)|(1<<CLKGENENB), host->base + SDI_SYSCLKENB);
	/* set SDIO clock to "detect" rate, ~400KHz:
	 * 49MHz/(62*2) = ~400KHz */
	writel(62, host->base + SDI_CLKDIV);

	mes_sdio_interrupt_enable(host);

	mes_sdio_reset_controller(host);
	mes_sdio_reset_dma(host);
	mes_sdio_reset_fifo(host);

//	mes_sdio_set_dma(host, 1);
	mes_sdio_set_dma(host, 0);

	mes_sdio_set_width(host, MMC_BUS_WIDTH_1);

	/* Set host data and response timeouts. */

#if 0	// 17mar11
//ok	writel((0x800000<<DTMOUT)|(0x80<<RSPTMOUT), host->base + SDI_TMOUT);
//ok	writel((0x400000<<DTMOUT)|(0x40<<RSPTMOUT), host->base + SDI_TMOUT);
	writel((0x200000<<DTMOUT)|(0x20<<RSPTMOUT), host->base + SDI_TMOUT);
//ok	writel((0x100000<<DTMOUT)|(0x10<<RSPTMOUT), host->base + SDI_TMOUT);
//FAILS	writel((0x080000<<DTMOUT)|(0x08<<RSPTMOUT), host->base + SDI_TMOUT);
#else	// original
	writel((0xFFFFFF<<DTMOUT)|(0xFF<<RSPTMOUT), host->base + SDI_TMOUT);
#endif	// 17mar11

	/* Set a defult block size: 512B is typical for MMC/SD cards. */
	writel(512, host->base + SDI_BLKSIZ);

	mes_sdio_set_fifo_th(host);

	/* Disable interrupts and clear any pending. */
	writel(0, host->base + SDI_INTMASK);
	mes_sdio_clear_int_status(host);
}

static void mes_sdio_clock_disable(struct mes_sdio_host *host)
{
	u32 reg = readl(host->base + SDI_SYSCLKENB);
	
	reg &= ~(1<<CLKGENENB);

	writel(reg, host->base + SDI_SYSCLKENB);
}

static inline bool mes_sdio_card_busy(struct mes_sdio_host *host)
{
	u32 reg = readl(host->base + SDI_STATUS);

	dev_dbg(&host->pdev->dev, "%s\n", __FUNCTION__);

	return !!(reg & ((1<<FSMBUSY)|(1<<DATBUSY)));
}

static void mes_sdio_enable_irq(struct mmc_host *mmc, int enable)
{
	u32 irqm = 0;
	struct mes_sdio_host *host = mmc_priv(mmc);

	irqm = readl(host->base + SDI_INTMASK);
	if(enable)
		irqm |= (1<<MSKSDIOINT);
	else
		irqm &= ~(1<<MSKSDIOINT);
	writel(irqm, host->base + SDI_INTMASK);
	host->sdio_irq_en = enable;
}

/*
 * Command and data transfer.
 */

/* Read the command result and, if there is a data stage and we are writing,
 * launch a DMA request.  If the command resulted in an error or if there is
 * no data stange, we are done handling this command and we report that to the
 * stack.  Otherwise we need to wait for the data stage to complete before we
 * report to the stack that the request is complete. */
static int mes_sdio_command_complete(struct mes_sdio_host *host)
{
	struct mmc_request *mrq = host->mrq;
	
	if (mrq->cmd->flags & MMC_RSP_PRESENT) {
		if (mrq->cmd->flags & MMC_RSP_136) {
			mrq->cmd->resp[0] = readl(host->base + SDI_RESP3);
			mrq->cmd->resp[1] = readl(host->base + SDI_RESP2);
			mrq->cmd->resp[2] = readl(host->base + SDI_RESP1);
			mrq->cmd->resp[3] = readl(host->base + SDI_RESP0);
		} else
			mrq->cmd->resp[0] = readl(host->base + SDI_RESP0);
	}

	if (mrq->cmd->error || !mrq->data) {
		/* IRQ context - safe to modify */
		host->mrq = NULL;
		mmc_request_done(host->mmc, mrq);
		
		return 1;
	}

	if (mrq->data && (mrq->data->flags & MMC_DATA_WRITE)) {
		dev_dbg(&host->pdev->dev, "launching DMA TX\n");
		soc_dma_transfer(host->sd_dma_tr);
		host->sd_sg.sg++;
		host->sd_sg.count--;
	}

	return 0;
}

static void mes_sdio_dma_done(struct mes_sdio_host *host)
{
	host->dma_active = 0;
	dma_unmap_sg(mmc_dev(host->mmc), host->data->sg, host->dma_nents,
			host->dma_dir);
	host->data = NULL;
	complete(&host->dma_transfer);
}

static void mes_sdio_transfer_complete(struct mes_sdio_host *host)
{
	struct mmc_data *data = host->mrq->data;

	dev_dbg(&host->pdev->dev, "data transfer complete\n");

	if (!data)
		return;

	if (data->error) {
		dev_err(&host->pdev->dev, "data error\n");
		if (host->dma_active) {
			soc_dma_trans_stop(host->sd_dma_tr);
			soc_dma_wait_ready(host->sd_dma_tr, 1000*1000);
			mes_sdio_set_dma(host, 0);
			mes_sdio_dma_done(host);
		}
	} else {
		data->bytes_xfered = data->blocks * data->blksz;
	}

	/* Complete the command now that the data transfer portion has
	 * finished. */
	/* IRQ context - safe to modify */
	host->mrq = NULL;
	mmc_request_done(host->mmc, data->mrq);
	return;
}

static irqreturn_t mes_sdio_dma_cb(int irq, void *dev_id)
{
	struct mes_sdio_host *host = (struct mes_sdio_host *)dev_id;

	WARN_ON(!host->dma_active);

	/* Perform scatter-gather in software */

	/* None left */
	if (!host->sd_sg.count) {
		if (host->dma_active) {
			dev_dbg(&host->pdev->dev, "DMA transfer completed\n");
			soc_dma_trans_stop(host->sd_dma_tr);
			soc_dma_wait_ready(host->sd_dma_tr, 1000*1000);
			mes_sdio_set_dma(host, 0);
			mes_sdio_dma_done(host);
		}
		
		return IRQ_HANDLED;
	}

	if(host->data->flags & MMC_DATA_WRITE) {
		host->sd_dma_tr->srcbase = sg_phys(host->sd_sg.sg);

	} else {
		host->sd_dma_tr->dstbase = sg_phys(host->sd_sg.sg);
	}
	host->sd_dma_tr->length  = host->sd_sg.sg->length;
	soc_dma_transfer(host->sd_dma_tr);

	/* Next buffer */
	host->sd_sg.sg++;
	host->sd_sg.count--;

	return IRQ_HANDLED;
}

static irqreturn_t mes_sdio_irq(int irq, void *dev_id)
{
	struct mes_sdio_host *host = (struct mes_sdio_host *)dev_id;
	u32 irqm = readl(host->base + SDI_MINTSTS);

	dev_dbg(&host->pdev->dev, "IRQ: 0x%08X\n", irqm);

	if (host->mrq) {
		host->mrq->cmd->error = check_command_error(host, irqm);
		if (host->mrq->cmd->error) {
			dev_dbg(&host->pdev->dev, "command error\n");
			/*
			 * If this failed command has retries avaiable, the
			 * core will call .request() in the completion
			 * handler.
			 *
			 * This is triggered, e.g., during mmc_rescan() when
			 * probing for SDIO cards with mmc_send_io_op_cond()
			 *
			 * We must clear interrupts *before* running the
			 * completion, or we risk dropping IRQs from the retry.
			 */

			/*
			 * *** MINTSTS/RINTSTS REGISTER QUIRK: ***
			 *
			 * (1<<RTOINT) is set slightly before (1<<CDINT)
			 * and triggers the ISR.  The consequence is that
			 * the first ISR run will see only RTOINT, and if the
			 * command has retries, the subsequent ISR run will
			 * see only CDINT, falsely interpretting it as success.
			 *
			 * This particularly problematic for SD memory cards
			 * during the SDIO probe, as they appear to respond
			 * to SD_IO_SEND_OP_COND, which is incorrect.
			 *
			 * The inter-bit delay is small, so we just spin
			 * briefly to let the register stabilize, and then
			 * clobber it. TODO: use a tasklet.
			 *
			 * nx_sdhc.h has additional descriptions of the
			 * interrupt bit semantics.
			 *
			 * The LF1000 was slow enough that the CPU never saw
			 * this  bit race.
			 *
			 * Beware: enabling DEBUG will slow everything down and
			 * mask this behavior.
			 */
			udelay(30);
			writel(~(1<<SDIOINT), host->base + SDI_RINTSTS);
			mes_sdio_command_complete(host);
			return IRQ_HANDLED;
		}

		if (irqm & (1<<CDINT)) {
			dev_dbg(&host->pdev->dev, "command done\n");
			if (mes_sdio_command_complete(host))
				goto out_req;
		}

		if (host->mrq->data && (irqm & (1<<DTOINT))) {
			dev_dbg(&host->pdev->dev, "data transfer over\n");
			host->mrq->data->error = check_data_error(host, irqm);
			mes_sdio_transfer_complete(host);
		}
	}

out_req:
	if (irqm & (1<<SDIOINT)) {
		dev_dbg(&host->pdev->dev, "SDIO interrupt occured\n");
		mmc_signal_sdio_irq(host->mmc);
	}

	writel(irqm, host->base + SDI_RINTSTS);
	return IRQ_HANDLED;
}

/* Set up and launch a transfer from the controller to memory. */
static void mes_sdio_start_dma_rx(struct mes_sdio_host *host)
{
	struct dma_trans	*tr = host->sd_dma_tr;
	struct scatterlist *sg = host->mrq->data->sg;

	dev_dbg(&host->pdev->dev, "%s\n", __FUNCTION__);

	if (host->dma_active)
		wait_for_completion(&host->dma_transfer);

	init_completion(&host->dma_transfer);
	host->dma_active = 1;
	host->dma_nents = dma_map_sg(mmc_dev(host->mmc), host->mrq->data->sg,
			host->mrq->data->sg_len, host->dma_dir);

	tr->tr_type	= DMA_TRANS_IO2MEM;
	tr->srcbase	= NX_SDHC_GetPhysicalAddress(host->channel);
	tr->dstbase	= sg_phys(sg);
	tr->length	= sg->length;

	soc_dma_transfer(tr);
	sg++;
	host->sd_sg.sg		= sg;
	host->sd_sg.count	= host->mrq->data->sg_len - 1;
}

/* Set up a transfer from memory to the controller.  The transfer will be 
 * launched once the hardware is ready. */
static void mes_sdio_start_dma_tx(struct mes_sdio_host *host)
{
	struct dma_trans	*tr = host->sd_dma_tr;
	struct scatterlist *sg = host->mrq->data->sg;

	dev_dbg(&host->pdev->dev, "%s\n", __FUNCTION__);

	if (host->dma_active)
		wait_for_completion(&host->dma_transfer);

	init_completion(&host->dma_transfer);
	host->dma_active = 1;
	host->dma_nents = dma_map_sg(mmc_dev(host->mmc), host->mrq->data->sg,
			host->mrq->data->sg_len, host->dma_dir);

	tr->tr_type   = DMA_TRANS_MEM2IO;
	tr->dstbase   = NX_SDHC_GetPhysicalAddress(host->channel);

	tr->srcbase 	  = sg_phys(sg);
	tr->length   	  = sg->length;
	host->sd_sg.sg 	  = sg;
	host->sd_sg.count = host->data->sg_len;
}

static void mes_sdio_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	u32 irqm = 0;
	u32 flags = 0;
	int delay = 0;
	unsigned long irqf;
	struct mes_sdio_host *host = mmc_priv(mmc);
	u8 resp = mmc_resp_type(mrq->cmd);
	dev_dbg(&host->pdev->dev, "%s\n", __FUNCTION__);

	/* NULL for a new command, host->mrq for a retry */
	WARN_ON((host->mrq != NULL) && (host->mrq != mrq));

	/* Purge stale IRQs. */
	while(mes_sdio_card_busy(host))
	{
		if(!in_interrupt())
			schedule();
		if(delay++ > 1000000)
		{
			dev_err(&host->pdev->dev, "%s: busy\n", __FUNCTION__);
			delay = 0;
		}
	}

	/* Unknown context - protect to modify */
	spin_lock_irqsave(&host->lock, irqf);
	host->mrq = mrq;
	spin_unlock_irqrestore(&host->lock, irqf);
	
	flags |= (1<<STARTCMD)|(mrq->cmd->opcode & 0x3F);

	dev_dbg(&host->pdev->dev, "cmd resp flags: 0x%X\n", resp);

	/* Send 80 clocks before the first command for initialization */
	/* TODO: this delay is already present in mmc_power_up() */
	if (mrq->cmd->opcode == MMC_GO_IDLE_STATE ||
	    mrq->cmd->opcode == SD_IO_SEND_OP_COND)
		flags |= (1<<SENDINIT);

	if (mrq->cmd->opcode != MMC_SEND_STATUS)
		flags |= (1<<WAITPRVDAT);

	if (resp & MMC_RSP_PRESENT) { /* expect a response */
		flags |= (1<<RSPEXP);
		irqm |= (1<<MSKRTO);
		if (resp & MMC_RSP_136) /* expect a long response */
			flags |= (1<<RSPLEN);
	}

	if (resp & MMC_RSP_CRC) /* expect valid CRC */
		flags |= (1<<CHKRSPCRC);

	/* We always want to know about command completion, HW errors, and 
	 * response errors. */
	/* TODO: what about EBE, ACD, SBE, RCRC? */
	irqm |= (1<<MSKHLE)|(1<<MSKRE)|(1<<MSKCD);

	/* Preserve the SDIO Interrupt setting. */
	irqm |= (host->sdio_irq_en<<MSKSDIOINT);

	/* Suggests that no card is present */
	if(readl(host->base + SDI_CMD) & (1<<STARTCMD)) {
		mrq->cmd->error = -ENOMEDIUM;
		mrq->cmd->retries = 0;
		spin_lock_irqsave(&host->lock, irqf);
		host->mrq = NULL;
		spin_unlock_irqrestore(&host->lock, irqf);
		mmc_request_done(mmc, mrq);
		return;
	}

	if (mrq->data) {
		int i;
		struct scatterlist *sg;
		u32 length = mrq->data->blocks * mrq->data->blksz;

		/* TODO: handle non-aligned data w/PIO or an intermediate
		 * buffer. */
		for_each_sg(mrq->data->sg, sg, mrq->data->sg_len, i) {
			if(unlikely(mrq->data->sg->offset & 0x3)) {
				dev_err(&host->pdev->dev, "invalid alignment for DMA\n");
				mrq->cmd->error = -EINVAL;
				mrq->cmd->retries = 0;
				spin_lock_irqsave(&host->lock, irqf);
				host->mrq = NULL;
				spin_unlock_irqrestore(&host->lock, irqf);
				mmc_request_done(mmc, mrq);
				return;
			}
		}

		dev_dbg(&host->pdev->dev, "transferring %d bytes\n", length);

		host->data = mrq->data;

		mes_sdio_set_dma(host, 1);

		if (mrq->data->blocks > 1 &&
			(mrq->cmd->opcode == MMC_READ_MULTIPLE_BLOCK ||
			 mrq->cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK)) {
			dev_dbg(&host->pdev->dev, "setting autostop\n");
			flags |= (1<<SENDAUTOSTOP);
			irqm |= (1<<MSKACD);
		}

		writel(mrq->data->blksz, host->base + SDI_BLKSIZ);
		writel(length, host->base + SDI_BYTCNT);

		irqm |= (1<<MSKDTO)|(1<<MSKHTO)|(1<<MSKFRUN);
		flags |= (1<<DATEXP);

		if (mrq->data->flags & MMC_DATA_STREAM) {
			dev_dbg(&host->pdev->dev, "data stream requested\n");
			flags |= (1<<TRMODE);
		}

		if (mrq->data->flags & MMC_DATA_READ) {
			irqm |= (1<<MSKDRTO);
			mes_sdio_start_dma_rx(host);
		}
		
		if (mrq->data->flags & MMC_DATA_WRITE) {
			flags |= (1<<RW);
			mes_sdio_start_dma_tx(host);
		}
	} else if (mrq->stop) {
		flags |= (1<<STOPABORT);
	}
	
	dev_dbg(&host->pdev->dev, "submitting cmd: 0x%04X irqm: 0x%04X\n",
			flags, irqm);

	/* Submit the command. */
	writel(irqm, host->base + SDI_INTMASK);
	writel(mrq->cmd->arg, host->base + SDI_CMDARG);
	writel(flags, host->base + SDI_CMD);
}

static int mes_sdio_get_ro(struct mmc_host *mmc)
{
	/* we don't support RO detection */
	return -ENOSYS;
}

static void mes_sdio_set_clock_out(struct mes_sdio_host *host, bool en)
{
	writel((en<<CLKENA), host->base + SDI_CLKENA);
}

static int mes_sdio_update_clock(struct mes_sdio_host *host)
{
	u32 tout, tmp;

	/* send a clock update command and wait for it to complete, repeat if
	 * a HLEINT occurs */
	while (1) {
		writel((1<<STARTCMD)|(1<<UPDATECLKONLY)|(1<<WAITPRVDAT),
				host->base + SDI_CMD);

		tout = 0;
		while (readl(host->base + SDI_CMD) & (1<<STARTCMD))
			if (++tout > 0x1000000)
				return 1;


		dev_dbg(&host->pdev->dev, 
			"mes_sdio_update_clock: tout %d\n", tout);
		tmp = readl(host->base + SDI_RINTSTS);
		if (!(tmp & (1<<HLEINT)))
			break;

		tmp |= (1<<HLEINT);
		writel(tmp, host->base + SDI_RINTSTS);
	}

	return 0;
}

static int mes_sdio_set_clock_hz(struct mes_sdio_host *host, u32 hz)
{
	int div, rem; 

	dev_dbg(&host->pdev->dev, "%s\n", __FUNCTION__);

	if (hz == 400000)
		div = DIV_400KHZ;
	else if (hz == host->mmc->f_max)
		div = SDIO_DIV;
	else {
		u32 clk_hz = cpu_get_clock_hz(SDIO_CLK_SRC)/SDIO_CLK_DIV;
		div = clk_hz / hz;
		rem = clk_hz % hz;
		if((hz/2) < rem)
			div++;
		if (div < 0)
			return 1;
		div >>= 1;
	}
	dev_dbg(&host->pdev->dev, "setting DIV=%d\n", div);

	/* disable the SDIO clock and set the divider */
	mes_sdio_set_clock_out(host, 0);
	writel(div & 0xFF, host->base + SDI_CLKDIV);
	host->div = div;

	if (mes_sdio_update_clock(host)) {
		dev_err(&host->pdev->dev, "can't set clock: disabled\n");
		return 1;
	}

	mes_sdio_set_clock_out(host, 1);

	if (mes_sdio_update_clock(host)) {
		dev_err(&host->pdev->dev, "can't set clock\n");
		return 1;
	}

	writel((1<<PCLKMODE)|(1<<CLKGENENB), host->base + SDI_SYSCLKENB);

	return 0;
}

static void mes_sdio_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct mes_sdio_host *host = mmc_priv(mmc);

	dev_dbg(&host->pdev->dev, "%s\n", __FUNCTION__);

	if (host->power_mode != MMC_POWER_OFF && mes_sdio_card_busy(host)) {
		dev_err(&host->pdev->dev, "%s.%d card (%p) channel(%d) busy\n",
			__FUNCTION__, __LINE__, host->base, host->channel);
	}
	//Set DAT3 (CS in SPI mode) high if requested or allow MMC controller to drive it if not
	if (ios->chip_select == MMC_CS_HIGH)
	{
		gpio_set_function(sdio_pins[host->channel][3], GPIO_GPIOFN);
		gpio_direction_output(sdio_pins[host->channel][3], 1);
	}
	else
	{	
		gpio_set_function(sdio_pins[host->channel][3], SDIO_GPIO_FUNC);
	}

	if (ios->bus_width != host->bus_width) {
		mes_sdio_set_width(host, ios->bus_width);
		host->bus_width = ios->bus_width;
	}

	if (ios->clock != host->clock_hz) {
		if (ios->clock > 0) {
			if (mes_sdio_set_clock_hz(host, ios->clock))
				dev_err(&host->pdev->dev,
				"%s.%d card (%p) can't set clock rate\n",
				__FUNCTION__, __LINE__, host->base);
			else {
				host->clock_hz = ios->clock;
				dev_dbg(&host->pdev->dev, 
					"mes_sdio_set_ios: clock %d hz\n", 
					host->clock_hz);
			}
		} else
			mes_sdio_clock_disable(host);
	}

	if (ios->power_mode != host->power_mode) {
		switch (ios->power_mode) {
			case MMC_POWER_OFF:
				mes_sdio_set_clock_out(host, 0);
				break;

			case MMC_POWER_ON:
				mes_sdio_set_clock_out(host, 1);
				mes_sdio_reset_dma(host);
				mes_sdio_reset_fifo(host);
				mdelay(5);
				break;

			case MMC_POWER_UP:
				mdelay(5);
				break;

			default:
				break;
		}
		host->power_mode = ios->power_mode;
	}
}

static struct mmc_host_ops mes_sdio_ops = {
	.request 		= mes_sdio_request,
	.get_ro			= mes_sdio_get_ro,
	.set_ios		= mes_sdio_set_ios,
	.enable_sdio_irq	= mes_sdio_enable_irq,
};

static int mes_sdio_probe(struct platform_device *pdev)
{
	struct mmc_host *mmc;
	struct mes_sdio_host *host = NULL;
	struct sdhc_plat_data *plat = pdev->dev.platform_data;
	struct resource *res;
	int ret;

	dev_dbg(&pdev->dev, "%s\n", __FUNCTION__);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENXIO;

	mmc = mmc_alloc_host(sizeof(struct mes_sdio_host), &pdev->dev);
	if (!mmc)
		return -ENOMEM;

	mmc->ops = &mes_sdio_ops;

	mmc->max_segs = 1;
	mmc->max_blk_size = 512;
	mmc->max_blk_count = 128;
	mmc->max_req_size = mmc->max_blk_size * mmc->max_blk_count;
	mmc->max_seg_size = mmc->max_req_size;

	mmc->f_min = 400000;
	mmc->f_max = cpu_get_clock_hz(SDIO_CLK_SRC)/SDIO_CLK_DIV;

	/* TODO: 3.3V/3.0V PAD I/O */
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;

	mmc->caps = MMC_CAP_4_BIT_DATA | MMC_CAP_SD_HIGHSPEED |
		MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SDIO_IRQ | MMC_CAP_NONREMOVABLE;

	if (!request_mem_region(res->start, RESSIZE(res), res->name)) {
		dev_err(&pdev->dev, "failed to get IO memory region\n");
		ret = -EBUSY;
		goto out_mem;
	}

	host = mmc_priv(mmc);
	host->mmc = mmc;
	host->mem = res;
	host->pdev = pdev;
	host->channel = pdev->id;
	host->div = 0;
	host->clock_hz = 0;
	host->dma_active = 0;
	host->bus_width = MMC_BUS_WIDTH_1;
	host->sdio_irq_en = 0;

	/* The MMC core calls set_ios() w/MMC_POWER_OFF during initialization
	 * while setting up the bus.  In this case, power_mode is actually a
	 * "don't care", so we ignore this potentially fatal power-off request
	 * by claiming we are already in that state.
	 */
	host->power_mode = MMC_POWER_OFF;

	host->base = ioremap(res->start, res->end - res->start + 1);
	if (!host->base) {
		dev_err(&pdev->dev, "failed to remap\n");
		ret = -ENOMEM;
		goto out_mem;
	}

	host->irq = platform_get_irq(pdev, 0);
	if (host->irq < 0) {
		dev_err(&pdev->dev, "failed to get an IRQ number\n");
		ret = -EINVAL;
		goto out_remap;
	}

	ret = request_irq(host->irq, mes_sdio_irq, 0, mmc_hostname(host->mmc), host);
	if (ret) {
		dev_err(&pdev->dev, "can't get SDIO%d IRQ\n", host->channel);
		ret = -ENOENT;
		goto out_irq;
	}

	host->sd_dma_tr = soc_dma_request(plat->dma_index, false);
	if(!host->sd_dma_tr) {
		dev_err(&pdev->dev, "can't get DMA channel\n");
		goto out_dma;
	}

	/* Set global DMA parameters.  Per-transaction set later as needed. */
	host->sd_dma_tr->requestid = (host->channel == 0 ) ?
		DMAINDEX_OF_SDHC0_MODULE : DMAINDEX_OF_SDHC1_MODULE;

	host->sd_dma_tr->bitwidth  = NX_SDHC_GetDMABusWidth(host->channel);

	ret = request_irq(PB_DMA_IRQ(host->sd_dma_tr->channel), mes_sdio_dma_cb, IRQF_DISABLED, mmc_hostname(mmc), host);
	if(ret) {
		dev_err(&pdev->dev, "can't get DMA IRQ: %d\n", ret);
		goto out_irq_dma;
	}

	spin_lock_init(&host->lock);
	gpio_request(sdio_pins[host->channel][3], NULL);

	/* prepare the hardware */
	mes_sdio_setup_controller(host);

	platform_set_drvdata(pdev, mmc);

	mes_sdio_init_debugfs(host);

	mmc_add_host(mmc);

	dev_dbg(&pdev->dev, "\"%s\" probe complete\n", dev_name(mmc_dev(host->mmc)));

	return 0;

out_irq_dma:
	soc_dma_release(host->sd_dma_tr);
out_dma:
	free_irq(host->irq, host);
out_irq:
	iounmap(host->base);
out_remap:
	release_mem_region(host->mem->start, RESSIZE(host->mem));
out_mem:
	mmc_free_host(mmc);

	return ret;
}

static int mes_sdio_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);
	struct mes_sdio_host *host = NULL;

	if (mmc) {
		host = mmc_priv(mmc);

		if (host->debug)
			debugfs_remove_recursive(host->debug);

		mmc_remove_host(mmc);
		mes_sdio_clock_disable(host);
		gpio_free(sdio_pins[host->channel][3]);
		writel(0, host->base + SDI_INTMASK);
		mes_sdio_clear_int_status(host);
		free_irq(host->irq, host);
		free_irq(PB_DMA_IRQ(host->sd_dma_tr->channel), host);
		soc_dma_release(host->sd_dma_tr);
		if (host->dma_active)
			complete(&host->dma_transfer);
		iounmap(host->base);
		release_mem_region(host->mem->start, RESSIZE(host->mem));
	}
	
	return 0;
}

#ifdef CONFIG_PM
static int mes_sdio_suspend(struct platform_device *dev, pm_message_t state)
{
	return 0;
}

static int mes_sdio_resume(struct platform_device *dev)
{
	return 0;
}
#else
#define mes_sdio_suspend NULL
#define mes_sdio_resume	NULL
#endif

static struct platform_driver mes_sdio_driver = {
	.probe		= mes_sdio_probe,
	.remove		= mes_sdio_remove,
	.suspend	= mes_sdio_suspend,
	.resume		= mes_sdio_resume,
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init mes_sdio_init(void)
{
	return platform_driver_register(&mes_sdio_driver);
}

static void __exit mes_sdio_exit(void)
{
	platform_driver_unregister(&mes_sdio_driver);
}

module_init(mes_sdio_init);
module_exit(mes_sdio_exit);

MODULE_DESCRIPTION("SD/SDIO Host Controller Driver for MagicEyes SoCs");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Scott Esters");
MODULE_ALIAS("platform:" DRIVER_NAME);
