/**
 * @file RSatelliteWatchdogTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 * Last edited 27 July 2022 by Brian Pitzel
 */

#include <RSatelliteWatchdogTask.h>
#include <RCommon.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** Satellite Watchdog Task delay (in ms). */
#define SATELLITE_WATCHDOG_TASK_DELAY_MS	(100)


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void SatelliteWatchdogTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	while (1) {

		// TODO: implement petting satellite watchdogs

		pdbPetWatchdog();
		vTaskDelay(SATELLITE_WATCHDOG_TASK_DELAY_MS);
	}
}
