#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
/*LINTLIBRARY*/
/*
 * Message Management Package
 *
 * David Harrison
 * University of California, 1985, 1989
 *
 * This file contains the implementation of the general purpose VEM
 * console message mechanism.
 */

#ifdef __STDC__
#include <stdarg.h>
#define _VA_LIST_
#else
#include <varargs.h>
#endif
#include "general.h"		/* General declarations for VEM     */
#include "message.h"		/* Self declaration                 */
#include "uprintf.h"		/* User-driven printf functionality */
#include "errtrap.h"		/* Error trapping                   */

char *msg_pkg_name = "message";

static Pointer curConsData = (Pointer) 0;	/* Current console data */
static void soWrite();		        /* Default console write  */
static void (*curConsFunc)() = soWrite; /* Current console write  */
static FILE *curLogFile = NIL(FILE);	/* Current logging file   */

char theArea[4096];
char *errMsgArea = theArea;

void vemMsgSetCons(conData, conWriteFunc)
Pointer conData;		/* Console window data                 */
void (*conWriteFunc)();		/* Function to write to console window */
/*
 * Sets (or resets) the current console window.  `conData' is 
 * arbitrary information passed to `conWriteFunc' which should
 * have the following form:
 *   void conWriteFunc(conData, str)
 *   Pointer conData;
 *   char *str;
 */
{
    curConsData = conData;
    curConsFunc = conWriteFunc;
}


vemStatus vemMsgFile(fileName)
STR fileName;		/* Name of file */
/*
 * This routine redirects the current VEM log file to the specified file.
 * Initially,  the log file is stderr.
 */
{
    FILE *newfile;

    newfile = fopen( util_file_expand(fileName), "w");
    if (!newfile) {
	return VEM_NOFILE;
    }
    if (curLogFile != NIL(FILE)) (void) fclose(curLogFile);
    curLogFile = newfile;
    return VEM_OK;
}

void vemMessage(msg, opt)
char *msg;			/* Null terminated string */
int opt;			/* Option bits            */
/*
 * Writes out a message to the current console window and/or the current
 * log file.  The options are:  MSG_NOLOG - don't write message to log file,
 * MSG_NODISP - do not display in console window,  MSG_MOREMD - use a
 * more (1) type display until a vemMessage where MSG_MOREMD is not
 * set.
 */
{
    /* Only partially implemented */
    if ((opt & MSG_NOLOG) == 0) {
	if (curLogFile == NIL(FILE)) {
	    if ((opt & MSG_DISP) == 0) {
		(void) fprintf(stderr, "%s", msg);
	    }
	} else {
	    (void) fprintf(curLogFile, "%s", msg);
	    (void) fflush(curLogFile);
	}
    }

    if (opt & MSG_DISP) {
	(*curConsFunc)(curConsData, msg);
    }
}

/*
 * Below is the new self-parsing version of vemMessage.  It handles
 * its own format string.
 */

#ifndef lint

#ifdef __STDC__
void vemMsg(int flags, char *fmt, ...)
#else
/*VARARGS2*/
void vemMsg(va_alist)
va_dcl
#endif
/*
 * Format: vemMsg(int flags, char *format, ...)
 * Writes out a message to the current console window and/or the current
 * log file.  The flags are MSG_C - write to console, MSG_L - write to
 * log file, MSG_A - write to both.
 */
{
    va_list ap;
#ifdef __STDC__
    va_start(ap, fmt);
#else
    char *fmt;
    int flags;

    va_start(ap);
    flags = va_arg(ap, int);
    fmt = va_arg(ap, char *);
#endif
    uprintf(errMsgArea, fmt, &ap);
    if (flags & MSG_L) {
	if (curLogFile == NIL(FILE)) {
	    if (flags & MSG_C) {
		(void) fprintf(stderr, "%s", errMsgArea);
	    }
	} else {
	    (void) fprintf(curLogFile, "%s", errMsgArea);
	    (void) fflush(curLogFile);
	}
    }
    if (flags & MSG_C) {
	(*curConsFunc)(curConsData, errMsgArea);
    }
}

#endif /* lint not defined */

/*ARGSUSED*/
static void soWrite(data, str)
Pointer data;
char *str;
/*
 * This is the default console error writing function.
 * It simply writes the message to stderr.
 */
{
    (void) fprintf(stderr, "%s", str);
}
