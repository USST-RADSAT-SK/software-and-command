/**
 * @file RImageCaptureTask.c
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#include <RImageCaptureTask.h>
#include <RCameraService.h>
#include <RCommon.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/



/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void ImageCaptureTask(void* parameters) {
	int error;

	// ignore the input parameter
	(void)parameters;

	// Set the default automatic image capture interval
	setImageCaptureInterval(IMAGE_CAPTURE_TASK_NORMAL_DELAY_MS);

	infoPrint("ImageCaptureTask started.");
	while (1) {

		// TODO: Uncomment function call when Brian's branch will be ready/merged
		// Check if satellite is currently in downlink/uplink mode (1) or not (0)
		uint8_t commIsActive = 0; //communicationPassModeActive();

		// Check if CubeSense is already in use (1) or not (0)
		uint8_t cubeSenseIsInUse = getCubeSenseUsageState();

		if (!commIsActive && !cubeSenseIsInUse) {
			// Check if ready for a new image capture
			if (getImageReadyForNewCaptureState()) {
				printf("READY for new image capture\n");

				// Request a new capture using the Earth sensor and SRAM2
				error = requestImageCaptureAndDetect(NADIR_SENSOR, SRAM2);
				if (error != SUCCESS) {
					printf("Error capturing an image.\n");
				} else {
					// Successful image capture, proceed to
					// start a task to download the image
					uint8_t imageSize = getImageDownloadSize();
					printf("Image download size = %i\n", imageSize);
					error = requestImageDownload(SRAM2, imageSize);
					if (error != SUCCESS) {
						printf("Error starting image download.\n");
					}
				}
			} else {
				printf("NOT READY for new image capture\n");

				// No request for a new capture and image not yet ready
				// for downlink, so retry to download the image
				if (!getImageReadyForDownlinkState()) {
					uint8_t imageSize = getImageDownloadSize();
					error = requestImageDownload(SRAM2, imageSize);
					if (error != SUCCESS) {
						printf("Error starting image download.\n");
					}
				}
			}
		}

		if (cubeSenseIsInUse) {
			// CubeSense was in use, wait only for a small delay to retry task
			vTaskDelay(IMAGE_CAPTURE_TASK_SHORT_DELAY_MS);
		} else {
			// Normal operations with normal delay between task execution
			vTaskDelay(getImageCaptureInterval());
		}
	}
}


