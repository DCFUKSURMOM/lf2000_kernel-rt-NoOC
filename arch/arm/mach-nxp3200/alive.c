/*******************************************************************************
 *
 * Daniel Lazzari Jr <dlazzari@leapfrog.com>
 * Copyright 2011 LeapFrog, Inc.
 *
 */

#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/export.h>
#include <linux/module.h>

#include <mach/nxp3200.h>

#include "prototype/module/nx_alive.h"

static struct NX_ALIVE_RegisterSet* regSet = NULL;

static void lf2000_alive_write_enable(bool enable)
{
        regSet->ALIVEPWRGATEREG = enable;
}

static void lf2000_alive_set_scratch(U32 value)
{
	lf2000_alive_write_enable(true);
	regSet->ALIVESCRATCHREG.ALIVESCRATCHSETREG = value;
	regSet->ALIVESCRATCHREG.ALIVESCRATCHRSTREG = ~value;
	lf2000_alive_write_enable(false);
}

/*********************************************
 * Sysfs
 */

static ssize_t lf2000_scratch_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG);
}

static ssize_t lf2000_scratch_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned long value;
	ssize_t status;
	
	status = strict_strtoul(buf, 0, &value);
	if(status == 0)
		lf2000_alive_set_scratch(value);
	return status ? status : size;
}

static DEVICE_ATTR(scratch, 0644,
		lf2000_scratch_show, lf2000_scratch_store);
#if 1	/* 15dec11 */

/* from emerald-boot's gpio.h */
/*
 * Scratchpad Register usage
 */
 
#define SCRATCH_POWER_POS          0
#define SCRATCH_POWER_SIZE         2

#define SCRATCH_SHUTDOWN_POS       2
#define SCRATCH_SHUTDOWN_SIZE      1

#define SCRATCH_REQUEST_POS 	   3
#define SCRATCH_REQUEST_SIZE  	   3

#define SCRATCH_BOOT_IMAGE_POS     6
#define SCRATCH_BOOT_IMAGE_SIZE    2

#define SCRATCH_BOARD_ID_POS       8
#define SCRATCH_BOARD_ID_SIZE      5

#define SCRATCH_CART_ID_POS       13
#define SCRATCH_CART_ID_SIZE       4

#define SCRATCH_BOOT_SOURCE_POS   17
#define SCRATCH_BOOT_SOURCE_SIZE   3

#define SCRATCH_PANIC_POS	  20
#define SCRATCH_PANIC_SIZE         2

#define SCRATCH_REBOOT_POS 22
#define SCRATCH_REBOOT_SIZE 1

#define SCRATCH_USER_0_POS        23
#define SCRATCH_USER_0_SIZE       (32-SCRATCH_USER_0_POS)

/*
 * SCRATCHPAD Enums
 * Note that scratchpad register is cleared to zero on first power up
 */

/* Track power state */
enum scratch_power {                 // set by bootstrap, read by others
        SCRATCH_POWER_FIRSTBOOT = 0, // only seen in bootstrap
        SCRATCH_POWER_COLDBOOT  = 1, // batteries replaced
        SCRATCH_POWER_WARMBOOT  = 2, // second and subsequent boots
};

/* Shutdown semaphore.  Set by Linux shutdown to signal clean system shutdown */
enum scratch_shutdown {
        SCRATCH_SHUTDOWN_CLEAN = 0,  // set by Linux at clean shutdown
        SCRATCH_SHUTDOWN_DIRTY = 1,  // cleared by bootstrap at boot
};

/* Choose boot partition.  Used by REQUEST_BOOT and ACTUAL_BOOT bits to
 * indicate the preferred boot partition and the partition booted
 */
enum scratch_boot_image {
        SCRATCH_BOOT_IMAGE_RECOVERY = 0,   // boot recovery image
        SCRATCH_BOOT_IMAGE_PLAY     = 1,   // normal boot
        SCRATCH_BOOT_IMAGE_2        = 2,   // unused
        SCRATCH_BOOT_IMAGE_3        = 3,   // unused
};

/* Choose boot partition.  Used by REQUEST_BOOT and ACTUAL_BOOT bits to
 * indicate the preferred boot partition and the partition booted
 */
enum scratch_boot_source {
        SCRATCH_BOOT_SOURCE_UNKNOWN = 0,   // 
        SCRATCH_BOOT_SOURCE_NOR     = 1,   // 
        SCRATCH_BOOT_SOURCE_NAND    = 2,   // 
        SCRATCH_BOOT_SOURCE_UART    = 3,   // 
        SCRATCH_BOOT_SOURCE_USB     = 4,   // 
};

/* save boot source. */
enum scratch_request {
	SCRATCH_REQUEST_PLAY    = 0,  // Launch Play if possible
	SCRATCH_REQUEST_RETURN  = 1,  // Return to Play if possible
	SCRATCH_REQUEST_UPDATE  = 2,  // Enter recovery in update mode
	SCRATCH_REQUEST_BATTERY = 3,  // Enter play: battery failed
	SCRATCH_REQUEST_UNCLEAN = 4,  // Enter play: dirty shutdown
	SCRATCH_REQUEST_FAILED  = 5,  // Enter recovery in update mode: boot failed
	SCRATCH_REQUEST_SHORT   = 6,  // Enter play in short-circuit mode
	SCRATCH_REQUEST_TRAPDOOR= 7,  // Enter recovery in trapdoor mode
};

/* save state if it was a reboot */
enum scratch_reboot {
	SCRATCH_REBOOT_NO = 0,
	SCRATCH_REBOOT_YES = 1,
};

/* from e-b's common.h */
#define BIT_MASK_ONES(b) ((1<<(b))-1)

void alive_set_panic(unsigned value)
{
	U32  cur_scratch_reg;
	cur_scratch_reg  = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_PANIC_SIZE) 
					<< SCRATCH_PANIC_POS);
	cur_scratch_reg |= (value & BIT_MASK_ONES(SCRATCH_PANIC_SIZE))
				<< SCRATCH_PANIC_POS;
	lf2000_alive_set_scratch(cur_scratch_reg);
}

unsigned alive_get_panic(void)
{
	return (regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
				  >> SCRATCH_PANIC_POS)
			    & BIT_MASK_ONES(SCRATCH_PANIC_SIZE);
}

static unsigned int lf2000_alive_get_scratch(void) 
{
	// U32 originalValue;
	return NX_ALIVE_GetScratchReg();
}

void lf2000_set_scratch_reboot(void)
{
	unsigned int scratchpad;
	U32 value;
	
	scratchpad = lf2000_alive_get_scratch();
	
	value = scratchpad | (1 << SCRATCH_REBOOT_POS);
	
	lf2000_alive_set_scratch(value); 
}
EXPORT_SYMBOL(lf2000_set_scratch_reboot);

#if 0	/* pm's original implementation */
static ssize_t lf2000_boot_image_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 
			(regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_BOOT_IMAGE_POS)
			 & BIT_MASK_ONES(SCRATCH_BOOT_IMAGE_SIZE));
}

static ssize_t lf2000_boot_image_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	U32  cur_scratch_reg;
	unsigned long value;
	ssize_t status;
	
	cur_scratch_reg = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	status = strict_strtoul(buf, 0, &value);
	if(status == 0) {
		cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_BOOT_IMAGE_SIZE) 
					<< SCRATCH_BOOT_IMAGE_POS);
		cur_scratch_reg |= ((value & BIT_MASK_ONES(SCRATCH_BOOT_IMAGE_SIZE))
			<< SCRATCH_BOOT_IMAGE_POS);
		lf2000_alive_set_scratch(cur_scratch_reg);
	}
	return status ? status : size;
}

static DEVICE_ATTR(boot_image, 0644,
		lf2000_boot_image_show, lf2000_boot_image_store);
/* -------------------------------------------------------------------------- */

static ssize_t lf2000_boot_source_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 
			(regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_BOOT_SOURCE_POS)
			 & BIT_MASK_ONES(SCRATCH_BOOT_SOURCE_SIZE));
}

static ssize_t lf2000_boot_source_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	U32  cur_scratch_reg;
	unsigned long value;
	ssize_t status;
	
	cur_scratch_reg = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	status = strict_strtoul(buf, 0, &value);
	if(status == 0) {
		cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_BOOT_SOURCE_SIZE) 
					<< SCRATCH_BOOT_SOURCE_POS);
		cur_scratch_reg |= ((value & BIT_MASK_ONES(SCRATCH_BOOT_SOURCE_SIZE))
			<< SCRATCH_BOOT_SOURCE_POS);
		lf2000_alive_set_scratch(cur_scratch_reg);
	}
	return status ? status : size;
}

static DEVICE_ATTR(boot_source, 0644,
		lf2000_boot_source_show, lf2000_boot_source_store);
/* -------------------------------------------------------------------------- */

static ssize_t lf2000_panic_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 
			(regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_PANIC_POS)
			 & BIT_MASK_ONES(SCRATCH_PANIC_SIZE));
}

static ssize_t lf2000_panic_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	U32  cur_scratch_reg;
	unsigned long value;
	ssize_t status;
	
	cur_scratch_reg = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	status = strict_strtoul(buf, 0, &value);
	if(status == 0) {
		cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_PANIC_SIZE) 
					<< SCRATCH_PANIC_POS);
		cur_scratch_reg |= ((value & BIT_MASK_ONES(SCRATCH_PANIC_SIZE))
			<< SCRATCH_PANIC_POS);
		lf2000_alive_set_scratch(cur_scratch_reg);
	}
	return status ? status : size;
}

static DEVICE_ATTR(panic, 0644,
		lf2000_panic_show, lf2000_panic_store);
/* -------------------------------------------------------------------------- */

static ssize_t lf2000_request_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 
			(regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_REQUEST_POS)
			 & BIT_MASK_ONES(SCRATCH_REQUEST_SIZE));
}

static ssize_t lf2000_request_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	U32  cur_scratch_reg;
	unsigned long value;
	ssize_t status;
	
	cur_scratch_reg = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	status = strict_strtoul(buf, 0, &value);
	if(status == 0) {
		cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_REQUEST_SIZE) 
					<< SCRATCH_REQUEST_POS);
		cur_scratch_reg |= ((value & BIT_MASK_ONES(SCRATCH_REQUEST_SIZE))
			<< SCRATCH_REQUEST_POS);
		lf2000_alive_set_scratch(cur_scratch_reg);
	}
	return status ? status : size;
}

static DEVICE_ATTR(request, 0644,
		lf2000_request_show, lf2000_request_store);
/* -------------------------------------------------------------------------- */

static ssize_t lf2000_shutdown_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 
			(regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_SHUTDOWN_POS)
			 & BIT_MASK_ONES(SCRATCH_SHUTDOWN_SIZE));
}

static ssize_t lf2000_shutdown_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	U32  cur_scratch_reg;
	unsigned long value;
	ssize_t status;
	
	status = strict_strtoul(buf, 0, &value);
	if(status == 0) {
		cur_scratch_reg = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
		cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_SHUTDOWN_SIZE) 
					<< SCRATCH_SHUTDOWN_POS);
		cur_scratch_reg |= ((value & BIT_MASK_ONES(SCRATCH_SHUTDOWN_SIZE))
			<< SCRATCH_SHUTDOWN_POS);
		lf2000_alive_set_scratch(cur_scratch_reg);
	}
	return status ? status : size;
}

static DEVICE_ATTR(shutdown, 0644,
		lf2000_shutdown_show, lf2000_shutdown_store);
/* -------------------------------------------------------------------------- */

static ssize_t lf2000_user_0_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", 
			(regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_SHUTDOWN_POS)
			 & BIT_MASK_ONES(SCRATCH_SHUTDOWN_SIZE));
}

static ssize_t lf2000_user_0_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	U32  cur_scratch_reg;
	unsigned long value;
	ssize_t status;
	
	status = strict_strtoul(buf, 0, &value);
	if(status == 0) {
		cur_scratch_reg = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
		cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_USER_0_SIZE) 
					<< SCRATCH_USER_0_POS);
		cur_scratch_reg |= ((value & BIT_MASK_ONES(SCRATCH_USER_0_SIZE))
			<< SCRATCH_USER_0_POS);
		lf2000_alive_set_scratch(cur_scratch_reg);
	}
	return status ? status : size;
}

static DEVICE_ATTR(user_0, 0644,
		lf2000_user_0_show, lf2000_user_0_store);

#else	/* implementations copied from */
	/* madrid.TRUNK/linux-dist/linux/arch/arm/mach-lf1000/gpio_main.c */
	/* and then modified for accessing the alive-scratch register     */

 
static ssize_t show_scratchpad(struct device *dev, 
		struct device_attribute *attr, char *buf)
{
	return sprintf( buf, "%08X\n", 
			regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG);
}

static ssize_t set_scratchpad(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	unsigned int x;

	if(sscanf(buf, "%x", &x) != 1)
		return -EINVAL;
	lf2000_alive_set_scratch((unsigned long)x);
	return count;
}
static DEVICE_ATTR(scratchpad, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
		   show_scratchpad, set_scratchpad);


static ssize_t show_boot_power(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	U32 power;

	power = (regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_POWER_POS)
		    & BIT_MASK_ONES(SCRATCH_POWER_SIZE);
	return sprintf(buf, "%s\n",
			(power == SCRATCH_POWER_WARMBOOT) ? "WARM" : 
			(power == SCRATCH_POWER_COLDBOOT) ? "COLD" : 
		        "FIRST");
}
static DEVICE_ATTR(boot_power, S_IRUSR|S_IRGRP|S_IROTH, show_boot_power, NULL);

static ssize_t show_shutdown(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	U32 shutdown;

	shutdown = (regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_SHUTDOWN_POS)
		       & BIT_MASK_ONES(SCRATCH_SHUTDOWN_SIZE);
	return sprintf(buf, "%s\n", (shutdown == SCRATCH_SHUTDOWN_DIRTY) ?
				"DIRTY" : "CLEAN");
}

static ssize_t set_shutdown(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	U32  cur_scratch_reg;

	cur_scratch_reg  = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_SHUTDOWN_SIZE) 
					<< SCRATCH_SHUTDOWN_POS);
	if (!strcasecmp(buf, "DIRTY\n"))
		cur_scratch_reg |= SCRATCH_SHUTDOWN_DIRTY
					<< SCRATCH_SHUTDOWN_POS;
	else if (!strcasecmp(buf, "CLEAN\n"))
		cur_scratch_reg |= SCRATCH_SHUTDOWN_CLEAN
					<< SCRATCH_SHUTDOWN_POS;
	else
		return -EINVAL;	// invalid string
	
	lf2000_alive_set_scratch(cur_scratch_reg);
	return count;		// read all chars
}
static DEVICE_ATTR(shutdown, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH,
			show_shutdown, set_shutdown);

static ssize_t show_request(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	U32 request;

	request = (regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  	>> SCRATCH_REQUEST_POS)
		      & BIT_MASK_ONES(SCRATCH_REQUEST_SIZE);
	switch(request) {
	case (int)SCRATCH_REQUEST_PLAY:       return(sprintf(buf, "PLAY\n"));
	case (int)SCRATCH_REQUEST_RETURN:     return(sprintf(buf, "RETURN\n"));
	case (int)SCRATCH_REQUEST_UPDATE:     return(sprintf(buf, "UPDATE\n"));
	case (int)SCRATCH_REQUEST_BATTERY:    return(sprintf(buf, "BATTERY\n"));
	case (int)SCRATCH_REQUEST_UNCLEAN:    return(sprintf(buf, "UNCLEAN\n"));
	case (int)SCRATCH_REQUEST_FAILED:     return(sprintf(buf, "FAILED\n"));
	case (int)SCRATCH_REQUEST_SHORT:      return(sprintf(buf, "SHORT\n"));
	case (int)SCRATCH_REQUEST_TRAPDOOR:   return(sprintf(buf, "TRAPDOOR\n"));
	}
	return(sprintf(buf, "UNKNOWN\n"));  // unexpected
}

static ssize_t set_request(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	U32  cur_scratch_reg;
	U32  request;

	if (!strcasecmp(buf, "PLAY\n"))
		request = SCRATCH_REQUEST_PLAY;
	else if (!strcasecmp(buf, "RETURN\n"))
		request = SCRATCH_REQUEST_RETURN;
	else if (!strcasecmp(buf, "UPDATE\n"))
		request = SCRATCH_REQUEST_UPDATE;
	else if (!strcasecmp(buf, "BATTERY\n"))
		request = SCRATCH_REQUEST_BATTERY;
	else if (!strcasecmp(buf, "UNCLEAN\n"))
		request = SCRATCH_REQUEST_UNCLEAN;
	else if (!strcasecmp(buf, "FAILED\n"))
		request = SCRATCH_REQUEST_FAILED;
	else if (!strcasecmp(buf, "SHORT\n"))
		request = SCRATCH_REQUEST_SHORT;
	else if (!strcasecmp(buf, "TRAPDOOR\n"))
		request = SCRATCH_REQUEST_TRAPDOOR;
	else
		return -EINVAL;	// invalid string

	cur_scratch_reg  = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_REQUEST_SIZE) 
					<< SCRATCH_REQUEST_POS);
	cur_scratch_reg |= request << SCRATCH_REQUEST_POS;
	lf2000_alive_set_scratch(cur_scratch_reg);

	return count;		// read all chars
}
static DEVICE_ATTR(request, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH,
			show_request, set_request);

static ssize_t show_boot_image(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	U32 boot_image;

	boot_image = (regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
				  >> SCRATCH_BOOT_IMAGE_POS)
			 & BIT_MASK_ONES(SCRATCH_BOOT_IMAGE_SIZE);
	switch(boot_image) {
	case SCRATCH_BOOT_IMAGE_RECOVERY: return(sprintf(buf, "RECOVERY\n"));
	case SCRATCH_BOOT_IMAGE_PLAY:     return(sprintf(buf, "PLAY\n"));
	case SCRATCH_BOOT_IMAGE_2:        return(sprintf(buf, "IMAGE_2\n"));
	case SCRATCH_BOOT_IMAGE_3:        return(sprintf(buf, "IMAGE_3\n"));
	}
	return(sprintf(buf, "UNKNOWN\n"));  // unexpected
}

static ssize_t set_boot_image(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	U32  cur_scratch_reg;
	U32  boot_image;

	if (!strcasecmp(buf, "RECOVERY\n"))
		boot_image = SCRATCH_BOOT_IMAGE_RECOVERY;
	else if (!strcasecmp(buf, "PLAY\n"))
		boot_image = SCRATCH_BOOT_IMAGE_PLAY;
	else if (!strcasecmp(buf, "IMAGE_2\n"))
		boot_image = SCRATCH_BOOT_IMAGE_2;
	else if (!strcasecmp(buf, "IMAGE_3\n"))
		boot_image = SCRATCH_BOOT_IMAGE_3;
	else
		return -EINVAL;	// invalid string

	cur_scratch_reg  = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_BOOT_IMAGE_SIZE) 
					<< SCRATCH_BOOT_IMAGE_POS);
	cur_scratch_reg |= boot_image << SCRATCH_BOOT_IMAGE_POS;
	lf2000_alive_set_scratch(cur_scratch_reg);

	return (count);		// read all chars
}
static DEVICE_ATTR(boot_image, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH,
			show_boot_image, set_boot_image);

static ssize_t show_boot_source(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	U32 boot_source;

	boot_source = (regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  	>> SCRATCH_BOOT_SOURCE_POS)
			  & BIT_MASK_ONES(SCRATCH_BOOT_SOURCE_SIZE);
	switch(boot_source) {
	case SCRATCH_BOOT_SOURCE_NOR:  return(sprintf(buf, "NOR\n"));
	case SCRATCH_BOOT_SOURCE_NAND: return(sprintf(buf, "NAND\n"));
	case SCRATCH_BOOT_SOURCE_UART: return(sprintf(buf, "UART\n"));
	case SCRATCH_BOOT_SOURCE_USB:  return(sprintf(buf, "USB\n"));
	case SCRATCH_BOOT_SOURCE_UNKNOWN:
		return(sprintf(buf, "UNKNOWN\n"));
	}
	return(sprintf(buf, "UNKNOWN\n"));  // unexpected
}

static DEVICE_ATTR(boot_source, S_IRUSR|S_IRGRP|S_IROTH, show_boot_source, NULL);

static ssize_t show_panic(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	return sprintf (buf, "%d\n", alive_get_panic());
}

static ssize_t set_panic(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int value;

	if(sscanf(buf, "%d", &value) != 1)
		return -EINVAL;

	alive_set_panic(value);

	return count;		// read all chars
}
static DEVICE_ATTR(panic, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH,
			show_panic, set_panic);

static ssize_t show_user_0(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	u32 val = (regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG
			  >> SCRATCH_SHUTDOWN_POS)
		      & BIT_MASK_ONES(SCRATCH_SHUTDOWN_SIZE);
	return(sprintf(buf, "%X\n", val));
}

static ssize_t set_user_0(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	U32  cur_scratch_reg;
	unsigned int x;

	if(sscanf(buf, "%X", &x) != 1)
		return -EINVAL;

	cur_scratch_reg  = regSet->ALIVESCRATCHREG.ALIVESCRATCHREADREG;
	cur_scratch_reg &= ~(BIT_MASK_ONES(SCRATCH_USER_0_SIZE) 
					<< SCRATCH_USER_0_POS);
	cur_scratch_reg |= ((x & BIT_MASK_ONES(SCRATCH_USER_0_SIZE))
				<< SCRATCH_USER_0_POS);
	lf2000_alive_set_scratch(cur_scratch_reg);

	return (count);		// read all chars
}
static DEVICE_ATTR(user_0, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH,
			show_user_0, set_user_0);


	
#endif	/* of of stuff copied from madrid.TRUNK */
#endif
	
static struct attribute *alive_attributes[] = {
	&dev_attr_scratch.attr,
#if 0	/* 15dec11 pm's original implementation*/
	&dev_attr_boot_image.attr,
	&dev_attr_boot_source.attr,
	&dev_attr_panic.attr,
	&dev_attr_boot_power.attr,
	&dev_attr_request.attr,
	&dev_attr_shutdown.attr,
	&dev_attr_user_0.attr,
#else
	&dev_attr_scratchpad.attr,
	&dev_attr_shutdown.attr,
	&dev_attr_request.attr,
	&dev_attr_boot_image.attr,
	&dev_attr_boot_source.attr,
	&dev_attr_boot_power.attr,
	&dev_attr_panic.attr,
	&dev_attr_user_0.attr,
#endif	/* 15dec11 */
	NULL,
};

static struct attribute_group alive_attr_group = {
	.attrs = alive_attributes
};

/*********************************************
 * Init/Cleanup
 */

static int lf2000_alive_probe(struct platform_device* pdev)
{
        regSet = (struct NX_ALIVE_RegisterSet *) ioremap((phys_addr_t)PHY_BASEADDR_ALIVE, 0x200);
        
	return sysfs_create_group(&pdev->dev.kobj, &alive_attr_group);
}

static int lf2000_alive_remove(struct platform_device* pdev)
{
        iounmap((volatile void *)regSet);
        
	sysfs_remove_group(&pdev->dev.kobj, &alive_attr_group);
	
	return 0;
}

static struct platform_driver lf2000_alive_driver = {
        .probe  = lf2000_alive_probe,
        .remove = lf2000_alive_remove,
        .driver = {
                .name   = "lf2000-alive",
                .owner  = THIS_MODULE,
        },
};

static int __init lf2000_alive_init(void)
{
	return platform_driver_probe(&lf2000_alive_driver, lf2000_alive_probe);
}

static void __exit lf2000_alive_exit(void)
{
	platform_driver_unregister(&lf2000_alive_driver);
}

module_init(lf2000_alive_init);
module_exit(lf2000_alive_exit);

MODULE_AUTHOR("Daniel Lazzari Jr");
MODULE_DESCRIPTION("Alive Register driver for LF2000");
MODULE_LICENSE("GPL");

