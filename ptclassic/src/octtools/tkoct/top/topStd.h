#if !defined(_TOPSTD_H_)
#define _TOPSTD_H_ 1
/* 
    RCS: $Header$
    TopStd.h :: top library

    90/Oct/01 KDW: Created as fixunix.h
    91/Mar/19 KDW: Copied from XPoleStd.h to TopStd.h

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/

/******************************************************************************
 *
 *			Includes & system externs
 *
 *****************************************************************************/

#if !defined(_TOPFIXUP_H_)
#include "topFixup.h"
#endif

/*
 * TOP_ARGS() is defined in topFixup.h
 */

#if !defined(ARGS)
#define ARGS(x) TOP_ARGS(x)
#endif

#if defined(_VA_LIST_)
#define ARGS_VA(x) ARGS(x)
#else
#define ARGS_VA(x) ()
#endif

#if defined(_XLIB_H_)
#define ARGS_X(x) ARGS(x)
#else
#define ARGS_X(x) ()
#endif

#if defined(_TCL)
#define ARGS_TCL(x) ARGS(x)
#else
#define ARGS_TCL(x) ()
#endif

#if defined(_TK)
#define ARGS_TK(x) ARGS(x)
#else
#define ARGS_TK(x) ()
#endif

#if !defined(TOP_HAVE_CXX)

/* included by topFixup.h #include <sys/types.h> */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <string.h>	/* string.h appears to be ANSI, strings.h is K&R */
#include <memory.h>

#endif

/*
 * The __TOP_AUTOINCLUDE__ ccp variable controls whether some include
 * files will automatically include are other include files that are
 * required.  Some people (C++'ers) love this feature, while others
 * view it as the root of all evil.  Define this as 1 to enable the
 * feature, 0 to disable: ``cc -D__TOP_AUTOINCLUDE__=0 ... ''
 * By default, it is on.
 */
#if !defined(__TOP_AUTOINCLUDE__)
#define __TOP_AUTOINCLUDE__ 1
#endif

/*
 * The __TOP_DEBUG__ cpp variable controls whether debugging code
 * gets compiled into the libraries.  The stdout printing generated
 * by these statements can be suppresed at run time; however, the
 * code will run somewhat slower if debugging is turned on.
 */
#if !defined(__TOP_DEBUG__)
#define __TOP_DEBUG__ 1
#endif

/******************************************************************************
 *
 *			Standard Types
 *
 *****************************************************************************/

typedef int TOPStatus;

typedef int TOPLogical;
#define TOP_FALSE	0
#define TOP_TRUE	1

#if defined(__GNUC__)
typedef void* TOPPtr;
#else
typedef char* TOPPtr;
#endif

#define TOP_Cast(type,val)	((type)(val))
#define TOP_Ptr(val)		TOP_Cast(TOPPtr,(val))
#define TOP_Char(val)		TOP_Cast(char,(val))
#define TOP_CharPtr(val)	TOP_Cast(char*,(val))
#define TOP_Int(val)		TOP_Cast(int,(val))
#define TOP_IntPtr(val)		TOP_Cast(int*,(val))
#define TOP_Float(val)		TOP_Cast(float,(val))
#define TOP_FloatPtr(val)	TOP_Cast(float*,(val))
#define TOP_Double(val)		TOP_Cast(double,(val))
#define TOP_DoublePtr(val)	TOP_Cast(double*,(val))

/* copied from Tk */
#define TOP_Offset(type,field) TOP_Int(TOP_CharPtr(&(TOP_Cast(type*,0)->field)))

#define TOP_NELTS(x)	( sizeof(x)/sizeof((x)[0]) )

typedef enum _TOPBool TOPBool;
enum _TOPBool { TOPB_False, TOPB_True };

typedef char* TOPMallocPtr;
typedef int TOPCompareFunc ARGS((const void *a, const void *b));

typedef long TOPMask;
#define TOP_BIT(k) (1<<(k))
#define TOP_IsOn(m,b)	( ((m) & (b)) )
#define TOP_OnB(m,b)	( ((m) & (b)) )
#define TOP_IsOff(m,b)	( ((m) & (b))==0 )
#define TOP_OffB(m,b)	( ((m) & (b))==0 )

#define TOP_MIN(a,b)	( (a)<=(b)?(a):(b) )
#define TOP_MAX(a,b)	( (a)>=(b)?(a):(b) )
#define TOP_ABS(x)	( ((x)>=0)?(x):(-(x)) )
#define TOP_SWAPTYPE(type,a,b) {	\
    type tmp___;			\
    tmp___ = (a);			\
    (a) = (b);				\
    (b) = tmp___;			\
}

#define TOP_ROUND(x,u)      (rint((x)/(u))*(u))
#define TOP_CEIL(x,u)       (ceil((x)/(u))*(u))
#define TOP_FLOOR(x,u)      (floor((x)/(u))*(u))

#define TOP_SQR(x)		((x)*(x))
#define TOP_NORMSQR(x,y)	( TOP_SQR(x) + TOP_SQR(y) )

typedef struct _TOPDblPoint2 TOPDblPoint2;
struct _TOPDblPoint2 {
    double		x;
    double		y;
};

/******************************************************************************
 *
 *			Backward compatibility
 *
 *****************************************************************************/

/*
 * In an ANSI world these wouldnt be needed, but some people are still
 * in the dark ages.  Beware side affects.
 */
#define TOP_ToUpper(c) ( islower(c) ? toupper(c) : (c) )
#define TOP_ToLower(c) ( isupper(c) ? tolower(c) : (c) )

#ifndef TRUE
#define TRUE TOP_TRUE
#endif

#ifndef FALSE
#define FALSE TOP_FALSE
#endif

#define topPTR TOPPtr
#define topMASK TOPMask;

/******************************************************************************
 *
 *			Stuff exported by this library
 *
 *****************************************************************************/

#include "topMsg.h"
#endif /* _TOPSTD_H_ */
