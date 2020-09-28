/**
 *	@file IsisAntS.h
 * 	@brief ISIS Deployable Antenna System
 */
#ifndef ISISANTS_H_
#define ISISANTS_H_

#define E_ANTS_SIDE_ERROR     -21 ///< Incorrect side of antenna system specified

/**
 *	Union consisting of bitflags used in the ISIS Antenna Systems Status.
 */
typedef union __attribute__ ((__packed__)) _ISISantsStatus
{
	unsigned char raw[2]; ///< Unformatted ISIS AntS status data
	struct __attribute__ ((__packed__))
	{
		unsigned short armed : 1,
		ant4Deploying : 1,
		ant4Timeout : 1,
		ant4Undeployed : 1,
		: 1,
		ant3Deploying : 1,
		ant3Timeout : 1,
		ant3Undeployed : 1,
		ignoreFlag : 1,
		ant2Deploying : 1,
		ant2Timeout : 1,
		ant2Undeployed : 1,
		: 1,
		ant1Deploying : 1,
		ant1Timeout : 1,
		ant1Undeployed : 1;
	} fields; ///< Struct with individual fields of ISIS AntS status data
} ISISantsStatus;

/**
 *	Union consisting of complete telemetry in the ISIS Antenna Systems Status.
 */
typedef union __attribute__ ((__packed__)) _ISISantsTelemetry
{
	unsigned char raw[8]; ///< Unformatted ISIS AntS telemetry
	struct __attribute__ ((__packed__))
	{
		unsigned short ants_temperature;
		ISISantsStatus ants_deployment ;
		unsigned int ants_uptime;
	} fields; ///< Struct with individual fields of ISIS AntS telemetry
} ISISantsTelemetry;

/**
 *	Struct for defining ISIS Antenna Systems Status I2C Address.
 */
typedef struct _ISISantsI2Caddress
{
	unsigned char addressSideA; ///< I2C address of the AntS side A microcontroller
	unsigned char addressSideB; ///< I2C address of the AntS side B microcontroller
} ISISantsI2Caddress;

/**
 *	Enumeration list of ISIS Antenna Systems Arming and Disarming Command.
 */
typedef enum _ISISantsArmStatus
{
	isisants_disarm = 0xAC,
	isisants_arm = 0xAD
} ISISantsArmStatus;

/**
 *	Enumeration list to select the antenna from ISIS Antenna Systems.
 */
typedef enum _ISISantsAntenna
{
	isisants_antenna1 = 0x00,
	isisants_antenna2 = 0x01,
	isisants_antenna3 = 0x02,
	isisants_antenna4 = 0x03
} ISISantsAntenna;

/**
 *	Enumeration list to select the antenna side from ISIS Antenna Systems.
 */
typedef enum _ISISantsSide
{
	isisants_sideA = 0x00,
	isisants_sideB = 0x01
} ISISantsSide;

/**
 *  Enumeration list to select the deployment override option from ISIS Antenna Systems.
 */
typedef enum _ISISantsDeploymentType
{
	isisants_overrideDeployment,
	isisants_normalDeployment
} ISISantsDeploymentType;

/*!
 * 	Initialize the ISIS Antenna Systems with the corresponding i2cAddress from the array of Antenna Systems I2C Address structure.
 *  @note       This function can only be called once.
 * 	@param[in]  address array of Antenna Systems I2C Address structure.
 * 	@param[in]  number number of attached Antenna Systems in the system to be initialized.
 * 	@return     The error status of the initialization, defined in <hal/errors.h>.
 */
int IsisAntS_initialize(ISISantsI2Caddress* address, unsigned char number);

/*!
 * 	Set the ISIS Antenna Systems arm status.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 *	@param[in]  armStatus arm/disarm command.
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_setArmStatus(unsigned char index, ISISantsSide side, ISISantsArmStatus armStatus);

/*!
 * 	Attempt to Deploy the ISIS Antenna Systems.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 *	@param[in]  antennaSelection Selected Antenna to be deployed.
 *	@param[in]  override override deployment option for the activation.
 *	@param[in]  deploymentTime maximum deployment timeout of the activation in seconds
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_attemptDeployment(unsigned char index, ISISantsSide side, ISISantsAntenna antennaSelection, ISISantsDeploymentType override, unsigned char deploymentTime);

/*!
 * 	Obtain ISIS Antenna Systems status byte.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 *	@param[out] status current status byte of the Antenna Systems.
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_getStatusData(unsigned char index, ISISantsSide side, ISISantsStatus* status);

/*!
 * 	Obtain ISIS Antenna Systems raw temperature value.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 *	@param[out] temperature ISIS Antenna Systems raw temperature value.
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_getTemperature(unsigned char index, ISISantsSide side, unsigned short* temperature);

/*!
 * 	Obtain ISIS Antenna Systems uptime value.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 *	@param[out] uptime ISIS Antenna Systems uptime value in seconds.
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_getUptime(unsigned char index, ISISantsSide side, unsigned int* uptime);

/*!
 * 	Obtain ISIS Antenna Systems all telemetry structure.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 *	@param[out] alltelemetry ISIS Antenna Systems telemetry struct.
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_getAlltelemetry(unsigned char index, ISISantsSide side, ISISantsTelemetry* alltelemetry);

/*!
 * 	Obtain the ISIS Antenna Systems Antenna Activation Count.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 *	@param[in]  antennaSelection obtain activation count from this selected antenna.
 *	@param[out] deploymentCount activation count of the selected antenna.
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_getActivationCount(unsigned char index, ISISantsSide side, ISISantsAntenna antennaSelection, unsigned char* deploymentCount);

/*!
 * 	Obtain the ISIS Antenna Systems Antenna Activation Time.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 *	@param[in]  antennaSelection obtain activation count from this selected antenna.
 *	@param[out] deploymentTime activation time of the selected antenna in 0.05 second increments
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_getActivationTime(unsigned char index, ISISantsSide side, ISISantsAntenna antennaSelection, unsigned short* deploymentTime);

/*!
 * 	Reset single side of the ISIS Antenna Systems.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_reset(unsigned char index, ISISantsSide side);

/*!
 * 	Cancel the current deployment of the ISIS Antenna Systems.
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address.
 *	@param[in]  side Antenna Systems side.
 * 	@return 	Error code according to <hal/errors.h>
 */
int IsisAntS_cancelDeployment(unsigned char index, ISISantsSide side);

/*!
 * 	Execute the automatic deployment sequence of the ISIS Antenna Systems
 *
 * 	@param[in]  index index of ISIS Antenna Systems I2C bus address
 *	@param[in]  side Antenna Systems side
 *	@param[in]  deploymentTime maximum deployment timeout of the activation in seconds
 * 	@return     Error code according to <hal/errors.h>
 */
int IsisAntS_autoDeployment(unsigned char index, ISISantsSide side, unsigned char deploymentTime);

#endif /* ISISANTS_H_ */
