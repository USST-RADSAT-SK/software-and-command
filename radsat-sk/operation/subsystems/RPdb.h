/**
 * @file RPdb.h
 * @date March 29, 2022
 * @author Tyrel Kostyk (tck290) and Isaac Poirier (iap992)
 */

#ifndef RPdb_H_
#define RPdb_H_

#include <stdint.h>
#include <RFileTransfer.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int pdbSunSensorData(sun_sensor_data* sunData);
int pdbTelemetry(eps_telemetry* dataStorage);
int pdbPetWatchdog(void);

#endif /* RPdb_H_ */
