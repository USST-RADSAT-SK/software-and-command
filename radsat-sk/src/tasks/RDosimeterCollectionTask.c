/**
 * @file RDosimeterCollectionTask.c
 * @date December 28, 2021
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#include <RDosimeter.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define DOSIMETER_READINGS_PER_DAY	(6)
#define MS_PER_DAY					(1000 * 60 * 60 * 24)
#define DOSIMETER_DELAY_MS			(MS_PER_DAY / DOSIMETER_READINGS_PER_DAY)


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void DosimeterCollectionTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	int error = 0;

	while (1) {

		// TODO: check flags (once they exist) to prevent running this task during communication mode

		// collect all readings from dosimeter
		error = dosimeterCollectData();

		// if an error was detected, try again once (if it fails again, no data will be taken this time)
		if (error)
			dosimeterCollectData();

		// delay
		vTaskDelay(DOSIMETER_DELAY_MS);
	}
}
