/**
    planeLociDh.c :: Data Handler for XP Plane Window/Graph for root-loci
**/

#include "topFixup.h"
#include <stdio.h>
#include <X11/Xlib.h>
#include "tcl.h"

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"
#include "eemSilar.h"
#include "eemLoci.h"

#include "xpXfa.h"
#include "xpPlane.h"

/**
	XPPlaneLociDH

	This stores local state needed by the data handler.  It
	is not currently used.
**/
typedef struct _XPPlaneLociDH XPPlaneLociDH;
struct _XPPlaneLociDH {
    XPPlaneDataExt	*pntExt;

    TOPMask		flags;
    TOPLogical		recalcMasterB;
    EEMBox		box;
    EEMLociGrp		master;
    EEMLociGrp		zoom;
    double 		KStart;
    double 		KEnd;
    double		KMark;
};

static TOPLogical
_xpPlaneLdhConstructor( XPPlaneDataExt *pExt) {
    XPPlaneLociDH	*pDH;

    pDH = MEM_ALLOC(XPPlaneLociDH);
    MEM_CLEAR(pDH);
    pDH->recalcMasterB = TOP_TRUE;

    eemLociGrpConstructor( &pDH->master, NULL);
    eemLociGrpConstructor( &pDH->zoom, &pDH->master);

    /* cross link data structures */
    pExt->userData = (TOPPtr) pDH;
    pDH->pntExt = pExt;
    return TOP_TRUE;
}

static TOPLogical
_xpPlaneLdhDestructor( XPPlaneDataExt *pExt) {
    XPPlaneLociDH	*pDH = (XPPlaneLociDH*) pExt->userData;
    XPPlaneLocus	*pPL;

    eemLociGrpDestructor( &pDH->zoom);
    eemLociGrpDestructor( &pDH->master);

    MEM_FREE(pDH);
    pExt->userData = pDH = NULL;
    return TOP_TRUE;
}

static TOPLogical
_xpPlaneLdhChangeSilar( XPPlaneDataExt *pExt, XPPlaneChangeType ct, int idx) {
    XPPlaneLociDH	*pDH = (XPPlaneLociDH*) pExt->userData;

    pDH->recalcMasterB = TOP_TRUE;
    xpPlaneDataUpdateOne( pExt, idx);
    return TOP_TRUE;
}

static TOPLogical
_xpPlaneLdhSetDrawK( XPPlaneDataExt *pExt, TOPMask flags, double ks, double ke){
    XPPlaneLociDH	*pDH = (XPPlaneLociDH*) pExt->userData;

    pDH->KStart = ks;
    pDH->KEnd = ke;

    eemRootLociKRange(&pDH->master, &pDH->zoom, &pDH->box, ks, ke, flags);  
}

static TOPLogical
_xpPlaneLdhSetMarkK( XPPlaneDataExt *pExt, TOPMask flags, double k) {
    XPPlaneLociDH	*pDH = (XPPlaneLociDH*) pExt->userData;

    pDH->KMark = k;
    
    if ( TOP_IsOn(flags, XPPlaneLF_Mark) ) {
	pDH->flags |= EEMLocusF_Mark;
        eemLocusPtAtK(&pDH->master,pDH->KMark, &pExt->wLociMarks);
    } else {
	pDH->flags &= ~EEMLocusF_Mark;
        eemSilarGrpSetNum( &pExt->wLociMarks, 0);
    }
    xpPlaneDataUpdateMarks( pExt);
}

static TOPLogical
_xpPlaneLdhCalcLoci( XPPlaneDataExt *pExt) {
    XPPlaneLociDH	*pDH = (XPPlaneLociDH*) pExt->userData;

    xpPlaneLociSetNum(pExt,0);

    pDH->box.lo.x = pExt->xfaX->loWin;	pDH->box.hi.x = pExt->xfaX->hiWin;
    pDH->box.lo.y = pExt->xfaY->loWin;	pDH->box.hi.y = pExt->xfaY->hiWin;
    pDH->box.delta.x = xpXfaScaleToWin( pExt->xfaX, 1);
    pDH->box.delta.y = xpXfaScaleToWin( pExt->xfaY, 1);

    if ( pDH->recalcMasterB ) {
        pDH->recalcMasterB = TOP_FALSE;
        eemRootLociMaster( &pDH->master, &pDH->box,
	  &pExt->wSilars, 0.0, TOP_FALSE);
    }
    eemRootLociZoomIn(&pDH->master, &pDH->zoom, &pDH->box);

    if ( TOP_IsOn(pDH->flags,EEMLocusF_Mark) ) {
	eemLocusPtAtK(&pDH->zoom, pDH->KMark,
	  &pExt->wLociMarks);
    }
}


static TOPLogical
_xpPlaneLdhMapLoci( XPPlaneDataExt *pExt) {
    XPPlaneLociDH	*pDH = (XPPlaneLociDH*) pExt->userData;
    EEMLocus		*pLocus;
    XPPlaneLocus	*pPL, *pPLBase;
    int			i, k, nl, np;
    int			xpix, ypix, xpixlast, ypixlast;
    EEMComplex		*wPt;
    XPoint		*vPt, *vPtBase;
    EEMLociGrp		*pGrp;

    pGrp = & pDH->zoom;
    nl = EEM_LOCI_Num(&pDH->master);
    xpPlaneLociSetNum(pExt,nl);
    pPLBase = &XPPLANE_LociGet(pExt,0);
    for (i=0, pPL=pPLBase; i < nl; i++) {
	pLocus = &EEM_LOCI_Get(&pDH->master,i);
	np = EEM_LOCUSPOS_Num(pLocus);
	if ( np < 2 )
	    continue;
	wPt = &EEM_LOCUSPOS_Get(pLocus,0);
	topVecSetNum( &pPL->vPts, np);
	vPtBase = &XPPLANE_LocusPtGet(pPL,0);
	for (k=0, vPt=vPtBase; k < np; k++, wPt++) {
	    xpix = xpXfaToView(pExt->xfaX,wPt->r);
	    ypix = xpXfaToView(pExt->xfaY,wPt->i);
	    if ( k!=0 && xpix == xpixlast && ypix == ypixlast )
		continue;
	    vPt->x = xpix; vPt->y = ypix; ++vPt;
	    xpixlast = xpix; ypixlast = ypix;
	}
	topVecSetNum( &pPL->vPts, vPt-vPtBase);
	++pPL;
    }
    xpPlaneLociSetNum(pExt,pPL-pPLBase);

    return TOP_TRUE;
}

static XPPlaneDataHandler _XpPlaneLociDataHandler = {
    _xpPlaneLdhConstructor,
    _xpPlaneLdhDestructor,
    _xpPlaneLdhChangeSilar,
    _xpPlaneLdhCalcLoci,
    _xpPlaneLdhMapLoci,
    _xpPlaneLdhSetDrawK,
    _xpPlaneLdhSetMarkK,
};

int
xpPlaneLociDhRegisterCmds( Tcl_Interp *ip) {
    xpRegisterDataHandler( ip, "loci", "XPPlaneDataHandler",
      &_XpPlaneLociDataHandler);
    return TCL_OK;
}
