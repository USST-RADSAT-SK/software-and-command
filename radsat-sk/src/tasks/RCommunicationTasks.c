/**
 * @file RCommunicationTasks.c
 * @date January 30, 2022
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#include <RCommunicationTasks.h>
#include <RTransceiver.h>
#include <RProtocolService.h>
#include <RTelecommandService.h>
#include <RFileTransferService.h>
#include <RCommunicationStateMachine.h>
#include <RRadsat.pb.h>
#include <RFileTransferService.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/semphr.h>

#include <string.h>

#include <RFileTransferService.h>
#include <RCommon.h>
#include <RTransceiver.h>
#include <RMessage.h>
#include <string.h>
#include <RCommon.h>



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

#define RX_MUTEX_WAIT_TICKS 100
#define TX_MUTEX_WAIT_TICKS 100
xSemaphoreHandle stateMachineMutex = NULL;



#define RX_MUTEX_WAIT_TICKS 100
#define TX_MUTEX_WAIT_TICKS 100
xSemaphoreHandle stateMachineMutex;


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

	/*
	stateMachineMutex = xSemaphoreCreateMutex();
	while(1){
		debugPrint("RxTask\n");
		vTaskDelay(10000);
	}
	*/


	int error = 0;				// error detection
	uint16_t rxFrameCount = 0;	// number of frames currently in the receiver's buffer
	uint16_t rxMessageSize = 0;	// size (in bytes) of a received frame
	uint8_t rxMessage[TRANCEIVER_RX_MAX_FRAME_SIZE] = { 0 };	// input buffer for received frames


	debugPrint("Starting Rx communication loop\n");
	while (1) {
		// get the number of frames currently in the receive buffer
		error = transceiverRxFrameCount(&rxFrameCount);
		if (error) errorPrint("%d\n", error);

		// obtain frames when present
		if (rxFrameCount > 0 && error == 0) {

			debugPrint("Receiving frame from transceiver.\n");

			// obtain new frame from the transceiver
			error = transceiverGetFrame(rxMessage, &rxMessageSize);

			// attempt to extract a message
			genericHandle(rxMessage, rxMessageSize);
		}
		vTaskDelay(150);
	}

}





/**
 * Transmits outgoing frames to the Transceiver's transmitter buffer.
 *
 * When NOT in a pass mode, this task does nothing. When in a pass and in the telecommand mode
 * (i.e. receiving telecommands), this task is responsible for transmitting the appropriate ACKs
 * (or NACKs) and updating the flags (that are local and private to this module). When in a pass
 * and in the File Transfer mode, this task is responsible for transmitting frames that are ready
 * for downlink transmission. Preparation of downlink messages is done by another module prior to
 * the pass duration.
 *
 * @note	This is a high priority task.
 * @note	When an operational error occurs (e.g. a call to the transceiver module failed), this
 * 			Task will simply ignore the operation and try again next time. Lower level modules
 * 			(e.g. the Transceiver) are responsible for reporting these errors to the system.
 * @param	parameters Unused.
 */
void CommunicationTxTask(void* parameters) {
	// ignore the input parameter
	(void)parameters;


	int error = 0;												// error detection
	uint8_t txSlotsRemaining = TRANCEIVER_TX_MAX_FRAME_COUNT;	// number of open frame slots in the transmitter's buffer
	uint8_t txMessageSize = 0;									// size (in bytes) of an outgoing frame
	uint8_t txMessage[TRANCEIVER_TX_MAX_FRAME_SIZE] = { 0 };	// output buffer for messages to be transmitted
	/*

	uint8_t txMessageSize2 = 0;
	uint8_t txMessage2[TRANCEIVER_TX_MAX_FRAME_SIZE] = "General Kenobi";

	txMessageSize = 11;
	txMessageSize2 = 14;

// uncomment this block for no transmission (including no beacon)

	while (0){
		error = IsisTrxvu_tcClearBeacon(0);
		debugPrint("beacon error = %d\n", error);
		vTaskDelay(5000);
	}

	error = IsisTrxvu_tcSetIdlestate(0, trxvu_idle_state_on); // remove

	if (error) debugPrint("Idle state error = %d", error); // remove
	while (1) {
		debugPrint("Sending frame to transceiver.\n");

		error = transceiverSendFrame(framedata, framesize, &txSlotsRemaining);
		if (error) debugPrint("value of error 1: %d\n", error);
		debugPrint("Message 1: \"%s\" \n", txMessage);

		error = transceiverSendFrame(txMessage, txMessageSize, &txSlotsRemaining);
		debugPrint("value of error 2: %d\n", error);
		debugPrint("Message 2: \"%s\" \n", txMessage);

		error = transceiverSendFrame(txMessage2, txMessageSize2, &txSlotsRemaining);
		debugPrint("value of error 3: %d\n", error);
		debugPrint("Message 3: \"%s\" \n", txMessage2);

		debugPrint("\n============================\n");

		vTaskDelay(5000); //COMMUNICATION_TX_TASK_LONG_DELAY_MS
	}*/

	debugPrint("Starting Tx communication loop\n");

	while (1) {
		txMessageSize = getNextFrame(txMessage);


		//debugPrint("txMessageSize = %d\n", txMessageSize);
		// send the message
		if (txMessageSize > 0){
			debugPrint("Sending frame to transceiver.\n");
			error = transceiverSendFrame(txMessage, txMessageSize, &txSlotsRemaining);
		}

		// increase Task delay time when the Transmitter's buffer is full to give it time to transmit
		if (txSlotsRemaining > 0)
			vTaskDelay(COMMUNICATION_TX_TASK_SHORT_DELAY_MS);
		else
			vTaskDelay(COMMUNICATION_TX_TASK_LONG_DELAY_MS);
	}
}



