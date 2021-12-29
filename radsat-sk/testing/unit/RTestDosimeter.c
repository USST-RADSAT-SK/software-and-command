/**
 * @file RTestDosimeter.c
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#ifdef TEST

#include <assert.h>
#include <RDosimeter.h>

/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Run a unit test to confirm that both Dosimeter boards can return a valid temperature reading
 *
 * @pre I2C must be initialized
 * @pre Both Dosimeter boards must be hooked up to the OBC via I2C and powered on
 */
static void testDosimeterTemperature(void) {

	// variables for requesting both board readings
	uint8_t boardOne = 0;
	uint8_t boardTwo = 1;

	// variable to store temperature readings
	float temperature = 0;

	// grab temperature reading from dosimeter board one
	temperature = dosimeterTemperature(boardOne);

	// temperature reading must not be exactly 0
	assert(temperature != 0);

	// grab temperature reading from dosimeter board two
	temperature = dosimeterTemperature(boardTwo);

	// temperature reading must not be exactly 0
	assert(temperature != 0);
}


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void testDosimeterAll(void) {
	testDosimeterTemperature();
}


#endif
