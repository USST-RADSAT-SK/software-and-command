/*
 * PinTest.c
 *
 *  Created on: 26-Feb-2013
 *      Author: Akhil Piplani
 */

#include <at91/boards/ISIS_OBC_G20/board.h>
#include <at91/commons.h>
#include <at91/peripherals/pio/pio.h>
#include <at91/utility/trace.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <hal/boolean.h>
#include <hal/Utility/util.h>

// Some pins are not included on purpose
#define AUX_PINS_PIOA	{1<<12 | 1<<13 | 1<<14 | 1<<15 | 1<<16 | 1<<17 | 1<<18 | 1<<19 | 1<<20 | 1<<21 | 1<<26 | 1<<27 | 1<<28 | 1<<29, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define AUX_PINS_PIOB	{1<<0  | 1<<1  | 1<<2  | 1<<3  | 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<20 | 1<<21 | 1<<22 | 1<<23 | 1<<24 | 1<<25 | 1<<26 | 1<<27 | 1<<28 | 1<<29 | 1<<30 | 1<<31, AT91C_BASE_PIOB, AT91C_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}
#define AUX_PINS_PIOC	{1<<4  | 1<<5  | 1<<6  | 1<<7  | 1<<9  | 1<<12 | 1<<13 | 1<<14 | 1<<15, AT91C_BASE_PIOC, AT91C_ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}

Boolean PinTest() {
	unsigned int choice;
	Pin PinsPIOA = AUX_PINS_PIOA;
	Pin PinsPIOB = AUX_PINS_PIOB;
	Pin PinsPIOC = AUX_PINS_PIOC;

	printf("\n\r PinTest: List of Pins Under Test: \n\r");
	printf(" PIOA: 12-21, 26-29 \n\r");
	printf(" PIOB: 0-3, 10-13, 20-31 \n\r");
	printf(" PIOC: 4-7, 9, 12-15 \n\r");
	printf("\n\r Continue? (1=Yes, 0=No) \n\r");
	while(UTIL_DbguGetIntegerMinMax(&choice, 0, 1) == 0);
	if(choice == 0) {
		return TRUE;
	}

	PIO_Configure(&PinsPIOA, PIO_LISTSIZE(&PinsPIOA));
	if(!PIO_Configure(&PinsPIOA, PIO_LISTSIZE(PinsPIOA))) {
		printf(" PinTest: Unable to configure PIOA pins as output! \n\r");
		while(1);
	}

	vTaskDelay(10);

	PIO_Configure(&PinsPIOB, PIO_LISTSIZE(&PinsPIOB));
	if(!PIO_Configure(&PinsPIOB, PIO_LISTSIZE(PinsPIOB))) {
		printf(" PinTest: Unable to configure PIOB pins as output! \n\r");
		while(1);
	}

	vTaskDelay(10);

	PIO_Configure(&PinsPIOC, PIO_LISTSIZE(&PinsPIOC));
	if(!PIO_Configure(&PinsPIOC, PIO_LISTSIZE(PinsPIOC))) {
		printf(" PinTest: Unable to configure PIOC pins as output! \n\r");
		while(1);
	}

	vTaskDelay(10);

	printf("\n\r PinTest: All pins should now be logic-0 (0V). Please check their states now. \n\r");
	printf(" PinTest: Press 1 then Enter when done. \n\r");
	UTIL_DbguGetInteger(&choice);

	PIO_Set(&PinsPIOA);
	vTaskDelay(10);
	PIO_Set(&PinsPIOB);
	vTaskDelay(10);
	PIO_Set(&PinsPIOC);

	vTaskDelay(10);

	printf("\n\r PinTest: All pins should now be logic-1 (3.3V). Please check their states now. \n\r");
	printf(" PinTest: Press 1 then Enter when done. \n\r");
	UTIL_DbguGetInteger(&choice);

	return TRUE;
}
