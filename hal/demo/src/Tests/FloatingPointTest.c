/*
 * FloatingPointTest.c
 *
 *  Created on: 20-Feb-2013
 *      Author: Akhil Piplani
 */

#include <hal/boolean.h>

#include <stdio.h>

Boolean floatingPointTest() {
	double pi = 3.14159265359;
	double largeNum = pi*10000.0;
	double largeNegNum = largeNum * -1.0;
	double smallNum = pi/1000.0;
	double smallNegNum = smallNum * -1.0;

	float pi_f = 3.14159265359;
	float largeNum_f = pi_f*10000.0;
	float largeNegNum_f = largeNum_f * -1.0;
	float smallNum_f = pi/1000.0;
	float smallNegNum_f = smallNum * -1.0;

	printf(" \n\r Testing Floating Point Printing. \n\r");

	printf("%f (expected:  3.142) \n\r", pi);
	printf("%f (expected:  3.642) \n\r", pi+0.5);
	printf("%f (expected:  31415.927) \n\r", largeNum);
	printf("%f (expected: -31415.927) \n\r", largeNegNum);
	printf("%f (expected:  0.003) \n\r", smallNum);
	printf("%f (expected: -0.003) \n\r\n\r", smallNegNum);

	printf("%f (expected:  3.142) \n\r", pi_f);
	printf("%f (expected:  3.642) \n\r", pi_f+0.5);
	printf("%f (expected:  31415.927) \n\r", largeNum_f);
	printf("%f (expected: -31415.927) \n\r", largeNegNum_f);
	printf("%f (expected:  0.003) \n\r", smallNum_f);
	printf("%f (expected: -0.003) \n\r\n\r", smallNegNum_f);

	printf("%e (expected:   3.142e+0) \n\r", pi);
	printf("%e (expected:   3.642e+0) \n\r", pi+0.5);
	printf("%E (expected:   3.142E+4) \n\r", largeNum);
	printf("%e (expected:  -3.142e+4) \n\r", largeNegNum);
	printf("%e (expected:   3.142e-3) \n\r", smallNum);
	printf("%E (expected:  -3.142E-3) \n\r\n\r", smallNegNum);

	printf("%E (expected:   3.142E+0) \n\r", pi_f);
	printf("%e (expected:   3.642e+0) \n\r", pi_f+0.5);
	printf("%e (expected:   3.142E+4) \n\r", largeNum_f);
	printf("%E (expected:  -3.142E+4) \n\r", largeNegNum_f);
	printf("%e (expected:   3.142e-3) \n\r", smallNum_f);
	printf("%E (expected:  -3.142E-3) \n\r", smallNegNum_f);

	printf(" \n\r Done! \n\r");
	return TRUE;
}
