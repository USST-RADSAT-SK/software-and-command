/**
 * @file RAntenna.c
 * @date Jan 15, 2022
 * @author Addi Amaya (caa746)
 */

#include <RAntenna.h>
#include <satellite-subsystems/IsisAntS.h>
#include <string.h>
#include <hal/errors.h>

/***************************************************************************************************
                                  PRIVATE DEFINITIONS AND VARIABLES
***************************************************************************************************/

/** Struct for defining Antenna Systems I2C Addresses. Configured as a single bus, both address write to same location */
ISISantsI2Caddress RAntennaI2Caddress = {ANTENNA_I2C_SLAVE_ADDR_PRIMARY,ANTENNA_I2C_SLAVE_ADDR_REDUNANT};

/** Struct that holds the current deployment status of the antenna*/
antennaDeploymentStatus RAntennaStatus;

/** Side A of the Antenna system*/
ISISantsSide RISISASide = isisants_sideA;

/** Side B of the Antenna system*/
ISISantsSide RISISBSide = isisants_sideB;

/** Armed Status for the antenna system*/
ISISantsArmStatus RArmed = isisants_arm;

/** Disarmed status for the antenna system*/
ISISantsArmStatus RDisarmed = isisants_disarm;

/** Track whether the antenna has been initialized yet */
static int antennaInitialized = 0;

/** Track Number of times the antenna attempted to deploy*/
static int antennaDeploymentAttempts = 0;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initializes the ISISpace Antenna driver
 *
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaInit(void) {

	// only allow initialization once (return without error if already initialized)
	if (antennaInitialized)
		return 0;

	int error = IsisAntS_initialize(&RAntennaI2Caddress, ANTENNAS_ON_BOARD);

	// update flag if successfully initialized
	if (!error)
		antennaInitialized = 1;

	// TODO: record errors (if present) to System Manager

	return error;
}

/**
 * Activate the auto-deployment mechanism on ISISpace Antenna System which will attempt to sequentially
 * deploy all anntennas present on the system without intervention. Has a built in timer per antenna deployment
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaDeploymentAttempt(void) {

	//Initialize the union for status
	ISISantsStatus RISISantsStatus = { .fields = { 0 } };

	//Get status of the antenna
	IsisAntS_getStatusData(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,&RISISantsStatus);

	//A Side deployment Attempt
	while ( antennaDeploymentAttempts < MAX_DEPLOYMENT_ATTEMPTS ) {

		//Get status of the antenna
		IsisAntS_getStatusData(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,&RISISantsStatus);

		//update the antenna status struct
		RAntennaStatus.DeployedAntennaOne = RISISantsStatus.fields.ant1Undeployed;
		RAntennaStatus.DeployedAntennaTwo = RISISantsStatus.fields.ant2Undeployed;
		RAntennaStatus.DeployedAntennaThree = RISISantsStatus.fields.ant3Undeployed;
		RAntennaStatus.DeployedAntennaFour = RISISantsStatus.fields.ant4Undeployed;
		RAntennaStatus.AntennaArmed = RISISantsStatus.fields.armed;


		//if one antenna is not deployed, repeat attempt
		if (!RAntennaStatus.DeployedAntennaOne || !RAntennaStatus.DeployedAntennaTwo || !RAntennaStatus.DeployedAntennaThree || !RAntennaStatus.DeployedAntennaFour) {

			//Arm A Side Antenna system
			IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISASide,RArmed);

			//updated armed status
			IsisAntS_getStatusData(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,&RISISantsStatus);
			RAntennaStatus.AntennaArmed = RISISantsStatus.fields.armed;

			//must be armed before deployment can occur
			if (RAntennaStatus.AntennaArmed) {

				//Start automatic deployment
				int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,MAX_DEPLOYMENT_TIMEOUT);
				if(error != 0) {

					// TODO: record errors (if present) to System Manager
					return error;
				}

				//disarm A Side Antenna system
				IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISASide,RDisarmed);
			}

		}

		antennaDeploymentAttempts += 1;
	}

	//reset attempt counter
	antennaDeploymentAttempts = 0;

	//B Side deployment Attempt
	while ( antennaDeploymentAttempts < MAX_DEPLOYMENT_ATTEMPTS ) {

		//Get status of the antenna
		IsisAntS_getStatusData(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,&RISISantsStatus);

		//update the antenna status struct
		RAntennaStatus.DeployedAntennaOne = RISISantsStatus.fields.ant1Undeployed;
		RAntennaStatus.DeployedAntennaTwo = RISISantsStatus.fields.ant2Undeployed;
		RAntennaStatus.DeployedAntennaThree = RISISantsStatus.fields.ant3Undeployed;
		RAntennaStatus.DeployedAntennaFour = RISISantsStatus.fields.ant4Undeployed;
		RAntennaStatus.AntennaArmed = RISISantsStatus.fields.armed;


		//if one antenna is not deployed, repeat attempt
		if (!RAntennaStatus.DeployedAntennaOne || !RAntennaStatus.DeployedAntennaTwo || !RAntennaStatus.DeployedAntennaThree || !RAntennaStatus.DeployedAntennaFour) {

			//Arm B Side Antenna system
			IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISBSide,RArmed);

			//updated armed status
			IsisAntS_getStatusData(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,&RISISantsStatus);
			RAntennaStatus.AntennaArmed = RISISantsStatus.fields.armed;

			//must be armed before deployment can occur
			if (RAntennaStatus.AntennaArmed) {

				//Start automatic deployment
				int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,MAX_DEPLOYMENT_TIMEOUT);
				if(error != 0) {

					// TODO: record errors (if present) to System Manager
					return error;
				}

				//disarm A Side Antenna system
				IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISBSide,RDisarmed);
			}

		}

		antennaDeploymentAttempts += 1;
	}


	return 0;
}

/**
 * Collect all telemetry from the ISISpace Antenna which includes temperatures, uptime, and antenna status
 *
 * @param side 0 for top side of antenna temperature, 1 for bottom side of the antenna temperature, will default to bottom side
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaTelemetry(antenna_telemetry_t* telemetry) {

	// ensure the pointer is valid
	if (telemetry == 0)
		return E_INPUT_POINTER_NULL;

	//create ISIS Telemetry struct
	ISISantsTelemetry RISISantsTelemetry = { .fields = { 0 } };

	//Execute command for Side A and Error Check
	int error = IsisAntS_getAlltelemetry(ANTENNA_I2C_SLAVE_ADDR_PRIMARY, RISISASide, &RISISantsTelemetry);
	if(error != 0) {

		// TODO: record errors (if present) to System Manager
		return error;
	}

	//Assign side A Telemetry to struct
	telemetry->sideA->deployStatus.DeployedAntennaOne = !RISISantsTelemetry.fields->ants_deployment.fields.ant1Undeployed;
	telemetry->sideA->deployStatus.DeployedAntennaTwo = !RISISantsTelemetry.fields->ants_deployment.fields.ant2Undeployed;
	telemetry->sideA->deployStatus.DeployedAntennaThree = !RISISantsTelemetry.fields->ants_deployment.fields.ant3Undeployed;
	telemetry->sideA->deployStatus.DeployedAntennaFour = !RISISantsTelemetry.fields->ants_deployment.fields.ant4Undeployed;
	telemetry->sideA->deployStatus.AntennaArmed = !RISISantsTelemetry.fields->ants_deployment.fields.armed;
	telemetry->sideA.board_temp = 0.00322581 * RISISantsTelemetry.fields.ants_temperature;
	telemetry->sideA.uptime = RISISantsTelemetry.fields.ants_uptime;

	//Execute command for Side B and Error Check
	int error = IsisAntS_getAlltelemetry(ANTENNA_I2C_SLAVE_ADDR_PRIMARY, RISISBSide, &RISISantsTelemetry);
	if(error != 0) {

		// TODO: record errors (if present) to System Manager
		return error;
	}

	//Assign side A Telemetry to struct
	telemetry->sideB->deployStatus.DeployedAntennaOne = !RISISantsTelemetry.fields->ants_deployment.fields.ant1Undeployed;
	telemetry->sideB->deployStatus.DeployedAntennaTwo = !RISISantsTelemetry.fields->ants_deployment.fields.ant2Undeployed;
	telemetry->sideB->deployStatus.DeployedAntennaThree = !RISISantsTelemetry.fields->ants_deployment.fields.ant3Undeployed;
	telemetry->sideB->deployStatus.DeployedAntennaFour = !RISISantsTelemetry.fields->ants_deployment.fields.ant4Undeployed;
	telemetry->sideB->deployStatus.AntennaArmed = !RISISantsTelemetry.fields->ants_deployment.fields.armed;
	telemetry->sideB.board_temp = 0.00322581 * RISISantsTelemetry.fields.ants_temperature;
	telemetry->sideB.uptime = RISISantsTelemetry.fields.ants_uptime;

	return 0;
}
