/**
 * @file RDebug.c
 * @date December 29, 2021
 * @author Tyrel Kostyk
 */

#include <RDebug.h>
#include <RUart.h>
#include <stdio.h>
#include <stdlib.h>
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
	// original fucntion:
	// int length = vsnprintf(buffer, MAX_DEBUG_CHAR_LENGTH, stringFormat, arguments) + 1;
	vsnprintf(buffer, MAX_DEBUG_CHAR_LENGTH, stringFormat, arguments);

	// done with the variadic arguments
	va_end(arguments);

	// transmit the UART message across UART_DEBUG_BUS/serial port 2
	// uartTransmit(UART_DEBUG_BUS, (const uint8_t *)buffer, length);

	// transmit the UART message across the debug serial port on jtag connector
	printf(buffer);

#endif /* DEBUG */
}

/**
 *  @brief    Reads an integer between a minimum and maximum value from the debug UART
 *  @param[in] min Minimum value accepted as input
 *  @param[in] max Maximum value accepted as input
 *  @param[out] pValue Pointer to storage location for input value
 *  @return   1 if successful, 0 if input is not a number or outside of specified range
 */
extern unsigned char debugRead(unsigned int *pValue, unsigned int min, unsigned int max) {
    int result = 0;
	char ascii_char;
    do {
		if (DBGU_IsRxReady()){
    		ascii_char = DBGU_GetChar();
			if (ascii_char >= '0' && ascii_char <= '9'){
				DBGU_PutChar(ascii_char);
				*pValue = *pValue * 10 + (ascii_char - '0');
			}else if (ascii_char == 0x7f){
				*pValue = *pValue / 10;
				DBGU_PutChar(0x7f);
			}else if (ascii_char == 0x0d){
				DBGU_PutChar('\n');
			}
		}else{
			vTaskDelay(10);
		}
    } while (ascii_char != 0x0d);

	if (result < min || result > max) {
        printf("Error: Value out of range\n");
        return 0;
    }
    return 1;
}
