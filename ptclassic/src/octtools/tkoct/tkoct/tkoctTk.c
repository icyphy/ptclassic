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

 tkoctTk.c :: high-level Tk interface to ged (graphics editor) widget
    Tk widget that displays OCT facets.  This replaces a small portion
    of what the octtools program VEM does.  This module of ged implements
    the high level interface to Tk: configuration and drawing.

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <tk.h>

#include "oct.h"
#include "oh.h"
#include "tr.h"
#include "errtrap.h"

#include "topStd.h"
#include "topMem.h"
#include "topVec.h"
#include "topTcl.h"

#include "toct.h"
#include "tkh.h"

#include "xpDefaults.h"
#include "tkoctInt.h"
#include "tkoct.h"

/*global*/ char _XpGedSubPackage[] = "xp.ged";
#define SPKG _XpGedSubPackage

static Tk_ConfigSpec _XpGedConfigSpecs[] = {

    /*
     * XPGed specs
     */
    {TK_CONFIG_UID, "-facet", "facet", "Facet",
	NULL, Tk_Offset(XPGed, facetUid), TK_CONFIG_NULL_OK},
    {TK_CONFIG_DOUBLE, "-scale", "scale", "Scale",
	"1.0", Tk_Offset(XPGed, mapScale), 0},

    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	NULL, Tk_Offset(XPGed, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	XP_DEF_BG_COLOR, Tk_Offset(XPGed, bgBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, NULL, NULL, NULL,
	NULL, Tk_Offset(XPGed, bgColor), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	XP_DEF_BG_MONO, Tk_Offset(XPGed, bgBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, NULL, NULL, NULL,
	NULL, Tk_Offset(XPGed, bgColor), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},

    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	XP_DEF_FG_COLOR, Tk_Offset(XPGed, foreground),
	  TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	XP_DEF_FG_MONO, Tk_Offset(XPGed, foreground),
	  TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},

    {TK_CONFIG_FONT, "-font", "font", "Font",
	XP_DEF_FONT, Tk_Offset(XPGed, font), 0},

    {TK_CONFIG_UID, "-labelfonts", "labelFonts", "Fonts",
	XP_DEF_FONT, Tk_Offset(XPGed, labelFonts), 0},

    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

static void _xpGedDestroy( XPGed *pGed);

static void
_xpGedCalcMap( XPGed *pGed) {
    pGed->mapX.pixOfs = 0;
    pGed->mapX.oct2pix = pGed->mapScale;
    pGed->mapX.pix2oct = 1.0/pGed->mapX.oct2pix;

    pGed->mapY.pixOfs = Tk_Height(pGed->tkwin);
    pGed->mapY.oct2pix = - pGed->mapScale;
    pGed->mapY.pix2oct = 1.0/pGed->mapY.oct2pix;
}

static void
_xpGedCalcNow( XPGed *pGed) {
#ifdef notdef
    if ( TOP_IsOn( pGed->flags, XPGedF_CalcVars) ) {
	pGed->flags &= ~XPGedF_CalcVars;
        _xpGedDataCalcVars( &pGed->data);
    }
#endif
    if ( TOP_IsOn( pGed->flags, XPGedF_CalcMap) ) {
	pGed->flags &= ~XPGedF_CalcMap;
        _xpGedCalcMap( pGed);
	pGed->flags |= XPGedF_CalcExpose;
    }
    if ( TOP_IsOn( pGed->flags, XPGedF_CalcExpose) ) {
	pGed->flags &= ~XPGedF_CalcExpose;
        _xpGedExpose( pGed);
    }
}

static void
_xpGedIdleCalcCB( ClientData cld) {
    XPGed *pGed = (XPGed *) cld;
    pGed->flags &= ~XPGedF_IdleCalc;
    TOP_PDBG((SPKG,"%s: idle calc %x%s", pGed->winPath, pGed->flags,
      TOP_IsOff(pGed->flags,XPGedF_Valid)?" (not valid)":""));
    if ( TOP_IsOff(pGed->flags,XPGedF_Valid) )
	return;
    _xpGedCalcNow( pGed);
}

static void
_xpGedIdleCalc( XPGed *pGed, int calcMask) {
    pGed->flags |= (calcMask & XPGedF_Calc);
    if ( TOP_IsOn( pGed->flags, XPGedF_IdleCalc)
      || pGed->tkwin == NULL 
      || !Tk_IsMapped(pGed->tkwin) ) {
	return;
    }
    pGed->flags |= XPGedF_IdleCalc;
    Tk_DoWhenIdle( _xpGedIdleCalcCB, (ClientData) pGed);
}

static void
_xpGedIdleCalcUndo( XPGed *pGed) {
    if ( TOP_IsOn(pGed->flags, XPGedF_IdleCalc) ) {
        Tk_CancelIdleCall( _xpGedIdleCalcCB, (ClientData) pGed);
	pGed->flags &= ~XPGedF_IdleCalc;
    }
}

/************************************************************************
 *
 *	XPGed Tk muck (the widget itself)
 *
 ************************************************************************/

static void
_xpGedDestroyCB( ClientData cld) {
    static char	func[] = "_xpGedDestroyCB";
    XPGed *pGed = (XPGed *) cld;
    Tcl_Interp	*ip = pGed->ip;

    if ( pGed->winPath == NULL ) {
	topProgMsg("%s: Duplicate destroy", func);
	return;
    }
    Tcl_DeleteCommand( ip, pGed->winPath);

    if (pGed->bgColor != NULL) {
	Tk_FreeColor(pGed->bgColor);
	pGed->bgColor = NULL;
    }
    if (pGed->bgBorder != NULL) {
	Tk_Free3DBorder(pGed->bgBorder);
	pGed->bgBorder = NULL;
    }
    if (pGed->winPath != NULL) {
	memFree(pGed->winPath);
	pGed->winPath = NULL;
    }
    if (pGed->cursor != None) {
	Tk_FreeCursor(pGed->dpy, pGed->cursor);
	pGed->cursor = None;
    }
    MEM_FREE(pGed);
}

static void
_xpGedDestroy( XPGed *pGed) {
    Tk_Window	tkw = pGed->tkwin;

    if ( tkw != NULL ) {
	pGed->tkwin = NULL;
	/*
	 * The Preserve/Release assures that the DestroyWindow will
	 * fully complete before we go wack stuff (preventing recursion)
	 */
	Tk_Preserve( (ClientData) pGed);
	Tk_DestroyWindow(tkw);
	Tk_Release( (ClientData) pGed);
	return;
    }
    _xpGedIdleCalcUndo( pGed);
    Tk_EventuallyFree( (ClientData) pGed, _xpGedDestroyCB);
}

static void
_xpGedEventCB( ClientData cld, XEvent *pEvent) {
    XPGed *pGed = (XPGed *) cld;

    /*IF*/ if ((pEvent->type == Expose) && (pEvent->xexpose.count == 0)) {
	_xpGedIdleCalc( pGed, XPGedF_CalcExpose);
    } else if ( pEvent->type == DestroyNotify ) {
	pGed->tkwin = NULL;
	_xpGedDestroy( pGed);
    }
}

static int
_xpGedConfig( Tcl_Interp *ip, XPGed *pGed, int aC, char **aV, int flags) {
    XGCValues		gcValues;
    unsigned long	gcMask;

    pGed->flags &= ~XPGedF_Valid;
    if (Tk_ConfigureWidget( ip, pGed->tkwin, _XpGedConfigSpecs,
	    aC, aV, (char *) pGed, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * config the background
     */
    Tk_SetBackgroundFromBorder(pGed->tkwin, pGed->bgBorder);

    /* 
     * config the normGC
     */
    if ( pGed->normGC != None )
	Tk_FreeGC( pGed->dpy, pGed->normGC);
    gcMask = GCFunction | GCForeground | GCBackground;
    gcValues.function = GXcopy;
    gcValues.foreground = pGed->foreground->pixel;
    gcValues.background = pGed->bgColor->pixel;
    gcMask |= GCFont;
    gcValues.font = pGed->font->fid;
    pGed->normGC = Tk_GetGC( pGed->tkwin, gcMask, &gcValues);

    /*
     * config the label fonts
     */
    if ( pGed->labelFonts == NULL )
	return TCL_OK;	/* early out */
    if ( pGed->labelFonts != pGed->oldLabelFonts ) {
	pGed->oldLabelFonts = NULL;
	if ( pGed->labelFontList != NULL ) {
	    tkhFreeFontList( pGed->labelFontList);
	    pGed->labelFontList = NULL;
	}
	if ( (pGed->labelFontList = tkhGetFontList( ip, pGed->labelFonts))
	  == NULL ) {
	    return TCL_ERROR;
	}
	pGed->oldLabelFonts = pGed->labelFonts;
    }


    /*
     * config the facet
     */
    if ( pGed->facetUid == NULL ) {
	pGed->oldFacetUid = NULL;
	return TCL_OK;	/* early out */
    }
    if ( pGed->facetUid != pGed->oldFacetUid ) {
	octObject	facet;
	if ( toctGetFacetByName( pGed->facetUid, &facet) != TCL_OK ) {
	    Tcl_AppendResult(ip,"invalid facet ``",pGed->facetUid,"''",NULL);
	    pGed->facetUid = pGed->oldFacetUid = NULL;
	    return TCL_ERROR;
	}
	pGed->facetId = facet.objectId;
	pGed->oldFacetUid = pGed->facetUid;
	TOP_PDBG((SPKG,"%s: facet id %x", pGed->winPath, pGed->facetId));
    }

    pGed->flags |= XPGedF_Valid;
    _xpGedIdleCalc( pGed, XPGedF_CalcExpose|XPGedF_CalcMap);

    return TCL_OK;
}

#ifdef UNUSED
static int
_xpGedGetTriant(Tk_Window tkwin, int x, int y) {
    int triant;
    int	w_third = Tk_Width(tkwin)/3, h_third = Tk_Height(tkwin)/3;
    if ( x < w_third ) 			triant = 0;
    else if ( x < 2*w_third ) 		triant = 1;
    else 				triant = 2;
    if ( y < h_third ) 			triant += 0;
    else if ( y < 2*h_third ) 		triant += 3;
    else 				triant += 6;
    return triant;
}
#endif /* UNUSED */

#define XPGed_ScaleAdj(val,by) \
    ( (by)>=0 ? (val) * (1.0+(by)) : (val) / (1.0-(by)) )


static void
_xpGedScanScale( XPGed *pGed, double newScale) {
    pGed->mapScale = newScale;
    pGed->mapX.octOfs = pGed->scanOctOfsX
      + pGed->scanX 
      * (1/pGed->scanOct2PixX - 1/pGed->mapScale);
    pGed->mapY.octOfs = pGed->scanOctOfsY
      - (Tk_Height(pGed->tkwin)-pGed->scanY) 
      * (1/pGed->scanOct2PixY + 1/pGed->mapScale);
    _xpGedIdleCalc( pGed, XPGedF_CalcMap|XPGedF_CalcExpose);
}

/**
    The per GED widget Tcl command handler.
**/
static int
_xpGedCmds( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    XPGed		*pGed = (XPGed*) cld;
    char		*cmd = aV[1], c;
    int			r = TCL_OK;
    TOPStrLenType	len;

    if ( aC < 2 ) {
	Tcl_AppendResult(ip,"usage: ",aV[0]," configure|center|map|imap|scan ?args?",
	  NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) pGed);
    c = *cmd; len = strlen(cmd);
    /*IF*/ if ( c=='c' && strncmp(cmd,"configure",len)==0 ) {
	if (aC <= 2) {
	    r = Tk_ConfigureInfo(ip, pGed->tkwin, _XpGedConfigSpecs,
		    (char *) pGed, (char *) NULL, 0);
	} else if (aC == 3) {
	    r = Tk_ConfigureInfo(ip, pGed->tkwin, _XpGedConfigSpecs,
		    (char *) pGed, aV[2], 0);
	} else {
	    r = _xpGedConfig(ip, pGed, aC-2, aV+2, TK_CONFIG_ARGV_ONLY);
	    if ( r != TCL_OK )
		Tcl_AppendResult(ip,aV[0]," ",cmd,": bad configuration",NULL);
	}
    } else if ( c=='c' && strncmp(cmd,"center",len)==0 ) {
	struct octPoint	cpt, cpt_new;
	if ( TOP_OnB(pGed->flags,XPGedF_CalcMap) )
            _xpGedCalcMap( pGed);
	cpt.x = Tk_Width(pGed->tkwin)/2;
	cpt.y = Tk_Height(pGed->tkwin)/2;
	_xpGedMapPixPt( pGed, &cpt);
	if ( aC == 2 ) {
	    topTclRetFmt( ip, "%d %d", cpt.x, cpt.y);
	} else if ( aC == 4 ) {
	    cpt_new = cpt;
	    if ( aV[2][0]!='\0' ) Tcl_GetInt(ip, aV[2], (int*)&cpt_new.x);
	    if ( aV[3][0]!='\0' ) Tcl_GetInt(ip, aV[3], (int*)&cpt_new.y);
	    pGed->mapX.octOfs += cpt_new.x - cpt.x;
	    pGed->mapY.octOfs += cpt_new.y - cpt.y;
    	    _xpGedIdleCalc( pGed, XPGedF_CalcMap|XPGedF_CalcExpose);
	} else {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",cmd," ?x y?",NULL);
	    goto error;
	}
    } else if ( c=='m' && strncmp(cmd,"map",len)==0 ) {
	octPoint	pt;
	TOPLogical	scaleB = TOP_FALSE;
	int		opt = 0;
	if ( aC>=3 && strcmp(aV[2],"-scale")==0 ) { scaleB = TOP_TRUE; ++opt; }
	if ( aC-opt!=4 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",cmd," ?-scale? oX oY",NULL);
	    goto error;
	}
	if ( topTclGetInt2( ip, aV[2+opt], aV[3+opt], (int*)&pt.x, (int*)&pt.y) != TCL_OK ){
	    Tcl_AppendResult(ip,"\n",aV[0]," ",cmd,": bad location.",NULL);
	    goto error;
	}
	if ( scaleB )	_xpGedScaleOctPt( pGed, &pt);
	else		_xpGedMapOctPt( pGed, &pt);
	topTclRetFmt( ip, "%d %d", pt.x, pt.y);
    } else if ( c=='i' && strncmp(cmd,"imap",len)==0 ) {
	octPoint	pt;
	TOPLogical	scaleB = TOP_FALSE;
	int		opt = 0;
	if ( aC>=3 && strcmp(aV[2],"-scale")==0 ) { scaleB = TOP_TRUE; ++opt; }
	if ( aC-opt != 4 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",cmd," ?-scale? vX vY",NULL);
	    goto error;
	}
	if ( topTclGetInt2( ip, aV[2+opt], aV[3+opt], (int*)&pt.x, (int*)&pt.y) != TCL_OK ){
	    Tcl_AppendResult(ip,"\n",aV[0]," ",cmd,": bad location.",NULL);
	    goto error;
	}
	if ( scaleB ) 	_xpGedScalePixPt( pGed, &pt);
	else		_xpGedMapPixPt( pGed, &pt);
	topTclRetFmt( ip, "%d %d", pt.x, pt.y);
    } else if ( c=='s' && strncmp(cmd,"scan",len)==0 ) {
	int	x, y;
	if ( aC != 5 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",cmd,
	      " mark|dragto|stretchto x y", NULL);
	    goto error;
	}
	if ( (Tcl_GetInt(ip, aV[3], &x) != TCL_OK)
	  || (Tcl_GetInt(ip, aV[4], &y) != TCL_OK) )
	    goto error;
	/*IF*/ if ( aV[2][0]=='m' ) {	/* mark */
	    pGed->scanOct2PixX = pGed->mapX.oct2pix;
	    pGed->scanOct2PixY = pGed->mapY.oct2pix;
	    pGed->scanOctOfsX = pGed->mapX.octOfs;
	    pGed->scanOctOfsY = pGed->mapY.octOfs;
	    pGed->scanX = x;
	    pGed->scanY = y;
	    TOP_PDBG((SPKG,"%s: scan mark",pGed->winPath));
	} else if ( aV[2][0]=='d' ) {	/* dragto */
	    pGed->mapX.octOfs = pGed->scanOctOfsX 
	      - (x-pGed->scanX)*5*pGed->mapX.pix2oct;
	    pGed->mapY.octOfs = pGed->scanOctOfsY 
	      - (y-pGed->scanY)*5*pGed->mapY.pix2oct;
    	    _xpGedIdleCalc( pGed, XPGedF_CalcExpose);
	} else if ( aV[2][0]=='c' ) {	/* contract */
	    TOP_PDBG((SPKG,"%s: contract",pGed->winPath));
	    _xpGedScanScale( pGed, pGed->scanOct2PixX * ((double)x)/y);
	} else if ( aV[2][0]=='s' ) {	/* stretchto */
	    float delta=((x-pGed->scanX) - (y-pGed->scanY))/10.0;
	    TOP_PDBG((SPKG,"%s: stretch",pGed->winPath));
	    _xpGedScanScale( pGed, XPGed_ScaleAdj(pGed->scanOct2PixX,delta));
	} else {
	    Tcl_AppendResult(ip,aV[0]," ",cmd,": bad option ``",aV[2],"''",
	      NULL);
	    goto error;
	}
    } else {
	Tcl_AppendResult(ip, aV[0],": bad command ``",cmd,"''",NULL);
	goto error;
    }
    Tk_Release((ClientData) pGed);
    return TCL_OK;

error:
    Tk_Release((ClientData) pGed);
    return TCL_ERROR;
}

/**
    The global command used to create a GED widget.
**/
static int
_xpGedCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    Tk_Window		mainWin;
    XPGed		*pGed;
    Tk_Window		myWin;

    if (aC < 2) {
	Tcl_AppendResult(ip,"usage: ",aV[0]," pathName ?options?",NULL);
	return TCL_ERROR;
    }
    if ( (mainWin=Tk_MainWindow(ip)) == NULL ) {
	Tcl_AppendResult(ip,"\n",aV[0],": error",NULL);
	return TCL_ERROR;
    }

    if ( (myWin = Tk_CreateWindowFromPath(ip, mainWin, aV[1], (char *) NULL)) 
      == NULL ) {
	return TCL_ERROR;
    }

    pGed = MEM_ALLOC(XPGed);
    MEM_CLEAR(pGed);
    pGed->tkwin = myWin;
    pGed->dpy = Tk_Display(pGed->tkwin);
    pGed->winPath = memStrSave(Tk_PathName(pGed->tkwin));
    pGed->ip = ip;
    pGed->flags = XPGedF_On;
    pGed->normGC = None;
    Tcl_InitHashTable( &pGed->layerIdHash, TCL_ONE_WORD_KEYS);

    Tk_SetClass( pGed->tkwin, "XPGedWidget");
    Tk_CreateEventHandler( pGed->tkwin, ExposureMask|StructureNotifyMask,
      _xpGedEventCB, (ClientData) pGed);
    if (_xpGedConfig( ip, pGed, aC-2, aV+2, 0) != TCL_OK) {
	goto error;
    }
    Tcl_CreateCommand( ip, pGed->winPath, _xpGedCmds,
      (ClientData) pGed, (void (*)()) NULL);

    ip->result = Tk_PathName(pGed->tkwin);
    return TCL_OK;

    error:
    MEM_FREE(pGed->winPath);
    Tk_DestroyWindow(pGed->tkwin);
    return TCL_ERROR;
}

/**
    This is globally exported and is call from AppInit().
**/
int
xpGedInit( Tcl_Interp *ip) {
    Tcl_CreateCommand( ip, "gedwidget", _xpGedCmd, 
      (ClientData)NULL, NULL);
    return TCL_OK;
}
