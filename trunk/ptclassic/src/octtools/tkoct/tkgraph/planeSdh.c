/* 
    seqSdh.c :: Simple Data Handler :: Planeuence Graph Widget

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

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"
#include "eemSilar.h"

#include "xpXfa.h"
#include "xpPlane.h"

/**
	XPPlaneSimpleDH

	This stores local state needed by the data handler.  It
	is not currently used.
**/
typedef struct _XPPlaneSimpleDH XPPlaneSimpleDH;
struct _XPPlaneSimpleDH {
    int			goo;
};

static TOPLogical
_xpPlaneSdhConstructor( XPPlaneDataExt *pExt) {
    XPPlaneSimpleDH	*pDH;

    pDH = MEM_ALLOC(XPPlaneSimpleDH);

    pExt->userData = (TOPPtr) pDH;
    return TOP_TRUE;
}

static TOPLogical
_xpPlaneSdhDestructor( XPPlaneDataExt *pExt) {
    XPPlaneSimpleDH	*pDH = (XPPlaneSimpleDH*) pExt->userData;

    MEM_FREE(pDH);
    pExt->userData = NULL;
    return TOP_TRUE;
}

static TOPLogical
_xpPlaneSdhChangeSilar( XPPlaneDataExt *pExt, XPPlaneChangeType ct, int idx) {
    /* XPPlaneSimpleDH	*pDH = (XPPlaneSimpleDH*) pExt->userData; */

    eemSilarGrpBB( &pExt->wSilars, &pExt->bbLo, &pExt->bbHi);
    xpPlaneDataUpdateOne( pExt, idx);
    return TOP_TRUE;
}

/*global*/ XPPlaneDataHandler XpPlaneSimpleDataHandler = {
    _xpPlaneSdhConstructor,
    _xpPlaneSdhDestructor,
    _xpPlaneSdhChangeSilar,
    NULL,	/* calc loci */
    NULL,	/* map loci */
};
