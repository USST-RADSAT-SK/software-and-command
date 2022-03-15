/**
 * @file RPdb.h
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#ifndef RPdb_H_
#define RPdb_H_

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
} SunSensorStatus;


/** Struct for passing all telemetry data */
typedef struct _PdbStatus {
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
	float PdbTemperature;
} PdbStatus;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int getSunSensorData(SunSensorStatus*);
int getPdbTelemetry(PdbStatus*);
int petPdbWatchdog(void);

#endif /* RPdb_H_ */
