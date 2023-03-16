/**
 * @file RPdb.c
 * @date April 09, 2022
 * @author Isaac Poirier (iap992)
 */

#include <RPdb.h>
#include <RI2c.h>
#include <string.h>
#include <RCommon.h>

void bigEndianUint16( uint16_t* val ) {
    *val = (*val << 8) | (*val >> 8 );
}

#ifdef DEBUG
static char* pdbGetErrorMessage(uint8_t error) {
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
#define NUM_SUN_SENSORS				(9)

/** Number of other Telem Command Bytes */
#define NUM_TELEM_CALLS 			(4)

/** Filter Mask and Comparison value for Recognizing Telemetry Calls */
#define TELEM_CALL_COMPARISON		(0x10)

#define PDB_ERROR 					(-110)


/***************************************************************************************************
                                          PRIVATE GLOBALS
***************************************************************************************************/
/**
 *  Commands to request data for each of the 6 sun-sensors
 *	located on each face of the CubeSat Solar Arrays.
 */
static uint32_t pdbSunSensorCommandBytes[NUM_SUN_SENSORS] = {
		0x10E110,
		0x14E110,
		0x15E110,
		0x20E110,
		0x24E110,
		0x25E110,
		0x30E110,
		0x34E110,
		0x35E110
};

static uint16_t resetCommand = 0x0080;
static uint16_t resetPDMCommand = 0x0f70;

/**
 * Commands for requesting output voltage readings for each bus and BCR
 */
static uint32_t pdbVoltageCommandBytes[4] = {
		0x80E210,		// Output Voltage of BCR
		0x20E210,		// Output Voltage of Battery Bus
		0x10E210,		// Output Voltage of 5V Bus
		0x00E210		// Output Voltage of 3.3V Bus
};

/**
 * Commands for requesting output current readings for each bus and BCR
 */
static uint32_t pdbCurrentCommandBytes[4] = {
		0x84E210,		// Output Current of BCR in mA
		0x24E210,		// Output Current of Battery Bus
		0x14E210,		// Output Current of 5V Bus
		0x04E210		// Output Current of 3.3V Bus
};


static uint8_t getLastError[2] = {
		0x03, 0x00
};

/**
 * Command for PDB Temperature Reading
 */
static uint32_t pdbTemperatureCommandBytes = 0x08e310;

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
int pdbSunSensorData(sun_sensor_data* sunData) {

	// Create temporary variables for sending and receiving I2C data
	uint8_t command[PDB_TELEM_COMMAND_LENGTH] = {0};
	uint16_t response = {0};

	// Create a temporary array for calculated sun data before transferring into the structure sunData
	uint16_t convertedData[NUM_SUN_SENSORS] = {0};

	// Send 6 commands to get each of the sun sensor's data
	for (int i = 0; i < NUM_SUN_SENSORS; i = i + 1) {

		// Load command into output buffer
		memcpy(command, &pdbSunSensorCommandBytes[i], PDB_TELEM_COMMAND_LENGTH);

		int error = pdbTalk(command, (uint8_t*)&response);

		if (error != SUCCESS && error > PDB_ERROR) {
			warningPrint("I2c Error = %d\n", error);
			continue;
			//return error;
		}

		bigEndianUint16(&response);
		convertedData[i] = response;
	}

	// Now store all of the calculated data into the proper slot in the sunData structure
	sunData->BCR1Voltage = convertedData[0];
	sunData->SA1ACurrent = convertedData[1];
	sunData->SA1BCurrent = convertedData[2];
	sunData->BCR2Voltage = convertedData[3];
	sunData->SA2ACurrent = convertedData[4];
	sunData->SA2BCurrent = convertedData[5];
	sunData->BCR3Voltage = convertedData[6];
	sunData->SA3ACurrent = convertedData[7];
	sunData->SA3BCurrent = convertedData[8];

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
int pdbTelemetry(eps_telemetry* dataStorage) {

	// Create an empty sunSensorData structure
	sun_sensor_data sunSensorData;

	(void) sunSensorData;
	// Get sun sensor data and store it in the new pdb_status_t Structure
	pdbSunSensorData(&dataStorage->sunSensorData);

	// Create temporary variables for sending and receiving I2C data
	uint8_t command[PDB_TELEM_COMMAND_LENGTH] = {0};
	uint16_t response;

	// Create a temporary array for calculated sun data before transferring into the structure sunData
	float storedData[NUM_TELEM_CALLS] = {0};

	// Send 4 commands to get ADC output voltage readings from the PDB
	for (int i = 0; i < NUM_TELEM_CALLS; i = i + 1) {

		memcpy(command, &pdbVoltageCommandBytes[i], PDB_TELEM_COMMAND_LENGTH);

		int error = pdbTalk(command, (uint8_t*)&response);

		if (error != SUCCESS && error > PDB_ERROR) {
			warningPrint("I2c Error = %d\n", error);
			continue;
		}

		bigEndianUint16(&response);
		storedData[i] = response;

	}

	dataStorage->outputVoltageBCR = storedData[0];
	dataStorage->outputVoltageBatteryBus = storedData[1];
	dataStorage->outputVoltage5VBus = storedData[2];
	dataStorage->outputVoltage3V3Bus = storedData[3];

	// Send 4 commands to get ADC output current readings from the PDB
	for (int i = 0; i < NUM_TELEM_CALLS; i = i + 1) {

		memcpy(command, &pdbCurrentCommandBytes[i], PDB_TELEM_COMMAND_LENGTH);

		int error = pdbTalk(command, (uint8_t*)&response);

		if (error != SUCCESS && error > PDB_ERROR) {
			warningPrint("I2c Error = %d\n", error);
			continue;
		}

		bigEndianUint16(&response);
		storedData[i] = response;
	}


	// Get ADC Output current readings from the PDB
	dataStorage->outputCurrentBCR_mA = storedData[0];
	dataStorage->outputCurrentBatteryBus = storedData[1];
	dataStorage->outputCurrent5VBus = storedData[2];
	dataStorage->outputCurrent3V3Bus = storedData[3];


	// Get ADC temperature reading from the PDB
	memcpy(command, &pdbTemperatureCommandBytes, PDB_TELEM_COMMAND_LENGTH);

	int error = pdbTalk(command, (uint8_t*)&response);

	if (error != SUCCESS && error > PDB_ERROR) {
		warningPrint("I2c Error = %d\n", error);
		return SUCCESS;
	}


	bigEndianUint16((uint16_t*)&response);
	dataStorage->PdbTemperature = response;

	return SUCCESS;
}


/**
 * Pet the Communications watchdog on the PDB using code 0x22
 * @return either an error if it occurred, or 0
 */
int pdbPetWatchdog(void) {
	// Create temporary variables for sending I2C data
	uint8_t command[PDB_COMMAND_LENGTH] = {0};

	memset(command, 0, sizeof(command));
	memcpy(command, &pdbWatchdogResetCommand, PDB_COMMAND_LENGTH);

	// One way communication so just use transmit using reset watchdog command 0x22
	int error = i2cTransmit(PDB_I2C_SLAVE_ADDR, command, PDB_COMMAND_LENGTH);

	if (error != SUCCESS) {
		return error;
	}

	return SUCCESS;
}


void pdbReset(void){
	uint8_t response[2];
	pdbTalk((uint8_t*)&resetCommand, response);
}

void pdbResetSat(void){
	uint8_t response[2];
	pdbTalk((uint8_t*)&resetPDMCommand, response);
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
	if ((command[0]) == TELEM_CALL_COMPARISON) {
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
	if(response[0] == 0xff){
		uint8_t pdbError[2];
		pdbTalk(getLastError, pdbError);
		//bigEndianUint16(&Error);
		errorPrint(" Error return = %u - %s", pdbError[1], pdbGetErrorMessage(pdbError[1]));
		return PDB_ERROR - pdbError[1];
	}

	return SUCCESS;
}

