/**
 * @file RTestSuite.c
 * @date December 29, 2021
 * @author Tyrel Kostyk (tck290)
 */




#ifdef TEST

#include <freertos/FreeRTOS.h>
//#include <freertos/semphr.h>
#include <freertos/task.h>

#include <stdio.h>
#include <hal/Utility/util.h>

#include <RCommon.h>

#include "demoTests/I2Ctest.h"
#include "demoTests/SPI_FRAM_RTCtest.h"
#include "demoTests/UARTtest.h"
#include "demoTests/TimeTest.h"
#include "demoTests/SupervisorTest.h"
#include "demoTests/checksumTest.h"

#include <RTestDosimeter.h>
#include <RSatelliteWatchdogTask.h>

#define RUN_SELECTION	0
#define RUN_ALL			1

#define MENU_DELAY		80


/** Satellite Watchdog Task Priority. Routinely pets (resets) satellite subsystem watchdogs; low priority task. */
static const int satelliteWatchdogTaskPriority = configMAX_PRIORITIES - 4;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


int runHealthCheck(void) {
	// TODO: Run health check
	return SUCCESS;
}

int testSelectIsiSpace(unsigned int autoSelection) {
	unsigned int selection = 0;
	int returnValue = 0;

	if (!autoSelection) {
		printf( "\n\r Select a test to perform: \n\r");
		printf("\t 1) I2C \n\r");
		printf("\t 2) SPI + FRAM + RTC \n\r");
		printf("\t 3) UART \n\r");
		printf("\t 4) Supervisor Controller Test - SPI interface \n\r");
		printf("\t 5) Time Test \n\r");
		printf("\t 6) Checksum Test \n\r");
		printf("\t 0) <- Return\n\r");

		while(UTIL_DbguGetIntegerMinMax(&selection, 0, 6) == 0) {
			vTaskDelay(MENU_DELAY);
		}
	}

	switch(selection) {
	case 1:
		returnValue = I2Ctest();
		break;
	case 2:
		returnValue = SPI_FRAM_RTCtest();
		break;
	case 3:
		returnValue = UARTtest();
		break;
	case 4:
		returnValue = SupervisorTest(FALSE);
		break;
	case 5:
		returnValue = TimeTest();
		break;
	case 6:
		returnValue = checksumTest();
		break;
	default:
		break;
	}

	return returnValue;
}

/**
 * Run all of the unit tests
 *
 * @pre All necessary HAL, SSI, etc. initializations completed
 */
int testSuiteRunAll(void) {
	int error = 0;
	error = testDosimeterAll();
	return error;
}

int testSelectRadsat(unsigned int autoSelection) {
	unsigned int selection = autoSelection;
	int returnValue = 0;

	if (!autoSelection) {
		printf( "\n\r Select a test to perform: \n\r");
		printf("\t 1) Run All Tests\n\r");
		printf("\t 4) Dosimeter\n\r");
		printf("\t 0) <- Return\n\r");

		while(UTIL_DbguGetIntegerMinMax(&selection, 0, 2) == 0) {
			vTaskDelay(MENU_DELAY);
		}
	}

	switch(selection) {
	case 1:
		returnValue = testSuiteRunAll();
		break;
	case 2:
		returnValue = testSelectDosimeter(RUN_SELECTION);
		break;
	default:
		break;
	}

	return returnValue;
}

void mainTestMenuTask(void* parameters) {
	// ignore the input parameter
	(void)parameters;

	unsigned int selection = 0;

	while(1) {
		if (!selection) {
			printf( "\n\r Select a test to perform: \n\r");
			printf("\t 1) Run All RADSAT Tests\n\r");
			printf("\t 2) Run Health Check\n\r");
			printf("\t 3) -> RADSAT Tests\n\r");
			printf("\t 4) -> ISISpace iOBC Tests\n\r");
			printf("\t 0) EXIT\n\r");

			while(UTIL_DbguGetIntegerMinMax(&selection, 0, 4) == 0) {
				vTaskDelay(MENU_DELAY);
				debugPrint("Pet\n");
			}
		}

		switch(selection) {
		case 1:
			testSelectRadsat(RUN_ALL);
			break;
		case 2:
			runHealthCheck();
			break;
		case 3:
			testSelectRadsat(RUN_SELECTION);
			break;
		case 4:
			testSelectIsiSpace(0);
			break;
		default:
			break;
		}
	}
}

int selectAndExecuteTests(void) { //unsigned int autoSelection) {
	// initialize the Satellite Watchdog Task
	static xTaskHandle satelliteWatchdogTaskHandle;
	static xTaskHandle testSuiteTaskHandle;

	int error = xTaskCreate(SatelliteWatchdogTask,
						(const signed char*)"Satellite Watchdog Task",
						4096,
						NULL,
						satelliteWatchdogTaskPriority,
						&satelliteWatchdogTaskHandle);

	if (error != pdPASS) {
		debugPrint("selectAndExecuteTests(): failed to create SatelliteWatchdogTask.\n");
		return E_GENERIC;
	}

	// Give time to pet watchdog
	vTaskDelay(1000);

	error = xTaskCreate(mainTestMenuTask,
							(const signed char*)"Test Suite Task",
							4096,
							NULL,
							configMAX_PRIORITIES - 4,
							&testSuiteTaskHandle);

		if (error != pdPASS) {
			debugPrint("selectAndExecuteTests(): failed to create SatelliteWatchdogTask.\n");
			return E_GENERIC;
		}


	return error;
}

#endif
