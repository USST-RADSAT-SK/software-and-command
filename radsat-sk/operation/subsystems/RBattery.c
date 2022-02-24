/**
 * @file RBattery.c
 * @date February 22, 2022
 * @author Isaac Poirier (iap992)
 */

#include <RBattery.h>
#include <RI2C.h>
#include <string.h>


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
/** Data Sent to the EPS via I2C for telemetry commands is 6 Bytes, 2 for the command, 4 for data*/
#define BATTERY_TELEM_COMMAND_LENGTH	(6)
/** Data sent to the EPS via I2C for all other commands is 4 Bytes, 2 for the command, 2 for data*/
#define BATTERY_COMMAND_LENGTH			(4)
/** Data Returned from the EPS via I2C is 4 Bytes */
#define BATTERY_RESPONSE_LENGTH			(4)
/** Delay between telemetry read/write operations with the EPS on I2C is 5ms */
#define BATTERY_I2C_TELEM_DELAY			(5)
/** Delay between other read/write operations with the EPS on I2C is 1ms */
#define BATTERY_I2C_DELAY				(1)


/***************************************************************************************************
                                          PRIVATE GLOBALS
***************************************************************************************************/

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

static uint16_t batteryI2cTalk(uint32_t command);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Request and store all of the relavant data (voltage, current, temperature) from the battery board
 * 		and battery daughter boards
 * @return A BatteryStatus object containing all of the telemetry of the battery board
 */
BatteryStatus getBatteryTelemetry(void){

	// Create a new BatteryStatus Structure
	BatteryStatus dataStorage;

	// Create a temporary variable for receiving I2C data
	uint16_t i2c_received;


	// Get ADC Output Voltages
	i2c_received = batteryI2cTalk(batteryVoltageCommandBytes[0]);
	dataStorage.outputVoltageBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE * i2c_received;

	i2c_received = batteryI2cTalk(batteryVoltageCommandBytes[1]);
	dataStorage.outputVoltage5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE * i2c_received;

	i2c_received = batteryI2cTalk(batteryVoltageCommandBytes[0]);
	dataStorage.outputVoltage3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE * i2c_received;


	// Get ADC Output Currents
	i2c_received = batteryI2cTalk(batteryCurrentCommandBytes[0]);
	dataStorage.outputCurrentBatteryBus = ADC_COUNT_TO_BATTERY_OUTPUT_CURRENT_MAG * i2c_received;

	i2c_received = batteryI2cTalk(batteryCurrentCommandBytes[1]);
	dataStorage.outputCurrentBatteryBus = ADC_COUNT_TO_5V_BUS_CURRENT_DRAW * i2c_received;

	i2c_received = batteryI2cTalk(batteryCurrentCommandBytes[2]);
	dataStorage.outputCurrentBatteryBus = ADC_COUNT_TO_3V3_BUS_CURRENT_DRAW * i2c_received;


	// Get ADC Temperatures
	i2c_received = batteryI2cTalk(batteryTemperatureCommandBytes[0]);
	dataStorage.motherboardTemp = (ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE * i2c_received)
			- ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT;

	i2c_received = batteryI2cTalk(batteryTemperatureCommandBytes[1]);
	dataStorage.daughterboardTemp1 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * i2c_received)
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	i2c_received = batteryI2cTalk(batteryTemperatureCommandBytes[2]);
	dataStorage.daughterboardTemp2 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * i2c_received)
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	i2c_received = batteryI2cTalk(batteryTemperatureCommandBytes[3]);
	dataStorage.daughterboardTemp3 = (ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SCALE * i2c_received)
				- ADC_COUNT_TO_DAUGHTERBOARD_TEMP_SHIFT;

	return dataStorage;
}



/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

static uint16_t batteryI2cTalk(uint32_t command) {

	// Create a temporary variable for receiving I2C data
	uint16_t i2c_data = 0;

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
							command, i2c_data, comm_delay);

	// return error? if an error occurs, else send the data back
	// TODO: Make this return 0 on a failure? Printf the error instead? Get advice for this
	if (error != 0) {
		return error;
	}
	else {
		return i2c_data;
	}

}





