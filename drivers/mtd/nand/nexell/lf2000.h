/*
 *  drivers/mtd/nand/lf2000.h
 *
 * Copyright 2011 LeapFrog Enterprises Inc.
 *
 * Leapfrog Firmware <firmware@leapfrog.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef LF2000_H
#define LF2000_H

#include <linux/semaphore.h>
#include <mach/soc.h>

/* 16apr12  Enable #define DEBUG_UNCORR_ERRORS to get output that might */
/*	    help us diagnose the unjustified uncorrectable errors.      */
/* *
/
#define DEBUG_UNCORR_ERRORS 
/ 
* */


// TODO: FIXME:	Decide if we ought to add a new option to Kconfig
/* Disable this #define to disable nand driver profiling */
/* */
#define CONFIG_MTD_NAND_LF2000_PROF	1
/* */

#ifdef CONFIG_MTD_NAND_LF2000_PROF
#include "prof.h"
void update_bitflips(int incr);
void update_bitflips_per_page(int errors_per_page, int block);
void update_retries_per_page(int retries);
#define UPDATE_BITFLIPS(err) update_bitflips(err)
#define UPDATE_BITFLIPS_PER_PAGE(errors_per_page, block) \
	update_bitflips_per_page(errors_per_page, block)
#define UPDATE_RETRIES_PER_PAGE(r) update_retries_per_page(r)
#else
#define UPDATE_BITFLIPS(err) do { } while (0)
#define UPDATE_BITFLIPS_PER_PAGE(errors_per_page, block) do { } while (0)
#define UPDATE_RETRIES_PER_PAGE(r) do { } while (0)
#endif	/* CONFIG_MTD_NAND_LF2000_PROF */



#define LF2000_BASE_NAND_BANK	0
#define LF2000_CART_NAND_BANK	1

#define LF2000_BCH_MAX_T	24


	/* lf2000_nand_data is derived from plat_nand_data, in nand/plat_nand.d */
	/* NOTE: we follow the convention that mtd.priv points to chip, and
	 *	 chip.priv points to the enclosing struct lf2000_nand_data.
	 */
struct lf2000_nand_data {
	struct nand_chip	chip;
	struct mtd_info		mtd;
	int					nand_bank;
	void __iomem		*io_base;
	const struct tag_bch_info * bch_info;
	int			nr_parts;
	struct mtd_partition	*parts;
	struct nand_ecclayout   oob_layout;
};


struct lf2000_nand_devices {
	void __iomem           		 * mem;	/* NAND controller IO memory */
	struct lf2000_nand_data      * onboard_nand;
	struct lf2000_nand_data      * cart_nand;
	struct platform_device		 * pdev;

	struct nand_hw_control		   controller;
	int                  		   cart_ready;
	int                 		   cart_ubi;
	struct semaphore    		   sem_hotswap;

	int	  * L;			/* [bch_max_t * 2 + 3] */
	int	  * u_L;		/* [bch_max_t*2 + 3] */
	int	  * reg;		/* [bch_max_t + 3] */
	int	  * s;			/* [bch_max_t * 2 + 1] */
	int	  * elp;		/* [bch_max_t*2 + 4][bch_max_t*2 + 4] */
	int	  * desc;		/* [bch_max_t*2 + 4] */
	struct dma_trans * tr;
};

extern struct lf2000_nand_devices lf2000_nand;

#define L_ENTRIES	(2*LF2000_BCH_MAX_T + 3)
#define uL_ENTRIES	(2*LF2000_BCH_MAX_T + 3)
#define reg_ENTRIES	(LF2000_BCH_MAX_T + 3)
#define s_ENTRIES	(2*LF2000_BCH_MAX_T + 1)
#define elp_ENTRIES	((2*LF2000_BCH_MAX_T + 4)*(2*LF2000_BCH_MAX_T + 4))
#define desc_ENTRIES	(2*LF2000_BCH_MAX_T + 4)
#define COMMON_BUFFER_SIZE \
		 sizeof(int)*(L_ENTRIES + uL_ENTRIES + reg_ENTRIES \
			      + s_ENTRIES + elp_ENTRIES + desc_ENTRIES)

struct tag_bch_arrays {
	int	bch_var_m;		/* log2(order of the galois field) */
	int	bch_var_n;		/* (2**bch_var_m) - 1 */
	short * bch_AlphaToTable;
	short * bch_IndexOfTable;
	int   * L;			/* [bch_var_t * 2 + 3] */
	int   * u_L;			/* [bch_var_t * 2 + 3] */
	int   * reg;			/* [bch_var_t + 3] */
	int   * s;			/* [bch_var_t * 2 + 1] */
	int   * elp;			/* [bch_var_t*2 + 4][bch_var_t*2 + 4] */
	int   * desc;			/* [bch_var_t*2 + 4] */
};


struct tag_bch_info {
	int bch_var_t;		/* max number of correctable bit errors */
	int bch_var_k;		/* number of bits in which 'bch_var_t' errors 
				 * can be corrected */
	int bch_var_m;		/* log2(order of the galois field) -- smallest
				 * X such that 2**X > 'bch_var_k' */
	int num_ecc_bytes;	/* number of bch ecc bytes needed to correct 
				 * 'bch_var_t' bit errors in 'bch_var_k' bits */
	const u8 * pEccX;	/* address of array of 'num_ecc_bytes' bytes
				 * that are XOR'd with the ECC values 
				 * calc'd by the nand controller; the results
				 * of the XOR are stored in NAND's OOB.  After
				 * reading ECC bytes from NAND's OOB, they are
				 * XOR'd with these values before loading them
				 * into the nand controller's ECC registers for
				 * decoding.
				 */
	struct tag_bch_arrays * p_arrays;
};

/* Prototypes and externs added 3oct13 to eliminate some compiler warnings */
int  nand_hw_ecc_check(struct mtd_info *mtd);
void lf2000_onfi(struct mtd_info *mtd, struct nand_chip *chip);
int  nand_block_markbad_first_and_last(struct mtd_info *mtd, loff_t ofs); 
void NX_NAND_OutputNandControllerRegs(void);
void NX_NAND_StartEccDecoding(int ecccmode, int num_bytes);
void NX_NAND_StartEccEncoding(int ecccmode, int num_bytes);

extern u32 report_ecc_errors;
extern u32 nor_write_addr_threshold;


#endif /* LF2000_H*/


