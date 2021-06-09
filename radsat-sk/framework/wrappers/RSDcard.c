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

/**
 * Function to initialize SD card. It will initilize the File System then initialize the chosen SD card
 * @pre volID is either 0 or 1
 * @param int volID: The ID of the SD card
 * @return 1 if initialization is successful, 0 otherwise
 */

uint32_t SDinit( uint16_t volID) {
	uint16_t ret;

	if( volID != 0 && volID != 1 ) {
		return 0;
	}

	//Initialize the memory to be used by filesystem
	hcc_mem_init();

	//Initialize the filesystem
	ret = fs_init();
	ASSERT( (ret == F_NO_ERROR), "fs_init pb: %d\n\r", ret);

	//Register this task with filesystem
	ret = f_enterFS();
	ASSERT( (ret == F_NO_ERROR), "f_enterFS pb: %d\n\r", ret);


	#if(_SAFE)
		//Initialize volID as safe
		ret = f_initvolume(0, atmel_mcipdc_initfunc, volID);

	#else
		//Initialize volID as nonsafe
		ret = f_initvolume_nonsafe(0, atmel_mcipdc_initfunc, volID);

	#endif
		ASSERT( ((ret == F_NO_ERROR) || (ret == F_ERR_NOTFORMATTED)), "f_initvolume pb: %d\n\r", ret);

	if(F_ERR_NOTFORMATTED == ret){
		//Format the filesystem
		ret = f_format( 0, F_FAT32_MEDIA );
		ASSERT( (ret == F_NO_ERROR), "f_format pb: %d\n\r", ret);
	}

	SD_TRACE_INFO("SD Card (%d) initialization completed!\n\r", volID);

	return 1;

}


/**
 * Function to de-initialize SD card. It will delete the File System
 * @pre volID is either 0 or 1
 * @param volID: The ID of the SD card
 * @return 1 if de-initialization is successful
 */
uint32_t SDstop( uint16_t volID ){

	//delete the volID
	f_delvolume(volID);

	//release this task from the filesystem
	f_releaseFS();

	//delete the filesystem
	fs_delete();

	//free the memory used by the filesystem
	hcc_mem_delete();

	SD_TRACE_INFO("SD Card (%d) de-initialization completed!\n\r", volID);

	return 1;
}

/**
 * Function to write to the SD card,
 * @post Data is written to the given file name
 * @param char* filename: the name of the file to be written
 * @return 1 if write is succesful
 */
uint32_t SDwrite(uint8_t* filename, uint16_t volID){
	uint16_t ret, i, bw;
	F_FILE* file;


	#if(_SAFE)
		//open file for writing in safe mode
		file = f_open(filename, "w");

	#else
		//open file for writing in nonsafe mode
		file = f_open_nonsafe(filename, "w");

	#endif
		//if file pointer is NULL, get the error
		ASSERT( (file), "f_open pb: %d\n\r", f_getlasterror() );

	for( i = 0; i < 4; i++ ){
		bw = f_write( pWriteBuff, 1, _BUFF_SIZE, file );

		// if bytes to write doesn't equal bytes written, get the error
		ASSERT( ( _BUFF_SIZE == bw ),  "f_write pb: %d\n\r", f_getlasterror() );

		// only after flushing can data be considered safe
		f_flush( file );
	}

	//data is also considered safe when file is closed
	ret = f_close( file );
	ASSERT( (ret == F_NO_ERROR ), "f_close pb: %d\n\r", ret);

	SD_TRACE_INFO("SD Card (%d) write operation completed!\n\r", volID);

	return 1;
}

/**
 * Function to read from the SD card
 * @param char* filename: the name of the file to be read
 * @return 1 if it is succesfull
 */
uint32_t SDread(uint8_t filename, uint16_t volID){
	uint16_t br;

	//open file for reading, which is always safe
	F_FILE* file = f_open(filename, "r");

	//if file pointer is NULL, get the error
	ASSERT( ( file ), "f_open pb: %d\n\r", f_getlasterror() );

	br = f_read(pReadBuff, 1, _BUFF_SIZE, file);

	//if bytes to read doesn't equal bytes read, get the error
	ASSERT( (_BUFF_SIZE == br),  "f_read pb: %d\n\r", f_getlasterror() );

	f_close(file);

	SD_TRACE_INFO("SD Card (%d) read operation completed!\n\r", volID);

	return 1;
}

