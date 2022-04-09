/**
 * @file RCamera.c
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153) & Addi Amaya (caa746)
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

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

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

#define TELEMETRY_0                  	((uint8_t) 0x80)
#define TELEMETRY_26                	((uint8_t) 0x9A)
#define TELEMETRY_40                    ((uint8_t) 0xA8)
#define TELEMETRY_72                    ((uint8_t) 0xC8)
#define TELEMETRY_73                    ((uint8_t) 0xC9)

#define TELEMETRY_0_LEN					((uint8_t) 12)
#define TELEMETRY_26_LEN				((uint8_t) 14)
#define TELEMETRY_40_LEN				((uint8_t) 18)
#define TELEMETRY_72_LEN			    ((uint8_t) 44)
#define TELEMETRY_73_LEN			    ((uint8_t) 44)
#define TELEMETRY_REQUEST_LEN			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_8			((uint8_t) 8)
#define TELEMETRY_REPLY_SIZE_10			((uint8_t) 10)
#define TELEMETRY_REPLY_SIZE_14			((uint8_t) 14)
#define TELEMETRY_REPLY_SIZE_40			((uint8_t) 40)

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
#define TELEMETRY_OFFSET_14             ((uint8_t) 16)
#define TELEMETRY_OFFSET_16             ((uint8_t) 18)
#define TELEMETRY_OFFSET_18             ((uint8_t) 20)
#define TELEMETRY_OFFSET_20             ((uint8_t) 22)
#define TELEMETRY_OFFSET_22             ((uint8_t) 24)
#define TELEMETRY_OFFSET_24             ((uint8_t) 26)
#define TELEMETRY_OFFSET_26             ((uint8_t) 28)
#define TELEMETRY_OFFSET_28             ((uint8_t) 30)
#define TELEMETRY_OFFSET_30             ((uint8_t) 32)
#define TELEMETRY_OFFSET_32             ((uint8_t) 34)
#define TELEMETRY_OFFSET_34             ((uint8_t) 36)
#define TELEMETRY_OFFSET_36             ((uint8_t) 38)
#define TELEMETRY_OFFSET_38             ((uint8_t) 40)
#define TELEMETRY_ID_OFFSET		        ((uint8_t) 2)

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
#define SUCCESSFULLY_CAPTURED			((uint8_t) 3)
#define EMPTY_FRAME_BUFFER				((uint8_t) 0)
#define MAX_COUNTER_AMOUNT				((uint16_t) 100)

#define SIZE_SELECTION_64               ((uint8_t) 4)
#define NextFrameNumLBS                 ((uint8_t) 1)
#define NextFrameNumMBS                 ((uint8_t) 1)
/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/
static uint8_t * MessageBuilder(uint8_t response_size);
static int tlmStatus(tlm_status_t *telemetry_reply);
static int tlmPower(tlm_power_t *telemetry_reply);
static int tlmConfig(tlm_config_t *telemetry_reply);
static int tlmReadSensorOneMask(tlm_read_sensor_mask_t *telemetry_reply);
static int tlmReadSensorTwoMask(tlm_read_sensor_mask_t *telemetry_reply);
static int tcCameraOneDetectionThreshold(uint8_t detectionThreshold);
static int tcCameraTwoDetectionThreshold(uint8_t detectionThreshold);
static int tcCameraOneAutoAdjust(uint8_t enabler);
static int tcCameraOneSettings(uint16_t exposureTime, uint8_t AGC, uint8_t blue_gain, uint8_t red_gain);
static int tcCameraTwoAutoAdjust(uint8_t enabler);
static int tcCameraTwoSettings(uint16_t exposureTime, uint8_t AGC, uint8_t blue_gain, uint8_t red_gain);
static int tcCameraOneBoresight(uint16_t X_Pixel, uint16_t Y_Pixel);
static int tcCameraTwoBoresight(uint16_t X_Pixel, uint16_t Y_Pixel);
static int tcCameraOneMask(uint8_t MaskNumber, uint16_t Xmin, uint16_t Xmax, uint16_t Ymin, uint16_t Ymax);
static int tcCameraTwoMask(uint8_t MaskNumber, uint16_t Xmin, uint16_t Xmax, uint16_t Ymin, uint16_t Ymax);
static int tcCameraOneDistortionCorrection(uint16_t mantissa1, uint8_t exponent1, uint16_t mantissa2, uint8_t exponent2,
		uint16_t mantissa3, uint8_t exponent3, uint16_t mantissa4, uint8_t exponent4, uint16_t mantissa5, uint8_t exponent5);
static int tcCameraTwoDistortionCorrection(uint16_t mantissa1, uint8_t exponent1, uint16_t mantissa2, uint8_t exponent2,
		uint16_t mantissa3, uint8_t exponent3, uint16_t mantissa4, uint8_t exponent4, uint16_t mantissa5, uint8_t exponent5);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int captureAndDownload(void){

	int error;

	uint8_t timeOutFlag;
	uint16_t internalCounter;

	uint8_t imagemean;


	tlm_telecommand_ack_t *telecommand_ack;
	tlm_detection_result_and_trigger_t *sensor_two_result;
	tlm_image_frame_info_t *image_frame_info;
	tlm_image_frame_t *image_frame;

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
	}

	// If there was a successful image capture, proceed with execution
	if (sensor_two_result->captureResult == SUCCESSFULLY_CAPTURED) {

		// Initialize image download to the Bottom half of SRAM 1
		error = tcInitImageDownload(SRAM1,BOTTOM_HALVE,SIZE_SELECTION_64);

		if (error != SUCCESS)
			return error;

		// Request telecommand acknowledgment
	   error = tlmTelecommandAcknowledge(telecommand_ack);

		if (error != SUCCESS)
			return error;

		// Confirm there was a Successful telecommand
		if(telecommand_ack->tc_error_flag == TC_NO_ERROR) {

			// Wait until there are no frames loaded into download buffer
			internalCounter = 0;
			while (timeOutFlag != 1) {

				// Request image frame info with TLM 65
				error = tlmImageFrameInfo(image_frame_info);

				// Checks to see if there are image frames in the download buffer
				 if(image_frame_info->imageFrameNumber == EMPTY_FRAME_BUFFER){
					 timeOutFlag = 1;
				 }

				 // Internal counter to ensure we dont deadlock ourselves
				 if (internalCounter >= MAX_COUNTER_AMOUNT) {
					 timeOutFlag = 1;
					 internalCounter = 0;
				 }
				 internalCounter++;

			}

			// Confirm that the download buffer is emopty
			if(image_frame_info->imageFrameNumber == EMPTY_FRAME_BUFFER){

				// Recieve the image frames
				error = tlmImageFrame(image_frame);

				if (error != 0)
					return error;

			}

			return SUCCESS;
		}


	return E_GENERIC;

	}
}


/*
 * Used to created a 3D vector from a detection of both sensors
 *
 * @note The RADSAT-SK ADCS will only have access to SRAM1
 * @param data a struct that will contain the components of 2 3D vectors
 * @return 0 on success, otherwise failure
 */
int detectionAndInterpret(detection_results_t *data){
	int error;
	uint16_t alphaSunSensor;
	uint16_t betaSunSensor;
	uint16_t alphaImageSensor;
	uint16_t betaImageSensor;
	tlm_detection_result_and_trigger_adcs_t *sun_sensor_data;
	tlm_detection_result_and_trigger_adcs_t *image_sensor_data;
	interpret_detection_result_t sun_sensor_coords;
	interpret_detection_result_t image_sensor_coords;


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
	if (sun_sensor_data->detectionResult != 7){
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
	if (image_sensor_data->detectionResult != 7){
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
	tlm_status_t *tlmStatusStruct;
	tlm_power_t *tlmPowerStruct;
	tlm_config_t *tlmConfigStruct;

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



}

int cameraConfig(CameraTelemetry *cameraTelemetry) {

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
static uint8_t * MessageBuilder(uint8_t response_size){

	// Define the total size the buffer should be
    uint8_t total_buffer_length = response_size + BASE_MESSAGE_LEN;

    // Dynamically Allocate a Buffer for telemetry response
    uint8_t* tlmBuffer = (uint8_t*) malloc(total_buffer_length * sizeof(uint8_t));

    // Initialize all elements in the buffer with zero
    for (uint8_t i = 0; i < total_buffer_length; i++){
    	tlmBuffer[i] = 0;
    }

    // Fill Buffer with default values
    for(uint8_t i = 0; i<total_buffer_length;i++){
        if (i == 0){
        	tlmBuffer[i] = START_IDENTIFIER1;
        }
        else if (i == 1){
            tlmBuffer[i] = START_IDENTIFIER2;
        }
        else if (i == total_buffer_length-2) {
        	tlmBuffer[i] = END_IDENTIFIER1;
        }
        else if (i == total_buffer_length-1) {
        	tlmBuffer[i] = END_IDENTIFIER2;
        }
        else{
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

	if (error != 0){
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
 * Used to retrieve the mask configuration of Sensor one of the Camera (TLM ID 72)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmReadSensorOneMask(tlm_read_sensor_mask_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_72;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_40);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_72_LEN);

	// Error Check on uartRecieve, if error, free allocated buffer
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->MinXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->MinXAreaOne));
	memcpy(&telemetry_reply->MaxXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->MaxXAreaOne));
	memcpy(&telemetry_reply->MinYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->MinYAreaOne));
	memcpy(&telemetry_reply->MaxYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->MaxYAreaOne));
	memcpy(&telemetry_reply->MinXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_8], sizeof(telemetry_reply->MinXAreaTwo));
	memcpy(&telemetry_reply->MaxXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_10], sizeof(telemetry_reply->MaxXAreaTwo));
	memcpy(&telemetry_reply->MinYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_12], sizeof(telemetry_reply->MinYAreaTwo));
	memcpy(&telemetry_reply->MaxYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_14], sizeof(telemetry_reply->MaxYAreaTwo));
	memcpy(&telemetry_reply->MinXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_16], sizeof(telemetry_reply->MinXAreaThree));
	memcpy(&telemetry_reply->MaxXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_18], sizeof(telemetry_reply->MaxXAreaThree));
	memcpy(&telemetry_reply->MinYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_20], sizeof(telemetry_reply->MinYAreaThree));
	memcpy(&telemetry_reply->MaxYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_22], sizeof(telemetry_reply->MaxYAreaThree));
	memcpy(&telemetry_reply->MinXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_24], sizeof(telemetry_reply->MinXAreaFourth));
	memcpy(&telemetry_reply->MaxXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_26], sizeof(telemetry_reply->MaxXAreaFourth));
	memcpy(&telemetry_reply->MinYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_28], sizeof(telemetry_reply->MinYAreaFourth));
	memcpy(&telemetry_reply->MaxYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_30], sizeof(telemetry_reply->MaxYAreaFourth));
	memcpy(&telemetry_reply->MinXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_32], sizeof(telemetry_reply->MinXAreaFifth));
	memcpy(&telemetry_reply->MaxXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_34], sizeof(telemetry_reply->MaxXAreaFifth));
	memcpy(&telemetry_reply->MinYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_36], sizeof(telemetry_reply->MinYAreaFifth));
	memcpy(&telemetry_reply->MaxYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_38], sizeof(telemetry_reply->MaxYAreaFifth));


	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to retrieve the mask configuration of Sensor one of the Camera (TLM ID 73)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmReadSensorTwoMask(tlm_read_sensor_mask_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_73;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_40);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_73_LEN);

	// Error Check on uartRecieve, if error, free allocated buffer
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->MinXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->MinXAreaOne));
	memcpy(&telemetry_reply->MaxXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->MaxXAreaOne));
	memcpy(&telemetry_reply->MinYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->MinYAreaOne));
	memcpy(&telemetry_reply->MaxYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->MaxYAreaOne));
	memcpy(&telemetry_reply->MinXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_8], sizeof(telemetry_reply->MinXAreaTwo));
	memcpy(&telemetry_reply->MaxXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_10], sizeof(telemetry_reply->MaxXAreaTwo));
	memcpy(&telemetry_reply->MinYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_12], sizeof(telemetry_reply->MinYAreaTwo));
	memcpy(&telemetry_reply->MaxYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_14], sizeof(telemetry_reply->MaxYAreaTwo));
	memcpy(&telemetry_reply->MinXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_16], sizeof(telemetry_reply->MinXAreaThree));
	memcpy(&telemetry_reply->MaxXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_18], sizeof(telemetry_reply->MaxXAreaThree));
	memcpy(&telemetry_reply->MinYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_20], sizeof(telemetry_reply->MinYAreaThree));
	memcpy(&telemetry_reply->MaxYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_22], sizeof(telemetry_reply->MaxYAreaThree));
	memcpy(&telemetry_reply->MinXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_24], sizeof(telemetry_reply->MinXAreaFourth));
	memcpy(&telemetry_reply->MaxXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_26], sizeof(telemetry_reply->MaxXAreaFourth));
	memcpy(&telemetry_reply->MinYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_28], sizeof(telemetry_reply->MinYAreaFourth));
	memcpy(&telemetry_reply->MaxYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_30], sizeof(telemetry_reply->MaxYAreaFourth));
	memcpy(&telemetry_reply->MinXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_32], sizeof(telemetry_reply->MinXAreaFifth));
	memcpy(&telemetry_reply->MaxXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_34], sizeof(telemetry_reply->MaxXAreaFifth));
	memcpy(&telemetry_reply->MinYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_36], sizeof(telemetry_reply->MinYAreaFifth));
	memcpy(&telemetry_reply->MaxYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_38], sizeof(telemetry_reply->MaxYAreaFifth));


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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
		return E_GENERIC;
	}

	return SUCCESS;
}


/*
 * Used to adjust the boresight pixel location for camera one (TC ID 50)
 *
 * @param X_Pixel 100 * (X_Pixel location of CAM1 boresight)
 * @param Y_Pixel 100 * (X_Pixel location of CAM1 boresight)
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraOneBoresight(uint16_t X_Pixel, uint16_t Y_Pixel) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_50_LEN);
	sizeOfBuffer = TELECOMMAND_50_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_50;

	// Fill buffer with X_pixel
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_0], &X_Pixel, sizeof(X_Pixel));

	// Fill buffer with Y_pixel
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_2], &Y_Pixel, sizeof(Y_Pixel));

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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to adjust the boresight pixel location for camera one (TC ID 51)
 *
 * @param X_Pixel 100 * (X_Pixel location of CAM2 boresight)
 * @param Y_Pixel 100 * (X_Pixel location of CAM2 boresight)
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraTwoBoresight(uint16_t X_Pixel, uint16_t Y_Pixel) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t  tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_51_LEN);
	sizeOfBuffer = TELECOMMAND_51_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_51;

	// Fill buffer with X_pixel
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_0], &X_Pixel, sizeof(X_Pixel));

	// Fill buffer with Y_pixel
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_2], &Y_Pixel, sizeof(Y_Pixel));

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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to set the mask for Camera 1
 *
 * @param MaskNumber Number indicating which of 5 (0 - 4) areas will be specified
 * @param Xmin Lower X limit of specified masked area
 * @param Xmax Upper X limit of specified masked area
 * @param Ymin Lower Y limit of specified masked area
 * @param Ymax Upper Y limit of specified masked area
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraOneMask(uint8_t MaskNumber, uint16_t Xmin, uint16_t Xmax, uint16_t Ymin, uint16_t Ymax) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_52_LEN);
	sizeOfBuffer = TELECOMMAND_52_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_52;

	// Fill buffer
	telecommandBuffer[TELECOMMAND_OFFSET_0] = MaskNumber;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_1], &Xmin, sizeof(Xmin));
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_3], &Xmax, sizeof(Xmax));
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_5], &Ymin, sizeof(Ymin));
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_7], &Ymax, sizeof(Ymax));

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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to set the mask for Camera 2
 *
 * @param MaskNumber Number indicating which of 5 (0 - 4) areas will be specified
 * @param Xmin Lower X limit of specified masked area
 * @param Xmax Upper X limit of specified masked area
 * @param Ymin Lower Y limit of specified masked area
 * @param Ymax Upper Y limit of specified masked area
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraTwoMask(uint8_t MaskNumber, uint16_t Xmin, uint16_t Xmax, uint16_t Ymin, uint16_t Ymax) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_53_LEN);
	sizeOfBuffer = TELECOMMAND_53_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_53;

	// Fill buffer
	telecommandBuffer[TELECOMMAND_OFFSET_0] = MaskNumber;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_1], &Xmin, sizeof(Xmin));
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_3], &Xmax, sizeof(Xmax));
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_5], &Ymin, sizeof(Ymin));
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_7], &Ymax, sizeof(Ymax));

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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
		return E_GENERIC;
	}

	return SUCCESS;
}

/*
 * Used to set to distortion correction coefficients for camera one
 *
 * @param mantissa1 mantissa of coefficient 1
 * @param exponent1 exponent of coefficient 1
 * @param mantissa2 mantissa of coefficient 2
 * @param exponent2 exponent of coefficient 2
 * @param mantissa3 mantissa of coefficient 3
 * @param exponent3 exponent of coefficient 3
 * @param mantissa4 mantissa of coefficient 4
 * @param exponent4 exponent of coefficient 4
 * @param mantissa5 mantissa of coefficient 5
 * @param exponent5 exponent of coefficient 5
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraOneDistortionCorrection(uint16_t mantissa1, uint8_t exponent1, uint16_t mantissa2, uint8_t exponent2,
		uint16_t mantissa3, uint8_t exponent3, uint16_t mantissa4, uint8_t exponent4, uint16_t mantissa5, uint8_t exponent5) {

	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_54_LEN);
	sizeOfBuffer = TELECOMMAND_54_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_54;

	// Fill buffer
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_0], &mantissa1, sizeof(mantissa1));
	telecommandBuffer[TELECOMMAND_OFFSET_2] = exponent1;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_3], &mantissa2, sizeof(mantissa2));
	telecommandBuffer[TELECOMMAND_OFFSET_5] = exponent2;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_6], &mantissa3, sizeof(mantissa3));
	telecommandBuffer[TELECOMMAND_OFFSET_8] = exponent3;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_9], &mantissa4, sizeof(mantissa4));
	telecommandBuffer[TELECOMMAND_OFFSET_11] = exponent4;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_12], &mantissa5, sizeof(mantissa5));
	telecommandBuffer[TELECOMMAND_OFFSET_14] = exponent5;


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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
		return E_GENERIC;
	}

	return SUCCESS;

}


/*
 * Used to set to distortion correction coefficients for camera two
 *
 * @param mantissa1 mantissa of coefficient 1
 * @param exponent1 exponent of coefficient 1
 * @param mantissa2 mantissa of coefficient 2
 * @param exponent2 exponent of coefficient 2
 * @param mantissa3 mantissa of coefficient 3
 * @param exponent3 exponent of coefficient 3
 * @param mantissa4 mantissa of coefficient 4
 * @param exponent4 exponent of coefficient 4
 * @param mantissa5 mantissa of coefficient 5
 * @param exponent5 exponent of coefficient 5
 * @return error of telecommand. 0 on success, otherwise failure
 */
static int tcCameraTwoDistortionCorrection(uint16_t mantissa1, uint8_t exponent1, uint16_t mantissa2, uint8_t exponent2,
		uint16_t mantissa3, uint8_t exponent3, uint16_t mantissa4, uint8_t exponent4, uint16_t mantissa5, uint8_t exponent5) {

	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_55_LEN);
	sizeOfBuffer = TELECOMMAND_55_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_55;

	// Fill buffer
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_0], &mantissa1, sizeof(mantissa1));
	telecommandBuffer[TELECOMMAND_OFFSET_2] = exponent1;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_3], &mantissa2, sizeof(mantissa2));
	telecommandBuffer[TELECOMMAND_OFFSET_5] = exponent2;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_6], &mantissa3, sizeof(mantissa3));
	telecommandBuffer[TELECOMMAND_OFFSET_8] = exponent3;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_9], &mantissa4, sizeof(mantissa4));
	telecommandBuffer[TELECOMMAND_OFFSET_11] = exponent4;
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_12], &mantissa5, sizeof(mantissa5));
	telecommandBuffer[TELECOMMAND_OFFSET_14] = exponent5;


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

	if (error != 0){
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_REPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0){
		return E_GENERIC;
	}

	return SUCCESS;

}
