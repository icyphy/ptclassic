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
+ 
#ifdef PTSVR4
/* 
* Yeah, this is ugly, but it's the only way to keep
* /usr/X/include/X11/memutil.h from overriding them. 
*/

#define REALLOC(type, obj, num)	\
    (obj) ? ((type *) realloc((char *) obj, sizeof(type) * (num))) : \
	    ((type *) malloc(sizeof(type) * (num)))
#define FREE(obj)		\
    if ((obj)) { (void) free((char *) (obj)); (obj) = 0; }
#endif /* PTSVR4 */

#ifndef UTILITY_H
#define UTILITY_H

#define UTILITY_PKG_NAME "utility"
/*
 *  assumes the memory manager is libmm.a
 *	- allows malloc(0) or realloc(obj, 0)
 *	- catches out of memory (and calls MMout_of_memory())
 *	- catch free(0) and realloc(0, size) in the macros
 */
#define NIL(type)		((type *) 0)
#define ALLOC(type, num)	\
    ((type *) malloc(sizeof(type) * (num)))
#define REALLOC(type, obj, num)	\
    (obj) ? ((type *) realloc((char *) obj, sizeof(type) * (num))) : \
	    ((type *) malloc(sizeof(type) * (num)))
#define FREE(obj)		\
    if ((obj)) { (void) free((char *) (obj)); (obj) = 0; }

#include "ansi.h"

EXTERN long  util_cpu_time
	NULLARGS;
EXTERN char *util_path_search
	ARGS((char *program));
EXTERN char *util_file_search
	ARGS((char *file, char *path, char *mode));
EXTERN int   util_pipefork
	ARGS((char **argv, FILE **toCommand, FILE **fromCommand, int *pid));
EXTERN int   util_csystem
	ARGS((char *command));
EXTERN char *util_print_time
	ARGS((long t));
EXTERN char *util_strsav
	ARGS((char *ptr));
EXTERN FILE* util_tmpfile
        NULLARGS;
EXTERN char *util_tilde_expand
	ARGS((char *filename));
EXTERN char *util_tilde_compress
	ARGS((char *filename));
EXTERN char *util_logical_expand
	ARGS((char *filename));
EXTERN char *util_logical_compress
	ARGS((char *filename, char* varname ));
EXTERN char *util_file_expand
	ARGS((char *filename));

EXTERN void util_register_user
	ARGS((char *user, char *directory));

EXTERN void util_setenv
        ARGS(( char *name, char *value ));
EXTERN void util_unsetenv
        ARGS(( char *name ));
EXTERN void util_printenv
        ARGS(( FILE *fp ));


/* Nicely format a double in engineering notation */
EXTERN char *util_pretty_print
        ARGS( ( double v ) );


#ifndef NIL_FN
#define NIL_FN(type) ((type (*)()) 0)
#endif /* NIL_FN */

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif /* MAX */
#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif /* MIN */
#ifndef ABS
#define ABS(a)		((a) > 0 ? (a) : -(a))
#endif /* ABS */


#ifdef lint
#undef ALLOC			/* allow for lint -h flag */
#undef REALLOC
#define ALLOC(type, num)	(((type *) 0) + (num))
#define REALLOC(type, obj, num)	((obj) + (num))
#endif /* lint */

#endif

