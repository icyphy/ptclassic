/* 
    topMsg.c

    TOP provides the top-level of functionality to applications.
    This module provides sub-packages with a clean interface
    to output textual information to the user.

    The sub-packages make calls to these functions.  These functions
    then call an application-defined function to perform the real work.
    This allows the same sub-packages to funcition "nicely" in both
    tty-based and X-based applications.

    Author: Kennard White

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
#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <stdarg.h>
#include <errno.h>

#include "topStd.h"
#include "topMem.h"
#include "topBuf.h"
#include "topMisc.h"


/*global*/ int TOPDebugMode = 0;
static TOPMsgHandlerFunc *TOPMsgHandler = NULL;

void topMsgSetDbgMode( int d) {
    TOPDebugMode = d;
}

/*global*/ char* TOPAppName = NULL;

void
topSetAppName( char *name) {
    MEM_FREE(TOPAppName);
    TOPAppName = memStrSave( name);
}

char*
topGetAppName() {
    return TOPAppName!=NULL ? TOPAppName : "<unknown>";
}

void topWriteErrorMsg( char *str) {
    write( 2, str, strlen(str));
}

TOPMsgHandlerFunc*
topMsgSetHandler( TOPMsgHandlerFunc *pNewFunc) {
    TOPMsgHandlerFunc	*pOldFunc = TOPMsgHandler;
    TOPMsgHandler = pNewFunc;
    return pOldFunc;
}

void topMsg( TOPMask flags, char *msg) {
    char		*buf;
    int			n;

    if ( msg == NULL || *msg == NULL ) {
	buf = topBufStrcpy( "(Unknown null message)\n");
    } else {
        buf = topBufStrcpy( msg);
	n = strlen( buf);
	if ( n > 0 && buf[n-1] != '\n' ) {
	    buf[n] = '\n';
	    buf[n+1] = NULL;
	}
    }

    if ( flags & TOP_MSG_OS ) {
	buf = topBufFmt( "%s: %s", topStrError(errno), buf);
    }
    if ( flags & TOP_MSG_WARN ) {
	buf = topBufFmt( "Warning: %s", buf);
    } else if ( flags & TOP_MSG_ERR ) {
	buf = topBufFmt( "Error: %s", buf);
    } else if ( flags & TOP_MSG_DBG ) {
	buf = topBufFmt( "Debug: %s", buf);
    }
    if ( flags & TOP_MSG_PROG ) {
	buf = topBufFmt( "Error in program: %s", buf);
    }
    if ( flags & TOP_MSG_DIE ) {
	/* TOP_MSG_BUFPADSIZE allows this */
	strcat( buf, "This is a fatal condition. Program is aborting...\n");
    }

    if ( TOPMsgHandler == NULL || (flags & TOP_MSG_DBG) ) {
        fputs( buf, stdout);
    } else {
	(*TOPMsgHandler)( flags, buf);
    }

    if ( flags & TOP_MSG_DIE ) {
        abort();
	exit( 1 );
    }
}

void topMsgVa( TOPMask flags, char *fmt, va_list args) {
    char *buf = topBufLFmtVa( fmt, args);
    topMsg( flags, buf);
    topBufUnlock( buf);
}


void topMsgFmt( TOPMask flags, char *fmt, ...) {
    va_list args;

    if ( (flags & TOP_MSG_DBG) && TOPDebugMode == 0 ) {
	return;
    }
    va_start( args, fmt);
    topMsgVa( flags, fmt, args);
    va_end( args);
}

void topDbgMsg( char *fmt, ...) {
    va_list args;

    if ( TOPDebugMode == 0 ) {
	return;
    }
    va_start( args, fmt);
    topMsgVa( TOP_MSG_DBG, fmt, args);
    va_end( args);
}

void topInfoMsg( char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    topMsgVa( TOP_MSG_INFO, fmt, args);
    va_end( args);
}

void topWarnMsg( char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    topMsgVa( TOP_MSG_WARN, fmt, args);
    va_end( args);
}

void topWarnSysMsg( char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    topMsgVa( TOP_MSG_WARN|TOP_MSG_OS, fmt, args);
    va_end( args);
}

void topErrMsg( char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    topMsgVa( TOP_MSG_ERR, fmt, args);
    va_end( args);
}

void topErrSysMsg( char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    topMsgVa( TOP_MSG_ERR|TOP_MSG_OS, fmt, args);
    va_end( args);
}

void topProgMsg( char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    /* defaults to error-class message */
    topMsgVa( TOP_MSG_ERR|TOP_MSG_PROG, fmt, args);
    va_end( args);
}

void topAbortMsg( char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    topMsgVa( TOP_MSG_ERR|TOP_MSG_PROG|TOP_MSG_DIE, fmt, args);
    va_end( args);
}

void topAssertMsg( char *pkg, char *file, int line) {
    topAbortMsg( "Assertion failed: package ``%s'', file ``%s'', line %d",
      pkg, file, line);
    exit(1);	/* just to make sure */
}

void topNullMsg( char *fmt, ...) {
    ;
}
