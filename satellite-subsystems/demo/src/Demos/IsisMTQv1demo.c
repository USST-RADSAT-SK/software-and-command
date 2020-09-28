/*
 * IsisMTQv1demo.c
 *
 *  Created on: 23 mrt. 2015
 *      Author: malv
 */
#include "IsisMTQv1demo.h"
#include "common.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/LED.h>
#include <hal/boolean.h>

#include <satellite-subsystems/IsisMTQv1.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Boolean getTemperature(void)
{
	unsigned short curr_temp = 0;

	printf("\r\n get MTQ Temperature \r\n");

	print_error(IsisMTQv1GetTemperature(0, &curr_temp));


	printf("\r\n Temperature: %d (C)\r\n", curr_temp);

	return TRUE;
}

static Boolean selectAndExecuteMtqDemoTest(void)
{
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf( "\n\r Select a test to perform: \n\r");
	printf("\t 1) MTQ get temperature  \n\r");
	printf("\t 2) Return to main menu \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 2) == 0);

	switch(selection)
	{
		case 1:
			offerMoreTests = getTemperature();
			break;
		case 2:
			offerMoreTests = FALSE;
			break;

		default:
			break;
	}

	return offerMoreTests;
}

static void initmain(void)
{
    unsigned char i2c_address = 0x10;

    print_error(isisMTQv1_initialize(&i2c_address, 1));
}

static void Mtqv1_mainDemo(void)
{
	Boolean offerMoreTests = FALSE;

	while(1)
	{
		offerMoreTests = selectAndExecuteMtqDemoTest();

		if(offerMoreTests == FALSE)
		{
			break;
		}
	}
}

Boolean MTQv1test(void)
{
	initmain();
	Mtqv1_mainDemo();

	return TRUE;
}
