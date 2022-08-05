/**
 * @file RTestSuite.h
 * @date December 29, 2021
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RTESTSUITE_H_
#define RTESTSUITE_H_

#include <RTestDosimeter.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int testSuiteRunAll(void);

int selectAndExecuteTests(void);


#endif /* RTESTSUITE_H_ */
