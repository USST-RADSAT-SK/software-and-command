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

#define OBC_TELEMETRY_ERROR 1

typedef struct _obcTelemetry_t {
	float temperature;
} obcTelemetry_t;



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int obcTelemetry(obcTelemetry_t* obcTelemetryBuffer);
void obcPetWatchdogs(void);



#endif /* ROBC_H_ */
