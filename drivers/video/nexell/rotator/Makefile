#
# Portions Copyright 2010 Nexell.
#
# Makefile for the LF2000 ROTATOR

obj-$(CONFIG_GRP_LF2000_ROTATOR)	+= rotator-drv.o
rotator-drv-objs					:= rotator_drv.o

################################################################################
# Module build:
# 	Link Nexell prototype function
################################################################################

ifeq ($(CONFIG_ARCH_NXP3200), y)
CPU	:= nxp3200
endif

LINUX_TOP := ../../../../
PROTOTYPE := $(LINUX_TOP)arch/arm/mach-$(CPU)/prototype/module

ifeq ($(CONFIG_GRP_LF2000_ROTATOR), m)
rotator-drv-objs					+= $(PROTOTYPE)/nx_rotator.o
endif