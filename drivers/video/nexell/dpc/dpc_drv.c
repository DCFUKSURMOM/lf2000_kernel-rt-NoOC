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
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>
#include <mach/dpc.h>

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "dpc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*------------------------------------------------------------------------------
 * 	DPC ops
 */
static int nx_dpc_ops_open(struct inode *inode, struct file *flip)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_dpc_ops_release(struct inode *inode, struct file *flip)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static bool nx_dpc_ops_ioc_dpc(unsigned int cmd, unsigned long arg)
{
	bool ret = true;
	DBGOUT("%c, ioctl cmd:%x, nr:%d ...\n", _IOC_TYPE(cmd), cmd, _IOC_NR(cmd));

	switch(cmd)
	{
	case IOCTL_DPC_GET_MODE_INFO:
		{
			uint buf[5];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: DpcMode	: 0=primary main, 1=secondary main, 2=clone, 3=multi monitor
			 * 	buf[1]	: MainWidth
			 * 	buf[2]	: MainHeight
			 * 	buf[3]	: SubWidth
			 * 	buf[4]	: SubHeight
			 */
			buf[0] = soc_dpc_get_mode();
			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = 2;
			#endif

			soc_dpc_get_resolution(0, &buf[1], &buf[2]);
			soc_dpc_get_resolution(1, &buf[3], &buf[4]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

static bool nx_dpc_ops_ioc_vid(unsigned int cmd, unsigned long arg)
{
	bool ret = true;
	DBGOUT("%c, ioctl cmd:%x, nr:%d ...\n", _IOC_TYPE(cmd), cmd, _IOC_NR(cmd));

	switch(cmd)
	{
	case IOCTL_DPC_VID_GET_AUTHOR:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
			return false;
			/*
			 * 	buf[0]	: Module : 0=primary, 1=secondary
			 * 	buf[1]	: ID	 : not use
			 */
			buf[1] = 1;

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_FREE_AUTHOR:
	case IOCTL_DPC_VID_RESET_AUTHOR:
		break;

	case IOCTL_DPC_VID_SET_FORMAT:
		{
			uint buf[5];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Id		: not use
			 * 	buf[2]	: FourCC	: FOURCC_MVS0, FOURCC_MVS2... (refer to dpc.h)
			 * 	buf[3]	: SrcW
			 * 	buf[4]	: SrcH
			 */
			if (0 > soc_dpc_set_vid_format(buf[0], buf[2], buf[3], buf[4]))
				ret = false;

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			if (0 > soc_dpc_set_vid_format(buf[0], buf[2], buf[3], buf[4]))
				ret = false;
			#endif
		}
		break;

	case IOCTL_DPC_VID_GET_FORMAT:
		{
			uint buf[5];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Id		: not use
			 * 	buf[2]	: fourcc	: FOURCC_MVS0, FOURCC_MVS2... (refer to dpc.h)
			 * 	buf[3]	: SrcW
			 * 	buf[4]	: SrcH
			 */
			buf[1] = 1;

			soc_dpc_get_vid_format(buf[0], &buf[2], &buf[3], &buf[4]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_ADDRESS:
		{
			uint buf[9];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Id		: not use
			 *  buf[2]	: LuAddr
			 *  buf[3]	: LuStride
			 *  buf[4]	: CbAddr
			 *  buf[5]	: cbStride
			 *  buf[6]	: CrAddr
			 *  buf[7]	: CrStride
			 *  buf[8]	: waitVSync
			 */
			soc_dpc_set_vid_address(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_vid_address(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);
			#endif
		}
		break;

	case IOCTL_DPC_VID_SET_POSITION:
		{
			uint buf[7];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Id		: not use
			 *  buf[2]	: Left
			 *  buf[3]	: Top
			 *  buf[4]	: Right
			 *  buf[5]	: Bottom
			 *  buf[6]	: waitVSync
			 */
			if (0 >soc_dpc_set_vid_position(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6]))
				return false;

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			if (0 >soc_dpc_set_vid_position(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6]))
				return false;
			#endif
		}
		break;

	case IOCTL_DPC_VID_SET_ENABLE:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Id		: not use
			 */
			soc_dpc_set_vid_enable(buf[0], 1);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_vid_enable(buf[0], 1);
			#endif
		}
		break;

	case IOCTL_DPC_VID_SET_DISABLE:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Id		: not use
			 */
			soc_dpc_set_vid_enable(buf[0], 0);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_vid_enable(buf[0], 0);
			#endif
		}
		break;

	case IOCTL_DPC_VID_SET_PRIORITY:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Priority	: 0,1,2,3
			 */
			soc_dpc_set_vid_priority(buf[0], buf[1]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_vid_priority(buf[0], buf[1]);
			#endif
		}
		break;

	case IOCTL_DPC_VID_GET_PRIORITY:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Priority	: 0,1,2,3
			 */
			buf[1] = soc_dpc_get_vid_priority(buf[0]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_COLORKEY:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: ColorKey	: R8,G8,B8
			* 	buf[2]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_colorkey(buf[0], buf[1], buf[2]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_vid_colorkey(buf[0], buf[1], buf[2]);
			#endif
		}
		break;

	case IOCTL_DPC_VID_GET_COLORKEY:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: ColorKey	: R8,G8,B8
			 */
			buf[1] = soc_dpc_get_vid_colorkey(buf[0]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_ALPHABLEND:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Alpha		: depth 15 ~ 0
			 * 	buf[2]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_color(buf[0], VID_COL_ALPHA, buf[1], buf[2]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_vid_color(buf[0], VID_COL_ALPHA, buf[1], buf[2]);
			#endif
		}
		break;

	case IOCTL_DPC_VID_GET_ALPHABLEND:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Alpha		: depth 15 ~ 0
			 */
			buf[1] = soc_dpc_get_vid_color(buf[0], VID_COL_ALPHA);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_BRIGHTNESS:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Bright	: -128 <= Range <= 128
			 * 	buf[2]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_color(buf[0], VID_COL_BRIGHT, buf[1], buf[2]);
		}
		break;

	case IOCTL_DPC_VID_GET_BRIGHTNESS:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Bright	: -128 <= Range <= 128
			 */
			buf[1] = soc_dpc_get_vid_color(buf[0], VID_COL_BRIGHT);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_HUE:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Hue		: 0 <= Range <= 360
			 * 	buf[2]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_color(buf[0], VID_COL_HUE, buf[1], buf[2]);
		}
		break;

	case IOCTL_DPC_VID_GET_HUE:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Hue		: 0 <= Range <= 360
			 */
			buf[1] = soc_dpc_get_vid_color(buf[0], VID_COL_HUE);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_CONTRAST:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Contrast	: 0 <= Range <= 8
			 * 	buf[2]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_color(buf[0], VID_COL_CONTRAST, buf[1], buf[2]);
		}
		break;

	case IOCTL_DPC_VID_GET_CONTRAST:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Contrast	: 0 <= Range <= 8
			 */
			buf[1] = soc_dpc_get_vid_color(buf[0], VID_COL_CONTRAST);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_SATURATION:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Satura	: -100 <= Range <= 100
			 * 	buf[2]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_color(buf[0], VID_COL_SATURATION, buf[1], buf[2]);
		}
		break;

	case IOCTL_DPC_VID_GET_SATURATION:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Satura	: -100 <= Range <= 100
			 */
			buf[1] = soc_dpc_get_vid_color(buf[0], VID_COL_SATURATION);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_GAMMA:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Gamma		: not support
			 * 	buf[2]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_color(buf[0], VID_COL_GAMMA, buf[1], buf[2]);
		}
		break;

	case IOCTL_DPC_VID_GET_GAMMA:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Gamma		: not support
			 */
			buf[1] = soc_dpc_get_vid_color(buf[0], VID_COL_GAMMA);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_HFILTER:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_hfilter(buf[0], buf[1]);
		}
		break;

	case IOCTL_DPC_VID_GET_HFILTER:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Enable	: 0=disable, 1=enable
			 */
			buf[1] = soc_dpc_get_vid_hfilter(buf[0]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_VID_SET_VFILTER:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_vid_vfilter(buf[0], buf[1]);
		}
		break;

	case IOCTL_DPC_VID_GET_VFILTER:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Enable	: 0=disable, 1=enable
			 */
			buf[1] = soc_dpc_get_vid_vfilter(buf[0]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

static bool nx_dpc_ops_ioc_rgb(unsigned int cmd, unsigned long arg)
{
	bool ret = true;
	DBGOUT("%c, ioctl cmd:%x, nr:%d ...\n", _IOC_TYPE(cmd), cmd, _IOC_NR(cmd));

	switch(cmd)
	{
	case IOCTL_DPC_RGB_SET_FORMAT:
		{
			uint buf[6];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: Format	: NX_MLC_RGBFMT (refer to nx_mlc.h)
			 * 	buf[3]	: SrcW
			 * 	buf[4]	: SrcH
			 * 	buf[5]	: PixelByte
			 */
			if (0 > soc_dpc_set_rgb_format(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]))
				ret = false;

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			if (0 > soc_dpc_set_rgb_format(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]))
				ret = false;
			#endif
		}
		break;

	case IOCTL_DPC_RGB_GET_FORMAT:
			{
			uint buf[6];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: Format	: NX_MLC_RGBFMT (refer to nx_mlc.h)
			 * 	buf[3]	: SrcW
			 * 	buf[4]	: SrcH
			 * 	buf[5]	: PixelByte
			 */
			soc_dpc_get_rgb_format(buf[0], buf[1], &buf[2], &buf[3], &buf[4], &buf[5]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_RGB_SET_POSITION:
		{
			uint buf[5];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: StartX
			 * 	buf[3]	: StartY
			 * 	buf[4]	: WaitVSync
			 */
			if (0 >soc_dpc_set_rgb_position(buf[0], buf[1], buf[2], buf[3], buf[4]))
				return false;

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			if (0 >soc_dpc_set_rgb_position(buf[0], buf[1], buf[2], buf[3], buf[4]))
				return false;
			#endif
		}
		break;

	case IOCTL_DPC_RGB_SET_ADDRESS:
		{
			uint buf[6];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: Address
			 * 	buf[3]	: HStride
			 * 	buf[4]	: VStride
			 * 	buf[5]	: WaitVSync
			 */
			soc_dpc_set_rgb_address(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_rgb_address(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
			#endif
		}
		break;

	case IOCTL_DPC_RGB_SET_ENABLE:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 */
			soc_dpc_set_rgb_enable(buf[0], buf[1], 1);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_rgb_enable(buf[0], buf[1], 1);
			#endif
		}
		break;

	case IOCTL_DPC_RGB_SET_DISABLE:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 */
			soc_dpc_set_rgb_enable(buf[0], buf[1], 0);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_rgb_enable(buf[0], buf[1], 0);
			#endif
		}
		break;

	case IOCTL_DPC_RGB_SET_ALPHABLEND:
		{
			uint buf[4];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: Alpha		: depth 15 ~ 0
			 * 	buf[3]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_rgb_color(buf[0], buf[1], RGB_COL_ALPHA, buf[2], buf[3]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_rgb_color(buf[0], buf[1], RGB_COL_ALPHA, buf[2], buf[3]);
			#endif
		}
		break;

	case IOCTL_DPC_RGB_GET_ALPHABLEND:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: Alpha		: depth 15 ~ 0
			 */
			buf[2] = soc_dpc_get_rgb_color(buf[0], buf[1], RGB_COL_ALPHA);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_RGB_SET_TRANSCOLOR:
		{
			uint buf[4];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: TransColor: R8,G8,B8
			 * 	buf[3]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_rgb_color(buf[0], buf[1], RGB_COL_TRANSP, buf[2], buf[3]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_rgb_color(buf[0], buf[1], RGB_COL_ALPHA, buf[2], buf[3]);
			#endif
		}
		break;

	case IOCTL_DPC_RGB_GET_TRANSCOLOR:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: TransColor: R8,G8,B8
			 */
			buf[2] = soc_dpc_get_rgb_color(buf[0], buf[1], RGB_COL_TRANSP);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_RGB_SET_INVERTCOLOR:
		{
			uint buf[4];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: Invert	: R8,G8,B8
			 * 	buf[3]	: Enable	: 0=disable, 1=enable
			 */
			soc_dpc_set_rgb_color(buf[0], buf[1], RGB_COL_INVERT, buf[2], buf[3]);
		}
		break;

	case IOCTL_DPC_RGB_GET_INVERTCOLOR:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: Invert	: R8,G8,B8
			 */
			buf[2] = soc_dpc_get_rgb_color(buf[0], buf[1], RGB_COL_INVERT);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_RGB_SET_BACKCOLOR:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: BackColor : R8,G8,B8
			 */
			soc_dpc_set_bgcolor(buf[0], buf[1]);
		}
		break;

	case IOCTL_DPC_RGB_GET_BACKCOLOR:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: BackColor : R8,G8,B8
			 */
			buf[1] = soc_dpc_get_bgcolor(buf[0]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_RGB_SET_INVALIDZONE:
		{
			uint buf[9];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 * 	buf[2]	: num
			 * 	buf[3]	: left
			 * 	buf[4]	: top
			 * 	buf[5]	: right
			 * 	buf[6]	: bottom
			 * 	buf[7]	: Enable
			 * 	buf[8]	: WaitVSync
			 */
			if (0 >soc_dpc_set_rgb_invalidzone(
					buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]
					))
				return false;
		}
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

static bool nx_dpc_ops_ioc_etc(unsigned int cmd, unsigned long arg)
{
	bool ret = true;
	DBGOUT("%c, ioctl cmd:%x, nr:%d ...\n", _IOC_TYPE(cmd), cmd, _IOC_NR(cmd));

	switch(cmd)
	{
	/*----------------------------------------------------------------------
	 * MLC layer control
	 */
	case IOCTL_DPC_LAYER_SET_TYPE:
	case IOCTL_DPC_LAYER_GET_TYPE:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer 	: 0 ~ 3  = RGB0, RGB1, RGB2, Video
			 * 	buf[2]	: Type	 	: RGB=1, VIDEO=2, else user define
			 */
			if (0 == buf[1] || 1 == buf[1] || 2 == buf[1])
				buf[2] = 1;

			if (3 == buf[1])
				buf[2] = 2;

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_LAYER_SET_ENABLE:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 */
			soc_dpc_set_layer_enable(buf[0], buf[1], 1);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_layer_enable(buf[0], buf[1], 1);
			#endif
		}
		break;

	case IOCTL_DPC_LAYER_SET_DISABLE:
		{
			uint buf[2];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Layer		: RGB=0, 1, 2
			 */
			soc_dpc_set_layer_enable(buf[0], buf[1], 0);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_layer_enable(buf[0], buf[1], 0);
			#endif
		}
		break;

	/*------------------------------------------------------------------
	 * DPC control
	 */
	case IOCTL_DPC_DEVICE_WAIT_VSYNC:
		{
			uint buf[1];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 */
			soc_dpc_wait_vsync(buf[0]);
		}
		break;

	case IOCTL_DPC_DEVICE_SET_OUT_FORMAT:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Format	: encoder out format (refer to cfg_main.h)
			 * 	buf[2]	: Type		: encoder out type (refer to cfg_main.h)
			 */
			ret = soc_dpc_set_lcd_out(buf[0], buf[1], buf[2]);
		}
		break;

	case IOCTL_DPC_DEVICE_GET_OUT_FORMAT:
		{
			uint buf[3];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 * 	buf[1]	: Format	: encoder out format (refer to cfg_main.h)
			 * 	buf[2]	: Type		: encoder out type (refer to cfg_main.h)
			 */
			soc_dpc_get_lcd_out(buf[0], &buf[1], &buf[2]);

			if (copy_to_user((void __user *)arg, (const void *)buf, sizeof(buf)))
				return false;
		}
		break;

	case IOCTL_DPC_DEVICE_OUT_ENABLE:
		{
			uint buf[1];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 */
			soc_dpc_set_out_enable(buf[0]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_out_enable(buf[0]);
			#endif
		}
		break;

	case IOCTL_DPC_DEVICE_OUT_DISABLE:
		{
			uint buf[1];
			if (copy_from_user(buf, (const void __user *)arg, sizeof(buf)))
				return false;
			/*
			 * 	buf[0]	: Module	: 0=primary, 1=secondary
			 */
			soc_dpc_set_out_disable(buf[0]);

			#ifdef CONFIG_FB_NEXELL_CLONE
			buf[0] = !buf[0];
			soc_dpc_set_out_disable(buf[0]);
			#endif
		}
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

static long nx_dpc_ops_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
{
	bool ret = true;
	DBGOUT("IoCtl (cmd:0x%x, type:%c, nr:%d) \n\n", cmd, _IOC_TYPE(cmd), _IOC_NR(cmd));

	switch(_IOC_TYPE(cmd))
	{
	case IOC_D_MAGIC:
		ret = nx_dpc_ops_ioc_dpc(cmd, arg);
		break;

	case IOC_V_MAGIC:
		ret = nx_dpc_ops_ioc_vid(cmd, arg);
		break;

	case IOC_R_MAGIC:
		ret = nx_dpc_ops_ioc_rgb(cmd, arg);
		break;

	case IOC_E_MAGIC:
		ret = nx_dpc_ops_ioc_etc(cmd, arg);
		break;

	default:
		ret = false;
		break;
	}

	DBGOUT("IoCtl (cmd:0x%x, type:%c, nr:%d, ret:%s) \n\n",
		cmd, _IOC_TYPE(cmd), _IOC_NR(cmd), ret?"done":"fail");
	return ret;
}

struct file_operations nx_dpc_ops = {
	.owner 			= THIS_MODULE,
	.open 			= nx_dpc_ops_open,
	.release		= nx_dpc_ops_release,
	.unlocked_ioctl	= nx_dpc_ops_ioctl,
};

/*--------------------------------------------------------------------------------
 * DPC platform_driver functions
 ---------------------------------------------------------------------------------*/
struct class *dpc_class;

static int nx_dpc_drv_probe(struct platform_device *pdev)
{
	int ret = register_chrdev(DPC_DEV_MAJOR, "DPC (Display Controller)", &nx_dpc_ops);
	if (0 > ret) {
		printk(KERN_ERR "fail, register device (%s, major:%d)\n",
			DPC_DEV_NAME, DPC_DEV_MAJOR);
		return ret;
	}

//	dpc_class = class_create(THIS_MODULE, "dpc");
//	device_create(dpc_class, NULL, MKDEV(DPC_DEV_MAJOR, 0), NULL, "dpc");

	printk(KERN_INFO "%s: register major:%d\n", pdev->name, DPC_DEV_MAJOR);
	return ret;
}

static int nx_dpc_drv_remove(struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);

	device_destroy(dpc_class, MKDEV(DPC_DEV_MAJOR, 0));
	class_destroy(dpc_class);

	unregister_chrdev(DPC_DEV_MAJOR, "DPC (Display Controller)");
	return 0;
}

static int nx_dpc_drv_suspend(struct platform_device *dev, pm_message_t state)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_dpc_drv_resume(struct platform_device *dev)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

static struct platform_driver dpc_plat_driver = {
	.probe		= nx_dpc_drv_probe,
	.remove		= nx_dpc_drv_remove,
	.suspend	= nx_dpc_drv_suspend,
	.resume		= nx_dpc_drv_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= DPC_DEV_NAME,
	},
};

static int __init nx_dpc_drv_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_register(&dpc_plat_driver);
}

static void __exit nx_dpc_drv_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&dpc_plat_driver);
}

module_init(nx_dpc_drv_init);
module_exit(nx_dpc_drv_exit);

MODULE_AUTHOR("jhkim <jhkin@nexell.co.kr>");
MODULE_DESCRIPTION("DPC (Display Controller) driver for the Nexell");
MODULE_LICENSE("GPL");

