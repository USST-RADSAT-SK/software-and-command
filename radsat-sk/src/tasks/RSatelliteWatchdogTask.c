/**
 * @file RSatelliteWatchdogTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 * Last edited 27 July 2022 by Brian Pitzel
 */

#include <RSatelliteWatchdogTask.h>
#include <Rtransceiver.h>
#include <RCommon.h>
#include <RPdb.h>
#include <RDebug.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


#ifdef INFOBAR
	#define printInfoBar debugPrint("[%-8.8s]\r", throbber()); fflush(stdout);
#else
	#define printInfoBar
#endif


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

		printInfoBar
		pdbPetWatchdog();
		resetBeacon();
		vTaskDelay(SATELLITE_WATCHDOG_TASK_DELAY_MS);
	}
}
