/**
 * @file RImage.c
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

#include <RCameraCommon.h>
#include <RUart.h>
#include <RImage.h>
#include <RCommon.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/task.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define TELECOMMAND_21               	((uint8_t) 0x15)
#define TELECOMMAND_64               	((uint8_t) 0x40)
#define TELECOMMAND_65               	((uint8_t) 0x41)

#define TELECOMMAND_21_LEN           	((uint8_t) 4)
#define TELECOMMAND_64_LEN           	((uint8_t) 4)
#define TELECOMMAND_65_LEN           	((uint8_t) 3)

#define TELEMETRY_3                 	((uint8_t) 0x83)
#define TELEMETRY_20                    ((uint8_t) 0x94)
#define TELEMETRY_21                 	((uint8_t) 0x95)
#define TELEMETRY_65                 	((uint8_t) 0xC1)

#define TELEMETRY_3_LEN				    ((uint8_t) 7)
#define TELEMETRY_20_AND_21_LEN			((uint8_t) 10)
#define TELEMETRY_65_LEN				((uint8_t) 7)

#define SIZE_OF_BITMAP					((uint8_t) 4096)

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/*
 * Used to send image capture telecommand (TC ID 21)
 *
 * @param camera defines which camera to use on Cubesense, camera 1 = 0, camera 2 = 1
 * @param SRAM defines which SRAM to use on Cubesense, SRAM1 = 0, SRAM2 = 1
 * @param location defines which SRAM slot to use within selected SRAM
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tcImageCapture(uint8_t camera, uint8_t SRAM, uint8_t location) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t  tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_21_LEN);
	sizeOfBuffer = TELECOMMAND_21_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_21;

    // Fill buffer with Camera selection
	telecommandBuffer[TELECOMMAND_OFFSET_0] = camera;

	// Fill buffer with SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_1] = SRAM;

	// Fill buffer with Location in SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_2] = location;

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
 * Used to receive telemetry for sensor results (TLM ID 20 & 21)
 *
 * @param camera defines the selected sensor from which the detection result will be received
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tlmSensorResult(uint8_t camera, tlm_detection_result_and_trigger_t *telemetry_reply) {
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
	if (camera == SUN_SENSOR) {
		telemetryBuffer[MESSAGE_ID_OFFSET] = TELEMETRY_20;
	} else if (camera == NADIR_SENSOR) {
		telemetryBuffer[MESSAGE_ID_OFFSET] = TELEMETRY_21;
	} else {
		return E_GENERIC;
	}

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = receiveAndUnescapeTelemetry(telemetryBuffer, TELEMETRY_20_AND_21_LEN);

	if (error != 0) {
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
 * @param SRAM defines which SRAM to use on Cubesense, SRAM 1 = 0, SRAM 2 = 1
 * @param location defines which SRAM slot to use within selected SRAM, 0 = top, 1 = bottom
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
	telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_64;

    // Fill buffer with Camera selection
	telecommandBuffer[TELECOMMAND_OFFSET_0] = SRAM;

	// Fill buffer with SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_1] = location;

	// Fill buffer with Location in SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_2] = size;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telecommandBuffer, sizeOfBuffer); // No escaping needed

	// Free the dynamically allocated buffer
	free(telecommandBuffer);

	if (error != 0) {
		printf("tcInitImageDownload(): Error during uartTransmit()...\n");
		return E_GENERIC;
	}

	// Dynamically allocate a buffer to hold the telecommand message with header and footer implemented
	telecommandResponse = MessageBuilder(TELECOMMAND_RESPONSE_LEN);
	sizeOfBuffer = TELECOMMAND_RESPONSE_LEN + BASE_MESSAGE_LEN;

	// Read automatically reply to telecommand
	error = uartReceive(UART_CAMERA_BUS, telecommandResponse, sizeOfBuffer);

	if (error != 0) {
		printf("tcInitImageDownload(): Error during uartReceive()...");
		free(telecommandResponse);
		return E_GENERIC;
	}

	// Receive the telecommand response from buffer
	tcErrorFlag = telecommandResponse[TELECOMMAND_RESPONSE_OFFSET];

	// Free the dynamically allocated buffer
	free(telecommandResponse);

	if (tcErrorFlag != 0) {
		printf("tcInitImageDownload(): Bad tcErrorFlag...\n");
		return E_GENERIC;
	}

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
	telemetryBuffer[MESSAGE_ID_OFFSET] = TELEMETRY_65;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0) {
		printf("tlmImageFrameInfo(): Error during uartTransmit()... (error=%d)\n", error);
		return E_GENERIC;
	}

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_3);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = receiveAndUnescapeTelemetry(telemetryBuffer, TELEMETRY_65_LEN);

	if (error != 0) {
		printf("tlmImageFrameInfo(): Error during receiveAndUnescapeTelemetry()... (error=%d)\n", error);
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
 * @param NextFrameNumber number of next frame to be loaded
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tcAdvanceImageDownload(uint16_t nextFrameNumber) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t  tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_65_LEN);
	sizeOfBuffer = TELECOMMAND_65_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_65;

	// Fill buffer with Next frame number
	memcpy(&telecommandBuffer[TELECOMMAND_OFFSET_0], &nextFrameNumber, sizeof(nextFrameNumber));

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
 * Used to receive telemetry for telecommand acknoledgment (TLM ID 3)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tlmTelecommandAcknowledge(tlm_telecommand_ack_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	//  Ensure the input pointers are not NULL
	if (telemetry_reply == 0)
		return E_GENERIC;

    // Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
    telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
    sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[MESSAGE_ID_OFFSET] = TELEMETRY_3;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0) {
		return E_GENERIC;
	}

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_3);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_3_LEN); // No unescaping needed

	if (error != 0) {
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->tc_error_flag, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->tc_error_flag));


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

	float  sum,mean;
	uint8_t  n = SIZE_OF_BITMAP;

	for (uint8_t j = 0 ; j < n; ++j) {
		sum = sum + image[j];
	}

	mean = sum / n;

	return mean;
}
