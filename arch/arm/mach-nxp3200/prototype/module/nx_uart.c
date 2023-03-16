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
//	Module		: UART
//	File		: nx_uart.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_uart.h"

#define NUMBER_UART_MODULE	(6)

static	struct
{
	struct NX_UART_RegisterSet *pRegister;

} __g_ModuleVariables[NUMBER_UART_MODULE] = { {CNULL, }, };

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicates that Initialize is successed.\n
 *			\b CFALSE indicates that Initialize is failed.\n
 *	@see	NX_UART_GetNumberOfModule
 */
CBOOL	NX_UART_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_UART_MODULE; i++ )
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
 *	@see		NX_UART_Initialize
 */
U32		NX_UART_GetNumberOfModule( void )
{
	return NUMBER_UART_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Module's physical address
 *	@see		NX_UART_GetSizeOfRegisterSet,
 *				NX_UART_SetBaseAddress,		NX_UART_GetBaseAddress,
 *				NX_UART_OpenModule,			NX_UART_CloseModule,
 *				NX_UART_CheckBusy,			NX_UART_CanPowerDown
 */
U32		NX_UART_GetPhysicalAddress( U32 ModuleIndex )
{
	static const U32 UartPhysicalAddr[NUMBER_UART_MODULE] =
					{
						PHY_BASEADDR_UART0_MODULE, PHY_BASEADDR_UART1_MODULE, PHY_BASEADDR_UART2_MODULE,
						PHY_BASEADDR_UART3_MODULE, PHY_BASEADDR_UART4_MODULE, PHY_BASEADDR_UART5_MODULE
					};

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	return	(U32)UartPhysicalAddr[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_UART_GetPhysicalAddress,
 *				NX_UART_SetBaseAddress,		NX_UART_GetBaseAddress,
 *				NX_UART_OpenModule,			NX_UART_CloseModule,
 *				NX_UART_CheckBusy,			NX_UART_CanPowerDown
 */
U32		NX_UART_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_UART_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_UART_GetPhysicalAddress,	NX_UART_GetSizeOfRegisterSet,
 *				NX_UART_GetBaseAddress,
 *				NX_UART_OpenModule,			NX_UART_CloseModule,
 *				NX_UART_CheckBusy,			NX_UART_CanPowerDown
 */
void	NX_UART_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	__g_ModuleVariables[ModuleIndex].pRegister = (struct NX_UART_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Module's base address.
 *	@see		NX_UART_GetPhysicalAddress,	NX_UART_GetSizeOfRegisterSet,
 *				NX_UART_SetBaseAddress,
 *				NX_UART_OpenModule,			NX_UART_CloseModule,
 *				NX_UART_CheckBusy,			NX_UART_CanPowerDown
 */
U32		NX_UART_GetBaseAddress( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that Initialize is successed. \n
 *				\b CFALSE	indicates that Initialize is failed.
 *	@see		NX_UART_GetPhysicalAddress,	NX_UART_GetSizeOfRegisterSet,
 *				NX_UART_SetBaseAddress,		NX_UART_GetBaseAddress,
 *				NX_UART_CloseModule,
 *				NX_UART_CheckBusy,			NX_UART_CanPowerDown
 */
CBOOL	NX_UART_OpenModule( U32 ModuleIndex )
{
	register struct NX_UART_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->LCON		= 0;
//	pRegister->UCON		= 0;
//	pRegister->FCON		= 6;		// FIFO Reset
//	pRegister->MCON		= 0x0040;	// Full channel,UART TX/RX, No AFC
//	pRegister->TIMEOUT	= 0x001F;
//	pRegister->INTCON	= 0x000F;

	WriteIOW(&pRegister->LCON, 0);
	WriteIOW(&pRegister->UCON, 0);
	WriteIOW(&pRegister->FCON, 6);		// FIFO Reset
	WriteIOW(&pRegister->MCON, 0x0040);	// Full channel,UART TX/RX, No AFC
	WriteIOW(&pRegister->TIMEOUT, 0x001F);
	WriteIOW(&pRegister->INTCON, 0x000F);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that Deinitialize is successed. \n
 *				\b CFALSE	indicates that Deinitialize is failed.
 *	@see		NX_UART_GetPhysicalAddress,	NX_UART_GetSizeOfRegisterSet,
 *				NX_UART_SetBaseAddress,		NX_UART_GetBaseAddress,
 *				NX_UART_OpenModule,
 *				NX_UART_CheckBusy,			NX_UART_CanPowerDown
 */
CBOOL	NX_UART_CloseModule( U32 ModuleIndex )
{
	register struct NX_UART_RegisterSet	*pRegister;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->LCON		= 0;
//	pRegister->UCON		= 0;
//	pRegister->FCON		= 6;		// FIFO Reset
//	pRegister->MCON		= 0x0040;	// Full channel,UART TX/RX, No AFC
//	pRegister->TIMEOUT	= 0x001F;
//	pRegister->INTCON	= 0x000F;

	WriteIOW(&pRegister->LCON, 0);
	WriteIOW(&pRegister->UCON, 0);
	WriteIOW(&pRegister->FCON, 6);		// FIFO Reset
	WriteIOW(&pRegister->MCON, 0x0040);	// Full channel,UART TX/RX, No AFC
	WriteIOW(&pRegister->TIMEOUT, 0x001F);
	WriteIOW(&pRegister->INTCON, 0x000F);
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that Module is Busy. \n
 *				\b CFALSE	indicates that Module is NOT Busy.
 *	@see		NX_UART_GetPhysicalAddress,	NX_UART_GetSizeOfRegisterSet,
 *				NX_UART_SetBaseAddress,		NX_UART_GetBaseAddress,
 *				NX_UART_OpenModule,			NX_UART_CloseModule,
 *				NX_UART_CanPowerDown
 */
CBOOL	NX_UART_CheckBusy( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that Ready to enter power-down stage. \n
 *				\b CFALSE	indicates that This module can't enter to power-down stage.
 *	@see		NX_UART_GetPhysicalAddress,	NX_UART_GetSizeOfRegisterSet,
 *				NX_UART_SetBaseAddress,		NX_UART_GetBaseAddress,
 *				NX_UART_OpenModule,			NX_UART_CloseModule,
 *				NX_UART_CheckBusy
 */
CBOOL	NX_UART_CanPowerDown( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	return CTRUE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Interrupt number
 *	@see											NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
S32		NX_UART_GetInterruptNumber( U32 ModuleIndex )
{
	const U32	UartInterruptNumber[NUMBER_UART_MODULE] =
				{
					INTNUM_OF_UART0_MODULE, INTNUM_OF_UART1_MODULE
				};

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	return	UartInterruptNumber[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	IntNum	Interrupt Number ( 0:Tx, 1:Rx, 2:Error, 3:Modem ).
 *	@param[in]	Enable	\b CTRUE	indicates that Interrupt Enable. \n
 *						\b CFALSE	indicates that Interrupt Disable.
 *	@return		None.
 *	@see		NX_UART_GetInterruptNumber,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
void	NX_UART_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable )
{
	const U32	PEND_POS	=	0;
	const U32	PEND_MASK	=	0x0F << PEND_POS;

	register struct NX_UART_RegisterSet*	pRegister;
	register U32	ReadValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( 4 > IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue	=	pRegister->INTCON & ~PEND_MASK;

	ReadValue	&=	~(1UL << (IntNum+4));
	ReadValue	|=	(U32)Enable << (IntNum+4) ;

//	pRegister->INTCON	=	ReadValue;

	WriteIOW(&pRegister->INTCON, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	IntNum	Interrupt Number ( 0:Tx, 1:Rx, 2:Error, 3:Modem ).
 *	@return		\b CTRUE	indicates that Interrupt is enabled. \n
 *				\b CFALSE	indicates that Interrupt is disabled.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *													NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
CBOOL	NX_UART_GetInterruptEnable( U32 ModuleIndex, S32 IntNum )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( 4 > IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return	(CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->INTCON >> (IntNum+4)) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set TX interrupt enable or disable. \n
 *							- EnableFlag[1] : Set RX interrupt enable or disable. \n
 *							- EnableFlag[2] : Set Error interrupt enable or disable. \n
 *							- EnableFlag[3] : Set Modem interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
void	NX_UART_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag )
{
	const U32	ENB_POS = 4;

	NX_ASSERT( 0x10 > EnableFlag );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->INTCON = (U16)(EnableFlag << ENB_POS);
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->INTCON, (U16)(EnableFlag << ENB_POS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : TX interrupt's setting value. \n
 *				- Return Value[1] : RX interrupt's setting value. \n
 *				- Return Value[2] : Error interrupt's setting value. \n
 *				- Return Value[3] : Modem interrupt's setting value. \n
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *													NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
U32		NX_UART_GetInterruptEnable32( U32 ModuleIndex )
{
	const U32	ENB_POS		= 4;
	const U32	ENB_MASK	= 0xF << ENB_POS;

	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)((__g_ModuleVariables[ModuleIndex].pRegister->INTCON & ENB_MASK) >> ENB_POS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	IntNum	Interrupt Number( 0:Tx, 1:Rx, 2:Error, 3:Modem ).
 *	@return		\b CTRUE	indicates that Pending is seted. \n
 *				\b CFALSE	indicates that Pending is Not Seted.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
CBOOL	NX_UART_GetInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( 4 > IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return	(CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->INTCON >> IntNum) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bitis NOT occured. \n
 *				- Return Value[0] : TX pending state. \n
 *				- Return Value[1] : RX pending state. \n
 *				- Return Value[2] : Error pending state. \n
 *				- Return Value[3] : Modem pending state. \n
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *													NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
U32		NX_UART_GetInterruptPending32( U32 ModuleIndex )
{
	const U32 PEND_MASK = 0x0F;

	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)(__g_ModuleVariables[ModuleIndex].pRegister->INTCON & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	IntNum	Interrupt number( 0:Tx, 1:Rx, 2:Error, 3:Modem ).
 *	@return		None.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
void	NX_UART_ClearInterruptPending( U32 ModuleIndex, S32 IntNum )
{
	const U32	PEND_POS	=	0;
	const U32	PEND_MASK	=	0x0F << PEND_POS;

	register struct NX_UART_RegisterSet*	pRegister;
	register U32	ReadValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( 4 > IntNum );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue	=	pRegister->INTCON & ~PEND_MASK;

	ReadValue	|=	1UL << (IntNum) ;

//	pRegister->INTCON	=	ReadValue;
	WriteIOW(&pRegister->INTCON, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : TX pending bit. \n"
 *								- PendingFlag[1] : RX pending bit. \n"
 *								- PendingFlag[2] : Error pending bit. \n"
 *								- PendingFlag[3] : Modem pending bit. \n"
 *	@return		None.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *													NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
void	NX_UART_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag )
{
	const U32	PEND_POS	=	0;
	const U32	PEND_MASK	=	0x0F << PEND_POS;

	register struct NX_UART_RegisterSet*	pRegister;
	register U32	ReadValue;

	NX_ASSERT( 0x10 > PendingFlag );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue	=	pRegister->INTCON & ~PEND_MASK;

	ReadValue	|=	PendingFlag ;

//	pRegister->INTCON	=	ReadValue;
	WriteIOW(&pRegister->INTCON, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	Enable	\b CTRUE	indicates that Set to all interrupt enable. \n
 *						\b CFALSE	indicates that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
void	NX_UART_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable )
{
	const U32	INT_MASK	=	0x0F << 4;

	register U32	SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue	=	0;

	if( Enable )
	{
		SetValue	|=	INT_MASK;
	}

//	__g_ModuleVariables[ModuleIndex].pRegister->INTCON	=	SetValue;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->INTCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicates that All interrupt is disabled.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *													NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
CBOOL	NX_UART_GetInterruptEnableAll( U32 ModuleIndex )
{
	const U32	INT_MASK	=	0x0F << 4;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->INTCON & INT_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicates that All pending is NOT seted.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,
 *				NX_UART_ClearInterruptPendingAll,	NX_UART_GetInterruptPendingNumber
 */
CBOOL	NX_UART_GetInterruptPendingAll( U32 ModuleIndex )
{
	const U32	PEND_MASK	=	0x0F;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->INTCON & PEND_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		None.
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *													NX_UART_GetInterruptPendingNumber
 */
void	NX_UART_ClearInterruptPendingAll( U32 ModuleIndex )
{
	const U32	PEND_POS	=	0;
	const U32	PEND_MASK	=	0x0F << PEND_POS;

	register struct NX_UART_RegisterSet*	pRegister;
	register U32	SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister	=	__g_ModuleVariables[ModuleIndex].pRegister;

//	pRegister->INTCON |= PEND_MASK;
	SetValue = pRegister->INTCON | PEND_MASK;

	WriteIOW(&pRegister->INTCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_UART_GetInterruptNumber,			NX_UART_SetInterruptEnable,
 *				NX_UART_GetInterruptEnable,			NX_UART_SetInterruptEnable32,
 *				NX_UART_GetInterruptEnable32,		NX_UART_GetInterruptPending,
 *				NX_UART_GetInterruptPending32,		NX_UART_ClearInterruptPending,
 *				NX_UART_ClearInterruptPending32,	NX_UART_SetInterruptEnableAll,
 *				NX_UART_GetInterruptEnableAll,		NX_UART_GetInterruptPendingAll,
 *				NX_UART_ClearInterruptPendingAll
 */
S32		NX_UART_GetInterruptPendingNumber( U32 ModuleIndex )	// -1 if None
{
	const U32 TXPEND_MASK	= 1UL << 0;
	const U32 RXPEND_MASK	= 1UL << 1;
	const U32 ERRPEND_MASK	= 1UL << 2;
	const U32 MPEND_MASK	= 1UL << 3;

	register struct NX_UART_RegisterSet	*pRegister;
	register U32 Pend;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	Pend	=	(pRegister->INTCON >> 4) & (pRegister->INTCON);

	if( Pend & TXPEND_MASK )
	{
		return 0;
	}
	else if( Pend & RXPEND_MASK )
	{
		return 1;
	}
	else if( Pend & ERRPEND_MASK )
	{
		return 2;
	}
	else if( Pend & MPEND_MASK )
	{
		return 3;
	}

	return -1;
}

//------------------------------------------------------------------------------
// DMA Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA peripheral index of UART's Tx
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		DMA peripheral index of UART's Tx
 *	@see		NX_UART_GetDMAIndex_Rx, NX_UART_GetDMABusWidth
 */
U32		NX_UART_GetDMAIndex_Tx( U32 ModuleIndex )
{
	const U32 UartDmaIndexTx[NUMBER_UART_MODULE] =
				{
					DMAINDEX_OF_UART0_MODULE_TX, DMAINDEX_OF_UART1_MODULE_TX
				};

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	return UartDmaIndexTx[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA peripheral index of UART's Rx
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		DMA peripheral index of UART's Rx
 *	@see		NX_UART_GetDMAIndex_Tx, NX_UART_GetDMABusWidth
 */
U32		NX_UART_GetDMAIndex_Rx( U32 ModuleIndex )
{
	const U32 UartDmaIndexRx[NUMBER_UART_MODULE] =
				{
					DMAINDEX_OF_UART0_MODULE_RX, DMAINDEX_OF_UART1_MODULE_RX
				};

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	return UartDmaIndexRx[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA bus width of UART
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		DMA bus width of UART
 *	@see		NX_UART_GetDMAIndex_Tx, NX_UART_GetDMAIndex_Rx
 */
U32		NX_UART_GetDMABusWidth( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );

	return 8;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@remarks	UART controller only support NX_PCLKMODE_ALWAYS.\n
 *				If user set to NX_PCLKMODE_DYNAMIC, then uart controller \b NOT operate.
 *	@see		NX_UART_GetClockPClkMode,
 *				NX_UART_SetClockSource,			NX_UART_GetClockSource,
 *				NX_UART_SetClockDivisor,		NX_UART_GetClockDivisor,
 *				NX_UART_SetClockDivisorEnable,	NX_UART_GetClockDivisorEnable
 */
void			NX_UART_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_UART_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->CLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//	pRegister->CLKENB = regvalue;
	WriteIODW(&pRegister->CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Current PCLK mode
 *	@see		NX_UART_SetClockPClkMode,
 *				NX_UART_SetClockSource,			NX_UART_GetClockSource,
 *				NX_UART_SetClockDivisor,		NX_UART_GetClockDivisor,
 *				NX_UART_SetClockDivisorEnable,	NX_UART_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_UART_GetClockPClkMode( U32 ModuleIndex )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->CLKENB & ( 1UL << PCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock source of clock generator
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@param[in]	ClkSrc	Select clock source of clock generator ( 0: PLL0, 1:PLL1 ).
 *	@remarks	UART have one clock generator. so \e Index must set to 0.
 *	@return		None.
 *	@see		NX_UART_SetClockPClkMode,		NX_UART_GetClockPClkMode,
 *				NX_UART_GetClockSource,
 *				NX_UART_SetClockDivisor,		NX_UART_GetClockDivisor,
 *				NX_UART_SetClockDivisorEnable,	NX_UART_GetClockDivisorEnable
 */
void	NX_UART_SetClockSource( U32 ModuleIndex, U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register U32 ReadValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( 0 == Index );
	NX_ASSERT( 2 > ClkSrc );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	ReadValue = __g_ModuleVariables[ModuleIndex].pRegister->CLKGEN;

	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;

//	__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN = ReadValue;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock source of specified clock generator.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@return		Clock source of clock generator ( 0:PLL0, 1:PLL1 ).
 *	@remarks	UART have one clock generator. so \e Index must set to 0.
 *	@see		NX_UART_SetClockPClkMode,		NX_UART_GetClockPClkMode,
 *				NX_UART_SetClockSource,			NX_UART_GetClockSource,
 *				NX_UART_SetClockDivisor,		NX_UART_GetClockDivisor,
 *				NX_UART_SetClockDivisorEnable,	NX_UART_GetClockDivisorEnable
 */
U32				NX_UART_GetClockSource( U32 ModuleIndex, U32 Index )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( 0 == Index );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return ( __g_ModuleVariables[ModuleIndex].pRegister->CLKGEN & CLKSRCSEL_MASK ) >> CLKSRCSEL_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock divisor of specified clock generator.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 )
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 256 ).
 *	@return		None.
 *	@remarks	UART have one clock generator. so \e Index must set to 0.
 *	@see		NX_UART_SetClockPClkMode,		NX_UART_GetClockPClkMode,
 *				NX_UART_SetClockSource,			NX_UART_GetClockSource,
 *				NX_UART_GetClockDivisor,
 *				NX_UART_SetClockDivisorEnable,	NX_UART_GetClockDivisorEnable
 */
void			NX_UART_SetClockDivisor( U32 ModuleIndex, U32 Index, U32 Divisor )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	register U32 ReadValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( 0 == Index );
	NX_ASSERT( 1 <= Divisor && Divisor <= 256 );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	ReadValue	=	__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN;

	ReadValue	&= ~CLKDIV_MASK;
	ReadValue	|= (Divisor-1) << CLKDIV_POS;

//	__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN = ReadValue;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock divisor of specified clock generator.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0	);
 *	@return		Clock divisor ( 1 ~ 256 ).
 *	@remarks	UART have one clock generator. so \e Index must set to 0.
 *	@see		NX_UART_SetClockPClkMode,		NX_UART_GetClockPClkMode,
 *				NX_UART_SetClockSource,			NX_UART_GetClockSource,
 *				NX_UART_SetClockDivisor,
 *				NX_UART_SetClockDivisorEnable,	NX_UART_GetClockDivisorEnable
 */
U32				NX_UART_GetClockDivisor( U32 ModuleIndex, U32 Index )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( 0 == Index );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return ((__g_ModuleVariables[ModuleIndex].pRegister->CLKGEN & CLKDIV_MASK) >> CLKDIV_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock generator's operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	Enable	\b CTRUE	indicates that Enable of clock generator. \n
 *						\b CFALSE	indicates that Disable of clock generator.
 *	@return		None.
 *	@see		NX_UART_SetClockPClkMode,		NX_UART_GetClockPClkMode,
 *				NX_UART_SetClockSource,			NX_UART_GetClockSource,
 *				NX_UART_SetClockDivisor,		NX_UART_GetClockDivisor,
 *				NX_UART_GetClockDivisorEnable
 */
void			NX_UART_SetClockDivisorEnable( U32 ModuleIndex, CBOOL Enable )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register U32 ReadValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	ReadValue	=	__g_ModuleVariables[ModuleIndex].pRegister->CLKENB;

	ReadValue	&=	~CLKGENENB_MASK;
	ReadValue	|= (U32)Enable << CLKGENENB_POS;

//	__g_ModuleVariables[ModuleIndex].pRegister->CLKENB	=	ReadValue;
	WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->CLKENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of clock generator's operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that Clock generator is enabled. \n
 *				\b CFALSE	indicates that Clock generator is disabled.
 *	@see		NX_UART_SetClockPClkMode,		NX_UART_GetClockPClkMode,
 *				NX_UART_SetClockSource,			NX_UART_GetClockSource,
 *				NX_UART_SetClockDivisor,		NX_UART_GetClockDivisor,
 *				NX_UART_SetClockDivisorEnable
 */
CBOOL			NX_UART_GetClockDivisorEnable( U32 ModuleIndex )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return	(CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->CLKENB & CLKGENENB_MASK) >> CLKGENENB_POS );
}

//--------------------------------------------------------------------------
// Configuration operations
//--------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable SIR mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	bEnb	Set as CTRUE to use SIR Mode
 *	@return		None.
 *	@remarks	determine whether or not to use the infra-red Tx/Rx mode
 *	@see												NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SetSIRMode( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 SIR_MODE_MASK = (1<<6);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	if ( CTRUE == bEnb )__g_ModuleVariables[ModuleIndex].pRegister->LCON =	(U16)( __g_ModuleVariables[ModuleIndex].pRegister->LCON | SIR_MODE_MASK );
//	else				__g_ModuleVariables[ModuleIndex].pRegister->LCON &= ~SIR_MODE_MASK;

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->LCON;

	if ( CTRUE == bEnb )SetValue =	(U16)( SetValue | SIR_MODE_MASK );
	else				SetValue &= ~SIR_MODE_MASK;

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->LCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get SIR mode's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that SIR Mode is Enabled.\n
 *				\b CFALSE	indicates that SIR Mode is Disabled.
 *	@see		NX_UART_SetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
CBOOL	NX_UART_GetSIRMode( U32 ModuleIndex )
{
	const U32 SIR_MODE_MASK = (1<<6);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->LCON & SIR_MODE_MASK )	{ return CTRUE; }
	else																	{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable Loop-back mode.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	bEnb	Set as CTRUE to use Loop-back Mode
 *	@return		None.
 *	@remarks	determine whether or not to use the Loop-back mode.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *														NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SetLoopBackMode( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 LOOPBACK_MODE_MASK = (1<<5);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	if ( CTRUE == bEnb )__g_ModuleVariables[ModuleIndex].pRegister->UCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->UCON | LOOPBACK_MODE_MASK );
//	else				__g_ModuleVariables[ModuleIndex].pRegister->UCON &= ~LOOPBACK_MODE_MASK;

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->UCON;

	if ( CTRUE == bEnb )SetValue = (U16)( SetValue | LOOPBACK_MODE_MASK );
	else				SetValue &= ~LOOPBACK_MODE_MASK;

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, SetValue);
}

//-------------------------------------------------------------------------------
/**
 *	@brief		Get Loop-Back mode's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that Loop-back mode is Enabled.\n
 *				\b CFALSE	Indicate that Loop-back mode is Disabled.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
CBOOL	NX_UART_GetLoopBackMode( U32 ModuleIndex )
{
	const U32 LOOPBACK_MODE_MASK = (1<<5);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->UCON & LOOPBACK_MODE_MASK )	{ return CTRUE; }
	else																		{ return CFALSE; }
}

//-------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable of Error interrupt when exception is generated
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	bEnb	\b CTRUE	indicates that Error interrupt Enable.\n
 *						\b CFALSE	indicates that Error interrupt Disable.
 *	@return		None.
 *	@remarks	Enable the UART to generate an interrupt if an exception, such as a
 *				break, frame error, parity error, or everrun error occurs during a receive operation.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *														NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SetIntEnbWhenExceptionOccur( U32 ModuleIndex, CBOOL bEnb )
{
	const U32	ERRSTATE_MASK = ( 1 << 6 );
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	if( CTRUE == bEnb ) __g_ModuleVariables[ModuleIndex].pRegister->UCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->UCON | ERRSTATE_MASK);
//	else				__g_ModuleVariables[ModuleIndex].pRegister->UCON &= ~ERRSTATE_MASK;

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->UCON;
	if( CTRUE == bEnb ) SetValue = (U16)( SetValue | ERRSTATE_MASK);
	else				SetValue &= ~ERRSTATE_MASK;

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, SetValue);
}

//-------------------------------------------------------------------------------
/**
 *	@brief		Get exception generation's state
 *	@param[in]	ModuleIndex	An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that exception generation is enabled.\n
 *				\b CFALSE	indicates that exception generation is disabled.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
CBOOL	NX_UART_GetIntEnbWhenExceptionOccur( U32 ModuleIndex )
{
	const U32	ERRSTATE_MASK = ( 1 << 6 );

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->UCON & ERRSTATE_MASK )	{ return CTRUE; }
	else																	{ return CFALSE;}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set frame configuration during UART trasmit and receive operation.
 *	@param[in]	ModuleIndex	An index of module ( 0 ~ 5 ).
 *	@param[in]	Parity		Parity generation/check type
 *	@param[in]	DataWidth	The number of data bit( 5, 6, 7, 8 bit )
 *	@param[in]	StopBit		The number of stop bit( 1, 2 )
 *	@return		None.
 *	@remarks	determine the number of data/stop bit and parity mode.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *														NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SetFrameConfiguration( U32 ModuleIndex, NX_UART_PARITYMODE Parity,	U32 DataWidth,	U32 StopBit )
{
	register U16 temp;

	const U32 SIRMODE_MASK	= 0x40;
	const U32 PARITY_BITPOS	= 3;
	const U32 STOP_BITPOS	= 2;
	const U32 DATA_BITPOS	= 0;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 0==Parity ||	( 4 <= Parity && Parity <= 7 ) );
	NX_ASSERT( 5 <= DataWidth && DataWidth <=8 );
	NX_ASSERT( (1==StopBit) || (2==StopBit) );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->LCON;

	temp = (U16)( (temp & SIRMODE_MASK)
			| (U16)( Parity			<<PARITY_BITPOS)
			| (U16)( (StopBit-1)	<<STOP_BITPOS)
			| (U16)( (DataWidth-5)	<<DATA_BITPOS) );

//	__g_ModuleVariables[ModuleIndex].pRegister->LCON = temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->LCON, temp);
}

//-------------------------------------------------------------------------------
/**
 *	@brief		Get Frame Configuration
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[out]	pParity	Parity generation/check type
 *	@param[out]	pDataWidth	The number of data bit ( 5, 6, 7, 8 )
 *	@param[out]	pStopBit	The number of stop bit ( 1, 2 )
 *	@return		None.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_GetFrameConfiguration( U32 ModuleIndex, NX_UART_PARITYMODE* pParity, U32* pDataWidth, U32* pStopBit )
{
	const U32	PARI_MASK	= ( 0x07 << 3 );
	const U32	PARI_BITPOS = 3;
	const U32	DATA_MASK	= ( 0x03 << 0 );
	const U32	DATA_BITPOS = 0;
	const U32	STOP_MASK	= ( 0x01 << 2 );
	const U32	STOP_BITPOS = 2;

	register U32 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CNULL != pParity );
	NX_ASSERT( CNULL != pDataWidth );
	NX_ASSERT( CNULL != pStopBit );

	temp	=	__g_ModuleVariables[ModuleIndex].pRegister->LCON;

	*pParity	=	(NX_UART_PARITYMODE)(( temp & PARI_MASK ) >> PARI_BITPOS);
	*pDataWidth =	(( temp & DATA_MASK ) >> DATA_BITPOS)+5;
	*pStopBit	=	(( temp & STOP_MASK ) >> STOP_BITPOS)+1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a BRD(Baud Rate Division) value.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	BRD	Baud rate division value, It must be greater than 0.
 *	@return		None.
 *	@remarks	The following fomula can determine the BRD\n
 *				BRD = (UART_CLK / (bps x 16)) - 1
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *														NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SetBRD( U32 ModuleIndex, U16 BRD )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( CNULL != BRD );

//	__g_ModuleVariables[ModuleIndex].pRegister->BRD = BRD;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->BRD, BRD);
}

//-------------------------------------------------------------------------------
/**
 *	@brief		Get Baud Rate Divison Value
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Baud Rate Divison Value ( 0x0 ~ 0xFFFF )
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
U16		NX_UART_GetBRD( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->BRD);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Make a BRD with a baud rate and clock in Hz.
 *	@param[in]	BaudRate	a desired baud rate.
 *	@param[in]	CLKinHz		a uart clock in Hz.
 *	@return		a calculated BRD value.
 *	@remarks	The following fomula can determine the BRD\n
 *				BRD = (UART_CLK / (bps x 16)) - 1
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *														NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
U16		NX_UART_MakeBRD( U32 BaudRate, U32 CLKinHz )
{
//	register int temp;
	NX_ASSERT( 0 < BaudRate );
	NX_ASSERT( 0 < CLKinHz );

//	temp = (int)(CLKinHz / (16 * BaudRate)) - 1;
//	NX_ASSERT( (0x0000FFFF >= temp) && (0 < temp) );
//	return (U16)temp;

	CLKinHz /= BaudRate;
	CLKinHz += 16/2;		// + (0.5) * 16
	CLKinHz /= 16;
	CLKinHz -= 1;

	NX_ASSERT( (0x0000FFFF >= CLKinHz) && (0 < CLKinHz) );
	return (U16)CLKinHz;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/Disable RX time out interrupt when UART FIFO is enabled.\n
 *				The interrupt is a receive interrupt.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 1 ).
 *	@param[in]	bEnb	\b CTRUE	indicates that RX Time Out interrupt Enable. \n
 *						\b CFALSE	indicates that RX Time Out interrupt Disable.
 *	@return		None.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SetRxTimeOutEnb( U32 ModuleIndex, CBOOL bEnb )
{
	const U32	RXTIME_MASK	=	( 0x01 << 7 );
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	if( CTRUE == bEnb ){ __g_ModuleVariables[ModuleIndex].pRegister->UCON	= (U16)( __g_ModuleVariables[ModuleIndex].pRegister->UCON |	RXTIME_MASK ); }
//	else				{ __g_ModuleVariables[ModuleIndex].pRegister->UCON	= (U16)( __g_ModuleVariables[ModuleIndex].pRegister->UCON & ~RXTIME_MASK ); }

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->UCON;

	if( CTRUE == bEnb ){ SetValue	= (U16)( SetValue |	RXTIME_MASK ); }
	else				{ SetValue	= (U16)( SetValue & ~RXTIME_MASK ); }

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Status of RX Time Out interrupt is Enabled or Disabled.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that RX Time Out Interrupt is Enabled.\n
 *				\b CFALSE	Indicate that RX Time Out Interrupt is Disabled.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *														NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
CBOOL	NX_UART_GetRxTimeOutEnb( U32 ModuleIndex )
{
	const U32	RXTIME_MASK	=	( 0x01 << 7 );

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->UCON & RXTIME_MASK ){ return CTRUE;	}
	else																{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a Rx Time-out period.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	TimeOut	a Rx time-out period ( 0x0 ~ 0xFFFF )
 *	@return		None.
 *	@remarks	In case of FIFO mode, when the number of data in FIFO does not
 *				reaches RX FIFO trigger level and does not receive data during
 *				a Rx time-out period, the Rx interrupt will be generated.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SetRxTimeOut( U32 ModuleIndex, U16 TimeOut )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->TIMEOUT = TimeOut;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->TIMEOUT, TimeOut);
}

//-------------------------------------------------------------------------------
/**
 *	@brief		Get a Rx Time-out period.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Rx time-out period ( 0x0 ~ 0xFFFF )
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *														NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
U16		NX_UART_GetRxTimeOut( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->TIMEOUT);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set irq Pending Signal(Rx, Tx) Clear
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		None.
 *	@remarks	This bit is cleared automatically.\n
 *				This Function execute one time when initialize.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SetSYNCPendClear( U32 ModuleIndex )
{
	const U32	SYNCPENDCLEAR_MASK	=	( 0x01 << 7 );
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->LCON | SYNCPENDCLEAR_MASK;
//	__g_ModuleVariables[ModuleIndex].pRegister->LCON	= (U16)( __g_ModuleVariables[ModuleIndex].pRegister->LCON | SYNCPENDCLEAR_MASK );
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->LCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set interrupt request type of Tx operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	IrqType	interrupt request type ( pulse or level )
 *	@return		None.
 *	@remarks	Pulse : Interrupt is requested just ONE PLUSE as soon as Tx buffer becomes empty
 *						or Tx FIFO reaches Trigger level in FIFO mode.\n
 *				Level : Interrupt is requested just LEVEL while Tx buffer is empty
 *						or Tx FIFO reaches in trigger level in FIFO mode.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *														NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void				NX_UART_SetTxIRQType( U32 ModuleIndex, NX_UART_IRQTYPE IrqType )
{
	const U32 TX_IRQTYPE_BITPOS = 9;
	register U32 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 2 > IrqType );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->UCON;
	temp &= ~(1<<TX_IRQTYPE_BITPOS);
	temp = ( temp | ((U32)IrqType<<TX_IRQTYPE_BITPOS) );
//	__g_ModuleVariables[ModuleIndex].pRegister->UCON = (U16)temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get interrupt request type of Tx operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Value of interrupt request type( pulse or level )
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
NX_UART_IRQTYPE	NX_UART_GetTxIRQType( U32 ModuleIndex )
{
	const U32 TX_IRQTYPE_MASK	= ( 0x01 << 9 );
	const U32 TX_IRQTYPE_BITPOS = 9;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return(NX_UART_IRQTYPE)((__g_ModuleVariables[ModuleIndex].pRegister->UCON & TX_IRQTYPE_MASK) >> TX_IRQTYPE_BITPOS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set interrupt request type of Rx operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	IrqType	interrupt request type ( pulse of level )
 *	@return		None.
 *	@remarks	Pulse : Interrupt is requested just ONE PULSE when Rx buffer receives the
 *						data Non-FIFO mode or reaches Rx FIFO trigger level in
 *						FIFO mode.\n
 *				Level : Interrupt is requested just LEVEL while Rx buffer is receiving data in
 *						Non-FIFO or reaches Rx FIFO trigger level in FIFO mode.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *														NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void			NX_UART_SetRxIRQType( U32 ModuleIndex, NX_UART_IRQTYPE IrqType )
{
	const U32 RX_IRQTYPE_BITPOS = 8;

	register U32 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 2 > IrqType );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->UCON;
	temp &= ~(1<<RX_IRQTYPE_BITPOS);
	temp |= ((U32)IrqType<<RX_IRQTYPE_BITPOS);
//	__g_ModuleVariables[ModuleIndex].pRegister->UCON = (U16)temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get interrupt request type of Rx operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Value of interrupt request type( pulse or level )
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
NX_UART_IRQTYPE	NX_UART_GetRxIRQType( U32 ModuleIndex )
{
	const U32 RX_IRQTYPE_MASK	= ( 0x01 << 8 );
	const U32 RX_IRQTYPE_BITPOS = 8;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (NX_UART_IRQTYPE)((__g_ModuleVariables[ModuleIndex].pRegister->UCON & RX_IRQTYPE_MASK) >> RX_IRQTYPE_BITPOS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Send a break signal during 1 fream time.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		None.
 *	@remarks	Auto cleared after sending the break signal
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *														NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void	NX_UART_SendBreak( U32 ModuleIndex )
{
	const U32 SENDBREAK_MASK = (1<<4);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->UCON | SENDBREAK_MASK;
//	__g_ModuleVariables[ModuleIndex].pRegister->UCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->UCON | SENDBREAK_MASK );
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Tx operation mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	mode		Operation mode ( Disable or Normal or DMA )
 *	@return		None.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void			NX_UART_SetTxOperationMode( U32 ModuleIndex, NX_UART_OPERMODE mode )
{
	const U32 TXOPERMODE_MASK	= (3<<2);
	const U32 TXOPERMODE_BITPOS	= 2;

	register U32 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 3 > mode );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->UCON;
	temp &= ~TXOPERMODE_MASK;
	temp |= ((U32)mode<<TXOPERMODE_BITPOS);
//	__g_ModuleVariables[ModuleIndex].pRegister->UCON = (U16)temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Tx operation mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Value of operation mode ( Disable or Normal or DMA )
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *														NX_UART_SetRxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
NX_UART_OPERMODE	NX_UART_GetTxOperationMode( U32 ModuleIndex )
{
	const U32 TXOPERMODE_MASK	= (3<<2);
	const U32 TXOPERMODE_BITPOS	= 2;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (NX_UART_OPERMODE)(( __g_ModuleVariables[ModuleIndex].pRegister->UCON & TXOPERMODE_MASK ) >> TXOPERMODE_BITPOS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Rx operation mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	mode		Operation mode ( Disable or Normal or DMA )
 *	@return		None.
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,
 *				NX_UART_GetRxOperationMode
 */
void			NX_UART_SetRxOperationMode( U32 ModuleIndex, NX_UART_OPERMODE mode )
{
	const U32 RXOPERMODE_MASK	= (3<<0);
	const U32 RXOPERMODE_BITPOS	= 0;

	register U32 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( 3 > mode );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->UCON;
	temp &= ~RXOPERMODE_MASK;
	temp |= ((U32)mode<<RXOPERMODE_BITPOS);
//	__g_ModuleVariables[ModuleIndex].pRegister->UCON = (U16)temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Rx operation mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Value of operation mode ( Disable or Normal or DMA )
 *	@see		NX_UART_SetSIRMode,						NX_UART_GetSIRMode,
 *				NX_UART_SetLoopBackMode,				NX_UART_GetLoopBackMode,
 *				NX_UART_SetIntEnbWhenExceptionOccur,	NX_UART_GetIntEnbWhenExceptionOccur,
 *				NX_UART_SetFrameConfiguration,			NX_UART_GetFrameConfiguration,
 *				NX_UART_SetBRD,							NX_UART_GetBRD,
 *				NX_UART_MakeBRD,						NX_UART_SetRxTimeOutEnb,
 *				NX_UART_GetRxTimeOutEnb,				NX_UART_SetRxTimeOut,
 *				NX_UART_GetRxTimeOut,					NX_UART_SetSYNCPendClear,
 *				NX_UART_SetTxIRQType,					NX_UART_GetTxIRQType,
 *				NX_UART_SetRxIRQType,					NX_UART_GetRxIRQType,
 *				NX_UART_SendBreak,						NX_UART_SetTxOperationMode,
 *				NX_UART_GetTxOperationMode,				NX_UART_SetRxOperationMode
 */
NX_UART_OPERMODE	NX_UART_GetRxOperationMode( U32 ModuleIndex )
{
	const U32 RXOPERMODE_MASK	= (3<<0);
	const U32 RXOPERMODE_BITPOS	= 0;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (NX_UART_OPERMODE)(( __g_ModuleVariables[ModuleIndex].pRegister->UCON & RXOPERMODE_MASK ) >> RXOPERMODE_BITPOS );
}

//--------------------------------------------------------------------------
//	FIFO control operations
//--------------------------------------------------------------------------
/**
 *	@brief		Set the trigger level of Tx FIFO
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	level	FIFO trigger level ( 0 byte(None), 4byte, 8byte, 12byte )
 *	@return		None.
 *	@see										NX_UART_GetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,	NX_UART_GetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,			NX_UART_IsTxFIFOReset,
 *				NX_UART_ResetRxFIFO,			NX_UART_IsRxFIFOReset,
 *				NX_UART_SetFIFOEnb,				NX_UART_GetFIFOEnb
 */
void	NX_UART_SetTxFIFOTriggerLevel( U32 ModuleIndex, U32 level )
{
	const U32 TXFIFOTL_MASK		= (3<<6);
	const U32 TXFIFOTL_BITPOS	= 6;

	register U32 temp;
	register U32 SetValue = 0;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( (0==level) || (4==level) || (8==level) || (12==level) );

	switch( level )
	{
		case 0:		SetValue = 0;	break;
		case 4:		SetValue = 1;	break;
		case 8:		SetValue = 2;	break;
		case 12:	SetValue = 3;	break;
	}

	temp = __g_ModuleVariables[ModuleIndex].pRegister->FCON;
	temp &= ~TXFIFOTL_MASK;
	temp |= ((U32)SetValue<<TXFIFOTL_BITPOS);
//	__g_ModuleVariables[ModuleIndex].pRegister->FCON = (U16)temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->FCON, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the trigger level of Tx FIFO
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Value of FIFO trigger level ( 0 byte(None), 4byte, 8byte, 12byte )
 *	@see		NX_UART_SetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,	NX_UART_GetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,			NX_UART_IsTxFIFOReset,
 *				NX_UART_ResetRxFIFO,			NX_UART_IsRxFIFOReset,
 *				NX_UART_SetFIFOEnb,				NX_UART_GetFIFOEnb
 */
U32		NX_UART_GetTxFIFOTriggerLevel( U32 ModuleIndex )
{
	const U32 TXFIFOTL_MASK		= (3<<6);
	const U32 TXFIFOTL_BITPOS	= 6;

	register U32	RetValue=0;
	register U32	ReadValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	ReadValue = (__g_ModuleVariables[ModuleIndex].pRegister->FCON & TXFIFOTL_MASK ) >> TXFIFOTL_BITPOS;

	switch( ReadValue )
	{
		case 0:		RetValue = 0;	break;
		case 1:		RetValue = 4;	break;
		case 2:		RetValue = 8;	break;
		case 3:		RetValue = 12;	break;
	}

	return RetValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set the trigger level of Rx FIFO
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	level	FIFO trigger level ( 1byte, 4byte, 8byte, 12byte )
 *	@return		None.
 *	@see		NX_UART_SetTxFIFOTriggerLevel,	NX_UART_GetTxFIFOTriggerLevel,
 *												NX_UART_GetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,			NX_UART_IsTxFIFOReset,
 *				NX_UART_ResetRxFIFO,			NX_UART_IsRxFIFOReset,
 *				NX_UART_SetFIFOEnb,				NX_UART_GetFIFOEnb
 */
void	NX_UART_SetRxFIFOTriggerLevel( U32 ModuleIndex, U32 level )
{
	const U32 RXFIFOTL_MASK		= (3<<4);
	const U32 RXFIFOTL_BITPOS	= 4;

	register U32 temp;
	register U32 SetValue=0;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );
	NX_ASSERT( (1==level) || (4==level) || (8==level) || (12==level) );

	switch( level )
	{
		case 1:		SetValue = 0;	break;
		case 4:		SetValue = 1;	break;
		case 8:		SetValue = 2;	break;
		case 12:	SetValue = 3;	break;
	}

	temp = __g_ModuleVariables[ModuleIndex].pRegister->FCON;
	temp &= ~RXFIFOTL_MASK;
	temp |= ((U32)SetValue<<RXFIFOTL_BITPOS);
//	__g_ModuleVariables[ModuleIndex].pRegister->FCON = (U16)temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->FCON, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the trigger level of Rx FIFO
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Value of FIFO trigger level ( 1byte, 4byte, 8byte, 12byte )
 *	@see		NX_UART_SetTxFIFOTriggerLevel,	NX_UART_GetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,			NX_UART_IsTxFIFOReset,
 *				NX_UART_ResetRxFIFO,			NX_UART_IsRxFIFOReset,
 *				NX_UART_SetFIFOEnb,				NX_UART_GetFIFOEnb
 */
U32		NX_UART_GetRxFIFOTriggerLevel( U32 ModuleIndex )
{
	const U32 RXFIFOTL_MASK		= (3<<4);
	const U32 RXFIFOTL_BITPOS	= 4;

	register U32	RetValue=0;
	register U32	ReadValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	ReadValue = (( __g_ModuleVariables[ModuleIndex].pRegister->FCON & RXFIFOTL_MASK ) >> RXFIFOTL_BITPOS );

	switch( ReadValue )
	{
		case 0:		RetValue = 1;	break;
		case 1:		RetValue = 4;	break;
		case 2:		RetValue = 8;	break;
		case 3:		RetValue = 12;	break;
	}

	return RetValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Reset Tx FIFO
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		None.
 *	@remarks	This bit is auto cleared when FIFO is reseted.
 *	@see		NX_UART_SetTxFIFOTriggerLevel,	NX_UART_GetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,	NX_UART_GetRxFIFOTriggerLevel,
 *												NX_UART_IsTxFIFOReset,
 *				NX_UART_ResetRxFIFO,			NX_UART_IsRxFIFOReset,
 *				NX_UART_SetFIFOEnb,				NX_UART_GetFIFOEnb
 */
void	NX_UART_ResetTxFIFO( U32 ModuleIndex )
{
	const U32 TX_FIFO_RESET	= (1<<2);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->FCON | TX_FIFO_RESET;
//	__g_ModuleVariables[ModuleIndex].pRegister->FCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->FCON | TX_FIFO_RESET );
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->FCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Tx FIFO's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that Reset is processing.\n
 *				\b CFALSE	Indicate that FIFO is reseted.
 *	@see		NX_UART_SetTxFIFOTriggerLevel,	NX_UART_GetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,	NX_UART_GetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,
 *				NX_UART_ResetRxFIFO,			NX_UART_IsRxFIFOReset,
 *				NX_UART_SetFIFOEnb,				NX_UART_GetFIFOEnb
 */

CBOOL	NX_UART_IsTxFIFOReset( U32 ModuleIndex )
{
	const U32 TX_FIFO_RESET	= (1<<2);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->FCON & TX_FIFO_RESET )	{ return CTRUE; }
	else																	{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Reset Rx FIFO
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		None.
 *	@remarks	This bit is auto cleared when FIFO is reseted.
 *	@see		NX_UART_SetTxFIFOTriggerLevel,	NX_UART_GetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,	NX_UART_GetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,			NX_UART_IsTxFIFOReset,
 *												NX_UART_IsRxFIFOReset,
 *				NX_UART_SetFIFOEnb,				NX_UART_GetFIFOEnb
 */
void	NX_UART_ResetRxFIFO( U32 ModuleIndex )
{
	const U32 RX_FIFO_RESET	= (1<<1);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->FCON | RX_FIFO_RESET;
//	__g_ModuleVariables[ModuleIndex].pRegister->FCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->FCON | RX_FIFO_RESET );
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->FCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Rx FIFO's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that Reset is processing.\n
 *				\b CFALSE	Indicate that FIFO is reseted.
 *	@see		NX_UART_SetTxFIFOTriggerLevel,	NX_UART_GetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,	NX_UART_GetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,			NX_UART_IsTxFIFOReset,
 *				NX_UART_ResetRxFIFO,
 *				NX_UART_SetFIFOEnb,				NX_UART_GetFIFOEnb
 */
CBOOL	NX_UART_IsRxFIFOReset( U32 ModuleIndex )
{
	const U32 RX_FIFO_RESET	= (1<<1);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->FCON & RX_FIFO_RESET )	{ return CTRUE; }
	else																	{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/disable FIFO mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	bEnb	Set as CTRUE to use FIFO
 *	@return		None.
 *	@see		NX_UART_SetTxFIFOTriggerLevel,	NX_UART_GetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,	NX_UART_GetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,			NX_UART_IsTxFIFOReset,
 *				NX_UART_ResetRxFIFO,			NX_UART_IsRxFIFOReset,
 *												NX_UART_GetFIFOEnb
 */
void	NX_UART_SetFIFOEnb( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 FIFOEN_MASK	= (1<<0);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->FCON;
//	if ( CTRUE == bEnb )__g_ModuleVariables[ModuleIndex].pRegister->FCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->FCON | FIFOEN_MASK );
//	else				__g_ModuleVariables[ModuleIndex].pRegister->FCON &= ~FIFOEN_MASK;
	if ( CTRUE == bEnb )SetValue = (U16)( SetValue | FIFOEN_MASK );
	else				SetValue &= ~FIFOEN_MASK;

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->FCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get FIFO's mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that FIFO is Enabled.\n
 *				\b CFALSE	Indicate that FIFO is Disabled.
 *	@see		NX_UART_SetTxFIFOTriggerLevel,	NX_UART_GetTxFIFOTriggerLevel,
 *				NX_UART_SetRxFIFOTriggerLevel,	NX_UART_GetRxFIFOTriggerLevel,
 *				NX_UART_ResetTxFIFO,			NX_UART_IsTxFIFOReset,
 *				NX_UART_ResetRxFIFO,			NX_UART_IsRxFIFOReset,
 *				NX_UART_SetFIFOEnb
 */
CBOOL	NX_UART_GetFIFOEnb( U32 ModuleIndex )
{
	const U32 FIFOEN_MASK	= (1<<0);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->FCON & FIFOEN_MASK ){ return CTRUE; }
	else																{ return CFALSE; }
}


//--------------------------------------------------------------------------
//	Smartmedia card control operations
//--------------------------------------------------------------------------
/**
 *	@brief		Enable/disable to transmit data to smartmedia card.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	bEnb	Set as CTRUE to transmit data to smartmedia card.
 *	@return		None.
 *	@remarks	If you want to use normal UART, you have to set it as CFALSE.
 *	@see								NX_UART_GetSCTxEnb,
 *				NX_UART_SetSCRxEnb,		NX_UART_GetSCRxEnb,
 *				NX_UART_SetSCTxMode,	NX_UART_SetSCRxMode,
 *				NX_UART_GetSCMode
 */
void	NX_UART_SetSCTxEnb( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 SMCTXENB_MASK	= (1<<5);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->MCON;
	
//	if( CTRUE == bEnb )	__g_ModuleVariables[ModuleIndex].pRegister->MCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->MCON | SMCTXENB_MASK );
//	else				__g_ModuleVariables[ModuleIndex].pRegister->MCON &= ~SMCTXENB_MASK;
	if( CTRUE == bEnb )	SetValue = (U16)( SetValue | SMCTXENB_MASK );
	else				SetValue &= ~SMCTXENB_MASK;

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get TX operation of Smart Card
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that Smart card TX is Enabled.\n
 *				\b CFALSE	indicates that Smart card TX is Disabled.
 *	@see		NX_UART_SetSCTxEnb,
 *				NX_UART_SetSCRxEnb,		NX_UART_GetSCRxEnb,
 *				NX_UART_SetSCTxMode,	NX_UART_SetSCRxMode,
 *				NX_UART_GetSCMode
 */
CBOOL	NX_UART_GetSCTxEnb( U32 ModuleIndex )
{
	const U32 SMCTXENB_MASK	= (1<<5);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->MCON & SMCTXENB_MASK )	{ return CTRUE; }
	else																	{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Enable/disable to receive data from smartmedia card.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	bEnb	Set as CTURE to receive data from smartmedia card.
 *	@return		None.
 *	@remarks	If you want to use normal UART, you have to set it as CTRUE.
 *	@see		NX_UART_SetSCTxEnb,		NX_UART_GetSCTxEnb,
 *										NX_UART_GetSCRxEnb,
 *				NX_UART_SetSCTxMode,	NX_UART_SetSCRxMode,
 *				NX_UART_GetSCMode
 */
void	NX_UART_SetSCRxEnb( U32 ModuleIndex, CBOOL bEnb )
{
	const U32 SMCRXENB_MASK	= (1<<6);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->MCON;
//	if( CTRUE == bEnb )	__g_ModuleVariables[ModuleIndex].pRegister->MCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->MCON | SMCRXENB_MASK );
//	else				__g_ModuleVariables[ModuleIndex].pRegister->MCON &= ~SMCRXENB_MASK;
	if( CTRUE == bEnb )	SetValue = (U16)( SetValue | SMCRXENB_MASK );
	else				SetValue &= ~SMCRXENB_MASK;

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get RX operation of Smart Card
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that Smart card RX is Enabled.\n
 *				\b CFALSE	Indicate that Smart card RX is Disabled.
 *	@see		NX_UART_SetSCTxEnb,		NX_UART_GetSCTxEnb,
 *				NX_UART_SetSCRxEnb,
 *				NX_UART_SetSCTxMode,	NX_UART_SetSCRxMode,
 *				NX_UART_GetSCMode
 */
CBOOL	NX_UART_GetSCRxEnb( U32 ModuleIndex )
{
	const U32 SMCRXENB_MASK	= (1<<6);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->MCON & SMCRXENB_MASK )	{ return CTRUE; }
	else																	{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set UART in order to transmit data to smartmedia card.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		None.
 *	@remarks	Same as SetSCTxEnb(CTRUE) and SetSCRxEnb(CFALSE).\n
 *	@see		NX_UART_SetSCTxEnb,		NX_UART_GetSCTxEnb,
 *				NX_UART_SetSCRxEnb,		NX_UART_GetSCRxEnb,
 *										NX_UART_SetSCRxMode,
 *				NX_UART_GetSCMode
 */
void	NX_UART_SetSCTxMode( U32 ModuleIndex )
{
	const U32 SCTXENB_MASK	= (1<<5);
	const U32 SCRXENB_MASK	= (1<<6);

	register U16 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->MCON;
	temp &= ~SCRXENB_MASK;
	temp =	(U16)( temp | SCTXENB_MASK );
//	__g_ModuleVariables[ModuleIndex].pRegister->MCON = temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set UART in order to receive data from smartmedia card.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		None.
 *	@remarks	Same as SetSCTxEnb(CFALSE) and SetSCRxEnb(CTURE)
 *	@see		NX_UART_SetSCTxEnb,		NX_UART_GetSCTxEnb,
 *				NX_UART_SetSCRxEnb,		NX_UART_GetSCRxEnb,
 *				NX_UART_SetSCTxMode,
 *				NX_UART_GetSCMode
 */
void	NX_UART_SetSCRxMode( U32 ModuleIndex )
{
	const U32 SMCTXENB_MASK	= (1<<5);
	const U32 SMCRXENB_MASK	= (1<<6);

	register U16 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->MCON;
	temp &= ~SMCTXENB_MASK;
	temp =	(U16)( temp | SMCRXENB_MASK );
//	__g_ModuleVariables[ModuleIndex].pRegister->MCON = temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Smart Card's Setting Mode
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Smart Card's mode( SCMODE_TX(2) or SCMODE_RX(1) or SCMODE_DISABLE(0) )
 *	@see		NX_UART_SetSCTxEnb,		NX_UART_GetSCTxEnb,
 *				NX_UART_SetSCRxEnb,		NX_UART_GetSCRxEnb,
 *				NX_UART_SetSCTxMode,	NX_UART_SetSCRxMode,
 */
NX_UART_SCMODE	NX_UART_GetSCMode( U32 ModuleIndex )
{
	const U32	SC_MASK		= ( 0x03 << 5 );
	const U32	SC_BITPOS	= 5;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return(NX_UART_SCMODE)( (__g_ModuleVariables[ModuleIndex].pRegister->MCON & SC_MASK) >> SC_BITPOS );
}

//--------------------------------------------------------------------------
//	Status operations
//--------------------------------------------------------------------------
/**
 *	@brief		Get Tx/Rx status
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		return status of Transmitter, Tx Buffer and Rx Buffer
 *	@remarks	Check state Using TXRXSTATUS.
 *	@code
 *		// mi : module index
 *		U32 state;
 *		state = NX_UART_GetTxRxStatus( mi );
 *		if( NX_UART_RX_BUFFER_READY & state ){ ... }
 *		if( NX_UART_TX_BUFFER_EMPTY & state ){ ... }
 *		...
 *	@endcode
 *	@see									NX_UART_GetErrorStatus,
 *				NX_UART_GetFIFOStatus,		NX_UART_GetTxFIFOCount,
 *				NX_UART_GetRxFIFOCount
 */
U32		NX_UART_GetTxRxStatus( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)(__g_ModuleVariables[ModuleIndex].pRegister->TRSTATUS & 0x7);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Error status
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		return status of Error which occurs during UART operation.
 *	@remarks		Check state Using ERRSTAT.
 *	@code
 *		// mi = module index
 *		U32 state;
 *		state = NX_UART_GetErrorStatus( mi );
 *		if( NX_UART_ERRSTAT_OVERRUN & state ){ ... }
 *		if( NX_UART_ERRSTAT_PARITY & state ) { ... }
 *		...
 *	@endcode
 *	@see		NX_UART_GetTxRxStatus,
 *				NX_UART_GetFIFOStatus, NX_UART_GetTxFIFOCount,
 *				NX_UART_GetRxFIFOCount
 */
U32		NX_UART_GetErrorStatus( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U32)(__g_ModuleVariables[ModuleIndex].pRegister->ESTATUS & 0xF);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get FIFO's status
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		FIFO's status
 *	@code
 *		// mi = module index
 *		U32 state;
 *		state = NX_UART_GetFIFOStatus( mi );
 *		if( NX_UART_FIFOSTAT_RXFULL & state ){ ... }
 *		if( NX_UART_FIFOSTAT_TXFULL & state ){ ... }
 *		...
 *	@endcode
 *	@see		NX_UART_GetTxRxStatus,		NX_UART_GetErrorStatus,
 *											NX_UART_GetTxFIFOCount,
 *				NX_UART_GetRxFIFOCount
 */
U32		NX_UART_GetFIFOStatus( U32 ModuleIndex )
{
	const U32 FIFO_MASK		= ( 0x07 << 8 );
	const U32 FIFO_BITPOS	= 8;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return ( ( __g_ModuleVariables[ModuleIndex].pRegister->FSTATUS & FIFO_MASK ) >> FIFO_BITPOS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the number of data in Tx FIFO.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		return the number of data in Tx FIFO.
 *	@see		NX_UART_GetTxRxStatus,	NX_UART_GetErrorStatus,
 *				NX_UART_GetFIFOStatus,
 *				NX_UART_GetRxFIFOCount
 */
U32		NX_UART_GetTxFIFOCount( U32 ModuleIndex )
{
	const U32 TXFIFO_FULL_POS	= 9;
	const U32 TXFIFO_FULL_MASK	= (0x1<<TXFIFO_FULL_POS);
	const U32 TXFIFOCOUNT_POS	= 4;
	const U32 TXFIFOCOUNT_MASK	= (0xF<<TXFIFOCOUNT_POS);

	register U32 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->FSTATUS;
	temp = ((temp & TXFIFO_FULL_MASK)>>(TXFIFO_FULL_POS-4)) | ((temp & TXFIFOCOUNT_MASK) >> TXFIFOCOUNT_POS);

	return temp;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get the number of data in Rx FIFO.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		return the number of data in Rx FIFO.
 *	@see		NX_UART_GetTxRxStatus,	NX_UART_GetErrorStatus,
 *				NX_UART_GetFIFOStatus,	NX_UART_GetTxFIFOCount
 */
U32		NX_UART_GetRxFIFOCount( U32 ModuleIndex )
{
	const U32 RXFIFO_FULL_POS	= 8;
	const U32 RXFIFO_FULL_MASK	= (0x1<<RXFIFO_FULL_POS);
	const U32 RXFIFOCOUNT_POS	= 0;
	const U32 RXFIFOCOUNT_MASK	= (0xF<<RXFIFOCOUNT_POS);

	register U32 temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->FSTATUS;
	temp = ((temp & RXFIFO_FULL_MASK)>>(RXFIFO_FULL_POS-4)) | ((temp & RXFIFOCOUNT_MASK) >> RXFIFOCOUNT_POS);
	return temp;
}

//--------------------------------------------------------------------------
// Modem operations
//--------------------------------------------------------------------------
/**
 *	@brief		Activate/Deactivate nDTR signal.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	bActive	Set as CTURE to activate nDTR signal.
 *	@return		None.
 *	@remarks	Output low level in active state.
 *	@see										NX_UART_GetDTR,
 *				NX_UART_SetRTS,					NX_UART_GetRTS,
 *				NX_UART_SetAutoFlowControl,		NX_UART_GetAutoFlowControl,
 *				NX_UART_SetHalfChannelEnable,	NX_UART_GetHalfChannelEnable,
 *				NX_UART_GetModemStatus
 */
void	NX_UART_SetDTR( U32 ModuleIndex, CBOOL bActive )
{
	const U32 DTR_MASK	= (1<<1);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->MCON;
//	if( CTRUE == bActive )	__g_ModuleVariables[ModuleIndex].pRegister->MCON = (U16)( __g_ModuleVariables[ModuleIndex].pRegister->MCON | DTR_MASK );
//	else					__g_ModuleVariables[ModuleIndex].pRegister->MCON &= ~DTR_MASK;
	if( CTRUE == bActive )	SetValue = (U16)( SetValue | DTR_MASK );
	else					SetValue &= ~DTR_MASK;

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DTR signal's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicates that DTR is Actived.\n
 *				\b CFALSE	indicates that DTR is Inactived.
 *	@remarks	Output low level in active state.
 *	@see		NX_UART_SetDTR,
 *				NX_UART_SetRTS,					NX_UART_GetRTS,
 *				NX_UART_SetAutoFlowControl,		NX_UART_GetAutoFlowControl,
 *				NX_UART_SetHalfChannelEnable,	NX_UART_GetHalfChannelEnable,
 *				NX_UART_GetModemStatus
 */
CBOOL	NX_UART_GetDTR( U32 ModuleIndex )
{
	const U32 DTR_MASK	= (1<<1);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->MCON & DTR_MASK )	{ return CTRUE; }
	else																{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Activate/Deactivate nRTS signal.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	bActive	Set as CTURE to activate nRTS signal.
 *	@return		None.
 *	@remarks	output low level in active state.\n
 *				In AFC mode, nRTS signal is controled automatically by UART module .
 *	@see		NX_UART_SetDTR,					NX_UART_GetDTR,
 *												NX_UART_GetRTS,
 *				NX_UART_SetAutoFlowControl,		NX_UART_GetAutoFlowControl,
 *				NX_UART_SetHalfChannelEnable,	NX_UART_GetHalfChannelEnable,
 *				NX_UART_GetModemStatus
 */
void	NX_UART_SetRTS( U32 ModuleIndex, CBOOL bActive )
{
	const U32 RTS_MASK	= (1<<0);
	register U32 SetValue;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	SetValue = __g_ModuleVariables[ModuleIndex].pRegister->MCON;
//	if( CTRUE == bActive )	__g_ModuleVariables[ModuleIndex].pRegister->MCON =	(U16)( __g_ModuleVariables[ModuleIndex].pRegister->MCON | RTS_MASK );
//	else					__g_ModuleVariables[ModuleIndex].pRegister->MCON &= ~RTS_MASK;
	if( CTRUE == bActive )	SetValue =	(U16)( SetValue | RTS_MASK );
	else					SetValue &= ~RTS_MASK;

	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, SetValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get RTS signal's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that RTS is Actived.\n
 *				\b CFALSE	Indicate that RTS is Inactived.
 *	@remarks	Output low level in active state.\n
 *				In AFC mode, nRTS signal is controled automatically by UART module .
 *	@see		NX_UART_SetDTR,					NX_UART_GetDTR,
 *				NX_UART_SetRTS,
 *				NX_UART_SetAutoFlowControl,		NX_UART_GetAutoFlowControl,
 *				NX_UART_SetHalfChannelEnable,	NX_UART_GetHalfChannelEnable,
 *				NX_UART_GetModemStatus
 */
CBOOL	NX_UART_GetRTS( U32 ModuleIndex )
{
	const U32 RTS_MASK	= (1<<0);

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->MCON & RTS_MASK )	{ return CTRUE; }
	else																{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Auto Flow Control	Operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	enable	\b CTRUE	indicates that Auto Flow Control Enable. \n
 *						\b CFALSE	indicates that Auto Flow Control Disable.
 *	@return		None.
 *	@see		NX_UART_SetDTR,					NX_UART_GetDTR,
 *				NX_UART_SetRTS,					NX_UART_GetRTS,
 *												NX_UART_GetAutoFlowControl,
 *				NX_UART_SetHalfChannelEnable,	NX_UART_GetHalfChannelEnable,
 *				NX_UART_GetModemStatus
 */
void	NX_UART_SetAutoFlowControl( U32 ModuleIndex, CBOOL enable )
{
	const U32	AFC_MASK = ( 0x01 << 4 );
	register U32	temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->MCON;

	if( enable == CTRUE )	{ temp |=	AFC_MASK; }
	else					{ temp &=	~AFC_MASK; }

//	__g_ModuleVariables[ModuleIndex].pRegister->MCON = (U16)temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Auto Flow Control	Operation's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that Auto Flow Control is Enabled.\n
 *				\b CFALSE	Indicate that Auto Flow Control is Disabled.
 *	@see		NX_UART_SetDTR,					NX_UART_GetDTR,
 *				NX_UART_SetRTS,					NX_UART_GetRTS,
 *				NX_UART_SetAutoFlowControl,
 *				NX_UART_SetHalfChannelEnable,	NX_UART_GetHalfChannelEnable,
 *				NX_UART_GetModemStatus
 */
CBOOL	NX_UART_GetAutoFlowControl( U32 ModuleIndex )
{
	const U32	AFC_MASK = ( 0x01 << 4 );

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->MCON & AFC_MASK ){ return CTRUE; }
	else																{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Half Channel Operation
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	enable	\b CTRUE	indicates that Half Channel Enable. \n
 *						\b CFALSE	indicates that Half Channel Disable.
 *	@return		None.
 *	@see		NX_UART_SetDTR,					NX_UART_GetDTR,
 *				NX_UART_SetRTS,					NX_UART_GetRTS,
 *				NX_UART_SetAutoFlowControl,		NX_UART_GetAutoFlowControl,
 *												NX_UART_GetHalfChannelEnable,
 *				NX_UART_GetModemStatus
 */
void	NX_UART_SetHalfChannelEnable( U32 ModuleIndex, CBOOL enable )
{
	const U32	HC_MASK = ( 0x01 << 7 );
	register U32	temp;

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	temp = __g_ModuleVariables[ModuleIndex].pRegister->MCON;

	if( CTRUE == enable )	{ temp |=	HC_MASK; }
	else					{ temp &=	~HC_MASK; }

//	__g_ModuleVariables[ModuleIndex].pRegister->MCON = (U16)temp;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Half Channel Operation's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		\b CTRUE	Indicate that Half Channel is Enable.\n
 *				\b CFALSE	Indicate that Half Channel is Disable.
 *	@see		NX_UART_SetDTR,					NX_UART_GetDTR,
 *				NX_UART_SetRTS,					NX_UART_GetRTS,
 *				NX_UART_SetAutoFlowControl,		NX_UART_GetAutoFlowControl,
 *				NX_UART_SetHalfChannelEnable,
 *				NX_UART_GetModemStatus
 */
CBOOL	NX_UART_GetHalfChannelEnable( U32 ModuleIndex )
{
	const U32	HC_MASK = ( 0x01 << 7 );

	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	if( __g_ModuleVariables[ModuleIndex].pRegister->MCON & HC_MASK ){ return CTRUE; }
	else															{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Modem Status.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		return status of Modem Pin ( DCD, RI, DSR, CTS )
 *	@remarks	Modem's status is cleared after status read.
 *	@code
 *		// mi = module index
 *		U32 state;
 *		state = NX_UART_GetModemStatus( mi );
 *		if( NX_UART_MODEMSTAT_DCD_CHANGED & state ){ ... }
 *		if( NX_UART_MODEMSTAT_RI_CHANGED	& state ){ ... }
 *		...
 *	@endcode

 *	@see		NX_UART_SetDTR,					NX_UART_GetDTR,
 *				NX_UART_SetRTS,					NX_UART_GetRTS,
 *				NX_UART_SetAutoFlowControl,		NX_UART_GetAutoFlowControl,
 *				NX_UART_SetHalfChannelEnable,	NX_UART_GetHalfChannelEnable,
 */
U32	NX_UART_GetModemStatus( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (__g_ModuleVariables[ModuleIndex].pRegister->MSTATUS & 0xFF );
}

//--------------------------------------------------------------------------
//	Basic operations
//--------------------------------------------------------------------------
/**
 *	@brief		Send a data to be transmitted.
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	Data	a data to be transmitted.
 *	@return		If the result is successful, then return CTRUE.
 *	@see		NX_UART_GetByte
 */
void	NX_UART_SendByte( U32 ModuleIndex, U8 Data )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->THB = Data;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->THB, Data);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a received data
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Value of Received Data
 *	@see		NX_UART_SendByte
 */
U8		NX_UART_GetByte( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return (U8)__g_ModuleVariables[ModuleIndex].pRegister->RHB;
}

//--------------------------------------------------------------------------
// UART Configuration Function
//--------------------------------------------------------------------------
/**
 *	@brief		Set UART Line Configuration
 *	@param[in]	ModuleIndex	An index of module ( 0 ~ 5 ).
 *	@param[in]	sirMode		Set as CTRUE to use SIR Mode
 *	@param[in]	Parity		Parity generation/check type
 *	@param[in]	DataWidth	The number of data bit ( 5, 6, 7, 8 )
 *	@param[in]	StopBit		The number of stop bit ( 1 ,2 )
 *	@return		None.
 *	@see									NX_UART_SetControlConfig,
 *				NX_UART_SetFIFOConfig,		NX_UART_SetBaudRateConfig,
 *				NX_UART_SetModemConfig,		NX_UART_GetLineConfig,
 *				NX_UART_GetControlConfig,	NX_UART_GetFIFOConfig,
 *				NX_UART_GetBaudRateConfig,	NX_UART_GetModemConfig
 */
void	NX_UART_SetLineConfig( U32 ModuleIndex, CBOOL sirMode, NX_UART_PARITYMODE Parity, U32 DataWidth, U32 StopBit )
{
	NX_UART_SetSIRMode( ModuleIndex, sirMode );
	NX_UART_SetFrameConfiguration( ModuleIndex, Parity, DataWidth, StopBit );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set UART Control Configuration
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	syncPendClear	CTRUE(Set irq Pending Signal(Rx, Tx) Clear), CFALSE( Nothing )
 *	@param[in]	txType			interrupt request type ( pulse or level )
 *	@param[in]	rxType			interrupt request type ( pulse or level )
 *	@param[in]	txOper			Operation mode ( Disable or Normal or DMA )
 *	@param[in]	rxOper			Operation mode ( Disable or Normal or DMA )
 *	@return		None
 *	@see		NX_UART_SetLineConfig,
 *				NX_UART_SetFIFOConfig,		NX_UART_SetBaudRateConfig,
 *				NX_UART_SetModemConfig,		NX_UART_GetLineConfig,
 *				NX_UART_GetControlConfig,	NX_UART_GetFIFOConfig,
 *				NX_UART_GetBaudRateConfig,	NX_UART_GetModemConfig
 */
void	NX_UART_SetControlConfig( U32 ModuleIndex, CBOOL syncPendClear, NX_UART_IRQTYPE txType, NX_UART_IRQTYPE rxType, NX_UART_OPERMODE txOper, NX_UART_OPERMODE rxOper )
{
	if( CTRUE == syncPendClear ){ NX_UART_SetSYNCPendClear( ModuleIndex ); }
	NX_UART_SetTxIRQType( ModuleIndex, txType );
	NX_UART_SetRxIRQType( ModuleIndex, rxType );
	NX_UART_SetTxOperationMode( ModuleIndex, txOper );
	NX_UART_SetRxOperationMode( ModuleIndex, rxOper );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set FIFO Control Configuration
 *	@param[in]	ModuleIndex			An index of module ( 0 ~ 5 ).
 *	@param[in]	fifoEnb			Set as CTRUE to use FIFO
 *	@param[in]	txFIFOReset		CTRUE( Reset Tx FIFO ), CFALSE( Nothing )
 *	@param[in]	rxFIFOReset		CTRUE( Reset Rx FIFO ), CFALSE( Nothing )
 *	@param[in]	txLevel			FIFO trigger level ( 0, 4byte, 8byte, 12byte )
 *	@param[in]	rxLevel			FIFO trigger level ( 1byte, 4byte, 8byte, 12byte )
 *	@return		None
 *	@see		NX_UART_SetLineConfig,		NX_UART_SetControlConfig,
 *											NX_UART_SetBaudRateConfig,
 *				NX_UART_SetModemConfig,		NX_UART_GetLineConfig,
 *				NX_UART_GetControlConfig,	NX_UART_GetFIFOConfig,
 *				NX_UART_GetBaudRateConfig,	NX_UART_GetModemConfig
 */
void	NX_UART_SetFIFOConfig( U32 ModuleIndex, CBOOL fifoEnb, CBOOL txFIFOReset, CBOOL rxFIFOReset, U32 txLevel, U32 rxLevel )
{
	NX_UART_SetFIFOEnb( ModuleIndex, fifoEnb );
	if( CTRUE == txFIFOReset ){ NX_UART_ResetTxFIFO( ModuleIndex ); }
	if( CTRUE == rxFIFOReset ){ NX_UART_ResetRxFIFO( ModuleIndex ); }
	NX_UART_SetTxFIFOTriggerLevel( ModuleIndex, txLevel );
	NX_UART_SetRxFIFOTriggerLevel( ModuleIndex, rxLevel );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Baud Rate & Rx Time Out Configuration
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	baudRate		Baud rate division value, It must be greater than 0.
 *	@param[in]	timeOut			a Rx time-out period ( 0x0 ~ 0xFFFF )
 *	@return		None
 *	@see		NX_UART_SetLineConfig,		NX_UART_SetControlConfig,
 *				NX_UART_SetFIFOConfig,
 *				NX_UART_SetModemConfig,		NX_UART_GetLineConfig,
 *				NX_UART_GetControlConfig,	NX_UART_GetFIFOConfig,
 *				NX_UART_GetBaudRateConfig,	NX_UART_GetModemConfig
 */
void	NX_UART_SetBaudRateConfig( U32 ModuleIndex, U16 baudRate, U16 timeOut )
{
	NX_UART_SetBRD( ModuleIndex, baudRate);
	NX_UART_SetRxTimeOut( ModuleIndex, timeOut);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set UART Modem Configuration
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	autoflow		CTRUE( Auto Flow Control Enable ), CFALSE( Auto Flow Control Disable )
 *	@param[in]	halfChannel		CTRUE( Half Channel Enable ), CFALSE( Half Channel Disable )
 *	@param[in]	SCTxEnb			CTRUE( Smart Card Tx Enable ), CFALSE( Smart Card Tx Disable )
 *	@param[in]	SCRxEnb			CTRUE( Smart Card Rx Enable ), CFALSE( Smart Card Rx Disable )
 *	@param[in]	DTR				CTRUE( DTR Active), CFALSE( DTR InActive )
 *	@param[in]	RTS				CTRUE( RTS Active), CFALSE( RTS InActive )
 *	@return		None
 *	@see		NX_UART_SetLineConfig,		NX_UART_SetControlConfig,
 *				NX_UART_SetFIFOConfig,		NX_UART_SetBaudRateConfig,
 *											NX_UART_GetLineConfig,
 *				NX_UART_GetControlConfig,	NX_UART_GetFIFOConfig,
 *				NX_UART_GetBaudRateConfig,	NX_UART_GetModemConfig
 */
void	NX_UART_SetModemConfig( U32 ModuleIndex, CBOOL autoflow, CBOOL halfChannel, CBOOL SCTxEnb, CBOOL SCRxEnb, CBOOL DTR, CBOOL RTS )
{
	NX_UART_SetAutoFlowControl( ModuleIndex, autoflow );
	NX_UART_SetHalfChannelEnable( ModuleIndex, halfChannel );
	NX_UART_SetSCTxEnb( ModuleIndex, SCTxEnb );
	NX_UART_SetSCRxEnb( ModuleIndex, SCRxEnb );
	NX_UART_SetDTR( ModuleIndex, DTR );
	NX_UART_SetRTS( ModuleIndex, RTS );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get UART Line Configuration
 *	@param[in]	ModuleIndex	An index of module ( 0 ~ 5 ).
 *	@param[out]	pSirMode	1 : SIR Mode, 0 : UART Mode
 *	@param[out]	pParity		Parity generation/check type
 *	@param[out]	pDataWidth	The number of data bit ( 5, 6, 7, 8 )
 *	@param[out]	pStopBit	The number of stop bit ( 1, 2 )
 *	@return		None.
 *	@see		NX_UART_SetLineConfig,		NX_UART_SetControlConfig,
 *				NX_UART_SetFIFOConfig,		NX_UART_SetBaudRateConfig,
 *				NX_UART_SetModemConfig,
 *				NX_UART_GetControlConfig,	NX_UART_GetFIFOConfig,
 *				NX_UART_GetBaudRateConfig,	NX_UART_GetModemConfig
 */
void	NX_UART_GetLineConfig( U32 ModuleIndex, U32* pSirMode, NX_UART_PARITYMODE* pParity, U32* pDataWidth, U32* pStopBit )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != pSirMode );
	NX_ASSERT( CNULL != pParity );
	NX_ASSERT( CNULL != pDataWidth );
	NX_ASSERT( CNULL != pStopBit );

	if( CTRUE == NX_UART_GetSIRMode( ModuleIndex ) )	{ *pSirMode = 1;	}
	else												{ *pSirMode = 0;	}

	NX_UART_GetFrameConfiguration( ModuleIndex, pParity, pDataWidth, pStopBit );

	NX_ASSERT( 0==(*pParity) ||	( 4 <= (*pParity) && (*pParity) <= 7 ) );
	NX_ASSERT( 5 <= (*pDataWidth) && (*pDataWidth) <=8 );
	NX_ASSERT( (1==(*pStopBit)) || (2==(*pStopBit)) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get UART Control Configuration
 *	@param[in]	ModuleIndex	An index of module ( 0 ~ 5 ).
 *	@param[out]	pTxType		interrupt request type ( pulse or level )
 *	@param[out]	pRxType		interrupt request type ( pulse or level )
 *	@param[out]	pTxOper		Operation mode ( Disable or Normal or DMA )
 *	@param[out]	pRxOper		Operation mode ( Disable or Normal or DMA )
 *	@return		None
 *	@see		NX_UART_SetLineConfig,		NX_UART_SetControlConfig,
 *				NX_UART_SetFIFOConfig,		NX_UART_SetBaudRateConfig,
 *				NX_UART_SetModemConfig,		NX_UART_GetLineConfig,
 *											NX_UART_GetFIFOConfig,
 *				NX_UART_GetBaudRateConfig,	NX_UART_GetModemConfig
 */
void	NX_UART_GetControlConfig( U32 ModuleIndex, NX_UART_IRQTYPE* pTxType, NX_UART_IRQTYPE* pRxType, NX_UART_OPERMODE* pTxOper, NX_UART_OPERMODE* pRxOper )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != pTxType );
	NX_ASSERT( CNULL != pRxType );
	NX_ASSERT( CNULL != pTxOper );
	NX_ASSERT( CNULL != pRxOper );

	*pTxType = NX_UART_GetTxIRQType( ModuleIndex );
	*pRxType = NX_UART_GetRxIRQType( ModuleIndex );
	*pTxOper = NX_UART_GetTxOperationMode( ModuleIndex );
	*pRxOper = NX_UART_GetRxOperationMode( ModuleIndex );

	NX_ASSERT( 2 > *pTxType );
	NX_ASSERT( 2 > *pRxType );
	NX_ASSERT( 3 > *pTxOper );
	NX_ASSERT( 3 > *pRxOper );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get FIFO Control Configuration
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[out]	pFIFOEnb		1 : FIFO Enable			0 : FIFO Disable
 *	@param[out]	pTxFIFOReset	1 : Processing of Reset	0 : Nothing
 *	@param[out]	pRxFIFOReset	1 : Processing of Reset	0 : Nothing
 *	@param[out]	pTxLevel		FIFO trigger level ( Empty, 4byte, 8byte, 12byte )
 *	@param[out]	pRxLevel		FIFO trigger level ( Empty, 4byte, 8byte, 12byte )
 *	@return		None
 *	@see		NX_UART_SetLineConfig,		NX_UART_SetControlConfig,
 *				NX_UART_SetFIFOConfig,		NX_UART_SetBaudRateConfig,
 *				NX_UART_SetModemConfig,		NX_UART_GetLineConfig,
 *				NX_UART_GetControlConfig,
 *				NX_UART_GetBaudRateConfig,	NX_UART_GetModemConfig
 */
void	NX_UART_GetFIFOConfig( U32 ModuleIndex, U32* pFIFOEnb, U32* pTxFIFOReset, U32* pRxFIFOReset, U32* pTxLevel, U32* pRxLevel )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != pFIFOEnb );
	NX_ASSERT( CNULL != pTxFIFOReset );
	NX_ASSERT( CNULL != pRxFIFOReset );
	NX_ASSERT( CNULL != pTxLevel );
	NX_ASSERT( CNULL != pRxLevel );

	if( CTRUE == NX_UART_GetFIFOEnb( ModuleIndex ) ){ *pFIFOEnb = 1; }
	else											{ *pFIFOEnb = 0; }

	if( CTRUE == NX_UART_IsTxFIFOReset( ModuleIndex ) )	{ *pTxFIFOReset = 1; }
	else												{ *pTxFIFOReset = 0; }

	if( CTRUE == NX_UART_IsRxFIFOReset( ModuleIndex ) )	{ *pRxFIFOReset = 1; }
	else												{ *pRxFIFOReset = 0; }

	*pTxLevel = NX_UART_GetTxFIFOTriggerLevel( ModuleIndex );
	*pRxLevel = NX_UART_GetRxFIFOTriggerLevel( ModuleIndex );

	NX_ASSERT( (0==(*pTxLevel)) || (4==(*pTxLevel)) || (8==(*pTxLevel)) || (12==(*pTxLevel)) );
	NX_ASSERT( (1==(*pRxLevel)) || (4==(*pRxLevel)) || (8==(*pRxLevel)) || (12==(*pRxLevel)) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Baud Rate & Rx Time Out Configuration
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[out]	pBaudRate		Baud rate division value, It must be greater than 0.
 *	@param[out]	pTimeOut		a Rx time-out period ( 0x0 ~ 0xFFFF )
 *	@return		None
 *	@see		NX_UART_SetLineConfig,		NX_UART_SetControlConfig,
 *				NX_UART_SetFIFOConfig,		NX_UART_SetBaudRateConfig,
 *				NX_UART_SetModemConfig,		NX_UART_GetLineConfig,
 *				NX_UART_GetControlConfig,	NX_UART_GetFIFOConfig,
 *											NX_UART_GetModemConfig
 */
void	NX_UART_GetBaudRateConfig( U32 ModuleIndex, U16* pBaudRate, U16* pTimeOut )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != pBaudRate );
	NX_ASSERT( CNULL != pTimeOut );

	*pBaudRate	= NX_UART_GetBRD( ModuleIndex );
	*pTimeOut	= NX_UART_GetRxTimeOut( ModuleIndex );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get UART Modem Configuration
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[out]	pAutoflow		1:( Auto Flow Control Enable ),	0:( Auto Flow Control Disable )
 *	@param[out]	pHalfChannel	1:( Half Channel Enable ),		0:( Half Channel Disable )
 *	@param[out]	pSCTxEnb		1:( Smart Card Tx Enable ),		0:( Smart Card Tx Disable )
 *	@param[out]	pSCRxEnb		1:( Smart Card Rx Enable ),		0:( Smart Card Rx Disable )
 *	@param[out]	pDTR			1:( DTR Active),				0:( DTR InActive )
 *	@param[out]	pRTS			1:( RTS Active),				0:( RTS InActive )
 *	@return		None
 *	@see		NX_UART_SetLineConfig,		NX_UART_SetControlConfig,
 *				NX_UART_SetFIFOConfig,		NX_UART_SetBaudRateConfig,
 *				NX_UART_SetModemConfig,		NX_UART_GetLineConfig,
 *				NX_UART_GetControlConfig,	NX_UART_GetFIFOConfig,
 *				NX_UART_GetBaudRateConfig
 */
void	NX_UART_GetModemConfig( U32 ModuleIndex, U32* pAutoflow, U32* pHalfChannel, U32* pSCTxEnb, U32* pSCRxEnb, U32* pDTR, U32* pRTS )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != pAutoflow );
	NX_ASSERT( CNULL != pHalfChannel );
	NX_ASSERT( CNULL != pSCTxEnb );
	NX_ASSERT( CNULL != pSCRxEnb );
	NX_ASSERT( CNULL != pDTR );
	NX_ASSERT( CNULL != pRTS );

	if( CTRUE == NX_UART_GetAutoFlowControl( ModuleIndex ) ){ *pAutoflow = 1; }
	else													{ *pAutoflow = 0; }

	if( CTRUE == NX_UART_GetHalfChannelEnable( ModuleIndex ) )	{ *pHalfChannel = 1; }
	else														{ *pHalfChannel = 0; }

	if( CTRUE == NX_UART_GetSCTxEnb( ModuleIndex ) ){ *pSCTxEnb = 1; }
	else											{ *pSCTxEnb = 0; }

	if( CTRUE == NX_UART_GetSCRxEnb( ModuleIndex ) ){ *pSCRxEnb = 1; }
	else											{ *pSCRxEnb = 0; }

	if( CTRUE == NX_UART_GetDTR( ModuleIndex ) ){ *pDTR= 1; }
	else										{ *pDTR= 0; }

	if( CTRUE == NX_UART_GetRTS( ModuleIndex ) ){ *pRTS = 1; }
	else										{ *pRTS = 0; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Line Control Register
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	value			Line Control Register Value.
 *	@return		None.
 *	@see					SetUCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,	GetUCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
void	NX_UART_SetLCON( U32 ModuleIndex, U16 value )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->LCON = value;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->LCON, value);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Control Register
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	value			Control Register Value.
 *	@return		None.
 *	@see		SetLCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,	GetUCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
void	NX_UART_SetUCON( U32 ModuleIndex, U16 value )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->UCON = value;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->UCON, value);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set FIFO Control Register
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	value			FIFO Control Register Value.
 *	@return		None.
 *	@see		SetLCON,	SetUCON,
 *							SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,	GetUCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
void	NX_UART_SetFCON( U32 ModuleIndex, U16 value )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->FCON = value;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->FCON, value);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Modem Control Register
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	value			Modem Control Register Value.
 *	@return		None.
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,	GetUCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
void	NX_UART_SetMCON( U32 ModuleIndex, U16 value )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->MCON = value;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->MCON, value);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set TimeOut Register
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	value			TimeOut Register Value.
 *	@return		None.
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,	SetMCON,
 *							SetINTCON,
 *				GetLCON,	GetUCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
void	NX_UART_SetTIMEOUT( U32 ModuleIndex, U16 value )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->TIMEOUT = value;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->TIMEOUT, value);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Interrupt Status Register
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@param[in]	value			Interrupt Status	Value.
 *	@return		None.
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT,
 *				GetLCON,	GetUCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
void	NX_UART_SetINTCON( U32 ModuleIndex, U16 value )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//	__g_ModuleVariables[ModuleIndex].pRegister->INTCON = value;
	WriteIOW(&__g_ModuleVariables[ModuleIndex].pRegister->INTCON, value);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Line Control Register's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Line Control Register's value
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *							GetUCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
U16		NX_UART_GetLCON( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->LCON;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Control Register's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Control Register's value
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
U16		NX_UART_GetUCON( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->UCON;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get FIFO Control Register's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		FIFO Control Register's value
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,	GetUCON,
 *							GetMCON,
 *				GetTIMEOUT, GetINTCON
 */
U16		NX_UART_GetFCON( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->FCON;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Modem Control Register's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Modem Control Register's value
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,	GetUCON,
 *				GetFCON,
 *				GetTIMEOUT, GetINTCON
 */
U16		NX_UART_GetMCON( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->MCON;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Time Out Register's Value
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Time Out Register's Value
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,	GetUCON,
 *				GetFCON,	GetMCON,
 *							GetINTCON
 */
U16		NX_UART_GetTIMEOUT( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->TIMEOUT;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Interrupt Status Register's state
 *	@param[in]	ModuleIndex		An index of module ( 0 ~ 5 ).
 *	@return		Interrupt Status Register's value
 *	@see		SetLCON,	SetUCON,
 *				SetFCON,	SetMCON,
 *				SetTIMEOUT, SetINTCON,
 *				GetLCON,	GetUCON,
 *				GetFCON,	GetMCON,
 *				GetTIMEOUT
 */
U16		NX_UART_GetINTCON( U32 ModuleIndex )
{
	NX_ASSERT( NUMBER_UART_MODULE > ModuleIndex );
	NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	return __g_ModuleVariables[ModuleIndex].pRegister->INTCON;
}
