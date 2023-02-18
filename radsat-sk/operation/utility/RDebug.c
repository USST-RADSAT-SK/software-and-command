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
#include <hal/errors.h>

#ifdef DEBUG
/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define MAX_DEBUG_CHAR_LENGTH	(4096)

#define DEL (0x7f) 
#define CR  (0x0d) 


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


int printResolvedErrorMessage(int errorValue){
	switch (errorValue){
		case E_NOT_INITIALIZED:
			printf("E_IS_INITIALIZED:-19:Attempt to initialize something that has already been initialized.");
			return 1;
		case E_GET_SEMAPHORE_FAILED:
			printf("E_GET_SEMAPHORE_FAILED:-20:Failed to obtain semaphore for subsystem communication.");
			return 1;
		case E_INDEX_ERROR:
			printf("E_INDEX_ERROR:-22:Incorrect index specified.");
			return 1;
		case E_BITRATE_INPUT_ERROR:
			printf("E_BITRATE_INPUT_ERROR:-23:Failed to set the bitrate of the TRXUV.");
			return 1;
		case E_CWCHAR_INPUT_ERROR:
			printf("E_CWCHAR_INPUT_ERROR:-24:Failed to set the cwcharrate of the TRXUV.");
			return 1;
		case E_IDLE_STATE_ERROR:
			printf("E_IDLE_STATE_ERROR:-25:Failed to go into idle mode of the TRXUV.");
			return 1;
		case E_OUTPUT_MODE_ERROR:
			printf("E_OUTPUT_MODE_ERROR:-26:Failed to go into outmode of the TRXUV.");
			return 1;
		case E_TRXUV_COMPONENT_ERROR:
			printf("E_TRXUV_COMPONENT_ERROR:-27:Failed to choose a device in the TRXUV.");
			return 1;
		case E_WRONG_ADC_CHANNEL:
			printf("E_WRONG_ADC_CHANNEL:-28:Failed to choose a correct ADC channel TRXUV.");
			return 1;
		case E_RESET_SYSTEM:
			printf("E_RESET_SYSTEM:-29:Failed to reset both microcontrollers in the TRXUV.");
			return 1;
		case E_MEM_ALLOC:
			printf("E_MEM_ALLOC:-30:Failed to allocate memory.");
			return 1;
		case E_ATT_ERROR:
			printf("E_ATT_ERROR:-31:Failed to set attenuation value  in the TXS.");
			return 1;
		case E_PARAM_OUTOFBOUNDS:
			printf("E_PARAM_OUTOFBOUNDS:-32:Failed to set attenuation value  in the TXS.");
			return 1;
		case E_TRXUV_FRAME_LENGTH:
			printf("E_TRXUV_FRAME_LENGTH:-33:Input frame lenght was greater than the maximum frame length of TRXUV.");
			return 1;
		case E_INPUT_POINTER_NULL:
			printf("E_INPUT_POINTER_NULL:-34:A pointer input to the function is NULL.");
			return 1;
		case E_COMMAND_NACKED:
			printf("E_COMMAND_NACKED:-35:A pointer input to the function is NULL.");
			return 1;
		default:
		    if (errorValue < 0)
		        printf("Unknown error:%d", errorValue);
			return 0;
	}
}

#ifndef TEST
// fromat string for throbber %-8.8s
char* throbber(void){
    static unsigned int pos = 0;
    static char patern[] = "        ........";
    pos = pos < 15 ? pos + 1 : 0;
    return &patern[pos];
}
#endif

void printTime(void){
	Time time = { 0 };
	int error = Time_get(&time);
	printf("%d/%d/%d %d:%d:%d GMT-6:00",
			time.month,
			time.date,
			time.year,
			time.hours-6,
			time.minutes,
			time.seconds
	);
	if (error) warningPrint("Clock not initialized... Do better.");
}

/**
 * @brief	 Prints an array of raw hex bytes to terminal.
 * @param[in] a pointer to print
 * @param[in] size of variable being printed
 */
void debugPrintHex(uint8_t* values, size_t size){
	debugPrint("          ");
	for (int i = 0; i<32; i++) {
		debugPrint("%02x ", i);
	}
	debugPrint("\n        .");

	for (int i = 0; i<32; i++) {
		debugPrint("___");
	}

	for (unsigned int i = 0; i<size-4; i++) {
		if (i % 32 == 0) {
			debugPrint("\n 0x%04x | ", i);
		}
		debugPrint("%02x ", values[i+4]);
	}
	debugPrint("\n\n");
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
#endif /* DEBUG */
