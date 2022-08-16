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


		// TEST PURPOSES ONLY
		int err = 0;
		CameraTelemetry cameraSettings = {0};
		err = cameraTelemetry(&cameraSettings);
		if (err != SUCCESS) {
			printf("Error getting camera settings.\n");
		}
		cameraSettings.cameraOneTelemetry.exposure = 150;
		cameraSettings.cameraTwoTelemetry.exposure = 150;
		cameraSettings.cameraOneTelemetry.detectionThreshold = 50;
		cameraSettings.cameraTwoTelemetry.detectionThreshold = 50;
		//cameraSettings.cameraOneTelemetry.autoAdjustMode = 1;
		//cameraSettings.cameraTwoTelemetry.autoAdjustMode = 1;
		err = cameraConfig(&cameraSettings);
		if (err != SUCCESS) {
			printf("Error setting camera settings.\n");
		}
		err = cameraTelemetry(&cameraSettings);
		if (err != SUCCESS) {
			printf("Error getting camera settings.\n");
		}

		// TEST PURPOSES ONLY: 0 = capture & download | 1 = attitude detection
		uint8_t testing = 1;

		if (testing == 0) {
			err = captureImage();
			vTaskDelay(1000);
			if (err != SUCCESS) {
				printf("Error capturing image.\n\n");
			} else {
				printf("Image captured successfully.\n\n");

				full_image_t image = {0};
				err = downloadImage(1, 1, 3, &image);
				if (err != SUCCESS) {
					printf("Error downloading image.\n\n");
				} else {
					printf("\nImage %i downloaded successfully.\n", image.image_ID);
					//printf("--- START OF FRAMES ---\n");
					//for(int i = 0; i < MAXIMUM_FRAMES; i++) {
					//	for(int j = 0; j < FRAME_BYTES; j++) {
					//		printf("%x", image.imageFrames[i]->image_bytes[j]);
					//	}
					//}
					//printf("\n--- END OF FRAMES ---\n");
				}
			}
		} else {
			detection_results_t data = {0};
			uint8_t counter = 0;
			err = 0;
			while(counter < 10) {
				printf("Try # %i\n", counter);
				err = detectionAndInterpret(&data);
				if (err) {
					printf("Error returned from detectionAndInterpret: %i\n", err);
				}
				vTaskDelay(2000);  // this delay was working earlier (but with 1s)
				counter++;
			}
			//detectionAndInterpret(&data);

			printf("--- Final Axis Data ---\n");
			printf("sunSensorX   = %d\n", data.sunSensorX);
			printf("sunSensorY   = %d\n", data.sunSensorY);
			printf("sunSensorZ   = %d\n", data.sunSensorZ);
			printf("imageSensorX = %d\n", data.imageSensorX);
			printf("imageSensorY = %d\n", data.imageSensorY);
			printf("imageSensorZ = %d\n", data.imageSensorZ);
			printf("----------------------\n\n");
		}

		vTaskDelay(ADCS_CAPURE_TASK_DELAY_MS);
	}
}
