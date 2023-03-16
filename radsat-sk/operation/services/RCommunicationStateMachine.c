/**
 * @file RCommunicationStateMachine.c
 * @date August 18, 2022
 * @author Brian Pitzel (brp240)
 */

#include <RCommunicationStateMachine.h>
#include <RCommunicationTasks.h>
#include <RTransceiver.h>
#include <RFileTransferService.h>
#include <RCommon.h>
#include <RObc.h>
#include <RFileTransferService.h>

#include <hal/Timing/Time.h>
#include <string.h>
#include <RPdb.h>
#include <RBattery.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>


#ifdef DEBUG
	static const char* getCommModeName(comm_mode_t mode){
		switch (mode){
		case commModeQuiet:
			return "Quiet";
		case commModePass:
			return "Pass";
		case commModeFileTransfer:
			return "FileX";
		default:
			return "Unknown";
		}
	}


	#undef INFOFMT
	#undef WARNINGFMT
	#undef ERRORFMT
	#undef infoPrint
	#undef warningPrint
	#undef errorPrint
	#define INFOFMT 		" %15s(), %5s Mode: "
	#define WARNINGFMT 		" %s, %d, %s(), %s Mode: "
	#define ERRORFMT 		" %s in function %s() at line %d, %s Mode... \n -> "
	#define infoPrint(fmt, ...) 	printf(INFOTEXT		INFOFMT		fmt "\n", __func__, getCommModeName(currentMode), ##__VA_ARGS__)
	#define warningPrint(fmt, ...) printf(WARNINGTEXT	WARNINGFMT 	fmt "\n", __FILENAME__, __LINE__, __func__, getCommModeName(currentMode), ##__VA_ARGS__)
	#define errorPrint(fmt, ...) 	printf(ERRORTEXT	ERRORFMT 	fmt "\n", __FILE__, __func__, __LINE__, getCommModeName(currentMode), ##__VA_ARGS__)
#endif
/***************************************************************************************************
                                   DEFINITIONS & PRIVATE GLOBALS
***************************************************************************************************/
static void passTimeoutCallback(xTimerHandle xTimer);
typedef enum _fileTransferModeStatus{
	newSession,
	oldSession
} fileTransferModeStatus_t;

/** Timer for pass mode */
static xTimerHandle passTimer;
static comm_mode_t currentMode = commModeQuiet;
static fileTransferModeStatus_t fileTransferSessionStatus = newSession;


/**
 * If no telecommand has been successfully received, send a nack down
 */
void sendNack(void) {
	uint8_t txSlotsRemaining = 0;
	uint8_t message[MAX_MESSAGE_SIZE] = { 0 };
	uint8_t size = protocolGenerate(commandNack, message);
	transceiverSendFrame(message, size, &txSlotsRemaining);
	infoPrint("Nack Sent.");
}


/**
 * If no telecommand has been successfully received, send a nack down
 */
void sendAck(void) {
	uint8_t txSlotsRemaining = 0;
	uint8_t message[MAX_MESSAGE_SIZE] = { 0 };
	uint8_t size = protocolGenerate(commandAck, message);
	transceiverSendFrame(message, size, &txSlotsRemaining);
	infoPrint("Ack Sent.");
}


int setMode(comm_mode_t mode, uint32_t passTime);

comm_mode_t getMode(void){
	return currentMode;
}


/**
 * handles all setmode functions and timer starting and ending.
 *
 * @param Communication mode to be set.
 * @param Time in seconds to start pass mode for. 0 defaults to MAX_PASS_MODE_DURATION.
 */
int setMode(comm_mode_t mode, uint32_t passTime) {
	portTickType actualPassTime = ((portTickType)passTime) * 1000 * portTICK_RATE_MS;

	if (((uint32_t)actualPassTime) > MAX_PASS_MODE_DURATION || actualPassTime == 0) {
		actualPassTime = MAX_PASS_MODE_DURATION;
	}

	// if the timer was not created yet, create it
	if (passTimer == NULL) {
		// create the timer; connect it to the callback
		passTimer = xTimerCreate((const signed char *)"passTimer",
								actualPassTime,
								pdFALSE,
								NULL,
								passTimeoutCallback);
		// Should never leave commModeQuiet if a timer can't be made because because the timer is the fail safe.
		if (passTimer == pdFAIL){
			errorPrint("Failed to create Timer");
			stopInterFrameFill();
			currentMode = commModeQuiet;
			return -90; // TODO: add error code for timer error
		}
		infoPrint("Pass Timer Initialized");
	}

	if (mode == commModeQuiet){
		stopInterFrameFill();
		currentMode = commModeQuiet;
		xTimerStop(passTimer, 0);
		infoPrint("Setting mode to Quiet mode");
	} else {
		if (xTimerIsTimerActive(passTimer) == pdFALSE){
			// set and start the timer immediately
			int SetPeriodError = xTimerChangePeriod(passTimer, actualPassTime,  0);
			// Should never leave commModeQuiet if a timer can't be started because because the timer is the fail safe.
			if (SetPeriodError == pdFAIL){
				errorPrint("Failed to start the timer. Mode mode set to Quiet");
				stopInterFrameFill();
				currentMode = commModeQuiet;
				return -90; // TODO: add error code for timer error
			}
			infoPrint("Pass timer started set to %lus", actualPassTime / portTICK_RATE_MS / 1000);
		}
		if (mode == commModePass){
			startInterFrameFill();
			currentMode = commModePass;
			infoPrint("Setting mode to Pass mode");
		} else if (currentMode == commModePass && mode == commModeFileTransfer){
			startInterFrameFill();
			currentMode = commModeFileTransfer;
			infoPrint("Setting mode to File Transfer mode");
		}
	}
	return 0;
}


/**
 * Callback function for the pass timer that ends the pass mode.
 *
 * @param timer A handle for a timer.
 */
static void passTimeoutCallback(xTimerHandle xTimer) {
	(void)xTimer;
	setMode(commModeQuiet, 0);
}

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Forcefully puts the state machine into quiet mode, without an automatic way out.
 *
 * Should only be put here via Telecommand from Ground Station. Only way out is through a
 * subsequent telecommand from the Ground Station (see @sa resumeTransmission).
 */
void ceaseTransmission(void) {
	setMode(commModeQuiet, 0);
}

/**
 * Updates the time on the satellite state machine after receiving the updateTime command from the ground station.
 */
int updateTime(uint32_t epochTime) {
	infoPrint("Setting time...");
	debugPrint("Before: ");
	printTime();
	debugPrint(" \n");
	int error = Time_setUnixEpoch(epochTime);
	if (error)
		errorPrint("Error Setting time: error= %d, most likely invalid input.", error);
	debugPrint("After: ");
	printTime();
	debugPrint(" \n");
	return error;
}



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

void processFileTransfer(commandType_t type){
	infoPrint("FProcess file tx");
	int size = 0;
	uint8_t txSlotsRemaining = 0;
	uint8_t messageContainer[TRANCEIVER_TX_MAX_FRAME_SIZE] = { 0 };
	switch (type) {
	case commandAck:
		if (fileTransferSessionStatus == newSession){
			size = fileTransferCurrentFrame(messageContainer);
		} else {
			size = fileTransferNextFrame(messageContainer);
		}
		mark
		infoPrint("File Transfer Ack received.");
		mark
		break;
	case commandNack:
		size = fileTransferCurrentFrame(messageContainer);
		infoPrint("File Transfer Nack received.");
		break;
	case commandBeginPass:
		fileTransferSessionStatus = newSession;
		setMode(commModePass, 0);
		sendAck();
		return;
	default:
		return;
	}
	mark
	if (size == 0){
		warningPrint("File Transfer size = 0...");
		sendNack();
		fileTransferSessionStatus = newSession;
		setMode(commModePass, 0);
		return;
	}
	mark
	transceiverSendFrame(messageContainer, size, &txSlotsRemaining);
	mark
	fileTransferSessionStatus = oldSession;
	mark
	return;
}


void processPassMode(commandType_t type, messageSubject_t* content){
	switch (type) {
	case commandCeaseTransmission:
		ceaseTransmission();
		return;
	case commandBeginPass:
		setMode(commModePass, content->BeginPass.passLength);
		sendAck();
		return;
	case commandBeginFileTransfer:
		setMode(commModeFileTransfer, 0);
		sendAck();
		return;
	case commandUpdateTime:
		infoPrint("Time = %lu\n", content->UpdateTime.unixTime);
		updateTime(content->UpdateTime.unixTime);
		sendAck();
		return;
	case commandReset:
		//resetCommand(content->Reset.device, content->Reset.hard);
		sendAck();
		return;
	case commandUnknownCommand:
		sendNack();
		return;
	case commandAck:
		sendNack();
		return;
	case commandNack:
		sendNack();
		return;
	case commandProtoUnwrapError:
		sendNack();
		return;
	case commandGeneralError:
		return;
	default:
		break;
	}
	return;
}


void processQuiet(commandType_t type, messageSubject_t* content){
	if (type == commandBeginPass){
		setMode(commModePass, content->BeginPass.passLength);
		sendAck();
	} else {
		vTaskDelay( COMMS_QUIET_DELAY );
	}
	return;
}


void processCommand(commandType_t type, messageSubject_t* content){
	if (type == commandCeaseTransmission){
		setMode(commModeQuiet, 0);
		return;
	} else if (type == commandReset){
		switch (content->Reset.device){
		case reset_device_obc:
			warningPrint("Resetting OBC hard");
			powerCycleIobc();
			return;
		case reset_device_trxvu:
			if (content->Reset.resetType) {
				warningPrint("Resetting TRXVU soft");
				transceiverSoftReset();
			}
			else {
				warningPrint("Resetting TRXVU hard");
				transceiverPowerCycle();
			}
			return;
		case reset_device_antenna:
			return;
		case reset_device_eps:
			if (content->Reset.resetType) {
				warningPrint("Resetting PDB soft");
				pdbReset();
			}
			else {
				warningPrint("Resetting PDB hard");
				pdbResetSat();
			}
			return;
		case reset_device_battery:
			warningPrint("Resetting battery");
			batteryReset();
			return;
		case reset_device_fram:
			warningPrint("Resetting FRAM");
			fileTransferReset();
			return;
		}

	}
	infoPrint("%d", getMode());
	switch (getMode()){
	case commModeQuiet:
		processQuiet(type, content);
		return;
	case commModePass:
		processPassMode(type, content);
		return;
	case commModeFileTransfer:
		mark
		processFileTransfer(type);
		mark
		break;
	}
	mark
}







