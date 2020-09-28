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
#ifndef _CONFIG_FAT_H_
#define _CONFIG_FAT_H_

#include "../version/ver_fat.h"
#if VER_FAT_MAJOR != 8
 #error "VER_FAT_MAJOR invalid"
#endif
#if VER_FAT_MINOR != 6
 #error "VER_FAT_MINOR invalid"
#endif


/****************************************************************************
 *
 * enable this if CAPI (Common API) is used
 *
 ***************************************************************************/
#define FN_CAPI_USED 0


/****************************************************************************
 *
 * OEM name
 *
 ***************************************************************************/
#define OEM_NAME     "MSDOS5.0"

/*#define OEM_NAME "EFFSFAT"*/


/****************************************************************************
 *
 *  Set F_LONGFILENAME to 1 to enable long filenames.
 *  Set it to 0 to support 8+3 format filenames only.
 *
 ***************************************************************************/
#define F_LONGFILENAME 0


/* definitions for short filenames */
#define F_MAXSNAME     8            /*  8 byte name  */
#define F_MAXSEXT      3            /*  3 byte extension  */


/****************************************************************************
 *
 *  Constants when not using CAPI
 *
 ***************************************************************************/
#if !FN_CAPI_USED


/****************************************************************************
 *
 *  Set HCC_UNICODE to 1 to enable Unicode support
 *  Unicode support will enable long filenames as well.
 *
 ***************************************************************************/
 #define HCC_UNICODE 0


/****************************************************************************
 *
 *  Wide-character functions use arguments and variables of type 'wchar' which
 *  can be redefined here.
 *
 ***************************************************************************/
 #ifdef __cplusplus
extern "C" {
 #endif

 #if HCC_UNICODE
typedef unsigned short  wchar;
 #endif

 #ifdef __cplusplus
}
 #endif


/****************************************************************************
 *
 *  Set USE_MALLOC to 0 when building FAT from sources and linking
 * to the application. All variables will be allocated statically during
 * compile time.
 *  Set USE_MALLOC to 1 to allow FAT to use psp_malloc() to allocate
 * memory for variables and cache. USE_MALLOC will also allow setting
 * max_volumes, max_files and max_tasks run-time which makes it possible
 * to build FAT as a separate library. In this case the application will
 * need to be compiled with this very same CONFIG file.
 *
 ***************************************************************************/
 #define USE_MALLOC        0


/****************************************************************************
 *
 * volumes definitions
 *
 ***************************************************************************/
 #define FAT_MAXVOLUME     2        /* maximum number of volumes */
 #define FAT_MAXTASK       16        /* maximum number of tasks */

 #if !USE_MALLOC
  #define FN_MAXVOLUME     FAT_MAXVOLUME
  #define FN_MAXTASK       FAT_MAXTASK
 #endif

 #define FN_MAXPATHNAME    256      /* maximum length of short filename with full path */

 #define FN_CURRDRIVE      0 /* setting the current drive at startup (-1 means no default current drive)*/

 #define F_PATH_SEPARATOR  '/'    /* set this to '\\' for FAT to use backslash as the pathname separator character */

 #define F_DRIVE_SEPARATOR ':'    /* drive name separator character in full pathnames */


/****************************************************************************
 *
 * Set F_FILE_CHANGED_EVENT to 1 if want event about file state changed
 *
 ***************************************************************************/

 #define F_FILE_CHANGED_EVENT 0


/****************************************************************************
 *
 * set F_SUPPORT_TI64K to 1 when TI DSP is used.
 *
 ***************************************************************************/

 #define F_SUPPORT_TI64K 0


/****************************************************************************
 *
 * set USE_TASK_SEPARATED_CWD to 1 (default) if every task need separated
 * current working folder.
 *
 ***************************************************************************/

 #define USE_TASK_SEPARATED_CWD 1


/****************************************************************************
 *
 * Close bracket for non CAPI
 *
 ***************************************************************************/

#else /* #if FN_CAPI_USED */


/****************************************************************************
 * Don't change this line - USE_MALLOC isn't supported with CAPI
 ***************************************************************************/
 #define USE_MALLOC    0

 #include "config_capi.h"

 #define FAT_MAXVOLUME FN_MAXVOLUME

#endif /* #elif FN_CAPI_USED */


/****************************************************************************
 *
 * Common defines (for non CAPI and CAPI)
 *
 ***************************************************************************/

#if F_LONGFILENAME || HCC_UNICODE
 #define FN_MAXLNAME 255                                       /* maximum length of long filename */
 #define FN_MAXNAME  FN_MAXLNAME                               /* maximum length of long filename */
#else
 #define FN_MAXNAME  ( F_MAXSNAME + F_MAXSEXT + 2 )            /* maximum length of short filename */
#endif

#if FN_MAXPATHNAME < FN_MAXNAME
 #error "FN_MAXPATHNAME mustn't be less than FN_MAXNAME"
#endif


#if !USE_MALLOC
 #define F_MAXFILES        20    /* maximum number of files */
#endif

#define F_MAXSEEKPOS       8    /* number of division of fast seeking */


/* F_DEF_SECTOR_SIZE is the default sector size which is always 512 */
#define F_DEF_SECTOR_SIZE  512

/* maximum supported sector size */
#define F_MAX_SECTOR_SIZE  2048


/* Enable FAT caching */
#define FATCACHE_ENABLE    1

/* Enable directory caching - effective only when F_LONGFILENAME is set also */
#define DIRCACHE_ENABLE    1

/* define of allocation of faster searching mechanism on big FAT32 volumes */
#define FATBITFIELD_ENABLE 0
#if FATBITFIELD_ENABLE && !USE_MALLOC
 #error "FATBITFIELD_ENABLE requires USE_MALLOC"
#endif


#define WR_DATACACHE_SIZE 64 /* min. 1 !!!! */


/****************************************************************************
 *
 * Last access date
 *
 ***************************************************************************/

#define F_UPDATELASTACCESSDATE 0

/* it defines if a file is opened for read to update lastaccess time */


/****************************************************************************
 *
 * Opened file size
 *
 ***************************************************************************/

#define F_FINDOPENFILESIZE 0

/* set F_FINDOPENFILESIZE to 0 if filelength needs to return with 0 for an opened file  */
/* other case filelength functions can return with opened file length also */


/****************************************************************************
 *
 * if Safe FAT is used
 *
 ***************************************************************************/

#define SAFEFAT 1


/****************************************************************************
 *
 * if Non-Safe (normal) FAT is used
 *
 ***************************************************************************/

#define NONSAFEFAT 1


/****************************************************************************
 *
 * F_DELETE_CONTENT can be set to 1 when f_delete_content function is needed
 * to be used. This function delete file content from the disk (depending
 * on media type).
 *
 ***************************************************************************/

#define F_DELETE_CONTENT 0


/****************************************************************************
 *
 * Set F_VOLNAME_SUPPORT to enable f_setvolname() and f_getvolname() calls
 * and the use of named volumes.
 *
 ***************************************************************************/

#define F_VOLNAME_SUPPORT 0


/****************************************************************************
 *
 * Set F_SHIFT_JIS_SUPPORT to enable SHIFT_JIS character encoding in file
 * and directory names
 *
 ***************************************************************************/

#define F_SHIFT_JIS_SUPPORT 0

#if F_SHIFT_JIS_SUPPORT && F_LONGFILENAME && !HCC_UNICODE
 #error "HCC_UNICODE is required with F_SHIFT_JIS_SUPPORT and F_LONGFILENAME"
#endif


/****************************************************************************
 *
 *  Volume-dependent configuration templates for up to 4 volumes based on the
 * actual value of FAT_MAXVOLUME.
 *  If more volumes would be required the user is advised to add more volume-
 * dependent settings based on the existing templates.
 ***************************************************************************/

/* Volume #1 is always present */
#define F_SZ_MAX_SECTOR1 512
#if F_MAX_SECTOR_SIZE < F_SZ_MAX_SECTOR1
 #error "F_MAX_SECTOR_SIZE < F_SZ_MAX_SECTOR1"
#endif
#if FATCACHE_ENABLE
 #define F_N_FATCACHE_BLOCKS1    4
 #define F_N_FATCACHE_READAHEAD1 8    /* max. 256 depending on F_MAX_SECTOR_SIZE */
#endif
#if DIRCACHE_ENABLE
 #define F_N_DIRCACHE_SECTORS1   8    /* max. 32 (<=max. cluster size) */
#endif

#if FAT_MAXVOLUME >= 2

/* Volume #2 */
 #define F_SZ_MAX_SECTOR2 2048
 #if F_MAX_SECTOR_SIZE < F_SZ_MAX_SECTOR2
  #error "F_MAX_SECTOR_SIZE < F_SZ_MAX_SECTOR2"
 #endif
 #if FATCACHE_ENABLE
  #define F_N_FATCACHE_BLOCKS2    4
  #define F_N_FATCACHE_READAHEAD2 8
 #endif
 #if DIRCACHE_ENABLE
  #define F_N_DIRCACHE_SECTORS2   8
 #endif
#endif /* #if FAT_MAXVOLUME >= 2 */

#if FAT_MAXVOLUME >= 3

/* Volume #3 */
 #define F_SZ_MAX_SECTOR3 2048
 #if F_MAX_SECTOR_SIZE < F_SZ_MAX_SECTOR3
  #error "F_MAX_SECTOR_SIZE < F_SZ_MAX_SECTOR3"
 #endif
 #if FATCACHE_ENABLE
  #define F_N_FATCACHE_BLOCKS3    4
  #define F_N_FATCACHE_READAHEAD3 8
 #endif
 #if DIRCACHE_ENABLE
  #define F_N_DIRCACHE_SECTORS3   8
 #endif
#endif /* #if FAT_MAXVOLUME >= 3 */

#if FAT_MAXVOLUME >= 4

/* Volume #4 */
 #define F_SZ_MAX_SECTOR4 512
 #if F_MAX_SECTOR_SIZE < F_SZ_MAX_SECTOR4
  #error "F_MAX_SECTOR_SIZE < F_SZ_MAX_SECTOR4"
 #endif
 #if FATCACHE_ENABLE
  #define F_N_FATCACHE_BLOCKS4    1
  #define F_N_FATCACHE_READAHEAD4 1
 #endif
 #if DIRCACHE_ENABLE
  #define F_N_DIRCACHE_SECTORS4   1
 #endif
#endif /* #if FAT_MAXVOLUME >= 4 */


/****************************************************************************
 *
 * end of config_fat.h
 *
 ***************************************************************************/

#endif /* _CONFIG_FAT_H_ */


