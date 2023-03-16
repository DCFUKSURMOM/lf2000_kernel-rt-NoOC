//------------------------------------------------------------------------------
//
//  Copyright (C) 2008 Nexell Co., All Rights Reserved
//  Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//  AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//  FOR A PARTICULAR PURPOSE.
//
//	Module     : I2C
//	File       : nx_i2c.c
//	Description:
//	Author     : Firmware Team
//	History    :
//------------------------------------------------------------------------------

#include "nx_i2c.h"

#define NUMBER_I2C_MODULE (2)

static  struct
{
    struct NX_I2C_RegisterSet *pRegister;

} __g_ModuleVariables[NUMBER_I2C_MODULE] = { {CNULL,}, };

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *  @brief  Initialize of prototype enviroment & local variables.
 *  @return \b CTRUE  indicates that Initialize is succeeded.\n
 *          \b CFALSE indicates that Initialize is failed.\n
 *  @see    NX_I2C_SelectModule,
 *          NX_I2C_GetCurrentModule,   NX_I2C_GetNumberOfModule
 */
CBOOL   NX_I2C_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		for( i=0; i < NUMBER_I2C_MODULE; i++ )
		{
			__g_ModuleVariables[i].pRegister = CNULL;
		}

		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get number of modules in the chip.
 *  @return     Module's number.
 *  @see        NX_I2C_Initialize,         NX_I2C_SelectModule,
 *              NX_I2C_GetCurrentModule
 */
U32     NX_I2C_GetNumberOfModule( void )
{
    return NUMBER_I2C_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *  @brief      Get module's physical address.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     Module's physical address
 *  @see        NX_I2C_GetSizeOfRegisterSet,
 *              NX_I2C_SetBaseAddress,       NX_I2C_GetBaseAddress,
 *              NX_I2C_OpenModule,           NX_I2C_CloseModule,
 *              NX_I2C_CheckBusy,            NX_I2C_CanPowerDown
 */
U32     NX_I2C_GetPhysicalAddress( U32 ModuleIndex )
{
    static const U32 I2CPhysicalAddr[NUMBER_I2C_MODULE] =
                    {
                        PHY_BASEADDR_I2C0_MODULE, PHY_BASEADDR_I2C1_MODULE
                    };

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );

    return  (U32)I2CPhysicalAddr[ModuleIndex];;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get a size, in byte, of register set.
 *  @return     Size of module's register set.
 *  @see        NX_I2C_GetPhysicalAddress,
 *              NX_I2C_SetBaseAddress,       NX_I2C_GetBaseAddress,
 *              NX_I2C_OpenModule,           NX_I2C_CloseModule,
 *              NX_I2C_CheckBusy,            NX_I2C_CanPowerDown
 */
U32     NX_I2C_GetSizeOfRegisterSet( void )
{
    return sizeof( struct NX_I2C_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set a base address of register set.
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[in]  BaseAddress Module's base address
 *  @return     None.
 *  @see        NX_I2C_GetPhysicalAddress,   NX_I2C_GetSizeOfRegisterSet,
 *              NX_I2C_GetBaseAddress,
 *              NX_I2C_OpenModule,           NX_I2C_CloseModule,
 *              NX_I2C_CheckBusy,            NX_I2C_CanPowerDown
 */
void    NX_I2C_SetBaseAddress( U32 ModuleIndex, U32 BaseAddress )
{
    NX_ASSERT( CNULL != BaseAddress );
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );

    __g_ModuleVariables[ModuleIndex].pRegister = (struct NX_I2C_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get a base address of register set
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     Module's base address.
 *  @see        NX_I2C_GetPhysicalAddress,   NX_I2C_GetSizeOfRegisterSet,
 *              NX_I2C_SetBaseAddress,
 *              NX_I2C_OpenModule,           NX_I2C_CloseModule,
 *              NX_I2C_CheckBusy,            NX_I2C_CanPowerDown
 */
U32     NX_I2C_GetBaseAddress( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );

    return (U32)__g_ModuleVariables[ModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Initialize selected modules with default value.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     \b CTRUE  indicates that Initialize is succeeded. \n
 *              \b CFALSE indicates that Initialize is failed.
 *  @see        NX_I2C_GetPhysicalAddress,   NX_I2C_GetSizeOfRegisterSet,
 *              NX_I2C_SetBaseAddress,       NX_I2C_GetBaseAddress,
 *              NX_I2C_CloseModule,
 *              NX_I2C_CheckBusy,            NX_I2C_CanPowerDown
 */
CBOOL   NX_I2C_OpenModule( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Deinitialize selected module to the proper stage.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     \b CTRUE  indicates that Deinitialize is succeeded. \n
 *              \b CFALSE indicates that Deinitialize is failed.
 *  @see        NX_I2C_GetPhysicalAddress,   NX_I2C_GetSizeOfRegisterSet,
 *              NX_I2C_SetBaseAddress,       NX_I2C_GetBaseAddress,
 *              NX_I2C_OpenModule,
 *              NX_I2C_CheckBusy,            NX_I2C_CanPowerDown
 */
CBOOL   NX_I2C_CloseModule( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether the selected modules is busy or not.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     \b CTRUE  indicates that Module is Busy. \n
 *              \b CFALSE indicates that Module is NOT Busy.
 *  @see        NX_I2C_GetPhysicalAddress,   NX_I2C_GetSizeOfRegisterSet,
 *              NX_I2C_SetBaseAddress,       NX_I2C_GetBaseAddress,
 *              NX_I2C_OpenModule,           NX_I2C_CloseModule,
 *              NX_I2C_CanPowerDown
 */
CBOOL   NX_I2C_CheckBusy( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );

    return NX_I2C_IsBusy( ModuleIndex );
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicaes whether the selected modules is ready to enter power-down stage
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     \b CTRUE  indicates that Ready to enter power-down stage. \n
 *              \b CFALSE indicates that This module can't enter to power-down stage.
 *  @see        NX_I2C_GetPhysicalAddress,   NX_I2C_GetSizeOfRegisterSet,
 *              NX_I2C_SetBaseAddress,       NX_I2C_GetBaseAddress,
 *              NX_I2C_OpenModule,           NX_I2C_CloseModule,
 *              NX_I2C_CheckBusy
 */
CBOOL   NX_I2C_CanPowerDown( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );

    return CTRUE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *  @brief      Get a interrupt number for interrupt controller.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     Interrupt number
 *  @see        									NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
S32     NX_I2C_GetInterruptNumber( U32 ModuleIndex )
{
    const U32   I2CInterruptNumber[NUMBER_I2C_MODULE] = { INTNUM_OF_I2C0_MODULE, INTNUM_OF_I2C1_MODULE };

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );

    return  I2CInterruptNumber[ModuleIndex];
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set a specified interrupt to be enable or disable.
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[in]  IntNum  Interrupt Number ( 0 ).
 *  @param[in]  Enable  \b CTRUE  indicates that Interrupt Enable. \n
 *                      \b CFALSE indicates that Interrupt Disable.
 *  @return     None.
 *  @remarks    I2C Module have one interrupt. So always \e IntNum set to 0.
 *  @see        NX_I2C_GetInterruptNumber,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
void    NX_I2C_SetInterruptEnable( U32 ModuleIndex, S32 IntNum, CBOOL Enable )
{
    const U32 IRQ_ENB_POS   = 5;
    const U32 IRQ_ENB_MASK  = 1UL << IRQ_ENB_POS;

    register struct NX_I2C_RegisterSet* pRegister;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( 0 == IntNum );
    NX_ASSERT( (0==Enable) ||(1==Enable) );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister   =   __g_ModuleVariables[ModuleIndex].pRegister;

    ReadValue   =   pRegister->ICCR;

    ReadValue   &=  ~IRQ_ENB_MASK;
    ReadValue   |=  (U32)Enable << IRQ_ENB_POS;

//  pRegister->ICCR  =   ReadValue;
    WriteIODW(&pRegister->ICCR, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether a specified interrupt is enabled or disabled.
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[in]  IntNum  Interrupt Number ( 0 ).
 *  @return     \b CTRUE  indicates that Interrupt is enabled. \n
 *              \b CFALSE indicates that Interrupt is disabled.
 *  @remarks    I2C Module have one interrupt. So always \e IntNum set to 0.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *													NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
CBOOL   NX_I2C_GetInterruptEnable( U32 ModuleIndex, S32 IntNum )
{
    const U32 IRQ_ENB_POS   = 5;
    const U32 IRQ_ENB_MASK  = 1UL << IRQ_ENB_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( 0 == IntNum );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->ICCR & IRQ_ENB_MASK) >> IRQ_ENB_POS );
}

//------------------------------------------------------------------------------
/**
 *  @brief		Set a specified interrupt to be enable or disable.
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below  \n
 *							- EnableFlag[0] : Set TX/RX interrupt enable or disable. \n
 *  @return		None.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
void    NX_I2C_SetInterruptEnable32( U32 ModuleIndex, U32 EnableFlag )
{
    const U32 IRQ_ENB_POS   = 5;
    const U32 IRQ_ENB_MASK  = 1UL << IRQ_ENB_POS;

    register struct NX_I2C_RegisterSet* pRegister;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister   =   __g_ModuleVariables[ModuleIndex].pRegister;

    ReadValue   =   pRegister->ICCR;

    ReadValue   &=  ~IRQ_ENB_MASK;
    ReadValue   |=  (U32)(EnableFlag & 0x01) << IRQ_ENB_POS;

//  pRegister->ICCR  =   ReadValue;
    WriteIODW(&pRegister->ICCR, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *  @brief		Indicates current setting value of interrupt enable bit.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : TX/RX interrupt's setting value. \n
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *													NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
U32		NX_I2C_GetInterruptEnable32( U32 ModuleIndex )
{
    const U32 IRQ_ENB_POS   = 5;
    const U32 IRQ_ENB_MASK  = 1UL << IRQ_ENB_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (U32)( (__g_ModuleVariables[ModuleIndex].pRegister->ICCR & IRQ_ENB_MASK) >> IRQ_ENB_POS );
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether a specified interrupt is pended or not
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[in]  IntNum  Interrupt Number ( 0 ).
 *  @return     \b CTRUE  indicates that Pending is seted. \n
 *              \b CFALSE indicates that Pending is Not Seted.
 *  @remarks    I2C Module have one interrupt. So always \e IntNum set to 0.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
CBOOL   NX_I2C_GetInterruptPending( U32 ModuleIndex, S32 IntNum )
{
    const U32 PEND_POS   = 0;
    const U32 PEND_MASK  = 1UL << PEND_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( 0 == IntNum );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)(__g_ModuleVariables[ModuleIndex].pRegister->PEND & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *  @brief		Indicates current setting value of interrupt pending bit.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : TX/RX pending state. \n
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *													NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
U32		NX_I2C_GetInterruptPending32( U32 ModuleIndex )
{
    const U32 PEND_POS   = 0;
    const U32 PEND_MASK  = 1UL << PEND_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (U32)(__g_ModuleVariables[ModuleIndex].pRegister->PEND & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Clear a pending state of specified interrupt.
 *  @param[in]  ModuleIndex	    A index of module.
 *  @param[in]  IntNum          Interrupt number ( 0 ).
 *  @return     None.
 *  @remarks    I2C Module have one interrupt. So always \e IntNum set to 0.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
void    NX_I2C_ClearInterruptPending( U32 ModuleIndex, S32 IntNum )
{
    const U32 PEND_POS   = 0;
    const U32 PEND_MASK  = 1UL << PEND_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( 0 == IntNum );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//  __g_ModuleVariables[ModuleIndex].pRegister->PEND =  PEND_MASK;
    WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->PEND, PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *  @brief		Clear a pending state of specified interrupt.
 *  @param[in]  ModuleIndex	    A index of module.
 *  @param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below  \n \n
 *								- PendingFlag[0] : TX/RX pending bit. \n
 *  @return		None.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *													NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
void    NX_I2C_ClearInterruptPending32( U32 ModuleIndex, U32 PendingFlag )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//  __g_ModuleVariables[ModuleIndex].pRegister->PEND = PendingFlag & 0x01;
    WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->PEND, PendingFlag & 0x01);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set all interrupts to be enables or disables.
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[in]  Enable  \b CTRUE  indicates that Set to all interrupt enable. \n
 *                      \b CFALSE indicates that Set to all interrupt disable.
 *  @return     None.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
void    NX_I2C_SetInterruptEnableAll( U32 ModuleIndex, CBOOL Enable )
{
    const U32 IRQ_ENB_POS   = 5;
    const U32 IRQ_ENB_MASK  = 1UL << IRQ_ENB_POS;

    register struct NX_I2C_RegisterSet* pRegister;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( (0==Enable) ||(1==Enable) );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister   =   __g_ModuleVariables[ModuleIndex].pRegister;

    ReadValue   =   pRegister->ICCR;

    ReadValue   &=  ~IRQ_ENB_MASK;
    ReadValue   |=  (U32)Enable << IRQ_ENB_POS;

//  pRegister->ICCR  =   ReadValue;
    WriteIODW(&pRegister->ICCR, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether some of interrupts are enable or not.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     \b CTRUE  indicates that At least one( or more ) interrupt is enabled. \n
 *              \b CFALSE indicates that All interrupt is disabled.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *													NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
CBOOL   NX_I2C_GetInterruptEnableAll( U32 ModuleIndex )
{
    const U32 IRQ_ENB_POS   = 5;
    const U32 IRQ_ENB_MASK  = 1UL << IRQ_ENB_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->ICCR & IRQ_ENB_MASK) >> IRQ_ENB_POS );
}

//------------------------------------------------------------------------------
/**
 *  @brief      Indicates whether some of interrupts are pended or not.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     \b CTRUE  indicates that At least one( or more ) pending is seted. \n
 *              \b CFALSE indicates that All pending is NOT seted.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,
 *				NX_I2C_ClearInterruptPendingAll,	NX_I2C_GetInterruptPendingNumber
 */
CBOOL   NX_I2C_GetInterruptPendingAll( U32 ModuleIndex )
{
    const U32 PEND_POS   = 0;
    const U32 PEND_MASK  = 1UL << PEND_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)(__g_ModuleVariables[ModuleIndex].pRegister->PEND & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Clear pending state of all interrupts.
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     None.
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *													NX_I2C_GetInterruptPendingNumber
 */
void    NX_I2C_ClearInterruptPendingAll( U32 ModuleIndex )
{
    const U32 PEND_POS   = 0;
    const U32 PEND_MASK  = 1UL << PEND_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//  __g_ModuleVariables[ModuleIndex].pRegister->PEND =  PEND_MASK;
    WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->PEND, PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get a interrupt number which has the most prority of pended interrupts
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     Pending Number( If all pending is not set then return -1 ).
 *  @see        NX_I2C_GetInterruptNumber,			NX_I2C_SetInterruptEnable,
 *				NX_I2C_GetInterruptEnable,			NX_I2C_SetInterruptEnable32,
 *				NX_I2C_GetInterruptEnable32,		NX_I2C_GetInterruptPending,
 *				NX_I2C_GetInterruptPending32,		NX_I2C_ClearInterruptPending,
 *				NX_I2C_ClearInterruptPending32,	NX_I2C_SetInterruptEnableAll,
 *				NX_I2C_GetInterruptEnableAll,		NX_I2C_GetInterruptPendingAll,
 *				NX_I2C_ClearInterruptPendingAll
 */
S32     NX_I2C_GetInterruptPendingNumber( U32 ModuleIndex )  // -1 if None
{
	const U32 IRQ_ENB_POS = 5;
    const U32 PEND_POS   = 0;
    const U32 PEND_MASK  = 1UL << PEND_POS;

	register struct NX_I2C_RegisterSet *pRegister;
	register U32 Pend;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

	pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	Pend = ((pRegister->ICCR >> IRQ_ENB_POS) & pRegister->PEND);

    if( Pend &  PEND_MASK )
    {
        return 0;
    }

    return -1;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *  @brief      Set a PCLK mode
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[in]  mode    PCLK mode
 *  @return     None.
 *  @remarks    I2C controller only support NX_PCLKMODE_ALWAYS.\n
 *              If user set to NX_PCLKMODE_DYNAMIC, then I2C controller \b NOT operate.
 *  @see        NX_I2C_GetClockPClkMode
 */
void            NX_I2C_SetClockPClkMode( U32 ModuleIndex, NX_PCLKMODE mode )
{
    const U32 PCLKMODE_POS  =   3;

    register U32 regvalue;
    register struct NX_I2C_RegisterSet* pRegister;

	U32 clkmode=0;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

	switch(mode)
	{
    	case NX_PCLKMODE_DYNAMIC:  clkmode = 0;		break;
    	case NX_PCLKMODE_ALWAYS:  	clkmode = 1;		break;
    	default: NX_ASSERT( CFALSE );
	}

    regvalue = pRegister->CLKENB;

    regvalue &= ~(1UL<<PCLKMODE_POS);
    regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//  pRegister->CLKENB = regvalue;
	WriteIODW(&pRegister->CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Get current PCLK mode
 *  @param[in]  ModuleIndex		A index of module.
 *  @return     Current PCLK mode
 *  @remarks    I2C controller only support NX_PCLKMODE_ALWAYS.\n
 *              If user set to NX_PCLKMODE_DYNAMIC, then I2C controller \b NOT operate.
 *  @see        NX_I2C_SetClockPClkMode,
 */
NX_PCLKMODE    NX_I2C_GetClockPClkMode( U32 ModuleIndex )
{
    const U32 PCLKMODE_POS  = 3;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    if( __g_ModuleVariables[ModuleIndex].pRegister->CLKENB & ( 1UL << PCLKMODE_POS ) )
    {
        return NX_PCLKMODE_ALWAYS;
    }

    return  NX_PCLKMODE_DYNAMIC;
}

//--------------------------------------------------------------------------
// Configuration Function
//--------------------------------------------------------------------------
/**
 *  @brief      Set PCLK divider and clock prescaler.
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[in]  PclkDivider     Set PCLK divider ( 16, 256 )
 *  @param[in]  Prescaler       Set prescaler. \n
 *                              2 ~ 16 ( when PCLK divider is seted 16 ). \n
 *                              1 ~ 16 ( when PCLK divider is seted 256 ).
 *  @return     None.
 *  @see        NX_I2C_GetClockPrescaler,
 *              NX_I2C_SetSlaveAddress,
 *              NX_I2C_SetDataDelay,           NX_I2C_SetDataDelay
 */
void    NX_I2C_SetClockPrescaler( U32 ModuleIndex, U32 PclkDivider, U32 Prescaler )
{
    const U32   CLKSRC_POS  = 6;
    const U32   CLKSRC_MASK = 1UL << CLKSRC_POS;
    const U32   CLK_SCALER_MASK = 0x0F;

    register U32 SetPclkDivider=0;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( (16==PclkDivider) || (256==PclkDivider) );
    NX_ASSERT( 1 <= Prescaler && Prescaler <= 16 );
    NX_ASSERT( (16!=PclkDivider) || ( 2 <= Prescaler && Prescaler <= 16) )
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    if( 16 == PclkDivider )
    {
        SetPclkDivider = 0;
    }
    else if( 256 == PclkDivider )
    {
        SetPclkDivider = 1;
    }

    ReadValue   =   __g_ModuleVariables[ModuleIndex].pRegister->ICCR;

    ReadValue   &= ~( CLKSRC_MASK | CLK_SCALER_MASK );
    ReadValue   |= ((SetPclkDivider << CLKSRC_POS) | (Prescaler-1));

//  __g_ModuleVariables[ModuleIndex].pRegister->ICCR = ReadValue;
    WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->ICCR, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *  @brief      Set PCLK divider and clock prescaler.
 *  @param[in]  ModuleIndex		A index of module.
 *  @param[out] pPclkDivider     Get PCLK divider ( 16, 256 )
 *  @param[out] pPrescaler       Get prescaler. \n
 *  @return     None.
 *  @see        NX_I2C_SetClockPrescaler,
 *              NX_I2C_SetSlaveAddress,
 *              NX_I2C_SetDataDelay,           NX_I2C_SetDataDelay
 */
void    NX_I2C_GetClockPrescaler( U32 ModuleIndex, U32* pPclkDivider, U32* pPrescaler )
{
    const U32   CLKSRC_POS  = 6;
    const U32   CLKSRC_MASK = 1UL << CLKSRC_POS;
    const U32   CLK_SCALER_MASK = 0x0F;

    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != pPclkDivider );
    NX_ASSERT( CNULL != pPrescaler );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    ReadValue   =   __g_ModuleVariables[ModuleIndex].pRegister->ICCR;

    if( ReadValue & CLKSRC_MASK )
    {
        if(CNULL != pPclkDivider) { *pPclkDivider = 256; }
    }
    else
    {
        if(CNULL != pPclkDivider) { *pPclkDivider = 16; }
    }

    if(CNULL != pPrescaler) { *pPrescaler = (ReadValue & CLK_SCALER_MASK)+1; }
}

//------------------------------------------------------------------------------
/**
 * @brief       Set Salve Address
 * @param[in]   ModuleIndex		A index of module.
 * @param[in]   Address    Value of Salve Address ( 0x02 ~ 0xFE )
 * @return      None.
 * @remarks     LSB[0] bit must set to 0. Slave Address can change ONLY ICSR[TXRX_ENB] bit is 0.
 * @see         NX_I2C_SetClockPrescaler,      NX_I2C_GetClockPrescaler
 *              NX_I2C_SetDataDelay,           NX_I2C_GetDataDelay
 */
void    NX_I2C_SetSlaveAddress( U32 ModuleIndex, U8 Address )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( 0 == (Address & 0x01) );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//  __g_ModuleVariables[ModuleIndex].pRegister->IAR  =   (U32)Address;
    WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->IAR, (U32)Address);
}

//------------------------------------------------------------------------------
/**
 * @brief       Set delay, in PCLKs, between SCL and SDA
 * @param[in]   ModuleIndex		A index of module.
 * @param[in]   delay    number of PCLK ( 1 ~ 32 ) for delay
 * @return      None.
 * @remarks     SDA must be changed at center of low state of SCL from I2C spec.
 *              For this, you have to set delay for SDA change position from falling edge of SCL when TX.
 *              This delay is also used for SDA fetch postiion from rising edge of SCL when RX.
 *              Usually this dealy is 1/4 of SCL period in PCLKs.
 * @see         NX_I2C_SetClockPrescaler,      NX_I2C_GetClockPrescaler
 *              NX_I2C_SetSlaveAddress,        NX_I2C_GetSlaveAddress,
 *                                              NX_I2C_GetDataDelay
 */
void    NX_I2C_SetDataDelay( U32 ModuleIndex, U32 delay )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( 1 <= delay && delay <= 32 );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//  __g_ModuleVariables[ModuleIndex].pRegister->QCNT_MAX = delay-1;
    WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->QCNT_MAX, delay-1);
}

//------------------------------------------------------------------------------
/**
 * @brief       Get PCLK number from rising or falling edge to data.
 * @param[in]   ModuleIndex		A index of module.
 * @return      Number of PCLK ( 1 ~ 32 )
 * @remarks     SDA must be changed at center of low state of SCL from I2C spec.
 *              For this, you have to set delay for SDA change position from falling edge of SCL when TX.
 *              This delay is also used for SDA fetch postiion from rising edge of SCL when RX.
 *              Usually this dealy is 1/4 of SCL period in PCLKs.
 * @see
 *              NX_I2C_SetSlaveAddress,        NX_I2C_GetSlaveAddress,
 *              NX_I2C_SetDataDelay
 */
U32     NX_I2C_GetDataDelay ( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return (__g_ModuleVariables[ModuleIndex].pRegister->QCNT_MAX + 1);
}

//------------------------------------------------------------------------------
// Operation Function
//------------------------------------------------------------------------------
/**
 * @brief      Set Ack Generation Enable or Diable
 * @param[in]  ModuleIndex		A index of module.
 * @param[in]  bAckGen   \b CTRUE indicates that Ack Generate. \n
 *                       \b CFALSE indicates that Ack Not Generation.
 * @return     None.
 * @remarks    Use only for receiver mode.
 * @see                                            NX_I2C_GetAckGenerationEnable,
 *             NX_I2C_ClearOperationHold,         NX_I2C_ControlMode,
 *             NX_I2C_WriteByte,                  NX_I2C_ReadByte,
 *             NX_I2C_IsBusy,
 *             NX_I2C_BusDisable
 */
void        NX_I2C_SetAckGenerationEnable( U32 ModuleIndex, CBOOL bAckGen )
{
    const U32 ACK_GEN_POS   = 7;
    const U32 ACK_GEN_MASK  = 1UL << ACK_GEN_POS;

    register struct NX_I2C_RegisterSet* pRegister;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( (0==bAckGen) ||(1==bAckGen) );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister   =   __g_ModuleVariables[ModuleIndex].pRegister;

    ReadValue   =   pRegister->ICCR;

    ReadValue   &=  ~ACK_GEN_MASK;
    ReadValue   |=  (U32)bAckGen << ACK_GEN_POS;

//  pRegister->ICCR  =   ReadValue;
    WriteIODW(&pRegister->ICCR, ReadValue);
}

//------------------------------------------------------------------------------
/**
 * @brief      Get Setting Value of Ack Generation
 * @param[in]  ModuleIndex		A index of module.
 * @return     \b CTRUE Indicate that Ack Generation Enabled. \n
 *             \b CFALSE Indicate that Ack Generation Disabled.
 * @see        NX_I2C_SetAckGenerationEnable,
 *             NX_I2C_ClearOperationHold,         NX_I2C_ControlMode,
 *             NX_I2C_WriteByte,                  NX_I2C_ReadByte,
 *             NX_I2C_IsBusy,
 *             NX_I2C_BusDisable
 */
CBOOL       NX_I2C_GetAckGenerationEnable( U32 ModuleIndex )
{
    const U32 ACK_GEN_POS   = 7;
    const U32 ACK_GEN_MASK  = 1UL << ACK_GEN_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->ICCR & ACK_GEN_MASK) >> ACK_GEN_POS );
}

//------------------------------------------------------------------------------
/**
 * @brief      Clear Operation Hold.
 * @return     None.
 * @remarks    I2C module's operation will be hold after transmiting or
 *             receiving a byte. Therefore you have to clear hold status to continue
 *             next progress.\n
 *             Also, user must clear hold status after module's start/stop setting.
 * @see        NX_I2C_SetAckGenerationEnable,     NX_I2C_GetAckGenerationEnable,
 *                                                 NX_I2C_ControlMode,
 *             NX_I2C_WriteByte,                  NX_I2C_ReadByte,
 *             NX_I2C_IsBusy,
 *             NX_I2C_BusDisable
 */
void        NX_I2C_ClearOperationHold  ( U32 ModuleIndex )
{
    const U32    OP_HOLD_MASK = ( 0x01 << 1 ) ;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//  __g_ModuleVariables[ModuleIndex].pRegister->PEND = (U16)OP_HOLD_MASK;
    WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->PEND, (U16)OP_HOLD_MASK);
}

//------------------------------------------------------------------------------
/**
 * @brief      Set I2C Control Mode
 * @param[in]  ModuleIndex	A index of module.
 * @param[in]  TxRxMode     Value of I2C's Mode ( Master tx/rx or Slave tx/rx )
 * @param[in]  Signal       Select Start/Stop signal ( NX_I2C_SIGNAL_START, NX_I2C_SIGNAL_STOP )
 * @return     None.
 * @remarks    This function make start/stop signal of I2C mode ( master tx/rx, slave tx/rx ).
 * @see        NX_I2C_SetAckGenerationEnable,     NX_I2C_GetAckGenerationEnable,
 *             NX_I2C_ClearOperationHold,
 *             NX_I2C_WriteByte,                  NX_I2C_ReadByte,
 *             NX_I2C_IsBusy,
 *             NX_I2C_BusDisable
 */
void        NX_I2C_ControlMode ( U32 ModuleIndex, NX_I2C_TXRXMODE TxRxMode, NX_I2C_SIGNAL Signal )
{
    const U32   TX_RX_POS       =   6;
    const U32   ST_BUSY_POS     =   5;
    const U32   TXRX_ENB_MASK   =   1UL << 4;
    const U32   ST_ENB_MASK     =   1UL << 12;

    register struct NX_I2C_RegisterSet*    pRegister;
    register U32    temp;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( 4 > TxRxMode );
    NX_ASSERT( 2 > Signal );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister = __g_ModuleVariables[ModuleIndex].pRegister;

    temp = pRegister->ICSR & 0x1F0F;

//  pRegister->ICSR =   ( temp | (TxRxMode<<TX_RX_POS) | (Signal<<ST_BUSY_POS) | TXRX_ENB_MASK | ST_ENB_MASK );
    WriteIODW(&pRegister->ICSR , ( temp | (TxRxMode<<TX_RX_POS) | (Signal<<ST_BUSY_POS) | TXRX_ENB_MASK | ST_ENB_MASK ));
}

//------------------------------------------------------------------------------
/**
 * @brief      Check I2C's status
 * @param[in]  ModuleIndex	A index of module.
 * @return     \b CTRUE Indicate that I2C is Busy.\n
 *             \b CFALSE Indicate that I2C is Not Busy.
 * @see        NX_I2C_SetAckGenerationEnable,     NX_I2C_GetAckGenerationEnable,
 *             NX_I2C_ClearOperationHold,         NX_I2C_ControlMode,
 *             NX_I2C_WriteByte,                  NX_I2C_ReadByte,
 *             NX_I2C_BusDisable
 */
CBOOL       NX_I2C_IsBusy( U32 ModuleIndex )
{
    const U32   ST_BUSY_POS     =   5;
    const U32   ST_BUSY_MASK    = 1UL << ST_BUSY_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->ICSR & ST_BUSY_MASK) >> ST_BUSY_POS );
}

//------------------------------------------------------------------------------
/**
 * @brief      Set Send Data
 * @param[in]  ModuleIndex	A index of module.
 * @param[in]  WriteData     Value of Write Data ( 0x0 ~ 0xFF )
 * @return     None.
 * @see        NX_I2C_SetAckGenerationEnable,     NX_I2C_GetAckGenerationEnable,
 *             NX_I2C_ClearOperationHold,         NX_I2C_ControlMode,
 *                                                 NX_I2C_ReadByte,
 *             NX_I2C_IsBusy,                     NX_I2C_BusDisable
 */
void        NX_I2C_WriteByte ( U32 ModuleIndex, U8 WriteData )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( 0x100 > WriteData );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

//  __g_ModuleVariables[ModuleIndex].pRegister->IDSR = (U32)( WriteData );
    WriteIODW(&__g_ModuleVariables[ModuleIndex].pRegister->IDSR, (U32)( WriteData ));
}

//------------------------------------------------------------------------------
/**
 * @brief      Get Received Data
 * @param[in]  ModuleIndex	A index of module.
 * @return     Received Data ( 0x0 ~ 0xFF )
 * @see        NX_I2C_SetAckGenerationEnable,     NX_I2C_GetAckGenerationEnable,
 *             NX_I2C_ClearOperationHold,         NX_I2C_ControlMode,
 *             NX_I2C_WriteByte,
 *             NX_I2C_IsBusy,                     NX_I2C_BusDisable
 */
U8          NX_I2C_ReadByte ( U32 ModuleIndex )
{
    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return (U8)__g_ModuleVariables[ModuleIndex].pRegister->IDSR;
}

//------------------------------------------------------------------------------
/**
 * @brief       I2C's Bus Disable
 * @param[in]   ModuleIndex	A index of module.
 * @return      None.
 * @remarks     Only use for Clear Arbitration fail status. \n
 *              Arbitration status means that conflicting two I2C master device when
 *              data send. \n
 *              This case, master device ( high prority ) send data, but master
 *              device(low prority) become arbitraion fail status.
 *
 * @see         NX_I2C_SetAckGenerationEnable,     NX_I2C_GetAckGenerationEnable,
 *              NX_I2C_ClearOperationHold,         NX_I2C_ControlMode,
 *              NX_I2C_WriteByte,                  NX_I2C_ReadByte,
 *              NX_I2C_IsBusy
 */
void        NX_I2C_BusDisable( U32 ModuleIndex )
{
    const U32 TXRX_ENB_MASK = ( 0x01 << 4 );

    register struct NX_I2C_RegisterSet* pRegister;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister  =   __g_ModuleVariables[ModuleIndex].pRegister;

	ReadValue  = pRegister->ICSR;
	ReadValue &= ~TXRX_ENB_MASK;

//  pRegister->ICSR = ReadValue;
    WriteIODW(&pRegister->ICSR, ReadValue);
}

//------------------------------------------------------------------------------
// Checking Function of external Interrupt's source when interrupt is occurred.
//------------------------------------------------------------------------------
/**
 * @brief       Check state of slave address is matched or NOT.
 * @param[in]   ModuleIndex	A index of module.
 * @return      \b CTRUE Indicate that Slave address is matched. \n
 *              \b CFALSE Indicate that Slave address is NOT matched.
 * @remarks     Interrupt is occurred when slave address is matched. \n
 * @see                                         NX_I2C_ClearSlaveAddressMatch,
 *              NX_I2C_IsGeneralCall,          NX_I2C_ClearGeneralCall,
 *              NX_I2C_IsSlaveRxStop,          NX_I2C_ClearSlaveRxStop,
 *              NX_I2C_IsBusArbitFail,         NX_I2C_IsACKReceived,
 *              NX_I2C_IsTxMode
 */
CBOOL       NX_I2C_IsSlaveAddressMatch( U32 ModuleIndex )
{
    const U32   SLAVE_MATCH_OCCUR_POS     =   10;
    const U32   SLAVE_MATCH_OCCUR_MASK    =   1UL << SLAVE_MATCH_OCCUR_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->ICSR & SLAVE_MATCH_OCCUR_MASK) >> SLAVE_MATCH_OCCUR_POS );
}

//------------------------------------------------------------------------------
/**
 * @brief       Clear state of slave address is matched.
 * @param[in]   ModuleIndex	A index of module.
 * @return      None.
 * @see         NX_I2C_IsSlaveAddressMatch,
 *              NX_I2C_IsGeneralCall,          NX_I2C_ClearGeneralCall,
 *              NX_I2C_IsSlaveRxStop,          NX_I2C_ClearSlaveRxStop,
 *              NX_I2C_IsBusArbitFail,         NX_I2C_IsACKReceived,
 *              NX_I2C_IsTxMode
 */
void        NX_I2C_ClearSlaveAddressMatch( U32 ModuleIndex )
{
    const U32   ST_ENB_MASK               = 1UL << 12;
    const U32   SLAVE_MATCH_OCCUR_POS     = 10;
    const U32   SLAVE_MATCH_OCCUR_MASK    = 1UL << SLAVE_MATCH_OCCUR_POS;

    register struct NX_I2C_RegisterSet* pRegister;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister   =   __g_ModuleVariables[ModuleIndex].pRegister;

    ReadValue   =    pRegister->ICSR;
    ReadValue   &= ~( ST_ENB_MASK | SLAVE_MATCH_OCCUR_MASK );

//  pRegister->ICSR = ReadValue;
    WriteIODW(&pRegister->ICSR, ReadValue);
}

//------------------------------------------------------------------------------
/**
 * @brief       Check state of General call is occurred or NOT.
 * @param[in]   ModuleIndex	A index of module.
 * @return      \b CTRUE Indicate that General call is occurred. \n
 *              \b CFALSE Indicate that General call is NOT occurred.
 * @remarks     Interrupt is occurred when general call is occurred.\n
 *              General call means that master device send a command to all slave device( broadcasting ).
 * @see         NX_I2C_IsSlaveAddressMatch,    NX_I2C_ClearSlaveAddressMatch,
 *                                              NX_I2C_ClearGeneralCall,
 *              NX_I2C_IsSlaveRxStop,          NX_I2C_ClearSlaveRxStop,
 *              NX_I2C_IsBusArbitFail,         NX_I2C_IsACKReceived,
 *              NX_I2C_IsTxMode
 */
CBOOL       NX_I2C_IsGeneralCall( U32 ModuleIndex )
{
    const U32   GENERAL_CALL_OCCUR_POS     =   9;
    const U32   GENERAL_CALL_OCCUR_MASK    =   1UL << GENERAL_CALL_OCCUR_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->ICSR & GENERAL_CALL_OCCUR_MASK) >> GENERAL_CALL_OCCUR_POS );
}

//------------------------------------------------------------------------------
/**
 * @brief       Clear state of general call is occurred.
 * @param[in]   ModuleIndex	A index of module.
 * @return      None.
 * @see         NX_I2C_IsSlaveAddressMatch,    NX_I2C_ClearSlaveAddressMatch,
 *              NX_I2C_IsGeneralCall,
 *              NX_I2C_IsSlaveRxStop,          NX_I2C_ClearSlaveRxStop,
 *              NX_I2C_IsBusArbitFail,         NX_I2C_IsACKReceived,
 *              NX_I2C_IsTxMode
 */
void        NX_I2C_ClearGeneralCall( U32 ModuleIndex )
{
    const U32   ST_ENB_MASK                = 1UL << 12;
    const U32   GENERAL_CALL_OCCUR_POS     = 9;
    const U32   GENERAL_CALL_OCCUR_MASK    = 1UL << GENERAL_CALL_OCCUR_POS;

    register struct NX_I2C_RegisterSet* pRegister;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister   =   __g_ModuleVariables[ModuleIndex].pRegister;

    ReadValue   =    pRegister->ICSR;
    ReadValue   &= ~( ST_ENB_MASK | GENERAL_CALL_OCCUR_MASK );

//  pRegister->ICSR = ReadValue;
    WriteIODW(&pRegister->ICSR, ReadValue);
}

//------------------------------------------------------------------------------
/**
 * @brief       Check state of slave RX is stopped or NOT.
 * @param[in]   ModuleIndex	A index of module.
 * @return      \b CTRUE Indicate that Slave RX is stopped. \n
 *              \b CFALSE Indicate that Slave RX is NOT stopped.
 * @remarks     Interrupt is occurred when slave RX is stopped.\n
 * @see         NX_I2C_IsSlaveAddressMatch,    NX_I2C_ClearSlaveAddressMatch,
 *              NX_I2C_IsGeneralCall,          NX_I2C_ClearGeneralCall,
 *                                              NX_I2C_ClearSlaveRxStop,
 *              NX_I2C_IsBusArbitFail,         NX_I2C_IsACKReceived,
 *              NX_I2C_IsTxMode
 */
CBOOL       NX_I2C_IsSlaveRxStop( U32 ModuleIndex )
{
    const U32   SLV_RX_STOP_POS     = 8;
    const U32   SLV_RX_STOP_MASK    = 1UL << SLV_RX_STOP_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->ICSR & SLV_RX_STOP_MASK) >> SLV_RX_STOP_POS );
}

//------------------------------------------------------------------------------
/**
 * @brief       Clear state of Slave RX is stopped.
 * @param[in]   ModuleIndex	A index of module.
 * @return      None.
 * @see         NX_I2C_IsSlaveAddressMatch,    NX_I2C_ClearSlaveAddressMatch,
 *              NX_I2C_IsGeneralCall,          NX_I2C_ClearGeneralCall,
 *              NX_I2C_IsSlaveRxStop,
 *              NX_I2C_IsBusArbitFail,         NX_I2C_IsACKReceived,
 *              NX_I2C_IsTxMode
 */
void        NX_I2C_ClearSlaveRxStop( U32 ModuleIndex )
{
    const U32   ST_ENB_MASK         = 1UL << 12;
    const U32   SLV_RX_STOP_POS     = 8;
    const U32   SLV_RX_STOP_MASK    = 1UL << SLV_RX_STOP_POS;

    register struct NX_I2C_RegisterSet* pRegister;
    register U32 ReadValue;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    pRegister   =   __g_ModuleVariables[ModuleIndex].pRegister;

    ReadValue   =    pRegister->ICSR;
    ReadValue   &= ~( ST_ENB_MASK | SLV_RX_STOP_MASK );

//  pRegister->ICSR = ReadValue;
    WriteIODW(&pRegister->ICSR, ReadValue);
}

//------------------------------------------------------------------------------
/**
 * @brief      Check Bus Arbitration status
 * @param[in]  ModuleIndex	A index of module.
 * @return     \b CTRUE Indicate that Bus Arbitration Failed. \n
 *             \b CFALSE Indicate that Bus Arbitration is Not Failed.
 * @remarks    Interrupt is Occured when Extend IRQ Enable and Bus arbitration is failed.
 * @see        NX_I2C_IsSlaveAddressMatch,    NX_I2C_ClearSlaveAddressMatch,
 *             NX_I2C_IsGeneralCall,          NX_I2C_ClearGeneralCall,
 *             NX_I2C_IsSlaveRxStop,          NX_I2C_ClearSlaveRxStop,
 *                                             NX_I2C_IsACKReceived,
 *             NX_I2C_IsTxMode
 */
CBOOL       NX_I2C_IsBusArbitFail( U32 ModuleIndex )
{
    const U32   ARBIT_FAIL_POS     = 3;
    const U32   ARBIT_FAIL_MASK   = 1UL << ARBIT_FAIL_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)( (__g_ModuleVariables[ModuleIndex].pRegister->ICSR & ARBIT_FAIL_MASK) >> ARBIT_FAIL_POS );
}

//------------------------------------------------------------------------------
/**
 * @brief      Check ACK Status
 * @param[in]  ModuleIndex	A index of module.
 * @return     \b CTRUE Indicate that ACK Received.\n
 *             \b CFALSE Indicate that ACK \b NOT received.
 * @remarks    Interrupt is Occured when Extend IRQ Enable and NAck Received.\n
 * @see        NX_I2C_IsSlaveAddressMatch,    NX_I2C_ClearSlaveAddressMatch,
 *             NX_I2C_IsGeneralCall,          NX_I2C_ClearGeneralCall,
 *             NX_I2C_IsSlaveRxStop,          NX_I2C_ClearSlaveRxStop,
 *             NX_I2C_IsBusArbitFail,
 *             NX_I2C_IsTxMode
 */
CBOOL       NX_I2C_IsACKReceived( U32 ModuleIndex )
{
    const U32   ACK_STATUS_POS    = 0;
    const U32   ACK_STATUS_MASK   = 1UL << ACK_STATUS_POS;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return  (CBOOL)(((__g_ModuleVariables[ModuleIndex].pRegister->ICSR & ACK_STATUS_MASK) >> ACK_STATUS_POS ) ^ 1);  // 0 : CTRUE, 1 : CFALSE
}


//------------------------------------------------------------------------------
/**
 * @brief      Check I2C's Mode ( Rx or Tx )
 * @return     \b CTRUE Indicate that I2C's mode is Tx \n
 *             \b CFALSE Indicate that I2C's mode is Rx.
 * @see        NX_I2C_IsSlaveAddressMatch,    NX_I2C_ClearSlaveAddressMatch,
 *             NX_I2C_IsGeneralCall,          NX_I2C_ClearGeneralCall,
 *             NX_I2C_IsSlaveRxStop,          NX_I2C_ClearSlaveRxStop,
 *             NX_I2C_IsBusArbitFail,         NX_I2C_IsACKReceived
 */
CBOOL       NX_I2C_IsTxMode( U32 ModuleIndex )
{
    const U32 TX_RX_MASK = 1UL << 6;

    NX_ASSERT( NUMBER_I2C_MODULE > ModuleIndex );
    NX_ASSERT( CNULL != __g_ModuleVariables[ModuleIndex].pRegister );

    return ( (__g_ModuleVariables[ModuleIndex].pRegister->ICSR & TX_RX_MASK) ? CTRUE : CFALSE );
}
