/* 
    tclRet.c

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
#include <tcl.h>
#include "topStd.h"
#include "topBuf.h"
#include "topTcl.h"

void topTclRet( Tcl_Interp *ip, char *msg) {
    Tcl_SetResult( ip, msg, TCL_VOLATILE);
}

void topTclRetVa( Tcl_Interp *ip, char *fmt, va_list args) {
    char	*buf;

    buf = topBufLFmtVa( fmt, args);
    topTclRet( ip, buf);
    topBufUnlock( buf);
}

void topTclRetFmt( Tcl_Interp *ip, char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    topTclRetVa( ip, fmt, args);
    va_end( args);
}

void topTclRetApnd( Tcl_Interp *ip, char *msg) {
    Tcl_AppendResult( ip, msg, NULL);
}

void topTclRetApndVa( Tcl_Interp *ip, char *fmt, va_list args) {
    char	*buf;

    buf = topBufLFmtVa( fmt, args);
    topTclRetApnd( ip, buf);
    topBufUnlock( buf);
}

void topTclRetApndFmt( Tcl_Interp *ip, char *fmt, ...) {
    va_list args;

    va_start( args, fmt);
    topTclRetApndVa( ip, fmt, args);
    va_end( args);
}
