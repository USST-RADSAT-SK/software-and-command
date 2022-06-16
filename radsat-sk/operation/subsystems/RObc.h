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

typedef struct _obc_telemetry_t {
	float temperature;
} obc_telemetry_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int obcTelemetry(obc_telemetry_t* telemetry);
void obcPetWatchdogs(void);


#endif /* ROBC_H_ */
