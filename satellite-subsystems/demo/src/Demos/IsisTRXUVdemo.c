/*
 * trxuv_operations.c
 *
 *  Created on: Jul 4, 2012
 *      Author: marcoalfer
 */

#include "IsisTRXUVdemo.h"
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

#include <satellite-subsystems/IsisTRXUV.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

////General Variables
#define TX_UPBOUND				30
#define TIMEOUT_UPBOUND			10

#define SIZE_RXFRAME	16
#define SIZE_TXFRAME	235

static Boolean radio_is_revB4 = TRUE;

// Test Function
static Boolean resetUVTest()
{
	printf("\r\n Resetting both IMC and ITC microcontrollers \r\n");
	IsisTrxuv_reset(0);

	return TRUE;
}

static Boolean itc_sendDefClSignTest(void)
{
	//Buffers and variables definition
	unsigned char testBuffer1[10]  = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40};
	unsigned char txCounter = 0;
	unsigned char avalFrames = 0;
	unsigned int timeoutCounter = 0;

	while(txCounter < 5 && timeoutCounter < 5)
	{
		printf("\r\n Transmission of single buffers with default callsign. AX25 Format. \r\n");
		print_error(IsisTrxuv_itcSendAX25DefClSign(0, testBuffer1, 10, &avalFrames));

		if ((avalFrames != 0)&&(avalFrames != 255))
		{
			printf("\r\n Number of frames in the buffer: %d  \r\n", avalFrames);
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

static Boolean itc_sendMsgCWTest(void)
{
	char cwBuffer[6] = {0x31,0x32,0x33,0x34,0x35};
	unsigned char txCounter = 0;
	unsigned char avalFrames = 0;
	unsigned int timeoutCounter = 0;

	while(txCounter < TX_UPBOUND && timeoutCounter < TIMEOUT_UPBOUND)
	{
		printf("\r\n Transmission of single buffers with default callsign. CW Format. \r\n");
		print_error(IsisTrxuv_itcSendCwMessage(0, cwBuffer, 20, &avalFrames));

		if ((avalFrames != 0)&&(avalFrames != 255))
		{
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

static Boolean itc_toggleIdleStateTest(void)
{
	static Boolean toggle_flag = 0;

	if(toggle_flag)
	{
	    print_error(IsisTrxuv_itcSetIdlestate(0, trxuv_idle_state_off));
		toggle_flag = FALSE;
	}
	else
	{
	    print_error(IsisTrxuv_itcSetIdlestate(0, trxuv_idle_state_on));
		toggle_flag = TRUE;
	}

	return TRUE;
}

static Boolean itc_setTxBitrate9600Test(void)
{
    print_error(IsisTrxuv_itcSetAx25Bitrate(0, trxuv_bitrate_9600));

	return TRUE;
}

static Boolean itc_setTxBitrate1200Test(void)
{
    print_error(IsisTrxuv_itcSetAx25Bitrate(0, trxuv_bitrate_1200));

	return TRUE;
}

static Boolean imc_getFrameCountTest(void)
{
	unsigned char RxCounter = 0;
	unsigned int timeoutCounter = 0;

	while(timeoutCounter < 4*TIMEOUT_UPBOUND)
	{

	    print_error(IsisTrxuv_imcGetFrameCount(0, &RxCounter));

		timeoutCounter++;


		vTaskDelay(10 / portTICK_RATE_MS);
	}
	printf("\r\n There are currently %d frames waiting in the RX buffer\r\n", RxCounter);

	return TRUE;
}

static Boolean imc_getFrameCmdTest(void)
{
	unsigned char RxCounter = 0;
	unsigned int i = 0;
	unsigned char rxframebuffer[SIZE_RXFRAME] = {0};
	ISIStrxuvRxFrame rxFrameCmd = {0, rxframebuffer};

	print_error(IsisTrxuv_imcGetFrameCount(0, &RxCounter));

	printf("\r\nThere are currently %d frames waiting in the RX buffer\r\n", RxCounter);

	while(RxCounter > 0)
	{
		print_error(IsisTrxuv_imcGetCommandFrame(0, &rxFrameCmd));

		printf("\r\nSize of the frame is = %d \r\n", rxFrameCmd.rx_length);

		printf("The received frame values are = ");

		for(i = 0; i < rxFrameCmd.rx_length; i++)
		{
			printf("%02x ", rxFrameCmd.rx_framedata[i]);
		}

		vTaskDelay(10 / portTICK_RATE_MS);
	}

	return TRUE;
}

static Boolean imc_getFrameCmdAndTxTest(void)
{
	unsigned char RxCounter = 0;
	unsigned int i = 0;
	unsigned char avalFrames = 0;
	unsigned char rxframebuffer[SIZE_RXFRAME] = {0};
	ISIStrxuvRxFrame rxFrameCmd = {0, rxframebuffer};

	print_error(IsisTrxuv_imcGetFrameCount(0, &RxCounter));

	printf("\r\nThere are currently %d frames waiting in the RX buffer\r\n", RxCounter);

	while(RxCounter > 0)
	{
		print_error(IsisTrxuv_imcGetCommandFrame(0, &rxFrameCmd));

		printf("\r\nSize of the frame is = %d \r\n", rxFrameCmd.rx_length);

		rxframebuffer[12] = '-';
		rxframebuffer[13] = 'O';
		rxframebuffer[14] = 'B';
		rxframebuffer[15] = 'C';

		print_error(IsisTrxuv_itcSendAX25DefClSign(0, rxframebuffer, SIZE_RXFRAME, &avalFrames));

		printf("The received frame values are = ");

		for(i = 0; i < rxFrameCmd.rx_length; i++)
		{
			printf("%02x ", rxFrameCmd.rx_framedata[i]);
		}
		printf("\r\n");

		print_error(IsisTrxuv_imcGetFrameCount(0, &RxCounter));

		vTaskDelay(10 / portTICK_RATE_MS);
	}

	return TRUE;
}

// Retrieve and print all IMC telemetry channels
static Boolean imc_getAllTelemTest(void)
{
	unsigned short telemetryValue;
	float eng_value = 0.0;
	ISIStrxuvTelemetry telemetry;

	// Telemetry values are presented as raw values
	printf("\r\nGet all Telemetry at once in raw values \r\n\r\n");
	print_error(IsisTrxuv_imcGetTelemetryAll(0, &telemetry));

	telemetryValue = telemetry.fields.rx_doppler;
	eng_value = ((float)telemetryValue/1023.0) * 3.3;
	printf("Doppler Voltage is = %f V\r\n", eng_value);

	telemetryValue = telemetry.fields.rx_rssi;
	eng_value = ((float)telemetryValue/1023.0) * 3.3;
	printf("RSSI value is = %f V\r\n", eng_value);

	telemetryValue = telemetry.fields.tx_reflpower;
	eng_value = ((float)telemetryValue) * ((float)telemetryValue) * 0.000239;
	printf("Reflected Power is = %f mW\r\n", eng_value);

	telemetryValue = telemetry.fields.tx_fwdpower;
	eng_value = ((float)telemetryValue) * ((float)telemetryValue) * 0.000239;
	printf("Forward Power is = %f mW\r\n", eng_value);

	telemetryValue = telemetry.fields.tx_current;
	if(radio_is_revB4)
	{
		eng_value = (((float)telemetryValue/1023.0) * 3.3) / 0.012;
	}
	else
	{
		eng_value = ((float)telemetryValue) * 0.395;
	}
	printf("Tx current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.rx_current;
	if(radio_is_revB4)
	{
		eng_value = (((float)telemetryValue/1023.0) * 3.3) / 0.02;
	}
	else
	{
		eng_value = ((float)telemetryValue) * 0.395;
	}
	printf("Rx current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.pa_temp;
	if(radio_is_revB4)
	{
		eng_value = ((float)telemetryValue * -0.2959) + 190;
	}
	else
	{
		eng_value = ((float)telemetryValue * 0.32258) - 50;
	}
	printf("Pa temp = %f deg. C\r\n", eng_value);

	telemetryValue = telemetry.fields.bus_volt;
	if(radio_is_revB4)
	{
		eng_value = ((float)telemetryValue/1023) * 24.033;
	}
	else
	{
		eng_value = ((float)telemetryValue/1023) * 5 * 3.3;
	}
	printf("Bus Voltage = %f V\r\n", eng_value);

	return TRUE;
}

static Boolean selectAndExecuteTRXUVDemoTest(void)
{
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf( "\n\r Select a test to perform: \n\r");
	printf("\t 1) Default Callsign Send Test\n\r");
	printf("\t 2) CW Message Send Test \n\r");
	printf("\t 3) Toggle Idle state \n\r");
	printf("\t 4) Change transmission bitrate to 9600 \n\r");
	printf("\t 5) Change transmission bitrate to 1200 \n\r");
	printf("\t 6) Get frame count \n\r");
	printf("\t 7) Get command frame \n\r");
	printf("\t 8) Get command frame and retransmit \n\r");
	printf("\t 9) Get all telemetry \n\r");
	printf("\t 10) Reset TRXUV both microcontrollers \n\r");
	printf("\t 11) Return to main menu \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 11) == 0);

	switch(selection) {
	case 1:
		offerMoreTests = itc_sendDefClSignTest();
		break;
	case 2:
		offerMoreTests = itc_sendMsgCWTest();
		break;
	case 3:
		offerMoreTests = itc_toggleIdleStateTest();
		break;
	case 4:
		offerMoreTests = itc_setTxBitrate9600Test();
		break;
	case 5:
		offerMoreTests = itc_setTxBitrate1200Test();
		break;
	case 6:
		offerMoreTests = imc_getFrameCountTest();
		break;
	case 7:
		offerMoreTests = imc_getFrameCmdTest();
		break;
	case 8:
		offerMoreTests = imc_getFrameCmdAndTxTest();
		break;
	case 9:
		offerMoreTests = imc_getAllTelemTest();
		break;
	case 10:
		offerMoreTests = resetUVTest();
		break;
	case 11:
		offerMoreTests = FALSE;
		break;

	default:
		break;
	}

	return offerMoreTests;
}

static void initmain(void)
{
    // Definition of I2C and TRXUV
	ISIStrxuvI2CAddress myTRXUVAddress[1];
	ISIStrxuvFrameLengths myTRXUVBuffers[1];
	ISIStrxuvBitrate myTRXUVBitrates[1];

	//I2C addresses defined
	myTRXUVAddress[0].addressImc = 0x50;
	myTRXUVAddress[0].addressItc = 0x51;

	//Buffer definition
	myTRXUVBuffers[0].maxAX25frameLengthTX = SIZE_TXFRAME;
	myTRXUVBuffers[0].maxAX25frameLengthRX = SIZE_RXFRAME;
	myTRXUVBuffers[0].maxCWframeLength = SIZE_TXFRAME;

	//Bitrate definition
	myTRXUVBitrates[0] = trxuv_bitrate_1200;

	//Initialize the trxuv subsystem
	print_error(IsisTrxuv_initialize(myTRXUVAddress, myTRXUVBuffers, myTRXUVBitrates, 1));
}

static void TRXUV_mainDemo(void)
{
	Boolean offerMoreTests = FALSE;

	while(1)
	{
		offerMoreTests = selectAndExecuteTRXUVDemoTest();

		if(offerMoreTests == FALSE)
		{
			break;
		}
	}
}

Boolean TRXUVtest(Boolean revB4)
{
	radio_is_revB4 = revB4;

	initmain();
	TRXUV_mainDemo();

	return TRUE;
}
