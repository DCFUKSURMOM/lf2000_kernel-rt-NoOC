//------------------------------------------------------------------------------
//
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	Module     : 
//	File       : nx_idct.c
//	Description: 
//	Author     : ricky(ricky)
//	History    : 
//         
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//	includes
//------------------------------------------------------------------------------
#include <nx_idct.h>

static	struct NX_IDCT_RegisterSet *__g_pRegister = CNULL;

CBOOL	NX_IDCT_Initialize( void )
{
	static CBOOL bInit = CFALSE;

	if( CFALSE == bInit )
	{
		__g_pRegister = CNULL;
		bInit = CTRUE;
	}

	return CTRUE;
}

U32		NX_IDCT_GetNumberOfModule( void )
{
	return NUMBER_OF_IDCT_MODULE;
}

U32		NX_IDCT_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_IDCT_MODULE );
}

U32		NX_IDCT_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_IDCT_RegisterSet );
}

void	NX_IDCT_SetBaseAddress( U32 BaseAddress )
{
	__g_pRegister = (struct NX_IDCT_RegisterSet *)BaseAddress;
}

U32		NX_IDCT_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/// @name   Module Interface
//------------------------------------------------------------------------------

CBOOL NX_IDCT_OpenModule( void )
{
//	NX_IDCT_StartIDCT();
	NX_ASSERT( !NX_IDCT_CheckBusy() );
	return CTRUE;
}

CBOOL NX_IDCT_CloseModule( void )
{
	NX_ASSERT( !NX_IDCT_CheckBusy() );
	return CTRUE;
}

void NX_IDCT_SetClockBClkMode(NX_BCLKMODE bclk)
{
	register U32 regval;
	NX_ASSERT( CNULL != __g_pRegister );
	regval = __g_pRegister->CLKENB;
	regval |= bclk<<0;
	__g_pRegister->CLKENB = regval;
}

NX_BCLKMODE	NX_IDCT_GetClockBClkMode( void )
{
	return (NX_BCLKMODE)(__g_pRegister->CLKENB & 0x3);
}

//------------------------------------------------------------------------------
//	Interrupt implementation
//------------------------------------------------------------------------------

S32     NX_IDCT_GetInterruptNumber( void )
{
	return (S32)INTNUM_OF_IDCT_MODULE;
}

//------------------------------------------------------------------------------
//	IDCT Operation
//------------------------------------------------------------------------------

CBOOL NX_IDCT_CheckBusy( void )
{
	NX_ASSERT( CNULL != __g_pRegister );
	return (CBOOL)(!(__g_pRegister->INT_STATUS & 0x1));
}

void NX_IDCT_StartIDCT( void )
{
	register U32 regval;
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CONT;
	regval |= 1<<0;
	__g_pRegister->CONT = regval;
}

CBOOL NX_IDCT_ChkIDCTDone( void )
{
	NX_ASSERT( CNULL != __g_pRegister );
	return (CBOOL)(__g_pRegister->INT_STATUS & 0x1 );
}

void NX_IDCT_SetIDCTBuffReset( void )
{
	register U32 regval;
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->CONT;
	regval |= 0x1<<1;
	__g_pRegister->CONT = regval;
}

void NX_IDCT_SetIDCTOutRange( U32 Range )
{
	register U32 regval;
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( (15>=Range) && (0<=Range) );

	regval = __g_pRegister->CONT;
	regval &= ~(0xF<<2);
	regval |= Range<<2;		// -2^n~2^n-1, 0~2^n-1
	__g_pRegister->CONT = regval;
}

U32 NX_IDCT_GetIDCTOutRange( void )
{
	NX_ASSERT( CNULL != __g_pRegister );
	return ((__g_pRegister->CONT >> 0x2) & 0xF);
}

void NX_IDCT_SetIDCTOutSign( CBOOL Sign )
{
	register U32 regval;
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( (CFALSE==Sign) || (CTRUE==Sign) );

	regval = __g_pRegister->CONT;
	if( Sign ) // -2^n~2^n-1
		regval |= 0x1<<6;
	else		// 0~2^n-1
		regval &= ~(0x1<<6);
	__g_pRegister->CONT = regval;
}

CBOOL NX_IDCT_GetIDCTOutSign( void )
{
	NX_ASSERT( CNULL != __g_pRegister );
	return ((__g_pRegister->CONT >> 0x6) & 0x1);
}

void NX_IDCT_SetIDCTClampEnb( CBOOL bEnb )
{
	register U32 regval;
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( (CFALSE==bEnb) || (CTRUE==bEnb) );

	regval = __g_pRegister->CONT;
	if( bEnb )
		regval |= 0x1<<7;
	else
		regval &= ~(0x1<<7);
	__g_pRegister->CONT = regval;
}

CBOOL NX_IDCT_GetIDCTClampEnb( void )
{
	NX_ASSERT( CNULL != __g_pRegister );
	return ((__g_pRegister->CONT >> 0x7) & 0x1);
}

void NX_IDCT_Write8By8Block( U16 * iquan_out )
{
	register U32 *iq_out = (U32*)iquan_out;
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != iq_out && !((U32)iq_out & 0x3));
	__g_pRegister->BUF[ 0] = iq_out[ 0];
	__g_pRegister->BUF[ 1] = iq_out[ 1];
	__g_pRegister->BUF[ 2] = iq_out[ 2];
	__g_pRegister->BUF[ 3] = iq_out[ 3];
	__g_pRegister->BUF[ 4] = iq_out[ 4];
	__g_pRegister->BUF[ 5] = iq_out[ 5];
	__g_pRegister->BUF[ 6] = iq_out[ 6];
	__g_pRegister->BUF[ 7] = iq_out[ 7];
	__g_pRegister->BUF[ 8] = iq_out[ 8];
	__g_pRegister->BUF[ 9] = iq_out[ 9];
	__g_pRegister->BUF[10] = iq_out[10];
	__g_pRegister->BUF[11] = iq_out[11];
	__g_pRegister->BUF[12] = iq_out[12];
	__g_pRegister->BUF[13] = iq_out[13];
	__g_pRegister->BUF[14] = iq_out[14];
	__g_pRegister->BUF[15] = iq_out[15];
	__g_pRegister->BUF[16] = iq_out[16];
	__g_pRegister->BUF[17] = iq_out[17];
	__g_pRegister->BUF[18] = iq_out[18];
	__g_pRegister->BUF[19] = iq_out[19];
	__g_pRegister->BUF[20] = iq_out[20];
	__g_pRegister->BUF[21] = iq_out[21];
	__g_pRegister->BUF[22] = iq_out[22];
	__g_pRegister->BUF[23] = iq_out[23];
	__g_pRegister->BUF[24] = iq_out[24];
	__g_pRegister->BUF[25] = iq_out[25];
	__g_pRegister->BUF[26] = iq_out[26];
	__g_pRegister->BUF[27] = iq_out[27];
	__g_pRegister->BUF[28] = iq_out[28];
	__g_pRegister->BUF[29] = iq_out[29];
	__g_pRegister->BUF[30] = iq_out[30];
	__g_pRegister->BUF[31] = iq_out[31];
}

void NX_IDCT_Read8By8Block ( U16 * pidct_out )
{
	register U32 *idct_out = (U32*)pidct_out;
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( CNULL != idct_out && !((U32)idct_out & 0x3) );

	idct_out[ 0] = __g_pRegister->BUF[ 0];
	idct_out[ 1] = __g_pRegister->BUF[ 1];
	idct_out[ 2] = __g_pRegister->BUF[ 2];
	idct_out[ 3] = __g_pRegister->BUF[ 3];
	idct_out[ 4] = __g_pRegister->BUF[ 4];
	idct_out[ 5] = __g_pRegister->BUF[ 5];
	idct_out[ 6] = __g_pRegister->BUF[ 6];
	idct_out[ 7] = __g_pRegister->BUF[ 7];
	idct_out[ 8] = __g_pRegister->BUF[ 8];
	idct_out[ 9] = __g_pRegister->BUF[ 9];
	idct_out[10] = __g_pRegister->BUF[10];
	idct_out[11] = __g_pRegister->BUF[11];
	idct_out[12] = __g_pRegister->BUF[12];
	idct_out[13] = __g_pRegister->BUF[13];
	idct_out[14] = __g_pRegister->BUF[14];
	idct_out[15] = __g_pRegister->BUF[15];
	idct_out[16] = __g_pRegister->BUF[16];
	idct_out[17] = __g_pRegister->BUF[17];
	idct_out[18] = __g_pRegister->BUF[18];
	idct_out[19] = __g_pRegister->BUF[19];
	idct_out[20] = __g_pRegister->BUF[20];
	idct_out[21] = __g_pRegister->BUF[21];
	idct_out[22] = __g_pRegister->BUF[22];
	idct_out[23] = __g_pRegister->BUF[23];
	idct_out[24] = __g_pRegister->BUF[24];
	idct_out[25] = __g_pRegister->BUF[25];
	idct_out[26] = __g_pRegister->BUF[26];
	idct_out[27] = __g_pRegister->BUF[27];
	idct_out[28] = __g_pRegister->BUF[28];
	idct_out[29] = __g_pRegister->BUF[29];
	idct_out[30] = __g_pRegister->BUF[30];
	idct_out[31] = __g_pRegister->BUF[31];
}
