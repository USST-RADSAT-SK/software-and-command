/**
 * @file RBattery.h
 * @date March 29, 2022
 * @author Isaac Poirier (iap992)
 */

#ifndef RBATTERY_H_
#define RBATTERY_H_

#include <stdint.h>
#include <RFileTransfer.h>



/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int batteryTelemetry(battery_telemetry* dataStorage);
int batteryIsNotSafe(uint8_t* safeFlag);
void batteryReset(void);


#endif /* RBATTERY_H_ */
