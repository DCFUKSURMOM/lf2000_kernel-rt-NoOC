//------------------------------------------------------------------------------
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: ECID
//	File		: nx_ecid.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_ecid.h"

static	struct NX_ECID_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Initialize of prototype enviroment & local variables.
 *	@return		\b CTRUE	indicate that Initialize is successed.\n
 *				\b CFALSE	indicate that Initialize is failed.\n
 *	@see		NX_ECID_GetNumberOfModule
 */
CBOOL	NX_ECID_Initialize( void )
{
	static CBOOL bInit = CFALSE;

	if( CFALSE == bInit )
	{
		__g_pRegister = CNULL;
		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@see		NX_ECID_Initialize
 */
U32		NX_ECID_GetNumberOfModule( void )
{
	return NUMBER_OF_ECID_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see									NX_ECID_GetSizeOfRegisterSet,
 *				NX_ECID_SetBaseAddress,		NX_ECID_GetBaseAddress,
 *				NX_ECID_OpenModule,			NX_ECID_CloseModule,
 *				NX_ECID_CheckBusy,			NX_ECID_CanPowerDown
 */
U32		NX_ECID_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_ECID_MODULE + (OFFSET_OF_ECID_MODULE * 0) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_ECID_GetPhysicalAddress,
 *				NX_ECID_SetBaseAddress,		NX_ECID_GetBaseAddress,
 *				NX_ECID_OpenModule,			NX_ECID_CloseModule,
 *				NX_ECID_CheckBusy,			NX_ECID_CanPowerDown
 */
U32		NX_ECID_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_ECID_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_ECID_GetPhysicalAddress,	NX_ECID_GetSizeOfRegisterSet,
 *											NX_ECID_GetBaseAddress,
 *				NX_ECID_OpenModule,			NX_ECID_CloseModule,
 *				NX_ECID_CheckBusy,			NX_ECID_CanPowerDown
 */
void	NX_ECID_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	__g_pRegister = (struct NX_ECID_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_ECID_GetPhysicalAddress,	NX_ECID_GetSizeOfRegisterSet,
 *				NX_ECID_SetBaseAddress,
 *				NX_ECID_OpenModule,			NX_ECID_CloseModule,
 *				NX_ECID_CheckBusy,			NX_ECID_CanPowerDown
 */
U32		NX_ECID_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_ECID_GetPhysicalAddress,	NX_ECID_GetSizeOfRegisterSet,
 *				NX_ECID_SetBaseAddress,		NX_ECID_GetBaseAddress,
 *											NX_ECID_CloseModule,
 *				NX_ECID_CheckBusy,			NX_ECID_CanPowerDown
 */
CBOOL	NX_ECID_OpenModule( void )
{
		return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_ECID_GetPhysicalAddress,	NX_ECID_GetSizeOfRegisterSet,
 *				NX_ECID_SetBaseAddress,		NX_ECID_GetBaseAddress,
 *				NX_ECID_OpenModule,
 *				NX_ECID_CheckBusy,			NX_ECID_CanPowerDown
 */
CBOOL	NX_ECID_CloseModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_ECID_GetPhysicalAddress,	NX_ECID_GetSizeOfRegisterSet,
 *				NX_ECID_SetBaseAddress,		NX_ECID_GetBaseAddress,
 *				NX_ECID_OpenModule,			NX_ECID_CloseModule,
 *											NX_ECID_CanPowerDown
 */
CBOOL	NX_ECID_CheckBusy( void )
{
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_ECID_GetPhysicalAddress,	NX_ECID_GetSizeOfRegisterSet,
 *				NX_ECID_SetBaseAddress,		NX_ECID_GetBaseAddress,
 *				NX_ECID_OpenModule,			NX_ECID_CloseModule,
 *				NX_ECID_CheckBusy
 */
CBOOL	NX_ECID_CanPowerDown( void )
{
	if( NX_ECID_CheckBusy() )
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
// ECID Operation.
//------------------------------------------------------------------------------
/**
 *	@brief		Get ECID
 *	@param[out]	pID	Buffer to save ECID.
 *	@return		None.
 *	@remark		\e pID indicate U32 type array that size is 4. \n
 *				If buffer is small, then buffer can overflow.\n
 *	@code
 *				U32 ECID[4];
 *				...
 *				NX_ECID_GetECID( ECID );
 *				...
 *	@endcode
 *	@see								NX_ECID_GetChipName,
 *				NX_ECID_GetGUID,		NX_ECID_GetBondingID
 */
void	NX_ECID_GetECID( U32 *pID )
{
	const U32 CS_MASK		= 1 << 6;
	const U32 SIGDEV_MASK	= 1 << 5;
	const U32 FSET_MASK		= 1 << 4;
	const U32 PRCHG_MASK	= 1 << 3;

	NX_ASSERT( CNULL != pID );
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->EC	=											PRCHG_MASK;
//	__g_pRegister->EC	= CS_MASK | SIGDEV_MASK;
//	__g_pRegister->EC	= CS_MASK | SIGDEV_MASK |	FSET_MASK;
//	__g_pRegister->EC	= CS_MASK |				FSET_MASK;
//	__g_pRegister->EC	= CS_MASK |				FSET_MASK	|	PRCHG_MASK;
//	__g_pRegister->EC	= CS_MASK |								PRCHG_MASK;
//	__g_pRegister->EC	=											PRCHG_MASK;
	WriteIODW(&__g_pRegister->EC,											PRCHG_MASK);
	WriteIODW(&__g_pRegister->EC, 	CS_MASK | SIGDEV_MASK);
	WriteIODW(&__g_pRegister->EC, 	CS_MASK | SIGDEV_MASK |	FSET_MASK);
	WriteIODW(&__g_pRegister->EC, 	CS_MASK |				FSET_MASK);
	WriteIODW(&__g_pRegister->EC, 	CS_MASK |				FSET_MASK	|	PRCHG_MASK);
	WriteIODW(&__g_pRegister->EC, 	CS_MASK |								PRCHG_MASK);
	WriteIODW(&__g_pRegister->EC,											PRCHG_MASK);

	*pID++ = __g_pRegister->IDREG[0];
	*pID++ = __g_pRegister->IDREG[1];
	*pID++ = __g_pRegister->IDREG[2];
	*pID++ = __g_pRegister->IDREG[3];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Chip Name
 *	@param[out]	pName	Buffer to save Chip Name
 *	@return		None.
 *	@remark		\e pName indicate \b char type array that size is 49. \n
 *				If buffer is small, then buffer can overflow.
 *	@code
 *				char ChipName[49];
 *				NX_ECID_GetChipName( ChipName );
 *	@endcode
 *	@see		NX_ECID_GetECID,
 *				NX_ECID_GetGUID,		NX_ECID_GetBondingID
 */
void	NX_ECID_GetChipName( char *pName )
{
	U32 i, j=0;

	NX_ASSERT( CNULL != pName );
	NX_ASSERT( CNULL != __g_pRegister );

	for( i=0; i<48; i++)
	{
		*pName++ = __g_pRegister->CHIPNAME[j++];
	}

	*pName = 0; // null
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Group ID
 *	@param[out]	pGUID	Buffer to save Group ID
 *	@return		None.
 *	@remark		\e pGUID indicate U32 type array that size is 4. \n
 *				If buffer is small, then buffer can overflow.
 *	@code
 *				U32 GUID[4];
 *				NX_ECID_GetGUID( GUID );
 *	@endcode
 *	@see		NX_ECID_GetECID,		NX_ECID_GetChipName,
 *										NX_ECID_GetBondingID
 */
void	NX_ECID_GetGUID( U32 *pGUID )
{
	NX_ASSERT( CNULL != pGUID );
	NX_ASSERT( CNULL != __g_pRegister );

	*pGUID++ = __g_pRegister->GUID[0];
	*pGUID++ = __g_pRegister->GUID[1];
	*pGUID++ = __g_pRegister->GUID[2];
	*pGUID++ = __g_pRegister->GUID[3];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Bonding ID
 *	@return		Bonding ID.
 *	@see		NX_ECID_GetECID,		NX_ECID_GetChipName,
 *				NX_ECID_GetGUID
 */
U32		NX_ECID_GetBondingID( void )
{
	const U32 BONDINGID_MASK = 0x03;

	NX_ASSERT( CNULL != __g_pRegister );

	return (__g_pRegister->EC & BONDINGID_MASK);
}
