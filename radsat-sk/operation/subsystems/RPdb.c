/**
 * @file RPdb.c
 * @date February 21, 2022
 * @author Isaac Poirier (iap992)
 */

#include <RPdb.h>
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
#define ADC_COUNT_TO_3V3_PDB_CURRENT_DRAW		((float) 0.001327547)
#define ADC_COUNT_TO_5V_PDB_CURRENT_DRAW		((float) 0.001327547)
#define ADC_COUNT_TO_BATTERY_BUS_OUTPUT_CURRENT	((float) 0.005237)
#define ADC_COUNT_TO_5V_BUS_OUTPUT_CURRENT		((float) 0.005237)
#define ADC_COUNT_TO_3V3_BUS_OUTPUT_CURRENT		((float) 0.005237)


/** Used in: (Temp in Deg C = (Scale * ADC Count) - Shift) */
#define ADC_COUNT_TO_SOLAR_ARRAY_TEMP_SCALE	((float) 0.4963)

// Can use this \/ interchangeably between solar array and motherboard temp shift
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT ((float) 273.15)
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE ((float) 0.372434)


/** Used in: (Irradiance = Constant * ADC Count) */
#define ADC_COUNT_TO_IRRADIANCE				((float) 1.59725)


/**
 * Constant Values for I2C Communication
 */
/** I2C Slave Address for PDB */
#define PDB_I2C_SLAVE_ADDR 			(0x2B)

/** Data Sent to the PDB via I2C for telemetry commands is 6 Bytes, 2 for the command, 4 for data*/
#define PDB_TELEM_COMMAND_LENGTH	(6)

/** Data sent to the PDB via I2C for all other commands is 4 Bytes, 2 for the command, 2 for data*/
#define PDB_COMMAND_LENGTH			(4)

/** Data Returned from the PDB via I2C is 4 Bytes */
#define PDB_RESPONSE_LENGTH			(4)

/** Delay between telemetry read/write operations with the PDB on I2C is 5ms */
#define PDB_I2C_TELEM_DELAY			(5)

/** Delay between other read/write operations with the PDB on I2C is 1ms */
#define PDB_I2C_DELAY				(1)


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
static uint32_t PdbSunSensorCommandBytes[6] = {
	0x10E11C,     // SA1A - yPos
	0x10E11D,     // SA1B - yNeg
	0x10E12C,     // SA2A - xNeg
	0x10E12D,     // SA2B - xPos
	0x10E13C,     // SA3A - zPos
	0x10E13D      // SA3B - zNeg
};

/**
 * 4 Byte commands for requesting output voltage readings for each bus and BCR
 */
static uint32_t PdbVoltageCommandBytes[4] = {
		0x10E280,		// Output Voltage of BCR
		0x10E220,		// Output Voltage of Battery Bus
		0x10E210,		// Output Voltage of 5V Bus
		0x10E200		// Output Voltage of 3.3V Bus
};

/**
 * 4 Byte commands for requesting output current readings for each bus and BCR
 */
static uint32_t PdbCurrentCommandBytes[4] = {
		0x10E284,		// Output Current of BCR in mA
		0x10E224,		// Output Current of Battery Bus
		0x10E214,		// Output Current of 5V Bus
		0x10E204		// Output Current of 3.3V Bus
};


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static uint16_t PdbI2cTalk(uint32_t command);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/**
 * Get the Irradiance value from the sun sensor on each face of the CubeSat
 *
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
		i2c_received = PdbI2cTalk(PdbSunSensorCommandBytes[i]);
		convertedData[i] = ADC_COUNT_TO_IRRADIANCE * i2c_received;
	}

	// Now store all of the calculated data into the proper slot in the sunData structure
	sunData.xPos = convertedData[3];
	sunData.xNeg = convertedData[2];
	sunData.yPos = convertedData[0];
	sunData.yNeg = convertedData[1];
	sunData.zPos = convertedData[4];
	sunData.zNeg = convertedData[5];
	// Could change these numbers into an ENUM with the names of the connections

	return sunData;
}



/**
 * Request and store all of the relevant data (voltage, current, temperature, sun sensor data)
 *  	from the PDB board
 *
 * @return A pointer to an PDBStatus object containing all of the telemetry for the PDB system
 */
PdbStatus getPdbTelemetry(void) {

	// Create a new PDBStatus Structure
	PdbStatus dataStorage;

	// Get sun sensor data and store it in the new PDBStatus Structure
	dataStorage.sunSensorData = getSunSensorData();

	// Create a temporary variable for receiving I2C data
	uint16_t i2c_received;


	// Get ADC output voltage readings from the PDB
	i2c_received = PdbI2cTalk(PdbVoltageCommandBytes[0]);
	dataStorage.outputVoltageBCR = ADC_COUNT_TO_BCR_OUTPUT_VOLTAGE * i2c_received;

	i2c_received = PdbI2cTalk(PdbVoltageCommandBytes[1]);
	dataStorage.outputVoltageBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE * i2c_received;

	i2c_received = PdbI2cTalk(PdbVoltageCommandBytes[2]);
	dataStorage.outputVoltage5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE * i2c_received;

	i2c_received = PdbI2cTalk(PdbVoltageCommandBytes[3]);
	dataStorage.outputVoltage3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE * i2c_received;


	// Get ADC Output current readings from the PDB
	i2c_received = PdbI2cTalk(PdbCurrentCommandBytes[0]);
	dataStorage.outputCurrentBCR_mA = ADC_COUNT_TO_BCR_OUTPUT_CURRENT * i2c_received;

	i2c_received = PdbI2cTalk(PdbCurrentCommandBytes[1]);
	dataStorage.outputCurrentBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_CURRENT * i2c_received;

	i2c_received = PdbI2cTalk(PdbCurrentCommandBytes[2]);
	dataStorage.outputCurrent5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_CURRENT * i2c_received;

	i2c_received = PdbI2cTalk(PdbCurrentCommandBytes[3]);
	dataStorage.outputCurrent3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_CURRENT * i2c_received;


	// Get ADC temperature reading from the PDB
	i2c_received = PdbI2cTalk(0x10E308);	// Telemetry code for motherboard temperature
	dataStorage.PdbTemperature = (ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE * i2c_received)
										- ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT ;

	return dataStorage;
}


/**
 * Pet the Communications watchdog on the PDB using code 0x22
 * @return either an error if it occurred, or 0
 */
int petPdbWatchdog(void){

	// One way communication so just use transmit using reset watchdog command 0x22
	int error = i2cTransmit(PDB_I2C_SLAVE_ADDR, 0x2200, PDB_COMMAND_LENGTH);
	if (error != 0) {
		return error;
	}
	else {
		return 0;
	}
}



/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/


static uint16_t PdbI2cTalk(uint32_t command) {

	// Create a temporary variable for receiving I2C data
	uint16_t i2c_data = 0;

	// Create a temp variable for passing the proper command length
	uint8_t comm_length;
	uint8_t comm_delay;

	// Variable selection for command length dependent on if it is a telemetry call
	// Byte mask to look only at the command field to see if it is 0x10 (telem call)
	if ((command && 0x0000FF0000) == 0x0000100000) {
		comm_length = PDB_TELEM_COMMAND_LENGTH;
		comm_delay = PDB_I2C_TELEM_DELAY;
	}
	else {
		comm_length = PDB_COMMAND_LENGTH;
		comm_delay = PDB_I2C_DELAY;
	}

	// tell PDB to give us data; and store into our internal buffer
	int error = i2cTalk(PDB_I2C_SLAVE_ADDR, comm_length, PDB_RESPONSE_LENGTH,
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
