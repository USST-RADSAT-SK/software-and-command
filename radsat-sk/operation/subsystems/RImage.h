/**
 * @file RImage.h
 * @date March 26, 2022
 * @author Addi Amaya (Caa746)
 */

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

// Struct for telecommand Capture Image Telecommand, ID 21
typedef struct _tc_capture_image_params_t {
	uint8_t camera;
	uint8_t sram;
	uint8_t whichHalf;
} tc_capture_image_params_t;

//  Struct for Initialize image download Telecommand, ID 64
typedef struct _tc_initialize_imageDownload_params_t {
	   uint8_t sramSelection;
	   uint8_t sramLocation;
	   uint8_t sizeSelection;
} tc_initialize_imageDownload_params_t;

//  Struct for Advance image download Telecommand, ID 65
typedef struct _tc_advance_imageDownload_params_t {
	   uint16_t nextFrameNumber;
} tc_advance_imageDownload_params_t;

/* Struct for telemetry Detection result and Trigger, ID 20-25 */
typedef struct _tlm_detection_result_and_trigger_t {
	uint16_t alpha;
	uint16_t beta;
	uint8_t  captureResult;
	uint8_t  detectionResult;
} tlm_detection_result_and_trigger_t;

/* Struct for telemetry image frame, ID 64 */
typedef struct _tlm_image_frame_t {
	uint8_t image_bytes[128];
} tlm_image_frame_t;

/* Struct for telemetry image frame info, ID 65 */
typedef struct _tlm_image_frame_info_t {
	uint16_t imageFrameNumber;
	uint8_t  checksum;
} tlm_image_frame_info_t;
/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
