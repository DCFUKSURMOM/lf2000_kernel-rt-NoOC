/*
 * linux/sound/soc/lf1000/didj-lfp100.c
 *
 * ALSA Machine driver for the LeapFrog LF1000 Didj style game console.
 * Supports the audio codec part of the LeapFrog LFP100 power/codec chip,
 * connected to the LF1000 SoC's I2S controller.
 *
 * Author: Scott Esters <sesters@leapfrog.com>
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation. 
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/sysfs.h>
#include <linux/i2c.h>

#include <sound/core.h>
#include <sound/soc.h>
#include <sound/pcm.h>

#include <mach/gpio.h>
#include <mach/platform_id.h>

//#include "lf1000-pcm.h"
//#include "lf1000-i2s.h"
#include "nexell-pcm.h"
#include "nexell-i2s.h"
#include "didj-lfp100.h"
#include "../codecs/lfp100.h"

#define DIDJ_DEFAULT_RATE	32000

static struct platform_device *didj_snd_device = NULL;

/*
 * Board ops
 */

static int didj_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = snd_pcm_substream_chip(substream);
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	int ret = 0;

	printk(KERN_INFO "%s: \n", __FUNCTION__);

	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret)
		return ret;

	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret)
		return ret;

	ret = snd_soc_dai_set_sysclk(cpu_dai, 0, DIDJ_DEFAULT_RATE, 1);
	if (ret)
		return ret;

	return 0;
}

static int didj_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	int ret = 0;

	printk(KERN_INFO "%s: \n", __FUNCTION__);

	/* set codec DAI configuration (slave clock) */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret) {
		printk(KERN_ERR "%s: snd_soc_dai_set_fmt ret=%d\n", __FUNCTION__, ret);
		return ret;
	}

	/* set cpu DAI configuration (I2S controller) */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret) {
		printk(KERN_ERR "%s: snd_soc_dai_set_fmt ret=%d\n", __FUNCTION__, ret);
		return ret;
	}

	ret = snd_soc_dai_set_sysclk(cpu_dai, 0, params_rate(params), 1);
	if (ret) {
		printk(KERN_ERR "%s: snd_soc_dai_set_sysclk ret=%d\n", __FUNCTION__, ret);
		return ret;
	}

	return 0;
}

static void didj_shutdown(struct snd_pcm_substream *substream)
{
	printk(KERN_INFO "%s: \n", __FUNCTION__);
	return;
}

static int didj_lfp100_probe(struct snd_soc_card *card)
{
	printk(KERN_INFO "%s: \n", __FUNCTION__);
	return 0;
}

static int didj_lfp100_resume_pre(struct snd_soc_card *card)
{
	printk(KERN_INFO "%s: \n", __FUNCTION__);
	return 0;
}

static int didj_lfp100_init(struct snd_soc_pcm_runtime *rtd)
{
	printk(KERN_INFO "%s: \n", __FUNCTION__);

	{
		struct snd_soc_codec *codec;
		int i;

		/* program codec defaults */
		codec = rtd->codec;
		if (get_leapfrog_platform() == RIO) {
			for (i = 0; i < ARRAY_SIZE(lfp100_rio_settings); i++) {
				codec->driver->write(codec,
					lfp100_rio_settings[i][0],
					lfp100_rio_settings[i][1]);
			}
		} else {
			for (i = 0; i < ARRAY_SIZE(lfp100_common_settings); i++) {
				codec->driver->write(codec,
					lfp100_common_settings[i][0],
					lfp100_common_settings[i][1]);
			}
		}
	}

	return 0;
}

static struct snd_soc_ops didj_ops = {
	.startup	= didj_startup,
	.shutdown	= didj_shutdown,
	.hw_params	= didj_hw_params,
};

/* didj digital audio interface glue - connects codec <--> CPU */

static struct snd_soc_dai_link didj_dai_lfp100_i2c_0[] = {
	{
		.name = "ASOC-LFP100",
		.stream_name = "LFP100 HiFi",
		.cpu_dai_name 	= NEXELL_SND_SOC_DAI_I2S,	/* nx_snd_i2s_driver name */
		.platform_name  = NEXELL_SND_SOC_DAI_PCM,	/* nx_snd_pcm_driver name */
		.codec_dai_name = "lfp100-hifi",			/* lfp100_dai's name */
		.codec_name 	= "lfp100-codec.0-0066",	/* lfp100_i2c_driver name + i2c bus-address */
		.init			= didj_lfp100_init,
		.ops = &didj_ops,
	},
};

/* didj digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link didj_dai_lfp100_i2c_1[] = {
	{
		.name = "ASOC-LFP100",
		.stream_name = "LFP100 HiFi",
		.cpu_dai_name 	= NEXELL_SND_SOC_DAI_I2S,	/* nx_snd_i2s_driver name */
		.platform_name  = NEXELL_SND_SOC_DAI_PCM,	/* nx_snd_pcm_driver name */
		.codec_dai_name = "lfp100-hifi",			/* lfp100_dai's name */
		.codec_name 	= "lfp100-codec.1-0066",	/* lfp100_i2c_driver name + i2c bus-address */
		.init			= didj_lfp100_init,
		.ops = &didj_ops,
	},
};

/* didj audio machine driver */
static struct snd_soc_card snd_soc_didj_lfp100_i2c_0 = {
	.name = "soc-audio-lfp100", //"Didj-LFP100",
	.dai_link = &didj_dai_lfp100_i2c_0[0],
	.num_links = ARRAY_SIZE(didj_dai_lfp100_i2c_0),
	.probe = &didj_lfp100_probe,
	.resume_pre = &didj_lfp100_resume_pre,
};

static struct snd_soc_card snd_soc_didj_lfp100_i2c_1 = {
	.name = "soc-audio-lfp100", //"Didj-LFP100",
	.dai_link = &didj_dai_lfp100_i2c_1[0],
	.num_links = ARRAY_SIZE(didj_dai_lfp100_i2c_1),
	.probe = &didj_lfp100_probe,
	.resume_pre = &didj_lfp100_resume_pre,
};

/*
 * sysfs interface
 */

static ssize_t show_force_audio(struct device *dev,
                struct device_attribute *attr, char *buf)
{
	struct lfp100_private *lfp100;
	if (is_board_lucy())
		lfp100 = i2c_get_clientdata(snd_soc_didj_lfp100_i2c_1.rtd->codec->control_data);
	else
		lfp100 = i2c_get_clientdata(snd_soc_didj_lfp100_i2c_0.rtd->codec->control_data);

        return(sprintf(buf,"%u\n", lfp100->force_audio));
}

static ssize_t set_force_audio(struct device *dev,
                struct device_attribute *attr, const char *buf, size_t count)
{
        unsigned int value;
	struct lfp100_private *lfp100;

	if (is_board_lucy())
		lfp100 = i2c_get_clientdata(snd_soc_didj_lfp100_i2c_1.rtd->codec->control_data);
	else
		lfp100 = i2c_get_clientdata(snd_soc_didj_lfp100_i2c_0.rtd->codec->control_data);

        if (sscanf(buf, "%x", &value) != 1)
                return -EINVAL;

        lfp100->force_audio = value;
        return count;
}

static DEVICE_ATTR(force_audio, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH,
        show_force_audio, set_force_audio);

static struct attribute *lfp100_attributes[] = {
        &dev_attr_force_audio.attr,
        NULL
};

static struct attribute_group lfp100_attr_group = {
	.attrs = lfp100_attributes
};

static int didj_audio_probe(struct platform_device *pdev)
{
	int ret;

	printk(KERN_INFO "%s: \n", __FUNCTION__);

	didj_snd_device = platform_device_alloc("soc-audio", -1);
	if (!didj_snd_device)
		return -ENOMEM;

	if (is_board_lucy())
		platform_set_drvdata(didj_snd_device, &snd_soc_didj_lfp100_i2c_1);
	else
		platform_set_drvdata(didj_snd_device, &snd_soc_didj_lfp100_i2c_0);

	ret = platform_device_add(didj_snd_device);
	if (ret) {
		printk(KERN_ERR "can't add sound device\n");
		platform_device_put(didj_snd_device);
		return ret;
	}

	ret = sysfs_create_group(&pdev->dev.kobj, &lfp100_attr_group);
	return 0;
}

static int __devexit didj_audio_remove(struct platform_device *pdev)
{
	platform_device_unregister(didj_snd_device);
	sysfs_remove_group(&pdev->dev.kobj, &lfp100_attr_group);
	didj_snd_device = NULL;

	return 0;
}

static struct platform_driver didj_audio_driver = {
	.probe	= didj_audio_probe,
	.remove	= didj_audio_remove,
	.driver	= {
		.name	= "lfp100-asoc",
		.owner	= THIS_MODULE,
	},
};

static int __init didj_audio_init(void)
{
	printk(KERN_INFO "%s: \n", __FUNCTION__);
	return platform_driver_register(&didj_audio_driver);
}
module_init(didj_audio_init);

static void __exit didj_audio_exit(void)
{
	printk(KERN_INFO "%s: \n", __FUNCTION__);
	platform_driver_unregister(&didj_audio_driver);
}
module_exit(didj_audio_exit);

MODULE_AUTHOR("Scott Esters <sesters@leapfrog.com>");
MODULE_DESCRIPTION("ALSA SoC Didj Consoles");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:soc-audio");
