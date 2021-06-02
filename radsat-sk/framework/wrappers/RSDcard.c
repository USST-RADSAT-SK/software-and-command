/**
 * @file RSDcard.c
 * @date May 22, 2021
 * @author Addi Amaya (caa746) Julian Pham (ngp143)
 *
 */

#include <RSDcard.h>

/***************************************************************************************************
                                             PUBLIC API
***************************************************************************************************/

#if ENABLE_SD_TRACES
	#define SD_TRACE_INFO		TRACE_INFO
	#define SD_TRACE_DEBUG		TRACE_DEBUG
	#define SD_TRACE_WARNING	TRACE_WARNING
	#define SD_TRACE_ERROR		TRACE_ERROR
	#define SD_TRACE_FATAL		TRACE_FATAL
#else
	#define SD_TRACE_INFO(...)	{ }
	#define SD_TRACE_DEBUG(...)	{ }
	#define SD_TRACE_WARNING(...)	{ }
	#define SD_TRACE_ERROR		TRACE_ERROR
	#define SD_TRACE_FATAL		TRACE_FATAL
#endif


unsigned char pWriteBuff[ _BUFF_SIZE ];
unsigned char pReadBuff[ _BUFF_SIZE ];

/*
 * This function demonstrates the basics of using the filesystem on the
 * SD Card. It handles initialization and tear down of the file system, and
 * write and read operations. For a complete list of available filesystem
 * commands refer to the user guide.
 */

uint32_t SDinit( uint16_t volID) {
	uint16_t ret;

	if( volID != 0 && volID != 1 ) {
		return 0;
	}

	hcc_mem_init(); /* Initialize the memory to be used by filesystem */

	ret = fs_init(); /* Initialize the filesystem */
	ASSERT( (ret == F_NO_ERROR), "fs_init pb: %d\n\r", ret);
	ret = f_enterFS(); /* Register this task with filesystem */
	ASSERT( (ret == F_NO_ERROR), "f_enterFS pb: %d\n\r", ret);

	#if(_SAFE)
		ret = f_initvolume(0, atmel_mcipdc_initfunc, volID); /* Initialize volID as safe */
	#else
		ret = f_initvolume_nonsafe(0, atmel_mcipdc_initfunc, volID); /* Initialize volID as nonsafe */
	#endif
	ASSERT( ((ret == F_NO_ERROR) || (ret == F_ERR_NOTFORMATTED)), "f_initvolume pb: %d\n\r", ret);

	if(F_ERR_NOTFORMATTED == ret){
		ret = f_format( 0, F_FAT32_MEDIA ); /* Format the filesystem */
		ASSERT( (ret == F_NO_ERROR), "f_format pb: %d\n\r", ret);
	}

	SD_TRACE_INFO("SD Card (%d) initialization completed!\n\r", volID);

	return 1;

}

uint32_t SDstop( uint16_t volID ){

	f_delvolume(volID); /* delete the volID */

	f_releaseFS(); /* release this task from the filesystem */

	fs_delete(); /* delete the filesystem */

	hcc_mem_delete(); /* free the memory used by the filesystem */

	SD_TRACE_INFO("SD Card (%d) de-initialization completed!\n\r", volID);

	return 1;
}

uint32_t SDwrite(uint8_t* filename, uint16_t volID){
	uint16_t ret, i, bw;
	F_FILE* file;

#if(_SAFE)
	file = f_open(filename, "w"); /* open file for writing in safe mode */
#else
	file = f_open_nonsafe(filename, "w"); /* open file for writing in nonsafe mode */
#endif
	ASSERT( (file), "f_open pb: %d\n\r", f_getlasterror() ); /* if file pointer is NULL, get the error */

	for( i = 0; i < 4; i++ ){
		bw = f_write( pWriteBuff, 1, _BUFF_SIZE, file );
		ASSERT( ( _BUFF_SIZE == bw ),  "f_write pb: %d\n\r", f_getlasterror() ); /* if bytes to write doesn't equal bytes written, get the error */
		f_flush( file ); /* only after flushing can data be considered safe */
	}

	ret = f_close( file ); /* data is also considered safe when file is closed */
	ASSERT( (ret == F_NO_ERROR ), "f_close pb: %d\n\r", ret);

	SD_TRACE_INFO("SD Card (%d) write operation completed!\n\r", volID);

	return 1;

}

uint32_t SDread(uint8_t filename, uint16_t volID){
	uint16_t br;

	F_FILE* file = f_open(filename, "r"); /* open file for reading, which is always safe */
	ASSERT( ( file ), "f_open pb: %d\n\r", f_getlasterror() ); /* if file pointer is NULL, get the error */

	br = f_read(pReadBuff, 1, _BUFF_SIZE, file);
	ASSERT( (_BUFF_SIZE == br),  "f_read pb: %d\n\r", f_getlasterror() ); /* if bytes to read doesn't equal bytes read, get the error */

	f_close(file);

	SD_TRACE_INFO("SD Card (%d) read operation completed!\n\r", volID);


	return 1;
}

