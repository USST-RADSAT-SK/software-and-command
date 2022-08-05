/**
 * @file RTestSuite.c
 * @date December 29, 2021
 * @author Tyrel Kostyk (tck290)
 */




#ifdef TEST

#include <stdio.h>
#include <hal/Utility/util.h>

#include "demoTests/I2Ctest.h"
#include "demoTests/SPI_FRAM_RTCtest.h"
#include "demoTests/UARTtest.h"
#include "demoTests/TimeTest.h"
#include "demoTests/SupervisorTest.h"
#include "demoTests/checksumTest.h"

#include <RTestDosimeter.h>

#define RUN_SELECTION	0
#define RUN_ALL			1

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int selectAndExecuteTests(unsigned int autoSelection) {
	unsigned int selection = autoSelection;
	int returnValue = 0;

	if (!autoSelection) {
		printf( "\n\r Select a test to perform: \n\r");
		printf("\t 1) Run All RADSAT Tests\n\r");
		printf("\t 2) -> RADSAT Tests\n\r");
		printf("\t 3) -> ISISpace iOBC Tests\n\r");
		printf("\t 0) EXIT\n\r");

		while(UTIL_DbguGetIntegerMinMax(&selection, 1, 3) == 0);
	}

	switch(selection) {
	case 1:
		returnValue = testSelectRadsat(RUN_ALL);
		break;
	case 2:
		returnValue = testSelectRadsat(RUN_SELECTION);
		break;
	case 3:
		returnValue = testSelectIsiSpace(void);
		break;
	default:
		break;
	}

	return returnValue;
}

int testSelectIsiSpace(void) {
	unsigned int selection = 0;
	int returnValue = 0;

	if (!autoSelection) {
		printf( "\n\r Select a test to perform: \n\r");
		printf("\t 1) I2C \n\r");
		printf("\t 2) SPI + FRAM + RTC \n\r");
		printf("\t 3) UART \n\r");
		printf("\t 4) Supervisor Controller Test - SPI interface \n\r");
		printf("\t 5) Board Test \n\r");
		printf("\t 6) Time Test \n\r");
		printf("\t 7) Checksum Test \n\r");
		printf("\t 0) <- Return\n\r");

		while(UTIL_DbguGetIntegerMinMax(&selection, 1, 7) == 0);
	}

	switch(selection) {
	case 1:
		returnValue = I2Ctest();
		break;
	case 3:
		returnValue = SPI_FRAM_RTCtest();
		break;
	case 4:
		returnValue = UARTtest();
		break;
	case 11:
		returnValue = SupervisorTest(FALSE);
		break;
	case 14:
		returnValue = TimeTest();
		break;
	case 15:
		returnValue = checksumTest();
		break;
	default:
		break;
	}

	return returnValue;
}

int testSelectRadsat(unsigned int autoSelection) {
	unsigned int selection = autoSelection;
	int returnValue = 0;

	if (!autoSelection) {
		printf( "\n\r Select a test to perform: \n\r");
		printf("\t 1) Run All Tests\n\r");
		printf("\t 2) I2C \n\r");
		printf("\t 3) SPI + FRAM + RTC \n\r");
		printf("\t 0) <- Return\n\r");

		while(UTIL_DbguGetIntegerMinMax(&selection, 1, 3) == 0);
	}

	switch(selection) {
	case 1:
		returnValue = I2Ctest();
		break;
	case 3:
		returnValue = SPI_FRAM_RTCtest();
		break;
	case 4:
		returnValue = UARTtest();
		break;
	case 11:
		returnValue = SupervisorTest(FALSE);
		break;
	case 14:
		returnValue = TimeTest();
		break;
	case 15:
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


#endif
