

#ifndef PB_RTELECOMMANDS_PB_H_INCLUDED
#define PB_RTELECOMMANDS_PB_H_INCLUDED

#include <stdint.h>

/* Enum definitions */
typedef enum _reset_device_t {
    reset_device_t_Obc = 0,
    reset_device_t_Transmitter = 1,
    reset_device_t_Receiver = 2,
    reset_device_t_AntennaSideA = 3,
    reset_device_t_AntennaSideB = 4
} reset_device_t;

// "H"
typedef struct _begin_file_transfer {
    uint16_t resp;
} begin_file_transfer;

// "H"
typedef struct _begin_pass {
    uint16_t passLength;
} begin_pass;

// "h"
typedef struct _cease_transmission {
    uint16_t duration;
} cease_transmission;

// "BB"
typedef struct _reset {
    uint8_t device;
    uint8_t hard;
} reset;

// "I"
typedef struct _update_time {
    uint32_t unixTime;
} update_time;

typedef union {
        begin_pass BeginPass;
        begin_file_transfer BeginFileTransfer;
        cease_transmission CeaseTransmission;
        update_time UpdateTime;
        reset Reset;
} telecommand_message;


#endif
