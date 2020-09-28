#include "IsisSPdemo.h"
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

#include <satellite-subsystems/IsisSolarPanel.h>

static Boolean SolarPanel_Temperature(void)
{
	unsigned short paneltemp[5];
	float conv_temp[5];

	print_error(IsisSolarPanel_getTemperature(slave3_spi, &paneltemp[0]));
	print_error(IsisSolarPanel_getTemperature(slave4_spi, &paneltemp[1]));
	print_error(IsisSolarPanel_getTemperature(slave5_spi, &paneltemp[2]));
	print_error(IsisSolarPanel_getTemperature(slave6_spi, &paneltemp[3]));
	print_error(IsisSolarPanel_getTemperature(slave7_spi, &paneltemp[4]));

	conv_temp[0] = (float)((short)paneltemp[0]) * 0.0078125;
	conv_temp[1] = (float)((short)paneltemp[1]) * 0.0078125;
	conv_temp[2] = (float)((short)paneltemp[2]) * 0.0078125;
	conv_temp[3] = (float)((short)paneltemp[3]) * 0.0078125;
	conv_temp[4] = (float)((short)paneltemp[4]) * 0.0078125;

	printf("\r\n Temperature values \r\n");
	printf("\r\n %f \n", conv_temp[0]);
	printf("\r\n %f \n", conv_temp[1]);
	printf("\r\n %f \n", conv_temp[2]);
	printf("\r\n %f \n", conv_temp[3]);
	printf("\r\n %f \n", conv_temp[4]);

	vTaskDelay(1 / portTICK_RATE_MS);

	return TRUE;
}

static Boolean selectAndExecuteSolarPanelsDemoTest(void)
{
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf( "\n\r Select a test to perform: \n\r");
	printf("\t 1) Solar Panel Temperature \n\r");
	printf("\t 2) Return to main menu \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 2) == 0);

	switch(selection) {
	case 1:
		offerMoreTests = SolarPanel_Temperature();
		break;
	case 2:
		offerMoreTests = FALSE;
		break;

	default:
		break;
	}

	return offerMoreTests;
}

Boolean SolarPaneltest(void)
{
	Boolean offerMoreTests = FALSE;

	while(1)
	{
		offerMoreTests = selectAndExecuteSolarPanelsDemoTest();

		if(offerMoreTests == FALSE)
		{
			break;
		}
	}

	return TRUE;
}

