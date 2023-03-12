/*
 * RCommunicationStateMachine.h
 *
 *  Created on: Aug 20, 2022
 *      Author: bpitz
 */

#ifndef RCOMMUNICATIONSTATEMACHINE_H_
#define RCOMMUNICATIONSTATEMACHINE_H_


#include <RProtocolService.h>


/** Abstraction of the communication modes */
typedef enum _comm_mode_t {
	commModeQuiet,	///> Prevent downlink transmissions and automatic state changes
	commModePass,	///> Receiving Telecommands from Ground Station
	commModeFileTransfer,	///> Transmitting data to the Ground Station
} comm_mode_t;


void beginPass();
void beginFileTransfer();
void ceaseTransmission();
int updateTime(uint32_t epochTime);
void resetSat();
void sendNack();
void sendAck();
void processCommand(commandType_t type, messageSubject_t* content);


#endif /* RCOMMUNICATIONSTATEMACHINE_H_ */
