#include <mach/platform.h>
#include "nx_aes.h"
#include "sysapi.h"
#include "nx_aes_regmap.h"
#include "nx_writeio.h"

static AES_REG *gAESReg = (AES_REG *)IO_ADDRESS(0xC0022000);


//----------------------------------------------------------------------------
//	Functions

AESContext *CreateAES( void )
{
	AESContext *hContext = NxMemAlloc( sizeof(AESContext) );
	if( hContext )
		NxMemSet( hContext, 0, sizeof(AESContext) );
	return hContext;
}

void DestroyAES( AESContext *hContext )
{
	if( hContext )
	{
		NxMemFree( hContext );
	}
}

#define SWAP(DATA)	((DATA&0xFF000000)>>24 |   \
					(DATA&0x00FF0000)>>8   |   \
					(DATA&0x0000FF00)<<8   |   \
					(DATA&0x000000FF)<<24 )

int AESRunCipher( AESContext *hContext, AESRunParam *pParam )
{
	unsigned int *pIn;
	unsigned int *pOut;
	unsigned int numBlocks, i;

	numBlocks = pParam->plainLength/16;

	if( pParam->direction )		//	Encrypt
	{
		pIn  = (unsigned int*)pParam->pPlain;
		pOut = (unsigned int*)pParam->pCipher;
	}
	else						//	Decrypt
	{
		pIn  = (unsigned int*)pParam->pCipher;
		pOut = (unsigned int*)pParam->pPlain;
	}

	//	Set Key
	gAESReg->aes_key[3] = SWAP(pParam->key[0]);
	gAESReg->aes_key[2] = SWAP(pParam->key[1]);
	gAESReg->aes_key[1] = SWAP(pParam->key[2]);
	gAESReg->aes_key[0] = SWAP(pParam->key[3]);

	//	Set
	gAESReg->aes_count[3] = pParam->iv[0];
	gAESReg->aes_count[2] = pParam->iv[1];
	gAESReg->aes_count[1] = pParam->iv[2];
	gAESReg->aes_count[0] = pParam->iv[3];

	gAESReg->aes_contol= 0x12;		//	Enable & CTR Mode

	for( i=0 ; i<numBlocks ; i++ )
	{
		gAESReg->aes_text_in[0] = pIn[0];
		gAESReg->aes_text_in[1] = pIn[1];
		gAESReg->aes_text_in[2] = pIn[2];
		gAESReg->aes_text_in[3] = pIn[3];

		gAESReg->aes_contol |= 0x1;		//	Start

		while( gAESReg->aes_contol&1 );

		pOut[0] = gAESReg->aes_text_in[0];
		pOut[1] = gAESReg->aes_text_in[1];
		pOut[2] = gAESReg->aes_text_in[2];
		pOut[3] = gAESReg->aes_text_in[3];

		pIn += 4;
		pOut += 4;

		gAESReg->aes_inc_count = 1;		//	increase counter
	}

	//	Read last counter ( optional )
	pParam->iv[0] = gAESReg->aes_count[3];
	pParam->iv[1] = gAESReg->aes_count[2];
	pParam->iv[2] = gAESReg->aes_count[1];
	pParam->iv[3] = gAESReg->aes_count[0];
	return 0;
}
