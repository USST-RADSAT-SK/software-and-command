/**
 * @file RCamera.h
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153) & Atharva Kulkarni (iya789)
 */

#ifndef RCAMERA_H_
#define RCAMERA_H_

#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
/*number of bytes in one frame of an image*/
#define FRAME_BYTES						128

/*maximum number of bytes in one image */
#define MAXIMUM_BYTES					1048576

/* Struct used to define ADCS Function*/
typedef struct _detection_results_t {
	float sunSensorX;
	float sunSensorY;
	float sunSensorZ;
	float nadirSensorX;
	float nadirSensorY;
	float nadirSensorZ;
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
typedef struct _CameraTelemetry_ConfigurationTelemetry {
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
	uint8_t image_bytes[FRAME_BYTES];
} tlm_image_frame_t;

/* Struct that holds full image with ID */
typedef struct _full_image_t {
	uint8_t image_ID;
	uint8_t imageSize;    // CubeSense's size (0 to 4)
	uint16_t framesCount;
	tlm_image_frame_t imageFrames[];
} full_image_t;

/****************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

full_image_t * initializeNewImage(uint8_t size);
int captureImage(void);
int downloadImage(uint8_t sram, uint8_t location, full_image_t *image);
int detectionAndInterpret(detection_results_t *data);
int cameraConfig(CameraTelemetry *cameraTelemetry);
int cameraTelemetry(CameraTelemetry *cameraTelemetry);

#endif /* RCAMERA_H_ */
