/**
 * @file RDebug.c
 * @date December 29, 2021
 * @author Tyrel Kostyk and Austin Hruska (jah385)
 */

#include <RDebug.h>
#include <RUart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <hal/Timing/Time.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define MAX_DEBUG_CHAR_LENGTH	(4096)

#define DEL (0x7f) 
#define CR  (0x0d) 


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/



/*void debugPrint(const char* stringFormat, ...) {
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

#endif \/* DEBUG *\/
}*/

int printTime(void){
	Time time = { 0 };
	int error = Time_get(&time);
	debugPrint("%d/%d/%d %d:%d:%d\n",
			time.month,
			time.date,
			time.year,
			time.hours,
			time.minutes,
			time.seconds
	);
	if (error) warningPrint("Clock not initialized... Do better.");
	return error;

}

#ifdef DEBUG
int tes(int error){
    if (error){
        printf("Error lol = %d\n", error);
    }
    return error;
}
#endif /* DEBUG */

/**
 * @brief	 Prints an array of raw hex bytes to terminal.
 * @param[in] a pointer to print
 * @param[in] size of variable being printed
 */
void debugPrintHex(uint8_t* values, size_t size){
	for (size_t i = 0; i < size; i++){
		printf("%02x", values[i]);
	}
	return;
}

/**
 *  @brief    Reads an integer between a minimum and maximum value from the debug UART but calls vTaskDelay to prevent blocking.
 *  @param[in] min Minimum value accepted as input
 *  @param[in] max Maximum value accepted as input
 *  @param[out] pValue Pointer to storage location for input value
 *  @return   1 if successful, 0 if input is not a number or outside of specified range
 */
extern unsigned char debugReadIntMinMax(unsigned int *pValue, unsigned int min, unsigned int max) {
	// input variable
	char ascii_char;
	*pValue = 0;

    do {
		// If there is a char waiting to get read from the debug uart buffer
		if (DBGU_IsRxReady()){
    		ascii_char = DBGU_GetChar();

			// add ascii digit to intiger 
			if (ascii_char >= '0' && ascii_char <= '9'){
				DBGU_PutChar(ascii_char);
				*pValue = *pValue * 10 + (ascii_char - '0');

			// Backspace remove digit in variable
			}else if (ascii_char == DEL){
				*pValue = *pValue / 10;
				DBGU_PutChar(DEL);

			// Finish/newline
			}else if (ascii_char == CR){
				DBGU_PutChar('\n');
			}

		// delay if there is nothing in the input buffer
		}else{
			vTaskDelay(10);
		}

    } while (ascii_char != CR);

	if (*pValue < min || *pValue > max) {
        printf("Error: Value out of range\n");
        return 0;
    }

    return 1;
}
