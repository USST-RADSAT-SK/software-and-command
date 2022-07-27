/**
 * @file RObc.h
 * @date April 2, 2022
 * @author Matthew Buglass (mab839)
 */

#ifndef ROBC_H_
#define ROBC_H_

#include <stdlib.h>




/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

// 255 is index for SPI communication
#define SUPERVISOR_INDEX	255

/** The number of channels used in the Supervisor Controller. */
#define SUPERVISOR_NUMBER_OF_ADC_CHANNELS	10


typedef struct _obc_telemetry_t {
	float temperature;
	int16_t adcValue[SUPERVISOR_NUMBER_OF_ADC_CHANNELS];
	uint32_t supervisorUptime;
	uint32_t IobcUptime;
	uint32_t IobcPowerCycleCount;

	temperature;

	int16_t voltage3v3In;
	int16_t voltageRef;
	int16_t voltageRtc;
	int16_t voltage3v3;
	int16_t voltage1v8;
	int16_t voltage1v0;

	int16_t current3v3;
	int16_t current1v8;
	int16_t current1v0;
} obc_telemetry_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int obcTelemetry(obc_telemetry_t* telemetry);
void obcPetWatchdogs(void);


#endif /* ROBC_H_ */
