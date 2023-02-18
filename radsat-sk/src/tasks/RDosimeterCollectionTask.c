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


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void DosimeterCollectionTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	int error = 0;

	infoPrint("DosimeterCollectionTask started.");
	while (1) {
		infoPrint("Collecting Dosimeter data!!!");

		// TODO: check flags (once they exist) to prevent running this task during communication mode

		// collect all readings from dosimeter
		error = dosimeterCollectData();
		// if an error was detected, try again once more (if it fails again, no data will be taken this time)
		if (error != 0){
			warningPrint("Failled to collect dosimeter data, trying again");
			dosimeterCollectData();
		}

		vTaskDelay(DOSIMETER_COLLECTION_TASK_DELAY_MS);
	}
}
