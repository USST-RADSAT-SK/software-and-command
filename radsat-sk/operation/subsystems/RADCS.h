/**
 * @file RImage.h
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/* Struct for telemetry Detection result and Trigger, ID 20-25 */
typedef struct _tlm_detection_result_and_trigger_adcs_t {
	uint16_t alpha;
	uint16_t beta;
	uint8_t captureResult;
	uint8_t detectionResult;
} tlm_detection_result_and_trigger_adcs_t;

/* Struct to define 3D vector */
typedef struct _interpret_detection_result_t {
	uint16_t X_AXIS;
	uint16_t Y_AXIS;
	uint16_t Z_AXIS;
} interpret_detection_result_t;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int tcImageCaputreAndDetection(uint8_t camera);
int tlmSensorOneResultAndDetectionSRAMOne(tlm_detection_result_and_trigger_adcs_t *telemetry_reply);
int tlmSensorTwoResultAndDetectionSRAMOne(tlm_detection_result_and_trigger_adcs_t *telemetry_reply);
interpret_detection_result_t detectionResult(uint16_t alpha, uint16_t beta);
