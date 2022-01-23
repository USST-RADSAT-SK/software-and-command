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

static communications_t commsData;
/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void resetPassData(communications_t comms);
static void startPassMode(void);
static void vCallbackFunction( xTimerHandle timer );

/***************************************************************************************************
											 PUBLIC API
***************************************************************************************************/

void receiverTask(void* parameters)
{
	(void)parameters;

	uint16_t rxFrameCount = 0;
	uint16_t* rxMessage[TRANCEIVER_RX_MAX_FRAME_SIZE];
	resetPassData(commsData);

	while(1) {

		int err = transceiverRxFrameCount(&rxFrameCount);

		if (rxFrameCount) {
			commsData.mode = 1;	// If there are frames in the rx buffer enter telecommand mode
			startPassMode();		// Start pass timer
		}

		while (commsData.mode) {
			if (commsData.mode == 1 && !commsData.telecommand.transmit) {			// Telecommand
				transceiverGetFrame(rxMessage, TRANCEIVER_RX_MAX_FRAME_SIZE);	// Might need to change later to get the size of the incoming message first

				// TODO: Pass Message to downlink manager and receive and ACK/NACK response and whether end of transmission
				// 			(0 = ACK; 1= NACK)
				int response = 0;
				int endOfTrans = 1;

				commsData.telecommand.response = response;		// Load response

				if (endOfTrans) {								// Change mode if received an end-of-transmission signal
					commsData.mode = 2;
				}

				commsData.telecommand.transmit = 1;			// Approve transmit
			}
			else if (commsData.mode == 2 && !commsData.fileTransfer.transmit) {	// File Transfer
				transceiverGetFrame(rxMessage, TRANCEIVER_RX_MAX_FRAME_SIZE);	// Might need to change later to get the size of the incoming message first

				// TODO: Pass Message to downlink manager and receive whether or not the ground station sent an ACK or NACK response
				// 			(0 = ACK; 1= NACK)
				int response = 0;

				commsData.telecommand.response = response;		// Load response

				commsData.telecommand.transmit = 1;			// Approve transmit
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
	uint8_t* message[TRANCEIVER_TX_MAX_FRAME_SIZE];
	uint8_t messageSize = TRANCEIVER_TX_MAX_FRAME_SIZE;

	while(1) {

		while (commsData.mode) {
			if (commsData.mode == 1 && commsData.telecommand.transmit) {	// Telecommand
				uint8_t* response = {commsData.telecommand.response};	// Grab the ACK/NACK from the downlink manager
				uint8_t txSlotsRemaining = 0;

				transceiverSendFrame(response, 1, &txSlotsRemaining); // Send the ACK/NACK

				commsData.telecommand.transmit = 0;		// Mark the message as sent

			}
			else if (commsData.mode == 2 && commsData.telecommand.transmit) {
				// When transitioning from telecommand to file transfer, need to
				// send end-of-transmission ACK/NACK before sending files
				uint8_t* response = {commsData.telecommand.response};	// Grab the ACK/NACK from the downlink manager

				transceiverSendFrame(response, 1, &txSlotsRemaining); // Send the ACK/NACK

				commsData.telecommand.transmit = 0;		// Mark the message as sent
			}
			else if (commsData.mode == 2 && commsData.fileTransfer.transmit) {	// File Transfer
				if (txSlotsRemaining > 0) {	// Make sure we don't overflow the tx buffer
					if (commsData.fileTransfer.response){	// Received a NACK from ground station, so re-send msg
						transceiverSendFrame(message, messageSize, &txSlotsRemaining); // Send the ACK/NACK

						commsData.telecommand.transmit = 0;		// Mark the message as sent
					}
					else {
						// TODO: Get new message and size from downlink manager and send it
						transceiverSendFrame(message, messageSize, &txSlotsRemaining); // Send the ACK/NACK

						commsData.telecommand.transmit = 0;		// Mark the message as sent
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
static void resetPassData(communications_t comms) {
	comms.mode = 0;
	comms.telecommand.transmit = 0;
	comms.telecommand.response = 0;
	comms.fileTransfer.transmit = 0;
	comms.fileTransfer.response = 0;
}


/**
 * Callback function for the pass timer that resets structs to a neutral state in preparation for the next pass
 *
 * @param timer A handle for a timer. However this is implicitly called and is passed without parameters
 * 				to xTimerCreate()
 */
static void vCallbackFunction( xTimerHandle timer ) {
	resetPassData(commsData);
}

/**
 * Starts a timer for the pass timeout
 */
static void startPassMode(void) {
	if (passtime == 0) {
		passtime = 1;

		if (passTimer == NULL) {
			// If the timer was not created yet, create it
			passTimer = xTimerCreate("passTimer", MAX_PASS_LENGTH, pdFALSE, PASS_TIMER_ID, vCallbackFunction);
			xTimerStart(passTimer, 0);
		}
		else {
			// Otherwise restart it
			xTimerReset(passTimer, 0);
		}
	}
}




