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
	adcs_burst burstResults = { 0 };


	infoPrint("AdcsCaptureTask started.");
	while (1) {

		// TODO: Uncomment function call when Brian's branch will be ready/merged
		// Check if satellite is currently in downlink/uplink mode (1) or not (0)
		uint8_t commIsActive = 0; //communicationPassModeActive();

		// Check if CubeSense is already in use (1) or not (0)
		uint8_t cubeSenseIsInUse = getCubeSenseUsageState();

		// Check if ready for a new ADCS burst measurements (1) or not (0)
		uint8_t adcsReadyForNewBurst = getADCSReadyForNewBurstState();

		if (!commIsActive && !cubeSenseIsInUse && adcsReadyForNewBurst) {
			infoPrint("Starting ADCS burst measurements\n");
			error = takeADCSBurstMeasurements(&burstResults);
			if (error) {
				errorPrint("Failed to capture ADCS measurements...\n");
			}
			//fileTransferAddMessage(&burstResults, sizeof(burstResults), file_transfer_adcs_burst_tag );
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
