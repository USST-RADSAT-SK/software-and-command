/**
 * @file RImage.h
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/* Enum of telemetry request ID for sensor result and new detection */
typedef enum _SensorResultAndDetection {
	sensor1_sram1  = 0x96, // TLM 22
	sensor2_sram2  = 0x97, // TLM 23
	sensor1_sram2  = 0x98, // TLM 24
	sensor2_sram1  = 0x99  // TLM 25
} SensorResultAndDetection;

/* Struct for telemetry Detection result and Trigger, ID 20-25 */
typedef struct _tlm_detection_result_and_trigger_adcs_t {
	uint16_t alpha;
	uint16_t beta;
	uint8_t captureResult;
	uint8_t detectionResult;
} tlm_detection_result_and_trigger_adcs_t;

/* Struct to define 3D vector */
typedef struct _interpret_detection_result_t {
	float X_AXIS;
	float Y_AXIS;
	float Z_AXIS;
} interpret_detection_result_t;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int tcImageCaptureAndDetection(uint8_t camera, uint8_t sram);
int tlmSensorResultAndDetection(tlm_detection_result_and_trigger_adcs_t *telemetry_reply, SensorResultAndDetection sensorSelection);
interpret_detection_result_t calculateDetectionVector(uint16_t alpha, uint16_t beta);
