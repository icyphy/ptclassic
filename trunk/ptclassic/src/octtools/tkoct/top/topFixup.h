#if !defined(_TOPFIXUP_H_)
#define _TOPFIXUP_H_ 1
/**
    RCS: $Header$
    topFixup.h

    In general, this file contains decl's to fixup the screwups of
    various manufactures.  Feel free to ifdef away to your heart's content
    and send me your changes (kennard@ohm.berkeley.edu).

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
**/

#include "compat.h"

#if defined (PTHPPA)
#include <values.h>		/* Pickup M_PI for topFmt.c and xfaDraw.c */
#endif

#if defined(PTHPPA_CFRONT)
/* sys/types.h pwd.h needs this */
#define _INCLUDE_POSIX_SOURCE
#endif

#if defined(PTHPPA_CFRONT)  || defined(PTSOL2_CFRONT)
/* memCore.c needs sbrk() */
#include <sys/unistd.h>
extern void *sbrk(int);
#else
extern caddr_t sbrk();
#endif

#if defined(mips)
#define __mips__ 1
#endif

#include <sys/types.h>	/* needed for prototypes dec'ls */

/* 
 * Define the TOP_ARGS() macro to deal with ansi vs non-ansi prototypes.
 * This use to be in topStd.h, but its useful here for properly declaring
 * functions that manufactures leave out.
 */
#if (defined(__STDC__) && !defined(NO_PROTOTYPE)) || defined(__cplusplus)
#define TOP_ARGS(x) x
#else
#define TOP_ARGS(x) ()
#endif

/*
 * These appear to be missing on most systems.
 */
#if !defined(PTHPPA)
extern int strncasecmp TOP_ARGS((const char *s1, const char *s2, size_t n));
#endif
/* Now defined in compat.h */
/*extern int strcasecmp TOP_ARGS((const char *s1, const char *s2));*/

#if defined(__sun__) && ! defined(__svr4__)

#ifdef DONT_USE
/* bad stdio.h, unbelievable...  */
extern int fputs();
extern int printf();
extern int fprintf();
extern int vsprintf();
extern int vfprintf();
extern int fseek();
extern int rewind();
extern int fclose();
extern int fflush();

/* bad ctype.h, unbelievable...  */
extern int tolower TOP_ARGS((int c));
extern int toupper TOP_ARGS((int c));

/* bad stdlib.h, unbelievable...  */
extern long strtol TOP_ARGS((const char *__nptr, char **__eptr, int __base));
extern int putenv TOP_ARGS((const char*));

#endif

/* for gcc 2.1 */
#define abort screwed_abort
#define exit screwed_exit
#include <stdlib.h>
#undef abort
#undef exit

#define free screwed_free
#include <malloc.h>
#undef free

#endif	/* __sun__ */

/*
 * sorry, but i keep getting caught by bad <math.h>
 */
extern double sqrt TOP_ARGS((double));
extern double ceil TOP_ARGS((double));
extern double floor TOP_ARGS((double));
extern double log TOP_ARGS((double));
extern double log2 TOP_ARGS((double));
extern double log10 TOP_ARGS((double));
extern double pow TOP_ARGS((double, double));
extern double exp2 TOP_ARGS((double));
extern double exp10 TOP_ARGS((double));
extern double rint TOP_ARGS((double));

/*
 * The type of the argument to strn type functions (e.g., strncmp).
 */
typedef unsigned TOPStrLenType;


#endif /* _TOPFIXUP_H_ */
