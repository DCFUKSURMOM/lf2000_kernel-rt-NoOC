//------------------------------------------------------------------------------
//
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: GPIO
//	File		: nx_gpio.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_gpio.h"

U32 __g_NX_GPIO_VALID_BIT[NUMBER_OF_GPIO_MODULE] = {
			0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x01FFFFFF		// A, B, C, D
		};

U32	__g_NX_GPIO_VALID_NUM_OF_INT[NUMBER_OF_GPIO_MODULE] = {
			32, 32, 32, 25		// A, B, C, D
		};

U32	__g_NX_GPIO_VALID_INT_MASK[NUMBER_OF_GPIO_MODULE] = {
			0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x01FFFFFF		// A, B, C, D
		};


static	struct
{
	struct NX_GPIO_RegisterSet *pRegister;

} __g_ModuleVariables[NUMBER_OF_GPIO_MODULE] = { {CNULL,}, };

enum { NX_GPIO_MAX_BIT = 32 };

//------------------------------------------------------------------------------
// inline Function
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief		Set 1bit value
 *	@param[in]	Value	This Value is changed when SetBit() executed.
 *	@param[in]	Bit		Bit number
 *	@param[in]	Enable	\b CTRUE( Set ).\n
 *						\b CFALSE( Clear )
 *	@return		None.
 *	@see		NX_GPIO_GetBit
 */
__inline void NX_GPIO_SetBit
(
	volatile U32* Value,
	U32 Bit,
	CBOOL Enable
)
{
	register U32 newvalue;

	NX_ASSERT( CNULL != Value );
	NX_ASSERT( NX_GPIO_MAX_BIT > Bit );

	newvalue = *Value;

	newvalue &=		~(1UL		<< Bit );
	newvalue |=		(U32)Enable << Bit ;

//	*Value = newvalue;
	WriteIODW(Value, newvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get bit value
 *	@param[in]	Value	Check this value of 1bit state
 *	@param[in]	Bit		Bit number
 *	@return		\b CTRUE	indicate that bit is Seted.\n
 *				\b CFALSE	indicate that bit is cleared.
 *	@see		NX_GPIO_SetBit
 */
__inline CBOOL NX_GPIO_GetBit
(
	U32 Value,
	U32 Bit
)
{
	NX_ASSERT( NX_GPIO_MAX_BIT > Bit );

	return (CBOOL)(( Value >> Bit ) & ( 1UL ) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set 2bit value
 *	@param[in]	Value		This Value is changed when SetBit2() executed.
 *	@param[in]	Bit			Bit number
 *	@param[in]	BitValue	Set value of bit
 *	@return		None.
 *	@see		NX_GPIO_GetBit2
 */
__inline void NX_GPIO_SetBit2
(
	volatile U32* Value,
	U32 Bit,
	U32 BitValue
)
{
	register U32 newvalue = *Value;

	NX_ASSERT( CNULL != Value );
	NX_ASSERT( NX_GPIO_MAX_BIT > Bit );

	newvalue = (U32)(newvalue & ~(3UL<<(Bit*2)));
	newvalue = (U32)(newvalue | (BitValue<<(Bit*2)));
//	*Value = newvalue;
	WriteIODW(Value, newvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get 2bit value
 *	@param[in]	Value	Check this value of 2bit state
 *	@param[in]	Bit		Bit number
 *	@return		2bit value
 *	@see		NX_GPIO_SetBit2
 */

__inline U32 NX_GPIO_GetBit2
(
	U32 Value,
	U32 Bit
)
{
	NX_ASSERT( NX_GPIO_MAX_BIT > Bit );

	return (U32)((U32)(Value>>(Bit*2)) & 3UL);
}

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.\n
 *	@see									NX_GPIO_GetNumberOfModule
 */
CBOOL	NX_GPIO_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_OF_GPIO_MODULE; i++ )
		{
			__g_ModuleVariables[i].pRegister = CNULL;
		}

		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 *	@see		NX_GPIO_Initialize
 */
U32		NX_GPIO_GetNumberOfModule( void )
{
	return NUMBER_OF_GPIO_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@param[in]	ModuleIndex		A index of module.
 *	@return		Module's physical address
 *	@see										NX_GPIO_GetSizeOfRegisterSet,
 *				NX_GPIO_SetBaseAddress,			NX_GPIO_GetBaseAddress,
 *				NX_GPIO_OpenModule,				NX_GPIO_CloseModule,
 *				NX_GPIO_CheckBusy,				NX_GPIO_CanPowerDown
 */
U32		NX_GPIO_GetPhysicalAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );

	return	(U32)( PHY_BASEADDR_GPIO_MODULE + (OFFSET_OF_GPIO_MODULE * ModuleIndex) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_GPIO_GetPhysicalAddress,
 *				NX_GPIO_SetBaseAddress,			NX_GPIO_GetBaseAddress,
 *				NX_GPIO_OpenModule,				NX_GPIO_CloseModule,
 *				NX_GPIO_CheckBusy,				NX_GPIO_CanPowerDown
 */
U32		NX_GPIO_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_GPIO_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		A index of module.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_GPIO_GetPhysicalAddress,		NX_GPIO_GetSizeOfRegisterSet,
 *												NX_GPIO_GetBaseAddress,
 *				NX_GPIO_OpenModule,				NX_GPIO_CloseModule,
 *				NX_GPIO_CheckBusy,				NX_GPIO_CanPowerDown
 */
void	NX_GPIO_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );

	__g_ModuleVariables[ModuleIndex].pRegister = (struct NX_GPIO_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@param[in]	ModuleIndex		A index of module.
 *	@return		Module's base address.
 *	@see		NX_GPIO_GetPhysicalAddress,		NX_GPIO_GetSizeOfRegisterSet,
 *				NX_GPIO_SetBaseAddress,
 *				NX_GPIO_OpenModule,				NX_GPIO_CloseModule,
 *				NX_GPIO_CheckBusy,				NX_GPIO_CanPowerDown
 */
U32		NX_GPIO_GetBaseAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@param[in]	ModuleIndex		A index of module.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_GPIO_GetPhysicalAddress,		NX_GPIO_GetSizeOfRegisterSet,
 *				NX_GPIO_SetBaseAddress,			NX_GPIO_GetBaseAddress,
 *												NX_GPIO_CloseModule,
 *				NX_GPIO_CheckBusy,				NX_GPIO_CanPowerDown
 */
CBOOL	NX_GPIO_OpenModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@param[in]	ModuleIndex		A index of module.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_GPIO_GetPhysicalAddress,		NX_GPIO_GetSizeOfRegisterSet,
 *				NX_GPIO_SetBaseAddress,			NX_GPIO_GetBaseAddress,
 *				NX_GPIO_OpenModule,
 *				NX_GPIO_CheckBusy,				NX_GPIO_CanPowerDown
 */
CBOOL	NX_GPIO_CloseModule( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@param[in]	ModuleIndex		A index of module.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_GPIO_GetPhysicalAddress,		NX_GPIO_GetSizeOfRegisterSet,
 *				NX_GPIO_SetBaseAddress,			NX_GPIO_GetBaseAddress,
 *				NX_GPIO_OpenModule,				NX_GPIO_CloseModule,
 *												NX_GPIO_CanPowerDown
 */
CBOOL	NX_GPIO_CheckBusy( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@param[in]	ModuleIndex		A index of module.
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_GPIO_GetPhysicalAddress,	NX_GPIO_GetSizeOfRegisterSet,
 *				NX_GPIO_SetBaseAddress,		NX_GPIO_GetBaseAddress,
 *				NX_GPIO_OpenModule,			NX_GPIO_CloseModule,
 *				NX_GPIO_CheckBusy
 */
CBOOL	NX_GPIO_CanPowerDown( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );

	return CTRUE;
}


//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see											NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
S32		NX_GPIO_GetInterruptNumber( void )
{
	return	INTNUM_OF_GPIO_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	IntNum	Interrupt Number. ( 0 ~ 31 ), if ModuleIndex is GPIOD then IntNum is only 0 ~ 24.
 *	@param[in]	Enable	\b CTRUE	indicate that Interrupt Enable. \n
 *						\b CFALSE	indicate that Interrupt Disable.
 *	@return		None.
 *	@see		NX_GPIO_GetInterruptNumber,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
void	NX_GPIO_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable )
{
	register	struct NX_GPIO_RegisterSet *pRegister;
	register	U32 ReadValue ;

	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( __g_NX_GPIO_VALID_NUM_OF_INT[ModuleIndex] > (U32)IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue = pRegister->GPIOxINTENB;

	ReadValue &= ~( (U32)1		<< IntNum);
	ReadValue |=	( (U32)Enable	<< IntNum );

//	pRegister->GPIOxINTENB = ReadValue;
	WriteIODW(&pRegister->GPIOxINTENB, ReadValue);

	NX_GPIO_SetDetectEnable(ModuleIndex, IntNum, Enable);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	IntNum	Interrupt Number.
 *	@return		\b CTRUE	indicate that Interrupt is enabled. \n
 *				\b CFALSE	indicate that Interrupt is disabled.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *													NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
CBOOL	NX_GPIO_GetInterruptEnable( U32 ModuleIndex, S32 IntNum )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( __g_NX_GPIO_VALID_NUM_OF_INT[ModuleIndex] > (U32)IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxINTENB & (1UL << IntNum ) )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set GPIO0 interrupt enable or disable. \n
 *							...
 *							- EnableFlag[31] : Set GPIO31 interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
void	NX_GPIO_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->GPIOxINTENB = (EnableFlag & __g_NX_GPIO_VALID_INT_MASK[ModuleIndex] );
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->GPIOxINTENB, (EnableFlag & __g_NX_GPIO_VALID_INT_MASK[ModuleIndex] ));

	NX_GPIO_SetDetectEnable32(ModuleIndex, EnableFlag);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : GPIO0 interrupt's setting value. \n
 *				...
 *				- Return Value[31] : GPIO31 interrupt's setting value. \n
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *													NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
U32		NX_GPIO_GetInterruptEnable32( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxINTENB & __g_NX_GPIO_VALID_INT_MASK[ModuleIndex] );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	IntNum	Interrupt Number.
 *	@return		\b CTRUE	indicate that Pending is seted. \n
 *				\b CFALSE	indicate that Pending is Not Seted.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
CBOOL	NX_GPIO_GetInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( __g_NX_GPIO_VALID_NUM_OF_INT[ModuleIndex] > (U32)IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxDET & (1UL << IntNum ) )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : GPIO0 pending state. \n
 *				...
 *				- Return Value[31] : GPIO31 pending state. \n
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *													NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
U32		NX_GPIO_GetInterruptPending32( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return(U32)( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxDET & __g_NX_GPIO_VALID_INT_MASK[ModuleIndex] );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	IntNum	Interrupt number.
 *	@return		None.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
void	NX_GPIO_ClearInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( __g_NX_GPIO_VALID_NUM_OF_INT[ModuleIndex] > (U32)IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDET = 1UL << IntNum;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDET, 1UL << IntNum);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : GPIO0 pending bit. \n
 *									...
 *								- PendingFlag[31] : GPIO31 pending bit. \n
 *	@return		None.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *													NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
void	NX_GPIO_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDET = (PendingFlag & __g_NX_GPIO_VALID_INT_MASK[ModuleIndex]);
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDET, (PendingFlag & __g_NX_GPIO_VALID_INT_MASK[ModuleIndex]));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	Enable	\b CTRUE	indicate that Set to all interrupt enable. \n
 *						\b CFALSE	indicate that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
void	NX_GPIO_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable )
{
	register		U32	setvalue;

	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( Enable )
	{	setvalue = ~(0UL);	}
	else
	{	setvalue = 0UL;	}

//	__g_ModuleVariables[ModuleIndex].pRegister->GPIOxINTENB	= setvalue;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->GPIOxINTENB, setvalue);

	NX_GPIO_SetInterruptEnable32(ModuleIndex, setvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@return		\b CTRUE	indicate that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicate that All interrupt is disabled.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *													NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
CBOOL	NX_GPIO_GetInterruptEnableAll( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxINTENB )
	{
		return CTRUE;
	}
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@return		\b CTRUE	indicate that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicate that All pending is NOT seted.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,
 *				NX_GPIO_ClearInterruptPendingAll,	NX_GPIO_GetInterruptPendingNumber
 */
CBOOL	NX_GPIO_GetInterruptPendingAll( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxDET )
	{
		return CTRUE;
	}
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@return		None.
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *													NX_GPIO_GetInterruptPendingNumber
 */
void	NX_GPIO_ClearInterruptPendingAll( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDET = (U32)(0xFFFFFFFF);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_GPIO_GetInterruptNumber,			NX_GPIO_SetInterruptEnable,
 *				NX_GPIO_GetInterruptEnable,			NX_GPIO_SetInterruptEnable32,
 *				NX_GPIO_GetInterruptEnable32,		NX_GPIO_GetInterruptPending,
 *				NX_GPIO_GetInterruptPending32,		NX_GPIO_ClearInterruptPending,
 *				NX_GPIO_ClearInterruptPending32,	NX_GPIO_SetInterruptEnableAll,
 *				NX_GPIO_GetInterruptEnableAll,		NX_GPIO_GetInterruptPendingAll,
 *				NX_GPIO_ClearInterruptPendingAll
 */
S32		NX_GPIO_GetInterruptPendingNumber( U32 ModuleIndex )	// -1 if None
{
	register struct NX_GPIO_RegisterSet	*pRegister;
	register	U32 intnum;
	register	U32 intpend;

	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	intpend = (pRegister->GPIOxINTENB & pRegister->GPIOxDET);

	for( intnum=0 ; intnum<32 ; intnum++ )
	{
		if( 0 != (intpend & (1UL<<intnum)) )
		{
			return intnum;
		}
	}

	return -1;
}

//------------------------------------------------------------------------------
// GPIO Operation.
//------------------------------------------------------------------------------

/**
 *	@brief		Set interrupt mode
 *	@param[in]	ModuleIndex	A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@param[in]	IntMode		GPIO interrupt detect mode
 *	@return		None.
 *	@see										NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
void				NX_GPIO_SetInterruptMode( U32 ModuleIndex, U32 BitNumber, NX_GPIO_INTMODE IntMode )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( NX_GPIO_INTMODE_RISINGNFALLINGEDGE >= IntMode );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_GPIO_SetBit2( &__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDETMODE[BitNumber/16], BitNumber%16, (U32)IntMode & 0x3UL );
	NX_GPIO_SetBit( &__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDETMODEEX, BitNumber, (U32)(IntMode>>2) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get interrupt mode
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@return		GPIO interrupt detect mode
 *	@see		NX_GPIO_SetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
NX_GPIO_INTMODE	NX_GPIO_GetInterruptMode( U32 ModuleIndex, U32 BitNumber )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (NX_GPIO_INTMODE)(NX_GPIO_GetBit2( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxDETMODE[BitNumber/16], BitNumber%16 ) |
							NX_GPIO_GetBit(__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDETMODEEX, BitNumber )<<2);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get gpio output enable
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@param[in]	OutputEnb	\b CTRUE	indicate that Output Mode. \n
 *							\b CFALSE	indicate that Input Mode.
 *	@return		None.
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *												NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
void	NX_GPIO_SetOutputEnable ( U32 ModuleIndex, U32 BitNumber, CBOOL OutputEnb )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( (0==OutputEnb) || (1==OutputEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_GPIO_SetBit( &__g_ModuleVariables[ModuleIndex].pRegister->GPIOxOUTENB, BitNumber, OutputEnb );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get gpio Event Detect
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@param[in]	OutputEnb	\b CTRUE	indicate that Event Detect Enabled. \n
 *							\b CFALSE	indicate that Event Detect Disabled.
 *	@return		None.
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *												NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
CBOOL			NX_GPIO_GetDetectEnable ( U32 ModuleIndex, U32 BitNumber )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return NX_GPIO_GetBit( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxDETENB, BitNumber );
}

CBOOL			NX_GPIO_GetDetectEnable32 ( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->GPIOxDETENB;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Event Detect Enable
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@return		GPIO interrupt detect mode
 *	@see		NX_GPIO_SetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
void			NX_GPIO_SetDetectEnable ( U32 ModuleIndex, U32 BitNumber, CBOOL DetectEnb )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( (0==DetectEnb) || (1==DetectEnb) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_GPIO_SetBit( &__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDETENB, BitNumber, DetectEnb );
}

void			NX_GPIO_SetDetectEnable32 ( U32 ModuleIndex, U32 EnableFlag )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->GPIOxDETENB, EnableFlag );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get gpio output enable
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@return		\b CTRUE	indicate that GPIO's current Setting is Output Mode.\n
 *				\b CFALSE	indicate that GPIO's current Setting is Input Mode.
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
CBOOL	NX_GPIO_GetOutputEnable ( U32 ModuleIndex, U32 BitNumber )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return NX_GPIO_GetBit( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxOUTENB, BitNumber );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set gpio output value
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@param[in]	Value		\b CTRUE	indicate that High Level. \n
 *							\b CFALSE	indicate that Low Level.
 *	@return		None.
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *												NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
void	NX_GPIO_SetOutputValue	( U32 ModuleIndex, U32 BitNumber, CBOOL Value )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( (0==Value) || (1==Value) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_GPIO_SetBit( &__g_ModuleVariables[ModuleIndex].pRegister->GPIOxOUT, BitNumber, Value );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get gpio output value
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@return		\b CTRUE	indicate that GPIO's output value is High Level.\n
 *				\b CFALSE	indicate that GPIO's output value is Low Level.
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
CBOOL	NX_GPIO_GetOutputValue	( U32 ModuleIndex, U32 BitNumber )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return NX_GPIO_GetBit( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxOUT, BitNumber );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get gpio input value
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@return		\b CTRUE	indicate that GPIO's input value is High Level.\n
 *				\b CFALSE	indicate that GPIO's input value is Low Level.
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *												NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
CBOOL	NX_GPIO_GetInputValue		( U32 ModuleIndex, U32 BitNumber )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );

	return NX_GPIO_GetBit( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxPAD, BitNumber );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Pull up of GPIO Pin
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@param[in]	enable		\b CTRUE	indicate that Pull Up. \n
 *							\b CFALSE	indicate that NOT Pull Up.
 *	@return		None.
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
void	NX_GPIO_SetPullUpEnable ( U32 ModuleIndex, U32 BitNumber, CBOOL enable)
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( (0==enable) || (1==enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_GPIO_SetBit( &__g_ModuleVariables[ModuleIndex].pRegister->GPIOxPUENB, BitNumber, enable );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get pull up value of GPIO Pin
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@return		GPIO pull up value
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *												NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
CBOOL	NX_GPIO_GetPullUpEnable ( U32 ModuleIndex, U32 BitNumber )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return NX_GPIO_GetBit( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxPUENB, BitNumber );
}


//--------------------------------------------------------------------------
// Pin Configuration
//--------------------------------------------------------------------------
/**
 *	@brief		Set PAD Fuction
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@param[in]	padfunc		Pad Function
 *	@return		None
 *	@remarks		Each gpio pins can use to GPIO Pin or Alternate Function 0 or Alternate Function 1 or \n
 *				Alternate Function 2. So This function Sets gpio pin's function.
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,
 *				NX_GPIO_GetPadFunction,			NX_GPIO_GetValidBit
 */
void	NX_GPIO_SetPadFunction( U32 ModuleIndex, U32 BitNumber, NX_GPIO_PADFUNC padfunc )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( NX_GPIO_PADFUNC_3 >= padfunc );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	NX_GPIO_SetBit2( &__g_ModuleVariables[ModuleIndex].pRegister->GPIOxALTFN[BitNumber/16], BitNumber%16, (U32)padfunc );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get PAD Fuction
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@param[in]	BitNumber	Bit number ( 0 ~ 31 ), if ModuleIndex is GPIOD then Bit number is only 0 ~ 24.
 *	@return		None
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *												NX_GPIO_GetValidBit
 */
NX_GPIO_PADFUNC	NX_GPIO_GetPadFunction( U32 ModuleIndex, U32 BitNumber )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );
	NX_ASSERT( 0 != (__g_NX_GPIO_VALID_BIT[ModuleIndex] & (1 << BitNumber)) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (NX_GPIO_PADFUNC)NX_GPIO_GetBit2( __g_ModuleVariables[ModuleIndex].pRegister->GPIOxALTFN[BitNumber/16], BitNumber%16 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get GPIO's valid bit
 *	@param[in]	ModuleIndex		A index of module. (0:GPIOA, 1:GPIOB, 2:GPIOC, 3:GPIOD )
 *	@return		GPIO's valid bit
 *	@remarks	Each bit's indicate corresponding GPIO pin. If retun value is 0x0FF, then
 *				This indicate that GPIO have eight pins ( GPIO0 ~ GPIO7 ).
 *	@see		NX_GPIO_SetInterruptMode,		NX_GPIO_GetInterruptMode,
 *				NX_GPIO_SetOutputEnable,		NX_GPIO_GetOutputEnable,
 *				NX_GPIO_SetOutputValue,			NX_GPIO_GetOutputValue,
 *				NX_GPIO_GetInputValue,			NX_GPIO_SetPullUpEnable,
 *				NX_GPIO_GetPullUpEnable,		NX_GPIO_SetPadFunction,
 *				NX_GPIO_GetPadFunction
 */
U32		NX_GPIO_GetValidBit( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_OF_GPIO_MODULE > ModuleIndex );

	return __g_NX_GPIO_VALID_BIT[ModuleIndex];
}
