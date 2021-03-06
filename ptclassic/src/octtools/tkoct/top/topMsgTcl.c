/* 
    topMsgPkg.c :: msg subpackage :: TOP Library

    Extension to topMsg to provide per-package debugging support.

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
#include "topMem.h"

static int
_topMsgDbgModeCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    int		flag = -1, ofs, i;
    char	buf[50];

    if (aC == 1) {
	sprintf( buf, "%d", TOPDebugMode);
	Tcl_SetResult( ip, buf, TCL_VOLATILE);
	return TCL_OK;
    }
    ofs = 1;
    /*IF*/ if ( strcmp( aV[ofs], "off")==0 || strcmp( aV[ofs], "0")==0 ) {
	++ofs;
	flag = 0;
    } else if ( strcmp( aV[ofs], "on")==0 || strcmp( aV[ofs], "1")==0 ) {
	++ofs;
	flag = 1;
    }
    if ( flag >= 0 && ofs == aC ) {
	topMsgSetDbgMode( flag);
    } else {
        for ( i=ofs; i < aC; i++) {
	    if ( flag < 0 ) {
		sprintf( buf, "%d ", topMsgPkgDbgOnB(aV[ofs]));
		Tcl_AppendResult( ip, buf, NULL);
	    } else {
	        topMsgPkgSet( aV[ofs], flag);
	    }
	}
    }
    return TCL_OK;
}

static int
_topMsgDbgCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    int			opts = 0;
    char		*pkgName = NULL;

    if ( aC >= 2 && strcmp( aV[1], "-pkg")==0 ) {
	pkgName = aV[2];
	opts += 2;
    }
    if ( aC != 1 + opts + 1 ) {
	Tcl_AppendResult(ip, "usage: ", aV[0], " ?-pkg pkgName? message", NULL);
	return TCL_ERROR;
    }
    topMsgPkgBuf( pkgName, aV[1+opts]);
    return TCL_OK;
}

int
topMsgRegisterCmds( Tcl_Interp *ip) {
    Tcl_CreateCommand( ip, "topdbgmode", _topMsgDbgModeCmd, NULL, NULL);
    Tcl_CreateCommand( ip, "topdbgmsg", _topMsgDbgCmd, NULL, NULL);
    return TCL_OK;
}
