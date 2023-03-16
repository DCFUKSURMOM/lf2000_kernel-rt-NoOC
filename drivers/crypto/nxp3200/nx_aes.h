#ifndef __nx_aes_ctrl_h__
#define __nx_aes_ctrl_h__

#include "nx_aes_ioctl.h"

enum {
	AES_MODE_POLLL		= 0,
	AES_MODE_1CH_DMA	= 1,
	AES_MODE_2CH_DMA	= 3
};

typedef struct AESContext
{
	unsigned int	mode;			//	Working Mode : POLL or 1CH DMA or 2CH DMA
	unsigned int	key[32/4];
	unsigned int	keyLength;
	unsigned int	iv[16/4];
	unsigned int	ivLength;
}AESContext;

AESContext *CreateAES(void);
void DestroyAES( AESContext *hContex );
int AESRunCipher( AESContext *hContex, AESRunParam *pParam );

#endif	//	__nx_aes_ctrl_h__
