#
# Copyright 2010 Nexell Co.
#
# Makefile for the Nexell VIP (Video Input Processor)

obj-$(CONFIG_GRP_NEXELL_VIP)	+= vip-drv.o
vip-drv-objs					:= vip_update.o vip_drv.o

################################################################################
# Module build:
# 	Link Nexell prototype function
################################################################################

ifeq ($(CONFIG_ARCH_NXP3200), y)
CPU	:= nxp3200
endif

LINUX_TOP := ../../../../
PROTOTYPE := $(LINUX_TOP)arch/arm/mach-$(CPU)/prototype/module

ifeq ($(CONFIG_GRP_NEXELL_VIP), m)
vip-drv-objs		+= $(PROTOTYPE)/nx_vip.o
endif