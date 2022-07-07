/**
 * @file RPdb.h
 * @date March 29, 2022
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#ifndef RPdb_H_
#define RPdb_H_

#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/


/** Struct for storing Sun Sensor Telemetry Data */
typedef struct _sun_sensor_status_t {
	float xPos;
	float xNeg;
	float yPos;
	float yNeg;
	float zPos;
	float zNeg;
} sun_sensor_status_t;


/** Struct for passing all telemetry data */
typedef struct _pdb_status_t {
	sun_sensor_status_t sunSensorData;
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
} pdb_status_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int pdbSunSensorData(sun_sensor_status_t* sunData);
int pdbTelemetry(pdb_status_t* dataStorage);
int pdbPetWatchDog(void);

#endif /* RPdb_H_ */
