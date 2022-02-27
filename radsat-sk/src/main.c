/**
 * @file main.c
 */

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/LED.h>
//#include <hal/Drivers/I2C.h>
//#include <hal/Drivers/UART.h>
//#include <hal/Storage/FRAM.h>

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

#include <RCommunicationTasks.h>
#include <RDosimeterCollectionTask.h>

#ifdef TEST
#include <RTestSuite.h>
#endif /* TEST */

/***************************************************************************************************
                                   DEFINITIONS AND PRIVATE GLOBALS
***************************************************************************************************/

/** How often the internal OBC Watchdog is kicked (i.e. pet, i.e. reset) in ms. */
#define OBC_WDOG_KICK_PERIOD_MS	(15 / portTICK_RATE_MS)

/** How often the external subsystem Watchdog timers are kicked (i.e. pet, i.e. reset) in ms. */
#define OBC_WDOG_KICK_PERIOD_MS	(15 / portTICK_RATE_MS)


/***************************************************************************************************
                                     PRIVATE FUNCTION PROTOTYPES
***************************************************************************************************/

static void initBoard(void);
static void initHal(void);
static void initSsi(void);

static void initObcWatchdog(void);
static void initTasks(void);

static void runTests(void);


/***************************************************************************************************
                                                MAIN
***************************************************************************************************/

int main(void) {

	// initialize internal OBC board settings
	initBoard();

	// initialize the HAL drivers
	initHal();

	// initialize the internal OBC watchdog (and the resetting of it)
	initObcWatchdog();

	// initialize the SSI library/drivers
	initSsi();

	// TODO: Subsystem Initialization (Transceiver, Downlink Manager, etc. if necessary)

	// run test suite - ONLY RUNS ON 'Test' BUILD CONFIGURATIONS
	runTests();

	// TODO: Antenna Diagnostic & Deployment (if necessary)
	// TODO: Satellite Diagnostic Check (if applicable - may be done later instead)

	// initialize the FreeRTOS Tasks for operation
	initTasks();

	// start the FreeRTOS Schedule - NEVER GETS PAST THIS LINE
	vTaskStartScheduler();

	// This function should never get here, nevertheless, please make sure that this last call doesn't get optimized away
	exit(0);
}


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

	debug("\n\r -- Using HAL version %s.%s.%s --\n\r", HalVersionMajor, HalVersionMinor, HalVersionRevision);

#endif

}


static initSsi(void) {

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

	xTaskGenericCreate(communicationRxTask, (const signed char*)"Communication RX Task", 1024, NULL, configMAX_PRIORITIES-2, &taskMainHandle, NULL, NULL);

}

static void runTests(void) {
#ifdef TEST

	// run unit test suite
	testSuiteRunAll();

	// TODO: Initialize FreeRTOS Tasks for Integration Testing

#endif
}
