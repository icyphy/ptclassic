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

 Programmer: Kennard White

    xfa.c (Transform Axis) :: XPole

    This packages performs float->integer transformation.
    The transform can be initialized to be either linear or
    logrithmic.

    Author: Kennard White
    90/10/03 KDW: created as tf.c
    12/20/90 KDW: extended to log scalings as xform.c

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <math.h>

#include "topStd.h"
#include "xpXfa.h"

static char _XpXfaPackage[] = "xp.xfa";
#define SPKG _XpXfaPackage

/**
    The transform is defined in terms of an integer viewport which is being
    mapped onto, and a floating point window which is being mapped from.
    All transforms require hiWin > loWin; however, it is acceptable
    for hiView < loView.

    The linear transform is defined as
	linFactor = (hiView-loView) / (hiWin-loWin)
	v = (w - loWin) * linFactor + loView
    No clipping is performed in linear mode.

    The logrithmic mode defines the following regions for a src val {w}.
    First define
	wLo' = max(loWin,threshWin)
	vLo' = flipB ? loView-threshView : loView+threshView
    Then the regions are:
	hiWin <= w		<==> clipped to hiView
	wLo' < w < hiWin	<==> log mapped onto (vLo',hiView)
	threshWin < w <= wLo'	<==> vLo'
	0 <= threshWin		<==> lin mapped onto (vLo,vLo')


    The logrithmic transform is defined as
	linFactor = (threshView) / (threshWin-0.0)
	logFactor = (hiView-(loView+threshView)) / (log(hiWin)-log(loWin))
	v = (log(w)-log(loWin)) * logFactor + loView+threshView	<==  w>=wClip
	v = (w - 0.0)  * linFactor + loView

    The structure XPXfaCalls has a bunch of callbacks; it is set
    to either linear or logrithmetic call backs.  This technique
    avoids having to "switch" on each call.

    The struture XPXfa is the user visable structure.
**/

/************************************************************************
 *		ReCalc
 ************************************************************************/

static double CalcLogades( XPXfa *xf, double w) {
    double l;
    if ( w < - xf->threshWin ) {
	l = xf->logThreshWin - log(- w);
    } else if ( w <= xf->threshWin ) {
	l = 0;
    } else {
        l = log(w) - xf->logThreshWin;
    }
    return( l );
}

static void xpXfaLinReCalc( XPXfa *xf) {
    double d = xf->hiWin - xf->loWin;
    ++(xf->serial);
    if ( d == 0 ) {
	xf->linFactor = 1;
	xf->valid = FALSE;
    } else {
        xf->linFactor = (xf->hiView - xf->loView) / d;
	xf->valid = TRUE;
    }
}

static void xpXfaLogReCalc( XPXfa *xf) {
    double	d;
    double	lw = xf->loWin;
    TOPLogical	flipB = xf->loView > xf->hiView;

    ++(xf->serial);
    if ( xf->threshWin <= 0 )	xf->threshWin = 1e-20;
    if ( lw < xf->threshWin ) {
	lw = xf->threshWin;
        xf->vClipSgn = flipB ? - xf->threshView : xf->threshView;
        xf->linFactor = xf->vClipSgn / (xf->threshWin);
    } else {
	xf->vClipSgn = 0;
        xf->linFactor = 0.0;
    }
    if ( xf->hiWin <= lw ) {
	xf->hiWin = lw * 10;	/* arbitrary */
        xf->valid = FALSE;
    } else {
        xf->valid = TRUE;
    }
    xf->logLoWin = log(lw);
    d = log(xf->hiWin) - xf->logLoWin;
    TOP_ASSERT(SPKG,d > 0);	/* the trap above should prevent this */
    xf->logFactor = (xf->hiView - (xf->loView+xf->vClipSgn)) / d;
}

static void xpXfaDSLogReCalc( XPXfa *xf) {
    XPXfaDstType dView = xf->hiView - xf->loView - 2*xf->threshView;
    double posLogades, negLogades, totLogades;

    ++(xf->serial);
    if ( xf->threshWin <= 0 )	xf->threshWin = 1e-20;
    xf->logThreshWin = log(xf->threshWin);
    posLogades = CalcLogades( xf, xf->hiWin);
    negLogades = CalcLogades( xf, xf->loWin);
    totLogades = posLogades - negLogades;
    xf->numDecades = totLogades / log(10.0);
    if ( totLogades == 0 ) {
	topWarnMsg( "xfa: DSLog: empty range (%f,%f)\n", xf->loWin, xf->hiWin);
	xf->logFactor = 1.0;
	xf->centerView = (xf->loView + xf->hiView)/2;
    } else {
	xf->fNegFraction = -negLogades / totLogades;
	xf->centerView = xf->fNegFraction*dView + xf->loView;
	xf->logFactor = dView / totLogades;
    }
    xf->linFactor = (xf->threshView) / (xf->threshWin);
}

static void xpXfaReCalc( XPXfa *xf) {
    (*(xf->Calls->ReCalc))( xf);
}


/************************************************************************
 *		GetWin
 ************************************************************************/

TOPLogical xpXfaLinGetWin( XPXfa *xf, XPXfaSrcType *lo, XPXfaSrcType *hi) {
    *lo = xf->loWin;
    *hi = xf->hiWin;
    return xf->valid;
}

TOPLogical xpXfaLogGetWin( XPXfa *xf, XPXfaSrcType *lo, XPXfaSrcType *hi) {
    *lo = TOP_MAX(xf->loWin, xf->threshWin);
    *hi = xf->hiWin;
    return xf->valid;
}

TOPLogical xpXfaDSLogGetPosWin( XPXfa *xf, XPXfaSrcType *lo, XPXfaSrcType *hi) {
    *lo = TOP_MAX(xf->loWin, xf->threshWin);
    *hi = xf->hiWin;
    return xf->valid;
}
TOPLogical xpXfaDSLogGetNegWin( XPXfa *xf, XPXfaSrcType *lo, XPXfaSrcType *hi) {
    *lo = xf->loWin;
    *hi = TOP_MIN(xf->hiWin, -xf->threshWin);
    return xf->valid;
}

TOPLogical xpXfaGetWin( XPXfa *xf, XPXfaSrcType *lo, XPXfaSrcType *hi) {
    return (*(xf->Calls->GetWin))( xf, lo, hi);
}

/************************************************************************
 *		GetView
 ************************************************************************/

TOPLogical xpXfaLinGetView( XPXfa *xf, XPXfaDstType *lo, XPXfaDstType *hi) {
    *lo = xf->loView;
    *hi = xf->hiView;
    return xf->valid;
}

TOPLogical xpXfaLogGetView( XPXfa *xf, XPXfaDstType *lo, XPXfaDstType *hi) {
    /* XXX: this needs to be updated to take into account the ViewThresh */
    *lo = xf->loView;
    *hi = xf->hiView;
    return xf->valid;
}

TOPLogical xpXfaDSLogGetPosView( XPXfa *xf, XPXfaDstType *lo, XPXfaDstType *hi) {
    /* XXX: this needs to be updated to take into account the ViewThresh */
    *lo = xf->loView;
    *hi = xf->hiView;
    return xf->valid;
}
TOPLogical xpXfaDSLogGetNegView( XPXfa *xf, XPXfaDstType *lo, XPXfaDstType *hi) {
    /* XXX: this needs to be updated to take into account the ViewThresh */
    *lo = xf->loView;
    *hi = xf->hiView;
    return xf->valid;
}

TOPLogical xpXfaGetView( XPXfa *xf, XPXfaDstType *lo, XPXfaDstType *hi) {
    return (*(xf->Calls->GetView))( xf, lo, hi);
}

/************************************************************************
 *		ToView
 ************************************************************************/

XPXfaDstType xpXfaLinToView( XPXfa *xf, XPXfaSrcType w) {
    return( nint((w - xf->loWin)*xf->linFactor) + xf->loView );
}

XPXfaDstType xpXfaLogToView( XPXfa *xf, XPXfaSrcType w) {
    if ( w <= 0.0 )
	return xf->loView;
    if ( w <= xf->threshWin ) {
	int vOfs = nint((w)*xf->linFactor);
	return( vOfs + xf->loView );
    }
    return( nint((log(w) - xf->logLoWin)*xf->logFactor)
      + xf->loView + xf->vClipSgn);
}

XPXfaDstType xpXfaDSLogToView( XPXfa *xf, XPXfaSrcType w) {
    int v;
    if ( w == 0 ) 			v = xf->centerView;
    else if ( w > 0 ) {
	if ( w <= xf->threshWin ) {
	     v = xf->centerView + nint((w)*xf->linFactor);
	} else if ( w >= xf->hiWin )	v = xf->hiView;
	else {
	    v = nint((log(w) - xf->logThreshWin)*xf->logFactor)
	      + xf->centerView + xf->threshView;
	}
    } else {
	if ( w >= - xf->threshWin ) {
	    v = xf->centerView + nint((w)*xf->linFactor);
	}
	else if ( w <= xf->loWin )	v = xf->loView;
	else {
	    v = nint(-(log(-w) - xf->logThreshWin)*xf->logFactor)
	      + xf->centerView - xf->threshView;
	}
    }
    return( v );
}

XPXfaDstType xpXfaToView( XPXfa *xf, XPXfaSrcType w) {
    return( (*(xf->Calls->ToView))( xf, w) );
}

/************************************************************************
 *		ToWin
 ************************************************************************/

XPXfaSrcType xpXfaLinToWin( XPXfa *xf, XPXfaDstType v) {
    return( (v - xf->loView)/xf->linFactor + xf->loWin );
}

XPXfaSrcType xpXfaLogToWin( XPXfa *xf, XPXfaDstType v) {
    int		v2 = v - xf->loView;
    if ( xf->logFactor >= 0.0 ) {
	if ( v2 <= xf->vClipSgn )
            return( v2 <= 0 ? 0.0 : v2/xf->linFactor );
    } else {
	if ( v2 >= xf->vClipSgn )
            return( v2 >= 0 ? 0.0 : v2/xf->linFactor );
    }
    return( exp((v2-(xf->vClipSgn))/xf->logFactor + xf->logLoWin) );
}

XPXfaSrcType xpXfaDSLogToWin( XPXfa *xf, XPXfaDstType v) {
    XPXfaSrcType w = 0;
    int v2;

    v -= xf->centerView;
    v2 = v;
    if ( xf->loView > xf->hiView )
	v2 = - v2;
    if ( v == 0 || (-v < xf->threshView && v < xf->threshView) ) {
	w = 0;
    } else if ( v2 > 0 ) {
    	w = exp( (v-xf->threshView)/xf->logFactor + xf->logThreshWin);
    } else if ( v2 < 0 ) {
	w = - exp( -(v+xf->threshView)/xf->logFactor + xf->logThreshWin);
    }
    return( w );
}

XPXfaSrcType xpXfaToWin( XPXfa *xf, XPXfaDstType v) {
    return( (*(xf->Calls->ToWin))( xf, v) );
}

/************************************************************************
 *		Scale
 ************************************************************************/

XPXfaSrcType xpXfaScaleToView( XPXfa *xf, XPXfaSrcType val) {
    /* only meaningful for linear */
    return( val * xf->linFactor );
}

XPXfaSrcType xpXfaScaleToWin( XPXfa *xf, XPXfaSrcType val) {
    /* only meaningful for linear */
    return( val / xf->linFactor );
}

/************************************************************************
 *		Tables
 ************************************************************************/

static XPXfaCalls LinCallTbl = {
    XPXfaMode_Lin, xpXfaLinReCalc, xpXfaLinGetWin, xpXfaLinGetView, 
    xpXfaLinToView, xpXfaLinToWin
};
static XPXfaCalls LogCallTbl = {
    XPXfaMode_Log, xpXfaLogReCalc, xpXfaLogGetWin, xpXfaLogGetView,
    xpXfaLogToView, xpXfaLogToWin
};
static XPXfaCalls DSLogCallTbl = {
    XPXfaMode_DSLog, xpXfaDSLogReCalc, xpXfaDSLogGetPosWin, xpXfaDSLogGetPosView,
    xpXfaDSLogToView, xpXfaDSLogToWin
};

void xpXfaSet( XPXfa *xf, XPXfaMode m,
  XPXfaDstType lView, XPXfaDstType hView, XPXfaSrcType lWin, XPXfaSrcType hWin){
    xf->loView = lView;
    xf->hiView = hView;
    xf->loWin = lWin;
    xf->hiWin = hWin;
    xpXfaSetMode( xf, m);
}

void xpXfaSetMode( XPXfa *xf, XPXfaMode m) {
    switch ( m ) {
    case XPXfaMode_Lin:	xf->Calls = &LinCallTbl; break;
    case XPXfaMode_Log:	xf->Calls = &LogCallTbl; break;
    case XPXfaMode_DSLog:	xf->Calls = &DSLogCallTbl; break;
    default:	assert( FALSE );
    }
    xf->threshView = 0;
    xf->threshWin = 1e-25;
    xpXfaReCalc( xf);
}

void xpXfaSetWin( XPXfa *xf, XPXfaSrcType lWin, XPXfaSrcType hWin) {
    xf->loWin = lWin;
    xf->hiWin = hWin;
    xpXfaReCalc( xf);
}

void xpXfaSetView( XPXfa *xf, XPXfaDstType lView, XPXfaDstType hView) {
    xf->loView = lView;
    xf->hiView = hView;
    xpXfaReCalc( xf);
}

void xpXfaSetThresh( XPXfa *xf, XPXfaDstType tv, XPXfaSrcType tw) {
    if ( tv < 0 || tw <= 0 ) {
	topWarnMsg("xfa: attempt to set invalid thresholds %d %g", tv, tw);
	return;
    }
    xf->threshView = tv;
    xf->threshWin = tw;
    xpXfaReCalc( xf);
}

TOPStatus
xpXfaConfig( XPXfa *xf, XPXfaMode m,
  XPXfaDstType vLo, XPXfaDstType vHi, XPXfaSrcType wLo, XPXfaSrcType wHi,
  XPXfaDstType vClip, XPXfaSrcType wClip) {
    switch ( m ) {
    case XPXfaMode_Lin:		xf->Calls = &LinCallTbl; break;
    case XPXfaMode_Log:		xf->Calls = &LogCallTbl; break;
    case XPXfaMode_DSLog:	xf->Calls = &DSLogCallTbl; break;
    default:
	topProgMsg("XPXfa: Bad mode %d", m);
	return -1;
    }
    if ( vClip < 0 || wClip <= 0 ) {
	topWarnMsg("XPXfa: invalid clip thresholds %d %g", vClip, wClip);
	return -1;
    }
    xf->loView = vLo;
    xf->hiView = vHi;
    xf->loWin = wLo;
    xf->hiWin = wHi;
    xf->threshView = vClip;
    xf->threshWin = wClip;
    xpXfaReCalc( xf);
    return 0;
}

void xpXfaDump( XPXfa *xf) {
    TOP_DBG(("XFA: (v=%d) (%d) (%f,%f) -> (%f,%f) via (%f,%f)\n",
      xf->valid,
      xf->Calls->Mode,
      xf->loWin, xf->hiWin,
      xf->loView, xf->hiView,
      xf->linFactor, xf->logFactor));
}

/************************************************************************
 *		Misc. Conversion
 ************************************************************************/

XPXfaMode
_xpXfaScaleStrToMode( char *scale) {
    /*IF*/ if ( strcasecmp( scale, "lin")==0 ) {
	return XPXfaMode_Lin;
    } else if ( strcasecmp( scale, "log")==0 ) {
	return XPXfaMode_Log;
    } else if ( strcasecmp( scale, "dslog")==0 ) {
	return XPXfaMode_DSLog;
    }
    return XPXfaMode_Null;
}

XPXfaSrcType xpXfaRangeToWin( XPXfa *xf, XPXfaDstType vLo, XPXfaDstType vHi) {
    return (*(xf->Calls->ToWin))( xf, vHi) - (*(xf->Calls->ToWin))( xf, vLo);
}

XPXfaDstType xpXfaRangeToView( XPXfa *xf, XPXfaSrcType wLo, XPXfaSrcType wHi) {
    return (*(xf->Calls->ToView))( xf, wHi) - (*(xf->Calls->ToView))( xf, wLo);
}

