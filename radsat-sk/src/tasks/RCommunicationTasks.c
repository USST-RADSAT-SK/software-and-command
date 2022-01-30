/**
 * @file RCommunicationTask.c
 * @date December 23, 2021
 * @author Tyrel Kostyk
 */

#include <RTransceiver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <RCommunicationTasks.h>
#include <float.h>

#define PASS_TIMER_ID ((uint8_t)1)
#define MAX_PASS_LENGTH ((uint16_t)240000) // 4 minutes in ms


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

typedef struct _telecommand_state_t {
	uint8_t transmitReady;		// 0 = idle (awaiting message); 1 = message processed (send response)
	uint8_t responseToSend;		// 0 = ACK; 1 = NACK
} telecommand_state_t;


typedef struct _fileTransfer_state_t {
	uint8_t transmitReady;		// 0 = idle (awaiting response); 1 = response received (send message)
	uint8_t responseReceived;		// 0 = ACK (send new message); 1 = NACK (re-send last message)
} fileTransfer_state_t;


typedef struct _communications_state_t {
	uint8_t mode;			// 0 = idle (not in passtime); 1 = telecommand mode; 2 = file transfer mode
	telecommand_state_t telecommand;
	fileTransfer_state_t fileTransfer;
} communications_state_t;


enum responseTypes{
	responseACK = 0,
	responseNACK = 1
};


enum commsModeTypes{
	commsIdleMode = 0,
	commsTelecommandMode = 1,
	commsFileTransferMode = 2
};


static xTimerHandle passTimer;
static communications_state_t commsState = { 0 };

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void resetPassData(communications_state_t* comms);
static void startPassMode(void);
static void passTimeoutCallback( xTimerHandle timer );

/***************************************************************************************************
											 PUBLIC API
***************************************************************************************************/

void receiverTask(void* parameters) {
	(void)parameters;

	// Instantiate receiver message handling variables
	uint16_t rxFrameCount = 0;
	uint8_t rxMessage[TRANCEIVER_RX_MAX_FRAME_SIZE] = { 0 };
	uint16_t rxMessageSize = 0;

	while(1) {
		// Reset and get the number of frames in the receive buffer
		rxFrameCount = 0;
		int receiverErr = transceiverRxFrameCount(&rxFrameCount);

		if (rxFrameCount > 0) {
			// If there are frames in the rx buffer, begin transmission and enter
			// telecommand mode and start a pass timer
			commsState.mode = commsTelecommandMode;
			startPassMode();
		}

		// Perform transmition operations while in telecommand or file transfer mode
		while (commsState.mode > 0) {
			// If we are in telecommand mode and we have sent our ACK/NACK from the previous message
			if (commsState.mode == commsTelecommandMode && !commsState.telecommand.transmitReady) {
				// Reset the message size tracker and buffer to read from the receiver buffer
				rxMessageSize = 0;
				memset(&rxMessage, 0, sizeof(rxMessage));
				transceiverGetFrame(&rxMessage, &rxMessageSize);

				// TODO: Pass Message to command manager and receive and ACK/NACK response
				//  and whether end of transmission
				// 			(0 = ACK; 1= NACK)
				int response = responseACK;
				int endOfTrans = 1;

				// Load whether to send an ACK or NACK
				commsState.telecommand.responseToSend = response;

				// Change mode if received an end-of-transmission signal
				if (endOfTrans == 1) {
					commsState.mode = commsFileTransferMode;
				}

				// Approve transmit to send the ACK or NACK response
				commsState.telecommand.transmitReady = 1;
			}
			// If we are in file transfer mode and we have sent our ACK/NACK from the previous message
			else if (commsState.mode == commsFileTransferMode && !commsState.fileTransfer.transmitReady) {
				// Reset the message size and buffer to read from the receiver buffer
				rxMessageSize = 0;
				memset(&rxMessage, 0, sizeof(rxMessage));
				transceiverGetFrame(&rxMessage, &rxMessageSize);


				// TODO: Pass Message to command manager and get back whether it
				//  was an ACK or NACK from the ground station
				// 			(0 = ACK; 1= NACK)
				int response = responseACK;

				// Load whether we received and ACK or NACK and approve a transmit
				commsState.telecommand.responseToSend = response;
				commsState.telecommand.transmitReady = 1;
			}
			// Pause to allow Transmitter Task to run
			vTaskDelay(1);
		}
		vTaskDelay(1);
	}
}


void transmitterTask(void* parameters) {
	(void)parameters;

	// Instantiate transmitter message handling variables
	uint8_t txSlotsRemaining = 0;
	uint8_t message[TRANCEIVER_TX_MAX_FRAME_SIZE] = { 0 };
	uint8_t messageSize = 0;

	while(1) {
		// Perform transmition operations while in telecommand or file transfer mode
		while (commsState.mode) {
			// If we are in telecommand mode and we are approved for a transmit
			if ((commsState.mode == commsTelecommandMode || commsState.mode == commsFileTransferMode) && commsState.telecommand.transmitReady) {
				// Get the response from the communications state structure and reset
				// the counter for the transmition slots remaining
				uint8_t response = commsState.telecommand.responseToSend;
				uint8_t txSlotsRemaining = 0;

				// Send the ACK/NACK response to the transmitter and
				// Mark the message as sent
				int tranmitterErr = transceiverSendFrame(&response, 1, &txSlotsRemaining);
				commsState.telecommand.transmitReady = 0;

				// TODO: Error check adding the message to the transmitter buffer
			}
			// If we are in file transfer mode but we still need to send an ACK/NACK from the telecommand mode
			else if (commsState.mode == commsFileTransferMode && commsState.telecommand.transmitReady) {
				uint8_t response = commsState.telecommand.responseToSend;
				uint8_t txSlotsRemaining = 0;

				transceiverSendFrame(response, 1, &txSlotsRemaining); // Send the ACK/NACK

				commsState.telecommand.transmitReady = 0;		// Mark the message as sent

				// TODO: Error check adding the message to the transmitter buffer
			}
			// If we are in file transfer mode and we are approved for a transmit
			else if (commsState.mode == commsFileTransferMode && commsState.fileTransfer.transmitReady) {	// File Transfer
				if (txSlotsRemaining > 0) {	// Make sure we don't overflow the tx buffer
					if (commsState.fileTransfer.responseReceived == responseNACK){	// Received a NACK from ground station, so re-send msg
						transceiverSendFrame(&message, messageSize, &txSlotsRemaining); // Send the ACK/NACK

						commsState.telecommand.transmitReady = 0;		// Mark the message as sent
					}
					else {
						// TODO: Get new message and size from downlink manager and send it
						transceiverSendFrame(&message, messageSize, &txSlotsRemaining); // Send the ACK/NACK

						commsState.telecommand.transmitReady = 0;		// Mark the message as sent
					}
				}
			}
			vTaskDelay(1);
		}
		vTaskDelay(1);
	}
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Reset the structure for coordinating down and uplinking
 *
 * @param comms A communications_t struct that holds the data for coordination communications.
 */
static void resetPassData(communications_state_t* comms) {
	comms->mode = 0;
	comms->telecommand.transmitReady = 0;
	comms->telecommand.responseToSend = 0;
	comms->fileTransfer.transmitReady = 0;
	comms->fileTransfer.responseReceived = 0;
}


/**
 * Callback function for the pass timer that resets structs to a neutral state in preparation for the next pass
 *
 * @param timer A handle for a timer. However this is implicitly called and is passed without parameters
 * 				to xTimerCreate()
 */
static void passTimeoutCallback(xTimerHandle timer) {
	resetPassData(&commsState);
}

/**
 * Starts a timer for the pass timeout
 */
static void startPassMode(void) {
	if (passTimer == NULL) {
		// If the timer was not created yet, create it
		passTimer = xTimerCreate("passTimer", MAX_PASS_LENGTH, pdFALSE, PASS_TIMER_ID, passTimeoutCallback);
		xTimerStart(passTimer, 0);
	}
	else {
		// Otherwise restart it
		xTimerReset(passTimer, 0);
	}
}




