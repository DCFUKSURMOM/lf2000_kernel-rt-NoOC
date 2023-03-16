#
# Copyright 2010 Nexell Co.
#
# Makefile for the Nexell Sound

obj-$(CONFIG_SND_NEXELL_SOC)		+= nexell-snd-pcm.o
nexell-snd-pcm-objs					:= nexell-pcm.o

obj-$(CONFIG_SND_NEXELL_SOC_I2S)	+= nexell-snd-i2s.o
nexell-snd-i2s-objs 				:= nexell-i2s.o

obj-$(CONFIG_SND_NEXELL_SOC_AC97) 	+= nexell-snd-ac97.o
nexell-snd-ac97-objs 				:= nexell-ac97.o

obj-$(CONFIG_SND_CODEC_CS42L52) 	+= nexell-snd-cs42l52.o
nexell-snd-cs42l52-objs 			:= nexell-cs42l52.o

obj-$(CONFIG_SND_CODEC_LFP100) 		+= nexell-snd-lfp100.o
nexell-snd-lfp100-objs 				:= didj-lfp100.o

################################################################################
# Module build:
# 	Link Nexell prototype function
################################################################################

ifeq ($(CONFIG_ARCH_NXP3200), y)
CPU	:= nxp3200
endif

LINUX_TOP := ../../../
PROTOTYPE := $(LINUX_TOP)arch/arm/mach-$(CPU)/prototype/module

ifeq ($(CONFIG_SND_NEXELL_SOC_I2S),m)
nexell-snd-i2s-objs		+= $(PROTOTYPE)/nx_audio.o
endif

ifeq ($(CONFIG_SND_NEXELL_SOC_AC97),m)
nexell-snd-ac97-objs	+= $(PROTOTYPE)/nx_audio.o
endif

