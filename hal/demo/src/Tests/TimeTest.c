/*
 * TimeTest.c
 *
 *  Created on: 10 dec. 2014
 *      Author: malv
 */

#include "Tests/TimeTest.h"

#include <at91/commons.h>
#include <at91/utility/trace.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <hal/boolean.h>
#include <hal/errors.h>
#include <hal/supervisor.h>
#include <hal/Drivers/SPI.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/I2C.h>
#include <hal/Timing/Time.h>
#include <hal/Timing/RTC.h>

#include <stdlib.h>
#include <string.h>

// Initialization

#define I2CBUS_SPEED		66000	///< Bus speed of main I2C bus in Hz
#define I2CBUS_TIMEOUT		10		///< Timeout per byte for each transmission on I2C bus in .100 msecs

#define TIME_SYNCINTERVAL	60	///< Interval at which to synchronize RTT and RTC in seconds

static void initEpochTest(void) {
	int retVal;
	Time fallbackTime = {.seconds = 0, .minutes = 0, .hours = 0, .day = 1, .date = 1, .month = 1, .year = 0xAA};
	Time tempTime = {0};

	retVal = I2C_start(I2CBUS_SPEED, I2CBUS_TIMEOUT);
	if(retVal != 0)
	{
		TRACE_WARNING("Initializing I2C failed with error %d! \n\r", retVal);
	}

	retVal = SPI_start(bus0_spi, slave0_spi);
	if(retVal != 0)
	{
		TRACE_WARNING("Initializing SPI failed with error %d! \n\r", retVal);
	}

	// Initialize time without a set-time value. The Time module will try to restore time from RTC.
	retVal = Time_start(NULL, TIME_SYNCINTERVAL);
	if(retVal != 0)
	{
		// Non-zero return value from Time does not necessarily mean we can't keep a valid time.
		TRACE_WARNING("Initializing Time failed with error %d! \n\r", retVal);
	}

	retVal = RTC_getTime(&tempTime);
	if(retVal==0 || tempTime.year==0) // RTC power-cycles to Y2K (stored as 0)
	{
		// Time is initialized with a dummy time-value.
		// If time is never set on the satellite, will start with 00:00:00, Sunday, January 1st, 2170 (0xAA = 170).
		// If the ground control ever retrieves time or a time-value close to this is found in any logs, a satellite-reset becomes clear.

		Time_set(&fallbackTime);
		TRACE_DEBUG(" Time restored to fallback value. \n\r");
	}

	retVal = Supervisor_start(NULL, 0);
	if(retVal != E_NO_SS_ERR)
	{
		TRACE_WARNING("Initializing Supervisor failed with error %d! \n\r", retVal);
	}
}

void taskEpochTest() {
	//unsigned int testIteration;
	unsigned int set_epochtime = 946684800;
	unsigned int get_epochtime = 0;
	unsigned long tickcount1 = 0, tickcount2 = 0, tickresult = 0;
	int retVal;

	printf("\n\r taskEpochTest: Initializing. \n\r\n\r");

	initEpochTest();

	printf("\n\r taskEpochTest: Done initializing. \n\r");

	tickcount1 = xTaskGetTickCount();

	Time_setUnixEpoch(951782400);

	//for(testIteration=0; testIteration<1440; testIteration++)
	while(get_epochtime < 1136073600)//1893456001)
	{
		retVal = Time_setUnixEpoch(set_epochtime);
		if(retVal != 0) {
			TRACE_WARNING(" taskThermalTest: Error during Time_setUnixEpoch: %d \n\r", retVal);
		}
		else
		{
			//vTaskDelay(1);

			retVal = Time_getUnixEpoch(&get_epochtime);
			if(retVal != 0) {
				TRACE_WARNING(" taskThermalTest: Error during Time_getUnixEpoch: %d \n\r", retVal);
			}
			else
			{
				if(set_epochtime == get_epochtime)
				{
					if(get_epochtime == 978307200)
					{
						TRACE_DEBUG("\n\r get_epochtime 2001: %d \n\r", get_epochtime);
					}
					if(get_epochtime == 1009843200)
					{
						TRACE_DEBUG("\n\r get_epochtime 2002: %d \n\r", get_epochtime);
					}
					if(get_epochtime == 1041379200)
					{
						TRACE_DEBUG("\n\r get_epochtime 2003: %d \n\r", get_epochtime);
					}
					if(get_epochtime == 1072915200)
					{
						TRACE_DEBUG("\n\r get_epochtime 2004: %d \n\r", get_epochtime);
					}
					if(get_epochtime == 1104537600)
					{
						TRACE_DEBUG("\n\r get_epochtime 2005: %d \n\r", get_epochtime);
					}
					if(get_epochtime == 1136073600)
					{
						TRACE_DEBUG("\n\r get_epochtime 2006: %d \n\r", get_epochtime);
					}
					set_epochtime+=60;
				}
				else
				{
					TRACE_WARNING(" taskThermalTest: Error during comparison between set and get \n\r");
					TRACE_DEBUG("\n\r set_epochtime: %d \n\r", set_epochtime);
					TRACE_DEBUG("\n\r get_epochtime: %d \n\r", get_epochtime);
					break;
				}
				//TRACE_DEBUG("\n\r get_epochtime value: %ld \n\r", get_epochtime);
			}
		}
	}

	tickcount2 = xTaskGetTickCount();

	if(tickcount1 < tickcount2)
	{
		tickresult = tickcount2 - tickcount1;
		TRACE_DEBUG("\n\r tick time: %ld \n\r", tickresult);
	}
	else
	{
		TRACE_DEBUG("\n\r tick time error \n\r");
	}

	// This would cause the watchdog to reset the ARM, this is done on purpose to test the watchdog.
	while(1);

}

Boolean TimeTest() {
	xTaskHandle taskTimeTestHandle;

	xTaskGenericCreate(taskEpochTest, (const signed char*)"taskEpochTest", 1024, NULL, 2, &taskTimeTestHandle, NULL, NULL);

	return FALSE;
}
