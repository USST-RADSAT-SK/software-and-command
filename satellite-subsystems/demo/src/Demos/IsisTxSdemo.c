/*
 * IsisTxSdemo.c
 *
 *  Created on: 4 mrt. 2015
 *      Author: malv
 */
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

#include <satellite-subsystems/IsisTxS.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Test Functions
static Boolean softResetTxsTest(void)
{
	printf("\r\n Software Reset of the TxS microcontroller \r\n");
	print_error(IsisTxsReset(0));

	return TRUE;
}

static Boolean halfMaxBitrateTxsTest(void)
{
	printf("\r\n The current bitrate has been set to half its maximum value \r\n");
	print_error(IsisTxsSetAx25Bitrate(0, txs_bitrate_half));

	return TRUE;
}

static Boolean MaxBitrateTxsTest(void)
{
	printf("\r\n The current bitrate has been set to its maximum value \r\n");
	print_error(IsisTxsSetAx25Bitrate(0, txs_bitrate_full));

	return TRUE;
}

static Boolean level4_5dBTxsTest(void)
{
	printf("\r\n The modulator attenuation level to 4,5 dB \r\n");
	print_error(IsisTxsSetModulatorAttenuator(0, txs_4_5dB_attenuation));

	return TRUE;
}

static Boolean level13_5dBTxsTest(void)
{
	printf("\r\n The modulator attenuation level to 13,5 dB \r\n");
	print_error(IsisTxsSetModulatorAttenuator(0, txs_13_5dB_attenuation));

	return TRUE;
}

static Boolean toggleTxsStateTest(void)
{
	static Boolean toggle_flag = 0;

	if(toggle_flag)
	{
	    print_error(IsisTxsSetState(0, txs_state_off));
		toggle_flag = FALSE;
	}
	else
	{
	    print_error(IsisTxsSetState(0, txs_state_on));
		toggle_flag = TRUE;
	}

	return TRUE;
}

static Boolean addDefaultCallsignFrames(void)
{
	//Buffers and variables definition
	unsigned char testBuffer1[10]  = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40};
	unsigned char txCounter = 0;
	isis_txs_buffavailable_t txsbuffinfo = {0};
	unsigned int timeoutCounter = 0;

	while(txCounter < 5 && timeoutCounter < 5)
	{
		printf("\r\n Transmission of single buffers with default callsign. AX25 Format. \r\n");
		print_error(IsisTxsSendAX25DefClSign(0, testBuffer1, 10, &txsbuffinfo));

		if(txsbuffinfo.bufferspace > 0 && txsbuffinfo.maxframelength >= 10)
		{
			printf("\r\n Max amount of bytes to fit in the buffer: %d  \r\n", txsbuffinfo.bufferspace);
			printf("\r\n Max length of a frame to fit in the buffer: %d  \r\n", txsbuffinfo.maxframelength);
			txCounter++;
		}
		else
		{
			vTaskDelay(100 / portTICK_RATE_MS);
			timeoutCounter++;
		}
	}

	return TRUE;
}

static Boolean getAllTelemTxSTest(void)
{
	unsigned short telemetryValue;
	unsigned char info_value, status_value;
	unsigned short templut_index = 0;
	float eng_value = 0.0;
	isis_txs_telemetry_t telemetry;

	// Telemetry values are presented as raw values
	printf("\r\nGet all Telemetry \r\n\r\n");
	print_error(IsisTxsGetHousekeepingData(0, &telemetry));

	status_value = telemetry.fields.first_status.rawValue;
	printf("Raw status value 1 = 0x%x \r\n", status_value);

	if (telemetry.fields.first_status.fields.TXS_STATE == 1)
	{
		printf("- TxS state on \r\n");
	}

	if (telemetry.fields.first_status.fields.TX_ENABLE == 1)
	{
		printf("- Transmit functionality is enabled \r\n");
	}

	if (telemetry.fields.first_status.fields.PA_ENABLE == 1)
	{
		printf("- Power amplifier is active \r\n");
	}

	switch(telemetry.fields.first_status.fields.PLL_LOCK)
	{
		case 0x00:
			printf("- PLL is inactive \r\n");
			break;
		case 0x01:
			printf("- PLL success \r\n");
			break;
		case 0x02:
			printf("- PLL Failed \r\n");
			break;
		case 0x03:
			printf("- PLL Busy \r\n");
			break;
	}

	if (telemetry.fields.first_status.fields.TXS_MODE == 1)
	{
		printf("- External mode on \r\n");
	}
	else
	{
		printf("- Nominal mode on \r\n");
	}

	status_value = telemetry.fields.second_status.rawValue;
	printf("\r\nRaw status value 2 = 0x%x \r\n", status_value);

	switch(telemetry.fields.second_status.fields.ATT_CTRL)
	{
		case 0x00:
			printf("- 0 dB of attenuation \r\n");
			break;
		case 0x01:
			printf("- 4.5 dB of attenuation \r\n");
			break;
		case 0x02:
			printf("- 9 dB of attenuation \r\n");
			break;
		case 0x03:
			printf("- 13.5 dB of attenuation \r\n");
			break;
	}

	switch(telemetry.fields.second_status.fields.LAST_RESET_CAUSE)
	{
		case 0x00:
			printf("- Watchdog Reset \r\n");
			break;
		case 0x01:
			printf("- Software Reset \r\n");
			break;
		case 0x02:
			printf("- External trigger Reset \r\n");
			break;
	}

	switch(telemetry.fields.second_status.fields.BIT_RATE_CTRL)
	{
		case 0x01:
			printf("- Bitrate to full rate \r\n");
			break;
		case 0x02:
			printf("- Bitrate to half rate \r\n");
			break;
		case 0x03:
			printf("- Bitrate to quarter rate \r\n");
			break;
		case 0x04:
			printf("- Bitrate to eighth rate \r\n");
			break;
	}

	printf("\r\nTxS uptime %d sec  \r\n\r\n", telemetry.fields.uptime);

	telemetryValue = telemetry.fields.txs_current;
	eng_value = (float)telemetryValue/4095.0;
	printf("- TxS current = %.2f \r\n", eng_value);

	telemetryValue = telemetry.fields.txs_voltage;
	eng_value = ((float)telemetryValue/4095.0) * 30.03;
	printf("- TxS voltage = %.2f \r\n", eng_value);

	telemetryValue = telemetry.fields.txs_board_temp;
	printf("- TxS board temp = %.2f deg. C\r\n", ((double)telemetryValue) * -0.0587 + 192.04);

	telemetryValue = telemetry.fields.txs_pa_temp;
    printf("- TxS PA temp = %.2f deg. C\r\n", ((double)telemetryValue) * -0.0587 + 192.04);

	info_value = telemetry.fields.txs_state_info;
	printf("\r\nRaw state info value = 0x%x \r\n", info_value);

	switch (info_value)
	{
		case power_on_reset:
			printf("- The Power On Reset has occurred. \r\n");
			break;
		case i2c_command:
			printf("- The I2C Command caused the TXS to change the mode. \r\n");
			break;
		case pll_cant_lock:
			printf("- The PLL cannot lock. \r\n");
			break;
		case temperature_switch:
			printf("- The temperature of the Power Amplifier getting too high that caused the state to change. \r\n");
			break;
	}

	telemetryValue = telemetry.fields.txs_temp_threshold;
	templut_index = ((unsigned int)telemetryValue* 3300/4095);
	printf("- TxS temperature threshold = %d \r\n", templut_index);

    printf("- Available buffer space = %d bytes\r\n", telemetry.fields.txs_available_bytes.bufferspace);
    printf("- Maximum allowed frame length = %d bytes\r\n", telemetry.fields.txs_available_bytes.maxframelength);

	return TRUE;
}


static Boolean selectAndExecuteTxSDemoTest(void)
{
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf( "\n\r Select a test to perform: \n\r");
	printf("\t 1) TxS software reset  \n\r");
	printf("\t 2) Set bit rate to half its maximum value \n\r");
	printf("\t 3) Set bit rate to maximum value \n\r");
	printf("\t 4) Set modulator attenuation level to 4,5 dB \n\r");
	printf("\t 5) Set modulator attenuation level to 13,5 dB \n\r");
	printf("\t 6) Toggle TxS state  \n\r");
	printf("\t 7) Add AX.25 frame with default callsign to data buffer\n\r");
	printf("\t 8) Request Housekeeping data \n\r");
	printf("\t 9) Return to main menu \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 9) == 0);

	switch(selection) {
	case 1:
		offerMoreTests = softResetTxsTest();
		break;
	case 2:
		offerMoreTests = halfMaxBitrateTxsTest();
		break;
	case 3:
		offerMoreTests = MaxBitrateTxsTest();
		break;
	case 4:
		offerMoreTests = level4_5dBTxsTest();
		break;
	case 5:
		offerMoreTests = level13_5dBTxsTest();
		break;
	case 6:
		offerMoreTests = toggleTxsStateTest();
		break;
	case 7:
		offerMoreTests = addDefaultCallsignFrames();
		break;
	case 8:
		offerMoreTests = getAllTelemTxSTest();
		break;
	case 9:
		offerMoreTests = FALSE;
		break;

	default:
		break;
	}

	return offerMoreTests;
}


Boolean IsisTxSdemoInit(void)
{
    unsigned char i2c_address = 0x35;
    unsigned int maxframe_length = 500;
    int rv;

    rv = IsisTxsInitialize(&i2c_address, &maxframe_length, 1);
    if(rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED)
    {
    	// we have a problem. Indicate the error. But we'll gracefully exit to the higher menu instead of
    	// hanging the code
    	TRACE_ERROR("\n\r IsisTxsInitialize() failed; err=%d! Exiting ... \n\r", rv);
    	return FALSE;
    }

    return TRUE;
}

void IsisTxSdemoLoop(void)
{
	Boolean offerMoreTests = FALSE;

	while(1)
	{
		offerMoreTests = selectAndExecuteTxSDemoTest(); // show the demo command line interface and handle commands

		if(offerMoreTests == FALSE)  // was exit/back
		{
			break;
		}
	}
}

Boolean IsisTxSdemoMain(void)
{
	if(IsisTxSdemoInit())                                 // initialize of I2C and IsisTRXVU subsystem drivers succeeded?
	{
		IsisTxSdemoLoop();                                // show the main IsisTRXVU demo interface and wait for user input
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}


Boolean TxStest(void)
{
	IsisTxSdemoMain();
	return TRUE;
}
