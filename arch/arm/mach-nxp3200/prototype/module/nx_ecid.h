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
//	File		: nx_ecid.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_ECID_H__
#define __NX_ECID_H__

#include "../nx_base/nx_prototype.h"

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	ECID ECID
//------------------------------------------------------------------------------
//@{

	/// @brief	ECID Module's Register List
	struct	NX_ECID_RegisterSet
	{
		volatile U32	IDREG[4];				///< 0x00 ~ 0x0C : ID Register
		volatile U8		CHIPNAME[48];			///< 0x10 ~ 0x3C : Chip Name Register
		volatile U32	_Reserved;				///< 0x40			: Reserved region
		volatile U32	GUID[4];				///< 0x44 ~ 0x50 : GUID Register
		volatile U32	EC;						///< 0x54			: Control Register
	};

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_ECID_Initialize( void );
U32		NX_ECID_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_ECID_GetPhysicalAddress( void );
U32		NX_ECID_GetSizeOfRegisterSet( void );
void	NX_ECID_SetBaseAddress( U32 BaseAddress );
U32		NX_ECID_GetBaseAddress( void );
CBOOL	NX_ECID_OpenModule( void );
CBOOL	NX_ECID_CloseModule( void );
CBOOL	NX_ECID_CheckBusy( void );
CBOOL	NX_ECID_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///		@name	ECID Operation.
//@{
void	NX_ECID_GetECID( U32 *pID );
void	NX_ECID_GetChipName( char *pName );
void	NX_ECID_GetGUID( U32 *pGUID );
U32		NX_ECID_GetBondingID( void );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_ECID_H__
