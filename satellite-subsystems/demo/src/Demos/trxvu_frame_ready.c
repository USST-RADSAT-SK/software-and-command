/*
 * trxvu_frame_ready.c
 *
 *  Created on: 18 mei 2018
 *      Author: PBOT
 */

#include "trxvu_frame_ready.h"

#include <at91/boards/ISIS_OBC_G20/board.h>
#include <at91/peripherals/pio/pio.h>
#include <at91/peripherals/pio/pio_it.h>

static const Pin _pin = {1 << 19, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_INPUT, PIO_DEFAULT};
static TRXVU_FR_Callback_t _callback;

static void _Handler(const Pin * pPin);

void TRXVU_FR_Enable(TRXVU_FR_Callback_t callback)
{
    //NOTE: This assumes PIO_InitializeInterrupts is called (see main)
	if(!callback)
	{
		return;
	}

	_callback = callback;

	PIO_Configure( &_pin,1 );
	PIO_ConfigureIt( &_pin, _Handler);
	PIO_EnableIt( &_pin );
}

void TRXVU_FR_Disable( void )
{
	PIO_DisableIt(&_pin);
}

int TRXVU_FR_IsReady( void )
{
	return (int)PIO_Get(&_pin);
}

static void _Handler(const Pin * pPin)
{
	if( PIO_Get(pPin) == 1 )
	{
		/* XXX: no need for null check since only enabled if callback */
		_callback();
	}
}
