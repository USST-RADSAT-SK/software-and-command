/**
 * @file RTelemetryCollectionTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RTelemetryCollectionTask.h>
#include <RCommon.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void TelemetryCollectionTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	infoPrint("TelemetryCollectionTask started.");
	while (1) {

		// TODO: implement telemetry collection

		infoPrint("Collecting satellite telemetry data.");

		vTaskDelay(TELEMETRY_COLLECTION_TASK_DELAY_MS);
	}
}
