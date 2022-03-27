/**
 * @file RCamera.h
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153)
 */

#ifndef RCAMERA_H_
#define RCAMERA_H_

#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
// Camera telecommand_ Define ID and the Length
typedef struct _camera_telecommand_t {
	uint16_t ID;
	uint8_t paramLength;
} camera_telecommand_t;

/*Define parameters*/

typedef struct _detection_results_t {
	uint16_t sunSensorX;
	uint16_t sunSensorY;
	uint16_t sunSensorZ;
	uint16_t imageSensorX;
	uint16_t imageSensorY;
	uint16_t imageSensorZ;
} detection_results_t;

// ResetTelecommand
typedef struct _tc_reset_Telecommand_params_t {
	uint8_t resetType;
} tc_reset_Telecommand_params_t;

// Clear SRAM overcurrent flags Telecommand
typedef struct _tc_clear_SRAM_overcurrent_params_t {
	uint8_t SRAMOverCurrentFlag;
} tc_clear_SRAM_over_current_t;

// Image capture & detection Telecommand
typedef struct _tc_image_capture_anddetect_params_t {
	uint8_t camera;
	uint8_t sram;
} tc_image_capture_anddetect_params_t ;

//Set sensor 1 detection threshold Telecommand
typedef struct _tc_set_sensor_one_detection_threshold_t {
	uint8_t detectionThreshold1;
} tc_set_sensor_one_detection_threshold_t ;

//Set sensor 2 detection threshold Telecommand
typedef struct _tc_set_sensor_two_detection_threshold_t {
	uint8_t detectionThreshold2;
} tc_set_sensor_two_detection_threshold_t ;

//Set sensor 1 auto-adjust Telecommand
typedef struct _tc_set_SensorOne_Autoadjust_t {
	uint8_t autoadjustenable1;
} tc_set_SensorOne_Autoadjust_t ;

//Set sensor 1 settings Telecommand
typedef struct _tc_set_SensorOne_Setting_t {
	uint8_t exposureTime1;
	uint8_t AGC1;
	uint8_t blueGain1;
	uint8_t redGain1;
} tc_set_SensorOne_Setting_t ;

//Set sensor 2 auto-adjust Telecommand
typedef struct _tc_set_SensorTwo_Autoadjust_t {
	uint8_t autoadjustenable2;
} tc_set_SensorTwo_Autoadjust_t ;

//Set sensor 2 settings Telecommand
typedef struct _tc_set_SensorTwo_Setting_t {
	 uint8_t exposureTime2;
	 uint8_t AGC2;
	 uint8_t blueGain2;
	 uint8_t redGain2;
} tc_set_SensorTwo_Setting_t ;

//Set sensor 1 boresight pixel location Telecommand
typedef struct _tc_set_SensorOne_Boresight_Pixel_Location_t {
	 uint16_t X_pixel1;
	 uint16_t Y_pixel1;
} tc_set_SensorOne_Boresight_Pixel_Location_t ;

//Set sensor 2 boresight pixel location Telecommand
typedef struct _tc_set_SensorTwo_Boresight_Pixel_Location_t {
	  uint16_t X_pixel2;
	  uint16_t Y_pixel2;
} tc_set_SensorTwo_Boresight_Pixel_Location_t ;

//Set sensor 1 mask Telecommand
typedef struct _tc_set_SensorOne_Mask_t {
	  uint8_t  maskNumber1;
	  uint16_t xMinimum1;
	  uint16_t xMaximum1;
	  uint16_t yMinimum1;
	  uint16_t yMaximum1;
} tc_set_SensorOne_Mask_t;

//Set sensor 2 mask Telecommand
typedef struct _tc_set_SensorTwo_Mask_t {
	  uint8_t  maskNumber2;
	  uint16_t xMinimum2;
	  uint16_t xMaximum2;
	  uint16_t yMinimum2;
	  uint16_t yMaximum2;
} tc_set_SensorTwo_Mask_t;

//Set sensor 1 distortion correction coefficients Telecommand
typedef struct _tc_set_SensorOne_distortionCorrectionCoefficient_params_t {
	   uint16_t Mantissa11;
	   uint8_t  Exponent11;
	   uint16_t Mantissa12;
	   uint8_t  Exponent12;
	   uint16_t Mantissa13;
	   uint8_t  Exponent13;
	   uint16_t Mantissa14;
	   uint8_t  Exponent14;
	   uint16_t Mantissa15;
	   uint8_t  Exponent15;
} tc_set_SensorOne_distortionCorrectionCoefficient_params_t;

//Set sensor 2 distortion correction coefficients Telecommand
typedef struct _tc_set_SensorTwo_distortionCorrectionCoefficient_t {
	   uint16_t Mantissa21;
	   uint8_t  Exponent21;
	   uint16_t Mantissa22;
	   uint8_t  Exponent22;
	   uint16_t Mantissa23;
	   uint8_t  Exponent23;
	   uint16_t Mantissa24;
	   uint8_t  Exponent24;
	   uint16_t Mantissa25;
	   uint8_t  Exponent25;
} tc_set_SensorTwo_distortionCorrectionCoefficient_t;


/* Struct for telmetry status, ID 0*/
typedef struct _tlm_status_t {
	uint8_t  nodeType;
	uint8_t  interfaceVersion;
	uint8_t  firmwareVersionMajor;
	uint8_t  firmwareVersionMinor;
	uint16_t runtimeSeconds;
	uint16_t runtimeMSeconds;
} tlm_status_t;

/* Struct for telmetry Serial number, ID 1 */
typedef struct _tlm_serial_number_t {
	uint64_t Nodetype;
} tlm_serial_number_t;

/* Struct for telmetry Serial number, ID 2 */
typedef struct _tlm_communication_status_t {
	uint16_t tcCounter;
	uint16_t tlmCounter;
	uint8_t  tcBufferOverunFlag;
	uint8_t  i2ctlmReadErrorFlag;
	uint8_t	 uarttlmProtocolErrorFlag;
	uint8_t  uartIncompleteMsgFlag;
} tlm_communication_status_t;

/* Struct for telemetry telecommand acknowledege, ID 3*/
typedef struct _tlm_telecommand_ack_t {
	uint8_t last_tc_id;
	uint8_t processed_flag;
	uint8_t tc_error_flag;
} tlm_telecommand_ack_t;


/* Struct for telemetry power, ID 26 */
typedef struct _tlm_power_t {
	uint16_t threeVcurrent;
	uint16_t sramOneCurrent;
	uint16_t sramTwoCurrent;
	uint16_t fiveVcurrent;
	uint8_t sramOneOverCurrent;
	uint8_t sramTwoOverCurrent;
} tlm_power_t;

/* Struct for telemetry configuration, ID 40 */
typedef struct _tlm_config_t{
	uint8_t cameraOneDetectionThrshld;
	uint8_t cameraTwoDetectionThrshld;
	uint8_t cameraOneAutoAdjustMode;
	uint16_t cameraOneExposure;
	uint8_t cameraOneAGC;
	uint8_t cameraOneBlueGain;
	uint8_t cameraOneRedGain;
	uint8_t cameraTwoAutoAdjustMode;
	uint16_t cameraTwoExposure;
	uint8_t cameraTwoAGC;
	uint8_t cameraTwoBlueGain;
	uint8_t cameraTwoRedGain;
} tlm_config_t;


/* Struct for telemetry full image, ID 66-69 */
typedef struct _tlm_full_image_t {
	uint8_t imageBytes[1048576];
} tlm_full_image_t;

/* struct for telemetry read sensor masks ID 72-73 */
typedef struct _tlm_read_sensor_mask_t {
	uint16_t MinXAreaOne;
	uint16_t MaxXAreaOne;
	uint16_t MinYAreaOne;
	uint16_t MaxYAreaOne;
	uint16_t MinXAreaTwo;
	uint16_t MaxXAreaTwo;
	uint16_t MinYAreaTwo;
	uint16_t MaxYAreaTwo;
	uint16_t MinXAreaThree;
	uint16_t MaxXAreaThree;
	uint16_t MinYAreaThree;
	uint16_t MaxYAreaThree;
	uint16_t MinXAreaFourth;
	uint16_t MaxXAreaFourth;
	uint16_t MinYAreaFourth;
	uint16_t MaxYAreaFourth;
	uint16_t MinXAreaFifth;
	uint16_t MaxXAreaFifth;
	uint16_t MinYAreaFifth;
	uint16_t MaxYAreaFifth;
} tlm_read_sensor_mask_t;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

//int cameraCaptureImage(camera_slot_t slot);
//int sendCameraCaptureImageTc();

#endif /* RCAMERA_H_ */
