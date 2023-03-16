/*******************************************************************
* File Name         		: spica_sd.c
* Module Name       	: SD/MMC
* Project Name      	: Core Logic Linux BSP Development
* Description       		: This file is the lowlevel SD host controller driver for
*							SD/MMC module.
*-------------------------------------------------------------------*
* Date          			Author         			Description of change
*-------------------------------------------------------------------*
*12-14-09    			Corelogic      		        Creation
**********************************************************************/
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/mmc/host.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/wait.h>
#include <linux/mmc/mmc.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <asm/dma.h>

// SPICA header
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>

#include "nexell_sdhc.h"

#define CLM7700_MMC_DEBUG 	(0)
#if CLM7700_MMC_DEBUG
#define SDHC_DBG(x...) printk(SDHC_DEV_NAME ":" x)
#else
#define SDHC_DBG(x...)
#endif

#define	SD_DETECT_LEVEL			0
#define	SD_DETECT_IRQ_MODE		NX_GPIO_INTMODE_LOWLEVEL
#define	SD_REMOVE_IRQ_MODE		NX_GPIO_INTMODE_HIGHLEVEL

// sesters: we do not support multiple SDIO_IRQ interrupts
//#define CONFIG_MMC_CL_HW_SDIO_IRQ

//#define MOVI_NAND_CH0_USE
#define DMA_USE		1
#define CFALSE 		0
#define CTRUE 		1

#define cl7700_sdi_read(host, reg)			__raw_readl(host->baseaddr + reg)
#if (0)
#define cl7700_sdi_write(host, reg, val)	__raw_writel(val, (host->baseaddr + reg))
#else
#define cl7700_sdi_write(host, reg, val)	WriteIODW((host->baseaddr + reg), val)
#endif

#define CLM7700_SDI_ERRORS (1<<SDHC_INT_DRTO)| (1<<SDHC_INT_EBE) | (1<<SDHC_INT_SBE)|(1<<SDHC_INT_DCRC) \
							| (1<<SDHC_INT_HLE)| (1<<SDHC_INT_FRUN) | (1<<SDHC_INT_HTO) |(1<<SDHC_INT_RTO) \
							| (1<<SDHC_INT_RCRC)|(1<<SDHC_INT_RE)

#define MAX_POLL_COUNT 20000000

#ifdef wakeup
DECLARE_WAIT_QUEUE_HEAD(WaitQueue_sdhc);
#endif

static int polling=0;
module_param(polling,int,0660);

struct sdhc_host_io {
	int 	detect;		/* card detect io pad */
	int		wprotect;	/* card write protect io pad */
};

struct sdhc_dma_sg {
	int 				count;
	struct scatterlist *sg;
};

struct cl7700_sdhc_host {

	struct mmc_request* mrq;
	struct mmc_command* cmd;
	struct mmc_data   * data;
	struct mmc_host   * mmc;
	struct device 	  * dev;
	struct clk 	  * clk;
	int 		    irq_sdi;
	int		    irq_cd;
	int		    irq_dma;
	int 		    irq_status;
	int		    dma_index;
	unsigned int 	    error_status;

	struct tasklet_struct error_tasklet;

	unsigned int	    readonly;
	unsigned int	    cd_state;

	unsigned int	    clock;

	unsigned int	    sdhc_index;
	unsigned int	    bus_width;
	unsigned int	    pre_bus_width;
	unsigned int 	    real_rate;
	unsigned int 	    sd_clock_change;
	void __iomem 	  * baseaddr;

	int 		    dma_data_cb;
	struct completion   dma_completion;

	// +++ Add by jhkim
	struct dma_trans  * sd_dma_tr;
	struct sdhc_host_io sd_io;
	struct sdhc_dma_sg  sd_sg;
	// --- Add by jhkim

	unsigned int	    HWClkDiv;

	unsigned char 	    Int_HLE_status;
	spinlock_t 	    lock;
	unsigned int	    dma_data_dir;
	unsigned int 	    sdio_int_en;

	unsigned int	    reqcompleted;
	unsigned int	    sdhcsuspend;
};

/*
 * debugfs Interface
 */

#ifdef CONFIG_DEBUG_FS

static int nexell_sdhc_regs_show(struct seq_file *s, void *data)
{
	struct mmc_host *mmc = s->private;
	struct cl7700_sdhc_host *host = mmc_priv(mmc);

	seq_printf(s, "SDI_CTRL       0x00:\t0x%08x\n", cl7700_sdi_read(host,SDI_CTRL));
	seq_printf(s, "SDI_PWREN      0x04:\t0x%08x\n", cl7700_sdi_read(host,SDI_PWREN));
	seq_printf(s, "SDI_CLKDIV     0x08:\t0x%08x\n", cl7700_sdi_read(host,SDI_CLKDIV));
	seq_printf(s, "SDI_CLKENA     0x10:\t0x%08x\n", cl7700_sdi_read(host,SDI_CLKENA));
	seq_printf(s, "SDI_TMOUT      0x14:\t0x%08x\n", cl7700_sdi_read(host,SDI_TMOUT));
	seq_printf(s, "SDI_CTYPE      0x18:\t0x%08x\n", cl7700_sdi_read(host,SDI_CTYPE));
	seq_printf(s, "SDI_BLKSIZ     0x1C:\t0x%08x\n", cl7700_sdi_read(host,SDI_BLKSIZ));
	seq_printf(s, "SDI_BYTCNT     0x20:\t0x%08x\n", cl7700_sdi_read(host,SDI_BYTCNT));
	seq_printf(s, "SDI_INTMASK    0x24:\t0x%08x\n", cl7700_sdi_read(host,SDI_INTMASK));
	seq_printf(s, "SDI_CMDARG     0x28:\t0x%08x\n", cl7700_sdi_read(host,SDI_CMDARG));
	seq_printf(s, "SDI_CMD        0x2C:\t0x%08x\n", cl7700_sdi_read(host,SDI_CMD));
	seq_printf(s, "SDI_RESP0      0x30:\t0x%08x\n", cl7700_sdi_read(host,SDI_RESP0));
	seq_printf(s, "SDI_RESP1      0x34:\t0x%08x\n", cl7700_sdi_read(host,SDI_RESP1));
	seq_printf(s, "SDI_RESP2      0x38:\t0x%08x\n", cl7700_sdi_read(host,SDI_RESP2));
	seq_printf(s, "SDI_RESP3      0x3C:\t0x%08x\n", cl7700_sdi_read(host,SDI_RESP3));
	seq_printf(s, "SDI_MINTSTS    0x40:\t0x%08x\n", cl7700_sdi_read(host,SDI_MINTSTS));
	seq_printf(s, "SDI_RINTSTS    0x44:\t0x%08x\n", cl7700_sdi_read(host,SDI_RINTSTS));
	seq_printf(s, "SDI_STATUS     0x48:\t0x%08x\n", cl7700_sdi_read(host,SDI_STATUS));
	seq_printf(s, "SDI_FIFOTH     0x4C:\t0x%08x\n", cl7700_sdi_read(host,SDI_FIFOTH));
	seq_printf(s, "SDI_TCBCNT     0x5C:\t0x%08x\n", cl7700_sdi_read(host,SDI_TCBCNT));
	seq_printf(s, "SDI_TBBCNT     0x60:\t0x%08x\n", cl7700_sdi_read(host,SDI_TBBCNT));
	seq_printf(s, "SDI_DEBNCE     0x64:\t0x%08x\n", cl7700_sdi_read(host,SDI_DEBNCE));

	// Leave data in FIFO
	//seq_printf(s, "SDI_DAT       0x100:\t0x%08x\n", cl7700_sdi_read(host,SDI_DAT));
	seq_printf(s, "SDI_CICTRL    0x200:\t0x%08x\n", cl7700_sdi_read(host,SDI_CICTRL));
	seq_printf(s, "SDI_SYSCLKENB 0x7C0:\t0x%08x\n", cl7700_sdi_read(host,SDI_SYSCLKENB));
	seq_printf(s, "SDI_CLKGEN    0x7C4:\t0x%08x\n", cl7700_sdi_read(host,SDI_CLKGEN));

	return 0;
}

static int nexell_sdhc_regs_open(struct inode *inode, struct file *file)
{
	return single_open(file, nexell_sdhc_regs_show, inode->i_private);
}

static const struct file_operations mmc_regs_fops = {
	.open           = nexell_sdhc_regs_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

static void nexell_sdhc_debugfs(struct mmc_host *mmc)
{
	if (mmc->debugfs_root)
		debugfs_create_file("regs", S_IRUSR, mmc->debugfs_root,
			mmc, &mmc_regs_fops);
}

#else

static void nexell_sdhc_debugfs(struct mmc_host *mmc)
{
	printk(KERN_INFO "%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);
}

#endif


void cl7700_sdhc_InterruptEnable(struct cl7700_sdhc_host *host, bool bEnb)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);

	regval = cl7700_sdi_read(host,SDI_CTRL);
	if(bEnb)	regval |= (1<<SDI_INT_ENABLE);
	else		regval &= ~(1<<SDI_INT_ENABLE);
	cl7700_sdi_write(host, SDI_CTRL, regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setClockDivisorEnable(struct cl7700_sdhc_host *host, bool bEnb )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_SYSCLKENB);
	if( bEnb )	regval |=  SDI_CLK_GENENB;
	else		regval &= ~SDI_CLK_GENENB;
	cl7700_sdi_write(host, SDI_SYSCLKENB, regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setOutputClockEnable(struct cl7700_sdhc_host *host, bool bEnb )
{
	unsigned int regval = 0;
	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CLKENA);
	if( bEnb )	regval |= SDI_CLK_ENABLE;
	else		regval &= ~SDI_CLK_ENABLE;
	cl7700_sdi_write(host, SDI_CLKENA, regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setClockPClkMode(struct cl7700_sdhc_host *host, CL7700_SDHC_PCLKMODE mode)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_SYSCLKENB);
	switch(mode){
		case SDHC_PCLKMODE_DYNAMIC:  	regval &= ~SDI_PCLKMODE;	break;
		case SDHC_PCLKMODE_ALWAYS : 	regval |=  SDI_PCLKMODE;		break;
	}
	cl7700_sdi_write(host,SDI_SYSCLKENB,regval);
	spin_unlock(&host->lock);
}

void cl7700_sdhc_setClockSource(struct cl7700_sdhc_host *host, unsigned int ClkSrc)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CLKGEN);
	regval &= ~CLKSRCSEL_MASK;
	regval |= ClkSrc << CLKSRCSEL_POS;
	cl7700_sdi_write(host,SDI_CLKGEN,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setClockDivisor(struct cl7700_sdhc_host *host, unsigned int Divider )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CLKGEN);
	regval &= ~CLKDIV_MASK;
	regval |= (Divider-1) << CLKDIV_POS;
	cl7700_sdi_write(host,SDI_CLKGEN,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setLowPowerClockMode(struct cl7700_sdhc_host *host, bool bEnb )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CLKENA);
	if( bEnb )	regval |=  LOWPWR;
	else		regval &= ~LOWPWR;
	cl7700_sdi_write(host, SDI_CLKENA, regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_resetController(struct cl7700_sdhc_host *host)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CTRL);
	regval &= ~((1<<DMARST) |(1<<FIFORST));
	regval |=  1<< CTRLRST;
	cl7700_sdi_write(host,SDI_CTRL,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_resetDMA(struct cl7700_sdhc_host *host)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host, SDI_CTRL);
	regval &= ~((1<<FIFORST) | (1<<CTRLRST));
	regval |=  (1<<DMARST);
	cl7700_sdi_write(host,SDI_CTRL,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_resetFIFO(struct cl7700_sdhc_host *host)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CTRL);
	regval &= ~((1<<DMARST) | (1<<CTRLRST));
	regval |=  (1<<FIFORST);
	cl7700_sdi_write(host,SDI_CTRL,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setDMAmode(struct cl7700_sdhc_host *host, bool bEnb )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CTRL);
	regval &= ~(1<<DMA_ENA_POS);	//wheom
	regval |= (unsigned int)bEnb << DMA_ENA_POS;
	cl7700_sdi_write(host,SDI_CTRL,regval);
	spin_unlock(&host->lock);
}


void	cl7700_sdhc_setReadWait(struct cl7700_sdhc_host *host, bool bEnb )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CTRL);
	regval &= ~(1<<SDI_READ_WAIT);	//wheom
	regval |= (unsigned int)bEnb << SDI_READ_WAIT;
	cl7700_sdi_write(host,SDI_CTRL,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setDataTimeOut(struct cl7700_sdhc_host *host, unsigned int dwTimeOut )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_TMOUT);
	regval &= ~DTMOUT_MASK;
	regval |= dwTimeOut << DTMOUT_POS;
	cl7700_sdi_write(host,SDI_TMOUT,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setResponseTimeOut(struct cl7700_sdhc_host *host, unsigned int dwTimeOut )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_TMOUT);
	regval &= ~RSPTMOUT_MASK;
	regval |= dwTimeOut << RSPTMOUT_POS;
	cl7700_sdi_write(host,SDI_TMOUT,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setFIFORxThreshold(struct cl7700_sdhc_host *host, unsigned int Threshold )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_FIFOTH);
	regval &= ~RXTH_MASK;
	regval |= Threshold << RXTH_POS;
	cl7700_sdi_write(host,SDI_FIFOTH,regval);
	spin_unlock(&host->lock);
}

void	cl7700_sdhc_setFIFOTxThreshold(struct cl7700_sdhc_host *host, unsigned int Threshold )
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_FIFOTH);
	regval &= ~TXTH_MASK;
	regval |= Threshold << TXTH_POS;
	cl7700_sdi_write(host,SDI_FIFOTH,regval);
	spin_unlock(&host->lock);
}

bool cl7700_sdhc_IsCommandBusy(struct cl7700_sdhc_host *host)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_CMD);
	regval = (regval >> STARTCMD_POS) & 0x1;
	spin_unlock(&host->lock);
	return (bool)regval;
}

bool	cl7700_sdhc_IsDataTransferBusy(struct cl7700_sdhc_host *host)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_STATUS);
	regval = (regval & FSMBUSY_MASK) >> FSMBUSY_POS;
	spin_unlock(&host->lock);
	return (bool)regval;
}

bool	cl7700_sdhc_IsCardDataBusy(struct cl7700_sdhc_host *host)
{
	unsigned int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	spin_lock(&host->lock);
	regval = cl7700_sdi_read(host,SDI_STATUS);
	regval = (regval & DATBUSY_MASK) >> DATBUSY_POS;
	spin_unlock(&host->lock);
	return (bool)regval;
}

void cl7700_sdhc_gpio_config(struct cl7700_sdhc_host *host)
{
	struct sdhc_host_io *sd_io = &host->sd_io;
	int io, val;

	SDHC_DBG("%s (ch:%d)\n", __func__, host->sdhc_index);

	io = sd_io->detect;
	SDHC_DBG("set card detect io[%d]\n", io);
	if(-1 != io) {
		soc_gpio_set_io_func(io, NX_GPIO_PADFUNC_GPIO);
		soc_gpio_set_io_dir(io, 0);
		soc_gpio_clr_int_pend(io);
		soc_gpio_set_int_mode(io, SD_DETECT_IRQ_MODE);
		soc_gpio_set_io_pullup(io, 1);

		val = soc_gpio_get_in_value(io);
		host->irq_cd = PB_PIO_IRQ(host->sd_io.detect);
	} else {
		val = SD_DETECT_LEVEL;
		host->irq_cd = -1;
	}

	io = sd_io->wprotect;
	SDHC_DBG("set card write protect io[%d]\n", io);
	if(-1 != io) {
		soc_gpio_set_io_func(io, NX_GPIO_PADFUNC_GPIO);
		soc_gpio_set_io_dir(io, 0);

		host->readonly = soc_gpio_get_in_value(io);
	}

	/* Set detect status */
	host->cd_state = (val == SD_DETECT_LEVEL ? 1 : 0);

	SDHC_DBG("%s (%s:%s)\n", __func__, val?"high":"low",
			val==SD_DETECT_LEVEL?"attached":"detached");
}

bool cl7700_sdhc_InitHardware(struct cl7700_sdhc_host *host)
{
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	// Initialize SDHC register
	cl7700_sdhc_setClockPClkMode(host,SDHC_PCLKMODE_ALWAYS);
	cl7700_sdhc_setClockSource(host,SDHC_CLK_SRC_CLOCK);
	cl7700_sdhc_setClockDivisor(host,SDHC_CLK_SRC_DIVID);
	cl7700_sdhc_setClockDivisorEnable(host,CTRUE);

	cl7700_sdhc_InterruptEnable(host,CTRUE);
	cl7700_sdhc_setOutputClockEnable(host, CFALSE);
	cl7700_sdi_write(host,SDI_CLKDIV, (host->HWClkDiv>>1));

	cl7700_sdhc_setDMAmode(host, CFALSE);
	// Reset Controller
	cl7700_sdhc_resetController(host);
	while(cl7700_sdi_read(host,SDI_CTRL) & (1<<CTRLRST));
	// Reset DMA
	cl7700_sdhc_resetDMA(host);
	while(cl7700_sdi_read(host,SDI_CTRL) & (1<<DMARST));			// Wait until the DMA reset is completed.
	/* ResetFIFO */
	cl7700_sdhc_resetFIFO(host);
	while(cl7700_sdi_read(host,SDI_CTRL) & (1<<FIFORST));


	cl7700_sdhc_setLowPowerClockMode(host, CFALSE);

	cl7700_sdhc_setDataTimeOut(host, 0xFFFFFF);
	cl7700_sdhc_setResponseTimeOut(host, 0xff);	//0x64;
//	host->pre_bus_width		= MMC_BUS_WIDTH_1;
	if(host->pre_bus_width == MMC_BUS_WIDTH_1)
		cl7700_sdi_write(host,SDI_CTYPE,0);		// bus width : 1 bit mode
	else if(host->pre_bus_width == MMC_BUS_WIDTH_4)
		cl7700_sdi_write(host,SDI_CTYPE,1);		// bus width : 4 bit mode
	cl7700_sdi_write(host,SDI_BLKSIZ,512);	// block length

	cl7700_sdhc_setFIFORxThreshold(host, (2-1));		// Issue when RX FIFO Count >= 8 x 4 bytes
	cl7700_sdhc_setFIFOTxThreshold(host, 8); 			// Issue when TX FIFO Count <= 8 x 4 bytes

	cl7700_sdi_write(host,SDI_INTMASK, 0x0);		//interrupt masking disable
	cl7700_sdi_write(host,SDI_RINTSTS, 0xFFFFFFFF);		//clear interrupt pending all

	//cl7700_sdhc_setReadWait(host,1);
//	mdelay(10);
	return CTRUE;
}

void cl7700_sdhc_start_command(struct cl7700_sdhc_host *host, struct mmc_command *cmd)
{
	unsigned int cmd_flag=0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	host->cmd = cmd;

	/* Set flag according to the command type */
	cmd_flag |= (SDHC_CMDFLAG_STARTCMD | SDHC_CMDFLAG_WAITPRVDAT);

	if((host->cmd->opcode == 13)||(host->cmd->opcode == 52)||(host->cmd->opcode == 53))		//MMC_SEND_STATUS
		cmd_flag &= ~(SDHC_CMDFLAG_WAITPRVDAT);

	if((host->cmd->opcode == 0) ||(host->cmd->opcode == 5))		//GO_IDLE_STATE or
		cmd_flag |= SDHC_CMDFLAG_SENDINIT;

	/* In case of the long response type command */
	if(host->cmd->flags & MMC_RSP_PRESENT){
		if(host->cmd->flags & MMC_RSP_136)
			cmd_flag |= SDHC_CMDFLAG_LONGRSP;
		else
			cmd_flag |= SDHC_CMDFLAG_SHORTRSP;
	}
	if(host->cmd->flags & MMC_RSP_CRC){
		cmd_flag |= SDHC_CMDFLAG_CHKRSPCRC;
	}

	if((host->cmd->opcode == 52)||(host->cmd->opcode == 53)){
		cmd_flag &= ~(SDHC_CMDFLAG_CHKRSPCRC);
	}
	if(host->cmd->opcode == 52){
		cmd_flag &= ~(0x200);
	}

	if(host->data){
		if((host->cmd->opcode == 18) || (host->cmd->opcode == 25))	{	//for SDIO not used
			if(host->data->blocks > 1)	// Multi block
				cmd_flag |= SDHC_CMDFLAG_SENDAUTOSTOP;
		}
		if(host->data->flags & MMC_DATA_WRITE)
			cmd_flag |= (SDHC_CMDFLAG_BLOCK |SDHC_CMDFLAG_TXDATA);
		if(host->data->flags & MMC_DATA_READ)
			cmd_flag |= (SDHC_CMDFLAG_BLOCK | SDHC_CMDFLAG_RXDATA);
		if(host->data->flags & MMC_DATA_STREAM)
			cmd_flag |= SDHC_CMDFLAG_STREAM;
	}

	cl7700_sdi_write(host,SDI_RINTSTS,0xFFFFFFFF);		// Clear interrupt pending all
	cl7700_sdi_write(host,SDI_INTMASK,0x0);				// Interrupt disable all
	cl7700_sdi_write(host,SDI_RINTSTS,0xFFFFFFFF);		// clear interrupt pending all

	if(host->sdio_int_en)
		cl7700_sdi_write(host, SDI_INTMASK, 0xFFCE|0x10000);			// Disable RXDR, TXDR in DMA mode
	else
		cl7700_sdi_write(host, SDI_INTMASK, 0xFFCE);			// Disable RXDR, TXDR in DMA mode

	cl7700_sdi_write(host, SDI_CMDARG, cmd->arg); 		// Set argument register

	cl7700_sdi_write(host, SDI_CMD, (cmd->opcode & 0xFF)|cmd_flag); // Set command register
	SDHC_DBG("[CMD:0x%02x] cmdreg =  0x%04x\n", cmd->opcode, (cmd->opcode & 0xFF)|cmd_flag);
}

static void cl7700_sdhc_xfer_done(struct cl7700_sdhc_host *host, struct mmc_data *data);
static void cl7700_sdhc_cmd_done(struct cl7700_sdhc_host *host, struct mmc_command *cmd)
{
	host->cmd = NULL;

	SDHC_DBG("%s(ch:%d, cmd:0x%x)\n", __func__, host->sdhc_index, (u_int)cmd);

	/* extract the responses */
	if (cmd->flags & MMC_RSP_PRESENT) {
		if (cmd->flags & MMC_RSP_136) {
			cmd->resp[0] = cl7700_sdi_read(host, SDI_RESP3);
			cmd->resp[1] = cl7700_sdi_read(host, SDI_RESP2);
			cmd->resp[2] = cl7700_sdi_read(host, SDI_RESP1);
			cmd->resp[3] = cl7700_sdi_read(host, SDI_RESP0);
			SDHC_DBG(" MMC_RSP_136:Response= %08x %08x %08x %08x\n",cmd->resp[3], cmd->resp[2],cmd->resp[1], cmd->resp[0]);

		}
		else {
			cmd->resp[0] = cl7700_sdi_read(host, SDI_RESP0);
			SDHC_DBG("MMC_RSP_SHORT: Response=%08x\n", cmd->resp[0]);
		}
	}

	if(host->data && (host->data->flags & MMC_DATA_WRITE)) {
		struct dma_trans *tr = host->sd_dma_tr;
		SDHC_DBG("%s(run dma ch:%d)\n", __func__, host->sdhc_index);
		soc_dma_transfer(tr);
		/* Next DMA buffer */
		host->sd_sg.sg++;
		host->sd_sg.count--;
	}

	/*complete the command when no error occured*/
	if(host->data == NULL || cmd->error != 0) {
		host->mrq = NULL;
		mmc_request_done(host->mmc, cmd->mrq);
		host->reqcompleted = 1;
	}else
	if(host->data != NULL && host->error_status != 0)
	{
		host->data->error = 1;
		cl7700_sdhc_xfer_done(host, host->data);
	}
}

static void cl7700_sdhc_xfer_done(struct cl7700_sdhc_host *host, struct mmc_data *data)
{
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	if(data->error == 0) {
		data->bytes_xfered += data->blocks *  data->blksz;
	}
	else
	{
		/* We got an error, let's free the DMA channel if it's
		     still allocated. */
		if(host->sd_dma_tr)
		{
			struct dma_trans *tr = host->sd_dma_tr;
			host->dma_data_cb = -1;

			//dma_reset(host->DMAch);
			//SDHC_DBG("DMA Reset !!\n");
			soc_dma_trans_stop(tr);

			if(soc_dma_wait_ready(tr, 1000*1000)) {
				SDHC_DBG("sdhc (%d) dma (%d)  active status !!!\n",
					host->sdhc_index, host->sd_dma_tr->channel);
			}

			// SDHC DMA mode disable
			//cl7700_sdhc_setDMAmode(host,CFALSE);
			//udelay(10);
			complete(&host->dma_completion);
		}
	}

	host->mrq = NULL;
	dma_unmap_sg(mmc_dev(host->mmc), host->data->sg, host->data->sg_len, host->dma_data_dir);
	mmc_request_done(host->mmc, data->mrq);
	host->reqcompleted = 1;
	#ifdef CONFIG_MMC_CL_HW_SDIO_IRQ
	udelay(150);
	#endif

	return;
}

static irqreturn_t cl7700_sdhc_dma_data_cb(int ch, void *ud)
{
	struct cl7700_sdhc_host *host = (struct cl7700_sdhc_host *) ud;
	struct dma_trans *tr = host->sd_dma_tr;

	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	if(! host->sd_dma_tr) {
		SDHC_DBG("MMC%d: DATA DMA callback while DMA not enabled?\n", host->sdhc_index);
		return IRQ_NONE;
	}
	/*Having received the DMA competion callback now disable the channel and
	hence free the channel. */
	host->dma_data_cb = -1;

	soc_dma_trans_stop(tr);

	soc_dma_wait_ready(tr, 1000*1000);
	SDHC_DBG("sdhc (%d) dma (%d)  active status !!!\n",
		host->sdhc_index, host->sd_dma_tr->channel);

	// SDHC DMA mode disable
	cl7700_sdhc_setDMAmode(host,CFALSE);

	complete(&host->dma_completion);

#ifdef CONFIG_MMC_CL_HW_SDIO_IRQ
	udelay(300);
#endif
	return IRQ_HANDLED;
}

static int cl7700_sdhc_init_dma(struct cl7700_sdhc_host *host)
{
	struct dma_trans *tr = NULL;

	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	host->dma_data_cb=-1;

	if(host->sd_dma_tr) {
		soc_dma_release(tr);
		return 0;
	}

	tr = soc_dma_request(host->dma_index, false);
	if(! tr) {
		SDHC_DBG("request dma failed\n");
		return -1;
	}

	host->sd_dma_tr = tr;
	host->irq_dma 	= PB_DMA_IRQ(host->sd_dma_tr->channel);

	printk("done: sdhc[%d] allocate dma ch[%d] ...\n",
		host->sdhc_index, host->sd_dma_tr->channel);

	return 0;
}

static void cl7700_sdhc_free_dma(struct cl7700_sdhc_host *host)
{
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	if(host){
		if(host->sd_dma_tr)
			soc_dma_release(host->sd_dma_tr);
	}
}

static void cl7700_sdhc_tasklet_error(unsigned long param)
{
	int end_command = 0;
	int end_transfer = 0;
	struct cl7700_sdhc_host *host = (struct cl7700_sdhc_host *)param;
	int status = host->irq_status;
	unsigned char err_status = host->error_status;

	disable_irq(host->irq_sdi);

	SDHC_DBG("%s(ch:%d, err:%d)\n", __func__, host->sdhc_index, err_status);

	if(status & (1<<SDHC_INT_HLE)) {
		SDHC_DBG("Hardware locked write error interrupt\n");
		if(host->cmd) {

			host->cmd->error |= -EAGAIN;
			end_command = 1;
			//host->Int_HLE_status = CTRUE;
			//mdelay(10);
		}
	}
	if(status & (1<<SDHC_INT_DRTO)) {
		SDHC_DBG("Data Read Timeout!!\n");
		if(host->cmd) {
			host->cmd->error |=  -ETIMEDOUT;
			end_command = 1;
		}
	}
	if(status & (1<<SDHC_INT_RCRC)) {
		SDHC_DBG("Response CRC error\n");
		if(host->cmd) {
			host->cmd->error |=  -EILSEQ;
			end_command = 1;
		}
	}
	if(status & (1<<SDHC_INT_RTO)) {
		SDHC_DBG("Response timeout(err:0x%x)\n", err_status);
		if(host->cmd) {
			host->cmd->error |=  -EILSEQ;
			end_command = 1;
		}
	}
	if(status & (1<<SDHC_INT_FRUN)) {
		SDHC_DBG("FIFO underrun/overrun error\n");
		if(host->cmd) {
			host->cmd->error |= -ETIMEDOUT;
			end_transfer = 1;
		}
	}
	if(status & (1<<SDHC_INT_RE)) {
		SDHC_DBG("Response error\n");
		if(host->cmd) {
			host->cmd->error |= -ETIMEDOUT;
			end_command = 1;
		}
	}
	if(status &  (1<<SDHC_INT_HTO)) {
		SDHC_DBG("Data starvation by host timeout\n");
		if(host->data) {
			host->data->error |= -ETIMEDOUT;
			end_transfer = 1;
//			if(host->dma_flag)
//				complete(&host->dma_completion);
//				host->dma_flag = 0;
		}
	}
	if(status &  (1<<SDHC_INT_DCRC)) {
		SDHC_DBG("Data CRC Error!!\n");
		if(host->data) {
			host->data->error |= -EILSEQ;
			end_transfer = 1;
		}
	}
	if(status & (1<<SDHC_INT_SBE)) {
		SDHC_DBG("Start bit Error!!\n");
		if(host->data) {
			host->data->error |= -EILSEQ;
			end_transfer = 1;
		}
	}
	if(status & (1<<SDHC_INT_EBE)) {
		SDHC_DBG("End bit Error!!\n");
		if(host->data) {
			host->data->error |= -EILSEQ;
			end_transfer = 1;
		}
	}

	if(end_command){
		SDHC_DBG("end command(status:0x%x, err:0x%x)\n", status, err_status);
		if(status & (1<<SDHC_INT_CD)){
			if(err_status)
				cl7700_sdhc_cmd_done(host, host->cmd);
			host->error_status = 0;
			//printk("2\n");
		}
		//printk("3\n");
	}
	if(end_transfer){
		SDHC_DBG("end transfer(err:0x%x)\n", err_status);
//		if(host->error_status)
		if(err_status)
			cl7700_sdhc_xfer_done(host, host->data);

		host->error_status = 0;
	}

	enable_irq(host->irq_sdi);

	return;
}

static irqreturn_t cl7700_sdhc_irq(int irq, void *dev_id)
{
	struct cl7700_sdhc_host *host = (struct cl7700_sdhc_host *)dev_id;

	unsigned int int_status, int_mask;
	int end_command  = 0;
	int end_transfer = 0;
	int temp;

	int_status = cl7700_sdi_read(host,SDI_RINTSTS);
	int_mask = cl7700_sdi_read(host,SDI_INTMASK);

	SDHC_DBG("%s(ch:%x, irq status:0x%x)\n", __func__, host->sdhc_index, int_status);

	/* Clear pending interrupt */
	//cl7700_sdi_write(host,SDI_RINTSTS,int_status);

	int_status &= int_mask;
	host->irq_status = int_status;
#if 1
	if(host->mmc->caps  & MMC_CAP_SDIO_IRQ){
		if(int_status & (1 << SDHC_INT_SDIO))
		{
			cl7700_sdi_write(host,SDI_RINTSTS,int_status&(0x10000));
			//printk("@-S\n");
			//printk("@-SDIO\n");
			//SDHC_DBG("SDIO Card Interrupt\n");
			//spin_unlock_irqrestore(&host->lock, iflags);
			mmc_signal_sdio_irq(host->mmc);

			return IRQ_HANDLED;
		}
	}

#endif
	cl7700_sdi_write(host,SDI_RINTSTS, int_status&~(0x10000));
	if(int_status & (CLM7700_SDI_ERRORS))
	{
		host->error_status = 1;
		temp = (int_status & (CLM7700_SDI_ERRORS));
		SDHC_DBG("error_tasklet = 0x%x:0x%x, err:%d\n",temp, int_status, host->error_status);
		tasklet_hi_schedule(&host->error_tasklet);
	}
	else
	{
		SDHC_DBG("no error = 0x%x, err:%d\n", int_status, host->error_status);
		/*command done*/
		if(int_status & (1<<SDHC_INT_CD))
		{
			if(host->cmd) {
				host->cmd->error = 0;
				end_command = 1;
			}
		}
		/*data done*/
#if 0
		if(int_status & ((1<<MES_SDHC_INT_TXDR) |(1 << MES_SDHC_INT_DTO)))
		{
			printk("SD: Transmit FIFO data request\n");
			if(host->data) {
				host->data->error = 0;
				end_transfer = 1;
				//wake_up_interruptible(&WaitQueue_data);
			}
		}
		if(int_status & (1<<MES_SDHC_INT_RXDR))
		{
			if(MMC_SWITCH == host->mrq->cmd->opcode)
				return IRQ_HANDLED;
			printk("SD: Receive FIFO data request\n");
			if(host->data) {
				host->data->error = 0;
				end_transfer = 1;
			}
		}
#else
		//DMA mode
		if(int_status & (1 << SDHC_INT_DTO))
		{
			if(host->data) {
				host->data->error = 0;
				end_transfer = 1;
			}
		}
#endif
		if(end_command){
			if(host->error_status){
				host->error_status = 0;
				host->cmd->error |=  -EILSEQ;
			}
			cl7700_sdhc_cmd_done(host, host->cmd);
		}
		if(end_transfer){
			cl7700_sdhc_xfer_done(host,host->data);
		}
	}

	SDHC_DBG("done %s(ch:%x, err:0x%x)\n\n", __func__, host->sdhc_index, host->error_status);

	return IRQ_HANDLED;
}

static irqreturn_t cl7700_sdhc_cd_irq(int irq, void *dev_id)
{
	struct cl7700_sdhc_host *host = (struct cl7700_sdhc_host *)dev_id;
	int delay =0;
	u_int io = host->sd_io.detect;

	SDHC_DBG("%s(ch:%d, irq:%d)\n", __func__, host->sdhc_index, irq);

	if(SD_DETECT_LEVEL == soc_gpio_get_in_value(io)){
		SDHC_DBG("CARD INSERT !!\n");
		cl7700_sdi_write(host,SDI_RINTSTS,0xFFFFFFFF);
		host->cd_state = 1;
		delay = 500;
		soc_gpio_set_int_mode(io, SD_REMOVE_IRQ_MODE);
	} else {
		SDHC_DBG("CARD REMOVED !!\n");
		cl7700_sdi_write(host,SDI_RINTSTS,0xFFFFFFFF);
		host->cd_state = 0;
		delay = 50;
		soc_gpio_set_int_mode(io, SD_DETECT_IRQ_MODE);
	}

	host->clock = 0;
	mmc_detect_change(host->mmc, msecs_to_jiffies(delay));

	return IRQ_HANDLED;
}

static irqreturn_t cl7700_sdhc_dma_irq(int irq, void *dev_id)
{
	struct cl7700_sdhc_host *host = (struct cl7700_sdhc_host *)dev_id;
	struct dma_trans   * tr = host->sd_dma_tr;
	struct scatterlist * sg = NULL;

	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	if(! host->sd_sg.count || ! host->sd_sg.sg)
		return cl7700_sdhc_dma_data_cb(host->sd_dma_tr->channel, host);

	sg = host->sd_sg.sg;

	if(host->data->flags & MMC_DATA_WRITE) {
		tr->srcbase = sg_phys(sg);
		tr->length  = sg->length;
		soc_dma_transfer(tr);

	} else {
		tr->dstbase = sg_phys(sg);
		tr->length  = sg->length;
		soc_dma_transfer(tr);
	}

	/* Next buffer */
	host->sd_sg.sg++;
	host->sd_sg.count--;

	return IRQ_HANDLED;
}

static int cl7700_sdhc_start_dma_data_transfer(struct cl7700_sdhc_host *host, struct mmc_request *req)
{
	struct scatterlist  *sg;
	struct dma_trans *tr = NULL;
	int ch = host->sdhc_index;

	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	/* If for some reason the DMA transfer is still active,
	   we wait for it to complete. This shouldn't normally happen. */
	if(host->dma_data_cb!= -1){
		SDHC_DBG("wait for completion\n");
		wait_for_completion(&host->dma_completion);
	}
	init_completion(&host->dma_completion);

	host->dma_data_cb = 0;
	/*Calculate the Transfer size*/
	// host->trans_size = req->data->blocks *  req->data->blksz;
	sg = host->data->sg;
	SDHC_DBG("sg.length =0x%x\n", sg->length);

	if(host->data->flags & MMC_DATA_WRITE) {
		/*Prepare the DMA control flags and the config flags for the data transfer.
      		In this case it is the write operation*/


   		host->dma_data_dir = DMA_TO_DEVICE;
   		dma_map_sg(NULL, sg, host->data->sg_len, host->dma_data_dir);

		tr            = host->sd_dma_tr;
		tr->tr_type   = DMA_TRANS_MEM2IO;
		tr->dstbase   = NX_SDHC_GetPhysicalAddress(ch);
		tr->requestid = NX_SDHC_GetDMAIndex(ch);
		tr->bitwidth  = NX_SDHC_GetDMABusWidth(ch);

		tr->srcbase 	  = sg_phys(sg);
		tr->length   	  = sg->length;
		host->sd_sg.sg 	  = sg;
		host->sd_sg.count = host->data->sg_len;

	}
	else	{	//Read
		/*Prepare the DMA control flags and the config flags for the data transfer.
		In this case it is the read operation*/
		host->dma_data_dir = DMA_FROM_DEVICE;
		dma_map_sg(NULL, sg, host->data->sg_len, host->dma_data_dir);

		tr            = host->sd_dma_tr;
		tr->tr_type   = DMA_TRANS_IO2MEM;
		tr->srcbase   = NX_SDHC_GetPhysicalAddress(ch);
		tr->requestid = NX_SDHC_GetDMAIndex(ch);
		tr->bitwidth  = NX_SDHC_GetDMABusWidth(ch);

		tr->dstbase = sg_phys(sg);
		tr->length  = sg->length;

		/* Run DMA */
		soc_dma_transfer(tr);

		/* Next DMA buffer */
		sg++;
		host->sd_sg.sg 	  = sg;
		host->sd_sg.count = host->data->sg_len -1;
	}

	return 0;
}

static void cl7700_sdhc_prepare_data(struct cl7700_sdhc_host *host, struct mmc_request *req)
{
	unsigned int fifo_val;
	int size = 0;

	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	/* SDIO Bus Width Setting */
	if(host->pre_bus_width != host->bus_width){
		switch (host->bus_width) {
		case MMC_BUS_WIDTH_1:
			SDHC_DBG("MMC_BUS_WIDTH : 1 bit\n");
			cl7700_sdi_write(host,SDI_CTYPE,0);		// 1 bit mode
			break;

		case MMC_BUS_WIDTH_4:
			SDHC_DBG("MMC_BUS_WIDTH : 4 bit\n");
			cl7700_sdi_write(host,SDI_CTYPE,1);		// 4 bit mode
			break;

		case MMC_BUS_WIDTH_8:
			SDHC_DBG("clm7700 SDHC hardware Not Support ->8 bit bus mode!!\n");
			cl7700_sdi_write(host,SDI_CTYPE,1);		// 4 bit mode
			break;
		}
		host->pre_bus_width = host->bus_width;
	}

	// SDHC DMA mode enable
	cl7700_sdhc_setDMAmode(host,CTRUE);

	/* Set block size register */
	cl7700_sdi_write(host, SDI_BLKSIZ, req->data->blksz);
	SDHC_DBG("data->blocks = 0x%x\n",req->data->blocks);
	size = req->data->blocks * req->data->blksz;
	//host->mrq->data->bytes_xfered = 0;
	SDHC_DBG("size = 0x%x\n", size);

	if(size >= 32){
		cl7700_sdhc_setFIFORxThreshold(host, (8-1));		// Issue when RX FIFO Count >= 8 x 4 bytes
		cl7700_sdhc_setFIFOTxThreshold(host, 8);
	}
	else{
		fifo_val = size/4;
		cl7700_sdhc_setFIFORxThreshold(host, fifo_val-1);		// Issue when RX FIFO Count >= 8 x 4 bytes
		cl7700_sdhc_setFIFOTxThreshold(host, fifo_val);
	}
	//Set byte conunt
	cl7700_sdi_write(host, SDI_BYTCNT, size);

	 /*if the host structure is initialized to use dma, then start data transfer through dma*/
	cl7700_sdhc_start_dma_data_transfer(host, req);
}


static void cl7700_sdhc_request(struct mmc_host *mmc, struct mmc_request *req)
{
	struct cl7700_sdhc_host *host = mmc_priv(mmc);
	int delaycount=0;
	/* For debug */
	SDHC_DBG("%s(ch:%d, request: [CMD] opcode:0x%02x:%d arg:0x%08x flags:%x retries:%u\n",
		__func__, host->sdhc_index, req->cmd->opcode, req->cmd->opcode,
		req->cmd->arg, req->cmd->flags, req->cmd->retries);
	if(host->sdhcsuspend)
	{
		while(host->sdhcsuspend)
			msleep(1);
	}
	host->reqcompleted = 0;

	while(cl7700_sdi_read(host, SDI_STATUS) & 0x600)
	{
//		udelay(1);
		if(!in_interrupt())
			schedule();
		if(delaycount++ >100000)
		{
			SDHC_DBG("%s(device is much busy)\n", __func__ );
			break;
		}
	}

	host->mrq = req;
	host->data = req->data;

	/* Prepare data transfer */
	if(host->data)
		cl7700_sdhc_prepare_data(host, req);

	cl7700_sdhc_start_command(host, req->cmd);
}

int cl7700_sdhc_clock_change(struct cl7700_sdhc_host *host)
{
	unsigned int TimeOut = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	cl7700_sdi_write(host, SDI_CMD,SDHC_CMDFLAG_STARTCMD | SDHC_CMDFLAG_UPDATECLKONLY | SDHC_CMDFLAG_WAITPRVDAT); // Set command register
	while(cl7700_sdhc_IsCommandBusy(host)) {
		if(TimeOut++ > 0x1000000)	{
			SDHC_DBG("FAIL, TIme-out to SDHC set clock !!!\n");
			return 1;
		}
	}

	return 0;
}

static void cl7700_sdhc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct cl7700_sdhc_host *host = mmc_priv(mmc);
	unsigned int sdi_clk_divider;
	const unsigned int src_clock = SDHC_CLK_SRC_FREQ;	//PLL 1
	//const u32 src_clock = 100000000;		//PCLK
	unsigned int retry = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	if(cl7700_sdhc_IsCardDataBusy(host)) {
		SDHC_DBG(" FAIL, Card data state is busy !!!\n");
	}
	if(cl7700_sdhc_IsDataTransferBusy(host)) {
		SDHC_DBG("FAIL, Current card bus is busy, remove card !!!\n");
	}
	if(ios->clock)	{
		if(ios->clock != host->clock){
			cl7700_sdhc_setOutputClockEnable(host, CFALSE);

			if(ios->clock >= CCLK_IN_FREQ)
				sdi_clk_divider = 1;
			else
				sdi_clk_divider = 2 * ceil_div(ceil_div(CCLK_IN_FREQ, ios->clock), 2);
			SDHC_DBG("sdi_clk_divider = %d\n",sdi_clk_divider);
			cl7700_sdi_write(host, SDI_CLKDIV, sdi_clk_divider>>1);

			host->HWClkDiv = sdi_clk_divider;
			SDHC_DBG("ios->clock = %d, divider =%d, ios->timing = %d\n",ios->clock,sdi_clk_divider,ios->timing);
			host->real_rate = src_clock/SDHC_CLK_SRC_DIVID/sdi_clk_divider;
   			host->clock = ios->clock;

repeat_1 :
			if(cl7700_sdhc_clock_change(host)){
				SDHC_DBG("clock change fail\n");
				return;
			}
//			retry = 0;
			if(host->Int_HLE_status == CTRUE){
				if(retry <100){
					retry++;
					goto repeat_1;
				}
			}
			host->Int_HLE_status = CFALSE;
			host->sd_clock_change = 1;
		}
	}
	else	{
		host->clock = 0;
		host->real_rate = 0;
		// Disable the SD controller
		cl7700_sdhc_setOutputClockEnable(host,CFALSE);
	}

	/* Power sequence - OFF -> ON -> UP */
	switch (ios->power_mode) {
	case MMC_POWER_OFF: /* power down SD bus */
		SDHC_DBG("MMC_POWER_OFF\n");
		// Disable the output clock.
		cl7700_sdhc_setOutputClockEnable(host,CFALSE);
		//S/W Reset
		#if 0
		cl7700_sdhc_resetController(host);        	// Reset the controller.
		while(cl7700_sdi_read(host,SDI_CTRL) & (1<<CTRLRST))
			/* NULL */ ;			// Wait until the controller reset is completed.
		mdelay(5);
		#endif
		break;

	case MMC_POWER_ON: /* power on SD bus */
		SDHC_DBG("MMC_POWER_ON\n");
		// Enable the output clock.
		cl7700_sdhc_setOutputClockEnable(host,CTRUE);
		#if 1
		// Reset DMA
		cl7700_sdhc_resetDMA(host);
		while(cl7700_sdi_read(host,SDI_CTRL) & (1<<2))
			/* NULL */ ;			// Wait until the DMA reset is completed.

		/* ResetFIFO */
		cl7700_sdhc_resetFIFO(host);
		while(cl7700_sdi_read(host,SDI_CTRL) & (1<<1))
			/* NULL */ ;

		// FIFO empty check
		if(!(cl7700_sdi_read(host,SDI_STATUS) & 0x4)) {		// FIFO empty bit position: [5] 
			SDHC_DBG("MES_SDHC_IsFIFOEmpty ERROR!!\n");
		}
		#endif

		mdelay(5);
		break;

	case MMC_POWER_UP:
		SDHC_DBG("MMC_POWER_UP\n");
		mdelay(5);
		break;

	default:
		break;
	}


	if(ios->power_mode == MMC_POWER_ON && host->sd_clock_change)	{
repeat_2 :
		if(cl7700_sdhc_clock_change(host)){
			printk("%s, clock change fail ....\n", __func__);
			return;
		}
		retry = 0;
		if(host->Int_HLE_status == CTRUE){
			if(retry <100){
				retry++;
				goto repeat_2;
			}
		}
		host->Int_HLE_status = CFALSE;
		host->sd_clock_change = 0;
	}

	// For debug
	if ((ios->power_mode == MMC_POWER_ON) ||(ios->power_mode == MMC_POWER_UP))
	{
		SDHC_DBG("SDHC: running at %d kHz (requested: %d kHz).\n",host->real_rate/1000, ios->clock/1000);
	}
	else
	{
		SDHC_DBG("SDHC: powered down.\n");
	}

	mdelay(30);

	host->bus_width = ios->bus_width;

}

static int cl7700_sdhc_get_ro(struct mmc_host *mmc)
{
	int present = -ENOSYS;
	struct cl7700_sdhc_host *host = mmc_priv(mmc);

	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	// *   0 for a read/write card,  *   1 for a read-only card
	if(-1 != host->sd_io.wprotect)
		present = soc_gpio_get_in_value(host->sd_io.wprotect);

	SDHC_DBG(">> SD card(WP) = %s \n",present ? "RO":"RW");

	return present;
}

static int cl7700_sdhc_card_present(struct mmc_host *mmc)
{
	struct cl7700_sdhc_host *host = mmc_priv(mmc);
	int present;
	u_int io = host->sd_io.detect;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	//*   1 for a absent card	 *   0 for a present card
	if(-1 != io)
		present = soc_gpio_get_in_value(io);
	else
		return -ENOSYS;

	host->cd_state = (~present) & 0x1;

	SDHC_DBG("SD card(CD) - %s \n", host->cd_state ? "Detect":"Not detect");

	if(-1 != io && host->cd_state)
		soc_gpio_set_int_mode(io, SD_REMOVE_IRQ_MODE);

	return host->cd_state;
}

static void cl7700_sdhc_enable_sdio_irq(struct mmc_host *mmc,int enable)
{
	struct cl7700_sdhc_host *host = mmc_priv(mmc);
	int regval = 0;
	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	regval = cl7700_sdi_read(host, SDI_INTMASK);
	if(enable){
		host->sdio_int_en = 1;
		cl7700_sdi_write(host, SDI_INTMASK, regval |0x10000);
	}
	else{
		host->sdio_int_en = 0;
		cl7700_sdi_write(host, SDI_INTMASK, regval & ~(0x10000));
	}
}
static struct mmc_host_ops cl7700_sdhc_ops = {
	/* request handler */
	.request		= cl7700_sdhc_request,
	/* host controller setting function */
	.set_ios		= cl7700_sdhc_set_ios,
	/* read-only detection, return 1 : read only, 0: read-write */
	.get_ro		= cl7700_sdhc_get_ro,
	/* card detection return 1: card inserted, 0: card not inserted */
	.get_cd		= cl7700_sdhc_card_present,
	.enable_sdio_irq = cl7700_sdhc_enable_sdio_irq,
};

extern unsigned lf2000_gpio_l2p(struct gpio_chip* chip, unsigned offset);

static int __devinit cl7700_sdhc_probe(struct platform_device *pdev)
{
	struct sdhc_plat_data *pdata = pdev->dev.platform_data;
	struct mmc_host *mmc;
	struct cl7700_sdhc_host *host = NULL;
	int ret = 0;

	SDHC_DBG("%s\n", __func__);

	if (pdata == NULL) {
		dev_err(&pdev->dev, "Platform Data is missing\n");
		return -ENXIO;
	}

	// Allocate memory space for host specific data structure
	mmc = mmc_alloc_host(sizeof(struct cl7700_sdhc_host), &pdev->dev);
	if (!mmc) {
		ret = -ENOMEM;
		goto probe_out;
	}
	// Set host specific pointer to allocated space
	host = mmc_priv(mmc);

	// Initialize host specific data
	/* Set value of mmc structure */
	mmc->ops	= &cl7700_sdhc_ops;
	mmc->f_min	= 400000;
	mmc->f_max	= CCLK_IN_FREQ;
	mmc->caps	= MMC_CAP_4_BIT_DATA;
    // No CD line available in the cartridge port SD bus, we have to poll.
	if (polling) {
    	mmc->caps       |=  MMC_CAP_NEEDS_POLL;
	}

#if CCLK_IN_FREQ > 25000000
	mmc->caps	|=  MMC_CAP_SD_HIGHSPEED | MMC_CAP_MMC_HIGHSPEED;
#endif
#ifdef CONFIG_MMC_CL_HW_SDIO_IRQ
	mmc->caps	|= MMC_CAP_SDIO_IRQ;
#endif
	mmc->ocr_avail 	= MMC_VDD_32_33 | MMC_VDD_33_34;
#if 0
	// Maximum number of segments. Depends on if the hardware can do scatter/gather or not.
	mmc->max_segs 		= 1;
//	mmc->max_phys_segs	= 16;

	// Maximum block size. This varies from controller to controller and is specified in the capabilities register.
	mmc->max_blk_size 	= 512;
	// Maximum block count.
	mmc->max_blk_count 	= 4095;

	// Maximum number of sectors in one transfer. Limited by DMA boundary size (?? KiB).
	mmc->max_req_size 	= mmc->max_blk_size * mmc->max_blk_count;
	mmc->max_seg_size	=  mmc->max_req_size;
#else
	// Maximum number of segments. Depends on if the hardware can do scatter/gather or not.
	mmc->max_segs 	= 1;
//	mmc->max_phys_segs	= 128;

	// Maximum block size. This varies from controller to controller and is specified in the capabilities register.
	mmc->max_blk_size 	= 512;
	// Maximum block count.
	mmc->max_blk_count 	= 128;

	// Maximum number of sectors in one transfer. Limited by DMA boundary size (?? KiB).
	mmc->max_req_size 	= 65536;
	mmc->max_seg_size	= 65536;
#endif
	// Set host specific structure
	host->sdhc_index		= pdev->id;	/* SDHC channel */
	host->mmc				= mmc;
	host->irq_cd			= lf2000_gpio_l2p(NULL, pdata->logical_io_detect);
	host->cd_state			= 0;
	host->clock				= 0;
	host->readonly			= 0;
	host->bus_width 		= 0;		// 1bit mode = MMC_BUS_WIDTH_1
	host->pre_bus_width		= MMC_BUS_WIDTH_1;
	host->sd_clock_change 	= 0;
	host->Int_HLE_status 	= 0;
	host->error_status		= 0;

	host->sd_dma_tr			= NULL;
	host->sd_sg.count		= 0;
	host->sd_sg.sg			= NULL;
	host->sd_io.detect		= lf2000_gpio_l2p(NULL, pdata->logical_io_detect);
	host->sd_io.wprotect		= lf2000_gpio_l2p(NULL, pdata->logical_io_wprotect);
	host->dma_index			= pdata->dma_index;
	host->reqcompleted		= 1;
	host->sdhcsuspend		= 0;

	host->irq_sdi 			= NX_SDHC_GetInterruptNumber(host->sdhc_index);
	host->baseaddr 			= (void __iomem *)
					  IO_ADDRESS(PHY_BASEADDR_SDHC_MODULE +  host->sdhc_index * OFFSET_OF_SDHC_MODULE);

	spin_lock_init(&host->lock);

	/* tasklet enable */
	tasklet_init(&host->error_tasklet, cl7700_sdhc_tasklet_error, (unsigned long)host);

	/* H/W initialize */
	cl7700_sdhc_gpio_config(host);
	cl7700_sdhc_InitHardware(host);

	/* Allocate the sdi interrupt */
	ret = request_irq(host->irq_sdi, cl7700_sdhc_irq, IRQF_DISABLED, mmc_hostname(mmc), host);
	if (ret) {
		SDHC_DBG("request sdhc interrupt failed, dev = %d ,irq = %d\n",
			host->sdhc_index, host->irq_sdi);
		goto probe_free_host;
	}

	/* Allocate the Card detect interrupt */
	if(-1 != host->irq_cd) {
		ret=request_irq(host->irq_cd, cl7700_sdhc_cd_irq, IRQF_DISABLED, mmc_hostname(mmc), host);
		if (ret) {
			SDHC_DBG("Request sd detect interrupt failed, irq = %d\n", host->irq_cd);
			goto probe_free_irq;
		}
	}

	if(0 != cl7700_sdhc_init_dma(host)) {
		SDHC_DBG("Failed to allocate DMA resources, Device cannot work");
		goto probe_free_irq;
	}

	ret = request_irq(host->irq_dma, cl7700_sdhc_dma_irq, IRQF_DISABLED, mmc_hostname(mmc), host);
	if (ret) {
		SDHC_DBG("Request sd dma interrupt failed, irq = %d\n", host->irq_dma);
		goto probe_free_irq_cd;
	}

	// Register SPICA host driver
	platform_set_drvdata(pdev, mmc);

	// Add host to MMC layer
	if(mmc_add_host(mmc) != 0) {
		SDHC_DBG("failed to add mmc host.\n");
		goto probe_free_irq_dma;
	}

	SDHC_DBG("host irq - sdi	:%d\n", host->irq_sdi);
	SDHC_DBG("host irq - detect :%d\n", host->irq_cd);
	SDHC_DBG("host irq - dma 	:%d\n", host->irq_dma);

	nexell_sdhc_debugfs(mmc);
	return 0;

probe_free_irq_dma:
	if(-1 != host->irq_dma)
    	free_irq(host->irq_dma, host);

probe_free_irq_cd:
	if(-1 != host->irq_cd)
    	free_irq(host->irq_cd, host);

probe_free_irq:
	if(-1 != host->irq_sdi)
 		free_irq(host->irq_sdi, host);

probe_free_host:
	mmc_free_host(mmc);

probe_out:

	return ret;
}

static int cl7700_sdhc_remove(struct platform_device *pdev)
{
	struct mmc_host	*mmc  = platform_get_drvdata(pdev);
	struct cl7700_sdhc_host	*host = mmc_priv(mmc);
	unsigned int regval = 0;

	SDHC_DBG("%s\n", __func__);

	mmc_remove_host((void *)host->mmc);

	//dma free
	cl7700_sdhc_free_dma(host);

	// SD clock disable
	regval = cl7700_sdi_read(host,SDI_CLKENA);
	regval &= ~(SDI_CLK_ENABLE);
	cl7700_sdi_write(host,SDI_CLKENA,regval);

	if(-1 != host->irq_sdi)
		free_irq(host->irq_sdi, host);

	if(-1 != host->irq_cd)
		free_irq(host->irq_cd, host);

	if(-1 != host->irq_dma)
		free_irq(host->irq_dma, host);


	tasklet_kill(&host->error_tasklet);

	mmc_free_host(mmc);
	platform_set_drvdata(pdev,NULL);

	return 0;
}

#if 0
static void cl7700_sdhc_shutdown(struct platform_device *pdev)
{
	struct mmc_host	*mmc = platform_get_drvdata(pdev);
	struct cl7700_sdhc_host *host = mmc_priv(mmc);

	SDHC_DBG("%s\n", __func__);

	if(-1 != host->irq_sdi)
		free_irq(host->irq_sdi, host);

	if(-1 != host->irq_cd)
		free_irq(host->irq_cd, host);

	if(-1 != host->irq_dma)
		free_irq(host->irq_dma, host);

	mmc_remove_host(mmc);
}
#endif

#ifdef CONFIG_PM

#define NX_SDMMC_TIMEOUT 0x10000
static void NX_SDHCSetClockEnable(struct mmc_host *mmc)
{
	volatile unsigned int timeout;
	struct cl7700_sdhc_host *host = mmc_priv(mmc);

repeat_1 :
//	cl7700_sdi_write( host, SDI_CMD, SDHC_CMDFLAG_STARTCMD | SDHC_CMDFLAG_UPDATECLKONLY | SDHC_CMDFLAG_WAITPRVDAT );
	cl7700_sdi_write( host, SDI_CMD, SDHC_CMDFLAG_STARTCMD | SDHC_CMDFLAG_UPDATECLKONLY | SDHC_CMDFLAG_STOPABORT );

	//--------------------------------------------------------------------------
	// 5. Wait until a update clock command is taken by the SDHC module.
	//    If a HLE is occurred, repeat 4.
	timeout = 0;
	while( cl7700_sdi_read(host, SDI_CMD) & SDHC_CMDFLAG_STARTCMD )
	{
		if( ++timeout > NX_SDMMC_TIMEOUT )
		{
			printk(("NX_SDMMC_SetClock : ERROR - Time-out to update clock3.\n"));
			return ;
		}
	}

	if( cl7700_sdi_read(host,SDI_RINTSTS) &  SDHC_INT_HLE )
	{
		cl7700_sdi_write( host, SDI_RINTSTS, 1<<SDHC_INT_HLE );
		goto repeat_1;
	}

	//--------------------------------------------------------------------------
	// 6. Enable the output clock.
	cl7700_sdhc_setOutputClockEnable( host, CTRUE);

	//--------------------------------------------------------------------------
	// 7. Start a command with NX_SDHC_CMDFLAG_UPDATECLKONLY flag.
repeat_2 :
//	cl7700_sdi_write( host, SDI_CMD, SDHC_CMDFLAG_STARTCMD | SDHC_CMDFLAG_UPDATECLKONLY | SDHC_CMDFLAG_WAITPRVDAT );
	cl7700_sdi_write( host, SDI_CMD, SDHC_CMDFLAG_STARTCMD | SDHC_CMDFLAG_UPDATECLKONLY | SDHC_CMDFLAG_STOPABORT );

	//--------------------------------------------------------------------------
	// 8. Wait until a update clock command is taken by the SDHC module.
	//    If a HLE is occurred, repeat 7.
	timeout = 0;
	while( cl7700_sdi_read(host, SDI_CMD) & SDHC_CMDFLAG_STARTCMD )
	{
		if( ++timeout > NX_SDMMC_TIMEOUT )
		{
			printk(("NX_SDMMC_SetClock : ERROR - TIme-out to update clock4.\n"));
			return ;
		}
	}

	if( cl7700_sdi_read(host,SDI_RINTSTS) &  SDHC_INT_HLE )
	{
		cl7700_sdi_write( host, SDI_RINTSTS, 1<<SDHC_INT_HLE );
		goto repeat_2;
	}
}

static int cl7700_sdhc_suspend(struct platform_device *pdev, pm_message_t state)
{
	int ret;
	struct mmc_host	*mmc = platform_get_drvdata(pdev);
	struct cl7700_sdhc_host *host = mmc_priv(mmc);

	SDHC_DBG("%s(ch:%d)\n", __func__, host->sdhc_index);

	ret = mmc_suspend_host(host->mmc);
	host->sdhcsuspend = 1;

	while(! host->reqcompleted)
		msleep(1);

	return ret;
}

static int cl7700_sdhc_resume(struct platform_device *pdev)
{
	int ret;
	struct mmc_host	*mmc = platform_get_drvdata(pdev);
	struct cl7700_sdhc_host *host = mmc_priv(mmc);

	SDHC_DBG("%s\n", __func__);

	cl7700_sdhc_gpio_config(host);
	cl7700_sdhc_InitHardware(host);
	NX_SDHCSetClockEnable(mmc);

	host->sdhcsuspend = 0;
	ret = mmc_resume_host(host->mmc);
	if (ret)
		return ret;
	return 0;
}
#else
#define cl7700_sdhc_suspend	NULL
#define cl7700_sdhc_resume		NULL
#endif

static struct platform_driver cl7700_sdhc_driver = {
	.probe		= cl7700_sdhc_probe,
	.remove		= cl7700_sdhc_remove,
//	.shutdown	= cl7700_sdhc_shutdown,
	.suspend	= cl7700_sdhc_suspend,
	.resume		= cl7700_sdhc_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= SDHC_DEV_NAME,
	},
};

static int __init cl7700_sdhc_init(void)
{
	SDHC_DBG("%s\n", __func__);
	return platform_driver_register(&cl7700_sdhc_driver);
}

static void __exit cl7700_sdhc_exit(void)
{
	SDHC_DBG("%s\n", __func__);
	platform_driver_unregister(&cl7700_sdhc_driver);
}

module_init(cl7700_sdhc_init);
module_exit(cl7700_sdhc_exit);

MODULE_DESCRIPTION("Corelogic MMC/SD Card Interface driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Corelogic platform team: Woohyun Eom");
MODULE_ALIAS("platform:clm7700-sd");

