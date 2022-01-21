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

typedef struct _telecommand_data_t {
	uint8_t transmit;		// 0 = idle (awaiting message); 1 = message processed (send response)
	uint8_t response;		// 0 = NACK; 1 = ACK
} telecommand_data_t;

typedef struct _fileTransfer_data_t {
	uint8_t transmit;		// 0 = idle (awaiting response); 1 = response received (send message)
	uint8_t response;		// 0 = NACK (re-send last message); 1 = ACK (send new message)
} fileTransfer_data_t;

typedef struct _communications_t {
	uint8_t mode;			// 0 = idle (not in passtime); 1 = telecommand mode; 2 = file transfer mode
	telecommand_data_t telecommand;
	fileTransfer_data_t fileTransfer;
} communications_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/


#endif /* RCOMMUNICATIONTASKS_H_ */
