/**
 * @file RSatelliteWatchdogTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 * Last edited 27 July 2022 by Brian Pitzel
 */

#include <RSatelliteWatchdogTask.h>
#include <RCommon.h>
#include <RPdb.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/



/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void SatelliteWatchdogTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	infoPrint("SatelliteWatchdogTask started.");
	while (1) {

		pdbPetWatchdog();
		vTaskDelay(SATELLITE_WATCHDOG_TASK_DELAY_MS);
	}
}
