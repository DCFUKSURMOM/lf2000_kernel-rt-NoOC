#ifndef __nx_writeio_h__
#define __nx_writeio_h__

#define WriteIODW(Addr,Data) (*(volatile unsigned int*)Addr)=((unsigned int)Data)
#define WriteIOW(Addr,Data) (*(volatile unsigned short*)Addr)=((unsigned short)Data)
#define WriteIOB(Addr,Data) (*(volatile unsigned char*)Addr)=((unsigned char)Data)


#endif	//	__nx_writeio_h__
