/* 
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
#include <stdio.h>
#include <stdarg.h>
#include <tcl.h>

#include "topStd.h"
#include "topMem.h"
#include "topBuf.h"
#include "topTcl.h"

static Tcl_Interp *TOPTclMsgInterp = NULL;
static int TOPTclMsgCnt = 0;
static TOPMask TOPTclMsgFlags = 0;
static TOPMsgHandlerFunc *TOPTclMsgOldHandler = NULL;

static void
_topTclCaptureMsgsCB( TOPMask flags, char* msg) {
    TOPTclMsgFlags |= flags;
    Tcl_AppendResult(TOPTclMsgInterp,"\n",msg,NULL);
}

void
topTclCaptureMsgsBegin( Tcl_Interp *ip) {
    if ( TOPTclMsgCnt++ != 0 )
	return;
    TOPTclMsgInterp = ip;
    TOPTclMsgFlags = 0;
    TOPTclMsgOldHandler = topMsgSetHandler(_topTclCaptureMsgsCB);
}

int
topTclCaptureMsgsEnd( Tcl_Interp *ip) {
    TOPMask	flags;

    if ( TOPTclMsgCnt <= 0 ) {
	topMsgSetHandler(NULL);
	topAbortMsg("Unbalanced topTclCaptureMsgs (cnt)");
    }
    if ( --TOPTclMsgCnt != 0 )
	return 0;
    if ( ip != TOPTclMsgInterp ) {
	topMsgSetHandler(NULL);
	topAbortMsg("Unbalanced topTclCaptureMsgs (ip)");
    }
    TOPTclMsgInterp = NULL;
    flags = TOPTclMsgFlags;
    TOPTclMsgFlags = 0;
    topMsgSetHandler(TOPTclMsgOldHandler);
    TOPTclMsgOldHandler = NULL;
    return flags;
}

char*
topTclGetRetMsg( Tcl_Interp *ip, int r) {
    if ( r == TCL_OK ) {
        if ( ip->result && ip->result[0] != '\0' ) {
	    return ip->result;
	} else {	
	    return "Ok";
        }
    } else {
	char *msg = Tcl_GetVar( ip, "errorInfo", TCL_GLOBAL_ONLY);
	if ( msg == NULL || msg[0] == '\0' ) {
	    msg = ip->result;
	    if ( msg == NULL || msg[0] == '\0' ) {
		msg = "Error";
	    }
	}
	return msg;
    }
}

/**************************************************************************
 *
 *		Support for "background" Tcl_Eval's
 *
 **************************************************************************/

void
topTclBgError( Tcl_Interp *ip, char *errmsg) {
    char		*saveerrmsg;
    char		*sV[3];
    char		*errcmd;
    int			r;

    if ( errmsg == NULL ) {
	/* grab the error out of the interp */
	errmsg = topTclGetRetMsg( ip, TCL_ERROR);
    }
    /*
     * Save the error msg for later reporting incase toperror doesnt handle it
     */
    saveerrmsg = memStrSave(errmsg);

    /*
     * Since errmsg almost certainly has blanks in it, use tcl_merge to
     * properly escape it.
     */
    sV[0] = "toperror";
    sV[1] = errmsg;
    errcmd = Tcl_Merge( 2, sV);
    r = Tcl_Eval( ip, errcmd);
    if ( r != TCL_OK ) {
        fprintf(stderr, "toperror failed to handle background error.\n");
	fprintf(stderr, "    Original error: %s\n", saveerrmsg);
        fprintf(stderr, "    Error in tkerror: %s\n", ip->result);
    }
    Tcl_ResetResult(ip);
    MEM_FREE(saveerrmsg);
}

int topTclBgEval( Tcl_Interp *ip, char *pkg, char *cmd) {
    int			r;
    char		*msg;

    TOP_PDBG((pkg, "TclBgEval: cmd %.60s", cmd));
    r = Tcl_Eval( ip,  cmd);
    msg = topTclGetRetMsg( ip, r);
    TOP_PDBG((pkg, "TclBgEval: ret %.60s", msg));
    if ( r != TCL_OK ) 
        topTclBgError( ip, msg);
    return r;
}

int topTclBgEvalVa( Tcl_Interp *ip, char *pkg, char *fmt, va_list args) {
    char		*buf;
    int			r;

    buf = topBufLFmtVa( fmt, args);
    r = topTclBgEval( ip, pkg, buf);
    topBufUnlock( buf);
    return r;
}

int topTclBgEvalFmt( Tcl_Interp *ip, char *pkg, char *fmt, ...) {
    va_list		args;
    int			r;

    va_start( args, fmt);
    r = topTclBgEvalVa( ip, pkg, fmt, args);
    va_end( args);
    return r;
}

/**************************************************************************
 *
 *		Normal Tcl_Eval's
 *
 **************************************************************************/

int 
topTclEval( Tcl_Interp *ip, char *pkg, char *cmd) {
    int			r;

    TOP_PDBG((pkg, "TclEval: cmd %.60s", cmd));
    r = Tcl_Eval( ip,  cmd);
    TOP_PDBG((pkg, "TclEval: ret %.60s", topTclGetRetMsg( ip, r)));
    return r;
}

int topTclEvalVa( Tcl_Interp *ip, char *pkg, char *fmt, va_list args) {
    char		*buf;
    int			r;

    buf = topBufLFmtVa( fmt, args);
    r = topTclEval( ip, pkg, buf);
    topBufUnlock( buf);
    return r;
}

int topTclEvalFmt( Tcl_Interp *ip, char *pkg, char *fmt, ...) {
    va_list		args;
    int			r;

    va_start( args, fmt);
    r = topTclEvalVa( ip, pkg, fmt, args);
    va_end( args);
    return r;
}

/************************************************************************
 *	Old sludge.
************************************************************************/

int topTclMsgEval( Tcl_Interp *ip, char *source, char *cmd) {
    int			r;

    TOP_DBG(("%s: TclEval: %.50s", source, cmd));
    r = Tcl_Eval( ip,  cmd);
    if ( r != TCL_OK ) {
	topWarnMsg("%s: TclEval: Error %d: %s", source,
	  r, ip->result ? ip->result : "???");
    } else if ( ip->result && ip->result[0] != NULL) {
	TOP_DBG(("%s: TclEval: %s", source, ip->result));
    } else {
	TOP_DBG(("%s: TclEval: Ok", source));
    }
    return r;
}

int topTclMsgEvalVa( Tcl_Interp *ip, char *source, char *fmt, va_list args) {
    char buf[1000];
    int r;

    vsprintf( buf, fmt, args);
    r = topTclMsgEval( ip, source, buf);
    return r;
}

int topTclMsgEvalFmt( Tcl_Interp *ip, char *source, char *fmt, ...) {
    va_list args;
    int r;

    va_start( args, fmt);
    r = topTclMsgEvalVa( ip, source, fmt, args);
    va_end( args);
    return r;
}
