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

 Programmer: Kennard White

    XPAxis

    Tk axis widget (axis labels for graphs)

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
#include "eemDflt.h"

#include "xpDefaults.h"
#include "xpXfa.h"
#include "xpAxis.h"
#include "axisInt.h"

#define SPKG _XpAxisSubPackage

static void _xpAxisSetTraceVarsXf( Tcl_Interp *ip, XPAxisWdg *pAxis);
static void _xpAxisMakeRangeNonTrivial( XPAxisWdg *pAxis);


/*
 * Information used for argv parsing.  Resources.
 */

#define BLACK           "Black"
#define WHITE           "White"
#define BISQUE1         "#ffe4c4"
#define MAROON          "#b03060"

#define DEF_AXIS_AUTOZOOM	"true"
#define DEF_AXIS_SIDE		"top"
#define	DEF_AXIS_FLIP		"false"
#define DEF_AXIS_SCALE		"lin"
#define DEF_AXIS_LO		"1.0"
#define AXIS_MID_KEY		1234567890
#define DEF_AXIS_MID		"1234567890"	/* ignored */
#define DEF_AXIS_HI		"10.0"
#define DEF_AXIS_CLIP		"1e-25"
#define DEF_AXIS_CLIPWIDTH	"20"
#define DEF_AXIS_RANGEPAD	"0.0"
#define DEF_AXIS_BG_COLOR	DEF_GRAPH_BACKGROUND
#define DEF_AXIS_BG_MONO	WHITE
#define DEF_AXIS_LABELFONT	"fixed"
#define DEF_AXIS_TICKMODE	"inside"
#define DEF_AXIS_TICK_COLOR	DEF_GRAPH_FOREGROUND
#define DEF_AXIS_TICK_MONO	BLACK
#define DEF_AXIS_TICKFONT	"fixed"
#define DEF_AXIS_TICKFMT	"%^.^1E"
#define DEF_AXIS_MINORTICLEN	"4"
#define DEF_AXIS_MAJORTICLEN	"7"
#define DEF_AXIS_MINORDIVS	"5"
#define DEF_AXIS_MAJORSTEP	"1.0"
#define DEF_AXIS_AUTOSIZE	"true"
#define DEF_AXIS_AUTOSIZEPAD	"1.1"
#define DEF_AXIS_RANGELO	"1.0"
#define DEF_AXIS_RANGEHI	"1.0"

static Tk_ConfigSpec _XpAxisConfigSpecs[] = {
    /* mode&state */
    {TK_CONFIG_WINDOW, "-datawindow", "dataWindow", "DataWindow",
	(char *) NULL, Tk_Offset(XPAxisWdg, dataWdgNew), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-command", "command", "Command",
	(char *) NULL, Tk_Offset(XPAxisWdg, dataWdgCmd), TK_CONFIG_NULL_OK},
    {TK_CONFIG_WINDOW, "-scrollwindow", "scrollWindow", "ScrollWindow",
	(char *) NULL, Tk_Offset(XPAxisWdg, scrollWdg), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-scrollcommand", "scrollCommand", "ScrollCommand",
	(char *) NULL, Tk_Offset(XPAxisWdg, scrollCmd), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BOOLEAN, "-showscrollbar", "showScrollbar", "ShowScrollbar",
	"1", Tk_Offset(XPAxisWdg, scrollB), 0},
    {TK_CONFIG_BOOLEAN, "-autosize", "autoSize", "AutoSize",
	DEF_AXIS_AUTOSIZE, Tk_Offset(XPAxisWdg, autoSizeB), 0},
    {TK_CONFIG_DOUBLE, "-autosizepad", "autoSizePad", "AutoSizePad",
	DEF_AXIS_AUTOSIZEPAD, Tk_Offset(XPAxisWdg, autoSizePad), 0},
    {TK_CONFIG_STRING, "-menu", "menu", "Menu",
	(char*) NULL, Tk_Offset(XPAxisWdg, menuName), 0},
    {TK_CONFIG_UID, "-state", "state", "State",
	"normal", Tk_Offset(XPAxisWdg, stateUid), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	"flat", Tk_Offset(XPAxisWdg, relief), 0},

    /* transform */
    {TK_CONFIG_UID, "-side", "side", "Side",
	DEF_AXIS_SIDE, Tk_Offset(XPAxisWdg, sideUid), 0},
    {TK_CONFIG_BOOLEAN, "-flip", "flip", "Flip",
	DEF_AXIS_FLIP, Tk_Offset(XPAxisWdg, flipB), 0},
    {TK_CONFIG_UID, "-scale", "scale", "Scale",
	DEF_AXIS_SCALE, Tk_Offset(XPAxisWdg, scaleUid), 0},
    {TK_CONFIG_DOUBLE, "-lo", "lo", "Lo",
	DEF_AXIS_LO, Tk_Offset(XPAxisWdg, wLo), 0},
    {TK_CONFIG_DOUBLE, "-hi", "hi", "Hi",
	DEF_AXIS_HI, Tk_Offset(XPAxisWdg, wHi), 0},
    {TK_CONFIG_DOUBLE, "-clip", "clip", "Clip",
	DEF_AXIS_CLIP, Tk_Offset(XPAxisWdg, wClip), 0},
    {TK_CONFIG_INT, "-clipwidth", "clipWidth", "ClipWidth",
	DEF_AXIS_CLIPWIDTH, Tk_Offset(XPAxisWdg, vClip), 0},
    {TK_CONFIG_DOUBLE, "-rangepad", "rangePad", "RangePad",
	DEF_AXIS_RANGEPAD, Tk_Offset(XPAxisWdg, padFactor), 0},
    {TK_CONFIG_DOUBLE, "-rangelo", "rangeLo", "RangeLo",
	DEF_AXIS_RANGELO, Tk_Offset(XPAxisWdg, wRangeLo), 0},
    {TK_CONFIG_DOUBLE, "-rangehi", "rangeHi", "RangeHi",
	DEF_AXIS_RANGEHI, Tk_Offset(XPAxisWdg, wRangeHi), 0},
    {TK_CONFIG_DOUBLE, "-scrolllo", "scrollLo", "Lo",
	DEF_AXIS_LO, Tk_Offset(XPAxisWdg, wScrollLo), 0},
    {TK_CONFIG_DOUBLE, "-scrollhi", "scrollHi", "Hi",
	DEF_AXIS_HI, Tk_Offset(XPAxisWdg, wScrollHi), 0},

    /* general visuals */
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	(char*) NULL, Tk_Offset(XPAxisWdg, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_AXIS_BG_COLOR, Tk_Offset(XPAxisWdg, bgBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, NULL, NULL, NULL,
	NULL, Tk_Offset(XPAxisWdg, bgColor), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_AXIS_BG_MONO, Tk_Offset(XPAxisWdg, bgBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, NULL, NULL, NULL,
	NULL, Tk_Offset(XPAxisWdg, bgColor), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_STRING, "-label", "label", "Label",
	NULL, Tk_Offset(XPAxisWdg, labelStr), TK_CONFIG_NULL_OK},
    {TK_CONFIG_FONT, "-labelfont", "labelFont", "Font",
	DEF_AXIS_LABELFONT, Tk_Offset(XPAxisWdg, labelFont), 0},
    {TK_CONFIG_BOOLEAN, "-showlabel", "showLabel", "ShowLabel",
	"1", Tk_Offset(XPAxisWdg, showLabelB), 0},
    {TK_CONFIG_COLOR, "-clipcolor", "clipColor", "Foreground",
	"blue", Tk_Offset(XPAxisWdg, clipColor),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-clipcolor", "clipColor", "Foreground",
	BLACK, Tk_Offset(XPAxisWdg, clipColor),
	TK_CONFIG_MONO_ONLY},

    /* ticks */
    {TK_CONFIG_UID, "-tickmode", "tickmode", "TickMode",
	DEF_AXIS_TICKMODE, Tk_Offset(XPAxisWdg, tickmodeUid), 0},
    {TK_CONFIG_COLOR, "-tickcolor", "tickColor", "Foreground",
	DEF_AXIS_TICK_COLOR, Tk_Offset(XPAxisWdg, tickColor),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-tickcolor", "tickColor", "Foreground",
	DEF_AXIS_TICK_MONO, Tk_Offset(XPAxisWdg, tickColor),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-foreground", "tickColor", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-fg", "tickColor", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_FONT, "-tickfont", "tickFont", "Font",
	DEF_AXIS_TICKFONT, Tk_Offset(XPAxisWdg, tickFont), 0},
    {TK_CONFIG_STRING, "-tickfmt", "tickFmt", "TickFmt",
	DEF_AXIS_TICKFMT, Tk_Offset(XPAxisWdg, tickFmt), 0},
    {TK_CONFIG_INT, "-minordivs", "minorDivs", "MinorDivs",
	DEF_AXIS_MINORDIVS, Tk_Offset(XPAxisWdg, reqMinorDivs), 0},
    {TK_CONFIG_DOUBLE, "-step", "step", "Step",
	DEF_AXIS_MAJORSTEP, Tk_Offset(XPAxisWdg, reqMajorStep), 0},
    {TK_CONFIG_INT, "-minorticklen", "minorTickLen", "TickLen",
	DEF_AXIS_MINORTICLEN, Tk_Offset(XPAxisWdg, vMinorTicLen), 0},
    {TK_CONFIG_INT, "-majorticklen", "majorTickLen", "TickLen",
	DEF_AXIS_MAJORTICLEN, Tk_Offset(XPAxisWdg, vMajorTicLen), 0},

    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

static Tk_Uid _XpAxisLeftUid = NULL;
static Tk_Uid _XpAxisRightUid = NULL;
static Tk_Uid _XpAxisTopUid = NULL;
static Tk_Uid _XpAxisBottomUid = NULL;
static Tk_Uid _XpAxisNoneUid = NULL;
static Tk_Uid _XpAxisInsideUid = NULL;
static Tk_Uid _XpAxisOutsideUid = NULL;

static void _xpAxisIdleCalcUndo( XPAxisWdg *pAxis);


static void
_xpAxisDestroyCB( ClientData cld) {
    XPAxisWdg	*pAxis = (XPAxisWdg *) cld;

    /* Tag is Tk_PathName */

    if (pAxis->dataWdgCmd != NULL) {
	memFree(pAxis->dataWdgCmd);
	pAxis->dataWdgCmd = NULL;
    }
    if (pAxis->bgBorder != NULL) {
	Tk_Free3DBorder(pAxis->bgBorder);
	pAxis->bgBorder = NULL;
    }
    if (pAxis->bgColor != NULL) {
	Tk_FreeColor(pAxis->bgColor);
	pAxis->bgColor = NULL;
    }
    if (pAxis->tickColor != NULL) {
	Tk_FreeColor(pAxis->tickColor);
	pAxis->tickColor = NULL;
    }
    if (pAxis->tickFont != NULL) {
	Tk_FreeFontStruct(pAxis->tickFont);
	pAxis->tickFont = NULL;
    }
    if (pAxis->tickGC != None) {
	Tk_FreeGC(pAxis->dpy, pAxis->tickGC);
	pAxis->tickGC = None;
    }
    topVecDestructor( &pAxis->Tics);
    if (pAxis->tickFmt != NULL) {
	memFree(pAxis->tickFmt);
	pAxis->tickFmt = NULL;
    }
    MEM_FREE(pAxis);
}

static void
_xpAxisDestroy( XPAxisWdg *pAxis) {
    if ( pAxis->tkwin != NULL ) {
	Tcl_DeleteCommand( pAxis->ip, Tk_PathName(pAxis->tkwin));
	/* unset the state variable (kills trace at same time) */
	Tcl_UnsetVar2( pAxis->ip, Tk_PathName(pAxis->tkwin), NULL,
	  TCL_GLOBAL_ONLY);
	pAxis->tkwin = NULL;
    }
    _xpAxisIdleCalcUndo( pAxis);
    Tk_EventuallyFree( (ClientData) pAxis, _xpAxisDestroyCB);
}

/*****************************************************************************
				Theory of Operation
		(aka why this collection of hacks sometimes works)

	There are several points of note:
	1) Our primary goal must be to prevent duplication redraws
	   of the graphing area.  We assume that redraws of the graph
	   is much more expensive than our internal calculations here
	   (not true for simple graphs, but true for things like root-loci).
	2) This axis is unlike many others in that we adaptively change
	   the number of digits in our tick labels.  The may cause the
	   axis to resize its own geometry.  But the labels themselves
	   depend on the geometry, so there is a feedback loop.  This
	   complicates the code.

	The following are the core functions/steps.
	GeomChk		Extracts the relevant info about our geometry.
			If it has "changed", it will initiate recalc.
	ConfigXf	Set the xfa based on the cur window geom and
			the limits, etc.  Whenever this changes,
			it will set flag for ConfigXfNotify.
	Labels		Recalculate all the labeling points.  Note that
			the label strings themselves arn't generated
			here, but rather in Expose under control of
			the LabelText flags.
	ConfigGeom	See if we want to use a different geometry based
			on our label widths, heights.
	ConfigXfNotify	Tell the graphing widget to redraw.  Also
			set the axis variable {xform} to describe
			the current transform.
	Vars		Set all the config-dependent axis variables.
			This happens whenever the primary axis config
			is called.
	Expose		Redraw the pixels.

*****************************************************************************/

static void
_xpAxisSetTraceVarsXf( Tcl_Interp *ip, XPAxisWdg *pAxis) {
    char	*n1 = Tk_PathName(pAxis->tkwin);
    char	*n2 = "xform";
    char	buf[200], *curval;
    int		vVal;

    vVal = pAxis->TF.hiView - pAxis->TF.loView;
    sprintf( buf, "%.15g %.15g %d %.15g", pAxis->wLo, pAxis->wHi,
      TOP_ABS(vVal), pAxis->wClip);
    curval = Tcl_GetVar2(ip, n1, n2, TCL_GLOBAL_ONLY);
    if ( curval != NULL && strcmp(curval,buf)==0 )
	return;
    Tcl_SetVar2(ip, n1, n2, buf, TCL_GLOBAL_ONLY);
}

/**
    Reset the scroll range {wScrollLo,wScrollHi} so that the current
    window range {wLo,wHi} occupies the center of the scroll range
    and is 1/10th the total scroll area.  For now, assume linear.
**/
static void
_xpAxisResetScroll( XPAxisWdg *pAxis) {
    double	wLen;
    wLen = pAxis->wHi - pAxis->wLo;
    pAxis->wScrollLo = pAxis->wLo - 4*wLen;
    pAxis->wScrollHi = pAxis->wHi + 4*wLen;
    xpXfaConfig( &pAxis->STF, pAxis->scaleMode,
      0, XPAXIS_SCROLLRES, pAxis->wScrollLo, pAxis->wScrollHi,
      pAxis->vClip, pAxis->wClip);
}

static void
_xpAxisConfigScrollChk( XPAxisWdg *pAxis) {
    int	sLen;
    if ( pAxis->wScrollLo > pAxis->wLo ) pAxis->wScrollLo = pAxis->wLo;
    if ( pAxis->wScrollHi < pAxis->wHi ) pAxis->wScrollHi = pAxis->wHi;
    xpXfaConfig( &pAxis->STF, pAxis->scaleMode,
      0, XPAXIS_SCROLLRES, pAxis->wScrollLo, pAxis->wScrollHi,
      pAxis->vClip, pAxis->wClip);
    sLen = xpXfaRangeToView( &pAxis->STF, pAxis->wLo, pAxis->wHi);
    if ( sLen < ((int)(.04*XPAXIS_SCROLLRES))
      || sLen > ((int)(.90*XPAXIS_SCROLLRES)) ) {
	_xpAxisResetScroll( pAxis);
    }
}

static int
_xpAxisConfigXf( Tcl_Interp *ip, XPAxisWdg *pAxis) {
    Tk_Window	aw, dw;
    int		dataWdgLen, vLo, vHi;

    pAxis->Flags &= ~XPAxisF_CalcXf;
    aw = pAxis->tkwin;
    dw = pAxis->dataWdg ? pAxis->dataWdg : aw;
    if ( pAxis->vertB ) {
        dataWdgLen = Tk_Height(dw);
	pAxis->vDataWdgOfs = Tk_Y(dw) - Tk_Y(aw);
	pAxis->vDataWdgLen = Tk_Height(dw);
    } else {
        dataWdgLen = Tk_Width(dw);
	pAxis->vDataWdgOfs = Tk_X(dw) - Tk_X(aw);
	pAxis->vDataWdgLen = Tk_Width(dw);
    }
    if ( ! pAxis->flipB ) {
        vLo = 0; vHi = dataWdgLen;
    } else {
        vLo = dataWdgLen; vHi = 0;
    }
    if ( xpXfaConfig( &pAxis->TF, pAxis->scaleMode,
      vLo, vHi, pAxis->wLo, pAxis->wHi,
      pAxis->vClip, pAxis->wClip) < 0 ) {
	topTclRetFmt( ip, "%s: xfa config failed\n", pAxis->Tag);
	return TCL_ERROR;
    }
    pAxis->wMid = xpXfaToWin( &pAxis->TF, dataWdgLen/2);
    if ( dataWdgLen != pAxis->vLen ) {
	pAxis->vLen = dataWdgLen;
    }
    if ( pAxis->scrollB && pAxis->scrollCmd ) {
	_xpAxisConfigScrollChk( pAxis);
    }
    _xpAxisIdleCalc( pAxis, XPAxisF_CalcGeom|XPAxisF_CalcLabels
      |XPAxisF_CalcXfNotify|XPAxisF_CalcExpose);
    return TCL_OK;
}

static int
_xpAxisConfigGeom( Tcl_Interp *ip, XPAxisWdg *pAxis) {
    Tk_Window	tkw = pAxis->tkwin;
    int		vW = 0, vH = 0, rH, rW, dummy;

    pAxis->Flags &= ~XPAxisF_CalcGeom;
    TOP_PDBG((SPKG,"%s: config geom (%d %d)", pAxis->Tag, 
      Tk_Width(tkw), Tk_Height(tkw)));

    if ( pAxis->labelLen > 0 ) {
	XTextExtents(pAxis->labelFont, pAxis->labelStr, pAxis->labelLen,
	  &dummy, &dummy, &dummy, &pAxis->labelBbox);
    }

    if ( pAxis->scrollB && pAxis->scrollWdg ) {
	Tk_Window	sbw = pAxis->scrollWdg;
	int		sbX, sbY, sbW, sbH;
	switch ( pAxis->side ) {
	case XPAxisSide_Right:
	    sbW = Tk_ReqWidth(sbw); sbH = Tk_Height(tkw);
	    sbX = 0; sbY = 0;
	    vW += sbW;
	    break;
	case XPAxisSide_Left:
	    sbW = Tk_ReqWidth(sbw); sbH = Tk_Height(tkw);
	    sbX = Tk_Width(tkw) - sbW; sbY = 0;
	    vW += sbW;
	    break;
	case XPAxisSide_Bottom:
	    sbW = Tk_Width(tkw); sbH = Tk_ReqHeight(sbw);
	    sbX = 0; sbY = 0;
	    vH += sbH;
	    break;
	case XPAxisSide_Top:
	    sbW = Tk_Width(tkw); sbH = Tk_ReqHeight(sbw);
	    sbX = 0; sbY = Tk_Height(tkw) - sbH;
	    vH += sbH;
	    break;
	default: TOP_ABORT(SPKG); return TCL_ERROR;
	}
	if ( Tk_X(sbw) != sbX || Tk_Y(sbw) != sbY )
	    Tk_MoveWindow(sbw, sbX, sbY);
	if ( Tk_Width(sbw) != sbW || Tk_Height(sbw) != sbH )
	    Tk_ResizeWindow(sbw, (unsigned)sbW, (unsigned)sbH);
	Tk_MapWindow(sbw);
    }

    if ( pAxis->vertB ) {
        vH = Tk_Height(tkw);
	vW += pAxis->vTicLabWidth + pAxis->tickCI.width/2;
	if ( pAxis->tickOutsideB ) {
	    vW += pAxis->vMajorTicLen + pAxis->tickCI.width/2;
	}
    } else {
        vW = Tk_Width(tkw);
	vH += pAxis->tickCI_height + XPAXIS_PAD_VERT;
	if ( pAxis->labelLen > 0 ) {
	    vH += pAxis->labelBbox.ascent + pAxis->labelBbox.descent 
	      + XPAXIS_PAD_VERT;
	}
	if ( pAxis->tickOutsideB ) {
	    vH += pAxis->vMajorTicLen + XPAXIS_PAD_VERT;
	}
    }
    rH = Tk_ReqHeight(tkw);	if ( rH < 5 )	rH = Tk_Height(tkw);
    rW = Tk_ReqWidth(tkw);	if ( rW < 5 )	rW = Tk_Width(tkw);
    if ( vW != rW || vH != rH ) {
	TOP_PDBG((SPKG,"%s: calc: geom req %dx%d %d", pAxis->Tag, vW, vH,
	  pAxis->vTicLabWidth));
	Tk_GeometryRequest( tkw, vW, vH);
        /* pAxis->Flags |= XPAxisF_CalcXf; */
    }

    return TCL_OK;
}

/**
    This is the geometry manager callback for the scollbar window.
**/
static void
_xpAxisScrollbarGeomCB( ClientData cld, Tk_Window sbw) {
    XPAxisWdg	*pAxis = (XPAxisWdg*) cld;

    _xpAxisConfigGeom( pAxis->ip, pAxis);    
}

static void
_xpAxisGeomCheck( XPAxisWdg *pAxis) {
    Tk_Window		tkw = pAxis->tkwin;

    pAxis->Flags &= ~XPAxisF_CalcGeomChk;
    if ( tkw == NULL )
	return;
#ifdef notdef
    if ( pAxis->vertB ) {
	if ( Tk_Width(tkw) == pAxis->axisGeom.width )
	    return;
    } else {
	if ( Tk_Height(tkw) == pAxis->axisGeom.height )
	    return;
    }
    pAxis->axisGeom.x = Tk_X(tkw);
    pAxis->axisGeom.y = Tk_Y(tkw);
    pAxis->axisGeom.width = Tk_Width(tkw);
    pAxis->axisGeom.height = Tk_Height(tkw);
#endif
    /* _xpAxisConfigGeom( pAxis->ip, pAxis); */
    /*
     * In a perfect world we should check to see if our geometry actually
     * changed.  But its difficult to do that correctly when considering
     * the interactions with requested geoms that might or might
     * not be granted (do you predict it will be or wont be??).
     * So instead, we always recalc.
     */
    pAxis->Flags |= XPAxisF_GeomXfNotify;
    _xpAxisIdleCalc( pAxis, XPAxisF_CalcXf);
}

static void
_xpAxisIdleCalcCB( ClientData cld) {
    XPAxisWdg *pAxis = (XPAxisWdg *) cld;
    
    pAxis->Flags &= ~XPAxisF_IdleCalc;

    Tcl_ResetResult(pAxis->ip);
    TOP_PDBG((SPKG,"%s: idle calc %04x", pAxis->Tag, pAxis->Flags));
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcGeomChk) ) {
        _xpAxisGeomCheck( pAxis);
    }
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcXf) ) {
        if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcNonTriv) ) {
	    pAxis->Flags &= ~XPAxisF_CalcNonTriv;
	    _xpAxisMakeRangeNonTrivial( pAxis);
	}
        _xpAxisConfigXf( pAxis->ip, pAxis);
    }
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcLabels) ) {
        _xpAxisCalcLabels( pAxis);
    }
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcGeom) ) {
        _xpAxisConfigGeom( pAxis->ip, pAxis);
    }
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcXfNotify) ) {
        _xpAxisSetTraceVarsXf( pAxis->ip, pAxis);
	if ( TOP_IsOn( pAxis->Flags, XPAxisF_IdleCalc) )	return;

	if ( pAxis->scrollB && pAxis->scrollCmd ) {
	    int vLo = xpXfaToView( &pAxis->STF, pAxis->wLo);
	    int vHi = xpXfaToView( &pAxis->STF, pAxis->wHi);
	    if ( pAxis->flipB ) {
		vLo = XPAXIS_SCROLLRES - vLo; vHi = XPAXIS_SCROLLRES - vHi;
		TOP_SWAPTYPE(int,vLo,vHi);
	    }
	    if ( vLo < vHi ) {
		/* If the transform is not valid, this might fail */
	        topTclBgEvalFmt(pAxis->ip,SPKG, "%s %d %d %d %d",
	          pAxis->scrollCmd, XPAXIS_SCROLLRES, vHi-vLo, vLo, vHi);
	    }
	}
    }

    if ( TOP_IsOn( pAxis->Flags, XPAxisF_IdleCalc) )	return;
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcXfNotify) ) {
	pAxis->Flags &= ~XPAxisF_CalcXfNotify;
	if ( pAxis->dataWdgCmd != NULL ) {
	    if ( TOP_IsOn( pAxis->Flags, XPAxisF_GeomXfNotify) ) {
	        topTclBgEvalFmt( pAxis->ip, SPKG, "%s 1", pAxis->dataWdgCmd);
		pAxis->Flags &= ~XPAxisF_GeomXfNotify;
	    } else {
	        topTclBgEval( pAxis->ip, SPKG, pAxis->dataWdgCmd);
	    }
	}
	if ( TOP_IsOn( pAxis->Flags, XPAxisF_IdleCalc) )	return;
    }
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcVars) ) {
	pAxis->Flags &= ~XPAxisF_CalcVars;
	Tcl_SetVar2( pAxis->ip, pAxis->Tag, "change-config", "1",
	  TCL_GLOBAL_ONLY);
	if ( TOP_IsOn( pAxis->Flags, XPAxisF_IdleCalc) )	return;
    }
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_CalcExpose) ) {
	pAxis->Flags &= ~XPAxisF_CalcExpose;
        _xpAxisExpose( pAxis);
    }
}

void
_xpAxisIdleCalc( XPAxisWdg *pAxis, int calcMask) {
    pAxis->Flags |= (calcMask & XPAxisF_Calc);
    if ( TOP_IsOn( pAxis->Flags, XPAxisF_IdleCalc)
      || pAxis->tkwin == NULL 
      || !Tk_IsMapped(pAxis->tkwin) ) {
	return;
    }
    pAxis->Flags |= XPAxisF_IdleCalc;
    Tk_DoWhenIdle( _xpAxisIdleCalcCB, (ClientData) pAxis);
}

static void
_xpAxisIdleCalcUndo( XPAxisWdg *pAxis) {
    if ( TOP_IsOn(pAxis->Flags, XPAxisF_IdleCalc) ) {
        Tk_CancelIdleCall( _xpAxisIdleCalcCB, (ClientData) pAxis);
	pAxis->Flags &= ~XPAxisF_IdleCalc;
    }
}

static void
_xpAxisEventCB( ClientData cld, XEvent *pEvent) {
    XPAxisWdg *pAxis = (XPAxisWdg *) cld;

    /*IF*/ if ((pEvent->type == Expose) && (pEvent->xexpose.count == 0)) {
	TOP_PDBG((SPKG,"%s: event Expose", pAxis->Tag));
	_xpAxisIdleCalc( pAxis, XPAxisF_CalcExpose);
    } else if ( pEvent->type == DestroyNotify ) {
	_xpAxisDestroy( pAxis);
    } else if ( pEvent->type == ConfigureNotify || pEvent->type == MapNotify) {
	TOP_PDBG((SPKG,"%s: event ConfigureNotify", pAxis->Tag));
	_xpAxisGeomCheck( pAxis);
    }
}

static void
_xpAxisDataEventCB( ClientData cld, XEvent *pEvent) {
    XPAxisWdg *pAxis = (XPAxisWdg *) cld;

    /*IF*/ if ( pEvent->type == DestroyNotify ) {
	pAxis->dataWdg = NULL;
    } else if ( pEvent->type == ConfigureNotify || pEvent->type == MapNotify) {
	_xpAxisGeomCheck( pAxis);
    }
}

static void
_xpAxisMakeRangeNonTrivial( XPAxisWdg *pAxis) {
    double wDelta;
    double wLo = pAxis->wLo;
    double wHi = pAxis->wHi;

    if ( wLo > wHi )
	TOP_SWAPTYPE(double, wLo, wHi);
    if ( eemDfltSameB(wLo,wHi) ) {
	switch ( pAxis->scaleMode ) {
	case XPXfaMode_Lin:
	    wDelta = eemDfltSameB(wLo,0.0) ? 10.0 : fabs(wLo) * .25;
	    wLo -= wDelta;
	    wHi += wDelta;
	    break;
	case XPXfaMode_Log:
	    /* cant do anything (reasonable) about 0 */
	    wLo /= 10.0;
	    wHi *= 15.0;
	    break;
	case XPXfaMode_DSLog:
	    /* cant do anything about 0 */
	    if ( wLo < 0 )      wLo *= 15.0;    else    wLo /= 10.0;
	    if ( wHi > 0 )      wHi *= 15.0;    else    wHi /= 10.0;
	    break;
	default:
	    topProgMsg("bad switch");
	    return;
	}
    }
    pAxis->wLo = wLo;
    pAxis->wHi = wHi;
}

static void
_xpAxisSetMidRange( XPAxisWdg *pAxis, double wMid) {
    double wDelta, wLo, wHi;

    _xpAxisMakeRangeNonTrivial( pAxis);
    wLo = pAxis->wLo;
    wHi = pAxis->wHi;

    switch ( pAxis->scaleMode ) {
    case XPXfaMode_Lin:
	wDelta = (wHi - wLo)/2.0;
	wLo = wMid - wDelta;
	wHi = wMid + wDelta;
	break;
    case XPXfaMode_Log:
    case XPXfaMode_DSLog:
	topWarnMsg("%s: -mid option is unimplemented for log scales",
	  pAxis->Tag);
	break;
    default:
	topProgMsg("bad switch");
	return;
    }
    pAxis->wLo = wLo;
    pAxis->wHi = wHi;
}

/**
    The parameter {factor} is the growth factor.  Denote the original
    window length (hi - lo) as {w} and the new as {w'}.
     For linear, w' = factor * w, and factor must be positive.
       1.0 ==> No change
       2.0 ==> double win range
       0.5 ==> halve the win range
     For log, ....
**/
static void
_xpAxisPadRange( XPAxisWdg *pAxis, double factor) {
    double wDelta, wLo, wHi, wRatio, f;

    _xpAxisMakeRangeNonTrivial( pAxis);
    wLo = pAxis->wLo;
    wHi = pAxis->wHi;

    switch ( pAxis->scaleMode ) {
    case XPXfaMode_Lin:
	/*
	 * d' = (h - l) * f/2 = w*f/2
	 * l' = l - d = l - w*f/2
	 * h' = h + d = h + w*f/2
	 * w' = h' - l' = h + w*f/2 - (l - w*f/2) = h-l + w*f = w*(1+f)
	 * ===> factor = 1+f <===> f = factor - 1
	 */
	f = factor-1;
	wDelta = (wHi - wLo) * f/2;
	wLo -= wDelta;
	wHi += wDelta;
	break;
    case XPXfaMode_Log:
    case XPXfaMode_DSLog:
	wRatio = (wLo == 0.0 || wHi == 0.0) ? 1.14 
	  : 1 + log(fabs(wHi/wLo)) * .15;
	if ( wLo < 0 )  wLo *= wRatio;  else    wLo /= wRatio;
	if ( wHi > 0 )  wHi *= wRatio;  else    wHi /= wRatio;
	break;
    default:
	topProgMsg("bad switch");
	return;
    }

    if ( wLo > wHi )		TOP_SWAPTYPE(double, wLo, wHi);
    pAxis->wLo = wLo;
    pAxis->wHi = wHi;
}



/**
    Update resources.  We dont know what will change,
    so we must recompute everything.  Some things we do immediately,
    and other things we defer using the IdleCalc.  For resources which
    we dont expect to change again soon, we handle now.
**/
static int
_xpAxisConfig( Tcl_Interp *ip, XPAxisWdg *pAxis) {
    XGCValues		gcValues;
    unsigned long	gcMask;

    Tk_SetBackgroundFromBorder(pAxis->tkwin, pAxis->bgBorder);

    /*
     * Track our overall configuration
     */
    /*IF*/ if ( pAxis->sideUid == _XpAxisLeftUid ) {
	pAxis->side = XPAxisSide_Left;
	pAxis->vertB = TOP_TRUE;
    } else if ( pAxis->sideUid == _XpAxisRightUid ) {
	pAxis->side = XPAxisSide_Right;
	pAxis->vertB = TOP_TRUE;
    } else if ( pAxis->sideUid == _XpAxisTopUid ) {
	pAxis->side = XPAxisSide_Top;
	pAxis->vertB = TOP_FALSE;
    } else if ( pAxis->sideUid == _XpAxisBottomUid ) {
	pAxis->side = XPAxisSide_Bottom;
	pAxis->vertB = TOP_FALSE;
    } else {
	Tcl_AppendResult(ip,pAxis->Tag,": invalid side ``",
	  pAxis->sideUid,"''",NULL);
	pAxis->side = XPAxisSide_Null;
	return TCL_ERROR;
    }
    /*IF*/ if ( pAxis->tickmodeUid == _XpAxisInsideUid ) {
	pAxis->tickInsideB = TOP_TRUE; pAxis->tickOutsideB = TOP_FALSE;
    } else if ( pAxis->tickmodeUid == _XpAxisOutsideUid ) {
	pAxis->tickInsideB = TOP_FALSE; pAxis->tickOutsideB = TOP_TRUE;
    } else if ( pAxis->tickmodeUid == _XpAxisNoneUid ) {
	pAxis->tickInsideB = TOP_FALSE; pAxis->tickOutsideB = TOP_FALSE;
    } else {
	Tcl_AppendResult(ip,pAxis->Tag,": invalid tickmode ``",
	  pAxis->tickmodeUid,"''",NULL);
	return TCL_ERROR;
    }
    if ( pAxis->scaleUid == NULL 
      ||(pAxis->scaleMode = _xpXfaScaleStrToMode( pAxis->scaleUid)) 
      == XPXfaMode_Null ) {
	pAxis->scaleMode = XPXfaMode_Null;
	Tcl_AppendResult(ip,pAxis->Tag,": invalid scale ``",
	  pAxis->scaleUid,"''",NULL);
	return TCL_ERROR;
    }
    if ( pAxis->scrollWdg ) {
	if ( pAxis->scrollB ) {
	    Tk_ManageGeometry(pAxis->scrollWdg, _xpAxisScrollbarGeomCB, 
	      (ClientData) pAxis);
	    /* map occurs in ConfigGeom() above */
	} else {
	    Tk_ManageGeometry(pAxis->scrollWdg, NULL, NULL);
	    Tk_UnmapWindow(pAxis->scrollWdg);
	}
    }

    /*
     * the transform
     */
    if ( pAxis->padFactor != 0.0 ) {
	_xpAxisPadRange( pAxis, pAxis->padFactor);
	pAxis->padFactor = 0.0;
    }
    _xpAxisMakeRangeNonTrivial( pAxis);

    /*
     * Handle visuals (label)
     */
    if ( pAxis->bgColor == NULL 
      || pAxis->labelFont == NULL || pAxis->tickColor == NULL ) {
	Tcl_AppendResult(ip,pAxis->Tag,": Invalid label color and/or font.",
	  NULL);
	return TCL_ERROR;
    }
    pAxis->labelLen = pAxis->showLabelB && pAxis->labelStr!=NULL 
      ? strlen(pAxis->labelStr) : 0;
    if ( pAxis->labelLen > 0 ) {
	if (pAxis->labelGC != None)
	    Tk_FreeGC(pAxis->dpy, pAxis->labelGC);
	gcValues.foreground = pAxis->tickColor->pixel;
	gcValues.background = pAxis->bgColor->pixel;
	gcValues.font = pAxis->labelFont->fid;
	gcMask = GCForeground|GCBackground|GCFont;
	pAxis->labelGC = Tk_GetGC(pAxis->tkwin, gcMask, &gcValues);
    }
    if ( pAxis->bgColor == NULL || pAxis->clipColor == NULL ) {
	Tcl_AppendResult(ip,pAxis->Tag,": Invalid clip color.",NULL);
	return TCL_ERROR;
    }
    if (pAxis->clipGC != None)
	Tk_FreeGC(pAxis->dpy, pAxis->clipGC);
    gcValues.foreground = pAxis->clipColor->pixel;
    gcValues.background = pAxis->bgColor->pixel;
    gcMask = GCForeground|GCBackground;
    pAxis->clipGC = Tk_GetGC(pAxis->tkwin, gcMask, &gcValues);

    /*
     * Handle tick styling (font & color)
     */
    if ( pAxis->bgColor == NULL 
      || pAxis->tickFont == NULL || pAxis->tickColor == NULL ) {
	Tcl_AppendResult(ip,pAxis->Tag,": Invalid tick color and/or font.",
	  NULL);
	return TCL_ERROR;
    }
    if (pAxis->tickGC != None)
	Tk_FreeGC(pAxis->dpy, pAxis->tickGC);
    gcValues.foreground = pAxis->tickColor->pixel;
    gcValues.background = pAxis->bgColor->pixel;
    gcValues.font = pAxis->tickFont->fid;
    gcMask = GCForeground|GCBackground|GCFont;
    pAxis->tickGC = Tk_GetGC(pAxis->tkwin, gcMask, &gcValues);

    pAxis->tickCI = pAxis->tickFont->max_bounds;
    pAxis->tickCI_height = pAxis->tickCI.ascent + pAxis->tickCI.descent;
    pAxis->vTicLabWidth = pAxis->tickCI.width * pAxis->tickLabDigits;
    topVecClear( &pAxis->Tics);

    TOP_PDBG((SPKG,"%s: side %s font %dx%d fmt %s maj %d min %d", pAxis->Tag, 
      pAxis->sideUid,
      pAxis->tickCI.width, pAxis->tickCI_height, pAxis->tickFmt,
      pAxis->vMajorTicLen, pAxis->vMinorTicLen));

    /*
     * Keep track of our associated dataWdg
     */
    if ( pAxis->dataWdg != pAxis->dataWdgNew ) {
	if ( pAxis->dataWdg ) {
	    Tk_DeleteEventHandler( pAxis->dataWdg, StructureNotifyMask,
	      _xpAxisDataEventCB, (ClientData) pAxis);
	}
	pAxis->dataWdg = pAxis->dataWdgNew;
        if ( pAxis->dataWdg ) {
    	    Tk_CreateEventHandler( pAxis->dataWdg, StructureNotifyMask,
	      _xpAxisDataEventCB, (ClientData) pAxis);
	}
    }
    /*
     * Our goal here to minimize the number of redisplays by the
     * graph window.  So its important that our xfa be as up-todate
     * as possible.  As a result, we may recalculate it several times,
     * and may even recalc the labels several times, but thats
     * better than recalc'ing the graph multiple times.
     *
     */
    pAxis->Flags |= XPAxisF_CalcVars;
    if ( _xpAxisConfigXf( ip, pAxis) != TCL_OK ) {
	return TCL_ERROR;
    }
    /*
     * ConfigXf() registers an appropriate IdleCalc(), so dont
     * need to do it here.
     */

    return TCL_OK;
}


static double*
_xpAxisGetValPtr( Tcl_Interp *ip, XPAxisWdg *pAxis, char *name) {
    switch ( name[0] ) {
    case 'l':	return &pAxis->wLo;
    case 'm':	return &pAxis->wMid;
    case 'h':	return &pAxis->wHi;
    case 'c':	return &pAxis->wClip;
    }
    Tcl_AppendResult(ip,"Expected lo,mid,hi,clip, got ``",name,"''",NULL);
    return NULL;
}

/**
    Performs a transformation mapping using the axis's current xform.
    The source domain {src} and destiniation domain {dst} may be:
      window:	the window space, e.g. floating point values
      fmtwindow:same as window, but formatted using topFmt().
      axisview:	the coordinate space (integer) of the axis widget
      drawview: the coordinate space (integer) of the draw widget
    Any combination (including identity) mapping is allowed.
    window values will be parsed using topFmt().  Result is returned
    in the interp result.  Only the first character of {src} and {dst}
    are used.
**/
static int
_xpAxisXformFmtVal( char *res, XPAxisWdg *pAxis, char *dst, double wVal) {
    char	*s, *fmtStr, fmtStrBuf[100];
    int		numExtra;

    if ( (s=strchr(dst,'+')) == NULL ) {
	fmtStr = pAxis->realTickFmt;
    } else {
	numExtra = atoi(s+1);
	topFmtDblCalcRange( pAxis->tickFmt, wVal, wVal,
	  pAxis->NumTicLabSameDigits+numExtra, fmtStrBuf);
	fmtStr = fmtStrBuf;
    }
    if ( ! topFmtDbl( res, fmtStr, wVal) )
	return TCL_ERROR;
    return TCL_OK;
}

static int
_xpAxisXformVal( Tcl_Interp *ip, XPAxisWdg *pAxis, 
  char *src, char *dst, char *val) {
    double		wVal;
    int			vVal, vDelta = 0;

    Tcl_ResetResult(ip);
    /*IF*/ if ( *src == 'w' ) {
	if ( Tcl_GetDouble( ip, val, &wVal) != TCL_OK )
	    return TCL_ERROR;
xformwin:
	/*IF*/ if ( *dst == 'w' ) {
	    sprintf( ip->result, "%g", wVal);
	} else if ( *dst == 'f' ) {
	    return _xpAxisXformFmtVal( ip->result, pAxis, dst, wVal);
	} else if ( *dst == 'd' ) {
	    vVal = xpXfaToView( &pAxis->TF, wVal);
	    sprintf( ip->result, "%d", vVal);
	} else if ( *dst == 'a' ) {
	    vVal = xpXfaToView( &pAxis->TF, wVal) + pAxis->vDataWdgOfs;
	    sprintf( ip->result, "%d", vVal);
	} else {
baddst:
	    Tcl_AppendResult( ip, "Bad dst domain ``", dst, "''", NULL);
	    return TCL_ERROR;
	}
    } else if ( *src == 'f' ) {
	if ( topPrsDblTcl( ip, val, &wVal) != TCL_OK )
	    return TCL_ERROR;
	goto xformwin;
    } else if ( *src == 'd' ) {
xformdata:
	if ( Tcl_GetInt(ip, val, &vVal) != TCL_OK )
	    return TCL_ERROR;
	vVal += vDelta;
	/*IF*/ if ( *dst == 'w' ) {
	    wVal = xpXfaToWin( &pAxis->TF, vVal);
	    sprintf( ip->result, "%g", wVal);
	} else if ( *dst == 'f' ) {
	    wVal = xpXfaToWin( &pAxis->TF, vVal);
	    return _xpAxisXformFmtVal( ip->result, pAxis, dst, wVal);
	} else if ( *dst == 'd' ) {
	    sprintf( ip->result, "%d", vVal);
	} else if ( *dst == 'a' ) {
	    vVal += pAxis->vDataWdgOfs;
	    sprintf( ip->result, "%d", vVal);
	} else {
	    goto baddst;
	}
    } else if ( *src == 'a' ) {
	vDelta = - pAxis->vDataWdgOfs;
	goto xformdata;
    } else {
        Tcl_AppendResult( ip, "Bad src domain``", src, "''", NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

static int
_xpAxisCmds( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    XPAxisWdg	*pAxis = (XPAxisWdg*) cld;
    int		result = TCL_OK;
    TOPStrLenType	len;
    char	*cmd, c;

    if (aC < 2) {
	Tcl_AppendResult(ip, "wrong # args: should be \"",
		aV[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) pAxis);

    cmd = aV[1]; c = *cmd; len=strlen(cmd);
    /*IF*/ if ( c=='c' && strncmp(cmd, "configure", len)==0 ) {
	if (aC == 2) {
	    result = Tk_ConfigureInfo(ip, pAxis->tkwin, _XpAxisConfigSpecs,
		    (char *) pAxis, (char *) NULL, 0);
	} else if (aC == 3) {
	    result = Tk_ConfigureInfo(ip, pAxis->tkwin, _XpAxisConfigSpecs,
		    (char *) pAxis, aV[2], 0);
	} else {
    	    if ( Tk_ConfigureWidget( ip, pAxis->tkwin, _XpAxisConfigSpecs,
	      aC-2, aV+2, (char *) pAxis, TK_CONFIG_ARGV_ONLY) != TCL_OK )
		goto error;
	    pAxis->zoomedFullB = TOP_FALSE;
	    if ( _xpAxisConfig( ip, pAxis) != TCL_OK )
		goto error;
 	}
    } else if ( c=='r' && strncmp(cmd,"region",len)==0 ) {
	int pos;
	char *region;

	if ( aC != 4 ) {
	    Tcl_AppendResult(ip, "usage: ", aV[0], " ", aV[1], "x y", NULL);
	    goto error;
	}
	result = pAxis->vertB 
	  ? Tcl_GetInt( ip, aV[3], &pos) : Tcl_GetInt( ip, aV[2], &pos);
	if ( result != TCL_OK )
	    goto error;
	pos -= pAxis->vDataWdgOfs;
	/*IF*/ if ( 3*pos < pAxis->vDataWdgLen ) {
	    region = pAxis->flipB ? "hi" : "lo";
	} else if ( 3*pos > 2*pAxis->vDataWdgLen ) {
	    region = pAxis->flipB ? "lo" : "hi";
	} else {
	    region = "mid";
	}
	Tcl_SetResult( ip, region, TCL_STATIC);
    } else if ( c=='p' && strncmp(cmd,"popuplayout",len)==0 ) {
	Tk_Window	wdg;
	int		wdgW, wdgH, xLo, xMid, xHi, yLo, yMid, yHi;
	char		buf[100];

	if ( aC != 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",cmd," window",NULL);
	    goto error;
	}
	if ( (wdg = Tk_NameToWindow( ip, aV[2], pAxis->tkwin)) == NULL ) {
	    Tcl_AppendResult( ip, "\n",aV[0]," ",cmd,": no such window", NULL);
	    goto error;
	}
	wdgH = Tk_ReqHeight(wdg);	if ( wdgH < 2 )	wdgH = Tk_Height(wdg);
	wdgW = Tk_ReqWidth(wdg);	if ( wdgW < 2 )	wdgW = Tk_Width(wdg);
	if ( pAxis->vertB ) {
	    xLo  = 0;	yLo  = 0;
	    xMid = 0;	yMid = (Tk_Height(pAxis->tkwin) - wdgH) / 2;
	    xHi  = 0;	yHi  =  Tk_Height(pAxis->tkwin) - wdgH;
	} else {
	    xLo  = 0;						yLo  = 0;
	    xMid = (Tk_Width(pAxis->tkwin) - wdgW) / 2;		yMid = 0;
	    xHi  =  Tk_Width(pAxis->tkwin) - wdgW;		yHi  = 0;
	}
	if ( pAxis->flipB ) {
	    TOP_SWAPTYPE(int,xLo,xHi);
	    TOP_SWAPTYPE(int,yLo,yHi);
	}
	sprintf( buf, "{%d %d} {%d %d} {%d %d}", xLo,yLo, xMid,yMid, xHi,yHi);
	Tcl_SetResult( ip, buf, TCL_VOLATILE);
    } else if ( c=='g' && strncmp(cmd,"grow",len)==0 ) {
	double	f;
	TOPLogical doFullB;
	if ( aC != 3 && aC != 4 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1],"factor ?full?",NULL);
	    goto error;
	}
	if ( (doFullB=(aC >= 4 && aV[3][0] == 'f')) ) {
	    pAxis->zoomedFullB = TOP_TRUE;
	    pAxis->wLo = pAxis->wRangeLo;
	    pAxis->wHi = pAxis->wRangeHi;
	}
	if ( Tcl_GetDouble( ip, aV[2], &f) != TCL_OK ) {
	    Tcl_AppendResult(ip, "\n",aV[0]," ",aV[1],": bad factor",NULL);
	    goto error;
	}
	_xpAxisPadRange( pAxis, f);
	if ( doFullB )
	    _xpAxisResetScroll( pAxis);
        if ( _xpAxisConfigXf( ip, pAxis) != TCL_OK )
	    goto error;
    } else if ( c=='s' && strncmp(cmd,"scroll",len)==0 ) {
	int vPos, vLen;
	if ( aC != 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," firstUnit",NULL);
	    goto error;
	}
	if ( Tcl_GetInt( ip, aV[2], &vPos) != TCL_OK ) {
	    Tcl_AppendResult(ip, "\n",aV[0]," ",aV[1],": bad unit",NULL);
	    goto error;
	}
	vLen = xpXfaRangeToView(&pAxis->STF,pAxis->wLo,pAxis->wHi);
	if ( ! pAxis->flipB ) {
	    pAxis->wLo = xpXfaToWin(&pAxis->STF,vPos);
	    pAxis->wHi = xpXfaToWin(&pAxis->STF,vPos+vLen);
	} else {
	    vPos = XPAXIS_SCROLLRES - vPos;
	    pAxis->wLo = xpXfaToWin(&pAxis->STF,vPos-vLen);
	    pAxis->wHi = xpXfaToWin(&pAxis->STF,vPos);
	}
	_xpAxisMakeRangeNonTrivial( pAxis);
        if ( _xpAxisConfigXf( ip, pAxis) != TCL_OK )
	    goto error;
    } else if ( c=='x' && strncmp(cmd,"xform",len)==0 ) {
	if ( aC != 5 ) {
	    Tcl_AppendResult(ip, "usage: ", aV[0], " ", aV[1],
	      " src dst val\n\tsrc,dst are one of win,fmtwin,axis,draw", NULL);
	    goto error;
	}
        if ( _xpAxisXformVal( ip, pAxis, aV[2], aV[3], aV[4]) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],
	      ": failed (",aV[2],",",aV[3],",",aV[4],")",NULL);
	}
    } else if ( c=='v' && strncmp(cmd,"value",len)==0 ) {
	double	*pVal;
	if ( aC != 3 && aC != 4 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",cmd,
	      " name ?val?", NULL);
	    goto error;
	}
	if ( (pVal = _xpAxisGetValPtr( ip, pAxis, aV[2])) == NULL )
	    goto error;
	if ( aC == 3 ) {
	    Tcl_ResetResult(ip);
	    sprintf( ip->result, "%g", *pVal);
	} else {
	    if ( topPrsDblTcl( ip, aV[3], pVal) != TCL_OK )
		goto error;
	    if ( pVal == &pAxis->wMid ) {
		_xpAxisSetMidRange( pAxis, pAxis->wMid);
		_xpAxisIdleCalc( pAxis, XPAxisF_CalcXf);
	    } else {
		_xpAxisIdleCalc( pAxis, XPAxisF_CalcXf|XPAxisF_CalcNonTriv);
	    }
	}
    } else {
	Tcl_AppendResult(ip, aV[0], ": bad command: ", cmd, NULL);
	goto error;
    }
    Tk_Release((ClientData) pAxis);
    return result;

    error:
    Tk_Release((ClientData) pAxis);
    return TCL_ERROR;
}



int
xpAxisCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    Tk_Window		appwin = (Tk_Window) cld;
    XPAxisWdg		*pAxis;
    Tk_Window		myWin;
    char		buf[50];

    if ( _XpAxisLeftUid == NULL ) {
	_XpAxisLeftUid   = Tk_GetUid("left");
	_XpAxisRightUid  = Tk_GetUid("right");
	_XpAxisTopUid    = Tk_GetUid("top");
	_XpAxisBottomUid = Tk_GetUid("bottom");
	_XpAxisNoneUid   = Tk_GetUid("none");
	_XpAxisInsideUid = Tk_GetUid("inside");
	_XpAxisOutsideUid = Tk_GetUid("outside");
    }
    if (aC < 2) {
	Tcl_AppendResult(ip, "wrong # args: should be \"",
	  aV[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    myWin = Tk_CreateWindowFromPath(ip, appwin, aV[1], (char *) NULL);
    if (myWin == NULL) {
	return TCL_ERROR;
    }

    pAxis = MEM_ALLOC(XPAxisWdg);
    MEM_CLEAR(pAxis);
    pAxis->tkwin = myWin;
    pAxis->dpy = Tk_Display(pAxis->tkwin);
    pAxis->Tag = Tk_PathName(pAxis->tkwin);
    pAxis->ip = ip;
    pAxis->Flags = XPAxisF_On;
    pAxis->tickGC = None;
    pAxis->tickLabDigits = 4;
    topVecConstructor( &pAxis->Tics, sizeof(XPAxisTick));

    Tk_SetClass( pAxis->tkwin, "XPAxis");
    Tk_CreateEventHandler( pAxis->tkwin, ExposureMask|StructureNotifyMask,
	    _xpAxisEventCB, (ClientData) pAxis);
#ifdef notdef
    Tk_CreateEventHandler(pAxis->tkwin, EnterWindowMask|LeaveWindowMask
	    |PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
	    ScaleMouseProc, (ClientData) pAxis);
#endif
    Tcl_CreateCommand( ip, Tk_PathName(pAxis->tkwin), _xpAxisCmds,
      (ClientData) pAxis, (void (*)()) NULL);
    if ( Tk_ConfigureWidget( ip, pAxis->tkwin, _XpAxisConfigSpecs,
      aC-2, aV+2, (char *) pAxis, 0) != TCL_OK )
	goto error;
    if (_xpAxisConfig( ip, pAxis) )
	goto error;
    _xpAxisCalcLabels( pAxis);
    _xpAxisConfigGeom( ip, pAxis);
    sprintf( buf, "0x%x", (int) pAxis);
    Tcl_SetVar2(ip, pAxis->Tag, "ptr", buf, TCL_GLOBAL_ONLY);
    _xpAxisSetTraceVarsXf( ip, pAxis);	/* force it once right now */

    ip->result = Tk_PathName(pAxis->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(pAxis->tkwin);
    return TCL_ERROR;
}
int
xpAxisGetHandle( Tcl_Interp *ip, char *name, XPAxisWdg **ppAxis) {
    XPAxisWdg	*pAxis;
    char	*myname, *addrStr;

    *ppAxis = NULL;
    if ( (addrStr = Tcl_GetVar2( ip, name, "ptr", TCL_GLOBAL_ONLY)) == NULL ) {
	goto error;
    }
    if ( Tcl_GetInt( ip, addrStr, (int*)&pAxis) != TCL_OK ) {
	goto error;
    }
    myname = Tk_PathName(pAxis->tkwin);
    if ( strcasecmp(myname, name)!=0 ) {
	goto error;
    }
    *ppAxis = pAxis;
    return TCL_OK;

error:
    Tcl_AppendResult( ip, "\nCouldn't get axis handle for ``",
      name, "''.", NULL);
    return TCL_ERROR;
}

XPXfa*
xpAxisGetXfa( XPAxisWdg *pAxis) {
    return & pAxis->TF;
}

/**
    Eventually the return code here will indicate the "valid" status
    of the current transform.  That is, if the winport and viewport
    ranges are valid and lead to a meaningful transform.
**/
TOPLogical
xpAxisGetDomain( XPAxisWdg *pAxis, double *pwLo, double *pwHi) {
    *pwLo = pAxis->wLo;
    *pwHi = pAxis->wHi;
    return TOP_TRUE;
}

void
xpAxisZoomFull( XPAxisWdg *pAxis) {
    pAxis->wLo = pAxis->wRangeLo;
    pAxis->wHi = pAxis->wRangeHi;
    if ( pAxis->autoSizePad > 1.0 )
	_xpAxisPadRange( pAxis, pAxis->autoSizePad);
    _xpAxisIdleCalc( pAxis, XPAxisF_CalcXf);
}

/**
    This function lets applications notify us of the "true" range
    of the data being displayed.  In autoScale mode, we will auto-magically
    adjust our wLo,wHi based on this information.
**/
void
xpAxisInformRange( XPAxisWdg *pAxis, double wLo, double wHi, TOPMask zoomFlags){
    double	wLen, wRangeLen;
    TOPLogical	sB;

    if ( wLo > wHi )
        TOP_SWAPTYPE(double, wLo, wHi);
    sB = eemDfltSameB(wLo,pAxis->wRangeLo) && eemDfltSameB(wHi,pAxis->wRangeHi);
    pAxis->wRangeLo = wLo;
    pAxis->wRangeHi = wHi;
    if ( sB )			return;
    if ( ! pAxis->autoSizeB )	return;
    if ( ! pAxis->zoomedFullB )	return;

    wLen = pAxis->wHi - pAxis->wLo;
    wRangeLen = pAxis->wRangeHi - pAxis->wRangeLo;
    if ( (TOP_IsOn(zoomFlags,XPAxisZM_Out) &&
       (pAxis->wRangeLo < pAxis->wLo || pAxis->wRangeHi > pAxis->wHi) )
      || (TOP_IsOn(zoomFlags,XPAxisZM_In) &&
       (wRangeLen>0 && wRangeLen > pAxis->wClip && 4*wRangeLen < wLen) ) ) {
	xpAxisZoomFull( pAxis);
    }
}

TOPLogical
xpAxisXformPendingB( XPAxisWdg *pAxis) {
    if ( TOP_IsOff( pAxis->Flags, XPAxisF_CalcXf) 
      && TOP_IsOff( pAxis->Flags, XPAxisF_CalcXfNotify) )
	return TOP_FALSE;
    pAxis->Flags |= XPAxisF_CalcXfNotify;
    return TOP_TRUE;
}

int
xpAxisRegisterCmds( Tcl_Interp *ip, Tk_Window appwin) {
    topFmtRegisterCmds( ip);
    eemDfltRegisterCmds( ip);
    topTkMiscRegisterCmds( ip, appwin);
    Tcl_CreateCommand( ip, "xpaxis", xpAxisCmd, (ClientData)appwin, NULL);
    return TCL_OK;
}
