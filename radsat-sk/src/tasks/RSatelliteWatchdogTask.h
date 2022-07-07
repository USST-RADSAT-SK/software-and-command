/**
 * @file RSatelliteWatchdog.h
 * @date February 27, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RSATELLITEWATCHDOGTASK_H_
#define RSATELLITEWATCHDOGTASK_H_


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void SatelliteWatchdogTask(void* parameters);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t comunicationMode(void);
uint8_t safeMode(void);



#endif /* RSATELLITEWATCHDOGTASK_H_ */
