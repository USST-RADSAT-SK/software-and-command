/**
 * @file RPdb.c
 * @date April 09, 2022
 * @author Isaac Poirier (iap992)
 */

#include <RPdb.h>
#include <RI2c.h>
#include <string.h>
#include <RCommon.h>


/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
/**
 * Conversion Constants for between ADC Values and Real Values
 */

/** Used in: (Volts = Constant * ADC Count) */
#define ADC_COUNT_TO_BCR_OUTPUT_VOLTAGE			((float) 0.008993157)
#define ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE	((float) 0.008978)
#define ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE		((float) 0.005865)
#define ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE		((float) 0.004311)


/** Used in: (mAmps = Constant * ADC Count) */
#define ADC_COUNT_TO_BCR_OUTPUT_CURRENT			((float) 14.662757)

/** Used in: (Amps = Constant * ADC Count) */
#define ADC_COUNT_TO_BATTERY_BUS_OUTPUT_CURRENT	((float) 0.005237)
#define ADC_COUNT_TO_5V_BUS_OUTPUT_CURRENT		((float) 0.005237)
#define ADC_COUNT_TO_3V3_BUS_OUTPUT_CURRENT		((float) 0.005237)


/** Used in: (Temp in Deg C = (Scale * ADC Count) - Shift) */
#define ADC_COUNT_TO_SOLAR_ARRAY_TEMP_SCALE		((float) 0.4963)

// Can use this \/ interchangeably between solar array and motherboard temp shift
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT 	((float) 273.15)
#define ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE 	((float) 0.372434)


/** Used in: (Irradiance = Constant * ADC Count) */
#define ADC_COUNT_TO_IRRADIANCE					((float) 1.59725)


/**
 * Constant Values for I2C Communication
 */
/** I2C Slave Address for PDB */
#define PDB_I2C_SLAVE_ADDR 			((uint16_t) 0x2B)

/** Data Sent to the PDB via I2C for telemetry commands is 3 Bytes, 1 for the command, 2 for data*/
#define PDB_TELEM_COMMAND_LENGTH	(3)

/** Data sent to the PDB via I2C for all other commands is 2 Bytes, 1 for the command, 1 for data*/
#define PDB_COMMAND_LENGTH			(2)

/** Data Returned from the PDB via I2C is 2 Bytes */
#define PDB_RESPONSE_LENGTH			(2)

/** Delay between telemetry read/write operations with the PDB on I2C is 5ms */
#define PDB_I2C_TELEM_DELAY_MS		(5)

/** Delay between other read/write operations with the PDB on I2C is 1ms */
#define PDB_I2C_DELAY_MS			(1)

/** Number of sun sensors on the CubeSat */
#define NUM_SUN_SENSORS				(6)

/** Number of other Telem Command Bytes */
#define NUM_TELEM_CALLS 			(4)

/** Filter Mask and Comparison value for Recognizing Telemetry Calls */
#define TELEM_MASK 					(0xFF0000)
#define TELEM_CALL_COMPARISON		(0x100000)


/***************************************************************************************************
                                          PRIVATE GLOBALS
***************************************************************************************************/
/**
 *  Commands to request data for each of the 6 sun-sensors
 *	located on each face of the CubeSat Solar Arrays.
 */
static uint32_t pdbSunSensorCommandBytes[6] = {
		0x10E11C,     // SA1A - yPos
		0x10E11D,     // SA1B - yNeg
		0x10E12C,     // SA2A - xNeg
		0x10E12D,     // SA2B - xPos
		0x10E13C,     // SA3A - zNeg
		0x10E13D      // SA3B - zPos
};

/**
 * Commands for requesting output voltage readings for each bus and BCR
 */
static uint32_t pdbVoltageCommandBytes[4] = {
		0x10E280,		// Output Voltage of BCR
		0x10E220,		// Output Voltage of Battery Bus
		0x10E210,		// Output Voltage of 5V Bus
		0x10E200		// Output Voltage of 3.3V Bus
};

/**
 * Commands for requesting output current readings for each bus and BCR
 */
static uint32_t pdbCurrentCommandBytes[4] = {
		0x10E284,		// Output Current of BCR in mA
		0x10E224,		// Output Current of Battery Bus
		0x10E214,		// Output Current of 5V Bus
		0x10E204		// Output Current of 3.3V Bus
};

/**
 * Command for PDB Temperature Reading
 */
static uint32_t pdbTemperatureCommandBytes = 0x10E308;

/**
 * Command for Resetting PDB Watchdog (0x2200)
 */
static uint8_t pdbWatchdogResetCommand[2] = {0x22, 0x00};


/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS
***************************************************************************************************/

static int pdbTalk(uint8_t* command, uint8_t* response);


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/
/**
 * Get the Irradiance (W/m^2) value from the sun sensor on each face of the CubeSat
 *
 * @param A pointer to a sun_sensor_status_t object, which will be filled with the converted ADC data for each sun sensor
 * @return an error code, or 0 if no error
 */
int getSunSensorData(sun_sensor_status_t* sunData) {

	// Create temporary variables for sending and receiving I2C data
	uint8_t command[PDB_TELEM_COMMAND_LENGTH] = {0};
	uint8_t response[PDB_RESPONSE_LENGTH] = {0};

	// Create a temporary array for calculated sun data before transferring into the structure sunData
	float convertedData[NUM_SUN_SENSORS] = {0};

	// Send 6 commands to get each of the sun sensor's data
	for (int i = 0; i < NUM_SUN_SENSORS; i = i + 1) {

		// Load command into output buffer
		memset(command, 0, sizeof(command));
		memcpy(command, &pdbSunSensorCommandBytes[i], PDB_TELEM_COMMAND_LENGTH);

		memset(response, 0, sizeof(response));

		int error = pdbTalk(command, response);

		if (error != SUCCESS) {
			return error;
		}

		memset(&convertedData[i], 0, sizeof(convertedData[i]));
		memcpy(&convertedData[i], response, PDB_RESPONSE_LENGTH);

		convertedData[i] = ADC_COUNT_TO_IRRADIANCE * ((float)convertedData[i]);
	}

	// Now store all of the calculated data into the proper slot in the sunData structure
	sunData->xPos = convertedData[3]; 	// SA2B
	sunData->xNeg = convertedData[2]; 	// SA2A
	sunData->yPos = convertedData[0]; 	// SA1A
	sunData->yNeg = convertedData[1]; 	// SA1B
	sunData->zPos = convertedData[5]; 	// SA3B
	sunData->zNeg = convertedData[4]; 	// SA3A
	// Could change these numbers into an ENUM with the names of the connections

	return SUCCESS;
}


/**
 * Request and store all of the relevant data (voltage, current, temperature, sun sensor data)
 *  	from the PDB board
 *
 * @param A pointer to an PDBStatus object containing all of the telemetry for the PDB system
 * @return either 0 or an error code
 */
int getPdbTelemetry(pdb_status_t* dataStorage) {

	// Create an empty sunSensorData structure
	sun_sensor_status_t sunSensorData;

	// Get sun sensor data and store it in the new pdb_status_t Structure
	getSunSensorData(&dataStorage->sunSensorData);

	// Create temporary variables for sending and receiving I2C data
	uint8_t command[PDB_TELEM_COMMAND_LENGTH] = {0};
	uint8_t response[PDB_RESPONSE_LENGTH] = {0};

	// Create a temporary array for calculated sun data before transferring into the structure sunData
	float storedData[NUM_TELEM_CALLS] = {0};

	// Send 4 commands to get ADC output voltage readings from the PDB
	for (int i = 0; i < NUM_TELEM_CALLS; i = i + 1) {

		memset(command, 0, sizeof(command));
		memcpy(command, &pdbVoltageCommandBytes[i], PDB_TELEM_COMMAND_LENGTH);

		memset(response, 0, sizeof(response));

		int error = pdbTalk(command, response);

		if (error != SUCCESS) {
			return error;
		}

		// Clear storedData[i] before using it
		memset(&storedData[i], 0, sizeof(storedData[i]));
		memcpy(&storedData[i], response, PDB_RESPONSE_LENGTH);

	}

	dataStorage->outputVoltageBCR = ADC_COUNT_TO_BCR_OUTPUT_VOLTAGE * storedData[0];
	dataStorage->outputVoltageBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_VOLTAGE * storedData[1];
	dataStorage->outputVoltage5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_VOLTAGE * storedData[2];
	dataStorage->outputVoltage3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_VOLTAGE * storedData[3];

	// Send 4 commands to get ADC output current readings from the PDB
	for (int i = 0; i < NUM_TELEM_CALLS; i = i + 1) {

		memset(command, 0, sizeof(command));
		memcpy(command, &pdbCurrentCommandBytes[i], PDB_TELEM_COMMAND_LENGTH);

		memset(response, 0, sizeof(response));

		int error = pdbTalk(command, response);

		if (error != SUCCESS) {
			return error;
		}

		memset(&storedData[i], 0, sizeof(storedData[i]));
		memcpy(&storedData[i], response, PDB_RESPONSE_LENGTH);
	}


	// Get ADC Output current readings from the PDB
	dataStorage->outputCurrentBCR_mA = ADC_COUNT_TO_BCR_OUTPUT_CURRENT * storedData[0];
	dataStorage->outputCurrentBatteryBus = ADC_COUNT_TO_BATTERY_BUS_OUTPUT_CURRENT * storedData[1];
	dataStorage->outputCurrent5VBus = ADC_COUNT_TO_5V_BUS_OUTPUT_CURRENT * storedData[2];
	dataStorage->outputCurrent3V3Bus = ADC_COUNT_TO_3V3_BUS_OUTPUT_CURRENT * storedData[3];


	// Get ADC temperature reading from the PDB
	memset(command, 0, sizeof(command));
	memcpy(command, &pdbTemperatureCommandBytes, PDB_TELEM_COMMAND_LENGTH);

	int error = pdbTalk(command, response);

	if (error != SUCCESS) {
		return error;
	}

	dataStorage->PdbTemperature = (ADC_COUNT_TO_MOTHERBOARD_TEMP_SCALE * (float)(*response))
										- ADC_COUNT_TO_MOTHERBOARD_TEMP_SHIFT;

	return SUCCESS;
}


/**
 * Pet the Communications watchdog on the PDB using code 0x22
 * @return either an error if it occurred, or 0
 */
int petPdbWatchdog(void) {
	// One way communication so just use transmit using reset watchdog command 0x22
	int error = i2cTransmit(PDB_I2C_SLAVE_ADDR, pdbWatchdogResetCommand, PDB_COMMAND_LENGTH);

	if (error != SUCCESS) {
		return error;
	}

	return SUCCESS;
}



/***************************************************************************************************
                                         PRIVATE FUNCTIONS
***************************************************************************************************/


static int pdbTalk(uint8_t* command, uint8_t* response) {

	// Check for null pointers
	if (command == NULL || response == NULL) {
		return E_INPUT_POINTER_NULL;
	}

	// Create a temp variable for passing the proper command length
	uint8_t comm_length;
	uint8_t comm_delay;

	// Variable selection for command length dependent on if it is a telemetry call
	// Byte mask to look only at the command field to see if it is 0x10 (telem call)
	//TODO: Will this method of comparison actually work with the new uint8_t* size of 'command'
	if ((command && TELEM_MASK) == TELEM_CALL_COMPARISON) {
		comm_length = PDB_TELEM_COMMAND_LENGTH;
		comm_delay = PDB_I2C_TELEM_DELAY_MS;
	}
	else {
		comm_length = PDB_COMMAND_LENGTH;
		comm_delay = PDB_I2C_DELAY_MS;
	}

	// tell PDB to give us data; and store into our internal buffer
	int error = i2cTalk(PDB_I2C_SLAVE_ADDR, comm_length, PDB_RESPONSE_LENGTH,
							command, response, comm_delay);

	if (error != SUCCESS) {
		return error;
	}

	return SUCCESS;
}
