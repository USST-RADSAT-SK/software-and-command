/**
 * @file RObc.h
 * @date April 2, 2022
 * @author Matthew Buglass (mab839)
 */

#include <RObc.h>
#include <hal/errors.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Timing/RTC.h>

/***************************************************************************************************
                                  PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/



/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
uint8_t obcTelemetry(obcTelemetry_t* obcTelemetryBuffer){
	float temperature = 0;
	int error = RTC_getTemperature(*temperature);

	if (error == 0) {
		obcTelemetryBuffer->temperature = temperature;
		return 0;

	}
	else {
		return OBC_TELEMETRY_ERROR;
	}
}

void obcPetWatchdogs(void){
	WDT_forceKick();
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/
