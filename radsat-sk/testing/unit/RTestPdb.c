/**
 * @file RTestDosimeter.c
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#include <stdio.h>
#include <hal/Utility/util.h>
#include <RDebug.h>
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
static int testPdb(void){

	debugPrint("!!!TESTS RUN HERE!!!");
	return 0;

}


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int testSelectPdb(unsigned int autoSelection) {
	unsigned int selection = autoSelection;
	int returnValue = 0;

	if (!autoSelection) {
		printf( "\n\r Select a test to perform: \n\r");
		printf("\t 1) Run all tests \n\r");
		printf("\t 2) Test Telemetry \n\r");
		printf("\t 0) <- Return\n\r");

		while(UTIL_DbguGetIntegerMinMax(&selection, 0, 2) == 0);
	}

	switch(selection) {
	case 1:
		returnValue = testPdbAll();
		break;
	case 2:
		returnValue = testPdb();
		break;
	default:
		break;
	}

	return returnValue;

}

int testPdbAll(void) {
	int error = 0;
	error = testpdb();
	return error;

}
