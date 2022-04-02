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
/* Struct used to define ADCS Function*/
typedef struct _detection_results_t {
	uint16_t sunSensorX;
	uint16_t sunSensorY;
	uint16_t sunSensorZ;
	uint16_t imageSensorX;
	uint16_t imageSensorY;
	uint16_t imageSensorZ;
} detection_results_t;

typedef struct _CameraTelemetry_PowerTelemetry {
    float current_3V3;
    float current_5V;
    float current_SRAM_1;
    float current_SRAM_2;
    uint8_t overcurrent_SRAM_1;
    uint8_t overcurrent_SRAM_2;
} CameraTelemetry_PowerTelemetry;

typedef struct _CameraTelemetry_ConfiguarationTelemetry {
    uint8_t detectionThreshold;
    uint8_t autoAdjustMode;
    uint16_t exposure;
    uint8_t autoGainControl;
    uint8_t blueGain;
    uint8_t redGain;
} CameraTelemetry_ConfigurationTelemetry;

/* Struct for Camera Telemetry Collection Function */
typedef struct _CameraTelemetry {
    uint16_t upTime;
    CameraTelemetry_PowerTelemetry powerTelemetry;
    CameraTelemetry_ConfigurationTelemetry cameraOneTelemetry;
    CameraTelemetry_ConfigurationTelemetry cameraTwoTelemetry;
} CameraTelemetry;



/* Struct for telmetry status, ID 0*/
typedef struct _tlm_status_t {
	uint8_t  nodeType;
	uint8_t  interfaceVersion;
	uint8_t  firmwareVersionMajor;
	uint8_t  firmwareVersionMinor;
	uint16_t runtimeSeconds;
	uint16_t runtimeMSeconds;
} tlm_status_t;

/* Struct for telmetry Serial number, ID 2 */
typedef struct _tlm_communication_status_t {
	uint16_t tcCounter;
	uint16_t tlmCounter;
	uint8_t  tcBufferOverunFlag;
	uint8_t  i2ctlmReadErrorFlag;
	uint8_t	 uarttlmProtocolErrorFlag;
	uint8_t  uartIncompleteMsgFlag;
} tlm_communication_status_t;

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
int captureAndDownload(void);
int detectionAndInterpret(detection_results_t *data);
int cameraTelemetry(CameraTelemetry *cameraTelemetry);

#endif /* RCAMERA_H_ */
