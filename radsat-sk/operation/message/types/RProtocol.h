
#ifndef PB_RPROTOCOL_PB_H_INCLUDED
#define PB_RPROTOCOL_PB_H_INCLUDED

#include <stdint.h>

// "B"
typedef struct _ack {
    uint8_t resp;
} ack;

// "B"
typedef struct _nack {
    uint8_t resp;
} nack;

// "B"
typedef union {
	ack Ack;
	nack Nack;
} protocol_message;

#endif
