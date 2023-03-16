#
# Copyright 2010 Nexell Co.
#
# Makefile for the LF2000 CSC (Color Space Converter)

obj-$(CONFIG_GRP_LF2000_CSC)	+= csc-drv.o
csc-drv-objs					:= csc_drv.o

################################################################################
# Module build:
# 	Link Nexell prototype function
################################################################################

ifeq ($(CONFIG_ARCH_NXP3200), y)
CPU	:= nxp3200
endif

LINUX_TOP := ../../../../
PROTOTYPE := $(LINUX_TOP)arch/arm/mach-$(CPU)/prototype/module

ifeq ($(CONFIG_GRP_LF2000_CSC), m)
csc-drv-objs					+= $(PROTOTYPE)/nx_csc.o
endif