/*
 * cs42l52.c
 *
 * ASoC Driver for Cirrus Logic CS42L52 codecs
 *
 * Copyright (c) 2010 jh kim <jhim@nexell.co.kr>
 *
 * Based on cs42l52.c - Author: Inchoon, Choi <sonne@bokwang.com>
 * Based on cs42l51.c - Copyright (c) Freescale Semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * For now:
 *  - Only I2C is support. Not SPI
 *  - master mode *NOT* supported
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>

#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>

#include <mach/platform.h>

#define AUDIO_NAME "cs42l52"

#define CS42L52_VERSION "0.1"

#define CS42L52_DEBUG 	(0)
#if CS42L52_DEBUG
#define DBG(x...) printk("cs42l52: " x)
#else
#define DBG(x...)
#endif

#undef	CONFIG_SND_CS42L52_SOC_DAPM

#define CS42L52_ID                  0x01
#define CS42L52_PWRCTL1             0x02
#define CS42L52_PWRCTL2             0x03
#define CS42L52_PWRCTL3             0x04
#define CS42L52_CLKCTL              0x05
#define CS42L52_IFCTL1              0x06
#define CS42L52_IFCTL2              0x07
#define CS42L52_INPUT_A             0x08
#define CS42L52_INPUT_B             0x09
#define CS42L52_ANALOG_HFPCTRL  	0x0A
#define CS42L52_ADC_HPFFREQ         0x0B
#define CS42L52_MISC_ADCCTL         0x0C
#define CS42L52_PLAYCTL1            0x0D
#define CS42L52_MISCCTL             0x0E
#define CS42L52_PLAYCTL2            0x0F
#define CS42L52_MICA_AMPCTL         0x10
#define CS42L52_MICB_AMPCTL         0x11
#define CS42L52_PGAA_VOL            0x12
#define CS42L52_PGAB_VOL            0x13
#define CS42L52_PASSTHRUA_VOL  	 	0x14
#define CS42L52_PASSTHRUB_VOL   	0x15
#define CS42L52_ADCA_VOL            0x16
#define CS42L52_ADCB_VOL            0x17
#define CS42L52_ADCMIXA_VOL			0x18
#define CS42L52_ADCMIXB_VOL         0x19
#define CS42L52_PCMMIXA_VOL         0x1A
#define CS42L52_PCMMIXB_VOL         0x1B
#define CS42L52_BEEP_FREQ           0x1C
#define CS42L52_BEEP_VOL            0x1D
#define CS42L52_BEEP_TONE           0x1E
#define CS42L52_TONECTL             0x1F
#define CS42L52_MASTRA_VOL          0x20
#define CS42L52_NASTRB_VOL          0x21
#define CS42L52_HPA_VOL             0x22
#define CS42L52_HPB_VOL             0x23
#define CS42L52_SPKA_VOL            0x24
#define CS42L52_SPKB_VOL            0x25
#define CS42L52_CH_MIXER            0x26
#define CS42L52_LMT_CTL1            0x27
#define CS42L52_LMT_CTL2            0x28
#define CS42L52_LMT_ATK_RATE    	0x29
#define CS42L52_ALC_CTL1            0x2A
#define CS42L52_ALC_REL_RATE    	0x2B
#define CS42L52_ALC_TH              0x2C
#define CS42L52_NOISE_CTL           0x2D
#define CS42L52_OVFL_CLK            0x2E
#define CS42L52_BATTERY             0x2F
#define CS42L52_VP_BATTER_LV    	0x30
#define CS42L52_SPK_STATUS          0x31
#define CS42L52_CHARGE_PUMP         0x34

#define CS42L52_FIRSTREG            0x01
#define CS42L52_LASTREG             0xFF
#define CS42L52_NUMREGS (CS42L52_LASTREG - CS42L52_FIRSTREG + 1)

/* Bit masks for the CS42L52 registers */
#define CS42L52_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 | SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |\
                                        SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000)

#define CS42L52_FORMATS (SNDRV_PCM_FMTBIT_S16_LE  | SNDRV_PCM_FMTBIT_S16_BE)

struct cs42l52_private {
	enum snd_soc_control_type control_type;
	void *control_data;
};

/*
 * cs42l52 register cache
 */
static const u16 cs42l52_reg[CS42L52_NUMREGS] = {
        0x0000, 0x00E3, 0x0001, 0x0007, 0x0005, 0x0050, 0x0000, 0x0000,
        0x0081, 0x0081, 0x00AA, 0x0000, 0x0000, 0x0060, 0x0000, 0x0000,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0080, 0x0080, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0088,
        0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
        0x007F, 0x00C0, 0x0000, 0x003F, 0x0000, 0x0000, 0x0000, 0x0000,
        0x0000, 0x0000, 0x003B, 0x0000, 0x005F,
};

#ifdef CONFIG_SND_CS42L52_SOC_DAPM
/* DPAM Widget Control */
static const char *cs42l52_input_select[] = {"Mic", "Line In", "", "Mix+Line In"};
static const char *cs42l52_equalizer[] = {"0db", "3dB", "6dB", "9dB", "12dB", "-3dB", "-6dB", "-9dB", "-12dB"};
static const struct soc_enum cs42l52_enum[] = {
        SOC_ENUM_SINGLE(CS42L52_ADC_R, 6, 4, cs42l52_input_select),
        SOC_ENUM_SINGLE(CS42L52_EQ_LP, 0, 9, cs42l52_equalizer),
        SOC_ENUM_SINGLE(CS42L52_EQ_BAND1, 0, 9, cs42l52_equalizer),
        SOC_ENUM_SINGLE(CS42L52_EQ_BAND2, 0, 9, cs42l52_equalizer),
        SOC_ENUM_SINGLE(CS42L52_EQ_BAND3, 0, 9, cs42l52_equalizer),
        SOC_ENUM_SINGLE(CS42L52_EQ_HP, 0, 9, cs42l52_equalizer),
};

/* kcontrol에서의 switch는 mute의 의미를 갖고, 0이면 Off(Mute), 1이면 On의 의미를 갖도록 설정 */
static const struct snd_kcontrol_new cs42l52_snd_controls[] = {
        SOC_DOUBLE_R("Master Playback Volume", CS42L52_DAC_L, CS42L52_DAC_R, 0, 31, 0),
        SOC_SINGLE("Master Playback Switch", CS42L52_DAC_L, 6, 1, 0),
        SOC_DOUBLE_R("Headphone Volume", CS42L52_HPH_OUT_L, CS42L52_HPH_OUT_R, 0, 31, 0),
        SOC_SINGLE("Headphone Switch", CS42L52_HPH_OUT_L, 7, 1, 1),
        SOC_SINGLE("Speaker Switch", CS42L52_LINE_OUT_L, 7, 1, 1),

        SOC_DOUBLE_R("Capture Volume", CS42L52_ADC_L, CS42L52_ADC_R, 0, 31, 0),
        SOC_SINGLE("Capture Switch", CS42L52_ADC_L, 6, 1, 0),
        SOC_DOUBLE_R("Line Capture Boost (+12dB)", CS42L52_LINE_IN_L, CS42L52_LINE_IN_R, 0, 31, 0),
        SOC_DOUBLE_R("Line Capture Switch", CS42L52_LINE_IN_L, CS42L52_LINE_IN_R, 5, 1, 0),
        SOC_DOUBLE_R("Mic Boost (+20dB)", CS42L52_MIC_L, CS42L52_MIC_R, 0, 31, 0),
        SOC_SINGLE("Mic capture Switch", CS42L52_MIC_L, 6, 1, 0),
        SOC_ENUM("Equalizer Low pass", cs42l52_enum[1]),
        SOC_ENUM("Equalizer Band pass 1", cs42l52_enum[2]),
        SOC_ENUM("Equalizer Band pass 2", cs42l52_enum[3]),
        SOC_ENUM("Equalizer Band pass 3", cs42l52_enum[4]),
        SOC_ENUM("Equalizer High pass", cs42l52_enum[5]),
};

/* Output Mixer */ // --> register field means 1 is OFF!

static const struct snd_kcontrol_new cs42l52_output_mixer_controls[] = {
        SOC_DAPM_SINGLE("Line Bypass Switch", CS42L52_SET3, 7, 1, 1),
        SOC_DAPM_SINGLE("Mic Sidetone Switch", CS42L52_SET3, 6, 1, 1),
        SOC_DAPM_SINGLE("HiFi Playback Switch", CS42L52_SET3, 5, 1, 1),
};

/* Input mux */

static const struct snd_kcontrol_new cs42l52_input_mux_controls =
                                                        SOC_DAPM_ENUM("Input Select", cs42l52_enum[0]);

static const struct snd_soc_dapm_widget cs42l52_dapm_widgets[] = {
        SND_SOC_DAPM_MIXER("Output Mixer", CS42L52_SET1, 2, 0, &cs42l52_output_mixer_controls[0], ARRAY_SIZE(cs42l52_output_mixer_controls)),
        SND_SOC_DAPM_DAC("DAC", "HiFi Playback", SND_SOC_NOPM, 6, 0),
        SND_SOC_DAPM_OUTPUT("LOUT"),
        SND_SOC_DAPM_OUTPUT("LHPOUT"),
        SND_SOC_DAPM_OUTPUT("ROUT"),
        SND_SOC_DAPM_OUTPUT("RHPOUT"),
        SND_SOC_DAPM_ADC("ADC", "HiFi Capture", CS42L52_ADC_L, 5, 0),
        SND_SOC_DAPM_MUX("Input Mux", SND_SOC_NOPM, 0, 0, &cs42l52_input_mux_controls),
        SND_SOC_DAPM_PGA("Line Input", CS42L52_SET1, 0, 0, NULL, 0),
        SND_SOC_DAPM_MICBIAS("Mic Bias", CS42L52_MIC_L, 6, 0),
        SND_SOC_DAPM_INPUT("MICIN"),
        SND_SOC_DAPM_INPUT("RLINEIN"),
        SND_SOC_DAPM_INPUT("LLINEIN"),
};

static const struct snd_soc_dapm_route cs42l52_routes[] = {
        /* output mixer */
        {"Output Mixer", "Line Bypass Switch", "Line Input"},
        {"Output Mixer", "HiFi Playback Switch", "DAC"},
        {"Output Mixer", "Mic Sidetone Switch", "Mic Bias"},

        /* outputs */
        {"RHPOUT", NULL, "Output Mixer"},
        {"ROUT", NULL, "Output Mixer"},
        {"LHPOUT", NULL, "Output Mixer"},
        {"LOUT", NULL, "Output Mixer"},

        /* input mux */
        {"ADC", NULL, "Input Mux"},
        {"Input Mux", "Line In", "Line Input"},
        {"Input Mux", "Mic", "Mic Bias"},

        /* inputs */
        {"Line Input", NULL, "LLINEIN"},
        {"Line Input", NULL, "RLINEIN"},
        {"Mic Bias", NULL, "MICIN"},
};
#endif // CONFIG_SND_CS42L52_SOC_DAPM


static int cs42l52_set_bias_level(struct snd_soc_codec *codec, enum snd_soc_bias_level level)
{
	unsigned int regs;

	DBG("Entered %s - %d\n", __func__, level);

	switch (level) {
	case SND_SOC_BIAS_ON:

		// HPx0 and Speaker Power On
		snd_soc_write(codec, CS42L52_PWRCTL3, 0xAF);
		regs = snd_soc_read(codec, CS42L52_PWRCTL1);
		if (!(regs & 0x18))
			regs = snd_soc_write(codec, CS42L52_PWRCTL1, 0x18);

		break;
	case SND_SOC_BIAS_PREPARE:
		//PREPARE: Prepare for audio operations. Called before DAPM switching for stream start and stop operations.
		break;
	case SND_SOC_BIAS_STANDBY:
		/* STANDBY: Low power standby state when no playback/capture operations are
					in progress. NOTE: The transition time between STANDBY and ON
					should be as fast as possible and no longer than 10ms.
		*/
		snd_soc_write(codec, CS42L52_PWRCTL3, 0xAF);	// HPx0 and Speaker Power On
		snd_soc_write(codec, CS42L52_PWRCTL1, 0x0);		// PGAA/B, ADCA/B Power On
		break;
	case SND_SOC_BIAS_OFF:
		//OFF: 	Power Off. No restrictions on transition times.
		snd_soc_write(codec, CS42L52_PWRCTL1, 0xFF);
		break;
	}

//	codec->bias_level = level;
	return 0;
}

static int cs42l52_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	DBG("Entered %s\n", __func__);
	return 0;
}

/* add non dapm controls */
static int cs42l52_startup(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
//	struct snd_soc_pcm_runtime *rtd = substream->private_data;
//	struct snd_soc_codec *codec = rtd->codec;
//	unsigned int val;
//
//	DBG("Entered %s\n", __func__);
//
//	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
//	} else {
//		val = snd_soc_read(codec, CS42L52_PWRCTL3);
//		val &= ~0x7;
//		snd_soc_write(codec, CS42L52_PWRCTL3, val);
//	}
	return 0;
}

static int cs42l52_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	unsigned int regs;

	DBG("+Entered %s - %d\n", __func__, mute);

	if (!codec)
		return -EINVAL;

	if (mute){
		// PCM Mix mute on
		regs = snd_soc_read(codec, CS42L52_PCMMIXA_VOL);
		regs |= 0xF0;	snd_soc_write(codec, CS42L52_PCMMIXA_VOL, regs);
		regs = snd_soc_read(codec, CS42L52_PCMMIXB_VOL);
		regs |= 0xF0;	snd_soc_write(codec, CS42L52_PCMMIXB_VOL, regs);
	} else {
		// PCM Mix mute off
		regs = snd_soc_read(codec, CS42L52_PCMMIXA_VOL);
		regs &= ~0xF0;	snd_soc_write(codec, CS42L52_PCMMIXA_VOL, regs);
		regs = snd_soc_read(codec, CS42L52_PCMMIXB_VOL);
		regs &= ~0xF0;	snd_soc_write(codec, CS42L52_PCMMIXB_VOL, regs);
	}

	DBG("-Entered %s - %d\n", __func__, mute);
	return 0;
}

static struct snd_soc_dai_ops cs42l52_dai_ops = {
	.digital_mute 	= cs42l52_mute,
	.startup 		= cs42l52_startup,
	.set_fmt 		= cs42l52_set_dai_fmt,
};

/*
 * ASoC probe function
 *
 * This function is called when the machine driver calls
 * platform_device_add().
 */
static int cs42l52_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	PM_DBGOUT("+%s (active:%d)\n", __func__, codec->active);

	if (!codec)
		return -EINVAL;

	cs42l52_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

static int cs42l52_resume(struct snd_soc_codec *codec)
{
	unsigned int val;

	PM_DBGOUT("+%s (active:%d)\n", __func__, codec->active);

	if (!codec)
		return -EINVAL;

	if (codec->active) {

		val = snd_soc_read(codec, CS42L52_ID);
		if ((val & 0xF8) != 0xE0) {
			printk("CS42L52 ChipID read error : 0x%x->0x%x\n", val, (val & 0xF8) >> 3);
			return -1;
		}
		DBG("CS42L52 CHIPID : %x, REVID : %x\n", (val & 0xF8) >> 3, val & 0x7);

		/* initialize CS42L52 */
		snd_soc_write(codec, CS42L52_PWRCTL1, 0x1F);		// power down

		// Required initialization setting
		// see p.37 of datasheet
		snd_soc_write(codec, 0x00, 0x99);
		snd_soc_write(codec, 0x3E, 0xBA);
		snd_soc_write(codec, 0x47, 0x80);

		val = snd_soc_read(codec, 0x32);
		snd_soc_write(codec, 0x32, val | (1 << 7));
		snd_soc_write(codec, 0x32, val & ~(1 << 7));

		snd_soc_write(codec, 0x00, 0x00);
		// End of required initialization setting..

		// Power down
		snd_soc_write(codec, CS42L52_PWRCTL2, 0x07);		// Mic. Power Down
		snd_soc_write(codec, CS42L52_PWRCTL3, 0xFF);		// All Power On

		// Interface set
		snd_soc_write(codec, CS42L52_IFCTL1, 0x27);			// 0010 0111
		snd_soc_write(codec, CS42L52_IFCTL2, 0x00); 		// 0000 0000

		// 0x81 (PGA to ADC)
		// Input is AIN1x
		snd_soc_write(codec, CS42L52_INPUT_A, 0x00);		// AIN1A
		snd_soc_write(codec, CS42L52_INPUT_B, 0x00);		// AIN1B

		snd_soc_write(codec, CS42L52_MISC_ADCCTL, 0x80);	// ADC Control ( Not Invert ), DIGSUM, DIGMUX
		snd_soc_write(codec, CS42L52_PLAYCTL1, 0x10);		// ADC Control ( Not Invert ), DIGSUM, DIGMUX

		// Miscellaneous Control, DIGSFT, DIGZC. Passthrough Mute
		snd_soc_write(codec, CS42L52_MISCCTL, 0x32);

		snd_soc_write(codec, CS42L52_ADCA_VOL, 0x00);		// ADCA Volume 0 dB
		snd_soc_write(codec, CS42L52_ADCB_VOL, 0x00);		// ADCB Volume 0 dB

		// Mute ADCMIX (loop-back sound)
		snd_soc_write(codec, CS42L52_ADCMIXA_VOL, 0x80);
		snd_soc_write(codec, CS42L52_ADCMIXB_VOL, 0x80);

		// default volume
		snd_soc_write(codec, CS42L52_HPA_VOL, 0xF0);		// set volume
		snd_soc_write(codec, CS42L52_HPB_VOL, 0xF0);		//		  "
		snd_soc_write(codec, CS42L52_SPKA_VOL, 0xF0);		//		  "
		snd_soc_write(codec, CS42L52_SPKB_VOL, 0xF0);		//		  "

		cs42l52_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
		cs42l52_set_bias_level(codec, SND_SOC_BIAS_PREPARE);
		cs42l52_set_bias_level(codec, SND_SOC_BIAS_ON);
	}

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

//---------------------------------------------------------------------
static int cs42l52_probe(struct snd_soc_codec *codec)
{
	struct cs42l52_private *cs42l52 = snd_soc_codec_get_drvdata(codec);
	int ret, val;

	DBG("Entered %s\n", __func__);

	codec->control_data = cs42l52->control_data;
	codec->reg_cache    = kmemdup(cs42l52_reg, sizeof(cs42l52_reg), GFP_KERNEL);

	ret = snd_soc_codec_set_cache_io(codec, 8, 8, cs42l52->control_type);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		return ret;
	}

	/*
	 * Output
	 */
	snd_soc_write(codec, CS42L52_PWRCTL1, 0x1F);		// power down

	// Required initialization setting
	// see p.37 of datasheet
	snd_soc_write(codec, 0x00, 0x99);
	snd_soc_write(codec, 0x3E, 0xBA);
	snd_soc_write(codec, 0x47, 0x80);
	val = snd_soc_read(codec, 0x32);

	snd_soc_write(codec, 0x32, val | (1 << 7));
	snd_soc_write(codec, 0x32, val & ~(1 << 7));
	snd_soc_write(codec, 0x00, 0x00);
	// End of required initialization setting..

	// Power down
	snd_soc_write(codec, CS42L52_PWRCTL2, 0x07);		// Mic. Power Down
	snd_soc_write(codec, CS42L52_PWRCTL3, 0xFF);		// All Power On

	// Interface set
	snd_soc_write(codec, CS42L52_IFCTL1, 0x27);			// 0010 0111
	snd_soc_write(codec, CS42L52_IFCTL2, 0x00); 		// 0000 0000

	// 0x81 (PGA to ADC)
	// Input is AIN1x
	snd_soc_write(codec, CS42L52_INPUT_A, 0x00);		// AIN1A
	snd_soc_write(codec, CS42L52_INPUT_B, 0x00);		// AIN1B
	snd_soc_write(codec, CS42L52_MISC_ADCCTL, 0x80);	// ADC Control ( Not Invert ), DIGSUM, DIGMUX
	snd_soc_write(codec, CS42L52_PLAYCTL1, 0x10);		// ADC Control ( Not Invert ), DIGSUM, DIGMUX

	// Miscellaneous Control, DIGSFT, DIGZC. Passthrough Mute
	snd_soc_write(codec, CS42L52_MISCCTL, 0x32);

	snd_soc_write(codec, CS42L52_ADCA_VOL, 0x00);		// ADCA Volume 0 dB
	snd_soc_write(codec, CS42L52_ADCB_VOL, 0x00);		// ADCB Volume 0 dB

	// Mute ADCMIX (loop-back sound)
	snd_soc_write(codec, CS42L52_ADCMIXA_VOL, 0x80);
	snd_soc_write(codec, CS42L52_ADCMIXB_VOL, 0x80);

	// default volume
	snd_soc_write(codec, CS42L52_HPA_VOL, 0xF0);		// set volume
	snd_soc_write(codec, CS42L52_HPB_VOL, 0xF0);		//		  "
	snd_soc_write(codec, CS42L52_SPKA_VOL, 0xF0);		//		  "
	snd_soc_write(codec, CS42L52_SPKB_VOL, 0xF0);		//		  "

	/*
	 * input line1
	 */
	snd_soc_write(codec, CS42L52_INPUT_A, (0<<5));		// CS42L52_INPUT_A (0x08), AIN1A
	snd_soc_write(codec, CS42L52_INPUT_B, (0<<5));		// CS42L52_INPUT_B (0x09), AIN1B
	snd_soc_write(codec, CS42L52_MISC_ADCCTL, 0x00);	// CS42L52_MISC_ADCCTL (0x0C), ADC Control ( Not Invert ), DIGSUM, DIGMUX
	snd_soc_write(codec, CS42L52_ANALOG_HFPCTRL, 0xA5);	// CS42L52_ANALOG_HFPCTRL (0x0A), HPF Control( default value );
	snd_soc_write(codec, CS42L52_ADC_HPFFREQ, 0x00);	// CS42L52_ADC_HPFFREQ (0x0B), HPF Corner Frequency( default value );
	snd_soc_write(codec, CS42L52_ADCA_VOL, 0x00);		// CS42L52_ADCA_VOL (0x16), ADCA Volume 0 dB
	snd_soc_write(codec, CS42L52_ADCB_VOL, 0x00);		// CS42L52_ADCB_VOL (0x17), ADCB Volume 0 dB
	snd_soc_write(codec, CS42L52_MISCCTL, 0x02);		// CS42L52_MISCCTL (0x0E), Miscellaneous Control, DIGSFT, DIGZC

	/* set bias level */
	cs42l52_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

#ifdef CONFIG_SND_CS42L52_SOC_DAPM
	snd_soc_add_codec_controls(codec, cs42l52_snd_controls,
		ARRAY_SIZE(cs42l52_snd_controls));
	snd_soc_dapm_new_controls(codec, cs42l52_dapm_widgets,
		ARRAY_SIZE(cs42l52_dapm_widgets));
	snd_soc_dapm_add_routes(codec, cs42l52_routes,
		ARRAY_SIZE(cs42l52_routes));
#endif
	DBG("Done %s\n", __func__);
	return 0;
}

static int cs42l52_remove(struct snd_soc_codec *codec)
{
	DBG("Entered %s\n", __func__);
	snd_soc_write(codec, CS42L52_PWRCTL3, 0xFF);	// HPx0 and Speaker Power On
	snd_soc_write(codec, CS42L52_PWRCTL1, 0x1F);	// power down
	return 0;
}

static struct snd_soc_codec_driver soc_codec_device_cs42l52 = {
	.probe 			= cs42l52_probe,
	.remove			= cs42l52_remove,
	.suspend		= cs42l52_suspend,
	.resume			= cs42l52_resume,
	.set_bias_level = cs42l52_set_bias_level,
	.reg_cache_size = CS42L52_NUMREGS,
	.reg_word_size 	= sizeof(u8),
};

static struct snd_soc_dai_driver cs42l52_dai = {
	.name = "cs42l52-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_96000,
		.formats = CS42L52_FORMATS,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_96000,
		.formats = CS42L52_FORMATS,
	},
	.ops = &cs42l52_dai_ops,
};

static int cs42l52_i2c_probe(struct i2c_client *i2c_client,
			const struct i2c_device_id *id)
{
	struct cs42l52_private *cs42l52;
    int ret = 0;

	DBG("Entered %s\n", __func__);

    ret = i2c_smbus_read_byte_data(i2c_client, CS42L52_ID);
	if ((ret & 0xF8) != 0xE0) {
		dev_err(&i2c_client->dev, "failed to read I2C\n");
		goto error;
	}

	dev_info(&i2c_client->dev, "found device cs42l52 rev %d\n", ret & 7);

	cs42l52 = kzalloc(sizeof(struct cs42l52_private), GFP_KERNEL);
	if (!cs42l52) {
		dev_err(&i2c_client->dev, "could not allocate codec\n");
		return -ENOMEM;
	}

	i2c_set_clientdata(i2c_client, cs42l52);
	cs42l52->control_data = i2c_client;
	cs42l52->control_type = SND_SOC_I2C;

	ret =  snd_soc_register_codec(&i2c_client->dev,
					&soc_codec_device_cs42l52, &cs42l52_dai, 1);
	if (ret < 0)
		kfree(cs42l52);
error:
	return ret;
}

static int cs42l52_i2c_remove(struct i2c_client *client)
{
	struct cs42l52_private *cs42l52 = i2c_get_clientdata(client);

	snd_soc_unregister_codec(&client->dev);
	kfree(cs42l52);
	return 0;
}

static const struct i2c_device_id cs42l52_id[] = {
	{"cs42l52", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, cs42l52_id);

static struct i2c_driver cs42l52_i2c_driver = {
	.driver = {
		.name = "cs42l52-codec",
		.owner = THIS_MODULE,
	},
	.id_table = cs42l52_id,
	.probe  = cs42l52_i2c_probe,
	.remove = cs42l52_i2c_remove,
};

static int __init cs42l52_init(void)
{
	int ret;

	ret = i2c_add_driver(&cs42l52_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "%s: can't add i2c driver\n", __func__);
		return ret;
	}
	return 0;
}
module_init(cs42l52_init);

static void __exit cs42l52_exit(void)
{
	i2c_del_driver(&cs42l52_i2c_driver);
}
module_exit(cs42l52_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Cirrus Logic CS42L52 ALSA SoC Codec Driver");
MODULE_LICENSE("GPL");
