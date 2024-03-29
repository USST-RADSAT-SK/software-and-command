/**
 * @file RAdcsCaptureTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RAdcsCaptureTask.h>
#include <RCameraService.h>
#include <RCommon.h>
#include <RCamera.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** How many ADCS readings to capture per hour. */
#define ADCS_CAPTURES_PER_HOUR					(1)

/** ADCS Capture Task normal delay (in ms). */
#define ADCS_CAPTURE_TASK_NORMAL_DELAY_MS		(MS_PER_HOUR / ADCS_CAPTURES_PER_HOUR)

/** ADCS Capture Task short delay (in ms). */
#define ADCS_CAPTURE_TASK_SHORT_DELAY_MS		(10000)


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void AdcsCaptureTask(void* parameters) {
	int error;

	// ignore the input parameter
	(void)parameters;

	// Set the default automatic ADCS capture interval
	setADCSCaptureInterval(ADCS_CAPTURE_TASK_NORMAL_DELAY_MS);

	// Initialize the ADCS capture settings (5 measurements, 5 seconds between measurements)
	setADCSBurstSettings(5, 5000);

	while (1) {

		// TODO: Uncomment function call when Brian's branch will be ready/merged
		// Check if satellite is currently in downlink/uplink mode (1) or not (0)
		uint8_t commIsActive = 0; //communicationPassModeActive();

		// Check if CubeSense is already in use (1) or not (0)
		uint8_t cubeSenseIsInUse = getCubeSenseUsageState();

		// Check if ready for a new ADCS burst measurements (1) or not (0)
		uint8_t adcsReadyForNewBurst = getADCSReadyForNewBurstState();

		if (!commIsActive && !cubeSenseIsInUse && adcsReadyForNewBurst) {
			printf("Starting ADCS burst measurements\n");
			error = takeADCSBurstMeasurements();
			if (error != 0) {
				printf("Failed to capture ADCS measurements...\n");
			}
		}

		if (cubeSenseIsInUse) {
			// CubeSense was in use, wait only for a small delay to retry task
			vTaskDelay(ADCS_CAPTURE_TASK_SHORT_DELAY_MS);
		} else {
			// Normal operations with normal delay between task execution
			vTaskDelay(getADCSCaptureInterval());
		}
	}
}
