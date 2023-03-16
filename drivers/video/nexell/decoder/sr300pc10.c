/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
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
#include <linux/delay.h>
#include <linux/ioctl.h>
#include <linux/i2c.h>

/* get configure value */
#include <generated/autoconf.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/vip.h>
#include "../vip/vip_drv.h"

#define	DBG_OUT		0

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "dec: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#include "sr300pc10_dat.h"

#define	SR300PC10_ADDRESS		(0x40)
#define	SR300PC10_DEVICE_ID		(0x93)

/*------------------------------------------------------------------------------
 * i2c func
 */
static struct i2c_client  * i2c_cli   = NULL;

#if (DBG_OUT)
static int i2c_read(unsigned char addr)
{
	struct i2c_client *c = i2c_cli;
	unsigned char buff[1];
	int rc;

	buff[0] = addr;
	if (1 != (rc = i2c_master_send(c, buff, 1))) {
		printk(KERN_ERR "sr300pc10: i2c i/o error: rc = %d (should be 1)\n", rc);
		return -1;
	}

	if (1 != (rc = i2c_master_recv(c, buff, 1))) {
		printk(KERN_ERR "sr300pc10: i2c i/o error: rc = %d (should be 1)\n", rc);
		return -1;
	}
	return (buff[0]);
}
#else
#define	i2c_read	(0)
#endif

static int i2c_write(unsigned char addr, unsigned char value)
{
	struct i2c_client *c = i2c_cli;
	unsigned char buff[2];
	int rc;

	buff[0] = addr;
	buff[1] = value;

//	DBGOUT("sr300pc10: writing 0x%02x 0x%02x\n", buff[0], buff[1]);
	if (2 != (rc = i2c_master_send(c, buff, 2))) {
		printk(KERN_ERR "sr300pc10: i2c i/o error: rc = %d (should be 2)\n", rc);
		return -1;
	}
	return 0;
}

/*------------------------------------------------------------------------------
 * sr300pc10 decoder ops
 */
struct video_resol {
	int		interlace;
	int		width;
	int		height;
	int		fps;
	void  * data;
	int	 	data_len;
};

#define	SR300PC10_1024x768_LEN	(sizeof(sr300pc100_1024x768)/sizeof(sr300pc100_1024x768[0]))
#define	SR300PC10_2048x1536_LEN	(sizeof(sr300pc100_2048x1536)/sizeof(sr300pc100_2048x1536[0]))

static struct video_resol _dec_res[] = {
	{ 0, 1024,  768, 10, &sr300pc100_1024x768 , SR300PC10_1024x768_LEN, },
	{ 0, 2048, 1536,  1, &sr300pc100_2048x1536, SR300PC10_2048x1536_LEN, },
};

#define	NUM_OF_RESOL_TYPE	(sizeof(_dec_res)/sizeof(_dec_res[0]))
static int curr_type = 0;

static int dec_init_device(int type)
{
	int i, len;
	char *data = NULL;
	char reg, val;

	DBGOUT("%s (type=%d)\n", __func__, type);

	if (type >= NUM_OF_RESOL_TYPE)
		return -1;

	data = (char *)_dec_res[type].data;
	len  = _dec_res[type].data_len;

	for (i = 0; len > i; i++) {
		reg = *data++;
		val = *data++;
		i2c_write(reg, val);
	}
	curr_type = type;
	return 0;
}

static int dec_get_type_nr(void)
{
	DBGOUT("%s (nr=%d)\n", __func__, NUM_OF_RESOL_TYPE);
	return NUM_OF_RESOL_TYPE;
}

static int dec_get_type(void)
{
	DBGOUT("%s (curr=%d)\n", __func__, curr_type);
	return curr_type;
}

static int dec_set_type(int type)
{
	DBGOUT("%s (type=%d)\n", __func__, type);
	return dec_init_device(type);
}

static int dec_det_type(int h_count, int v_count)
{
	DBGOUT("%s (curr=%d, hc=%4d, vc=%d)\n", __func__, curr_type, h_count, v_count);
	return curr_type;
}

static int dec_get_prop(int type, struct vip_prop *prop)
{
	DBGOUT("%s\n", __func__);
	if (type >= NUM_OF_RESOL_TYPE)
		return -1;

	prop->interlace  = _dec_res[type].interlace;
	prop->video_type = type;
	prop->h_active   = _dec_res[type].width;
	prop->v_active   = _dec_res[type].height;
	prop->fps    	 = _dec_res[type].fps;

	return 0;
}

static int dec_set_resol(int width, int height, int fps)
{
	int i = 0, type = 0;
	int ret = -1;

	DBGOUT("%s\(w=%4d, h=%4d)n", __func__, width, height);

	for (; NUM_OF_RESOL_TYPE > i; i++) {
		if ( _dec_res[i].width  == width &&
			 _dec_res[i].height == height &&
			 _dec_res[i].fps    == fps ) {
			type = i, ret = 0;
			break;
		}
	}

	if (0 > ret)
		return ret;

	return dec_init_device(type);
}

static void dec_get_sync(struct vip_sync * sync)
{
	int width = 0, height = 0, fps = 0;
	int type  = curr_type;

	switch (type) {
	case 0:
		width  = _dec_res[type].width;
		height = _dec_res[type].height;
		fps    = _dec_res[type].fps;

		sync->video_type	=    0;		/* this video type num */

		sync->interlace 	=    0;
		sync->external_sync =    0;
		sync->h_active     	= width; 	/* active width */
		sync->h_frontporch 	=    7; 	/* horizontal blank : front porch */
		sync->h_syncwidth  	=    1; 	/* horizontal blank : sync width */
		sync->h_backporch  	=    1; 	/* horizontal blank : back porch */
		sync->v_active     	= height; 	/* active height */
		sync->v_frontporch 	=    1; 	/* vertical blank : front porch (4) */
		sync->v_syncwidth  	=    1; 	/* vertical blank : sync width */
		sync->v_backporch  	=    1; 	/* vertical blank : back porch */
		sync->framepersec  	=   fps;
		break;
	case 1:
		width  = _dec_res[type].width;
		height = _dec_res[type].height;
		fps    = _dec_res[type].fps;

		sync->video_type	=    1;		/* this video type num */

		sync->interlace 	=    0;
		sync->external_sync =    0;
		sync->h_active     	= width; 	/* active width */
		sync->h_frontporch 	=    7; 	/* horizontal blank : front porch */
		sync->h_syncwidth  	=    1; 	/* horizontal blank : sync width */
		sync->h_backporch  	=    1; 	/* horizontal blank : back porch */
		sync->v_active     	= height; 	/* active height */
		sync->v_frontporch 	=    1; 	/* vertical blank : front porch (4) */
		sync->v_syncwidth  	=    1; 	/* vertical blank : sync width */
		sync->v_backporch  	=    1; 	/* vertical blank : back porch */
		sync->framepersec  	=   fps;
		break;
	default:
		break;
	}

	sync->clock_invert = 1;
	DBGOUT("%s (curr=%d, w=%4d, h=%4d)\n", __func__, curr_type, width, height);
}

static int dec_suspend(void)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

static int dec_resume(void)
{
	PM_DBGOUT("%s\n", __func__);
	return dec_init_device(curr_type);
}

/* decoder operations */
struct vip_dec_ops sr300pc10_ops = {
	.suspend		= dec_suspend,
	.resume			= dec_resume,
	.get_type_nr	= dec_get_type_nr,
	.get_type		= dec_get_type,
	.set_type		= dec_set_type,
	.det_type		= dec_det_type,
	.get_prop		= dec_get_prop,
	.set_resol		= dec_set_resol,
	.get_sync		= dec_get_sync,
};

/*------------------------------------------------------------------------------
 * sr300pc10 init
 */
static int __init sr300pc10_dev_init(void)
{
	struct i2c_board_info info;
	struct i2c_adapter  * adap;

	int 	port = CONFIG_SR300PC10_I2C_PORT;
	u_short addr = (SR300PC10_ADDRESS>>1);
	int 	ret  = 0;

	adap = i2c_get_adapter(port);
	if (! adap) {
		printk(KERN_ERR "%s: fail, can't get i2c adapter %d\n",
			__func__, port);
		return -1;
	}
	/* must be clear */
	memset(&info, 0, sizeof(info));

	info.addr = addr;
	strlcpy(info.type, "sr300pc10", I2C_NAME_SIZE);

	i2c_cli = i2c_new_device(adap, &info);
	i2c_put_adapter(adap);
	if (! i2c_cli) {
		printk(KERN_ERR "%s: fail, can't add i2c device at 0x%x\n",
			__func__, (unsigned int)info.addr);
		return -1;
	}

	dec_init_device(curr_type);	/* default VGA, 15 FPS */

	/* register to vip */
	ret = register_vip_decoder(&sr300pc10_ops, 0, 0);
	ret = register_vip_decoder(&sr300pc10_ops, 0, 1);
	printk(KERN_INFO "%s: sr300pc10 connect to vip=%d, i2c=%d\n",
		ret ? "FAIL" : "DONE", 0, port);

	return 0;
}

static void __exit sr300pc10_dev_exit(void)
{
	DBGOUT("%s\n", __func__);

	/* unregister from vip */
	unregister_vip_decoder(&sr300pc10_ops, 0, 0);
	unregister_vip_decoder(&sr300pc10_ops, 0, 1);

	/* release i2c adapter */
	if (i2c_cli)
		i2c_unregister_device(i2c_cli);

	i2c_cli = NULL;
}

#ifdef MODULE
module_init(sr300pc10_dev_init);
module_exit(sr300pc10_dev_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sr300pc10 for nexell video decoder");
#else
/* after i2c and vip driver */
late_initcall(sr300pc10_dev_init);
module_exit(sr300pc10_dev_exit);
#endif