/**
    topMsgPkg.c :: msg subpackage :: TOP Library

    Extension to topMsg to provide per-package debugging support.

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
**/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
#endif

#include "topFixup.h"
#include <stdarg.h>
#include "topStd.h"
#include "topMem.h"
#include "topBuf.h"

#define TOP_MSGPKG_MINPKGS	8
#define TOP_MSGPKG_MAXLEN	200
#define TOP_MSGPKG_ROOT		( _TopMsgPkgs[0] )

typedef struct _TOPMsgPkgInfo TOPMsgPkgInfo;
struct _TOPMsgPkgInfo {
    char		flags;
    char		len;
    TOPMsgPkgInfo	*pnt;
    char		name[1];	/* this grows */
};

static int		_TopMsgNumPkgs = 0;
static TOPMsgPkgInfo**	_TopMsgPkgs = NULL;	/* array of ptrs to info */


static int
_topMsgPkgGrow() {
    int i, n, r;

    if ( _TopMsgPkgs == NULL || _TopMsgNumPkgs <= 0 ) {
	_TopMsgNumPkgs = 0;
	n = TOP_MSGPKG_MINPKGS;
	_TopMsgPkgs = (TOPMsgPkgInfo**) calloc( (unsigned)n,
	  sizeof(TOPMsgPkgInfo*));
    } else {
	n = 2 * _TopMsgNumPkgs;
    	_TopMsgPkgs = (TOPMsgPkgInfo**) realloc( _TopMsgPkgs, 
	  sizeof(TOPMsgPkgInfo*)*n);
    }
    for ( i=_TopMsgNumPkgs; i < n; i++) {
    	_TopMsgPkgs[i] = NULL;
    }
    r = _TopMsgNumPkgs;
    _TopMsgNumPkgs = n;
    return r;
}

static int
_topMsgPkgGetSlot() {
    int		i;

    for (i=0; i < _TopMsgNumPkgs; i++) {
	if ( _TopMsgPkgs[i] == NULL )
	    return i;
    }
    return _topMsgPkgGrow();
}

static TOPMsgPkgInfo*
_topMsgPkgFind( char *pkgName) {
    int		len, i;

    len = strlen(pkgName);
    for (i=0; i < _TopMsgNumPkgs; i++) {
	if ( _TopMsgPkgs[i] == NULL )	continue;
	if (len == _TopMsgPkgs[i]->len 
	  && strcmp(pkgName,_TopMsgPkgs[i]->name)==0 ) {
	    return _TopMsgPkgs[i];
	}
    }
    return NULL;
}

static TOPMsgPkgInfo*
_topMsgPkgAdd( char *pkgName) {
    TOPMsgPkgInfo	*pInfo;
    int			i, len;

    i = _topMsgPkgGetSlot();
    len = strlen(pkgName);
    pInfo = (TOPMsgPkgInfo*) malloc(sizeof(TOPMsgPkgInfo)-1+len+1);
    pInfo->pnt = NULL;
    pInfo->flags = 0;
    pInfo->len = len;
    strcpy( pInfo->name, pkgName);
    _TopMsgPkgs[i] = pInfo;
    return pInfo;
}

/**
    Find struct associated with {pkgName}.  If it doesnt exist,
    make one.  If {pkgName} is hierarchical, then get entry for
    parent and reference that one.
**/
static TOPMsgPkgInfo*
_topMsgPkgGet( char *pkgName) {
    TOPMsgPkgInfo       *pInfo;
    char		*s;

    if ( (pInfo = _topMsgPkgFind( pkgName)) == NULL ) {
	pInfo = _topMsgPkgAdd( pkgName);
	if ( (s = strrchr( pkgName, '.')) != NULL ) {
	    *s = '\0';
	    pInfo->pnt = _topMsgPkgGet( pkgName);
	    *s = '.';
	}
    }
    return pInfo;
}

static TOPMsgPkgInfo*
_topMsgPkgGetPnt( char *pkgName) {
    TOPMsgPkgInfo       *pInfo;

    pInfo = _topMsgPkgGet( pkgName);
    while ( pInfo->pnt != NULL ) {
	pInfo = pInfo->pnt;
    }
    return pInfo;
}

void
topMsgPkgSet( char *pkgName, int flags) {
    TOPMsgPkgInfo	*pInfo;
    int			i;
    TOPStrLenType	len;
    char		basePkg[TOP_MSGPKG_MAXLEN];

    len = strlen(pkgName);
    if ( pkgName[len-1] == '*' ) {
	/* set all "real" child entries */
	for (i=0; i < _TopMsgNumPkgs; i++) {
	    pInfo = _TopMsgPkgs[i];
	    if ( pInfo == NULL || pInfo->pnt != NULL )	continue;
	    if ( strncmp( pkgName, pInfo->name, len-1)==0 )
		pInfo->flags = flags;
	}
	if ( len >= 2 && pkgName[len-2] == '.' )
	    --len;
	if ( len == 1 ) {
	    return;
	}
	strcpy( basePkg, pkgName);
	basePkg[len-1] = '\0';
	pkgName = basePkg;
    }
    pInfo = _topMsgPkgGet( pkgName);
    pInfo->flags = flags;
    pInfo->pnt = NULL;
}

TOPLogical
topMsgPkgDbgOnB( char *pkgName) {
    TOPMsgPkgInfo	*pInfo;

    pInfo = _topMsgPkgGetPnt( pkgName);
    return pInfo->flags != 0;
}

static void
topMsgPkgBuf_core( char *pkgName, char *msg) {
    char *buf;
    buf = pkgName ? topBufFmt( "%s: %s", pkgName, msg) : msg;
    topMsg( TOP_MSG_DBG, buf);
}

void 
topMsgPkgBuf( char *pkgName, char *msg) {
    if ( TOPDebugMode == 0 || !topMsgPkgDbgOnB(pkgName) )
	return;
    topMsgPkgBuf_core( pkgName, msg);
}

static void
topMsgPkgVa_core( char *pkgName, char *fmt, va_list args) {
    char *buf = topBufLFmtVa( fmt, args);
    topMsgPkgBuf_core( pkgName, buf);
    topBufUnlock( buf);
}

void
topMsgPkgVa( char *pkgName, char *fmt, va_list args) {
    if ( TOPDebugMode == 0 || !topMsgPkgDbgOnB(pkgName) )
	return;
    topMsgPkgVa_core( pkgName, fmt, args);
}

void
topMsgPkgFmt( char *pkgName, char *fmt, ...) {
    va_list args;

    if ( TOPDebugMode == 0 || !topMsgPkgDbgOnB(pkgName) )
	return;
    va_start( args, fmt);
    topMsgPkgVa_core( pkgName, fmt, args);
    va_end( args);
}
