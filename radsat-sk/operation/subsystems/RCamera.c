/**
 * @file RCamera.c
 * @date December 23, 2021
 * @author Shiva Moghtaderi (shm153) & Addi Amaya (caa746)
 */

#include <RCamera.h>
#include <RUart.h>
#include <hal/errors.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <RImage.h>
#include <RADCS.h>
#include <RCommon.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

#define TELEMETRY_0                  	((uint8_t) 0x80)
#define TELEMETRY_26                	((uint8_t) 0x9A)
#define TELEMETRY_40                    ((uint8_t) 0xA8)
#define TELEMETRY_72                    ((uint8_t) 0xC8)
#define TELEMETRY_73                    ((uint8_t) 0xC9)

#define TELEMETRY_0_LEN					((uint8_t) 12)
#define TELEMETRY_26_LEN				((uint8_t) 14)
#define TELEMETRY_40_LEN				((uint8_t) 18)
#define TELEMETRY_72_LEN			    ((uint8_t) 44)
#define TELEMETRY_73_LEN			    ((uint8_t) 44)
#define TELEMETRY_REQUEST_LEN			((uint8_t) 1)
#define TELEMETRY_REPLY_SIZE_8			((uint8_t) 8)
#define TELEMETRY_REPLY_SIZE_10			((uint8_t) 10)
#define TELEMETRY_REPLY_SIZE_14			((uint8_t) 14)
#define TELEMETRY_REPLY_SIZE_40			((uint8_t) 40)

#define TELEMETRY_OFFSET_0              ((uint8_t) 2)
#define TELEMETRY_OFFSET_1              ((uint8_t) 3)
#define TELEMETRY_OFFSET_2              ((uint8_t) 4)
#define TELEMETRY_OFFSET_3              ((uint8_t) 5)
#define TELEMETRY_OFFSET_4              ((uint8_t) 6)
#define TELEMETRY_OFFSET_5              ((uint8_t) 7)
#define TELEMETRY_OFFSET_6              ((uint8_t) 8)
#define TELEMETRY_OFFSET_7              ((uint8_t) 9)
#define TELEMETRY_OFFSET_8              ((uint8_t) 10)
#define TELEMETRY_OFFSET_9              ((uint8_t) 11)
#define TELEMETRY_OFFSET_10             ((uint8_t) 12)
#define TELEMETRY_OFFSET_11             ((uint8_t) 13)
#define TELEMETRY_OFFSET_12             ((uint8_t) 14)
#define TELEMETRY_OFFSET_13             ((uint8_t) 15)
#define TELEMETRY_OFFSET_14             ((uint8_t) 16)
#define TELEMETRY_OFFSET_16             ((uint8_t) 18)
#define TELEMETRY_OFFSET_18             ((uint8_t) 20)
#define TELEMETRY_OFFSET_20             ((uint8_t) 22)
#define TELEMETRY_OFFSET_22             ((uint8_t) 24)
#define TELEMETRY_OFFSET_24             ((uint8_t) 26)
#define TELEMETRY_OFFSET_26             ((uint8_t) 28)
#define TELEMETRY_OFFSET_28             ((uint8_t) 30)
#define TELEMETRY_OFFSET_30             ((uint8_t) 32)
#define TELEMETRY_OFFSET_32             ((uint8_t) 34)
#define TELEMETRY_OFFSET_34             ((uint8_t) 36)
#define TELEMETRY_OFFSET_36             ((uint8_t) 38)
#define TELEMETRY_OFFSET_38             ((uint8_t) 40)
#define TELEMETRY_ID_OFFSET		        ((uint8_t) 2)

#define BASE_MESSAGE_LEN				((uint8_t) 4)
#define START_IDENTIFIER1               ((uint16_t) 0x1F)
#define START_IDENTIFIER2               ((uint16_t) 0x7F)
#define FILLER							((uint16_t) 0x00)
#define END_IDENTIFIER1                 ((uint16_t) 0x1F)
#define END_IDENTIFIER2                 ((uint16_t) 0xFF)

#define SUN_SENSOR	         	        ((uint8_t) 0)
#define IMAGE_SENSOR	                ((uint8_t) 1)
#define SRAM1                           ((uint8_t) 0)
#define SRAM2                           ((uint8_t) 1)


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/
static uint8_t * MessageBuilder(uint8_t response_size);
static int tlmStatus(tlm_status_t *telemetry_reply);
static int tlmPower(tlm_power_t *telemetry_reply);
static int tlmConfig(tlm_config_t *telemetry_reply);
static int tlmReadSensorOneMask(tlm_read_sensor_mask_t *telemetry_reply);
static int tlmReadSensorTwoMask(tlm_read_sensor_mask_t *telemetry_reply);
/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int captureAndDownload(void){
	return 0;
}

int detectionAndInterpret(void){
	return 0;
}

int cameraTelemetry(void){
	return 0;
}

int cameraConfig(void){
	return 0;
}

/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/*
 * Used to dynamically allocated buffer sizes as each telemetry and telecommand
 * require different sizes
 *
 * @note must use Free() to free the allocated memory when finished using the buffer
 * @param response_size defines how many data bytes are required in the buffer
 * @return dynamically allocated buffer
 * */
static uint8_t * MessageBuilder(uint8_t response_size){

	// Define the total size the buffer should be
    uint8_t total_buffer_length = response_size + BASE_MESSAGE_LEN;

    // Dynamically Allocate a Buffer for telemetry response
    uint8_t* tlmBuffer = (uint8_t*) malloc(total_buffer_length * sizeof(uint8_t));

    // Initialize all elements in the buffer with zero
    for (uint8_t i = 0; i < total_buffer_length; i++){
    	tlmBuffer[i] = 0;
    }

    // Fill Buffer with default values
    for(uint8_t i = 0; i<total_buffer_length;i++){
        if (i == 0){
        	tlmBuffer[i] = START_IDENTIFIER1;
        }
        else if (i == 1){
            tlmBuffer[i] = START_IDENTIFIER2;
        }
        else if (i == total_buffer_length-2) {
        	tlmBuffer[i] = END_IDENTIFIER1;
        }
        else if (i == total_buffer_length-1) {
        	tlmBuffer[i] = END_IDENTIFIER2;
        }
        else{
        	tlmBuffer[i] = FILLER;
        }
    }

    return tlmBuffer;

}

/*
 * Used to retrieve the status of the Camera (TLM ID 0)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmStatus(tlm_status_t *telemetry_reply) {
	uint8_t *telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// ensure the input pointers are not NULL
	if (telemetry_reply == 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_0;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_8);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_0_LEN);

	if (error != 0){
		free(telemetryBuffer);
		return E_GENERIC;
	}

	// Fill telemetry reply, data from uart read starts at index two
	telemetry_reply->nodeType = telemetryBuffer[TELEMETRY_OFFSET_0];
	telemetry_reply->interfaceVersion = telemetryBuffer[TELEMETRY_OFFSET_1];
	telemetry_reply->firmwareVersionMajor = telemetryBuffer[TELEMETRY_OFFSET_2];
	telemetry_reply->firmwareVersionMinor = telemetryBuffer[TELEMETRY_OFFSET_3];
	memcpy(&telemetry_reply->runtimeSeconds, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->runtimeSeconds));
	memcpy(&telemetry_reply->runtimeMSeconds, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->runtimeMSeconds));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;

}

/*
 * Used to retrieve the power status of the Camera (TLM ID 26)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmPower(tlm_power_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// ensure the input pointers are not NULL
	if (telemetry_reply == 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_26;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_10);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_26_LEN);

	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->threeVcurrent, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->threeVcurrent));
	memcpy(&telemetry_reply->sramOneCurrent, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->sramOneCurrent));
	memcpy(&telemetry_reply->sramTwoCurrent, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->sramTwoCurrent));
	memcpy(&telemetry_reply->fiveVcurrent, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->fiveVcurrent));
	telemetry_reply->sramOneOverCurrent = telemetryBuffer[TELEMETRY_OFFSET_8];
	telemetry_reply->sramTwoOverCurrent = telemetryBuffer[TELEMETRY_OFFSET_9];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to retrieve the configuration status of the Camera (TLM ID 40)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmConfig(tlm_config_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// ensure the input pointers are not NULL
	if (telemetry_reply == 0)
		return E_GENERIC;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_40;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_14);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_40_LEN);

	// Error Check on uartRecieve, if error, free allocated buffer
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	telemetry_reply->cameraOneDetectionThrshld = telemetryBuffer[TELEMETRY_OFFSET_0];
	telemetry_reply->cameraTwoDetectionThrshld = telemetryBuffer[TELEMETRY_OFFSET_1];
	telemetry_reply->cameraOneAutoAdjustMode = telemetryBuffer[TELEMETRY_OFFSET_2];
	memcpy(&telemetry_reply->cameraOneExposure, &telemetryBuffer[TELEMETRY_OFFSET_3], sizeof(telemetry_reply->cameraOneExposure));
	telemetry_reply->cameraOneAGC = telemetryBuffer[TELEMETRY_OFFSET_5];
	telemetry_reply->cameraOneBlueGain = telemetryBuffer[TELEMETRY_OFFSET_6];
	telemetry_reply->cameraOneRedGain = telemetryBuffer[TELEMETRY_OFFSET_7];
	telemetry_reply->cameraTwoAutoAdjustMode = telemetryBuffer[TELEMETRY_OFFSET_8];
	memcpy(&telemetry_reply->cameraTwoExposure, &telemetryBuffer[TELEMETRY_OFFSET_9], sizeof(telemetry_reply->cameraTwoExposure));
	telemetry_reply->cameraTwoAGC = telemetryBuffer[TELEMETRY_OFFSET_11];
	telemetry_reply->cameraTwoBlueGain = telemetryBuffer[TELEMETRY_OFFSET_12];
	telemetry_reply->cameraTwoRedGain = telemetryBuffer[TELEMETRY_OFFSET_13];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to retrieve the mask configuration of Sensor one of the Camera (TLM ID 72)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmReadSensorOneMask(tlm_read_sensor_mask_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_72;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_40);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_72_LEN);

	// Error Check on uartRecieve, if error, free allocated buffer
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->MinXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->MinXAreaOne));
	memcpy(&telemetry_reply->MaxXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->MaxXAreaOne));
	memcpy(&telemetry_reply->MinYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->MinYAreaOne));
	memcpy(&telemetry_reply->MaxYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->MaxYAreaOne));
	memcpy(&telemetry_reply->MinXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_8], sizeof(telemetry_reply->MinXAreaTwo));
	memcpy(&telemetry_reply->MaxXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_10], sizeof(telemetry_reply->MaxXAreaTwo));
	memcpy(&telemetry_reply->MinYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_12], sizeof(telemetry_reply->MinYAreaTwo));
	memcpy(&telemetry_reply->MaxYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_14], sizeof(telemetry_reply->MaxYAreaTwo));
	memcpy(&telemetry_reply->MinXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_16], sizeof(telemetry_reply->MinXAreaThree));
	memcpy(&telemetry_reply->MaxXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_18], sizeof(telemetry_reply->MaxXAreaThree));
	memcpy(&telemetry_reply->MinYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_20], sizeof(telemetry_reply->MinYAreaThree));
	memcpy(&telemetry_reply->MaxYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_22], sizeof(telemetry_reply->MaxYAreaThree));
	memcpy(&telemetry_reply->MinXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_24], sizeof(telemetry_reply->MinXAreaFourth));
	memcpy(&telemetry_reply->MaxXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_26], sizeof(telemetry_reply->MaxXAreaFourth));
	memcpy(&telemetry_reply->MinYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_28], sizeof(telemetry_reply->MinYAreaFourth));
	memcpy(&telemetry_reply->MaxYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_30], sizeof(telemetry_reply->MaxYAreaFourth));
	memcpy(&telemetry_reply->MinXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_32], sizeof(telemetry_reply->MinXAreaFifth));
	memcpy(&telemetry_reply->MaxXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_34], sizeof(telemetry_reply->MaxXAreaFifth));
	memcpy(&telemetry_reply->MinYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_36], sizeof(telemetry_reply->MinYAreaFifth));
	memcpy(&telemetry_reply->MaxYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_38], sizeof(telemetry_reply->MaxYAreaFifth));


	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

/*
 * Used to retrieve the mask configuration of Sensor one of the Camera (TLM ID 73)
 *
 * @param telemetry_reply struct that holds all the information from the telemetry response
 * @return error of telecommand attempt. 0 on success, otherwise failure
 * */
static int tlmReadSensorTwoMask(tlm_read_sensor_mask_t *telemetry_reply) {
	uint8_t* telemetryBuffer;
	uint16_t sizeOfBuffer;
	int error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REQUEST_LEN);
	sizeOfBuffer = TELEMETRY_REQUEST_LEN + BASE_MESSAGE_LEN;

    // Fill buffer with telemetry ID
	telemetryBuffer[TELEMETRY_ID_OFFSET] = TELEMETRY_73;

    // Send Telemetry Request
	error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, sizeOfBuffer);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = MessageBuilder(TELEMETRY_REPLY_SIZE_40);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_73_LEN);

	// Error Check on uartRecieve, if error, free allocated buffer
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->MinXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->MinXAreaOne));
	memcpy(&telemetry_reply->MaxXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->MaxXAreaOne));
	memcpy(&telemetry_reply->MinYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->MinYAreaOne));
	memcpy(&telemetry_reply->MaxYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->MaxYAreaOne));
	memcpy(&telemetry_reply->MinXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_8], sizeof(telemetry_reply->MinXAreaTwo));
	memcpy(&telemetry_reply->MaxXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_10], sizeof(telemetry_reply->MaxXAreaTwo));
	memcpy(&telemetry_reply->MinYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_12], sizeof(telemetry_reply->MinYAreaTwo));
	memcpy(&telemetry_reply->MaxYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_14], sizeof(telemetry_reply->MaxYAreaTwo));
	memcpy(&telemetry_reply->MinXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_16], sizeof(telemetry_reply->MinXAreaThree));
	memcpy(&telemetry_reply->MaxXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_18], sizeof(telemetry_reply->MaxXAreaThree));
	memcpy(&telemetry_reply->MinYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_20], sizeof(telemetry_reply->MinYAreaThree));
	memcpy(&telemetry_reply->MaxYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_22], sizeof(telemetry_reply->MaxYAreaThree));
	memcpy(&telemetry_reply->MinXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_24], sizeof(telemetry_reply->MinXAreaFourth));
	memcpy(&telemetry_reply->MaxXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_26], sizeof(telemetry_reply->MaxXAreaFourth));
	memcpy(&telemetry_reply->MinYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_28], sizeof(telemetry_reply->MinYAreaFourth));
	memcpy(&telemetry_reply->MaxYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_30], sizeof(telemetry_reply->MaxYAreaFourth));
	memcpy(&telemetry_reply->MinXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_32], sizeof(telemetry_reply->MinXAreaFifth));
	memcpy(&telemetry_reply->MaxXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_34], sizeof(telemetry_reply->MaxXAreaFifth));
	memcpy(&telemetry_reply->MinYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_36], sizeof(telemetry_reply->MinYAreaFifth));
	memcpy(&telemetry_reply->MaxYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_38], sizeof(telemetry_reply->MaxYAreaFifth));


	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	return SUCCESS;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------

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

#define START_IDENTIFIER1               ((uint16_t) 0x1F)
#define START_IDENTIFIER2               ((uint16_t) 0x7F)
#define FILLER							((uint16_t) 0x00)
#define END_IDENTIFIER1                 ((uint16_t) 0x1F)
#define END_IDENTIFIER2                 ((uint16_t) 0xFF)

#define TELECOMMAND_ID_0                ((uint8_t) 0x00)
#define TELECOMMAND_ID_11               ((uint8_t) 0x0B)
#define TELECOMMAND_ID_20               ((uint8_t) 0x14)
#define TELECOMMAND_ID_21               ((uint8_t) 0x15) //In RImage.c
#define TELECOMMAND_ID_40               ((uint8_t) 0x28)
#define TELECOMMAND_ID_41               ((uint8_t) 0x29)
#define TELECOMMAND_ID_42               ((uint8_t) 0x2A)
#define TELECOMMAND_ID_43               ((uint8_t) 0x2B)
#define TELECOMMAND_ID_44               ((uint8_t) 0x2C)
#define TELECOMMAND_ID_45               ((uint8_t) 0x2D)
#define TELECOMMAND_ID_50               ((uint8_t) 0x32)
#define TELECOMMAND_ID_51               ((uint8_t) 0x33)
#define TELECOMMAND_ID_52               ((uint8_t) 0x34)
#define TELECOMMAND_ID_53               ((uint8_t) 0x35)
#define TELECOMMAND_ID_54               ((uint8_t) 0x36)
#define TELECOMMAND_ID_55               ((uint8_t) 0x37)
#define TELECOMMAND_ID_64               ((uint8_t) 0x40) //In RImage.c
#define TELECOMMAND_ID_65               ((uint8_t) 0x41) //In RImage.c

#define CAPTURE_IMAGE_CMD_SIZE	        ((uint8_t) 3)
#define CAPTURE_IMAGE_TC_SIZE	        ((uint8_t) 8)
#define TELEMETRY_ACK_SIZE	            ((uint8_t) 5)

#define TELECOMMAND_OVERHEAD	                ((uint8_t) (sizeof(tc_header_t) + sizeof(tc_trailer_t) + sizeof(telecommand_id_t)))
#define TELECOMMAND_HEADER_INDEX	            ((uint8_t)  0)
#define TELECOMMAND_ID_INDEX		            ((uint8_t)  sizeof(tc_header_t))
#define TELECOMMAND_PARAM_INDEX		            ((uint8_t)  (TELECOMMAND_ID_INDEX + sizeof(telecommand_id_t)))
#define TELECOMMAND_TRAILER_INDEX(paramLength)	((uint8_t)  (TELECOMMAND_PARAM_INDEX + paramLength))

#define RESET_COMMUNICATION_INTERFACES  ((uint8_t) 1)
#define RESET_CAMERAS                   ((uint8_t) 2)
#define REST_MCU                        ((uint8_t) 3)

#define CLEAR_SRAM1_FLAG                ((uint8_t) 0)
#define CLEAR_SRAM2_FLAG                ((uint8_t) 1)
#define CLEAR_BOTH_FLAGS                ((uint8_t) 2)

#define CAMERA_ONE	                    ((uint8_t) 0)
#define CAMERA_TWO	                    ((uint8_t) 1)

#define IMAGE_SENSOR	                ((uint8_t) CAMERA_TWO)
#define SUN_SENSOR	         	        ((uint8_t) CAMERA_ONE)

#define SRAM1                           ((uint8_t) 0)
#define SRAM2                           ((uint8_t) 1)

#define TOP_HALVE                       ((uint8_t) 0)
#define BOTTOM_HALVE                    ((uint8_t) 1)

#define DETECTION_THRESHOLD_VALUE1      ((uint8_t) 1)
#define DETECTION_THRESHOLD_VALUE2      ((uint8_t) 1)

#define AUTOADJUST_DISABLED             ((uint8_t) 0)
#define AUTOADJUST_ENABLED              ((uint8_t) 1)

#define EXPOSURE_REGISTER_VALUE1        ((uint8_t) 1)
#define GAIN_CONTROL_REGISTER1          ((uint8_t) 1)
#define BLUE_GAIN_CONTROL_REGISTER1     ((uint8_t) 1)
#define RED_GAIN_CONTROL_REGISTER1      ((uint8_t) 1)

#define EXPOSURE_REGISTER_VALUE2        ((uint8_t) 1)
#define GAIN_CONTROL_REGISTER2          ((uint8_t) 1)
#define BLUE_GAIN_CONTROL_REGISTER2     ((uint8_t) 1)
#define RED_GAIN_CONTROL_REGISTER2      ((uint8_t) 1)

#define X_PIXEL_LOCATION_CAM1_BORESIGHT ((uint8_t) 1)
#define Y_PIXEL_LOCATION_CAM1_BORESIGHT ((uint8_t) 1)

#define X_PIXEL_LOCATION_CAM2_BORESIGHT ((uint8_t) 1)
#define Y_PIXEL_LOCATION_CAM2_BORESIGHT ((uint8_t) 1)

#define AREA_NUMBER1                    ((uint8_t) 1)
#define LOWER_X_LIMIT1                  ((uint8_t) 1)
#define UPPER_X_LIMIT1                  ((uint8_t) 1)
#define LOWER_Y_LIMIT1                  ((uint8_t) 1)
#define UPPER_Y_LIMIT1                  ((uint8_t) 1)

#define AREA_NUMBER2                    ((uint8_t) 1)
#define LOWER_X_LIMIT2                  ((uint8_t) 1)
#define UPPER_X_LIMIT2                  ((uint8_t) 1)
#define LOWER_Y_LIMIT2                  ((uint8_t) 1)
#define UPPER_Y_LIMIT2                  ((uint8_t) 1)

#define MANTISSA11_VALUE                ((uint8_t) 1)
#define EXPONENT11_VALUE                ((uint8_t) 1)
#define MANTISSA12_VALUE                ((uint8_t) 1)
#define EXPONENT12_VALUE                ((uint8_t) 1)
#define MANTISSA13_VALUE                ((uint8_t) 1)
#define EXPONENT13_VALUE                ((uint8_t) 1)
#define MANTISSA14_VALUE                ((uint8_t) 1)
#define EXPONENT14_VALUE                ((uint8_t) 1)
#define MANTISSA15_VALUE                ((uint8_t) 1)
#define EXPONENT15_VALUE                ((uint8_t) 1)

#define MANTISSA21_VALUE                ((uint8_t) 1)
#define EXPONENT21_VALUE                ((uint8_t) 1)
#define MANTISSA22_VALUE                ((uint8_t) 1)
#define EXPONENT22_VALUE                ((uint8_t) 1)
#define MANTISSA23_VALUE                ((uint8_t) 1)
#define EXPONENT23_VALUE                ((uint8_t) 1)
#define MANTISSA24_VALUE                ((uint8_t) 1)
#define EXPONENT24_VALUE                ((uint8_t) 1)
#define MANTISSA25_VALUE                ((uint8_t) 1)
#define EXPONENT25_VALUE                ((uint8_t) 1)

#define IMAGE_SIZE_1024                 ((uint8_t) 0)
#define IMAGE_SIZE_512                  ((uint8_t) 1)
#define IMAGE_SIZE_256                  ((uint8_t) 2)
#define IMAGE_SIZE_128                  ((uint8_t) 3)
#define IMAGE_SIZE_64                   ((uint8_t) 4)

#define NEXT_FRAME_NUMBER_VALUE         ((uint8_t) 4)

//Telemetry define
#define REQUEST_TELEMETRY_SIZE	        ((uint16_t) 5)
#define TELEMETRY_OFFSET_0              ((uint8_t) 2)
#define TELEMETRY_OFFSET_1              ((uint8_t) 3)
#define TELEMETRY_OFFSET_2              ((uint8_t) 4)
#define TELEMETRY_OFFSET_3              ((uint8_t) 5)
#define TELEMETRY_OFFSET_4              ((uint8_t) 6)
#define TELEMETRY_OFFSET_5              ((uint8_t) 7)
#define TELEMETRY_OFFSET_6              ((uint8_t) 8)
#define TELEMETRY_OFFSET_7              ((uint8_t) 9)
#define TELEMETRY_OFFSET_8              ((uint8_t) 10)
#define TELEMETRY_OFFSET_9              ((uint8_t) 11)
#define TELEMETRY_OFFSET_10             ((uint8_t) 12)
#define TELEMETRY_OFFSET_11             ((uint8_t) 13)
#define TELEMETRY_OFFSET_12             ((uint8_t) 14)
#define TELEMETRY_OFFSET_13             ((uint8_t) 15)
#define TELEMETRY_OFFSET_14             ((uint8_t) 16)
#define TELEMETRY_OFFSET_15             ((uint8_t) 17)
#define TELEMETRY_OFFSET_16             ((uint8_t) 18)
#define TELEMETRY_OFFSET_17             ((uint8_t) 19)
#define TELEMETRY_OFFSET_18             ((uint8_t) 20)
#define TELEMETRY_OFFSET_19             ((uint8_t) 21)
#define TELEMETRY_OFFSET_20             ((uint8_t) 22)
#define TELEMETRY_OFFSET_21             ((uint8_t) 23)
#define TELEMETRY_OFFSET_22             ((uint8_t) 24)
#define TELEMETRY_OFFSET_23             ((uint8_t) 25)
#define TELEMETRY_OFFSET_24             ((uint8_t) 26)
#define TELEMETRY_OFFSET_25             ((uint8_t) 27)
#define TELEMETRY_OFFSET_26             ((uint8_t) 28)
#define TELEMETRY_OFFSET_27             ((uint8_t) 29)
#define TELEMETRY_OFFSET_28             ((uint8_t) 30)
#define TELEMETRY_OFFSET_29             ((uint8_t) 31)
#define TELEMETRY_OFFSET_30             ((uint8_t) 32)
#define TELEMETRY_OFFSET_31             ((uint8_t) 33)
#define TELEMETRY_OFFSET_32             ((uint8_t) 34)
#define TELEMETRY_OFFSET_33             ((uint8_t) 35)
#define TELEMETRY_OFFSET_34             ((uint8_t) 36)
#define TELEMETRY_OFFSET_35             ((uint8_t) 37)
#define TELEMETRY_OFFSET_36             ((uint8_t) 38)
#define TELEMETRY_OFFSET_37             ((uint8_t) 39)
#define TELEMETRY_OFFSET_38             ((uint8_t) 40)
#define TELEMETRY_ID_0                  ((uint8_t) 0x80)
#define TELEMETRY_ID_0_SIZE				((uint16_t) 12)
#define TELEMETRY_ID_1                  ((uint8_t) 0x81)
#define TELEMETRY_ID_1_SIZE				((uint16_t) 10)
#define TELEMETRY_ID_2                  ((uint8_t) 0x82)
#define TELEMETRY_ID_2_SIZE				((uint16_t) 12)
#define TELEMETRY_ID_3                  ((uint8_t) 0x83)
#define TELEMETRY_ID_3_SIZE				((uint16_t) 7 )
#define TELEMETRY_ID_19                 ((uint8_t) 0x93)
#define TELEMETRY_ID_20                 ((uint8_t) 0x94) 	//In RImage.c
#define TELEMETRY_ID_20_SIZE			((uint8_t) 10)		//In RImage.c
#define TELEMETRY_ID_21                 ((uint8_t) 0x95)	//In RImage.c
#define TELEMETRY_ID_21_SIZE			((uint8_t) 10)		//In RImage.c
#define TELEMETRY_ID_22                 ((uint8_t) 0x96)
#define TELEMETRY_ID_22_SIZE			((uint8_t) 10)
#define TELEMETRY_ID_23                 ((uint8_t) 0x97)
#define TELEMETRY_ID_23_SIZE			((uint8_t) 10)
#define TELEMETRY_ID_24                 ((uint8_t) 0x98)
#define TELEMETRY_ID_24_SIZE			((uint8_t) 10)
#define TELEMETRY_ID_25                 ((uint8_t) 0x99)
#define TELEMETRY_ID_25_SIZE			((uint8_t) 10)
#define TELEMETRY_ID_26                 ((uint8_t) 0x9A)
#define TELEMETRY_ID_26_SIZE			((uint8_t) 14)
#define TELEMETRY_ID_40                 ((uint8_t) 0xA8)
#define TELEMETRY_ID_40_SIZE			((uint8_t) 14)
#define TELEMETRY_ID_64                 ((uint8_t) 0xC0) 	//In RImage.c
#define TELEMETRY_ID_64_SIZE			((uint8_t) 132) 	//In RImage.c
#define TELEMETRY_ID_65                 ((uint8_t) 0xC1)	//In RImage.c
#define TELEMETRY_ID_65_SIZE			((uint8_t) 7)		//In RImage.c
#define TELEMETRY_ID_66                 ((uint8_t) 0xC2)
#define TELEMETRY_ID_66_SIZE			((uint8_t) 1048580)
#define TELEMETRY_ID_67                 ((uint8_t) 0xC3)
#define TELEMETRY_ID_67_SIZE			((uint8_t) 1048580)
#define TELEMETRY_ID_68                 ((uint8_t) 0xC4)
#define TELEMETRY_ID_68_SIZE			((uint8_t) 1048580)
#define TELEMETRY_ID_69                 ((uint8_t) 0xC5)
#define TELEMETRY_ID_69_SIZE			((uint8_t) 1048580)
#define TELEMETRY_ID_72                 ((uint8_t) 0xC8)
#define TELEMETRY_ID_72_SIZE			((uint8_t) 44)
#define TELEMETRY_ID_73                 ((uint8_t) 0xC9)
#define TELEMETRY_ID_73_SIZE			((uint8_t) 44)

#define TLM_REPLY_SIZE_1  				((uint8_t) 1)
#define TLM_REPLY_SIZE_2  				((uint8_t) 2)
#define TLM_REPLY_SIZE_3  				((uint8_t) 3)
#define TLM_REPLY_SIZE_4  				((uint8_t) 4)
#define TLM_REPLY_SIZE_6  				((uint8_t) 6)
#define TLM_REPLY_SIZE_8  				((uint8_t) 8)
#define TLM_REPLY_SIZE_10  				((uint8_t) 10)
#define TLM_REPLY_SIZE_12  				((uint8_t) 12)
#define TLM_REPLY_SIZE_14  				((uint8_t) 14)
#define TLM_REPLY_SIZE_20  				((uint8_t) 20)
#define TLM_REPLY_SIZE_40  				((uint8_t) 40)
#define TLM_REPLY_SIZE_128  			((uint8_t) 128)
#define TLM_REPLY_SIZE_1048576  		(1048576)

#define TELEMETRY_OVERHEAD	            ((uint8_t) (sizeof(TELECOMMAND_OVERHEAD))
#define TELEMETRY_HEADER_INDEX	        ((uint8_t)  0)
#define REQUESTING_TLM_ID_INDEX	        (2)
#define TELEMETRY_FRAME_ID_SIZE			(1)
#define TELEMETRY_PARAM_INDEX		    ((uint8_t)  (TELEMETRY_ID_INDEX + sizeof(telecommand_id_t)))
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

#define BASE_MESSAGE_SIZE 	((uint8_t) 4)

#define IMAGE_1                                     ((uint8_t) 4)
#define IMAGE_2                                     ((uint8_t) 4)
#define IMAGE_3                                     ((uint8_t) 4)

#define ret                                         ((uint8_t) 4)

#define DOWNLOAD_INDEX1                             ((uint8_t) 4)
#define DOWNLOAD_INDEX2                             ((uint8_t) 4)
#define DOWNLOAD_INDEX3                             ((uint8_t) 4)


int SRAM_FLAG_1=0;   //0 for empty, 1 for full
int SRAM_FLAG_2=0;
int SRAM_FLAG_3=0;





/** Invalid input parameters to the Camera module. */
#define ERROR_INVALID_PARAM		(-36)

static const uint8_t ackResponse[TELEMETRY_ACK_SIZE] = { 0x1F, 0x7F, 0x00, 0x1F, 0xFF };

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/
/** Used to build buffers */
static uint8_t * telemetryMessageBuilder(uint8_t response_size);

/** Sends Telemetry ID 3 and receives feedback from Camera */
static int tlmTelecommandAcknowledge(tlm_telecommand_ack_t *telemetry_reply);

/***************************************************************************************************
                                       PUBLIC FUNCTION STUBS
***************************************************************************************************/

/***************************************************************************************************
                                      FUNCTION DECLARATIONS
***************************************************************************************************/
// ResetTelecommand
camera_telecommand_t resetTelecommand = {
		.ID = TELECOMMAND_ID_0,
		.paramLength = 1
};

// Clear SRAM overcurrent flags Telecommand
camera_telecommand_t clearSRAMOvercurrentFlags = {
		.ID = TELECOMMAND_ID_11,
		.paramLength = 1
};

// Image capture & detection Telecommand
camera_telecommand_t captureAndDetect = {
		.ID = TELECOMMAND_ID_20,
		.paramLength = 2
};

// Capture Image Telecommand
camera_telecommand_t captureImageTelecommand = {
		.ID = TELECOMMAND_ID_21,
		.paramLength = 3
};

//Set sensor 1 detection threshold Telecommand
camera_telecommand_t setSensorOneDetectionThreshold = {
		.ID = TELECOMMAND_ID_40,
		.paramLength = 1
};

//Set sensor 2 detection threshold Telecommand
camera_telecommand_t setSensorTwoDetectionThreshold = {
			.ID = TELECOMMAND_ID_41,
			.paramLength = 1
};

//Set sensor 1 auto-adjust Telecommand
camera_telecommand_t setSensorOneAutoadjust = {
			.ID = TELECOMMAND_ID_42,
			.paramLength = 1
};

//Set sensor 1 settings Telecommand
camera_telecommand_t setSensorOneSetting = {
			.ID = TELECOMMAND_ID_43,
			.paramLength = 5
};

//Set sensor 2 auto-adjust Telecommand
camera_telecommand_t setSensorTwoAutoadjust = {
			.ID = TELECOMMAND_ID_44,
			.paramLength = 1
};

//Set sensor 2 settings Telecommand
camera_telecommand_t setSensorTwoSetting = {
			.ID = TELECOMMAND_ID_45,
			.paramLength = 5
};

//Set sensor 1 boresight pixel location Telecommand
camera_telecommand_t setSensorOneBoresightPixelLocation = {
			.ID = TELECOMMAND_ID_50,
			.paramLength = 4
};

//Set sensor 2 boresight pixel location Telecommand
camera_telecommand_t setSensorTwoBoresightPixelLocation = {
			.ID = TELECOMMAND_ID_51,
			.paramLength = 4
};

//Set sensor 1 mask Telecommand
camera_telecommand_t setSensorOneMask = {
			.ID = TELECOMMAND_ID_52,
			.paramLength = 9
};

//Set sensor 2 mask Telecommand
camera_telecommand_t setSensorTwoMask = {
			.ID = TELECOMMAND_ID_53,
			.paramLength = 9
};

//Set sensor 1 distortion correction coefficients Telecommand
camera_telecommand_t setSensorOneDistortionCorrectionCoefficient = {
			.ID = TELECOMMAND_ID_54,
			.paramLength = 15
};

//Set sensor 2 distortion correction coefficients Telecommand
camera_telecommand_t setSensorTwoDistortionCorrectionCoefficient = {
			.ID = TELECOMMAND_ID_55,
			.paramLength = 15
};

//Initialize image download Telecommand
camera_telecommand_t initializeImageDownload = {
			.ID = TELECOMMAND_ID_64,
			.paramLength = 3
};

//Advance image download Telecommand
camera_telecommand_t advanceimageDownload = {
			.ID = TELECOMMAND_ID_65,
			.paramLength = 2
};

//Header and trailer
tc_header_t header = {
		.header1 = START_IDENTIFIER1,
		.header2 = START_IDENTIFIER2
};

tc_trailer_t trailer = {
		.trailer1 = END_IDENTIFIER1,
		.trailer2 = END_IDENTIFIER2
};
/***************************************************************************************************
                                             PRIVATE API
****************************************************************************************************/
static int sendResetTc(uint8_t RresetType)
{
    // determine size of telecommand
	uint8_t resetTcSize = TELECOMMAND_OVERHEAD + resetTelecommand.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[resetTcSize];

	// start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 0
	commandBuffer[TELECOMMAND_ID_INDEX] = resetTelecommand.ID; //resetTelecmd.ID

    // telecommand parameters- Interface control document. Page:25
	tc_reset_Telecommand_params_t params;
	params.resetType = RresetType;

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

    // end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(captureImageTelecommand.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
    // return 0 if an error occurs in UART

	//TODO: Implement feature with error manager
	if (error != 0)
		return error;

	return 0;
}
/***************************************************************************************************/
static int sendClearSramOverCurrentFlagTc(uint8_t SRAMOverCurrentFlag)
{
   // determine size of telecommand
	uint8_t clearSramOverCurrentFlagTcSize = TELECOMMAND_OVERHEAD + clearSRAMOvercurrentFlags.paramLength;

   // build  camera capture telecommand
	uint8_t commandBuffer[clearSramOverCurrentFlagTcSize];

   // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

   // telecommand ID 0
	commandBuffer[TELECOMMAND_ID_INDEX] = clearSRAMOvercurrentFlags.ID;

   // telecommand parameters- Interface control document. Page:25
	tc_clear_SRAM_over_current_t params;
	params.SRAMOverCurrentFlag= CLEAR_SRAM1_FLAG;

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));
   // end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(captureImageTelecommand.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
   // return 0 if an error occurs
	if (error != 0)
		return error;

	return 0;
}
/***************************************************************************************************/
static int sendCameraCaptureImageTc(uint8_t sensor, uint8_t sram, uint8_t half)
{
    // determine size of telecommand
	uint8_t cameraCaptureImageTcSize = TELECOMMAND_OVERHEAD + captureImageTelecommand.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[cameraCaptureImageTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 21
	commandBuffer[TELECOMMAND_ID_INDEX] = captureImageTelecommand.ID;

    //telecommand parameters- Interface control document. Page:25
	tc_capture_image_params_t params;
	params.camera = sensor;
	params.sram = sram;
	params.whichHalf = half;

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

    // end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(captureImageTelecommand.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
    // return 0 if an error occurs
	if (error != 0)
		return error;

	return 0;
}
/***************************************************************************************************/
static int sendImageAndCaptureDetectionTc(uint8_t camera, uint8_t sram)
{
    // determine size of telecommand
	uint8_t imageAndCaptureDetectionTcSize = TELECOMMAND_OVERHEAD + captureAndDetect.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[imageAndCaptureDetectionTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 21
	commandBuffer[TELECOMMAND_ID_INDEX] = captureAndDetect.ID;

    // telecommand parameters- Interface control document. Page:25
	tc_image_capture_anddetect_params_t params;
	params.camera = CAMERA_ONE;
	params.sram = SRAM1;

	memcpy(&commandBuffer[TELECOMMAND_PARAM_INDEX], &params, sizeof(params));

    // end of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_TRAILER_INDEX(captureAndDetect.paramLength)],
		   &trailer,
		   sizeof(trailer));

	int error = uartTransmit(UART_CAMERA_BUS, commandBuffer, sizeof(commandBuffer));
    // return 0 if an error occurs
	if (error != 0)
		return error;

	return 0;
}
/***************************************************************************************************/
/*The threshold is set to a default value calculated for robustness.
If the user wishes to change the threshold values, telecommands 40
and 41 can be used. It is however recommended that the default values be used.*/
static int sendSetSensorOneDetectionThresholdTc(uint8_t detectionThreshold1)
{
    // determine size of telecommand
	uint8_t setSensorOneDetectionThresholdTcSize = TELECOMMAND_OVERHEAD + setSensorOneDetectionThreshold.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[setSensorOneDetectionThresholdTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 40
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorOneDetectionThreshold.ID;

    //telecommand parameters- Interface control document. Page:26
    tc_set_sensor_one_detection_threshold_t params;
           params.detectionThreshold1= DETECTION_THRESHOLD_VALUE1;

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
/***************************************************************************************************/
static int sendSetSensorTwoDetectionThresholdTc(uint8_t detectionThreshold2)
{
    // determine size of telecommand
	uint8_t setSensorTwoDetectionThresholdTcSize = TELECOMMAND_OVERHEAD +  setSensorTwoDetectionThreshold.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[setSensorTwoDetectionThresholdTcSize];

   // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

   // telecommand ID 41
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorTwoDetectionThreshold.ID;

  //telecommand parameters- Interface control document. Page:26
  tc_set_sensor_two_detection_threshold_t params;
		params.detectionThreshold2= DETECTION_THRESHOLD_VALUE2;

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
/***************************************************************************************************/
/*The default exposure setting for the sun sensor is set to a fixed calculated value. It is recommended that the
client does not alter this value. The Nadir sensor uses an auto-exposure algorithm to adjust the changing
brightness of the earth in the sensor’s image. These values are calibrated for robust sensor operation.
If, however the client wishes to use the Nadir sensor as a greyscale imager, the exposure can be set by using
telecommands 42 – 45.*/
static int sendSetSensorOneAutoadjustTc(uint8_t autoadjustenable1)
{
   // determine size of telecommand
	uint8_t setSensorOneAutoadjustTcSize = TELECOMMAND_OVERHEAD + setSensorOneAutoadjust.paramLength;

   // build  camera capture telecommand
	uint8_t commandBuffer[setSensorOneAutoadjustTcSize];

   // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

   // telecommand ID 42
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorOneAutoadjust.ID;

   //telecommand parameters- Interface control document. Page:26
	tc_set_SensorOne_Autoadjust_t params;
	params.autoadjustenable1= AUTOADJUST_ENABLED;

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
/***************************************************************************************************/
static int sendSetSensorOneSettingTc(uint16_t exposureTime1, uint8_t AGC1, uint8_t blueGain1, uint8_t redGain1)
{
   // determine size of telecommand
	uint8_t setSensorOneSettingTcSize = TELECOMMAND_OVERHEAD + setSensorOneSetting.paramLength;

   // build  camera capture telecommand
	uint8_t commandBuffer[setSensorOneSettingTcSize];

   // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 43
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorOneSetting.ID;

    //telecommand parameters- Interface control document. Page:26
	tc_set_SensorOne_Setting_t params;
	params.exposureTime1= EXPOSURE_REGISTER_VALUE1;
			params.AGC1=GAIN_CONTROL_REGISTER1;
			params.blueGain1= BLUE_GAIN_CONTROL_REGISTER1;
			params.redGain1= RED_GAIN_CONTROL_REGISTER1;

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
/***************************************************************************************************/
static int sendSetSensorTwoAutoadjustTc(uint8_t autoadjustenable2)
{
    // determine size of telecommand
	uint8_t setSensorTwoAutoadjustTcSize = TELECOMMAND_OVERHEAD + setSensorTwoAutoadjust.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[setSensorTwoAutoadjustTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 44
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorTwoAutoadjust.ID;

    //telecommand parameters- Interface control document. Page:27
   tc_set_SensorTwo_Autoadjust_t params;
      params.autoadjustenable2=AUTOADJUST_ENABLED;

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
/***************************************************************************************************/
static int sendSetSensorTwoSettingTc(uint16_t exposureTime2, uint8_t AGC2,uint8_t blueGain2,uint8_t redGain2)
{
    // determine size of telecommand
	uint8_t setSensorTwoSettingTcSize = TELECOMMAND_OVERHEAD + setSensorTwoSetting.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[setSensorTwoSettingTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 45
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorTwoSetting.ID;

    //telecommand parameters- Interface control document. Page:27
	tc_set_SensorTwo_Setting_t params;
	params.exposureTime2= EXPOSURE_REGISTER_VALUE2;
			params.AGC2=GAIN_CONTROL_REGISTER2;
			params.blueGain2= BLUE_GAIN_CONTROL_REGISTER2;
			params.redGain2= RED_GAIN_CONTROL_REGISTER2;

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
/***************************************************************************************************/
static int sendSetSensorOneBoresightPixelLocationTc(uint16_t Y_pixel1, uint16_t X_pixel1)
	{
    // determine size of telecommand
		uint8_t setSensorOneBoresightPixelLocationTcSize = TELECOMMAND_OVERHEAD + setSensorOneBoresightPixelLocation.paramLength;

    // build  camera capture telecommand
		uint8_t commandBuffer[setSensorOneBoresightPixelLocationTcSize];

    // start of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 50
		commandBuffer[TELECOMMAND_ID_INDEX] = setSensorOneBoresightPixelLocation.ID;

    // telecommand parameters- Interface control document. Page:27
    tc_set_SensorOne_Boresight_Pixel_Location_t params;
		params.X_pixel1=X_PIXEL_LOCATION_CAM1_BORESIGHT;
		params.Y_pixel1=Y_PIXEL_LOCATION_CAM1_BORESIGHT;

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
/***************************************************************************************************/
static int sendSetSensorTwoBoresightPixelLocationTc(uint16_t X_pixel2, uint16_t Y_pixel2)
		{
    // determine size of telecommand
	uint8_t setSensorTwoBoresightPixelLocationTcSize = TELECOMMAND_OVERHEAD + setSensorTwoBoresightPixelLocation.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[setSensorTwoBoresightPixelLocationTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 51
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorTwoBoresightPixelLocation.ID;

    //telecommand parameters- Interface control document. Page:27
	tc_set_SensorTwo_Boresight_Pixel_Location_t params;
				params .X_pixel2=X_PIXEL_LOCATION_CAM2_BORESIGHT;
				params .Y_pixel2=Y_PIXEL_LOCATION_CAM2_BORESIGHT;


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
/***************************************************************************************************/
static int sendSetSensorOneMaskTc(uint8_t maskNumber1, uint16_t xMinimum1, uint16_t xMaximum1,uint16_t yMinimum1, uint16_t yMaximum1)
			{
    // determine size of telecommand
	uint8_t setSensorOneMaskTcSize = TELECOMMAND_OVERHEAD + setSensorOneMask.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[setSensorOneMaskTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 52
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorOneMask.ID;

    //telecommand parameters- Interface control document. Page:28
	tc_set_SensorOne_Mask_t params;
		params .maskNumber1=AREA_NUMBER1;
		params .xMinimum1=LOWER_X_LIMIT1;
		params .xMaximum1=UPPER_X_LIMIT1;
		params .yMinimum1=LOWER_Y_LIMIT1;
		params .yMaximum1=UPPER_Y_LIMIT1;

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
/***************************************************************************************************/
static int sendSetSensorTwoMaskTc(uint8_t maskNumber2, uint16_t xMinimum2, uint16_t xMaximum2,uint16_t yMinimum2, uint16_t yMaximum2)
		{
        // determine size of telecommand
		uint8_t setSensorTwoMaskTcSize = TELECOMMAND_OVERHEAD + setSensorTwoMask.paramLength;

        // build  camera capture telecommand
		uint8_t commandBuffer[setSensorTwoMaskTcSize];

        // start of message identifiers
		memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

        // telecommand ID 53
		commandBuffer[TELECOMMAND_ID_INDEX] = setSensorTwoMask.ID;

        //telecommand parameters- Interface control document. Page:28
		tc_set_SensorTwo_Mask_t params;
		params.maskNumber2=AREA_NUMBER2;
		params.xMinimum2=LOWER_X_LIMIT2;
		params.xMaximum2=UPPER_X_LIMIT2;
		params.yMinimum2=LOWER_Y_LIMIT2;
		params.yMaximum2=UPPER_Y_LIMIT2;

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
/***************************************************************************************************/
static int sendSetSensorOneDistortionCorrectionCoefficientTc(uint16_t Mantissa11, uint8_t Exponent11, uint16_t Mantissa12, uint8_t Exponent12,
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
      commandBuffer[TELECOMMAND_ID_INDEX] = setSensorOneDistortionCorrectionCoefficient.ID;

      //telecommand parameters- Interface control document. Page:29
      tc_set_SensorOne_distortionCorrectionCoefficient_params_t params;
                params.Mantissa11=MANTISSA11_VALUE;
                params.Exponent11=EXPONENT11_VALUE;
                params.Mantissa12=MANTISSA12_VALUE;
                params.Exponent12=EXPONENT12_VALUE;
                params.Mantissa13=MANTISSA13_VALUE;
                params.Exponent13=EXPONENT13_VALUE;
                params.Mantissa14=MANTISSA14_VALUE;
                params.Exponent14=EXPONENT14_VALUE;
                params.Mantissa15=MANTISSA15_VALUE;
                params.Exponent15=EXPONENT15_VALUE;

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
/***************************************************************************************************/
static int sendSetSensorTwoDistortionCorrectionCoefficientTc(uint16_t Mantissa21, uint8_t Exponent21, uint16_t Mantissa22, uint8_t Exponent22,
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
	commandBuffer[TELECOMMAND_ID_INDEX] = setSensorTwoDistortionCorrectionCoefficient.ID;

    //telecommand parameters- Interface control document. Page:30
    tc_set_SensorTwo_distortionCorrectionCoefficient_t params;
           params.Mantissa21=MANTISSA21_VALUE;
           params.Exponent21=EXPONENT21_VALUE;
           params.Mantissa22=MANTISSA22_VALUE;
           params.Exponent22=EXPONENT22_VALUE;
           params.Mantissa23=MANTISSA23_VALUE;
           params.Exponent23=EXPONENT23_VALUE;
           params.Mantissa24=MANTISSA24_VALUE;
           params.Exponent24=EXPONENT24_VALUE;
           params.Mantissa25=MANTISSA25_VALUE;
           params.Exponent25=EXPONENT25_VALUE;

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
/***************************************************************************************************/
static int sendInitializeImageDownloadTc(uint8_t sramSelection, uint8_t sramLocation, uint8_t sizeSelection)
	{
    // determine size of telecommand
	uint8_t initializeImageDownloadTcSize = TELECOMMAND_OVERHEAD + initializeImageDownload.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[initializeImageDownloadTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 64
	commandBuffer[TELECOMMAND_ID_INDEX] = initializeImageDownload.ID;

    //telecommand parameters- Interface control document. Page:31
	tc_initialize_imageDownload_params_t params;
		params.sramSelection=SRAM1;
		params.sramLocation =TOP_HALVE;
		params.sizeSelection=IMAGE_SIZE_1024;

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
/***************************************************************************************************/
static int sendAdvanceimageDownloadTc(uint16_t nextFrameNumber)
{
    // determine size of telecommand
	uint8_t advanceimageDownloadTcSize = TELECOMMAND_OVERHEAD + advanceimageDownload.paramLength;

    // build  camera capture telecommand
	uint8_t commandBuffer[advanceimageDownloadTcSize];

    // start of message identifiers
	memcpy(&commandBuffer[TELECOMMAND_HEADER_INDEX], &header, sizeof(header));

    // telecommand ID 65
	commandBuffer[TELECOMMAND_ID_INDEX] = advanceimageDownload.ID;

    // telecommand parameters- Interface control document. Page:31
	tc_advance_imageDownload_params_t params;
	    params.nextFrameNumber=NEXT_FRAME_NUMBER_VALUE;

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

//TODO: Implement null pointer check for all tlm funcitons
//TODO: Make function docstrings for tlm functions
//TODO: Make function stubs for all telemetry functions

static int tlmStatus(tlm_status_t *telemetry_reply) {
	uint8_t *telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_0;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_8);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_0_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	telemetry_reply->nodeType = telemetryBuffer[TELEMETRY_OFFSET_0];
	telemetry_reply->interfaceVersion = telemetryBuffer[TELEMETRY_OFFSET_1];
	telemetry_reply->firmwareVersionMajor = telemetryBuffer[TELEMETRY_OFFSET_2];
	telemetry_reply->firmwareVersionMinor = telemetryBuffer[TELEMETRY_OFFSET_3];
	memcpy(&telemetry_reply->runtimeSeconds, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->runtimeSeconds));
	memcpy(&telemetry_reply->runtimeMSeconds, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->runtimeMSeconds));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;

}

static int tlmSerialNumber(tlm_serial_number_t *telemetry_reply) {
	uint8_t *telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_1;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_1_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->Nodetype, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->Nodetype));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmCommunicationStatus(tlm_communication_status_t *telemetry_reply) {
	uint8_t *telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_2;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_8);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_2_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->tcCounter, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->tcCounter));
	memcpy(&telemetry_reply->tlmCounter, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->tlmCounter));
	telemetry_reply->tcBufferOverunFlag = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->i2ctlmReadErrorFlag = telemetryBuffer[TELEMETRY_OFFSET_5];
	telemetry_reply->uarttlmProtocolErrorFlag = telemetryBuffer[TELEMETRY_OFFSET_6];
	telemetry_reply->uartIncompleteMsgFlag = telemetryBuffer[TELEMETRY_OFFSET_7];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmTelecommandAcknowledge(tlm_telecommand_ack_t *telemetry_reply) {

	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_3;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_3);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_3_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	telemetry_reply->last_tc_id = telemetryBuffer[TELEMETRY_OFFSET_0];
	telemetry_reply->processed_flag = telemetryBuffer[TELEMETRY_OFFSET_1];
	telemetry_reply->tc_error_flag = telemetryBuffer[TELEMETRY_OFFSET_2];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmSensorOneResult(tlm_detection_result_and_trigger_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_20;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_20_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmSensorTwoResult(tlm_detection_result_and_trigger_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_21;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_21_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmSensorOneResultAndDetectionSRAMOne(tlm_detection_result_and_trigger_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_22;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_22_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmSensorTwoResultAndDetectionSRAMTwo(tlm_detection_result_and_trigger_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_23;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_23_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmSensorOneResultAndDetectionSRAMTwo(tlm_detection_result_and_trigger_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_24;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_24_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmSensorTwoResultAndDetectionSRAMOne(tlm_detection_result_and_trigger_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_25;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_6);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_25_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->alpha, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->alpha));
	memcpy(&telemetry_reply->beta, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->beta));
	telemetry_reply->captureResult = telemetryBuffer[TELEMETRY_OFFSET_4];
	telemetry_reply->detectionResult = telemetryBuffer[TELEMETRY_OFFSET_5];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmPower(tlm_power_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_26;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_10);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_26_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->threeVcurrent, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->threeVcurrent));
	memcpy(&telemetry_reply->sramOneCurrent, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->sramOneCurrent));
	memcpy(&telemetry_reply->sramTwoCurrent, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->sramTwoCurrent));
	memcpy(&telemetry_reply->fiveVcurrent, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->fiveVcurrent));
	telemetry_reply->sramOneOverCurrent = telemetryBuffer[TELEMETRY_OFFSET_8];
	telemetry_reply->sramTwoOverCurrent = telemetryBuffer[TELEMETRY_OFFSET_9];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmConfig(tlm_config_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_40;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_14);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_40_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	telemetry_reply->cameraOneDetectionThrshld = telemetryBuffer[TELEMETRY_OFFSET_0];
	telemetry_reply->cameraTwoDetectionThrshld = telemetryBuffer[TELEMETRY_OFFSET_1];
	telemetry_reply->cameraOneAutoAdjustMode = telemetryBuffer[TELEMETRY_OFFSET_2];
	memcpy(&telemetry_reply->cameraOneExposure, &telemetryBuffer[TELEMETRY_OFFSET_3], sizeof(telemetry_reply->cameraOneExposure));
	telemetry_reply->cameraOneAGC = telemetryBuffer[TELEMETRY_OFFSET_5];
	telemetry_reply->cameraOneBlueGain = telemetryBuffer[TELEMETRY_OFFSET_6];
	telemetry_reply->cameraOneRedGain = telemetryBuffer[TELEMETRY_OFFSET_7];
	telemetry_reply->cameraTwoAutoAdjustMode = telemetryBuffer[TELEMETRY_OFFSET_8];
	memcpy(&telemetry_reply->cameraTwoExposure, &telemetryBuffer[TELEMETRY_OFFSET_9], sizeof(telemetry_reply->cameraTwoExposure));
	telemetry_reply->cameraTwoAGC = telemetryBuffer[TELEMETRY_OFFSET_11];
	telemetry_reply->cameraTwoBlueGain = telemetryBuffer[TELEMETRY_OFFSET_12];
	telemetry_reply->cameraTwoRedGain = telemetryBuffer[TELEMETRY_OFFSET_13];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmImageFrame(tlm_image_frame_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_64;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_128);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_64_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->image_bytes, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->image_bytes));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmImageFrameInfo(tlm_image_frame_info_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_65;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_3);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_65_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->imageFrameNumber, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->imageFrameNumber));
	telemetry_reply->checksum = telemetryBuffer[TELEMETRY_OFFSET_2];

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmFullImageSRAMOneLocationOne(tlm_full_image_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_66;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_1048576);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_66_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->imageBytes, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->imageBytes));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmFullImageSRAMOneLocationTwo(tlm_full_image_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_67;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_1048576);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_67_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->imageBytes, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->imageBytes));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmFullImageSRAMTwoLocationOne(tlm_full_image_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_68;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_1048576);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_68_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->imageBytes, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->imageBytes));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmFullImageSRAMTwoLocationTwo(tlm_full_image_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_69;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_1048576);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_69_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->imageBytes, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->imageBytes));

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmReadSensorOneMask(tlm_read_sensor_mask_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_72;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_40);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_72_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->MinXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->MinXAreaOne));
	memcpy(&telemetry_reply->MaxXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->MaxXAreaOne));
	memcpy(&telemetry_reply->MinYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->MinYAreaOne));
	memcpy(&telemetry_reply->MaxYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->MaxYAreaOne));
	memcpy(&telemetry_reply->MinXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_8], sizeof(telemetry_reply->MinXAreaTwo));
	memcpy(&telemetry_reply->MaxXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_10], sizeof(telemetry_reply->MaxXAreaTwo));
	memcpy(&telemetry_reply->MinYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_12], sizeof(telemetry_reply->MinYAreaTwo));
	memcpy(&telemetry_reply->MaxYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_14], sizeof(telemetry_reply->MaxYAreaTwo));
	memcpy(&telemetry_reply->MinXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_16], sizeof(telemetry_reply->MinXAreaThree));
	memcpy(&telemetry_reply->MaxXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_18], sizeof(telemetry_reply->MaxXAreaThree));
	memcpy(&telemetry_reply->MinYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_20], sizeof(telemetry_reply->MinYAreaThree));
	memcpy(&telemetry_reply->MaxYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_22], sizeof(telemetry_reply->MaxYAreaThree));
	memcpy(&telemetry_reply->MinXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_24], sizeof(telemetry_reply->MinXAreaFourth));
	memcpy(&telemetry_reply->MaxXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_26], sizeof(telemetry_reply->MaxXAreaFourth));
	memcpy(&telemetry_reply->MinYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_28], sizeof(telemetry_reply->MinYAreaFourth));
	memcpy(&telemetry_reply->MaxYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_30], sizeof(telemetry_reply->MaxYAreaFourth));
	memcpy(&telemetry_reply->MinXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_32], sizeof(telemetry_reply->MinXAreaFifth));
	memcpy(&telemetry_reply->MaxXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_34], sizeof(telemetry_reply->MaxXAreaFifth));
	memcpy(&telemetry_reply->MinYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_36], sizeof(telemetry_reply->MinYAreaFifth));
	memcpy(&telemetry_reply->MaxYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_38], sizeof(telemetry_reply->MaxYAreaFifth));


	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

static int tlmReadSensorTwoMask(tlm_read_sensor_mask_t *telemetry_reply){
	uint8_t* telemetryBuffer;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TELEMETRY_FRAME_ID_SIZE);

    // Fill buffer with telemetry ID
	telemetryBuffer[REQUESTING_TLM_ID_INDEX] = TELEMETRY_ID_73;

    // Send Telemetry Request
	int error = uartTransmit(UART_CAMERA_BUS, telemetryBuffer, REQUEST_TELEMETRY_SIZE);

	// Free the dynamically allocated buffer
	free(telemetryBuffer);

    // Error Check on uartTransmit
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0)
		return error;

	// Dynamically allocate a buffer to hold the telemetry message with header and footer implemented
	telemetryBuffer = telemetryMessageBuilder(TLM_REPLY_SIZE_40);

    // Reading Automatic reply from CubeSense regarding status of Telemetry request
	error = uartReceive(UART_CAMERA_BUS, telemetryBuffer, TELEMETRY_ID_73_SIZE);

	// Error Check on uartRecieve, if error, free allocated buffer
	//TODO: Change 0 to SUCCESS when merged with alpha branch
	if (error != 0){
		free(telemetryBuffer);
		return error;
	}

	// Fill telemetry reply, data from uart read starts at index two
	memcpy(&telemetry_reply->MinXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_0], sizeof(telemetry_reply->MinXAreaOne));
	memcpy(&telemetry_reply->MaxXAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_2], sizeof(telemetry_reply->MaxXAreaOne));
	memcpy(&telemetry_reply->MinYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_4], sizeof(telemetry_reply->MinYAreaOne));
	memcpy(&telemetry_reply->MaxYAreaOne, &telemetryBuffer[TELEMETRY_OFFSET_6], sizeof(telemetry_reply->MaxYAreaOne));
	memcpy(&telemetry_reply->MinXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_8], sizeof(telemetry_reply->MinXAreaTwo));
	memcpy(&telemetry_reply->MaxXAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_10], sizeof(telemetry_reply->MaxXAreaTwo));
	memcpy(&telemetry_reply->MinYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_12], sizeof(telemetry_reply->MinYAreaTwo));
	memcpy(&telemetry_reply->MaxYAreaTwo, &telemetryBuffer[TELEMETRY_OFFSET_14], sizeof(telemetry_reply->MaxYAreaTwo));
	memcpy(&telemetry_reply->MinXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_16], sizeof(telemetry_reply->MinXAreaThree));
	memcpy(&telemetry_reply->MaxXAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_18], sizeof(telemetry_reply->MaxXAreaThree));
	memcpy(&telemetry_reply->MinYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_20], sizeof(telemetry_reply->MinYAreaThree));
	memcpy(&telemetry_reply->MaxYAreaThree, &telemetryBuffer[TELEMETRY_OFFSET_22], sizeof(telemetry_reply->MaxYAreaThree));
	memcpy(&telemetry_reply->MinXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_24], sizeof(telemetry_reply->MinXAreaFourth));
	memcpy(&telemetry_reply->MaxXAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_26], sizeof(telemetry_reply->MaxXAreaFourth));
	memcpy(&telemetry_reply->MinYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_28], sizeof(telemetry_reply->MinYAreaFourth));
	memcpy(&telemetry_reply->MaxYAreaFourth, &telemetryBuffer[TELEMETRY_OFFSET_30], sizeof(telemetry_reply->MaxYAreaFourth));
	memcpy(&telemetry_reply->MinXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_32], sizeof(telemetry_reply->MinXAreaFifth));
	memcpy(&telemetry_reply->MaxXAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_34], sizeof(telemetry_reply->MaxXAreaFifth));
	memcpy(&telemetry_reply->MinYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_36], sizeof(telemetry_reply->MinYAreaFifth));
	memcpy(&telemetry_reply->MaxYAreaFifth, &telemetryBuffer[TELEMETRY_OFFSET_38], sizeof(telemetry_reply->MaxYAreaFifth));


	// Free the dynamically allocated buffer
	free(telemetryBuffer);

	//TODO: Change 0 to SUCCESS when merged with alpha branch
	return 0;
}

/*
 * Used to dynamically allocated buffer sizes as each telemetry and telecommand
 * require different sizes
 *
 * @note must use Free() to free the allocated memory when finished using the buffer
 * @param response_size defines how many data bytes are required in the buffer
 * @return dynamically allocated buffer
 * */
static uint8_t * telemetryMessageBuilder(uint8_t response_size){

	// Define the total size the buffer should be
    uint8_t total_buffer_length = response_size + BASE_MESSAGE_SIZE;

    // Dynamically Allocate a Buffer for telemetry response
    uint8_t* tlmBuffer = (uint8_t*) malloc(total_buffer_length * sizeof(uint8_t));

    // Initialize all elements in the buffer with zero
    for (uint8_t i = 0; i < total_buffer_length; i++){
    	tlmBuffer[i] = 0;
    }

    // Fill Buffer with default values
    for(uint8_t i = 0; i<total_buffer_length;i++){
        if (i == 0){
        	tlmBuffer[i] = START_IDENTIFIER1;
        }
        else if (i == 1){
            tlmBuffer[i] = START_IDENTIFIER2;
        }
        else if (i == total_buffer_length-2) {
        	tlmBuffer[i] = END_IDENTIFIER1;
        }
        else if (i == total_buffer_length-1) {
        	tlmBuffer[i] = END_IDENTIFIER2;
        }
        else{
        	tlmBuffer[i] = FILLER;
        }
    }

    return tlmBuffer;

}

/************************************************Calculate the mean***********************************************/
//  Calculate mean
/* For the filtering first we should download the image. Its better to download size 64 of the image.
 So the image size is 64*64. It is constant.*/

static int calculateMeanOfTheImage(uint8_t DOWNLOADED64IMAGE)
{

	uint8_t  i = 64;
	uint8_t  j = 64;
	uint8_t  a = 0;
	uint8_t  b = 0;
	uint8_t  sum=0;
	uint8_t  n = i*j;
	uint8_t  IMAGE_MEAN  =0;

for (a = 0 ; a < i; ++a){
	for (b = 0; b < j; ++b){
		sum = sum + DOWNLOADED64IMAGE[a][b];
		//TODO: replace the DOWNLOADED64IMAGE with the correct name. It is received from tlm image frame.
	  }
   }

IMAGE_MEAN= sum / n;

 	return IMAGE_MEAN;

}

/***************************************************************************************************

****************************************************************************************************/

int main(){

//  Capture 3 images

ret= sendCameraCaptureImageTc(uint8_t CAMERA_TWO, uint8_t SRAM1, uint8_t BOTTOM_HALVE);
tlmTelecommandAcknowledge(tlm_telecommand_ack_t *telemetry_reply);

if (tc_error_flag = 0)
{
	SRAM_FLAG_1=1;
}

ret = sendCameraCaptureImageTc(uint8_t CAMERA_TWO, uint8_t SRAM2, uint8_t TOP_HALVE);
tlmTelecommandAcknowledge(tlm_telecommand_ack_t *telemetry_reply);

if (tc_error_flag = 0)
{
	SRAM_FLAG_2=1;
}

ret = sendCameraCaptureImageTc(uint8_t CAMERA_TWO, uint8_t SRAM2, uint8_t BOTTOM_HALVE);
tlmTelecommandAcknowledge(tlm_telecommand_ack_t *telemetry_reply);

if (tc_error_flag = 0)
{
	SRAM_FLAG_3=1;
}

//  Check all the flags. All falgs are 1= all SRAM locations are full. So the image download will be started.
if (SRAM_FLAG_1=1 && SRAM_FLAG_2=1 && SRAM_FLAG_2=1)
{
 DOWNLOAD_INDEX1 = sendInitializeImageDownloadTc(uint8_t SRAM1, uint8_t BOTTOM_HALVE, uint8_t IMAGE_SIZE_64);
 IMAGE_1 = tlmImageFrame(tlm_image_frame_t *telemetry_reply);
 DOWNLOAD_INDEX1 =sendInitializeImageDownloadTc(uint8_t SRAM2, uint8_t TOP_HALVE, uint8_t IMAGE_SIZE_64);
 IMAGE_2 = tlmImageFrame(tlm_image_frame_t *telemetry_reply);
 DOWNLOAD_INDEX1 =sendInitializeImageDownloadTc(uint8_t SRAM2, uint8_t BOTTOM_HALVE, uint8_t IMAGE_SIZE_64);
 IMAGE_3 = tlmImageFrame(tlm_image_frame_t *telemetry_reply);
}

// Check all the images are downloaded. So image filtering can be started.
if (IMAGE_1 != 0 && IMAGE_2 != 0 && IMAGE_3 != 0)
{

IMAGE_MEAN = calculateMeanOfTheImage(uint8_t IMAGE_1);
  if(IMAGE_MEAN< 100 || IMAGE_MEAN> 900000)
  {
     SRAM_FLAG_1=0;
  }

IMAGE_MEAN = calculateMeanOfTheImage(uint8_t IMAGE_2);
  if(IMAGE_MEAN< 100 || IMAGE_MEAN> 900000)
  {
     SRAM_FLAG_2=0;
  }

IMAGE_MEAN = calculateMeanOfTheImage(uint8_t IMAGE_3);
  if(IMAGE_MEAN< 100 || IMAGE_MEAN> 900000)
   {
     SRAM_FLAG_3=0;
   }
}
