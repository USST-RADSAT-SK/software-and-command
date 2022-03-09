
/**
 * @file RCamera.c
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153)
 */


#include <RCamera.h>
#include <RUart.h>
#include <hal/errors.h>
#include <string.h>
#include <stdio.h>



/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
//Telecommand define
typedef struct __attribute__((__packed__)) _tc_header_t {
	uint8_t header1;
	uint8_t header2;
} tc_header_t;

typedef struct __attribute__((__packed__)) _tc_trailer_t {
	uint8_t trailer1;
	uint8_t trailer2;
} tc_trailer_t;

typedef uint8_t telecommand_id_t;

#define startidentifier1              ((uint8_t) 0x1F)
#define startidentifier2              ((uint8_t) 0x7F)

#define telecommandid0                ((uint8_t) 0x00)
#define telecommandid11               ((uint8_t) 0x0B)
#define telecommandid20               ((uint8_t) 0x14)
#define telecommandid21               ((uint8_t) 0x15)
#define telecommandid40               ((uint8_t) 0x28)
#define telecommandid41               ((uint8_t) 0x29)
#define telecommandid42               ((uint8_t) 0x2A)
#define telecommandid43               ((uint8_t) 0x2B)
#define telecommandid44               ((uint8_t) 0x2C)
#define telecommandid45               ((uint8_t) 0x2D)
#define telecommandid50               ((uint8_t) 0x32)
#define telecommandid51               ((uint8_t) 0x33)
#define telecommandid52               ((uint8_t) 0x34)
#define telecommandid53               ((uint8_t) 0x35)
#define telecommandid54               ((uint8_t) 0x36)
#define telecommandid55               ((uint8_t) 0x37)
#define telecommandid64               ((uint8_t) 0x40)
#define telecommandid65               ((uint8_t) 0x41)

#define CAPTURE_IMAGE_CMD_SIZE	      ((uint8_t) 3)
#define CAPTURE_IMAGE_TC_SIZE	      ((uint8_t) 8)
#define REQUEST_TELEMETRY_SIZE	      ((uint8_t) 5)
#define TELEMETRY_ACK_SIZE	          ((uint8_t) 5)

#define TELECOMMAND_OVERHEAD	                ((uint8_t) (sizeof(tc_header_t) + sizeof(tc_trailer_t) + sizeof(telecommand_id_t)))
#define TELECOMMAND_HEADER_INDEX	            ((uint8_t)  0)
#define TELECOMMAND_ID_INDEX		            ((uint8_t)  sizeof(tc_header_t))
#define TELECOMMAND_PARAM_INDEX		            ((uint8_t)  (TELECOMMAND_ID_INDEX + sizeof(telecommand_id_t)))
#define TELECOMMAND_TRAILER_INDEX(paramLength)	((uint8_t)  (TELECOMMAND_PARAM_INDEX + paramLength))

#define resetCommunicationInterfaces  ((uint8_t) 1)
#define resetCameras                  ((uint8_t) 2)
#define resetMCU                      ((uint8_t) 3)

#define ClearSRAM1flag                ((uint8_t) 0)
#define ClearSRAM2flag                ((uint8_t) 1)
#define ClearBothFlags                ((uint8_t) 2)

#define CAMERA_ONE	                  ((uint8_t) 0)
#define CAMERA_TWO	                  ((uint8_t) 1)

#define IMAGE_SENSOR	              ((uint8_t) CAMERA_TWO)
#define SUN_SENSOR	         	      ((uint8_t) CAMERA_ONE)

#define SRAM1                         ((uint8_t) 0)
#define SRAM2                         ((uint8_t) 1)

#define TOP_HALVE                     ((uint8_t) 0)
#define BOTTOM_HALVE                  ((uint8_t) 1)

#define detectionThresholdValue1      ((uint8_t) 1)
#define detectionThresholdValue2      ((uint8_t) 1)

#define autoadjustdisabled            ((uint8_t) 0)
#define autoadjustenabled             ((uint8_t) 1)

#define exposureRegisterValue1        ((uint8_t) 1)
#define gainControlRegister1          ((uint8_t) 1)
#define blueGainControlRegister1      ((uint8_t) 1)
#define redGainControlRegister1       ((uint8_t) 1)

#define exposureRegisterValue2        ((uint8_t) 1)
#define gainControlRegister2          ((uint8_t) 1)
#define blueGainControlRegister2      ((uint8_t) 1)
#define redGainControlRegister2       ((uint8_t) 1)

#define xPixelLocationCAM1Boresight   ((uint8_t) 1)
#define yPixelLocationCAM1Boresight   ((uint8_t) 1)

#define xPixelLocationCAM2Boresight   ((uint8_t) 1)
#define yPixelLocationCAM2Boresight   ((uint8_t) 1)

#define areaNumber1                   ((uint8_t) 1)
#define lowerXLimit1                  ((uint8_t) 1)
#define upperXLimit1                  ((uint8_t) 1)
#define lowerYLimit1                  ((uint8_t) 1)
#define upperYLimit1                  ((uint8_t) 1)

#define areaNumber2                   ((uint8_t) 1)
#define lowerXLimit2                  ((uint8_t) 1)
#define upperXLimit2                  ((uint8_t) 1)
#define lowerYLimit2                  ((uint8_t) 1)
#define upperYLimit2                  ((uint8_t) 1)

#define Mantissa11Value               ((uint8_t) 1)
#define Exponent11Value               ((uint8_t) 1)
#define Mantissa12Value               ((uint8_t) 1)
#define Exponent12Value               ((uint8_t) 1)
#define Mantissa13Value               ((uint8_t) 1)
#define Exponent13Value               ((uint8_t) 1)
#define Mantissa14Value               ((uint8_t) 1)
#define Exponent14Value               ((uint8_t) 1)
#define Mantissa15Value               ((uint8_t) 1)
#define Exponent15Value               ((uint8_t) 1)

#define Mantissa21Value               ((uint8_t) 1)
#define Exponent21Value               ((uint8_t) 1)
#define Mantissa22Value               ((uint8_t) 1)
#define Exponent22Value               ((uint8_t) 1)
#define Mantissa23Value               ((uint8_t) 1)
#define Exponent23Value               ((uint8_t) 1)
#define Mantissa24Value               ((uint8_t) 1)
#define Exponent24Value               ((uint8_t) 1)
#define Mantissa25Value               ((uint8_t) 1)
#define Exponent25Value               ((uint8_t) 1)

#define ImageSize1024                 ((uint8_t) 0)
#define ImageSize512                  ((uint8_t) 1)
#define ImageSize256                  ((uint8_t) 2)
#define ImageSize128                  ((uint8_t) 3)
#define ImageSize64                   ((uint8_t) 4)

#define nextFrameNumberValue          ((uint8_t) 4)

#define endidentifier1                ((uint8_t) 0x1F)
#define endidentifier2                ((uint8_t) 0xFF)

//Telemetry define
#define telemetryid0                  ((uint8_t) 0x80)
#define telemetryid1                  ((uint8_t) 0x81)
#define telemetryid2                  ((uint8_t) 0x82)
#define telemetryid3                  ((uint8_t) 0x83)
#define telemetryid19                 ((uint8_t) 0x93)
#define telemetryid20                 ((uint8_t) 0x94)
#define telemetryid21                 ((uint8_t) 0x95)
#define telemetryid22                 ((uint8_t) 0x96)
#define telemetryid23                 ((uint8_t) 0x97)
#define telemetryid24                 ((uint8_t) 0x98)
#define telemetryid25                 ((uint8_t) 0x99)
#define telemetryid26                 ((uint8_t) 0x9A)
#define telemetryid40                 ((uint8_t) 0xA8)
#define telemetryid64                 ((uint8_t) 0xC0)
#define telemetryid65                 ((uint8_t) 0xC1)
#define telemetryid66                 ((uint8_t) 0xC2)
#define telemetryid67                 ((uint8_t) 0xC3)
#define telemetryid68                 ((uint8_t) 0xC4)
#define telemetryid69                 ((uint8_t) 0xC5)
#define telemetryid72                 ((uint8_t) 0xC8)
#define telemetryid73                 ((uint8_t) 0xC9)

#define TELEMETRY_OVERHEAD	          ((uint8_t) (sizeof(TELECOMMAND_OVERHEAD))
#define TELEMETRY_HEADER_INDEX	      ((uint8_t)  0)
#define TELEMETRY_ID_INDEX		      ((uint8_t)  sizeof(tc_header_t))
#define TELEMETRY_PARAM_INDEX		  ((uint8_t)  (TELEMETRY_ID_INDEX + sizeof(telecommand_id_t)))
#define TELEMETRY_TRAILER_INDEX(paramLength)  ((uint8_t)  (TELEMETRY_PARAM_INDEX + paramLength))

#define STATUS_TELEMETRY_SIZE                       ((uint8_t) 8)
#define COMMUNICATION_STATUS_TELEMETRY_SIZE         ((uint8_t) 8)
#define SERIAL_NUMBER_TELEMETRY_SIZE                ((uint8_t) 6)
#define POWER_TELEMETRY_SIZE                        ((uint8_t) 10)
#define CONFIGURATION_TELEMETRY_SIZE                ((uint8_t) 14)
#define IMAGE_FRAME_TELEMETRY_SIZE                  ((uint8_t) 128)
#define IMAGE_FRAME_INFO_TELEMETRY_SIZE             ((uint8_t) 3)
#define FULL_IMAGE_SRAM1_LOCATION1_TELEMETRY_SIZE   ((uint8_t) 1048576)
#define FULL_IMAGE_SRAM1_LOCATION2_TELEMETRY_SIZE   ((uint8_t) 1048576)
#define FULL_IMAGE_SRAM2_LOCATION1_TELEMETRY_SIZE   ((uint8_t) 1048576)
#define FULL_IMAGE_SRAM2_LOCATION2_TELEMETRY_SIZE   ((uint8_t) 1048576)
#define READ_SENSOR1_MASK_TELEMETRY_SIZE            ((uint8_t) 40)
#define READ_SENSOR2_MASK_TELEMETRY_SIZE            ((uint8_t) 40)

/** Invalid input parameters to the Camera module. */
#define ERROR_INVALID_PARAM		(-36)

static const uint8_t ackResponse[TELEMETRY_ACK_SIZE] = { 0x1F, 0x7F, 0x00, 0x1F, 0xFF };

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

// static int confirmPreviousTelecommand(void);

//ResetTelecommand
camera_telecommand_t resetTelecommand = {
		.ID = 0x00,
		.paramLength = 1
	};
typedef struct _tc_reset_Telecommand_params_t {
	uint8_t resetType;
} tc_reset_Telecommand_params_t;

//Clear SRAM overcurrent flags Telecommand
camera_telecommand_t clearSRAMOvercurrentFlags = {
		.ID = 0x0B,
		.paramLength = 1
	};
typedef struct _tc_clear_SRAM_overcurrent_params_t {
		uint8_t SRAMOverCurrentFlag;
	} _tc_clear_SRAM_over_current_t;

//Capture & detect Telecommand
camera_telecommand_t captureAndDetect = {
		.ID = 0x14,
		.paramLength = 2
	};
typedef struct _tc_image_capture_anddetect_params_t {
	uint8_t camera;
	uint8_t sram;
	} _tc_image_capture_anddetect_params_t ;

//Capture Image Telecommand
camera_telecommand_t captureImageTelecommand = {
		.ID = 0x15,
		.paramLength = 3
	};
typedef struct _tc_capture_image_params_t {
	uint8_t camera;
	uint8_t sram;
	uint8_t whichHalf;
} tc_capture_image_params_t;

//Set sensor 1 detection threshold Telecommand
camera_telecommand_t setSensorOneDetectionThreshold = {
		.ID = 0x28,
		.paramLength = 1
	};
typedef struct _tc_set_sensor_one_detection_threshold_t {
		uint8_t detectionThreshold1;
		} _tc_set_sensor_one_detection_threshold_t ;

//Set sensor 2 detection threshold Telecommand
camera_telecommand_t setSensorTwoDetectionThreshold = {
			.ID = 0x29,
			.paramLength = 1
		};
typedef struct _tc_set_sensor_two_detection_threshold_t {
		uint8_t detectionThreshold2;
		} _tc_set_sensor_two_detection_threshold_t ;

//Set sensor 1 auto-adjust Telecommand
camera_telecommand_t setSensorOneAutoadjust = {
			.ID = 0x2A,
			.paramLength = 1
		};
typedef struct _tc_set_SensorOne_Autoadjust_t {
		uint8_t autoadjustenable1;
		} _tc_set_SensorOne_Autoadjust_t ;

//Set sensor 1 settings Telecommand
camera_telecommand_t setSensorOneSetting = {
			.ID = 0x2B,
			.paramLength = 5
		};
typedef struct _tc_set_SensorOne_Setting_t {
		uint8_t exposureTime1;
		uint8_t AGC1;
		uint8_t blueGain1;
		uint8_t redGain1;
		} _tc_set_SensorOne_Setting_t ;

//Set sensor 2 auto-adjust Telecommand
camera_telecommand_t setSensorTwoAutoadjust = {
			.ID = 0x2C,
			.paramLength = 1
		};
typedef struct _tc_set_SensorTwo_Autoadjust_t {
		uint8_t autoadjustenable2;
		} _tc_set_SensorTwo_Autoadjust_t ;

//Set sensor 2 settings Telecommand
camera_telecommand_t setSensorTwoSetting = {
			.ID = 0x2D,
			.paramLength = 5
		};
typedef struct _tc_set_SensorTwo_Setting_t {
	   uint8_t exposureTime2;
	   uint8_t AGC2;
	   uint8_t blueGain2;
	   uint8_t redGain2;
	} _tc_set_SensorTwo_Setting_t ;

//Set sensor 1 boresight pixel location Telecommand
camera_telecommand_t setSensorOneBoresightPixelLocation = {
			.ID = 0x32,
			.paramLength = 4
		};
typedef struct _tc_set_SensorOne_Boresight_Pixel_Location_t {
	   uint16_t X_pixel1;
	   uint16_t Y_pixel1;
} _tc_set_SensorOne_Boresight_Pixel_Location_t ;

//Set sensor 2 boresight pixel location Telecommand
camera_telecommand_t setSensorTwoBoresightPixelLocation = {
			.ID = 0x33,
			.paramLength = 4
		};
typedef struct _tc_set_SensorTwo_Boresight_Pixel_Location_t {
	   uint16_t X_pixel2;
	   uint16_t Y_pixel2;
} _tc_set_SensorTwo_Boresight_Pixel_Location_t ;


//Set sensor 1 mask Telecommand
camera_telecommand_t setSensorOneMask = {
			.ID = 0x34,
			.paramLength = 9
		};
typedef struct _tc_set_SensorOne_Mask_t {
	   uint8_t  maskNumber1;
	   uint16_t xMinimum1;
	   uint16_t xMaximum1;
	   uint16_t yMinimum1;
	   uint16_t yMaximum1;
} _tc_set_SensorOne_Mask_t;

//Set sensor 2 mask Telecommand
camera_telecommand_t setSensorTwoMask = {
			.ID = 0x35,
			.paramLength = 9
		};
typedef struct _tc_set_SensorTwo_Mask_t {
	   uint8_t  maskNumber2;
	   uint16_t xMinimum2;
	   uint16_t xMaximum2;
	   uint16_t yMinimum2;
	   uint16_t yMaximum2;
} _tc_set_SensorTwo_Mask_t;

//Set sensor 1 distortion correction coefficients Telecommand
camera_telecommand_t setSensorOneDistortionCorrectionCoefficient = {
			.ID = 0x36,
			.paramLength = 15
		};
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
} _tc_set_SensorOne_distortionCorrectionCoefficient_params_t;

//Set sensor 2 distortion correction coefficients Telecommand
camera_telecommand_t setSensorTwoDistortionCorrectionCoefficient = {
			.ID = 0x37,
			.paramLength = 15
		};
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
} _tc_set_SensorTwo_distortionCorrectionCoefficient_t;

//Initialize image download Telecommand
camera_telecommand_t initializeImageDownload = {
			.ID = 0x40,
			.paramLength = 3
		};
typedef struct _tc_initialize_imageDownload_params_t {
	   uint8_t sramSelection;
	   uint8_t sramLocation;
	   uint8_t sizeSelection;
} _tc_initialize_imageDownload_params_t;

//Advance image download Telecommand
camera_telecommand_t advanceimageDownload = {
			.ID = 0x41,
			.paramLength = 2
		};
typedef struct _tc_advance_imageDownload_params_t {
	   uint16_t nextFrameNumber;
	} _tc_advance_imageDownload_params_t;

//Header and footer
tc_header_t header = {
		.header1 = startidentifier1,
		.header2 = startidentifier2
};

tc_trailer_t trailer = {
		.trailer1 = endidentifier1,
		.trailer2 = endidentifier2
};
/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
int sendResetTc(uint8_t resetType)
{
// determine size of telecommand
	uint8_t resetTcSize = TELECOMMAND_OVERHEAD + resetTelecommand.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[resetTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 0
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid0;

//telecommand parameters- Interface control document. Page:25
	tc_reset_Telecommand_params_t params = {
				.resetType= resetCommunicationInterfaces,
		};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));
// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(captureImageTelecommand.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}
/**************************************************************************************************************************/
int sendClearSramOverCurrentFlagTc(uint8_t SRAMOverCurrentFlag)
{
// determine size of telecommand
	uint8_t clearSramOverCurrentFlagTcSize = TELECOMMAND_OVERHEAD + clearSRAMOvercurrentFlags.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[clearSramOverCurrentFlagTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 0
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid11;

//telecommand parameters- Interface control document. Page:25
	_tc_clear_SRAM_overcurrent_params_t params = {
				.SRAMOverCurrentFlag= ClearSRAM1flag,
		};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));
// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(captureImageTelecommand.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}
/**************************************************************************************************/
int sendCameraCaptureImageTc(uint8_t sensor, uint8_t sram, uint8_t half)
{
// determine size of telecommand
	uint8_t cameraCaptureImageTcSize = TELECOMMAND_OVERHEAD + captureImageTelecommand.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[cameraCaptureImageTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 21
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid21;

//telecommand parameters- Interface control document. Page:25
	tc_capture_image_params_t params = {
			.camera = sensor,
			.sram = sram,
			.whichHalf = half
	};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(captureImageTelecommand.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}
/******************************************************************************************************/
int sendImageAndCaptureDetectionTc(uint8_t camera, uint8_t sram)
{
// determine size of telecommand
	uint8_t imageAndCaptureDetectionTcSize = TELECOMMAND_OVERHEAD + captureAndDetect.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[imageAndCaptureDetectionTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 21
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid20;

//telecommand parameters- Interface control document. Page:25
	_tc_image_capture_anddetect_params_t params = {
			.camera = CAMERA_ONE,
			.sram = SRAM1,
};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(_tc_image_capture_anddetect_params_t.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}
/**********************************************************************************************/
/*The threshold is set to a default value calculated for robustness.
If the user wishes to change the threshold values, telecommands 40
and 41 can be used. It is however recommended that the default values be used.*/

int sendSetSensorOneDetectionThresholdTc(uint8_t detectionThreshold1)
{
// determine size of telecommand
	uint8_t setSensorOneDetectionThresholdTcSize = TELECOMMAND_OVERHEAD + setSensorOneDetectionThreshold.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[setSensorOneDetectionThresholdTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 40
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid40;

//telecommand parameters- Interface control document. Page:26
	_tc_set_sensor_one_detection_threshold_t params = {
			.detectionThreshold1= detectionThresholdValue1,
};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorOneDetectionThreshold.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}

/****************************************************************************************************/
int sendSetSensorTwoDetectionThresholdTc(uint8_t detectionThreshold2)
{
// determine size of telecommand
	uint8_t setSensorTwoDetectionThresholdTcSize = TELECOMMAND_OVERHEAD +  setSensorTwoDetectionThreshold.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[setSensorTwoDetectionThresholdTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 41
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid41;

//telecommand parameters- Interface control document. Page:26
	_tc_set_sensor_two_detection_threshold_t params = {
			.detectionThreshold2= detectionThresholdValue2,
		};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorTwoDetectionThreshold.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}

/**************************************************************************************************************/
/*The default exposure setting for the sun sensor is set to a fixed calculated value. It is recommended that the
client does not alter this value. The Nadir sensor uses an auto-exposure algorithm to adjust the changing
brightness of the earth in the sensor’s image. These values are calibrated for robust sensor operation.
If, however the client wishes to use the Nadir sensor as a greyscale imager, the exposure can be set by using
telecommands 42 – 45.*/

int sendSetSensorOneAutoadjustTc(uint8_t autoadjustenable1)
{
// determine size of telecommand
	uint8_t setSensorOneAutoadjustTcSize = TELECOMMAND_OVERHEAD + setSensorOneAutoadjust.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[setSensorOneAutoadjustTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 42
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid42;

//telecommand parameters- Interface control document. Page:26
	_tc_set_SensorOne_Autoadjust_t params = {
			.autoadjustenable1= autoadjustenabled,
		};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorOneAutoadjust.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}

/*************************************************************************************************************/

int sendSetSensorOneSettingTc(uint16_t exposureTime1, uint8_t AGC1, uint8_t blueGain1, uint8_t redGain1)
{
// determine size of telecommand
	uint8_t setSensorOneSettingTcSize = TELECOMMAND_OVERHEAD + setSensorOneSetting.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[setSensorOneSettingTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 43
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid43;

//telecommand parameters- Interface control document. Page:26
	_tc_set_SensorOne_Setting_t params = {
			.exposureTime1= exposureRegisterValue1,
			.AGC1=gainControlRegister1,
			.blueGain1= blueGainControlRegister1,
			.redGain1= redGainControlRegister1,
		};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorOneSetting.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}
/********************************************************************************************************/
int sendSetSensorTwoAutoadjustTc(uint8_t autoadjustenable2)
{
// determine size of telecommand
	uint8_t setSensorTwoAutoadjustTcSize = TELECOMMAND_OVERHEAD + setSensorTwoAutoadjust.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[setSensorTwoAutoadjustTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 44
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid44;

//telecommand parameters- Interface control document. Page:27
	_tc_set_SensorTwo_Autoadjust_t params = {
			.autoadjustenable2=autoadjustenabled,
		};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorTwoAutoadjust.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}
/********************************************************************************************************************/
int sendSetSensorTwoSettingTc(uint16_t exposureTime2, uint8_t AGC2,uint8_t blueGain2,uint8_t redGain2)
{
// determine size of telecommand
	uint8_t setSensorTwoSettingTcSize = TELECOMMAND_OVERHEAD + setSensorTwoSetting.paramLength;

// build  camera capture telecommand
	uint8_t commandBuffer[setSensorTwoSettingTcSize];

// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 45
	commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid45;

//telecommand parameters- Interface control document. Page:27
	_tc_set_SensorTwo_Setting_t params = {
			.exposureTime2= exposureRegisterValue2,
			.AGC2=gainControlRegister2,
			.blueGain2= blueGainControlRegister2,
			.redGain2= redGainControlRegister2,
		};

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorTwoSetting.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
}
/*******************************************************************************************************************/
	int sendSetSensorOneBoresightPixelLocationTc(uint16_t Y_pixel1, uint16_t X_pixel1)
	{
// determine size of telecommand
		uint8_t setSensorOneBoresightPixelLocationTcSize = TELECOMMAND_OVERHEAD + setSensorOneBoresightPixelLocation.paramLength;

// build  camera capture telecommand
		uint8_t commandBuffer[setSensorOneBoresightPixelLocationTcSize];

// start of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 50
		commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid50;

// telecommand parameters- Interface control document. Page:27
		_tc_set_SensorOne_Boresight_Pixel_Location_t params = {
				   .X_pixel1=xPixelLocationCAM1Boresight,
				   .Y_pixel1=yPixelLocationCAM1Boresight,
			};

		memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorOneBoresightPixelLocation.paramLength)],
			   &trailer,
			   sizeof(trailer));

		int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
		if (error != 0)
			return 0;
   }
/****************************************************************************************************/
int sendSetSensorTwoBoresightPixelLocationTc(uint16_t X_pixel2, uint16_t Y_pixel2)
		{
// determine size of telecommand
			uint8_t setSensorTwoBoresightPixelLocationTcSize = TELECOMMAND_OVERHEAD + setSensorTwoBoresightPixelLocation.paramLength;

// build  camera capture telecommand
		uint8_t commandBuffer[setSensorTwoBoresightPixelLocationTcSize];

// start of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 51
		commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid51;

//telecommand parameters- Interface control document. Page:27
		_tc_set_SensorTwo_Boresight_Pixel_Location_t params = {
					  .X_pixel2=xPixelLocationCAM2Boresight,
					  .Y_pixel2=yPixelLocationCAM2Boresight,
			};

		memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
			memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorTwoBoresightPixelLocation.paramLength)],
				   &trailer,
				   sizeof(trailer));

			int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
			if (error != 0)
				return 0;
		}
/********************************************************************************************************************/
int sendSetSensorOneMaskTc(uint8_t maskNumber1, uint16_t xMinimum1, uint16_t xMaximum1,uint16_t yMinimum1, uint16_t yMaximum1)
			{
// determine size of telecommand
				uint8_t setSensorOneMaskTcSize = TELECOMMAND_OVERHEAD + setSensorOneMask.paramLength;

// build  camera capture telecommand
				uint8_t commandBuffer[setSensorOneMaskTcSize];

// start of message identifiers
				memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 52
				commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid52;

//telecommand parameters- Interface control document. Page:28
				_tc_set_SensorOne_Mask_t params = {
						   .maskNumber1=areaNumber1,
						   .xMinimum1=lowerXLimit1,
						   .xMaximum1=upperXLimit1,
						   .yMinimum1=lowerYLimit1,
						   .yMaximum1=upperYLimit1,
				};

				memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
				memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorOneMask.paramLength)],
					   &trailer,
					   sizeof(trailer));

				int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
				if (error != 0)
					return 0;
			}
/****************************************************************************************************************/
int sendSetSensorTwoMaskTc(uint8_t maskNumber2, uint16_t xMinimum2, uint16_t xMaximum2,uint16_t yMinimum2, uint16_t yMaximum2)
		{
// determine size of telecommand
		uint8_t setSensorTwoMaskTcSize = TELECOMMAND_OVERHEAD + setSensorTwoMask.paramLength;

// build  camera capture telecommand
		uint8_t commandBuffer[setSensorTwoMaskTcSize];

// start of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 53
		commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid53;

//telecommand parameters- Interface control document. Page:28
		_tc_set_SensorTwo_Mask_t params = {
							   .maskNumber2=areaNumber2,
							   .xMinimum2=lowerXLimit2,
							   .xMaximum2=upperXLimit2,
							   .yMinimum2=lowerYLimit2,
							   .yMaximum2=upperYLimit2,
					};

		memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	    memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorTwoMask.paramLength)],
						 &trailer,
						 sizeof(trailer));

int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
if (error != 0)
	return 0;
		}
/**********************************************************************************************************/
int sendSetSensorOneDistortionCorrectionCoefficientTc(uint16_t Mantissa11, uint8_t Exponent11, uint16_t Mantissa12, uint8_t Exponent12,
		                                              uint16_t Mantissa13, uint8_t Exponent13, uint16_t Mantissa14, uint8_t Exponent14,
		                                              uint16_t Mantissa15, uint8_t Exponent15)
{
// determine size of telecommand
uint8_t setSensorOneDistortionCorrectionCoefficientTcSize = TELECOMMAND_OVERHEAD + setSensorOneDistortionCorrectionCoefficient.paramLength;

// build  camera capture telecommand
uint8_t commandBuffer[setSensorOneDistortionCorrectionCoefficientTcSize];

// start of message identifiers
memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 54
commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid54;

//telecommand parameters- Interface control document. Page:29
_tc_set_SensorOne_distortionCorrectionCoefficient_params_t params = {
		   .Mantissa11=Mantissa11Value,
		   .Exponent11=Exponent11Value,
		   .Mantissa12=Mantissa12Value,
		   .Exponent12=Exponent12Value,
		   .Mantissa13=Mantissa13Value,
		   .Exponent13=Exponent13Value,
		   .Mantissa14=Mantissa14Value,
		   .Exponent14=Exponent14Value,
		   .Mantissa15=Mantissa15Value,
		   .Exponent15=Exponent15Value,
};

		memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

		// end of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorOneDistortionCorrectionCoefficient.paramLength)],
			   &trailer,
			   sizeof(trailer));

		int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
		// return 0 if an error occurs
		if (error != 0)
			return 0;
}
//*****************************************************************************************************************
int sendSetSensorTwoDistortionCorrectionCoefficientTc(uint16_t Mantissa21, uint8_t Exponent21, uint16_t Mantissa22, uint8_t Exponent22,
								                          uint16_t Mantissa23, uint8_t Exponent23, uint16_t Mantissa24, uint8_t Exponent24,
								                          uint16_t Mantissa25, uint8_t Exponent25)
	{
// determine size of telecommand
	uint8_t setSensorTwoDistortionCorrectionCoefficientTcSize = TELECOMMAND_OVERHEAD + setSensorTwoDistortionCorrectionCoefficient.paramLength;

// build  camera capture telecommand
		uint8_t commandBuffer[setSensorTwoDistortionCorrectionCoefficientTcSize];

// start of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 55
		commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid55;

//telecommand parameters- Interface control document. Page:30
_tc_set_SensorTwo_distortionCorrectionCoefficient_t params = {
		   .Mantissa21=Mantissa21Value,
		   .Exponent21=Exponent21Value,
		   .Mantissa22=Mantissa22Value,
		   .Exponent22=Exponent22Value,
		   .Mantissa23=Mantissa23Value,
		   .Exponent23=Exponent23Value,
		   .Mantissa24=Mantissa24Value,
		   .Exponent24=Exponent24Value,
		   .Mantissa25=Mantissa25Value,
		   .Exponent25=Exponent25Value,
};

		memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(setSensorTwoDistortionCorrectionCoefficient.paramLength)],
	 &trailer,
	 sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
	if (error != 0)
		return 0;
	}
//******************************************************************************************************************
int sendInitializeImageDownloadTc(uint8_t sramSelection, uint8_t sramLocation, uint16_t sizeSelection)
	{
// determine size of telecommand
		uint8_t initializeImageDownloadTcSize = TELECOMMAND_OVERHEAD + initializeImageDownload.paramLength;

// build  camera capture telecommand
		uint8_t commandBuffer[initializeImageDownloadTcSize];

// start of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 64
		commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid64;

//telecommand parameters- Interface control document. Page:31
		_tc_initialize_imageDownload_params_t params = {
				   .sramSelection=SRAM1,
				   .sramLocation =TOP_HALVE,
				   .sizeSelection=ImageSize1024,
		};

		memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(initializeImageDownload.paramLength)],
			   &trailer,
			   sizeof(trailer));

		int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
		if (error != 0)
			return 0;
	}
//******************************************************************************************************************
int sendAdvanceimageDownloadTc(uint16_t nextFrameNumber)
{
// determine size of telecommand
		uint8_t advanceimageDownloadTcSize = TELECOMMAND_OVERHEAD + advanceimageDownload.paramLength;

// build  camera capture telecommand
		uint8_t commandBuffer[advanceimageDownloadTcSize];

// start of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

// telecommand ID 65
		commandBuffer[TELECOMMAND_ID_INDEX] = telecommandid65;

//telecommand parameters- Interface control document. Page:31
		_tc_advance_imageDownload_params_t params = {
							   .nextFrameNumber=nextFrameNumberValue,
		};

		memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

// end of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(advanceimageDownload.paramLength)],
				&trailer,
			    sizeof(trailer));

int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
// return 0 if an error occurs
if (error != 0)
	return 0;
}

//********************************************Telemetry request*************************************************************************
 int requestTelecommandAcknowledgeTm(void)
{
	uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
	memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));


// telemetry frame id 3
	telemetryBuffer[2] =telemetryid3;

// end of message identifiers
	memcpy(&telemetryBuffer[TELECOMMAND_TRAILER_INDEX(advanceimageDownload.paramLength)],
			&trailer,
		    sizeof(trailer));

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;

//Receive from UART
	uint8_t acknowledgeBuffer[TELEMETRY_ACK_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, acknowledgeBuffer, TELEMETRY_ACK_SIZE);

	/*if (memcmp(acknowledgeBuffer, ackResponse, sizeof(acknowledgeBuffer)))*/

//Telemetry parameters- Interface control document. Page:19
	if ((acknowledgeBuffer && 0x00F00) == 1) {
//message fails
	   return ERROR_INVALID_PARAM;
	}
	else if ((acknowledgeBuffer && 0x00F00) == 0) {
//no errors
		return 0;
	}
}
/************************************************************************************************************/
int requeststatusTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 0
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid0;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t statusTm[STATUS_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, statusTm, STATUS_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:17
	uint8_t nodeType             = statusTm  &&  ((unsigned int)0x0000000F);
	uint8_t interfaceVersion     = statusTm  &&  ((unsigned int)0x000000F0);
	uint8_t firmWareVersionMajor = statusTm  &&  ((unsigned int)0x00000F00);
	uint8_t firmWareVersionMinor = statusTm  &&  ((unsigned int)0x0000F000);
	uint8_t runTime_Seconds      = statusTm  &&  ((unsigned int)0x00FF0000);
	uint8_t runTime_MilliSeconds = statusTm  &&  ((unsigned int)0xFF000000);
}
/************************************************************************************************************/
int requestCommunicationStatusTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 2
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid2;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

	// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t communicationStatusTm[COMMUNICATION_STATUS_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, communicationStatusTm, COMMUNICATION_STATUS_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:18
	uint8_t tcCounter               = communicationStatusTm && ((unsigned int)0x000000FF);
	uint8_t tlmCounter              = communicationStatusTm && ((unsigned int)0x0000FF00);
	uint8_t tcBufferOverRunFlag     = communicationStatusTm && ((unsigned int)0x000F0000);
	uint8_t i2cTlmReadErrorFlag     = communicationStatusTm && ((unsigned int)0x00F00000);
	uint8_t uartProtocolErrorFlag   = communicationStatusTm && ((unsigned int)0x0F000000);
	uint8_t uartIncompleteErrorFlag = communicationStatusTm && ((unsigned int)0xF0000000);
}
/************************************************************************************************************/
int requestSerialNumberTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 1
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid1;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

	// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t serialNumberTm[SERIAL_NUMBER_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, serialNumberTm, SERIAL_NUMBER_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:18
	uint8_t nodeTypeSerialNumber = serialNumberTm;
}

// TODO: Read about telemetry 20-25
/************************************************************************************************************/
int requestPowerTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 26
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid26;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

	// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t powerTm[POWER_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, powerTm, POWER_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:20
	uint8_t voltage3v3Current = powerTm && ((unsigned int)0x00000000FF);
	uint8_t sram1Current      = powerTm && ((unsigned int)0x000000FF00);
	uint8_t sram2Current      = powerTm && ((unsigned int)0x0000FF0000);
	uint8_t voltage5Current   = powerTm && ((unsigned int)0x00FF000000);
	uint8_t sram1OverCurrent  = powerTm && ((unsigned int)0x0F00000000);
	uint8_t sram2OverCurrent  = powerTm && ((unsigned int)0xF000000000);
}
/************************************************************************************************************/
int requestConfigurationTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 40
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid40;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

	// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t configurationTm[CONFIGURATION_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, configurationTm, CONFIGURATION_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:21
	uint8_t camera1DetectionThreshold = configurationTm && ((unsigned int)0x0000000000000F);
	uint8_t camera2DetectionThreshold = configurationTm && ((unsigned int)0x000000000000F0);
	uint8_t camera1AutoAdjustMode     = configurationTm && ((unsigned int)0x00000000000F00);
	uint8_t camera1Exposure           = configurationTm && ((unsigned int)0x0000000000F000);
	uint8_t camera1AGC                = configurationTm && ((unsigned int)0x00000000F00000);
	uint8_t camera1BlueGain           = configurationTm && ((unsigned int)0x0000000F000000);
	uint8_t camera1RedGain            = configurationTm && ((unsigned int)0x000000F0000000);
	uint8_t camera2AutoAdjustMode     = configurationTm && ((unsigned int)0x00000F00000000);
	uint8_t camera2Exposure           = configurationTm && ((unsigned int)0x0000F000000000);
	uint8_t camera2AGC                = configurationTm && ((unsigned int)0x00F00000000000);
	uint8_t camera2BlueGain           = configurationTm && ((unsigned int)0x0F000000000000);
	uint8_t camera2RedGain            = configurationTm && ((unsigned int)0xF0000000000000);
}
/************************************************************************************************************/
int requestImageFrameTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 64
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid64;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

	// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t imageFrameTm[IMAGE_FRAME_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, imageFrameTm, IMAGE_FRAME_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:21
	uint8_t imageBytes = imageFrameTm;

}
/************************************************************************************************************/
int requestImageFrameInfoTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 65
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid65;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;;
//Receive from UART
    uint8_t imageFrameInfoTm[IMAGE_FRAME_INFO_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, imageFrameInfoTm, IMAGE_FRAME_INFO_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:22
	uint8_t imageFrameNumber = imageFrameInfoTm && ((unsigned int)0x0FF);
	uint8_t checksum         = imageFrameInfoTm && ((unsigned int)0xF00);
}
/************************************************************************************************************/
int requestFullImageSram1Location1Tm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 66
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid66;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t fullImageSram1Location1Tm[FULL_IMAGE_SRAM1_LOCATION1_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, fullImageSram1Location1Tm, FULL_IMAGE_SRAM1_LOCATION1_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:22
	uint8_t fullImageSram1Location1Bytes = fullImageSram1Location1Tm;
}
/************************************************************************************************************/
int requestFullImageSram1Location2Tm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 67
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid67;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t fullImageSram1Location2Tm[FULL_IMAGE_SRAM1_LOCATION2_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, fullImageSram1Location2Tm, FULL_IMAGE_SRAM1_LOCATION2_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:22
	uint8_t fullImageSram1Location2Bytes = fullImageSram1Location2Tm;
}
/************************************************************************************************************/
int requestFullImageSram2Location1Tm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 68
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid68;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t fullImageSram2Location1Tm[FULL_IMAGE_SRAM2_LOCATION1_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, fullImageSram2Location1Tm, FULL_IMAGE_SRAM2_LOCATION1_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:22
	uint8_t fullImageSram2Location1Bytes = fullImageSram2Location1Tm;
}
/************************************************************************************************************/
int requestFullImageSram2Location2Tm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 69
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid69;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t fullImageSram2Location2Tm[FULL_IMAGE_SRAM2_LOCATION2_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, fullImageSram2Location2Tm, FULL_IMAGE_SRAM2_LOCATION2_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:22
	uint8_t  fullImageSram2Location2Bytes = fullImageSram2Location2Tm;
}
/************************************************************************************************************/
int requestReadSensor1MaskTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 72
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid72;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t readSensor1MaskTm[READ_SENSOR1_MASK_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, readSensor1MaskTm, READ_SENSOR1_MASK_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:23
	//TODO complete this part. Maybe by using counter.
}
/************************************************************************************************************/
int requestReadSensor2MaskTm(void)
{
uint8_t telemetryBuffer[REQUEST_TELEMETRY_SIZE];

// start of message identifiers
memcpy(&telemetryBuffer[REQUEST_TELEMETRY_SIZE], &header, sizeof(header));

// telemetry frame id 73
telemetryBuffer[TELEMETRY_ID_INDEX] =telemetryid73;

//Send to UART
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer , REQUEST_TELEMETRY_SIZE);

// return 0 if an error occurs
	if (error != 0)
		return 0;
//Receive from UART
    uint8_t readSensor2MaskTm[READ_SENSOR1_MASK_TELEMETRY_SIZE] = { 0 };
	error = uartReceive(UART_CAMERA_BUS, readSensor2MaskTm, READ_SENSOR2_MASK_TELEMETRY_SIZE);

//Telemetry parameters- Interface control document. Page:24
	//TODO complete this part. Maybe by using counter.
}
