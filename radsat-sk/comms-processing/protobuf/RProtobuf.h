/**
 * @file RProtobuf.h
 * @date Jan 25, 2021
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RPROTOBUF_H_
#define RPROTOBUF_H_

#include <common.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include <radsat.pb.h>


/***************************************************************************************************
                                             DEFINITIONS
***************************************************************************************************/

#define PROTO_MAX_ENCODED_SIZE	((uint16_t)RadsatMessage_size)	///> The max size of a message


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t protoEncode( RadsatMessage *rawMessage, uint8_t *outgoingBuffer );
uint8_t protoDecode( uint8_t *incomingBuffer, RadsatMessage *decodedMessage );


#endif /* RPROTOBUF_H_ */
