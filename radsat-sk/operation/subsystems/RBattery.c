/**
 * @file RBattery.c
 * @date March 29, 2022
 * @author Isaac Poirier (iap992)
 */

#include <RBattery.h>
#include <RCommon.h>
#include <RI2c.h>
#include <string.h>

static void bigEndianUint16( uint16_t* val ) {
    *val = (*val << 8) | (*val >> 8 );
}

#ifdef DEBUG
static char* batGetErrorMessage(uint8_t error) {
	switch (error) {
	case 0x10:
		return "CRC code does not match data";
	case 0x01:
		return "Unknown command received";
	case 0x02:
		return "Supplied data incorrect when processing command";
	case 0x03:
		return "Selected channel does not exist";
	case 0x04:
		return "Selected channel is currently inactive";
	case 0x13:
		return "A reset had to occur";
	case 0x14:
		return "There was an error with the ADC acquisition";
	case 0x20:
		return "Reading from EEPROM generated an error";
	case 0x30:
		return "Generic warning about an error on the internal SPI bus";
	default:
		return "Unknow error code";
	}
}

#endif
/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
/**
 * Conversion Constants for between ADC Values and Real Values
 */
/** Used in: (Volts = Constant * ADC Count) */
#define ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE		((float) 0.008993)
#define ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE			((float) 0.005865)
#define ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE			((float) 0.004311)

#define BATTERY_VOLTAGE_SAFEMODE_THRESHOLD			((float) 6.5)

/** Used in: (mAmps = Constant * ADC Count) */
#define ADC_COUNT_TO_BATTERY_OUTPUT_CURRENT_MAG		((float) 14.662757)
#define ADC_COUNT_TO_5V_BUS_CURRENT_DRAW			((float) 1.327547)
#define ADC_COUNT_TO_3V3_BUS_CURRENT_DRAW			((float) 1.32754)

#define BATTERY_CURRENT_DIRECTION_THRESHOLD			((int) 512)

/** Used in: (Temp in Deg C = (Scale * ADC Count) - Shift) */
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT 		((float) 273.15)
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE 		((float) 0.372434)

#define ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT 		((float) 238.57)
#define ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE 		((float) 0.3976)

#define BAT_ERROR (-100)


/**
 * Constant Values for I2C Communication
 */
/** I2C Slave Address for EPS */
#define BATTERY_I2C_SLAVE_ADDR 			(0x2A)

/** Data Sent to the EPS via I2C for telemetry commands is 3 Bytes, 1 for the command, 2 for data*/
#define BATTERY_TELEM_COMMAND_LENGTH	(3)

/** Data sent to the EPS via I2C for all other commands is 2 Bytes, 1 for the command, 1 for data*/
#define BATTERY_COMMAND_LENGTH			(2)

/** Data Returned from the EPS via I2C is 4 Bytes */
#define BATTERY_RESPONSE_LENGTH			(2)

/** Delay between telemetry read/write operations with the EPS on I2C is 5ms */
#define BATTERY_I2C_TELEM_DELAY			(5)

/** Delay between other read/write operations with the EPS on I2C is 1ms */
#define BATTERY_I2C_DELAY				(1)

/** Number of voltage calls to the Battery */
#define NUMBER_OF_VOLTAGE_COMMANDS (3)

/** Number of current calls to the Battery*/
#define NUMBER_OF_CURRENT_COMMANDS (4)

/** Number of temperature calls to the Battery */
#define NUMBER_OF_TEMP_COMMANDS (4)

/** Filter Mask and Comparison value for Recognizing Telemetry Calls */
#define TELEM_MASK 					(0xFF)
#define TELEM_CALL_COMPARISON		(0x10)


/***************************************************************************************************
                                          PRIVATE GLOBALS
***************************************************************************************************/
/** Charging or discharging enum for current direction of the battery */
enum _battery_current_direction {
	batteryDischarging,
	batteryCharging
};

static uint8_t getLastError[2] = {
		0x03, 0x00
};

/**
 * 3 Byte commands for requesting output voltage readings for each bus
 */
static uint32_t batteryVoltageCommandBytes[3] = {
		0x80E210,		// Output Voltage of Battery
		0x10E210,		// Output Voltage of 5V Bus
		0x00E210		// Output Voltage of 3.3V Bus
};

/**
 * 3 Byte commands for requesting output current readings for each bus
 */
static uint32_t batteryCurrentCommandBytes[4] = {
		0x84E210,		// Output Current of Battery Bus in mA
		0x14E210,		// Output Current of 5V Bus in mA
		0x04E210,		// Output Current of 3.3V Bus in mA
		0x8eE210		// Battery Current Direction
};

/**
 * 4 Byte commands for requesting temperature readings for each board
 */
static uint32_t batteryTemperatureCommandBytes[4] = {
		0x08E310,		// Motherboard Temperature
		0x98E310,		// Daughterboard 1 Temperature
		0xA8E310,		// Daughterboard 2 Temperature
		0xB8E310		// Daughterboard 3 Temperature
};

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static int batteryTalk(uint8_t* command, uint8_t* response);
static int checkSafeFlag(uint8_t* safeFlag);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Request and store all of the relevant data (voltage, current, temperature) from the battery board
 * 		and battery daughter boards
 *
 * @param a pointer to a battery_status_t structure
 *
 * @return 0 on success, else an error
 */
int batteryTelemetry(battery_telemetry* dataStorage) {

	// Create temporary variables for sending and receiving I2C data
	uint8_t command[BATTERY_TELEM_COMMAND_LENGTH] = { 0 };
	uint16_t response = 0;

	// Create a temporary array for calculated voltage and current before transferring into the battery_status_t structure
	float storedData[NUMBER_OF_CURRENT_COMMANDS] = { 0 };

	// Create a temporary array for calculated temperatures before transferring into the battery_status_t structure
	float storedTemperatureData[NUMBER_OF_TEMP_COMMANDS] = { 0 };


	// Send 3 commands to get ADC output voltage readings from the Battery
	for (int i = 0; i < NUMBER_OF_VOLTAGE_COMMANDS; i = i + 1) {
		memset(command, 0, sizeof(command));
		memcpy(command, &batteryVoltageCommandBytes[i], BATTERY_TELEM_COMMAND_LENGTH);

		int error = batteryTalk(command, (uint8_t*)&response);

		if (error != SUCCESS && error > BAT_ERROR) {
			errorPrint("Error = %d\n", error);
			continue;
			//return error;
		}

		bigEndianUint16(&response);
		storedData[i] = response;
	}

	// Get ADC Output Voltages
	dataStorage->outputVoltageBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE * storedData[0];
	dataStorage->outputVoltage5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE * storedData[1];
	dataStorage->outputVoltage3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE * storedData[2];


	// Send 4 commands to get ADC output current readings from the Battery
	for (int i = 0; i < NUMBER_OF_CURRENT_COMMANDS; i = i + 1) {
		memset(command, 0, sizeof(command));
		memcpy(command, &batteryCurrentCommandBytes[i], BATTERY_TELEM_COMMAND_LENGTH);

		int error = batteryTalk(command, (uint8_t*)&response);

		if (error != SUCCESS && error > BAT_ERROR) {
			errorPrint("Error = %d\n", error);
			continue;
			//return error;
		}

		bigEndianUint16(&response);
		storedData[i] = response;
	}

	// Get ADC Output Currents
	dataStorage->outputCurrentBatteryBus = ADC_COUNT_TO_BATTERY_OUTPUT_CURRENT_MAG * storedData[0];
	dataStorage->outputCurrent5VBus  = ADC_COUNT_TO_5V_BUS_CURRENT_DRAW * storedData[1];
	dataStorage->outputCurrent3V3Bus = ADC_COUNT_TO_3V3_BUS_CURRENT_DRAW * storedData[2];

	// An if/else to assign forward/backwards directionality to batteryCurrentDirection
	if (storedData[3] < BATTERY_CURRENT_DIRECTION_THRESHOLD) {
		dataStorage->batteryCurrentDirection = batteryCharging;
	}
	else {
		dataStorage->batteryCurrentDirection = batteryDischarging;
	}

	// Send 4 commands to get ADC temperature readings from the Battery
	for (int i = 0; i < NUMBER_OF_TEMP_COMMANDS; i = i + 1) {
		memset(command, 0, sizeof(command));
		memcpy(command, &batteryTemperatureCommandBytes[i], BATTERY_TELEM_COMMAND_LENGTH);

		int error = batteryTalk(command, (uint8_t*)&response);

		if (error != SUCCESS && error > BAT_ERROR) {
			errorPrint("Error = %d\n", error);
			continue;
			//return error;
		}

		bigEndianUint16(&response);
		storedTemperatureData[i] = response;
	}

	// Get ADC Temperatures
	dataStorage->motherboardTemp = (ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE * storedTemperatureData[0])
			- ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT;

	dataStorage->daughterboardTemp1 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * storedTemperatureData[1])
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	dataStorage->daughterboardTemp2 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * storedTemperatureData[2])
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	dataStorage->daughterboardTemp3 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * storedTemperatureData[3])
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	return SUCCESS;
}

/**
 * Set the safeFlag to show if the current voltage of the battery is below our threshold value
 *
 * @param a pointer to the safeFlag (a uint8_t)
 *
 * @return 0 on success, else an error
 */
int batteryIsNotSafe(uint8_t* safeFlag) {

	int error = checkSafeFlag(safeFlag);

	if (error != SUCCESS) {
		return error;
	}

	return SUCCESS;
}


/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

/**
 * Communicates with the Battery board over I2C using the i2cTalk() commands
 *
 * @param A pointer to a array containing the command to be sent to the Battery
 * @param A pointer to the array in which the response from the battery will be stored
 *
 * @return 0 on success, else an error
 */
static int batteryTalk(uint8_t* command, uint8_t* response) {

	// Create a temporary variable for receiving I2C data
	// char* response = {0};

	// Check for null pointers
	if (command == NULL || response == NULL) {
		return E_INPUT_POINTER_NULL;
	}

	// Create a temp variable for passing the proper command length
	uint8_t comm_length;
	uint8_t comm_delay;

	// Variable selection for command length dependent on if it is a telemetry call
	// Byte mask to look only at the command field to see if it is 0x10 (telem call)
	if (command[0] == TELEM_CALL_COMPARISON) {
		comm_length = BATTERY_TELEM_COMMAND_LENGTH;
		comm_delay = BATTERY_I2C_TELEM_DELAY;
	}
	else {
		comm_length = BATTERY_COMMAND_LENGTH;
		comm_delay = BATTERY_I2C_DELAY;
	}

	// tell Battery to give us data; and store into our internal buffer
	int error = i2cTalk(BATTERY_I2C_SLAVE_ADDR, comm_length, BATTERY_RESPONSE_LENGTH,
							command, response, comm_delay);

	if (error != SUCCESS) {
		return error;
	}
	if(response[0] == 0xff){
		uint8_t batError[2];
		batteryTalk(getLastError, batError);
		//bigEndianUint16(&Error);
		errorPrint(" Error return = %u - %s", batError[1], batGetErrorMessage(batError[1]));
		return BAT_ERROR - batError[1];
	}

	return SUCCESS;
}


/**
 * Send a telecommand via I2C to check on the current battery voltage, and if under 6.5V, raise the safeFlag
 *
 * @param a pointer to the safeFlag variable
 *
 * @return 0 on success, else an error
 */
static int checkSafeFlag(uint8_t* safeFlag) {

	// Create temporary variables for sending and receiving I2C data
	uint8_t command[BATTERY_TELEM_COMMAND_LENGTH] = {0};
	uint8_t response[BATTERY_RESPONSE_LENGTH] = {0};

	memset(command, 0, sizeof(command));
	memcpy(command, &batteryVoltageCommandBytes[0], BATTERY_TELEM_COMMAND_LENGTH);

	memset(response, 0, sizeof(response));

	// Get the battery output voltage
	int error = batteryTalk(command, response);

	if (error != SUCCESS) {
		return error;
	}

	float converted_value = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE * (float)(*response);

	// If the voltage is less than 6.5V then raise the safeFlag to send the cubeSat into safe mode.
	if (converted_value < BATTERY_VOLTAGE_SAFEMODE_THRESHOLD) {
		*safeFlag = 1;
	}
	else {
		*safeFlag = 0;
	}

	return SUCCESS;
}




