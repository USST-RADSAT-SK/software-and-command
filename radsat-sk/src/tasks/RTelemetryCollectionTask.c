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

/** How many telemetry readings to collect per hour. */
#define TELEMETRY_READINGS_PER_HOUR			(10)

/** Telemetry Collection Task delay (in ms). */
#define TELEMETRY_COLLECTION_TASK_DELAY_MS	(MS_PER_HOUR / TELEMETRY_READINGS_PER_HOUR)


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void TelemetryCollectionTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	while (1) {

		// TODO: implement telemetry collection

		debugPrint("TelemetryCollectionTask(): About to collect satellite telemetry data.\n");

		vTaskDelay(TELEMETRY_COLLECTION_TASK_DELAY_MS);
	}
}
