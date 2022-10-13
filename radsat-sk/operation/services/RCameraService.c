/**
 * @file RCameraService.c
 * @date September 28, 2022
 * @author
 */

#include <RCameraService.h>
#include <RCameraCommon.h>
#include <RCamera.h>
#include <RCommon.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>

/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/* Number of attempts for image capture with successful detection results */
#define VALID_IMAGE_RETRY_COUNT			(10)

/* Delay in milliseconds to allow image capture to be completed */
#define IMAGE_CAPTURE_DELAY_MS			(1000)

/** Stack size (in bytes) allotted to the image download FreeRTOS Task. */
#define DOWNLOAD_TASK_STACK_SIZE		(configMINIMAL_STACK_SIZE + 50)

/** Current camera settings and initialization flag. **/
static CameraSettings cameraSettings = { 0 };
//static uint8_t cameraSettingsInitialized = 0;

/** Interval (in ms) for the automatic image and ADCS capture tasks. **/
int adcsCaptureInterval = 0;
int imageCaptureInterval = 0;

/** Size used for image download during automatic image capture **/
/** 0 = 1024x1024, 1 = 512x512, 2 = 256x256, 3 = 128x128, 4 = 64x64 **/
uint8_t imageDownloadSize = 4;

/** Pointer to a local image frames prepared for downlink. */
static full_image_t *image;
/** Flag indicating image is ready for downlink. **/
static uint8_t imageReadyForDownlink = 0;
/** Flag indicating system ready for a new image capture. **/
static uint8_t imageReadyForNewCapture = 1;

/** Index of the current image frame prepared for downlink. */
static int currentImageFrameIndex = -1;

/** Flag indicating that the CubeSense is currently in use. Used to prevent conflicts. **/
static uint8_t cubeSenseIsInUse = 0;

/** FreeRTOS Task Handles. */
static xTaskHandle imageDownloadTaskHandle;

/** Image Download Task Priority. Periodically download image frames; medium priority task. */
static const int imageDownloadTaskPriority = configMAX_PRIORITIES - 3;

/* Struct for image download parameters and local variable */
typedef struct _image_download_t {
	uint8_t sram;
	uint8_t size;
} image_download_t;
static image_download_t downloadParameters = {0};

/* Struct for ADCS burst measurement parameters and local variable */
typedef struct _adcs_capture_settings_t {
	uint8_t nbMeasurements;
	int interval;
} adcs_capture_settings_t;
static adcs_capture_settings_t adcsSettings = {0};

/** Pointer to a local ADCS measurements results prepared for downlink. */
static adcs_detection_results_t *adcsResults;
/** Flag indicating ADCS is ready for a new burst (1=ready, 0=not ready). **/
static uint8_t adcsReadyForNewBurst = 1;

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

void ImageDownloadTask(void* parameters);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/*
 * Trigger a CubeSense reset.
 *
 * @param resetOption defines the system to reset, 1 = reset communication, 2 = reset cameras, 3 = reset MCU
 * @return 0 on success, otherwise failure
 */
int requestReset(uint8_t resetOption) {
	// Check for invalid reset option
	if (resetOption < 1 || resetOption > 3)
		return E_GENERIC;

	// Flag CubeSense as "in use"
	cubeSenseIsInUse = 1;

	int error = executeReset(resetOption);
	cubeSenseIsInUse = 0;
	return error;
}


/*
 * Set settings for both CubeSense cameras.
 *
 * @param sunSettings defines the Sun camera settings to set
 * @param nadirSettings defines the nadir camera settings to set
 * @return error, 0 for success, otherwise failure
 */
int setCamerasSettings(CameraSettings_ConfigurationSettings sunSettings, CameraSettings_ConfigurationSettings nadirSettings) {
	// Flag CubeSense as "in use"
	cubeSenseIsInUse = 1;

	int error;

	// TODO: Do we need this or not? Is the FULL struct sent via telecommand?
	/*if (!cameraSettingsInitialized) {
		printf("Getting camera settings...\n");
		error = getSettings(&cameraSettings);
		if (error != SUCCESS) {
			printf("Failed to get camera settings...\n");
			cubeSenseIsInUse = 0;
			return E_GENERIC;
		}
		printf("Exposure = %i\n", cameraSettings.cameraTwoSettings.exposure);
		printf("Detection Threshold = %i\n", cameraSettings.cameraTwoSettings.detectionThreshold);
		cameraSettingsInitialized = 1;
	}*/

	cameraSettings.cameraOneSettings = sunSettings;
	cameraSettings.cameraTwoSettings = nadirSettings;

	// TODO: TO DELETE
	//cameraSettings.cameraTwoSettings.exposure = 10000;
	//cameraSettings.cameraTwoSettings.detectionThreshold = 150;

	error = setSettings(&cameraSettings);
	cubeSenseIsInUse = 0;
	if (error != SUCCESS) {
		printf("Failed to update camera settings...\n");
		return E_GENERIC;
	}

	return SUCCESS;
}



/*
 * Set the interval used for automatic ADCS capture.
 *
 * @param interval defines the delay between each run of the ADCS capture task
 */
void setADCSCaptureInterval(int interval) {
	adcsCaptureInterval = interval;
}


/*
 * Get the interval used for automatic ADCS capture.
 *
 * @return interval, in milliseconds
 */
int getADCSCaptureInterval(void) {
	return adcsCaptureInterval;
}

/*
 * Set the capture settings used for ADCS measurement, which
 * will execute a sequential burst of measurements.
 *
 * @param nbMeasurements defines the number of measurements in the burst
 * @param interval defines the interval (in ms) between measurements in a burst
 */
void setADCSBurstSettings(uint8_t nbMeasurements, int interval) {
	adcsSettings.nbMeasurements = nbMeasurements;
	adcsSettings.interval = interval;
}


/*
 * Take a burst of measurements for ADCS and
 * store the results in a static variable.
 *
 * @return error, 0 for success, otherwise failure
 */
int takeADCSBurstMeasurements(void) {
	// Flag CubeSense as "in use"
	cubeSenseIsInUse = 1;

	int error;

	// Free allocated memory of struct
	free(adcsResults);

	// Allocate memory to struct to store measurements
	adcsResults = initializeNewADCSResults(adcsSettings.nbMeasurements);

	// Run a first image capture & detect so the first results can be read
	error = triggerNewDetectionForBothSensors();
	if (error != SUCCESS) {
		printf("Failed to trigger a first capture and detect...\n");
	}

	// Wait for first captures to be done
	vTaskDelay(adcsSettings.interval);

	// Iterate to get the detection results
	uint8_t resultIndex = 0;
	printf("Number of measurements = %d\n", adcsSettings.nbMeasurements);
	for (int i = 0; i < adcsSettings.nbMeasurements; i++) {
		printf("Detection measurement #%d\n", i);
		// Get detection results and trigger a new detection
		detection_results_t detectionResult = {0};
		error = getResultsAndTriggerNewDetection(&detectionResult);
		if (error == SUCCESS) {
			// Store the successful result
			adcsResults->results[resultIndex] = detectionResult;
			resultIndex++;
		}

		// Wait before the next measurement
		vTaskDelay(adcsSettings.interval);
	}
	// Store the number of valid measurements
	adcsResults->validMeasurementsCount = resultIndex;

	// Set the ADCS readiness flag so no new burst is executed
	// if enough detection results were successfully
	adcsReadyForNewBurst = adcsResults->validMeasurementsCount > adcsSettings.nbMeasurements/2 ? 0 : 1;

	cubeSenseIsInUse = 0;
	return SUCCESS;
}


/*
 * Get the ADCS burst measurement results.
 *
 * @return struct holding the ADCS measurement results for the burst
 */
adcs_detection_results_t * getADCSBurstResults(void) {
	return adcsResults;
}


/*
 * Memory allocation and initialization of a new ADCS results structure
 *
 * @note it is important to free the allocated pointer memory after usage
 * @param nbMeasurements defines the number of measurements desired
 * @return pointer to the new ADCS results struct in memory
 */
adcs_detection_results_t * initializeNewADCSResults(uint8_t nbMeasurements) {
	adcs_detection_results_t *adcs = calloc(1, sizeof(*adcs) + sizeof(detection_results_t) * nbMeasurements);
	printf("ADCS Results allocated memory size = %i bytes\n", sizeof(*adcs) + sizeof(detection_results_t) * nbMeasurements);
	return adcs;
}


/*
 * Set the flag indicating that ADCS is ready for a new burst of measurements.
 */
void setADCSReadyForNewBurst(void) {
	adcsReadyForNewBurst = 1;
}


/*
 * Get the flag indicating if ADCS is ready for a new burst.
 * This flag needs to be reset to 1 via a telecommand.
 *
 * @return ADCS readiness, 1 for ready, 0 for not ready
 */
uint8_t getADCSReadyForNewBurstState(void) {
	return adcsReadyForNewBurst;
}


/*
 * Set the interval used for automatic image capture.
 *
 * @param interval defines the delay between each run of the image capture task
 */
void setImageCaptureInterval(int interval) {
	imageCaptureInterval = interval;
}


/*
 * Get the interval used for automatic image capture.
 *
 * @return interval, in milliseconds
 */
int getImageCaptureInterval(void) {
	return imageCaptureInterval;
}


/*
 * Set the image download size used for automatic image capture.
 *
 * @param size defines the image download size ranging from 0 (largest) to 4 (smallest)
 */
void setImageDownloadSize(uint8_t size) {
	// Check for invalid size
	if (size > 4)
		size = 4;

	imageDownloadSize = size;
}


/*
 * Get the image download size used for automatic image capture.
 *
 * @return size, ranging from 0 to 4 (see CubeSense's manual)
 */
uint8_t getImageDownloadSize(void) {
	return imageDownloadSize;
}


/*
 * Trigger a new image capture given the specified parameters.
 *
 * @param camera defines which sensor to use to capture an image, 0 = Camera 1, 1 = Camera 2
 * @param sram defines which SRAM to use on CubeSense, 0 = SRAM1, 1 = SRAM2
 * @param size defines the image size used to allocate memory, 0 = 1024x1024, 1 = 512x512, 2 = 256x256, 3 = 128x128, 4 = 64x64
 * @param location defines which SRAM slot to use within selected SRAM, 0 = top, 1 = bottom
 * @return error, 0 on success, otherwise failure
 */
int requestImageCapture(uint8_t camera, uint8_t sram, uint8_t location) {
	// Flag CubeSense as "in use"
	cubeSenseIsInUse = 1;

	int error = captureImage(camera, sram, location);
	cubeSenseIsInUse = 0;
	return error;
}


/*
 * Trigger a new image capture given the specified parameters, run
 * the detection algorithm on said image, and download the image into RAM.
 *
 * @param camera defines which sensor to use to capture an image, 0 = Camera 1, 1 = Camera 2
 * @param sram defines which SRAM to use on CubeSense, 0 = SRAM1, 1 = SRAM2
 * @param size defines the image size used to allocate memory, 0 = 1024x1024, 1 = 512x512, 2 = 256x256, 3 = 128x128, 4 = 64x64
 * @return error, 0 on success, otherwise failure
 */
int requestImageCaptureDetectAndDownload(uint8_t camera, uint8_t sram, uint8_t size) {
	int error = requestImageCaptureAndDetect(camera, sram);
	if (error != SUCCESS) {
		return error;
	}

	error = requestImageDownload(sram, size);
	if (error != SUCCESS) {
		return error;
	}

	return SUCCESS;
}


/*
 * Trigger a new image capture given the specified parameters
 * and run the detection algorithm on said image.
 * Once we have successful detection, another picture is taken in the bottom SRAM location.
 *
 * @param camera defines which sensor to use to capture an image, 0 = Camera 1, 1 = Camera 2
 * @param sram defines which SRAM to use on CubeSense, 0 = SRAM1, 1 = SRAM2
 * @return error, 0 on success, otherwise failure
 */
int requestImageCaptureAndDetect(uint8_t camera, uint8_t sram) {
	// Flag CubeSense as "in use"
	cubeSenseIsInUse = 1;

	int error;
	uint8_t retryCount = 0;
	uint8_t detectionStatus = 1;

	// Capture an image and run the detection algorithm to get the results
	// Retry a few times if results are invalid
	while (detectionStatus != 0 && retryCount < VALID_IMAGE_RETRY_COUNT) {
		printf("\nImage capture attempt: %d\n", retryCount);
		// Capture image
		error = captureImageAndDetect(camera, sram);
		if (error == SUCCESS) {
			// Wait to allow the image capture to complete
			vTaskDelay(IMAGE_CAPTURE_DELAY_MS);

			// Get the detection status using the nadir sensor
			detectionStatus = getSingleDetectionStatus(camera == 0 ? sensor1 : sensor2);
		} else {
			printf("Failed during image capture...\n");
		}

		// Wait before a retry
		vTaskDelay(IMAGE_CAPTURE_DELAY_MS);

		// Increase retry counter
		retryCount++;
	}

	// Detection results are still invalid, stop trying
	if (detectionStatus != 0) {
		printf("Failed to capture a valid image...\n");
		// Flag CubeSense as "not used"
		cubeSenseIsInUse = 0;
		return E_GENERIC;
	}

	// Detection success, so take another image in the bottom location
	error = requestImageCapture(camera, sram, BOTTOM_HALVE);
	if (error != SUCCESS) {
		printf("Failed to capture image in bottom location...\n");
	} else {
		printf("Successfully captured image in bottom location.\n");
	}
	vTaskDelay(IMAGE_CAPTURE_DELAY_MS);

	// Flag CubeSense as "not used"
	cubeSenseIsInUse = 0;

	// Reset flag so no new capture is made
	imageReadyForNewCapture = 0;
	return SUCCESS;
}


/*
 * Start the download of the image into RAM.
 *
 * @param sram defines which SRAM to use on CubeSense, 0 = SRAM1, 1 = SRAM2
 * @param size defines the image size used to allocate memory, 0 = 1024x1024, 1 = 512x512, 2 = 256x256, 3 = 128x128, 4 = 64x64
 * @return error, 0 on success, otherwise failure
 */
int requestImageDownload(uint8_t sram, uint8_t size) {
	// Store the download parameters to be used by the image download task
	downloadParameters.sram = sram;
	downloadParameters.size = size;

	// Create a task to download an image from CubeSense
	int error = xTaskCreate(ImageDownloadTask,
							(const signed char*)"Image Download Task",
							DOWNLOAD_TASK_STACK_SIZE,
							NULL,
							imageDownloadTaskPriority,
							&imageDownloadTaskHandle);
	if (error != pdPASS) {
		debugPrint("requestImageDownload(): failed to create ImageDownloadTask.\n");
		return E_GENERIC;
	}

	return SUCCESS;
}


/*
 * Set the image ready for new capture flag.
 */
void setImageReadyForNewCapture(void) {
	imageReadyForNewCapture = 1;
}


/*
 * Get the image ready for new capture flag.
 *
 * @return ready for new capture state, 0 for not ready, 1 for ready
 */
uint8_t getImageReadyForNewCaptureState(void) {
	return imageReadyForNewCapture;
}


/*
 * Get the ready for downlink state of the stored image.
 *
 * @return ready for downlink state, 0 for not ready, 1 for ready
 */
uint8_t getImageReadyForDownlinkState(void) {
	return imageReadyForDownlink;
}


/*
 * Get the total number of image frames available for downlink.
 *
 * @return number of image frames
 */
uint16_t getImageFramesCount(void) {
	// Check stored image validity
	if (!imageReadyForDownlink)
		return 0;

	return image->framesCount;
}


/*
 * Get the CubeSense usage state. This should be used to prevent
 * different CubeSense functions being called at the same time.
 *
 * @return CubeSense usage, 1 if CubeSense is in use, 0 if it's unused
 */
uint8_t getCubeSenseUsageState(void) {
	return cubeSenseIsInUse;
}


/*
 * Set the image transfer frame index to the specified value.
 *
 * @param index defines the value to set the image transfer frame index
 */
void setImageTransferFrameIndex(int index) {
	currentImageFrameIndex = index;
}

/**
 * Provide the next image frame. Calling the function passed the number of image frames
 * will loop back at the first image frame (circular buffer).
 *
 * @param frame pointer to a buffer that the frame will be placed into. Set by function.
 * @return The size of the image frame placed into the buffer; 0 on error.
 */
uint8_t imageTransferNextFrame(image_frame_t* frame) {
	// Check for null pointer
	if (frame == NULL)
		return 0;

	// Check stored image validity
	if (!imageReadyForDownlink)
		return 0;

	// Validate that there is a next frame
	int nextFrameIndex = currentImageFrameIndex + 1;
	if (nextFrameIndex < 0)
		return 0;

	// Increment image frame index
	currentImageFrameIndex++;

	// Circular indexing
	if (currentImageFrameIndex >= image->framesCount)
		currentImageFrameIndex = 0;

	// Get image frame size
	uint8_t size = sizeof(image->imageFrames[currentImageFrameIndex].image_bytes);

	// Copy the image frame to the argument pointer
	//printf("Copying image frame index %d containing %i bytes.\n", currentImageFrameIndex, size);
	memcpy(frame->image_bytes, image->imageFrames[currentImageFrameIndex].image_bytes, size);
	frame->frameIndex = currentImageFrameIndex;

	// Return size of the frame
	return size;
}


/**
 * Provide the current image frame.
 *
 * @param frame pointer to a buffer that the frame will be placed into. Set by function.
 * @return The size of the image frame placed into the buffer; 0 on error.
 */
uint8_t imageTransferCurrentFrame(image_frame_t* frame) {
	return imageTransferSpecificFrame(frame, currentImageFrameIndex);
}


/**
 * Provide the image frame at the specified index.
 *
 * @param frame pointer to a buffer that the frame will be placed into. Set by function.
 * @param index defines which image frame to copy in the buffer
 * @return The size of the image frame placed into the buffer; 0 on error.
 */
uint8_t imageTransferSpecificFrame(image_frame_t* frame, int index) {
	// Check for null pointer
	if (frame == NULL)
		return 0;

	// Check stored image validity
	if (!imageReadyForDownlink)
		return 0;

	// Validate image frame index
	if (index < 0 || index >= image->framesCount)
		return 0;

	// Get image frame size
	uint8_t size = sizeof(image->imageFrames[index].image_bytes);

	// Copy the image frame to the argument pointer
	printf("\nCopying image frame index %i containing %i bytes.\n", index, size);
	memcpy(frame->image_bytes, image->imageFrames[index].image_bytes, size);
	frame->frameIndex = index;

	// Return size of the frame
	return size;
}

/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

/*
 * Task to download an image from CubeSense into RAM.
 *
 * @param parameters defines the task parameters (unused)
 */
void ImageDownloadTask(void* parameters) {
	// Flag CubeSense as "in use"
	cubeSenseIsInUse = 1;

	// ignore the input parameter
	(void)parameters;

	printf("ImageDownloadTask 1: SRAM = %i | Size = %i\n", downloadParameters.sram, downloadParameters.size);

	// Reset the image downlink ready flag since allocated memory is freed
	imageReadyForDownlink = 0;
	// Free the previous image allocated memory
	free(image);
	// Reset the image frame index
	currentImageFrameIndex = -1;

	// Initialize a new block of memory for the new image
	image = initializeNewImage(downloadParameters.size);
	// Download all image frames and store them in RAM
	int error = downloadImage(downloadParameters.sram, BOTTOM_HALVE, image);
	if (error != SUCCESS) {
		printf("\nImageDownloadTask(): Failed to download all image frames...\n");
	} else {
		printf("\nImageDownloadTask(): Successfully downloaded image!\n");

		// Flag the stored image as valid
		imageReadyForDownlink = 1;
	}

	// Flag CubeSense as "not used"
	cubeSenseIsInUse = 0;

	// Let this task delete itself
	vTaskDelete(imageDownloadTaskHandle);
}
