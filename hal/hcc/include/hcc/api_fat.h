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
#ifndef _API_FAT_H_
#define _API_FAT_H_

#include <stdint.h>

/* include configuration settings */
#include "../config/config_fat.h"


/* include definition of F_DRIVER */
#include "api_mdriver.h"

#include "../version/ver_fat.h"
#if VER_FAT_MAJOR != 8
 #error "VER_FAT_MAJOR invalid"
#endif
#if VER_FAT_MINOR != 6
 #error "VER_FAT_MINOR invalid"
#endif


#if !( FN_CAPI_USED )
 #include "api_fs_err.h"
#endif


/****************************************************************************
 *
 * open bracket for C++ compatibility
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 *
 * structure defines
 *
 ***************************************************************************/

#define F_MAXNAME       FN_MAXNAME
#define F_MAXLNAME      FN_MAXLNAME

#if ( !FN_CAPI_USED )
 #define F_FILE         FN_FILE
 #define F_FIND         FN_FIND
 #define F_SPACE        FN_SPACE
 #define F_MAXPATHNAME  FN_MAXPATHNAME
 #define F_SEEK_SET     FN_SEEK_SET
 #define F_SEEK_END     FN_SEEK_END
 #define F_SEEK_CUR     FN_SEEK_CUR
 #define F_SEEK_NOWRITE FN_SEEK_NOWRITE
 #define F_STAT         FN_STAT
#endif


/****************************************************************************
 *
 *  f_format_progress(...) will be called by f_format() periodically.
 *
 ***************************************************************************/
typedef void ( * F_FORMAT_PROGRESS_FUNC )( uint32_t i_sector_written, uint32_t i_sector_total );

extern F_FORMAT_PROGRESS_FUNC  f_format_progress;

#define f_set_format_progress( format_progress ) f_format_progress = format_progress


/****************************************************************************
 *
 *  external single-byte to wide-char conversion function
 *
 *  called by _towchar( ) and needs to convert some single-byte characters (1 or 2)
 *  to a single UNICODE wide-byte character. Returns the number of input characters
 *  used in *p_len_src.
 *  Returns 0 if the conversion was successful.
 *
 ***************************************************************************/
#if HCC_UNICODE
typedef uint32_t ( * F_ASCII_TO_UNICODE_FUNC )( wchar * p_dst, const char * p_src, uint32_t * p_len_src );

extern F_ASCII_TO_UNICODE_FUNC  f_ascii_to_unicode;

 #define f_set_ascii_to_unicode( ascii_to_unicode ) f_ascii_to_unicode = ascii_to_unicode


/****************************************************************************
 *
 *  external UNICODE conversion function
 *
 *  called by _fromwchar( ) and _f_createlfn( ), needs to convert a single wide-byte
 *  character to ASCII. *p_len_dst is the available space in *p_dst. Upon return
 *  *p_len_dst will hold the number of characters written to *p_dst.
 *  Returns 0 if the conversion was successful.
 *
 ***************************************************************************/
typedef uint32_t ( *F_UNICODE_TO_ASCII_FUNC )( char * p_dst, const wchar src, uint32_t * p_len_dst );

extern F_UNICODE_TO_ASCII_FUNC  f_unicode_to_ascii;

 #define f_set_unicode_to_ascii( unicode_to_ascii ) f_unicode_to_ascii = unicode_to_ascii

#endif /* HCC_UNICODE */


#ifndef NULL
 #define NULL (void *)0
#endif


/* HCC_UNICODE implies long filename (LFN) support */
#if HCC_UNICODE || F_LONGFILENAME
 #define LFN_ENABLE 1
#else
 #define LFN_ENABLE 0
#endif


/* LFN_ENABLE is needed for DIRCACHE_ENABLE to take effect */
#if LFN_ENABLE && DIRCACHE_ENABLE
 #define F_DIRCACHE 1
#else
 #define F_DIRCACHE 0
#endif


/* long filenames always have the type 'W_CHAR' which can be either 'char' or
 * 'wchar' depending on the actual HCC_UNICODE setting */
#if !FN_CAPI_USED
 #if HCC_UNICODE
  #define W_CHAR wchar
 #else
  #define W_CHAR char
 #endif
#endif


/* public structure for FN_FILE */
typedef struct
{
  void * reference;     /* reference which fileint used */
} FN_FILE;

/* F_NAME structure definition */
#if !LFN_ENABLE
typedef struct
{
  int   drivenum;                   /*  drive number 0-A 1-B 2-C  */
  char  path[F_MAXPATHNAME];        /*  pathnam  /directory1/dir2/   */
  char  filename[F_MAXSNAME];       /*  filename  */
  char  fileext[F_MAXSEXT];         /*  extension  */
} F_NAME;
#else
typedef struct
{
  int     drivenum;                 /*  drive number 0-A 1-B 2-C  */
  W_CHAR  path[F_MAXPATHNAME];      /*  pathname /directory1/dir2/   */
  W_CHAR  lname[F_MAXLNAME];        /*  long file name   */
} F_NAME;
#endif /* elif !LFN_ENABLE */

typedef struct
{
  unsigned long  cluster;       /* which cluster is used */
  unsigned long  prevcluster;   /* previous cluster for bad block handling */
  unsigned long  sectorbegin;   /* calculated sector start */
  unsigned long  sector;        /* current sector */
  unsigned long  sectorend;     /* last saector position of the cluster */
  unsigned long  pos;           /* current position */
} F_POS;

typedef struct
{
  char            filename[F_MAXPATHNAME];   /* file name+ext */
  char            name[F_MAXSNAME];          /* file name */
  char            ext[F_MAXSEXT];            /* file extension */
  unsigned char   attr;                      /* attribute of the file */

  unsigned short  ctime;                    /* creation time */
  unsigned short  cdate;                    /* creation date */
  unsigned long   filesize;                 /* length of file */

  unsigned long   cluster;                  /* current file starting position */
  F_NAME          findfsname;               /* find properties */
  F_POS           pos;                      /* position of the current list */
} FN_FIND;

#if HCC_UNICODE
typedef struct
{
  W_CHAR          filename[F_MAXPATHNAME];   /* file name+ext */
  char            name[F_MAXSNAME];          /* file name */
  char            ext[F_MAXSEXT];            /* file extension */
  unsigned char   attr;                      /* attribute of the file */

  unsigned short  ctime;                    /* creation time */
  unsigned short  cdate;                    /* creation date */
  unsigned long   filesize;                 /* length of file */

  unsigned long   cluster;                  /* current file starting position */
  F_NAME          findfsname;               /* find properties */
  F_POS           pos;                      /* position of the current list */
} FN_WFIND;
#endif


#if USE_MALLOC && ( FATCACHE_ENABLE || F_DIRCACHE )

typedef struct
{
  uint32_t  n_sectors;              /* maximum number of sectors */

 #if FATCACHE_ENABLE
  uint16_t  n_fatcache_blocks;      /* number of FATCACHE blocks */
  uint16_t  n_fatcache_readahead;   /* number of sectors in a FATCACHE block */
 #endif

 #if F_DIRCACHE
  uint16_t  n_dircache_sectors;     /* number of sectors in DIRCACHE */
 #endif
} F_CACHE_CONFIG;

#endif /* USE_MALLOC && ( FATCACHE_ENABLE || F_DIRCACHE ) */


/* attribute file/directory bitpattern definitions */
#define F_ATTR_ARC          0x20
#define F_ATTR_DIR          0x10
#define F_ATTR_VOLUME       0x08
#define F_ATTR_SYSTEM       0x04
#define F_ATTR_HIDDEN       0x02
#define F_ATTR_READONLY     0x01

/*  definitions for ctime  */
#define F_CTIME_SEC_SHIFT   0
#define F_CTIME_SEC_MASK    0x001f  /* 0-30 in 2seconds */
#define F_CTIME_MIN_SHIFT   5
#define F_CTIME_MIN_MASK    0x07e0  /* 0-59  */
#define F_CTIME_HOUR_SHIFT  11
#define F_CTIME_HOUR_MASK   0xf800  /* 0-23 */

/*  definitions for cdate  */
#define F_CDATE_DAY_SHIFT   0
#define F_CDATE_DAY_MASK    0x001f  /* 0-31 */
#define F_CDATE_MONTH_SHIFT 5
#define F_CDATE_MONTH_MASK  0x01e0  /* 1-12 */
#define F_CDATE_YEAR_SHIFT  9
#define F_CDATE_YEAR_MASK   0xfe00  /* 0-119 (1980+value) */

/* definition for a media and f_format */
enum
{
/* 0 */
  F_UNKNOWN_MEDIA,

/* 1 */ F_FAT12_MEDIA,

/* 2 */ F_FAT16_MEDIA,

/* 3 */ F_FAT32_MEDIA
};

/* definition for partitions */
typedef struct
{
  unsigned long  secnum;                /* number of sectors in this partition */
  unsigned char  system_indicator;      /* use F_SYSIND_XX values*/
  unsigned char  bootable;              /* if not 0, bootable (active) bit of partition will be set */
} F_PARTITION;

/* select system indication for creating partition */
#define F_SYSIND_DOSFAT12         0x01
#define F_SYSIND_DOSFAT16UPTO32MB 0x04
#define F_SYSIND_DOSFAT16OVER32MB 0x06
#define F_SYSIND_DOSFAT32         0x0b

/* these values for extended partition */
#define F_SYSIND_EXTWIN           0x0f
#define F_SYSIND_EXTDOS           0x05

/* definition for f_getfreespace */
typedef struct
{
  unsigned long  total;
  unsigned long  free;
  unsigned long  used;
  unsigned long  bad;

  unsigned long  total_high;
  unsigned long  free_high;
  unsigned long  used_high;
  unsigned long  bad_high;
} FN_SPACE;

/* definition for f_stat*/
typedef struct
{
  unsigned long   filesize;
  unsigned short  createdate;
  unsigned short  createtime;
  unsigned short  modifieddate;
  unsigned short  modifiedtime;
  unsigned short  lastaccessdate;
  unsigned char   attr;                 /* 00ADVSHR */
  int             drivenum;
} FN_STAT;


/****************************************************************************
 *
 * defines for f_seek
 *
 ***************************************************************************/

/* Beginning of file */
#ifdef SEEK_SET
 #define FN_SEEK_SET SEEK_SET
#else
 #define FN_SEEK_SET 0
#endif

/* Current position of file pointer */
#ifdef SEEK_CUR
 #define FN_SEEK_CUR SEEK_CUR
#else
 #define FN_SEEK_CUR 1
#endif

/* End of file */
#ifdef SEEK_END
 #define FN_SEEK_END    SEEK_END
#else
 #define FN_SEEK_END    2
#endif

/* Zeros won't be written when seeking beyond end of file */
#define FN_SEEK_NOWRITE 16


/****************************************************************************
 *
 * for file changed events
 *
 ***************************************************************************/

#if F_FILE_CHANGED_EVENT && ( !FN_CAPI_USED )

typedef struct
{
  unsigned char   action;
  unsigned char   flags;
  unsigned char   attr;
  unsigned short  ctime;
  unsigned short  cdate;
  unsigned long   filesize;
  W_CHAR          filename[F_MAXPATHNAME];
} ST_FILE_CHANGED;

typedef void ( * F_FILE_CHANGED_EVENTFUNC )( ST_FILE_CHANGED * fc );

extern F_FILE_CHANGED_EVENTFUNC  f_filechangedevent;

 #define f_setfilechangedevent( filechangeevent ) f_filechangedevent = filechangeevent

/* flags */

 #define FFLAGS_NONE              0x00000000

 #define FFLAGS_FILE_NAME         0x00000001
 #define FFLAGS_DIR_NAME          0x00000002
 #define FFLAGS_NAME              0x00000003
 #define FFLAGS_ATTRIBUTES        0x00000004
 #define FFLAGS_SIZE              0x00000008
 #define FFLAGS_LAST_WRITE        0x00000010

/* actions */

 #define FACTION_ADDED            0x00000001
 #define FACTION_REMOVED          0x00000002
 #define FACTION_MODIFIED         0x00000003
 #define FACTION_RENAMED_OLD_NAME 0x00000004
 #define FACTION_RENAMED_NEW_NAME 0x00000005

#endif /* if F_FILE_CHANGED_EVENT && ( !FN_CAPI_USED ) */


/****************************************************************************
 *
 * function defines
 *
 ***************************************************************************/

#if ( !FN_CAPI_USED )
 #define fs_init                                                        fn_init
 #define fs_start                                                       fn_start
 #define fs_stop                                                        fn_stop
 #define fs_delete                                                      fsn_delete
 #define f_createdriver( driver, driver_init, driver_param )            fm_createdriver( driver, driver_init, driver_param )
 #define f_releasedriver( driver )                                      fm_releasedriver( driver )
 #define f_createpartition( driver, parnum, par )                       fm_createpartition( driver, parnum, par )
 #define f_getpartition( driver, parnum, par )                          fm_getpartition( driver, parnum, par )
 #define f_initvolume( drvnumber, driver_init, driver_param )           fm_initvolume( drvnumber, driver_init, driver_param )
 #define f_initvolumepartition( drvnumber, driver, partition )          fm_initvolumepartition( drvnumber, driver, partition )
 #if ( SAFEFAT && NONSAFEFAT )
  #define f_initvolume_nonsafe( drvnumber, driver_init, driver_param )  fm_initvolume_nonsafe( drvnumber, driver_init, driver_param )
  #define f_initvolumepartition_nonsafe( drvnumber, driver, partition ) fm_initvolumepartition_nonsafe( drvnumber, driver, partition )
 #endif
 #if ( ( !SAFEFAT ) && NONSAFEFAT )
  #define f_initvolume_nonsafe( drvnumber, driver_init, driver_param )  fm_initvolume( drvnumber, driver_init, driver_param )
  #define f_initvolumepartition_nonsafe( drvnumber, driver, partition ) fm_initvolumepartition( drvnumber, driver, partition )
 #endif
 #define f_getlasterror fm_getlasterror

 #define f_delvolume( drvnumber )                                       fm_delvolume( drvnumber )
 #define f_get_volume_count()                                           fm_get_volume_count()
 #define f_get_volume_list( buf )                                       fm_get_volume_list( buf )
 #define f_checkvolume( drvnumber )                                     fm_checkvolume( drvnumber )
 #if F_VOLNAME_SUPPORT
  #define f_setvolname( drivenum, name )                                fm_setvolname( drivenum, name )
  #define f_getvolname( drivenum, buffer, maxlen )                      fm_getvolname( drivenum, buffer, maxlen )
 #endif
 #define f_format( drivenum, fattype )                                  fm_format( drivenum, fattype )
 #define f_getcwd( buffer, maxlen )                                     fm_getcwd( buffer, maxlen )
 #define f_getdcwd( drivenum, buffer, maxlen )                          fm_getdcwd( drivenum, buffer, maxlen )
 #define f_chdrive( drivenum )                                          fm_chdrive( drivenum )
 #define f_getdrive fm_getdrive
 #define f_getfreespace( drivenum, pspace )                             fm_getfreespace( drivenum, pspace )

 #define f_chdir( dirname )                                             fm_chdir( dirname )
 #define f_mkdir( dirname )                                             fm_mkdir( dirname )
 #define f_rmdir( dirname )                                             fm_rmdir( dirname )

 #define f_findfirst( filename, find )                                  fm_findfirst( filename, find )
 #define f_findnext( find )                                             fm_findnext( find )
 #define f_move( filename, newname )                                    fm_move( filename, newname )
 #define f_rename( old_name, new_name )                                 fm_rename( ( old_name ), ( new_name ) )
 #define f_filelength( filename )                                       fm_filelength( filename )

 #if (SAFEFAT)
  #define f_abortclose( filehandle )                                    fm_abortclose( filehandle )
 #endif
 #define f_close( filehandle )                                          fm_close( filehandle )
 #define f_flush_filebuffer( filehandle )                               fm_flush_filebuffer( filehandle )
 #define f_flush( filehandle )                                          fm_flush( filehandle )
 #define f_open( filename, mode )                                       fm_open( filename, mode )
 #if ( SAFEFAT && NONSAFEFAT )
  #define f_open_nonsafe( filename, mode )                              fm_open_nonsafe( filename, mode )
 #endif
 #if ( ( !SAFEFAT ) && NONSAFEFAT )
  #define f_open_nonsafe( filename, mode )                              fm_open( filename, mode )
 #endif
 #define f_truncate( filename, length )                                 fm_truncate( filename, length )
 #define f_ftruncate( filehandle, length )                              fm_ftruncate( filehandle, length )

 #define f_read( buf, size, size_st, filehandle )                       fm_read( buf, size, size_st, filehandle )
 #define f_write( buf, size, size_st, filehandle )                      fm_write( buf, size, size_st, filehandle )

 #define f_seek( filehandle, offset, whence )                           fm_seek( filehandle, offset, whence )
 #define f_seteof( filehandle )                                         fm_seteof( filehandle )

 #define f_tell( filehandle )                                           fm_tell( filehandle )
 #define f_getc( filehandle )                                           fm_getc( filehandle )
 #define f_putc( ch, filehandle )                                       fm_putc( ch, filehandle )
 #define f_rewind( filehandle )                                         fm_rewind( filehandle )
 #define f_eof( filehandle )                                            fm_eof( filehandle )

 #define f_stat( filename, stat )                                       fm_stat( filename, stat )
 #define f_fstat( filehandle, stat )                                    fm_fstat( filehandle, stat )
 #define f_gettimedate( filename, pctime, pcdate )                      fm_gettimedate( filename, pctime, pcdate )
 #define f_settimedate( filename, ctime, cdate )                        fm_settimedate( filename, ctime, cdate )
 #define f_delete( filename )                                           fm_delete( filename )
 #if F_DELETE_CONTENT
  #define f_deletecontent( filename )                                   fm_deletecontent( filename )
 #endif

 #define f_getattr( filename, attr )                                    fm_getattr( filename, attr )
 #define f_setattr( filename, attr )                                    fm_setattr( filename, attr )

 #define f_getlabel( drivenum, label, len )                             fm_getlabel( drivenum, label, len )
 #define f_setlabel( drivenum, label )                                  fm_setlabel( drivenum, label )

 #define f_get_oem( drivenum, str, maxlen )                             fm_get_oem( drivenum, str, maxlen )
#endif /* if ( !FN_CAPI_USED ) */

#if HCC_UNICODE
 #if ( !FN_CAPI_USED )
  #define F_WFIND FN_WFIND
  #define f_wgetcwd( buffer, maxlen )                fm_wgetcwd( buffer, maxlen )
  #define f_wgetdcwd( drivenum, buffer, maxlen )     fm_wgetdcwd( drivenum, buffer, maxlen )
  #define f_wchdir( dirname )                        fm_wchdir( dirname )
  #define f_wmkdir( dirname )                        fm_wmkdir( dirname )
  #define f_wrmdir( dirname )                        fm_wrmdir( dirname )
  #define f_wfindfirst( filename, find )             fm_wfindfirst( filename, find )
  #define f_wfindnext( find )                        fm_wfindnext( find )
  #define f_wmove( filename, newname )               fm_wmove( filename, newname )
  #define f_wrename( old_name, new_name )            fm_wrename( ( old_name ), ( new_name ) )
  #define f_wfilelength( filename )                  fm_wfilelength( filename )
  #define f_wopen( filename, mode )                  fm_wopen( filename, mode )
  #if ( SAFEFAT && NONSAFEFAT )
   #define f_wopen_nonsafe( filename, mode )         fm_wopen_nonsafe( filename, mode )
  #endif
  #if ( ( !SAFEFAT ) && NONSAFEFAT )
   #define f_wopen_nonsafe( filename, mode )         fm_wopen( filename, mode )
  #endif
  #define f_wtruncate( filename, length )            fm_wtruncate( filename, length )
  #define f_wstat( filename, stat )                  fm_wstat( filename, stat )
  #define f_wgettimedate( filename, pctime, pcdate ) fm_wgettimedate( filename, pctime, pcdate )
  #define f_wsettimedate( filename, ctime, cdate )   fm_wsettimedate( filename, ctime, cdate )
  #define f_wdelete( filename )                      fm_wdelete( filename )
  #if F_DELETE_CONTENT
   #define f_wdeletecontent( filename )              fm_wdeletecontent( filename )
  #endif
  #define f_wgetattr( filename, attr )               fm_wgetattr( filename, attr )
  #define f_wsetattr( filename, attr )               fm_wsetattr( filename, attr )
 #endif /* if ( !FN_CAPI_USED ) */
#endif  /* if HCC_UNICODE */


/****************************************************************************
 *
 * function externs
 *
 ***************************************************************************/

int fn_init ( void );
int fn_start ( void );
int fn_stop ( void );
int fsn_delete ( void );

#if FN_CAPI_USED
 #include "api_capi.h"
#endif

int fm_initvolume ( int drvnumber, F_DRIVERINIT driver_init, unsigned long driver_param );
int fm_initvolumepartition ( int drvnumber, F_DRIVER * driver, int partition );
#if ( SAFEFAT && NONSAFEFAT )
int fm_initvolume_nonsafe ( int drvnumber, F_DRIVERINIT driver_init, unsigned long driver_param );
int fm_initvolumepartition_nonsafe ( int drvnumber, F_DRIVER * driver, int partition );
#endif
int fm_createpartition ( F_DRIVER * driver, int parnum, F_PARTITION * par );
int fm_createdriver ( F_DRIVER * * driver, F_DRIVERINIT driver_init, unsigned long driver_param );
int fm_releasedriver ( F_DRIVER * driver );
int fm_getpartition ( F_DRIVER * driver, int parnum, F_PARTITION * par );
int fm_delvolume ( int drvnumber );
int fm_checkvolume ( int drvnumber );
int fm_get_volume_count ( void );
int fm_get_volume_list ( int * buf );
#if F_VOLNAME_SUPPORT
int fm_setvolname ( int drivenum, const char * p_name );
int fm_getvolname ( int drivenum, char * p_buffer, int maxlen );
#endif
int fm_format ( int drivenum, long fattype );
int fm_getcwd ( char * buffer, int maxlen );
int fm_getdcwd ( int drivenum, char * buffer, int maxlen );
int fm_chdrive ( int drivenum );
int fm_getdrive ( void );
int fm_getfreespace ( int drivenum, FN_SPACE * pspace );
int fm_getlasterror ( void );

int fm_chdir ( const char * dirname );
int fm_mkdir ( const char * dirname );
int fm_rmdir ( const char * dirname );

int fm_findfirst ( const char * filename, FN_FIND * find );
int fm_findnext ( FN_FIND * find );
int fm_move ( const char * filename, const char * newname );
int fm_rename ( const char * oldname, const char * newname );
long fm_filelength ( const char * filename );

#if ( SAFEFAT )
int fm_abortclose ( FN_FILE * filehandle );
#endif
int fm_close ( FN_FILE * filehandle );
int fm_flush_filebuffer ( FN_FILE * filehandle );
int fm_flush ( FN_FILE * filehandle );
FN_FILE * fm_open ( const char * filename, const char * mode );
#if ( SAFEFAT && NONSAFEFAT )
FN_FILE * fm_open_nonsafe ( const char * filename, const char * mode );
#endif
FN_FILE * fm_truncate ( const char * filename, unsigned long length );
int  fm_ftruncate ( FN_FILE * filehandle, unsigned long length );

long fm_read ( void * buf, long size, long size_st, FN_FILE * filehandle );
long fm_write ( const void * buf, long size, long size_st, FN_FILE * filehandle );

int fm_seek ( FN_FILE * filehandle, long offset, long whence );

long fm_tell ( FN_FILE * filehandle );
int fm_getc ( FN_FILE * filehandle );
int fm_putc ( int ch, FN_FILE * filehandle );
int fm_rewind ( FN_FILE * filehandle );
int fm_eof ( FN_FILE * filehandle );
int fm_seteof ( FN_FILE * filehandle );

int fm_stat ( const char * filename, F_STAT * stat );
int fm_fstat ( FN_FILE * p_filehandle, F_STAT * p_stat );
int fm_gettimedate ( const char * filename, unsigned short * pctime, unsigned short * pcdate );
int fm_settimedate ( const char * filename, unsigned short ctime, unsigned short cdate );
int fm_delete ( const char * filename );
#if F_DELETE_CONTENT
int fm_deletecontent ( const char * filename );
#endif

int fm_getattr ( const char * filename, unsigned char * attr );
int fm_setattr ( const char * filename, unsigned char attr );

int fm_getlabel ( int drivenum, char * label, long len );
int fm_setlabel ( int drivenum, const char * label );

int fm_get_oem ( int drivenum, char * str, long maxlen );

#if ( !FN_CAPI_USED )
int f_enterFS ( void );
void f_releaseFS ( void );
#endif

#if HCC_UNICODE
int fm_wgetcwd ( wchar * buffer, int maxlen );
int fm_wgetdcwd ( int drivenum, wchar * buffer, int maxlen );
int fm_wchdir ( const wchar * dirname );
int fm_wmkdir ( const wchar * dirname );
int fm_wrmdir ( const wchar * dirname );
int fm_wfindfirst ( const wchar * filename, FN_WFIND * find );
int fm_wfindnext ( FN_WFIND * find );
int fm_wmove ( const wchar * filename, const wchar * newname );
int fm_wrename ( const wchar * oldname, const wchar * newname );
long fm_wfilelength ( const wchar * filename );
FN_FILE * fm_wopen ( const wchar * filename, const wchar * mode );
 #if ( SAFEFAT && NONSAFEFAT )
FN_FILE * fm_wopen_nonsafe ( const wchar * filename, const wchar * mode );
 #endif
FN_FILE * fm_wtruncate ( const wchar * filename, unsigned long length );
int fm_wstat ( const wchar * filename, F_STAT * stat );
int fm_wgettimedate ( const wchar * filename, unsigned short * pctime, unsigned short * pcdate );
int fm_wsettimedate ( const wchar * filename, unsigned short ctime, unsigned short cdate );
int fm_wdelete ( const wchar * filename );
 #if F_DELETE_CONTENT
int fm_wdeletecontent ( const wchar * filename );
 #endif
int fm_wgetattr ( const wchar * filename, unsigned char * attr );
int fm_wsetattr ( const wchar * filename, unsigned char attr );
#endif /* if HCC_UNICODE */


/****************************************************************************
 *
 * closing bracket for C++ compatibility
 *
 ***************************************************************************/

#ifdef __cplusplus
}
#endif


/****************************************************************************
 *
 * end of api_fat.h
 *
 ***************************************************************************/

#endif /* _API_FAT_H_ */



