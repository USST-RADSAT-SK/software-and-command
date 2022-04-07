/**
 * @file RImage.c
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

#include <RUart.h>
#include <RImage.h>
#include <RCommon.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
#define TELECOMMAND_21               	((uint8_t) 0x15)
#define TELECOMMAND_64               	((uint8_t) 0x40)
#define TELECOMMAND_65               	((uint8_t) 0x41)

#define TELECOMMAND_21_LEN           	((uint8_t) 4)
#define TELECOMMAND_64_LEN           	((uint8_t) 4)
#define TELECOMMAND_65_LEN           	((uint8_t) 3)
#define TELECOMMAND_RESPONSE_LEN		((uint16_t)1)

#define TELECOMMAND_OFFSET_ID           ((uint8_t) 2)
#define TELECOMMAND_OFFSET_0            ((uint8_t) 3)
#define TELECOMMAND_OFFSET_1            ((uint8_t) 4)
#define TELECOMMAND_OFFSET_2            ((uint8_t) 5)
#define TELECOMMAND_REPONSE_OFFSET		((uint8_t) 2)

#define TELEMETRY_3                 	((uint8_t) 0x83)
#define TELEMETRY_21                 	((uint8_t) 0x95)
#define TELEMETRY_64                 	((uint8_t) 0xC0)
#define TELEMETRY_65                 	((uint8_t) 0xC1)

#define TELEMETRY_3_LEN				    ((uint8_t) 7)
#define TELEMETRY_21_LEN				((uint8_t) 10)
#define TELEMETRY_64_LEN				((uint8_t) 132)
#define TELEMETRY_65_LEN				((uint8_t) 7)
#define TELEMETRY_REQUEST_LEN			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_1			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_3			((uint8_t) 3)
#define TELEMETRY_REPLY_SIZE_6			((uint8_t) 6)

#define TELEMETRY_OFFSET_0              ((uint8_t) 2)
#define TELEMETRY_OFFSET_2              ((uint8_t) 4)
#define TELEMETRY_OFFSET_4              ((uint8_t) 6)
#define TELEMETRY_OFFSET_5              ((uint8_t) 7)
#define TELEMETRY_ID_OFFSET		        ((uint8_t) 2)

#define SIZE_OF_THUMBNAIL				((uint8_t) 64)
#define SIZE_OF_BITMAP					((uint8_t) 4096)

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

/***************************************************************************************************
                                          PRIVATE GLOBALS
***************************************************************************************************/


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/
static uint8_t * MessageBuilder(uint8_t response_size);

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/*
 * Used to send image capture telecommand (TC ID 21) to image sensor
 *
 * @param SRAM defines which SRAM to use on Cubesense
 * @param location defines which SRAM slot to use within selected SRAM
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tcImageCaputre(uint8_t SRAM, uint8_t location) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t  tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_21_LEN);
	sizeOfBuffer = TELECOMMAND_21_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_21;

    // Fill buffer with Camera selection
	telecommandBuffer[TELECOMMAND_OFFSET_0] = IMAGE_SENSOR;

	// Fill buffer with SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_1] = SRAM;

	// Fill buffer with Location in SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_2] = location;

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
 * Used to receive telemetry for sensor 2 results (TLM ID 21)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tlmSensorTwoResult(tlm_detection_result_and_trigger_t *telemetry_reply) {
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
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_21;

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
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_21_LEN);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];


	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to send image capture telecommand (TC ID 64)
 *
 * @param SRAM defines which SRAM to use on Cubesense
 * @param location defines which SRAM slot to use within selected SRAM
 * @param size defines the resolution of the image to download, 4 = 64x64, 1 = 512x512, 0 = 1024x1024
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tcInitImageDownload(uint8_t SRAM, uint8_t location, uint8_t size) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_64_LEN);
	sizeOfBuffer = TELECOMMAND_64_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_64;

    // Fill buffer with Camera selection
	telecommandBuffer[TELECOMMAND_OFFSET_0] = SRAM;

	// Fill buffer with SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_1] = location;

	// Fill buffer with Location in SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_2] = size;

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
 * Used to receive telemetry for image frame (TLM ID 65)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tlmImageFrameInfo(tlm_image_frame_info_t *telemetry_reply) {
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
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_65;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_3);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_64_LEN);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->imageFrameNumber, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->imageFrameNumber));
	telemetry_reply->checksum = telemetryBuffer[TELEMETRY_OFFSET_2];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to send advance image capture telecommand (TC ID 65)
 *
 * @param NextFrameNumLBS is the first 8 bytes for the telecommmand
 * @param NextFrameNumMSB is the last 8 bytes for the telecommand
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tcAdvanceImageDownload(uint8_t NextFrameNumLBS, uint8_t NextFrameNumMSB) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t  tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_65_LEN);
	sizeOfBuffer = TELECOMMAND_65_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[TELECOMMAND_OFFSET_ID] = TELECOMMAND_65;

    // Fill buffer with Next frame number lsb
	telecommandBuffer[TELECOMMAND_OFFSET_0] = NextFrameNumLBS;

	// Fill buffer with SRAMNext frame number MSB
	telecommandBuffer[TELECOMMAND_OFFSET_1] = NextFrameNumMSB;

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
 * Used to receive telemetry for telecommand acknoledgment (TLM ID 3)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tlmTelecommandAcknowledge(tlm_telecommand_ack_t *telemetry_reply) {
uint8_t* telemetryBuffer;
int error;

    //  Ensure the input pointers are not NULL
if (telemetry_reply == 0)
return E_GENERIC;

    // Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
    telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
    sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_3;

   // Send Telemetry Request
	 error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Free the dynamically allocated buffer
	free(telemetryBuffer);


	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_REPLY_SIZE_3);

  // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_3_LEN);


	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->tc_error_flag,&telemetryBuffer[TELEMETRY_OFFSET_2],sizeof(telemetry_reply->tc_error_flag));


	// Free the dynamically allocated buffer
	free(telemetryBuffer);
	return SUCCESS;
}

/*
 * Used to calculate the mean value of bit map
 *
 * @param image the downloaded array of bytes from the camera
 * @return the mean value of the for the image
 */

  int calculateMeanOfTheImage(uint8_t *image) {
 	uint8_t  i,j = SIZE_OF_THUMBNAIL;
 	uint8_t  sum,mean;
 	uint8_t  n = SIZE_OF_BITMAP;

 	for (i = 0 ; i < n; ++i){
 			sum = sum + image[i];
 	  }
 	mean = sum / n;
  	return mean;
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
