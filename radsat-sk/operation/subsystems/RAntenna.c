/**
 * @file RAntenna.c
 * @date December 28, 2021
 * @author Addi Amaya (caa746)
 */

#include <RAntenna.h>
#include <satellite-subsystems/IsisAntS.h>
#include <string.h>
#include <hal/errors.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/
#define ANTENNAS_DEPLOYED (0)

/***************************************************************************************************
                                         PRIVATE VARIABLES
***************************************************************************************************/
ISISantsI2Caddress RAntennaI2Caddress = {ANTENNA_I2C_SLAVE_ADDR_PRIMARY,ANTENNA_I2C_SLAVE_ADDR_REDUNDANT};
antennaDeploymentStatus RantennaDeploymentStatus;
static int AntennaInitialized = 0;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

/**
 * Initializes the ISISpace Antenna driver
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaInit(void) {

	if (AntennaInitialized)
		return E_IS_INITIALIZED;

	int error = IsisAntS_initialize(&RAntennaI2Caddress, 4);

	if (error == 0)
		AntennaInitialized = 1;

	return error;
}

/**
 * Activate the auto-deployment mechanism on ISISpace Antenna System
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaDeployment(void) {

	//deploying A side
	if (RantennaDeploymentStatus.DeployedsideA == 0) {
		int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,60);
		if(error != 0)
			return error;
		RantennaDeploymentStatus.DeployedsideA = 1;
	}

	//deploying B side
	if (RantennaDeploymentStatus.DeployedsideB == 0) {
		int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_REDUNDANT,isisants_sideB,60);
		if(error != 0)
			return error;
		RantennaDeploymentStatus.DeployedsideB = 1;
	}

	return ANTENNAS_DEPLOYED;
}

int antennaGetTemperature(void) {

}
