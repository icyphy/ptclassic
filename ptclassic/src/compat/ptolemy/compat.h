/*****************************************************************
Version identification:
$Id$

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
  
#if defined(hpux) || defined(__hpux)
/* HP PA, cfront or g++ */
#define PTHPPA
#endif

#if defined(PTHPPA) && ! defined(__GNUC__)
/* HP PA, cfront only, not g++ */
#define PTHPPA_CFRONT
#endif

#if defined(__sgi)
/* SGI running IRIX5.x */
#define PTIRIX5
#endif

#if defined(PTHPPA) && ! defined(__GNUC__)
/* SGI running IRIX5.x with native SGI CC*/
#define PTIRIX5_CFRONT
#endif

#if defined(linux)
#define PTLINUX
#endif

#if defined(netbsd_i386)
#define PTNBSD_386
#endif

#if defined(__sparc) && (defined(__svr4__) || defined(__SVR4))
/* Sun SPARC running Solaris2.x, SunC++ or g++ */
#ifndef SOL2
#define SOL2
#endif

#define PTSOL2
#endif

#if defined(__sparc) && (defined(__svr4__) || defined(__SVR4)) && !defined(__GNUC__)
/* Sun SPARC running Solaris2.x, with something other than gcc/g++ */
#define PTSOL2_CFRONT
#endif

#if defined(__sparc) && !defined(__svr4__)
/* Really, we mean sun4 running SunOs4.1.x, Sun C++ or g++ */
#define SUN4
#define PTSUN4
#endif

#if defined(__sparc) && !defined(__svr4__) && !defined(__GNUC__)
/* Really, we mean sun4 running SunOs4.1.x with something other than gcc/g++ */
#define SUN4
#define PTSUN4_CFRONT
#endif

#if defined(ultrix)
/* DEC MIPS running Ultrix4.x */
#define PTULTRIX
#define PTMIPS
#endif

/***************************************************************************/
/* Used to keep function paramters straight.  Note that SunOS4.1.3 cc
 *  is non ANSI, so we can't use function paramaters everywhere.
 */
#ifndef ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args)	args
#else
#define ARGS(args)	()
#endif
#endif

/* Volatile produces warnings under some cfront versions */
#if !defined(VOLATILE)
#if defined(PTHPPA_CFRONT) || defined(PTSUN4_CFRONT)
#define VOLATILE
#else
#define VOLATILE volatile
#endif /* PTSUN4_CFRONT or PTHPPA_CFRONT */
#endif /* ! VOLATILE */

#if !defined(PTLINUX) && !defined(PTNBSD_386)
#if defined(USG) && ! defined(PTHPPA) && ! defined(PTAIX)
extern int sprintf();
#else
#ifndef PTIRIX5
#ifndef PTSOL2
#ifndef PTULTRIX
#ifndef PTHPPA
#ifndef PTAIX
#ifndef PTALPHA
#if !(defined(sun) && defined (__GNUC__)) && !defined(hppa) && !defined(__hppa__)
#if defined(sun) && !defined(__GNUC__) && defined(__cplusplus) && !defined(SOL2) && 
/* SunOS4.1.3 Sun Cfront */	
#else
extern char *sprintf();
#endif
#endif /*sun && __GNUC__*/
#endif /* PTALPHA */
#endif /* PTAIX */
#endif /* PTHPPA */
#endif /* PTULTRIX */
#endif /* PTSOL2 */
#endif /* PTIRIX5 */
#endif
#endif /* PTLINUX */

#if defined(__GNUC__) && !defined(PTNBSD_386)

#include <stdio.h>			/* Get the decl for FILE.  sigh.
					 * Also get sprintf() for linux. */
#include <sys/types.h>			/* Need for bind(). */

#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H		/* mips Ultrix4.3A requires this
				   otherwise we get conflicts with
				   thor/kernel/rpc.c and ptkNet.c */
#include <sys/socket.h>
#endif

#ifdef PTSUN4
#include <sys/time.h>			/* For select() */
/* Place all SunOS4.1.3 specific declarations here. (In alphabetical order). */

extern int accept(int, struct sockaddr *, int *);
extern void bzero(char *, int); 	/* vem/rpc/server.c */

extern int fgetc(FILE *);
extern int fputs(const char *, FILE *);

/* pxgraph/xgraph.c, octtools/Xpackages/rpc/{appOct.c,rpc.c} all call fread()*/
extern long unsigned int 
fread(void *, long unsigned int, long unsigned int, FILE *);
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

#if !defined(PTIRIX5) && !defined(PTHPPA) && ! defined(PTALPHA)
				/* thor/kernel/rpc.c use bind2(), listen(). */
extern int bind(int, struct sockaddr *, int);
#endif /* ! PTIRIX5 && ! PTHPPA */

extern void endpwent();		/* octtools/Packages/fc/fc.c and
				   octtools/Packages/utility/texpand.c */

extern int fclose (FILE *);
extern int fflush (FILE *);
extern int fprintf (FILE *, const char *, ...);
extern int fscanf (FILE *, const char *, ...);

#ifdef PTSOL2
extern double hypot(double, double); /* kernel/ComplexSubset.h */
#endif

extern int listen(int, int);

#ifdef PTNBSD_386
extern off_t lseek();		/* octtools/vem/serverVem.c uses lseek(). */
#else
extern long lseek();
#endif /* PTNBSD_386 */

extern int pclose(FILE *);
extern void perror (const char *);
extern int printf (const char *, ...);

#if !defined(PTHPPA) && !defined(PTLINUX) && !defined(PTALPHA) && !defined(PTSOL2_4)
extern int putenv (char *);
#endif /* ! PTHPPA && ! PTLINUX && ! PTALPHA */

extern int puts (const char *);

#if defined(PTSUN4) || defined(PTULTRIX)
/* This should have been defined in sys/time.h.  kernel/SimControl.cc
   uses it.
 */
extern int setitimer( int, struct itimerval *, struct itimerval *);
#endif /* PTSUN4 || PTULTRIX */

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
#endif /* __GNUC__ */



#ifdef NEED_SYS_ERRLIST
#ifdef PTNBSD_386
extern const char *const sys_errlist[];
#else
extern char *sys_errlist[];
#endif /* PTNBSD_386 */
extern int sys_nerr;
extern int errno;
#endif /* NEED_SYS_ERRLIST */


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


/* Fix up casts for kernel/TimeVal.cc */

#ifdef PTHPPA
#define TV_SEC_TYPE	unsigned long
#endif /* PTHPPA */

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
#if defined(aiws) || defined(_IBMR2) || defined(SYSV) || defined(PTALPHA)
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
#if defined(PTHPPA) || defined(SYSV) || defined(PTLINUX) || defined(PTALPHA)
#define USE_SYSV_CURSES
#endif

/* Do we need to defined stricmp()?  See octtools/installColors/installColors.c
 */
#if defined(PT_ULTRIX) || defined(PTHPPA) || defined(PTIRIX5) || defined(PTSOL2) || defined(PTLINUX) || defined(PTALPHA) || defined(PTNBSD_386) || defined(PTAIX)
#define NEED_STRICMP
#endif

/* Do we have termios.h?  See octtools/Xpackages/iv/ivGetLine.c */
#if defined(PTHPPA) || defined(SYSV) || defined(PTIRIX5) || defined(PTLINUX)
#define HAS_TERMIOS
#endif

/* Is sys_siglist[] present?  See octtools/vem/rpc/vemRPC.c */
#if defined(PTHPPA) || defined(SYSV) || defined(PTLINUX)
#define NO_SYS_SIGLIST
#endif 

/* Should we use getrlimit()?  See octtools/vem/rpc/{server.c, serverNet.c} */
#if defined(PTHPPA) || defined(SYSV)
#define USE_GETRLIMIT
#endif

/* End of octtools specific defines */

#ifdef __cplusplus
}
#endif

#endif /* PT_COMPAT_H */
