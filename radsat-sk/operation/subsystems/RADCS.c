/**
 * @file RADCS.c
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

#include <RUart.h>
#include <RADCS.h>
#include <RCommon.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define TELECOMMAND_20    	          	((uint8_t) 0x14)

#define TELECOMMAND_20_LEN              ((uint8_t) 3)
#define TELECOMMAND_RESPONSE_LEN		((uint16_t)1)

#define TELECOMMAND_OFFSET_ID           ((uint8_t) 2)
#define TELECOMMAND_OFFSET_0            ((uint8_t) 3)
#define TELECOMMAND_OFFSET_1            ((uint8_t) 4)
#define TELECOMMAND_REPONSE_OFFSET		((uint8_t) 2)

#define TELEMETRY_22                 ((uint8_t) 0x96)
#define TELEMETRY_25                 ((uint8_t) 0x99)

#define TELEMETRY_22_LEN				((uint8_t) 10)
#define TELEMETRY_25_LEN				((uint8_t) 10)
#define TELEMETRY_REQUEST_LEN			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_6			((uint8_t) 6)

#define TELEMETRY_OFFSET_0              ((uint8_t) 2)
#define TELEMETRY_OFFSET_2              ((uint8_t) 4)
#define TELEMETRY_OFFSET_4              ((uint8_t) 6)
#define TELEMETRY_OFFSET_5              ((uint8_t) 7)
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

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/
static uint8_t * MessageBuilder(uint8_t response_size);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/*
 * Used to send image capture telecommand (TC ID 20) to image sensor
 *
 * @param camera defines which camera to use for detection
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tcImageCaputreAndDetection(uint8_t camera) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_20_LEN);
	sizeOfBuffer = TELECOMMAND_20_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_20;

	// Fill buffer with SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_0] = camera;

	// Fill buffer with Location in SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_1] = SRAM1;

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
 * Used to request the previous detection results (TLM ID 22)
 *
 * @param camera defines which camera to use for detection
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tlmSensorOneResultAndDetectionSRAMOne(tlm_detection_result_and_trigger_adcs_t *telemetry_reply) {
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
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_22;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_22_LEN);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to request the previous detection results (TLM ID 25)
 *
 * @param camera defines which camera to use for detection
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tlmSensorTwoResultAndDetectionSRAMOne(tlm_detection_result_and_trigger_adcs_t *telemetry_reply) {
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
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_25;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_25_LEN);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to interpret detection results into a 3D vector
 *
 * @param alpha result after executing TLM22 or 25
 * @param beta result after executing TLM22 or 25
 * @return struct containing the components of the 3D Vector
 */
interpret_detection_result_t detectionResult(uint16_t alpha, uint16_t beta) {
	uint16_t theda;
	uint16_t phi;
	interpret_detection_result_t data;

	theda = sqrt((alpha/100) * (alpha/100) + (beta/100) * (beta/100));
	phi = atan2(beta,alpha);

	data.X_AXIS = sin(theda)*cos(phi);
	data.Y_AXIS = sin(theda)*sin(phi);
	data.Z_AXIS = cos(theda);

	return data;

}

/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/*
 * Used to dynamically allocated buffer sizes as each telecommand
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
    uint8_t* Buffer = (uint8_t*) malloc(total_buffer_length * sizeof(uint8_t));

    // Initialize all elements in the buffer with zero
    for (uint8_t i = 0; i < total_buffer_length; i++){
    	Buffer[i] = 0;
    }

    // Fill Buffer with default values
    for(uint8_t i = 0; i<total_buffer_length;i++){
        if (i == 0){
        	Buffer[i] = START_IDENTIFIER1;
        }
        else if (i == 1){
            Buffer[i] = START_IDENTIFIER2;
        }
        else if (i == total_buffer_length-2) {
        	Buffer[i] = END_IDENTIFIER1;
        }
        else if (i == total_buffer_length-1) {
        	Buffer[i] = END_IDENTIFIER2;
        }
        else{
        	Buffer[i] = FILLER;
        }
    }

    return Buffer;
}
