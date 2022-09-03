/**
 * @file RCommunicationStateMachine.c
 * @date August 18, 2022
 * @author Brian Pitzel (brp240)
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

/** Abstraction of the response states */
typedef enum _response_state_t {
	responseStateIdle	= 0,	///> Awaiting response from Ground Station
	responseStateReady	= 1,	///> Ready to transmit to Ground Station
} response_state_t;


/** Abstraction of the ACK/NACK return types */
typedef enum _response_t {
	responseAck		= protocol_message_Ack_tag,	///> Acknowledge (the message was received properly)
	responseNack	= protocol_message_Nack_tag,	///> Negative Acknowledge (the message was NOT received properly)
} response_t;


/** Abstraction of the communication modes */
typedef enum _comm_mode_t {
	commModeQuiet			= -1,	///> Prevent downlink transmissions and automatic state changes
	commModeIdle			= 0,	///> Not in a pass
	commModeTelecommand		= 1,	///> Receiving Telecommands from Ground Station
	commModeFileTransfer	= 2,	///> Transmitting data to the Ground Station
} comm_mode_t;


/** Co-ordinates tasks during the telecommand phase */
typedef struct _telecommand_state_t {
	response_state_t transmitReady;	///> Whether the Satellite is ready to transmit a response (ACK, NACK, etc.)
	response_t responseToSend;		///> What response to send, when ready
} telecommand_state_t;


/** Co-ordinates tasks during the file transfer phase */
typedef struct _file_transfer_state_t {
	response_state_t transmitReady;		///> Whether the Satellite is ready to transmit another Frame (telemetry, etc.)
	response_t responseReceived;		///> What response was received (ACK, NACK, etc.) regarding the previous message
	uint8_t transmissionErrors;			///> Error counter for recording consecutive NACKs
} file_transfer_state_t;


/** Wrapper structure for communications co-ordination */
typedef struct _communication_state_t {
	comm_mode_t mode;					///> The current state of the Communications Tasks
	telecommand_state_t telecommand;	///> The state during the Telecommand mode
	file_transfer_state_t fileTransfer;	///> The state during the File Transfer mode
} communication_state_t;

/** Timer for pass mode */
static xTimerHandle passTimer;

/** Timer for quiet mode */
static xTimerHandle quietTimer;

/** Communication co-ordination structure */
static communication_state_t state = { 0 };


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Updates the state machine after receiving an ack message from the ground station.
 * Update response received to "ack", and set file transfer status to "ready".
 *
 */
void ackReceived(void) {
	// the only time this function should be called is when we are in fileTransferMode. the
	// if statement might be redundant
	if (state.mode == commModeFileTransfer && !state.fileTransfer.transmitReady){
		state.fileTransfer.responseReceived = responseAck;
		state.fileTransfer.transmitReady = responseStateReady;
	}
}

/**
 * Updates the state machine after receiving an nack message from the ground station.
 * Update response received to "nack", and set file transfer status to "ready".
 */
void nackReceived(void) {
	// the only time this function should be called is when we are in fileTransferMode. the
	// if statement might be redundant
	if (state.mode == commModeFileTransfer && !state.fileTransfer.transmitReady){
		state.fileTransfer.responseReceived = responseNack;
		state.fileTransfer.transmitReady = responseStateReady;
	}
}

/**
 * Updates the state machine after receiving a beginPass command from the ground station.
 * Resets the state, updates mode to telecommand, begins the pass timer and readys an "ack" to send down.
 */
void beginPass(void) {
	if ((state.mode == commModeTelecommand || state.mode == commModeIdle)
	&& (!state.telecommand.transmitReady)){
		startPassMode();

		// send down the ack
		state.telecommand.transmitReady = responseStateReady;
		state.telecommand.responseToSend = responseAck;
	}
}

/**
 * Updates the state machine after receiving a beginFileTransfer command from the ground station.
 * Update response received to "nack", and set file transfer status to "ready".
 */
void beginFileTransfer(void) {
	if (state.mode == commModeTelecommand){
		state.mode = commModeFileTransfer;

		// todo: should we be sending an ack after begin file transfer? This is a confusing part.
		// there will be collisions between state.telecommand (for sending ack) and state.fileTransfer
		// (which we need to be in for beginFileTransfer)
	}
}

/**
 * Forcefully puts the state machine into quiet mode, without an automatic way out.
 *
 * Should only be put here via Telecommand from Ground Station. Only way out is through a
 * subsequent telecommand from the Ground Station (see @sa resumeTransmission).
 */
void ceaseTransmission(void) {

	// reset the local communication state
	resetState();

	// cancel any communication mode timers that may be running
	xTimerStop(passTimer, 0);
	xTimerStop(quietTimer, 0);

	// enter quiet mode
	state.mode = commModeQuiet;
}

/**
 * Forcefully puts the state machine back into idle mode, ready to continue normal operations.
 *
 * Should only be called via Telecommand from the Ground Station, following a previous Telecommand
 * that had the Satellite cease transmissions (see @sa ceaseTransmission).
 */
void resumeTransmission(void) {

	// reset the local communication state
	resetState();

	// start a pass mode (clearly are in one if we received this command)
	startPassMode();
}

/**
 * Updates the time on the satellite state machine after receiving the updateTime command from the ground station.
 */
void updateTime(void) {
	if (state.mode == commModeTelecommand){
		// todo: implement updateTime

		// send down the ack
		state.telecommand.transmitReady = responseStateReady;
		state.telecommand.responseToSend = responseAck;
	}
}

/**
 * Forces a hard reset of the satellite after receiving a reset command from the ground station
 */
void reset(void) {
	if (state.mode == commModeTelecommand){
		// todo: implement reset

		// send down the ack
		state.telecommand.transmitReady = responseStateReady;
		state.telecommand.responseToSend = responseAck;
	}
}

/**
 * If no telecommand has been successfully received, send a nack down
 */
void sendNack(void) {
	if (state.mode == commModeTelecommand){
		state.telecommand.transmitReady = responseStateReady;
		state.telecommand.responseToSend = responseNack;
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

/**
 * Based on the current state of the machine, obtain the appropriate next frame to transmit
 *
 * @returns the size of the next frame to transmit; 0 if failure.
 * @param txMessage: a pointer to a buffer of uint8_t's to store the frame in
 *
 */

uint8_t getNextFrame(uint8_t *txMessage) {

	uint8_t txMessageSize;

	// if we are in telecommand mode:
	if(state.mode == commModeTelecommand && state.telecommand.transmitReady == responseStateReady){

		// serialize the ack/nack response to be sent
		txMessageSize = protocolGenerate(state.telecommand.responseToSend, txMessage);

		// prepare to receive next message
		if (txMessageSize > 0)
			state.telecommand.transmitReady = responseStateIdle;

		return txMessageSize;

		}

	// if we are in Idle mode:
	if(state.mode == commModeIdle || state.mode == commModeQuiet){
		// no message to send
		return 0;
	}

	// if we are in FileTransfer mode:
	if(state.mode == commModeFileTransfer && state.fileTransfer.transmitReady == responseStateReady){

		// ACK received from ground Station; obtain next message and send it
		if (state.fileTransfer.responseReceived == responseAck) {

			// clear transmission error counter
			state.fileTransfer.transmissionErrors = 0;

			// obtain new message and size from File Transfer Service
			txMessageSize = fileTransferNextFrame(txMessage);

			/** todo: this part needs to be looked over more closely
			 *
			 * // prepare to receive ACK/NACK
			 * if (txMessageSize > 0)
			 * 	state.telecommand.transmitReady = responseStateIdle;
			 * // force NACK in order to resend the packet
			 * else
			 *	state.fileTransfer.responseReceived = responseNack;
			 */

			return txMessageSize;
		}

		// NACK received from ground Station; re-send the previous message
		else {

			// record the NACK
			state.fileTransfer.transmissionErrors++;

			// abort transmission if the error limit is exceeded
			if (state.fileTransfer.transmissionErrors > NACK_ERROR_LIMIT)
				endPassMode();

			// todo: return the same message that was just returned

			// prepare to receive ACK/NACK
				if (error == 0)
					state.telecommand.transmitReady = responseStateIdle;
		}
	}

	// todo: if we are in ImageTransfer mode:
	// if(state.mode == commModeImageTransfer && state.imageTransfer.transmitReady == responseStateReady){}

	// todo: default/no return state:
	//if (state.mode == default)
	//	return 0;
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
static void endPassMode(void) {

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

