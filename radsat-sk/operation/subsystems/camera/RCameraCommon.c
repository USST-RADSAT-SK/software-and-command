/**
 * @file RCameraCommon.c
 * @date August 19, 2022
 */

#include <RCameraCommon.h>
#include <RUart.h>
//#include <RImage.h>
#include <RCommon.h>
#include <stdlib.h>
#include <string.h>
//#include <freertos/task.h>

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/*
 * Used to dynamically allocated buffer sizes as each telecommand and telemetry
 * require different sizes
 *
 * @note must use Free() to free the allocated memory when finished using the buffer
 * @param message_size defines how many data bytes are required in the buffer
 * @return dynamically allocated buffer
 * */
uint8_t * MessageBuilder(uint8_t message_size) {

	// Define the total size the buffer should be
    uint8_t total_buffer_length = message_size + BASE_MESSAGE_LEN;

    // Dynamically Allocate a buffer for telecommand and telemetry
    uint8_t* buffer = malloc(sizeof(*buffer) * total_buffer_length);

    // Fill buffer with default values
    for(uint8_t i = 0; i < total_buffer_length; i++) {
        if (i == 0) {
        	buffer[i] = ESCAPE_CHARACTER;
        }
        else if (i == 1) {
            buffer[i] = START_IDENTIFIER;
        }
        else if (i == total_buffer_length-2) {
        	buffer[i] = ESCAPE_CHARACTER;
        }
        else if (i == total_buffer_length-1) {
        	buffer[i] = END_IDENTIFIER;
        }
        else {
        	buffer[i] = FILLER;
        }
    }

    return buffer;
}


/*
 * Used to escape and transmit UART telecommand messages
 *
 * @param telecommandBuffer is a pointer to a telecommand buffer to escape and transmit
 * @param messageSize defines how many data bytes are required in the buffer
 * @return error of telecommand attempt. 0 on success, otherwise failure
 */
int escapeAndTransmitTelecommand(uint8_t *telecommandBuffer, uint8_t messageSize) {
	uint8_t *escapedBuffer;
	uint8_t escapeBufferIndex;
	uint8_t escapeCharacterCount = 0;
	uint8_t newMessageSize;
	int error;

	// Count the number of characters that should be escaped in the data bytes
	for(uint8_t i = TELECOMMAND_OFFSET_0; i < messageSize - 2; i++) {
		if (telecommandBuffer[i] == ESCAPE_CHARACTER) {
			escapeCharacterCount++;
		}
	}

	// If no escaping is needed, send the telecommand as is and let function caller free the buffer
	if (escapeCharacterCount == 0) {
		return uartTransmit(UART_CAMERA_BUS, telecommandBuffer, messageSize);
	}

	// Calculate the new buffer size
	newMessageSize = messageSize + escapeCharacterCount;

	// Build a new telecommand buffer
	escapedBuffer = MessageBuilder(newMessageSize - BASE_MESSAGE_LEN);

	// Fill the new buffer adding escape characters
	escapeBufferIndex = MESSAGE_ID_OFFSET;
	for(uint8_t i = MESSAGE_ID_OFFSET; i < messageSize - 2; i++, escapeBufferIndex++) {
		escapedBuffer[escapeBufferIndex] = telecommandBuffer[i];
		if (telecommandBuffer[i] == ESCAPE_CHARACTER) {
			// Add an extra escape character
			escapeBufferIndex++;
			escapedBuffer[escapeBufferIndex] = telecommandBuffer[i];
		}
	}

	// Send the escaped telecommand buffer
	error = uartTransmit(UART_CAMERA_BUS, escapedBuffer, newMessageSize);

	// Free the new escaped buffer
	free(escapedBuffer);

	return error;
}


/*
 * Used to receive and unescape UART telemetry messages
 *
 * @param telemetryBuffer is a pointer to a telemetry buffer where data will be stored
 * @param messageSize defines how many data bytes are required in the buffer
 * @return error of telecommand attempt. 0 on success, otherwise failure
 */
int receiveAndUnescapeTelemetry(uint8_t *telemetryBuffer, uint8_t messageSize) {
	// Receive the message via UART
	int error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, messageSize);
	if (error != 0) {
		printf("receiveAndUnescapeTelemetry(): Error during first uartReceive()... (error=%d)\n", error);
		return E_GENERIC;
	}

	// Check for escaped characters in the data bytes
	for(uint8_t i = TELEMETRY_OFFSET_0; i < messageSize - 2; i++) {
		if (telemetryBuffer[i] == ESCAPE_CHARACTER) {
			// Left shift array to remove the duplicated escape character
			memmove(&telemetryBuffer[i+1], &telemetryBuffer[i+2], (messageSize-i-1)*sizeof(uint8_t));
			// Read one more byte from UART and store at the end of the buffer
			error = uartReceive(UART_CAMERA_BUS, &telemetryBuffer[messageSize-1], 1);
			if (error != 0) {
				printf("receiveAndUnescapeTelemetry(): Error during second uartReceive()... (error=%d)\n", error);
				return E_GENERIC;
			}
		}
	}

	return SUCCESS;
}
