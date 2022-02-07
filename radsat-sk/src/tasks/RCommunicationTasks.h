/**
 * @file RCommunicationTask.c
 * @date January 30, 2021
 * @author Tyrel Kostyk (tck290) and Matthew Buglass (mab839)
 */

#ifndef RCOMMUNICATIONTASKS_H_
#define RCOMMUNICATIONTASKS_H_

#include <stdint.h>


/***************************************************************************************************
											 PUBLIC API
***************************************************************************************************/

void communicationRxTask(void* parameters);
void communicationTxTask(void* parameters);

void communicationEndPassMode(void);
uint8_t communicationPassModeActive(void);


#endif /* RCOMMUNICATIONTASKS_H_ */
