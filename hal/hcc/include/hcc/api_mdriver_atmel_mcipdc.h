/****************************************************************************
 *
 *            Copyright (c) 2007-2011 by HCC Embedded
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
 * Vaci Ut 76
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#ifndef _API_MDRIVER_ATMEL_MCIPDC_H
#define _API_MDRIVER_ATMEL_MCIPDC_H

#include "api_mdriver.h"

#include "../version/ver_mdriver_atmel_mcipdc.h"
#if VER_MDRIVER_ATMEL_MCIPDC_MAJOR != 1 || VER_MDRIVER_ATMEL_MCIPDC_MINOR != 4
 #error Incompatible MDRIVER_RAM version number!
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern F_DRIVER * atmel_mcipdc_initfunc ( unsigned long driver_param );

#define MMCSD_ERR_NOTPLUGGED -1 /* for high level */

enum
{
  MMC_NO_ERROR
  , MMC_ERR_NOTINITIALIZED = 101
  , MMC_ERR_NOTPLUGGED
  , MMC_ERR_INIT
  , MMC_ERR_CMD
  , MMC_ERR_WRITEPROTECT
  , MMC_ERR_TRANS
};


#ifdef __cplusplus
}
#endif

#endif /* _API_MDRIVER_ATMEL_MCIPDC_H */

