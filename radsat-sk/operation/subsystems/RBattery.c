/**
 * @file RBattery.c
 * @date March 29, 2022
 * @author Isaac Poirier (iap992)
 */

#include <RBattery.h>
#include <RI2C.h>
#include <string.h>
#include <hal/errors.h>


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


/** Used in: (mAmps = Constant * ADC Count) */
#define ADC_COUNT_TO_BATTERY_OUTPUT_CURRENT_MAG		((float) 14.662757)
#define ADC_COUNT_TO_5V_BUS_CURRENT_DRAW			((float) 1.327547)
#define ADC_COUNT_TO_3V3_BUS_CURRENT_DRAW			((float) 1.327547)

#define BATTERY_CURRENT_DIRECTION_THRESHOLD			((int) 512)

/** Used in: (Temp in Deg C = (Scale * ADC Count) - Shift) */
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT 		((float) 273.15)
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE 		((float) 0.372434)

#define ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT 		((float) 238.57)
#define ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE 		((float) 0.3976)


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

/** Number of voltage/current calls to the Battery */
#define NUMBER_OF_VOLTAGE_AND_CURRENT_COMMANDS (3)

/** Number of temperature calls to the Battery */
#define NUMBER_OF_TEMP_COMMANDS (4)


/***************************************************************************************************
                                          PRIVATE GLOBALS
***************************************************************************************************/

static uint8_t safeFlag;

/**
 * 3 Byte commands for requesting output voltage readings for each bus
 */
static uint32_t batteryVoltageCommandBytes[3] = {
		0x10E280,		// Output Voltage of Battery
		0x10E210,		// Output Voltage of 5V Bus
		0x10E200		// Output Voltage of 3.3V Bus
};

/**
 * 3 Byte commands for requesting output current readings for each bus
 */
static uint32_t batteryCurrentCommandBytes[3] = {
		0x10E284,		// Output Current of Battery Bus in mA
		0x10E214,		// Output Current of 5V Bus in mA
		0x10E204		// Output Current of 3.3V Bus in mA
};

/**
 * 4 Byte commands for requesting temperature readings for each board
 */
static uint32_t batteryTemperatureCommandBytes[4] = {
		0x10E308,		// Motherboard Temperature
		0x10E398,		// Daughterboard 1 Temperature
		0x10E3A8,		// Daughterboard 2 Temperature
		0x10E3B8		// Daughterboard 3 Temperature
};

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static int batteryTalk(uint8_t* command, uint8_t* response);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Request and store all of the relavant data (voltage, current, temperature) from the battery board
 * 		and battery daughter boards
 * @return A BatteryStatus object containing all of the telemetry of the battery board
 */
int getBatteryTelemetry(BatteryStatus* dataStorage){

	// Create temporary variables for sending and receiving I2C data
	uint8_t i2c_command[BATTERY_TELEM_COMMAND_LENGTH] = {0};
	uint8_t i2c_data[BATTERY_RESPONSE_LENGTH] = {0};

	// Create a temporary array for calculated voltage and current before transferring into the BatteryStatus structure
	float storedData[NUMBER_OF_VOLTAGE_AND_CURRENT_COMMANDS] = {0};

	// Create a temporary array for calculated temperatures before transferring into the BatteryStatus structure
	float storedTempData[NUMBER_OF_TEMP_COMMANDS] = {0};


	// Send 3 commands to get ADC output voltage readings from the Battery
	for(int i = 0; i < NUMBER_OF_VOLTAGE_AND_CURRENT_COMMANDS; i = i + 1){
		memcpy(i2c_command, &batteryVoltageCommandBytes[i], BATTERY_TELEM_COMMAND_LENGTH);
		int error = batteryTalk(i2c_command, i2c_data);

		if(error != 0){
			return error;
		}

		memcpy(&storedData[i], i2c_data, BATTERY_RESPONSE_LENGTH);
	}
	// Get ADC Output Voltages
	dataStorage->outputVoltageBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE * storedData[0];
	dataStorage->outputVoltage5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE * storedData[1];
	dataStorage->outputVoltage3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE * storedData[2];


	// Send 3 commands to get ADC output current readings from the Battery
	for(int i = 0; i < NUMBER_OF_VOLTAGE_AND_CURRENT_COMMANDS; i = i + 1){
		memcpy(i2c_command, &batteryCurrentCommandBytes[i], BATTERY_TELEM_COMMAND_LENGTH);
		int error = batteryTalk(i2c_command, i2c_data);

		if(error != 0){
			// TODO: Some Error Raising here?
			// return the error
			return error;
		}

		memcpy(&storedData[i], i2c_data, BATTERY_RESPONSE_LENGTH);
	}
	// Get ADC Output Currents
	dataStorage->outputCurrentBatteryBus = ADC_COUNT_TO_BATTERY_OUTPUT_CURRENT_MAG * storedData[0];
	dataStorage->outputCurrent5VBus; = ADC_COUNT_TO_5V_BUS_CURRENT_DRAW * storedData[1];
	dataStorage->outputCurrent3V3Bus = ADC_COUNT_TO_3V3_BUS_CURRENT_DRAW * storedData[2];


	// Send 4 commands to get ADC output current readings from the Battery
	for(int i = 0; i < NUMBER_OF_TEMP_COMMANDS; i = i + 1){
		memcpy(i2c_command, &batteryTemperatureCommandBytes[i], BATTERY_TELEM_COMMAND_LENGTH);
		int error = batteryTalk(i2c_command, i2c_data);

		if(error != 0){
			// TODO: Some Error Raising here?
			// return the error
			return error;
		}

		memcpy(&storedTempData[i], i2c_data, BATTERY_RESPONSE_LENGTH);
	}
	// Get ADC Temperatures
	dataStorage->motherboardTemp = (ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE * storedTempData[0])
			- ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT;

	dataStorage->daughterboardTemp1 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * storedTempData[1])
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	dataStorage->daughterboardTemp2 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * storedTempData[2])
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	dataStorage->daughterboardTemp3 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * storedTempData[3])
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	return 0;
}


/**
 * Send a telecommand via I2C to check on the current battery voltage, and if under 6.5V, raise the global safeFlag
 * @return nothing.
 */
int checkSafeFlag(void){

	// Create temporary variables for sending and receiving I2C data
	uint8_t i2c_command[BATTERY_TELEM_COMMAND_LENGTH] = {0};
	uint8_t i2c_data[BATTERY_RESPONSE_LENGTH] = {0};

	memcpy(i2c_command, &batteryVoltageCommandBytes[0], BATTERY_TELEM_COMMAND_LENGTH);

	// Get the battery output voltage
	int error = batteryTalk(i2c_command, i2c_data);

	if(error != 0){
		// TODO: Some Error Raising here?
		// return the error
		return error;
	}

	float converted_value = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE * (float)(*i2c_data);

	// If the voltage is less than 6.5V then raise the safeFlag to send the cubeSat into safe mode.
	 // TODO: generate an error?
	if(converted_value < 6.5){
		safeFlag = 1;
	}
	else{
		safeFlag = 0;
	}

	return 0;
}

/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

static int batteryTalk(uint8_t* command, uint8_t* response) {

	// Create a temporary variable for receiving I2C data
	// char* i2c_data = {0};

	// Check for null pointers
	if(command == NULL || response == NULL){
		return E_INPUT_POINTER_NULL;
	}

	// Create a temp variable for passing the proper command length
	uint8_t comm_length;
	uint8_t comm_delay;

	// Variable selection for command length dependent on if it is a telemetry call
	// Byte mask to look only at the command field to see if it is 0x10 (telem call)
	if ((command && 0x0000FF0000) == 0x0000100000) {
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

	// return error? if an error occurs, else send the data back
	// TODO: Make this return 0 on a failure? Printf the error instead? Get advice for this
	if (error != 0) {
		return error;
	}
	else {
		return 0;
	}

}





