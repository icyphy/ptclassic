#if !defined(_TOPFIXUPXX_H_)
#define _TOPFIXUPXX_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
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
