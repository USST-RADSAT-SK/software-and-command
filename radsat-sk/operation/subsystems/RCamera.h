/**
 * @file RCamera.h
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153)
 */

#ifndef RCAMERA_H_
#define RCAMERA_H_

#include <stdint.h>
/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
/* Struct used to define ADCS Function*/
typedef struct _detection_results_t {
	uint16_t sunSensorX;
	uint16_t sunSensorY;
	uint16_t sunSensorZ;
	uint16_t imageSensorX;
	uint16_t imageSensorY;
	uint16_t imageSensorZ;
} detection_results_t;

/* Struct that holds all power related telemetry */
typedef struct _CameraTelemetry_PowerTelemetry {
    float current_3V3;
    float current_5V;
    float current_SRAM_1;
    float current_SRAM_2;
    uint8_t overcurrent_SRAM_1;
    uint8_t overcurrent_SRAM_2;
} CameraTelemetry_PowerTelemetry;

/* Struct that holds all Camera configuration related telemetry */
typedef struct _CameraTelemetry_ConfiguarationTelemetry {
    uint8_t detectionThreshold;
    uint8_t autoAdjustMode;
    uint16_t exposure;
    uint8_t autoGainControl;
    uint8_t blueGain;
    uint8_t redGain;
} CameraTelemetry_ConfigurationTelemetry;

/* Struct for Camera Telemetry Collection Function */
typedef struct _CameraTelemetry {
    uint16_t upTime;
    CameraTelemetry_PowerTelemetry powerTelemetry;
    CameraTelemetry_ConfigurationTelemetry cameraOneTelemetry;
    CameraTelemetry_ConfigurationTelemetry cameraTwoTelemetry;
} CameraTelemetry;

/* Struct for telemetry image frame */
typedef struct _tlm_image_frame_t {
	uint8_t image_bytes[128];
} tlm_image_frame_t;

//TODO: RCamera.h: Change the struct to a dynamic allocation or something less wasteful for full image
/* Struct that holds full image with ID */
typedef struct _full_image_t {
	uint8_t image_ID;
	tlm_image_frame_t *imageFrames[8192];
}full_image_t;

/****************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
int downloadImage(uint8_t sram, uint8_t location, uint8_t size, full_image_t *image);
int detectionAndInterpret(detection_results_t *data);
int cameraTelemetry(CameraTelemetry *cameraTelemetry);

#endif /* RCAMERA_H_ */
