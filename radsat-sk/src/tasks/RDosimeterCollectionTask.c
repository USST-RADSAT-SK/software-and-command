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


void DosimeterCollectionTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	while(1) {
		// collect all readings from dosimeter
		int error = dosimeterCollectData();

		// if an error was detected, try again once (if it fails again, no data will be taken this time)
		if (error != 0)
			dosimeterCollectData();

		// delay
		vTaskDelay(DOSIMETER_DELAY_MS);
	}
}
