/*
 * trxuv_operations.c
 *
 *  Created on: Jul 4, 2012
 *      Author: marcoalfer
 */

#include "IsisAntSdemo.h"
#include "common.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/LED.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <satellite-subsystems/IsisAntS.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

////General Variables
#define	AUTO_DEPLOYMENT_TIME	10
#define MANUAL_DEPLOYMENT_TIME  10

// Function calls to reset both sides of the AntS
static Boolean resetAntSTest(unsigned char index)
{
	ISISantsSide activeSide = isisants_sideA;
	unsigned char antennaSystemsIndex = index;

	printf("\r\n Resetting sideA \r\n");
	print_error(IsisAntS_reset(antennaSystemsIndex, activeSide));

	activeSide = isisants_sideB;
	vTaskDelay(5 / portTICK_RATE_MS);

	printf("\r\n Resetting sideB \r\n");
	print_error(IsisAntS_reset(antennaSystemsIndex, activeSide));

	return TRUE;
}

static void printDeploymentStatus(unsigned char antenna_id, unsigned char status)
{
	printf("Antenna %d: ", antenna_id);
	if(status == 0)
	{
		printf("deployed\n\r");
	}
	else
	{
		printf("undeployed\n\r");
	}
}

// Function calls to get the current status of both sides of the AntS
static void getStatusAntSTest(unsigned char index, ISISantsSide side)
{
	ISISantsStatus currentStatus;

	print_error(IsisAntS_getStatusData(index, side, &currentStatus));

	printf("\r\nAntS ");
	if(side == isisants_sideA)
	{
		printf("side A");
	}
	else
	{
		printf("side B");
	}

	printf(" current deployment status 0x%x 0x%x (raw value) \r\n", currentStatus.raw[0], currentStatus.raw[1]);
	printf("Arm status: %s \r\n", currentStatus.fields.armed==0?"disarmed":"armed");
	printDeploymentStatus(1, currentStatus.fields.ant1Undeployed);
	printDeploymentStatus(2, currentStatus.fields.ant2Undeployed);
	printDeploymentStatus(3, currentStatus.fields.ant3Undeployed);
	printDeploymentStatus(4, currentStatus.fields.ant4Undeployed);
	printf("Override: %s \r\n", currentStatus.fields.ignoreFlag==0?"inactive":"active");
}

// Function calls to get the current temperature on both sides of the AntS
static Boolean tempAntSTest(unsigned char index)
{
	unsigned char antennaSystemsIndex = index;
	unsigned short currTemp = 0;
	float eng_value = 0;

	print_error(IsisAntS_getTemperature(antennaSystemsIndex, isisants_sideA, &currTemp));
	eng_value = ((float)currTemp * -0.2922) + 190.65;
	printf("\r\n AntS side A temperature %f deg. C\r\n", eng_value);

	print_error(IsisAntS_getTemperature(antennaSystemsIndex, isisants_sideB, &currTemp));
	eng_value = ((float)currTemp * -0.2922) + 190.65;
	printf("\r\n AntS side B temperature %f deg. C\r\n", eng_value);

	return TRUE;
}

// Function calls to get the current uptime on both sides of the AntS
static Boolean uptimeAntSTest(unsigned char index)
{
	unsigned char antennaSystemsIndex = index;
	unsigned int uptime = 0;

	//IsisAntS_getTemperature(antennaSystemsIndex, isisants_sideA, &currTemp);
	print_error(IsisAntS_getUptime(antennaSystemsIndex, isisants_sideA, &uptime));
	printf("\r\n AntS side A uptime %d sec. \r\n", uptime);

	print_error(IsisAntS_getUptime(antennaSystemsIndex, isisants_sideB, &uptime));
	printf("\r\n AntS side B uptime %d sec. \r\n", uptime);

	return TRUE;
}

// Function calls to get a block of telemetry on both sides of the AntS
static Boolean telemAntSTest(unsigned char index, ISISantsSide side)
{
	unsigned char antennaSystemsIndex = index;
	ISISantsTelemetry allTelem;
	float eng_value = 0;

	printf("\r\nAntS ");
	if(side == isisants_sideA)
	{
		printf("side A \r\n");
	}
	else
	{
		printf("side B \r\n");
	}

	print_error(IsisAntS_getAlltelemetry(antennaSystemsIndex, side, &allTelem));

	printf("Current deployment status 0x%x 0x%x (raw value) \r\n", allTelem.fields.ants_deployment.raw[0], allTelem.fields.ants_deployment.raw[1]);
	printDeploymentStatus(1, allTelem.fields.ants_deployment.fields.ant1Undeployed);
	printDeploymentStatus(2, allTelem.fields.ants_deployment.fields.ant2Undeployed);
	printDeploymentStatus(3, allTelem.fields.ants_deployment.fields.ant3Undeployed);
	printDeploymentStatus(4, allTelem.fields.ants_deployment.fields.ant4Undeployed);

	eng_value = ((float)allTelem.fields.ants_temperature * -0.2922) + 190.65;
	printf("\r\n AntS temperature %f deg. C\r\n", eng_value);
	printf("\r\n AntS uptime %d sec. \r\n", allTelem.fields.ants_uptime);

	return TRUE;
}

static Boolean setARMStatus(unsigned char index, ISISantsSide side, Boolean arm)
{
    printf( "DISARMING antenna system side %c \n\r", side + 'A');

	print_error(IsisAntS_setArmStatus(index, side, isisants_disarm));

	vTaskDelay(5 / portTICK_RATE_MS);

	if(arm)
	{
		unsigned int command = 0;

	    printf( "ARM antenna system side %c? (1=yes, 0=abort): \n\r", side + 'A');

	    while(UTIL_DbguGetIntegerMinMax(&command, 0, 1) == 0);

	    if(command == 1)
	    {
	    	int stat;

		    stat = IsisAntS_setArmStatus(index, side, isisants_arm);

		    print_error(stat);

		    if(stat == E_NO_SS_ERR)
		    {
		    	printf( "antenna system side %c successfully ARMED \n\r===>>> auto/manual deploy will deploy antennas when commanded <<<===\n\r", side + 'A');
		    }
		    else
		    {
		    	printf( "antenna system side %c arming failed \n\r", side + 'A');
		    }

			vTaskDelay(5 / portTICK_RATE_MS);
	    }
	    else
	    {
		    printf( "Aborted ARMING antenna system side %c  \n\r", side + 'A');
	    }
	}

	return TRUE;
}

static Boolean autoDeploymentAntSTest(unsigned char index, ISISantsSide side)
{
	unsigned char antennaSystemsIndex = index;
	int rv;

	printf("Auto deployment side %c ...\n\r", side + 'A');

	{	// check ARM status; if not ARMed no actual deployment will result
		ISISantsStatus status;
		rv = IsisAntS_getStatusData(index, side, &status);

		if(rv)
		{
			printf( "ERROR: Getting status side %c failed! rv=%d. Arm status unknown. Continuing ... \n\r", side + 'A', rv);
		}
		else
		{
			if(status.fields.armed)
			{
				printf( "Arm status side %c: ARMED. Deployment will result. \n\r", side + 'A');
			}
			else
			{
				printf( "Arm status side %c: DISARMED. No deployment will result. ARM first if deployment is desired. \n\r", side + 'A');
			}
		}
	}

	rv = IsisAntS_autoDeployment(antennaSystemsIndex, side, AUTO_DEPLOYMENT_TIME);
	if(rv)
	{
		printf( "ERROR: IsisAntS_autoDeployment command side %c failed! rv=%d \n\r", side + 'A', rv);
	}
	else
	{
		printf( "Auto-deployment command side %c successfully issued. \n\r", side + 'A');
	}

	return TRUE;
}


static Boolean manualDeploymentAntSTest(unsigned char index, ISISantsSide side)
{
    unsigned int antennaSelection = 0;
    unsigned char antennaSystemsIndex = index;
    int rv;

	printf("Manual deployment side %c ...\n\r", side + 'A');

	{	// check ARM status; if not ARMed no actual deployment will result
		ISISantsStatus status;
		rv = IsisAntS_getStatusData(index, side, &status);

		if(rv)
		{
			printf( "ERROR: Getting status side %c failed! rv=%d. Arm status unknown. Continuing ... \n\r", side + 'A', rv);
		}
		else
		{
			if(status.fields.armed)
			{
				printf( "Arm status side %c: ARMED. Deployment will result. \n\r", side + 'A');
			}
			else
			{
				printf( "Arm status side %c: DISARMED. No deployment will result. ARM first if deployment is desired. \n\r", side + 'A');
			}
		}
	}

    printf( "Select antenna to deploy on side %c (1, 2, 3, 4 or 5 to abort): \n\r", side + 'A');
    while(UTIL_DbguGetIntegerMinMax(&antennaSelection, 1, 5) == 0);

	print_error(IsisAntS_attemptDeployment(antennaSystemsIndex, side, antennaSelection-1, isisants_normalDeployment, MANUAL_DEPLOYMENT_TIME));
	printf( "Waiting %ds. for deployment of antenna %d on side %c\n\r...", MANUAL_DEPLOYMENT_TIME, antennaSelection, side + 'A');
	vTaskDelay(MANUAL_DEPLOYMENT_TIME*1000 / portTICK_RATE_MS);
    
    return TRUE;
}

Boolean selectAndExecuteAntSDemoTest(unsigned char index)
{
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf("\n\r Select a test to perform: \n\r");
	printf("\t 1) AntS reset - both sides \n\r");
	printf("\t 2) Ants status - both sides \n\r");
	printf("\t 3) AntS uptime - both sides \n\r");
	printf("\t 4) AntS temperature - both sides \n\r");
	printf("\t 5) AntS telemetry - side A \n\r");
	printf("\t 6) AntS telemetry - side B \n\r");
	printf("\t 7) AntS ARM deployment - side A \n\r");
	printf("\t 8) AntS ARM deployment - side B \n\r");
	printf("\t 9) AntS DISARM deployment - both sides \n\r");
	printf("\t 10) AntS autodeployment - side A\n\r");
	printf("\t 11) AntS autodeployment - side B\n\r");
    printf("\t 12) AntS manual deployment - side A\n\r");
    printf("\t 13) AntS manual deployment - side B\n\r");
	printf("\t 14) Return to main menu \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 14) == 0);

	switch(selection) {
	case 1:
		offerMoreTests = resetAntSTest(index);
		break;
	case 2:
		getStatusAntSTest(index, isisants_sideA);
		vTaskDelay(5 / portTICK_RATE_MS);
		getStatusAntSTest(index, isisants_sideB);
		break;
	case 3:
		offerMoreTests = uptimeAntSTest(index);
		break;
	case 4:
		offerMoreTests = tempAntSTest(index);
		break;
	case 5:
		offerMoreTests = telemAntSTest(index, isisants_sideA);
		break;
	case 6:
		offerMoreTests = telemAntSTest(index, isisants_sideB);
		break;
	case 7:
		offerMoreTests = setARMStatus(index, isisants_sideA, TRUE);
		break;
	case 8:
		offerMoreTests = setARMStatus(index, isisants_sideB, TRUE);
		break;
	case 9:
		setARMStatus(index, isisants_sideA, FALSE);
		setARMStatus(index, isisants_sideB, FALSE);
		break;
	case 10:
		offerMoreTests = autoDeploymentAntSTest(index, isisants_sideA);
		break;
	case 11:
		offerMoreTests = autoDeploymentAntSTest(index, isisants_sideB);
		break;
    case 12:
        offerMoreTests = manualDeploymentAntSTest(index, isisants_sideA);
        break;
    case 13:
        offerMoreTests = manualDeploymentAntSTest(index, isisants_sideB);
        break;
	case 14:
		offerMoreTests = FALSE;
		break;

	default:
		break;
	}

	return offerMoreTests;
}

static void initmain(void)
{
    int retValInt = 0;

	ISISantsI2Caddress myAntennaAddress[2];
	myAntennaAddress[0].addressSideA = 0x31;
	myAntennaAddress[0].addressSideB = 0x32;

	//Initialize the I2C
	retValInt = I2C_start(66000, 10);

	if(retValInt != 0)
	{
		TRACE_FATAL("\n\r I2Ctest: I2C_start_Master for Ants test: %d! \n\r", retValInt);
	}

	//Initialize the AntS system
	print_error(IsisAntS_initialize(myAntennaAddress, 1));
}

static void Ants_mainDemo(void)
{
	Boolean offerMoreTests = FALSE;

	while(1)
	{
		offerMoreTests = selectAndExecuteAntSDemoTest(0);

		if(offerMoreTests == FALSE)
		{
			break;
		}
	}
}


Boolean AntStest(void)
{
	initmain();
	Ants_mainDemo();

	return TRUE;
}

