//------------------------------------------------------------------------------
//
//  Copyright (C) 2009 Nexell Co., All Rights Reserved
//  Nexell Co. Proprietary & Confidential
//
//	MAGICEYES INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//  AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//  FOR A PARTICULAR PURPOSE.
//
//	Module     :
//	File       :
//	Description:
//	Author     : Goofy
//	History    :
//------------------------------------------------------------------------------
#if (0)
#include <common.h>
#include <asm/errno.h>
#include <nand.h>
#include <platform.h>
#else
#include <linux/kernel.h>
#include <mach/platform.h>
#endif

#include <linux/slab.h>
#include <linux/mtd/nand.h>
#include "lf2000.h"
#include "nand_ecc.h"
extern struct lf2000_nand_devices lf2000_nand;



#if	(1)
#define DBGOUT(msg...)		{ printk(KERN_INFO "ecc_func: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 					\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",		\
			__FILE__, __FUNCTION__, __LINE__),	\
		printk(KERN_ERR msg); }


//------------------------------------------------------------------------------
// NAND CTRL REGISGER:
//------------------------------------------------------------------------------
#define BASEADDR_NFREG			IO_ADDRESS(PHY_BASEADDR_NAND_MODULE)
#define BASEADDR_NFCTRL			(BASEADDR_NFREG)			// 0xC002B07C
#define BASEADDR_NFECC			(BASEADDR_NFREG + 0x04)		// 0xC002B080
#define BASEADDR_NFORGECC		(BASEADDR_NFREG + 0x20)		// 0xC002B09C
#define BASEADDR_NFECCSTATUS	(BASEADDR_NFREG + 0x40)		// 0xC002B0BC
#define BASEADDR_NFSYNDROME		(BASEADDR_NFREG + 0x44)		// 0xC002B0C0
#define BASEADDR_TIMING			(BASEADDR_NFREG + 0x64)		// 0xC002B0E0
#define T_ACS	0x0C
#define T_COS	0x10
#define T_ACC0	0x14
#define T_OCH	0x1C
#define T_CAH	0x20

#define REG_NFCTRL				(*(volatile unsigned int *)(BASEADDR_NFREG))
#define REG_NFECCSTATUS			(*(volatile unsigned int *)(BASEADDR_NFECCSTATUS))

//------------------------------------------------------------------------------
// Register Bits
#define NX_NFCTRL_NCSENB			(1U<<31)
#define NX_NFCTRL_AUTORESET			(1U<<30)
#define NX_NFCTRL_ECCMODE			(7U<<27)
#define NX_NFCTRL_ECCMODE_16		(3U<<27)
#define NX_NFCTRL_ECCMODE_24		(4U<<27)
#define NX_NFCTRL_IRQPEND			(1U<<15)
#define NX_NFCTRL_ECCRST			(1U<<11)
#define NX_NFCTRL_RNB				(1U<< 9)
#define NX_NFCTRL_IRQENB			(1U<< 8)
#define NX_NFCTRL_BANK				(3U<< 0)

#define NX_NFECCSTATUS_ERROR		(1U<< 2)
#define NX_NFECCSTATUS_DECDONE		(1U<< 1)
#define NX_NFECCSTATUS_ENCDONE		(1U<< 0)

#define NX_NFECCCTRL_ERRNUM			25
#define NX_NFECCCTRL_DECMODE		24	// 0: encoder 1: decoder
#define NX_NF_ENCODE			0
#define NX_NF_DECODE			1
#define NX_NFECCCTRL_ELPLOAD		23	// set after elp registers
#define NX_NFECCCTRL_ZEROPAD		22
#define NX_NFECCCTRL_RUNECC			21	// ecc start
#define NX_NFECCCTRL_ELPNUM			16	// number of elp (0x1F)
#define NX_NFECCCTRL_PDATACNT		10	// number of parity bit (0x3F)
#define NX_NFECCCTRL_DATACNT		0	// nand data count value(write) (0x3FF)

//------------------------------------------------------------------------------


void NX_NAND_CreateLookupTable(struct nand_chip *chip)
{
	const struct tag_bch_info * bch_info;
	struct tag_bch_arrays     * bch_arrays;
	int	i;
	int	mask;	// Register states
	unsigned int p;	// Primitive polynomial
	short * pAlphaTo;
	short * pIndexOf;
	int   * buf;
	int	allocBytes;

	bch_info   = ((struct lf2000_nand_data *)chip->priv)->bch_info;
	bch_arrays = bch_info->p_arrays;
	if (bch_arrays->bch_AlphaToTable != NULL) {
		return;				
	}
	if (bch_info->bch_var_t == 24)
		p = 0x41D5;
	else
		p = 0x25AF;

	allocBytes = 2 * sizeof(short) * (1 + bch_arrays->bch_var_n);

	buf = kzalloc(allocBytes, GFP_KERNEL);

	if(NULL == buf) {
		printk(KERN_INFO "NX_NAND_CreateLookupTable(): allocation failure\n");
		return;
	}
	bch_arrays->L	= lf2000_nand.L;
	bch_arrays->u_L	= lf2000_nand.u_L;
	bch_arrays->reg	= lf2000_nand.reg;
	bch_arrays->s	= lf2000_nand.s;
	bch_arrays->elp	= lf2000_nand.elp;
	bch_arrays->desc= lf2000_nand.desc;

	bch_arrays->bch_AlphaToTable = (short *)(buf);
	bch_arrays->bch_IndexOfTable = bch_arrays->bch_AlphaToTable
					+ (1 + bch_arrays->bch_var_n);

	pAlphaTo = bch_arrays->bch_AlphaToTable;
	pIndexOf = bch_arrays->bch_IndexOfTable;

	// Galois field implementation with shift registers
	// Ref: L&C, Chapter 6.7, pp. 217
	mask = 1;
	pAlphaTo[bch_arrays->bch_var_m] = 0;
	for ( i=0 ; i<bch_arrays->bch_var_m ; i++ )	{
		pAlphaTo[ i ] = mask;
		pIndexOf[ pAlphaTo[i] ] = i;

		if ( p & (1U<<i) )
			pAlphaTo[bch_arrays->bch_var_m] ^= mask;

		mask <<= 1 ;
	}

	pIndexOf[ pAlphaTo[bch_arrays->bch_var_m] ] = bch_arrays->bch_var_m;
	mask >>= 1;
	for ( i=bch_arrays->bch_var_m+1 ; i<bch_arrays->bch_var_n ; i++ ) {
		if ( pAlphaTo[i-1] >= mask )
			pAlphaTo[i] = pAlphaTo[bch_arrays->bch_var_m] 
					^ ((pAlphaTo[i-1] ^ mask) << 1);
		else
			pAlphaTo[i] = pAlphaTo[i-1] << 1;

		pIndexOf[pAlphaTo[i]] = i;
	}
	pAlphaTo[i] = 0;
	pIndexOf[0] = -1;
}

//------------------------------------------------------------------------------
static int	NX_BCH_MODULAR(int index, int modular)
{
	while ( index >= modular )
		index -= modular;

	return index;
}


//------------------------------------------------------------------------------

void NX_NAND_SetResetECC(int EccMode)
{
	const U32 BIT_SIZE	= 3;
	const U32 BIT_POS	= 27;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register U32 regval;

	if (   (4 != EccMode)  && (8 != EccMode) && (12!=EccMode) 
		&& (16 != EccMode) && (24!= EccMode) )
	{
		printk(KERN_INFO "NX_NAND_SetResetECC(%d)\n", EccMode);
		return;
	}

	if(EccMode == 24)
		EccMode = 4;
	else
	{
		EccMode /= 4;	// NFECCMODE[2:0] = 0(4), 1(8), 2(12), 3(16), 4(24)
		EccMode--;
	}

	regval  = REG_NFCTRL;
	regval &= ~(BIT_MASK);	// Unmask bits.
	regval |= (EccMode << BIT_POS);

	// Reset H/W BCH decoder.
	REG_NFCTRL = regval | NX_NFCTRL_ECCRST;
}



#define BASEADDR_NFECCCTRL		(BASEADDR_NFREG + 0x88)		// 0xC002B104
#define BASEADDR_NFECC7			(BASEADDR_NFREG + 0x8C)
#define BASEADDR_NFORGECC7		(BASEADDR_NFREG + 0x9C)
#define BASEADDR_NFSYNDROME8	(BASEADDR_NFREG + 0xAC)
#define BASEADDR_NFELP0			(BASEADDR_NFREG + 0xBC)		// 0xC002B138
#define BASEADDR_NFELOC0		(BASEADDR_NFREG + 0xEC)		// 0xC002B168

#define REG_NFECCCTRL			(*(volatile unsigned int *)(BASEADDR_NFECCCTRL))

#define NFECCCTRL_Encode		(NX_NF_ENCODE << NX_NFECCCTRL_DECMODE)
#define NFECCCTRL_Decode		(NX_NF_DECODE << NX_NFECCCTRL_DECMODE)
#define NFECCCTRL_Start			(1 << NX_NFECCCTRL_RUNECC)	// 0x00200000

#define NFECCCTRL_ParityCount4	(6 << 10)
#define NFECCCTRL_ParityCount8	(12 << 10)
#define NFECCCTRL_ParityCount12	(19 << 10)
#define NFECCCTRL_ParityCount16	(25 << 10)
#define NFECCCTRL_ParityCount24	(41 << 10)


void NX_NAND_StartEccDecoding(int eccmode, int num_bytes)
{
	unsigned int reg_value;

	if (   (eccmode != 4) &&  (eccmode != 8) && (eccmode != 12)
		&& (eccmode != 16) && (eccmode != 24))
	{
		printk(KERN_INFO "NX_NAND_StartEccDecoding: bad eccmode (%d); using 8\n",
				eccmode);
		eccmode = 8;
	}
	if ((num_bytes != 512) && (num_bytes != 1024))
	{
		printk(KERN_INFO "NX_NAND_StartEccDecoding: bad num_bytes (%d); using 512\n",
				num_bytes);
		num_bytes = 512;
	}
	switch (eccmode) {
	case 4:		reg_value = NFECCCTRL_ParityCount4;	break;
	case 8:		reg_value = NFECCCTRL_ParityCount8;	break;
	case 12:	reg_value = NFECCCTRL_ParityCount12;	break;
	case 16:	reg_value = NFECCCTRL_ParityCount16;	break;
	case 24:	reg_value = NFECCCTRL_ParityCount24;	break;
	default:	reg_value = 0;				break;
	}
	reg_value |= (NFECCCTRL_Decode | NFECCCTRL_Start
				       | (eccmode << NX_NFECCCTRL_ELPNUM));

	reg_value |= (num_bytes - 1);
	REG_NFECCCTRL = reg_value;
}

void NX_NAND_StartEccEncoding(int eccmode, int num_bytes)
{
	unsigned int reg_value;

	if (   (eccmode != 4) &&  (eccmode != 8) && (eccmode != 12)
		&& (eccmode != 16) && (eccmode != 24))
	{
		printk(KERN_INFO "NX_NAND_StartEccEncoding: bad eccmode (%d); using 8\n",
				eccmode);
		eccmode = 8;
	}
	if ((num_bytes != 512) && (num_bytes != 1024))
	{
		printk(KERN_INFO "NX_NAND_StartEccEncoding: bad num_bytes (%d); using 512\n",
				num_bytes);
		num_bytes = 512;
	}
	switch (eccmode) {
	case 4:		reg_value = NFECCCTRL_ParityCount4;		break;
	case 8:		reg_value = NFECCCTRL_ParityCount8;		break;
	case 12:	reg_value = NFECCCTRL_ParityCount12;	break;
	case 16:	reg_value = NFECCCTRL_ParityCount16;	break;
	case 24:	reg_value = NFECCCTRL_ParityCount24;	break;
	default:	reg_value = 0;							break;
	}
	reg_value |= (NFECCCTRL_Encode | NFECCCTRL_Start);

	reg_value |= (num_bytes - 1);
	REG_NFECCCTRL = reg_value;
}


//------------------------------------------------------------------------------
void NX_NAND_WaitForDecoding(void)
{
	while (0==(REG_NFECCSTATUS & NX_NFECCSTATUS_DECDONE)) { ; }
}

//------------------------------------------------------------------------------
void NX_NAND_WaitForEncoding(void)
{
	while (0==(REG_NFECCSTATUS & NX_NFECCSTATUS_ENCDONE)) { ; }
}

//------------------------------------------------------------------------------
int	NX_NAND_GetErrorStatus(void)
{
	if (REG_NFECCSTATUS & NX_NFECCSTATUS_ERROR)
		return 1;

	return 0;
}

static void NANDFlash_SetELP(unsigned int *pELP, 
			     int NumberOfElp,
			     struct nand_chip *chip)
{
	const struct tag_bch_info * bch_info;
	int i;
	volatile U32 *pRegELP = (volatile U32 *)(BASEADDR_NFELP0);
	
	bch_info   = ((struct lf2000_nand_data *)chip->priv)->bch_info;

	for(i=0; i < bch_info->bch_var_t/2; i++)	
		*(pRegELP+i) = 0x00000000;
#if 1	/* 16apr12  This is the current	version, but I think the #else	*/
	/*	    bracket (from iROMBOOT) has slightly faster code	*/
	for (i=0; i < NumberOfElp; i += 2)
	{
		/* if there are at least 2 more ELPs, load 2; else just 1 */
		if (i < NumberOfElp - 1) {
			*pRegELP++ = (*pELP & 0x3fff) | ((*(pELP+1) & 0x3FFF)<<14);
			pELP += 2;
		} else {
			*pRegELP = (*pELP & 0x3fff);
		}
	}
#else
	for(i=0; i<NumberOfElp/2; i++)
	{
		*pRegELP++ =	((*(pELP+0) & 0x3FFF)<<0) |
						((*(pELP+1) & 0x3FFF)<<14);
		pELP+=2;
	}
	if (NumberOfElp & 0x01)
	{
		*pRegELP = ((*(pELP+0) & 0x3FFF)<<0);
	}
#endif
}

#define elp(a,b) pelp[(a)*(ttx2 + 4) + b]

int	NX_NAND_GetErrorLocation(int *pOddSyn, 
				 int *pLocation, 
				 int *ErrCnt,
				 struct nand_chip *chip)
{
	register int i, j;
	int	  count;
	const struct tag_bch_info * bch_info;
	struct tag_bch_arrays     * bch_arrays;
	short * pAlphaTo;
	short * pIndexOf;
	int   * pL;
	int   * pUL;
	int   * preg;
	int   * ps;
	int   * pelp;
	int   *	pdesc;
	int	tt;
	int	ttx2;
	int	u;			// u = 'mu' + 1 and u ranges from 
					//      -1 to 2*t (see L&C)
	int	q;	
	int	nn;	

	bch_info   = ((struct lf2000_nand_data *)chip->priv)->bch_info;
	bch_arrays = bch_info->p_arrays;
	pL	   = bch_arrays->L;
	pUL	   = bch_arrays->u_L;
	preg	   = bch_arrays->reg;
	ps	   = bch_arrays->s;
	pelp	   = bch_arrays->elp;
	pdesc	   = bch_arrays->desc;
	
	pAlphaTo   = bch_arrays->bch_AlphaToTable;
	pIndexOf   = bch_arrays->bch_IndexOfTable;

	nn	   = bch_arrays->bch_var_n;
	tt	   = bch_info->bch_var_t;
	ttx2	   = 2 * tt;

	for( i=0 ; i < tt ; i++ )
		ps[i*2 + 1] = pOddSyn[i];

	// Even syndrome = (Odd syndrome) ** 2
	for (i = 2; i <= ttx2; i = i + 2) {
	 	j = i / 2;
		if (ps[j] == 0)
			ps[i] = 0;
		else
			ps[i] =  pAlphaTo[NX_BCH_MODULAR(2 * pIndexOf[ps[j]],
							 nn)];
			//ps[i] =  alpha_to[(2 * index_of[ps[j]]) % nn];
	}
	// Having errors, begin decoding procedure
	// Simplified Berlekamp-Massey Algorithm for Binary BCH codes
	// 	Ref: Blahut, pp.191, Chapter 7.6 
	// 	Ref: L&C, pp.212, Chapter 6.4
	//
	// Following the terminology of Lin and Costello's book:   
	// 	desc[u] is the 'mu'th discrepancy, where  
	// 	u='mu'+1 and 
	// 	'mu' (the Greek letter!) is the step number ranging 
	// 		from -1 to 2*t (see L&C)
	// 	l[u] is the degree of the elp at that step, and 
	// 	u_L[u] is the difference between the step number 
	// 		and the degree of the elp. 

	// initialise table entries
	for (i = 1; i <= ttx2; i++) 
		ps[i] = pIndexOf[ps[i]];

	pdesc[0] = 0;				/* index form */
	pdesc[1] = ps[1];			/* index form */
	elp(0,0) = 1;				/* polynomial form */
	elp(1,0) = 1;				/* polynomial form */
	for (i = 1; i < ttx2; i++) {
		elp(0,i) = 0;			/* polynomial form */
		elp(1,i) = 0;			/* polynomial form */
	}
	pL[0] = 0;
	pL[1] = 0;
	pUL[0] = -1;
	pUL[1] = 0;
	u = -1; 
 
	do {
		// even loops always produce no discrepany so they can be skipped
		u = u + 2; 
		if (pdesc[u] == -1) {
			pL[u + 2] = pL[u];
			for (i = 0; i <= pL[u]; i++)
				elp(u + 2,i) = elp(u,i); 
		}
		else {
			/* search for words with greatest u_L[q] 
			 *for which desc[q]!=0 
			 */
			q = u - 2;
			if (q<0) q=0;
			// Look for first non-zero desc[q] 
			while ((pdesc[q] == -1) && (q > 0))
				q=q-2;
			if (q < 0) q = 0;

			// Find q such that desc[u]!=0 and u_L[q] is maximum
			if (q > 0) {
				j = q;
			  	do {
			    		j=j-2;
					if (j < 0) j = 0;
			    		if (   (pdesc[j] != -1)
					    && (pUL[q] < pUL[j]))
			      			q = j;
			  	} while (j > 0);
			}
 
			// store degree of new elp polynomial
			if (pL[u] > pL[q] + u - q)
				pL[u + 2] = pL[u];
			else
				pL[u + 2] = pL[q] + u - q;
 
			// Form new elp(x)
			for (i = 0; i < ttx2; i++) 
				elp(u + 2,i) = 0;
			for (i = 0; i <= pL[q]; i++) 
				if (elp(q,i) != 0)
					elp(u + 2,i + u - q) = 
		pAlphaTo[NX_BCH_MODULAR((pdesc[u] + nn - pdesc[q] 
						  + pIndexOf[elp(q,i)]), nn)];
			for (i = 0; i <= pL[u]; i++) 
				elp(u + 2,i) ^= elp(u,i);
		}
		pUL[u + 2] = u+1 - pL[u + 2];
 
		/* Form (u+2)th discrepancy.  
		 * No discrepancy computed on last iteration 
		 */
		if (u < ttx2) {	
			if (ps[u + 2] != -1)
				pdesc[u + 2] = pAlphaTo[ps[u + 2]];
			else 
				pdesc[u + 2] = 0;

			for (i = 1; i <= pL[u + 2]; i++) 
				if (   (ps[u + 2 - i] != -1) 
				    && (elp(u + 2,i) != 0))
		        		pdesc[u + 2] ^= 
					  pAlphaTo[NX_BCH_MODULAR(ps[u + 2 - i]
							+ pIndexOf[elp(u + 2,i)],
								  nn)];
		 	// put desc[u+2] into index form 
			pdesc[u + 2] = pIndexOf[pdesc[u + 2]];	
		}
	} while ((u < (ttx2-1)) && (pL[u + 2] <= tt)); 
	u=u+2;
	pL[ttx2-1] = pL[u];
		
 	if (pL[ttx2-1] > tt) 
	{
		if(ErrCnt)
			*ErrCnt = pL[ttx2-1];
		if (report_ecc_errors)
			printk(KERN_INFO "L[x] > NX_BCH_VAR_T:   (%d) > (%d)\n",
				pL[ttx2-1], tt);
		return -1;
	}
	else
	{
		// Chien's search to find roots of the error location polynomial
		// Ref: L&C pp.216, Fig.6.1
		/* New Nexell and Micron 2.0 code */
		for( i=1 ; i<=pL[ttx2 - 1] ; i++ )
			preg[i] = elp(u, i);


		count = pL[ttx2 - 1];	// get elp num for set

		NANDFlash_SetELP((U32*)&preg[1], count, chip);		// set elp

		REG_NFECCCTRL =   (1 << NX_NFECCCTRL_ELPLOAD) 		// 0x00800000
				| (tt << NX_NFECCCTRL_ELPNUM)
				| ((bch_info->bch_var_m * tt/8 - 1)
					<< NX_NFECCCTRL_PDATACNT)
				| (bch_info->bch_var_k/8 - 1);

			// check bit 1 of the ECCSTATUS register
		while ( 0 == (REG_NFECCSTATUS & NX_NFECCSTATUS_DECDONE) )
			;

		count = (REG_NFECCCTRL >> NX_NFECCCTRL_ERRNUM) & 0x1F;	
				// get error count from the ECCCTRL register

		if(count == pL[ttx2 - 1])
		{
			static U16 pELoc[24];
			//NX_MCUS_GetErrLoc16(pELoc);
			{
				U32 ix, regvalue;
				volatile U32 *pRegErrLoc = 
					     (volatile U32 *)(BASEADDR_NFELOC0);
				U16 * pE = &pELoc[0];
				for(ix=0; ix< 12; ix++)
				{
					regvalue = *pRegErrLoc++;
					*pE++ = ((regvalue>>0  & 0x3FFF)^0x007);
					*pE++ = ((regvalue>>14 & 0x3FFF)^0x007);
				}
			}
			for(i=0; i<count; i++)
				pLocation[i] = pELoc[i];
			*ErrCnt = count;
			return count;
		}
		else
		{
			if(ErrCnt)
				*ErrCnt = count;
			if (report_ecc_errors)
				printk(KERN_INFO "ERROR: count != "
					"L[(NX_BCH_VAR_T*2)-1]"
					 ": %d, %d\n",
					count, pL[ttx2 - 1]);
			return -1;
		}
	}
}




//------------------------------------------------------------------------------
void NX_NAND_GetGenECC(unsigned int *pECC, int EccMode)
{
	int i, num;
	volatile U32 *pRegECC = (volatile U32 *)BASEADDR_NFECC;

	switch (EccMode) {
	case  4: num = 2;	break;
	case  8: num = 4;	break;
	case 12: num = 5;	break;
	case 16: num = 7;	break;
	case 24: num = 11;	break;
	default:
		ERROUT("not support ECC %d bit\n", EccMode);
		return;
	}

	for (i=0 ; (i<num) && (i < 7); i++)
		*pECC++ = *pRegECC++;


	if (i < num) {
		pRegECC = (volatile U32 *)BASEADDR_NFECC7;
		for ( ; i<num; i++) {
			*pECC++ = *pRegECC++;
		}
	}
	else if (EccMode == 4) {
		--pECC;					/* step back to the last value stored */
		*pECC &= 0x00F0FFFF;	/* and mask out irrelevant stuff */
	}
	else if (EccMode == 12) {
		--pECC;					/* step back to the last value stored */
		*pECC &= 0xF0FFFFFF;	/* and mask out irrelevant stuff */
	}
}

void NX_NAND_SetOriECC(unsigned int *pECC, int EccMode)
{
	int i, num;
	volatile U32 *pRegOrgECC = (volatile U32 *)BASEADDR_NFORGECC;

	switch (EccMode) {
	case  4: num = 2;	break;
	case  8: num = 4;	break;
	case 12: num = 5;	break;
	case 16: num = 7;	break;
	case 24: num = 11;	break;
	default:
		ERROUT("not support ECC %d bit\n", EccMode);
		return;
	}

	for (i=0 ; (i < num) && (i < 7); i++)
		*pRegOrgECC++ = *pECC++;

	if (i < num) {
		pRegOrgECC = (volatile U32 /*__iomem*/ *)BASEADDR_NFORGECC7;
		for ( ; i < num; ++i) {
			*pRegOrgECC++ = *pECC++;
		}
	}
}

//------------------------------------------------------------------------------
void NX_NAND_GetOddSyndrome(int *pSyndrome, struct nand_chip * chip)
{
	const U32 BIT_SIZE	= 14;
	const U32 BIT_POS	= 16;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);

	register volatile U32 *pReg;
	register U32 regval;
	int i;
	const struct tag_bch_info * bch_info;

	bch_info   = ((struct lf2000_nand_data *)chip->priv)->bch_info;

	NX_ASSERT( NULL != pSyndrome );

	pReg = (volatile U32 *)BASEADDR_NFSYNDROME;

	for ( i=0 ; (i<(bch_info->bch_var_t/2)) && (i < 8) ; i++ ) {
		regval = *pReg++;
		*pSyndrome++ = (int)(regval & BIT_MASK);
		*pSyndrome++ = (int)((regval >> BIT_POS) & BIT_MASK);
	}

	if ( i < (bch_info->bch_var_t/2)) {
		pReg = (volatile U32 *)BASEADDR_NFSYNDROME8;
		/* don't re-init i */
		for (   ;  i< (bch_info->bch_var_t/2) ; i++ ) 
		{
			regval = *pReg++;
			*pSyndrome++ = (int)(regval & BIT_MASK);
			*pSyndrome++ = (int)((regval >> BIT_POS) & BIT_MASK);
		}
	}
}

unsigned int NX_NAND_Get_NFCONTROL(void) {
	return REG_NFCTRL;
}

#define NFCONTROL_RO_BITS_MASK	0x07FF76C0
void  NX_NAND_Set_NFCONTROL(unsigned int value) {
	unsigned int reg_value;

	value     &= ~NFCONTROL_RO_BITS_MASK;	// mask out the read-only bits
	reg_value  = REG_NFCTRL;
	reg_value &= NFCONTROL_RO_BITS_MASK;

	REG_NFCTRL = reg_value | value;
}

void NX_NAND_Get_Timing(unsigned int * pACS,
			unsigned int * pCOS,
			unsigned int * pOCH,
			unsigned int * pCAH,
			unsigned int * pACC0) 
{
	if (pACS)  *pACS  = (*(volatile unsigned int *)(BASEADDR_TIMING + T_ACS));
	if (pCOS)  *pCOS  = (*(volatile unsigned int *)(BASEADDR_TIMING + T_COS));
	if (pOCH)  *pOCH  = (*(volatile unsigned int *)(BASEADDR_TIMING + T_OCH));
	if (pCAH)  *pCAH  = (*(volatile unsigned int *)(BASEADDR_TIMING + T_CAH));
	if (pACC0) *pACC0 = (*(volatile unsigned int *)(BASEADDR_TIMING + T_ACC0));
}

int NX_NAND_Set_Timing(unsigned int index, unsigned int value)
{
	volatile unsigned int * p;

	switch (index) {
	case kACS:	p = (volatile unsigned int *)(BASEADDR_TIMING + T_ACS);	break;
	case kCOS:	p = (volatile unsigned int *)(BASEADDR_TIMING + T_COS);	break;
	case kOCH:	p = (volatile unsigned int *)(BASEADDR_TIMING + T_OCH);	break;
	case kCAH:	p = (volatile unsigned int *)(BASEADDR_TIMING + T_CAH);	break;
	case kACC0:	p = (volatile unsigned int *)(BASEADDR_TIMING + T_ACC0);break;
	default:
		return -1;
	}
	*p = value;
	return 0;
}

int NX_NAND_Get_ECC_regs(unsigned int * pbuf)
{
	int i;
	volatile unsigned int * pregs;

	if (NULL == pbuf)
		return -1;
	
	for (i = 0, pregs = (volatile unsigned int *)BASEADDR_NFECC;
		 i < 7; ++i) {
		*pbuf++ = *pregs++;
	}
	for ( pregs = (volatile unsigned int *)BASEADDR_NFECC7;
		 i < 11; ++i) {
		*pbuf++ = *pregs++;
	}
	return 0;
}

int NX_NAND_Get_OrigECC_regs(unsigned int * pbuf)
{
	int i;
	volatile unsigned int * pregs;

	if (NULL == pbuf)
		return -1;
	
	for (i = 0, pregs = (volatile unsigned int *)BASEADDR_NFORGECC;
		 i < 7; ++i) {
		*pbuf++ = *pregs++;
	}
	for ( pregs = (volatile unsigned int *)BASEADDR_NFORGECC7;
		 i < 11; ++i) {
		*pbuf++ = *pregs++;
	}
	return 0;
}

unsigned int NX_NAND_Get_NFCNT(void)
{
	return (*(volatile unsigned int *)(BASEADDR_NFREG+0x3c));
}

unsigned int NX_NAND_Get_NFECCSTATUS(void)
{
	return (*(volatile unsigned int *)(BASEADDR_NFREG+0x40));
}

int NX_NAND_Get_SYNDROMES(unsigned int * pbuf)
{
	int i;
	volatile unsigned int * pregs;

	if (NULL == pbuf)
		return -1;
	
	for (i = 0, pregs = (volatile unsigned int *)BASEADDR_NFSYNDROME;
		 i < 8; ++i) {
		*pbuf++ = *pregs++;
	}
	for ( pregs = (volatile unsigned int *)BASEADDR_NFSYNDROME8;
		 i < 12; ++i) {
		*pbuf++ = *pregs++;
	}
	return 0;
}

unsigned int NX_NAND_Get_NFECCCTRL(void) {
	return REG_NFECCCTRL;
}

#define NFECCCTRL_RO_BITS_MASK	0xFE000000
void  NX_NAND_Set_NFECCCTRL(unsigned int value) {
	unsigned int reg_value;

	value     &= ~NFECCCTRL_RO_BITS_MASK;	// mask out the read-only bits
	reg_value  = REG_NFECCCTRL;
	reg_value &= NFECCCTRL_RO_BITS_MASK;

	REG_NFECCCTRL = reg_value | value;
}

int NX_NAND_Get_ELP_regs(unsigned int * pbuf) {
	int i;
	volatile unsigned int * pregs;

	if (NULL == pbuf)
		return -1;
	
	for (i = 0, pregs = (volatile unsigned int *)BASEADDR_NFELP0;
		 i < 12; ++i) {
		*pbuf++ = *pregs++;
	}
	return 0;
}

int NX_NAND_Set_ELP_reg(unsigned int index, unsigned int value) {
	if (index >= 12)
		return -1;
	*((volatile unsigned int *)BASEADDR_NFELP0 + index) = value;
	return 0;
}

int NX_NAND_Get_ELOC_regs(unsigned int * pbuf) {
	int i;
	volatile unsigned int * pregs;

	if (NULL == pbuf)
		return -1;
	
	for (i = 0, pregs = (volatile unsigned int *)BASEADDR_NFELOC0;
		 i < 12; ++i) {
		*pbuf++ = *pregs++;
	}
	return 0;
}

#if 1	/* Shorter, and all we usually want */

void NX_NAND_OutputNandControllerRegs(void)
{
	printk(KERN_INFO "NFCONTROL %08x; "
			 "ECCSTATUS %08x; "
			 "ECCCTRL   %08x\n",
			NX_NAND_Get_NFCONTROL(),
			NX_NAND_Get_NFECCSTATUS(),
			NX_NAND_Get_NFECCCTRL());
}

#else
void NX_NAND_OutputNandControllerRegs(void)
{
	printk(KERN_INFO "NFCONTROL 0x%08x\n", NX_NAND_Get_NFCONTROL());
	printk(KERN_INFO "ORIGECC   %08x %08x %08x %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x20)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x24)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x28)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x2c)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x30)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x34)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x38)));
	printk(KERN_INFO "NFCNT     %08x\n", NX_NAND_Get_NFCNT());
	printk(KERN_INFO "ECCSTATUS %08x\n", NX_NAND_Get_NFECCSTATUS());
	printk(KERN_INFO "SYNDROMEs %08x %08x %08x %08x %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x44)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x48)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x4c)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x50)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x54)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x58)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x5c)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x60)));
	printk(KERN_INFO "TIMEACS  %08x\n",
			(*(volatile unsigned int *)(BASEADDR_TIMING + T_ACS)));
	printk(KERN_INFO "TIMECOS  %08x\n",
			(*(volatile unsigned int *)(BASEADDR_TIMING + T_COS)));
	printk(KERN_INFO "TIMEACC0 %08x\n",
			(*(volatile unsigned int *)(BASEADDR_TIMING + T_ACC0)));
	printk(KERN_INFO "TIME0CH  %08x\n",
			(*(volatile unsigned int *)(BASEADDR_TIMING + T_OCH)));
	printk(KERN_INFO "TIMECAH  %08x\n",
			(*(volatile unsigned int *)(BASEADDR_TIMING + T_CAH)));
	printk(KERN_INFO "ECCCTRL  %08x\n", NX_NAND_Get_NFECCCTRL());
	printk(KERN_INFO "ORIGECC2 %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x9c)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xa0)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xa4)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xa8)));
	printk(KERN_INFO "Syndrom2 %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xac)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xb0)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xb4)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xb8)));
	printk(KERN_INFO "ELP0-3   %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xbc)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xc0)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xc4)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xc8)));
	printk(KERN_INFO "ELP4-7   %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xcc)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xd0)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xd4)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xd8)));
	printk(KERN_INFO "ELP8-11  %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xdc)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xe0)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xe4)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xe8)));
	printk(KERN_INFO "ELOC0-3  %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xec)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xf0)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xf4)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xf8)));
	printk(KERN_INFO "ELOC4-7  %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0xfc)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x100)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x104)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x108)));
	printk(KERN_INFO "ELOC8-11 %08x %08x %08x %08x\n",
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x10c)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x110)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x114)),
			(*(volatile unsigned int *)(BASEADDR_NFREG+0x118)));
}
#endif	/* 12apr12 */

