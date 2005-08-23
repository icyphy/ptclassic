/*****************************************************************
Version identification:
@(#)compat.h	1.65 09/21/00

Copyright (c) 1990-1999 The Regents of the University of California.
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

#ifndef PT_COMPAT_H
#define PT_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
/* Define #defines for each Ptolemy architecture. (Alphabetical, please) */

#if defined(_AIX)
/* IBM rs6000 and powerpc running AIX */
#define PTAIX
#endif

#if defined(_AIX) && ! defined(__GNUC__)
/* AIX with IBM xlc and xlC */
#define PTAIX_XLC
#endif

#if defined(__alpha)
/* DEC Alpha */
#define PTALPHA
#endif
  
#if defined(freebsd)
/* FreeBSD */
#define PTFREEBSD
#endif

#if defined(hpux) || defined(__hpux)
/* HP PA, cfront or g++ */
#define PTHPPA
#endif

#if defined(PTHPPA) && ! defined(__GNUC__)
/* HP PA, cfront only, not g++ */
#define PTHPPA_CFRONT
#endif

#if defined(__sgi) && defined(IRIX5)
/* SGI running IRIX5.x */
#define PTIRIX5
#endif

#if defined(__sgi) && ! defined(__GNUC__)
/* SGI running IRIX5.x with native SGI CC*/
#define PTIRIX5_CFRONT
#endif

#if defined(__sgi) && ! defined(__GNUC__) && defined(IRIX6) && defined (_ABIO32)
/* SGI running IRIX6.x , native SGI CC in 32 bits mode */
#define PTIRIX5			/* SGI says it is 100% compatible with IRIX5 */
#define PTIRIX6
#define PTIRIX6_32_CFRONT
#endif

#if defined(__sgi) && ! defined(__GNUC__) && defined(IRIX6) && defined (_ABI64)
/* SGI running IRIX6.x , native SGI CC in 64 bit mode */
#define PTIRIX6
#define PTIRIX6_64_CFRONT
#endif

#if defined(linux)
#define PTLINUX
#endif

#if defined(PTLINUX) && defined(__ELF__)
/* Shorthand for Linux and ELF */
#define PTLINUX_ELF
#endif

#if defined(netbsd_i386)
#define PTNBSD_386
#endif

#if defined(i386) && (defined(__svr4__) || defined(__SVR4))
/* Intelx86 PC running Solaris2.x, SunC++ or g++ */
#ifndef SOL2
#define SOL2
#endif
 
#define PTSOL2
#define PTSOL2PC
#endif

#if defined(sparc) && (defined(__svr4__) || defined(__SVR4))
/* Sun SPARC running Solaris2.x, SunC++ or g++ */
#ifndef SOL2
#define SOL2
#endif

#define PTSOL2
#endif

#if defined(sparc) && (defined(__svr4__) || defined(__SVR4)) && !defined(__GNUC__)
/* Sun SPARC running Solaris2.x, with something other than gcc/g++ */
#define PTSOL2_CFRONT
#endif

#if defined(sparc) && !(defined(__svr4__) || defined(PTSOL2_CFRONT))
/* Really, we mean sun4 running SunOs4.1.x, Sun C++ or g++ */
#define SUN4
#define PTSUN4
#endif

#if defined(sparc) && !defined(__svr4__) && !defined(__GNUC__)
/* Really, we mean sun4 running SunOs4.1.x with something other than gcc/g++ */
#define SUN4
#define PTSUN4_CFRONT
#endif

/* Unixware1.1
*#define PTSVR4
*/

#if defined(ultrix)
/* DEC MIPS running Ultrix4.x */
#define PTULTRIX
#define PTMIPS
#endif

/* Definition of common 64-bit preprocessor symbols */
#if defined(PTALPHA) || defined(PTIRIX6)
#define PT64BIT
#endif

/***************************************************************************/
/* Used to keep function paramters straight.  Note that SunOS4.1.3 cc
 *  is non ANSI, so we can't use function parameters everywhere.
 */
#ifndef ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args)	args
#else
#define ARGS(args)	()
#endif
#endif

/* Volatile produces warnings under some cfront versions, and under
 * non-ansi cc compilers, such as SunOS4.1 cc
 */
#if !defined(VOLATILE)
#if defined(PTHPPA_CFRONT) || defined(PTSUN4_CFRONT) || !(defined(__STDC__) || defined(__cplusplus))
#define VOLATILE
#else
#define VOLATILE volatile
#endif /* PTSUN4_CFRONT or PTHPPA_CFRONT or non-ansi cc*/
#endif /* ! VOLATILE */

/* This is really lame, so lets just not use it.
 */
#ifdef NEVER
#if !defined(PTLINUX) && !defined(PTNBSD_386) && !defined(PTFREEBSD)
#if defined(USG) && ! defined(PTHPPA) && ! defined(PTAIX) && !defined(PTSVR4) && !defined(PTSOL2)
extern int sprintf();
#else
#ifndef PTIRIX5
#ifndef PTSOL2
#ifndef PTULTRIX
#ifndef PTHPPA
#ifndef PTAIX
#ifndef PTALPHA
#ifndef PTSVR4
#if !(defined(sun) && defined (__GNUC__)) && !defined(hppa) && !defined(__hppa__)
#if defined(sun) && !defined(__GNUC__) && defined(__cplusplus) && !defined(SOL2)
/* SunOS4.1.3 Sun Cfront */	
#else
extern char *sprintf();
#endif
#endif /*sun && __GNUC__*/
#endif /* PTSVR4 */
#endif /* PTALPHA */
#endif /* PTAIX */
#endif /* PTHPPA */
#endif /* PTULTRIX */
#endif /* PTSOL2 */
#endif /* PTIRIX5 */
#endif
#endif /* PTLINUX */
#endif /* NEVER */

#if defined(__GNUC__) && !defined(PTNBSD_386) && !defined(PTFREEBSD)

#include <stdio.h>			/* Get the decl for FILE.  sigh.
					 * Also get sprintf() for linux. */
#include <sys/types.h>			/* Need for bind(). */

#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H		/* mips Ultrix4.3A requires this
				   otherwise we get conflicts with
				   thor/kernel/rpc.c and ptkNet.c */
#include <sys/socket.h>
#endif

#ifdef PTSOL2
  /* /usr/openwin/include/Xib.h */
#define FUNCPROTO 1
#endif

#ifdef PTSUN4
#include <sys/time.h>			/* For select() */
/* Place all SunOS4.1.3 specific declarations here. (In alphabetical order). */

extern int accept(int, struct sockaddr *, int *);
extern void bzero(char *, int); 	/* vem/rpc/server.c */

extern int fgetc(FILE *);
extern int fputs(const char *, FILE *);

/* pxgraph/xgraph.c, octtools/Xpackages/rpc/{appOct.c,rpc.c},
   utils/libptdsp/ptdspMuLaw.c all call fread()*/
extern long unsigned int 
fread(void *, long unsigned int, long unsigned int, FILE *);

/* utils/libptdsp/ptdspMuLaw.c uses fseek */
int fseek(FILE *, long, int);

extern long unsigned int 
fwrite(const void *, long unsigned int, long unsigned int, FILE *);

extern int getdtablesize();		/* vem/rpc/server.c */

extern int gethostname(char *, int);	/* vem/rcp/serverNet.c (among others)*/

/* Don't define getsockname for SOL2.  thor/kernel/rpc.c calls getsockname().*/
extern int getsockname(int, struct sockaddr *, int *);

/* octtools/Xpackages/iv/ivGetLine.c calls ioctl */
extern int ioctl(int, int request, caddr_t);

/* octtools/Xpackages/rpc/{appNet.c,serverNet.c} thor/analyzer/X11/event.c
   all call select */
extern select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

/* Under SunOS4.1.3, strcasecmp is in strings.h, not string.h */
/* octtools/Packages/tap/quantize.c uses strcasecmp */
extern int strcasecmp(const char *, const char *);

extern int system(const char *);

/* Under SunOS4.1.3, tolower() and toupper are not in ctype.h */
/* vem/strings/str.c (and other files ) use tolower() and toupper(). */
extern int tolower(int c);
extern int toupper(int c);

extern int ungetc(int, FILE *);
extern int vfork();
extern int vfprintf(FILE *, const char *, char *);

#endif /* SUN4 */

/* IBM RS6000 running AIX */
#ifdef PTAIX
#include <sys/select.h>
#endif


/* Here we define common missing function prototypes */
/* Alphabetical, please */

#if !defined(PTIRIX5) && !defined(PTHPPA) && ! defined(PTALPHA) && !defined(PTLINUX)
				/* thor/kernel/rpc.c use bind2(), listen(). */
#if defined(PTFREEBSD) || defined(PTSOL2_6) && ! defined(PTSOL7)
/* Under linux and libc-5.2.18, bind() takes a const second arg */
extern int bind(int, const struct sockaddr *, int);
#else /* PTFREEBSD || PTSOL2_6 && ! PTSOL7 */
#if ! defined(PTSOL7)
extern int bind(int, struct sockaddr *, int);
#endif /* ! PTSOL7 */
#endif /* PTFREEBSD || PTSOL2_6 && ! PTSOL7 */
#endif /* ! PTIRIX5 && ! PTHPPA && ! PTALPHA && ! PTLINUX*/

extern void endpwent();		/* octtools/Packages/fc/fc.c and
				   octtools/Packages/utility/texpand.c */

extern int fclose (FILE *);
extern int fflush (FILE *);
extern int fprintf (FILE *, const char *, ...);
extern int fscanf (FILE *, const char *, ...);

#ifdef PTSOL2
extern double hypot(double, double); /* kernel/ComplexSubset.h */
#endif

#if !defined( __GLIBC__) || (__GLIBC__ < 2)
extern int listen(int, int);
#endif

#if defined(PTNBSD_386) || defined(PTFREEBSD)
extern off_t lseek();		/* octtools/vem/serverVem.c uses lseek(). */
#else
#if defined(PTHPPA) && !defined(PTHPPA_CFRONT) && defined(PTHPUX10)
/* Under HPUX-10.20 and gcc-2.7.2, don't define lseek */
#else
extern long lseek();
#endif /* PTHPPA && ! PTHPPA_CFRONT */
#endif /* PTNBSD_386 || PTFREEBSD */

extern int pclose(FILE *);
extern void perror (const char *);
extern int printf (const char *, ...);

#if !defined(PTHPPA) && !defined(PTLINUX) && !defined(PTALPHA) && !(defined(PTSOL2) && !defined(PTSOL2_3)) && !defined(PTSOL2_4) && !defined(PTNT)
extern int putenv (char *);
#endif /* ! PTHPPA && ! PTLINUX && ! PTALPHA */

extern int puts (const char *);

#if defined(PTSUN4) || defined(PTULTRIX)
/* This should have been defined in sys/time.h.  kernel/SimControl.cc
   uses it.
 */
extern int setitimer( int, struct itimerval *, struct itimerval *);
#endif /* PTSUN4 || PTULTRIX */

#if defined(PTLINUX)
#include <sys/time.h>		/* for select() */
#endif

#if !defined(PTALPHA)
extern void setpwent();		/* octtools/Packages/fc/fc.c and
				   octtools/Packages/utility/texpand.c */
#endif

extern int sscanf (const char *, const char *, ...);
extern int socket(int, int, int); /* thor/kernel/rpc.c uses socket() */

#if ! defined(PTAIX)
extern int symlink(const char *, const char *);	/* CGCTarget.cc */
#endif

extern int unlink(const char *);

/* End of common missing function prototypes */
#else  /* !__GNUC__ */
#ifdef PTIRIX5_CFRONT           /* dpwe addns */
#include <unistd.h>             /* for select() */
#include <stdio.h>              /* for sprintf() */
#endif /* PTIRIX5_CFRONT */
#endif /* __GNUC__ */


#ifdef NEED_SYS_ERRLIST
#if defined(PTNBSD_386) || defined(PTFREEBSD)
/* See also kernel/State.h */
extern const int sys_nerr;
extern const char *const sys_errlist[];
extern int errno;
#else
#ifdef PT_NT4VC
#include <stdlib.h>
#include <errno.h>
#else /* PT_NT4VC */
#if defined  (__CYGWIN__)
#define sys_nerr _sys_nerr
#define sys_errlist _sys_errlist
#include <errno.h>
#else /* __CYGWIN__ */
#if defined (__GLIBC__) && (__GLIBC__ >= 2)
#include <errno.h>
#else
extern int sys_nerr;
extern char *sys_errlist[];
extern int errno;
#endif /* (__GLIBC__) && (__GLIBC__ >= 2)*/
#endif /* __CYGWIN__ */
#endif /* PTNBSD_386 */
#endif /* PT_NT4VC */
#endif /* NEED_SYS_ERRLIST */

#ifdef NEED_TIMEVAL
/* See kernel/Clock.cc */
#ifdef PT_NT4VC
#include <winsock.h>
#endif
#endif /* NEED_TIMEVAL */

/* Under NT, we need to open files in either textmode or binarymode.
 * In theory, modern Unixes should be able to handle "wb", but why try?
 */
#ifdef PTNT
#define PT_FOPEN_WRITE_BINARY "wb"
#define PT_FOPEN_READ_BINARY "rb"
#else
#define PT_FOPEN_WRITE_BINARY "w"
#define PT_FOPEN_READ_BINARY "r"
#endif    

#ifdef PTNT
/* src/kernel/TimeVal.cc uses timercmp */
#define	timercmp(tvp, uvp, cmp) \
	/* CSTYLED */ \
	((tvp)->tv_sec cmp (uvp)->tv_sec || \
	((tvp)->tv_sec == (uvp)->tv_sec && \
	/* CSTYLED */ \
	(tvp)->tv_usec cmp (uvp)->tv_usec))
#endif /* PT_NT */

/* Here we define common types that differ by platform */

/* thor/analyzerX11/event.c, octtools/vem/rpc/serverNet.c,
 octtools/Xpackages/rpc/appNet.c use FDS_TYPE */
#ifdef PTHPPA

#ifndef FDS_TYPE
#define FDS_TYPE (int *)
#endif

#else /* PTHPPA */

#ifndef FDS_TYPE
#define FDS_TYPE (fd_set *)
#endif

#endif /* PTHPPA */


#ifdef PTSVR4
#define FDS_TYPE (fd_set *)
#define SYSV
#endif /* PTSVR4 */

/* Fix up casts for kernel/TimeVal.cc */

#if defined(PTHPPA) && defined(PTHPUX10)
#define TV_SEC_TYPE	time_t
#else
#if defined (PTHPPA)
#define TV_SEC_TYPE	unsigned long
#endif /* PTHPPA */
#endif /* PTHPPA && PTHPUX10*/

#ifndef TV_USEC_TYPE
#define TV_USEC_TYPE	long int
#endif

#ifndef TV_SEC_TYPE
#define TV_SEC_TYPE	long int
#endif

/* End of common types that differ by platform */

/* Start of octtools specific defines */
/* Decide whether to include dirent.h or sys/dir.h.
 * octtools/Packages/fc.h uses this
 */
#if defined(aiws) || defined(_IBMR2) || defined(SYSV) || defined(PTALPHA) || defined(PTSOL2PC)
#define USE_DIRENT_H
#endif

/* Does wait() take an int* arg or a union wait * arg
 * DEC Ultrix wait() takes union wait *
 * See octtools/Packages/utility/csystem.c
 */
#if defined(_IBMR2) || defined(SYSV) || defined(PTALPHA) || defined(PTHPPA) ||defined(PTSUN4) || defined(PTSOL2) || defined(PTIRIX5)
#define WAIT_TAKES_INT_STAR
#endif

/* Use wait3 or waitPid()?
 * Used in octtools/Packages/utils/pipefork.c, vov/lib.c
 */
#if defined(SYSV) || defined(PTALPHA)
#define USE_WAITPID
#endif

/* Use SystemV curses?  See octtools/attache/io.c
 */
#if defined(PTHPPA) || defined(SYSV) || defined(PTLINUX) || defined(PTALPHA) || defined(PTFREEBSD) ||defined(PTNBSD_386)
#define USE_SYSV_CURSES
#endif

/* Do we need to defined stricmp()?  See octtools/installColors/installColors.c
 */
#if defined(PT_ULTRIX) || defined(PTHPPA) || defined(PTIRIX5) || defined(PTSOL2) || defined(PTLINUX) || defined(PTALPHA) || defined(PTNBSD_386) || defined(PTAIX) || defined(PTFREEBSD)
#define NEED_STRICMP
#endif

/* Do we have termios.h?  See octtools/Xpackages/iv/ivGetLine.c */
#if defined(PTHPPA) || defined(SYSV) || defined(PTIRIX5) || defined(PTLINUX) || defined(PTFREEBSD) || defined(PTNBSD_386) || defined(PTSOL2PC)
#define HAS_TERMIOS
#endif

/* Is sys_siglist[] present?  See octtools/vem/rpc/vemRPC.c */
#if defined(PTHPPA) || defined(SYSV) || defined(PTLINUX) || defined(PTFREEBSD) || defined(PTNBSD_386) || defined(PTSOL2PC)
#define NO_SYS_SIGLIST
#endif 

/* Should we use getrlimit()?  See octtools/vem/rpc/{server.c, serverNet.c} */
#if defined(PTHPPA) || defined(SYSV)
#define USE_GETRLIMIT
#endif

/* Is char* environ defined? See octtools/Packages/vov/lib.c */
#if defined(PTHPPA) || defined(PTLINUX)
/* Under PTHPPA and PTLINUX, we already have environ declaration. */
#else
#define NEED_ENVIRON_DECL
#endif

/* End of octtools specific defines */

#ifdef __cplusplus
}
#endif

#endif /* PT_COMPAT_H */
