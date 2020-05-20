/*  comms-processing_tasks.c

    Created by Minh Phong Bill Truong on April 28th 2020
 */
#include "FreeRTOS.h"
#include "semphr.h"

#include <stdio.h>
#include "comms-processing_tasks.h"

//==============================================================================
//                                   GLOBALS
//==============================================================================


//==============================================================================
//                                  FUNCTIONS
//==============================================================================

/**
 *  @brief Call functions required to deparse xxx
 *  
 *  The deparser task receives the data that is to be sent down (clarify) and calls the
 *  required functions to handle the formatting of the data into a packet. 
 * 
 *  @header	"software-and-command/main-applicatoion/comms-processing_tasks.c"
 *	@param	Unused
 *	@pre	None
 * 	@post	Formatted xxx is stored in memory
 * 	@return	None
 */
void deparseTask(void* pvparameters) {
    while(1) {
        /*
        * Wait until signal to deparse xxx is given
        */
        if (xSemaphoreTake(deparseSignal, portMAX_DELAY)){
            deparseDataTask(void* pvparameters); // Stub function call
            encodeTask(void* pvparameters); // Stub function call
        }
    }
    return 0;
}