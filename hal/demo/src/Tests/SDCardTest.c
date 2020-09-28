/*
 * SDCardTest.c
 *
 *  Created on: 17 jul. 2014
 *      Author: pbot
 */

#include "SDCardTest.h"

#include <hcc/api_hcc_mem.h>
#include <hcc/api_fat.h>
#include <hcc/api_fat_test.h>

#include <hal/boolean.h>
#include <hal/Utility/util.h>

#include <stdio.h>

Boolean SDCardTest( void )
{
	int err;
	unsigned int sdcard;

	printf("\nPlease select a the SD Card you would like to test (0 or 1)?: \n");
	while( UTIL_DbguGetIntegerMinMax( &sdcard, 0, 1 ) == 0 );

	err = hcc_mem_init ( );
	if( err )
	{
		return FALSE;
	}

	err = fs_init();
	if( err )
	{
		return FALSE;
	}

	err = f_enterFS();
	if( err )
	{
		return FALSE;
	}

	f_dotest( sdcard );

	f_releaseFS();
	fs_delete();

	hcc_mem_delete();

	return TRUE;
}
