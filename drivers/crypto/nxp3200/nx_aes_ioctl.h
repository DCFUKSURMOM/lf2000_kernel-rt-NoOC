/*
 *	nx_aes_ioctl.h
 *
 *	History :
 *		2011-03-29 by Ray
 *			- Create
 */

#ifndef __nx_aes_ioctl_h__
#define __nx_aes_ioctl_h__

//	Device Driver Info
#define NX_DEV_AES_MAJOR		265
#define NX_DEV_AES_NAME			"nx_aes"


#define	NX_AES_NOERR				( 0)
#define	NX_AES_ERROR				(-1)
#define	NX_AES_E_CIPHER				(-2)		//	en/decrypt error
#define	NX_AES_E_INVALIDHANDLE		(-3)		//	invalid handle
#define	NX_AES_E_INVALIDKEY			(-4)		//	invalid key
#define	NX_AES_E_INVALIDIV			(-5)
#define	NX_AES_E_NOTSUPPORT			(-6)		//	not support

#define	NX_AES_MODE_CTR				(1)
#define	NX_AES_MODE_CBC				(2)
#define	NX_AES_MODE_MAX				(-1)

#define IOCTL_AES_DRIVER_BASE		(0xAE5AE5)

//------------------------------------------------------------------------------
//	AES Driver IO Control code
//------------------------------------------------------------------------------
enum	{
	IOCTL_AES_RUN	=  (IOCTL_AES_DRIVER_BASE << 8) + 0,
};


//------------------------------------------------------------------------------
//	AES Device I/O Control Data Type
//------------------------------------------------------------------------------

//	Mode
enum {
	NX_AES_MODE_POLLL		= 0,
	NX_AES_MODE_1CH_DMA		= 1,
	NX_AES_MODE_2CH_DMA		= 3
};

typedef struct _tagAESRunParam
{
	unsigned int	mode;		//	Mode
	unsigned int	direction;
	unsigned char	*pPlain;
	unsigned int	plainLength;
	unsigned char	*pCipher;
	unsigned int	cipherLength;
	unsigned int	key[32/4];
	unsigned int	keyLength;
	unsigned int	iv[16/4];
	unsigned int	ivLength;
}AESRunParam;


#endif	//	__nx_aes_ioctl_h__
