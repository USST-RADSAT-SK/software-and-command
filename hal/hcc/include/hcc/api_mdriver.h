/****************************************************************************
 *
 *            Copyright (c) 2003-2013 by HCC Embedded
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
#ifndef _API_MDRIVER_H_
#define _API_MDRIVER_H_

#include "../version/ver_mdriver.h"
#if VER_MDRIVER_MAJOR != 1 || VER_MDRIVER_MINOR != 1
 #error Incompatible MDRIVER version number!
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
  unsigned short  number_of_cylinders;
  unsigned short  sector_per_track;
  unsigned short  number_of_heads;
  unsigned long   number_of_sectors;
  unsigned char   media_descriptor;

  unsigned short  bytes_per_sector;
} F_PHY;

/* media descriptor to be set in getphy function */
#define F_MEDIADESC_REMOVABLE  0xf0
#define F_MEDIADESC_FIX        0xf8

/* return bitpattern for driver getphy function */
#define F_ST_MISSING           0x00000001
#define F_ST_CHANGED           0x00000002
#define F_ST_WRPROTECT         0x00000004

/* Mask for getting mdriver init parameter without modifying flags */
#define MDRIVER_PARAM_MASK     ( 0x00FFFFFFu )

/* Read Only mask for the mdriver init function */
#define MDRIVER_FLAG_WRPROTECT ( 1u << 24 )

enum
{
  F_IOCTL_MSG_ENDOFDELETE
  , F_IOCTL_MSG_MULTIPLESECTORERASE
};

typedef struct
{
  void         * one_sector_databuffer;
  unsigned long  start_sector;
  unsigned long  sector_num;
} ST_IOCTL_MULTIPLESECTORERASE;

/* Driver definitions */
typedef struct F_DRIVER  F_DRIVER;

typedef int          ( * F_WRITESECTOR )( F_DRIVER * driver, void * data, unsigned long sector );
typedef int          ( * F_WRITEMULTIPLESECTOR )( F_DRIVER * driver, void * data, unsigned long sector, int cnt );
typedef int          ( * F_READSECTOR )( F_DRIVER * driver, void * data, unsigned long sector );
typedef int          ( * F_READMULTIPLESECTOR )( F_DRIVER * driver, void * data, unsigned long sector, int cnt );
typedef int          ( * F_GETPHY )( F_DRIVER * driver, F_PHY * phy );
typedef long         ( * F_GETSTATUS )( F_DRIVER * driver );
typedef void         ( * F_RELEASE )( F_DRIVER * driver );
typedef int          ( * F_IOCTL )( F_DRIVER * driver, unsigned long msg, void * iparam, void * oparam );

typedef struct F_DRIVER
{
  int            separated;     /* signal if the driver is separated */

  unsigned long  user_data;     /* user defined data */
  void         * user_ptr;      /* user define pointer */

  /* driver functions */
  F_WRITESECTOR          writesector;
  F_WRITEMULTIPLESECTOR  writemultiplesector;
  F_READSECTOR           readsector;
  F_READMULTIPLESECTOR   readmultiplesector;
  F_GETPHY               getphy;
  F_GETSTATUS            getstatus;
  F_RELEASE              release;
  F_IOCTL                ioctl;
} _F_DRIVER;

typedef F_DRIVER *( * F_DRIVERINIT )( unsigned long driver_param );

/* When initvolume the driver will assign automatically a free driver */
#define F_AUTO_ASSIGN (unsigned long)( MDRIVER_PARAM_MASK )

#ifdef __cplusplus
}
#endif

#endif /* _API_MDRIVER_H_ */


