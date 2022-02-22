/**
 * @file REps.h
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#ifndef REPS_H_
#define REPS_H_

#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/


/** Struct for storing Sun Sensor Telemetry Data */
typedef struct _sunSensorStatus {
	float xPos;
	float xNeg;
	float yPos;
	float yNeg;
	float zPos;
	float zNeg;
	uint16_t timestamp;
} SunSensorStatus;


/** Struct for passing all telemetry data */
typedef struct _epsStatus {
	SunSensorStatus sunSensorData;
	// Output Voltages
	float outputVoltageBCR;
	float outputVoltageBatteryBus;
	float outputVoltage5VBus;
	float outputVoltage3V3Bus;
	// Output Currents
	float outputCurrentBCR_mA;
	float outputCurrentBatteryBus;
	float outputCurrent5VBus;
	float outputCurrent3V3Bus;
	// PDB Motherboard Temperature
	float epsTemperature;
	uint16_t timestamp;
} EpsStatus;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

SunSensorStatus getSunSensorData(void);
EpsStatus getEpsTelemetry(void);
int petEpsWatchdog(void);

#endif /* REPS_H_ */
