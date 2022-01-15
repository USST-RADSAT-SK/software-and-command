/**
 * @file RAntenna.c
 * @date Jan 15, 2022
 * @author Addi Amaya (caa746)
 */

#include <RAntenna.h>
#include <satellite-subsystems/IsisAntS.h>
#include <string.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/



/***************************************************************************************************
                                         PRIVATE VARIABLES
***************************************************************************************************/
ISISantsI2Caddress RAntennaI2Caddress = {ANTENNA_I2C_SLAVE_ADDR_PRIMARY,ANTENNA_I2C_SLAVE_ADDR_PRIMARY};
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
		return 0;

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
		int error = IsisAntS_autoDeployment(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,60);
		if(error != 0)
			return error;
		RantennaDeploymentStatus.DeployedsideB = 1;
	}

	return 0;
}

/**
 * Collect temperature data from the ISISpace Antenna
 *
 * @param side 0 for top side of antenna temperature, 1 for bottom side of the antenna temperature, will default to bottom side
 * @return 0 for success, non-zero for failure. See hal/errors.h for details.
 */
int antennaGetTemperature(uint16_t side) {
	unsigned short RAntennaTemperature = 0;
	int error = 0;
	switch(side) {
		case 0:
			error = IsisAntS_getTemperature(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideA,&RAntennaTemperature);
			if (error != 0) { //error occured
				return error;
			}
			break;
		case 1:
			error = IsisAntS_getTemperature(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,&RAntennaTemperature);
			if (error != 0) { //error occured
				return error;
			}
			break;
		default:
			error = IsisAntS_getTemperature(ANTENNA_I2C_SLAVE_ADDR_PRIMARY,isisants_sideB,&RAntennaTemperature);
			if (error != 0) { //error occured
				return error;
			}
	}

	//TODO: Conversion of raw temperature data to valid temperature data
	//Problem: Don't know the type of temperature sensor and are unsure how to convert raw temperature data


}
