/**
 * @file RCamera.c
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153) & Addi Amaya (caa746) & Atharva Kulkarni (iya789)
 */

#include <RCamera.h>
#include <RUart.h>
#include <hal/errors.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <RImage.h>
#include <RADCS.h>
#include <RCommon.h>
#include <hal/Timing/RTT.h>

/***************************************************************************************************
                               PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

/* Telecommand ID numbers and Related Parameters */
#define TELECOMMAND_40               	((uint8_t) 0x28)
#define TELECOMMAND_41               	((uint8_t) 0x29)
#define TELECOMMAND_42               	((uint8_t) 0x2A)
#define TELECOMMAND_43                  ((uint8_t) 0x2B)
#define TELECOMMAND_44                  ((uint8_t) 0x2C)
#define TELECOMMAND_45                  ((uint8_t) 0x2D)
#define TELECOMMAND_50                  ((uint8_t) 0x32)
#define TELECOMMAND_51                  ((uint8_t) 0x33)
#define TELECOMMAND_52               	((uint8_t) 0x34)
#define TELECOMMAND_53               	((uint8_t) 0x35)
#define TELECOMMAND_54                  ((uint8_t) 0x36)
#define TELECOMMAND_55                  ((uint8_t) 0x37)

#define TELECOMMAND_40_LEN              ((uint8_t) 2)
#define TELECOMMAND_41_LEN              ((uint8_t) 2)
#define TELECOMMAND_42_LEN              ((uint8_t) 2)
#define TELECOMMAND_43_LEN              ((uint8_t) 6)
#define TELECOMMAND_44_LEN              ((uint8_t) 2)
#define TELECOMMAND_45_LEN              ((uint8_t) 6)
#define TELECOMMAND_50_LEN              ((uint8_t) 5)
#define TELECOMMAND_51_LEN              ((uint8_t) 5)
#define TELECOMMAND_52_LEN              ((uint8_t) 10)
#define TELECOMMAND_53_LEN              ((uint8_t) 10)
#define TELECOMMAND_54_LEN              ((uint8_t) 16)
#define TELECOMMAND_55_LEN              ((uint8_t) 16)
#define TELECOMMAND_RESPONSE_LEN		((uint16_t)1)

#define TELECOMMAND_OFFSET_ID           ((uint8_t) 2)
#define TELECOMMAND_OFFSET_0            ((uint8_t) 3)
#define TELECOMMAND_OFFSET_1            ((uint8_t) 4)
#define TELECOMMAND_OFFSET_2            ((uint8_t) 5)
#define TELECOMMAND_OFFSET_3            ((uint8_t) 6)
#define TELECOMMAND_OFFSET_4            ((uint8_t) 7)
#define TELECOMMAND_OFFSET_5            ((uint8_t) 8)
#define TELECOMMAND_OFFSET_6            ((uint8_t) 9)
#define TELECOMMAND_OFFSET_7            ((uint8_t) 10)
#define TELECOMMAND_OFFSET_8            ((uint8_t) 11)
#define TELECOMMAND_OFFSET_9            ((uint8_t) 12)
#define TELECOMMAND_OFFSET_11           ((uint8_t) 14)
#define TELECOMMAND_OFFSET_12           ((uint8_t) 15)
#define TELECOMMAND_OFFSET_14           ((uint8_t) 17)
#define TELECOMMAND_REPONSE_OFFSET		((uint8_t) 2)

/* Telemetry ID numbers and Related Parameters */
#define TELEMETRY_0                  	((uint8_t) 0x80)
#define TELEMETRY_26                	((uint8_t) 0x9A)
#define TELEMETRY_40                    ((uint8_t) 0xA8)
#define TELEMETRY_64                 	((uint8_t) 0xC0)

#define TELEMETRY_0_LEN					((uint8_t) 12)
#define TELEMETRY_26_LEN				((uint8_t) 14)
#define TELEMETRY_40_LEN				((uint8_t) 18)
#define TELEMETRY_64_LEN				((uint8_t) 132)
#define TELEMETRY_REQUEST_LEN			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_1			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_8			((uint8_t) 8)
#define TELEMETRY_REPLY_SIZE_10			((uint8_t) 10)
#define TELEMETRY_REPLY_SIZE_14			((uint8_t) 14)

#define TELEMETRY_OFFSET_0              ((uint8_t) 2)
#define TELEMETRY_OFFSET_1              ((uint8_t) 3)
#define TELEMETRY_OFFSET_2              ((uint8_t) 4)
#define TELEMETRY_OFFSET_3              ((uint8_t) 5)
#define TELEMETRY_OFFSET_4              ((uint8_t) 6)
#define TELEMETRY_OFFSET_5              ((uint8_t) 7)
#define TELEMETRY_OFFSET_6              ((uint8_t) 8)
#define TELEMETRY_OFFSET_7              ((uint8_t) 9)
#define TELEMETRY_OFFSET_8              ((uint8_t) 10)
#define TELEMETRY_OFFSET_9              ((uint8_t) 11)
#define TELEMETRY_OFFSET_10             ((uint8_t) 12)
#define TELEMETRY_OFFSET_11             ((uint8_t) 13)
#define TELEMETRY_OFFSET_12             ((uint8_t) 14)
#define TELEMETRY_OFFSET_13             ((uint8_t) 15)
#define TELEMETRY_ID_OFFSET		        ((uint8_t) 2)

/* General definitions */
#define BASE_MESSAGE_LEN				((uint8_t) 4)
#define START_IDENTIFIER1               ((uint16_t) 0x1F)
#define START_IDENTIFIER2               ((uint16_t) 0x7F)
#define FILLER							((uint16_t) 0x00)
#define END_IDENTIFIER1                 ((uint16_t) 0x1F)
#define END_IDENTIFIER2                 ((uint16_t) 0xFF)

#define SUN_SENSOR	         	        ((uint8_t) 0)
#define IMAGE_SENSOR	                ((uint8_t) 1)
#define SRAM1                           ((uint8_t) 0)
#define SRAM2                           ((uint8_t) 1)
#define TOP_HALVE                       ((uint8_t) 0)
#define BOTTOM_HALVE                    ((uint8_t) 1)

#define LOCKED							((uint8_t) 1)
#define UNLOCKED						((uint8_t) 0)

#define TC_NO_ERROR						((uint8_t) 0)

//number of bytes in one frame of an image
#define FRAME_BYTES						128

//array size to convert hex to decimal
#define ARRAY_SIZE 						20

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
	uint8_t  tcBufferOverunFlag;
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
typedef struct _tlm_config_t{
	uint8_t cameraOneDetectionThrshld;
	uint8_t cameraTwoDetectionThrshld;
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
	uint8_t imageBytes[1048576];
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
static uint8_t * MessageBuilder(uint8_t response_size);
static int tlmStatus(tlm_status_t *telemetry_reply);
static int tlmPower(tlm_power_t *telemetry_reply);
static int tlmConfig(tlm_config_t *telemetry_reply);
static int tlmImageFrame(tlm_image_frame_t *telemetry_reply);
static int tcCameraOneDetectionThreshold(uint8_t detectionThreshold);
static int tcCameraTwoDetectionThreshold(uint8_t detectionThreshold);
static int tcCameraOneAutoAdjust(uint8_t enabler);
static int tcCameraOneSettings(uint16_t exposureTime, uint8_t AGC, uint8_t blue_gain, uint8_t red_gain);
static int tcCameraTwoAutoAdjust(uint8_t enabler);
static int tcCameraTwoSettings(uint16_t exposureTime, uint8_t AGC, uint8_t blue_gain, uint8_t red_gain);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/*
 * Used to capture an image with CubeSense Camera
 *
 * @return error, 0 on successful, otherwise failure
 * */
 int capture(void) {

	int error;
	tlm_telecommand_ack_t *telecommand_ack = {0};    //pointer to struct
	tlm_detection_result_and_trigger_t *sensor_two_result = {0};

	// Send Telecommand to Camera to Take a Photo
	error = tcImageCaputre(SRAM1, BOTTOM_HALVE);

	if (error != SUCCESS)
		return error;

	// Request telecommand acknowledgment with TLM 3
   error = tlmTelecommandAcknowledge(telecommand_ack);

	if (error != SUCCESS)
		return error;

	// Confirm there was a Successful telecommand
	if(telecommand_ack->tc_error_flag == TC_NO_ERROR) {

		// Request for sensor 2 (Camera) results
		error = tlmSensorTwoResult(sensor_two_result);

		if (error != SUCCESS)
			return error;

		// Checks if the image was successfully captured
		if (sensor_two_result->captureResult != 2)
			return E_GENERIC;

		return SUCCESS;

	}
	return error;
}



/*
 * Used to Download an image from CubeSense Camera
 *
 * @param sram defines which SRAM to use on Cubesense
 * @param location defines which SRAM slot to use within selected SRAM, 0 = top, 1 = bottom
 * @param size defines the resolution of the image to download, 0 = 1024x1024, 1 = 512x512, 2 = 256x256, 3 = 128x128, 4 = 64x64,
 * @param image where the entire photo will reside with an image ID
 *
 * @return error, 0 on success, otherwise failure
 * */
int downloadImage(uint8_t sram, uint8_t location, uint8_t size, full_image_t *image) {
	unsigned int startTime;
	unsigned int currentTime;
	unsigned int elapsedTime;
	int imageFrameNum = 1;
	int error;
	uint16_t numOfFrames;
	tlm_image_frame_info_t *imageFrameInfo = {0};
	tlm_image_frame_t *imageFrame = {0};

	// Send telecommand 64 to initialized a download
	error = tcInitImageDownload(sram,location,size);
	if (error != SUCCESS) {
		return error;
	}

	// From size of image download determine loop variable
	switch(size) {
		case 0: numOfFrames = 8192; break; 	// 1024x1024
		case 1: numOfFrames = 2048; break;
		case 2: numOfFrames = 512; break;
		case 3: numOfFrames = 128; break;
		case 4: numOfFrames = 32; break; 	// 64x64
		default: numOfFrames = 32; break;
	}

	// Reset all parameters
	startTime = 0;
	currentTime = 0;
	elapsedTime = 0;
	imageFrameNum = 1;
	memset(image->imageFrames,0,sizeof(image->imageFrames));

	// Loop for the amount of frames that are being downloaded
	for (uint16_t i = 0; i < numOfFrames; i++) {

		// Start a timer and record start time, resets timer on function call
		RTT_start();
		startTime = RTT_GetTime();

		// Need to wait until on board camera buffer empties for frame, timer used as a back up to ensure we dont deadlock, set for 2 seconds
		while((imageFrameNum != 0) || (elapsedTime <= 2)) {

			// Request Telemetry 65 To get the status of image frame buffer
			error = tlmImageFrameInfo(imageFrameInfo);
			if(error != SUCCESS){
				return error;
			}

			imageFrameNum = imageFrameInfo->imageFrameNumber;

			currentTime = RTT_GetTime();
			elapsedTime = currentTime - startTime;

		}

		// Collect Image Frame with TLM 64
		error = tlmImageFrame(imageFrame);
		if(error != SUCCESS) {
			return error;
		}

		// Store Image Frame inside master struct
		image->imageFrames[i] = imageFrame;

		// Advance Image Download to Continue to the next Frame
		error = tcAdvanceImageDownload(i);
		if(error != SUCCESS) {
			return error;
		}
	}

	// Give the image an ID
	image->image_ID = sram + location;

	return SUCCESS;
}



/*
 * Used to created a 3D vector from a detection of both sensors
 *
 * @note The RADSAT-SK ADCS will only have access to SRAM1
 * @param data a struct that will contain the components of 2 3D vectors
 * @return 0 on success, otherwise failure
 */
int detectionAndInterpret(detection_results_t *data) {
	int error;
	uint16_t alphaSunSensor;
	uint16_t betaSunSensor;
	uint16_t alphaImageSensor;

	uint16_t betaImageSensor;
	tlm_detection_result_and_trigger_adcs_t *sun_sensor_data = {0};
	tlm_detection_result_and_trigger_adcs_t *image_sensor_data = {0};
	interpret_detection_result_t sun_sensor_coords = {0};
	interpret_detection_result_t image_sensor_coords = {0};


	// Send Telecommand 20, Image Capture and Detection for camera 1, Sun sensor
	error = tcImageCaputreAndDetection(SUN_SENSOR);

	if (error != 0)
		return error;

	// Request results of detection from TC 20 with TLM 22
	error = tlmSensorOneResultAndDetectionSRAMOne(sun_sensor_data);

	if (error != 0)
		return error;

	// If there was a failure to detect try again
	if (sun_sensor_data->detectionResult != 7) {
		error = tlmSensorOneResultAndDetectionSRAMOne(sun_sensor_data);

		if (error != 0)
			return error;
	}

	// If it was still a failure, set alpha and beta to zero
	if (sun_sensor_data->detectionResult != 7) {
		alphaSunSensor = 0;
		betaSunSensor = 0;
	}
	else {
		alphaSunSensor = sun_sensor_data->alpha;
		betaSunSensor = sun_sensor_data->beta;
	}

	// Send Telecommand 20, Image Capture and Detection for camera 2, image sensor
	error = tcImageCaputreAndDetection(IMAGE_SENSOR);

	if (error != 0)
		return error;

	// Request results of detection from TC 20 with TLM 22
	error = tlmSensorOneResultAndDetectionSRAMOne(image_sensor_data);

	if (error != 0)
		return error;

	// If there was a failure to detect try again
	if (image_sensor_data->detectionResult != 7) {
		error = tlmSensorOneResultAndDetectionSRAMOne(image_sensor_data);

		if (error != 0)
			return error;
	}

	// If it was still a failure, set alpha and beta to zero
	if (image_sensor_data->detectionResult != 7) {
		alphaSunSensor = 0;
		betaSunSensor = 0;
	}
	else {
		alphaImageSensor = image_sensor_data->alpha;
		betaImageSensor = image_sensor_data->beta;
	}

	sun_sensor_coords = detectionResult(alphaSunSensor, betaSunSensor);
	image_sensor_coords = detectionResult(alphaImageSensor, betaImageSensor);

	// Fill struct with data
	data->sunSensorX = sun_sensor_coords.X_AXIS;
	data->sunSensorY = sun_sensor_coords.Y_AXIS;
	data->sunSensorZ = sun_sensor_coords.Z_AXIS;
	data->imageSensorX = image_sensor_coords.X_AXIS;
	data->imageSensorY = image_sensor_coords.Y_AXIS;
	data->imageSensorZ = image_sensor_coords.Z_AXIS;

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
	tlm_status_t *tlmStatusStruct = {0};
	tlm_power_t *tlmPowerStruct = {0};
	tlm_config_t *tlmConfigStruct = {0};

	// Grab All telemetry and check if successful while doing so
	error = tlmStatus(tlmStatusStruct);

	if (error != SUCCESS)
		return error;

	error = tlmPower(tlmPowerStruct);

	if (error != SUCCESS)
		return error;

	error = tlmConfig(tlmConfigStruct);

	if (error != SUCCESS)
		return error;

	// Assign telemetry to master telemetry struct
	cameraTelemetry->upTime = tlmStatusStruct->runtimeSeconds;
	cameraTelemetry->powerTelemetry.current_3V3 = tlmPowerStruct->threeVcurrent;
	cameraTelemetry->powerTelemetry.current_5V = tlmPowerStruct->fiveVcurrent;
	cameraTelemetry->powerTelemetry.current_SRAM_1 = tlmPowerStruct->sramOneCurrent;
	cameraTelemetry->powerTelemetry.current_SRAM_2 = tlmPowerStruct->sramTwoCurrent;
	cameraTelemetry->powerTelemetry.overcurrent_SRAM_1 = tlmPowerStruct->sramOneOverCurrent;
	cameraTelemetry->powerTelemetry.overcurrent_SRAM_2 = tlmPowerStruct->sramTwoOverCurrent;
	cameraTelemetry->cameraOneTelemetry.autoAdjustMode = tlmConfigStruct->cameraOneAutoAdjustMode;
	cameraTelemetry->cameraOneTelemetry.autoGainControl = tlmConfigStruct->cameraOneAGC;
	cameraTelemetry->cameraOneTelemetry.blueGain = tlmConfigStruct->cameraOneBlueGain;
	cameraTelemetry->cameraOneTelemetry.detectionThreshold = tlmConfigStruct->cameraOneDetectionThrshld;
	cameraTelemetry->cameraOneTelemetry.exposure = tlmConfigStruct->cameraOneExposure;
	cameraTelemetry->cameraOneTelemetry.redGain = tlmConfigStruct->cameraOneRedGain;
	cameraTelemetry->cameraTwoTelemetry.autoAdjustMode = tlmConfigStruct->cameraTwoAutoAdjustMode;
	cameraTelemetry->cameraTwoTelemetry.autoGainControl = tlmConfigStruct->cameraTwoAGC;
	cameraTelemetry->cameraTwoTelemetry.blueGain = tlmConfigStruct->cameraTwoBlueGain;
	cameraTelemetry->cameraTwoTelemetry.detectionThreshold = tlmConfigStruct->cameraTwoDetectionThrshld;
	cameraTelemetry->cameraTwoTelemetry.exposure = tlmConfigStruct->cameraTwoExposure;
	cameraTelemetry->cameraTwoTelemetry.redGain = tlmConfigStruct->cameraTwoRedGain;

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
	error = tcCameraOneAutoAdjust(cameraTelemetry->cameraOneTelemetry.autoAdjustMode);
	if(error != SUCCESS) {
		return error;
	}

	// Update Auto adjust for camera two
	error = tcCameraTwoAutoAdjust(cameraTelemetry->cameraTwoTelemetry.autoAdjustMode);
	if (error != SUCCESS) {
		return error;
	}

	// Update detection Threshold for camera one
	error = tcCameraOneDetectionThreshold(cameraTelemetry->cameraOneTelemetry.detectionThreshold);
	if (error != SUCCESS) {
		return error;
	}

	// Update detection Threshold for camera two
	error = tcCameraTwoDetectionThreshold(cameraTelemetry->cameraTwoTelemetry.detectionThreshold);
	if (error != SUCCESS) {
		return error;
	}

	// Update camera one settings
	error = tcCameraOneSettings(cameraTelemetry->cameraOneTelemetry.exposure,
				cameraTelemetry->cameraOneTelemetry.autoGainControl,
				cameraTelemetry->cameraOneTelemetry.blueGain,
				cameraTelemetry->cameraOneTelemetry.redGain);
	if(error != SUCCESS) {
		return error;
	}

	// Update camera Two settings
	error = tcCameraTwoSettings(cameraTelemetry->cameraTwoTelemetry.exposure,
				cameraTelemetry->cameraTwoTelemetry.autoGainControl,
				cameraTelemetry->cameraTwoTelemetry.blueGain,
				cameraTelemetry->cameraTwoTelemetry.redGain);
	if(error != SUCCESS) {
		return error;
	}



	return SUCCESS;
}

/*
 * Filtering out bad images using a gray scale filter
 *
 * @param size defines the resolution of the image to download, 0 = 1024x1024, 1 = 512x512, 2 = 256x256, 3 = 128x128, 4 = 64x64,
 * @param image where the entire photo will reside with an image ID
 * @return 0 on success, otherwise faliure
 * */
int SaturationFilter(uint8_t size, full_image_t *image) {

	uint8_t numOfFrames;
	uint8_t sum;
	uint16_t avg = 0;

	switch(size) {
		case 0: numOfFrames = 8192; break; 	// 1024x1024
		case 1: numOfFrames = 2048; break;
		case 2: numOfFrames = 512; break;
		case 3: numOfFrames = 128; break;
		case 4: numOfFrames = 32; break; 	// 64x64
		default: numOfFrames = 32; break;
	}

	tlm_image_frame_info_t *imageFrameInfo = {0};
	tlm_image_frame_t *imageFrame = {0};
	uint8_t frameArray = imageFrame->image_bytes;

	//average of all the average of all frame bytes
	for (int j; j <= numOfFrames; j++) {
		uint16_t sumOfAverages = avg[j];
		int allFrameAverage = sumOfAverages/numOfFrames;

		//average of one frame's bytes
		for (int i = 0; i <= FRAME_BYTES ; i ++ ) {
			sum += HexToDec(frameArray)[i];
			avg = sum/FRAME_BYTES;
		}
		// checking if the overall average is in reasonable range
		if (allFrameAverage < 5) {
			return 1;
		}
		else if (allFrameAverage > 240) {
			return 1;
		}
		else {
			return 0;
		}
	}

	return 0;
}

/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/*
 * Used to dynamically allocated buffer sizes as each telemetry and telecommand
 * require different sizes
 *
 * @note must use Free() to free the allocated memory when finished using the buffer
 * @param response_size defines how many data bytes are required in the buffer
 * @return dynamically allocated buffer
 * */
static uint8_t * MessageBuilder(uint8_t response_size) {

	// Define the total size the buffer should be
    uint8_t total_buffer_length = response_size + BASE_MESSAGE_LEN;

    // Dynamically Allocate a Buffer for telemetry response
    uint8_t* tlmBuffer = (uint8_t*) malloc(total_buffer_length * sizeof(uint8_t));

    // Initialize all elements in the buffer with zero
    for (uint8_t i = 0; i < total_buffer_length; i++) {
    	tlmBuffer[i] = 0;
    }

    // Fill Buffer with default values
    for(uint8_t i = 0; i<total_buffer_length;i++) {
        if (i == 0) {
        	tlmBuffer[i] = START_IDENTIFIER1;
        }
        else if (i == 1) {
            tlmBuffer[i] = START_IDENTIFIER2;
        }
        else if (i == total_buffer_length-2) {
        	tlmBuffer[i] = END_IDENTIFIER1;
        }
        else if (i == total_buffer_length-1) {
        	tlmBuffer[i] = END_IDENTIFIER2;
        }
        else {
        	tlmBuffer[i] = FILLER;
        }
    }

    return tlmBuffer;

}

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
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_0;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_8);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_0_LEN);

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
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_26;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_10);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_26_LEN);

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
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_40;

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
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_40_LEN);

	// Error Check on uartRecieve, if error, free allocated buffer
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	telemetry_reply->cameraOneDetectionThrshld = telemetryBuffer[TELEMETRY_OFFSET_0];
	telemetry_reply->cameraTwoDetectionThrshld = telemetryBuffer[TELEMETRY_OFFSET_1];
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
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_64;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_1);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_64_LEN);

	if (error != 0){
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
 * Used to adjust the detection threshold for camera 1
 *
 * @param detectionThreshold the value for threshold
 * @return error on tc attempt, 0 on success, otherwise failure
 */
static int tcCameraOneDetectionThreshold(uint8_t detectionThreshold) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_40_LEN);
	sizeOfBuffer = TELECOMMAND_40_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_40;

	// Fill buffer with detection threshold
	telecommandBuffer[TELECOMMAND_OFFSET_0] = detectionThreshold;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telecommandBuffer, sizeOfBuffer);

	if (error != 0){
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to adjust the detection threshold for camera 2
 *
 * @param detectionThreshold the value for threshold
 * @return error on tc attempt, 0 on success, otherwise failure
 */
static int tcCameraTwoDetectionThreshold(uint8_t detectionThreshold) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_41_LEN);
	sizeOfBuffer = TELECOMMAND_41_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_41;

	// Fill buffer with detection threshold
	telecommandBuffer[TELECOMMAND_OFFSET_0] = detectionThreshold;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telecommandBuffer, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to adjust the enable/disable the auto-adjust for camera one
 *
 * @param enabler Determines to enable or disable the auto adjust
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraOneAutoAdjust(uint8_t enabler) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_42_LEN);
	sizeOfBuffer = TELECOMMAND_42_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_42;

	// Fill buffer with detection threshold
	telecommandBuffer[TELECOMMAND_OFFSET_0] = enabler;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telecommandBuffer, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

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
 * @param exposureTime changes the exposure value register
 * @param AGC changes the gain control register
 * @param blue_gain changes the blue gain control register
 * @param red_gain changes the red gain control register
 */
static int tcCameraOneSettings(uint16_t exposureTime, uint8_t AGC, uint8_t blue_gain, uint8_t red_gain) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_43_LEN);
	sizeOfBuffer = TELECOMMAND_43_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_43;

	// Fill buffer with exposureTime
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_0], &exposureTime, sizeof(exposureTime));

	// Fill buffer with AGC
	telecommandBuffer[TELECOMMAND_OFFSET_2] = AGC;

	// Fill buffer with blue gain
	telecommandBuffer[TELECOMMAND_OFFSET_3] = blue_gain;

	// Fill buffer with red gain
	telecommandBuffer[TELECOMMAND_OFFSET_4] = red_gain;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telecommandBuffer, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to adjust the enable/disable the auto-adjust for camera one
 *
 * @param enabler Determines to enable or disable the auto adjust
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraTwoAutoAdjust(uint8_t enabler) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_44_LEN);
	sizeOfBuffer = TELECOMMAND_44_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_44;

	// Fill buffer with detection threshold
	telecommandBuffer[TELECOMMAND_OFFSET_0] = enabler;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telecommandBuffer, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to Adjust the camera 2 settings and change the exposure, gain, blue, red control
 *
 * @param exposureTime changes the exposure value register
 * @param AGC changes the gain control register
 * @param blue_gain changes the blue gain control register
 * @param red_gain changes the red gain control register
 */
static int tcCameraTwoSettings(uint16_t exposureTime, uint8_t AGC, uint8_t blue_gain, uint8_t red_gain) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_45_LEN);
	sizeOfBuffer = TELECOMMAND_45_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_45;

	// Fill buffer with exposureTime
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_0], &exposureTime, sizeof(exposureTime));

	// Fill buffer with AGC
	telecommandBuffer[TELECOMMAND_OFFSET_2] = AGC;

	// Fill buffer with blue gain
	telecommandBuffer[TELECOMMAND_OFFSET_3] = blue_gain;

	// Fill buffer with red gain
	telecommandBuffer[TELECOMMAND_OFFSET_4] = red_gain;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telecommandBuffer, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		return E_GENERIC;
	}

	return SUCCESS;
}
/*
 * Converts Hexadecimal into decimal
 * @param hex a hex input
 * @return decimal value of hex input
 */

static HexToDec(char hex) {

    long long decimal = 0, base = 1;
    int i = 0, value, length;

    /* Get hexadecimal value from user */
    fflush(stdin);
    fgets(hex,ARRAY_SIZE,stdin);
    length = strlen(hex);

    for(i = length--; i >= 0; i--) {

        if(hex[i] >= '0' && hex[i] <= '9') {
            decimal += (hex[i] - 48) * base;
            base *= 16;
        }
        else if(hex[i] >= 'A' && hex[i] <= 'F') {
            decimal += (hex[i] - 55) * base;
            base *= 16;
        }
        else if(hex[i] >= 'a' && hex[i] <= 'f') {
            decimal += (hex[i] - 87) * base;
            base *= 16;
        }
    }
    return decimal;
}







