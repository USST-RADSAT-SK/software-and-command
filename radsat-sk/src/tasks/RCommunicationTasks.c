/**
 * @file RCommunicationTasks.c
 * @date January 30, 2022
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#include <RCommunicationTasks.h>
#include <RTransceiver.h>
#include <RProtocolService.h>
#include <RCommunicationStateMachine.h>
#include <RTransceiver.h>
#include <RMessage.h>
#include <RCommon.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include <string.h>

/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** Communication Receive Task delay (in ms). */
#define COMMUNICATION_RX_TASK_DELAY_MS			1000//((portTickType)1)

/** Communication Transmit Task delay (in ms) during typical operation. */
#define COMMUNICATION_TX_TASK_SHORT_DELAY_MS	100//((portTickType)1)

/**
 * Communication Transmit Task delay (in ms) when the transmitter's buffer is full.
 *
 * Transmission speed: 9600 bps => roughly 1 byte per ms; This delay should long enough to transmit
 * one full frame.
 */
#define COMMUNICATION_TX_TASK_LONG_DELAY_MS		1000/((portTickType)TRANCEIVER_TX_MAX_FRAME_SIZE)


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/





/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

/**
 * Receive and process frames received by the Transceiver's receiver buffer (when available).
 *
 * Constantly checks for the existence of received frames within the Transceiver; if they exist,
 * this task will retrieve and process them. If they are telecommands, they are immediately sent off
 * to another module for further processing. If the received message is an ACK/NACK, this task
 * updates the proper flags (local and private to this module) to continue communication operations
 * as guided by our protocol (single ACK-NACK responses per message).
 *
 * @note	This is a high priority task, and must never be disabled for extented periods of time.
 * @note	When an operational error occurs (e.g. a call to the transceiver module failed), this
 * 			Task will simply ignore the operation and try again next time. Lower level modules
 * 			(e.g. the Transceiver) are responsible for reporting those errors to the system.
 * @param	parameters Unused.
 */
void CommunicationRxTask(void* parameters) {
	// ignore the input parameter
	(void)parameters;

	int error = 0;				// error detection
	uint16_t rxFrameCount = 0;	// number of frames currently in the receiver's buffer
	uint16_t rxMessageSize = 0;	// size (in bytes) of a received frame
	uint8_t rxMessage[TRANCEIVER_RX_MAX_FRAME_SIZE] = { 0 };	// input buffer for received frames

	infoPrint("CommunicationRxTask started.");
	while (1) {

		int beacon = IsisTrxvu_tcClearBeacon();

		// get the number of frames currently in the receive buffer
		error = transceiverRxFrameCount(&rxFrameCount);
		if (error) errorPrint("%d\n", error);
		 messageSubject_t messageData = { 0 };

		// obtain frames when present
		if (rxFrameCount > 0 && error == 0) {

			infoPrint("Receiving frame from transceiver.");

			// obtain new frame from the transceiver
			error = transceiverGetFrame(rxMessage, &rxMessageSize);

			// attempt to extract a message
			commandType_t type = genericHandle(rxMessage, rxMessageSize, &messageData);
			processCommand(type, &messageData);

		}
		vTaskDelay(150);
	}

}
