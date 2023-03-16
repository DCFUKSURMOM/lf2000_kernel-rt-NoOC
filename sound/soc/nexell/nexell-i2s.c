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
#include <linux/version.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/delay.h>

#include <sound/soc.h>

/* Nexell headers */
#include <mach/platform.h>
#include <mach/soc.h>
#include <mach/lfp100.h>

#include "nexell-i2s.h"
#include "nexell-pcm.h"

extern u_int cpu_get_clock_hz(cfg_sys_clksrc clk);

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "i2s: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define DEBUG_DUMP			(0)

struct i2s_snd_dev {
	struct i2s_plat_data 	  *pdata;
	struct snd_soc_dai_driver *pdrv;
};

#if (DEBUG_DUMP)
static void i2s_dump_regs(void)
{
	struct NX_AUDIO_RegisterSet* pReg = (struct NX_AUDIO_RegisterSet*)NX_AUDIO_GetBaseAddress();

	printk(KERN_INFO "I2S_CTRL:          %04x\n", pReg->I2S_CTRL);
	printk(KERN_INFO "I2S_CONFIG:        %04x\n", pReg->I2S_CONFIG);
	printk(KERN_INFO "AUDIO_BUFF_CTRL:   %04x\n", pReg->AUDIO_BUFF_CTRL);
	printk(KERN_INFO "AUDIO_BUFF_CONFIG: %04x\n", pReg->AUDIO_BUFF_CONFIG);
	printk(KERN_INFO "AUDIO_STATUS0:     %04x\n", pReg->AUDIO_STATUS0);
	printk(KERN_INFO "AUDIO_STATUS1:     %04x\n", pReg->AUDIO_STATUS1);
	printk(KERN_INFO "AUDIO_IRQ_ENA:     %04x\n", pReg->AUDIO_IRQ_ENA);
	printk(KERN_INFO "AUDIO_IRQ_PEND:    %04x\n", pReg->AUDIO_IRQ_PEND);
	printk(KERN_INFO "CLKENB:            %08x\n", pReg->CLKENB);
	printk(KERN_INFO "CLKGEN0:           %08x\n", pReg->CLKGEN[0][0]);
	printk(KERN_INFO "CLKGEN1:           %08x\n", pReg->CLKGEN[1][0]);
}
#endif

//------------------------------------------------------------------------------
// Nexell i2s control
//------------------------------------------------------------------------------
static void i2s_init_device(struct i2s_plat_data *pdata)
{
	DBGOUT("%s\n", __func__);

	NX_AUDIO_SetBaseAddress(IO_ADDRESS(NX_AUDIO_GetPhysicalAddress()));
	NX_AUDIO_OpenModule();

	/* set I2S clock */
	NX_AUDIO_SetClockDivisorEnable(CFALSE);
	NX_AUDIO_SetClockPClkMode(NX_PCLKMODE_ALWAYS);

	NX_AUDIO_SetClockSource (0, pdata->clk_src0);
	NX_AUDIO_SetClockDivisor(0, pdata->clk_div0);
	NX_AUDIO_SetClockOutInv (0, pdata->clk_inv0 ? CTRUE : CFALSE);

	printk(KERN_INFO "%s:%s.%d NX_AUDIO_GetClockOutInv = %s\n",
		__FILE__,__func__, __LINE__,
		(NX_AUDIO_GetClockOutInv(0) ? "CTRUE" : "CFALSE"));
	printk(KERN_INFO "%s:%s.%d NX_AUDIO_GetClockDivisor = %d\n",
		__FILE__,__func__, __LINE__,
		NX_AUDIO_GetClockDivisor(0));

	NX_AUDIO_SetClockSource (1, 7);	// sync clkgen 0 and 1
	NX_AUDIO_SetClockDivisor(1, pdata->clk_div1);
	NX_AUDIO_SetClockOutInv (1, pdata->clk_inv1 ? CTRUE : CFALSE);

	NX_AUDIO_SetClockOutEnb (1, pdata->master ? CTRUE : CFALSE);
	NX_AUDIO_SetClockDivisorEnable(CTRUE);
	msleep(1);

	NX_AUDIO_SetClockSource(1, pdata->clk_src1);	// set clkgen 1 source
	msleep(1);

	/* set mode(master/slave) */
	NX_AUDIO_SetI2SMasterMode(pdata->master ? CTRUE : CFALSE);

	NX_AUDIO_SetSyncPeriod(pdata->sync_bit);

	/* set I2S misc controller */
	NX_AUDIO_SetI2SControllerReset(CFALSE);

	NX_AUDIO_SetInterfaceMode(pdata->trans_mode);
	NX_AUDIO_SetPCMOUTDataWidth(pdata->pcmin_w);
	NX_AUDIO_SetPCMINDataWidth(pdata->pcmout_w);

	NX_AUDIO_SetI2SLoopBackEnable(CFALSE);
	NX_AUDIO_SetI2SLinkOn();
	NX_AUDIO_SetAudioBufferPCMOUTEnable(CFALSE);
	NX_AUDIO_SetI2SOutputEnable(CFALSE);
	NX_AUDIO_SetAudioBufferPCMINEnable(CFALSE);
	NX_AUDIO_SetI2SInputEnable(CFALSE);
}

static void i2s_exit_device(struct i2s_plat_data *pdata)
{
	DBGOUT("%s\n", __func__);

	NX_AUDIO_SetAudioBufferPCMOUTEnable(CFALSE);
	NX_AUDIO_SetI2SOutputEnable(CFALSE);
	NX_AUDIO_SetAudioBufferPCMINEnable(CFALSE);
	NX_AUDIO_SetI2SInputEnable(CFALSE);

	/* sysnc clkgen 0 and 1 */
	if (CFALSE == (pdata->master ? CTRUE : CFALSE))	{
		NX_AUDIO_SetClockSource(1, 7);	/* set mclk source clock */
		msleep(1);
	}

	NX_AUDIO_SetClockDivisorEnable(CFALSE);
	NX_AUDIO_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);
	NX_AUDIO_CloseModule();
}

static int i2s_start_device(struct snd_pcm_substream *substream)
{
	int ret = 0, count = 10;

	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		NX_AUDIO_SetI2SOutputEnable(CTRUE);
		NX_AUDIO_SetAudioBufferPCMOUTEnable(CTRUE);
	} else {
		NX_AUDIO_SetI2SInputEnable(CTRUE);
		NX_AUDIO_SetAudioBufferPCMINEnable(CTRUE);

		while ((CTRUE != NX_AUDIO_IsPCMInBufferReady()) && (count-- > 0))
			msleep(10);

		if (CTRUE != NX_AUDIO_IsPCMInBufferReady()) {
			printk(KERN_ERR "nexell-i2s: Error, I2S is not ready for capture\n");
			ret= -1;
		}
	}

#if (DEBUG_DUMP)
	i2s_dump_regs();
#endif

	return ret;
}

static void i2s_stop_device(struct snd_pcm_substream *substream)
{
	struct nx_runtime_data *rtd = substream->runtime->private_data;
	struct dma_trans 	   *tr  = rtd->dma_tr;

	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	if (soc_dma_check_run(tr))
		soc_dma_trans_stop(tr);

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		NX_AUDIO_SetI2SOutputEnable(CFALSE);
		NX_AUDIO_SetAudioBufferPCMOUTEnable(CFALSE);
	} else {
		NX_AUDIO_SetAudioBufferPCMINEnable(CFALSE);
		NX_AUDIO_SetI2SInputEnable(CFALSE);
	}
}

/*--------------------------------------------------------------------------------
 * sound i2s dai ops
 ---------------------------------------------------------------------------------*/
static int  nx_i2s_ops_startup(struct snd_pcm_substream *substream,	struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		i2s_start_device(substream);
	return 0;
}

static void nx_i2s_ops_shutdown(struct snd_pcm_substream *substream,struct snd_soc_dai *dai)
{
	struct nx_runtime_data *rtd = substream->runtime->private_data;
	struct dma_trans 	   *tr  = rtd->dma_tr;
	volatile int wait = 0;

	DBGOUT("%s\n", __func__);
	/*
	 *  wait for dma transfer end.
	 */
	while (tr && soc_dma_check_run(tr) &&
			DMA_STOP_TIMEOUT > wait) {
			wait++, msleep(1);
	}

	if (wait >= DMA_STOP_TIMEOUT)
		soc_dma_trans_stop(tr);

	i2s_stop_device(substream);
}

static int nx_i2s_ops_trigger(struct snd_pcm_substream *substream, int cmd, struct snd_soc_dai *dai)
{
	int ret = 0;

	DBGOUT("%s(cmd:0x%x)\n", __func__, cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		ret = i2s_start_device(substream);
		break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		i2s_stop_device(substream);
		break;

	default:
		ret = -EINVAL;
	}
	return ret;
}

static int nx_i2s_ops_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	DBGOUT("%s: rate %d:%d\n", __func__, params->rate_num, params->rate_den);

	// Only need to support 48KHz or 32KHz for now (PLL divisors = 12 or 18)
	if (params->rate_den > 0 && params->rate_num > 0) {
		struct i2s_snd_dev   *i2s   = snd_soc_dai_get_drvdata(dai);
		struct i2s_plat_data *pdata = i2s->pdata;

		if (pdata->sample_rate != params->rate_num / params->rate_den
				|| pdata->clk_div0 != NX_AUDIO_GetClockDivisor(0)) {
			NX_AUDIO_SetClockDivisor(0, pdata->clk_div0 * pdata->sample_rate * params->rate_den / params->rate_num);
			printk(KERN_INFO "%s: rate %d changed to %d:%d\n", __func__, pdata->sample_rate, params->rate_num, params->rate_den);
			printk(KERN_INFO "%s: clkdiv=%d, clkinv=%d\n", __func__, NX_AUDIO_GetClockDivisor(0), NX_AUDIO_GetClockOutInv(0));
		}
	}

	return 0;
}

static int  nx_i2s_ops_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	DBGOUT("%s fmt=%08x\n", __func__, fmt);
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		NX_AUDIO_SetInterfaceMode(NX_AUDIO_IF_I2S);
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		NX_AUDIO_SetInterfaceMode(NX_AUDIO_IF_LEFT);
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		NX_AUDIO_SetInterfaceMode(NX_AUDIO_IF_RIGHT);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int  nx_i2s_ops_set_sysclk(struct snd_soc_dai *dai, int clk_id, unsigned int freq, int dir)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
static int nx_i2s_ops_set_pll(struct snd_soc_dai *codec_dai, int pll_id,
			    int source, unsigned int freq_in,
			    unsigned int freq_out)
#else
static int nx_i2s_ops_set_pll(struct snd_soc_dai *dai,	int pll_id,
				unsigned int freq_in, unsigned int freq_out)
#endif
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_i2s_ops_set_clkdiv(struct snd_soc_dai *dai, int div_id, int div)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
static int nx_i2s_ops_set_tdm_slot(struct snd_soc_dai *dai, unsigned int tx_mask, unsigned int rx_mask,
				int slots, int slot_width)
#else
static int nx_i2s_ops_set_tdm_slot(struct snd_soc_dai *cpu_dai,	unsigned int mask, int slots)
#endif
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_i2s_ops_set_tristate(struct snd_soc_dai *dai, int tristate)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_i2s_ops_digital_mute(struct snd_soc_dai *dai, int mute)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static struct snd_soc_dai_ops nx_i2s_dai_ops = {
	.startup		= nx_i2s_ops_startup,
	.shutdown		= nx_i2s_ops_shutdown,
	.trigger		= nx_i2s_ops_trigger,
	.hw_params		= nx_i2s_ops_hw_params,
	.set_fmt		= nx_i2s_ops_set_fmt,
	.set_sysclk		= nx_i2s_ops_set_sysclk,
	.set_pll 		= nx_i2s_ops_set_pll,
	.set_clkdiv 	= nx_i2s_ops_set_clkdiv,
	.set_tdm_slot 	= nx_i2s_ops_set_tdm_slot,
	.set_tristate 	= nx_i2s_ops_set_tristate,
	.digital_mute 	= nx_i2s_ops_digital_mute,
};

/*--------------------------------------------------------------------------------
 * sound i2s dai suspend/resume
 ---------------------------------------------------------------------------------*/
static int nx_i2s_dai_suspend(struct snd_soc_dai *dai)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_i2s_dai_resume(struct snd_soc_dai *dai)
{
	struct i2s_snd_dev   *i2s   = snd_soc_dai_get_drvdata(dai);
	struct i2s_plat_data *pdata = i2s->pdata;

	PM_DBGOUT("+%s\n", __func__);
	i2s_init_device(pdata);
	PM_DBGOUT("-%s\n", __func__);

	return 0;
}

static int nx_i2s_dai_probe(struct snd_soc_dai *dai)
{
	struct i2s_snd_dev   *i2s   = snd_soc_dai_get_drvdata(dai);
	struct i2s_plat_data *pdata = i2s->pdata;

	DBGOUT("%s\n", __func__);
	i2s_init_device(pdata);

	return 0;
}

static int nx_i2s_dai_remove(struct snd_soc_dai *dai)
{
	struct i2s_snd_dev   *i2s   = snd_soc_dai_get_drvdata(dai);
	struct i2s_plat_data *pdata = i2s->pdata;

	DBGOUT("%s\n", __func__);
	i2s_exit_device(pdata);

	return 0;
}

struct snd_soc_dai_driver nx_snd_i2s_dai = {
	.probe 		= nx_i2s_dai_probe,
	.remove		= nx_i2s_dai_remove,
	.suspend	= nx_i2s_dai_suspend,
	.resume 	= nx_i2s_dai_resume,

	.playback	= {
		.channels_min 	= 2,
		.channels_max 	= 2,
		},

	.capture 	= {
		.channels_min 	= 2,
		.channels_max 	= 2,
		},

	.ops = &nx_i2s_dai_ops,
};

static __devinit int nx_snd_i2s_probe(struct platform_device *pdev)
{
	struct i2s_plat_data 	 * plat = pdev->dev.platform_data;
	struct i2s_snd_dev   	 * i2s  = NULL;
	struct snd_soc_dai_driver* dai  = &nx_snd_i2s_dai;
	int    ret = 0;

	DBGOUT("%s\n", __func__);

    /*  allocate i2c_port data */
    i2s = kzalloc(sizeof(struct i2s_snd_dev), GFP_KERNEL);
    if (!i2s) {
        printk(KERN_ERR "fail, %s allocate driver info ...\n", pdev->name);
        return -ENOMEM;
    }

	dai->playback.rates   = NEXELL_I2S_RATES | SNDRV_PCM_RATE_CONTINUOUS; //plat->sample_rate;
	dai->playback.formats = NEXELL_I2S_FORMATS;

	dai->capture.rates    = NEXELL_I2S_RATES | SNDRV_PCM_RATE_CONTINUOUS; //plat->sample_rate;
	dai->capture.formats  = NEXELL_I2S_FORMATS;

	ret = snd_soc_register_dai(&pdev->dev, dai);
	if (ret)
		goto err_out;

	/* invert MCLK for LFP100/LFP200 */
	plat->clk_inv0 = (lfp100_have_lfp100() ? CTRUE : CFALSE);

	/* insure correct clock divider for runtime PLL */
	{
		int div0 = plat->clk_div0;
		int div1 = plat->clk_div1;
		int div2 = plat->sync_bit;
		int rate = plat->sample_rate;
		int div3 = rate * div1 * div2;
		/* round clock rate up by adding in 1/2 the divisor amount before math truncation */
		int pllx = cpu_get_clock_hz((cfg_sys_clksrc)plat->clk_src0);
		int divx = (pllx + (div3 / 2)) / div3;

		printk(KERN_INFO "%s: requested rate is: %d, new rate is:%d\n",
			pdev->name, rate, (pllx / (divx * div1 * div2 )));

		if (divx != div0) {
			printk(KERN_INFO "%s: changing clk0 from %d to %d for pll=%d\n",
					pdev->name, div0, divx, pllx);
			plat->clk_div0 = divx;
		}
	}

	i2s->pdata = plat;
	i2s->pdrv  = dai;

	platform_set_drvdata(pdev, i2s);

	return ret;

err_out:
	kfree(i2s);
	return ret;
}

static __devexit int nx_snd_i2s_remove(struct platform_device *pdev)
{
	struct i2s_snd_dev *i2s = platform_get_drvdata(pdev);

	DBGOUT("%s\n", __func__);

	snd_soc_unregister_dai(&pdev->dev);

	if (i2s)
		kfree(i2s);

	return 0;
}

/*--------------------------------------------------------------------------------
 * sound i2s platform
 ---------------------------------------------------------------------------------*/
static struct platform_driver nx_snd_i2s_driver = {
	.probe  = nx_snd_i2s_probe,
	.remove = nx_snd_i2s_remove,
	.driver = {
		.name 	= I2S_DEV_NAME,
		.owner 	= THIS_MODULE,
	},
};

static int __init nx_snd_i2s_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_register(&nx_snd_i2s_driver);
}

static void __exit nx_snd_i2s_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&nx_snd_i2s_driver);
}

module_init(nx_snd_i2s_init);
module_exit(nx_snd_i2s_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Sound I2S driver for the Nexell");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:"I2S_DEV_NAME);
MODULE_ALIAS("platform:nx-i2s");

