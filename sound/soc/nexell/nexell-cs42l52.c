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

#include <linux/module.h>
#include <linux/platform_device.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>

#include <mach/platform.h>

#include "nexell-i2s.h"
#include "nexell-pcm.h"
#include "nexell-cs42l52.h"

#if (1)
#define DBGOUT(msg...)		{ printk(KERN_INFO "cs42l52-asoc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*--------------------------------------------------------------------------------
 * sound soc ops
 */
static int cs42l52_codec_startup(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static void cs42l52_codec_shutdown(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);
}

static int cs42l52_codec_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	int ret = 0;

	DBGOUT("%s\n", __func__);

	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	return 0;
}


static int (* cpu_resume_fn)(struct snd_soc_dai *dai) = NULL;

static int cs42l52_codec_resume_pre(struct platform_device *pdev)
{
	struct snd_soc_card *card = platform_get_drvdata(pdev);
	struct snd_soc_dai *cpu_dai = card->rtd->cpu_dai;
	int ret = 0;

	PM_DBGOUT("+%s\n", __func__);

	/*
	 * first execute cpu(i2s) resume and execute codec resume.
	 */
	if (cpu_dai->driver->resume && ! cpu_resume_fn) {
		cpu_resume_fn  = cpu_dai->driver->resume;
		cpu_dai->driver->resume = NULL;
	}

	if (cpu_resume_fn)
		ret = cpu_resume_fn(cpu_dai);

	PM_DBGOUT("-%s\n", __func__);

	return ret;
}

/*--------------------------------------------------------------------------------
 * sound soc struct
 */
static struct snd_soc_ops cs42l52_codec_ops = {
	.startup 		= cs42l52_codec_startup,
	.shutdown 		= cs42l52_codec_shutdown,
	.hw_params 		= cs42l52_codec_hw_params,
};

static struct snd_soc_dai_link cs42l52_dai_link = {
	.name 			= "ASOC-CS42L52",
	.stream_name 	= "CS42L52 HiFi",
	.cpu_dai_name 	= NEXELL_SND_SOC_DAI_I2S,	/* nx_snd_i2s_driver name */
	.platform_name  = NEXELL_SND_SOC_DAI_PCM,	/* nx_snd_pcm_driver name */
	.codec_dai_name = "cs42l52-hifi",			/* cs42l52_dai's name */
	.codec_name 	= "cs42l52-codec.0-004a",	/* cs42l52_i2c_driver name + i2c bus-address */
	.ops 			= &cs42l52_codec_ops,
};

static struct snd_soc_card cs42l52_card = {
	.name 			= "soc-audio-cs42l52",
	.dai_link 		= &cs42l52_dai_link,
	.num_links 		= 1,
	.resume_pre		= &cs42l52_codec_resume_pre,
};

/*--------------------------------------------------------------------------------
 * sound pcm platform
 ---------------------------------------------------------------------------------*/
static struct platform_device * cs42l52_plat_device;

static int __init nx_snd_codec_init(void)
{
	struct snd_soc_card *card = &cs42l52_card;
	struct snd_soc_dai *cpu_dai = NULL;
	int ret = 0;

	DBGOUT("%s\n", __func__);

	cs42l52_plat_device = platform_device_alloc("soc-audio", -1);
	if (! cs42l52_plat_device) {
		printk(KERN_ERR "%s: fail platform_device_alloc for codec ...\n", __func__);
		return -ENOMEM;
	}

	platform_set_drvdata(cs42l52_plat_device, &cs42l52_card);

	ret = platform_device_add(cs42l52_plat_device);
	if (ret) {
		platform_device_put(cs42l52_plat_device);
		return ret;
	}

	cpu_dai = card->rtd->cpu_dai;

	/* set i2s rates, format */
#if 0
	cpu_dai->playback.rates   = NEXELL_I2S_RATES;
	cpu_dai->playback.formats = NEXELL_I2S_FORMATS;
	cpu_dai->capture.rates    = NEXELL_I2S_RATES;
	cpu_dai->capture.formats  = NEXELL_I2S_FORMATS;
#endif

	return ret;
}

static void __exit nx_snd_codec_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_device_unregister(cs42l52_plat_device);
}

module_init(nx_snd_codec_init);
module_exit(nx_snd_codec_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Sound codec-cs42l52 driver for the Nexell");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:soc-audio");
