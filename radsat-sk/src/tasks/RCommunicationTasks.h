/*
 * RCommunicationTasks.h
 *
 *  Created on: Jan 21, 2022
 *      Author: Owner
 */

#ifndef RCOMMUNICATIONTASKS_H_
#define RCOMMUNICATIONTASKS_H_

#include <float.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

typedef struct _telecommand_state_t {
	uint8_t transmitReady;		// 0 = idle (awaiting message); 1 = message processed (send response)
	uint8_t responseToSend;		// 0 = ACK; 1 = NACK
} telecommand_state_t;

typedef struct _fileTransfer_state_t {
	uint8_t transmitReady;		// 0 = idle (awaiting response); 1 = response received (send message)
	uint8_t responseReceived;		// 0 = ACK (send new message); 1 = NACK (re-send last message)
} fileTransfer_state_t;

typedef struct _communications_state_t {
	uint8_t mode;			// 0 = idle (not in passtime); 1 = telecommand mode; 2 = file transfer mode
	telecommand_state_t telecommand;
	fileTransfer_state_t fileTransfer;
} communications_state_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


#endif /* RCOMMUNICATIONTASKS_H_ */
