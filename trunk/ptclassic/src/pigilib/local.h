#ifndef _LOCAL_H
#define _LOCAL_H 1

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

/* local.h  edg
Version identification:
$Id$
This is a local include file for useful macros and definitions.
*/

#include "sol2compat.h"		/* define boolean */
#include "compat.h"		/* define architecture settings */
#include "ansi.h"		/* define ARGS, EXTERN, and HAS_STDARG */

#ifdef HAS_STDARG
#include <stddef.h>
#define RAW_PTR void *
#else
#ifdef hpux /* hppa.cfront requires this */
#define RAW_PTR void *
#else
#define RAW_PTR char *
#endif /* hpux */
#endif /* HAS_STDARG */

#define TRUE	1
#define FALSE	0

#define EOS '\0' /* end of string */

/* Sun CC (SC1.0) defines __cplusplus */
#if !defined(__cplusplus) && !defined(PTAIX_XLC)
extern RAW_PTR calloc ARGS((size_t,size_t));
extern RAW_PTR malloc ARGS((size_t));
#endif /* __cplusplus */

#include <sys/param.h>

#if defined(PTHPPA) && !defined(PTHPUX10)
/* HPUX10.20 will fail if the line below is defined, however HPUX9.x
 does not have getwd(). */
#define getwd(foo) getcwd(foo,MAXPATHLEN-1)
#else /* PTHPPA */
#if !defined(PTALPHA) && !defined(PTLINUX) && !defined(PTIRIX5) && !defined(PTHPPA)
extern char *getwd ARGS((char *));
#endif /* PTALPHA ... */
#endif /* PTHPPA */
  
/* buffer length for messages */
#define MSG_BUF_MAX 512

/* If a star is not in a domain, then we specify NODOMAIN as the domain.
   This is useful if we are trying to make an icon for a HTML file.
   MkStar() in icon.c and MkStarIcon() in mkIcon.c use NODOMAIN
 */
#define NODOMAIN "NODOMAIN"


#endif  /* _LOCAL_H */
