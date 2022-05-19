/**
 * @file RObc.c
 * @date April 2, 2022
 * @author Matthew Buglass (mab839)
 */

#include <RObc.h>
#include <RCommon.h>
#include <hal/errors.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Timing/RTC.h>

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Retrieve Temperature telemetry from OBC
 *
 * @return	Error code; 0 for success, otherwise see hal/errors.h.
 */
int obcTelemetry(obcTelemetry_t* telemetry){
	float temperature = 0;
	int error = RTC_getTemperature(&temperature);

	if (error != SUCCESS) {
		// TODO: record errors (if present) to System Manager
		return error;
	}

	telemetry->temperature = temperature;
	return SUCCESS;

}

