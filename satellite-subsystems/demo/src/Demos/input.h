/*
 * input.h
 *
 *  Created on: 21 aug. 2015
 *      Author: pbot
 */

#ifndef INCLUDE_UTIL_INPUT_H_
#define INCLUDE_UTIL_INPUT_H_

#include <stdint.h>

int8_t INPUT_GetINT8( char* printStr );
int16_t INPUT_GetINT16( char* printStr );
int32_t INPUT_GetINT32( char* printStr );
uint8_t INPUT_GetUINT8( char* printStr );
uint16_t INPUT_GetUINT16( char* printStr );
uint32_t INPUT_GetUINT32( char* printStr );
uint8_t INPUT_GetHEX8( char* printStr );
uint16_t INPUT_GetHEX16( char* printStr );
float INPUT_GetFLOAT( char* printStr );
double INPUT_GetDOUBLE( char* printStr );
void INPUT_GetSTRING( char* printStr, char * const str, int len );

#endif /* INCLUDE_UTIL_INPUT_H_ */
