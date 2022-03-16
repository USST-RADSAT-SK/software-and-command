/**
 * @file RTestSuite.c
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#ifdef TEST

#include <RTestDosimeter.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Run all of the unit tests
 *
 * @pre All necessary HAL, SSI, etc. initializations completed
 */
void testSuiteRunAll(void) {
	testDosimeterAll();
}


#endif
