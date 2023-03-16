/*
 *	nx_aes_regmap.h
 *
 *	History :
 *		2011-03-29 by Ray
 *			- Create
 */

#ifndef __nx_aes_regmap_h__
#define __nx_aes_regmap_h__

typedef struct AES_REG{
	volatile unsigned int	aes_key[4];			//	REGAES_KEY	:	base+0x00 ~ base+0x0C
	volatile unsigned int	aes_count[4];		//	AES counter	:	base+0x10 ~ base+0x1C
	volatile unsigned int	aes_text_in[4];		//	Text In/Out	:	base+0x20 ~ base+0x2C
	volatile unsigned int	aes_inc_count;		//	AES counter	:	base+0x30
	volatile unsigned int	aes_contol;			//	contol		:	base+0x34
}AES_REG;

#endif	//	__nx_reg_regmap_h__
