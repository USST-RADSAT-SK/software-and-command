/**
 * @file RImage.h
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
/* Struct for telemetry Detection result and Trigger, ID 20-25 */
typedef struct _tlm_detection_result_and_trigger_t {
	uint16_t alpha;
	uint16_t beta;
	uint8_t captureResult;
	uint8_t detectionResult;
} tlm_detection_result_and_trigger_t;

/* Struct for telemetry image frame, ID 64 */
typedef struct _tlm_image_frame_t {
	uint8_t image_bytes[128];
} tlm_image_frame_t;

/* Struct for telemetry image frame info, ID 65 */
typedef struct _tlm_image_frame_info_t {
	uint16_t imageFrameNumber;
	uint8_t checksum;
} tlm_image_frame_info_t;
/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
