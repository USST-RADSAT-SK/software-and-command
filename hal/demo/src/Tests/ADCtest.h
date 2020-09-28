/*
 * ADCtest.h
 *
 *  Created on: 26-Feb-2013
 *      Author: Akhil Piplani
 */

#ifndef ADCTEST_H_
#define ADCTEST_H_

#include <hal/boolean.h>

Boolean ADCtest();
Boolean ADCtestSingleShot();


// The following functions should only be used if you want to perform an ADC test without the use of a test-task.
int ADCtest_initalize();
void ADCtest_printReadout();

#endif /* ADCTEST_H_ */
