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
/* ANSI Compatible stdlib.h stub */

#ifndef __cplusplus

#include <sys/types.h>

#ifndef ultrix4
extern double atof(const char *);
extern int atoi(const char *);
extern long atol(const char *);
extern void abort(void);
extern void *calloc(size_t, size_t);
extern void exit(int);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern char *getenv(const char *);
#else
extern double atof( const char *);
extern int atoi( const char *);
extern long atol( const char *);
extern void abort(void);
extern void *calloc(size_t, size_t);
extern void exit(int);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern char *getenv(char *);
#endif

/* should be in stdio.h */
extern void perror( const char *);

#ifdef LINT
#undef putc
#endif

#endif
