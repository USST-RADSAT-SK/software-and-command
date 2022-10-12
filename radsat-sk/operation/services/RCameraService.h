/**
 * @file RCameraService.h
 * @date September 28, 2022
 * @author
 */

#ifndef RCAMERASERVICE_H_
#define RCAMERASERVICE_H_

#include <RCamera.h>
#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** Struct that holds an arbitrary number of ADCS measurement results **/
typedef struct _adcs_detection_results_t {
	uint8_t validMeasurementsCount;
	detection_results_t results[];
} adcs_detection_results_t;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/** General functions **/
int requestReset(uint8_t resetOption);
int setCamerasSettings(CameraSettings newSettings);
/***********************/

/** ADCS Capture functions **/
void setADCSCaptureSettings(uint8_t nbMeasurements, int interval);
int takeADCSBurstMeasurements(void);
adcs_detection_results_t * getADCSBurstResults(void);
adcs_detection_results_t * initializeNewADCSResults(uint8_t nbMeasurements);

void setADCSReadyForNewBurst(void);
uint8_t getADCSReadyForNewBurstState(void);
/****************************/

/** Image Capture functions **/
void setImageCaptureInterval(int interval);
int getImageCaptureInterval(void);

int setImageDownloadSize(uint8_t size);
uint8_t getImageDownloadSize(void);

int requestImageCapture(uint8_t camera, uint8_t sram, uint8_t location);
int requestImageCaptureDetectAndDownload(uint8_t camera, uint8_t sram, uint8_t size);
int requestImageCaptureAndDetect(uint8_t camera, uint8_t sram);
int requestImageDownload(uint8_t sram, uint8_t size);
void clearImage(void);

void setImageReadyForNewCapture(void);
uint8_t getImageReadyForNewCaptureState(void);
uint8_t getImageReadyForDownlinkState(void);
uint16_t getImageFramesCount(void);
uint8_t getCubeSenseUsageState(void);

void setImageTransferFrameIndex(uint16_t index);
uint8_t imageTransferNextFrame(uint8_t* frame);
uint8_t imageTransferCurrentFrame(uint8_t* frame);
uint8_t imageTransferSpecificFrame(uint8_t* frame, uint16_t index);
/*****************************/

#endif /* RCAMERASERVICE_H_ */
