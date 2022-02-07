/**
 * @file RCommunicationTask.c
 * @date January 30, 2021
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#include <RTransceiver.h>
#include <RCommunicationTasks.h>

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** Maximum possible duration of a pass. */
#define MAX_PASS_LENGTH	((portTickType)(15*60*1000)) // 15 minutes (in ms)


/** Abstraction of the ACK/NACK states */
typedef enum _response_state_t {
	responseStateIdle	= 0,	// Awaiting response from Ground Station
	responseStateReady	= 1,	// Ready to transmit to Ground Station
} response_state_t;


/** Abstraction of the ACK/NACK return types */
typedef enum _response_t {
	responseACK		= 0,	// Acknowledge (the message was received properly)
	responseNACK	= 1,	// Negative Acknowledge (the message was NOT received properly)
} response_t;


/** Abstraction of the communication modes */
typedef enum _comms_mode_t {
	commsModeIdle			= 0,	// Not in a pass
	commsModeTelecommand	= 1,	// Receiving Telecommands from Ground Station
	commsModeFileTransfer	= 2,	// Transmitting data to the Ground Station
} comms_mode_t;


/** Co-ordinates tasks during the telecommand phase */
typedef struct _telecommand_state_t {
	response_state_t transmitReady;	// Whether the Satellite is ready to transmit a response (ACK, NACK, etc.)
	response_t responseToSend;		// What response to send, when ready
} telecommand_state_t;


/** Co-ordinates tasks during the file transfer phase */
typedef struct _fileTransfer_state_t {
	uint8_t transmitReady;		// Whether the Satellite is ready to transmit another Frame (telemetry, etc.)
	uint8_t responseReceived;	// What response was received (ACK, NACK, etc.) regarding the previous message
} fileTransfer_state_t;


/** Wrapper structure for communications co-ordination */
typedef struct _communications_state_t {
	uint8_t mode;						// The current state of the Communications Tasks
	telecommand_state_t telecommand;	// The state during the Telecommand mode
	fileTransfer_state_t fileTransfer;	// The state during the File Transfer mode
} communications_state_t;


/** Instantiate the timer for pass time */
static xTimerHandle passTimer;
/** Instantiate the communication co-orditation structure */
static communications_state_t communicationsState = { 0 };


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void startPassMode(void);
static void resetCommunicationState(xTimerHandle timer);


/***************************************************************************************************
											 PUBLIC API
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
 * @todo	More advanced error handling?
 *
 * @note	This is a high priority task, and must never be disabled for extented periods of time.
 * @param	parameters Unused.
 */
void CommunicationsRxTask(void* parameters) {
	(void)parameters;

	int error = 0;				// error detection
	uint16_t rxFrameCount = 0;	// number of frames currently in the receiver's buffer
	uint16_t rxMessageSize = 0;	// size (in bytes) of a received frame
	uint8_t rxMessage[TRANCEIVER_RX_MAX_FRAME_SIZE] = { 0 };	// input buffer for received frames

	while (1) {
		// get the number of frames currently in the receive buffer
		rxFrameCount = 0;
		error = transceiverRxFrameCount(&rxFrameCount);

		// enter telecommand (and pass) mode, and start a pass timer
		if (rxFrameCount > 0) {
			communicationsState.mode = commsModeTelecommand;
			startPassMode();
		}

		// continue receive operations for duration of pass mode
		while (communicationsState.mode > commsModeIdle) {

			// telecommand mode, awaiting the next telecommand from the Ground Station
			if (communicationsState.mode == commsModeTelecommand
			&& !communicationsState.telecommand.transmitReady) {

				// obtain new frame from the transceiver
				rxMessageSize = 0;
				memset(rxMessage, 0, sizeof(rxMessage));
				error = transceiverGetFrame(rxMessage, &rxMessageSize);

				// TODO: forward message to command centre and determine ACK/NACK response to send
				// TODO: determine if this message was signalling the end of telecommand mode
				response_t response = responseACK;
				int endOfTelecommandMode = 1;

				// load ACK/NACK response to send down to Ground Station
				communicationsState.telecommand.responseToSend = response;

				// prepare for file transfer mode
				if (endOfTelecommandMode)
					communicationsState.mode = commsModeFileTransfer;

				// prepare to send ACK/NACK response
				communicationsState.telecommand.transmitReady = responseStateReady;
			}

			// file transfer mode, awaiting ACK/NACK from the Ground Station
			else if (communicationsState.mode == commsModeFileTransfer
			     && !communicationsState.fileTransfer.transmitReady)
			{

				// obtain new frame from the transceiver
				rxMessageSize = 0;
				memset(rxMessage, 0, sizeof(rxMessage));
				error = transceiverGetFrame(rxMessage, &rxMessageSize);

				// TODO: forward message to command centre and extract the received ACK/NACK response
				response_t response = responseACK;

				// prepare to send subsequent (or resend previous) file transfer frame
				communicationsState.fileTransfer.responseReceived = response;
				communicationsState.fileTransfer.transmitReady = responseStateReady;
			}

			vTaskDelay(1);
		}

		vTaskDelay(1);
	}
}


/**
 * Transmits outgoing frames to the Transceiver's transmitter buffer.
 *
 * When NOT in a pass mode, this task does nothing. When in a pass and in the telecommand mode
 * (i.e. receiving telecommands), this task is responsible for transmitting the appropriate ACKs
 * (or NACKs) and updating the flags (that are local and private to this module). When in a pass
 * and in the File Transfer mode, this task is responsible for transmitting frames that are ready
 * for downlink transmission. Preperation of downlink messages is done by another module prior to
 * the pass duration.
 *
 * @todo	More advanced error handling?
 *
 * @note	This is a high priority task.
 * @param	parameters Unused.
 */
void CommunicationsTxTask(void* parameters) {
	(void)parameters;

	int error = 0;					// error detection
	uint8_t txSlotsRemaining = 0;	// number of frames currently in the transmitter's buffer
	uint8_t txMessageSize = 0;		// size (in bytes) of an outgoing frame
	uint8_t txMessage[TRANCEIVER_TX_MAX_FRAME_SIZE] = { 0 };	// output buffer for messages to be transmitted

	while(1) {
		// Perform transmition operations while in telecommand or file transfer mode
		while (communicationsState.mode) {
			// If we have and ACK/NACK to send back to the ground station
			//		There is not check for mode, because we can have an
			//		ACK/NACK to send to the ground station during
			//		normal telecommand operation or if we have just switched
			//		to file transfer mode. Originally they were seperate
			//		but had duplicated code.
			if (communicationsState.telecommand.transmitReady) {
				// Get the response from the communications state structure
				uint8_t response = communicationsState.telecommand.responseToSend;

				// Send the ACK/NACK response to the transmitter and
				// Mark the message as sent
				error = transceiverSendFrame(&response, 1, &txSlotsRemaining);
				communicationsState.telecommand.transmitReady = 0;

				// TODO: Error check adding the message to the transmitter buffer
			}
			// If we are in file transfer mode and we are approved to transmit a message
			else if (communicationsState.mode == commsModeFileTransfer &&
					communicationsState.fileTransfer.transmitReady) {
					// Received a NACK from ground station, so re-send the last message
				if (communicationsState.fileTransfer.responseReceived == responseNACK){
					// Re-send the last message and mark it as sent for the receiver task
					error = transceiverSendFrame(txMessage, txMessageSize, &txSlotsRemaining);
					communicationsState.telecommand.transmitReady = 0;
				}
				// If we received and ACK, load a new message from the downlink manager and send it
				else {
					// TODO: Get new message and size from downlink manager and send it

					// Send the message and mark it as sent for the receiver task
					error = transceiverSendFrame(txMessage, txMessageSize, &txSlotsRemaining);
					communicationsState.telecommand.transmitReady = responseStateIdle;
				}
			}
			// Pause to allow Receiver Task to run
			vTaskDelay(1);
		}
		vTaskDelay(1);
	}
}


/**
 * Reset the structure for coordinating downlinking and uplinking.
 *
 * Signals the end of a pass. Use with caution.
 *
 * @param comms A locally global struct that holds the data for maintaining comms state.
 */
void communicationsEndPassMode(void) {
	memset(&communicationsState, 0, sizeof(communications_state_t));
}


/**
 * Indicate if the Satellite is currently in a communications mode.
 *
 * @returns 1 (true) if Satellite is uplinking or downlinking; 0 (false) otherwise.
 */
uint8_t communicationsPassModeActive(void) {
	return (communicationsState.mode > commsModeIdle);
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Starts a timer for the max-length pass timeout.
 */
static void startPassMode(void) {

	// if the timer was not created yet, create it
	if (passTimer == NULL) {
		// create the timer; connect it to the callback
		passTimer = xTimerCreate((const signed char *)"passTimer",
								 MAX_PASS_LENGTH,
								 pdFALSE,
								 NULL,
								 resetCommunicationState);

		// start the timer immediately
		xTimerStart(passTimer, 0);
	}

	// otherwise restart it
	else {
		xTimerReset(passTimer, 0);
	}
}


/**
 * Callback function for the pass timer that resets comms to a neutral state.
 *
 * @param timer A handle for a timer.
 */
static void resetCommunicationState(xTimerHandle xTimer) {
	(void)xTimer;
	communicationsEndPassMode();
}




