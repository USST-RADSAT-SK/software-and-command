/**
 * @file RDebug.h
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#ifndef RDEBUG_H_
#define RDEBUG_H_

#include <stdint.h>


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void debugPrint(const char* stringFormat, ...);

extern unsigned char debugReadIntMinMax(unsigned int *pValue, unsigned int min, unsigned int max);

#endif /* RDEBUG_H_ */
