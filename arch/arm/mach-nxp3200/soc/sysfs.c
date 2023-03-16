/*
 * (C) Copyright 2012
 * Scott Esters <sesters@leapfrog.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/hardirq.h>
#include <linux/platform_device.h>
#include <linux/device.h>

/* nexell soc headers */
#include <mach/devices.h>
#include <mach/platform.h>
#include <mach/soc.h>

#if (0)
#define DBGOUT(msg...)		{ printk("adc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*-----------------------------------------------------------------------------*/

/*
 * sysfs Interface
 */
static ssize_t show_nxp3200_regs(struct device *pdev,
				struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "[0xC002B000]: MEMBW:        0x%8.8X\n"
				    "[0xC002B004]: MEMTIMEACSL:  0x%8.8X\n"
				    "[0xC002B008]: MEMTIMEACSH:  0x%8.8X\n"
				    "[0xC002B00C]: MEMTIMECOSL:  0x%8.8X\n"
				    "[0xC002B010]: MEMTIMECOSH:  0x%8.8X\n"
				    "[0xC002B014]: MEMTIMEACC0:  0x%8.8X\n"
				    "[0xC002B018]: MEMTIMEACC1:  0x%8.8X\n"
				    "[0xC002B01C]: MEMTIMEACC2:  0x%8.8X\n"
				    "\n"
				    "[0xC002B020]: MEMTIMESACC0: 0x%8.8X\n"
				    "[0xC002B024]: MEMTIMESACC1: 0x%8.8X\n"
				    "[0xC002B028]: MEMTIMESACC2: 0x%8.8X\n"
				    "[0xC002B02C]: MEMTIMEWACC0: 0x%8.8X\n"
				    "[0xC002B030]: MEMTIMEWACC1: 0x%8.8X\n"
				    "[0xC002B034]: MEMTIMEWACC2: 0x%8.8X\n"
				    "\n"
				    "[0xC002B038]: MEMTIMECOHL:  0x%8.8X\n"
				    "[0xC002B03C]: MEMTIMECOHH:  0x%8.8X\n"
				    "[0xC002B040]: MEMTIMECAHL:  0x%8.8X\n"
				    "[0xC002B044]: MEMTIMECAHH:  0x%8.8X\n"
				    "[0xC002B048]: MEMBURSTL:    0x%8.8X\n"
				    "[0xC002B04C]: MEMBURSTH:    0x%8.8X\n"
				    "[0xC002B050]: MEMWAIT:      0x%8.8X\n",
			pRegister->MEMBW,
			pRegister->MEMTIMEACS[0],
			pRegister->MEMTIMEACS[1],
			pRegister->MEMTIMECOS[0],
			pRegister->MEMTIMECOS[1],
			pRegister->MEMTIMEACC[0],
			pRegister->MEMTIMEACC[1],
			pRegister->MEMTIMEACC[2],

			pRegister->MEMTIMESACC[0],
			pRegister->MEMTIMESACC[1],
			pRegister->MEMTIMESACC[2],
			pRegister->MEMTIMEWACC[0],
			pRegister->MEMTIMEWACC[1],
			pRegister->MEMTIMEWACC[2],

			pRegister->MEMTIMECOH[0],
			pRegister->MEMTIMECOH[1],
			pRegister->MEMTIMECAH[0],
			pRegister->MEMTIMECAH[1],
			pRegister->MEMBURST[0],
			pRegister->MEMBURST[1],
			pRegister->MEMWAIT);

				
}

static DEVICE_ATTR(regs, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_nxp3200_regs, NULL);

static ssize_t show_membw(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMBW);
}

static ssize_t set_membw(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li", &temp) != 1)
		return -EINVAL;
	pRegister->MEMBW = temp;
	return(count);
} 
	
static DEVICE_ATTR(membw, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_membw, set_membw);

static ssize_t show_memtimeacsl(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMEACS[0]);
}

static ssize_t set_memtimeacsl(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMEACS[0] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimeacsl, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimeacsl, set_memtimeacsl);

static ssize_t show_memtimeacsh(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMEACS[1]);
}

static ssize_t set_memtimeacsh(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMEACS[1] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimeacsh, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimeacsh, set_memtimeacsh);

static ssize_t show_memtimecosl(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMECOS[0]);
}

static ssize_t set_memtimecosl(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMECOS[0] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimecosl, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimecosl, set_memtimecosl);

static ssize_t show_memtimecosh(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMECOS[1]);
}

static ssize_t set_memtimecosh(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMECOS[1] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimecosh, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimecosh, set_memtimecosh);

static ssize_t show_memtimeacc0(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMEACC[0]);
}

static ssize_t set_memtimeacc0(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMEACC[0] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimeacc0, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimeacc0, set_memtimeacc0);

static ssize_t show_memtimeacc1(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMEACC[1]);
}

static ssize_t set_memtimeacc1(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMEACC[1] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimeacc1, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimeacc1, set_memtimeacc1);

static ssize_t show_memtimeacc2(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMEACC[2]);
}

static ssize_t set_memtimeacc2(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMEACC[2] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimeacc2, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimeacc2, set_memtimeacc2);

static ssize_t show_memtimesacc0(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMESACC[0]);
}

static ssize_t set_memtimesacc0(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMESACC[0] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimesacc0, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimesacc0, set_memtimesacc0);

static ssize_t show_memtimesacc1(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMESACC[1]);
}

static ssize_t set_memtimesacc1(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMESACC[1] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimesacc1, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimesacc1, set_memtimesacc1);

static ssize_t show_memtimesacc2(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMESACC[2]);
}

static ssize_t set_memtimesacc2(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMESACC[2] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimesacc2, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimesacc2, set_memtimesacc2);

static ssize_t show_memtimewacc0(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMEWACC[0]);
}

static ssize_t set_memtimewacc0(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMEWACC[0] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimewacc0, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimewacc0, set_memtimewacc0);

static ssize_t show_memtimewacc1(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMEWACC[1]);
}

static ssize_t set_memtimewacc1(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMEWACC[1] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimewacc1, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimewacc1, set_memtimewacc1);

static ssize_t show_memtimewacc2(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMEWACC[2]);
}

static ssize_t set_memtimewacc2(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMEWACC[2] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimewacc2, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimewacc2, set_memtimewacc2);

static ssize_t show_memtimecohl(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMECOH[0]);
}

static ssize_t set_memtimecohl(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMECOH[0] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimecohl, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimecohl, set_memtimecohl);

static ssize_t show_memtimecohh(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMECOH[1]);
}

static ssize_t set_memtimecohh(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMECOH[1] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimecohh, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimecohh, set_memtimecohh);

static ssize_t show_memtimecahl(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMECAH[0]);
}

static ssize_t set_memtimecahl(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMECAH[0] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimecahl, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimecahl, set_memtimecahl);

static ssize_t show_memtimecahh(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMTIMECAH[1]);
}

static ssize_t set_memtimecahh(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMTIMECAH[1] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memtimecahh, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memtimecahh, set_memtimecahh);

static ssize_t show_memburstl(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMBURST[0]);
}

static ssize_t set_memburstl(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMBURST[0] = temp;
	return(count);
} 
	
static DEVICE_ATTR(memburstl, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memburstl, set_memburstl);

static ssize_t show_membursth(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMBURST[1]);
}

static ssize_t set_membursth(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMBURST[1] = temp;
	return(count);
} 
	
static DEVICE_ATTR(membursth, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_membursth, set_membursth);

static ssize_t show_memwait(struct device *pdev,
	struct device_attribute* attr, char *buf)
{
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return sprintf(buf, "ERR\n");
	else
		return sprintf(buf, "0x%8.8X\n", pRegister->MEMWAIT);
}

static ssize_t set_memwait(struct device *pdev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	long temp;
	register struct NX_MCUS_RegisterSet *pRegister =
		(struct NX_MCUS_RegisterSet *)NX_MCUS_GetBaseAddress();

	if (pRegister == NULL)
		return -ENXIO;

	if (sscanf(buf, "%li",&temp) != 1)
		return -EINVAL;
	pRegister->MEMWAIT = temp;
	return(count);
} 
	
static DEVICE_ATTR(memwait, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
		show_memwait, set_memwait);

static struct attribute *regs_attributes[] = {
	&dev_attr_regs.attr,
	&dev_attr_membw.attr,
	&dev_attr_memtimeacsl.attr,
	&dev_attr_memtimeacsh.attr,
	&dev_attr_memtimecosl.attr,
	&dev_attr_memtimecosh.attr,
	&dev_attr_memtimeacc0.attr,
	&dev_attr_memtimeacc1.attr,
	&dev_attr_memtimeacc2.attr,
	&dev_attr_memtimesacc0.attr,
	&dev_attr_memtimesacc1.attr,
	&dev_attr_memtimesacc2.attr,
	&dev_attr_memtimewacc0.attr,
	&dev_attr_memtimewacc1.attr,
	&dev_attr_memtimewacc2.attr,
	&dev_attr_memtimecohl.attr,
	&dev_attr_memtimecohh.attr,
	&dev_attr_memtimecahl.attr,
	&dev_attr_memtimecahh.attr,
	&dev_attr_memburstl.attr,
	&dev_attr_membursth.attr,
	&dev_attr_memwait.attr,
	NULL,
};

static struct attribute_group regs_attr_group = {
	.attrs = regs_attributes,
};

static int __init lf2000_sysfs_probe(struct platform_device *pdev)
{
	int ret;

	printk(KERN_INFO "%s:%s.%d start\n", __FILE__, __func__, __LINE__);
	ret = sysfs_create_group(&pdev->dev.kobj, &regs_attr_group);
	if (ret) {
		printk(KERN_INFO "%s:%s.%d removing sysfs entry\n", __FILE__, __func__, __LINE__);
		sysfs_remove_group(&pdev->dev.kobj, &regs_attr_group);
	}
	printk(KERN_INFO "%s:%s.%d end\n", __FILE__, __func__, __LINE__);
	return ret;
}

static int lf2000_sysfs_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &regs_attr_group);
	return 0;
}

static struct platform_driver lf2000_sysfs_driver = {
	.remove = lf2000_sysfs_remove,
	.driver	= {
		.name	= SYSFS_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init lf2000_sysfs_init(void)
{
	return platform_driver_probe(&lf2000_sysfs_driver, lf2000_sysfs_probe);
}

static void __exit lf2000_sysfs_exit(void)
{
	platform_driver_unregister(&lf2000_sysfs_driver);
}

module_init(lf2000_sysfs_init);
module_exit(lf2000_sysfs_exit);

MODULE_AUTHOR("leapfrog.com");
MODULE_DESCRIPTION("LF2000 cpu sysfs entries");
MODULE_LICENSE("GPL");
