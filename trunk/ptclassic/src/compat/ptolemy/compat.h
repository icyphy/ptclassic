/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/

#ifndef PT_COMPAT_H
#define PT_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__sparc) && defined(__svr4__)
#define SOL2
#endif

#if defined(__sparc) && !defined(__svr4__)
/* Really, we mean sun4 running sunos4.1.x */
#define SUN4
#endif

#ifndef ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args)	args
#else
#define ARGS(args)	()
#endif
#endif

#ifndef linux
#if defined(USG) 
extern int sprintf();
#else
#ifndef sgi
#ifndef SOL2
#ifndef ultrix
#if !(defined(sun) && defined (__GNUC__)) && !defined(hppa) && !defined(__hppa__)
#if defined(sun) && !defined(__GNUC__) && defined(__cplusplus) && !defined(SOL2)
/* SunOS4.1.3 Sun Cfront */	
#else
extern char *sprintf();
#endif
#endif /*sun && __GNUC__*/
#endif /*ultrix*/
#endif /*SOL2*/
#endif /*sgi*/
#endif
#endif /*linux*/

#ifdef __GNUC__

#include <stdio.h>			/* Get the decl for FILE.  sigh.
					 * Also get sprintf() for linux. */
#include <sys/types.h>			/* Need for bind(). */
#include <sys/socket.h>			/* Need for bind(). */

#ifdef SUN4
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

/* Alphabetical, please */
				/* thor/kernel/rpc.c use bind(), listen(). */
extern int bind(int, struct sockaddr *, int);

extern void endpwent();		/* octtools/Packages/fc/fc.c and
				   octtools/Packages/utility/texpand.c */

extern int fclose (FILE *);
extern int fflush (FILE *);
extern int fprintf (FILE *, const char *, ...);
extern int fscanf (FILE *, const char *, ...);
extern int listen(int, int);
extern int pclose(FILE *);
extern void perror (const char *);
extern int printf (const char *, ...);
#if !defined(hppa) && !defined(__hppa__) && !defined(linux)
extern int putenv (char *);
#endif
extern int puts (const char *);

extern void setpwent();		/* octtools/Packages/fc/fc.c and
				   octtools/Packages/utility/texpand.c */

extern int sscanf (const char *, const char *, ...);
extern int socket(int, int, int); /* thor/kernel/rpc.c uses socket() */
extern int unlink(const char *);
#endif /* __GNUC__ */

#ifdef __cplusplus
}
#endif

#endif /* PT_COMPAT_H */
