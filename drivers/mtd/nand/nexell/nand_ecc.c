/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#if (0)
#include <common.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <linux/mtd/nand.h>
#include <nand.h>
#include <platform.h>
#else
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mtd/nand.h>
#include <mach/platform.h>
#include <asm/io.h>
#endif

#if defined (CONFIG_MTD_NAND_NEXELL_HWECC) || defined (CONFIG_SYS_NAND_HW_ECC)

#include "nand_ecc.h"
#include "lf2000.h"


/* NOTE: enable the following #define to use XOR'd ECC bytes.
 *	 When it processes an all-FF packet of bytes, the LF2000's 
 * nand controller's ECC calculator generates a set of ECC bytes that are 
 * not all 0xFF.  Therefore when reading an erased page in which a few bits
 * are not '1', the controller's ECC verifier cannot identify the flipped
 * bits, so the firmware cannot correct them.
 *
 * To avoid this situation, we store in NAND modified ECC bytes.  They are
 * modified by XORing them with the ECC bytes that are generated for a set
 * of all-FF bytes.  When we read from NAND, we read the modified ECC bytes,
 * XOR them again (obtaining the originally generated ECC bytes), and loading
 * the resulting bytes into the LF2000's ECC registers.
 *
 */

static struct tag_bch_arrays bch_13_arrays = {
	13,		/* bch_var_m */
	8191,		/* bch_var_n */
	NULL,		/* bch_AlphaToTable */
	NULL		/* bch_IndexOfTable */
};

static struct tag_bch_arrays bch_14_arrays = {
	14,		/* bch_var_m */
	16383,		/* bch_var_n */
	NULL,		/* bch_AlphaToTable */
	NULL		/* bch_IndexOfTable */
};

#define NUM_4BIT_ECC_BYTES	7
#define NUM_8BIT_ECC_BYTES	13
#define NUM_12BIT_ECC_BYTES	20
#define NUM_16BIT_ECC_BYTES	26
#define NUM_24BIT_ECC_BYTES	42

/* NOTE: we use the (u8)(0xff & ~0xYZ) construction to avoid compiler
 * 	 warnings about truncation.
 */
static const u8 ecc4_xor_factors[NUM_4BIT_ECC_BYTES] = {
	(u8)(0xff & ~0x97), (u8)(0xff & ~0x38), (u8)(0xff & ~0x79), 
	(u8)(0xff & ~0xab), (u8)(0xff & ~0x9d), (u8)(0xff & ~0x49), 
	(u8)(0xff & ~0xd0)
};
static const u8 ecc8_xor_factors[NUM_8BIT_ECC_BYTES] = {
	(u8)(0xff & ~0x37), (u8)(0xff & ~0x83), (u8)(0xff & ~0xc7), 
	(u8)(0xff & ~0xcc), (u8)(0xff & ~0x13), (u8)(0xff & ~0x48), 
	(u8)(0xff & ~0xde), (u8)(0xff & ~0xd2), (u8)(0xff & ~0x82), 
	(u8)(0xff & ~0x3d), (u8)(0xff & ~0xfc), (u8)(0xff & ~0x4b), 
	(u8)(0xff & ~0x04) 
};
static const u8 ecc12_xor_factors[NUM_12BIT_ECC_BYTES] = {
	(u8)(0xff & ~0x6f), (u8)(0xff & ~0xd7), (u8)(0xff & ~0x44), 
	(u8)(0xff & ~0x62), (u8)(0xff & ~0x24), (u8)(0xff & ~0x4c), 
	(u8)(0xff & ~0xeb), (u8)(0xff & ~0xba), (u8)(0xff & ~0xfc), 
	(u8)(0xff & ~0x4b), (u8)(0xff & ~0xdc), (u8)(0xff & ~0xe2), 
	(u8)(0xff & ~0xa5), (u8)(0xff & ~0x1b), (u8)(0xff & ~0x19), 
	(u8)(0xff & ~0x44), (u8)(0xff & ~0x95), (u8)(0xff & ~0xf5), 
	(u8)(0xff & ~0xaa), (u8)(0xff & ~0x80) 
};
static const u8 ecc16_xor_factors[NUM_16BIT_ECC_BYTES] = {
	(u8)(0xff & ~0x95), (u8)(0xff & ~0xf2), (u8)(0xff & ~0x8f), 
	(u8)(0xff & ~0x75), (u8)(0xff & ~0xaf), (u8)(0xff & ~0xee), 
	(u8)(0xff & ~0x40), (u8)(0xff & ~0xca), (u8)(0xff & ~0xda), 
	(u8)(0xff & ~0x4f), (u8)(0xff & ~0x0d), (u8)(0xff & ~0x62), 
	(u8)(0xff & ~0x5f), (u8)(0xff & ~0x0b), (u8)(0xff & ~0xef), 
	(u8)(0xff & ~0x5d), (u8)(0xff & ~0xbb), (u8)(0xff & ~0xee), 
	(u8)(0xff & ~0xa8), (u8)(0xff & ~0x9b), (u8)(0xff & ~0xf7), 
	(u8)(0xff & ~0x7a), (u8)(0xff & ~0x13), (u8)(0xff & ~0x19),
	(u8)(0xff & ~0x62), (u8)(0xff & ~0xf4)
};
static const u8 ecc24_xor_factors[NUM_24BIT_ECC_BYTES] = {
	(u8)(0xff & ~0x7b), (u8)(0xff & ~0x08), (u8)(0xff & ~0x8f), 
	(u8)(0xff & ~0x93), (u8)(0xff & ~0x21), (u8)(0xff & ~0x2f), 
	(u8)(0xff & ~0xd4), (u8)(0xff & ~0xb7), (u8)(0xff & ~0x40), 
	(u8)(0xff & ~0x56), (u8)(0xff & ~0x9f), (u8)(0xff & ~0x43), 
	(u8)(0xff & ~0xee), (u8)(0xff & ~0x9f), (u8)(0xff & ~0xd2), 
	(u8)(0xff & ~0xe0), (u8)(0xff & ~0x84), (u8)(0xff & ~0x0b), 
	(u8)(0xff & ~0x62), (u8)(0xff & ~0x58), (u8)(0xff & ~0xd8), 
	(u8)(0xff & ~0xb1), (u8)(0xff & ~0x5a), (u8)(0xff & ~0x0c), 
	(u8)(0xff & ~0x2c), (u8)(0xff & ~0x6a), (u8)(0xff & ~0x47), 
	(u8)(0xff & ~0xfc), (u8)(0xff & ~0xa5), (u8)(0xff & ~0xaf), 
	(u8)(0xff & ~0x6e), (u8)(0xff & ~0xd7), (u8)(0xff & ~0xde), 
	(u8)(0xff & ~0x73), (u8)(0xff & ~0x4a), (u8)(0xff & ~0x57), 
	(u8)(0xff & ~0xe2), (u8)(0xff & ~0x85), (u8)(0xff & ~0xad), 
	(u8)(0xff & ~0xeb), (u8)(0xff & ~0xc4), (u8)(0xff & ~0x05)
};

static const struct tag_bch_info bch_info_4_bits = {
	4, 8 * 512, 13, NUM_4BIT_ECC_BYTES, ecc4_xor_factors, &bch_13_arrays
};
static const struct tag_bch_info bch_info_8_bits = {
	8, 8 * 512, 13, NUM_8BIT_ECC_BYTES, ecc8_xor_factors, &bch_13_arrays
};
static const struct tag_bch_info bch_info_12_bits = {
	12, 8 * 512, 13, NUM_12BIT_ECC_BYTES, ecc12_xor_factors, &bch_13_arrays
};
static const struct tag_bch_info bch_info_16_bits = {
	16, 8 * 512, 13, NUM_16BIT_ECC_BYTES, ecc16_xor_factors, &bch_13_arrays
};
static const struct tag_bch_info bch_info_24_bits = {
	24, 8 * 1024, 14, NUM_24BIT_ECC_BYTES, ecc24_xor_factors, &bch_14_arrays
};

/* The struct nand_chip.priv member points to a struct tag_bch_info */


#if	(0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "ecc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 					\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",		\
			__FILE__, __FUNCTION__, __LINE__),	\
		printk(KERN_ERR msg); }


/*
 * 0x05	= Bad marker (256/512 byte pages)
 * 0x00 = Bad marker (2048/4096   byte pages)
 * 0x01 = Reserved   (2048/4096   byte pages)
 */

#define	ECC_HW_MAX_BYTES  (44) 	//  4 bit ecc ( 7byte),  8 bit ecc (13byte),
				// 16 bit ecc (26byte), 24 bit ecc (42byte)

/*------------------------------------------------------------------------------
 * u-boot nand hw ecc interface
 */
static int nand_sw_ecc_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	int i;
	struct nand_chip *chip = mtd->priv;

	for (i = 0; len > i; i++)
		if (buf[i] != readb(chip->IO_ADDR_R))
			return -EFAULT;
	return 0;
}


static void FlipErrorBits( int num_errors, int * e_pos, 
			   int eccsize, uint32_t * pdata)
{
	int i;

	for (i=0 ; i < num_errors; i++) {
#ifndef NAND_ACCESS_TIMING	// we don't want to time serial output
		if (report_ecc_errors)
			printk(KERN_INFO "  error #%d in bit %d "
					 "(byte 0x%x, mask %x)\n",
				i+1, e_pos[i], e_pos[i]/8, 
				1 << (e_pos[i] & 7));
#endif

		if (   (0   <= e_pos[i])
			&& (e_pos[i] < (8 * eccsize)))
		{
#ifndef NAND_ACCESS_TIMING	// we don't want to time serial output
			if (report_ecc_errors)
			{
				uint32_t bad_value, fixed_value;

				bad_value   = pdata[e_pos[i] / 32];
				fixed_value = bad_value ^ 1U<<(e_pos[i] % 32);
				printk(KERN_INFO "    change 0x%08x to 0x%08x\n",
						bad_value, fixed_value);
			}
#endif
			pdata[e_pos[i] / 32] ^= 1U << (e_pos[i] & 31);
		}
	}
}


#define CURRENT_ECC_MODE \
	( ((struct lf2000_nand_data *)chip->priv)->bch_info->bch_var_t)

/**
 * nand_hw_ecc_read_subpage - hardware-ecc-based sub-page read function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @data_offs:	offset of requested data within the page
 * @readlen:	data length (in bytes_
 * @bufpoi:	page buffer to store read data
 *
 * Reads enough data from the currently addressed NAND page to ensure that
 * the bytes from data_offs to data_offs + readlen -1 are in the buffer.
 * Reads data in chunks that correspond to the number of bytes protected by a
 * set of ECC bytes.  Stores those complete chunks in the page buffer.
 *
 * Returns: 
 *	0   	if it detected no error
 *	N >= 0	where N is the max number of bitflips that were corrected in
 *		any of the ECC chunks read.  In this case the routine adds the
 *		total number of corrected errors to mtd->ecc_stats.corrected.
 *
 * Unlike earlier versions of this routine, after detecting an uncorrectable
 * ECC error, the routine now continues to read the rest of the requested
 * bytes.  When it detects and uncorrectable ECC error, the routine increments
 * mtd->ecc_stats.failed.
 * NOTE: this routine's return values do not follow the convention for other
 *	 read_subpage() functions.  Therefore we must ensure that the routine
 * is called by only lf2000_nand_do_read_ops(), which expects and uses the
 * newly defined return values.
 */
static int nand_hw_ecc_read_subpage(struct mtd_info   * mtd, 
				    struct nand_chip  * chip,
				    uint32_t		data_offs, 
				    uint32_t		readlen, 
				    uint8_t	      * bufpoi)

{
	int start_step, end_step, num_steps;
	int i, n;
	int pageoffs, eccoffs;

	int eccbytes = chip->ecc.bytes;		/* # of bytes of ECC per chunk */
	int eccsize  = chip->ecc.size;		/* # of bytes per chunk */

	uint32_t  ecc_buff[ECC_HW_MAX_BYTES/4];
	uint8_t  *ecc_code = (uint8_t*)ecc_buff;
	uint32_t *eccpos   = chip->ecc.layout->eccpos;
	uint8_t  *p;

	int max_corrected = 0;
	int ret = 0;
	int err = 0;
	int total_errors = 0;
	int o_syn[LF2000_BCH_MAX_T], e_pos[LF2000_BCH_MAX_T];
	const struct tag_bch_info * bch_info;
	const u8 * pXOR;

	bch_info = ((struct lf2000_nand_data *)chip->priv)->bch_info;
	pXOR     = bch_info->pEccX;

	/* Column address wihin the page aligned to ECC size */
	start_step = data_offs / chip->ecc.size;
	end_step   = (data_offs + readlen - 1) / chip->ecc.size;
	num_steps  = end_step - start_step + 1;

	/* Data size aligned to ECC ecc.size*/
	/* Read only the relevant ECC bytes
	 * NOTE: this assumes ecc bytes are contiguous and sequential
	 */
	pageoffs = start_step * eccsize;
	p	 = bufpoi + pageoffs;		/* buffer position */
	n	 = start_step * eccbytes;	/* skip this many ecc bytes */
	eccoffs  = mtd->writesize + eccpos[n];

	DBGOUT( "%s, readlen=%6d, start=%4d, ene=%4d, num=%4d, "
		"eccoffs=%4d, pageoffs=%6d, eccpoi=%d\n",
		__func__, readlen, start_step, end_step, num_steps, 
		eccoffs, pageoffs, n);

	/* Read only the relevant ECC bytes from the oob */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, eccoffs, -1);
	ndelay(200);	/* needed for some NANDs */
	chip->read_buf(mtd, chip->oob_poi + eccpos[n], num_steps * eccbytes);

	/* read data */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, pageoffs, -1);
	ndelay(200);	/* needed for some NANDs */

	for ( ; num_steps; num_steps--, p += eccsize) {
		for (i = 0; i < eccbytes; i++, n++)
			ecc_code[i] = chip->oob_poi[eccpos[n]] ^ pXOR[i];

		/* Reset the NAND controller's ECC decoder, 
		 * write ecc data to original ecc registers,
		 * and start the controller's ECC decoder */
		NX_NAND_SetResetECC(CURRENT_ECC_MODE);
		NX_NAND_SetOriECC((uint32_t*)ecc_code, CURRENT_ECC_MODE);
		NX_NAND_StartEccDecoding(CURRENT_ECC_MODE, eccsize);

		/* read data */
		chip->read_buf(mtd, p, eccsize);

		/* wait decoding  and error correct */
		NX_NAND_WaitForDecoding();

		if (NX_NAND_GetErrorStatus()) {

#ifndef NAND_ACCESS_TIMING	// we don't want to time serial output
			printk(KERN_INFO "read_subpage: ECC error in subpage %d\n", 
					end_step - num_steps + 1);
#else
			start_ecc_timer();
#endif
			/* Try to correct the error */
			NX_NAND_GetOddSyndrome(&o_syn[0], chip);
			ret = NX_NAND_GetErrorLocation( &o_syn[0], &e_pos[0], 
							&err, chip);

			if (0 > ret) {	/* uncorrectable errors; stop reading */
#ifdef NAND_ACCESS_TIMING	// we don't want to time serial output
				stop_ecc_timer(25);	
					// TODO: FIXME: replace 25 with a number
					// of errors that's appropriate for this
					// NAND (1 + max # correctable)
#else
				if (report_ecc_errors)
					printk(KERN_INFO "Read_subpage: "
					  	"uncorrectable ecc errs (step:"
					  	"%d, pageoffs:%d, errcnt:%d)\n",
						(end_step - start_step + 1 
							  - num_steps),
						 pageoffs, err);
				NX_NAND_OutputNandControllerRegs();
#endif
				mtd->ecc_stats.failed++;
			} 
			else { /* Correctable errors; 
				* flip the bad bits not in OOB */
				if (err > max_corrected)
					max_corrected = err;

#ifndef NAND_ACCESS_TIMING	// we don't want to time serial output
				if (report_ecc_errors)
					printk(KERN_INFO 
						"Subpage_read correctable"
						" errors: nand pageoffs %d, "
						"errors %2d, bit:\n", 
						pageoffs, err);
#endif
				FlipErrorBits( err,e_pos,eccsize,(uint32_t*)p);
#ifdef NAND_ACCESS_TIMING
				stop_ecc_timer(err);
#else
				printk(KERN_INFO "\n");
#endif
				UPDATE_BITFLIPS(err);
				ret = 0;
				total_errors += err;
			}
		}	/* if error */
	}	/* for */

	DBGOUT("DONE %s, ret=%d\n", __func__, ret);

	ret = max_corrected;
	mtd->ecc_stats.corrected += total_errors;

	return ret;
}


#ifndef NAND_ACCESS_TIMING
static void PrintSubpageErrorInfo(	int eccbytes, int eccsize,
					uint8_t  *ecc_code,
					uint8_t  *p)
{
	int i;

	printk(KERN_INFO "Read these ecc bytes for the subpage:\n");

	if (eccbytes == 42) {
		printk(	"  %02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x\n",
			ecc_code[0], ecc_code[1], ecc_code[2], ecc_code[3],
			ecc_code[4], ecc_code[5], ecc_code[6], ecc_code[7],
			ecc_code[8], ecc_code[9], ecc_code[10], ecc_code[11],
			ecc_code[12], ecc_code[13], ecc_code[14], ecc_code[15]);
		printk(	"  %02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x\n",
			ecc_code[16], ecc_code[17], ecc_code[18], ecc_code[19],
			ecc_code[20], ecc_code[21], ecc_code[22], ecc_code[23],
			ecc_code[24], ecc_code[25], ecc_code[28], ecc_code[27],
			ecc_code[28], ecc_code[29], ecc_code[30], ecc_code[31]);
		printk(	"  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			ecc_code[32], ecc_code[33], ecc_code[34], ecc_code[35],
			ecc_code[36], ecc_code[37], ecc_code[38], ecc_code[39],
			ecc_code[40], ecc_code[41]);
	}
	else {
		printk(	"  %02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x\n",
			ecc_code[0], ecc_code[1], ecc_code[2], ecc_code[3],
			ecc_code[4], ecc_code[5], ecc_code[6], ecc_code[7],
			ecc_code[8], ecc_code[9], ecc_code[10], ecc_code[11],
			ecc_code[12], ecc_code[13], ecc_code[14], ecc_code[15]);
		printk(	"  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			ecc_code[16], ecc_code[17], ecc_code[18], ecc_code[19],
			ecc_code[20], ecc_code[21], ecc_code[22], ecc_code[23],
			ecc_code[24], ecc_code[25]);
	}
	printk(KERN_INFO "Read these bytes in subpage:\n");
	for (i = 0; i < eccsize; i += 16) {
		printk(	"%02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x\n",
			*(p+i),   *(p+i+1), *(p+i+2), *(p+i+3),
			*(p+i+4), *(p+i+5), *(p+i+6), *(p+i+7),
			*(p+i+8), *(p+i+9), *(p+i+10), *(p+i+11),
			*(p+i+12), *(p+i+13), *(p+i+14), *(p+i+15));
	}
	NX_NAND_OutputNandControllerRegs();
}
#endif

/**
 * nand_hw_ecc_read_page - hardware-ecc-based page read function
 * @mtd:		mtd info structure
 * @chip:		nand chip info structure
 * @buf:		page buffer to store read data
 * @page:		index of page
 *
 * Reads the contents of the currently addressed NAND page into the 
 * specified buffer.
 *
 * Returns:	
 *	0   	if it detected no error
 *	N >= 0	where N is the max number of bitflips that were corrected in
 *		any of the ECC chunks read.  In this case the routine adds the
 *		total number of corrected errors to mtd->ecc_stats.corrected.
 *
 * Unlike earlier versions of this routine, after detecting an uncorrectable
 * ECC error, the routine now continues to read the rest of the requested
 * bytes.  When it detects and uncorrectable ECC error, the routine increments
 * mtd->ecc_stats.failed.
 * NOTE: this routine's return values do not follow the convention for other
 *	 read_page() functions.  Therefore we must ensure that the routine
 * is called by only lf2000_nand_do_read_ops(), which expects and uses the
 * newly defined return values.
 */
// enable one of the following
#define TimeStepsInPageRead	0
//#define TimeStepsInPageRead	1

#if TimeStepsInPageRead
int read_current_timer(unsigned long *timer_value);
#define GET_CURTIME(x) read_current_timer(&(x))
#define SET_TIME(a,b)  (a) = (b)
#define ADD_TIME(a,b)  (a) += (b)
#else
#define GET_CURTIME(x) 
#define SET_TIME(a,b) 
#define ADD_TIME(a,b) 
#endif	// TimeStepsInPageRead



static int nand_hw_ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip,
				uint8_t *buf, int page)
{
	int i, n, ret = 0;

	int eccsteps = chip->ecc.steps;
	int eccbytes = chip->ecc.bytes;
	int eccsize  = chip->ecc.size;
static uint32_t  ecc_buff[ECC_HW_MAX_BYTES/4];
	uint8_t  *ecc_code = (uint8_t*)ecc_buff;
	uint32_t *eccpos   = chip->ecc.layout->eccpos;
	uint8_t  *p = buf;

	int max_corrected = 0;
	int err = 0;
	int errors_per_page = 0;
 static int o_syn[LF2000_BCH_MAX_T], e_pos[LF2000_BCH_MAX_T];
	int block;
#if TimeStepsInPageRead
	unsigned long t0, t1, t2, t3;
	unsigned long t_ecc, t_oob, t_xfer, t_decode, t_check, t_total;
#endif	// TimeStepsInPageRead
	const struct tag_bch_info * bch_info;
	const u8 * pXOR;

	bch_info = ((struct lf2000_nand_data *)chip->priv)->bch_info;
	pXOR     = bch_info->pEccX;

	block = page >> (chip->phys_erase_shift - chip->page_shift); 

	SET_TIME(t_ecc,   0);
	SET_TIME(t_xfer,  0);
	SET_TIME(t_decode,0);
	SET_TIME(t_oob,   0);
	SET_TIME(t_check, 0);
	GET_CURTIME(t0);

	if (512 >= mtd->writesize) {
		/* read oob */
		chip->ecc.read_oob(mtd, chip, page, 1);
		/* read data */
		chip->cmdfunc(mtd, NAND_CMD_READ0, 0x00, page);
	} else {
		/* read ECC bytes from the oob */
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, 
				mtd->writesize + eccpos[0], -1);
		ndelay(200);	/* needed for some NANDs */
		chip->read_buf(mtd, chip->oob_poi + eccpos[0], 
				eccsteps * eccbytes);

	GET_CURTIME(t_oob);
	SET_TIME(t_oob, t_oob - t0);
		/* prepare to read data */
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, 0, -1);
		ndelay(200);	/* needed for some NANDs */
	}

	for (n = 0; eccsteps; eccsteps--, p += eccsize) {
		GET_CURTIME(t1);
		{  /* NOTE: This assumes eccpos[x+1] = eccpos[x] + 1 */
			uint8_t  * dst = &ecc_code[0];
			uint8_t  * src = &chip->oob_poi[eccpos[n]];
			
			for (i = 0; i < eccbytes; i++) {
				*dst++ = *src++ ^ pXOR[i];
			}
			n += eccbytes;
		}
		/* write ecc data to orignal ecc register */
		NX_NAND_SetResetECC(CURRENT_ECC_MODE);
		NX_NAND_SetOriECC((uint32_t*)ecc_code, CURRENT_ECC_MODE);
		NX_NAND_StartEccDecoding(CURRENT_ECC_MODE, eccsize);

		GET_CURTIME(t2);
		ADD_TIME(t_ecc, t2 - t1);
		GET_CURTIME(t1);

		/* Now read a chunk of data */
		chip->read_buf(mtd, p, eccsize);

		GET_CURTIME(t2);
		ADD_TIME(t_xfer, t2 - t1);

		/* wait for the controller's decoding to finish */
		NX_NAND_WaitForDecoding();

		GET_CURTIME(t3);
		ADD_TIME(t_decode, t3 - t2);
		GET_CURTIME(t1);

		if (NX_NAND_GetErrorStatus()) 
		{

#ifndef NAND_ACCESS_TIMING	// we don't want to time serial output
			if (report_ecc_errors)
				printk(KERN_INFO "read_page: uncorrectable ECC "
					"error in subpage %d\n", 
					chip->ecc.steps - eccsteps);
#else
			start_ecc_timer();
#endif

			/* Try to count errors, find them, and fix them */
			NX_NAND_GetOddSyndrome(&o_syn[0], chip);
			ret = NX_NAND_GetErrorLocation( &o_syn[0], &e_pos[0], 
							&err, chip);

			if (0 > ret) {	/* Uncorrectable errors */
#ifdef NAND_ACCESS_TIMING	// we don't want to time serial output
				stop_ecc_timer(25);	
					// TODO: FIXME: replace 25 with a number
					// of errors that's appropriate for this
					// NAND (1 + max # correctable)
#else
				if (report_ecc_errors)
				{
					printk(KERN_INFO "Read_page: nand detect"
						" ecc errs, can't correct "
						"(step:%d, page:%d, errcnt:%d)\n",
						(chip->ecc.steps - eccsteps), 
						page, err);
					NX_NAND_OutputNandControllerRegs();
				}
#endif
				mtd->ecc_stats.failed++;
			} 
			else {	/* Correctable errors */
				if (err > max_corrected)
					max_corrected = err;

// 12apr12  Enabled the printk even when timing; then disabled again
#ifndef NAND_ACCESS_TIMING	// we don't want to time serial output
				if (report_ecc_errors)
					printk(KERN_INFO "nand page %d, %d"
						 " correctable errors\n", 
						page, err);
#endif
				FlipErrorBits( err, e_pos, eccsize, (uint32_t *)p);
#ifdef NAND_ACCESS_TIMING
				stop_ecc_timer(err);
#else
				if (report_ecc_errors)
					printk(KERN_INFO "\n");
#endif
				UPDATE_BITFLIPS(err);
				ret = 0;
				errors_per_page += err;
			}
		}	// if (NX_NAND_GetErrorStatus())
		GET_CURTIME(t3);
		ADD_TIME(t_check, t3 - t1);
	}		// for ( eccsteps )

	GET_CURTIME(t_total);
	SET_TIME(t_check, t_total - t1);
	SET_TIME(t_total, t_total - t0);


	ret = max_corrected;
	mtd->ecc_stats.corrected += errors_per_page;

#if TimeStepsInPageRead
	printk(KERN_INFO "read_pg ticks: oob %d, ecc %d, "
					 "xfer %d, decode %d, chk %d, "
					 "total %d, ret %d\n",
			(int)t_oob, (int)t_ecc, (int)t_xfer, 
			(int)t_decode, (int)t_check, (int)t_total, ret);
#endif	// TimeStepsInPageRead
	return ret;
}

// TODO: FIXME: make this more efficient; see Nexell's code or nand_base.c
//				code.
    /* returns 1 if all 'len' bytes at buf are 0xff;
     *         0 if at least one of the first 'len' bytes at buf is not 0xff
     */
static int all_bytes_ff( const uint8_t * buf, int len)
{
	int rem   = (3 & (unsigned int)buf);
	int allFF = 1;

	if (rem) {
		while ( (len > 0) && (rem < 4)) {
			if (*buf++ != 0xFF) {
				allFF = 0;
				break;
			}
			++rem;
			--len;
		}
	}
	if (allFF) {
		if (0 == (3 & (unsigned int)buf)) {
			u32 * p = (u32 *)buf;

			for ( ; len > 3; len -= 4) {
				if (*p++ != 0xFFFFFFFF) {
					allFF = 0;
					break;
				}
			}
			if (allFF) {
				for (buf = (uint8_t *)p; len > 0; --len) {
					if (*buf++ != 0xFF) {
						allFF = 0;
						break;
					}
				}
			}
		} else {
			//dev_info(&nand.pdev->dev, "!@#$ all_bytes_ff()\n");
			for ( ; len > 0; --len) {
				if (*buf++ != 0xFF) {
					allFF = 0;
					break;
				}
			}
		}
	}
	return allFF;
}


static void nand_hw_ecc_write_page(struct mtd_info *mtd, struct nand_chip *chip,
				  const uint8_t *buf)
{
	int i, n;
	int eccsteps = chip->ecc.steps;
	int eccbytes = chip->ecc.bytes;
	int eccsize  = chip->ecc.size;

static uint32_t  ecc_buff[ECC_HW_MAX_BYTES/4];
	uint8_t  *ecc_code = (uint8_t*)ecc_buff;
	uint32_t *eccpos   = chip->ecc.layout->eccpos;
	uint8_t  *p = (uint8_t *)buf;
	const struct tag_bch_info * bch_info;
	const u8 * pXOR;

	bch_info = ((struct lf2000_nand_data *)chip->priv)->bch_info;
	pXOR     = bch_info->pEccX;

	DBGOUT("%s\n", __func__);
	/* First check if the page is all FF; if it is, don't bother writing
	 * to the NAND.  Just verify that the nand page is all FF
	 */
	if ( !all_bytes_ff( buf, mtd->writesize) ) {

		/* write data and get ecc */
		for (n = 0; eccsteps; eccsteps--, p += eccsize) {

			NX_NAND_SetResetECC(CURRENT_ECC_MODE);
			NX_NAND_StartEccEncoding(CURRENT_ECC_MODE, eccsize);

			/* write page */
			chip->write_buf(mtd, p, eccsize);

			/* get ecc code from ecc register */
			NX_NAND_WaitForEncoding();
			NX_NAND_GetGenECC((uint32_t *)ecc_code, CURRENT_ECC_MODE);

			/* set oob with ecc */
			for (i = 0; eccbytes > i; i++, n++)
				chip->oob_poi[eccpos[n]] = ecc_code[i] ^ pXOR[i];
		}
		/* write oob */
		chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);
	}   /* !all_bytes_ff */
}

static int nand_hw_ecc_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
#if defined (CONFIG_MTD_NAND_VERIFY_WRITE)
	struct nand_chip *chip = mtd->priv;
	int eccsteps = chip->ecc.steps;
	int eccbytes = chip->ecc.bytes;
	int eccsize  = chip->ecc.size;

	uint32_t  ecc_buff[ECC_HW_MAX_BYTES/4];
	uint8_t  *ecc_code = (uint8_t*)ecc_buff;
	uint32_t *eccpos   = chip->ecc.layout->eccpos;
	uint8_t  *p = (uint8_t *)buf;

	int eccoffs;
	int err, ret = 0;
	int	o_syn[LF2000_BCH_MAX_T], e_pos[LF2000_BCH_MAX_T];
	int i, n;
	const struct tag_bch_info * bch_info;
	const u8 * pXOR;

	bch_info = ((struct lf2000_nand_data *)chip->priv)->bch_info;
	pXOR     = bch_info->pEccX;

	if (512 >= mtd->writesize)
		return nand_sw_ecc_verify_buf(mtd, buf, len);

	eccoffs  = mtd->writesize;
	eccsteps = chip->ecc.steps;
	p = (uint8_t *)buf;

	/* note> chip->oob_poi is saved when write_page */
#if (1)
	/* read ECC bytes from the oob */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, eccoffs + eccpos[0], -1);
	ndelay(200);	/* needed for some NANDs */
	chip->read_buf(mtd, chip->oob_poi + eccpos[0], eccsteps * eccbytes);

	/* read data */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, 0, -1);
	ndelay(200);	/* needed for some NANDs */
#endif

	for (n = 0; eccsteps; eccsteps--, p += eccsize) {

		for (i = 0; eccbytes > i; i++, n++)
			ecc_code[i] = chip->oob_poi[eccpos[n]] ^ pXOR[i];

		/* set orignal ecc register */
		NX_NAND_SetResetECC(CURRENT_ECC_MODE);
		NX_NAND_SetOriECC((uint32_t *)ecc_code, CURRENT_ECC_MODE);

		/* read data */
		chip->read_buf(mtd, p, eccsize);

		/* wait decoding  and error correct */
		NX_NAND_WaitForDecoding();

		if (NX_NAND_GetErrorStatus()) {
			NX_NAND_GetOddSyndrome(&o_syn[0], chip);
			ret = NX_NAND_GetErrorLocation(&o_syn[0], &e_pos[0], &err, chip);

			if (0   > ret) {
				ERROUT( "nand detect ecc errs, can't correct "
					"(step:%d, errcnt:%d, ret:%d)\n",
					(chip->ecc.steps - eccsteps), err, ret);
				return -EIO;
			}
			// TODO: Either get rid of this routine or add code 
			//	 here to fix the detected errors
		}
	}
	// TODO: ...and add code here to compare what was read with what was in
	//		 the buffer.  Looks as though this routine now reads from NAND
	// into the buffer, so we'll need to change that.
	// TODO: ... OR -- maybe this routine is ok as it is -- it just checks that
	// the page can be read back without uncorrectable errors.  Not a very
	// thorough check, but maybe it's good enough.
#endif /* CONFIG_MTD_NAND_VERIFY_WRITE */
	return 0;
}

int nand_hw_ecc_check(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_ecclayout *layout = chip->ecc.layout;
	struct nand_oobfree *oobfree  = chip->ecc.layout->oobfree;
	uint32_t *eccpos = chip->ecc.layout->eccpos;
	int ecctotal = chip->ecc.total;
	int i = 0, n = 0;
	int ret = 0;

	if (512 > mtd->writesize) {
		printk(KERN_INFO "NAND ecc: page size %d not support hw ecc\n",
			mtd->writesize);
		chip->ecc.mode 		= NAND_ECC_SOFT;
		chip->ecc.read_page 	= NULL;
		chip->ecc.read_subpage 	= NULL;
		chip->ecc.write_page 	= NULL;
		chip->ecc.layout	= NULL;
		chip->verify_buf	= nand_sw_ecc_verify_buf;

		if ( chip->buffers &&
			!(chip->options & NAND_OWN_BUFFERS)) {
			kfree(chip->buffers);
			chip->buffers = NULL;
		}
		ret = nand_scan_tail(mtd);
		printk(KERN_INFO "NAND ecc: Software \n");
		return ret;
	}

	/*
	 * set ecc layout
	 */
	if (16 >= mtd->oobsize) {
		for (i = 0, n = 0; ecctotal>i; i++, n++) {
			if (5 == n) n += 1;	// Bad marker
			eccpos[i] = n;
		}
		oobfree->offset  = n;
		oobfree->length  = mtd->oobsize - ecctotal - 1;
		layout->oobavail = oobfree->length;
		if (NAND_ECC_NONE == chip->ecc.mode) {
			printk( "No ECC, bad '5', ecc 0~4,6~%d (%d),"
				" free %d~%d (%d)\n",
				ecctotal+1-1, ecctotal, oobfree->offset,
				oobfree->offset+oobfree->length-1, oobfree->length);
		}
		else if (NAND_ECC_SOFT == chip->ecc.mode) {
			printk( "S/W 1-bit ECC, bad '5', ecc 0~4,6~%d (%d),"
				" free %d~%d (%d)\n",
				ecctotal+1-1, ecctotal, oobfree->offset,
				oobfree->offset+oobfree->length-1, oobfree->length);
		}
		else {
			printk( "ECC mode %d, bad '5', ecc 0~4,6~%d (%d),"
				" free %d~%d (%d)\n",
				chip->ecc.mode, ecctotal+1-1, ecctotal, 
				oobfree->offset, 
				oobfree->offset+oobfree->length-1, 
				oobfree->length);
		}
	} else {
		oobfree->offset  = 2;
		if (mtd->oobsize < ecctotal + 2) {
			printk( KERN_INFO "oobsize (%d) < ecctotal (%d) + 2\n",
				mtd->oobsize, ecctotal);
			return -1;
		}
		oobfree->length  = mtd->oobsize - ecctotal - 2;
		layout->oobavail = oobfree->length;

		n = oobfree->offset + oobfree->length;
		for (i = 0; i < ecctotal; i++, n++)
			eccpos[i] = n;

		if (NAND_ECC_NONE == chip->ecc.mode) {
			printk(KERN_INFO "No ECC, bblk '0,1', "
					 "ecc %d~%d (%d), free 2~%d (%d)\n",
				oobfree->offset+oobfree->length, n-1,
				ecctotal, oobfree->length+2-1, oobfree->length);
		}
		else if (NAND_ECC_SOFT == chip->ecc.mode) {
			printk(KERN_INFO "S/W 1-bit ECC, bblk '0,1', "
					 "ecc %d~%d (%d), free 2~%d (%d)\n",
				oobfree->offset+oobfree->length, n-1,
				ecctotal, oobfree->length+2-1, oobfree->length);
		}
		else {
			const struct tag_bch_info * bch_info;

			bch_info = ((struct lf2000_nand_data *)chip->priv)->bch_info;
			printk(KERN_INFO "H/W %d-bit ECC, bad '0,1', "
					 "ecc %d~%d (%d), free 2~%d (%d)\n",
				bch_info->bch_var_t, 
				oobfree->offset+oobfree->length, n-1,
				ecctotal, oobfree->length+2-1, oobfree->length);
		}
	}
	return ret;
}

#if 1	/* 31jan12 */

/**
 * Scan a given block partially
 * Derived from scan_block_fast() in nand_bbt.c.
 * Modified to take advantage of what we know about big mlc NANDs.
 * It returns 0 if the first byte of the spare area of the block's first and
 * last pages are equal to 0xFF.  If the first byte of the spare area of the
 * block's first page is not 0xFF, it returns 1.  If the first byte of the
 * spare area of the block's last page is not 0xFF, it returns 2.
 */
static 
int scan_micron_block_faster(struct mtd_info *mtd, struct nand_bbt_descr *bd,
			   loff_t offs, uint8_t *buf)
{
	struct mtd_oob_ops ops;
	int ret;

	//ops.ooblen = mtd->oobsize;
	ops.ooblen = 1;
	ops.oobbuf = buf;
	ops.ooboffs = 0;
	ops.datbuf = NULL;
	ops.mode = MTD_OPS_PLACE_OOB;

		/* Read the first byte of the spare area and check if it's 0x00 
		 */
	ret = mtd->_read_oob(mtd, offs, &ops);
	if (ret)
		return ret;

	return (0x00 == buf[0]) ? 1 : 0;
}


/**
 * create_micron_bbt - [GENERIC] Create a bad block table by scanning the device
 * @mtd:	MTD device structure
 * @buf:	temporary buffer
 * @bd:		descriptor for the good/bad block search pattern
 * @chip:	create the table for a specific chip, -1 read all chips.
 *		Applies only if NAND_BBT_PERCHIP option is set
 *
 * Create a bad block table by scanning the device
 * for the given good/bad block identify pattern
 */
static int create_micron_bbt(struct mtd_info *mtd, uint8_t *buf,
	struct nand_bbt_descr *bd, int chip)
{
	struct nand_chip *this = mtd->priv;
	int i, numblocks, len, scanlen;
	int startblock;
	loff_t from;
	size_t readlen;
	loff_t blocksize;

	printk(KERN_INFO "Scanning micron device for bad blocks: timer cnt %d\n",
		NX_TIMER_GetTimerCounter(0));

	len = 1;

	if (!(bd->options & NAND_BBT_SCANEMPTY)) {
		/* We need only read few bytes from the OOB area */
		scanlen = 0;
		readlen = bd->len;
	} else {
		/* Full page content should be read */
		scanlen = mtd->writesize + mtd->oobsize;
		readlen = len * mtd->writesize;
	}

		/* Note that numblocks is 2 * (real numblocks) here, see i+=2
		 * below as it makes shifting and masking less painful */
	numblocks = mtd->size >> (this->bbt_erase_shift - 1);
	startblock = 0;
	from = 0;

	blocksize  = (1 << this->bbt_erase_shift);
	for (i = startblock; i < numblocks;) {
		int ret;

		ret = scan_micron_block_faster(mtd, bd, from, buf);

		if (ret < 0)
			return ret;

		if (ret) {
			this->bbt[i >> 3] |= 0x03 << (i & 0x6);
			printk(KERN_WARNING "Bad eraseblock %d at 0x%012llx"
					    "    %s page oob: 0x%02x\n",
			       i >> 1, (unsigned long long)from,
				   ((ret > 1) ? "last" : "first"), buf[0]);
			mtd->ecc_stats.badblocks++;
		}

		i += 2;
		from += blocksize;
	}
	printk(KERN_INFO "              create_micron_bbt#2: timer cnt %d; "
		"startblock %d, numblocks %d\n", 
		NX_TIMER_GetTimerCounter(0), startblock, numblocks);
	return 0;
}


static int nand_scan_micron_bbt(struct mtd_info *mtd, 
				 struct nand_bbt_descr *bd)
{
	struct nand_chip *this = mtd->priv;
	int len, res = 0;

	len = mtd->size >> (this->bbt_erase_shift + 2);
	/* Allocate memory (2bit per block) and clear the memory bad block table */
	this->bbt = (uint8_t *)kzalloc(len, GFP_KERNEL);
	if (!this->bbt) {
		printk(KERN_ERR "nand_scan_micron_bbt: Out of memory\n");
		return -ENOMEM;
	}

	// this code is from inline nand_memory_bbt() in nand_bbt.c
	bd->options &= ~NAND_BBT_SCANEMPTY;
	res = create_micron_bbt(mtd, this->buffers->databuf, bd, -1);
	if (res) {
		printk(KERN_ERR "nand_scan_micron_bbt: "
				"Can't create the RAM-based BBT\n");
		kfree(this->bbt);
		this->bbt = NULL;
	}
	return res;
}

/* This is derived from nand_default_bbt() in nand_bbt.c.
 * It is needed in order to have the bbt creator check for 0x00 in the first 
 * byte of the spare area of the first page of an eraseblock.  
 */
extern int nand_create_badblock_pattern(struct nand_chip *this);
static int nand_micron_bbt(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;

	/* Is a flash based bad block table requested ? */
	chip->bbt_td = NULL;
	chip->bbt_md = NULL;
	if (!chip->badblock_pattern)
		nand_create_badblock_pattern(chip);

	return nand_scan_micron_bbt(mtd, chip->badblock_pattern);
}

#endif


int lf2000_nand_read(struct mtd_info *mtd, loff_t from, size_t len,
		                    size_t *retlen, uint8_t *buf);
int lf2000_nand_read_oob(struct mtd_info *mtd, loff_t from,
		            	 struct mtd_oob_ops *ops);
int nand_hw_ecc_init(struct mtd_info  *mtd, int hw_ecc_mode)
{
	struct nand_chip *chip = mtd->priv;
	const struct tag_bch_info * bch_info;
	struct lf2000_nand_data   * pnd;

	DBGOUT("%s\n", __func__);

	/*
	 * ecc.bytes:
	 *  4 bit ecc need  52 bit (6.5B) ecc code per 512 Bytes
	 *  8 bit ecc need 104 bit (13 B) ecc code per 512 Bytes
	 * 16 bit ecc need 208 bit (26 B) ecc code per 512 Bytes
	 * 24 bit ecc need 336 bit (42 B) ecc code per 1024 Bytes
	 */
	switch (hw_ecc_mode) {
	case  4: 	bch_info = &bch_info_4_bits; 	break;
	case  8: 	bch_info = &bch_info_8_bits; 	break;
	case 12: 	bch_info = &bch_info_12_bits; 	break;
	case 16: 	bch_info = &bch_info_16_bits; 	break;
	case 24: 	bch_info = &bch_info_24_bits; 	break;
	default:
		ERROUT("not support ecc %d mode !!!\n", hw_ecc_mode);
		return -1;
	}
	pnd = (struct lf2000_nand_data *)chip->priv;
	pnd->bch_info = bch_info;

	printk(KERN_INFO "NAND ecc: %d-bit Hardware\n", hw_ecc_mode);
	chip->ecc.mode 		= NAND_ECC_HW;
	chip->ecc.read_page 	= nand_hw_ecc_read_page;
	chip->ecc.read_subpage 	= nand_hw_ecc_read_subpage;
	chip->ecc.write_page 	= nand_hw_ecc_write_page;
	chip->ecc.size 		= bch_info->bch_var_k / 8;
					// ecc calc'd over this many bytes
	chip->ecc.bytes		= bch_info->num_ecc_bytes;
					// and generates this  many ecc bytes
	chip->ecc.layout	= &pnd->oob_layout;
	chip->verify_buf	= nand_hw_ecc_verify_buf;

	NX_MCUS_SetECCMode(bch_info->bch_var_t);
	NX_NAND_CreateLookupTable(chip);
		// NOTE: Even though nand_scan_tail() sometimes undoes these
		// assignments, leave them here because this routine is also 
		// called via the sysfs functions.
	mtd->_read     = lf2000_nand_read;
	mtd->_read_oob = lf2000_nand_read_oob;

	return 0;
}

/* TODO: FIXME: use of nand_block_markbad_first_and_last() for Micron NANDs
 *		is a temporary expedient. 
 * Eventually lf2000_onfi() will store the address of a different function
 * in chip->block_markbad.
 */

void lf2000_onfi(struct mtd_info *mtd, struct nand_chip *chip)
{
	struct nand_onfi_params *p = &chip->onfi_params;

printk(KERN_INFO "nand_flash_detect_onfi: mfr %s, model %s, options 0x%x\n",
		p->manufacturer, p->model, chip->options);
printk(KERN_INFO "    pgsize 0x%x, blksize 0x%x, oobsize 0x%x, chipsize 0x%llx\n",
		(unsigned int)mtd->writesize, mtd->erasesize, mtd->oobsize, 
		/*(unsigned int)*/chip->chipsize);
printk(KERN_INFO "    bits per cell %d, prgms per page %d, ecc bits %d\n",
		p->bits_per_cell, p->programs_per_page, p->ecc_bits );
	if (1 == p->programs_per_page) {
		chip->options |= NAND_NO_SUBPAGE_WRITE;
	}
	if (p->ecc_bits == 1) {
		chip->ecc.mode = NAND_ECC_SOFT;
		printk(KERN_INFO "ONFI NAND ecc: Software \n");
	}
#if 0	/* just for testing, because I have no NAND that requires 1-bit ECC */
	else if (p->ecc_bits <= 4) {
		chip->ecc.mode = NAND_ECC_SOFT;
		printk(KERN_INFO "ONFI NAND ecc: Software \n");
	}
#endif
#if 0	/* to test detection of oob too small for ecc bytes */
	else if (p->ecc_bits <= 4) {
		nand_hw_ecc_init(mtd, 16);
	}
#endif
	else if (p->ecc_bits <= 8) {
		nand_hw_ecc_init(mtd, 8);
	}
	else if (p->ecc_bits <= 16) {
		nand_hw_ecc_init(mtd, 16);
	}
	else {
		nand_hw_ecc_init(mtd, 24);
	}
	// TODO: ought to add checks to ensure there's enough oob space to hold
	//		 the ecc bytes.
	chip->block_markbad = nand_block_markbad_first_and_last;
	chip->scan_bbt = nand_micron_bbt;
}


#endif /* CONFIG_MTD_NAND_NEXELL_HWECC */

