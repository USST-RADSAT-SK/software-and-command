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

/** number of attached Antennas in the system */
#define ANTENNAS_ON_BOARD 4

/** Struct for defining Antenna Systems I2C Addresses. Configured as a single bus, both address write to same location */
ISISantsI2Caddress RAntennaI2Caddress = {ANTENNA_I2C_SLAVE_ADDR_PRIMARY,ANTENNA_I2C_SLAVE_ADDR_REDUNANT};

/** Struct that holds the current deployment status of the antenna*/
antennaDeploymentStatus RantennaDeploymentStatus;

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

/** Track whether Antenna One has be been deployed */
static int antennaOneDeployed = 0;

/** Track whether Antenna Two has be been deployed */
static int antennaTwoDeployed = 0;

/** Track whether Antenna Three has be been deployed */
static int antennaThreeDeployed = 0;

/** Track whether Antenna Four has be been deployed */
static int antennaFourDeployed = 0;

/** Track whether Antenna A Side is Armed*/
static int antennaASideArmed = 0;

/** Track whether Antenna B Side is Armed*/
static int antennaBSideArmed = 0;

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

	//Get Current Status of the Antenna

	//Arm A Side Antenna system
	IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISASide,RArmed);

	//Check if A side was armed, if not, try again

	//Must be armed, and at least one of the A side antennas are not deployed

	//deploy A side Attempt
	if (RantennaDeploymentStatus.DeployedsideA == 0) {
		int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,60);
		if(error != 0)
			return error;
		RantennaDeploymentStatus.DeployedsideA = 1;
	}

	//Check to see if Antennas were deployed, if not attempts again

	//Disarm A Side Antenna System
	IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISASide,RDisarmed);

	//Check if A Side if Disarmed, if not, attempt again

	//Arm B Side Antenna System
	IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISBSide,RArmed);

	//Check if B Side is armed, if not, attempt again

	//deploying B side
	if (RantennaDeploymentStatus.DeployedsideB == 0) {
		int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,60);
		if(error != 0)
			return error;
		RantennaDeploymentStatus.DeployedsideB = 1;
	}

	//Check to see if Atnennas were deployed, if not, attempt again

	//Disarm B Side Antenna System
	IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISBSide,RDisarmed)

	//Check to see if B side is disarmed, if not, attempt again


	return 0;
}

/**
 * Collect all telemetry from the ISISpace Antenna which includes temperatures, uptime, and antenna status
 *
 * @param side 0 for top side of antenna temperature, 1 for bottom side of the antenna temperature, will default to bottom side
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int atennaGetAllTelemetry()
