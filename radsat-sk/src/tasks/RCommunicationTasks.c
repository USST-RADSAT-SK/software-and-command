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

#define PASS_TIMER_ID 1
#define MAX_PASS_LENGTH 240000 // 4 minutes in ms

static int passtime = 0;
static xTimerHandle passTimer;

static communications_state_t commsState = { 0 };

typedef enum _response_t{ACK, NACK} _response_t;
typedef enum _comms_mode_t{IDLE, TELECOMMAND, FILE_TRANSFER} comms_mode_t;

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void resetPassData(communications_state_t* comms);
static void startPassMode(void);
static void passTimeoutCallback( xTimerHandle timer );

/***************************************************************************************************
											 PUBLIC API
***************************************************************************************************/

void receiverTask(void* parameters)
{
	(void)parameters;

	uint16_t rxFrameCount = 0;
	uint8_t rxMessage[TRANCEIVER_RX_MAX_FRAME_SIZE] = { 0 };
	uint16_t rxMessageSize = 0;

	while(1) {

		rxFrameCount = 0;
		int err = transceiverRxFrameCount(&rxFrameCount);

		if (rxFrameCount) {
			commsState.mode = TELECOMMAND;	// If there are frames in the rx buffer enter telecommand mode
			startPassMode();		// Start pass timer
		}

		while (commsState.mode) {		// While in telecommand or file transfer
			if (commsState.mode == TELECOMMAND && !commsState.telecommand.transmitReady) {			// Telecommand
				rxMessageSize = 0;
				memset(&rxMessage, 0, sizeof(rxMessage));
				transceiverGetFrame(&rxMessage, &rxMessageSize);

				// TODO: Pass Message to command manager and receive and ACK/NACK response and whether end of transmission
				// 			(0 = ACK; 1= NACK)
				int response = 0;
				int endOfTrans = 1;

				commsState.telecommand.responseToSend = response;		// Load response

				if (endOfTrans) {								// Change mode if received an end-of-transmission signal
					commsState.mode = FILE_TRANSFER;
				}

				commsState.telecommand.transmitReady = 1;			// Approve transmit
			}
			else if (commsState.mode == FILE_TRANSFER && !commsState.fileTransfer.transmitReady) {	// File Transfer
				rxMessageSize = 0;
				memset(&rxMessage, 0, sizeof(rxMessage));
				transceiverGetFrame(&rxMessage, &rxMessageSize);	// Might need to change later to get the size of the incoming message first

				// TODO: Pass Message to command manager and receive whether or not the ground station sent an ACK or NACK response
				// 			(0 = ACK; 1= NACK)
				int response = 0;

				commsState.telecommand.responseToSend = response;		// Load response

				commsState.telecommand.transmitReady = 1;			// Approve transmit
			}
			vTaskDelay(1);
		}
		vTaskDelay(1);
	}
}

void transmitterTask(void* parameters)
{
	(void)parameters;
	uint8_t txSlotsRemaining = 0;
	uint8_t message[TRANCEIVER_TX_MAX_FRAME_SIZE] = { 0 };
	uint8_t messageSize = 0;

	while(1) {

		while (commsState.mode) {	// While in pass range
			if (commsState.mode == TELECOMMAND && commsState.telecommand.transmitReady) {	// Telecommand
				uint8_t response = commsState.telecommand.responseToSend;	// Grab the ACK/NACK from the downlink manager
				uint8_t txSlotsRemaining = 0;

				// TODO error check for failed frame
				transceiverSendFrame(&response, 1, &txSlotsRemaining); // Send the ACK/NACK

				commsState.telecommand.transmitReady = 0;		// Mark the message as sent

			}
			else if (commsState.mode == FILE_TRANSFER && commsState.telecommand.transmitReady) {
				// When transitioning from telecommand to file transfer, need to
				// send end-of-transmission ACK/NACK before sending files
				uint8_t response = commsState.telecommand.responseToSend;	// Grab the ACK/NACK from the downlink manager
				uint8_t txSlotsRemaining = 0;

				transceiverSendFrame(response, 1, &txSlotsRemaining); // Send the ACK/NACK

				commsState.telecommand.transmitReady = 0;		// Mark the message as sent
			}
			else if (commsState.mode == FILE_TRANSFER && commsState.fileTransfer.transmitReady) {	// File Transfer
				if (txSlotsRemaining > 0) {	// Make sure we don't overflow the tx buffer
					if (commsState.fileTransfer.responseReceived == NACK){	// Received a NACK from ground station, so re-send msg
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
static void passTimeoutCallback( xTimerHandle timer ) {
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




