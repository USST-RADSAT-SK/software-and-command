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

void CommunicationsRxTask(void* parameters);
void CommunicationsTxTask(void* parameters);

void communicationsEndPassMode(void);
uint8_t communicationsPassModeActive(void);


#endif /* RCOMMUNICATIONTASKS_H_ */
