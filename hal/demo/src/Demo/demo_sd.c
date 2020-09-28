/*
 * demo_sd.c
 *
 *  Created on: 24 mrt. 2015
 *      Author: pbot
 */

#include <hcc/api_fat.h>
#include <hcc/api_hcc_mem.h>
#include <hcc/api_fat_test.h>
#include <hcc/api_mdriver_atmel_mcipdc.h>

#include <hal/Utility/fprintf.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <at91/utility/assert.h>

/*
 * Warning!
 *   Be careful not to mix the HCC definitions with stdio definitions when using
 *   the filesystem function calls. The two are very similar. HCC definitions
 *   have a F_ prefix.
 */
//#include <stdio.h>

#define ENABLE_DEMO_SD_TRACES 1
#if ENABLE_DEMO_SD_TRACES
	#define DEMO_SD_TRACE_INFO		TRACE_INFO
	#define DEMO_SD_TRACE_DEBUG		TRACE_DEBUG
	#define DEMO_SD_TRACE_WARNING	TRACE_WARNING
	#define DEMO_SD_TRACE_ERROR		TRACE_ERROR
	#define DEMO_SD_TRACE_FATAL		TRACE_FATAL
#else
	#define DEMO_SD_TRACE_INFO(...)	{ }
	#define DEMO_SD_TRACE_DEBUG(...)	{ }
	#define DEMO_SD_TRACE_WARNING(...)	{ }
	#define DEMO_SD_TRACE_ERROR		TRACE_ERROR
	#define DEMO_SD_TRACE_FATAL		TRACE_FATAL
#endif

#define _SAFE 1
#define _BUFF_SIZE ( 2 * 1024 )

unsigned char pWriteBuff[ _BUFF_SIZE ];
unsigned char pReadBuff[ _BUFF_SIZE ];

static void _TaskTestFileLogWrite( void * pvParam );
static void _TaskTestFileLogRead( void * pvParam );
static void _MemSetTestPattern( unsigned char * pBuffer, unsigned int size );

static xTaskHandle _task[2] = {0};

/*
 * This function demonstrates the basics of using the filesystem on the
 * SD Card. It handles initialization and tear down of the file system, and
 * write and read operations. For a complete list of available filesystem
 * commands refer to the user guide.
 */
void DEMO_SD_Basic( int volID )
{
	int ret, i, bw, br;
	F_FILE *file;

	if( volID != 0 && volID != 1 )
	{
		return;
	}

	/*
	 * 1. Initialize:
	 */

	hcc_mem_init(); /* Initialize the memory to be used by filesystem */

	ret = fs_init(); /* Initialize the filesystem */
	ASSERT( (ret == F_NO_ERROR ), "fs_init pb: %d\n\r", ret);

	ret = f_enterFS(); /* Register this task with filesystem */
	ASSERT( (ret == F_NO_ERROR ), "f_enterFS pb: %d\n\r", ret);

	/*
	 * 2. Selecting SD Card:
	 */

#if(_SAFE)
	ret = f_initvolume( 0, atmel_mcipdc_initfunc, volID ); /* Initialize volID as safe */
#else
	ret = f_initvolume_nonsafe( 0, atmel_mcipdc_initfunc, volID ); /* Initialize volID as nonsafe */
#endif
	ASSERT( ( ( ret == F_NO_ERROR ) || ( ret == F_ERR_NOTFORMATTED ) ), "f_initvolume pb: %d\n\r", ret);

	if( F_ERR_NOTFORMATTED == ret )
	{
		ret = f_format( 0, F_FAT32_MEDIA ); /* Format the filesystem */
		ASSERT( ( ret == F_NO_ERROR ), "f_format pb: %d\n\r", ret);
	}

	DEMO_SD_TRACE_INFO( "SD Card (%d) initialization completed!\n\r", volID );

	/*
	 * 3. Write Operation:
	 */

#if(_SAFE)
	file = f_open( "test.bin", "w" ); /* open file for writing in safe mode */
#else
	file = f_open_nonsafe( filename, "w" ); /* open file for writing in nonsafe mode */
#endif
	ASSERT( (file), "f_open pb: %d\n\r", f_getlasterror() ); /* if file pointer is NULL, get the error */

	for( i = 0; i < 4; i++ )
	{
		bw = f_write( pWriteBuff, 1, _BUFF_SIZE, file );
		ASSERT( ( _BUFF_SIZE == bw ),  "f_write pb: %d\n\r", f_getlasterror() ); /* if bytes to write doesn't equal bytes written, get the error */

		f_flush( file ); /* only after flushing can data be considered safe */
	}

	ret = f_close( file ); /* data is also considered safe when file is closed */
	ASSERT( (ret == F_NO_ERROR ), "f_close pb: %d\n\r", ret);

	DEMO_SD_TRACE_INFO( "SD Card (%d) write operation completed!\n\r", volID );

	/*
	 * 3. Read Operation:
	 */

	file = f_open( "test.bin", "r" ); /* open file for reading, which is always safe */
	ASSERT( ( file ), "f_open pb: %d\n\r", f_getlasterror() ); /* if file pointer is NULL, get the error */

	br = f_read( pReadBuff, 1, _BUFF_SIZE, file );
	ASSERT( ( _BUFF_SIZE == br ),  "f_read pb: %d\n\r", f_getlasterror() ); /* if bytes to read doesn't equal bytes read, get the error */

	f_close( file );
	ASSERT( ( ret == F_NO_ERROR ), "f_close pb: %d\n\r", ret );

	DEMO_SD_TRACE_INFO( "SD Card (%d) read operation completed!\n\r", volID );

	/*
	 * 4. Stop SD Card:
	 */

	f_delvolume( volID ); /* delete the volID */

	/*
	 * Selecting SD Card:
	 *   A new SD card can be selected using the procedure in (2).
	 */

	/*
	 * Tear down:
	 *   When all file operations are done, the following sequence of calls
	 *   should be executed to properly cleanup the hardware and software.
	 */

	f_releaseFS(); /* release this task from the filesystem */

	fs_delete(); /* delete the filesystem */

	hcc_mem_delete(); /* free the memory used by the filesystem */

	DEMO_SD_TRACE_INFO( "SD Card (%d) de-initialization completed!\n\r", volID );
}

void DEMO_SD_FileTasks( int volID )
{
	int ret;

	if( volID != 0 && volID != 1 )
	{
		return;
	}

	hcc_mem_init();

	ret = fs_init();
	ASSERT( (ret == F_NO_ERROR ), "fs_init pb: %d\n\r", ret);

	/*
	 * Since we are going to use f_* functions (f_initvolume(...) and f_format(...))
	 * we need register task with filesystem.
	 */
	ret = f_enterFS();
	ASSERT( (ret == F_NO_ERROR ), "f_enterFS pb: %d\n\r", ret);

#if(_SAFE)
	ret = f_initvolume( 0, atmel_mcipdc_initfunc, volID );
#else
	ret = f_initvolume_nonsafe( 0, atmel_mcipdc_initfunc, volID );
#endif
	ASSERT( ( ( ret == F_NO_ERROR ) || ( ret == F_ERR_NOTFORMATTED ) ), "f_initvolume pb: %d\n\r", ret);

	if( ret == F_ERR_NOTFORMATTED )
	{
		ret = f_format( 0, F_FAT32_MEDIA );
		ASSERT( ( ret == F_NO_ERROR ), "f_format pb: %d\n\r", ret);
	}

	f_releaseFS( );

	xTaskGenericCreate( _TaskTestFileLogWrite, (const signed char*)"TestFileLogWrite", 4096, NULL, configMAX_PRIORITIES-2, &_task[0], NULL, NULL);
	xTaskGenericCreate( _TaskTestFileLogRead,  (const signed char*)"TestFileLogRead", 4096, NULL, configMAX_PRIORITIES-3, &_task[1], NULL, NULL);

	while(1)
	{
		vTaskDelay(1);
	}
}

/**
 * Test string functionality on the file-system by creating a text file
 * "strings.txt" and writing various strings to the file using the f_printf()
 * command. The user should verify these strings manually.
 */
void DEMO_SD_StringFunc( int volID )
{
	F_FILE *file;
	int ret;
	char filename[] = "strings.txt";

	DEMO_SD_TRACE_INFO("Testing FatFS string functionality on SD Card...\r\n");

	ret = hcc_mem_init();
	ASSERT( (ret == HCC_MEM_SUCCESS ), "hcc_mem_init pb: %d\n\r", ret);

	ret = fs_init();
	ASSERT( (ret == F_NO_ERROR ), "fs_init pb: %d\n\r", ret);

	ret = f_enterFS();
	ASSERT( (ret == F_NO_ERROR ), "f_enterFS pb: %d\n\r", ret);

#if(_SAFE)
	ret = f_initvolume( 0, atmel_mcipdc_initfunc, volID );
	ASSERT( ( ( ret == F_NO_ERROR ) || ( ret == F_ERR_NOTFORMATTED ) ), "f_initvolume pb: %d\n\r", ret);
#else
	ret = f_initvolume_nonsafe( 0, atmel_mcipdc_initfunc, 0 );
	ASSERT( ( ( ret == F_NO_ERROR ) || ( ret == F_ERR_NOTFORMATTED ) ), "f_initvolume_nonsafe pb: %d\n\r", ret);
#endif

	file = f_open(filename,"w");
	ASSERT( F_NO_ERROR == f_getlasterror(), "f_open pb: 0x%X\n\r", f_getlasterror());

	// Write test strings

	f_printf(file, "%d\n", 1234);            /* "1234" */
	f_printf(file, "%6d,%3d%%\n", -200, 5);  /* "  -200,  5%" */
	f_printf(file, "%ld\n", 12345L);         /* "12345" */
	f_printf(file, "%06d\n", 25);            /* "000025" */
	f_printf(file, "%06d\n", -25);           /* "000-25" */
	f_printf(file, "%-6d\n", 25);            /* "25    " */
	f_printf(file, "%u\n", -1);              /* "65535" or "4294967295" */
	f_printf(file, "%04x\n", 0xAB3);         /* "0ab3" */
	f_printf(file, "%08LX\n", 0x123ABCL);    /* "00123ABC" */
	f_printf(file, "%016b\n", 0x550F);       /* "0101010100001111" */
	f_printf(file, "%s\n", "String");        /* "String" */
	f_printf(file, "%8s\n", "abc");          /* "     abc" */
	f_printf(file, "%-8s\n", "abc");         /* "abc     " */
	f_printf(file, "%c\n", 'a');             /* "a" */

	ret = f_close(file);
	ASSERT( F_NO_ERROR == ret, "f_close pb: 0x%X\n\r", ret);

	f_releaseFS();
	fs_delete();

	hcc_mem_delete();

	DEMO_SD_TRACE_INFO("Success!\n\r");
}

static void _TaskTestFileLogWrite( void * pvParam )
{
	F_FILE *file;
	int ret;
	int bytesWritten;

	char filename[] = "log.bin";

	_MemSetTestPattern( pWriteBuff, _BUFF_SIZE );

	/*
	 * This is a new task which needs to be registered with filesystem before
	 * using any f_* based functions. This only needs to be done once per task.
	 */
	ret = f_enterFS();
	ASSERT( (ret == F_NO_ERROR ), "f_enterFS pb: %d\n\r", ret);

#if(_SAFE)
	file = f_open( filename, "a" );
	ASSERT( (file), "f_open pb: %d\n\r", f_getlasterror() );
#else
	file = f_open_nonsafe( filename, "a" );
	ASSERT( (file), "f_open_nonsafe pb: %d\n\r", f_getlasterror() );
#endif

	do
	{
		bytesWritten = f_write(pWriteBuff, 1, _BUFF_SIZE, file);

		f_flush(file);

		DEMO_SD_TRACE_INFO( "%d bytes of data written!\n\r", bytesWritten );

		vTaskDelay( (portTickType)1000 );
	}
	while( f_getlasterror() == F_NO_ERROR );

	ASSERT( ( f_getlasterror() == F_NO_ERROR ), "f_write pb: %d\n\r", f_getlasterror() );

	f_releaseFS( );

	DEMO_SD_TRACE_INFO("Done!\n\r");

	vTaskDelete(NULL);

	(void)pvParam;
}

static void _TaskTestFileLogRead( void * pvParam )
{
	F_FILE *file;
	int ret;
	int bytesRead;

	char filename[] = "log.bin"; /* a=0, b=1, etc */

	/*
	 * This is a new task which needs to be registered with filesystem before
	 * using any f_* based functions. This only needs to be done once per task.
	 */
	ret = f_enterFS();
	ASSERT( (ret == F_NO_ERROR ), "f_enterFS pb: %d\n\r", ret);

#if(_SAFE)
	file = f_open( filename, "r" );
	ASSERT( (file), "f_open pb: %d\n\r", f_getlasterror() );
#else
	file = f_open_nonsafe( filename, "r" );
	ASSERT( (file), "f_open_nonsafe pb: %d\n\r", f_getlasterror() );
#endif

	do
	{
		bytesRead = f_read( pReadBuff, 1, _BUFF_SIZE, file );

		DEMO_SD_TRACE_INFO( "%d bytes of data read!\n\r", bytesRead );

		vTaskDelay( (portTickType)1000 );
	}
	while( f_getlasterror() == F_NO_ERROR );

	ASSERT( ( f_getlasterror() == F_NO_ERROR ), "f_read pb: %d\n\r", f_getlasterror() );

	f_releaseFS();

	DEMO_SD_TRACE_INFO("Done!\n\r");

	vTaskDelete(NULL);

	(void)pvParam;
}

static void _MemSetTestPattern( unsigned char * pBuffer, unsigned int size )
{
	unsigned int i;

	for ( i = 0; i < size; i++ )
	{
		if ( (i & 1) == 0 ) /* even */
		{
			pBuffer[i] = ( i & 0x55 );
		}
		else /* odd */
		{
			pBuffer[i] = ( i & 0xAA );
		}
	}
}
