/***************************************************************************
 *
 *            Copyright (c) 2010-2014 by HCC Embedded
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
#ifndef _API_HCC_MEM_H
#define _API_HCC_MEM_H

#include "../psp/include/psp_types.h"

#include "../version/ver_hcc_mem.h"
#if VER_HCC_MEM_MAJOR != 1 || VER_HCC_MEM_MINOR != 21
 #error Incompatible HCC_MEM version number!
#endif
#include "../version/ver_oal.h"
#if VER_OAL_MAJOR != 2
 #error Incompatible OAL version number!
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum
{
  HCC_MEM_SUCCESS = 0
  , HCC_MEM_ERROR
};

int hcc_mem_init ( void );
int hcc_mem_start ( void );
int hcc_mem_stop ( void );
int hcc_mem_delete ( void );

#ifdef __cplusplus
}
#endif

#endif /* ifndef _API_HCC_MEM_H */

