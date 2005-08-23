/* Version Identification:
 * @(#)errtrap.h	1.3	09/22/98
 */
/*
Copyright (c) 1990-1998 The Regents of the University of California.
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
#ifndef ERRTRAP_H
#define ERRTRAP_H

#include "ansi.h"
#include <setjmp.h>

#define ERR_PKG_NAME	"errtrap"

OCT_EXTERN void errProgramName
	ARGS((char *progName));
OCT_EXTERN void errCore
	ARGS((int flag));
typedef void (*EH)
	ARGS((char *, int, char *));
OCT_EXTERN void errPushHandler
	ARGS((EH hndlr));
OCT_EXTERN void errPopHandler
	NULLARGS;
OCT_EXTERN void errRaise
	ARGS((char *pkgName, int code, char *format, ... ));
OCT_EXTERN void errPass
	ARGS((char *format, ... ));

#define ERR_IGNORE(expr)	\
    {					\
	if ( ! setjmp(errJmpBuf)) {	\
	    errIgnPush();		\
	    expr;			\
	}				\
	errIgnPop();			\
    }
extern jmp_buf errJmpBuf;
OCT_EXTERN void errIgnPush
	NULLARGS;
OCT_EXTERN void errIgnPop
	NULLARGS;
OCT_EXTERN int errStatus
	ARGS((char **pkgNamePtr, int *codePtr, char **messagePtr));

#endif /* ERRTRAP_H */
