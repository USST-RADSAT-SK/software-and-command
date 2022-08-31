/**
 * @file RTestDosimeter.c
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#include <assert.h>
#include <RDosimeter.h>
#include <RCommon.h>
#include <hal/Utility/util.h>
#include <RFileTransfer.pb.h>
#include <stdint.h>
#include <stdio.h>


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Run a unit test to confirm that both Dosimeter boards can return a valid temperature reading
 *
 * @pre I2C must be initialized
 * @pre Both Dosimeter boards must be hooked up to the OBC via I2C and powered on
 */
int checkDosimeter(void) {


	// prepare a protobuf struct to populate with data
	dosimeter_data data = { 0 };

	int error = dosimeterData(&data);

	if (error) {
		debugPrint("checkDosimeter: dosimeterData returned error = %d/n", error);
	}

	printDosimeterData(&data);

	error = 16;

	error -= data.boardOne.channelZero  < 3300.0f && data.boardOne.channelZero  > 2000.0f;
	error -= data.boardOne.channelOne   < 3300.0f && data.boardOne.channelOne   > 2000.0f;
	error -= data.boardOne.channelTwo   < 3300.0f && data.boardOne.channelTwo   > 2000.0f;
	error -= data.boardOne.channelThree < 3300.0f && data.boardOne.channelThree > 2000.0f;
	error -= data.boardOne.channelFour  < 3300.0f && data.boardOne.channelFour  > 2000.0f;
	error -= data.boardOne.channelFive  < 3300.0f && data.boardOne.channelFive  > 2000.0f;
	error -= data.boardOne.channelSix   < 3300.0f && data.boardOne.channelSix   > 2000.0f;
	error -= data.boardOne.channelSeven < 150.0f  && data.boardOne.channelSeven > -51.0f;

	error -= data.boardTwo.channelZero  < 3300.0f && data.boardTwo.channelZero  > 2000.0f;
	error -= data.boardTwo.channelOne   < 3300.0f && data.boardTwo.channelOne   > 2000.0f;
	error -= data.boardTwo.channelTwo   < 3300.0f && data.boardTwo.channelTwo   > 2000.0f;
	error -= data.boardTwo.channelThree < 3300.0f && data.boardTwo.channelThree > 2000.0f;
	error -= data.boardTwo.channelFour  < 3300.0f && data.boardTwo.channelFour  > 2000.0f;
	error -= data.boardTwo.channelFive  < 3300.0f && data.boardTwo.channelFive  > 2000.0f;
	error -= data.boardTwo.channelSix   < 3300.0f && data.boardTwo.channelSix   > 2000.0f;
	error -= data.boardTwo.channelSeven < 150.0f  && data.boardTwo.channelSeven > -51.0f;

	return error;

	// variables for requesting both board readings
	// TODO: replace with real enum values once dosimeter code is merged in
	int boardOne = 0;
	int boardTwo = 1;



	// variable to store temperature readings
	int16_t temperature = 0;

	// grab temperature reading from dosimeter board one
	temperature = dosimeterTemperature(boardOne);

	// temperature reading must be between -50 and +150
	assert(temperature >= -51);
	assert(temperature <= 150);

	// grab temperature reading from dosimeter board two
	temperature = dosimeterTemperature(boardTwo);

	// temperature reading must be between -50 and +150
	assert(temperature >= -51);
	assert(temperature <= 150);

	return 0;
}


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int testDosimeterAll(void) {
	int error = 0;
	error = checkDosimeter();
	return error;
}

int testSelectDosimeter(unsigned int autoSelection) {
	unsigned int selection = autoSelection;
	int returnValue = 0;

	if (!autoSelection) {
		printf( "\n\r Select a test to perform: \n\r");
		printf("\t 1) Run all tests\n\r");
		printf("\t 2) Run Dosimeter Health Check\n\r");
		printf("\t 0) <- Return\n\r");

		while(UTIL_DbguGetIntegerMinMax(&selection, 0, 2) == 0);
	}

	switch(selection) {
	case 1:
		returnValue = testDosimeterAll();
		break;
	case 2:
		returnValue = checkDosimeter();
		break;
	default:
		break;
	}

	return returnValue;
}
