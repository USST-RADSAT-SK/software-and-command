/**
 * @file RMessage.h
 * @date February 20, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RMESSAGE_H_
#define RMESSAGE_H_

#include <stdint.h>


/***************************************************************************************************
                                             DEFINITIONS
***************************************************************************************************/

typedef struct __attribute__((packed)) _radsat_sk_header_t {
	uint32_t preamble;		///> A 32-bit tag that identifies the start of the message
	uint32_t crc;			///> Cyclical Redundancy Check of the message following this header
	uint16_t topicTag;		///> The Protobuf topic tag ID
	uint16_t messageTag;	///> The Protobuf message tag ID
	uint16_t size;			///> The size of the message in bytes (NOT including the header)
} radsat_sk_header_t;

#define RADSAT_SK_MESSAGE_PREAMBLE	((uint32_t)0x55535354)	////> "USST" in ASCII code



/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

void messageWrap(uint8_t* dataToWrap, uint16_t topicTag, uint16_t messageTag, uint8_t* wrappedMessage);
void messageUnwrap(uint8_t* wrappedMessage, uint8_t* unwrappedData);


#endif /* RMESSAGE_H_ */
