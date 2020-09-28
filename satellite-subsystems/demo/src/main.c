/*
 * main.c
 *      Author: Akhil
 */

#include "Demos/IsisAntSdemo.h"
#include "Demos/GomEPSdemo.h"
#include "Demos/IsisSPdemo.h"
#include "Demos/IsisSPv2demo.h"
#include "Demos/IsisTRXUVdemo.h"
#include "Demos/IsisTRXVUdemo.h"
#include "Demos/IsisTxSdemo.h"
#include "Demos/IsisMTQv1demo.h"
#include "Demos/IsisMTQv2demo.h"
#include "Demos/cspaceADCSdemo.h"
#include "Demos/ScsGeckoDemo.h"
#include "Demos/isis_eps_demo.h"
#include <satellite-subsystems/version/version.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>
#include <at91/peripherals/pio/pio_it.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/LED.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/SPI.h>
#include <hal/boolean.h>
#include <hal/version/version.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ENABLE_MAIN_TRACES 1
#if ENABLE_MAIN_TRACES
	#define MAIN_TRACE_INFO			TRACE_INFO
	#define MAIN_TRACE_DEBUG		TRACE_DEBUG
	#define MAIN_TRACE_WARNING		TRACE_WARNING
	#define MAIN_TRACE_ERROR		TRACE_ERROR
	#define MAIN_TRACE_FATAL		TRACE_FATAL
#else
	#define MAIN_TRACE_INFO(...)	{ }
	#define MAIN_TRACE_DEBUG(...)	{ }
	#define MAIN_TRACE_WARNING(...)	{ }
	#define MAIN_TRACE_ERROR		TRACE_ERROR
	#define MAIN_TRACE_FATAL		TRACE_FATAL
#endif

Boolean selectAndExecuteTest()
{
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;

	//Initialize the I2C
	int retValInt = I2C_start(100000, 10);
	if(retValInt != 0)
	{
		TRACE_FATAL("\n\r I2C_start_Master for demo: %d! \n\r", retValInt);
	}

	retValInt = SPI_start(bus1_spi, slave7_spi);
	if(retValInt != 0)
	{
		TRACE_WARNING("\n\r SPI_start for demo: %d! \n\r", retValInt);
	}

	printf( "\n\r Select the device to be tested to perform: \n\r");
	printf("\t 1) TRXUV rev. B3 test \n\r");
	printf("\t 2) TRXUV rev. B4 test \n\r");
	printf("\t 3) TRXVU test \n\r");
	printf("\t 4) TxS test \n\r");
	printf("\t 5) AntS test \n\r");
	printf("\t 6) Solar Panels test \n\r");
	printf("\t 7) Solar Panels V2 test \n\r");
	printf("\t 8) MTQv1 test \n\r");
	printf("\t 9) MTQv2 test \n\r");
	printf("\t 10) Gomspace EPS test \n\r");
	printf("\t 11) ISIS EPS Test \n\r");
	printf("\t 12) CubeSpace ADCS test \n\r");
	printf("\t 13) Gecko Test \n\r");


	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 13) == 0);

	switch(selection)
	{
		case 1:
			offerMoreTests = TRXUVtest(FALSE);
			break;
		case 2:
			offerMoreTests = TRXUVtest(TRUE);
			break;
		case 3:
			offerMoreTests = TRXVUtest();
			break;
		case 4:
			offerMoreTests = TxStest();
			break;
		case 5:
			offerMoreTests = AntStest();
			break;
		case 6:
			offerMoreTests = SolarPaneltest();
			break;
		case 7:
			offerMoreTests = SolarPanelv2test();
			break;
		case 8:
			offerMoreTests = MTQv1test();
			break;
		case 9:
			offerMoreTests = IsisMTQv2test();
			break;
		case 10:
			offerMoreTests = GomEPStest();
			break;
		case 11:
			offerMoreTests = isis_eps__test();
			break;
		case 12:
			offerMoreTests = cspaceADCStest();
			break;
		case 13:
			offerMoreTests = GeckoTest();
			break;

		default:
			break;
	}

	return offerMoreTests;
}

void taskMain()
{
	Boolean offerMoreTests = FALSE;

	WDT_startWatchdogKickTask(10 / portTICK_RATE_MS, FALSE);

	while(1)
	{
		LED_toggle(led_1);

		offerMoreTests = selectAndExecuteTest();

		if(offerMoreTests == FALSE)
		{
			break;
		}
	}

	while(1) {
		LED_toggle(led_1);
		vTaskDelay(500);
	}

}

int main()
{
	unsigned int i;
	xTaskHandle taskMainHandle;

	TRACE_CONFIGURE_ISP(DBGU_STANDARD, 2000000, BOARD_MCK);
	// Enable the Instruction cache of the ARM9 core. Keep the MMU and Data Cache disabled.
	CP15_Enable_I_Cache();

	LED_start();

	// The actual watchdog is already started, this only initializes the watchdog-kick interface.
	WDT_start();

	PIO_InitializeInterrupts(AT91C_AIC_PRIOR_LOWEST+4);

	printf("\n\nDemo applications for ISIS OBC Satellite Subsystems Library built on %s at %s\n", __DATE__, __TIME__);
	printf("\nDemo applications use:\n");
	printf("* Sat Subsys lib version %s.%s.%s built on %s at %s\n",
			SatelliteSubsystemsVersionMajor, SatelliteSubsystemsVersionMinor, SatelliteSubsystemsVersionRevision,
			SatelliteSubsystemsCompileDate, SatelliteSubsystemsCompileTime);
	printf("* HAL lib version %s.%s.%s built on %s at %s\n", HalVersionMajor, HalVersionMinor, HalVersionRevision,
			HalCompileDate, HalCompileTime);

	LED_wave(1);
	LED_waveReverse(1);
	LED_wave(1);
	LED_waveReverse(1);

	MAIN_TRACE_DEBUG("\t main: Starting main task.. \n\r");
	xTaskGenericCreate(taskMain, (const signed char*)"taskMain", 4096, NULL, configMAX_PRIORITIES-2, &taskMainHandle, NULL, NULL);

	MAIN_TRACE_DEBUG("\t main: Starting scheduler.. \n\r");
	vTaskStartScheduler();

	// This part should never be reached.
	MAIN_TRACE_DEBUG("\t main: Unexpected end of scheduling \n\r");

	//Flash some LEDs for about 100 seconds
	for (i=0; i < 2500; i++)
	{
		LED_wave(1);
		MAIN_TRACE_DEBUG("MAIN: STILL ALIVE %d\n\r", i);
	}
	exit(0);
}
