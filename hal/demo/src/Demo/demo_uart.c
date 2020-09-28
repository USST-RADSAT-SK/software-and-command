/*
 * demo_uart.c
 *
 *  Created on: 12 dec. 2014
 *      Author: pbot
 */

#include <hal/Drivers/UART.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <string.h>

#define PACKET_TIMEOUT_NONE 0
#define PACKET_TIMEOUT_FULL 0xFFFF
#define PACKET_SIZE_MAX     10

#define BUFFER_SIZE_MAX 16

#define UART_BUS bus0_uart

static void _VariablePacketTask( void* pvParameters );
static void _UnknownPacketTask( void* pvParameters );
static void _ProcessTransfer( UARTgenericTransfer *transfer );
void _UnknownPacketCallback(SystemContext context, xSemaphoreHandle sem);

/*
 * Note!
 * For this demo we are making the following assumptions:
 * - Packets have variable length but maximum size, therefore use timeout
 * - A minimum amount of idle time exists between packets, therefore use timeout
 */
void DEMO_UART_SetupVariablePacketTask(void)
{
	int retVal;

	UARTconfig config = {AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT, 115200, 1, rs232_uart, PACKET_TIMEOUT_FULL};

	retVal = UART_start(UART_BUS, config);
	if(retVal != 0)
	{
		TRACE_WARNING("\n\r DEMO_UART_SetupVariablePacketTask: UART_start returned %d! \n\r", retVal);
		while(1);
	}

	xTaskGenericCreate(_VariablePacketTask, (const signed char*)"VariablePacketTask", 4096, NULL, 2, NULL, NULL, NULL);
}

static void _VariablePacketTask( void* pvParameters )
{
	int retVal;
	int writeStringLen;
	char writeString[128];
	unsigned char packetBuffer[PACKET_SIZE_MAX+1] = {0};

	(void)pvParameters;

	writeStringLen = snprintf( writeString, sizeof(writeString), "Task is waiting for variable packet with maximum size of %d!\n\r", PACKET_SIZE_MAX );

	while(1)
	{
		// reset packet

		memset( packetBuffer, 0, sizeof(packetBuffer) );

		// transmit packet
		/*
		 * Because a timeout was specified, task will block until full packet
		 * has been received, or the bus went idle after receiving some data.
		 */

		// blocking write & read

		retVal = UART_write( UART_BUS, (unsigned char*)writeString, writeStringLen );
		if(retVal != 0)
		{
			TRACE_WARNING("\n\r _VariablePacketTask: UART_write returned %d! \n\r", retVal);
			while(1);
		}

		retVal = UART_read( UART_BUS, packetBuffer, PACKET_SIZE_MAX );
		if(retVal != 0)
		{
			TRACE_WARNING("\n\r _VariablePacketTask: UART_read returned %d! \n\r", retVal);
			while(1);
		}

		// process packet
		/*
		 * In this example the packet is processed by writing it over the debug
		 * UART in the same task. However, the risk exists that an incoming
		 * packet might be missed while processing this packet. Ideally, the
		 * received packet should be put in a queue for processing by another
		 * task and the next packet should be read as soon as possible.
		 */

		printf( "Task received a packet [%s] !\n\r", packetBuffer );
	}
}

/*
 * Note!
 * For this demo we are making the following assumptions:
 * - Packets have unknown length.
 * - Packets can arrive in batches.
 * Given the above assumptions, it is not possible to configure the driver
 * in such a way that a transfer equals a packet. A double buffer system is
 * implemented. When one buffer is full (or timeout occurred) the next buffer
 * is queued and the full buffer is passed to a function which processes
 * the buffer (i.e. extracts the packets).
 */
void DEMO_UART_SetupUnknownPacketTask( void )
{
	int retVal;

	UARTconfig config = {AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_OVER_16 | AT91C_US_NBSTOP_1_BIT, 115200, 1, rs232_uart, PACKET_TIMEOUT_FULL};

	retVal = UART_start(UART_BUS, config);
	if(retVal != 0)
	{
		TRACE_WARNING("\n\r DEMO_UART_SetupUnknownPacketTask: UART_start returned %d! \n\r", retVal);
		while(1);
	}

	xTaskGenericCreate(_UnknownPacketTask, (const signed char*)"_UnknownPacketTask", 1024, NULL, 2, NULL, NULL, NULL);
}

static void _UnknownPacketTask( void* pvParameters )
{
	(void)pvParameters;

	int retVal, i;

	// setup transfer 1

	unsigned char buffer1[BUFFER_SIZE_MAX+1] = {0};
	UARTtransferStatus transferResult1;
	xSemaphoreHandle packet1Semaphore;

	vSemaphoreCreateBinary(packet1Semaphore);
	if( !packet1Semaphore )
	{
		while(1);
	}

	UARTgenericTransfer transfer1 =
	{
			.bus = UART_BUS,
			.direction = read_uartDir,
			.readData = buffer1,
			.readSize = BUFFER_SIZE_MAX,
			.postTransferDelay = 0,
			.result = &transferResult1,
			.semaphore = packet1Semaphore,
			.callback = _UnknownPacketCallback
	};

	// setup transfer 2

	unsigned char buffer2[BUFFER_SIZE_MAX+1] = {0};
	UARTtransferStatus transferResult2;
	xSemaphoreHandle packet2Semaphore;

	vSemaphoreCreateBinary(packet2Semaphore);
	if( !packet2Semaphore )
	{
		while(1);
	}

	UARTgenericTransfer transfer2 =
	{
			.bus = UART_BUS,
			.direction = read_uartDir,
			.readData = buffer2,
			.readSize = BUFFER_SIZE_MAX,
			.postTransferDelay = 0,
			.result = &transferResult2,
			.semaphore = packet2Semaphore,
			.callback = _UnknownPacketCallback
	};

	// queue transfer 1
	xSemaphoreTake(transfer1.semaphore, (portTickType)1);
	retVal = UART_queueTransfer(&transfer1);
	if( retVal != 0 )
	{
		xSemaphoreGive(transfer1.semaphore);
		vSemaphoreDelete(transfer1.semaphore);
		while(1);
	}

	// queue transfer 2
	/*
	 * this avoids missing data after receiving and processing transfer 1
	 */
	xSemaphoreTake(transfer2.semaphore, (portTickType)1);
	retVal = UART_queueTransfer(&transfer2);
	if( retVal != 0 )
	{
		xSemaphoreGive(transfer2.semaphore);
		vSemaphoreDelete(transfer2.semaphore);
		while(1);
	}

	// display startup message

	int writeStringLen;
	char writeString[128];

	writeStringLen = snprintf( writeString, sizeof(writeString), "Task is waiting for unknown batch of bytes!\n\r");
	retVal = UART_write( UART_BUS, (unsigned char*)writeString, (unsigned int)writeStringLen );
	if( retVal != 0 )
	{
		TRACE_WARNING( "\n\r _UnknownPacketTask: UART_write returned %d! \n\r", retVal );
		while(1);
	}

	while(1)
	{
#if(0)
		/* block on semaphore */
		xSemaphoreTake(transfer1.semaphore, (portTickType)portMAX_DELAY);
#else
		/* busy block to test for starvation */
		while( xSemaphoreTake(transfer1.semaphore, (portTickType)0) != pdTRUE )
		{
			for( i = 10000; i != 0; i-- );
			vTaskDelay( (portTickType)0 );
		}
#endif
		xSemaphoreGive(transfer1.semaphore);

		// process transfer 1
		_ProcessTransfer( &transfer1 );

		// reset transfer 1
		memset( (void*)transfer1.readData, 0, transfer1.readSize );

		// re-queue transfer 1
		xSemaphoreTake(transfer1.semaphore, (portTickType)1);
		retVal = UART_queueTransfer(&transfer1);
		if(retVal != 0)
		{
			xSemaphoreGive(transfer1.semaphore);
			vSemaphoreDelete(transfer1.semaphore);
			while(1);
		}

#if(0)
		/* block on semaphore */
		xSemaphoreTake(transfer2.semaphore, (portTickType)portMAX_DELAY);
#else
		/* busy block to test for starvation */
		while( xSemaphoreTake(transfer2.semaphore, (portTickType)0) != pdTRUE )
		{
			for( i = 10000; i != 0; i-- );
			vTaskDelay( (portTickType)0 );
		}
#endif
		xSemaphoreGive(transfer2.semaphore);

		// process transfer 2
		_ProcessTransfer( &transfer2 );

		// reset transfer 2
		memset( (void*)transfer2.readData, 0, transfer2.readSize );

		// re-queue transfer 2
		xSemaphoreTake(transfer2.semaphore, (portTickType)1);
		retVal = UART_queueTransfer(&transfer2);
		if( retVal != 0 )
		{
			xSemaphoreGive(transfer2.semaphore);
			vSemaphoreDelete(transfer2.semaphore);
			while(1);
		}
	}
}

static void _ProcessTransfer( UARTgenericTransfer *transfer )
{
	char string[128];

	int length = snprintf( string, sizeof(string), "Task received %d byte(s) [%s] with result [%d]!\n\r", UART_getPrevBytesRead( transfer->bus ), transfer->readData, *(transfer->result) );
	UART_write( UART_BUS, (unsigned char*)string, length );

//	printf( "Task received %d byte(s) [%s] with result [%d]!\n\r", UART_getPrevBytesRead( transfer->bus ), transfer->readData, *(transfer->result) );

#if(0)
	/*
	 * busy wait to test if a delayed context switch
	 * causes bytes to be missed when switching buffers
	 */
	int i;
	for( i = 10000000; i != 0; i-- );
#endif
}


void _UnknownPacketCallback(SystemContext context, xSemaphoreHandle sem)
{
	signed portBASE_TYPE flag = pdFALSE;

	if(context == task_context)
	{
		xSemaphoreGive(sem);
	}
	else
	{
		xSemaphoreGiveFromISR(sem, &flag);
	}
}

