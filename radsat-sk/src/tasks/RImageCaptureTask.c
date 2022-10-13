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

/** How many images to capture per day. */
#define IMAGE_CAPTURES_PER_DAY					(1)

/** Image Capture Task normal delay (in ms). */
#define IMAGE_CAPTURE_TASK_NORMAL_DELAY_MS		(MS_PER_DAY / IMAGE_CAPTURES_PER_DAY)

/** Image Capture Task short delay (in ms), used when task couldn't execute because CubeSense is in use. */
#define IMAGE_CAPTURE_TASK_SHORT_DELAY_MS		(10000)


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void ImageCaptureTask(void* parameters) {
	int error;

	// ignore the input parameter
	(void)parameters;

	// Set the default automatic image capture interval
	setImageCaptureInterval(IMAGE_CAPTURE_TASK_NORMAL_DELAY_MS);

	while (1) {

		printf("Image capture interval = %i\n", getImageCaptureInterval());

		// TODO: Uncomment function call when Brian's branch will be ready/merged
		// Check if satellite is currently in downlink/uplink mode (1) or not (0)
		uint8_t commIsActive = 0; //communicationPassModeActive();

		// Check if CubeSense is already in use (1) or not (0)
		uint8_t cubeSenseIsInUse = getCubeSenseUsageState();
		printf("ImageCaptureTask(): cubeSenseIsInUse = %d\n", cubeSenseIsInUse);

		if (!commIsActive && !cubeSenseIsInUse) {
			// Check if ready for a new image capture
			if (getImageReadyForNewCaptureState()) {
				printf("READY for new image capture\n");

				// Request a new capture using the Earth sensor and SRAM2
				error = requestImageCaptureAndDetect(NADIR_SENSOR, SRAM2);
				if (error != SUCCESS) {
					printf("Error capturing an image.\n");
				} else {
					printf("Image captured successfully!\n");

					// Start a task to download the image
					uint8_t imageSize = getImageDownloadSize();
					printf("Image download size = %i\n", imageSize);
					error = requestImageDownload(SRAM2, imageSize);
					if (error != SUCCESS) {
						printf("Error starting image download.\n");
					} else {
						printf("Image download has started!\n");
					}
				}
			} else {
				printf("NOT READY for new image capture\n");

				// No request for a new capture and image not yet ready
				// for downlink, so retry to download the image
				if (!getImageReadyForDownlinkState()) {
					uint8_t imageSize = getImageDownloadSize();
					printf("Image download size = %i\n", imageSize);
					error = requestImageDownload(SRAM2, imageSize);
					if (error != SUCCESS) {
						printf("Error starting image download.\n");
					} else {
						printf("Image download has started!\n");
					}
				}
			}
		}


		// TODO: Call this during downlink to get camera frames one by one
		// Validate that an image is ready for downlink
		if (getImageReadyForDownlinkState()) {
			printf("Image is READY for downlink\n");

			uint16_t imageFramesCount = getImageFramesCount();
			printf("Number of frames to downlink: %i\n", imageFramesCount);
			if (imageFramesCount > 0) {
				image_frame_t frame = {0};
				for (int i = 0; i < imageFramesCount; i++) {
					uint8_t frameSize = imageTransferNextFrame(&frame);
					if (frameSize == 0) {
						printf("Error getting frame #%i (frame of size 0)...\n", i);
						break;
					} else {
						printf("Frame #%i copied successfully: ", frame.frameIndex);
						for	(int j = 0; j < 9; j++) {
							printf("%i, ", frame.image_bytes[j]);
						}
						printf("%i\n", frame.image_bytes[9]);
					}
				}

				/*printf("Get first frame\n");
				uint8_t frameSize = imageTransferNextFrame(&frame);
				if (frameSize == 0) {
					printf("Error getting frame #%i (frame of size 0)\n", 0);
					break;
				} else {
					for	(int i = 0; i < 128; i++) {
						printf("%i, ", frame[i]);
					}
					printf("\n");
				}

				printf("Get frame #10\n");
				frameSize = imageTransferSpecificFrame(&frame, 10);
				if (frameSize == 0) {
					printf("Error getting frame #%i (frame of size 0)\n", 10);
					break;
				} else {
					for	(int i = 0; i < 128; i++) {
						printf("%i, ", frame[i]);
					}
					printf("\n");
				}

				printf("Set index and get frame #20\n");
				setImageTransferFrameIndex(19);
				frameSize = imageTransferNextFrame(&frame);
				if (frameSize == 0) {
					printf("Error getting frame #%i (frame of size 0)\n", 20);
					break;
				} else {
					for	(int i = 0; i < 128; i++) {
						printf("%i, ", frame[i]);
					}
					printf("\n");
				}

				printf("Get current frame\n");
				frameSize = imageTransferCurrentFrame(&frame);
				if (frameSize == 0) {
					printf("Error getting frame #%i (frame of size 0)\n", 20);
					break;
				} else {
					for	(int i = 0; i < 128; i++) {
						printf("%i, ", frame[i]);
					}
					printf("\n");
				}*/
			}
		} else {
			printf("NO image is ready for downlink\n");
		}

		printf("\nImageCaptureTask(): Finished task.\n");

		if (cubeSenseIsInUse) {
			// CubeSense was in use, wait only for a small delay to retry task
			vTaskDelay(IMAGE_CAPTURE_TASK_SHORT_DELAY_MS);
		} else {
			// Normal operations with normal delay between task execution
			//vTaskDelay(getImageCaptureInterval());
			vTaskDelay(10000);
		}
	}
}


