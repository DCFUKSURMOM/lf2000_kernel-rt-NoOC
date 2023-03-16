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
#ifndef __VIP_DRV_H__
#define __VIP_DRV_H__

#include <linux/kernel.h>

struct vip_sync {
	unsigned int	video_type;		/* 0 = NTSC, 1 = PAL, 2 = Others */
	int				interlace;
	int				external_sync;
	int				h_active;
	int				h_frontporch;
	int				h_syncwidth;
	int				h_backporch;
	int				v_active;
	int				v_frontporch;
	int				v_syncwidth;
	int				v_backporch;
	int				framepersec;

	unsigned int 	data_order;			/* 0(def): CBY0CRY1, 1: CRY1CBY0, 2: Y0CBY1CR, 3: Y1CRY0CB */
	int			 	clock_invert;		/* default 0: VIP clock invert */

	/* Not use NXP2120 */
    int			    ext_field_sig;  	/* default 0: FALSE (0: not use, 1: use)    */
    int				ext_field_sel;  	/* default 0: Bypass[Low odd] (1:Invert[Low even], 2: Fix 0[odd], 3: Fix 1[even]) */
    int				ext_dvalid_sig;   	/* default 0: FALSE (0: not use, 1: use)    */
    int				ext_dvalid_pol;   	/* default 0: FALSE (0: disable, 1: enable) */
    int				field_invert;   	/* default 0: FALSE, use when Interlaced mode for clipper or decimator. */

};

/* decoder operations */
struct vip_dec_ops {
	int		(*init) 		(void);
	int		(*exit)			(void);
	int		(*suspend) 		(void);
	int		(*resume)		(void);
	int		(*start)		(void);
	int		(*stop)			(void);
	int		(*get_type_nr)	(void);								/* total support video type num */
	int		(*get_type) 	(void);								/* get current video type */
	int		(*set_type) 	(int type);							/* select video type */
	int		(*det_type) 	(int h_count, int v_count);			/* detect video type */
	int		(*get_prop)		(int type, struct vip_prop *prop);	/* get video type's property */
	int		(*set_resol) 	(int width, int height, int fps);	/* select video type with resolution */
	void	(*get_sync)		(struct vip_sync *);				/* get selected type's sync parameter */
};

extern int  register_vip_decoder  (struct vip_dec_ops* dec_ops, int module, int port);
extern void unregister_vip_decoder(struct vip_dec_ops* dec_ops, int module, int port);

#endif	/* __VIP_DRV_H__ */
