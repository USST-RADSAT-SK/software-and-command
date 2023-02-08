/**
 * @file RProtocolService.h
 * @date February 21, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RPROTOCOLSERVICE_H_
#define RPROTOCOLSERVICE_H_

#include <RProtocol.pb.h>
#include <RMessage.h>
#include <RTelecommands.pb.h>
#include <stdint.h>


/***************************************************************************************************
                                             DEFINITIONS
***************************************************************************************************/

#define MAX_MESSAGE_SIZE 	RADSAT_SK_MAX_MESSAGE_SIZE

typedef enum _commandType_t {
	commandProtoUnwrapError = -49,
	commandUnknownCommand,
	commandGeneralError = -1,
	commandNack = 0,
	commandAck,
	commandBeginPass,
	commandBeginFileTransfer,
	commandCeaseTransmission,
	commandUpdateTime,
	commandReset
} commandType_t;

typedef union {
    ack Ack;
    nack Nack;
    begin_pass BeginPass;
    begin_file_transfer BeginFileTransfer;
    cease_transmission CeaseTransmission;
    update_time UpdateTime;
    reset Reset;
} messageSubject_t;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t protocolGenerate(commandType_t command, uint8_t* wrappedMessage);
commandType_t genericHandle(uint8_t* wrappedMessage, uint8_t size, messageSubject_t* messageData);


#endif /* RPROTOCOLSERVICE_H_ */
