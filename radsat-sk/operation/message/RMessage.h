/**
 * @file RMessage.h
 * @date February 20, 2022
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RMESSAGE_H_
#define RMESSAGE_H_

#include <stdint.h>
#include <crc.h>
#include <RRadsat.h>


/***************************************************************************************************
                                             DEFINITIONS
***************************************************************************************************/

/** Header that will precede all RADSAT-SK messages. */
typedef struct __attribute__((packed)) _radsat_sk_raw_message_t {
	uint16_t preamble;		///> A hardcoded tag that identifies the start of a RADSAT-SK message
	crc_t crc;				///> Cyclical Redundancy Check of all of the following bytes
	uint8_t size;			///> The size of the message in bytes (NOT including the header)
	uint32_t timestamp;		///> The time (in seconds since Unix Epoch) that the message was formatted
	uint8_t body[radsat_message_size];			///> Body of the message
} radsat_sk_raw_message_t;

/** The hardcoded preamble that will begin every RADSAT-SK message. The starting year of the RADSAT-SK Project*/
#define RADSAT_SK_MESSAGE_PREAMBLE	(0x2018)

/** The size of the RADSAT-SK message header. */
#define RADSAT_SK_HEADER_SIZE	(sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint32_t))


/** The offset of the CRC start location within the RADSAT-SK Message Header. */
#define RADSAT_SK_HEADER_CRC_OFFSET	(sizeof(((radsat_sk_raw_message_t*)0)->preamble)+sizeof(((radsat_sk_raw_message_t*)0)->crc))

/** The maximum size of a RADSAT-SK message (including the message header and all other overhead). */
#define RADSAT_SK_MAX_MESSAGE_SIZE	((uint16_t)sizeof(radsat_sk_raw_message_t))


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

uint8_t messageWrap(radsat_message* rawMessage, radsat_sk_raw_message_t* wrappedMessage);
uint8_t messageUnwrap(uint8_t* wrappedMessage, uint8_t size, radsat_message* rawMessage);


#endif /* RMESSAGE_H_ */
