/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
#ifndef _local_h
#define _local_h 1

/* local.h  edg
Version identification:
$Id$
This is a local include file for useful macros and definitions.
*/

#include "ansi.h"
#ifdef HAS_STDARG
#include <stddef.h>
#define RAW_PTR void *
#else
#define RAW_PTR char *
#endif

/* boolean data type */
typedef int boolean;
#define TRUE	1
#define FALSE	0

#define EOS '\0' /* end of string */

extern RAW_PTR calloc ARGS((size_t,size_t));
extern RAW_PTR malloc ARGS((size_t));

#include <sys/param.h>

#ifdef USG
extern int sprintf ARGS((char*, const char*, ...));
extern char *getcwd ARGS((char *,size_t));
#define getwd(foo) getcwd(foo,MAXPATHLEN-1)
#else
extern char *sprintf ARGS((char*, const char*, ...));
extern char *getwd ARGS((char *));
#endif

/* buffer length constants */
#define MSG_BUF_MAX 512 /* for messages */

#endif
