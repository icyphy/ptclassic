/**************************************************************************
 *                      Annapolis Micro Systems Inc.                      *
 *                         190 Admiral Cochrane                           *
 *                              Suite 130                                 *
 *                         Annapolis, MD  21401                           *
 **************************************************************************/

/**************************************************************************
 *  File       :  wfoss.h
 *  Project    :  Wildforce
 *  Description:  Runtime library for Wildforce
 *  Author     :  M. Gray
 *  Copyright  :  Annapolis Micro Systems Inc., 1998
 *
 *  Notes
 *   o This is a central point for adding Operating System specific
 *     headers, to minimize updates to other .h and .c files in API 
 *    level ports.
 *
 **************************************************************************/

#ifndef __WFOSS_H__
#define __WFOSS_H__

#if defined( WIN32 ) && (! defined(__DRV__))
#include <windows.h>
#endif

#ifndef WFAPI
#ifdef __cplusplus
#define WFAPI  extern "C"
#else
#define WFAPI  extern
#endif
#endif

#if ( !defined( WIN32 ) )

#if (__GNUC__ == 2) && (__GNUC_MINOR__ < 91)
/* Don't include sys/inttypes.h under egcs-1.1.1 (__GNUC_MINOR__ == 91)
   or g++-2.95.1 (__GNUC_MINOR__ == 95) */
#include <sys/inttypes.h>
#include <sys/types.h>
#endif

#ifndef UCHAR
#define UCHAR unsigned char
#endif

#ifndef CHAR
#define CHAR char
#endif

#ifndef USHORT
#define USHORT uint16_t
#endif

#ifndef DWORD
#define DWORD uint32_t
#endif

#ifndef ULONG
#define ULONG DWORD
#endif

#ifndef LONG
#define LONG int32_t
#endif

#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef BOOLEAN
#define BOOLEAN int
#endif

//#ifndef FLOAT
//#define FLOAT   float
//#endif

#ifndef LPVOID
#define LPVOID  void *
#endif

#ifndef PULONG
#define PULONG  ULONG *
#endif

#ifndef HANDLE
#define HANDLE  int
#endif

#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif


#endif /* sentinel */
