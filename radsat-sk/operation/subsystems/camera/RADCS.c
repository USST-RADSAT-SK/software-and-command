/**
 * @file RADCS.c
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

#include <RCameraCommon.h>
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

#define TELEMETRY_22_TO_25_LEN			((uint8_t) 10)

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/*
 * Used to send image capture & detect telecommand (TC ID 20)
 *
 * @param camera defines which camera to use for capture and detection, camera 1 = 0, camera 2 = 1
 * @param sram defines which SRAM to use on Cubesense, SRAM1 = 0, SRAM2 = 1
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
int tcImageCaptureAndDetection(uint8_t camera, uint8_t sram) {
	uint8_t *telecommandBuffer;
	uint8_t *telecommandResponse;
	uint16_t sizeOfBuffer;
	uint8_t tcErrorFlag;
	int error;

	// Dynamically allocate a buffer to hold the Telecommand message with header and footer implemented
	telecommandBuffer = MessageBuilder(TELECOMMAND_20_LEN);
	sizeOfBuffer = TELECOMMAND_20_LEN + BASE_MESSAGE_LEN;

	// Fill buffer with Telecommand ID
	telecommandBuffer[MESSAGE_ID_OFFSET] = TELECOMMAND_20;

	// Fill buffer with SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_0] = camera;

	// Fill buffer with Location in SRAM
	telecommandBuffer[TELECOMMAND_OFFSET_1] = sram;

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
 * Used to request the detection results and trigger a new detection (TLM ID 22 to 25)
 *
 * @param telemetry_reply defines where the detection results will be stored
 * @param sensorSelection defines the selected sensor and SRAM to get the detection results from
 * @return error of telemetry request attempt. 0 on success, otherwise failure
 * */
int tlmSensorResultAndDetection(tlm_detection_result_and_trigger_adcs_t *telemetry_reply, SensorResultAndDetection sensorSelection) {
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
	telemetryBuffer[MESSAGE_ID_OFFSET] = sensorSelection;

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
	error = receiveAndUnescapeTelemetry(telemetryBuffer, TELEMETRY_22_TO_25_LEN);

	if (error != 0) {
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
 * @param alpha result in centidegrees after executing TLM22 or 25
 * @param beta result in centidegrees after executing TLM22 or 25
 * @return struct containing the components of the 3D Vector
 */
interpret_detection_result_t detectionResult(uint16_t alpha, uint16_t beta) {
	float theta;
	float phi;
	interpret_detection_result_t data = {0};

	theta = sqrt(pow((float)alpha/100, 2) + pow((float)beta/100, 2)) * M_PI/180;
	phi = atan2(beta, alpha);

	data.X_AXIS = sin(theta) * cos(phi);
	data.Y_AXIS = sin(theta) * sin(phi);
	data.Z_AXIS = cos(theta);

	return data;
}
