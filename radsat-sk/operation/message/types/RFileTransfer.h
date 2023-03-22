
#ifndef RFILETRANSFER
#define RFILETRANSFER

#include <stdint.h>
#pragma pack(1)
/* Enum definitions */
typedef enum _image_type_t {
    image_type_t_FullResolution = 0,
    image_type_t_HalfResolution = 1,
    image_type_t_QuarterResolution = 2,
    image_type_t_Thumbnail = 3
} image_type_t;




typedef struct _camera_configuration_telemetry {
    uint8_t detectionThreshold;
    uint8_t autoAdjustMode;
    uint16_t exposure;
    uint8_t autoGainControl;
    uint8_t blueGain;
    uint8_t redGain;
} camera_configuration_telemetry;

typedef struct _camera_power_telemetry {
	uint16_t current_3V3;
    uint16_t current_5V;
    uint16_t current_SRAM_1;
    uint16_t current_SRAM_2;
    uint16_t overcurrent_SRAM_1;
    uint16_t overcurrent_SRAM_2;
} camera_power_telemetry;

// "HHHHHHHBBHBBBBBHBBB"
typedef struct _camera_telemetry {
    uint16_t uptime;
    camera_power_telemetry powerTelemetry;
    camera_configuration_telemetry cameraOneTelemetry;
    camera_configuration_telemetry cameraTwoTelemetry;
} camera_telemetry;

typedef struct {
	uint8_t size;
	uint8_t bytes[200];
} image_packet_data_t;

typedef struct _image_packet {
    uint32_t id;
    image_type_t type;
    image_packet_data_t data;
} image_packet;


typedef struct _component_error_report {
    uint32_t component;
    int32_t error;
} component_error_report;

typedef struct _error_record {
    uint32_t timeRecorded;
    uint8_t count;
} error_record;

typedef struct _error_report_summary {
    uint8_t moduleErrorCount[29];
    uint8_t componentErrorCount[19];
} error_report_summary;

typedef struct _module_error_report {
    uint32_t module;
    int32_t error;
} module_error_report;



/* IHHIHH */
typedef struct __attribute__ ((__packed__)) _adcs_detection {
	uint32_t sunTimestamp;
	uint16_t sunAlphaAngle;
	uint16_t sunBetaAngle;
	uint32_t nadirTimestamp;
	uint16_t nadirAlphaAngle;
	uint16_t nadirBetaAngle;
} adcs_detection;


//
typedef struct __attribute__ ((__packed__)) _adcs_burst {
	adcs_detection detections[15];
} adcs_burst;

/** Antenna Deployment status Struct
 * H
 */
typedef struct __attribute__ ((__packed__)) {
	unsigned short armed : 1,
	ant4Deploying : 1,
	ant4Timeout : 1,
	ant4Undeployed : 1,
	: 1,
	ant3Deploying : 1,
	ant3Timeout : 1,
	ant3Undeployed : 1,
	ignoreFlag : 1,
	ant2Deploying : 1,
	ant2Timeout : 1,
	ant2Undeployed : 1,
	: 1,
	ant1Deploying : 1,
	ant1Timeout : 1,
	ant1Undeployed : 1;
} antenna_deployment_status; ///< Struct with individual fields of ISIS AntS status data


/** Struct that holds telemetry for one side of the Antenna
 * HfI
 */
typedef struct __attribute__ ((__packed__)) _antenna_telemetry_side_t {
	antenna_deployment_status deployStatus;	///< Antenna Deployment status.
	float boardTemp; 					 		///< Antenna board temperature.
	unsigned int uptime;							///< Antenna Uptime in Seconds.
} antenna_side_data;


// "HHHHHfIHHHHHfI"
typedef struct _antenna_telemetry {
    antenna_side_data sideA;
    antenna_side_data sideB;
} antenna_telemetry;


// "fffffffffff"
typedef struct _battery_telemetry {
    float outputVoltageBatteryBus;
    float outputVoltage5VBus;
    float outputVoltage3V3Bus;
    float outputCurrentBatteryBus;
    float outputCurrent5VBus;
    float outputCurrent3V3Bus;
    float batteryCurrentDirection;
    float motherboardTemp;
    float daughterboardTemp1;
    float daughterboardTemp2;
    float daughterboardTemp3;
} battery_telemetry;


// TODO: MAKE ADCS
// TODO: Add OBC fill
typedef struct _obc_adc_values_t {
    uint16_t temperature;
    uint16_t voltage_3v3in;
    uint16_t voltage_3v3;
    uint16_t voltage_2v5;
    uint16_t voltage_1v8;
    uint16_t voltage_1v0;
    uint16_t current_3v3;
    uint16_t current_1v8;
    uint16_t current_1v0;
    uint16_t voltage_rtc;
} obc_adc_values_t;

// python recipe "IIIBHHHHHHHHHH"
typedef struct _obc_telemetry {
    uint32_t supervisorUptime;
    uint32_t iobcUptime;
    uint32_t iobcResetCount;
    uint8_t adcUpdateFlag;
    obc_adc_values_t obc_adc_values;
} obc_telemetry;

/*
// "fffffffffII"
typedef struct _receiver_telemetry {
    float rxDoppler;
    float rxRssi;
    float busVoltage;
    float totalCurrent;
    float txCurrent;
    float rxCurrent;
    float powerAmplifierCurrent;
    float powerAmplifierTemperature;
    float boardTemperature;
    uint32_t uptime;
    uint32_t frames;
} receiver_telemetry;

// "fffffffffI"
typedef struct _transmitter_telemetry {
    float reflectedPower;
    float forwardPower;
    float busVoltage;
    float totalCurrent;
    float txCurrent;
    float rxCurrent;
    float powerAmplifierCurrent;
    float powerAmplifierTemperature;
    float boardTemperature;
    uint32_t uptime;
} transmitter_telemetry;
*/
// "HHHHHHHHHHH"
typedef struct __attribute__ ((__packed__)) _receiver_telemetry {
    uint16_t rxDoppler;
    uint16_t rxRssi;
    uint16_t busVoltage;
    uint16_t totalCurrent;
    uint16_t txCurrent;
    uint16_t rxCurrent;
    uint16_t powerAmplifierCurrent;
    uint16_t powerAmplifierTemperature;
    uint16_t boardTemperature;
    uint16_t uptime;
    uint16_t frames;
} receiver_telemetry;

// "HHHHHHHHHH"
typedef struct __attribute__ ((__packed__)) _transmitter_telemetry {
    uint16_t reflectedPower;
    uint16_t forwardPower;
    uint16_t busVoltage;
    uint16_t totalCurrent;
    uint16_t txCurrent;
    uint16_t rxCurrent;
    uint16_t powerAmplifierCurrent;
    uint16_t powerAmplifierTemperature;
    uint16_t boardTemperature;
    uint16_t uptime;
} transmitter_telemetry;

// "HHHHHHHHHHHHHHHHHHHHH"
typedef struct __attribute__ ((__packed__))_transceiver_telemetry {
    receiver_telemetry receiver;
    transmitter_telemetry transmitter;
} transceiver_telemetry;


// "ffffffff"
typedef struct _dosimeter_board_data {
    float channelZero;
    float channelOne;
    float channelTwo;
    float channelThree;
    float channelFour;
    float channelFive;
    float channelSix;
    float channelSeven;
} dosimeter_board_data;

// "ffffffffffffffff"
typedef struct _dosimeter_data {
    dosimeter_board_data boardOne;
    dosimeter_board_data boardTwo;
} dosimeter_data;

/*
// "fffffffff"
typedef struct _sun_sensor_data {
	float BCR1Voltage;
	float SA1ACurrent;
	float SA1BCurrent;
	float BCR2Voltage;
	float SA2ACurrent;
	float SA2BCurrent;
	float BCR3Voltage;
	float SA3ACurrent;
	float SA3BCurrent;
} sun_sensor_data;

// "fffffffffffffff"
typedef struct _eps_telemetry {
    sun_sensor_data sunSensorData;
    float outputVoltageBCR;
    float outputVoltageBatteryBus;
    float outputVoltage5VBus;
    float outputVoltage3V3Bus;
    float outputCurrentBCR_mA;
    float outputCurrentBatteryBus;
    float outputCurrent5VBus;
    float outputCurrent3V3Bus;
    float PdbTemperature;
} eps_telemetry;
*/

// "HHHHHHHHH"
typedef struct _sun_sensor_data {
	uint16_t BCR1Voltage;
	uint16_t SA1ACurrent;
	uint16_t SA1BCurrent;
	uint16_t BCR2Voltage;
	uint16_t SA2ACurrent;
	uint16_t SA2BCurrent;
	uint16_t BCR3Voltage;
	uint16_t SA3ACurrent;
	uint16_t SA3BCurrent;
} sun_sensor_data;

// "HHHHHHHHHHHHHHHHHH"
typedef struct _eps_telemetry {
    sun_sensor_data sunSensorData;
    uint16_t outputVoltageBCR;
    uint16_t outputVoltageBatteryBus;
    uint16_t outputVoltage5VBus;
    uint16_t outputVoltage3V3Bus;
    uint16_t outputCurrentBCR_mA;
    uint16_t outputCurrentBatteryBus;
    uint16_t outputCurrent5VBus;
    uint16_t outputCurrent3V3Bus;
    uint16_t PdbTemperature;
} eps_telemetry;

typedef union _file_transfer_message {
	obc_telemetry ObcTelemetry;
	transceiver_telemetry TransceiverTelemetry;
	camera_telemetry CameraTelemetry;
	eps_telemetry EpsTelemetry;
	battery_telemetry BatteryTelemetry;
	antenna_telemetry AntennaTelemetry;
	dosimeter_data DosimeterData;
	image_packet ImagePacket;
	module_error_report ModuleErrorReport;
	component_error_report ComponentErrorReport;
	error_report_summary ErrorReportSummary;
	adcs_burst	AdcsBerst;
} file_transfer_message;

#endif