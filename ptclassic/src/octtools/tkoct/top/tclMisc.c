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

int
topTclGetInt2( Tcl_Interp *ip, char *xStr, char *yStr, int *pX, int *pY) {
    if ( Tcl_GetInt( ip, xStr, pX) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nGetInt2: bad x coord.",NULL);
	return TCL_ERROR;
    }
    if ( Tcl_GetInt( ip, yStr, pY) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nGetInt2: bad y coord.",NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

int
topTclPrsInt( Tcl_Interp *ip, char **pStr, int *pX) {
    int		xval;
    char	*str = *pStr, *str_next;

    xval = strtol( str, &str_next, 0);
    if ( str==str_next ) {
	Tcl_AppendResult(ip,"PrsInt: expected integer, got ``",str,"''",NULL);
	return TCL_ERROR;
    }
    *pX = xval;
    *pStr = str_next;
    return TCL_OK;
}

int
topTclPrsInt2( Tcl_Interp *ip, char **pStr, int *pX, int *pY) {
    if ( topTclPrsInt( ip, pStr, pX) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nPrsInt2: Bad x coord.",NULL);
	return TCL_ERROR;
    }
    if ( topTclPrsInt( ip, pStr, pY) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nPrsInt2: Bad y coord.",NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}


int
topTclPrsInt4( Tcl_Interp *ip, char **pStr,
  int *pX1,int *pY1, int *pX2,int *pY2) {
    if ( topTclPrsInt( ip, pStr, pX1) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nPrsInt4: Bad x1 coord.",NULL);
	return TCL_ERROR;
    }
    if ( topTclPrsInt( ip, pStr, pY1) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nPrsInt4: Bad y1 coord.",NULL);
	return TCL_ERROR;
    }
    if ( topTclPrsInt( ip, pStr, pX2) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nPrsInt4: Bad x1 coord.",NULL);
	return TCL_ERROR;
    }
    if ( topTclPrsInt( ip, pStr, pY2) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nPrsInt4: Bad y2 coord.",NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

static int _TopTclUidInitB = TOP_FALSE;
static Tcl_HashTable _TopTclUidTbl;

TOPTclUid
topTclGetUid(char *string) {
    int dummy;
    if (!_TopTclUidInitB) {
	Tcl_InitHashTable(&_TopTclUidTbl, TCL_STRING_KEYS);
	_TopTclUidInitB = TOP_TRUE;
    }
    return (TOPTclUid) Tcl_GetHashKey(&_TopTclUidTbl,
      Tcl_CreateHashEntry(&_TopTclUidTbl, string, &dummy));
}
