/**
 * @file RBattery.h
 * @date March 29, 2022
 * @author Isaac Poirier (iap992)
 */

#ifndef RBATTERY_H_
#define RBATTERY_H_

#include <stdint.h>


#endif /* RBATTERY_H_ */


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

uint8_t safeFlag;


/** Struct for passing all telemetry data */
typedef struct _batteryStatus {
	// Output Voltages
	float outputVoltageBatteryBus;
	float outputVoltage5VBus;
	float outputVoltage3V3Bus;
	// Output Currents
	float outputCurrentBatteryBus;
	float outputCurrent5VBus;
	float outputCurrent3V3Bus;
	// Temperatures
	float motherboardTemp;
	float daughterboardTemp1;
	float daughterboardTemp2;
	float daughterboardTemp3;
} BatteryStatus;



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int getBatteryTelemetry(BatteryStatus*);

int checkSafeFlag(void);


//#endif  /* RBATTERY_H_ */
