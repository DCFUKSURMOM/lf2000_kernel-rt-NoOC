
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/io.h>
#include <linux/crc32.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <asm/system_info.h>

#include <mach/irqs.h>
#include <mach/iomap.h>
#include <mach/devices.h>
#include <mach/nxp3200.h>
#include <mach/board_revisions.h>

/*------------------------------------------------------------------------------
 * NOR Information
 */
#define	LF2000_NOR_FLASH_BASE_LOW	0x00000000
#define	LF2000_NOR_FLASH_BASE_HIGH0	0x34000000
#define	LF2000_NOR_FLASH_BASE_HIGH1	0x04000000

#define	LF2000_NOR_FLASH_SIZE			( 512 * 1024 )
#define	LF2000_NOR_MFGDATA_SIZE		( 4096 )
#define	LF2000_NOR_BOOT_SIZE			( LF2000_NOR_FLASH_SIZE - 2*LF2000_NOR_MFGDATA_SIZE )

static struct mtd_partition flash_parts[] = {
	{
		.name           = "NOR_Boot",
		.offset         = 0x00000000,
		.size           = LF2000_NOR_BOOT_SIZE,
	},
	{
		.name           = "MfgData0",
		.offset         = LF2000_NOR_BOOT_SIZE,
		.size           = LF2000_NOR_MFGDATA_SIZE,
	},
	{
		.name           = "MfgData1",
		.offset         = LF2000_NOR_BOOT_SIZE + LF2000_NOR_MFGDATA_SIZE,
		.size           = LF2000_NOR_MFGDATA_SIZE,
	},
};

static struct physmap_flash_data flash_data = {
	.width		= 1,		/* 8-bit */
	.nr_parts		= ARRAY_SIZE(flash_parts),
	.parts		= flash_parts,
};

static struct resource nor_flash_resources[] = {
	{
		.start		= LF2000_NOR_FLASH_BASE_LOW,
		.end			= LF2000_NOR_FLASH_BASE_LOW + LF2000_NOR_FLASH_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= LF2000_NOR_FLASH_BASE_HIGH0,
		.end			= LF2000_NOR_FLASH_BASE_HIGH0 + LF2000_NOR_FLASH_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	}
};

static struct platform_device nor_flash_device = {
	.name		= "physmap-flash",
	.id		= 1,
	.resource	= nor_flash_resources,
	.num_resources	= 1,
	.dev		= {
		.platform_data = &flash_data,
	}
};

static struct platform_device nor_flash_device2 = {
	.name		= "physmap-flash",
	.id		= 2,
	.resource	= &(nor_flash_resources[1]),
	.num_resources	= 1,
	.dev		= {
		.platform_data = &flash_data,
	}
};

#if defined(CONFIG_MTD_M25P80)

static struct flash_platform_data nxp3200_spi_slave_data = {
	.type		= "pm25ld040",
	.nr_parts	= ARRAY_SIZE(flash_parts),
	.parts		= flash_parts,
};

static struct spi_board_info spi_flash_board_info[] __initdata = {
	{
		.modalias	= "m25p80",
		.platform_data	= &nxp3200_spi_slave_data,
		.max_speed_hz	= 25000000,
		.irq		= IRQ_PHY_SSPSPI0,
		.bus_num	= 0,
		.chip_select	= 0,
	},
};

#endif

static int __init lf2000_flash_init(void)
{
	if (system_rev == LF2000_BOARD_VTK) {
		printk(KERN_CRIT "Skip looking for NOR\n");
	} else {
		printk(KERN_CRIT "Initializing NOR\n");
		platform_device_register(&nor_flash_device);
		platform_device_register(&nor_flash_device2);
	}

#if defined(CONFIG_MTD_M25P80)
	spi_register_board_info(spi_flash_board_info, ARRAY_SIZE(spi_flash_board_info));
	printk("plat: register pflash\n");
#endif

	return 0;
}

device_initcall(lf2000_flash_init);

