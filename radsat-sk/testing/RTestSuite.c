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

#include <RTestUtils.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** Satellite Watchdog Task Priority. Routinely pets (resets) satellite subsystem watchdogs; low priority task. */
static const int satelliteWatchdogTaskPriority = configMAX_PRIORITIES - 4;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int selectAndExecuteTests(void) {
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

int runHealthCheck(unsigned int autoSelection) {
	// TODO: Run health check
	return SUCCESS;
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

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

		while(debugReadIntMinMax(&selection, 0, 6) == 0) {
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
int testSuiteRunAll(unsigned int autoSelection) {
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
		printf("\t 2) Dosimeter\n\r");
		printf("\t 0) <- Return\n\r");

		while(debugReadIntMinMax(&selection, 0, 2) == 0) {
			vTaskDelay(MENU_DELAY);
		}
	}

	switch(selection) {
	case 1:
		returnValue = testSuiteRunAll(RUN_SELECTION);
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

	char* menuTitles[] = {
		"Run All RADSAT Tests\n",
		"Run Health Check\n",
		"RADSAT Tests -> \n",
		"ISISpace iOBC Tests ->\n"
	};

	TestMenuFunction menuFunctions[] = {
		testSelectRadsat,
		runHealthCheck,
		testSelectRadsat,
		testSelectIsiSpace
	};

	return testingMenu(0, menuFunctions, menuTitles, 4);
}

#endif
