/*
 *  drivers/mtd/nand/nexell/prof.h
 *
 * Copyright 2011 LeapFrog Enterprises Inc.
 *
 * Robert Dowling <rdowling@Leapfrog.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef PROF_H
#define PROF_H

/* Nand profiling support */

/* Define the types of nand operations we can accumulate data on */
enum prof_type { 
	NS_READ_BUF, 
	NS_READ_SUBPAGE,
	NS_READ_PAGE,
	NS_READ_RAW,
	NS_SEND_RD_CMD,
	NS_WRITE,
	NS_ERASE,
	NS_ERASE2,
	NS_LOCK,
	NS_MAX 
};

/* The collector function */
extern void nand_stats_accum (enum prof_type type, int in, int block);
void update_bitflips(int incr);
void update_bitflips_per_page(int errors_per_page, int block);


/* Enable the following #define if you want to collect nand access timing data*/
#define NAND_ACCESS_TIMING 1

#ifdef NAND_ACCESS_TIMING


/* If you want to collect timing data for reads from nand, enable the
 * following #define, which enables timing in lf2000_nand_do_read_ops()
 */
#define TIME_NAND_READ_ENTIRE  1 

/* If you want to collect timing data for writes to nand, enable one of the
 * next two #defines.
 *
 * If TIME_NAND_WRITE_PARTS is defined, timing is collected separately for
 * calculation of ECC and for actual write to nand.
 * If TIME_NAND_WRITE_ENTIRE is defined, timing is collected for the entire
 * calculate/write process.
 * If both symbols are defined, the reported number of writes will increase by 2
 * and the reported total times will not be correct.
 */
//#define TIME_NAND_WRITE_PARTS   1
#define TIME_NAND_WRITE_ENTIRE  1

void start_ecc_timer(void);
void stop_ecc_timer(int badbits);

#endif	// NAND_ACCESS_TIMING


#endif	// PROF_H

