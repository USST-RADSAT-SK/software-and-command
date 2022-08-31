/**
 * @file RDosimeterCollectionTask.c
 * @date December 28, 2021
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#include <RDosimeterCollectionTask.h>
#include <RDosimeter.h>
#include <RCommon.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** How many dosimeter payload readings to collect per day. */
#define DOSIMETER_READINGS_PER_DAY			(6)

/** Dosimeter Collection Task delay (in ms). */
#define DOSIMETER_COLLECTION_TASK_DELAY_MS	(1000)


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void DosimeterCollectionTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	int error = 0;

	while (1) {

		// TODO: check flags (once they exist) to prevent running this task during communication mode

		debugPrint("DosimeterCollectionTask(): About to collect Dosimeter payload data.\n");

		// collect all readings from dosimeter
		error = dosimeterCollectData();

		// if an error was detected, try again once more (if it fails again, no data will be taken this time)
		if (error != 0)
			dosimeterCollectData();

		vTaskDelay(DOSIMETER_COLLECTION_TASK_DELAY_MS);
	}
}
