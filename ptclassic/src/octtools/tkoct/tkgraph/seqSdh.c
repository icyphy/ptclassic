/* 
    seqSdh.c :: Simple Data Handler :: Sequence Graph Widget

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

#include <stdio.h>		/* fixes a NULL redef problem */
#include <X11/Xlib.h>

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"

#include "xpXfa.h"
#include "xpAxis.h"
#include "xpSeq.h"

/************************************************************************

				XPSeqXfaSimple
	This the default data-handler for the Tk sequence widget (seqTk.c).
	It is a simplistic mechanism that allows real-valued points (y-axis)
	over a real-valued domain (x-axis).  The points may be spaced at
	arbitrary intervals over the x-axis; however, the x-axis coordinates
	must monotonically increase.

************************************************************************/

/**
	XPSeqSimpleDH

	The {domainIdxLo} and {domainIdxHi} are indexes (0-based) into
	{wPtsVec}.  They are the first and last points that that will
	be mapped and given to the core for drawing.  In general, these
	two points will be the points just *outside* the current
	viewport.  This is so that in connect line mode, there will be
	lines drawn to the edge.  It also provides a line when when
	none of the points are on the viewport.
**/
typedef struct _XPSeqSimpleDH XPSeqSimpleDH;
struct _XPSeqSimpleDH {
    XPXfa		*pXfa;		/* mapping func; from axis */
    TOPVector		wPtsVec;		/* of TOPDblPoint2 */

    int			domainIdxLo;
    int			domainIdxHi;
};

/**
	Search the vector of source points stored in the handler for
	some value.  Return the index of the greatest point whose x-val
	is less than {val}.  In general, the return value
	is the index where {val} would be inserted into the list.  Cases:

	  normal ==> array[retval] <= val < array[retval]
	  empty array ==> rets 0
	  off lo end (val is less than all others) ==> rets 0
	  off hi end (val is greater than all others)  ==> rets len of array

	Note that the equel case really shouldnt happen, so we dont worry
	about it too much.
**/
static unsigned
_xpSeqSdhFindPtByDomain( XPSeqSimpleDH *pDH, double val) {
    int			n;
    TOPVector		*wPts = &pDH->wPtsVec;

    n = TOPVEC_GETNUM(wPts);
    if ( n == 0 )
	return 0;
    if ( val > TOPVEC_GET( wPts, n-1, TOPDblPoint2).x )
	return n;
    if ( val < TOPVEC_GET( wPts, 0, TOPDblPoint2).x )
	return 0;

    /* simple linear search for now; replace with binary later */
    for ( n--; n >= 0; n--) {
        if ( val > TOPVEC_GET( wPts, n, TOPDblPoint2).x )
	    return n+1;
    }
    return 0;	/* shouldn't get here */
}

static TOPLogical
_xpSeqSdhConstructor( XPSeqDataExt *pExt) {
    XPSeqSimpleDH	*pDH;

    pDH = MEM_ALLOC(XPSeqSimpleDH);
    topVecConstructor( &pDH->wPtsVec, sizeof(TOPDblPoint2));

    pExt->userData = (TOPPtr) pDH;
    return TOP_TRUE;
}

static TOPLogical
_xpSeqSdhDestructor( XPSeqDataExt *pExt) {
    XPSeqSimpleDH	*pDH = (XPSeqSimpleDH*) pExt->userData;

    topVecDestructor( &pDH->wPtsVec);

    MEM_FREE(pDH);
    pExt->userData = NULL;
    return TOP_TRUE;
}

static TOPLogical
_xpSeqSdhConfig( XPSeqDataExt *pExt) {
    XPSeqSimpleDH	*pDH = (XPSeqSimpleDH*) pExt->userData;
    int			n = TOPVEC_Num(&pDH->wPtsVec);
    
    if ( pExt->xfaX == NULL || pExt->xfaY == NULL ) {
	/* MUST have axis and cooresponding xfas */
	return TOP_FALSE;
    }

    pDH->domainIdxLo = _xpSeqSdhFindPtByDomain( pDH, pExt->wDomainLo) - 1;
    if ( pDH->domainIdxLo < 0 )
	pDH->domainIdxLo = 0;
    pDH->domainIdxHi = _xpSeqSdhFindPtByDomain( pDH, pExt->wDomainHi);
    if ( pDH->domainIdxHi >= n )
	pDH->domainIdxHi = n-1;

    pExt->minDom = TOPVEC_Get(&pDH->wPtsVec, 0, TOPDblPoint2).x;
    pExt->maxDom = TOPVEC_Get(&pDH->wPtsVec, n-1, TOPDblPoint2).x;
    return TOP_TRUE;
}

static TOPLogical
_xpSeqSdhMap( XPSeqDataExt *pExt) {
    XPSeqSimpleDH	*pDH = (XPSeqSimpleDH*) pExt->userData;
    unsigned		i, lidx, hidx;
    TOPVector		*vPts = &pExt->vPtsVec;
    TOPVector		*wPts = &pDH->wPtsVec;
    TOPDblPoint2	*wPt;
    XPoint		*vPt;
    double		xval = 0.0, yval = 0.0, ymin = 0.0, ymax = 0.0;

    lidx = pDH->domainIdxLo;
    hidx = pDH->domainIdxHi;
    if ( ((int)lidx) < 0 || hidx<lidx || hidx >= TOPVEC_GETNUM(wPts) ) {
        topVecClear( vPts);
	return TOP_TRUE;
    }
    topVecSetNum( vPts, (unsigned)(hidx-lidx+1));
    for ( i=lidx; i <= hidx; i++) {
	wPt = &TOPVEC_GET(wPts, i, TOPDblPoint2);
	vPt = &TOPVEC_GET(vPts, i, XPoint);
	xval = wPt->x; yval = wPt->y;
	vPt->x = xpXfaToView( pExt->xfaX, xval);
	vPt->y = xpXfaToView( pExt->xfaY, yval);
	if ( yval < ymin || i==lidx )  ymin = yval;
	if ( yval > ymax || i==lidx )  ymax = yval;
    }
    pExt->vZero = xpXfaToView( pExt->xfaY, 0.0);
    pExt->minVal = ymin;
    pExt->maxVal = ymax;
    return TOP_TRUE;
}

static TOPLogical
_xpSeqSdhInsertPts( XPSeqDataExt *pExt, TOPDblPoint2 *pts, unsigned num) {
    XPSeqSimpleDH	*pDH = (XPSeqSimpleDH*) pExt->userData;
    TOPVector		*wPts = &pDH->wPtsVec;
    unsigned		n = TOPVEC_GETNUM( wPts);
    unsigned		i, k;

    for ( i=0; i < num; i++, pts++) {
	if ( n == 0 || TOPVEC_TAIL(wPts, TOPDblPoint2).x <= pts->x ) {
	    TOPVEC_APPEND(wPts, TOPDblPoint2) = *pts;
	} else {
	    k = _xpSeqSdhFindPtByDomain( pDH, pts->x);
	    TOPVEC_INSERT(wPts, k, TOPDblPoint2) = *pts;
	}
    }
    pExt->configReqB = TOP_TRUE;
    return TOP_TRUE;
}

static TOPLogical
_xpSeqSdhDeletePts( XPSeqDataExt *pExt, double wLo, double wHi) {
    return TOP_FALSE;
}




/*global*/ XPSeqDataHandler XpSeqSimpleDataHandler = {
    _xpSeqSdhConstructor,
    _xpSeqSdhDestructor,
    _xpSeqSdhConfig,
    _xpSeqSdhMap,
    _xpSeqSdhInsertPts,
    _xpSeqSdhDeletePts,
};
