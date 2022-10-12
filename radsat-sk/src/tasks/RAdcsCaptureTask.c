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

	// Initialize the ADCS capture settings (5 measurements, 5 seconds between measurements)
	setADCSCaptureSettings(5, 5000);

	while (1) {

		// TODO: Uncomment function call when Brian's branch will be ready/merged
		// Check if satellite is currently in downlink/uplink mode (1) or not (0)
		uint8_t commIsActive = 0; //communicationPassModeActive();

		// Check if CubeSense is already in use (1) or not (0)
		uint8_t cubeSenseIsInUse = getCubeSenseUsageState();
		printf("AdcsCaptureTask(): cubeSenseIsInUse = %d\n", cubeSenseIsInUse);

		// Check if ready for a new ADCS burst measurements (1) or not (0)
		uint8_t adcsReadyForNewBurst = getADCSReadyForNewBurstState();

		if (!commIsActive && !cubeSenseIsInUse && adcsReadyForNewBurst) {
			printf("Starting ADCS burst measurements\n");
			error = takeADCSBurstMeasurements();
			if (error != 0) {
				printf("Failed to capture ADCS measurements...\n");
			} else {
				printf("Successfully captured ADCS measurements.\n");
			}
		}

		// TODO: Call this during downlink to get ADCS burst results
		uint8_t adcsReadyForDownlink = !getADCSReadyForNewBurstState();
		if (adcsReadyForDownlink) {
			adcs_detection_results_t * adcsResults = getADCSBurstResults();
			if (adcsResults != NULL) {
				printf("Number of valid measurements: %d\n", adcsResults->validMeasurementsCount);

				for (int i = 0; i < adcsResults->validMeasurementsCount; i++) {
					printf("Sun timestamp: %lu\n", adcsResults->results[i].sunTimestamp);
					printf("Sun angles: %i | %i\n", adcsResults->results[i].sunAlphaAngle, adcsResults->results[i].sunBetaAngle);
					printf("Earth timestamp: %lu\n", adcsResults->results[i].nadirTimestamp);
					printf("Earth angles: %i | %i\n", adcsResults->results[i].nadirAlphaAngle, adcsResults->results[i].nadirBetaAngle);
				}
			}
		}


		printf("\nAdcsCaptureTask(): Finished task.\n");

		if (cubeSenseIsInUse) {
			// CubeSense was in use, wait only for a small delay to retry task
			vTaskDelay(ADCS_CAPTURE_TASK_SHORT_DELAY_MS);
		} else {
			// Normal operations with normal delay between task execution
			//vTaskDelay(ADCS_CAPTURE_TASK_NORMAL_DELAY_MS);
			vTaskDelay(10000);
		}
	}
}
