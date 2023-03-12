/**
 * @file RCamera.h
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153) & Atharva Kulkarni (iya789)
 */

#ifndef RCAMERA_H_
#define RCAMERA_H_

#include <RCameraCommon.h>
#include <stdint.h>
#include <time.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
/*number of bytes in one frame of an image*/
#define FRAME_BYTES						128

/*maximum number of bytes in one image */
#define MAXIMUM_BYTES					1048576

/* Struct used to hold the successful image detection angles */
typedef struct _detection_results_t {
	uint32_t sunTimestamp;
	uint16_t sunAlphaAngle;
	uint16_t sunBetaAngle;
	uint32_t nadirTimestamp;
	uint16_t nadirAlphaAngle;
	uint16_t nadirBetaAngle;
} detection_results_t;

/* Struct that holds all power related settings */
typedef struct _CameraSettings_PowerSettings {
    float current_3V3;
    float current_5V;
    float current_SRAM_1;
    float current_SRAM_2;
    uint8_t overcurrent_SRAM_1;
    uint8_t overcurrent_SRAM_2;
} CameraSettings_PowerSettings;

/* Struct that holds all Camera configuration related settings */
typedef struct _CameraSettings_ConfigurationSettings {
    uint8_t detectionThreshold;
    uint8_t autoAdjustMode;
    uint16_t exposure;
    uint8_t autoGainControl;
    uint8_t blueGain;
    uint8_t redGain;
} CameraSettings_ConfigurationSettings;

/* Struct for Camera Settings Collection Function */
typedef struct _CameraSettings {
    uint16_t upTime;
    CameraSettings_PowerSettings powerSettings;
    CameraSettings_ConfigurationSettings cameraOneSettings;
    CameraSettings_ConfigurationSettings cameraTwoSettings;
} CameraSettings;

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
int captureImage(uint8_t camera, uint8_t sram, uint8_t location);
int captureImageAndDetect(uint8_t camera, uint8_t sram);
int downloadImage(uint8_t sram, uint8_t location, full_image_t *image);
int getSingleDetectionStatus(SensorResultAndDetection sensorSelection);
int getResultsAndTriggerNewDetection(detection_results_t *data);
int triggerNewDetectionForBothSensors(void);
int setSettings(CameraSettings *cameraSettings);
int getSettings(CameraSettings *cameraSettings);
int executeReset(uint8_t resetOption);

#endif /* RCAMERA_H_ */
