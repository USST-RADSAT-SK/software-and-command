/**
 * @file RDebug.c
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#include <RDebug.h>
#include <RUart.h>
#include <stdio.h>
#include <stdarg.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define MAX_DEBUG_CHAR_LENGTH	(4096)


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void debugPrint(const char* stringFormat, ...) {
#ifdef DEBUG

	// initialize the variadic argument list
	va_list arguments;
	va_start(arguments, stringFormat);

	// buffer to hold the final string after formatting
	char buffer[MAX_DEBUG_CHAR_LENGTH] = { 0 };

	// format the string, store it in the internal buffer (add 1 for the string terminating byte)
	int length = vsnprintf(buffer, MAX_DEBUG_CHAR_LENGTH, stringFormat, arguments) + 1;

	// done with the variadic arguments
	va_end(arguments);

	// transmit the UART message across the debug (secondary) serial port
	uartTransmit(UART_DEBUG_BUS, (const uint8_t *)buffer, length);

#endif /* DEBUG */
}
