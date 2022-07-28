/**
 * @file main.c
 */

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <hal/Timing/WatchDogTimer.h>
#include <hal/Timing/Time.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/I2C.h>

#include <RMessage.h>

#include <hal/boolean.h>
#include <hal/Utility/util.h>
#include <hal/version/version.h>

#include <at91/peripherals/cp15/cp15.h>
#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <stdlib.h>

#include <RDebug.h>
#include <RFram.h>
#include <RI2c.h>
#include <RUart.h>

#include <RTransceiver.h>
#include <RCommon.h>

#include <RCommunicationTasks.h>
#include <RDosimeterCollectionTask.h>
#include <RImageCaptureTask.h>
#include <RAdcsCaptureTask.h>
#include <RTelemetryCollectionTask.h>
#include <RSatelliteWatchdogTask.h>


#include <satellite-subsystems/IsisTRXVU.h>


/***************************************************************************************************
                                   DEFINITIONS AND PRIVATE GLOBALS
***************************************************************************************************/

/** How often the internal OBC Watchdog is kicked (i.e. pet, i.e. reset) in ms. */
#define OBC_WDOG_KICK_PERIOD_MS	(15 / portTICK_RATE_MS)

/** Default stack size (in bytes) allotted to each FreeRTOS Task. */
#define DEFAULT_TASK_STACK_SIZE	(4096)

/** FreeRTOS Task Handles. */
static xTaskHandle missionInitTaskHandle;
static xTaskHandle communicationRxTaskHandle;
static xTaskHandle communicationTxTaskHandle;
static xTaskHandle dosimeterCollectionTaskHandle;
//static xTaskHandle imageCaptureTaskHandle;
//static xTaskHandle adcsCaptureTaskHandle;
//static xTaskHandle telemetryCollectionTaskHandle;
static xTaskHandle satelliteWatchdogTaskHandle;

/** Communication Transmit Task Priority. Downlinks messages when necessary; very high priority task. */
static const int communicationTxTaskPriority = configMAX_PRIORITIES - 1;
/** Communication Receive Task Priority. Constantly listening for messages; high priority task. */
static const int communicationRxTaskPriority = configMAX_PRIORITIES - 2;

/** Dosimeter Collection Task Priority. Periodically collects payload data; medium priority task. */
static const int dosimeterCollectionTaskPriority = configMAX_PRIORITIES - 3;
/** Image Capture Task Priority. Periodically collects image data; medium priority task. */
//static const int imageCaptureTaskPriority = configMAX_PRIORITIES - 3;
/** ADCS Capture Task Priority. Periodically collects ADCS data; medium priority task. */
//static const int adcsCaptureTaskPriority = configMAX_PRIORITIES - 3;

/** Telemetry Collection Task Priority. Periodically collects satellite telemetry; low priority task. */
//static const int telemetryCollectionTaskPriority = configMAX_PRIORITIES - 4;
/** Satellite Watchdog Task Priority. Routinely pets (resets) satellite subsystem watchdogs; low priority task. */
static const int satelliteWatchdogTaskPriority = configMAX_PRIORITIES - 4;
/** Mission Init Task Priority. Does initializations that need to be ran post-scheduler; low priority task. */
static const int missionInitTaskPriority = configMAX_PRIORITIES - 4;


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static int initBoard(void);
static int initDrivers(void);
static int initTime(void);
static int initSubsystems(void);

static int initObcWatchdog(void);
static int initMissionTasks(void);

void MissionInitTask(void* parameters);


/***************************************************************************************************
                                                MAIN
***************************************************************************************************/

/**
 * The main application entry point.
 * @return Never returns.
 */
int main(void) {

	// initialize internal OBC board settings
	int error = initBoard();

	if (error != SUCCESS) {
		debugPrint("main(): failed during system initialization.\n");
		// TODO: report to system manager
	}

#ifdef TEST

	// TODO: run tests

#else	/* TEST */

	// TODO: Antenna Diagnostic & Deployment (if necessary)

	// TODO: Satellite Diagnostic Check (if applicable - may be done later instead)

	// initialize the Mission Initialization Task
	error = xTaskCreate(MissionInitTask,
						(const signed char*)"Mission Initialization Task",
						DEFAULT_TASK_STACK_SIZE,
						NULL,
						missionInitTaskPriority,
						&missionInitTaskHandle);

	if (error != pdPASS) {
		debugPrint("main(): failed to create MissionInitTask.\n");
		// TODO: report to system manager
	}

#endif	/* TEST */

	// start the FreeRTOS Scheduler - NEVER GETS PAST THIS LINE
	vTaskStartScheduler();

	debugPrint("main(): failed to start the FreeRTOS Scheduler.\n");

	// should never get here
	exit(0);
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Initialize low-level MCU/OBC configuration settings.
 */
static int initBoard(void) {

	// Enable the Instruction cache of the ARM9 core. Keep the MMU and Data Cache disabled.
	CP15_Enable_I_Cache();

	return SUCCESS;
}


/**
 * Initialize the low-level peripheral drivers used on the OBC.
 */
static int initDrivers(void) {

	int error = SUCCESS;

	// initialize the Auxillary Camera UART port for communication with Camera
	error = uartInit(UART_CAMERA_BUS);
	if (error != SUCCESS) {
		debugPrint("initDriver(): failed to initialize Camera UART.\n");
		return error;
	}

	// initialize the FRAM memory module for safe persistent storage
	error = framInit();
	if (error != SUCCESS) {
		debugPrint("initDriver(): failed to initialize FRAM.\n");
		return error;
	}

	// initialize the I2C bus for general inter-component communication
	error = i2cInit();
	if (error != SUCCESS) {
		debugPrint("initDriver(): failed to initialize I2C.\n");
		return error;
	}

	return error;
}


/**
 * Initialize the RTC and RTT, setting the default time in the process.
 */
static int initTime(void) {

	int error = SUCCESS;

	// create Time struct with default times (estimated Launch date)
	Time time = { 0 };
	time.year = 22;		// 2022
	time.month = 8;		// August
	time.date = 1;		// the 1st
	time.day = 2;		// Monday
	time.hours = 12;	// 12:00:00
	time.minutes = 0;	// 12:00:00
	time.seconds = 0;	// 12:00:00

	// the time (in seconds) between RTC and RTT synchronizations
	const unsigned int syncInterval = 120;

	// initilize the RTC and RTT and set the default time
	error = Time_start(&time, syncInterval);
	if (error != SUCCESS)
		debugPrint("initTime(): failed to initialize RTC and RTT.\n");

	return error;
}


/**
 * Initialize external subsystem modules and the Satellite Subsystem Interface library.
 */
static int initSubsystems(void) {

	int error = SUCCESS;

	// initialize the Transceiver module
	error = transceiverInit();
	if (error != SUCCESS) {
		debugPrint("initSubsystems(): failed to initialize Transceiver subsystem.\n");
		return error;
	}

	// TODO: initialize the other subsystems that require explicit initialization

	return error;
}


/**
 * Initialize a low-priority task that automatically resets the OBC's internal Watchdog.
 *
 * On all builds except for Release, this will also enable an LED that blink every time that the
 * Watchdog timer is reset, allowing for a simple heartbeat to show proper OBC operation.
 *
 * @note This also initializes the Watchdog API.
 */
static int initObcWatchdog(void) {

	int error = SUCCESS;

#ifdef RELEASE
	error = WDT_startWatchdogKickTask(OBC_WDOG_KICK_PERIOD_MS, FALSE);
#else
	error = WDT_startWatchdogKickTask(OBC_WDOG_KICK_PERIOD_MS, TRUE);
#endif

	if (error != SUCCESS)
		debugPrint("initObcWatchdog(): failed to start background OBC WDOG task.\n");

	return error;
}


/**
 * Initialize all of the FreeRTOS tasks used during typical mission operation.
 */
static int initMissionTasks(void) {

	int error = pdPASS;

	// initialize the Communication Receive Task
	error = xTaskCreate(CommunicationRxTask,
						(const signed char*)"Communication Receive Task",
						DEFAULT_TASK_STACK_SIZE,
						NULL,
						communicationRxTaskPriority,
						&communicationRxTaskHandle);

	if (error != pdPASS) {
		debugPrint("initMissionTasks(): failed to create CommunicationRxTask.\n");
		return E_GENERIC;
	}

	// initialize the Communication Transmit Task
	error = xTaskCreate(CommunicationTxTask,
						(const signed char*)"Communication Transmit Task",
						DEFAULT_TASK_STACK_SIZE,
						NULL,
						communicationTxTaskPriority,
						&communicationTxTaskHandle);

	if (error != pdPASS) {
		debugPrint("initMissionTasks(): failed to create CommunicationTxTask.\n");
		return E_GENERIC;
	}

	// initialize the Dosimeter Collection Task
	error = xTaskCreate(DosimeterCollectionTask,
						(const signed char*)"Dosimeter Collection Task",
						DEFAULT_TASK_STACK_SIZE,
						NULL,
						dosimeterCollectionTaskPriority,
						&dosimeterCollectionTaskHandle);

	if (error != pdPASS) {
		debugPrint("initMissionTasks(): failed to create DosimeterCollectionTask.\n");
		return E_GENERIC;
	}
/*
	// initialize the Image Capture Task
	error = xTaskCreate(ImageCaptureTask,
						(const signed char*)"Image Capture Task",
						DEFAULT_TASK_STACK_SIZE,
						NULL,
						imageCaptureTaskPriority,
						&imageCaptureTaskHandle);

	if (error != pdPASS) {
		debugPrint("initMissionTasks(): failed to create ImageCaptureTask.\n");
		return E_GENERIC;
	}

	// initialize the ADCS Capture Task
	error = xTaskCreate(AdcsCaptureTask,
						(const signed char*)"ADCS Capture Task",
						DEFAULT_TASK_STACK_SIZE,
						NULL,
						adcsCaptureTaskPriority,
						&adcsCaptureTaskHandle);

	if (error != pdPASS) {
		debugPrint("initMissionTasks(): failed to create AdcsCaptureTask.\n");
		return E_GENERIC;
	}

	// initialize the Telemetry Collection Task
	error = xTaskCreate(TelemetryCollectionTask,
						(const signed char*)"Telemetry Collection Task",
						DEFAULT_TASK_STACK_SIZE,
						NULL,
						telemetryCollectionTaskPriority,
						&telemetryCollectionTaskHandle);

	if (error != pdPASS) {
		debugPrint("initMissionTasks(): failed to create TelemetryCollectionTask.\n");
		return E_GENERIC;
	}
*/
	// initialize the Satellite Watchdog Task
	error = xTaskCreate(SatelliteWatchdogTask,
						(const signed char*)"Satellite Watchdog Task",
						DEFAULT_TASK_STACK_SIZE,
						NULL,
						satelliteWatchdogTaskPriority,
						&satelliteWatchdogTaskHandle);

	if (error != pdPASS) {
		debugPrint("initMissionTasks(): failed to create SatelliteWatchdogTask.\n");
		return E_GENERIC;
	}

	return SUCCESS;
}


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

/**
 * Initialize FreeRTOS Tasks and other mission related modules.
 *
 * This functionality was placed into a FreeRTOS Task as some of the functionality and
 * initializations require the FreeRTOS Scheduler to already be running.
 *
 * @param parameters
 */
void MissionInitTask(void* parameters) {

	// ignore the input parameter
	(void)parameters;

	int error = SUCCESS;

	// initialize the Hardware Abstraction Library (HAL) drivers
	error = initDrivers();
	if (error != SUCCESS) {
		// TODO: report to system manager
		debugPrint("MissionInitTask(): failed to initialize Drivers.\n");
	}
	vTaskDelay(1000);
	//uint8_t command[] = {0x32, 0x20, 0x5E, 0x98, 0x02};

	//uint8_t command[] = { };
	//uint8_t command[] = {0x32, 0x00, 0x06, 0x4a, 0xc8};
	//error = I2C_write(TRANSCEIVER_TX_I2C_SLAVE_ADDR, command, 5);
	//if (error) printf("%d\n", error);
	//i2cTransmit(TRANSCEIVER_TX_I2C_SLAVE_ADDR, command, 5);
	//uint8_t commandRX[] = {0x32, 0xA6, 0x39, 0x02, 0x00};
	//I2C_write(TRANSCEIVER_RX_I2C_SLAVE_ADDR, commandRX, 5);
	//i2cTransmit(TRANSCEIVER_RX_I2C_SLAVE_ADDR, commandRX, 5);
	//if (error) printf("%d\n", error);

	// initialize external components and the Satellite Subsystem Interface (SSI)
	error = initSubsystems();
	if (error != SUCCESS) {
		// TODO: report to system manager
		debugPrint("MissionInitTask(): failed to initialize Subsystems.\n");
	}

	// initialize the internal OBC watchdog, and start a task that automatically pets it
	error = initObcWatchdog();
	if (error != SUCCESS) {
		// TODO: report to system manager
		debugPrint("MissionInitTask(): failed to initialize Obc Watchdog.\n");
	}

	// initialize the RTC and RTT to the default time
	error = initTime();
	if (error != SUCCESS) {
		// TODO: report to system manager
		debugPrint("MissionInitTask(): failed to initialize the time.\n");
	}
	// = 40ADF319

	/*
	    camera_telemetry tempcam = {0};
		tempcam.uptime = 100;
		file_transfer_message temptx = {0};
		temptx.which_message = file_transfer_message_CameraTelemetry_tag;
		temptx.CameraTelemetry = tempcam;
		radsat_message* tempraw = {0};
		tempraw.which_service = radsat_message_FileTransferMessage_tag;
		tempraw.FileTransferMessage = temptx;
		uint8_t* wrappedMessage = {};
		uint8_t messageSize;
		messageSize = messageWrap(radsat_message* tempraw, uint8_t* wrappedMessage);
		*/

	/* protocolGenerate testing */


//	#define TRANCEIVER_TX_MAX_FRAME_SIZE 235
//	#define RADSAT_SK_HEADER_SIZE (sizeof(radsat_sk_header_t))
//
//	/** Abstraction of the response states */
//	typedef enum _response_state_t {
//		responseStateIdle	= 0,	///> Awaiting response from Ground Station
//		responseStateReady	= 1,	///> Ready to transmit to Ground Station
//	} response_state_t;
//
//	/** Abstraction of the ACK/NACK return types */
//	typedef enum _response_t {
//		responseAck		= protocol_message_Ack_tag,	///> Acknowledge (the message was received properly)
//		responseNack	= protocol_message_Nack_tag,	///> Negative Acknowledge (the message was NOT received properly)
//	} response_t;
//
//	/** Abstraction of the communication modes */
//	typedef enum _comm_mode_t {
//		commModeQuiet			= -1,	///> Prevent downlink transmissions and automatic state changes
//		commModeIdle			= 0,	///> Not in a pass
//		commModeTelecommand		= 1,	///> Receiving Telecommands from Ground Station
//		commModeFileTransfer	= 2,	///> Transmitting data to the Ground Station
//	} comm_mode_t;
//
//	/** Co-ordinates tasks during the telecommand phase */
//	typedef struct _telecommand_state_t {
//		response_state_t transmitReady;	///> Whether the Satellite is ready to transmit a response (ACK, NACK, etc.)
//		response_t responseToSend;		///> What response to send, when ready
//	} telecommand_state_t;
//
//	/** Co-ordinates tasks during the file transfer phase */
//	typedef struct _file_transfer_state_t {
//		response_state_t transmitReady;		///> Whether the Satellite is ready to transmit another Frame (telemetry, etc.)
//		response_t responseReceived;		///> What response was received (ACK, NACK, etc.) regarding the previous message
//		uint8_t transmissionErrors;			///> Error counter for recording consecutive NACKs
//	} file_transfer_state_t;
//
//	/** Wrapper structure for communications co-ordination */
//	typedef struct _communication_state_t {
//		comm_mode_t mode;					///> The current state of the Communications Tasks
//		telecommand_state_t telecommand;	///> The state during the Telecommand mode
//		file_transfer_state_t fileTransfer;	///> The state during the File Transfer mode
//	} communication_state_t;
//
//
///*
//	static communication_state_t state = { 0 };
//	state.telecommand.responseToSend = protocol_message_Nack_tag;
//	error = 0;												// error detection
//	uint8_t txSlotsRemaining;	// number of open frame slots in the transmitter's buffer
//	uint8_t txMessageSize = 0;									// size (in bytes) of an outgoing frame
//	uint8_t txMessage[TRANCEIVER_TX_MAX_FRAME_SIZE] = { 0 };	// output buffer for messages to be transmitted
//
//	txMessageSize = protocolGenerate(state.telecommand.responseToSend, txMessage);
//
//	debugPrint("txMssageSize = %d \n", txMessageSize);
//	for (int i = 0; i < txMessageSize; i++)
//	{
//		debugPrint("%x ", txMessage[i]);
//	}
//	debugPrint("\n");
//
//	vTaskDelay(10000); // test timestamp
//
//	// ack
//	state.telecommand.responseToSend = protocol_message_Ack_tag;
//	txMessageSize = 0;
//
//	txMessageSize = protocolGenerate(state.telecommand.responseToSend, txMessage);
//
//	debugPrint("txMssageSize = %d \n", txMessageSize);
//	for (int i = 0; i < txMessageSize; i++)
//	{
//		debugPrint("%x ", txMessage[i]);
//	}
//	debugPrint("\n");

	/* protoEncode testing */

	/*
	#define TRANCEIVER_TX_MAX_FRAME_SIZE 235
	#define RADSAT_SK_HEADER_SIZE (sizeof(radsat_sk_header_t))
	uint8_t wrappedMessageSize = 0;
	uint8_t wrappedMessage[TRANCEIVER_TX_MAX_FRAME_SIZE];

	//generate new RADSAT-SK message to serialize
	radsat_message rawMessage = { 0 };

	//populate the message
	rawMessage.which_service = radsat_message_FileTransferMessage_tag;
	rawMessage.FileTransferMessage.which_message = 1;// file_transfer_ObcTelemetry_tag;
	rawMessage.FileTransferMessage.ObcTelemetry.mode = 2;
	rawMessage.FileTransferMessage.ObcTelemetry.uptime = 4000;
	rawMessage.FileTransferMessage.ObcTelemetry.rtcTime = 3000;
	rawMessage.FileTransferMessage.ObcTelemetry.rtcTemperature = 25;

	wrappedMessageSize = protoEncode(&rawMessage, wrappedMessage);

	debugPrint("messageSize = %d \n", wrappedMessageSize);
	for (int i = 0; i < wrappedMessageSize; i++)
	{
		debugPrint("%x ", wrappedMessage[i]);
	}
	debugPrint("\n");

	*/

	/* messageWrap testing */
	/*
	#define TRANCEIVER_TX_MAX_FRAME_SIZE 235
	#define RADSAT_SK_HEADER_SIZE (sizeof(radsat_sk_header_t))
	uint8_t wrappedMessageSize = 0;
	uint8_t wrappedMessage[TRANCEIVER_TX_MAX_FRAME_SIZE];

	//generate new RADSAT-SK message to serialize
	radsat_message rawMessage = { 0 };

	//populate the message
	rawMessage.which_service = radsat_message_FileTransferMessage_tag;
	rawMessage.FileTransferMessage.which_message = 1;// file_transfer_ObcTelemetry_tag;
	rawMessage.FileTransferMessage.ObcTelemetry.mode = 2;
	rawMessage.FileTransferMessage.ObcTelemetry.uptime = 4000;
	rawMessage.FileTransferMessage.ObcTelemetry.rtcTime = 3000;
	rawMessage.FileTransferMessage.ObcTelemetry.rtcTemperature = 25;

	wrappedMessageSize = messageWrap(&rawMessage, wrappedMessage);

	debugPrint("messageSize = %d \n", wrappedMessageSize);
	for (int i = 0; i < wrappedMessageSize; i++)
	{
		debugPrint("%x ", wrappedMessage[i]);
	}
	debugPrint("\n");
	*/


	/* messageUnwrap testing (implicitly tests protoDecode)*/


	// test one
	/*
		radsat_message result = {0};
		uint8_t outputSize;
		uint8_t wrappedMessage[] = {0x19, 0x21, 0x6f, 0xc6, 0x07, 0xe9, 0xf8, 0xe0, 0x63, 0x1b, 0x05, 0x0b, 0x03, 0x09, 0x0a};
		uint8_t inputSize = 15; // Size of header + message
		outputSize = messageUnwrap(wrappedMessage, inputSize, &result);
		debugPrint("which_service: %d \n", (uint8_t) result.which_service);
		debugPrint("message size: %d \n", outputSize);
		if (result.which_service == radsat_message_TelecommandMessage_tag){
			debugPrint("Successfully read that this message is a telecommand message.\n");
			// obtain the specific telecommand
			uint8_t telecommand = (uint8_t) result.TelecommandMessage.which_message;
			if(telecommand == telecommand_message_BeginPass_tag){
				debugPrint("Successfully read that this message is a Begin Pass message. \n");
				debugPrint("passLength value read as: %d. Should be 11.\n", result.TelecommandMessage.BeginPass.passLength);
				debugPrint("output size is: %d\n", outputSize);
			}
		}
		*/

		//test two
/*
	radsat_message result = {0};
	uint8_t outputSize;
	uint8_t wrappedMessage[] = {0x19, 0x21, 0x0a, 0x84, 0x3d, 0x04, 0xfe, 0xe0, 0x63, 0x13, 0x3b, 0x1b, 0x39, 0x09, 0x04, 0x13, 0x19, 0x0c, 0x9b, 0x98, 0xa8, 0x41, 0x14, 0x9b, 0x98, 0xa8, 0x41, 0x1c, 0x9b, 0x98, 0xa8, 0x41, 0x24, 0x9b, 0x98, 0xa8, 0x41, 0x29, 0x04, 0x31, 0x04, 0x1b, 0x0d, 0x09, 0x04, 0x11, 0x04, 0x19, 0x04, 0x21, 0x04, 0x29, 0x04, 0x31, 0x04, 0x23, 0x0d, 0x09, 0x04, 0x11, 0x04, 0x19, 0x04, 0x21, 0x04, 0x29, 0x04, 0x31, 0x04};
	uint8_t inputSize = 69; // Size of header + message
	outputSize = messageUnwrap(wrappedMessage, inputSize, &result);
	debugPrint("which_service: %d \n", (uint8_t) result.which_service);
	debugPrint("message size: %d \n", outputSize);
	if (result.which_service == radsat_message_FileTransferMessage_tag){
		debugPrint("Successfully read that this message is a FileTransfer message.\n");
		// obtain the specific telecommand
		uint8_t telecommand = (uint8_t) result.FileTransferMessage.which_message;
		if(telecommand == file_transfer_message_CameraTelemetry_tag){
			debugPrint("Successfully read that this message is a Camera Telemetry message. \n");
			debugPrint("uptime value read as: %d. Should be 5.\n", result.FileTransferMessage.CameraTelemetry.uptime);
			debugPrint("current 3v3 from powerTelemetry submessage: %f. Should be 5.3.\n", result.FileTransferMessage.CameraTelemetry.powerTelemetry.current_3V3);
			debugPrint("detectionThreshold from cameraTwoTelemetry submessage: %d. Should be 5.\n", result.FileTransferMessage.CameraTelemetry.cameraOneTelemetry.detectionThreshold);
			debugPrint("output size is: %d\n", outputSize);
		}
	}
	*/






	//vTaskDelay(1000);


	//printf("start here\n");


	//uint8_t mess[200] = {0};
	//for (int i= 0; i < 200; i++){
	//	mess[i] = 0b10101010;
	//}
	//while (1){
	//	error = IsisTrxvu_tcSendAX25DefClSign(0, mess, 200, 0);
	//	if (error) debugPrint("error = %d\n",error);
	//	vTaskDelay(100);
	//}

	// initialize the FreeRTOS Tasks used for typical mission operation
	initMissionTasks();
	if (error != SUCCESS) {
		// TODO: report to system manager
		debugPrint("MissionInitTask(): failed to initialize FreeRTOS Mission Tasks.\n");
	}

	// let this task delete itself
	vTaskDelete(NULL);
}
