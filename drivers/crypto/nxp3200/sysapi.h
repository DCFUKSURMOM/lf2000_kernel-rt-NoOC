#ifndef	__sysapi_h__
#define	__sysapi_h__

void *NxMemAlloc( unsigned int size );
void *NxMemCalloc(unsigned int num, unsigned int typeSize);
void NxMemFree( void *ptr );
void NxMemSet( void *ptr, int data, unsigned int size );
void NxMemCpy( void *dst, void *src, unsigned int size );

#endif	//	__sysapi_h__
