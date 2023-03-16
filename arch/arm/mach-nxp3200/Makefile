#
# Copyright 2010 Nexell Co.
#
# Makefile for the Nexell CPU

obj-y	:=	cpu.o nxp3200.o timer.o irq.o device.o flash.o alive.o board_pm.o board_gpio.o
obj-y   +=  prototype/module/
obj-y	+=	soc/
obj-y	+=	vr5/
obj-y	+=	keyboard.o

lf2000_idct-objs	:=	soc/idct.o

obj-$(CONFIG_DEBUG_LL_UART) += lldebug.o

obj-$(CONFIG_PM) +=	pm.o sleep.o

obj-$(CONFIG_GENERIC_GPIO) += gpio.o gpio_map.o

#obj-$(CONFIG_SOC_LFP100)	+= lfp100.o

obj-$(CONFIG_LF2000_IDCT)	+= lf2000_idct.o

obj-$(CONFIG_SYSFS)	+=	board.o

#include $(srctree)/arch/arm/mach-nxp3200/libproto.mk

################################################################################
#	Build options for prototype
################################################################################
KBUILD_CFLAGS += -I$(srctree)/$(MACHINE)prototype/nx_base	\
				 -I$(srctree)/$(MACHINE)prototype/module

ifeq ($(CONFIG_PROTOTYPE_NXP3200_REL),y)
KBUILD_CFLAGS += -D__LINUX__ -D__PRINTK__ -DNX_RELEASE
else
KBUILD_CFLAGS += -D__LINUX__ -D__PRINTK__ -DNX_DEBUG
endif

################################################################################
#	Build options for nexell soc
################################################################################
KBUILD_CFLAGS += -mtune=cortex-a9 -mfpu=vfp -mfloat-abi=softfp
# -mfpu=vfpv3
