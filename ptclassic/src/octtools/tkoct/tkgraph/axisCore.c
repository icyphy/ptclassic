/*******************************************************************
SCCS version identification
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

    Author: Kennard White

    axisCore.c :: XPAxis

    Tk axis widget (axis labels for graphs)
    In theory, this file contains the non (or less) Tk-dependent code.
    That is, non-X code and X-lib only code.

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <tk.h>

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"
#include "topTcl.h"
#include "topMisc.h"

#include "xpXfa.h"
#include "xpAxis.h"
#include "axisInt.h"

/*global*/ char _XpAxisSubPackage[] = "xp.axis.misc";
#define SPKG _XpAxisSubPackage
/*global*/ char _XpAxisSubPackageTics[] = "xp.axis.tics";
#define SPKG_TICS _XpAxisSubPackageTics

/**
    layout for right aligned: halfchar space, label, halfchar, longtic

    XXX: this algorithm should be run during the recalc phase,
       and the data stored in arrays so we can do all this
       with just a few calls
**/


static void
_xpCalcTickLayout( XPAxisWdg *pAxis) {
    Tk_Window		tkw = pAxis->tkwin;
    int			x1, x2, y1, y2, tx, ty;
    XPAxisFlt		wVal;
    int			orgnX, orgnY, orgfX, orgfY, dvVal, avVal, tlvVal;
    int			ticlen, tickarea, halfchar;
    int			avHeight, avWidth;
    int			vLblWidth = pAxis->vTicLabWidth;
    TOPLogical		tinB;

    avWidth = Tk_Width(tkw);
    avHeight = Tk_Height(tkw);
    halfchar = pAxis->tickCI.width / 2;

    if ( pAxis->tickOutsideB ) {
	orgnX = Tk_Width(tkw);	orgfX = 0;
	orgnY = Tk_Height(tkw);	orgfY = 0;
    	tickarea = pAxis->vMajorTicLen 
	  + (pAxis->vertB ? XPAXIS_PAD_VERT : halfchar);
    } else {
        orgnX = 0;	orgfX = Tk_Width(pAxis->dataWdg);
        orgnY = 0;	orgfY = Tk_Height(pAxis->dataWdg);
	tickarea = 0;
    }

    TOPVEC_LOOP_FORW_BEGIN( &pAxis->Tics, XPAxisTick, pTick) {
	wVal = pTick->wVal;
	dvVal = xpXfaToView( &pAxis->TF, wVal);
	avVal = dvVal + pAxis->vDataWdgOfs;
	ticlen = pTick->Flags & XPAxisTicF_Major ? 
	  pAxis->vMajorTicLen : pAxis->vMinorTicLen;
	if ( pAxis->tickOutsideB ) {
	    tlvVal = avVal;
	} else {
	    tlvVal = dvVal;
	    ticlen = -ticlen;
	}
	switch ( pAxis->side ) {
	case XPAxisSide_Right:
	    x1 = orgnX - ticlen;	y1 = tlvVal;
	    x2 = orgnX;			y2 = tlvVal;
	    tx = avWidth - vLblWidth - tickarea
	      - pAxis->tickCI.lbearing;
	    ty = avVal + (pAxis->tickCI.ascent - pAxis->tickCI.descent)/2;
	    tinB = avVal >= 0 && avVal < avHeight;
	    break;
	case XPAxisSide_Left:
	    x1 = orgfX;				y1 = tlvVal;
	    x2 = orgfX + ticlen;		y2 = tlvVal;
	    tx = tickarea - pAxis->tickCI.lbearing;
	    ty = avVal + (pAxis->tickCI.ascent - pAxis->tickCI.descent)/2;
	    tinB = avVal >= 0 && avVal < avHeight;
	    break;
	case XPAxisSide_Top:
	    x1 = tlvVal;			y1 = orgfY + ticlen;
	    x2 = tlvVal;			y2 = orgfY;
	    tx = avVal - vLblWidth / 2 - pAxis->tickCI.lbearing;
	    ty = tickarea + pAxis->tickCI.ascent;
	    tinB = avVal >= 0 && avVal < avWidth;
	    break;
	case XPAxisSide_Bottom:
	    x1 = tlvVal;		y1 = orgnY - ticlen;
	    x2 = tlvVal;		y2 = orgnY;
	    tx = avVal - vLblWidth / 2 - pAxis->tickCI.lbearing;
	    ty = avHeight - tickarea - XPAXIS_PAD_VERT;
	    tinB = avVal >= 0 && avVal < avWidth;
	    break;
        default:
	    topAbortMsg("bad axis align type");
	    return;
	}
        TOP_PDBG((SPKG_TICS,"%s: Tic: %d (%d,%d)x(%d,%d) %dx%d %g",
	  pAxis->Tag, tinB, x1, y1, x2, y2, tx, ty, wVal));
	if ( ! tinB ) {
	    pTick->tl1.x = -100;
	} else {
	    pTick->tl1.x = x1; pTick->tl1.y = y1;
	    pTick->tl2.x = x2; pTick->tl2.y = y2;
	    if ( TOP_IsOn( pTick->Flags, XPAxisTicF_Label) ) {
		if ( pAxis->vertB ) {
		    int dtop = ty - pAxis->tickCI.ascent - 0;
		    int dbot = ty + pAxis->tickCI.lbearing - avHeight;
		    if ( dtop < 0 )			ty -= dtop;
		    else if ( dbot > 0 )		ty -= dbot;
		} else {
		    int dleft = tx + pAxis->tickCI.lbearing - 0;
		    int dright = tx + vLblWidth + pAxis->tickCI.rbearing - avWidth;
		    if ( dleft < 0 )			tx -= dleft;
		    else if ( dright > 0 )		tx -= dright;
		}
		pTick->ts.x = tx;
		pTick->ts.y = ty;
	    }
	}
    } TOPVEC_LOOP_FORW_END();
}

void
_xpAxisExpose( XPAxisWdg *pAxis) {
    Tk_Window		tkw = pAxis->tkwin;
    Display		*dpy;
    Window		win;
    int			x1, x2, y1, y2, tx, ty, c1, c2, cx1, cy1, cx2, cy2;
    int			ofs = pAxis->vDataWdgOfs;
    int			avHeight, avWidth;

    TOP_PDBG((SPKG,"%s: expose", pAxis->Tag));

    if ( tkw == NULL || !Tk_IsMapped(tkw) || pAxis->side == XPAxisSide_Null )
        return;
    dpy = Tk_Display( tkw);
    win = Tk_WindowId( tkw);
    XClearWindow( dpy, win);

    if ( TOP_IsOff(pAxis->Flags, XPAxisF_On) 
      || TOP_IsOn( pAxis->Flags, XPAxisF_Calc) ) {
        TOP_PDBG((SPKG,"%s: invalid state for exposure  (flags %d)",
	  pAxis->Tag, pAxis->Flags));
	return;
    }
    avWidth = Tk_Width(tkw);
    avHeight = Tk_Height(tkw);

    if ( TOP_IsOff(pAxis->Flags,XPAxisF_LabelText) ) {
	TOPVEC_LOOP_FORW_BEGIN( &pAxis->Tics, XPAxisTick, pTick) {
	    if ( TOP_IsOn( pTick->Flags, XPAxisTicF_Label) ) {
		topFmtDbl( pTick->Lbl, pAxis->realTickFmt, pTick->wVal);
	    }
	} TOPVEC_LOOP_FORW_END();
	pAxis->Flags |= XPAxisF_LabelText;
	_xpCalcTickLayout( pAxis);
    }

    switch ( pAxis->TF.Calls->Mode ) {
    case XPXfaMode_Log:
	c1 = pAxis->TF.loView + pAxis->TF.vClipSgn;	c2 = pAxis->TF.loView;
	break;
    case XPXfaMode_DSLog:
	c1 = pAxis->TF.centerView + pAxis->TF.vClipSgn;
	c2 = pAxis->TF.centerView - pAxis->TF.vClipSgn;
	break;
    default:
	c1 = c2 = 0;
    }

    tx = ty = -100;
    switch ( pAxis->side ) {
    case XPAxisSide_Right:
	x1 = avWidth - 1;		y1 = ofs;
	x2 = avWidth - 1;		y2 = ofs + pAxis->vDataWdgLen;
	cx1 = cx2 = x1;			cy1 = c1 + ofs; cy2 = c2 + ofs;
	break;
    case XPAxisSide_Left:
	x1 = 0;				y1 = ofs;
	x2 = 0;				y2 = ofs + pAxis->vDataWdgLen;
	cx1 = cx2 = x1;			cy1 = c1 + ofs; cy2 = c2 + ofs;
	break;
    case XPAxisSide_Top:
	x1 = ofs;			y1 = 0;
	x2 = ofs + pAxis->vDataWdgLen;	y2 = 0;
	tx = ofs + pAxis->vDataWdgLen/2 - pAxis->labelBbox.width/2;
	ty = avHeight-1 - pAxis->labelBbox.descent - XPAXIS_PAD_VERT;
	if ( pAxis->scrollB && pAxis->scrollWdg ) ty -= Tk_Height(pAxis->scrollWdg);
	cx1 = c1 + ofs; cx2 = c2 + ofs;	cy1 = cy2 = y1;
	break;
    case XPAxisSide_Bottom:
	x1 = ofs;			y1 = avHeight - 1;
	x2 = ofs + pAxis->vDataWdgLen;	y2 = avHeight - 1;
	tx = ofs + pAxis->vDataWdgLen/2 - pAxis->labelBbox.width/2;
	ty = pAxis->labelBbox.ascent + XPAXIS_PAD_VERT;
	if ( pAxis->scrollB && pAxis->scrollWdg ) ty += Tk_Height(pAxis->scrollWdg);
	cx1 = c1 + ofs; cx2 = c2 + ofs;	cy1 = cy2 = y1;
	break;
    default: 
	topProgMsg("bad axis align type");
	return;
    }
    XDrawLine( dpy, win, pAxis->tickGC, x1, y1, x2, y2);
    if ( c1 != c2 ) {
        XDrawLine( dpy, win, pAxis->clipGC, cx1, cy1, cx2, cy2);
    }
    if ( pAxis->labelLen > 0 && tx > -100 ) {
	XDrawString( dpy, win, pAxis->labelGC, tx, ty, 
	  pAxis->labelStr, pAxis->labelLen);
    }

    TOPVEC_LOOP_FORW_BEGIN( &pAxis->Tics, XPAxisTick, pTick) {
	if ( pTick->tl1.x <= -100 )	continue;
	if ( pAxis->tickOutsideB ) {
	    XDrawLine( dpy, win, pAxis->tickGC, pTick->tl1.x, pTick->tl1.y,
	      pTick->tl2.x, pTick->tl2.y);
	}
	if ( TOP_IsOn( pTick->Flags, XPAxisTicF_Label) ) {
	    XDrawString( dpy, win, pAxis->tickGC, pTick->ts.x, pTick->ts.y, 
	      pTick->Lbl, (int)strlen(pTick->Lbl));
	}
    } TOPVEC_LOOP_FORW_END();
}

void
xpAxisExposeDrawWin( XPAxisWdg *pAxis) {
    Tk_Window	tkw = pAxis->dataWdg;
    Display*	dpy = Tk_Display(tkw);
    Window	win = Tk_WindowId(tkw);

    if ( pAxis->tickInsideB ) {
	TOPVEC_LOOP_FORW_BEGIN( &pAxis->Tics, XPAxisTick, pTick) {
	    if ( pTick->tl1.x <= -100 )	continue;
	    XDrawLine( dpy, win, pAxis->tickGC, pTick->tl1.x, pTick->tl1.y,
	      pTick->tl2.x, pTick->tl2.y);
	} TOPVEC_LOOP_FORW_END();
    }
}

/**
    Label width calcuation.  Depending on the scale of the axis,
    the type of the axis, and the display format of the axis labels,
    different digit length labels are needed to make the label value
    meaningful.  We quantify this by recognizing that adjacent "leading"
    digits of a label will be the same, and for meaningfullness, their
    tail digits must differ.  We also the user to specify how many digits
    must differ, up to some maximum via the resources:
	 *Axis.minTicLabelDiffer:
	 *Axis.maxTicLabelLength:
    Internally, we compute the worst case (i.e., over all possible
    two adjcent labels) number of digits that are the same
    {XPAxisWdg.NumSameDigits}.  We then add this to minTicLabelDiffer
    to compute the total number of digits that must be displayed.

    This must be modified according to current label format:
	SCI - this is easist because all digits are significant.

    For log-scale axis, the worse case occurs ...
**/

static void
_xpAxisAddTick( XPAxisWdg *pAxis, TOPMask flags, double wVal) {
    XPAxisTick	*pTick;
    pTick = &TOPVEC_APPEND( &pAxis->Tics, XPAxisTick);
    pTick->Flags = flags;
    pTick->wPos = wVal;
    pTick->wVal = wVal;
}

static void
_xpAxisCalcLinSubLabels( XPAxisWdg *pAxis, int numDivs,
  XPAxisFlt wBase, XPAxisFlt wSubStep, TOPLogical negB) {
    int		sl;
    double	wSubVal;

    for ( sl=1; sl < numDivs; sl++) {
	wSubVal = wBase + sl * wSubStep;
	if ( wSubVal <= pAxis->wLo || wSubVal >= pAxis->wHi )
	    continue;
	_xpAxisAddTick( pAxis, 0, negB ? -wSubVal : wSubVal);
    }
}

/**
    This function decides where to put the labels for linear scaling.
    Most of the original code here (xpole versions 1.2 and prior)
    has been tossed and replaced by the ideas from "graph-1.0"
    by Sani Nassif and George Howlett.
**/
static void
_xpAxisCalcLinLabelsCore( XPAxisWdg *pAxis, XPAxisFlt wLo, XPAxisFlt wHi,
  int maxmajor, TOPLogical negB) {
    double	wVal, wDiff, wStep, wMinTick, wMaxTick, wSubStep, wScale;
    double	wStepBase, wMaxVal;
    int		nl, numtics, numDivs;

    if ( maxmajor < 2 ) {
	topWarnMsg("%s: No room for labels (lin,%d)\n", pAxis->Tag, maxmajor);
	return;
    }
    if ( pAxis->vertB && maxmajor >= 5 )
	maxmajor /= 2;
    /* 
     * Check for invalid range.  This shouldnt be needed (its taken
     * care of by a higher level), but it cant hurt to check again.
     */
    if ( wLo > wHi ) {
	TOP_SWAPTYPE(double,wLo,wHi);
    } else if ( wLo == wHi ) {
	wLo = wLo * 0.9 - 1.0;
	wHi = wHi * 1.1 + 1.0;
    }
    wDiff = wHi - wLo;
    wScale = 1.0; topFmtGetScale( pAxis->realTickFmt, &wScale);
    wStepBase = 10.0;
    wStep = wScale * exp10(floor(log10(wDiff)));
    for ( nl=0; ; nl++) {
	wMinTick = TOP_CEIL(wLo, wStep);
	wMaxTick = TOP_FLOOR(wHi, wStep);
	numtics = wMinTick > wMaxTick ? 0
	  : TOP_Int(rint((wMaxTick - wMinTick)/wStep)) + 1;
	if ( numtics > maxmajor ) {
	    wStep *= wStepBase; wStepBase /= 2.0;
	} else if ( numtics < 3 ) {
	    wStep /= wStepBase;
	} else {
	    break;
	}
	if ( nl > 5 ) {
	    TOP_PDBG(("%s: linlabelcalc: too many iteractions", pAxis->Tag));
	    break;
	}
    }

    numDivs = pAxis->reqMinorDivs;
    if ( numDivs < 0 ) {
	numDivs = 5;
    }
    wSubStep =  numDivs > 0 ? wStep / numDivs : 1.0;
    _xpAxisCalcLinSubLabels( pAxis, numDivs, wMinTick-wStep, wSubStep, negB);
    for ( nl=0; nl < numtics; nl++) {
        wVal = wMinTick + nl * wStep;
	_xpAxisAddTick( pAxis, XPAxisTicF_Major | XPAxisTicF_Label, wVal);
	_xpAxisCalcLinSubLabels( pAxis, numDivs, wVal, wSubStep, negB);
    }
    wMaxVal = TOP_MAX(TOP_ABS(wMinTick),TOP_ABS(wMaxTick));
    pAxis->NumTicLabSameDigits =
      TOP_Int(ceil( -log10(TOP_ABS(wStep)))) - TOP_Int(ceil( -log10(wMaxVal)));
}

static void
_xpAxisCalcLinLabels( XPAxisWdg *pAxis, XPAxisFlt wLo, XPAxisFlt wHi) {
    _xpAxisCalcLinLabelsCore( pAxis, wLo, wHi, pAxis->MaxMajorTics, TOP_FALSE);
}

/**
    Adds labels starting at decade {eStart}, for {eNum} decades.
    The first label in the decade will be labeled major.  The
    other eNum-1 decade ticks will be unlabeled major, and ticks
    within a decate will be unlabeled minor.
**/
static void
_xpAxisCalcLogSubLabels( XPAxisWdg *pAxis, int eStart, int eNum,
  double wLo, double wHi, TOPLogical negB) {
    int		decCnt, linCnt;
    double	wVal, wValNext, wValSub;

    wVal = exp10((double)eStart);
    for ( decCnt=0; decCnt < eNum; decCnt++, wVal = wValNext) {
	if ( (wValNext = wVal * 10.0) < wLo )	continue;
	if ( wVal >= wLo ) {
	    _xpAxisAddTick( pAxis,
	      (decCnt==0?XPAxisTicF_Label:0)|XPAxisTicF_Major,
	      negB ? - wVal : wVal);
	}
	for ( linCnt=2; linCnt <= 9; linCnt++) {
	    wValSub = wVal*linCnt;
	    if ( wValSub < wLo )	continue;
	    if ( wValSub > wHi )	return;
	    _xpAxisAddTick( pAxis, 0, negB ? - wValSub : wValSub);
	}
    }
}

static void DoCalcLogLabels( XPAxisWdg *pAxis, XPAxisFlt wLo, XPAxisFlt wHi,
  int maxmajor, TOPLogical negB) {
    int		eMinMajor, eMaxMajor, eDiff, ePerMajor, eCnt;
    double	eLo, eHi;

    if ( maxmajor <= 1 ) {
	topWarnMsg("No room for labels (log)\n", maxmajor);
	return;
    }
    if ( wLo <= pAxis->wClip )
	wLo = pAxis->wClip;
    if ( wLo <= 0 || wLo >= wHi ) {
        topWarnMsg("Invalid range for log scale (%g,%g,%d)", wLo, wHi,maxmajor);
        wLo = 1.0; wHi = 100.0;
    }
    eLo = log10(wLo);		eHi = log10(wHi);
    eMinMajor = ceil(eLo);	eMaxMajor = floor(eHi);
    eDiff = eMaxMajor - eMinMajor + 1;
    if ( eDiff < 2 ) {
	/* Log labeling over a small area is very much like linear */
        _xpAxisCalcLinLabelsCore( pAxis, wLo, wHi, maxmajor, negB);
	return;
    }
    /* yeah: just label the (atleast) two major points */
    ePerMajor = ((eDiff-1) / maxmajor) + 1;
    if ( pAxis->vertB && eDiff >= 4*ePerMajor )
	ePerMajor *= 2;
    eMinMajor = TOP_FLOOR(eMinMajor,(double)ePerMajor);
    eMaxMajor = TOP_CEIL( eMaxMajor,(double)ePerMajor);
    for ( eCnt=eMinMajor-ePerMajor; eCnt <= eMaxMajor; eCnt += ePerMajor) {
	_xpAxisCalcLogSubLabels( pAxis, eCnt, ePerMajor, wLo, wHi, negB);
    }
    pAxis->NumTicLabSameDigits = 0;
}

static void CalcLogLabels( XPAxisWdg *pAxis, XPAxisFlt wLo, XPAxisFlt wHi) {
    DoCalcLogLabels( pAxis, wLo, wHi, pAxis->MaxMajorTics, FALSE);
}

static void CalcDSLogLabels( XPAxisWdg *pAxis, XPAxisFlt wLo, XPAxisFlt wHi) {
    XPAxisFlt		wNegLo, wNegHi;
    int			nummaj = pAxis->MaxMajorTics;

    /* th wLo, wHi passed in is the PosWin */
    xpXfaDSLogGetNegWin( &pAxis->TF, &wNegLo, &wNegHi);
    if ( wNegLo > 0 && wLo < wHi ) {
        DoCalcLogLabels( pAxis, wLo, wHi, nummaj, FALSE);
    } else if ( wHi < 0 && wNegLo < wNegHi ) {
        DoCalcLogLabels( pAxis, fabs(wNegHi), fabs(wNegLo), nummaj, TRUE);
    } else {
	double f = pAxis->TF.fNegFraction;
        DoCalcLogLabels( pAxis, fabs(wNegHi), fabs(wNegLo), 
	  (int)(f*nummaj), TRUE);
        DoCalcLogLabels( pAxis, wLo, wHi, (int)((1-f)*nummaj), FALSE);
    }
}

void 
_xpAxisCalcLabels( XPAxisWdg *pAxis) {
    XPAxisFlt	wLo, wHi;
    int		numMaxLabels, expectdigs, err;

    if ( pAxis->scaleMode == XPXfaMode_Null || pAxis->tickFont == NULL 
      || pAxis->vDataWdgLen < 10 ) {
	goto error;
    }
    expectdigs = pAxis->tickLabDigits;
    if ( pAxis->vertB ) {
        numMaxLabels = pAxis->vDataWdgLen / (1.5 * pAxis->tickCI_height);
    } else {
	numMaxLabels = pAxis->vDataWdgLen
	  / ((expectdigs+2) * pAxis->tickCI.width);
    }
    pAxis->MaxMajorTics = numMaxLabels;

    xpXfaGetWin( &pAxis->TF, &wLo, &wHi);
    TOP_PDBG((SPKG,"%s: calclabels: win=(%g,%g)\n", pAxis->Tag, wLo, wHi));

    topVecSetNum( &pAxis->Tics, 0);

    switch ( pAxis->scaleMode ) {
    case XPXfaMode_Lin:		_xpAxisCalcLinLabels( pAxis, wLo, wHi);   break;
    case XPXfaMode_Log:		CalcLogLabels( pAxis, wLo, wHi);   break;
    case XPXfaMode_DSLog:	CalcDSLogLabels( pAxis, wLo, wHi); break;
    default:
	topProgMsg("XPAxis: %s: bad scale mode", pAxis->Tag);
    } 
    if ( pAxis->NumTicLabSameDigits < 0 )
	pAxis->NumTicLabSameDigits = 0;
    if ( pAxis->NumTicLabSameDigits > 10 )
	pAxis->NumTicLabSameDigits = 10;
 
    pAxis->tickLabDigits = topFmtDblCalcRange( pAxis->tickFmt,
      TOPVEC_HEAD(&pAxis->Tics,XPAxisTick).wVal,
      TOPVEC_TAIL(&pAxis->Tics,XPAxisTick).wVal,
      pAxis->NumTicLabSameDigits, pAxis->realTickFmt);
    if ( pAxis->tickLabDigits <= 1 )
	goto error;
    pAxis->vTicLabWidth = pAxis->tickCI.width * pAxis->tickLabDigits;
    TOP_PDBG((SPKG,"%s: labelstats: num %d same %d tot %d wid %d fmt %s",
      pAxis->Tag,
      TOPVEC_Num(&pAxis->Tics), pAxis->NumTicLabSameDigits, 
      pAxis->tickLabDigits, pAxis->vTicLabWidth, pAxis->realTickFmt));

    err = expectdigs - pAxis->tickLabDigits;
    if ( err < 0 || err > 1 ) {
	if ( pAxis->vertB ) {
	    /* we must grow or shrink axis width */
	    _xpAxisIdleCalc( pAxis, XPAxisF_CalcGeom);
	} else {
	    static int RecurCount = 0;
	    /* labels are either overlapping or too far apart */
	    /* we have to average in order to prevent unstable feedback */
	    TOP_PDBG((SPKG,"%s: calc: horiz label width recursion: %d %d",
	      pAxis->Tag, expectdigs, pAxis->tickLabDigits));
	    pAxis->tickLabDigits = (1+expectdigs+pAxis->tickLabDigits)/2;
	    if ( ++RecurCount < 5 )
	        _xpAxisCalcLabels( pAxis);
	    --RecurCount;
	    return;
	}
    }

    pAxis->Flags &= ~XPAxisF_LabelText;

/*done:;*/
    pAxis->Flags |= XPAxisF_CalcExpose;
    pAxis->Flags &= ~XPAxisF_CalcLabels;
    return;
error:;
    TOP_PDBG((SPKG,"%s: labels: error...clearing", pAxis->Tag));
    pAxis->tickLabDigits = 1;
    pAxis->vTicLabWidth = pAxis->tickCI.width * pAxis->tickLabDigits;
    topVecClear( &pAxis->Tics);
    return;
}

