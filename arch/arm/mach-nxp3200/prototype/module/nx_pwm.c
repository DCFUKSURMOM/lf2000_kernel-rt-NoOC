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
//	Module		: PWM
//	File		: nx_pwm.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include "nx_pwm.h"

static	struct NX_PWM_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.\n
 *	@see										NX_PWM_GetNumberOfModule
 */
CBOOL	NX_PWM_Initialize( void )
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
 *	@return		Module's number.
 *	@see		NX_PWM_Initialize
 */
U32		NX_PWM_GetNumberOfModule( void )
{
	return NUMBER_OF_PWM_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see										NX_PWM_GetSizeOfRegisterSet,
 *				NX_PWM_SetBaseAddress,			NX_PWM_GetBaseAddress,
 *				NX_PWM_OpenModule,				NX_PWM_CloseModule,
 *				NX_PWM_CheckBusy,				NX_PWM_CanPowerDown
 */
U32		NX_PWM_GetPhysicalAddress( void )
{
	return	(U32)PHY_BASEADDR_PWM_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_PWM_GetPhysicalAddress,
 *				NX_PWM_SetBaseAddress,			NX_PWM_GetBaseAddress,
 *				NX_PWM_OpenModule,				NX_PWM_CloseModule,
 *				NX_PWM_CheckBusy,				NX_PWM_CanPowerDown
 */
U32		NX_PWM_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_PWM_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_PWM_GetPhysicalAddress,		NX_PWM_GetSizeOfRegisterSet,
 *												NX_PWM_GetBaseAddress,
 *				NX_PWM_OpenModule,				NX_PWM_CloseModule,
 *				NX_PWM_CheckBusy,				NX_PWM_CanPowerDown
 */
void	NX_PWM_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	__g_pRegister = (struct NX_PWM_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_PWM_GetPhysicalAddress,		NX_PWM_GetSizeOfRegisterSet,
 *				NX_PWM_SetBaseAddress,
 *				NX_PWM_OpenModule,				NX_PWM_CloseModule,
 *				NX_PWM_CheckBusy,				NX_PWM_CanPowerDown
 */
U32		NX_PWM_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_PWM_GetPhysicalAddress,		NX_PWM_GetSizeOfRegisterSet,
 *				NX_PWM_SetBaseAddress,			NX_PWM_GetBaseAddress,
 *												NX_PWM_CloseModule,
 *				NX_PWM_CheckBusy,				NX_PWM_CanPowerDown
 */
CBOOL	NX_PWM_OpenModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_PWM_GetPhysicalAddress,		NX_PWM_GetSizeOfRegisterSet,
 *				NX_PWM_SetBaseAddress,			NX_PWM_GetBaseAddress,
 *				NX_PWM_OpenModule,
 *				NX_PWM_CheckBusy,				NX_PWM_CanPowerDown
 */
CBOOL	NX_PWM_CloseModule( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_PWM_GetPhysicalAddress,		NX_PWM_GetSizeOfRegisterSet,
 *				NX_PWM_SetBaseAddress,			NX_PWM_GetBaseAddress,
 *				NX_PWM_OpenModule,				NX_PWM_CloseModule,
 *												NX_PWM_CanPowerDown
 */
CBOOL	NX_PWM_CheckBusy( void )
{
	const U32 CLKGENENB_MASK = 1 << 2;

	if( __g_pRegister->PWM_CLKENB & CLKGENENB_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_PWM_GetPhysicalAddress,		NX_PWM_GetSizeOfRegisterSet,
 *				NX_PWM_SetBaseAddress,			NX_PWM_GetBaseAddress,
 *				NX_PWM_OpenModule,				NX_PWM_CloseModule,
 *				NX_PWM_CheckBusy
 */
CBOOL	NX_PWM_CanPowerDown( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see										NX_PWM_GetClockPClkMode,
 *				NX_PWM_SetClockDivisorEnable,	NX_PWM_GetClockDivisorEnable,
 *				NX_PWM_SetClockSource,			NX_PWM_GetClockSource,
 *				NX_PWM_SetClockDivisor,			NX_PWM_GetClockDivisor
 */
void			NX_PWM_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register struct NX_PWM_RegisterSet* pRegister;
	register U32 regvalue;
	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->PWM_CLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//	pRegister->PWM_CLKENB = regvalue;
	WriteIODW(&pRegister->PWM_CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@return		Current PCLK mode
 *	@see		NX_PWM_SetClockPClkMode,
 *				NX_PWM_SetClockDivisorEnable,	NX_PWM_GetClockDivisorEnable,
 *				NX_PWM_SetClockSource,			NX_PWM_GetClockSource,
 *				NX_PWM_SetClockDivisor,			NX_PWM_GetClockDivisor
 */
NX_PCLKMODE	NX_PWM_GetClockPClkMode( void )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->PWM_CLKENB & ( 1UL << PCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock generator to be enabled or disabled.
 *	@param[in]	Enable	\b CTRUE	indicate that Clock generator enable. \n
 *						\b CFALSE	indicate that Clock generator disable.
 *	@return		None.
 *	@see		NX_PWM_SetClockPClkMode,		NX_PWM_GetClockPClkMode,
 *												NX_PWM_GetClockDivisorEnable,
 *				NX_PWM_SetClockSource,			NX_PWM_GetClockSource,
 *				NX_PWM_SetClockDivisor,			NX_PWM_GetClockDivisor
 */
void			NX_PWM_SetClockDivisorEnable( CBOOL Enable )
{
	const U32 CLKGENENB_POS	= 2;
	register struct NX_PWM_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = pRegister->PWM_CLKENB;

	regvalue &= ~(1UL		<< CLKGENENB_POS);
	regvalue |=	(U32)Enable << CLKGENENB_POS;

//	pRegister->PWM_CLKENB = regvalue;
	WriteIODW(&pRegister->PWM_CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a clock generator is enabled or not.
 *	@return		\b CTRUE	indicate that Clock generator is enabled. \n
 *				\b CFALSE	indicate that Clock generator is disabled.
 *	@see		NX_PWM_SetClockPClkMode,		NX_PWM_GetClockPClkMode,
 *				NX_PWM_SetClockDivisorEnable,
 *				NX_PWM_SetClockSource,			NX_PWM_GetClockSource,
 *				NX_PWM_SetClockDivisor,			NX_PWM_GetClockDivisor
 */
CBOOL			NX_PWM_GetClockDivisorEnable( void )
{
	const U32 CLKGENENB_POS	= 2;
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( ( __g_pRegister->PWM_CLKENB >> CLKGENENB_POS ) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock source for clock generator.
 *	@param[in]	Index		Select clock generator
 *	@param[in]	ClkSrc		Select clock source ( 0 (PLL0), 1 (PLL1) )
 *	@return		None.
 *	@remarks	PWM module have only ONE clock generator. so Index is always setting to 0.
 *	@see		NX_PWM_SetClockPClkMode,		NX_PWM_GetClockPClkMode,
 *				NX_PWM_SetClockDivisorEnable,	NX_PWM_GetClockDivisorEnable,
 *												NX_PWM_GetClockSource,
 *				NX_PWM_SetClockDivisor,			NX_PWM_GetClockDivisor
 */
void			NX_PWM_SetClockSource( U32 Index, U32 ClkSrc )
{
	const U32	CLKSRCSEL_POS	= 2;
	const U32	CLKSRCSEL_MASK	= 0x7;
	register struct NX_PWM_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( 1 >= ClkSrc );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = pRegister->PWM_CLKGEN;

	regvalue &= ~(CLKSRCSEL_MASK << CLKSRCSEL_POS);
	regvalue |= ( ClkSrc << CLKSRCSEL_POS );

//	pRegister->PWM_CLKGEN = regvalue;
	WriteIODW(&pRegister->PWM_CLKGEN, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current clock source for clock generator.
 *	@param[in]	Index		Select clock generator .
 *	@return		Clock source of clock generator( 0 (PLL0), 1 (PLL1) ).
 *	@remarks	PWM module have only ONE clock generator. so Index is always setting to 0.
 *	@see		NX_PWM_SetClockPClkMode,		NX_PWM_GetClockPClkMode,
 *				NX_PWM_SetClockDivisorEnable,	NX_PWM_GetClockDivisorEnable,
 *				NX_PWM_SetClockSource,
 *				NX_PWM_SetClockDivisor,			NX_PWM_GetClockDivisor
 */
U32				NX_PWM_GetClockSource( U32 Index )
{
	const U32	CLKSRCSEL_POS	= 2;
	const U32	CLKSRCSEL_MASK	= 0x7;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(( __g_pRegister->PWM_CLKGEN >> CLKSRCSEL_POS ) & CLKSRCSEL_MASK );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a clock divider for clock generator.
 *	@param[in]	Index		Select clock generator( 0 ).
 *	@param[in]	Divisor		Divide value of clock generator( 1 ~ 256 ).
 *	@return		None.
 *	@remarks	PWM module have only ONE clock generator. so Index is always setting to 0.
 *	@see		NX_PWM_SetClockPClkMode,		NX_PWM_GetClockPClkMode,
 *				NX_PWM_SetClockDivisorEnable,	NX_PWM_GetClockDivisorEnable,
 *				NX_PWM_SetClockSource,			NX_PWM_GetClockSource,
 *												NX_PWM_GetClockDivisor
 */
void			NX_PWM_SetClockDivisor( U32 Index, U32 Divisor )
{
	const U32 CLKDIV_POS	= 5;
	const U32 CLKDIV_MASK	= 0xFF;

	register struct NX_PWM_RegisterSet* pRegister;
	register U32 regvalue;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( (1 <= Divisor) && ( 256 >= Divisor ) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;
	regvalue = pRegister->PWM_CLKGEN;

	regvalue &= ~(CLKDIV_MASK << CLKDIV_POS);
	regvalue |= (Divisor-1) << CLKDIV_POS;

//	pRegister->PWM_CLKGEN = regvalue;
	WriteIODW(&pRegister->PWM_CLKGEN, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current clock divider of clock generator
 *	@param[in]	Index		Select clock generator( 0 ).
 *	@return		Clock divider value of current clock generator( 1 ~ 256 ).
 *	@remarks	PWM module have only ONE clock generator. so Index is always setting to 0.
 *	@see		NX_PWM_SetClockPClkMode,		NX_PWM_GetClockPClkMode,
 *				NX_PWM_SetClockDivisorEnable,	NX_PWM_GetClockDivisorEnable,
 *				NX_PWM_SetClockSource,			NX_PWM_GetClockSource,
 *				NX_PWM_SetClockDivisor
 */
U32				NX_PWM_GetClockDivisor( U32 Index )
{
	const U32 CLKDIV_POS	= 5;
	const U32 CLKDIV_MASK	= 0xFF;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(((__g_pRegister->PWM_CLKGEN >> CLKDIV_POS ) & CLKDIV_MASK ) +1 ) ;
}

//------------------------------------------------------------------------------
// PWM Operation.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 *	@brief		Set PreScaler Value
 *	@param[in]	channel		Select PWM Channel ( 0 ~ 1 )
 *	@param[in]	prescale	PreSacler Value ( 1 ~ 128 )
 *	@return		None.
 *	@see								NX_PWM_GetPreScale,
 *				NX_PWM_SetPolarity,		NX_PWM_GetPolarity,
 *				NX_PWM_SetPeriod,		NX_PWM_GetPeriod,
 *				NX_PWM_SetDutyCycle,	NX_PWM_GetDutyCycle
 */
void	NX_PWM_SetPreScale( U32 channel, U32 prescale )
{
	volatile U16 *pRegister;
	register U32 shift, temp;
	const	U32 mask = 0x7F;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > channel );
	NX_ASSERT( 0 < prescale && 128 >= prescale );

	pRegister = &__g_pRegister->PWM2[channel/2].PWM_PREPOL;

	shift = ( channel & 1 ) * 8;
	temp	= *pRegister;
	temp &= ~(mask << shift );
	temp |= ( (prescale-1) << shift );

//	*pRegister = (U16)temp;
	WriteIOW(pRegister, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get PreScaler Value
 *	@param[in]	channel		Select PWM Channel ( 0 ~ 1 )
 *	@return		PreScaler	Value ( 1 ~ 128 )
 *	@see		NX_PWM_SetPreScale,
 *				NX_PWM_SetPolarity,		NX_PWM_GetPolarity,
 *				NX_PWM_SetPeriod,		NX_PWM_GetPeriod,
 *				NX_PWM_SetDutyCycle,	NX_PWM_GetDutyCycle
 */
U32		NX_PWM_GetPreScale( U32 channel )
{
	volatile U16 *pRegister;
	register U32 shift, temp;
	const	U32 mask = 0x7F;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > channel );

	pRegister = &__g_pRegister->PWM2[channel/2].PWM_PREPOL;

	shift = ( channel & 1 ) * 8;
	temp	= *pRegister;

	return ((temp >> shift) & mask )+1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Polarity of PWM signal.
 *	@param[in]	channel		Select PWM Channel ( 0 ~ 1 )
 *	@param[in]	bypass		\b CTRUE	indicate that Polarity set to Bypass. \n
 *							\b CFALSE	indicate that Polarity set to Invert.
 *	@return		None.
 *	@see		NX_PWM_SetPreScale,		NX_PWM_GetPreScale,
 *										NX_PWM_GetPolarity,
 *				NX_PWM_SetPeriod,		NX_PWM_GetPeriod,
 *				NX_PWM_SetDutyCycle,	NX_PWM_GetDutyCycle
 */
void	NX_PWM_SetPolarity( U32 channel, CBOOL bypass )
{
	volatile U16 *pRegister;
	register U32 shift, temp;
	const	U32 mask = 0x01;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > channel );
	NX_ASSERT( (0==bypass) || (1==bypass) );

	pRegister = &__g_pRegister->PWM2[channel/2].PWM_PREPOL;

	if( channel & 1 ){ shift = 15; }
	else				{ shift = 7; }

	temp	= *pRegister;
	temp &= ~(mask << shift );
	temp |= ( (U32)bypass << shift );

//	*pRegister = (U16)temp;
	WriteIOW(pRegister, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Polarity of PWM signal.
 *	@param[in]	channel		Select PWM Channel ( 0 ~ 1 )
 *	@return		\b CTRUE	indicate that Polarity setting is Bypass. \n
 *				\b CFALSE	indicate that Polarity setting is Invert.
 *	@see		NX_PWM_SetPreScale,		NX_PWM_GetPreScale,
 *				NX_PWM_SetPolarity,
 *				NX_PWM_SetPeriod,		NX_PWM_GetPeriod,
 *				NX_PWM_SetDutyCycle,	NX_PWM_GetDutyCycle
 */
CBOOL	NX_PWM_GetPolarity( U32 channel )
{
	volatile U16 *pRegister;
	register U32 shift, temp;
	const	U32 mask = 0x01;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > channel );

	pRegister = &__g_pRegister->PWM2[channel/2].PWM_PREPOL;

	if( channel & 1 ){ shift = 15; }
	else				{ shift = 7; }

	temp	= *pRegister;

	return (CBOOL)( (temp >> shift) & mask );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Period
 *	@param[in]	channel		Select PWM Channel ( 0 ~ 1 )
 *	@param[in]	period		Period Value ( 1 ~ 1024 )
 *	@return		None.
 *	@see		NX_PWM_SetPreScale,		NX_PWM_GetPreScale,
 *				NX_PWM_SetPolarity,		NX_PWM_GetPolarity,
 *										NX_PWM_GetPeriod,
 *				NX_PWM_SetDutyCycle,	NX_PWM_GetDutyCycle
 */
void	NX_PWM_SetPeriod( U32 channel, U32 period )
{
	volatile U16 *pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > channel );
	NX_ASSERT( 1024 >= period && period > 0);

	pRegister = &__g_pRegister->PWM2[channel/2].PWM_PERIOD[channel%2];

	WriteIOW(pRegister, (U16)(period-1));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Period
 *	@param[in]	channel		Select PWM Channel ( 0 ~ 1 )
 *	@return		Period		Value ( 1 ~ 1024 )
 *	@see		NX_PWM_SetPreScale,		NX_PWM_GetPreScale,
 *				NX_PWM_SetPolarity,		NX_PWM_GetPolarity,
 *				NX_PWM_SetPeriod,
 *				NX_PWM_SetDutyCycle,	NX_PWM_GetDutyCycle
 */
U32		NX_PWM_GetPeriod( U32 channel )
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > channel );

	return(U32)(ReadIOW(&__g_pRegister->PWM2[channel/2].PWM_PERIOD[channel%2])+1);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Duty Cycle ( Set the length of PWM Signal's Low area )
 *	@param[in]	channel		Select PWM Channel ( 0 ~ 1 )
 *	@param[in]	duty		duty Value( 1 ~ 1024 )
 *	@return		None.
 *	@remarks	If set to 0, the PWM Output is 1.\n
 *				If set bigger than PWM Period, the PWM Output is 0.
 *	@see		NX_PWM_SetPreScale,		NX_PWM_GetPreScale,
 *				NX_PWM_SetPolarity,		NX_PWM_GetPolarity,
 *				NX_PWM_SetPeriod,		NX_PWM_GetPeriod,
 *										NX_PWM_GetDutyCycle
 */
void	NX_PWM_SetDutyCycle( U32 channel, U32 duty )
{
	volatile U16 *pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > channel );
	NX_ASSERT( 1024 >= duty && duty >= 0 );

	if (0 == duty) duty = 1;

	pRegister = &__g_pRegister->PWM2[channel/2].PWM_DUTY[channel%2];

	WriteIOW(pRegister, (U16)(duty-1));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Duty Cycle
 *	@param[in]	channel		Select PWM Channel ( 0 ~ 1 )
 *	@return		duty Value( 1 ~ 1024 )
 *	@see		NX_PWM_SetPreScale,		NX_PWM_GetPreScale,
 *				NX_PWM_SetPolarity,		NX_PWM_GetPolarity,
 *				NX_PWM_SetPeriod,		NX_PWM_GetPeriod,
 *				NX_PWM_SetDutyCycle
 */
U32		NX_PWM_GetDutyCycle( U32 channel )
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 2 > channel );

	return(U32)(ReadIOW(&__g_pRegister->PWM2[channel/2].PWM_DUTY[channel%2])+1);
}
