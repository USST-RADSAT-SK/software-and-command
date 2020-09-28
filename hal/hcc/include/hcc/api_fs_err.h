/***************************************************************************
 *
 *            Copyright (c) 2005-2014 by HCC Embedded
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
#ifndef _API_FS_ERR_H_
#define _API_FS_ERR_H_

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 * File system error codes
 ***************************************************************************/
#define F_NO_ERROR              0
#define F_ERR_INVALIDDRIVE      1
#define F_ERR_NOTFORMATTED      2
#define F_ERR_INVALIDDIR        3
#define F_ERR_INVALIDNAME       4
#define F_ERR_NOTFOUND          5
#define F_ERR_DUPLICATED        6
#define F_ERR_NOMOREENTRY       7
#define F_ERR_NOTOPEN           8
#define F_ERR_EOF               9
#define F_ERR_RESERVED          10
#define F_ERR_NOTUSEABLE        11
#define F_ERR_LOCKED            12
#define F_ERR_ACCESSDENIED      13
#define F_ERR_NOTEMPTY          14
#define F_ERR_INITFUNC          15
#define F_ERR_CARDREMOVED       16
#define F_ERR_ONDRIVE           17
#define F_ERR_INVALIDSECTOR     18
#define F_ERR_READ              19
#define F_ERR_WRITE             20
#define F_ERR_INVALIDMEDIA      21
#define F_ERR_BUSY              22
#define F_ERR_WRITEPROTECT      23
#define F_ERR_INVFATTYPE        24
#define F_ERR_MEDIATOOSMALL     25
#define F_ERR_MEDIATOOLARGE     26
#define F_ERR_NOTSUPPSECTORSIZE 27
#define F_ERR_UNKNOWN           28
#define F_ERR_DRVALREADYMNT     29
#define F_ERR_TOOLONGNAME       30
#define F_ERR_NOTFORREAD        31
#define F_ERR_DELFUNC           32
#define F_ERR_ALLOCATION        33
#define F_ERR_INVALIDPOS        34
#define F_ERR_NOMORETASK        35
#define F_ERR_NOTAVAILABLE      36
#define F_ERR_TASKNOTFOUND      37
#define F_ERR_UNUSABLE          38
#define F_ERR_CRCERROR          39
#define F_ERR_CARDCHANGED       40

#ifdef __cplusplus
}
#endif

#endif /* _API_FS_ERR_H_ */

