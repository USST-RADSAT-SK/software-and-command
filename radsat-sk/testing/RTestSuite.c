/**
 * @file RTestSuite.c
 * @date December 29, 2021
 * @author Tyrel Kostyk (tck290) and Austin Hruska (jah385)
 */



#ifdef TEST

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>
#include <hal/Utility/util.h>

#include <RCommon.h>
#include <RTestUtils.h>

#include <RTestDosimeter.h>
#include <RTestBattery.h>
#include <RSatelliteWatchdogTask.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** Satellite Watchdog Task Priority. Routinely pets (resets) satellite subsystem watchdogs; low priority task. */
static const int satelliteWatchdogTaskPriority = configMAX_PRIORITIES - 4;


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

void mainTestMenuTask(void* parameters);


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
	(void) autoSelection;
	// TODO: Run health check
	return SUCCESS;
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

int testSelectIsiSpace(unsigned int autoSelection) {
	char* menuTitles[] = {
	};

	TestMenuFunction menuFunctions[] = {
	};

	return testingMenu(autoSelection, menuFunctions, menuTitles, 6);
}


/**
 * Run all of the unit tests
 *
 * @pre All necessary HAL, SSI, etc. initializations completed
 */
int testSuiteRunAll(unsigned int autoSelection) {
	(void) autoSelection;
	int error = 0;
	error = testDosimeterAll();
	return error;
}

int testSelectRadsat(unsigned int autoSelection) {
	char* menuTitles[] = {
		"Run All Tests",
		"-> Dosimeter",
		"-> Battery"
	};

	TestMenuFunction menuFunctions[] = {
		testSuiteRunAll,
		testSelectDosimeter,
		testSelectBattery
	};

	return testingMenu(autoSelection, menuFunctions, menuTitles, 3);
}

void mainTestMenuTask(void* parameters) {
	// ignore the input parameter
	(void)parameters;

	char* menuTitles[] = {
		"Run All RADSAT Tests",
		"Run Health Check",
		"-> RADSAT Tests",
		"-> ISISpace iOBC Tests"
	};

	TestMenuFunction menuFunctions[] = {
		testSelectRadsat,
		runHealthCheck,
		testSelectRadsat,
		testSelectIsiSpace
	};

	testingMenu(RUN_SELECTION, menuFunctions, menuTitles, 4);

	vTaskDelete(NULL);
}

#endif
