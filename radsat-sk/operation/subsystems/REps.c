/**
 * @file REps.c
 * @date February 21, 2022
 * @author Isaac Poirier (iap992)
 */

#include <REps.h>
#include <RI2c.h>
#include <string.h>



/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
/**
 * Conversion Constants for between ADC Values and Real Values
 */

/** Used in: (Volts = Constant * ADC Count) */
#define ADC_COUNT_TO_BCR_OUTPUT_VOLTAGE			((float) 0.008993157)
#define ADC_COUNT_TO_BCR_VOLTAGE				((float) 0.0322581)
#define ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE	((float) 0.008978)
#define ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE		((float) 0.005865)
#define ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE		((float) 0.004311)


/** Used in: (mAmps = Constant * ADC Count) */
#define ADC_COUNT_TO_BCR_OUTPUT_CURRENT			((float) 14.662757)

/** Used in: (Amps = Constant * ADC Count) */
#define ADC_COUNT_TO_BCR_AMPERAGE				((float) 0.0009775)
#define ADC_COUNT_TO_3V3_EPS_CURRENT_DRAW		((float) 0.001327547)
#define ADC_COUNT_TO_5V_EPS_CURRENT_DRAW		((float) 0.001327547)
#define ADC_COUNT_TO_BATTERY_BUS_OUTPUT_CURRENT	((float) 0.005237)
#define ADC_COUNT_TO_5V_BUS_OUTPUT_CURRENT		((float) 0.005237)
#define ADC_COUNT_TO_3V3_BUS_OUTPUT_CURRENT		((float) 0.005237)


/** Used in: (Temp in Deg C = (Scale * ADC Count) - Shift) */
#define ADC_COUNT_TO_SOLAR_ARRAY_TEMP_SCALE	((float) 0.4963)
#define ADC_COUNT_TO_SOLAR_ARRAY_TEMP_SHIFT ((float) 273.15)
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE ((float) 0.372434)


/** Used in: (Irradiance = Constant * ADC Count) */
#define ADC_COUNT_TO_IRRADIANCE				((float) 1.59725)


/**
 * Constant Values for I2C Communication
 */
/** I2C Slave Address for EPS */
#define EPS_I2C_SLAVE_ADDR 			(0x2B)
/** Data Sent to the EPS via I2C for telemetry commands is 6 Bytes, 2 for the command, 4 for data*/
#define EPS_TELEM_COMMAND_LENGTH	(6)
/** Data sent to the EPS via I2C for all other commands is 4 Bytes, 2 for the command, 2 for data*/
#define EPS_COMMAND_LENGTH			(4)
/** Data Returned from the EPS via I2C is 4 Bytes */
#define EPS_RESPONSE_LENGTH			(4)
/** Delay between telemetry read/write operations with the EPS on I2C */
#define EPS_I2C_TELEM_DELAY			(5)


/**
 * Constant Values for I2C Communication
 */
/** Number of sun sensors on the CubeSat */
#define NUM_SUN_SENSORS		(6)




/***************************************************************************************************
                                          PRIVATE GLOBALS
***************************************************************************************************/
/**
 *  4 Byte commands to request data for each of the 6 sun-sensors
 *	located on each face of the CubeSat Solar Arrays.
 */
static uint16_t epsSunSensorCommandBytes[6] = {
	0x10E11C,     // SA1A - xPos? TODO: Fill out which side is which connection
	0x10E11D,     // SA1B -
	0x10E12C,     // SA2A -
	0x10E12D,     // SA2B -
	0x10E13C,     // SA3A -
	0x10E13D      // SA3B -
};

/**
 * 4 Byte commands for requesting output voltage readings for each bus and BCR
 */
static uint16_t epsVoltageCommandBytes[4] = {
		0x10E280,		// Output Voltage of BCR
		0x10E220,		// Output Voltage of Battery Bus
		0x10E210,		// Output Voltage of 5V Bus
		0x10E200		// Output Voltage of 3.3V Bus
};

/**
 * 4 Byte commands for requesting output current readings for each bus and BCR
 */
static uint16_t epsCurrentCommandBytes[4] = {
		0x10E284,		// Output Current of BCR in mA
		0x10E224,		// Output Current of Battery Bus
		0x10E214,		// Output Current of 5V Bus
		0x10E204		// Output Current of 3.3V Bus
};


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static uint16_t epsI2cTalk(uint32_t command);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/**
 * Get the Irradiance value from the sun sensor on each face of the CubeSat
 *
 * @param N/A
 * @return A pointer to a SunSensorStatus object containing the converted ADC data for each sun sensor
 */
SunSensorStatus getSunSensorData(void) {

	// Create a new SunSensorStatus Structure
	SunSensorStatus sunData;

	// Create a temporary variable for receiving I2C data
	uint16_t i2c_received;

	// Create a temporary array for calculated sun data before transferring into the structure sunData
	float convertedData[NUM_SUN_SENSORS];

	// Send 6 commands to get each of the sun sensor's data
	for(int i = 0; i < NUM_SUN_SENSORS; i = i + 1){
		i2c_received = epsI2cTalk(epsSunSensorCommandBytes[i]);
		convertedData[i] = ADC_COUNT_TO_IRRADIANCE * i2c_received;
	}

	// Now store all of the calculated data into the proper slot in the sunData structure
	sunData.xPos = convertedData[0];
	sunData.xNeg = convertedData[1];
	sunData.yPos = convertedData[2];
	sunData.yNeg = convertedData[3];
	sunData.zPos = convertedData[4];
	sunData.zNeg = convertedData[5];
	// Could change these numbers into an ENUM with the names of the connections
	/** TODO: Add Timestamp data into sunData  */

	return sunData;
}



/**
 * Request and store all of the relevant data (voltage, current, temperature, sun sensor data)
 *  	from the EPS board
 *
 * @param N/A
 * @return A pointer to an EpsStatus object containing all of the telemetry for the EPS system
 */
EpsStatus getEpsTelemetry(void) {

	// Create a new EpsStatus Structure
	EpsStatus dataStorage;

	// Get sun sensor data and store it in the new EpsStatus Structure
	dataStorage.sunSensorData = getSunSensorData();

	// Create a temporary variable for receiving I2C data
	uint16_t i2c_received;

	// Get ADC output voltage readings from the EPS
	i2c_received = epsI2cTalk(epsVoltageCommandBytes[0]);
	dataStorage.outputVoltageBCR = ADC_COUNT_TO_BCR_OUTPUT_VOLTAGE * i2c_received;

	i2c_received = epsI2cTalk(epsVoltageCommandBytes[1]);
	dataStorage.outputVoltageBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE * i2c_received;

	i2c_received = epsI2cTalk(epsVoltageCommandBytes[2]);
	dataStorage.outputVoltage5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE * i2c_received;

	i2c_received = epsI2cTalk(epsVoltageCommandBytes[3]);
	dataStorage.outputVoltage3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE * i2c_received;


	// Get ADC Output current readings from the EPS
	i2c_received = epsI2cTalk(epsCurrentCommandBytes[0]);
	dataStorage.outputCurrentBCR_mA = ADC_COUNT_TO_BCR_OUTPUT_CURRENT * i2c_received;

	i2c_received = epsI2cTalk(epsCurrentCommandBytes[1]);
	dataStorage.outputCurrentBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_CURRENT * i2c_received;

	i2c_received = epsI2cTalk(epsCurrentCommandBytes[2]);
	dataStorage.outputCurrent5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_CURRENT * i2c_received;

	i2c_received = epsI2cTalk(epsCurrentCommandBytes[3]);
	dataStorage.outputCurrent3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_CURRENT * i2c_received;


	return dataStorage;
}






/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/

//static EpsStatus get


static uint16_t epsI2cTalk(uint32_t command) {

	// Create a temporary variable for receiving I2C data
	uint16_t i2c_data = 0;

	// Create a temp variable for passing the proper command length
	uint8_t comm_length;

	// Variable selection for command length dependent on if it is a telemetry call
	// Byte mask to look only at the command field to see if it is 0x10 (telem call)
	if ((command && 0x0000FF0000) == 0x0000100000) {
		comm_length = EPS_TELEM_COMMAND_LENGTH;
	}
	else {
		comm_length = EPS_COMMAND_LENGTH;
	}

	// tell EPS to give us sun sensor data; and store into our internal buffer
	int error = i2cTalk(EPS_I2C_SLAVE_ADDR, comm_length, EPS_RESPONSE_LENGTH,
							command, i2c_data, EPS_I2C_TELEM_DELAY);

	// return error? if an error occurs, else send the data back
	// TODO: Make this return 0 on a failure? Printf the error instead? Get advice for this
	if (error != 0) {
		return error;
	}
	else {
		return i2c_data;
	}

}
