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

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <asm/io.h>
#include <asm/sizes.h>
#include <asm/mach-types.h>
#include <asm/system_info.h>
#include "../../mtdcore.h"

/* GPIO headers */
#include <mach/gpio.h>
#include <linux/gpio.h>
#include <linux/lf2000/gpio.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/board_revisions.h>

/* For deciding what mtd partition to use */
#include <mach/platform_id.h>


#if	(0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "nand: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 				\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",	\
			__FILE__, __FUNCTION__, __LINE__),	\
		printk(KERN_ERR msg); }

#define CLEAR_RnB(r)						\
	r = NX_MCUS_GetInterruptPending(0);			\
	if (r) {						\
		NX_MCUS_ClearInterruptPending(0); 		\
		NX_MCUS_GetInterruptPending  (0); 		\
	}
#define CHECK_RnB()	NX_MCUS_GetInterruptPending(0);

#include <linux/sched.h>
#include <asm/stacktrace.h>
#include <asm/traps.h>
#include <asm/unwind.h>

#if defined (CONFIG_MTD_NAND_NEXELL_HWECC) || defined (CONFIG_MTD_NAND_NEXELL_HWECC_MODULE)
extern int nand_hw_ecc_init (struct mtd_info  *mtd, int hw_ecc_mode);
extern int nand_hw_ecc_check(struct mtd_info  *mtd);
#endif

extern int nand_get_device( struct nand_chip *chip, struct mtd_info *mtd, 
						    int new_state);
extern void nand_release_device(struct mtd_info *mtd);

/* Leapfrog specific stuff */
#define LF2000	1
#ifdef LF2000
#include <linux/platform_device.h>
#include <linux/delay.h>
#include "../../ubi/ubi-media.h"
#include "lf2000.h"
#include "nand_ecc.h"

#if	(0)
U32		NX_CLKPWR_GetSystemResetConfiguration( void );
#endif

#define NAND_CART_DETECT_LEVEL 0

/* Here are prototypes of MTD Interface functions defined in nand_base.c 
 * They are called from lf2000_nand_read_oob() and lf2000_nand_do_read_ops()
 */
int  nand_do_read_oob(struct mtd_info *mtd, loff_t from,
                      struct mtd_oob_ops *ops);
uint8_t *nand_transfer_oob(struct nand_chip *chip, uint8_t *oob,
         		   struct mtd_oob_ops *ops, size_t len);

/* These functions are referenced by nand_hw_ecc_init() in nand_ecc.c */
int lf2000_nand_read(struct mtd_info *mtd, loff_t from, size_t len,
		            	           size_t *retlen, uint8_t *buf);
int lf2000_nand_read_oob(struct mtd_info *mtd, loff_t from,
		         struct mtd_oob_ops *ops);

/*
 * Private device structure
 *
 *  onboard_nand points to info about the base NAND flash
 *  cart_nand    points to info about the cartridge NAND flash
 *  controller is the control structure for the LF2000's NAND controller
 */

struct lf2000_nand_devices lf2000_nand = {
	.mem          = NULL,
	.onboard_nand = NULL,
	.cart_nand    = NULL,
	.pdev	      = NULL,
	.cart_ready   = 0,
	.cart_ubi     = 0,

	.L	      = NULL,
	.u_L	      = NULL,
	.reg	      = NULL,
	.s	      = NULL,
	.elp	      = NULL,
	.desc	      = NULL
};


static int lf2000_cart_remove(void);
static int init_lf2000_cart_nand(struct nand_chip *chip);

#endif	/* LF2000 */



/*------------------------------------------------------------------------------
 * nand partition
 */
static const char *part_probes[] = { "cmdlinepart", NULL };

#if defined (CONFIG_MTD_NAND_NEXELL_PARTS_MAP)
#include <mach/cfg_nand_map.h>
static inline void def_mtd_parts(struct mtd_info *mtd, struct mtd_partition *parts)
{
}
#else
#define BOOTLD_OFFSET	(8*1024*1024)
static struct mtd_partition partition_map[] = {
	{
		.name   = "Data parts",
		.offset = BOOTLD_OFFSET,
	},
};

static inline void def_mtd_parts(struct mtd_info *mtd, struct mtd_partition *parts)
{
	parts->size = mtd->size - BOOTLD_OFFSET;
}

#endif /* CONFIG_MTD_NAND_NEXELL_PARTS_MAP */


static struct mtd_partition partition_info_cart[] = {
	{ .name		= "Cartridge",
	  .offset	= 0,
 	  .size		= MTDPART_SIZ_FULL },
};

/* ---------------------------------------------------------------------------
 * Sysfs stuff
 * --------------------------------------------------------------------------*/
// Now CONFIG_MTD_NAND_LF2000_PROF is #defined (or not) in lf2000.h
// TODO: FIXME:	Decide if we ought to add a new option to Kconfig
#ifdef CONFIG_MTD_NAND_LF2000_PROF

#define MAX_NUM_ERASE_BLOCKS    4096
static u32 block_erase_counts[MAX_NUM_ERASE_BLOCKS];
static u32 block_read_counts[MAX_NUM_ERASE_BLOCKS];
static u32 block_write_counts[MAX_NUM_ERASE_BLOCKS];
static u32 block_bitflip_counts[MAX_NUM_ERASE_BLOCKS];
static u32 total_erases;
static u32 total_read_commands;
static u32 total_page_reads;
static u32 total_subpage_reads;
static u32 total_raw_reads;
static u32 total_read_bufs;
static u32 total_writes;
static u32 total_bitflips;

static unsigned long ws_n[NS_MAX], ws_sum[NS_MAX], ws_min[NS_MAX], ws_max[NS_MAX];
static long ws_start[NS_MAX];
static int ws_any = 0;

#define MAX_ERRORS	24
#define MAX_ERRORS_PER_PAGE	(8*MAX_ERRORS)

#define MAX_RETRIES		(32)
static int max_retries_per_page = -1;
static u32 histo_of_retries_per_page[MAX_RETRIES + 2];
	/*  0 for no error, 
	 * 32 for all attempt failed w/ uncorrectable errors,
	 * 33 for invalid # of retries (< 0 or > 32) */

static int max_errors_per_page = -1;
static u32 errors_per_page_counts[MAX_ERRORS_PER_PAGE+1];

#ifdef CONFIG_MTD_NAND_LF2000_PROF

void update_bitflips(int incr) {
	total_bitflips += incr;
}

void update_bitflips_per_page(int errors_per_page, int block)
{
	if ((errors_per_page < 0) || (errors_per_page > MAX_ERRORS_PER_PAGE))
		errors_per_page = MAX_ERRORS_PER_PAGE;

	errors_per_page_counts[errors_per_page] += 1;

	if (errors_per_page > max_errors_per_page) 
		max_errors_per_page = errors_per_page;

	if (block < MAX_NUM_ERASE_BLOCKS)
		block_bitflip_counts[block] += errors_per_page;
}

void update_retries_per_page(int retries)
{
	if ((retries < 0) || (retries > MAX_RETRIES + 1))
		retries = MAX_RETRIES + 1;

	histo_of_retries_per_page[retries] += 1;

	if (retries > max_retries_per_page) 
		max_retries_per_page = retries;
}
#endif	/* CONFIG_MTD_NAND_LF2000_PROF */

// NOTE: NAND_ACCESS_TIMING and others are #defined (or not) in prof.h
#ifdef NAND_ACCESS_TIMING

U32	NX_TIMER_GetTimerCounter( U32 ModuleIndex );

static u32 ecc_start_time;
static int max_badbits = -1;

void start_ecc_timer(void) 
{
	ecc_start_time = NX_TIMER_GetTimerCounter(0);
}

static u32 ecc_times[MAX_ERRORS+2]; // +2 for none and uncorrectable
static u32 err_counts[MAX_ERRORS+2]; // +2 for none and uncorrectable
void stop_ecc_timer(int badbits) 
{
	u32 delta = NX_TIMER_GetTimerCounter(0) - ecc_start_time;

	if ((badbits < 0) || (badbits > MAX_ERRORS))
		badbits = MAX_ERRORS+1;
	ecc_times[badbits]  += delta;
	err_counts[badbits] += 1;
	if (badbits > max_badbits) max_badbits = badbits;
}

static u32 start_time;
static void timer_start(void)
{
	start_time = NX_TIMER_GetTimerCounter(0);
}

/* stop the stopwatch, and return the time */
static u32 timer_stop(void)
{
	return NX_TIMER_GetTimerCounter(0) - start_time;
}


static u32 nand_num_reads;
	/* The cumulative nand read times for the tree traversal script
	 * overflow 32-bits.
	 * I had trouble figuring out how to output info for 64-bit values,
	 * so I decided to accumulate MAX_TIMES_PER_BIN times in a 32-bit
	 * variable, and then average however many of them there are.
	 * If a test needs more than NUM_TIME_BINS bins, it just wraps around and
	 * re-uses the oldest one.
	 * There were also some problems with division of an unsigned long long.
	 */
#define MAX_TIMES_PER_BIN	200000
#define NUM_TIME_BINS		1000
static u32 cumulative_nand_read_times[NUM_TIME_BINS];
static u32 avg_nand_read_times[NUM_TIME_BINS];

static u32 nand_read_time_min;
static u32 nand_read_time_max;
static u32 nand_read_calc_time;
static u32 nand_read_check_time;
static u32 nand_num_writes;
static u32 nand_write_time;
static u32 nand_write_time_min;
static u32 nand_write_time_max;
static u32 nand_write_calc_time;

static void init_nand_access_times(void) {
	int i;
	for (i = 0; i < NUM_TIME_BINS; ++i) {
		cumulative_nand_read_times[i] = 0;
		avg_nand_read_times[i]        = 0;
	}
	nand_num_reads	= nand_read_time_min
					= nand_read_time_max
					= nand_read_calc_time
					= nand_read_check_time
					= nand_num_writes
					= nand_write_time
					= nand_write_time_min
					= nand_write_time_max
					= nand_write_calc_time
					= 0;
}

static u32 get_avg_nand_read_time(void) {
	u32 i;
	u32 sum;

	if (nand_num_reads == 0) {
		return 0;
	}
	else if (nand_num_reads < MAX_TIMES_PER_BIN) {
		return cumulative_nand_read_times[0] / nand_num_reads;
	}	
	else if (nand_num_reads <= NUM_TIME_BINS * MAX_TIMES_PER_BIN) {
		for (sum = i = 0; avg_nand_read_times[i] > 0; ++i) {
			sum += avg_nand_read_times[i];
		}
		return sum / i;
	}	
	else {
		for (sum = i = 0; i < NUM_TIME_BINS; ++i) {
			if (avg_nand_read_times[i] > 0) {
				sum += avg_nand_read_times[i];
			}
		}
		return sum / (NUM_TIME_BINS - 1);
	}
}

static void update_nand_read_time_info( void )
{
	int bin;
	int idx;
	u32 new_read_time = timer_stop();

	bin = nand_num_reads / MAX_TIMES_PER_BIN;
	if (bin >= NUM_TIME_BINS) {
		bin = bin % NUM_TIME_BINS;
	}
	idx = nand_num_reads % MAX_TIMES_PER_BIN;
	if (idx == 0) {
		cumulative_nand_read_times[bin] = 0;
		avg_nand_read_times[bin]        = 0;
	}
	cumulative_nand_read_times[bin] += new_read_time;
	if (idx == MAX_TIMES_PER_BIN - 1) {
		avg_nand_read_times[bin] = 
			cumulative_nand_read_times[bin]/MAX_TIMES_PER_BIN;
	}
	if (0 == nand_num_reads++) {
		nand_read_time_min = nand_read_time_max = new_read_time;
	}
	else if (new_read_time < nand_read_time_min) {
		nand_read_time_min = new_read_time;
	}
	else if (new_read_time > nand_read_time_max) {
		nand_read_time_max = new_read_time;
	}
}

int read_current_timer(unsigned long *timer_value) {
	*timer_value = NX_TIMER_GetTimerCounter(0);
	return 1;
}
#endif	// NAND_ACCESS_TIMING


static void nand_stats_erase (void)
{
	int i;
	for (i=0; i<NS_MAX; i++)
		ws_n[i] = ws_sum[i] = 0;

#ifdef NAND_ACCESS_TIMING
	max_badbits = -1;

	for (i=0; i < MAX_ERRORS+2; i++) {
		ecc_times[i]  = 0;
		err_counts[i] = 0;
	}
#endif
	for (i = 0; i < MAX_ERRORS_PER_PAGE+1; ++i)
		errors_per_page_counts[i] = 0;
	max_errors_per_page = -1;

	for (i = 0; i <= MAX_RETRIES + 1; ++i)
		histo_of_retries_per_page[i] = 0;
	max_retries_per_page = -1;
}

/*------------------------------------------------------------------------------
 * This routine's conversion factor was calculated this way:
 *
 * PLL 0/1 setting
 *------------------------------------------------------------------------------
 * Fout = (M*Fin) / (P*S), where P = PDIV, M = MDIV, S = 2^SDIV
 *------------------------------------------------------------------------------
 * #define	CFG_SYS_PLL_PMSFUNC(_FIN_, _P_, _M_, _S_)			\
 *	((_M_) * ((_FIN_)/((_P_)*(1UL<<(_S_)))))
 * #define	CFG_SYS_PLLFIN		12000000UL
 *------------------------------------------------------------------------------
 * PLL1 PMS : 147,500,000 Hz    P=3, M=666, SDIV=3 (default)
 * PLL1 PMS : 192,000,000 Hz    P=2, M=256, SDIV=3
 *------------------------------------------------------------------------------
 * #define CFG_SYS_PLL1_USE	CTRUE
 * #define CFG_SYS_PLL1_P	3		//  1 <=, <= 63
 * #define CFG_SYS_PLL1_M	295		// 13 <=, <= 255
 * #define CFG_SYS_PLL1_S	3		// 0, 1, 2, 3
 * #define CFG_SYS_PLL1_FREQ	CFG_SYS_PLL_PMSFUNC(CFG_SYS_PLLFIN, \
 * 				 CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S)
 * 
 * ==> ((_M_) * ((_FIN_)/((_P_)*(1UL<<(_S_)))))
 * 	((666) * ((12000000)/((3)*(1UL<<(3)))))
 * 	((666) * ((12000000)/((3)*(8))))
 * 	((666) * ((12000000)/(24)))
 * 	((666) * (500000))
 * 	333,000,000
 * 
 * ==> PLL1_FREQ/64 = 5,203,125
 * 
 * ==> timer0 tick is .192 microsec.
 * 
 *----------------------------------------------------------------------------*/

#define	NX_TIMER_TICK	0	/* use timer 0 */

static unsigned long ticks_to_microsecs( unsigned long ticks )
{
	u_int   clock_source = NX_TIMER_GetClockSource(NX_TIMER_TICK, 0);
	u_int   clock_divisor = NX_TIMER_GetClockDivisor(NX_TIMER_TICK, 0);
	float ticks_per_usec = 1000000.0 / (cpu_get_clock_hz(clock_source) / clock_divisor);
	return (u32)(ticks_per_usec * ticks);
}


/* Accumulate a start (in=1) or stop (in=0) time for a given type of access */
void nand_stats_accum (enum prof_type type, int in, int block)
{
	long curtime, delta;
	if (type >= NS_MAX) {
		dev_alert(&lf2000_nand.pdev->dev,
				"nand_stats_accum: type=%d > NS_MAX", type);
		return;
	}
	if (!ws_any) {
		/* First time through, erase stats  */
		nand_stats_erase ();
		ws_any = 1;
	}
#ifdef NAND_ACCESS_TIMING
	read_current_timer ((unsigned long *)&curtime);
#else
	curtime = 0;
#endif
	if (in) {
		ws_start[type] = curtime;
	} else {
#ifdef NAND_ACCESS_TIMING
		delta = curtime - ws_start[type];
#else
		delta = 1;
#endif
		ws_sum[type] += delta;
		if (!ws_n[type]) {
			/* First data point, set min and max */
			ws_min[type] = ws_max[type] = delta;
		} else {
			if (ws_min[type] > delta)
				ws_min[type] = delta;
			if (ws_max[type] < delta)
				ws_max[type] = delta;
		}
		ws_n[type]++;
		switch(type) {
		case NS_READ_PAGE:	 
			++total_page_reads;
			if (block < MAX_NUM_ERASE_BLOCKS)
				block_read_counts[block]++;		
			break;
		case NS_READ_SUBPAGE:	 
			++total_subpage_reads;
			if (block < MAX_NUM_ERASE_BLOCKS)
				block_read_counts[block]++;		
			break;
		case NS_READ_RAW:	 
			++total_raw_reads;
			if (block < MAX_NUM_ERASE_BLOCKS)
				block_read_counts[block]++;		
			break;
		case NS_SEND_RD_CMD:	 
			++total_read_commands;
			break;
		case NS_READ_BUF:	 
			++total_read_bufs;
			break;
		case NS_WRITE:	 
			++total_writes;	
			if (block < MAX_NUM_ERASE_BLOCKS)
				block_write_counts[block]++;		
			break;
		case NS_ERASE:	 
			++total_erases;	
			if (block < MAX_NUM_ERASE_BLOCKS)
				block_erase_counts[block]++;		
			break;
		default:							
			break;
		}
		if ((block >= MAX_NUM_ERASE_BLOCKS) || (block < 0))
			printk(KERN_INFO "nsa block %d\n", block);
	}
}

static ssize_t show_access_stats(struct device *dev, 
				 struct device_attribute *attr, char *buf)
{
	int x=0, i;
	static char *title[] = {
		"RdBuf    ",
		"RdSubPage", 
		"RdPage   ", 
		"ReadRaw  ",
		"RdCmds   ",
		"Write    ",
		"Erase    ",
		"Eras2    ",
		"Lock     "
	};
	x += sprintf (buf+x, "NAND access stats:\n"); 
		/* for now let's not output the Lock stats (always 0) */
	for (i=0; i<NS_MAX - 1; i++) 
	{
		if (ws_n[i]) {
			x += sprintf (buf+x, "  %s N=%lu %lu/%ld/%ld ticks"
					     "   [%lu/%lu/%lu microsec]\n", 
				      title[i], ws_n[i], ws_min[i], 
					ws_sum[i]/ws_n[i], ws_max[i],
				    ticks_to_microsecs(ws_min[i]), 
					ticks_to_microsecs(ws_sum[i]/ws_n[i]), 
					ticks_to_microsecs(ws_max[i]));
		} else {
			x += sprintf (buf+x, "  %s N=0L 0L 0L 0L\n", title[i]);
		}
	}
#ifdef NAND_ACCESS_TIMING
	x += sprintf(buf+x, "Error Correction Times\n  #bad  count  time  avg\n");
	for (i = 0; i <= max_badbits; ++i) {
		if (err_counts[i] > 0) {
			x += sprintf(buf+x, "   %d    %d   %d  %d ticks"
					    "  [%ld  %ld microsec]\n",
				i, err_counts[i], ecc_times[i], 
				ecc_times[i]/err_counts[i],
				ticks_to_microsecs((unsigned long)ecc_times[i]), 
				ticks_to_microsecs((unsigned long)ecc_times[i]
					    /err_counts[i]));
		}
	}
#endif
	x += sprintf(buf+x, "Errors per page: max %d\n", max_errors_per_page);
	if (max_errors_per_page > 0) {
		x += sprintf(buf+x, "  Errors  Count\n");
		for (i = 0; i <= max_errors_per_page; ++i) {
			if (errors_per_page_counts[i] > 0) {
				x += sprintf(buf+x, "   %d      %d\n",
						i, errors_per_page_counts[i]);
			}
		}
	}
	x += sprintf(buf+x, "Retries per page: max %d\n", max_retries_per_page);
	if (max_retries_per_page > 0) {
		x += sprintf(buf+x, "  Retries  Count\n");
		for (i = 0; i <= max_retries_per_page; ++i) {
			if (histo_of_retries_per_page[i] > 0) {
				x += sprintf(buf+x, "   %d      %d\n", 
					     i, histo_of_retries_per_page[i]);
			}
		}
	}
	return x;
}

static ssize_t clear_access_stats(struct device *dev, 
				  struct device_attribute *attr, 
				  const char *buf, 
				  size_t count)
{
	nand_stats_erase ();
	return count;
}

static DEVICE_ATTR(access_stats, 
			S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
   			show_access_stats, clear_access_stats);
/*-----------------------------------------------------------------*/

static int block_access_block_index;
static ssize_t show_block_access_stats( struct device *dev, 
					struct device_attribute *attr, 
					char *buf)
{
	int x, i, j;
	int something_to_print = 0;
	
	x = 0;
	for (i = 0, j = block_access_block_index; 
		 (i < 100) && (j < MAX_NUM_ERASE_BLOCKS); ++i, ++j) {
		if (   block_erase_counts[j] 
			|| block_read_counts[j]
			|| block_write_counts[j]) {
			if (!something_to_print) {
				something_to_print = 1;
				x += sprintf (buf+x, "NAND Block Access stats:\n"); 
				x += sprintf (buf+x, " Blk#    Reads  Writes"
						     " Erases Bitflips\n"); 
			}
			x += sprintf (buf+x, " %4d   %6d  %6d %6d %6d\n",
					j, block_read_counts[j], 
					block_write_counts[j],
					block_erase_counts[j], 
					block_bitflip_counts[j]); 
		}
	}
	if (!something_to_print) {
		x += sprintf (buf+x, "No NAND Block Accesses for blocks %d - %d\n",
					block_access_block_index, j-1); 
	}
	block_access_block_index = j;
	if (block_access_block_index >= MAX_NUM_ERASE_BLOCKS)
		block_access_block_index = 0;
	return x;
}

static ssize_t clear_block_access_stats( struct device *dev, 
					 struct device_attribute *attr, 
					 const char *buf, 
					 size_t count)
{
	int i;

	for (i = 0; i < MAX_NUM_ERASE_BLOCKS; ++i) {
		block_erase_counts[i] = block_read_counts[i]
				      = block_write_counts[i]
				      = block_bitflip_counts[i]
				      = 0;
	}
	block_access_block_index = 0;
	return count;
}

static DEVICE_ATTR(block_access_stats, 
		   S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
		   show_block_access_stats, clear_block_access_stats);
/*-----------------------------------------------------------------*/

static ssize_t show_nand_accesses(struct device *dev, 
				  struct device_attribute *attr, char *buf)
{
	int x;

	x = sprintf (buf, "NAND accesses:\n read commands %d\n page reads %d\n"
			  " subpage reads %d\n raw reads %d\n read_bufs %d\n"
			  " page writes %d\n block erasures %d\n bitflips %d\n", 
			total_read_commands, total_page_reads, 
			total_subpage_reads, total_raw_reads, total_read_bufs,
			total_writes, total_erases, total_bitflips);
	return x;
}


static ssize_t clear_nand_accesses(struct device *dev, 
				   struct device_attribute *attr, 
				   const char *buf, size_t count)
{
	total_erases        = 0;
	total_page_reads    = 0;
	total_subpage_reads = 0;
	total_raw_reads     = 0;
	total_read_commands = 0;
	total_writes        = 0;
	total_bitflips      = 0;
	total_read_bufs     = 0;
	return count;
}

static DEVICE_ATTR(nand_accesses, 
		   S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 	
		   show_nand_accesses, clear_nand_accesses);
/*-----------------------------------------------------------------*/


#ifdef NAND_ACCESS_TIMING
static ssize_t show_nand_read_timing(struct device *dev, 
				     struct device_attribute *attr, char *buf)
{
	int x;

	x = sprintf (buf, "NAND read timing:\n"
				"    nand_num_reads %u\n"
				"    read time (ticks): min  %u, avg  %u, max  %u\n"
			    "    read time (usecs): min  %lu, avg  %lu, max  %lu\n", 
			nand_num_reads, 
			nand_read_time_min, 
			get_avg_nand_read_time(), 
			nand_read_time_max,
			ticks_to_microsecs(nand_read_time_min),
			ticks_to_microsecs(get_avg_nand_read_time()),
			ticks_to_microsecs(nand_read_time_max));
	return x;
}


static ssize_t clear_nand_read_timing(struct device *dev, 
				      struct device_attribute *attr, 
				      const char *buf, size_t count)
{
	int i;
	for (i = 0; i < NUM_TIME_BINS; ++i) {
		cumulative_nand_read_times[i] = 0;
		avg_nand_read_times[i]        = 0;
	}
    nand_num_reads     = 0;
    nand_read_time_min = 0;
    nand_read_time_max = 0;
	return count;
}

static DEVICE_ATTR(nand_read_timing, 
		   S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 	
		   show_nand_read_timing, clear_nand_read_timing);
/*-----------------------------------------------------------------*/
#endif	// NAND_ACCESS_TIMING

#define NAND_STATS_ACCUM(a,b,c) nand_stats_accum(a,b,c)

#else	/* CONFIG_MTD_NAND_LF2000_PROF not defined */

#define NAND_STATS_ACCUM(a,b,c) do { } while (0)

#endif	/* End #ifdef CONFIG_MTD_NAND_LF2000_PROF */



#include <linux/sysfs.h>

static ssize_t show_nand_timing(struct device *dev, 
				struct device_attribute *attr, char *buf)
{
	ssize_t len = 0;
	unsigned int ACS, COS, OCH, CAH, ACC0;
	NX_NAND_Get_Timing( &ACS, &COS, &OCH, &CAH, &ACC0); 

	len = sprintf(buf,"0: ACS  0x%08x\n1: COS  0x%08x\n2: OCH  0x%08x\n"
			  "3: CAH  0x%08x\n4: ACC0 0x%08x\n",
			   ACS, COS, OCH, CAH, ACC0);
	return len;
}

static ssize_t set_nand_timing(	struct device *dev,
				struct device_attribute *attr, 
				const char *buf, size_t count)
{
	unsigned int index, value;

	if (   (sscanf(buf, "%u,%x", &index, &value) != 2)
		|| (index > kACC0))
		return -EINVAL;
	
	if (0 != NX_NAND_Set_Timing(index, value))
		return -EINVAL;

	return count;
}

static DEVICE_ATTR(timing, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
		   show_nand_timing, set_nand_timing);
/*-----------------------------------------------------------------*/

#define NAND_OC_ONBOARD	0
#define NAND_OC_CART	1

static ssize_t show_ecc_mode(int oc, char *buf)
{
	const struct tag_bch_info * bch_info;
	struct nand_chip *chip;
	ssize_t len = 0;

	if (oc == NAND_OC_ONBOARD) {
		if (   (NULL == lf2000_nand.onboard_nand)
			|| (NULL == lf2000_nand.onboard_nand->mtd.priv)) {
			len = sprintf(buf, "Onboard NAND not found!?\n");
			return len;
		}
		chip = (struct nand_chip *)lf2000_nand.onboard_nand->mtd.priv;
	} else {
		if (   (NULL == lf2000_nand.cart_nand)
			|| (NULL == lf2000_nand.cart_nand->mtd.priv)) {
			len = sprintf(buf, "Cartridge NAND not found\n");
			return len;
		}
		chip = (struct nand_chip *)lf2000_nand.cart_nand->mtd.priv;
	}

	if (chip->ecc.mode == NAND_ECC_NONE) 
		len = sprintf(buf, "ecc_mode: NONE\n");
	else if (chip->ecc.mode == NAND_ECC_SOFT) 
		len = sprintf(buf, "ecc_mode: 1 bit\n");
	else {
		bch_info = ((struct lf2000_nand_data *)chip->priv)->bch_info;
		len = sprintf(buf, "ecc_mode: %d bits\n", bch_info->bch_var_t);
	}
	return len;
}

void nand_setup_for_swecc( struct nand_chip *chip );
void nand_setup_for_no_ecc( struct mtd_info *mtd, struct nand_chip *chip ); 
 
static ssize_t set_ecc_mode( int oc, const char *buf, size_t count)
{
	struct nand_chip *chip;
	struct mtd_info  *mtd;
	unsigned int new_mode;

	if(sscanf(buf, "%u", &new_mode) != 1)
		return -EINVAL;
	switch (new_mode) {
	case 0:
	case 1:
	case 4:
	case 8:
	case 12:
	case 16:
	case 24:
		break;

	default:
		return -EINVAL;
	}
	if (oc == NAND_OC_ONBOARD) {
		if (   (NULL == lf2000_nand.onboard_nand)
			|| (NULL == lf2000_nand.onboard_nand->mtd.priv)) {
			printk(KERN_INFO "Onboard NAND not found!?\n");
			return -EINVAL;
		}
		mtd  = &lf2000_nand.onboard_nand->mtd;
	} else {
		if (   (NULL == lf2000_nand.cart_nand)
			|| (NULL == lf2000_nand.cart_nand->mtd.priv)) {
			printk(KERN_INFO "Cartridge NAND not found!?\n");
			return -EINVAL;
		}
		mtd  = &lf2000_nand.cart_nand->mtd;
	}
	chip = (struct nand_chip *)mtd->priv;

	if (new_mode == 0) {
		chip->ecc.mode = NAND_ECC_NONE;
		nand_setup_for_no_ecc(mtd, chip);
	}
	else if (new_mode == 1) {
		chip->ecc.mode = NAND_ECC_SOFT;
		chip->ecc.size = 256;
		nand_setup_for_swecc(chip);
	}
	else if (nand_hw_ecc_init(mtd, new_mode) != 0) 
	{
		return -EINVAL;
	}
	chip->ecc.steps = mtd->writesize / chip->ecc.size;
	chip->ecc.total = chip->ecc.steps * chip->ecc.bytes;
printk(KERN_INFO "ecc.steps %d, ecc.total %d, ecc.size %d\n",
		chip->ecc.steps, chip->ecc.total, chip->ecc.size);
	if (0 != nand_hw_ecc_check(mtd)) {
		return -EINVAL;
	}
	return count;
}

static ssize_t show_ecc_mode_onboard(struct device *dev, 
				     struct device_attribute *attr, char *buf)
{
	return show_ecc_mode( NAND_OC_ONBOARD, buf);
}

static ssize_t set_ecc_mode_onboard(struct device *dev,
				    struct device_attribute *attr, 
				    const char *buf, size_t count)
{
	return set_ecc_mode( NAND_OC_ONBOARD, buf, count);
}

static ssize_t show_ecc_mode_cart(struct device *dev, 
				  struct device_attribute *attr, char *buf)
{
	return show_ecc_mode( NAND_OC_CART, buf);
}

static ssize_t set_ecc_mode_cart(struct device *dev,
				 struct device_attribute *attr, 
				 const char *buf, size_t count)
{
	return set_ecc_mode( NAND_OC_CART, buf, count);
}

static DEVICE_ATTR(ecc_mode_onboard, 
			S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
			show_ecc_mode_onboard, set_ecc_mode_onboard);
static DEVICE_ATTR(ecc_mode_cart, 
			S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
			show_ecc_mode_cart, set_ecc_mode_cart);
/*-----------------------------------------------------------------*/


static ssize_t set_nfcontrol( struct device *dev,
			      struct device_attribute *attr, 
			      const char *buf, size_t count)
{
	unsigned int value;

	if(sscanf(buf, "%u", &value) != 1)
		return -EINVAL;
	
	NX_NAND_Set_NFCONTROL(value);

	return count;
}

static ssize_t show_nfcontrol(struct device *dev, 
			      struct device_attribute *attr, char *buf)
{
	ssize_t len;

	len = sprintf(buf, "NFCONTROL: 0x%08x\n", NX_NAND_Get_NFCONTROL());

	return len;
}

static DEVICE_ATTR(nfcontrol, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
		   show_nfcontrol, set_nfcontrol);
/*-----------------------------------------------------------------*/


static ssize_t show_ecc_regs(struct device *dev, 
			     struct device_attribute *attr, char *buf)
{
	unsigned int ecc_regs[11];
	ssize_t len = 0;
	int		i;

	if (0 == NX_NAND_Get_ECC_regs(ecc_regs)) {
		for (i = 0; i < 11; ++i) {
			len += sprintf( buf+len, "NFECC%d: 0x%08x\n", 
					i, ecc_regs[i]);
		}
	}
	return len;
}

static DEVICE_ATTR(ecc_regs, S_IRUSR|S_IRGRP|S_IROTH, 
		   show_ecc_regs, NULL);
/*-----------------------------------------------------------------*/



static ssize_t show_orig_ecc_regs(struct device *dev, 
				  struct device_attribute *attr, char *buf)
{
	unsigned int orig_ecc_regs[11];
	ssize_t len = 0;
	int		i;

	if (0 == NX_NAND_Get_OrigECC_regs(orig_ecc_regs)) {
		for (i = 0; i < 11; ++i) {
			len += sprintf( buf+len, "OrigECC%d: 0x%08x\n", 
					i, orig_ecc_regs[i]);
		}
	}
	return len;
}
		/* I think there is no need to set the OrigECC regs at run-time */
static DEVICE_ATTR(orig_ecc_regs, S_IRUSR|S_IRGRP|S_IROTH, 
		   show_orig_ecc_regs, NULL);
/*-----------------------------------------------------------------*/



static ssize_t show_nfcnt(struct device *dev, 
			  struct device_attribute *attr, char *buf)
{
	unsigned int nfcnt;
	ssize_t len;

	nfcnt = NX_NAND_Get_NFCNT();
	len = sprintf(buf, "NFRdCnt 0x%03x; NFWrCnt 0x%03x\n", 
			    nfcnt & 0x3FF, (nfcnt >> 16) & 0x3FF);

	return len;
}

static DEVICE_ATTR(nfcnt, S_IRUSR|S_IRGRP|S_IROTH, show_nfcnt, NULL);
/*-----------------------------------------------------------------*/



static ssize_t show_nfeccstatus(struct device *dev, 
				struct device_attribute *attr, char *buf)
{
	unsigned int nfeccstatus;
	ssize_t len;

	nfeccstatus = NX_NAND_Get_NFECCSTATUS();
	len = sprintf(buf, "NFECCSTATUS 0x%08x\n", nfeccstatus);

	return len;
}

static DEVICE_ATTR(nfeccstatus, S_IRUSR|S_IRGRP|S_IROTH, 
		   show_nfeccstatus, NULL);
/*-----------------------------------------------------------------*/


static ssize_t show_syndromes(struct device *dev, 
			      struct device_attribute *attr, char *buf)
{
	unsigned int syndromes[12];
	ssize_t len = 0;
	int	i;

	if (0 == NX_NAND_Get_SYNDROMES(syndromes)) {
		for (i = 0; i < 12; ++i) {
			len += sprintf(buf+len, "Syndrome%d: 0x%08x\n", i, syndromes[i]);
		}
	}
	return len;
}

static DEVICE_ATTR(syndromes, S_IRUSR|S_IRGRP|S_IROTH, show_syndromes, NULL);
/*-----------------------------------------------------------------*/


static ssize_t set_nfeccctrl( struct device *dev,
			      struct device_attribute *attr, 
			      const char *buf, size_t count)
{
	unsigned int value;

	if(sscanf(buf, "%u", &value) != 1)
		return -EINVAL;
	
	NX_NAND_Set_NFECCCTRL(value);

	return count;
}

static ssize_t show_nfeccctrl(struct device *dev, 
			      struct device_attribute *attr, char *buf)
{
	ssize_t len;

	len = sprintf(buf, "NFECCCTRL: 0x%08x\n", NX_NAND_Get_NFECCCTRL());

	return len;
}

static DEVICE_ATTR(nfeccctrl, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
		   show_nfeccctrl, set_nfeccctrl);
/*-----------------------------------------------------------------*/


static ssize_t set_elp_reg( struct device *dev,
			    struct device_attribute *attr, 
			    const char *buf, size_t count)
{
	unsigned int index, value;

	if (   (sscanf(buf, "%u,%u", &index, &value) != 2)
	    || (index >= 12))
		return -EINVAL;
	
	if (0 != NX_NAND_Set_ELP_reg(index, value))
		return -EINVAL;

	return count;
}

static ssize_t show_elp_regs(struct device *dev, 
			     struct device_attribute *attr, char *buf)
{
	unsigned int elp_regs[12];
	ssize_t len = 0;
	int	i;

	if (0 == NX_NAND_Get_ELP_regs(elp_regs)) {
		for (i = 0; i < 12; i += 2) {
			len += sprintf(buf+len, "ELP%d: 0x%08x\tELP%d 0x%08x\n", 
					i, elp_regs[i], i+1, elp_regs[i+1]);
		}
	}
	return len;
}

static DEVICE_ATTR(elp_regs, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
		   show_elp_regs, set_elp_reg);
/*-----------------------------------------------------------------*/


static ssize_t show_eloc_regs(struct device *dev, 
			      struct device_attribute *attr, char *buf)
{
	unsigned int eloc_regs[12];
	ssize_t len = 0;
	int		i;

	if (0 == NX_NAND_Get_ELOC_regs(eloc_regs)) {
		for (i = 0; i < 12; i += 2) {
			len += sprintf(buf+len, "ELOC%d: 0x%08x\tELOC%d 0x%08x\n", 
					i, eloc_regs[i], i+1, eloc_regs[i+1]);
		}
	}
	return len;
}

static DEVICE_ATTR(eloc_regs, S_IRUSR|S_IRGRP|S_IROTH, 
		   show_eloc_regs, NULL);
/*-----------------------------------------------------------------*/

//#ifdef CONFIG_MTD_NAND_LF1000_HOTSWAP

static ssize_t get_cart_hotswap_state(struct device *dev, 
				      struct device_attribute *attr, char *buf)
{
	int ready = 0;

	/* first check if a struct lf2000_nand_data for a cart has been allocated */
	if(lf2000_nand.cart_nand != NULL) {
		ready = lf2000_nand.cart_ready;
	}
	return sprintf (buf, "%d\t%d\n", ready, lf2000_nand.cart_ubi);
}

static ssize_t set_cart_hotswap_state(struct device *dev, 
				      struct device_attribute *attr, 
				      const char *buf, size_t count)
{
	struct resource *res;
	int cart_parts_nb = 0;
	int hotswap_state;
	struct platform_device *pdev__ ;	
	struct mtd_partition *cart_parts = NULL;
	struct nand_chip * chip;
	struct mtd_info  * mtd;
	size_t nread;
	uint32_t magic;
	ssize_t ret = count;
	int scan;
	int i = 0;
	
	printk(KERN_INFO "%s: 1 \n", __func__);

	if(sscanf(buf, "%d", &hotswap_state) != 1)
		return -EINVAL;
	else printk(KERN_INFO "%s: %d\n", __func__, hotswap_state);

	pdev__= to_platform_device(dev);
	res = platform_get_resource(pdev__, IORESOURCE_MEM, 0);
	if(!res) {
		dev_err(dev, "nand: cart_hotswap() failed to get resource!\n");
		return -ENXIO;
	}

	if (down_interruptible(&lf2000_nand.sem_hotswap))
		return -ERESTARTSYS;
		
	gpio_request(CARTRIDGE_DETECT,"Cartridge Detect");
	gpio_request(NAND_CHIP_SELECT,"Nand Chip Select");

	if ((0 != hotswap_state) && (1 != hotswap_state)) 
	{
		gpio_set_function(CARTRIDGE_DETECT, 0); /* configure CARTRIDGE_DETECT as GPIO */
		gpio_direction_input(CARTRIDGE_DETECT); /* and an input */
		if (gpio_get_value(CARTRIDGE_DETECT) != NAND_CART_DETECT_LEVEL) 
		{
			hotswap_state = 0;
		}  
		else 
		{
			hotswap_state = 1;
		}
	}

	if(0 == hotswap_state) {	/* cart is removed */
		lf2000_nand.cart_ready = 0;
		lf2000_cart_remove();
		dev_info(dev, "cartridge removed !\n");		
	}
	else 
	{  /* cart is inserted */
		
		/* check if a cartridge is inserted */
		gpio_set_function(NAND_CHIP_SELECT, 1); /* configure NAND_CHIP_SELECT as AltFn1 */	
		gpio_set_function(CARTRIDGE_DETECT, 0); /* configure CARTRIDGE_DETECT as GPIO */
		gpio_direction_input(CARTRIDGE_DETECT); /* and an input */
		if (gpio_get_value(CARTRIDGE_DETECT) != NAND_CART_DETECT_LEVEL) 
		{
			dev_err(dev, "cartridge insertion can't be confirmed by driver\n");
			ret = -EAGAIN;
			goto out;
		}
//..............................................................................
// TODO: FIXME: Looks as though on E2K board, CIP_L is connected to GPIOA[18].
//				It must be configured as a GIO (function 0) input.
// Pin Descriptions on p.17 of the LF2000 datasheet indicate GPIOA[18] can
// operate only as an input, but maybe that's only when its alternate func#1
// (RX2) is selected.  The Ball description on p23 indicates it's I/O.
//
//				Something like this:
//		NX_GPIO_SetPadFunction(0,18,0);
//		NX_GPIO_SetOutputEnable ( 0, 18, CFALSE );
//..............................................................................
// Sam sez it's necessary to configure GPIOC[31:21] and GPIOD[0] to be
// alt-function SDEX bus. 
// 10Oct12 - spalav removed the conditional compiling logic for VTK too, 
// GPIOC20 is taken care of, others are not NAND control pins? 

		dev_info(dev, "cartridge inserted\n");
			
		if(lf2000_nand.cart_ready == 1){
			dev_err(dev, "cartridge driver was ready\n");
			goto out;
		}
		/* Allocate memory for the onboard nand data struct */
		lf2000_nand.cart_nand = (struct lf2000_nand_data *)
					kzalloc(sizeof(struct lf2000_nand_data),
					GFP_KERNEL);
		if (!lf2000_nand.cart_nand) {
			dev_err(dev,"failed to allocate cart_nand device structure.\n");
			ret = -ENOMEM;
			goto out;
		}
		lf2000_nand.cart_nand->io_base = lf2000_nand.mem;
		chip = &lf2000_nand.cart_nand->chip;
		mtd  = &lf2000_nand.cart_nand->mtd;

		mtd->priv  = chip;
		mtd->owner = THIS_MODULE;
		mtd->name  = "lf2000-cart";
		chip->controller = &lf2000_nand.controller;
		chip->priv = lf2000_nand.cart_nand;	

		init_lf2000_cart_nand(chip);


		/* After unsuccessful scan, delay for an increasing period */
		/* and then try again */
		do {
			scan = nand_scan(mtd, 1);
			if (0 == scan)
				break;
			if (-EPERM == scan) {
				break;	/* permanent error, don't retry */
			}

			udelay(10000 << i);	/* 10 msec, 20 msec, 40 msec */
			printk(KERN_INFO "Delayed %d microsec before again"
					 " calling nand_scan\n",
				10000 << i);
		} while (++i < 4);

		if (i > 1)
			dev_info(dev, "tried to scan cartridge %d times\n",i);
			
		if (scan) {
			lf2000_nand.cart_ready = -1;
			dev_err(dev, "cartridge inserted, but NAND not detected !\n");
			nand_release(mtd);
			kfree(lf2000_nand.cart_nand);	
			lf2000_nand.cart_nand = NULL;
			ret = -EPERM;
			goto out;
		}
		printk(KERN_INFO "cart chip options: 0x%08x\n", chip->options);

		if (chip->ecc.mode == NAND_ECC_HW) {
			mtd->_read     = lf2000_nand_read;
			mtd->_read_oob = lf2000_nand_read_oob;
		}
#if 0	// FIXME: TODO: Do we want/need something like this, or can we
	//		restrict profile stats to the onboard NAND?
#ifdef CONFIG_MTD_NAND_LF2000_PROF
		chip->erase_cmd = lf2000_single_erase_cmd;
#endif // CONFIG_MTD_NAND_LF2000_PROF
#endif

		if ( 0 != (ret = nand_hw_ecc_check(mtd))) {
			lf2000_nand.cart_ready = -1;
			dev_err(dev, "nand_hw_ecc_check() found error!\n");
			nand_release(mtd);
			kfree(lf2000_nand.cart_nand);	
			lf2000_nand.cart_nand = NULL;
			ret = -EPERM;
			goto out;
		}

		cart_parts_nb = parse_mtd_partitions(mtd, part_probes, &cart_parts, NULL);
		if (cart_parts_nb == 0) {
			cart_parts    = partition_info_cart;
			cart_parts_nb = ARRAY_SIZE(partition_info_cart);
		}
			/* Register the cartridge partitions, if it exists */
		mtd_device_register(mtd, cart_parts, cart_parts_nb);

		mtd->_read(mtd, 0, sizeof(uint32_t), &nread, (void *)&magic);

		magic = be32_to_cpu(magic);
		if (magic == UBI_EC_HDR_MAGIC) {
			lf2000_nand.cart_ubi=1;
			dev_info(dev, "cartridge has UBI layer, nread=%d\n", nread);
		} 
		else {
			lf2000_nand.cart_ubi=0;
			dev_info(dev,"cartridge has no UBI, nread=%d\n", nread);
		}
		lf2000_nand.cart_ready = 1;
		dev_info(dev, "cart driver ready !\n");
	} 
out:	
	up(&lf2000_nand.sem_hotswap);	
	return ret;
}
static DEVICE_ATTR(cart_hotswap, 
		   S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH,
		   get_cart_hotswap_state, set_cart_hotswap_state);
/*-----------------------------------------------------------------*/

static void nand_dev_read_buf_dma(struct mtd_info *mtd, uint8_t *buf, int len);
static void nand_dev_read_buf(struct mtd_info *mtd, uint8_t *buf, int len);

static ssize_t set_nand_dma( struct device *dev,
			     struct device_attribute *attr, 
			     const char *buf, size_t count)
{
	unsigned int value;

	if(sscanf(buf, "%u", &value) != 1)
		return -EINVAL;
	
	if (value) {
		if (lf2000_nand.tr == NULL) {
			lf2000_nand.tr = soc_dma_request(CFG_DMA_NAND_RW, CFALSE);
			if (lf2000_nand.tr != NULL) {
				lf2000_nand.onboard_nand->chip.read_buf = nand_dev_read_buf_dma;
				lf2000_nand.tr->channel   = CFG_DMA_NAND_RW;
				lf2000_nand.tr->tr_type   = DMA_TRANS_IO2MEM;
				lf2000_nand.tr->srcbase   = PHY_BASEADDR_NAND_STATIC;
				lf2000_nand.tr->requestid = 0;
				lf2000_nand.tr->bitwidth  = 32;
			}
			else {
				printk(KERN_INFO "Failed to get DMA channel for NAND (%d)\n",
						CFG_DMA_NAND_RW);
			}
		}
	}
	else {
		if (lf2000_nand.tr != NULL) {
			soc_dma_release(lf2000_nand.tr);
			lf2000_nand.tr = NULL;
		}
		lf2000_nand.onboard_nand->chip.read_buf = nand_dev_read_buf;
	}

	return count;
}

static ssize_t show_nand_dma(struct device *dev, 
			     struct device_attribute *attr, char *buf)
{
	ssize_t len;

	len = sprintf(buf, "nand_dma: %s\n", 
		  	(lf2000_nand.onboard_nand->chip.read_buf 
				== nand_dev_read_buf_dma) ? "ON" : "OFF");
	return len;
}

static DEVICE_ATTR(nand_dma, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
		show_nand_dma, set_nand_dma);


/*-----------------------------------------------------------------*/

/* Not much information about ECC errors is written to the serial port
 * unless report_ecc_errors is non-zero AND the device was booted in
 * VERBOSE mode.
 */
u32 report_ecc_errors = 0;

static ssize_t set_report_ecc_errors( struct device *dev,
			     struct device_attribute *attr, 
			     const char *buf, size_t count)
{
	unsigned int value;

	if(sscanf(buf, "%u", &value) != 1)
		return -EINVAL;
	
	report_ecc_errors = (value) ? 1 : 0;

	return count;
}

static ssize_t show_report_ecc_errors(struct device *dev, 
			     struct device_attribute *attr, char *buf)
{
	ssize_t len;

	len = sprintf(buf, "report_ecc_errors: %d (%s)\n", 
			report_ecc_errors,
		  	report_ecc_errors ? "Yes" : "No");
	return len;
}

static DEVICE_ATTR(report_ecc_errors, 
		   S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, 
		   show_report_ecc_errors, set_report_ecc_errors);


/*-----------------------------------------------------------------*/


static ssize_t show_ticks(struct device *dev, 
			  struct device_attribute *attr, char *buf)
{
	ssize_t len;

	len = sprintf(buf, "ticks: %d\n", NX_TIMER_GetTimerCounter(0));

	return len;
}

static DEVICE_ATTR(ticks, S_IRUSR|S_IRGRP|S_IROTH, show_ticks, NULL);
/*-----------------------------------------------------------------*/

/******************************************************************************/

/* Protect NOR from accidental erasure:
 * Create an address threshold; addresses LOWER than this address are
 *   blocked from erase or write commands to the nor (will return -EPERM).
 * By default, set the threshold up high, to protect everything in NOR
 */
/* NOTE 3oct13: this cannot be static, because it is referenced from 
 *	mtd/chips/cfi_cmdset_0002.c and 
 *	mtd/devices/m25p80.c
 */
u32 nor_write_addr_threshold = 0x7fffffff;

static ssize_t show_nor_write_addr_threshold(struct device *dev, 
					     struct device_attribute *attr, 
					     char *buf)
{
	return sprintf (buf, "0x%08x\n", nor_write_addr_threshold);
}

static ssize_t set_nor_write_addr_threshold(struct device *dev, 
					    struct device_attribute *attr, 
					    const char *buf, size_t count)
{
	if(sscanf(buf, "0x%x", &nor_write_addr_threshold) != 1)
		if(sscanf(buf, "%d", &nor_write_addr_threshold) != 1)
			return -EINVAL;
	return count;
}

static DEVICE_ATTR(nor_write_addr_threshold, 
		   S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH,
		   show_nor_write_addr_threshold, set_nor_write_addr_threshold);

/******************************************************************************/

static struct attribute *nand_attributes[] = {
	&dev_attr_timing.attr,
	&dev_attr_ecc_mode_onboard.attr,
	&dev_attr_ecc_mode_cart.attr,
	&dev_attr_nfcontrol.attr,
	&dev_attr_ecc_regs.attr,
	&dev_attr_orig_ecc_regs.attr,
	&dev_attr_nfcnt.attr,
	&dev_attr_nfeccstatus.attr,
	&dev_attr_syndromes.attr,
	&dev_attr_nfeccctrl.attr,
	&dev_attr_elp_regs.attr,
	&dev_attr_eloc_regs.attr,
	&dev_attr_cart_hotswap.attr,
	&dev_attr_nand_dma.attr,
	&dev_attr_report_ecc_errors.attr,
#ifdef CONFIG_MTD_NAND_LF2000_PROF
	&dev_attr_access_stats.attr,
	&dev_attr_block_access_stats.attr,
	&dev_attr_nand_accesses.attr,
#ifdef NAND_ACCESS_TIMING
	&dev_attr_nand_read_timing.attr,
#endif
	&dev_attr_ticks.attr,
#endif
	&dev_attr_nor_write_addr_threshold.attr,
	NULL
};

static struct attribute_group nand_attr_group = {
	.attrs = nand_attributes
};


/*------------------------------------------------------------------------------
 * nand interface
 */
static int lf2000_cart_remove(void)
{
	/* Release resources, unregister device */
	if(lf2000_nand.cart_nand)
		nand_release(&lf2000_nand.cart_nand->mtd);
	
	if(lf2000_nand.cart_nand)
		kfree(lf2000_nand.cart_nand);
	
	lf2000_nand.cart_nand = NULL;
	
	return 0;
}

/* Only for L2K, M2K and BID */
static void lf_nand_select(struct mtd_info *mtd, int chipnr)
{
	struct nand_chip * chip = (struct nand_chip *)mtd->priv;
	DBGOUT("%s, chipnr=%d\n", __func__, chipnr);
	//printk(KERN_INFO "%s: 3 chipnr=%d\n", __func__, chipnr);

	if (-1 == chipnr) {
		NX_MCUS_SetNFCSEnable(CFALSE);
	} else {
		NX_MCUS_SetNFBank(((struct lf2000_nand_data *)chip->priv)->nand_bank);
		NX_MCUS_SetNFCSEnable(CTRUE);
		NX_MCUS_SetInterruptEnable( 0, CTRUE);
	}
}

/* Select functions for all other platforms */
static void onboard_nand_select(struct mtd_info *mtd, int chipnr)
{
	DBGOUT("%s, chipnr=%d\n", __func__, chipnr);
	//printk(KERN_INFO "%s: 3 chipnr=%d\n", __func__, chipnr);

	if (-1 == chipnr) {
		NX_MCUS_SetNFCSEnable(CFALSE);
	} else {
		NX_MCUS_SetNFBank(LF2000_BASE_NAND_BANK);
		NX_MCUS_SetNFCSEnable(CTRUE);
		NX_MCUS_SetInterruptEnable( 0, CTRUE);
	}
}

static void cart_nand_select(struct mtd_info *mtd, int chipnr)
{
	DBGOUT("%s, chipnr=%d\n", __func__, chipnr);
	//printk(KERN_INFO "%s: 3 chipnr=%d\n",__func__, chipnr);

	if (-1 == chipnr) {
		NX_MCUS_SetNFCSEnable(CFALSE);
	} else {
		NX_MCUS_SetNFBank(LF2000_CART_NAND_BANK);
		NX_MCUS_SetNFCSEnable(CTRUE);
		NX_MCUS_SetInterruptEnable( 0, CTRUE);
	}
}
// Till here

#define MASK_CLE	0x10	/* NFCM   + 2C00_0000 */
#define MASK_ALE	0x18	/* NFADDR + 2C00_0000 */

static void nand_dev_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	void __iomem* addr = chip->IO_ADDR_W;
	int ret = 0;

	if (cmd == NAND_CMD_NONE)
		return;

	if (ctrl & NAND_CLE)
		writeb(cmd, addr + MASK_CLE);
	else if (ctrl & NAND_ALE)
		writeb(cmd, addr + MASK_ALE);

	if (cmd != NAND_CMD_RESET &&
		cmd != NAND_CMD_READSTART)
		CLEAR_RnB(ret);
}

static int nand_dev_ready(struct mtd_info *mtd)
{
	int ret;
	CLEAR_RnB(ret);
	DBGOUT("[%s, RnB=%d]\n", ret?"READY":"BUSY", NX_MCUS_IsNFReady());
	return ret;
}

#include <linux/dma-mapping.h>
#include <asm/memory.h>
#define DMASRCADDR	0
#define DMADSTADDR	4
#define DMALENGTH	8
#define DMAREQID	0x0A
#define DMAMODE		0x0C
#define DMA_INTPEND	0x20000	/* bit 17 of DMAMODE register */

static void nand_dev_read_buf_dma(struct mtd_info *mtd, uint8_t *buf, int len)
{
	dma_addr_t dma_handle;
	do {
		dma_handle = dma_map_single(&mtd->dev, buf, len, DMA_FROM_DEVICE);
		if (0 == dma_handle) {
			printk(KERN_INFO "NULL dma_handle\n");
			cond_resched();
		}
	} while (0 == dma_handle);

	lf2000_nand.tr->dstbase = (unsigned int)dma_handle;
	lf2000_nand.tr->length  = (unsigned int)len;
	soc_dma_transfer(lf2000_nand.tr);
#if 0	// 12oct11
	// add code to sleep while waiting on the wait queue
	// (I suppose w/ a timeout).
#else
	soc_dma_wait_ready(lf2000_nand.tr, 100000);
#endif
	dma_unmap_single(&mtd->dev, dma_handle, len, DMA_FROM_DEVICE);
}

static void nand_dev_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;

	if (len >= chip->ecc.size)
		NAND_STATS_ACCUM(NS_READ_BUF, 1, 0);

	readsl(chip->IO_ADDR_R, buf, (len >> 2));
	if (len & 3)
		readsb(chip->IO_ADDR_R, buf + (len & ~0x3), (len & 3));

	if (len >= chip->ecc.size)
		NAND_STATS_ACCUM (NS_READ_BUF, 0, 0);
}

static void nand_dev_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	writesl(chip->IO_ADDR_W, buf, (len >> 2));
	if (len & 3)
		writesb(chip->IO_ADDR_W, buf + (len & ~0x3), (len & 3));
}
/*------------------------------------------------------------------------------
 * u-boot nand module
 */

extern int nand_get_device( struct nand_chip *chip, 
							struct mtd_info *mtd, 
							int new_state);
extern void nand_release_device(struct mtd_info *mtd);
int nand_do_write_oob(struct mtd_info *mtd, loff_t to,
			     	  struct mtd_oob_ops *ops);

/**
 * nand_block_bad_first_or_last
 * @mtd:	MTD device structure
 * @ofs:	offset from device start
 * @getchip:	0, if the chip is already selected
 *
 * Returns nonzero (TRUE) if block is bad.
 *
 * This is derived from the default implementation (nand_block_bad() in
 * nand_base.c).
 * It considers a block to be bad if it finds a non-FF value in the first oob
 * byte of either the first page or the last page of the block.  This conforms
 * to the method that Samsung and Hynix use for marking bad blocks in their 
 * 2GB sub-50nm MLC NANDs.
 */
static int 
nand_block_bad_first_or_last(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	int page, chipnr, res = 0;
	struct nand_chip *chip = mtd->priv;
	u8 bad;

	page = (int)(ofs >> chip->page_shift) & chip->pagemask;

	if (getchip) {
		chipnr = (int)(ofs >> chip->chip_shift);

		nand_get_device(chip, mtd, FL_READING);

		/* Select the NAND device */
		chip->select_chip(mtd, chipnr);
	}
		/* since the LF2000 doesn't support 16-bit NANDs, we omit the 
		 * check for 16-bit nand bus.
		 */
	chip->cmdfunc(mtd, NAND_CMD_READOOB, chip->badblockpos, page);
	bad = chip->read_byte(mtd);

	res = (bad != 0xFF);
	if (!res) {	/* first page oob does not indicate bad.  Check last page. */
		// TODO: Check if this adjustment is correct.  
		//	 Is ofs initially the offset to the first page of the block?
		ofs += mtd->erasesize - mtd->writesize;
		page = (int)(ofs >> chip->page_shift) & chip->pagemask;
		chip->cmdfunc(mtd, NAND_CMD_READOOB, chip->badblockpos, page);
		bad = chip->read_byte(mtd);
		res = (bad != 0xFF);
	}
	if (getchip)
		nand_release_device(mtd);

	return res;
}

/**
 * nand_block_markbad_first_and_last
 * @mtd:	MTD device structure
 * @ofs:	offset from device start
 *
 * This overrides the default implementation.
 * It stores a non-FF value in the oob of both the first and last pages of
 * the block.  This conforms to the method that Samsung and Hynix use for
 * marking bad blocks in their 2GB sub-50nm MLC NANDs.
 */
int nand_block_markbad_first_and_last(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_chip *chip = mtd->priv;
	uint8_t buf = 0;
	int block, ret;
	struct erase_info ei;


	/* Get block number */
	block = (int)(ofs >> chip->bbt_erase_shift);

	/* Erase before trying to mark bad */
	memset(&ei, 0, sizeof(struct erase_info));

	ei.mtd      = mtd;
	ei.addr     = ofs;
	ei.len      = mtd->erasesize;
	ei.callback = NULL;
	ei.priv     = 0;
	ret = mtd->_erase( mtd, &ei);
	if (ret) {
		printk(KERN_INFO "nand_block_markbad_first_and_last():"
				 " mtd->_erase() returned %d\n", ret);
	}

	if (chip->bbt)
		chip->bbt[block >> 2] |= 0x01 << ((block & 0x03) << 1);

	/* Do we have a flash based bad block table ? */
	if (chip->bbt_options & NAND_BBT_USE_FLASH)
		ret = nand_update_bbt(mtd, ofs);
	else {
		struct mtd_oob_ops ops;

		nand_get_device(chip, mtd, FL_WRITING);

		/* Write to first and last page.
		 * If we detect an error, we immediately quit the procedure. 
		 * We write one byte per location because the LF2000 supports only
		 * an 8-bit nand bus.
		 */
		ops.len	    = ops.ooblen = 1;
		ops.datbuf  = NULL;
		ops.oobbuf  = &buf;
		ops.ooboffs = 0;
		ops.mode    = MTD_OPS_PLACE_OOB;

		ret = nand_do_write_oob(mtd, ofs, &ops);

		if (!ret) {
			ofs += mtd->erasesize - mtd->writesize;
			ret  = nand_do_write_oob(mtd, ofs, &ops);
		}
		nand_release_device(mtd);
	}
	if (!ret)
		mtd->ecc_stats.badblocks++;

	return ret;
}


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
int scan_block_faster(struct mtd_info *mtd, struct nand_bbt_descr *bd,
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

		/* Read the first byte of the spare area and check if it's 0xFF */
		/* TODO: FIXME: This could be sped up by moving code from 
		 *				nand_do_read_oob() inline here.
		 */
	ret = mtd->_read_oob(mtd, offs, &ops);

	if (ret)
		return ret;

	if (0xFF != buf[0])
		return 1;

	ret = mtd->_read_oob(mtd, offs + mtd->erasesize - mtd->writesize, &ops);
	if (ret)
		return ret;

	if (0xFF != buf[0])
		return 2;

	return 0;
}

/* this is derived from create_bbt() in nand_bbt.c */

/**
 * create_bbt - [GENERIC] Create a bad block table by scanning the device
 * @mtd:	MTD device structure
 * @buf:	temporary buffer
 * @bd:		descriptor for the good/bad block search pattern
 * @chip:	create the table for a specific chip, -1 read all chips.
 *		Applies only if NAND_BBT_PERCHIP option is set
 *
 * Create a bad block table by scanning the device
 * for the given good/bad block identify pattern
 */
static int create_big_mlc_bbt(struct mtd_info *mtd, uint8_t *buf,
	struct nand_bbt_descr *bd, int chip)
{
	struct nand_chip *this = mtd->priv;
	int i, numblocks, len, scanlen;
	int startblock;
	loff_t from;
	size_t readlen;

	printk(KERN_INFO "Scanning big mlc device for bad blocks\n");

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

	for (i = startblock; i < numblocks;) {
		int ret;

		ret = scan_block_faster(mtd, bd, from, buf);

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
		from += (1 << this->bbt_erase_shift);
	}
	return 0;
}

static int nand_scan_big_mlc_bbt(struct mtd_info *mtd, 
								 struct nand_bbt_descr *bd)
{
	struct nand_chip *this = mtd->priv;
	int len, res = 0;

	len = mtd->size >> (this->bbt_erase_shift + 2);
	/* Allocate memory (2bit per block) and clear the memory bad block table */
	this->bbt = kzalloc(len, GFP_KERNEL);
	if (!this->bbt) {
		printk(KERN_ERR "nand_scan_big_mlc_bbt: Out of memory\n");
		return -ENOMEM;
	}

	// this code is from inline nand_memory_bbt() in nand_bbt.c
	bd->options &= ~NAND_BBT_SCANEMPTY;
	res = create_big_mlc_bbt(mtd, this->buffers->databuf, bd, -1);
	if (res) {
		printk(KERN_ERR "nand_bbt: Can't create the RAM-based BBT\n");
		kfree(this->bbt);
		this->bbt = NULL;
	}
	return res;
}


/* This is derived from nand_default_bbt() in nand_bbt.c.
 * It is needed in order to have the bbt creator check both the first and last
 * pages of an eraseblock.  This is needed for the sub-50nm MLC NANDs from
 * Samsung and Hynix.
 */
extern int nand_create_badblock_pattern(struct nand_chip *this);
static int nand_big_mlc_bbt(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;

	/* Is a flash based bad block table requested ? */
	chip->bbt_td = NULL;
	chip->bbt_md = NULL;
	if (!chip->badblock_pattern)
		nand_create_badblock_pattern(chip);

	return nand_scan_big_mlc_bbt(mtd, chip->badblock_pattern);
}


/**
 * lf2000_nand_do_read_ops - Leapfrog replacement for nand_do_read_ops()
 *
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @ops:	oob ops structure
 *
 * Internal function. Called with chip held.
 *
 * Differences from nand_do_read_ops():
 *   code for profiling
 *   code for counting and timing read operations
 *   use of CONFIG_MTD_NAND_LF1000_MLC_SCRUB_THRESHOLD
 *   calls to printk() in order to report errors.
 */



static int lf2000_nand_do_read_ops(struct mtd_info *mtd, loff_t from,
			                       struct mtd_oob_ops *ops)
{
	int chipnr, page, realpage, col, bytes, aligned;
	struct nand_chip *chip = mtd->priv;
	struct mtd_ecc_stats stats;
	struct lf2000_nand_data * pnd;
	int blkcheck = (1 << (chip->phys_erase_shift - chip->page_shift)) - 1;
	int sndcmd = 1;
	int ret = 0;
	uint32_t readlen = ops->len;
	uint32_t oobreadlen = ops->ooblen;
	uint32_t max_oobsize = ops->mode == MTD_OPS_AUTO_OOB ?
		mtd->oobavail : mtd->oobsize;

	uint8_t *bufpoi, *oob, *buf;
	uint32_t numCorrected;
	int	 max_corrected = 0;
	int	 block;
	int	 num_retries;
	int	 max_retries;
	int	 uncorrectable_errors;

	uncorrectable_errors = 0;
	num_retries	     = 0;
	max_retries	     = 0;

	stats = mtd->ecc_stats;
	numCorrected = stats.corrected;

	chipnr = (int)(from >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	realpage = (int)(from >> chip->page_shift);
	page = realpage & chip->pagemask;

	col = (int)(from & (mtd->writesize - 1));

	buf = ops->datbuf;
	oob = ops->oobbuf;

	while(1) {
		bytes = min(mtd->writesize - col, readlen);
		aligned = (bytes == mtd->writesize);

		/* Is the current page in the buffer ? */
		if (realpage != chip->pagebuf || oob) {
			bufpoi = aligned ? buf : chip->buffers->databuf;

			block = page >> (chip->phys_erase_shift 
						- chip->page_shift); 

			if (likely(sndcmd)) {
				nand_stats_accum (NS_SEND_RD_CMD, 1, block);
				chip->cmdfunc(mtd, NAND_CMD_READ0, 0x00, page);
				nand_stats_accum(NS_SEND_RD_CMD, 0, block);
				sndcmd = 0;
			}
			/* Now read the page into the buffer */
			if (unlikely(ops->mode == MTD_OPS_RAW))
			{
				nand_stats_accum (NS_READ_RAW, 1, block);
				ret = chip->ecc.read_page_raw(mtd, chip, 
								bufpoi, page);
				nand_stats_accum (NS_READ_RAW, 0, block);
			}
			else if (!aligned && !oob) {
				nand_stats_accum (NS_READ_SUBPAGE, 1, block);
				ret = chip->ecc.read_subpage(mtd, chip, col,
							     bytes, bufpoi);
				nand_stats_accum (NS_READ_SUBPAGE, 0, block);
			}
			else {
				nand_stats_accum (NS_READ_PAGE, 1, block);
				ret = chip->ecc.read_page(mtd, chip, 
							  bufpoi, page);
#ifdef TIME_NAND_READ_ENTIRE
				update_nand_read_time_info();
#endif
				nand_stats_accum (NS_READ_PAGE, 0, block);
			}

			/* if bitflips were corrected, update the max number
			 * of bitflips per chunk, if appropriate.
			 */
            		if ( mtd->ecc_stats.corrected != numCorrected ) {
				if (ret > max_corrected)
					max_corrected = ret;
				ret	     = 0;
				UPDATE_BITFLIPS_PER_PAGE(
						mtd->ecc_stats.corrected 
						- numCorrected, block);
                		numCorrected = mtd->ecc_stats.corrected;

#ifndef NAND_ACCESS_TIMING	// we don't want to time serial output
				if (report_ecc_errors)
					printk(KERN_INFO "Corrected error while"
						" reading %d bytes at offset"
						" %d from page %d\n",
					   	bytes, col, realpage);
#endif
            		}
			/* if there was an uncorrectable ECC error, check
			 * if we've exhausted all retries.  If we haven't,
			 * try again to read the same section.
			 * If we have exhaused all retries, go on to the
			 * next section.
			 */
			if (mtd->ecc_stats.failed != stats.failed) {
				stats.failed = mtd->ecc_stats.failed;
				if (report_ecc_errors)
					printk(KERN_INFO "Error reading %d "
						"bytes from page %d\n", 
						 bytes, realpage);

				if (++num_retries < MAX_RETRIES) {
					sndcmd = 1;
					printk(KERN_INFO "after retry %d\n", 
						num_retries);
					continue;
				}
				else {
					printk(KERN_ERR "All retries failed"
						" when reading %d bytes"
						" from page %d\n",
						bytes, realpage);
					++uncorrectable_errors;
					/* flag the fact there are uncorrected
					 * errors in the buffer */
				}
            		}
			if (num_retries > max_retries)
				max_retries = num_retries;

			UPDATE_RETRIES_PER_PAGE( num_retries);
			if ((num_retries > 0) && (num_retries < MAX_RETRIES)) {
				if (report_ecc_errors)
					printk(KERN_INFO "Success on retry %d\n",
						num_retries);
			}
			num_retries = 0;

			/* Transfer not aligned data */
			if (!aligned) {
				memcpy(buf, chip->buffers->databuf + col, bytes);
				/* set chip->pagebuf to indicate 'no page' */
				/* TODO: FIXME: Is this the way to do it?  */
				chip->pagebuf = 0xFFFFFFFF;
			}
			buf += bytes;

			if (unlikely(oob)) {

				int toread = min(oobreadlen, max_oobsize);

				if (toread) {
					oob = nand_transfer_oob(chip, oob, 
								ops, toread);
					oobreadlen -= toread;
				}
			}

			if (!(chip->options & NAND_NO_READRDY)) {
				/*
				 * Apply delay or wait for ready/busy pin. Do
				 * this before the AUTOINCR check, so no
				 * problems arise if a chip which does auto
				 * increment is marked as NOAUTOINCR by the
				 * board driver.
				 */
				if (!chip->dev_ready)
					udelay(chip->chip_delay);
				else
					nand_wait_ready(mtd);
			}
		} else {
			memcpy(buf, chip->buffers->databuf + col, bytes);
			buf += bytes;
		}

		readlen -= bytes;

		if (!readlen)
			break;

		/* For subsequent reads align to page boundary. */
		col = 0;
		/* Increment page address */
		realpage++;

		page = realpage & chip->pagemask;
		/* Check, if we cross a chip boundary */
		if (!page) {
			chipnr++;
			chip->select_chip(mtd, -1);
			chip->select_chip(mtd, chipnr);
		}

		/* Check, if the chip supports auto page increment
		 * or if we have hit a block boundary.
		 */
		if (!NAND_CANAUTOINCR(chip) || !(page & blkcheck))
			sndcmd = 1;
	}

	ops->retlen = ops->len - (size_t) readlen;
	if (oob)
		ops->oobretlen = ops->ooblen - oobreadlen;

	if (ret)
		return ret;

	if (uncorrectable_errors) {
		printk(KERN_INFO "lf2000_nand_do_read_ops() returns -EBADMSG\n");
		return -EBADMSG;
	}

	if (max_retries > 0) {
		if (report_ecc_errors)
	        	printk(KERN_INFO "nand_do_read_ops: ECC error: from "
	                         "%llx; realpage %x; page %x; readlen %d\n",	
	                	from, realpage, page, ops->len);

		/* We reach here after successful retry following
		 * uncorrectable errors.  Return -EUCLEAN, which
		 * will cause the block to be scheduled for scrubbing      
		 */
		return -EUCLEAN;
    	}

	/* chip->priv is the address of a struct lf2000_nand_data which
	 * contains a pointer to a const struct tag_bch_info whose 't'
	 * member is the max number of correctable errors.
 	 * Return -EUCLEAN if the max number of bitflips (per ECC chunk)
	 * corrected is > 't' - 2.
	 *
	 * If the pointer to a tag_bch_info is NULL (1-bit ECC is used for
	 * the NAND), return -EUCLEAN if any bitflip was found
	 *
	 * NOTE: this routine will be called for only whose NANDs on which we're
	 *	 using hw ecc, so we don't expect to find a NULL pointer
	 *	 to atag_bch_info.
	 * NOTE: the value we ought to use for 'limit' is subject to change
	 *	 after we know more about the pattern of degradation in 
	 * the NANDs we use.
	 */
	pnd = (struct lf2000_nand_data *)chip->priv;
	if ((pnd != NULL) && (pnd->bch_info != NULL)) {
		int limit;

	/* To reduce the chance of jumping to uncorrectable errors,      */
	/* use a lower threshold.  We've seen jumps of 5, so we'll say   */
	/* if the number of bitflips is > 18 (>= 19), return a value     */
	/* that will cause scrubbing.					 */
		limit = pnd->bch_info->bch_var_t - 5;

		if (max_corrected >= limit) 
		{
			if (report_ecc_errors)
				printk(KERN_INFO "lf2000_nand_do_rd_ops: " 						"max_corrected: %d; limit %d\n",
					 max_corrected, limit);
			return -EUCLEAN;
		} else return 0;
	}
	else {
		return  mtd->ecc_stats.corrected - stats.corrected ? -EUCLEAN : 0;
	}
}


/**
 * lf2000_nand_read - Leapfrog's replacement for nand_read()
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @len:	number of bytes to read
 * @retlen:	pointer to variable to store the number of read bytes
 * @buf:	the databuffer to put data
 *
 * Get hold of the chip and call nand_do_read
 *
 * Leapfrog has a special version of this function in order to call
 * lf2000_nand_do_read_ops() instead of the standard nand_do_read_ops().
 * Except for that call, this function is identical to nand_do_read_ops().
 */
int lf2000_nand_read(struct mtd_info *mtd, loff_t from, size_t len,
		                    size_t *retlen, uint8_t *buf)
{
	struct nand_chip *chip = mtd->priv;
	struct mtd_oob_ops ops;
	int ret;

	/* Do not allow reads past end of device */
	if ((from + len) > mtd->size)
		return -EINVAL;
	if (!len)
		return 0;

	nand_get_device(chip, mtd, FL_READING);

	ops.len    = len;
	ops.datbuf = buf;
	ops.oobbuf = NULL;
	ops.mode   = MTD_OPS_PLACE_OOB;

	ret = lf2000_nand_do_read_ops(mtd, from, &ops);

	*retlen = ops.retlen;

	nand_release_device(mtd);

	return ret;
}

/**
 * lf2000_nand_read_oob - Leapfrog replacement for nand_read_oob().
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @ops:	oob operation description structure
 *
 * NAND read data and/or out-of-band data
 *
 * This function differs from nand_read_oob() only by calling 
 * lf2000_nand_do_read_ops() instead of nand_do_read_ops().
 */
int lf2000_nand_read_oob(struct mtd_info *mtd, loff_t from,
		            	 struct mtd_oob_ops *ops)
{
	struct nand_chip *chip = mtd->priv;
	int ret = -ENOTSUPP;

	ops->retlen = 0;


	/* Do not allow reads past end of device */
	if (ops->datbuf && (from + ops->len) > mtd->size) {
		pr_debug("nand_read_oob: "
		      "Attempt read beyond end of device\n");
		return -EINVAL;
	}
	nand_get_device(chip, mtd, FL_READING);

	switch(ops->mode) {
	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_AUTO_OOB:
	case MTD_OPS_RAW:
		break;
	default:
		goto out;
	}

	if (!ops->datbuf) {
		ret = nand_do_read_oob(mtd, from, ops);
	}
	else {
		ret = lf2000_nand_do_read_ops(mtd, from, ops);
	}
 out:
	nand_release_device(mtd);
	return ret;
}



/**
 * lf2000_nand_write_page - write one page
 * Leapfrog's replacement for nand_write_page()
 * @mtd:	MTD device structure
 * @chip:	NAND chip descriptor
 * @buf:	the data to write
 * @page:	page number to write
 * @cached:	cached programming
 * @raw:	use _raw version of write_page
 */
#if 0	/* 3oct13  For test/debug only */
	/* This version changes the selected NAND when called to write to 
	 * page 1.
	 * This code was implemented to demonstrate that the bug in the
	 * cartridge NAND access initialization code could cause the NAND
	 * to contain an erased page followed by a written page.
	 */
static int lf2000_nand_write_page(struct mtd_info *mtd, struct nand_chip *chip,
			   const uint8_t *buf, int page, int cached, int raw)
{
	int status;
#if 1	/* 3oct13 */
	int	pageZero;
	int	originalBank;
#endif	/* 3oct13 */
	int block;
	block = page >> (chip->phys_erase_shift - chip->page_shift); 
	NAND_STATS_ACCUM (NS_WRITE, 1, block);
	chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page);

#if 1	/* 3oct13 */
	/* See page 0 unwritten when this is enabled 
	pageZero = (page == 0);
	 */
	pageZero = (page == 1);
	if (pageZero) {
		originalBank = NX_MCUS_GetNFBank();
		NX_MCUS_SetNFBank(LF2000_CART_NAND_BANK);
	}
#endif	/* 3oct13 */
	if (unlikely(raw))
		chip->ecc.write_page_raw(mtd, chip, buf);
	else
		chip->ecc.write_page(mtd, chip, buf);

	/*
	 * Cached programming disabled for now, Not sure if its worth the
	 * trouble. The speed gain is not very impressive. (2.3->2.6Mib/s)
	 */
	cached = 0;

	if (!cached || !(chip->options & NAND_CACHEPRG)) {

		chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);
		status = chip->waitfunc(mtd, chip);
		NAND_STATS_ACCUM (NS_WRITE, 0, block);
#if 1	/* 3oct13 */
		if (pageZero) {
			NX_MCUS_SetNFBank(originalBank);
			printk(	KERN_ERR 
				"lf2000_nand_write_page(0): status 0x%x\n",
				status);
		}
#endif	/* 3oct13 */
		/*
		 * See if operation failed and additional status checks are
		 * available
		 */
		if ((status & NAND_STATUS_FAIL) && (chip->errstat)) 
		{
			status = chip->errstat(mtd, chip, FL_WRITING, status,
					       page);
			printk(KERN_ERR "lf2000_nand_write_page(): after call "
				"to chip->errstat(): status 0x%x\n", status);
		}

		if (status & NAND_STATUS_FAIL)
		{
			printk(KERN_ERR "lf2000_nand_write_page() returns -EIO\n");
			return -EIO;
		}
	} else {
		chip->cmdfunc(mtd, NAND_CMD_CACHEDPROG, -1, -1);
		status = chip->waitfunc(mtd, chip);
		NAND_STATS_ACCUM (NS_WRITE, 0, block);
	}

#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	/* Send command to read back the data */
	chip->cmdfunc(mtd, NAND_CMD_READ0, 0, page);

	if (chip->verify_buf(mtd, buf, mtd->writesize))
		return -EIO;
#endif
	return 0;
}


#else
static int lf2000_nand_write_page(struct mtd_info *mtd, struct nand_chip *chip,
			   const uint8_t *buf, int page, int cached, int raw)
{
	int status;

	int block;
	block = page >> (chip->phys_erase_shift - chip->page_shift); 
	NAND_STATS_ACCUM (NS_WRITE, 1, block);
	chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page);

	if (unlikely(raw))
		chip->ecc.write_page_raw(mtd, chip, buf);
	else
		chip->ecc.write_page(mtd, chip, buf);

	/*
	 * Cached programming disabled for now, Not sure if its worth the
	 * trouble. The speed gain is not very impressive. (2.3->2.6Mib/s)
	 */
	cached = 0;

	if (!cached || !(chip->options & NAND_CACHEPRG)) {

		chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);
		status = chip->waitfunc(mtd, chip);
		NAND_STATS_ACCUM (NS_WRITE, 0, block);
		/*
		 * See if operation failed and additional status checks are
		 * available
		 */
		if ((status & NAND_STATUS_FAIL) && (chip->errstat)) 
		{
			status = chip->errstat(mtd, chip, FL_WRITING, status,
					       page);
			printk(KERN_ERR "lf2000_nand_write_page(): after call "
				"to chip->errstat(): status 0x%x\n", status);
		}

		if (status & NAND_STATUS_FAIL)
		{
			printk(KERN_ERR "lf2000_nand_write_page() returns -EIO\n");
			return -EIO;
		}
	} else {
		chip->cmdfunc(mtd, NAND_CMD_CACHEDPROG, -1, -1);
		status = chip->waitfunc(mtd, chip);
		NAND_STATS_ACCUM (NS_WRITE, 0, block);
	}

#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	/* Send command to read back the data */
	chip->cmdfunc(mtd, NAND_CMD_READ0, 0, page);

	if (chip->verify_buf(mtd, buf, mtd->writesize))
		return -EIO;
#endif
	return 0;
}
#endif	/* end of the "real" version */



/* This routine examines the nand's ID bytes and extracts from them and saves
 * lots of information about the nand's geometry and the amount of error
 * correction that it needs.
 */
static int lf2000_nand_init_size(struct mtd_info *mtd, 
								 struct nand_chip *chip,
								 u8 *id_data)
{
	int extid;
	int busw;

	printk(KERN_INFO "%s: id bytes: "
				 "%02x %02x %02x %02x %02x %02x %02x %02x\n",
		__FUNCTION__,
		id_data[0], id_data[1], id_data[2], id_data[3],
		id_data[4], id_data[5], id_data[6], id_data[7]);

		/* The 3rd id byte holds MLC / multichip data */
	chip->cellinfo = id_data[2];
		/* The 4th id byte has sizes of page, oob, and eraseblock */
	extid = id_data[3];

		/* Leapfrog uses at least 2 NAND devices that emit 6 ID bytes
		 * which have non-traditional field definitions.  
		 * One of these is a Samsung K9GAG08U0E.
 		 * The other is a Hynix H27UAG8T2B.
		 *
		 * We first check if the 3rd ID byte indicates that a cell contains
		 * more than 1 bit (2 levels), if the 8 ID bytes in id_data[] start to
		 * repeat after 6 bytes (i.e., if id_data[0] == id_data[6] and
		 * id_data[1] == id_data[7]).
		 * If not, or if the manufacturer is not Samsung or Hynix, we interpret
		 * the ID bytes in the traditional way.
		 * If they do, we interpret a Samsung nand's fields one way and Hynix
		 * another way.
		 */
	if (   (chip->cellinfo & NAND_CI_CELLTYPE_MSK) 
		&& (id_data[5] != 0x00) 
		&& ((id_data[0] == id_data[6]) && (id_data[1] == id_data[7]))
		&& (   (id_data[0] == NAND_MFR_SAMSUNG)
			|| (id_data[0] == NAND_MFR_HYNIX)) )
	{
		int ecc_level;

			/* no subpage writes on these MLC NANDs */
		chip->options	   |= NAND_NO_SUBPAGE_WRITE;
		chip->block_bad		= nand_block_bad_first_or_last;
		chip->block_markbad	= nand_block_markbad_first_and_last;
		chip->scan_bbt		= nand_big_mlc_bbt;

		/* The 5th ecc byte indicates required ECC level */
		ecc_level = (id_data[4] >> 4) & 0x07;

			/* Calc pagesize */
		mtd->writesize = 2048 << (extid & 0x03);
		if (id_data[0] == NAND_MFR_SAMSUNG)
		{
			/* Calc oobsize */
			/* NOTE: oobsize is indicated by bits 6, 3, and 2 
			 * of the 4th ID byte:
			 * bits 7654.3210   HEX	  Description
			 *		x0xx.00xx  0x00 : reserved
			 *		x0xx.01xx  0x04 : 128 bytes
			 *		x0xx.10xx  0x08 : 218 bytes
			 *		x0xx.11xx  0x0C : 400 bytes
			 *		x1xx.00xx  0x40 : 436 bytes
			 *		x1xx.01xx  0x44 : 512 bytes
			 *		x1xx.10xx  0x48 : 640 bytes
			 *		x1xx.11xx  0x4C : reserved
			 *
			 * clear the unused bits, leaving only 6, 3, and 2
			 */
			switch (extid & 0x4C) {
			case 0x04:	mtd->oobsize = 128;	break;
			case 0x08: 	mtd->oobsize = 218;	break;
			case 0x0C:	mtd->oobsize = 400;	break;
			case 0x40:	mtd->oobsize = 436;	break;
			case 0x44:	mtd->oobsize = 512;	break;
			case 0x48:	mtd->oobsize = 640;	break;
			case 0x00:	/* reserved */
			case 0x4C:	/* reserved */
			default:	mtd->oobsize = 0;	break;
			}
				/* Calc blocksize 
				 * NOTE: blocksize is indicated by bits 7, 5, and 4
				 * of the 4th ID byte:
				 *				000: 128K
				 *				001: 256K
				 *				010: 512K
				 *				011: 1M
				 * 100,101,110, 111: reserved
				 * This code treats all the reserved values as though their
				 * msb is 0.  Not exactly right, but what else to do?
				 */
			mtd->erasesize = (128 * 1024) << ((extid >> 4) & 0x03);
				/* Get the required ecc strength */
			switch (ecc_level) {
			case 0:		// 1 bit / 512 bytes
// TODO: FIXME: do we need to have a nand_sw_ecc_init(chip) routine
//				to init all the function pointers and other fields of chip?
				chip->ecc.mode = NAND_ECC_SOFT;
				printk(KERN_INFO "NAND ecc: Software \n");
				break;
			case 1:		// 2 bits / 512 bytes
				nand_hw_ecc_init(mtd, 4);
				break;
			case 2:		// 4 bits / 512 bytes
				nand_hw_ecc_init(mtd, 4);
				break;
			default:	
				// TODO: FIXME: decide what to do for default case
				// 8-bit ecc uses 13 ecc bytes per 512 bytes of data.
				// If the spare area has at least 16 bytes per 512
				// bytes of data, the ecc bytes will fit in the oob.
			case 3:		// 8 bits / 512 bytes
				nand_hw_ecc_init(mtd, 8);
				break;
			case 4:		// 16 bits / 512 bytes
				nand_hw_ecc_init(mtd, 16);
				break;
			case 5:		// 24 bits / 1024 bytes
				nand_hw_ecc_init(mtd, 24);
				break;
			}
		}
		else {	/* Hynix */
			/* Calc oobsize */
			/* NOTE: oobsize is indicated by bits 6, 3, and 2 
			 * of the 4th ID byte:
			 * (see the Hynix H27UAG8T2B datasheet, p20)
			 * bits 7654.3210   HEX	  Description
			 *		x0xx.00xx  0x00 : 128 bytes
			 *		x0xx.01xx  0x04 : 224 bytes
			 *		x0xx.10xx  0x08 : 448 bytes
			 *		x0xx.11xx  0x0C : reserved
			 *		x1xx.00xx  0x40 : reserved
			 *		x1xx.01xx  0x44 : reserved
			 *		x1xx.10xx  0x48 : reserved
			 *		x1xx.11xx  0x4C : reserved
			 *
			 * clear the unused bits, leaving only 6, 3, and 2
			 */
			switch (extid & 0x4C) {
			case 0x00:	mtd->oobsize = 128;	break;
			case 0x04:	mtd->oobsize = 224;	break;
			case 0x08: 	mtd->oobsize = 448;	break;
			case 0x0C:	mtd->oobsize = 64;	break;
			case 0x40:	mtd->oobsize = 32;	break;
			case 0x44:	mtd->oobsize = 16;	break;
			case 0x48:	mtd->oobsize = 640;	break;
			case 0x4C:	/* reserved */
			default:	mtd->oobsize = 0;	break;
			}
				/* Calc blocksize */
			/* Mask out all bits except 7, 5, and 4 */
			switch (extid & 0xB0) {
			case 0x00: mtd->erasesize =  128 * 1024; break;
			case 0x10: mtd->erasesize =  256 * 1024; break;
			case 0x20: mtd->erasesize =  512 * 1024; break;
			case 0x30: mtd->erasesize =  768 * 1024; break;
			case 0x80: mtd->erasesize = 1024 * 1024; break;
			case 0x90: mtd->erasesize = 2048 * 1024; break;
			case 0xA0: /* reserved */
			case 0xB0: /* reserved */
				   mtd->erasesize = 0;	break;
			}
				/* Get the required ecc strength */
			switch (ecc_level) {
			case 0:		// 1 bit / 512 bytes
				chip->ecc.mode = NAND_ECC_SOFT;
				printk(KERN_INFO "NAND ecc: Software \n");
				break;
			case 1:		// 2 bits / 512 bytes
				nand_hw_ecc_init(mtd, 4);
				break;
			case 2:		// 4 bits / 512 bytes
				nand_hw_ecc_init(mtd, 4);
				break;
			case 3:		// 8 bits / 512 bytes
				nand_hw_ecc_init(mtd, 8);
				break;
			case 4:		// 16 bits / 512 bytes
				nand_hw_ecc_init(mtd, 16);
				break;
			case 5:		// 24 bits / 2048 bytes
			case 6:		// 24 bits / 1024 bytes
			default:	/* reserved; default to 24 bits / 1KB */
				nand_hw_ecc_init(mtd, 24);
				printk(KERN_INFO "NAND ecc: 24-bit HW\n");
				break;
			}
		}
		busw = 0;
	} 
	// NOTE: If we need to deal with other types of non-traditional NANDs,
	// we can insert code here to check for them and to deal with them.

	else {	/* Sometimes we read invalid ID bytes; usually the first one 
		 * is not a recognized manufacturer code.  Do the processing
		 * that's in nand_get_flash_type() only if the first ID byte
		 * is a recognzied mfr code.  This might be wrong sometimes,
		 * but is will catch many of the errors.
		 */
		switch (id_data[0]) {
		case NAND_MFR_TOSHIBA:
		case NAND_MFR_SAMSUNG:
		case NAND_MFR_FUJITSU:
		case NAND_MFR_NATIONAL:
		case NAND_MFR_RENESAS:
		case NAND_MFR_STMICRO:
		case NAND_MFR_HYNIX:
		case NAND_MFR_MICRON:
		case NAND_MFR_AMD:
			/* This processing is identical to code in 
			 * nand_get_flash_type().
			 * First calc pagesize */
			mtd->writesize = 1024 << (extid & 0x03);
			extid >>= 2;
				/* Calc oobsize */
			mtd->oobsize = (8 << (extid & 0x01)) * (mtd->writesize >> 9);
			extid >>= 2;
				/* Calc blocksize (multiples of 64KiB) */
			mtd->erasesize = (64 * 1024) << (extid & 0x03);
			extid >>= 2;
				/* Get buswidth information */
			busw = (extid & 0x01) ? NAND_BUSWIDTH_16 : 0;
			break;
		default:	/* Force an error for unexpected type of NAND */
			printk(KERN_INFO "Non-Samsung, non-Hynix, non-ONFI unit\n");
			busw = -1; /* indicate an error */
			break;
		}
	}
	return busw;
}


static void init_lf2000_nand_controller(void)
{
	NX_MCUS_SetBaseAddress((U32)IO_ADDRESS(NX_MCUS_GetPhysicalAddress()));
	NX_MCUS_SetAutoResetEnable(CTRUE);

	NX_MCUS_ClearInterruptPending(0);
	NX_MCUS_SetInterruptEnableAll(CFALSE);

	NX_MCUS_SetNFBank(0);
	NX_MCUS_SetNFCSEnable(CFALSE);
}

/* TODO: NOTE: LinuxDist-Emerald's nand driver had a special lf1000_nand_wait()
 *		routine that differed from the standard nand_wait() routine
 *	by waiting a bit longer before checking if the nand controller is ready.
 *	If we encounter problems with NAND i/o, we could try using a similar
 *	routine here.
 *	LD-E's nand driver also has a special lf1000_nand_write_page() routine.
 *	It has this comment:
 * We use a special version that checks for all FF before trying to write
 * to the NAND.  This is necessary when using UBIFS on MLC NAND.
 *	
 *	When we start using ubifs, we too might need a special version.
 */
	/* TODO: FIXME:	figure out if we need (a la lf1000) a lf2000-specific
	 *				function for chip->waitfunc().
	 */
static int init_lf2000_nand(struct nand_chip *chip)
{
	int ret = 0;
	printk(KERN_INFO "%s: 4 \n", __func__);

	/* insert callbacks */
	chip->IO_ADDR_R 	= (void __iomem *)__PB_IO_MAP_NAND_VIRT;
	chip->IO_ADDR_W 	= (void __iomem *)__PB_IO_MAP_NAND_VIRT;
	chip->options		= 0; /* 8 bit bus width */
	chip->cmd_ctrl 		= nand_dev_ctrl;
	chip->dev_ready 	= nand_dev_ready;
	chip->chip_delay 	= 15;
	chip->read_buf 		= nand_dev_read_buf;
	chip->write_buf 	= nand_dev_write_buf;
	chip->write_page	= lf2000_nand_write_page;
	chip->init_size		= lf2000_nand_init_size;
	chip->ecc.mode		= NAND_ECC_SOFT; /* default for small blk nand*/

	return ret;
}

static int init_lf2000_onboard_nand(struct nand_chip *chip)
{
	struct lf2000_nand_data * pnd;
	printk(KERN_INFO "%s: 2 \n", __func__);
	
	pnd = (struct lf2000_nand_data *)chip->priv;
	pnd->nand_bank = LF2000_BASE_NAND_BANK;
	/* NOTE: there's little need to add a check that pnd is non-null; 
	 * 	 inspection of this routine's caller indicates that the
	 *	 routine will not be called if chip->priv is NULL.
	 */
	switch(system_rev) {
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_LUCY_CIP:
		case LF2000_BOARD_VALENCIA:
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
		case LF2000_BOARD_VALENCIA_CIP:
			chip->select_chip = lf_nand_select;
			break;
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
			chip->select_chip = onboard_nand_select;
			break;
		default: 
			printk(KERN_ERR "%s GPIO mapping does not support " \
				"Board Revision = 0x%X\n",
				__func__, system_rev);
	}

	return init_lf2000_nand(chip);
}

static int init_lf2000_cart_nand(struct nand_chip *chip)
{
	struct lf2000_nand_data * pnd;
	printk(KERN_INFO "%s: 2 \n", __func__);
	
	pnd = (struct lf2000_nand_data *)chip->priv;
	pnd->nand_bank = LF2000_CART_NAND_BANK;
	/* NOTE: there's little need to add a check that pnd is non-null; 
	 * 	 inspection of this routine's caller indicates that the
	 *	 routine will not be called if chip->priv is NULL.
	 */
	switch(system_rev) {
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_LUCY_CIP:
		case LF2000_BOARD_VALENCIA:
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
		case LF2000_BOARD_VALENCIA_CIP:
			chip->select_chip = lf_nand_select;
			break;
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
			chip->select_chip = cart_nand_select;
			break;
		default:
			printk(KERN_ERR "%s GPIO mapping does not support " \
				"Board Revision = 0x%X\n",
				__func__, system_rev);
	}

	return init_lf2000_nand(chip);
}

static int nand_dev_check(struct mtd_info *mtd)
{
	int ret = 0;
#if defined (CONFIG_MTD_NAND_NEXELL_HWECC)
	ret = nand_hw_ecc_check(mtd);
#endif
	return ret;
}

#if !defined (CONFIG_SYS_NAND_MAX_CHIPS)
#define CONFIG_SYS_NAND_MAX_CHIPS   1
#endif


#ifdef CONFIG_MTD_NAND_LF2000_PROF
/**
 * single_erase_cmd - [GENERIC] NAND standard block erase command function
 * @mtd:	MTD device structure
 * @page:	the page address of the block which will be erased
 *
 * Identical to single_erase_cmd, except for profiling support and the call
 * to chip->waitfunc().
 */
static void lf2000_single_erase_cmd(struct mtd_info *mtd, int page)
{
	struct nand_chip *chip = mtd->priv;

	/* Send commands to erase a block */
	chip->cmdfunc(mtd, NAND_CMD_ERASE1, -1, page);
	chip->cmdfunc(mtd, NAND_CMD_ERASE2, -1, -1);
}
#endif	// CONFIG_MTD_NAND_LF2000_PROF


/*
 * Configure the LF2000's nand controller and probe for the onboard NAND device.
 */
static int __devinit lf2000_nand_probe(struct platform_device *pdev)
{
// 16aug11	TODO: Try re-enabling all this pdata stuff and check if the driver
//				  still works ok.

#if 0	// 14jul11	Eliminating pdata (and not accessing 
		//			pdev->dev.platform_data) might make the initialized data
		// structs in arch/arm/mach-nxp3200/device.c a bit simpler.
	struct platform_nand_data *pdata;
#endif	// 14jul11
	struct lf2000_nand_data *data;
	struct nand_chip * chip;
	struct mtd_info  * mtd;
	struct resource  * res;
	int ret      = 0;
	int parts_nr = 0;
	int cmd_part = 1;

#if 0	// 14jul11
	if (NULL == (void *)pdev->dev.platform_data) {
		dev_err(&pdev->dev, "pdev->dev.platform_data is NULL\n");
		return -EINVAL;
	}
	pdata = pdev->dev.platform_data;
	if (pdata->chip.nr_chips < 1) {
		dev_err(&pdev->dev, "invalid number of chips specified\n");
		return -EINVAL;
	}
#endif	// 14jul11

	printk(KERN_INFO "%s: 1 \n", __func__);
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "failed to get resource!\n");
		return -ENXIO;
	}
	if (!request_mem_region(res->start, resource_size(res),
							dev_name(&pdev->dev))) {
		dev_err(&pdev->dev, "request_mem_region failed\n");
		return -EBUSY;
	}
	lf2000_nand.mem	= ioremap(res->start, resource_size(res));
	if(lf2000_nand.mem == NULL) {
		dev_err(&pdev->dev, "failed to ioremap\n");
		ret = -ENOMEM;
		goto fail_remap;
	}
	/* initialize the controller structure */
	spin_lock_init(&lf2000_nand.controller.lock);
	init_waitqueue_head(&lf2000_nand.controller.wq);

	/* Allocate memory for the common buffers (and zero it) */
	lf2000_nand.L  = kzalloc(COMMON_BUFFER_SIZE, GFP_KERNEL);
	if (!lf2000_nand.L) {
		dev_err(&pdev->dev, "failed to allocate common buffers.\n");
		ret = -ENOMEM;
		goto fail_alloc_buffers;
	}

	/* Allocate memory for the onboard nand data struct (and zero it) */
	data = kzalloc( sizeof(struct lf2000_nand_data), GFP_KERNEL);
	if (!data) {
		dev_err(&pdev->dev, "failed to allocate onboard device structure.\n");
		ret = -ENOMEM;
		goto fail_alloc_onboard;
	}


	// TODO: FIXME: figure out how to initialize data->nr_parts and
	//				data->parts.

	lf2000_nand.pdev	 = pdev;
	lf2000_nand.onboard_nand = data;
	lf2000_nand.cart_ready   = 0;
	lf2000_nand.cart_ubi	 = 0;

	lf2000_nand.u_L		 = lf2000_nand.L   + L_ENTRIES;
	lf2000_nand.reg		 = lf2000_nand.u_L + uL_ENTRIES;
	lf2000_nand.s		 = lf2000_nand.reg + reg_ENTRIES;
	lf2000_nand.elp		 = lf2000_nand.s   + s_ENTRIES;
	lf2000_nand.desc	 = lf2000_nand.elp + elp_ENTRIES;

	sema_init(&lf2000_nand.sem_hotswap, 1);

	lf2000_nand.onboard_nand->io_base = lf2000_nand.mem;
	chip = &lf2000_nand.onboard_nand->chip;
	mtd  = &lf2000_nand.onboard_nand->mtd;

	chip->priv = lf2000_nand.onboard_nand;	
	mtd->priv  = chip;
	mtd->owner = THIS_MODULE;
	mtd->name  = dev_name(&pdev->dev);

	// TODO: FIXME: figure out if we need to configure a gpio pin
	//				to serve as the "cartridge-inserted" pin
	/* Here's the code from LinuxDist-Emerald:
	gpio_configure_pin(NAND_CART_DETECT_PORT, NAND_CART_DETECT_PIN,
			   GPIO_GPIOFN, 0, 1, 0);
	 */

	chip->controller = &lf2000_nand.controller;
#if 0	// 14jul11	For now, pdata->chip.options is zero
	chip->options	|= pdata->chip.options;
#endif	// 14jul11
#if 0	// 13jul11  Maybe this isn't needed for our purposes?
	chip->ecc.hwctl	 = pdata->ctrl.hwcontrol;	// TODO: check/fix this
#endif	// 13jul11

#if (0)
	printk(KERN_INFO "System Reset Config: 0x%08x\n", 
		NX_CLKPWR_GetSystemResetConfiguration());
#endif


	init_lf2000_nand_controller();
	init_lf2000_onboard_nand(chip);

	// TODO: FIXME: shall we replace nand_scan() with lf2000_nand_scan()
	//				similar to LinuxDist-Emerald's lf1000_nand_scan()?

	/* Scan to find existance of the device */
#if 1	// 14jul11
	if (nand_scan(mtd, 1)) 
#else
	if (nand_scan(mtd, pdata->chip.nr_chips)) 
#endif	// 14jul11
	{
		ret = -ENXIO;
		goto out;
	}
printk(KERN_INFO "onboard chip options: 0x%08x\n", chip->options);

	if (chip->ecc.mode == NAND_ECC_HW) {
		mtd->_read     = lf2000_nand_read;
		mtd->_read_oob = lf2000_nand_read_oob;
	}

#ifdef CONFIG_MTD_NAND_LF2000_PROF
	chip->erase_cmd = lf2000_single_erase_cmd;
#endif // CONFIG_MTD_NAND_LF2000_PROF

	if (0 != (ret = nand_dev_check(mtd)))
		goto pre_out;

	// TODO: FIXME: Shall we store "lf2000-onboard" (or similar) in
	//				onboard's mtd_info.name field?

	parts_nr  = parse_mtd_partitions(mtd, part_probes, &data->parts, NULL);
	if (0 >= parts_nr) {
		cmd_part  = 0;
		if(get_leapfrog_platform() == RIO)
		{
			data->parts = partition_map_8G;
			parts_nr = ARRAY_SIZE(partition_map_8G);
			def_mtd_parts(&data->mtd, partition_map_8G);
		}
		else
		{
			data->parts = partition_map_2G4G;
			parts_nr  = ARRAY_SIZE(partition_map_2G4G);
			def_mtd_parts(&data->mtd, partition_map_2G4G);
		}
	}

	/* Register the partitions */
	mtd_device_register(&data->mtd, data->parts, parts_nr);

	printk(KERN_NOTICE "Using %s partition definition\n",
		cmd_part?"command line":"static");
	if (0 != sysfs_create_group(&pdev->dev.kobj, &nand_attr_group)) {
		dev_warn(&pdev->dev, "error creating sysfs group\n");
	}
#ifdef NAND_ACCESS_TIMING
	init_nand_access_times();
#endif

	if (!ret)
		return ret;
pre_out:
	nand_release(mtd);
out:
#if 0	// 14jul11
	if (pdata->ctrl.remove)
		pdata->ctrl.remove(pdev);
	platform_set_drvdata(pdev, NULL);
#endif	// 14jul11
	kfree(lf2000_nand.onboard_nand);
fail_alloc_onboard:
	kfree(lf2000_nand.L);
fail_alloc_buffers:
	iounmap(lf2000_nand.mem);
fail_remap:
	release_mem_region(res->start, (res->end - res->start) + 1);
	return ret;
}


/*
 * Remove a NAND device.
 */
static int __devexit lf2000_nand_remove(struct platform_device *pdev)
{
#if 0	// 12jul11  disabled for now because not referenced
	struct lf2000_nand_data *data = platform_get_drvdata(pdev);
#endif
	struct platform_nand_data *pdata = pdev->dev.platform_data;
	struct resource *res;

	if (lf2000_nand.onboard_nand) {
		nand_release(&lf2000_nand.onboard_nand->mtd);
		if (lf2000_nand.onboard_nand->parts 
			&& lf2000_nand.onboard_nand->parts != pdata->chip.partitions)
			kfree(lf2000_nand.onboard_nand->parts);
		kfree(lf2000_nand.onboard_nand);
		lf2000_nand.onboard_nand = NULL;
	}
	if (lf2000_nand.cart_nand) {
		nand_release(&lf2000_nand.cart_nand->mtd);
		if (lf2000_nand.cart_nand->parts 
			&& lf2000_nand.cart_nand->parts != pdata->chip.partitions)
			kfree(lf2000_nand.cart_nand->parts);
		kfree(lf2000_nand.cart_nand);
		lf2000_nand.cart_nand = NULL;
	}
	if (lf2000_nand.L) {
		kfree(lf2000_nand.L);
		lf2000_nand.L = NULL;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	sysfs_remove_group(&pdev->dev.kobj, &nand_attr_group);

	if (pdata->ctrl.remove)
		pdata->ctrl.remove(pdev);
	if (lf2000_nand.mem != NULL)
		iounmap(lf2000_nand.mem);

	release_mem_region(res->start, resource_size(res));

	return 0;
}


static struct platform_driver lf2000_nand_driver = {
	.probe		= lf2000_nand_probe,
	.remove		= __devexit_p(lf2000_nand_remove),
	.driver		= {
		.name	= "lf2000-nand",
		.owner	= THIS_MODULE,
	},
};

static int __init lf2000_nand_init(void)
{
	return platform_driver_register(&lf2000_nand_driver);
}

static void __exit lf2000_nand_exit(void)
{
	platform_driver_unregister(&lf2000_nand_driver);
}


module_init(lf2000_nand_init);
module_exit(lf2000_nand_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("LeapfrogFirmware <firmware@leapfrog.com>");
MODULE_DESCRIPTION("MTD nand driver for the LF2000");	


