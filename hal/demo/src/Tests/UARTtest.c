/*
 * UARTtest.c
 *
 *  Created on: 20-Feb-2013
 *      Author: Akhil Piplani
 */

#include <at91/boards/ISIS_OBC_G20/board.h>
#include <at91/utility/trace.h>
#include <at91/commons.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <freertos/projdefs.h>

#include <hal/Drivers/UART.h>
#include <hal/interruptPriorities.h>
#include <hal/boolean.h>
#include <hal/Utility/util.h>

#include <string.h>
#include <stdio.h>

void taskUARTtest(void *arguments) {
	int retValInt = 0;
	unsigned int readSize = 4, i;
	unsigned char readData[16] = {0}, writeData[16] = {0};
	UARTbus bus = *((UARTbus*)arguments);

	while(1) {
		retValInt = UART_read(bus, readData, readSize);
		if(retValInt != 0) {
			TRACE_WARNING("\n\r taskUARTtest: UART_read returned: %d for bus %d \n\r", retValInt, bus);
		}

		for(i=0; i<readSize; i++) {
			if(readData[i]>='a' && readData[i]<='z') {
				writeData[i] = readData[i] - 'a' + 'A';
			}
			else {
				writeData[i] = readData[i];
			}
		}
		writeData[i]   = '\n';
		writeData[i+1] = '\r';

		retValInt = UART_write(bus, writeData, readSize+2); // Write 2 bytes more than we received for \n\r
		if(retValInt != 0) {
			TRACE_WARNING("\n\r taskUARTtest: UART_write returned: %d for bus %d \n\r", retValInt, bus);
		}

		vTaskDelay(1);
	}
}

Boolean UARTtest() {
	int retValInt = 0;
	unsigned int bus2type = 0;
	xTaskHandle taskUART0testHandle, taskUART2testHandle;
	static UARTbus UARTtestBus[2] = {bus0_uart, bus2_uart};

	UARTconfig configBus0 = {.mode = AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT,
								.baudrate = 115200, .timeGuard = 1, .busType = rs232_uart, .rxtimeout = 0xFFFF};
	UARTconfig configBus2 = {.mode = AT91C_US_USMODE_HWHSH  | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT,
								.baudrate = 115200, .timeGuard = 1, .busType = rs232_uart, .rxtimeout = 0xFFFF};

	printf("\n This test will receive 4 characters over UART, capitalize them and send them back. \n");
	printf(" If you send \"12ab\", you will receive back \"12AB\" on the same bus. \n");

	printf("\n Please select a configuration for UART2 (0=RS232 1=RS422): \n");
	UTIL_DbguGetIntegerMinMax(&bus2type, 0, 1);

	if(bus2type != 0) {
		configBus2.mode = AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT;
		configBus2.busType = rs422_noTermination_uart;
	}

	// Both UART peripherals must be started separately as they can use different configurations.
	retValInt = UART_start(bus0_uart, configBus0);
	if(retValInt != 0) {
		TRACE_WARNING("\n\r UARTtest: UART_start returned %d! \n\r", retValInt);
		while(1);
	}
	retValInt = UART_start(bus2_uart, configBus2);
	if(retValInt != 0) {
		TRACE_WARNING("\n\r UARTtest: UART_start returned %d! \n\r", retValInt);
		while(1);
	}

	// Instantiate two separate versions of taskUARTtest and pass different bus-id's as a parameter.
	xTaskGenericCreate(taskUARTtest, (const signed char*)"taskUARTtest-0", 1024, (void*)&UARTtestBus[0], 2, &taskUART0testHandle, NULL, NULL);
	xTaskGenericCreate(taskUARTtest, (const signed char*)"taskUARTtest-2", 1024, (void*)&UARTtestBus[1], 2, &taskUART2testHandle, NULL, NULL);

	return FALSE;
}
