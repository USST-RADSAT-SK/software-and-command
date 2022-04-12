/**
 * @file RCommunicationTasks.h
 * @date January 30, 2022
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#ifndef RCOMMUNICATIONTASKS_H_
#define RCOMMUNICATIONTASKS_H_

#include <stdint.h>


/***************************************************************************************************
                                           FREERTOS TASKS
***************************************************************************************************/

void CommunicationRxTask(void* parameters);
void CommunicationTxTask(void* parameters);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t communicationPassModeActive(void);


#endif /* RCOMMUNICATIONTASKS_H_ */
