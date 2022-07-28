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
#include <RErrorManager.h>


/***************************************************************************************************
                                  PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

/** Struct for defining Antenna Systems I2C Addresses. Configured as a single bus, both address write to same location */
ISISantsI2Caddress RAntennaI2Caddress = {ANTENNA_I2C_SLAVE_ADDR_PRIMARY,ANTENNA_I2C_SLAVE_ADDR_REDUNANT};

/** Struct that holds the current deployment status of the antenna*/
antenna_deployment_status_t RAntennaStatus = { 0 };

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

	if (error != SUCCESS)
			errorReportComponent(moduleFram , error)

	return error;
}

/**
 * Activate the auto-deployment mechanism on ISISpace Antenna System which will attempt to sequentially
 * deploy all antennas present on the system without intervention. Has a built in timer per antenna deployment
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaDeploymentAttempt(void) {

	// For Error detection
	int error = 0;

	// Initialize the union for antenna status
	ISISantsStatus RISISantsStatus = { .fields = { 0 } };

	// A Side deployment Attempt
	while ( antennaDeploymentAttempts < MAX_DEPLOYMENT_ATTEMPTS ) {

		// Get status of the A side antenna
		error = IsisAntS_getStatusData(ANTENNA_INDEX, isisants_sideA, &RISISantsStatus);

		// Error check for requesting antenna status
		if(error != 0) {

			errorReportComponent(componentSsiAntenna,error);
			return error;
		}

		// Update the antenna status struct
		RAntennaStatus.DeployedAntennaOne = !RISISantsStatus.fields.ant1Undeployed;
		RAntennaStatus.DeployedAntennaTwo = !RISISantsStatus.fields.ant2Undeployed;
		RAntennaStatus.DeployedAntennaThree = !RISISantsStatus.fields.ant3Undeployed;
		RAntennaStatus.DeployedAntennaFour = !RISISantsStatus.fields.ant4Undeployed;
		RAntennaStatus.AntennaArmed = RISISantsStatus.fields.armed;


		// If one antenna is not deployed repeat attempt, otherwise do nothing
		if (!RAntennaStatus.DeployedAntennaOne || !RAntennaStatus.DeployedAntennaTwo || !RAntennaStatus.DeployedAntennaThree || !RAntennaStatus.DeployedAntennaFour) {

			// Arm A Side Antenna system
			error = IsisAntS_setArmStatus(ANTENNA_INDEX, isisants_sideA, isisants_arm);

			// Error check for Arming A Side
			if(error != 0) {

				
				errorReportComponent(componentSsiAntenna,error);

				return error;
			}

			// Request Antenna Status
			error = IsisAntS_getStatusData(ANTENNA_INDEX, isisants_sideA, &RISISantsStatus);

			// Error check for antenna status
			if(error != 0) {

				errorReportComponent(componentSsiAntenna,error);

				return error;
			}

			// Update Armed Section of struct
			RAntennaStatus.AntennaArmed = RISISantsStatus.fields.armed;

			// Must be armed before deployment can occur
			if (RAntennaStatus.AntennaArmed) {

				// Start automatic deployment
				int error = IsisAntS_autoDeployment(ANTENNA_INDEX, isisants_sideA, MAX_DEPLOYMENT_TIMEOUT);

				// Check if autoDeployment failed
				if(error != 0) {

					errorReportComponent(componentSsiAntenna,error);

					return error;
				}

				// Disarm A Side Antenna system
				error = IsisAntS_setArmStatus(ANTENNA_INDEX, isisants_sideA, isisants_disarm);

				// Check if disarm failed
				if(error != 0) {

					errorReportComponent(componentSsiAntenna,error);

					return error;
				}
			}
		}

		// Increment deployment attempts for side A
		antennaDeploymentAttempts += 1;
	}

	// reset_t attempt counter for Side B
	antennaDeploymentAttempts = 0;

	// B Side deployment Attempt
	while ( antennaDeploymentAttempts < MAX_DEPLOYMENT_ATTEMPTS ) {

		// Get status of the antenna
		error = IsisAntS_getStatusData(ANTENNA_INDEX, isisants_sideB, &RISISantsStatus);

		// Error check for requesting antenna status
		if(error != 0) {

			errorReportComponent(componentSsiAntenna,error);

			return error;
		}

		// Update the antenna status struct, replaces data from A side
		RAntennaStatus.DeployedAntennaOne = !RISISantsStatus.fields.ant1Undeployed;
		RAntennaStatus.DeployedAntennaTwo = !RISISantsStatus.fields.ant2Undeployed;
		RAntennaStatus.DeployedAntennaThree = !RISISantsStatus.fields.ant3Undeployed;
		RAntennaStatus.DeployedAntennaFour = !RISISantsStatus.fields.ant4Undeployed;
		RAntennaStatus.AntennaArmed = RISISantsStatus.fields.armed;


		// If one antenna is not deployed, repeat attempt
		if (!RAntennaStatus.DeployedAntennaOne || !RAntennaStatus.DeployedAntennaTwo || !RAntennaStatus.DeployedAntennaThree || !RAntennaStatus.DeployedAntennaFour) {

			// Arm B Side Antenna system
			error = IsisAntS_setArmStatus(ANTENNA_INDEX, isisants_sideB, isisants_arm);

			// Error check for requesting antenna status
			if(error != 0) {

				errorReportComponent(componentSsiAntenna,error);

				return error;
			}

			// Request Antenna Status
			error = IsisAntS_getStatusData(ANTENNA_INDEX, isisants_sideB, &RISISantsStatus);

			// Error check for antenna status
			if(error != 0) {

				errorReportComponent(componentSsiAntenna,error);

				return error;
			}

			// Update Armed Section of struct
			RAntennaStatus.AntennaArmed = RISISantsStatus.fields.armed;

			// Must be armed before deployment can occur
			if (RAntennaStatus.AntennaArmed) {

				// Start automatic deployment
				int error = IsisAntS_autoDeployment(ANTENNA_INDEX, isisants_sideB, MAX_DEPLOYMENT_TIMEOUT);

				// Check if autoDeployment failed
				if(error != 0) {

					
					errorReportComponent(componentSsiAntenna,error);

					return error;
				}

				//disarm B Side Antenna system
				error = IsisAntS_setArmStatus(ANTENNA_INDEX, isisants_sideB, isisants_disarm);

				// Check if disarm failed
				if(error != 0) {

					errorReportComponent(componentSsiAntenna,error);

					return error;
				}
			}

		}
		// Increment deployment attempts for side B
		antennaDeploymentAttempts += 1;
	}


	return SUCCESS;
}

/**
 * Collect all telemetry from the ISISpace Antenna which includes temperatures, uptime, and antenna status
 *
 * @param side 0 for top side of antenna temperature, 1 for bottom side of the antenna temperature, will default to bottom side
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaTelemetry(antenna_telemetry_t* telemetry) {

	int error;

	// ensure the pointer is valid
	if (telemetry == 0)
		return E_INPUT_POINTER_NULL;

	// create ISIS Telemetry struct
	ISISantsTelemetry RISISantsTelemetry = { .fields = { 0 } };

	// Execute Telemetry command for Side A
	error = IsisAntS_getAlltelemetry(ANTENNA_INDEX, isisants_sideA, &RISISantsTelemetry);

	// Error check for Isis Antenna function
	if(error != 0) {

		errorReportComponent(componentSsiAntenna,error);

		return error;
	}

	// Assign side A Telemetry to struct
	telemetry->sideA.deployStatus.DeployedAntennaOne = !RISISantsTelemetry.fields.ants_deployment.fields.ant1Undeployed;
	telemetry->sideA.deployStatus.DeployedAntennaTwo = !RISISantsTelemetry.fields.ants_deployment.fields.ant2Undeployed;
	telemetry->sideA.deployStatus.DeployedAntennaThree = !RISISantsTelemetry.fields.ants_deployment.fields.ant3Undeployed;
	telemetry->sideA.deployStatus.DeployedAntennaFour = !RISISantsTelemetry.fields.ants_deployment.fields.ant4Undeployed;
	telemetry->sideA.deployStatus.AntennaArmed = !RISISantsTelemetry.fields.ants_deployment.fields.armed;
	telemetry->sideA.board_temp = 0.00322581 * RISISantsTelemetry.fields.ants_temperature;
	telemetry->sideA.uptime = RISISantsTelemetry.fields.ants_uptime;

	// Execute command for Side B
	error = IsisAntS_getAlltelemetry(ANTENNA_INDEX, isisants_sideB, &RISISantsTelemetry);

	// Error check for Isis Antenna function
	if(error != 0) {

		errorReportComponent(componentSsiAntenna,error);

		return error;
	}

	// Assign side B Telemetry to struct
	telemetry->sideB.deployStatus.DeployedAntennaOne = !RISISantsTelemetry.fields.ants_deployment.fields.ant1Undeployed;
	telemetry->sideB.deployStatus.DeployedAntennaTwo = !RISISantsTelemetry.fields.ants_deployment.fields.ant2Undeployed;
	telemetry->sideB.deployStatus.DeployedAntennaThree = !RISISantsTelemetry.fields.ants_deployment.fields.ant3Undeployed;
	telemetry->sideB.deployStatus.DeployedAntennaFour = !RISISantsTelemetry.fields.ants_deployment.fields.ant4Undeployed;
	telemetry->sideB.deployStatus.AntennaArmed = !RISISantsTelemetry.fields.ants_deployment.fields.armed;
	telemetry->sideB.board_temp = 0.00322581 * RISISantsTelemetry.fields.ants_temperature;
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
		
		errorReportComponent(componentSsiAntenna,error);

		return error;
	}

	// Reset side B antenna. See section 6.2 of Antenna System User Manual
	error = IsisAntS_reset(ANTENNA_INDEX, isisants_sideB);

	if (error != SUCCESS) {		
		errorReportComponent(componentSsiAntenna,error);
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
		errorReportComponent(componentSsiAntenna,error);

		return error;
	}

	*temperatureOne = ((float)temperature * -0.2922) + 190.65;

	// get temperature from side B of antenna
	error = IsisAntS_getTemperature(ANTENNA_INDEX, isisants_sideA, &temperature);
	*temperatureTwo = ((float)temperature * -0.2922) + 190.65;

	if (error != SUCCESS) {
		errorReportComponent(componentSsiAntenna,error);
	}

	return error;
}

