#if !defined(_TOPFIXUP_H_)
#define _TOPFIXUP_H_ 1
/**
    RCS: $Header$
    topFixup.h

    In general, this file contains decl's to fixup the screwups of
    various manufactures.  Feel free to ifdef away to your heart's content
    and send me your changes (kennard@ohm.berkeley.edu).

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
**/

#include "compat.h"

#if defined (PTHPPA)
#include <values.h>		/* Pickup M_PI for topFmt.c and xfaDraw.c */
#endif

#if defined(PTHPPA_CFRONT)
/* sys/types.h pwd.h needs this */
#define _INCLUDE_POSIX_SOURCE
#endif

#if defined(PTHPPA_CFRONT)
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
