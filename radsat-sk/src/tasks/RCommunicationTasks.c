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
typedef enum _comm_mode_t {
	commsModeIdle			= 0,	// Not in a pass
	commsModeTelecommand	= 1,	// Receiving Telecommands from Ground Station
	commsModeFileTransfer	= 2,	// Transmitting data to the Ground Station
} comm_mode_t;


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
typedef struct _communication_state_t {
	uint8_t mode;						// The current state of the Communications Tasks
	telecommand_state_t telecommand;	// The state during the Telecommand mode
	fileTransfer_state_t fileTransfer;	// The state during the File Transfer mode
} communication_state_t;


/** Instantiate the timer for pass time */
static xTimerHandle passTimer;
/** Instantiate the communication co-orditation structure */
static communication_state_t state = { 0 };


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
void communicationRxTask(void* parameters) {
	(void)parameters;

	int error = 0;				// error detection
	uint16_t rxFrameCount = 0;	// number of frames currently in the receiver's buffer
	uint16_t rxMessageSize = 0;	// size (in bytes) of a received frame
	uint8_t rxMessage[TRANCEIVER_RX_MAX_FRAME_SIZE] = { 0 };	// input buffer for received frames

	while (1) {

		// get the number of frames currently in the receive buffer
		rxFrameCount = 0;
		error = transceiverRxFrameCount(&rxFrameCount);

		// handle frames accordingly
		if (rxFrameCount > 0) {

			// obtain new frame from the transceiver
			rxMessageSize = 0;
			memset(rxMessage, 0, sizeof(rxMessage));
			error = transceiverGetFrame(rxMessage, &rxMessageSize);

			// transition out of idle mode and into pass mode
			if (state.mode == commsModeIdle) {

				// enter telecommand (and pass) mode, and start a pass timer
				startPassMode();
			}

			// telecommand mode, awaiting the next telecommand from the Ground Station
			if (state.mode == commsModeTelecommand && !state.telecommand.transmitReady) {

				// TODO: forward message to command centre and determine ACK/NACK response to send
				// TODO: determine if this message was signalling the end of telecommand mode
				response_t response = responseACK;
				int endOfTelecommandMode = 1;

				// prepare for file transfer mode (if necessary)
				if (endOfTelecommandMode)
					state.mode = commsModeFileTransfer;

				// prepare to send ACK/NACK response
				state.telecommand.responseToSend = response;
				state.telecommand.transmitReady = responseStateReady;
			}

			// file transfer mode, awaiting ACK/NACK from the Ground Station
			else if (state.mode == commsModeFileTransfer && !state.fileTransfer.transmitReady) {

				// TODO: forward message to command centre and extract the received ACK/NACK response
				response_t response = responseACK;

				// prepare to send subsequent (or resend previous) file transfer frame
				state.fileTransfer.responseReceived = response;
				state.fileTransfer.transmitReady = responseStateReady;
			}
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
void communicationTxTask(void* parameters) {
	(void)parameters;

	int error = 0;					// error detection
	uint8_t txSlotsRemaining = 0;	// number of frames currently in the transmitter's buffer
	uint8_t txMessageSize = 0;		// size (in bytes) of an outgoing frame
	uint8_t txMessage[TRANCEIVER_TX_MAX_FRAME_SIZE] = { 0 };	// output buffer for messages to be transmitted

	while (1) {

		// pass mode is active
		if (communicationPassModeActive()) {

			// telecommand mode (or leaving it), ready to send ACK/NACK to the Ground Station
			if (state.telecommand.transmitReady) {

				// TODO: serialize the ACK/NACK response to be sent
				uint8_t response = state.telecommand.responseToSend;
				uint8_t responseSize = sizeof(response);

				// send the message
				error = transceiverSendFrame(&response, responseSize, &txSlotsRemaining);
				state.telecommand.transmitReady = responseStateIdle;
			}

			// file transfer mode, ready to transmit a message
			else if (state.mode == commsModeFileTransfer && state.fileTransfer.transmitReady) {

				// NACK received from ground Station; re-send the previous message
				if (state.fileTransfer.responseReceived == responseNACK) {
					error = transceiverSendFrame(txMessage, txMessageSize, &txSlotsRemaining);
					state.telecommand.transmitReady = responseStateIdle;
				}

				// ACK received from ground Station; obtain next message and send it
				else {
					// TODO: obtain new message and size from downlink manager

					// send the message
					error = transceiverSendFrame(txMessage, txMessageSize, &txSlotsRemaining);
					state.telecommand.transmitReady = responseStateIdle;
				}
			}
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
void communicationEndPassMode(void) {
	memset(&state, 0, sizeof(communication_state_t));
}


/**
 * Indicate if the Satellite is currently in a communications mode.
 *
 * @returns 1 (true) if Satellite is uplinking or downlinking; 0 (false) otherwise.
 */
uint8_t communicationPassModeActive(void) {
	return (state.mode > commsModeIdle);
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Starts a timer for the max-length pass timeout.
 */
static void startPassMode(void) {

	// set the mode (telecommand communications are first)
	state.mode = commsModeTelecommand;

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

	// otherwise simply restart it
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
	communicationEndPassMode();
}

