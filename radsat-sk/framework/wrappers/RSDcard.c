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



/**
 * Function to initialize SD card. It will initialize the File System then initialize the chosen SD card
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
		ret = f_initvolume(volID, atmel_mcipdc_initfunc, volID);

	#else
		//Initialize volID as nonsafe
		ret = f_initvolume_nonsafe(volID, atmel_mcipdc_initfunc, volID);

	#endif
		ASSERT( ((ret == F_NO_ERROR) || (ret == F_ERR_NOTFORMATTED)), "f_initvolume pb: %d\n\r", ret);

	if(F_ERR_NOTFORMATTED == ret){
		//Format the filesystem
		ret = f_format(volID, F_FAT32_MEDIA);
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
 * @param filename: the name of the file to be written
 * @param voldID: the ID of the SD card
 * @param pWriteBuff: the pointer to data to be written to the file
 * @param buff_size: the size of the data
 * @param numItems: number of items to be written
 * @return 1 if write is successful
 */
uint32_t SDwrite(uint8_t* filename, uint16_t volID, uint8_t* pWriteBuff, uint32_t buff_size, uint16_t numItems){
	uint16_t ret, bw;
	F_FILE* file;


	#if(_SAFE)
		//open file for writing in safe mode
		file = f_open(filename, "w");

	#else
		//open file for writing in nonsafe modes
		file = f_open_nonsafe(filename, "w");

	#endif
		//if file pointer is NULL, get the error
		ASSERT( (file), "f_open pb: %d\n\r", f_getlasterror() );


	bw = f_write(pWriteBuff, buff_size, numItems, file);

	// if bytes to write doesn't equal bytes written, get the error
	ASSERT( ( numItems == bw ),  "f_write pb: %d\n\r", f_getlasterror() );

	//data is also considered safe when file is closed
	ret = f_close( file );
	ASSERT( (ret == F_NO_ERROR ), "f_close pb: %d\n\r", ret);

	SD_TRACE_INFO("SD Card (%d) write operation completed!\n\r", volID);

	return 1;
}

/**
 * Function to read from the SD card
 * @param filename: the name of the file to be read
 * @param volID: the ID of the SD card
 * @param pReadBuff: the buffer to store data
 * @param numItems: number of items to be stored
 * @return 1 if it is successful
 */
uint32_t SDread(uint8_t* filename, uint16_t volID, uint8_t* pReadBuff, uint32_t numItems){
	uint16_t br;
	uint32_t buff_size;

	//open file for reading, which is always safe
	F_FILE* file = f_open(filename, "r");

	//if file pointer is NULL, get the error
	ASSERT( ( file ), "f_open pb: %d\n\r", f_getlasterror() );

	//get the size of items to be read
	buff_size = f_filelength(filename);

	br = f_read(pReadBuff, buff_size, numItems, file);

	//if bytes to read doesn't equal bytes read, get the error
	ASSERT( (_BUFF_SIZE == br),  "f_read pb: %d\n\r", f_getlasterror() );

	f_close(file);

	SD_TRACE_INFO("SD Card (%d) read operation completed!\n\r", volID);

	return 1;
}

