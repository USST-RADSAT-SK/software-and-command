/**
 * @file RAntenna.c
 * @date Jan 15, 2022
 * @author Addi Amaya (caa746) and Atharva Kulkanri (iya789)
 */

#include <RAntenna.h>
#include <satellite-subsystems/IsisAntS.h>
#include <string.h>
#include <hal/errors.h>
#include <hal/Timing/Time.h>
#include <RCommon.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define DISABLE_ANT

/***************************************************************************************************
                                  PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

/** Struct for defining Antenna Systems I2C Addresses. Configured as a single bus, both address write to same location */
ISISantsI2Caddress RAntennaI2Caddress = {ANTENNA_I2C_SLAVE_ADDR_PRIMARY,ANTENNA_I2C_SLAVE_ADDR_REDUNANT};

/** Struct that holds the current deployment status of the antenna*/
antenna_deployment_status RAntennaStatus = { 0 };

/** Track whether the antenna has been initialized yet */
static int antennaInitialized = 0;

/** Track Number of times the antenna attempted to deploy*/
static int antennaDeploymentAttempts = 0;


/** Index of the Antenna*/
#define ANTENNA_INDEX 0


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initializes the ISISpace Antenna driver
 *
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaInit(void) {

	// Only allow initialization once (return without error if already initialized)
	if (antennaInitialized)
		return SUCCESS;

	// Isis Function call for antenna initialization
	int error = IsisAntS_initialize(&RAntennaI2Caddress, ANTENNAS_ON_BOARD);

	// Update flag if successfully initialized
	if (error == 0)
		antennaInitialized = 1;

	// TODO: record errors (if present) to System Manager
	return error;
}


/**
 * Activate the auto-deployment mechanism on ISISpace Antenna System which will attempt to sequentially
 * deploy all antennas present on the system without intervention. Has a built in timer per antenna deployment
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
void antennaDeploymentAttempt(void* parameters) {
	// ignore the input parameter
	(void)parameters;

	// For Error detection
	int error = 0;
	int antennaSide = isisants_sideA;
	// Initialize the union for antenna status
	ISISantsStatus RISISantsStatus = { .fields = { 0 } };

	while (1) {
		infoPrint("Starting Antenna Task");

		// Get status of the A side antenna
		error = IsisAntS_getStatusData(ANTENNA_INDEX, isisants_sideA, &RISISantsStatus);

		warningPrint("Antenna Status = 0x%x", *((uint16_t*)RISISantsStatus.raw));
		// if Antenna is diployed then don't attempt.
		if ((*((uint16_t*)RISISantsStatus.raw) & 0x8888) == 0) {
			// Disarm Antenna system
			error = IsisAntS_setArmStatus(ANTENNA_INDEX, antennaSide, isisants_disarm);
			if (error) errorPrint("Set Arm Error = %d", error);
			else infoPrint("Disarming side side %d", antennaSide);
			vTaskDelete(NULL);
		}


		if (antennaDeploymentAttempts > MAX_DEPLOYMENT_ATTEMPTS){
			antennaSide = isisants_sideB;
		} else if (antennaDeploymentAttempts > MAX_DEPLOYMENT_ATTEMPTS * 2){
			antennaDeploymentAttempts = 0;
			antennaSide = isisants_sideA;
		}

		// Arm Antenna system
#ifndef DISABLE_ANT
		error = IsisAntS_setArmStatus(ANTENNA_INDEX, antennaSide, isisants_arm);
#endif
		if (error) errorPrint("Set Arm Error = %d", error);

		// Request Antenna Status
		error = IsisAntS_getStatusData(ANTENNA_INDEX, antennaSide, &RISISantsStatus);
		if (error) errorPrint("Get Status Error = %d", error);

		// Must be armed before deployment can occur
		if (RISISantsStatus.fields.armed) {
			infoPrint("Attempting deployment on side %d", antennaSide);
			// Start automatic deployment
#ifndef DISABLE_ANT
			IsisAntS_autoDeployment(ANTENNA_INDEX, antennaSide, MAX_DEPLOYMENT_TIMEOUT);
#endif

		}

		// Increment deployment attempts
		antennaDeploymentAttempts += 1;

		// Wait between deployment attempts to ensure disarming doesn't happen too quickly
		vTaskDelay(INTER_DEPLOYMENT_DELAY_MS);

		// Disarm Antenna system
		error = IsisAntS_setArmStatus(ANTENNA_INDEX, antennaSide, isisants_disarm);
		if (error) errorPrint("Set Arm Error = %d", error);
		else infoPrint("Disarming side side %d", antennaSide);

	}
}

/**
 * Collect all telemetry from the ISISpace Antenna which includes temperatures, uptime, and antenna status
 *
 * @param side 0 for top side of antenna temperature, 1 for bottom side of the antenna temperature, will default to bottom side
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaTelemetry(antenna_telemetry* telemetry) {

	int error;

	// ensure the pointer is valid
	if (telemetry == 0)
		return E_INPUT_POINTER_NULL;

	// create ISIS Telemetry struct
	ISISantsTelemetry RISISantsTelemetry = { 0 };

	// Execute Telemetry command for Side A
	error = IsisAntS_getAlltelemetry(ANTENNA_INDEX, isisants_sideA, &RISISantsTelemetry);

	// Error check for Isis Antenna function
	if(error != 0) {

		// TODO: record errors (if present) to System Manager
		return error;
	}

	// Assign side A Telemetry to struct
	memcpy(&telemetry->sideA.deployStatus, &RISISantsTelemetry.fields.ants_deployment.raw, sizeof(RISISantsTelemetry.fields.ants_deployment.raw));
	telemetry->sideA.boardTemp = 0.00322581 * RISISantsTelemetry.fields.ants_temperature;
	telemetry->sideA.uptime = RISISantsTelemetry.fields.ants_uptime;

	// Execute command for Side B
	error = IsisAntS_getAlltelemetry(ANTENNA_INDEX, isisants_sideB, &RISISantsTelemetry);

	// Error check for Isis Antenna function
	if(error != 0) {

		// TODO: record errors (if present) to System Manager
		return error;
	}

	// Assign side B Telemetry to struct
	memcpy(&telemetry->sideB.deployStatus, &RISISantsTelemetry.fields.ants_deployment.raw, sizeof(RISISantsTelemetry.fields.ants_deployment.raw));
	telemetry->sideB.boardTemp = 0.00322581 * RISISantsTelemetry.fields.ants_temperature;
	telemetry->sideB.uptime = RISISantsTelemetry.fields.ants_uptime;

	return SUCCESS;
}

/**
 * Resets microcontroller of the antenna
 * Cancels the current deployement of the Antenna systems
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaReset(void) {

	// Reset side A antenna. See section 6.2 of Antenna System User Manual
	int error = IsisAntS_reset(ANTENNA_INDEX, isisants_sideA);

	if (error != SUCCESS) {
		// TODO: record errors (if present) to System Manager
		return error;
	}

	// Reset side B antenna. See section 6.2 of Antenna System User Manual
	error = IsisAntS_reset(ANTENNA_INDEX, isisants_sideB);

	if (error != SUCCESS) {
		// TODO: record errors (if present) to System Manager
	}

	return error;
}

/**
 * Gives temperature of both sides of the antenna
 *
 * @param A side temperature
 * @param B side temperature
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaTemperature(float* temperatureOne, float* temperatureTwo) {

	unsigned short temperature = 0;

	// get temperature from side A of antenna
	int error = IsisAntS_getTemperature(ANTENNA_INDEX, isisants_sideA, &temperature);

	if (error != SUCCESS) {
		// TODO: record errors (if present) to System Manager
		return error;
	}

	*temperatureOne = ((float)temperature * -0.2922) + 190.65;

	// get temperature from side B of antenna
	error = IsisAntS_getTemperature(ANTENNA_INDEX, isisants_sideA, &temperature);
	*temperatureTwo = ((float)temperature * -0.2922) + 190.65;

	if (error != SUCCESS) {
		// TODO: record errors (if present) to System Manager
	}

	return error;
}

