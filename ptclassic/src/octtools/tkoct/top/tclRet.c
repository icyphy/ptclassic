/* 
    tclRet.c

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
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
