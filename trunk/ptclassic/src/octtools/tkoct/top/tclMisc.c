/* 
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
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
