#include "IsisSPdemo.h"

#include <satellite-subsystems/IsisSolarPanelv2.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>

#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/SPI.h>
#include <hal/Utility/util.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <stdint.h>

Boolean SolarPanelv2_Temperature()
{
	int error;
	int panel;
	uint8_t status = 0;
	int32_t paneltemp = 0;
	float conv_temp;

	IsisSolarPanelv2_wakeup();

	printf("\r\n Temperature values \r\n");

	for( panel = 0; panel < ISIS_SOLAR_PANEL_COUNT; panel++ )
	{
		error = IsisSolarPanelv2_getTemperature(panel, &paneltemp, &status);
		if( error )
		{
			printf("Panel %d : Error (%d), Status (0x%X) \r\n", panel, error, status);
			continue;
		}

		conv_temp = (float)(paneltemp) * ISIS_SOLAR_PANEL_CONV;

		printf("Panel %d : %f \n", panel, conv_temp);
	}

	IsisSolarPanelv2_sleep();

	vTaskDelay( 1 / portTICK_RATE_MS );

	return TRUE;
}

Boolean selectAndExecuteSolarPanelsv2DemoTest()
{
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf("\n\r Select a test to perform: \n\r");
	printf("\t 1) Solar Panel Temperature \n\r");
	printf("\t 2) Return to main menu \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 2) == 0);

	switch(selection)
	{
	case 1:
		offerMoreTests = SolarPanelv2_Temperature();
		break;
	case 2:
		offerMoreTests = FALSE;
		break;

	default:
		break;
	}

	return offerMoreTests;
}

void SolarPanelsv2_mainDemo()
{
	Boolean offerMoreTests = FALSE;

	while(1)
	{
		offerMoreTests = selectAndExecuteSolarPanelsv2DemoTest();

		if(offerMoreTests == FALSE)
		{
			break;
		}
	}
}


Boolean SolarPanelv2test()
{
	int retValInt = 0;

	retValInt = IsisSolarPanelv2_initialize(slave0_spi);
	if(retValInt != 0)
	{
		TRACE_WARNING("\n\r IsisSolarPaneltest: IsisSolarPanelv2_initialize returned %d! \n\r", retValInt);
	}

	retValInt = IsisSolarPanelv2_sleep();
	if(retValInt != 0)
	{
		TRACE_WARNING("\n\r IsisSolarPaneltest: IsisSolarPanelv2_sleep returned %d! \n\r", retValInt);
	}

	SolarPanelsv2_mainDemo();

	return TRUE;
}

