/* Version Identification:
 * $Id$
 */
/*
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

#ifdef linux
#define __linux		linux
#define _POSIX_SOURCE
#define _GNU_SOURCE
#define _BSD_SOURCE
#endif


#if defined(aix) || defined(_AIX)
#define __aix		aix
#define _POSIX_SOURCE
#define _GNU_SOURCE
#define _BSD_SOURCE
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

#if defined(__sparc) && defined(__svr4__)
#define SOL2
#define SYSV
#endif

#ifdef _IBMR2

#if defined(aix) || defined(_AIX)
#define _BSD 44
#else
#define _BSD
#endif /* aix */

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
/* We have not included compat.h here yet so we cannot use PTALPHA */
#if defined(vax) || defined(__alpha)
typedef long int32;		/* See kernel/Fix.h also */
typedef short int16;
#else
     /* Ansi-C promises that these definitions should always work */
typedef long int32;
typedef int int16;
#endif /* vax */
#endif /* MACHDEP_INCLUDED */

#include "ansi.h"

#include "compat.h"
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

#ifdef linux
#include <ansidecl.h>
#endif

#include <stdio.h>
#include <ctype.h>

#ifdef PTSOL2
/* Solaris2.3 defines type boolean as an enum, which is a no no.
 * see also pigilib/compat.h
 */
#define boolean sun_boolean
#include <unistd.h>
#undef boolean
#else
#include <sys/types.h>
#endif /* PTSOL2 */

#ifdef PTHPPA
#include <unistd.h>
#endif

#ifndef TYPEDEF_BOOLEAN
#define TYPEDEF_BOOLEAN 1
typedef int boolean;
#endif /* TYPEDEF_BOOLEAN */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

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

#if defined(PTSUN4) || defined(ultrix4) || defined(PTLINUX)  || defined(PTHPPA) || defined(PTNBSD_386)
#define SIGNAL_FN	void
#else
/* sequent, ultrix2, 4.3BSD (vax, hp), sunos3 */
#define SIGNAL_FN	int
#endif


/* Some systems have 'fixed' certain functions which used to be int */
#if defined(PTULTRIX) || defined(SABER) || defined(PTHPPA) || defined(PTAIX) || defined(PTLINUX) || defined(__STDC__) || defined(PTSOL2) || defined(PNBSD_386)
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
/* Sun CC (SC1.0) defines __cplusplus, but not __STDC__ */
#if !defined(__STDC__) && !defined(sprite)  && !defined(__cplusplus)
extern FILE *popen(), *tmpfile();
extern int pclose();
#ifndef clearerr	/* is a macro on many machines, but not all */
#if defined(PTIRIX5)
#if defined(__SYSTYPE_SVR4) || defined(SYSTYPE_SVR4)
/* SGI irix5 */
extern void	clearerr(FILE *);
#endif /* SVR4 */
#else  /* PTIRIX5 */
extern VOID_HACK clearerr();
#endif /* PTIRIX5 */
#endif /* clearerr */
#ifndef _IBMR2
#ifndef rewind
#ifndef PTIRIX5
#if !defined(PTSOL2) && !defined(__GNUC__)
extern VOID_HACK rewind();
#endif /* PTSOL2 */
#endif /* PTIRIX5 */
#endif /* rewind */
#endif /* _IBMR2 */
#endif /* __STDC__ */


/* most machines don't give us a header file for these */
#if defined(__STDC__) || defined(sprite) || defined(__cplusplus) || defined(PTSOL2) || defined(PTHPPA) || defined(PTIRIX5)
#include <stdlib.h>
#else

#if defined(PTHPPA)
extern int abort();
extern void free(), exit(), perror();
#else
#if defined(_IBMR2) && !defined(PTAIX_XLC)
extern int abort(), exit();
extern void free(), perror();
#else /* _IBMR2 && !PTAIX_XLC*/
#if defined(SYSV)
extern int abort();
extern void free();
extern void perror();
#else /* SYSV */
#if !defined(PTAIX_XLC)
extern VOID_HACK abort(), free(), exit(), perror();
#endif /* !PTAIX_XLC */
#endif /* SYSV */
#endif /* _IBMR2 */
extern char *getenv();
#if defined(ultrix4) || defined(SYSV)
/* Hack for Ptolemy pigilib/local.h:  skip this if we've seen local.h
   already */
#ifndef _local_h
extern void *malloc(), *realloc(), *calloc();
#endif /* _local_h */
#else
#if !defined(PTAIX_XLC)
extern char *malloc(), *realloc(), *calloc();
#endif /* PTAIX_XLC */
#endif
#endif

#if defined(PTHPPA) || defined(aiws) || defined(PTLINUX) ||(defined(PTSOL2) || defined(PTNBSD_386) && ! defined(BSD)) 
extern int sprintf();
#else
#ifndef _IBMR2
#ifndef PTIRIX5
extern char *sprintf();
#endif /* PTIRIX5 */
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
#if defined(__STDC__) || defined(sprite) || defined(PTHPPA) || defined(__cplusplus) || defined(PTLINUX) || defined(PTNBSD_386) || defined(PTAIX)
#include <string.h>
#else
/* ANSI C string.h -- 1/11/88 Draft Standard */
#if defined(ultrix4)
#include <strings.h>
#else
#ifndef PTAIX_XLC
extern char *strcpy(), *strncpy(), *strcat(), *strncat(), *strerror();
extern char *strpbrk(), *strtok(), *strchr(), *strrchr(), *strstr();
extern int strcoll(), strncmp();
#endif /* PTAIX_XLC */
#ifndef PTIRIX5
#ifndef PTSOL2
#if !(defined(sun) && defined(__GNUC__)) && !defined(PTAIX_XLC)
extern int strxfrm(), strlen(), strspn(), strcspn();
#endif
#endif /* PTSOL2 */
#endif /* PTIRIX5 */
#ifndef PTAIX_XLC
extern char *memmove(), *memccpy(), *memchr(), *memcpy(), *memset();
extern int memcmp(), strcmp();
#endif /* PTAIX_XLC */
#endif /* ultrix4 */
#endif /* __STDC__ */

#ifdef lint
#undef putc			/* correct lint '_flsbuf' bug */
#endif /* lint */

#if defined(PTLINUX) || defined(PTNBSD_386)
#undef srandom
#undef random
#undef bzero
#endif

/* a few extras */
#if ! defined(_std_h)
#ifdef PTHPPA
extern VOID_HACK srand48();
extern long lrand48();
#define random() lrand48()
#define srandom(a) srand48(a)
#define bzero(a,b) memset(a, 0, b)
#else
#if !defined(PTALPHA)
/* Sun CC 2.1 requires the __cplusplus below */
#if !defined(PTIRIX5) && !defined(__cplusplus)
extern VOID_HACK srandom();
#endif
extern long random();
#endif /* ! PTALPHA */
#endif
#endif /* _std_h */

/*
**  _std_h is defined in /usr/local/lib/g++ -- include/std.h
*/

/* This causes errors on a Sun.
#if ! defined(_std_h)
#if defined(ultrix4) || defined(PTHPPA) || defined(PTLINUX) || defined(PTNBSD_36)
extern unsigned sleep();
#else
extern VOID_HACK sleep();
#endif
#endif  \* _std_h */

/* assertion macro */
#ifndef assert
#if defined(__STDC__) || defined(sprite) || defined(PTNBSD_386)
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
#if defined(__STDC__) || defined(POSIX) || defined(PTLINUX) || defined(PTNBSD_386)
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

#if defined(PTSOL2) || defined(PTHPPA) || defined(SUN4)
/* gcc-2.5.8 under Solaris2.3 with Openwindows3.3 prints warning
   messages while compiling octtools/Maxport/Maxport.c. 
   message was: warning: cast discards `const' from pointer target type
 */
#define XTSTRINGDEFINES
#endif


#ifdef PTSOL2
extern void qsort
	ARGS((void *, size_t, size_t, int (*)(CONST void *, CONST void *)));
extern int system(CONST char *);
/* vem/rpc/serverNet.c and vem/rpc/vemRpc use gethostname() */
extern int gethostname( char * name, int namelen); 
#endif

#if defined(sun) && !defined(SOL2)
#include <unistd.h>
/* Don't include memset, it should be /usr/include/memory.h */
/*extern char * memset ARGS(( char *, char *, int));*/
#include <memory.h>

/* Don't include a declaration for qsort, include stdlib.h instead */
#include <stdlib.h>
/*extern void qsort
	ARGS((void *, size_t, size_t, int (*)(const void *, const void *)));*/
#endif /* PTSOL2 */

#ifdef PTIRIX5
/* The vfork man page says:
 *   vfork is no longer supported in IRIX as of Release 4.0.  By default, IRIX 
 *   does not preallocate swap space and thus fork(2) provides the same
 *   performance advantages as vfork.
 */
#define vfork() fork()
#endif /* PTIRIX5 */

#endif /* PORT_H */
