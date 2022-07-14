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


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int selectAndExecuteTest(void) {
	unsigned int selection = 0;
	int offerMoreTests = 1;

	printf( "\n\r Select a test to perform: \n\r");
	printf("\t 1) I2C \n\r");
	printf("\t 3) SPI + FRAM + RTC \n\r");
	printf("\t 4) UART \n\r");
	printf("\t 11) Supervisor Controller Test - SPI interface \n\r");
	printf("\t 13) Board Test \n\r");
	printf("\t 14) Time Test \n\r");
	printf("\t 15) Checksum Test \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 15) == 0);

	switch(selection) {
	case 1:
		offerMoreTests = I2Ctest();
		break;
	case 3:
		offerMoreTests = SPI_FRAM_RTCtest();
		break;
	case 4:
		offerMoreTests = UARTtest();
		break;
	case 11:
		offerMoreTests = SupervisorTest(FALSE);
		break;
	case 14:
		offerMoreTests = TimeTest();
		break;
	case 15:
		offerMoreTests = checksumTest();
		break;
	default:
		break;
	}

	return offerMoreTests;
}



/**
 * Run all of the unit tests
 *
 * @pre All necessary HAL, SSI, etc. initializations completed
 */
void testSuiteRunAll(void) {
	testDosimeterAll();
}


#endif
