/**************************************************************************
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

#if defined(__sparc) && defined(__svr4__)
#define SOL2
#endif

#if defined(USG) || defined(linux)
extern int sprintf();
#else
#ifndef sgi
#ifndef SOL2
#ifndef ultrix
extern char *sprintf();
#endif /*ultrix*/
#endif /*SOL2*/
#endif /*sgi*/
#endif

#ifdef __GNUC__
/* Alphabetical, please */
extern int fclose (FILE *);
extern int fflush (FILE *);
extern int fprintf (FILE *, const char *, ...);
extern int fscanf (FILE *, const char *, ...);
extern void perror (const char *);
extern int printf (const char *, ...);
extern int putenv (char *);
extern int puts (const char *);
extern int sscanf (const char *, const char *, ...);
extern int unlink(const char *);
#endif /* __GNUC__ */

