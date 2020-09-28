/***************************************************************************
 *
 *            Copyright (c) 2003-2014 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci ut 76
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#ifndef _API_FAT_TEST_H
#define _API_FAT_TEST_H

#include <stdint.h>

#include "../version/ver_fat.h"
#if ( VER_FAT_MAJOR != 8 )
 #error Incompatible FAT version number!
#endif

#include "../version/ver_fat_test.h"
#if ( ( VER_FAT_TEST_MAJOR != 2 ) || ( VER_FAT_TEST_MINOR != 1 ) )
 #error "VER_FAT_TEST_MAJOR invalid"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void f_dotest ( uint32_t vol_id );

#ifdef __cplusplus
}
#endif

#endif /* _API_FAT_TEST_H*/

