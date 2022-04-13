/**
 * @file RBattery.h
 * @date March 29, 2022
 * @author Isaac Poirier (iap992)
 */

#ifndef RBATTERY_H_
#define RBATTERY_H_

#include <stdint.h>



/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/


/** Struct for passing all telemetry data */
typedef struct _battery_status_t {
	// Output Voltages
	float outputVoltageBatteryBus;
	float outputVoltage5VBus;
	float outputVoltage3V3Bus;
	// Output Currents
	float outputCurrentBatteryBus;
	float outputCurrent5VBus;
	float outputCurrent3V3Bus;
	float batteryCurrentDirection;
	// Temperatures
	float motherboardTemp;
	float daughterboardTemp1;
	float daughterboardTemp2;
	float daughterboardTemp3;
} battery_status_t;



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int batteryTelemetry(battery_status_t* dataStorage);
int batteryIsNotSafe(uint8_t* safeFlag);


#endif /* RBATTERY_H_ */
