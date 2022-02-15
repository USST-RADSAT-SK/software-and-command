/**
 * @file RAntenna.c
 * @date Jan 15, 2022
 * @author Addi Amaya (caa746)
 */

#include <RAntenna.h>
#include <satellite-subsystems/IsisAntS.h>
#include <string.h>

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
		RAntennaStatus.AntennaArmedASide = RISISantsStatus.fields.armed;


		//if one antenna is not deployed, repeat attempt
		if (!RAntennaStatus.DeployedAntennaOne || !RAntennaStatus.DeployedAntennaTwo || !RAntennaStatus.DeployedAntennaThree || !RAntennaStatus.DeployedAntennaFour) {

			//Arm A Side Antenna system
			IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISASide,RArmed);

			//updated armed status
			IsisAntS_getStatusData(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,&RISISantsStatus);
			RAntennaStatus.AntennaArmedASide = RISISantsStatus.fields.armed;

			//must be armed before deployment can occur
			if (RAntennaStatus.AntennaArmedASide) {

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
		RAntennaStatus.AntennaArmedBSide = RISISantsStatus.fields.armed;


		//if one antenna is not deployed, repeat attempt
		if (!RAntennaStatus.DeployedAntennaOne || !RAntennaStatus.DeployedAntennaTwo || !RAntennaStatus.DeployedAntennaThree || !RAntennaStatus.DeployedAntennaFour) {

			//Arm B Side Antenna system
			IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISBSide,RArmed);

			//updated armed status
			IsisAntS_getStatusData(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,&RISISantsStatus);
			RAntennaStatus.AntennaArmedBSide = RISISantsStatus.fields.armed;

			//must be armed before deployment can occur
			if (RAntennaStatus.AntennaArmedBSide) {

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

}
