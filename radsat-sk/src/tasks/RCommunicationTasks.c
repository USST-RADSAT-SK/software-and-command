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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include <string.h>


/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/

/** Maximum possible duration of a pass is 15 minutes; value set in ms. */
#define MAX_PASS_MODE_DURATION	((portTickType)(15*60*1000))

/** Typical duration of quiet mode is 15 minutes; value set in ms. */
#define QUIET_MODE_DURATION		((portTickType)(15*60*1000))

/** Maximum amount of consecutive NACKs before transmission is aborted. */
#define NACK_ERROR_LIMIT		((uint8_t)15)


/** Communication Receive Task delay (in ms). */
#define COMMUNICATION_RX_TASK_DELAY_MS			((portTickType)1)

/** Communication Transmit Task delay (in ms) during typical operation. */
#define COMMUNICATION_TX_TASK_SHORT_DELAY_MS	((portTickType)1)

/**
 * Communication Transmit Task delay (in ms) when the transmitter's buffer is full.
 *
 * Transmission speed: 9600 bps => roughly 1 byte per ms; This delay should long enough to transmit
 * one full frame.
 */
#define COMMUNICATION_TX_TASK_LONG_DELAY_MS		((portTickType)TRANCEIVER_TX_MAX_FRAME_SIZE)


///** Abstraction of the response states */
//typedef enum _response_state_t {
//	responseStateIdle	= 0,	///> Awaiting response from Ground Station
//	responseStateReady	= 1,	///> Ready to transmit to Ground Station
//} response_state_t;
//
//
///** Abstraction of the ACK/NACK return types */
//typedef enum _response_t {
//	responseAck		= protocol_message_Ack_tag,	///> Acknowledge (the message was received properly)
//	responseNack	= protocol_message_Nack_tag,	///> Negative Acknowledge (the message was NOT received properly)
//} response_t;
//
//
///** Abstraction of the communication modes */
//typedef enum _comm_mode_t {
//	commModeQuiet			= -1,	///> Prevent downlink transmissions and automatic state changes
//	commModeIdle			= 0,	///> Not in a pass
//	commModeTelecommand		= 1,	///> Receiving Telecommands from Ground Station
//	commModeFileTransfer	= 2,	///> Transmitting data to the Ground Station
//} comm_mode_t;
//
//
///** Co-ordinates tasks during the telecommand phase */
//typedef struct _telecommand_state_t {
//	response_state_t transmitReady;	///> Whether the Satellite is ready to transmit a response (ACK, NACK, etc.)
//	response_t responseToSend;		///> What response to send, when ready
//} telecommand_state_t;
//
//
///** Co-ordinates tasks during the file transfer phase */
//typedef struct _file_transfer_state_t {
//	response_state_t transmitReady;		///> Whether the Satellite is ready to transmit another Frame (telemetry, etc.)
//	response_t responseReceived;		///> What response was received (ACK, NACK, etc.) regarding the previous message
//	uint8_t transmissionErrors;			///> Error counter for recording consecutive NACKs
//} file_transfer_state_t;
//
//
///** Wrapper structure for communications co-ordination */
//typedef struct _communication_state_t {
//	comm_mode_t mode;					///> The current state of the Communications Tasks
//	telecommand_state_t telecommand;	///> The state during the Telecommand mode
//	file_transfer_state_t fileTransfer;	///> The state during the File Transfer mode
//} communication_state_t;


///** Timer for pass mode */
//static xTimerHandle passTimer;
//
///** Timer for quiet mode */
//static xTimerHandle quietTimer;
//
///** Communication co-orditation structure */
//static communication_state_t state = { 0 };


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void startPassMode(void);
static void endPassMode(void);
static void endPassModeCallback(xTimerHandle timer);

static void startQuietMode(void);
static void endQuietModeCallback(xTimerHandle timer);

static void resetState(void);

static void ceaseTransmission(void);
static void resumeTransmission(void);


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

	uint8_t messageTag = 0;

	while (1) {

		// get the number of frames currently in the receive buffer
		rxFrameCount = 0;
		error = transceiverRxFrameCount(&rxFrameCount);

		// obtain frames when present
		if (rxFrameCount > 0 && error == 0) {

			// obtain new frame from the transceiver
			rxMessageSize = 0;
			memset(rxMessage, 0, sizeof(rxMessage));
			error = transceiverGetFrame(rxMessage, &rxMessageSize);

			// attempt to extract a protocol message
			messageTag = protocolHandle(rxMessage, rxMessageSize);

			// if a protocol message
			if (messageTag != 0){

				// handle protocol
				switch (messageTag) {

					// ack
					case (protocol_message_Ack_tag):
						ackReceived();
						break;

					// nack
					case (protocol_message_Nack_tag):
						nackReceived();
						break;

					default:
						nackReceived();
						break;
				}
			}

			// otherwise, attempt to extract and execute a telecommand message
			else{

				messageTag = telecommandHandle(rxMessage, rxMessageSize);

				// if a valid message
				if (messageTag != 0){

					// handle protocol
					switch (messageTag) {

						// beginPass
						case (telecommand_message_BeginPass_tag):
							beginPass();
							break;

						// beginFileTransfer
						case (telecommand_message_BeginFileTransfer_tag):
							beginFileTransfer();
							break;

						// ceaseTransmission
						case (telecommand_message_CeaseTransmission_tag):
							ceaseTransmission();
							break;

						// ceaseTransmission
						case (telecommand_message_ResumeTransmission_tag):
							resumeTransmission();
							break;

						// updateTime
						case (telecommand_message_UpdateTime_tag):
							updateTime();
							break;

						// reset
						case (telecommand_message_Reset_tag):
							reset();
							break;

						default:
							// todo: should the default be to send a nack?
							sendNack();
							break;
					}
				}
				// if no telecommand message successfully received, send a nack
				else{
					sendNack();
				}
			}
		}
		vTaskDelay(COMMUNICATION_RX_TASK_DELAY_MS);
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

	while (1) {

		txMessageSize = getNextFrame();

		// send the message
		if (txMessageSize > 0)
			error = transceiverSendFrame(txMessage, txMessageSize, &txSlotsRemaining);

		// increase Task delay time when the Transmitter's buffer is full to give it time to transmit
		if (txSlotsRemaining > 0)
			vTaskDelay(COMMUNICATION_TX_TASK_SHORT_DELAY_MS);
		else
			vTaskDelay(COMMUNICATION_TX_TASK_LONG_DELAY_MS);
	}
}


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Indicate if the Satellite is currently in a communications mode.
 *
 * @returns 1 (true) if Satellite is uplinking or downlinking; 0 (false) otherwise.
 */
uint8_t communicationPassModeActive(void) {
	return (state.mode > commModeIdle);
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Starts a timer for the max-length pass timeout.
 */
static void startPassMode(void) {

	// reset any previous communication state, to ensure a fresh start
	resetState();

	// set the mode (telecommand communications are first during a pass)
	state.mode = commModeTelecommand;

	// if the timer was not created yet, create it
	if (passTimer == NULL) {
		// create the timer; connect it to the callback
		passTimer = xTimerCreate((const signed char *)"passTimer",
								 MAX_PASS_MODE_DURATION,
								 pdFALSE,
								 NULL,
								 endPassModeCallback);

		// start the timer immediately
		xTimerStart(passTimer, 0);
	}

	// otherwise simply restart it
	else {
		xTimerReset(passTimer, 0);
	}
}


/**
 * End the pass mode by resetting local variables and temporarily entering quiet mode.
 *
 * Signals the end of a pass, temporarily disabling all transmissions. Use with caution.
 */
void endPassMode(void) {

	// reset the local communication state
	resetState();

	// enter quiet mode (with a timer to exit it)
	startQuietMode();
}


/**
 * Callback function for the pass timer that ends the pass mode.
 *
 * @param timer A handle for a timer.
 */
static void endPassModeCallback(xTimerHandle xTimer) {
	(void)xTimer;
	endPassMode();
}


/**
 * Starts a timer for the max-length pass timeout.
 */
static void startQuietMode(void) {

	// enter quiet mode
	state.mode = commModeQuiet;

	// if the timer was not created yet, create it
	if (quietTimer == NULL) {
		// create the timer; connect it to the callback
		quietTimer = xTimerCreate((const signed char *)"quietTimer",
								  QUIET_MODE_DURATION,
								  pdFALSE,
								  NULL,
								  endQuietModeCallback);

		// start the timer immediately
		xTimerStart(quietTimer, 0);
	}

	// otherwise simply restart it
	else {
		xTimerReset(quietTimer, 0);
	}
}


/**
 * Callback function for the quiet timer that resets comms to a neutral state.
 *
 * @param timer A handle for a timer.
 */
static void endQuietModeCallback(xTimerHandle xTimer) {
	(void)xTimer;
	resetState();
}


/**
 * reset_t the structure for coordinating downlinking and uplinking.
 */
static void resetState(void) {
	memset(&state, 0, sizeof(communication_state_t));
}


/**
 * Forcefully puts the communication Tasks into quiet mode, without an automatic way out.
 *
 * Should only be put here via Telecommand from Ground Station. Only way out is through a
 * subsequent telecommand from the Ground Station (see @sa resumeTransmission).
 */
static void ceaseTransmission(void) {

	// reset the local communication state
	resetState();

	// cancel any communication mode timers that may be running
	xTimerStop(passTimer, 0);
	xTimerStop(quietTimer, 0);

	// enter quiet mode
	state.mode = commModeQuiet;
}


/**
 * Forcefully puts the communication Tasks back into idle mode, ready to continue normal operations.
 *
 * Should only be called via Telecommand from the Ground Station, following a previous Telecommand
 * that had the Satellite cease transmissions (see @sa ceaseTransmission).
 */
static void resumeTransmission(void) {

	// reset the local communication state
	resetState();

	// start a pass mode (clearly are in one if we received this command)
	startPassMode();
}

