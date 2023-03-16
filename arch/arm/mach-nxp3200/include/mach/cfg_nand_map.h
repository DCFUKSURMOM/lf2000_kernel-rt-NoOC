/*
 * arch/arm/plat-nxp3200/e2k/include/cfg_nand_map.h
 * Copyright 2007-2010 LeapFrog Enterprises Inc.
 *
 * Scott Esters <sesters@leapfrog.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

/* 2MB erase block size check, also works for 1MB */
#define LF_ERASE_BLOCK_SIZE	0x200000

#define LF_P0_SIZE 	0x00400000
#define LF_P1_SIZE 	0x01000000
#define LF_P2_SIZE_2G4G	0x0A000000
#define LF_P2_SIZE_8G	0x0A000000
#define LF_P3_SIZE	MTDPART_SIZ_FULL

#if ((LF_P0_SIZE % LF_ERASE_BLOCK_SIZE) || (LF_P1_SIZE % LF_ERASE_BLOCK_SIZE) || \
     (LF_P2_SIZE_2G4G % LF_ERASE_BLOCK_SIZE) || (LF_P2_SIZE_8G % LF_ERASE_BLOCK_SIZE) || \
     (LF_P3_SIZE % LF_ERASE_BLOCK_SIZE))
#error "NAND partitions must be multiple of erase block."
#endif

struct mtd_partition partition_map_2G4G[] = {
        { .name         = "Reserved",
          .offset       = 0,
          .size         = LF_P0_SIZE},
        { .name         = "Kernel",
          .offset       = LF_P0_SIZE,
          .size         = LF_P1_SIZE },
        { .name         = "RFS",
          .offset       = LF_P0_SIZE + LF_P1_SIZE,
          .size         = LF_P2_SIZE_2G4G},
        { .name         = "Bulk",
          .offset       = LF_P0_SIZE + LF_P1_SIZE + LF_P2_SIZE_2G4G,
          .size         = LF_P3_SIZE },
};

struct mtd_partition partition_map_8G[] = {
        { .name         = "Reserved",
          .offset       = 0,
          .size         = LF_P0_SIZE},
        { .name         = "Kernel",
          .offset       = LF_P0_SIZE,
          .size         = LF_P1_SIZE },
        { .name         = "RFS",
          .offset       = LF_P0_SIZE + LF_P1_SIZE,
          .size         = LF_P2_SIZE_8G},
        { .name         = "Bulk",
          .offset       = LF_P0_SIZE + LF_P1_SIZE + LF_P2_SIZE_8G,
          .size         = LF_P3_SIZE },
};

#undef LF_P0_SIZE
#undef LF_P1_SIZE
#undef LF_P2_SIZE_2G4G
#undef LF_P2_SIZE_8G
#undef LF_P3_SIZE
#undef LF_ERASE_BLOCK_SIZE

