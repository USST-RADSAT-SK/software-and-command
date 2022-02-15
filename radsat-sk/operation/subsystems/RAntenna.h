/**
 * @file RAntenna.h
 * @date December 12, 2021
 * @author Tyrel Kostyk (tck290)
 */

#ifndef RANTENNA_H_
#define RANTENNA_H_

#include <stdint.h>

/***************************************************************************************************
                                            DEFINITIONS
***************************************************************************************************/

/** I2C slave address for antenna receive port, primary */
#define ANTENNA_I2C_SLAVE_ADDR_PRIMARY (0x31)

/** I2C slave address for antenna receive port, primary */
#define ANTENNA_I2C_SLAVE_ADDR_REDUNANT (0x32)

/** Antenna Deployment status */
typedef struct _antennaDeploymentStatus {
	int DeployedAntennaOne;
	int DeployedAntennaTwo;
	int DeployedAntennaThree;
	int DeployedAntennaFour;
	int SideAArmed;
	int SideBArmed;
} antennaDeploymentStatus;


/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int antennaInit(void);
int antennaDeployment(void);
int antennaGetTemperature(uint16_t side);


#endif /* RANTENNA_H_ */
