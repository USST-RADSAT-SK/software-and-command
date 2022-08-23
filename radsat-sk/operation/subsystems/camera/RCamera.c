/**
 * @file RCamera.c
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153), Addi Amaya (caa746) and Atharva Kulkarni (iya789)
 */

#include <RCameraCommon.h>
#include <RCamera.h>
#include <RUart.h>
#include <hal/errors.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <RImage.h>
#include <RADCS.h>
#include <RCommon.h>
#include <math.h>
#include <freertos/task.h>

/***************************************************************************************************
                               PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

/* Interval in milliseconds before retrying a frame info request (40ms seems to be the lower limit) */
#define IMAGE_FRAME_INTERVAL_MS			40

/* Maximum number of requests before timing out (set to 2 seconds) */
#define IMAGE_FRAME_MAX_RETRY			(2000 / IMAGE_FRAME_INTERVAL_MS)

/* Telecommand ID numbers and Related Parameters */
#define TELECOMMAND_40               	((uint8_t) 0x28)
#define TELECOMMAND_41               	((uint8_t) 0x29)
#define TELECOMMAND_42               	((uint8_t) 0x2A)
#define TELECOMMAND_43                  ((uint8_t) 0x2B)
#define TELECOMMAND_44                  ((uint8_t) 0x2C)
#define TELECOMMAND_45                  ((uint8_t) 0x2D)

#define TELECOMMAND_40_AND_41_LEN       ((uint8_t) 2)
#define TELECOMMAND_42_AND_44_LEN       ((uint8_t) 2)
#define TELECOMMAND_43_AND_45_LEN       ((uint8_t) 6)

/* Telemetry ID numbers and Related Parameters */
#define TELEMETRY_0                  	((uint8_t) 0x80)
#define TELEMETRY_2						((uint8_t) 0x82)
#define TELEMETRY_26                	((uint8_t) 0x9A)
#define TELEMETRY_40                    ((uint8_t) 0xA8)
#define TELEMETRY_64                 	((uint8_t) 0xC0)

#define TELEMETRY_0_LEN					((uint8_t) 12)
#define TELEMETRY_2_LEN					((uint8_t) 12)
#define TELEMETRY_26_LEN				((uint8_t) 14)
#define TELEMETRY_40_LEN				((uint8_t) 18)
#define TELEMETRY_64_LEN				((uint8_t) 132)

/* Struct for telmetry status, ID 0*/
typedef struct _tlm_status_t {
	uint8_t  nodeType;
	uint8_t  interfaceVersion;
	uint8_t  firmwareVersionMajor;
	uint8_t  firmwareVersionMinor;
	uint16_t runtimeSeconds;
	uint16_t runtimeMSeconds;
} tlm_status_t;

/* Struct for telmetry Serial number, ID 2 */
typedef struct _tlm_communication_status_t {
	uint16_t tcCounter;
	uint16_t tlmCounter;
	uint8_t  tcBufferOverrunFlag;
	uint8_t  i2ctlmReadErrorFlag;
	uint8_t	 uarttlmProtocolErrorFlag;
	uint8_t  uartIncompleteMsgFlag;
} tlm_communication_status_t;

/* Struct for telemetry power, ID 26 */
typedef struct _tlm_power_t {
	uint16_t threeVcurrent;
	uint16_t sramOneCurrent;
	uint16_t sramTwoCurrent;
	uint16_t fiveVcurrent;
	uint8_t sramOneOverCurrent;
	uint8_t sramTwoOverCurrent;
} tlm_power_t;

/* Struct for telemetry configuration, ID 40 */
typedef struct _tlm_config_t {
	uint8_t cameraOneDetectionThreshold;
	uint8_t cameraTwoDetectionThreshold;
	uint8_t cameraOneAutoAdjustMode;
	uint16_t cameraOneExposure;
	uint8_t cameraOneAGC;
	uint8_t cameraOneBlueGain;
	uint8_t cameraOneRedGain;
	uint8_t cameraTwoAutoAdjustMode;
	uint16_t cameraTwoExposure;
	uint8_t cameraTwoAGC;
	uint8_t cameraTwoBlueGain;
	uint8_t cameraTwoRedGain;
} tlm_config_t;

/* Struct for telemetry full image, ID 66-69 */
typedef struct _tlm_full_image_t {
	uint8_t imageBytes[MAXIMUM_BYTES];
} tlm_full_image_t;

/* struct for telemetry read sensor masks ID 72-73 */
typedef struct _tlm_read_sensor_mask_t {
	uint16_t MinXAreaOne;
	uint16_t MaxXAreaOne;
	uint16_t MinYAreaOne;
	uint16_t MaxYAreaOne;
	uint16_t MinXAreaTwo;
	uint16_t MaxXAreaTwo;
	uint16_t MinYAreaTwo;
	uint16_t MaxYAreaTwo;
	uint16_t MinXAreaThree;
	uint16_t MaxXAreaThree;
	uint16_t MinYAreaThree;
	uint16_t MaxYAreaThree;
	uint16_t MinXAreaFourth;
	uint16_t MaxXAreaFourth;
	uint16_t MinYAreaFourth;
	uint16_t MaxYAreaFourth;
	uint16_t MinXAreaFifth;
	uint16_t MaxXAreaFifth;
	uint16_t MinYAreaFifth;
	uint16_t MaxYAreaFifth;
} tlm_read_sensor_mask_t;

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/
static void printDetectionData(tlm_detection_result_and_trigger_adcs_t *data);
static int tlmStatus(tlm_status_t *telemetry_reply);
static int tlmPower(tlm_power_t *telemetry_reply);
static int tlmConfig(tlm_config_t *telemetry_reply);
static int tlmImageFrame(tlm_image_frame_t *telemetry_reply);
static int tcCameraDetectionThreshold(uint8_t camera, uint8_t detectionThreshold);
static int tcCameraAutoAdjust(uint8_t camera, uint8_t enabler);
static int tcCameraSettings(uint8_t camera, uint16_t exposureTime, uint8_t AGC, uint8_t blue_gain, uint8_t red_gain);
static uint16_t getNumberOfFramesFromSize(uint8_t size);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/*
 * Memory allocation and initialization of a new image of a given size
 *
 * @note it is important to free the allocated pointer memory after usage
 * @param size defines the image size used to allocate memory, 0 = 1024x1024, 1 = 512x512, 2 = 256x256, 3 = 128x128, 4 = 64x64
 * @return pointer to a new image in memory
 */
full_image_t * initializeNewImage(uint8_t size) {
	uint16_t numberOfFrames = getNumberOfFramesFromSize(size);
	full_image_t *image = calloc(1, sizeof(*image) + sizeof(tlm_image_frame_t) * numberOfFrames);
	printf("Image allocated memory size = %i bytes\n", sizeof(*image) + sizeof(tlm_image_frame_t) * numberOfFrames);
	if (image != NULL) {
		image->imageSize = size;
		image->framesCount = numberOfFrames;
	}
	return image;
}

/*
 * Used to capture an image with CubeSense Camera
 *
 * @return error, 0 on successful, otherwise failure
 * */
int captureImage(void) {
	int error;
	tlm_telecommand_ack_t telecommand_ack = {0};

	// Send Telecommand to Camera to Take a Photo
	error = tcImageCapture(NADIR_SENSOR, SRAM2, BOTTOM_HALVE);

	if (error != SUCCESS)
		return error;

	// Request telecommand acknowledgment with TLM 3
	error = tlmTelecommandAcknowledge(&telecommand_ack);

	if (error != SUCCESS)
		return error;

	return telecommand_ack.tc_error_flag;
}


/*
 * Used to Download an image from CubeSense Camera
 *
 * @param sram defines which SRAM to use on Cubesense
 * @param location defines which SRAM slot to use within selected SRAM, 0 = top, 1 = bottom
 * @param image defines a pointer to where the entire photo will reside with an image ID
 *
 * @return error, 0 on success, otherwise failure
 * */
int downloadImage(uint8_t sram, uint8_t location, full_image_t *image) {
	int imageFrameNum = 1;
	int error;
	uint8_t counter = 0;
	uint8_t isFirstRequest = 1;
	tlm_image_frame_info_t imageFrameInfo = {0};
	tlm_image_frame_t imageFrame = {0};

	// Verify if image has been initialized
	if (image == NULL) {
		return E_GENERIC;
	}

	printf("\n--- Initializing Image Download (TC 64) of SRAM=%i and location=%i---\n\n", sram, location);
	// Send telecommand 64 to initialize a download
	error = tcInitImageDownload(sram, location, image->imageSize);
	if (error != SUCCESS) {
		return error;
	}

	// Loop for the amount of frames that are being downloaded
	for (uint16_t i = 0; i < image->framesCount; i++) {
		printf("\nFRAME NUMBER = %i  |  Attempts:", i);
		// Request image frame status until image frame is loaded in the camera buffer,
		// the counter is used to ensure we don't deadlock
		counter = 0;
		while((imageFrameNum != i) && (counter < IMAGE_FRAME_MAX_RETRY)) {
			if (isFirstRequest == 1) {
				isFirstRequest = 0;
			} else {
				vTaskDelay(IMAGE_FRAME_INTERVAL_MS); // Delay in ms between each frame info request
			}
			printf(" %i ", counter);
			error = tlmImageFrameInfo(&imageFrameInfo);
			if(error != SUCCESS){
				return error;
			}

			imageFrameNum = imageFrameInfo.imageFrameNumber;
			counter++;
		}

		// Collect Image Frame with TLM 64
		error = tlmImageFrame(&imageFrame);
		if(error != SUCCESS) {
			return error;
		}

		// Store Image Frame inside master struct
		image->imageFrames[i] = imageFrame;

		if (i+1 < image->framesCount) {
			// Advance Image Download to Continue to the next Frame
			error = tcAdvanceImageDownload(i+1);
			if(error != SUCCESS) {
				return error;
			}
		}
	}

	// Give the image an ID
	image->image_ID = sram + (location << 1);

	return SUCCESS;
}

// TODO: REMOVE. Test purposes only.
char capture_results[6][25] = {
		"Startup",
		"Capture Pending",
		"Success - Own SRAM",
		"Success - Other SRAM",
		"Camera timeout",
		"SRAM overcurrent"
};
char detection_results[8][25] = {
		"Startup",
		"Not scheduled",
		"Detection Pending",
		"Error - Too many edges",
		"Error - Not enough edges",
		"Error - Bad fit",
		"Error - Sun not found",
		"Success"
};

void printDetectionData(tlm_detection_result_and_trigger_adcs_t *data) {
	printf("\n--- Detection Data ---\n");
	printf("Alpha Angle      = %d\n", data->alpha);
	printf("Beta Angle       = %d\n", data->beta);
	printf("Capture Result   = %d (%s)\n", data->captureResult, capture_results[data->captureResult]);
	printf("Detection Result = %d (%s)\n", data->detectionResult, detection_results[data->detectionResult]);
	printf("----------------------\n");
}

/*
 * Used to created a 3D vector from a detection of both sensors
 *
 * @note The RADSAT-SK ADCS will only have access to SRAM1
 * @param data a struct that will contain the components of 2 3D vectors
 * @return 0 on success, otherwise failure
 */
int detectionAndInterpret(detection_results_t *data) {
	int error = 0;
	uint16_t alphaSunSensor;
	uint16_t betaSunSensor;
	uint16_t alphaNadirSensor;
	uint16_t betaNadirSensor;
	tlm_detection_result_and_trigger_adcs_t sun_sensor_data = {0};
	tlm_detection_result_and_trigger_adcs_t nadir_sensor_data = {0};
	interpret_detection_result_t sun_sensor_coords = {0};
	interpret_detection_result_t nadir_sensor_coords = {0};

	printf("\n--------- SUN ---------");

	// Request results of detection with TLM 22
	error = tlmSensorResultAndDetection(&sun_sensor_data, sensor1_sram1);
	printDetectionData(&sun_sensor_data);

	if (error != 0)
		return error;

	// If it was still a failure, set alpha and beta to zero
	if (sun_sensor_data.detectionResult != 7) {
		alphaSunSensor = 0;
		betaSunSensor = 0;
	}
	else {
		alphaSunSensor = sun_sensor_data.alpha;
		betaSunSensor = sun_sensor_data.beta;
	}

	printf("\n-------- NADIR --------");

	// Request results of detection with TLM 23
	error = tlmSensorResultAndDetection(&nadir_sensor_data, sensor2_sram2);
	printDetectionData(&nadir_sensor_data);

	if (error != 0)
		return error;

	// If it was still a failure, set alpha and beta to zero
	if (nadir_sensor_data.detectionResult != 7) {
		alphaNadirSensor = 0;
		betaNadirSensor = 0;
	}
	else {
		alphaNadirSensor = nadir_sensor_data.alpha;
		betaNadirSensor = nadir_sensor_data.beta;
	}

	sun_sensor_coords = detectionResult(alphaSunSensor, betaSunSensor);
	nadir_sensor_coords = detectionResult(alphaNadirSensor, betaNadirSensor);

	// Fill struct with data
	data->sunSensorX = sun_sensor_coords.X_AXIS;
	data->sunSensorY = sun_sensor_coords.Y_AXIS;
	data->sunSensorZ = sun_sensor_coords.Z_AXIS;
	data->nadirSensorX = nadir_sensor_coords.X_AXIS;
	data->nadirSensorY = nadir_sensor_coords.Y_AXIS;
	data->nadirSensorZ = nadir_sensor_coords.Z_AXIS;

	return SUCCESS;
}

/*
 * Used to collect the telemetry on the CubeSense Camera
 *
 * @param cameraTelemetry a struct that will used to house all important telemetry on board
 * @return 0 on success, otherwise failure
 */
int cameraTelemetry(CameraTelemetry *cameraTelemetry) {
	int error;
	tlm_status_t tlmStatusStruct = {0};
	tlm_power_t tlmPowerStruct = {0};
	tlm_config_t tlmConfigStruct = {0};

	// Grab All telemetry and check if successful while doing so
	error = tlmStatus(&tlmStatusStruct);

	if (error != SUCCESS)
		return error;

	error = tlmPower(&tlmPowerStruct);

	if (error != SUCCESS)
		return error;

	error = tlmConfig(&tlmConfigStruct);

	if (error != SUCCESS)
		return error;

	// Assign telemetry to master telemetry struct
	cameraTelemetry->upTime = tlmStatusStruct.runtimeSeconds;
	cameraTelemetry->powerTelemetry.current_3V3 = tlmPowerStruct.threeVcurrent;
	cameraTelemetry->powerTelemetry.current_5V = tlmPowerStruct.fiveVcurrent;
	cameraTelemetry->powerTelemetry.current_SRAM_1 = tlmPowerStruct.sramOneCurrent;
	cameraTelemetry->powerTelemetry.current_SRAM_2 = tlmPowerStruct.sramTwoCurrent;
	cameraTelemetry->powerTelemetry.overcurrent_SRAM_1 = tlmPowerStruct.sramOneOverCurrent;
	cameraTelemetry->powerTelemetry.overcurrent_SRAM_2 = tlmPowerStruct.sramTwoOverCurrent;
	cameraTelemetry->cameraOneTelemetry.autoAdjustMode = tlmConfigStruct.cameraOneAutoAdjustMode;
	cameraTelemetry->cameraOneTelemetry.autoGainControl = tlmConfigStruct.cameraOneAGC;
	cameraTelemetry->cameraOneTelemetry.blueGain = tlmConfigStruct.cameraOneBlueGain;
	cameraTelemetry->cameraOneTelemetry.detectionThreshold = tlmConfigStruct.cameraOneDetectionThreshold;
	cameraTelemetry->cameraOneTelemetry.exposure = tlmConfigStruct.cameraOneExposure;
	cameraTelemetry->cameraOneTelemetry.redGain = tlmConfigStruct.cameraOneRedGain;
	cameraTelemetry->cameraTwoTelemetry.autoAdjustMode = tlmConfigStruct.cameraTwoAutoAdjustMode;
	cameraTelemetry->cameraTwoTelemetry.autoGainControl = tlmConfigStruct.cameraTwoAGC;
	cameraTelemetry->cameraTwoTelemetry.blueGain = tlmConfigStruct.cameraTwoBlueGain;
	cameraTelemetry->cameraTwoTelemetry.detectionThreshold = tlmConfigStruct.cameraTwoDetectionThreshold;
	cameraTelemetry->cameraTwoTelemetry.exposure = tlmConfigStruct.cameraTwoExposure;
	cameraTelemetry->cameraTwoTelemetry.redGain = tlmConfigStruct.cameraTwoRedGain;

	return SUCCESS;
}

/*
 * In the case the ground station wants to adjust camera settings
 *
 * @param cameraTelemetry a struct that will contain the values that want to be adjusted
 * */
int cameraConfig(CameraTelemetry *cameraTelemetry) {
	int error;

	// Update Auto adjust for camera one
	error = tcCameraAutoAdjust(SUN_SENSOR, cameraTelemetry->cameraOneTelemetry.autoAdjustMode);
	if(error != SUCCESS) {
		return error;
	}

	// Update Auto adjust for camera two
	error = tcCameraAutoAdjust(NADIR_SENSOR, cameraTelemetry->cameraTwoTelemetry.autoAdjustMode);
	if (error != SUCCESS) {
		return error;
	}

	// Update detection Threshold for camera one
	error = tcCameraDetectionThreshold(SUN_SENSOR, cameraTelemetry->cameraOneTelemetry.detectionThreshold);
	if (error != SUCCESS) {
		return error;
	}

	// Update detection Threshold for camera two
	error = tcCameraDetectionThreshold(NADIR_SENSOR, cameraTelemetry->cameraTwoTelemetry.detectionThreshold);
	if (error != SUCCESS) {
		return error;
	}

	// Update camera one settings
	error = tcCameraSettings(SUN_SENSOR, cameraTelemetry->cameraOneTelemetry.exposure,
				cameraTelemetry->cameraOneTelemetry.autoGainControl,
				cameraTelemetry->cameraOneTelemetry.blueGain,
				cameraTelemetry->cameraOneTelemetry.redGain);
	if(error != SUCCESS) {
		return error;
	}

	// Update camera Two settings
	error = tcCameraSettings(NADIR_SENSOR, cameraTelemetry->cameraTwoTelemetry.exposure,
				cameraTelemetry->cameraTwoTelemetry.autoGainControl,
				cameraTelemetry->cameraTwoTelemetry.blueGain,
				cameraTelemetry->cameraTwoTelemetry.redGain);
	if(error != SUCCESS) {
		return error;
	}

	return SUCCESS;
}

/*
 * Filtering out unwanted images. Use the images within range of 40 to 240 on the grayscale range
 *
 * @post return 0 if image is in desired range 1 if it is not
 * @param image where the entire photo will reside with an image ID
 * @return 0 on success, otherwise failure
 * */
int SaturationFilter(full_image_t *image) {
	uint16_t sumOfAverages = 0;
	uint16_t allFrameAverage = 0;

	for (int i = 0; i < image->framesCount; i++) {
		int sum = 0;
		//average of one frame's bytes
		for	(int j = 0; j < FRAME_BYTES; j++) {
			sum += image->imageFrames[i].image_bytes[j];
		}
		sumOfAverages += sum/FRAME_BYTES;
	}

	//average of all the average of all frame bytes
	allFrameAverage = sumOfAverages/image->framesCount;

	// checking if the overall average is in reasonable range
	if (allFrameAverage < 40 || allFrameAverage > 240) {
		return 1;
	}

	return SUCCESS;
}

/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/
/*
 * Used to retrieve the status of the Camera (TLM ID 0)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmStatus(tlm_status_t *telemetry_reply) {
	uint8_t *telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// ensure the input pointers are not NULL
	if (telemetry_reply == 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[MESSAGE_ID_OFFSET] = TELEMETRY_0;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_8);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = receiveAndUnescapeTelemetry(telemetryBuffer, TELEMETRY_0_LEN);

	if (error != 0) {
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply, data from uart read starts at index two
	telemetry_reply->nodeType = telemetryBuffer[TELEMETRY_OFFSET_0];
	telemetry_reply->interfaceVersion = telemetryBuffer[TELEMETRY_OFFSET_1];
	telemetry_reply->firmwareVersionMajor = telemetryBuffer[TELEMETRY_OFFSET_2];
	telemetry_reply->firmwareVersionMinor = telemetryBuffer[TELEMETRY_OFFSET_3];
	memcpy(&telemetry_reply->runtimeSeconds, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->runtimeSeconds));
	memcpy(&telemetry_reply->runtimeMSeconds, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->runtimeMSeconds));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to retrieve the power status of the Camera (TLM ID 26)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmPower(tlm_power_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// ensure the input pointers are not NULL
	if (telemetry_reply == 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[MESSAGE_ID_OFFSET] = TELEMETRY_26;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_10);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = receiveAndUnescapeTelemetry(telemetryBuffer, TELEMETRY_26_LEN);

	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->threeVcurrent, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->threeVcurrent));
	memcpy(&telemetry_reply->sramOneCurrent, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->sramOneCurrent));
	memcpy(&telemetry_reply->sramTwoCurrent, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->sramTwoCurrent));
	memcpy(&telemetry_reply->fiveVcurrent, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->fiveVcurrent));
	telemetry_reply->sramOneOverCurrent = telemetryBuffer[TELEMETRY_OFFSET_8];
	telemetry_reply->sramTwoOverCurrent = telemetryBuffer[TELEMETRY_OFFSET_9];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to retrieve the configuration status of the Camera (TLM ID 40)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmConfig(tlm_config_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// ensure the input pointers are not NULL
	if (telemetry_reply == 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[MESSAGE_ID_OFFSET] = TELEMETRY_40;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_14);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = receiveAndUnescapeTelemetry(telemetryBuffer, TELEMETRY_40_LEN);

	// Error Check on uartRecieve, if error, free allocated buffer
	if (error != 0) {
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	telemetry_reply->cameraOneDetectionThreshold = telemetryBuffer[TELEMETRY_OFFSET_0];
	telemetry_reply->cameraTwoDetectionThreshold = telemetryBuffer[TELEMETRY_OFFSET_1];
	telemetry_reply->cameraOneAutoAdjustMode = telemetryBuffer[TELEMETRY_OFFSET_2];
	memcpy(&telemetry_reply->cameraOneExposure, &telemetryBuffer[TELEMETRY_OFFSET_3], sizeof(telemetry_reply->cameraOneExposure));
	telemetry_reply->cameraOneAGC = telemetryBuffer[TELEMETRY_OFFSET_5];
	telemetry_reply->cameraOneBlueGain = telemetryBuffer[TELEMETRY_OFFSET_6];
	telemetry_reply->cameraOneRedGain = telemetryBuffer[TELEMETRY_OFFSET_7];
	telemetry_reply->cameraTwoAutoAdjustMode = telemetryBuffer[TELEMETRY_OFFSET_8];
	memcpy(&telemetry_reply->cameraTwoExposure, &telemetryBuffer[TELEMETRY_OFFSET_9], sizeof(telemetry_reply->cameraTwoExposure));
	telemetry_reply->cameraTwoAGC = telemetryBuffer[TELEMETRY_OFFSET_11];
	telemetry_reply->cameraTwoBlueGain = telemetryBuffer[TELEMETRY_OFFSET_12];
	telemetry_reply->cameraTwoRedGain = telemetryBuffer[TELEMETRY_OFFSET_13];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to receive telemetry for image frame (TLM ID 64)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tlmImageFrame(tlm_image_frame_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// ensure the input pointers are not NULL
	if (telemetry_reply == 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[MESSAGE_ID_OFFSET] = TELEMETRY_64;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_128);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = receiveAndUnescapeTelemetry(telemetryBuffer, TELEMETRY_64_LEN);

	if (error != 0) {
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->image_bytes, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->image_bytes));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to adjust the detection threshold for the given camera
 *
 * @param camera defines the camera to which the threshold will be applied
 * @param detectionThreshold the value for threshold
 * @return error on telecommand attempt, 0 on success, otherwise failure
 */
static int tcCameraDetectionThreshold(uint8_t camera, uint8_t detectionThreshold) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_40_AND_41_LEN);
	sizeOfBuffer = TELECOMMAND_40_AND_41_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	if (camera == SUN_SENSOR) {
		telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_40;
	} else if (camera == NADIR_SENSOR) {
		telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_41;
	} else {
		return E_GENERIC;
	}

	// Fill buffer with detection threshold
	telecommandBuffer[TELECOMMAND_OFFSET_0] = detectionThreshold;

    // Send Telecommand
	error = escapeAndTransmitTelecommand(telecommandBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		free(telecommandResponse);
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_RESPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to adjust the enable/disable the auto-adjust for the given camera
 *
 * @param camera defines the camera to which the auto-adjust will be applied
 * @param enabler Determines to enable or disable the auto adjust
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraAutoAdjust(uint8_t camera, uint8_t enabler) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_42_AND_44_LEN);
	sizeOfBuffer = TELECOMMAND_42_AND_44_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	if (camera == SUN_SENSOR) {
		telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_42;
	} else if (camera == NADIR_SENSOR) {
		telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_44;
	} else {
		return E_GENERIC;
	}

	// Fill buffer with detection threshold
	telecommandBuffer[TELECOMMAND_OFFSET_0] = enabler;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telecommandBuffer, sizeOfBuffer); // No escaping needed

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		free(telecommandResponse);
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_RESPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to Adjust the camera 1 settings and change the exposure, gain, blue, red control
 *
 * @param camera defines the camera to which the settings will be applied
 * @param exposureTime changes the exposure value register
 * @param AGC changes the gain control register
 * @param blue_gain changes the blue gain control register
 * @param red_gain changes the red gain control register
 */
static int tcCameraSettings(uint8_t camera, uint16_t exposureTime, uint8_t AGC, uint8_t blue_gain, uint8_t red_gain) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_43_AND_45_LEN);
	sizeOfBuffer = TELECOMMAND_43_AND_45_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	if (camera == SUN_SENSOR) {
		telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_43;
	} else if (camera == NADIR_SENSOR) {
		telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_45;
	} else {
		return E_GENERIC;
	}

	// Fill buffer with exposureTime
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_0], &exposureTime, sizeof(exposureTime));

	// Fill buffer with AGC
	telecommandBuffer[TELECOMMAND_OFFSET_2] = AGC;

	// Fill buffer with blue gain
	telecommandBuffer[TELECOMMAND_OFFSET_3] = blue_gain;

	// Fill buffer with red gain
	telecommandBuffer[TELECOMMAND_OFFSET_4] = red_gain;

    // Send Telecommand
	error = escapeAndTransmitTelecommand(telecommandBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		free(telecommandResponse);
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_RESPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Get the number of frames required for a given CubeSense image size
 *
 * @param size defines the resolution of the image to download, 0 = 1024x1024, 1 = 512x512, 2 = 256x256, 3 = 128x128, 4 = 64x64
 * @return number of frames corresponding to the desired size
 */
static uint16_t getNumberOfFramesFromSize(uint8_t size) {
	switch(size) {
		case 0: return 8192; // 1024x1024
		case 1: return 2048; // 512x512
		case 2: return 512;  // 256x256
		case 3: return 128;  // 128x128
		case 4: return 32; 	 // 64x64
		default: return 32;
	}
}
