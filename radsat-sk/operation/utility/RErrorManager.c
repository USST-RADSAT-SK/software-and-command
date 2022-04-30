/**
 * @file RErrorManager.c
 * @date April 30, 2022
 * @author Tyrel Kostyk
 */

#include <RErrorManager.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/***************************************************************************************************
                                   DEFINITIONS & PRIVATE VARIABLES
***************************************************************************************************/

/** Times that a module/componenent must be error-free before having its error count cleared. */
#define	MODULE_ERROR_RESET_TIME_MS		((uint32_t)5*60*1000)	///> 5 minutes
#define	COMPONENT_ERROR_RESET_TIME_MS	((uint32_t)30*60*1000)	///> 30 minutes

/** A record of the current error status for a particular module or component. */
typedef struct _error_record_t {
	uint32_t tickTimeOfLastError;
	uint32_t errorCount;
} error_record_t;

// Local arrays for tracking error records for all modules and components
static error_record_t moduleErrors[moduleCount] = { 0 };
static error_record_t componentErrors[componentCount] = { 0 };


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void errorReportInternal(module_t module, int error) {

	// record time of previous error occurance
	uint32_t oldTime = moduleErrors[module].tickTimeOfLastError;

	// obtain current time
	uint32_t currentTime = xTaskGetTickCount();

	// record time of new error occurance
	moduleErrors[module].tickTimeOfLastError = currentTime;

	// if enough time has passed; clear previous error counts
	if ((currentTime - oldTime) > MODULE_ERROR_RESET_TIME_MS)
		moduleErrors[module].errorCount = 0;

	// increment the error counter for this module
	moduleErrors[module].errorCount++;

	// TODO: format & send error report to the file transfer service

	// TODO: reset logic

}


void errorReportExternal(component_t component, int error) {

	// record time of previous error occurance
	uint32_t oldTime = componentErrors[component].tickTimeOfLastError;

	// obtain current time
	uint32_t currentTime = xTaskGetTickCount();

	// record time of new error occurance
	componentErrors[component].tickTimeOfLastError = currentTime;

	// if enough time has passed; clear previous error counts
	if ((currentTime - oldTime) > COMPONENT_ERROR_RESET_TIME_MS)
		componentErrors[component].errorCount = 0;

	// increment the error counter for this component
	componentErrors[component].errorCount++;

	// TODO: format & send error report to the file transfer service

	// TODO: reset logic

}

