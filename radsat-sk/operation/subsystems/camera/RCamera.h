/**
 * @file RCamera.h
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153) & Atharva Kulkarni (iya789)
 */

#ifndef RCAMERA_H_
#define RCAMERA_H_

#include <RCameraCommon.h>
#include <RFileTransfer.h>
#include <RTelecommands.h>
#include <stdint.h>
#include <time.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
/*number of bytes in one frame of an image*/
#define FRAME_BYTES						128

/*maximum number of bytes in one image */
#define MAXIMUM_BYTES					1048576

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
int getResultsAndTriggerNewDetection(adcs_detection *data);
int triggerNewDetectionForBothSensors(void);
int setSettings(Camera_Configuration *cameraSettings);
int getSettings(camera_telemetry *cameraSettings);
int executeReset(uint8_t resetOption);

#endif /* RCAMERA_H_ */
