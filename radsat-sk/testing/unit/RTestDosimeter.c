/**
 * @file RTestDosimeter.c
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#include <assert.h>
#include <RDosimeter.h>
#include <stdint.h>


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Run a unit test to confirm that both Dosimeter boards can return a valid temperature reading
 *
 * @pre I2C must be initialized
 * @pre Both Dosimeter boards must be hooked up to the OBC via I2C and powered on
 */
static int testDosimeterTemperature(void) {

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

int testSelectDosimeter(void) {
	return 0;
}

int testDosimeterAll(void) {
	int error = 0;
	error = testDosimeterTemperature();
	return error;
}
