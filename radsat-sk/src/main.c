/**
 * @file main.c
 */

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/LED.h>

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

#include <RCommunicationTasks.h>
#include <RDosimeterCollectionTask.h>
#include <RImageCaptureTask.h>
#include <RAdcsCaptureTask.h>
#include <RTelemetryCollectionTask.h>
#include <RSatelliteWatchdogTask.h>


#ifdef TEST
#include <RTestSuite.h>
#endif /* TEST */


/***************************************************************************************************
                                   DEFINITIONS AND PRIVATE GLOBALS
***************************************************************************************************/

/** How often the internal OBC Watchdog is kicked (i.e. pet, i.e. reset) in ms. */
#define OBC_WDOG_KICK_PERIOD_MS	(15 / portTICK_RATE_MS)

/** Default stack size (in bytes) allotted to each FreeRTOS Task. */
#define DEFAULT_TASK_STACK_SIZE	(4096)

/** FreeRTOS Task Handles. */
static xTaskHandle communicationRxTaskHandle;
static xTaskHandle communicationTxTaskHandle;
static xTaskHandle dosimeterCollectionTaskHandle;
static xTaskHandle imageCaptureTaskHandle;
static xTaskHandle adcsCaptureTaskHandle;
static xTaskHandle telemetryCollectionTaskHandle;
static xTaskHandle satelliteWatchdogTaskHandle;

/** Communication Transmit Task Priority. Downlinks messages when necessary; very high priority task. */
static const int communicationTxTaskPriority = configMAX_PRIORITIES - 1;
/** Communication Receive Task Priority. Constantly listening for messages; high priority task. */
static const int communicationRxTaskPriority = configMAX_PRIORITIES - 2;

/** Dosimeter Collection Task Priority. Periodically collects payload data; medium priority task. */
static const int dosimeterCollectionTaskPriority = configMAX_PRIORITIES - 3;
/** Image Capture Task Priority. Periodically collects image data; medium priority task. */
static const int imageCaptureTaskPriority = configMAX_PRIORITIES - 3;
/** ADCS Capture Task Priority. Periodically collects ADCS data; medium priority task. */
static const int adcsCaptureTaskPriority = configMAX_PRIORITIES - 3;

/** Telemetry Collection Task Priority. Periodically collects satellite telemetry; low priority task. */
static const int telemetryCollectionTaskPriority = configMAX_PRIORITIES - 4;
/** Satellite Watchdog Task Priority. Routinely pets (resets) satellite subsystem watchdogs; low priority task. */
static const int satelliteWatchdogTaskPriority = configMAX_PRIORITIES - 4;


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static void initBoard(void);
static void initHal(void);
static void initSubsystems(void);

static void initObcWatchdog(void);
static void initTasks(void);

static void runTests(void);


/***************************************************************************************************
                                                MAIN
***************************************************************************************************/

int main(void) {

	// initialize internal OBC board settings
	initBoard();

	// initialize the Hardware Abstraction Library (HAL) drivers
	initHal();

	// initialize external components and the Satellite Subsystem Interface (SSI)
	initSubsystems();

	// initialize the internal OBC watchdog (and the resetting of it)
	initObcWatchdog();

	// run test suite - ONLY RUNS ON 'Test' BUILD CONFIGURATIONS
	runTests();

	// TODO: Antenna Diagnostic & Deployment (if necessary)

	// TODO: Satellite Diagnostic Check (if applicable - may be done later instead)

	// initialize the FreeRTOS Tasks for operation
	initTasks();

	// start the FreeRTOS Scheduler - NEVER GETS PAST THIS LINE
	vTaskStartScheduler();

	// this function should never get here, nevertheless, please make sure that this last call doesn't get optimized away
	exit(0);
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

static void initBoard(void) {

	// Enable the Instruction cache of the ARM9 core. Keep the MMU and Data Cache disabled.
	CP15_Enable_I_Cache();

}


static void initHal(void) {

	// initialize the FRAM memory module for safe persistent storage
	framInit();

	// initialize the Auxillary Camera UART port for communication with Camera
	uartInit(UART_CAMERA_BUS);

	// initialize the I2C bus for general inter-component communication
	i2cInit();

#ifndef RELEASE

	// initilize the LED API
	LED_start();

	// initialize the Debug UART port for debug printing
	uartInit(UART_DEBUG_BUS);

	debugPrint("\n\r -- Using HAL version %s.%s.%s --\n\r", HalVersionMajor, HalVersionMinor, HalVersionRevision);

#endif

}


static void initSubsystems(void) {

	// initialize the Transceiver module
	transceiverInit();

}


/**
 * Initialize a low-priority task that automatically resets the OBC's internal Watchdog.
 *
 * On all builds except for Release, this will also enable an LED that blink every time that the
 * Watchdog timer is reset, allowing for a simple heartbeat to show proper OBC operation.
 *
 * @note This also initializes the Watchdog API.
 */
static void initObcWatchdog(void) {

#ifdef RELEASE
	WDT_startWatchdogKickTask(OBC_WDOG_KICK_PERIOD_MS, FALSE);
#else
	WDT_startWatchdogKickTask(OBC_WDOG_KICK_PERIOD_MS, TRUE);
#endif

}


static void initTasks(void) {

	// initialize the Communication Receive Task
	xTaskGenericCreate(CommunicationRxTask,
					   (const signed char*)"Communication Receive Task",
					   DEFAULT_TASK_STACK_SIZE,
					   NULL,
					   communicationRxTaskPriority,
					   &communicationRxTaskHandle,
					   NULL, NULL);

	// initialize the Communication Transmit Task
	xTaskGenericCreate(CommunicationTxTask,
					   (const signed char*)"Communication Transmit Task",
					   DEFAULT_TASK_STACK_SIZE,
					   NULL,
					   communicationTxTaskPriority,
					   &communicationTxTaskHandle,
					   NULL, NULL);

	// initialize the Dosimeter Collection Task
	xTaskGenericCreate(DosimeterCollectionTask,
					   (const signed char*)"Dosimeter Collection Task",
					   DEFAULT_TASK_STACK_SIZE,
					   NULL,
					   dosimeterCollectionTaskPriority,
					   &dosimeterCollectionTaskHandle,
					   NULL, NULL);

	// initialize the Image Capture Task
	xTaskGenericCreate(ImageCaptureTask,
					   (const signed char*)"Image Capture Task",
					   DEFAULT_TASK_STACK_SIZE,
					   NULL,
					   imageCaptureTaskPriority,
					   &imageCaptureTaskHandle,
					   NULL, NULL);

	// initialize the Image Capture Task
	xTaskGenericCreate(AdcsCaptureTask,
					   (const signed char*)"ADCS Capture Task",
					   DEFAULT_TASK_STACK_SIZE,
					   NULL,
					   adcsCaptureTaskPriority,
					   &adcsCaptureTaskHandle,
					   NULL, NULL);

	// initialize the Telemetry Collection Task
	xTaskGenericCreate(TelemetryCollectionTask,
					   (const signed char*)"Telemetry Collection Task",
					   DEFAULT_TASK_STACK_SIZE,
					   NULL,
					   telemetryCollectionTaskPriority,
					   &telemetryCollectionTaskHandle,
					   NULL, NULL);

	// initialize the Satellite Watchdog Task
	xTaskGenericCreate(SatelliteWatchdogTask,
					   (const signed char*)"Satellite Watchdog Task",
					   DEFAULT_TASK_STACK_SIZE,
					   NULL,
					   satelliteWatchdogTaskPriority,
					   &satelliteWatchdogTaskHandle,
					   NULL, NULL);

}


static void runTests(void) {
#ifdef TEST

	// run unit test suite
	testSuiteRunAll();

	// TODO: Initialize FreeRTOS Tasks for Integration Testing

#endif
}
