/**
 * @file RCameraCommon.h
 * @date August 19, 2022
 */

#ifndef RCAMERACOMMON_H_
#define RCAMERACOMMON_H_

#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
#define TELECOMMAND_OFFSET_0            ((uint8_t) 3)
#define TELECOMMAND_OFFSET_1            ((uint8_t) 4)
#define TELECOMMAND_OFFSET_2            ((uint8_t) 5)
#define TELECOMMAND_OFFSET_3            ((uint8_t) 6)
#define TELECOMMAND_OFFSET_4            ((uint8_t) 7)
#define TELECOMMAND_RESPONSE_OFFSET		((uint8_t) 2)
#define TELECOMMAND_RESPONSE_LEN		((uint16_t) 1)

#define TELEMETRY_REQUEST_LEN			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_1			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_3			((uint8_t) 3)
#define TELEMETRY_REPLY_SIZE_6			((uint8_t) 6)
#define TELEMETRY_REPLY_SIZE_8			((uint8_t) 8)
#define TELEMETRY_REPLY_SIZE_10			((uint8_t) 10)
#define TELEMETRY_REPLY_SIZE_14			((uint8_t) 14)
#define TELEMETRY_REPLY_SIZE_128		((uint8_t) 128)

#define TELEMETRY_OFFSET_0              ((uint8_t) 2)
#define TELEMETRY_OFFSET_1              ((uint8_t) 3)
#define TELEMETRY_OFFSET_2              ((uint8_t) 4)
#define TELEMETRY_OFFSET_3              ((uint8_t) 5)
#define TELEMETRY_OFFSET_4              ((uint8_t) 6)
#define TELEMETRY_OFFSET_5              ((uint8_t) 7)
#define TELEMETRY_OFFSET_6              ((uint8_t) 8)
#define TELEMETRY_OFFSET_7              ((uint8_t) 9)
#define TELEMETRY_OFFSET_8              ((uint8_t) 10)
#define TELEMETRY_OFFSET_9              ((uint8_t) 11)
#define TELEMETRY_OFFSET_10             ((uint8_t) 12)
#define TELEMETRY_OFFSET_11             ((uint8_t) 13)
#define TELEMETRY_OFFSET_12             ((uint8_t) 14)
#define TELEMETRY_OFFSET_13             ((uint8_t) 15)

#define MESSAGE_ID_OFFSET		        ((uint8_t) 2) // For both telecommand and telemetry

#define BASE_MESSAGE_LEN				((uint8_t) 4)
#define ESCAPE_CHARACTER               	((uint16_t) 0x1F)
#define START_IDENTIFIER                ((uint16_t) 0x7F)
#define FILLER							((uint16_t) 0x00)
#define END_IDENTIFIER                  ((uint16_t) 0xFF)

#define SUN_SENSOR	         	        ((uint8_t) 0)
#define NADIR_SENSOR	                ((uint8_t) 1)
#define SRAM1                           ((uint8_t) 0)
#define SRAM2                           ((uint8_t) 1)
#define TOP_HALVE                       ((uint8_t) 0)
#define BOTTOM_HALVE                    ((uint8_t) 1)

/* Enum of telemetry request ID for sensor result and new detection */
typedef enum _SensorResultAndDetection {
	sensor1        = 0x94, // TLM 20 (no new detection)
	sensor2        = 0x95, // TLM 21 (no new detection)
	sensor1_sram1  = 0x96, // TLM 22
	sensor2_sram2  = 0x97, // TLM 23
	sensor1_sram2  = 0x98, // TLM 24
	sensor2_sram1  = 0x99  // TLM 25
} SensorResultAndDetection;

/* Struct to define 3D vector */
typedef struct _interpret_detection_result_t {
	float X_AXIS;
	float Y_AXIS;
	float Z_AXIS;
} interpret_detection_result_t;

/****************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
uint8_t* MessageBuilder(uint8_t response_size);
int escapeAndTransmitTelecommand(uint8_t *telecommandBuffer, uint8_t messageSize);
int receiveAndUnescapeTelemetry(uint8_t *telemetryBuffer, uint8_t messageSize);

#endif /* RCAMERACOMMON_H_ */
