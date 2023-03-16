#
# linux/drivers/nand/nexell/Makefile
#

obj-$(CONFIG_MTD_NAND_NEXELL)		+= nx-nand.o
nx-nand-objs				:= nand.o 
obj-$(CONFIG_MTD_NAND_LFCART)		+= lfcart.o
obj-$(CONFIG_MTD_NAND_NEXELL_HWECC)	+= nand_hw_ecc.o nand_ecc.o

################################################################################

ifeq ($(CONFIG_ARCH_NXP2120), y)
CPU	:= nxp2120
endif

PROTODIR := ../../../../arch/arm/mach-$(CPU)/prototype/module

ifeq ($(CONFIG_MTD_NAND_NEXELL),m)
nx-nand-objs		+= $(PROTODIR)/nx_mcus.o
endif
