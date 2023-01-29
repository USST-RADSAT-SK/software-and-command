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

/** Struct prepared for downlink that holds the ADCS measurement results **/
typedef struct _adcs_detection_results_t {
	uint8_t validMeasurementsCount;
	detection_results_t results[];  // Variable size, corresponding to the number of measurements in a burst
} adcs_detection_results_t;

/* Struct prepared for downlink that holds an image frame */
typedef struct _image_frame_t {
	uint16_t frameIndex;
	uint8_t image_bytes[FRAME_BYTES];
} image_frame_t;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/** General functions **/
int requestReset(uint8_t resetOption);
int setCamerasSettings(CameraSettings_ConfigurationSettings sunSettings, CameraSettings_ConfigurationSettings nadirSettings);
/***********************/

/** ADCS Capture functions **/
void setADCSCaptureInterval(int interval);
int getADCSCaptureInterval(void);

void setADCSBurstSettings(uint8_t nbMeasurements, int interval);
int takeADCSBurstMeasurements(void);
adcs_detection_results_t * getADCSBurstResults(void);
adcs_detection_results_t * initializeNewADCSResults(uint8_t nbMeasurements);

void setADCSReadyForNewBurst(void);
uint8_t getADCSReadyForNewBurstState(void);
/****************************/

/** Image Capture functions **/
void setImageCaptureInterval(int interval);
int getImageCaptureInterval(void);

void setImageDownloadSize(uint8_t size);
uint8_t getImageDownloadSize(void);

int requestImageCapture(uint8_t camera, uint8_t sram, uint8_t location);
int requestImageCaptureDetectAndDownload(uint8_t camera, uint8_t sram, uint8_t size);
int requestImageCaptureAndDetect(uint8_t camera, uint8_t sram);
int requestImageDownload(uint8_t sram, uint8_t size);

void setImageReadyForNewCapture(void);
uint8_t getImageReadyForNewCaptureState(void);
uint8_t getImageReadyForDownlinkState(void);
uint16_t getImageFramesCount(void);
uint8_t getCubeSenseUsageState(void);

void setImageTransferFrameIndex(int index);
uint8_t imageTransferNextFrame(image_frame_t* frame);
uint8_t imageTransferCurrentFrame(image_frame_t* frame);
uint8_t imageTransferSpecificFrame(image_frame_t* frame, int index);
/*****************************/

#endif /* RCAMERASERVICE_H_ */
