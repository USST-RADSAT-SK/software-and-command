/**
 * @file RImage.h
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

#include <RCameraCommon.h>
#include <time.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/* Struct for telemetry Detection result and Trigger, ID 20-25 */
typedef struct _tlm_detection_result_and_trigger_adcs_t {
	uint32_t timestamp;
	uint16_t alpha;
	uint16_t beta;
	uint8_t captureResult;
	uint8_t detectionResult;
} tlm_detection_result_and_trigger_adcs_t;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int tcImageCaptureAndDetection(uint8_t camera, uint8_t sram);
int tlmSensorResultAndDetection(tlm_detection_result_and_trigger_adcs_t *telemetry_reply, SensorResultAndDetection sensorSelection);
//interpret_detection_result_t calculateDetectionVector(uint16_t alpha, uint16_t beta);
