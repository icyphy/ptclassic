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
extern char *sprintf();
#endif /*sun && __GNUC__*/
#endif /*ultrix*/
#endif /*SOL2*/
#endif /*sgi*/
#endif
#endif /*linux*/

#ifdef __GNUC__

#include <stdio.h>		/* Get the decl for FILE.  sigh.
				 * Also get sprintf() for linux. */
/* Alphabetical, please */
extern int fclose (FILE *);
extern int fflush (FILE *);
extern int fprintf (FILE *, const char *, ...);
extern int fscanf (FILE *, const char *, ...);
/* gcc-2.5.8 under SunOS4.1.3 can't deal with a full declaration of
 * pclose, or we get an undefined symbol:   _pclose__FP6_iobuf 
 * cg/kernel/CGUtilities.cc uses pclose().
 */
/*extern int pclose(FILE *);*/
/*extern int pclose(...);*/
extern void perror (const char *);
extern int printf (const char *, ...);
#if !defined(hppa) && !defined(__hppa__) && !defined(linux)
extern int putenv (char *);
#endif
extern int puts (const char *);
extern int sscanf (const char *, const char *, ...);
extern int unlink(const char *);
#endif /* __GNUC__ */

#ifdef __cplusplus
}
#endif

#endif /* PT_COMPAT_H */
