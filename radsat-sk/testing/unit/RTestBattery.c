/**
 * @file RTestDosimeter.c
 * @date December 29, 2021
 * @author Austin Hruska (jah385)
 */

#include <assert.h>
#include <RBattery.h>
#include <RCommon.h>
#include <hal/Utility/util.h>
#include <RFileTransfer.pb.h>
#include <stdint.h>
#include <stdio.h>
#include <RTestUtils.h>

void printBatteryTelemetry(battery_status_t* dataStorage);

/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Run a unit test to confirm that both Dosimeter boards can return a valid temperature reading
 *
 * @pre I2C must be initialized
 * @pre Both Dosimeter boards must be hooked up to the OBC via I2C and powered on
 */
int checkBattery(unsigned int autoSelection) {
	(void) autoSelection;

	battery_status_t dataStorage = { 0 };

	int error = batteryTelemetry(&dataStorage);
	if (error) {
		debugPrint("batteryTelemetry returned error = %d\n", error);
		return error;
	}

	printBatteryTelemetry(&dataStorage);

	return 0;
}


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void printBatteryTelemetry(battery_status_t* dataStorage) {
	debugPrint("Output Voltage\n");
	debugPrint("outputVoltageBatteryBus = %f\n", dataStorage->outputVoltageBatteryBus);
	debugPrint("outputVoltage5VBus      = %f\n", dataStorage->outputVoltage5VBus);
	debugPrint("outputVoltage3V3Bus     = %f\n", dataStorage->outputVoltage3V3Bus);
	debugPrint("Output Current\n");
	debugPrint("outputCurrentBatteryBus = %f\n", dataStorage->outputCurrentBatteryBus);
	debugPrint("outputCurrent5VBus      = %f\n", dataStorage->outputCurrent5VBus);
	debugPrint("outputCurrent3V3Bus     = %f\n", dataStorage->outputCurrent3V3Bus);
	debugPrint("batteryCurrentDirection = %f\n", dataStorage->batteryCurrentDirection);
	debugPrint("Temperatures\n");
	debugPrint("motherboardTemp    = %f\n", dataStorage->motherboardTemp);
	debugPrint("daughterboardTemp1 = %f\n", dataStorage->daughterboardTemp1);
	debugPrint("daughterboardTemp2 = %f\n", dataStorage->daughterboardTemp2);
	debugPrint("daughterboardTemp3 = %f\n", dataStorage->daughterboardTemp3);
	return;
}

int testBatteryAll(unsigned int autoSelection) {
	int error = 0;
	error = checkBattery(autoSelection);
	return error;
}

int testSelectBattery(unsigned int autoSelection) {
	char* menuTitles[] = {
		"Run all tests",
		"Run Battery Health Check"
	};

	TestMenuFunction menuFunctions[] = {
		testBatteryAll,
		checkBattery
	};

	return testingMenu(autoSelection, menuFunctions, menuTitles, 2);
}
