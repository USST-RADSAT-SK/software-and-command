

#ifndef PB_RTELECOMMANDS_PB_H_INCLUDED
#define PB_RTELECOMMANDS_PB_H_INCLUDED

#include <stdint.h>

/* Enum definitions */
typedef enum _reset_device_t {
    reset_device_obc		= 0,
    reset_device_trxvu		= 1,
    reset_device_antenna	= 2,
    reset_device_eps		= 3,
    reset_device_battery	= 4,
    reset_device_fram		= 5
} reset_device_t;


typedef enum _reset_t {
    reset_hard	= 0,
    reset_soft	= 1,
} reset_t;

// "H"
typedef struct _begin_file_transfer {
    uint16_t resp;
} begin_file_transfer;

// "H"
typedef struct _begin_pass {
    uint16_t passLength;
} begin_pass;

// "H"
typedef struct _cease_transmission {
    uint16_t duration;
} cease_transmission;

// "BB"
typedef struct _reset {
    uint8_t device;
    uint8_t resetType;
} reset;

/* Struct that holds all Camera configuration related settings */
// "BBHBBB"
typedef struct _Camera_Configuration_Settings {
    uint8_t detectionThreshold;
    uint8_t autoAdjustMode;
    uint16_t exposure;
    uint8_t autoGainControl;
    uint8_t blueGain;
    uint8_t redGain;
} Camera_Configuration_Settings;

/* Struct for Camera Settings Collection Function */
// "BBHBBBBBHBBB"
typedef struct _Camera_Configuration {
	Camera_Configuration_Settings cameraOneSettings;
	Camera_Configuration_Settings cameraTwoSettings;
} Camera_Configuration;


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
