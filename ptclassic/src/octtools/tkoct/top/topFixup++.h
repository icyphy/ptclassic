#if !defined(_TOPFIXUPXX_H_)
#define _TOPFIXUPXX_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
/**
    topFixup++.h :: top library
**/

#define TOP_HAVE_CXX

#if !defined(PROTOTYPE)
#define PROTOTYPE
#endif

#if !defined(__cplusplus)
#define __cplusplus
#endif

#define __MEMORY_H	/* dont use CC's memory.h */

#if defined(__GNUG__)
#include <std.h>
extern "C" {
    extern int strcasecmp( char *s1, char *s2);
    extern int strncasecmp( char *s1, char *s2, int n);
}
#define _STDLIB		/* fake out tcl's stdlib.h */
#define _STRING		/* fake out tcl's string.h */
#endif

#ifdef notdef
extern "C" {
/* this picks up tcl's string.h */
#include <string.h>
}
#endif

#if !defined(__GNUG__)
extern long strtol( char *, char **, int);
extern double strtod( char *, char **);
#endif
#endif /* _TOPFIXUPXX_H_ */
