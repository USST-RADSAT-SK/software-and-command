/**
 * @file RObc.h
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
int obcTelemetry(obcTelemetry_t* obcTelemetryBuffer){
	float temperature = 0;
	int error = RTC_getTemperature(&temperature);

	if (error != SUCCESS) {
		return error;
	}

	obcTelemetryBuffer->temperature = temperature;
	return SUCCESS;

}

/**
 * Pet the watchdog on the OBC
 *
 * @return	N/A
 */
void obcPetWatchdogs(void){
	WDT_forceKick();
}

