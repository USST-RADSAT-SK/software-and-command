/*
 * util.c
 *
 *  Created on: 21 aug. 2015
 *      Author: pbot
 */

#include <hal/Timing/WatchDogTimer.h>

#include <at91/peripherals/dbgu/dbgu.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define _MAX_INPUT_BUFFER_LENGTH 64

static char _input[_MAX_INPUT_BUFFER_LENGTH];

static int _GetInput( char *input, int len );

int8_t INPUT_GetINT8( char* printStr )
{
	long value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtol((char*)_input, &pEnd, 0 );

		if( value > INT8_MAX )
		{
			printf("Value too large!\n\r");
			continue;
		}
		else if(value < INT8_MIN)
		{
			printf("Value too small!\n\r");
			continue;
		}

		done = 1;
	}
	while(!done);

	return (int8_t)value;
}

int16_t INPUT_GetINT16( char* printStr )
{
	long value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtol((char*)_input, &pEnd, 0 );

		if( value > INT16_MAX )
		{
			printf("Value too large!\n\r");
			continue;
		}
		else if(value < INT16_MIN)
		{
			printf("Value too small!\n\r");
			continue;
		}

		done = 1;
	}
	while(!done);

	return (int16_t)value;
}

int32_t INPUT_GetINT32( char* printStr )
{
	long value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtol((char*)_input, &pEnd, 0 );

		if( value > INT32_MAX )
		{
			printf("Value too large!\n\r");
			continue;
		}
		else if(value < INT32_MIN)
		{
			printf("Value too small!\n\r");
			continue;
		}

		done = 1;
	}
	while(!done);

	return (uint32_t)value;
}

uint8_t INPUT_GetUINT8( char* printStr )
{
	unsigned long value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtoul((char*)_input, &pEnd, 0 );

		if( value > UINT8_MAX )
		{
			printf("Value too large!\n\r");
			continue;
		}

		done = 1;
	}
	while(!done); /* TODO: timeout! */

	return (uint8_t)value;
}

uint16_t INPUT_GetUINT16( char* printStr )
{
	unsigned long value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtoul((char*)_input, &pEnd, 0 );

		if( value > UINT16_MAX )
		{
			printf("Value too large!\n\r");
			continue;
		}

		done = 1;
	}
	while(!done); /* TODO: timeout! */

	return (uint16_t)value;
}

uint32_t INPUT_GetUINT32( char* printStr )
{
	unsigned long value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtoul((char*)_input, &pEnd, 0 );

		if( value > UINT32_MAX )
		{
			printf("Value too large!\n\r");
			continue;
		}

		done = 1;
	}
	while(!done); /* TODO: timeout! */

	return (uint32_t)value;
}

uint8_t INPUT_GetHEX8( char* printStr )
{
	unsigned long value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtoul((char*)_input, &pEnd, 16 );

		if( value > UINT8_MAX )
		{
			printf("Value too large!\n\r");
			continue;
		}

		done = 1;
	}
	while(!done); /* TODO: timeout! */

	return (uint8_t)value;
}

uint16_t INPUT_GetHEX16( char* printStr )
{
	unsigned long value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtoul((char*)_input, &pEnd, 16 );

		if( value > UINT16_MAX )
		{
			printf("Value too large!\n\r");
			continue;
		}

		done = 1;
	}
	while(!done); /* TODO: timeout! */

	return (uint16_t)value;
}

float INPUT_GetFLOAT( char* printStr )
{
	float value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtof((char*)_input, &pEnd );

		/* TODO: check limits? */

		done = 1;
	}
	while(!done); /* TODO: timeout! */

	return value;
}

double INPUT_GetDOUBLE( char* printStr )
{
	double value = 0;
	char *pEnd = NULL;
	int done = 0;

	do
	{
		printf("%s", printStr);
		fflush(stdout);

		if( !_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH ) )
		{
			printf("Buffer overrun!\n\r");
			continue;
		}

		value = strtod((char*)_input, &pEnd );

		/* TODO: check limits? */

		done = 1;
	}
	while(!done); /* TODO: timeout! */

	return value;
}

void INPUT_GetSTRING( char* printStr, char * const str, int len )
{
	printf("%s", printStr);

	fflush(stdout);

	_GetInput( str, len );
}

static int _GetInput( char *input, int len )
{
	uint8_t key;
	int index, inputOK;

	index = 0;
	inputOK = 0;

	do
	{
		do
		{
			WDT_forceKickEveryNms(10);
			vTaskDelay(1);
			/* TODO: timeout ? */
		}
		while( !DBGU_IsRxReady() );

		key = DBGU_GetChar();
		DBGU_PutChar( key );

		if( key == 0x0A || key == 0x0D )
		{
			printf("\n\r");
			input[index] = 0; /* terminate string */
			inputOK = 1;
			break;
		}
		else if( key == 0x08 || key == 0x7F )
		{
			index--; /* backspace */
		}
		else
		{
			input[index++] = key;
		}
	}
	while( index < len );

	return inputOK;
}
