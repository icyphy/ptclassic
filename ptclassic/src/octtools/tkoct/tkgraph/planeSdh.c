/*******************************************************************
SCCS version identification
$Id$

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

    Author: Kennard White

    seqSdh.c :: Simple Data Handler :: Planeuence Graph Widget

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
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
