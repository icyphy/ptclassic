/* Version Identification:
 * $Id$
 */
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#ifndef PORT_H
#define PORT_H

/* 
 * Take care of the new ULTRIX 4.2 ansi_compat.h, in case we also
 * use gcc or g++. (Aug 17 1991)
 */

#if  !defined(__ANSI_COMPAT) && defined(__GNUC__) && defined(ultrix) 

#define __ANSI_COMPAT

#ifdef ultrix
#define	__ultrix      ultrix
#endif

#ifdef unix
#define	__unix        unix
#endif

#ifdef bsd4_2
#define	__bsd4_2      bsd4_2
#endif

#ifdef vax
#define __vax 	      vax
#endif

#ifdef VAX
#define __VAX 	      VAX
#endif

#ifdef mips
#define	__mips        mips
#endif

#ifdef host_mips
#define	__host_mips   host_mips
#endif

#ifdef MIPSEL
#define	__MIPSEL      MIPSEL
#endif

#ifdef MIPSEB
#define	__MIPSEB      MIPSEB
#endif

#ifdef SYSTEM_FIVE
#define	__SYSTEM_FIVE SYSTEM_FIVE
#endif

#ifdef POSIX
#define	__POSIX       POSIX
#endif

#ifdef GFLOAT
#define __GFLOAT	GFLOAT
#endif

#ifdef LANGUAGE_C
#define	__LANGUAGE_C  LANGUAGE_C
#endif

#ifdef vaxc
#define __vaxc	 vaxc
#define __VAXC   VAXC
#define __vax11c vax11c
#define __VAX11C VAX11C
#endif

#ifdef MOXIE
#define __MOXIE   MOXIE
#endif

#ifdef ULTRIX022
#define __ULTRIX022 ULTRIX022
#endif

#endif /*  defined(__GNUC__) && defined(ultrix) */

#ifdef SABER
#define volatile
#endif

#ifdef _IBMR2
#define _BSD
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE		/* Argh!  IBM strikes again */
#endif
#ifndef _ALL_SOURCE
#define _ALL_SOURCE		/* Argh!  IBM strikes again */
#endif
#ifndef _ANSI_C_SOURCE
#define _ANSI_C_SOURCE		/* Argh!  IBM strikes again */
#endif
#endif

/*
 * int32 should be defined as the most economical sized integer capable of
 * holding a 32 bit quantity
 * int16 should be similarly defined
 */

/* XXX hack */
#ifndef MACHDEP_INCLUDED
#define MACHDEP_INCLUDED
#ifdef vax
typedef int int32;
typedef short int16;
#else
     /* Ansi-C promises that these definitions should always work */
typedef long int32;
typedef int int16;
#endif /* vax */
#endif /* MACHDEP_INCLUDED */


/*
 *   Do not do anything if already somebody else is doing it.
 *  _std_h     is defined by the g++ std.h file
 *  __STDC__
 */
#if !defined(__STDC__) && !defined(_std_h)


#ifndef __DATE__
#ifdef CUR_DATE
#define __DATE__	CUR_DATE
#else
#define __DATE__	"unknown-date"
#endif /* CUR_DATE */
#endif /* __DATE__ */

#ifndef __TIME__
#ifdef CUR_TIME
#define __TIME__	CUR_TIME
#else
#define __TIME__	"unknown-time"
#endif /* CUR_TIME */
#endif /* __TIME__ */
#endif /* __STDC__ */

#ifdef sun386
#define PORTAR
#endif

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#undef HUGE
#include <math.h>
#include <signal.h>

#if defined(ultrix)  /* { */
#if defined(_SIZE_T_) /* { */
#define ultrix4
#else		     /* } else { */
#if defined(SIGLOST) /* { */
#define ultrix3
#else                /* } else { */
#define ultrix2
#endif               /* } */
#endif               /* } */
#endif               /* } */

#if defined(ultrix3) && defined(mips)
extern double rint();
extern double trunc();
#endif

#if defined(sun) && defined(FD_SETSIZE)
#define sunos4
#else
#define sunos3
#endif

#if defined(sequent) || defined(news800)
#define LACK_SYS5
#endif

#if defined(ultrix3) || defined(sunos4) || defined(_IBMR2) || defined(ultrix4)
#define SIGNAL_FN	void
#else
/* sequent, ultrix2, 4.3BSD (vax, hp), sunos3 */
#define SIGNAL_FN	int
#endif


/* Some systems have 'fixed' certain functions which used to be int */
#if defined(ultrix) || defined(SABER) || defined(hpux) || defined(aiws) || defined(apollo) || defined(AIX) || defined(__STDC__)
#define VOID_HACK void
#else
#define VOID_HACK int
#endif

#ifndef NULL
#define NULL 0
#endif /* NULL */

/*
 * CHARBITS should be defined only if the compiler lacks "unsigned char".
 * It should be a mask, e.g. 0377 for an 8-bit machine.
 */

#ifndef CHARBITS
#	define	UNSCHAR(c)	((unsigned char)(c))
#else
#	define	UNSCHAR(c)	((c)&CHARBITS)
#endif

#define SIZET int

#ifdef __STDC__
#define VOIDSTAR   void *
#else
#define VOIDSTAR   char *
#endif /* __STDC__ */

#ifndef CONST
#ifdef __STDC__
#define CONST const
#else
#define CONST
#endif /* __STDC__ */
#endif /* CONST */


/* Some machines fail to define some functions in stdio.h */
#if !defined(__STDC__) && !defined(sprite)
extern FILE *popen(), *tmpfile();
extern int pclose();
#ifndef clearerr	/* is a macro on many machines, but not all */
extern VOID_HACK clearerr();
#endif /* clearerr */
#ifndef _IBMR2
#ifndef rewind
extern VOID_HACK rewind();
#endif /* rewind */
#endif /* _IBMR2 */
#endif /* __STDC__ */


/* most machines don't give us a header file for these */
#if defined(__STDC__) || defined(sprite) || defined(__cplusplus)
#include <stdlib.h>
#else
#ifdef hpux
extern int abort();
extern void free(), exit(), perror();
#else
#ifdef _IBMR2
extern int abort(), exit();
extern void free(), perror();
#else
extern VOID_HACK abort(), free(), exit(), perror();
#endif
extern char *getenv();
#ifdef ultrix4
extern void *malloc(), *realloc(), *calloc();
#else
extern char *malloc(), *realloc(), *calloc();
#endif
#endif
#if defined(aiws) || defined(hpux)
extern int sprintf();
#else
#ifndef _IBMR2
extern char *sprintf();
#endif
#endif
extern int system();
extern double atof();
extern long atol();
#ifndef _IBMR2
extern int sscanf();
#endif
#endif /* __STDC__ */


/* some call it strings.h, some call it string.h; others, also have memory.h */
#if defined(__STDC__) || defined(sprite) || defined(hpux)
#include <string.h>
#else
/* ANSI C string.h -- 1/11/88 Draft Standard */
#if defined(ultrix4)
#include <strings.h>
#else
extern char *strcpy(), *strncpy(), *strcat(), *strncat(), *strerror();
extern char *strpbrk(), *strtok(), *strchr(), *strrchr(), *strstr();
extern int strcoll(), strxfrm(), strncmp(), strlen(), strspn(), strcspn();
extern char *memmove(), *memccpy(), *memchr(), *memcpy(), *memset();
extern int memcmp(), strcmp();
#endif /* ultrix4 */
#endif /* __STDC__ */

#ifdef lint
#undef putc			/* correct lint '_flsbuf' bug */
#endif /* lint */

/* a few extras */
#if ! defined(_std_h)
#ifdef hpux
extern VOID_HACK srand48();
extern long lrand48();
#define random() lrand48()
#define srandom(a) srand48(a)
#define bzero(a,b) memset(a, 0, b)
#else
extern VOID_HACK srandom();
extern long random();
#endif
#endif /* _std_h */

/*
**  _std_h is defined in /usr/local/lib/g++ -- include/std.h
*/

/* This causes errors on a Sun.
#if ! defined(_std_h)
#if defined(ultrix3) || defined(ultrix4) ||  defined(hpux)
extern unsigned sleep();
#else
extern VOID_HACK sleep();
#endif
#endif  \* _std_h */

/* assertion macro */
#ifndef assert
#if defined(__STDC__) || defined(sprite)
#include <assert.h>
#else
#ifndef NDEBUG
#define assert(ex) {\
    if (! (ex)) {\
	(void) fprintf(stderr, "Assertion failed: file %s, line %d\n",\
	    __FILE__, __LINE__);\
	(void) fflush(stdout);\
	abort();\
    }\
}
#else
#define assert(ex) {;}
#endif
#endif
#endif

/* handle various limits */
#if defined(__STDC__) || defined(POSIX)
#include <limits.h>
#else
#ifndef _IBMR2
#define USHRT_MAX	(~ (unsigned short int) 0)
#define UINT_MAX	(~ (unsigned int) 0)
#define ULONG_MAX	(~ (unsigned long int) 0)
#define SHRT_MAX	((short int) (USHRT_MAX >> 1))
#define INT_MAX		((int) (UINT_MAX >> 1))
#define LONG_MAX	((long int) (ULONG_MAX >> 1))
#endif
#endif

#ifdef sequent
#define SIG_FLAGS(s)    (s).sv_onstack
#else
#define SIG_FLAGS(s)    (s).sv_flags
#endif

#endif /* PORT_H */
