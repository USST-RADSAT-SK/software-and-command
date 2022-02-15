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

/** Struct for defining Antenna Systems I2C Addresses.
 *
 *	Note: Configured as a single bus which means that the primary (0x31) and redundant (0x32)
 *			write to the exact same bus. To avoid confusion, primary is used for both I2C addresses
 *			to emphasize the idea of the single bus.  */
ISISantsI2Caddress RAntennaI2Caddress = {ANTENNA_I2C_SLAVE_ADDR_PRIMARY,ANTENNA_I2C_SLAVE_ADDR_PRIMARY};

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

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initializes the ISISpace Antenna driver
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

	//Arm A Side Antenna system
	IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISASide,RArmed);

	//deploying A side
	if (RantennaDeploymentStatus.DeployedsideA == 0) {
		int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,60);
		if(error != 0)
			return error;
		RantennaDeploymentStatus.DeployedsideA = 1;
	}

	//Disarm A Side Antenna System
	IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISASide,RDisarmed);

	//Arm B Side Antenna System
	IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISBSide,RArmed);

	//deploying B side
	if (RantennaDeploymentStatus.DeployedsideB == 0) {
		int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,60);
		if(error != 0)
			return error;
		RantennaDeploymentStatus.DeployedsideB = 1;
	}

	//Disarm B Side Antenna System
	IsisAntS_setArmStatus(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,RISISBSide,RDisarmed)


	return 0;
}

/**
 * Collect all telemetry from the ISISpace Antenna which includes temperatures, uptime, and antenna status
 *
 * @param side 0 for top side of antenna temperature, 1 for bottom side of the antenna temperature, will default to bottom side
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int atennaGetAllTelemetry()
