//------------------------------------------------------------------------------
//
//  Copyright (C) 2009 Nexell Co., All Rights Reserved
//  Nexell Co. Proprietary & Confidential
//
//  Module     :
//  File       : nx_idct.h
//  Description:
//  Author     : ricky(ricky)
//  Export     :
//  History    :
//------------------------------------------------------------------------------
#ifndef _NX_IDCT_H
#define _NX_IDCT_H

//------------------------------------------------------------------------------
//  includes
//------------------------------------------------------------------------------
#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @brief  IDCT register set structure
//------------------------------------------------------------------------------
struct NX_IDCT_RegisterSet
{
	volatile U32 BUF[32];			// 0x000~0x07C
	volatile U32 CONT;				// 0x080
	volatile U32 INT_ENB;			// 0x084
	volatile U32 INT_STATUS;		// 0x088
			 U32 __Reserved[(0x7C0-0x08C)/4];	// 0x08C~7BC
	volatile U32 CLKENB;			// 0x7C0
};

typedef enum 
{
	NX_IDCT_RANGE_2048 = 0,
	NX_IDCT_RANGE_256  = 1
}NX_IDCT_RANGE;

//------------------------------------------------------------------------------
/// @function  NX_IDCT
/// @brief  IDCT
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// @name   Module Interface
//@{
CBOOL   NX_IDCT_Initialize( void );
U32     NX_IDCT_GetNumberOfModule( void );
//@} 

//------------------------------------------------------------------------------
///  @name   Basic Interface
//@{
U32     NX_IDCT_GetPhysicalAddress( void );
U32     NX_IDCT_GetSizeOfRegisterSet( void );
void    NX_IDCT_SetBaseAddress( U32 BaseAddress );
U32     NX_IDCT_GetBaseAddress( void );
CBOOL   NX_IDCT_OpenModule( void );
CBOOL   NX_IDCT_CloseModule( void );
//@} 

//------------------------------------------------------------------------------
///  @name   Interrupt Interface
//@{
S32     NX_IDCT_GetInterruptNumber( void );
//@}

//------------------------------------------------------------------------------
///  @name   Clock Control Interface
//@{

void		NX_IDCT_SetClockBClkMode( NX_BCLKMODE mode );
NX_BCLKMODE	NX_IDCT_GetClockBClkMode( void );

//@}

//--------------------------------------------------------------------------
/// @name   Module customized operations
//--------------------------------------------------------------------------
//@{

CBOOL NX_IDCT_CheckBusy( void );
void NX_IDCT_StartIDCT( void );
CBOOL NX_IDCT_ChkIDCTDone( void );
void NX_IDCT_SetIDCTBuffReset( void );
void NX_IDCT_SetIDCTOutRange( U32 Range );
U32 NX_IDCT_GetIDCTOutRange( void );
void NX_IDCT_SetIDCTOutSign( CBOOL Sign );
CBOOL NX_IDCT_GetIDCTOutSign( void );
void NX_IDCT_SetIDCTClampEnb( CBOOL bEnb );
CBOOL NX_IDCT_GetIDCTClampEnb( void );
void NX_IDCT_Write8By8Block( U16 * iquan_out );
void NX_IDCT_Read8By8Block ( U16 * pidct_out );
//@}

#ifdef	__cplusplus
}
#endif

#endif // _NX_IDCT_H
