/**
 * @file RAdcsCaptureTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RAdcsCaptureTask.h>
#include <RCommon.h>
#include <RCamera.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** How many ADCS readings to capture per hour. */
#define ADCS_CAPTURES_PER_HOUR	(1)

/** ADCS Capture Task delay (in ms). */
#define ADCS_CAPURE_TASK_DELAY_MS	(MS_PER_HOUR / ADCS_CAPTURES_PER_HOUR)


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void AdcsCaptureTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	while (1) {

		// TODO: implement ADCS capture

		debugPrint("AdcsCaptureTask(): About to capture ADCS data.\n");

		detection_results_t data = {0};
		detectionAndInterpret(&data);

		printf("\n--- Final Axis Data ---\n");
		printf("sunSensorX = %d\n", data.sunSensorX);
		printf("sunSensorY = %d\n", data.sunSensorY);
		printf("sunSensorZ = %d\n", data.sunSensorZ);
		printf("imageSensorX = %d\n", data.imageSensorX);
		printf("imageSensorY = %d\n", data.imageSensorY);
		printf("imageSensorZ = %d\n", data.imageSensorZ);
		printf("----------------------\n\n");

		vTaskDelay(ADCS_CAPURE_TASK_DELAY_MS);
	}
}
