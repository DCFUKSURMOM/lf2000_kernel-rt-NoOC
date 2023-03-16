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
#ifndef __NAND_ECC_H__
#define __NAND_ECC_H__

/* u-boot yaffs2 */
#if defined (CFG_NAND_WRITE_YAFFS2) && defined(CONFIG_SYS_NAND_HW_ECC) && (CFG_NAND_ECC_MODE == 8)
#warning "===================================="
#warning "hw 8bit ecc mode not support yaffs2 "
#warning "-> change to 4bit hw ecc"
#warning "===================================="
#undef	CFG_NAND_ECC_MODE
#define	CFG_NAND_ECC_MODE		4
#endif

/* linux kernel yaffs2 */
#if defined (CONFIG_YAFFS_FS) && defined(CONFIG_MTD_NAND_NEXELL_HWECC) && (CFG_NAND_ECC_MODE == 8)
#warning "===================================="
#warning "hw 8bit ecc mode not support yaffs2 "
#warning "-> change to 4bit hw ecc"
#warning "===================================="
#undef	CFG_NAND_ECC_MODE
#define	CFG_NAND_ECC_MODE		4
#endif

void 	NX_NAND_CreateLookupTable(struct nand_chip * chip);
void 	NX_NAND_SetResetECC(int EccMode);
void 	NX_NAND_WaitForDecoding(void);
void 	NX_NAND_WaitForEncoding(void);
int	NX_NAND_GetErrorStatus(void);
int	NX_NAND_GetErrorLocation(int *pOddSyn, int *pLocation, int *ErrCnt, 
				 struct nand_chip * chip);
void 	NX_NAND_GetGenECC(unsigned int *pECC, int EccMode);
void 	NX_NAND_SetOriECC(unsigned int *pECC, int EccMode);
void 	NX_NAND_GetOddSyndrome(int *pSyndrome, struct nand_chip * chip);

unsigned int NX_NAND_Get_NFCONTROL(void);
void NX_NAND_Set_NFCONTROL(unsigned int value);
unsigned int NX_NAND_Get_NFECCCTRL(void);
void NX_NAND_Set_NFECCCTRL(unsigned int value);

void NX_NAND_Get_Timing(unsigned int * pACS,
			unsigned int * pCOS,
			unsigned int * pOCH,
			unsigned int * pCAH,
			unsigned int * pACC0); 
int NX_NAND_Set_Timing(unsigned int index, unsigned int value);
#define kACS	0
#define kCOS	1
#define kOCH	2
#define kCAH	3
#define kACC0	4

int NX_NAND_Get_ECC_regs(unsigned int * pbuf);
int NX_NAND_Get_OrigECC_regs(unsigned int * pbuf);
unsigned int NX_NAND_Get_NFCNT(void);
unsigned int NX_NAND_Get_NFECCSTATUS(void);
int NX_NAND_Get_SYNDROMES(unsigned int * pbuf);
int NX_NAND_Get_ELP_regs(unsigned int * pbuf);
int NX_NAND_Set_ELP_reg(unsigned int index, unsigned int value);
int NX_NAND_Get_ELOC_regs(unsigned int * pbuf);

int nand_hw_ecc_init(struct mtd_info *mtd, int hw_ecc_mode);

#endif /* __NAND_ECC_H__ */
