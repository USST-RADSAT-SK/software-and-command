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

/** I2C Slave Address for Antenna Receive Port */
#define ANTENNA_I2C_SLAVE_ADDR_PRIMARY (0x31)

typedef struct _antennaDeploymentStatus {
	int DeployedsideA;
	int DeployedsideB;
} antennaDeploymentStatus;

typedef struct _antennaTemperature {
	int TemperatureSideA;
	int TemperatureSideB;
} antennaTemperature;

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

int antennaInit(void);
int antennaDeployment(void);
antennaTemperature antennaGetTemperature(void);


#endif /* RANTENNA_H_ */
