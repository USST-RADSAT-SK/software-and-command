/*
 * ScsGeckoDemo.c
 *
 *  Created on: 29 jan. 2018
 *      Author: pbot
 */

#include <satellite-subsystems/SCS_Gecko/gecko_driver.h>
#include <satellite-subsystems/SCS_Gecko/gecko_use_cases.h>

#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/SPI.h>
#include <hal/Utility/util.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <hcc/api_fat.h>
#include <hcc/api_hcc_mem.h>
#include <hcc/api_fat_test.h>
#include <hcc/api_mdriver_atmel_mcipdc.h>

#include <stdint.h>

#define _IMG_NUM 1
#define _ADC_GAIN 53
#define _PGA_GAIN 3
#define _EXPOSURE 2048
#define _FRAME_AMOUNT 1
#define _FRAME_COUNT 1
#define _SD_CARD 0
#define _IMAGE_HEIGHT 1088
#define _IMAGE_WIDTH 2048
#define _IMAGE_BYTES (_IMAGE_HEIGHT*_IMAGE_WIDTH)
#define _SPI_GECKO_BUS_SPEED MHZ(5)

#define _IMAGE_HEADER 604
//Adding 100 extra bytes in case the gecko camera sends more bytes than expected
static uint8_t _imageBuffer[_IMAGE_BYTES+_IMAGE_HEADER+100];

Boolean Gecko_GetRegister( void )
{
	unsigned int reg = 0;
	printf("Register to read (0-14)\n\r");
	while(UTIL_DbguGetIntegerMinMax(&reg, 1, 14) == 0);

	unsigned int regVal;
	GECKO_GetReg(0, (uint8_t)reg,(uint32_t*)&regVal,GECKO_Endianness_big);

	printf("Register 0x%02X = 0x%08X\n\r",reg,regVal);

	return TRUE;
}

Boolean Gecko_SetRegister( void )
{
	unsigned int reg = 0;
	printf("Register to set (0-14)\n\r");
	while(UTIL_DbguGetIntegerMinMax(&reg, 0, 14) == 0);

	unsigned int regVal;
	printf("Register value to set\n\r0x");fflush(stdout);
	while(UTIL_DbguGetHexa32(&regVal) == 0);

	GECKO_SetReg(0, (uint8_t)reg,(uint32_t)regVal,GECKO_Endianness_big);
	printf("\n\rSet register 0x%02X = 0x%08X\n\r",reg,regVal);

	GECKO_GetReg(0, (uint8_t)reg,(uint32_t*)&regVal,GECKO_Endianness_big);
	printf("Register 0x%02X = 0x%08X\n\r",reg,regVal);

	return TRUE;
}

Boolean Gecko_DisplayVersion( void )
{
	uint8_t versionHW = GECKO_GetVersionHW(0);
	uint8_t versionSWMajor = GECKO_GetVersionSwMajor(0);
	uint8_t versionSWMinor = GECKO_GetVersionSwMinor(0);
	uint8_t versionSWBuild = GECKO_GetVersionSwBuild(0);

	printf("\n\r Version: %u.%u.%u.%u\n\r", versionHW, versionSWMajor, versionSWMinor, versionSWBuild);

	return TRUE;
}

Boolean Gecko_DisplayUptime( void )
{
	uint32_t uptime = GECKO_GetUpTime(0);

	printf("\n\r %lu\n\r", uptime);

	return TRUE;
}

Boolean Gecko_TakeImage( void )
{
	int err = GECKO_UC_TakeImage(0, _ADC_GAIN,_PGA_GAIN,_EXPOSURE,1,1,_IMG_NUM);
	if( err )
	{
		TRACE_ERROR("Error (%d) taking image!\n\r",err);
	}

	return TRUE;
}

Boolean Gecko_ReadImage( gecko_output_type imageType )
{
	int i = 0;
	for (i = 0; i < 1; i++)
	{
		uint32_t imgSize = 0;
		int err = GECKO_UC_ReadImage(0, _IMG_NUM,(uint32_t*)_imageBuffer, sizeof(_imageBuffer), &imgSize, imageType);
		if( err )
		{
			TRACE_ERROR("Error (%d) reading image!\n\r",err);
		}

		if (imgSize > 0)
		{
			F_FILE *file = NULL;
			F_FILE *allFile = NULL;
			char rawname[] = "gecko0.raw";
			char binname[] = "gecko0.bin";
			char j2kname[] = "gecko0.j2k";
			char j2bname[] = "gecko0.j2b";
			rawname[5] = '0' + i;
			binname[5] = '0' + i;
			j2kname[5] = '0' + i;
			j2bname[5] = '0' + i;

			if (imageType == gecko_output_raw
					|| imageType == gecko_output_binned)
			{
				printf("Now dealing with %s\n\r", rawname);
				file = f_open( rawname, "w" ); /* open file for writing in safe mode */
				allFile = f_open( binname, "w" ); /* open file for writing in safe mode */
			}
			else
			{
				printf("Now dealing with %s\n\r", j2kname);
				file = f_open( j2kname, "w" ); /* open file for writing in safe mode */
				allFile = f_open( j2bname, "w" ); /* open file for writing in safe mode */
			}

			if( !file )
			{
				TRACE_ERROR("f_open pb: %d\n\r", f_getlasterror() ); /* if file pointer is NULL, get the error */
			}

			int bw = f_write( _imageBuffer + _IMAGE_HEADER, 1, imgSize - _IMAGE_HEADER, file );
			if( (int)(imgSize - _IMAGE_HEADER) != bw )
			{
				TRACE_ERROR("f_write pb: %d\n\r", f_getlasterror() ); /* if bytes to write doesn't equal bytes written, get the error */
			}

			f_flush( file ); /* only after flushing can data be considered safe */

			bw = f_write( _imageBuffer, 1, imgSize, allFile );
			if( (int)imgSize != bw )
			{
				TRACE_ERROR("f_write pb: %d\n\r", f_getlasterror() ); /* if bytes to write doesn't equal bytes written, get the error */
			}
			f_flush(allFile);
			f_close(allFile);

			err = f_close( file ); /* data is also considered safe when file is closed */
			if( err )
			{
				TRACE_ERROR("f_close pb: %d\n\r", err);
			}
		}
		else
		{
			TRACE_WARNING("Image size is 0. No file was written!\n\r");
		}
	}

	return TRUE;
}

Boolean Gecko_ReadImageLVDS( gecko_output_type imageType )
{
	int err = GECKO_UC_ReadImageLVDS(0, _IMG_NUM, gecko_lvds_10MHz, imageType);
	if( err )
	{
		TRACE_ERROR("Error (%d) reading image!\n\r",err);
	}
	return TRUE;
}

Boolean Gecko_EraseImage( void )
{
	int err = GECKO_UC_EraseBlock(0, _IMG_NUM);
	if( err )
	{
		TRACE_ERROR("Error (%d) erasing image!\n\r",err);
	}

	return TRUE;
}

Boolean Gecko_PrintAllRegisters( void )
{
	unsigned int reg;
	for(reg=1;reg<=0x0E;reg++)
	{
		unsigned int regVal;
		GECKO_GetReg(0, (uint8_t)reg,(uint32_t*)&regVal,GECKO_Endianness_big);
		printf("Register 0x%02X = 0x%08X\n\r",reg,regVal);
	}

	for(reg=0x20;reg<=0x30;reg++)
	{
		unsigned int regVal;
		GECKO_GetReg(0, (uint8_t)reg,(uint32_t*)&regVal,GECKO_Endianness_big);
		printf("Register 0x%02X = 0x%08X\n\r",reg,regVal);
	}

	return TRUE;
}

Boolean Gecko_PrintTelemetry( void )
{
	GECKO_Telemetry_eng_t tlm;
	GECKO_GetTelemetry_Engineering(0, &tlm);

	printf("\nGENERAL TELEMETRY\n\r");
	printf("Uptime:        %d\n\r", (unsigned int)tlm.uptime);
	printf("\nCURRENTS & VOLTAGES\n\r");
	printf("Input 5V:      %.3fA @ %.3fV\n\r", tlm.input5v_current, tlm.input5v_voltage );
	printf("FPGA 1V0:      %.3fA @ %.3fV\n\r", tlm.fpga1v_current, tlm.fpga1v_voltage );
	printf("FPGA 1V8:      %.3fA @ %.3fV\n\r", tlm.fpga1v8_current, tlm.fpga1v8_voltage );
	printf("FPGA 2V5:      %.3fA @ %.3fV\n\r", tlm.fpga2v5_current, tlm.fpga2v5_voltage );
	printf("FPGA 3V3:      %.3fA @ %.3fV\n\r", tlm.fpga3v3_current, tlm.fpga3v3_voltage );
	printf("Flash 1V8:     %.3fA @ %.3fV\n\r", tlm.flash1v8_current, tlm.flash1v8_voltage );
	printf("Flash 3V3:     %.3fA @ %.3fV\n\r", tlm.flash3v3_current, tlm.flash3v3_voltage );
	printf("Sensor 1V8:    %.3fA @ %.3fV\n\r", tlm.sensor1v8_current, tlm.sensor1v8_voltage );
	printf("Sensor Pix:    %.3fA @ %.3fV\n\r", tlm.sensor_pix_current, tlm.sensor_pix_voltage );
	printf("Sensor 3V3:    %.3fA @ %.3fV\n\r", tlm.sensor3v3_current, tlm.sensor3v3_voltage );
	printf("Flash VTT09:   %.3fV\n\r", tlm.flash_vtt09);

	printf("\nTEMPERATURES:\n\r");
	printf("SM U3 AB:      %.3f\n\r", tlm.temp_sm_u3_ab);
	printf("SM U3 BC:      %.3f\n\r", tlm.temp_sm_u3_bc);
	printf("REG U6:        %.3f\n\r", tlm.temp_reg_u6);
	printf("REG U8:        %.3f\n\r", tlm.temp_reg_u8);
	printf("FLASH:         %.3f\n\r", tlm.temp_flash);
	printf("FPGA:          %.3f\n\r", tlm.temp_fpga);
	printf("Sensor (raw):  %.3f\n\r", tlm.temp_sensor);

	//The extended telemetry is not available with engineering values,
	// so we need to grab the telemetry again
	GECKO_UC_Telemetry_t extTlm;
	GECKO_UC_GetTelemetry(0, &extTlm);
	printf("\nEXTENDED TELEMETRY:\n\r");
	printf("Status:        0x%08X\n\r", (unsigned int)extTlm.state);
	printf("Last SPI read: %d bytes\n\r", (unsigned int)extTlm.spi_bytes_read);

	return TRUE;
}

static uint8_t _GetBinnedPixel(uint8_t binPixelSize, int x, int y)
{
	uint16_t value=0;
	int i,j;
	for(j=0;j<binPixelSize;j++)
		for(i=0;i<binPixelSize;i++)
			value += _imageBuffer[(y+j*2)*_IMAGE_WIDTH + (x+i*2)]; // need to jump 2 because of bayer pattern
	return (uint8_t)(value/(binPixelSize*binPixelSize));
}

Boolean Gecko_BinImage( void )
{
	unsigned int binLevel = 0;
	printf("Select bin level (1-3), where 2^(bin level) is the size reduction of the image.\n\r");
	while(UTIL_DbguGetIntegerMinMax(&binLevel, 1, 3) == 0);

	/* read data */
	F_FILE *file = f_open( "gecko.raw", "r" ); /* open file for writing in safe mode */
	if( !file )
	{
		TRACE_ERROR("f_open pb: %d\n\r", f_getlasterror() ); /* if file pointer is NULL, get the error */
	}

	unsigned int br = f_read( _imageBuffer, 1, sizeof(_imageBuffer), file );
	if( sizeof(_imageBuffer) != br )
	{
		TRACE_ERROR("f_read pb: %d\n\r", f_getlasterror() ); /* if bytes to write doesn't equal bytes written, get the error */
	}

	int err = f_close( file ); /* data is also considered safe when file is closed */
	if( err )
	{
		TRACE_ERROR("f_close pb: %d\n\r", err);
	}

	/* bin data */
	uint8_t binPixelSize = (1<<binLevel);
	uint8_t (*binBuffer)[_IMAGE_HEIGHT/binPixelSize][_IMAGE_WIDTH/binPixelSize] = (uint8_t(*)[][_IMAGE_WIDTH/binPixelSize])_imageBuffer;

	unsigned int x,y;
	for(y=0;y<_IMAGE_HEIGHT/binPixelSize;y+=2)
	{
		for(x=0;x<_IMAGE_WIDTH/binPixelSize;x+=2)
		{
			(*binBuffer)[y][x] = _GetBinnedPixel(binPixelSize,x*binPixelSize,y*binPixelSize);         // Red pixel
			(*binBuffer)[y][x+1] = _GetBinnedPixel(binPixelSize,x*binPixelSize+1,y*binPixelSize);     // Green pixel
			(*binBuffer)[y+1][x] = _GetBinnedPixel(binPixelSize,x*binPixelSize,y*binPixelSize+1);     // Green pixel
			(*binBuffer)[y+1][x+1] = _GetBinnedPixel(binPixelSize,x*binPixelSize+1,y*binPixelSize+1); // Blue
		}
	}

	/* save data */
	char filename[13] = {0};
	sprintf(filename,"bin%d.raw",binLevel);

	file = f_open( filename, "w" ); /* open file for writing in safe mode */
	if( !file )
	{
		TRACE_ERROR("f_open pb: %d\n\r", f_getlasterror() ); /* if file pointer is NULL, get the error */
	}

	unsigned int bw = f_write( _imageBuffer, 1,sizeof(*binBuffer), file );
	if(sizeof(*binBuffer) != bw )
	{
		TRACE_ERROR("f_write pb: %d\n\r", f_getlasterror() ); /* if bytes to write doesn't equal bytes written, get the error */
	}

	f_flush( file ); /* only after flushing can data be considered safe */

	err = f_close( file ); /* data is also considered safe when file is closed */
	if( err )
	{
		TRACE_ERROR("f_close pb: %d\n\r", err);
	}

	return TRUE;
}

Boolean selectAndExecuteGeckoDemoTest( void )
{
	unsigned int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf("\n\r Select a test to perform: \n\r");

	printf("\t 1) Display Version \n\r");
	printf("\t 2) Display Uptime \n\r");
	printf("\t 3) Print telemetry \n\r");
	printf("\t 4) Erase image \n\r");
	printf("\t 5) Take image \n\r");
	printf("\t 6) Read image raw (SPI) \n\r");
	printf("\t 7) Read image jpeg x2 (SPI) \n\r");
	printf("\t 8) Read image raw (LVDS) \n\r");
	printf("\t 9) Read image jpeg x2 (LVDS) \n\r");
	printf("\t 10) Get Register \n\r");
	printf("\t 11) Set Register \n\r");
	printf("\t 12) Print all registers \n\r");
	printf("\t 13) Bin saved image \n\r");
	printf("\t 14) Return to main menu \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 14) == 0);

	switch (selection)
	{
	case 1:
		offerMoreTests = Gecko_DisplayVersion();
		break;
	case 2:
		offerMoreTests = Gecko_DisplayUptime();
		break;
	case 3:
		offerMoreTests = Gecko_PrintTelemetry();
		break;
	case 4:
		offerMoreTests = Gecko_EraseImage();
		break;
	case 5:
		offerMoreTests = Gecko_TakeImage();
		break;
	case 6:
		offerMoreTests = Gecko_ReadImage(gecko_output_raw);
		break;
	case 7:
		offerMoreTests = Gecko_ReadImage(gecko_output_jpeg_2x);
		break;
	case 8:
		offerMoreTests = Gecko_ReadImageLVDS(gecko_output_raw);
		break;
	case 9:
		offerMoreTests = Gecko_ReadImageLVDS(gecko_output_jpeg_2x);
		break;
	case 10:
		offerMoreTests = Gecko_GetRegister();
		break;
	case 11:
		offerMoreTests = Gecko_SetRegister();
		break;
	case 12:
		offerMoreTests = Gecko_PrintAllRegisters();
		break;
	case 13:
		offerMoreTests = Gecko_BinImage();
		break;
	case 14:
		offerMoreTests = FALSE;
		break;

	default:
		break;
	}

	return offerMoreTests;
}

int _InitFileSystem( void )
{
	int ret;

	hcc_mem_init(); /* Initialize the memory to be used by filesystem */

	ret = fs_init(); /* Initialize the filesystem */
	if(ret != F_NO_ERROR )
	{
		TRACE_ERROR("fs_init pb: %d\n\r", ret);
		return -1;
	}

	ret = f_enterFS(); /* Register this task with filesystem */
	if(ret != F_NO_ERROR )
	{
		TRACE_ERROR("f_enterFS pb: %d\n\r", ret);
		return -1;
	}

	ret = f_initvolume( 0, atmel_mcipdc_initfunc, _SD_CARD ); /* Initialize volID as safe */

	if( F_ERR_NOTFORMATTED == ret )
	{
		TRACE_ERROR("Filesystem not formated!\n\r");
		return -1;
	}
	else if( F_NO_ERROR != ret)
	{
		TRACE_ERROR("f_initvolume pb: %d\n\r", ret);
		return -1;
	}

	return 0;
}

void _DeinitFileSystem( void )
{
	f_delvolume( _SD_CARD ); /* delete the volID */
	f_releaseFS(); /* release this task from the filesystem */

	fs_delete(); /* delete the filesystem */

	hcc_mem_delete(); /* free the memory used by the filesystem */
}

Boolean ScsGeckoDemoInit(void)
{
	if( _InitFileSystem() )
	{
		TRACE_ERROR("Error initializing the filesystem!\n\r");
		return FALSE;
	}

	const SPIslaveParameters params = { bus1_spi, mode0_spi, slave1_spi, 5, 0, _SPI_GECKO_BUS_SPEED, 0 };
	if( GECKO_UC_Init( &params, 1 ) )
	{
		TRACE_ERROR("Error initializing the gecko camera!\n\r");
		return FALSE;
	}

	return TRUE;
}

void ScsGeckoDemoLoop(void)
{
	Boolean offerMoreTests = FALSE;

	while (1)
	{
		offerMoreTests = selectAndExecuteGeckoDemoTest();

		if (offerMoreTests == FALSE)
		{
			break;
		}
	}
}

Boolean ScsGeckoDemoMain(void)
{
	if(ScsGeckoDemoInit())
	{
		ScsGeckoDemoLoop();

		_DeinitFileSystem();

		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

Boolean GeckoTest( void )
{
	ScsGeckoDemoMain();
	return TRUE;
}


