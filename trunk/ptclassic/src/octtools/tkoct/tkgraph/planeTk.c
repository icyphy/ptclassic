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

    planeTk.c

    Tk widget for input/output of silararities on the Z or S plane.

    90/Sep/30 KDW: Created as zplane.c
    91/Jan/09 KDW: Extracted graphics stuff into plane.c
    92/Jun/22 KDW: Rewritten into tk widget

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
#include "eemSilar.h"

#include "xpDefaults.h"
#include "xpRubber.h"
#include "xpXfa.h"
#include "xpAxis.h"
#include "xpPlane.h"

static char _XpPlaneSubPackage[] = "xp.plane";
#define SPKG _XpPlaneSubPackage

/************************************************************************
 *
 *	data types and such
 *
 ************************************************************************/

typedef struct _XPPlaneAxis XPPlaneAxis;
typedef struct _XPPlaneData XPPlaneData;
typedef struct _XPPlaneWdg XPPlaneWdg;

struct _XPPlaneAxis {
    XPPlaneWdg*		PntWdg;
    char		*path;		/* resource -xaxis or -yaxis */
    XPAxisHandle	axisHandle;	/* handle for path */
    XPXfa		*xfa;		/* xfa for axis */
};

/*
 * In our PlaneSilar, Tag1 is used for mark the "indexed" silars, that is,
 * the silars which the current command will operate on.
 * Tag2 is used to mark silars which the client wants notify cbs about.
 */
typedef struct _XPPlaneSilar XPPlaneSilar;
struct _XPPlaneSilar {
    TOPMask		flags;
    int			power;
    XPoint		vLoc;
    XPoint		vLocConj;
};
/*
 * The command interface allows the set of silars to be acted on
 * in a variety of ways...This is handled by the function SilarGetIndex().
 * It returns an int that is either a number >=0 that is a silar index,
 * or it is one of the cases below:
 */

#define XPPS_None	(-1)		/* No silar matches */
#define XPPS_Tag1	(-2)		/* more than one; uses EEMSilarF_Tag1 */

/*
 * The function SilarGetIndex() allows the caller to specify the allowable
 * ways the set of silar(s) may be specified:
 */
typedef enum _XPPlaneSelectType XPPlaneSelectType;
enum _XPPlaneSelectType {
    XPPST_None,	
    XPPST_ExactlyOne,	/* exactly one silar may be selected */
    XPPST_TagAny,	/* Tag1 flaging should be used, error if empty */
    XPPST_TagAtleast,	/* Tag1 flaging used, ok to be empty */
};


struct _XPPlaneData {
    XPPlaneWdg*		PntWdg;
    char*		dataSrc;	/* resource -datasrc */
    XColor*		normColor;	/* resource -normcolor */
    XColor*		selColor;	/* resource -selcolor */
    int			lineWidth;	/* resource -linewidth */
    XFontStruct*	powerFont;	/* resource -powerfont */
    GC			normGC;		/* draw unselected silars this way */
    GC			selGC;		/* draw selected silars this way */
    int			vSilarRadius;	/* resource -radius */
    EEMComplex		rngLo;
    EEMComplex		rngHi;

    XPPlaneDataExt	ext;
    XPPlaneDataHandler	*dh;
    TOPVector		vSilars;	/* of XPPlaneSilar */
    TOPVector		vLociMarks;	/* of XPPlaneSilar */
    char*		notifyCmd;	/* resource -notifycommand */
};


/******************** VSILS **********************************************/

#define XPPLANE_VSILS_Num(pData)	TOPVEC_Num(&(pData)->vSilars)
#define XPPLANE_VSILS_Get(pData,i) \
    TOPVEC_Get(&(pData)->vSilars,i,XPPlaneSilar)

#define XPPLANE_VSILS_LOOP_BEGIN(pData,idx,pPS) \
    TOPVEC_IDX_LOOP_FORW_BEGIN(&(pData)->vSilars,idx,XPPlaneSilar,pPS)

#define XPPLANE_VSILS_LOOP_END() \
    TOPVEC_IDX_LOOP_FORW_END()

/******************** WSILS **********************************************/

#define XPPLANE_WSILS_Num(pData)	EEM_SILAR_Num(&(pData)->ext.wSilars)
#define XPPLANE_WSILS_Get(pData,i)	EEM_SILAR_Get(&(pData)->ext.wSilars,i)

#define XPPLANE_WSILS_LOOP_BEGIN(pData,idx,pSilar) \
    EEM_SILAR_IDX_LOOP_FORW_BEGIN(&(pData)->ext.wSilars,idx,pSilar)

#define XPPLANE_WSILS_LOOP_END() \
    EEM_SILAR_IDX_LOOP_FORW_END()

/******************** VMARKS **********************************************/

#define XPPLANE_VMARKS_Num(pData)	TOPVEC_Num(&(pData)->vLociMarks)
#define XPPLANE_VMARKS_Get(pData,i) \
    TOPVEC_GET(&(pData)->vLociMarks,i,XPPlaneSilar)

#define XPPLANE_VMARKS_LOOP_BEGIN(pData,idx,pPS) \
    TOPVEC_IDX_LOOP_FORW_BEGIN(&(pData)->vLociMarks,idx,XPPlaneSilar,pPS)

#define XPPLANE_VMARKS_LOOP_END() \
    TOPVEC_IDX_LOOP_FORW_END()

/******************** WMARKS **********************************************/

#define XPPLANE_WMARKS_Num(pData)	EEM_SILAR_Num(&(pData)->ext.wLociMarks)
#define XPPLANE_WMARKS_Get(pData,i)	EEM_SILAR_Get(&(pData)->ext.wLociMarks,i)

#define XPPLANE_WMARKS_LOOP_BEGIN(pData,idx,pSilar) \
    EEM_SILAR_IDX_LOOP_FORW_BEGIN(&(pData)->ext.wLociMarks,idx,pSilar)

#define XPPLANE_WMARKS_LOOP_END() \
    EEM_SILAR_IDX_LOOP_FORW_END()

/******************************************************************/

#define DEF_PLANEDATA_NORMCOLOR_COLOR	"white"
#define DEF_PLANEDATA_NORMCOLOR_MONO	BLACK
#define DEF_PLANEDATA_SELCOLOR_COLOR	"red"
#define DEF_PLANEDATA_SELCOLOR_MONO	BLACK
#define DEF_PLANEDATA_LINE_WIDTH	"1"
#define DEF_PLANEDATA_POWERFONT		"fixed"
#define DEF_PLANEDATA_RADIUS		"4"

struct _XPPlaneWdg {
    TOPMask		flags;
    Tcl_Interp*		ip;		/* our interpreter */
    Tk_Window		tkwin;		/* our window */
    Display*		dpy;		/* dpy of tkwin */
    char*		winPath;	/* strsave of Tk_PathName(tkwin) */
    XPPlaneAxis		xAxis;
    XPPlaneAxis		yAxis;

    /* visuals */
    Cursor		cursor;		/* resource -cursor */
    Tk_3DBorder		bgBorder;	/* resource -background */
    XColor*		bgColor;	/* resource -background */
    XColor*		foreground;	/* resource -foreground */
    XFontStruct*	font;		/* resource -font */
    GC			annoGC;		/* draw annotations this way */
    TOPLogical		selBoxB;	/* resource -selbox */

    int			vMaxSelDist;	/* resource -maxseldist */
    int			axisSelDist;	/* resource -axisseldist */
    TOPLogical		unitAspectB;	/* resource -aspect */
    TOPLogical		unitCircleB;	/* resource -unitCircle */

    XPPlaneData		data;		/* the one and only */
};

/*
 * XPPlaneWdg.flags
 */
#define XPPlaneWdgF_CalcExpose	TOP_BIT(0)
#define XPPlaneWdgF_CalcMap	TOP_BIT(1)
#define XPPlaneWdgF_CalcVars	TOP_BIT(2)
#define XPPlaneWdgF_Calc	(XPPlaneWdgF_CalcExpose|XPPlaneWdgF_CalcMap \
				  |XPPlaneWdgF_CalcVars)
#define XPPlaneWdgF_IdleCalc	TOP_BIT(8)
#define XPPlaneWdgF_On		TOP_BIT(9)
#define XPPlaneWdgF_Valid	TOP_BIT(10)	/* req resources are valid */
#define XPPlaneWdgF_IgnoreVars	TOP_BIT(11)

#define DEF_PLANEWDG_BG_COLOR	DEF_GRAPH_BACKGROUND
#define DEF_PLANEWDG_BG_MONO	WHITE
#define DEF_PLANEWDG_FG_COLOR	"yellow"
#define DEF_PLANEWDG_FG_MONO	BLACK
#define DEF_PLANEWDG_FONT	"fixed"
#define DEF_PLANEWDG_SELBOX	"false"
#define DEF_PLANEWDG_ASPECT	"true"
#define DEF_PLANEWDG_UNITCIRCLE	"true"
#define DEF_PLANEWDG_MAXSELDIST	"20"

static Tk_ConfigSpec _XpPlaneConfigSpecs[] = {
    /*
     * XPPlaneData specs
     */
    {TK_CONFIG_STRING, "-datasrc", "datasrc", "DataSrc",
	NULL, Tk_Offset(XPPlaneWdg, data.dataSrc), 0},
    {TK_CONFIG_STRING, "-dataspec", "dataspec", "DataSpec",
	NULL, Tk_Offset(XPPlaneWdg, data.ext.dataSpec), 0},
    {TK_CONFIG_STRING, "-notifycommand", "notifyCommand", "Command",
	NULL, Tk_Offset(XPPlaneWdg, data.notifyCmd), 0},
    {TK_CONFIG_COLOR, "-normcolor", "normColor", "Foreground",
	DEF_PLANEDATA_NORMCOLOR_COLOR, Tk_Offset(XPPlaneWdg, data.normColor),
	  TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-normcolor", "normColor", "Foreground",
	DEF_PLANEDATA_NORMCOLOR_MONO, Tk_Offset(XPPlaneWdg, data.normColor),
	  TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, "-selcolor", "selColor", "Foreground",
	DEF_PLANEDATA_SELCOLOR_COLOR, Tk_Offset(XPPlaneWdg, data.selColor),
	  TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-selcolor", "selColor", "Foreground",
	DEF_PLANEDATA_SELCOLOR_MONO, Tk_Offset(XPPlaneWdg, data.selColor),
	  TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_INT, "-linewidth", "lineWidth", "LineWidth",
	DEF_PLANEDATA_LINE_WIDTH, Tk_Offset(XPPlaneWdg, data.lineWidth), 0},
    {TK_CONFIG_FONT, "-powerfont", "powerFont", "Font",
	DEF_PLANEDATA_POWERFONT, Tk_Offset(XPPlaneWdg, data.powerFont), 0},
    {TK_CONFIG_INT, "-radius", "radius", "Radius",
	DEF_PLANEDATA_RADIUS, Tk_Offset(XPPlaneWdg, data.vSilarRadius), 0},

    /*
     * XPPlaneAxis specs
     */
    {TK_CONFIG_STRING, "-xaxis", "xaxis", "Axis",
	NULL, Tk_Offset(XPPlaneWdg, xAxis.path), 0},
    {TK_CONFIG_STRING, "-yaxis", "yaxis", "Axis",
	NULL, Tk_Offset(XPPlaneWdg, yAxis.path), 0},

    /*
     * XPPlaneWdg specs
     */
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	NULL, Tk_Offset(XPPlaneWdg, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_PLANEWDG_BG_COLOR, Tk_Offset(XPPlaneWdg, bgBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, NULL, NULL, NULL,
	NULL, Tk_Offset(XPPlaneWdg, bgColor), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_PLANEWDG_BG_MONO, Tk_Offset(XPPlaneWdg, bgBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, NULL, NULL, NULL,
	NULL, Tk_Offset(XPPlaneWdg, bgColor), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},

    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_PLANEWDG_FG_COLOR, Tk_Offset(XPPlaneWdg, foreground),
	  TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_PLANEWDG_FG_MONO, Tk_Offset(XPPlaneWdg, foreground),
	  TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},

    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_PLANEWDG_FONT, Tk_Offset(XPPlaneWdg, font), 0},

    {TK_CONFIG_BOOLEAN, "-selbox", "selbox", "Selbox",
	DEF_PLANEWDG_SELBOX, Tk_Offset(XPPlaneWdg, selBoxB), 0},
    {TK_CONFIG_BOOLEAN, "-aspect", "aspect", "Aspect",
	DEF_PLANEWDG_ASPECT, Tk_Offset(XPPlaneWdg, unitAspectB), 0},
    {TK_CONFIG_BOOLEAN, "-unitCircle", "unitCircle", "UnitCircle",
	DEF_PLANEWDG_UNITCIRCLE, Tk_Offset(XPPlaneWdg, unitCircleB), 0},
    {TK_CONFIG_INT, "-maxseldist", "maxseldist", "MaxSelDist",
	DEF_PLANEWDG_MAXSELDIST, Tk_Offset(XPPlaneWdg, vMaxSelDist), 0},

    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

static void
_xpPlaneSilarBB( XPPlaneData *pData, EEMComplex *pLo, EEMComplex *pHi);

static void _xpPlaneDestroy( XPPlaneWdg *pPlane);

/************************************************************************
 *
 *	XPPlane drawing (the real stuff!)
 *
 ************************************************************************/

/**
    Draw X & Y axes.  This is easier than drawing arbitrary lines.
**/
static void
_xpPlaneDrawAxes( XPPlaneWdg *pPlane) {
    Tk_Window		tkw = pPlane->tkwin;
    Display		*dpy = Tk_Display(tkw);
    Window		win = Tk_WindowId(tkw);
    GC			gc = pPlane->annoGC;
    int			x1, y1;

    if ( pPlane->yAxis.xfa->loWin < 0.0 && 0.0 < pPlane->yAxis.xfa->hiWin ) {
	/* x-axis */
        y1 = xpXfaToView(pPlane->yAxis.xfa,0.0);
        XDrawLine( dpy, win, gc, 0,y1,  Tk_Width(tkw),y1);
    }
    if ( pPlane->xAxis.xfa->loWin < 0.0 && 0.0 < pPlane->xAxis.xfa->hiWin ) {
	/* y-axis */
        x1 = xpXfaToView(pPlane->xAxis.xfa,0.0);
        XDrawLine( dpy, win, gc, x1,0,  x1,Tk_Height(tkw));
    }
    /*XXX: draw axis labels */
}

static void
_xpPlaneDrawSilar2( XPPlaneWdg *pPlane, XPPlaneData *pData, 
  TOPLogical SelB, int x, int y, int power) {
    Display		*dpy = Tk_Display(pPlane->tkwin);
    Window		win = Tk_WindowId(pPlane->tkwin);
    unsigned		r = pData->vSilarRadius;
    GC			gc = SelB ? pData->selGC : pData->normGC;
    int			powNum;

    if ( power > 0 ) {
	/* increase r to compinsate for diameter vs daigonal */
	r += 1;
        XDrawArc( dpy, win, gc, x-(int)r, y-(int)r, 2*r, 2*r, 0, 359*64);
	powNum = power;
    } else if ( power < 0 ) {
	XDrawPoint( dpy, win, gc, x,  y);
	XDrawLine( dpy, win, gc, x-(int)r, y-(int)r,  x+(int)r, y+(int)r);
	XDrawLine( dpy, win, gc, x-(int)r, y+(int)r,  x+(int)r, y-(int)r);
	powNum = - power;
    } else {
	return;
    }
    if ( SelB && pPlane->selBoxB ) {
	XDrawRectangle( dpy, win, gc, x-1-(int)r, y-1-(int)r,  2*r+2, 2*r+2);
    }
    if ( powNum > 1 ) {
	char buf[10];
	sprintf( buf, "%d", powNum);
	XDrawString( dpy, win, gc, x+3+(int)r, y+3+(int)r, buf,
	  (int)strlen(buf));
    }
}

#define XPPLANE_DRAW_SILAR_NORM( pData, pPS)		\
    _xpPlaneDrawSilar2( (pData)->PntWdg, (pData),	\
      TOP_IsOn((pPS)->flags,EEMSilarF_Sel),		\
      (pPS)->vLoc.x, (pPS)->vLoc.y, (pPS)->power)

#define XPPLANE_DRAW_SILAR_CONJ( pData, pPS)			\
    _xpPlaneDrawSilar2( (pData)->PntWdg, (pData),		\
      TOP_IsOn((pPS)->flags,EEMSilarF_Sel),			\
      (pPS)->vLocConj.x, (pPS)->vLocConj.y, (pPS)->power)

static void
_xpPlaneDrawSilar( XPPlaneData *pData, XPPlaneSilar *pPS) {
    if ( TOP_IsOn(pPS->flags,EEMSilarF_VisNorm) ) {
        XPPLANE_DRAW_SILAR_NORM( pData, pPS);
    }
    if ( TOP_IsOn(pPS->flags,EEMSilarF_VisConj) ) {
    	XPPLANE_DRAW_SILAR_CONJ( pData, pPS);
    }
}

static void
_xpPlaneDrawLociMark( XPPlaneData *pData, XPPlaneSilar *pPS) {
    XPPlaneWdg		*pPlane = pData->PntWdg;
    Display		*dpy = Tk_Display(pPlane->tkwin);
    Window		win = Tk_WindowId(pPlane->tkwin);
    int			r = pData->vSilarRadius;
    TOPLogical		SelB = TOP_FALSE;
    GC			gc = SelB ? pData->selGC : pData->normGC;
    int			x = pPS->vLoc.x;
    int			y = pPS->vLoc.y;
    XPoint		pts[5];

    if ( TOP_IsOff(pPS->flags,EEMSilarF_VisNorm) || pPS->power == 0 ) {
	return;
    }
    /* diamond */
    pts[0].x = x;	pts[0].y = y-r;
    pts[1].x = x+r;	pts[1].y = y;
    pts[2].x = x;	pts[2].y = y+r;
    pts[3].x = x-r;	pts[3].y = y;
    pts[4].x = x;	pts[4].y = y-r;

    XDrawLines( dpy, win, gc, pts, 4, CoordModeOrigin);
}

static void
_xpPlaneDrawLoci( XPPlaneData *pData) {
    XPPlaneWdg		*pPlane = pData->PntWdg;
    Tk_Window		tkw = pPlane->tkwin;
    Display		*dpy = Tk_Display(tkw);
    Window		win = Tk_WindowId(tkw);
    TOPLogical		SelB = TOP_FALSE;
    GC			gc = SelB ? pData->selGC : pData->normGC;
    XPPlaneDataExt	*pExt = &pData->ext;
    XPPlaneLocus	*pPL;
    int			i, np;

    for ( i=0; i < XPPLANE_LociNum(pExt); i++) {
	pPL = & XPPLANE_LociGet(pExt,i);
	if ( (np = XPPLANE_LocusPtNum(pPL)) < 2 )
	    continue;
	XDrawLines( dpy, win, gc, XPPLANE_LocusPtArray(pPL), np,
	  CoordModeOrigin);
    }
}

static void
_xpPlaneExpose( XPPlaneWdg *pPlane) {
    Tk_Window		tkw = pPlane->tkwin;
    Display		*dpy;
    Window		win;

    if ( tkw == NULL || !Tk_IsMapped(tkw) )
	return;

    dpy = Tk_Display(pPlane->tkwin);
    win = Tk_WindowId(pPlane->tkwin);
    XClearWindow( dpy, win);
    _xpPlaneDrawAxes( pPlane);
    if ( pPlane->unitCircleB ) {
	xpXfaDrawCircle( dpy, win, pPlane->annoGC, 
	  pPlane->xAxis.xfa, pPlane->yAxis.xfa, 0.0, 0.0, 1.0);
    }

    {
	XPPlaneData	*pData = &pPlane->data;

	TOPVEC_LOOP_FORW_BEGIN( &pData->vSilars, XPPlaneSilar, pPS) {
	    _xpPlaneDrawSilar( pData, pPS);
	} TOPVEC_LOOP_FORW_END();
	_xpPlaneDrawLoci( pData);
	TOPVEC_LOOP_FORW_BEGIN( &pData->vLociMarks, XPPlaneSilar, pPS) {
	    _xpPlaneDrawLociMark( pData, pPS);
	} TOPVEC_LOOP_FORW_END();
    }
    xpAxisExposeDrawWin( pPlane->xAxis.axisHandle);
    xpAxisExposeDrawWin( pPlane->yAxis.axisHandle);
}

static void
_xpPlaneMapSilar( XPPlaneData *pData, EEMSilar *pSilar, XPPlaneSilar *pPS) {
    XPPlaneWdg	*pPlane = pData->PntWdg;
    int		xloc;

    pPS->flags = 
      (pSilar->flags & ~(EEMSilarF_Vis|EEMSilarF_Tag1|EEMSilarF_Tag2))
      | (pPS->flags & (EEMSilarF_Tag1|EEMSilarF_Tag2));
    if ( (pPS->power = pSilar->power) == 0 )
	return;
    /*
     * Perform clipping in win-space since the mapping can "wrap-around"
     * when the user zooms up real close.  Clip the "norm" and "conj" silars
     * seperately, since they can be far away.  Clip on imaginary part first
     * since thats the same for both.
     */
    if ( pSilar->loc.r < pPlane->xAxis.xfa->loWin
      || pSilar->loc.r > pPlane->xAxis.xfa->hiWin ) {
	return;
    }
    xloc = xpXfaToView( pPlane->xAxis.xfa, pSilar->loc.r);

    if ( pSilar->loc.i >= pPlane->yAxis.xfa->loWin
      && pSilar->loc.i <= pPlane->yAxis.xfa->hiWin ) {
	pPS->vLoc.x = xloc;
    	pPS->vLoc.y = xpXfaToView( pPlane->yAxis.xfa, pSilar->loc.i);
	pPS->flags |= EEMSilarF_VisNorm;
    }
    if ( TOP_IsOn(pSilar->flags,EEMSilarF_Conj)
      && (-pSilar->loc.i) >= pPlane->yAxis.xfa->loWin
      && (-pSilar->loc.i) <= pPlane->yAxis.xfa->hiWin ) {
	pPS->vLocConj.x = xloc;
	pPS->vLocConj.y = xpXfaToView( pPlane->yAxis.xfa, -pSilar->loc.i);
	pPS->flags |= EEMSilarF_VisConj;
    }
}

static void
_xpPlaneDataMapMarks( XPPlaneData *pData) {
    XPPlaneSilar	*pPS;

    topVecSetNum( &pData->vLociMarks, XPPLANE_WMARKS_Num(pData));
    XPPLANE_WMARKS_LOOP_BEGIN( pData, silNum, pSilar) {
	pPS = &XPPLANE_VMARKS_Get(pData, silNum);
	_xpPlaneMapSilar( pData, pSilar, pPS);
	pPS->flags |= EEMSilarF_Tag1;
    } XPPLANE_WMARKS_LOOP_END();
}

/*
 * This function will map all of the silars in {wSilars} into {vSilars}.
 * All old information is lost, so use of this function assumes a complete
 * screen redraw.
 */
static void 
_xpPlaneDataMapAll( XPPlaneData *pData) {
    XPPlaneWdg		*pPlane = pData->PntWdg;
    XPPlaneSilar	*pPS;

    /* map silars */
    topVecSetNum( &pData->vSilars, XPPLANE_WSILS_Num(pData));
    XPPLANE_WSILS_LOOP_BEGIN( pData, silNum, pSilar) {
	pPS = &XPPLANE_VSILS_Get(pData, silNum);
	_xpPlaneMapSilar( pData, pSilar, pPS);
	pPS->flags |= EEMSilarF_Tag1;
    } XPPLANE_WSILS_LOOP_END();

    _xpPlaneSilarBB( pData, &pData->rngLo, &pData->rngHi);
    xpAxisInformRange( pPlane->xAxis.axisHandle,
      pData->rngLo.r, pData->rngHi.r, XPAxisZM_In|XPAxisZM_Out);
    xpAxisInformRange( pPlane->yAxis.axisHandle,
      pData->rngLo.i, pData->rngHi.i, XPAxisZM_In|XPAxisZM_Out);

    /* map loci */
    if ( pData->dh != NULL && pData->dh->calcLoci != NULL ) {
	(*(pData->dh->calcLoci))( &pData->ext);
    }
    if ( pData->dh != NULL && pData->dh->mapLoci != NULL ) {
	(*(pData->dh->mapLoci))( &pData->ext);
    }

    _xpPlaneDataMapMarks( pData);
}

static void 
_xpPlaneDataCalcVars( XPPlaneData *pData) {
    XPPlaneWdg		*pPlane = pData->PntWdg;
    Tcl_Interp		*ip = pPlane->ip;
    char		*n1 = pPlane->winPath;
    char		buf[100];
    
    pPlane->flags |= XPPlaneWdgF_IgnoreVars;
    eemCplxFmt( buf, &pData->ext.wSilars.gain, NULL);
    Tcl_SetVar2( ip, n1, "gain", buf, TCL_GLOBAL_ONLY);
    pPlane->flags &= ~XPPlaneWdgF_IgnoreVars;
}

static char*
_xpPlaneDataTraceVars( ClientData cld, Tcl_Interp *ip,
  char *n1_foo, char *n2, int f) {
    XPPlaneData		*pData = (XPPlaneData*) cld;
    XPPlaneWdg		*pPlane = pData->PntWdg;
    char		*n1 = pPlane->winPath;
    char		*newval;

    if ( TOP_IsOn(pPlane->flags,XPPlaneWdgF_IgnoreVars) )
	return NULL;
    newval = Tcl_GetVar2( ip, n1, n2, TCL_GLOBAL_ONLY);
    /*IF*/ if ( strcmp(n2,"gain")==0 ) {
	if ( eemCplxStrPrsTcl( ip, newval, &pData->ext.wSilars.gain) != TCL_OK )
	    return "Invalid complex number (gain)";
	if ( pData->dh && pData->dh->changeSilar )
	    (*(pData->dh->changeSilar))( &pData->ext, XPPlaneCT_Modify, -1);
    } else {
	return NULL;
    }
    return NULL;
}

static void
_xpPlaneCalcNow( XPPlaneWdg *pPlane) {
    if ( TOP_IsOn( pPlane->flags, XPPlaneWdgF_CalcVars) ) {
	pPlane->flags &= ~XPPlaneWdgF_CalcVars;
        _xpPlaneDataCalcVars( &pPlane->data);
    }
    if ( TOP_IsOn( pPlane->flags, XPPlaneWdgF_CalcMap) ) {
	pPlane->flags &= ~XPPlaneWdgF_CalcMap;
        _xpPlaneDataMapAll( &pPlane->data);
	pPlane->flags |= XPPlaneWdgF_CalcExpose;
    }
    if ( TOP_IsOn( pPlane->flags, XPPlaneWdgF_CalcExpose) ) {
	pPlane->flags &= ~XPPlaneWdgF_CalcExpose;
        _xpPlaneExpose( pPlane);
    }
}

static void
_xpPlaneIdleCalcCB( ClientData cld) {
    XPPlaneWdg *pPlane = (XPPlaneWdg *) cld;
    
    pPlane->flags &= ~XPPlaneWdgF_IdleCalc;
    if ( xpAxisXformPendingB( pPlane->xAxis.axisHandle)
      || xpAxisXformPendingB( pPlane->yAxis.axisHandle) ) {
	return;
    }
    TOP_PDBG((SPKG,"%s: idle calc %x%s", pPlane->winPath, pPlane->flags,
      TOP_IsOff(pPlane->flags,XPPlaneWdgF_Valid)?" (not valid)":""));
    if ( TOP_IsOff(pPlane->flags,XPPlaneWdgF_Valid) )
	return;
    _xpPlaneCalcNow( pPlane);
}

static void
_xpPlaneIdleCalc( XPPlaneWdg *pPlane, int calcMask) {
    pPlane->flags |= (calcMask & XPPlaneWdgF_Calc);
    if ( TOP_IsOn( pPlane->flags, XPPlaneWdgF_IdleCalc)
      || pPlane->tkwin == NULL 
      || !Tk_IsMapped(pPlane->tkwin) ) {
	return;
    }
    pPlane->flags |= XPPlaneWdgF_IdleCalc;
    Tk_DoWhenIdle( _xpPlaneIdleCalcCB, (ClientData) pPlane);
}

static void
_xpPlaneIdleCalcUndo( XPPlaneWdg *pPlane) {
    if ( TOP_IsOn(pPlane->flags, XPPlaneWdgF_IdleCalc) ) {
        Tk_CancelIdleCall( _xpPlaneIdleCalcCB, (ClientData) pPlane);
	pPlane->flags &= ~XPPlaneWdgF_IdleCalc;
    }
}

/************************************************************************
 *
 *	XPPlaneAxis Tk muck
 *
 ************************************************************************/

static void
_xpPlaneAxisConstructor( XPPlaneAxis *pPAxis, XPPlaneWdg *pPlane) {
    pPAxis->PntWdg = pPlane;
    pPAxis->path = NULL;
    pPAxis->axisHandle = NULL;
    pPAxis->xfa = NULL;
}

static void
_xpPlaneAxisDestructor( XPPlaneAxis *pPAxis) {
    if ( pPAxis->path != NULL ) {
	memFree( pPAxis->path);
        pPAxis->path = NULL;
    }
}

static int
_xpPlaneAxisConfig( Tcl_Interp *ip, XPPlaneAxis *pPAxis) {
    XPPlaneWdg		*pPlane = pPAxis->PntWdg;

    if ( pPAxis->path == NULL || pPAxis->path[0] == '\0' ) {
	pPAxis->axisHandle = NULL;
	pPAxis->xfa = NULL;
	Tcl_AppendResult(ip,"Must specify axis path",NULL);
	return TCL_ERROR;
    }
    if ( xpAxisGetHandle( ip, pPAxis->path, &pPAxis->axisHandle) != TCL_OK ) {
        Tcl_AppendResult( ip, "\n%s: lookup failed for axis %s",
	  pPlane->winPath, pPAxis->path);
	return TCL_ERROR;
    }
    pPAxis->xfa = xpAxisGetXfa( pPAxis->axisHandle);
    return TCL_OK;
}

/************************************************************************
 *
 *	XPPlaneData Tk muck
 *
 ************************************************************************/

#define EXT_OFFSET		Tk_Offset(XPPlaneData,ext)
#define PLANE_EXT_TO_DATA(pExt)	\
    TOP_Cast(XPPlaneData*, TOP_CharPtr(pExt)-EXT_OFFSET)

static int
_xpPlaneDataSetDataHandler( XPPlaneData *pData, XPPlaneDataHandler *newDH) {
    if ( pData->dh && pData->dh->destructor ) {
	(*(pData->dh->destructor))( &pData->ext);
    }
    pData->dh = NULL;
    if ( newDH ) {
	if ( newDH->constructor ) {
	    if ( (*(newDH->constructor))( &pData->ext) == TOP_FALSE )
		return TCL_ERROR;
	}
	pData->dh = newDH;
    }
    return TCL_OK;
}

void
xpPlaneDataUpdateOne( XPPlaneDataExt *pExt, int idx) {
    XPPlaneData		*pData = PLANE_EXT_TO_DATA(pExt);
    XPPlaneWdg		*pPlane = pData->PntWdg;
    Tk_Window		tkw = pPlane->tkwin;
    EEMSilar		*pSilar;
    XPPlaneSilar	*pPS;

    if ( idx == -1 ) {
        _xpPlaneIdleCalc( pPlane, XPPlaneWdgF_CalcVars);
	return;
    }
    if ( tkw == NULL || !Tk_IsMapped(tkw) ) {
	/* save away a remap for later */
        _xpPlaneIdleCalc( pPlane, XPPlaneWdgF_CalcMap);
	return;
    }
    if ( idx < TOPVEC_Num(&pData->vSilars) ) {
        pPS = &TOPVEC_Get(&pData->vSilars,idx,XPPlaneSilar);
	/* erase the current silar image from the screen */
        _xpPlaneDrawSilar( pData, pPS);
    } else {
	/* grow our vector to match */
	topVecSetNum(&pData->vSilars, (unsigned)idx+1);
        pPS = &TOPVEC_Get(&pData->vSilars,idx,XPPlaneSilar);
    }
    if ( TOP_IsOn(pPS->flags,EEMSilarF_Tag2) && pData->notifyCmd != NULL ) {
        topTclBgEvalFmt( pPlane->ip, SPKG, "%s %s %d", pData->notifyCmd,
	  pPlane->winPath, idx);
    }
    if ( idx < XPPLANE_WSILS_Num(pData) ) {
        pSilar = &XPPLANE_WSILS_Get(pData,idx);
	/* map it and draw it */
        _xpPlaneMapSilar( pData, pSilar, pPS);
        _xpPlaneDrawSilar( pData, pPS);
    } else {
	; /* its a delete notify.  We've already erased image...all done */
    }
}

void
xpPlaneDataUpdateMarks( XPPlaneDataExt *pExt) {
    XPPlaneData		*pData = PLANE_EXT_TO_DATA(pExt);
    XPPlaneWdg		*pPlane = pData->PntWdg;
    Tk_Window		tkw = pPlane->tkwin;

    if ( tkw == NULL || !Tk_IsMapped(tkw) ) {
	/* save away a remap for later */
        _xpPlaneIdleCalc( pPlane, XPPlaneWdgF_CalcMap);
	return;
    }
    _xpPlaneDataMapMarks( pData);
    _xpPlaneIdleCalc( pPlane, XPPlaneWdgF_CalcExpose);
}

void
xpPlaneDataUpdateAll( XPPlaneDataExt *pExt) {
    XPPlaneData		*pData = PLANE_EXT_TO_DATA(pExt);
    XPPlaneWdg		*pPlane = pData->PntWdg;

    /*
     * we grow our vector now so that we dont have to worry
     * about referenceing beyond the end of the vSilars array
     * during a tcl command.
     */
    topVecInsureNum( &pData->vSilars, XPPLANE_WSILS_Num(pData));
    _xpPlaneIdleCalc( pPlane, XPPlaneWdgF_CalcMap|XPPlaneWdgF_CalcVars);
}

void
xpPlaneDataDestroy( XPPlaneDataExt *pExt) {
    XPPlaneData		*pData = PLANE_EXT_TO_DATA(pExt);
    XPPlaneWdg		*pPlane = pData->PntWdg;

    _xpPlaneDataSetDataHandler( pData, NULL);
    if ( pPlane->tkwin != NULL )
        _xpPlaneDestroy( pPlane);
}


void
xpPlaneLocusConstructor( XPPlaneLocus *pPL) {
    pPL->flags = 0;
    topVecConstructor( &pPL->vPts, sizeof(XPoint));
}

void
xpPlaneLocusDestructor( XPPlaneLocus *pPL) {
    pPL->flags = 0;
    topVecDestructor( &pPL->vPts);
}

void
xpPlaneLociSetNum( XPPlaneDataExt *pExt, unsigned n) {
    int		i, curnum = XPPLANE_LociNum(pExt);

    for (i=n; i < curnum; i++) {	/* shrink it */
	xpPlaneLocusDestructor( &XPPLANE_LociGet(pExt,i));
    }
    topVecSetNum( &pExt->vLoci, n);
    for (i=curnum; i < n; i++) {	/* grow it */
	xpPlaneLocusConstructor( &XPPLANE_LociGet(pExt,i));
    }
}

static void
_xpPlaneDataConstructor( XPPlaneData *pData, XPPlaneWdg *pPlane) {
    MEM_CLEAR(pData);
    pData->PntWdg = pPlane;
    pData->normGC = None;
    pData->selGC = None;
    topVecConstructor( &pData->vSilars, sizeof(XPPlaneSilar));
    topVecConstructor( &pData->vLociMarks, sizeof(XPPlaneSilar));
    eemSilarGrpConstructor( &pData->ext.wSilars);
    eemSilarGrpConstructor( &pData->ext.wLociMarks);
    topVecConstructor( &pData->ext.vLoci, sizeof(XPPlaneLocus));
    _xpPlaneDataSetDataHandler( pData, &XpPlaneSimpleDataHandler);
}

static void
_xpPlaneDataDestructor( XPPlaneData *pData) {
    _xpPlaneDataSetDataHandler( pData, NULL);
    eemSilarGrpDestructor( &pData->ext.wSilars);
    eemSilarGrpDestructor( &pData->ext.wLociMarks);
    xpPlaneLociSetNum( &pData->ext, 0);
    topVecDestructor( &pData->ext.vLoci);
    topVecDestructor( &pData->vSilars);
    topVecDestructor( &pData->vLociMarks);
    if ( pData->normGC != None ) {
	 Tk_FreeGC( pData->PntWdg->dpy, pData->normGC);
	 pData->normGC = None;
    }
    if ( pData->selGC != None ) {
	 Tk_FreeGC( pData->PntWdg->dpy, pData->selGC);
	 pData->selGC = None;
    }
    pData->PntWdg = NULL;
}

static int
_xpPlaneDataDrawK( Tcl_Interp *ip, XPPlaneData *pData, int aC, char **aV) {
    TOPMask	flags = 0;
    char	*slo, *shi;
    double	klo, khi;

    if ( pData->dh == NULL || pData->dh->drawK == NULL ) {
	Tcl_AppendResult(ip,"Operation not supported by handler",NULL);
	return TCL_ERROR;
    }
    slo = aV[0];
    if ( slo[0] == '-' && TOP_ToLower(slo[1]) == 'i' ) {
	flags |= XPPlaneLF_NegInf;
    } else {
	if ( Tcl_GetDouble( ip, slo, &klo) != TCL_OK )
	    return TCL_ERROR;
    }

    shi = aV[1];
    if ( TOP_ToLower(shi[0]) == 'i' ) {
	flags |= XPPlaneLF_PosInf;
    } else {
	if ( Tcl_GetDouble( ip, shi, &khi) != TCL_OK )
	    return TCL_ERROR;
    }
    if ( ! (*(pData->dh->drawK))( &pData->ext, flags, klo, khi) ) {
	Tcl_AppendResult(ip,"Handler failed.",NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

static int
_xpPlaneDataMarkK( Tcl_Interp *ip, XPPlaneData *pData, int aC, char **aV) {
    TOPMask	flags = 0;
    double	klo;

    if ( pData->dh == NULL || pData->dh->markK == NULL ) {
	Tcl_AppendResult(ip,"Operation not supported by handler",NULL);
	return TCL_ERROR;
    }
    if ( aC <= 0 ) {
	klo = 0.0;
    } else {
	flags |= XPPlaneLF_Mark;
	if ( Tcl_GetDouble( ip, aV[0], &klo) != TCL_OK )
	    return TCL_ERROR;
    }
    if ( ! (*(pData->dh->markK))( &pData->ext, flags, klo) ) {
	Tcl_AppendResult(ip,"Handler failed.",NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}


/**
    By convention, a {which} of -1 means create a new silar.
    Action depends on {aC}:
	pos:	create a new silar or modify an existing silar
		according to {aV}.  If {which} is invalid,
		then create a new silar, otherwise modify that
		specified by {which}.
        0:	return info on that silar (as in configInfo).
	-1: 	delete the silar according to {which}
**/
static int
_xpPlaneDataCmd( Tcl_Interp *ip, XPPlaneData *pData, 
  int which, char conjCode, int aC, char **aV) {
    EEMSilar		*pSilar;
    int			numsils = XPPLANE_WSILS_Num(pData);
    XPPlaneChangeType	ct;
    TOPLogical		dhr = TOP_TRUE;

    if ( pData->dh == NULL || pData->dh->changeSilar == NULL ) {
	Tcl_AppendResult( ip, "data handler: cmds not supported",
	  NULL);
	return TCL_ERROR;
    }
    pSilar = ( which >= 0 && which < numsils ) 
      ? &XPPLANE_WSILS_Get(pData, which) : NULL;

    if ( aC == 0 ) { 
	int doFlip = TOP_FALSE;
	if ( conjCode != '\0' && EEM_SILAR_ConjB(pSilar) ) {
	    doFlip = conjCode == '+' ? pSilar->loc.i < 0 :  pSilar->loc.i > 0;
	}
	if ( doFlip )	pSilar->loc.i = 0 - pSilar->loc.i;
	eemSilarFmtTcl( ip, pSilar, TOP_FALSE);
	if ( doFlip )	pSilar->loc.i = 0 - pSilar->loc.i;
	return TCL_OK;
    }
    if ( aC == -1 ) {
	ct = XPPlaneCT_Delete;
	if ( which >= 0 ) {
    	    XPPLANE_WSILS_Get(pData, which).power = 0;
    	    dhr = (*(pData->dh->changeSilar))( &pData->ext, ct, which);
	} else if ( which == XPPS_Tag1 ) {
	    XPPLANE_VSILS_LOOP_BEGIN(pData, silIdx, pPS) {
		if ( TOP_IsOn(pPS->flags,EEMSilarF_Tag1) ) {
    	            XPPLANE_WSILS_Get(pData, silIdx).power = 0;
    	    	    dhr = (*(pData->dh->changeSilar))( &pData->ext, ct, silIdx);
		    if ( ! dhr )	break;
		}
	    } XPPLANE_VSILS_LOOP_END();
	}
    } else {
	if ( which >= 0 && which < numsils ) {
	    ct = XPPlaneCT_Modify;
	} else {
	    which = eemSilarGrpGetAvail( &pData->ext.wSilars);
	    ct = XPPlaneCT_Insert;
	}
	pSilar = &XPPLANE_WSILS_Get( pData, which);
	if ( eemSilarPrsTcl( ip, pSilar, aC, aV) != TCL_OK ) {
	    return TCL_ERROR;
	}
	dhr = (*(pData->dh->changeSilar))( &pData->ext, ct, which);
    }
    if ( ! dhr ) {
	Tcl_AppendResult( ip, "data handler: cmd failed", NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/************************************************************************
 *
 *	XPPlaneWdg Tk muck
 *
 ************************************************************************/

static void
_xpPlaneDestroyCB( ClientData cld) {
    static char	func[] = "_xpPlaneDestroyCB";
    XPPlaneWdg *pPlane = (XPPlaneWdg *) cld;
    Tcl_Interp	*ip = pPlane->ip;

    if ( pPlane->winPath == NULL ) {
	topProgMsg("%s: Duplicate destroy", func);
	return;
    }
    Tcl_DeleteCommand( ip, pPlane->winPath);
    Tcl_UnsetVar2( ip, pPlane->winPath, NULL, TCL_GLOBAL_ONLY);
    if ( pPlane->data.PntWdg != NULL )
        _xpPlaneDataDestructor( &pPlane->data);
    _xpPlaneAxisDestructor( &pPlane->xAxis);
    _xpPlaneAxisDestructor( &pPlane->yAxis);

    if (pPlane->bgColor != NULL) {
	Tk_FreeColor(pPlane->bgColor);
	pPlane->bgColor = NULL;
    }
    if (pPlane->bgBorder != NULL) {
	Tk_Free3DBorder(pPlane->bgBorder);
	pPlane->bgBorder = NULL;
    }
    if (pPlane->winPath != NULL) {
	memFree(pPlane->winPath);
	pPlane->winPath = NULL;
    }
    if (pPlane->cursor != None) {
	Tk_FreeCursor(pPlane->dpy, pPlane->cursor);
	pPlane->cursor = None;
    }
    MEM_FREE(pPlane);
}

static void
_xpPlaneDestroy( XPPlaneWdg *pPlane) {
    Tk_Window	tkw = pPlane->tkwin;

    if ( tkw != NULL ) {
	pPlane->tkwin = NULL;
	/*
	 * The Preserve/Release assures that the DestroyWindow will
	 * fully complete before we go wack stuff (preventing recursion)
	 */
	Tk_Preserve( (ClientData) pPlane);
	Tk_DestroyWindow(tkw);
	Tk_Release( (ClientData) pPlane);
	return;
    }
    _xpPlaneIdleCalcUndo( pPlane);
    Tk_EventuallyFree( (ClientData) pPlane, _xpPlaneDestroyCB);
}

static void
_xpPlaneEventCB( ClientData cld, XEvent *pEvent) {
    XPPlaneWdg *pPlane = (XPPlaneWdg *) cld;

    /*IF*/ if ((pEvent->type == Expose) && (pEvent->xexpose.count == 0)) {
	_xpPlaneIdleCalc( pPlane, XPPlaneWdgF_CalcExpose);
    } else if ( pEvent->type == DestroyNotify ) {
	pPlane->tkwin = NULL;
	_xpPlaneDestroy( pPlane);
    }
}


static void
_xpPlaneSilarBB( XPPlaneData *pData, EEMComplex *pLo, EEMComplex *pHi) {
    EEMComplex	lo, hi;
    EEMSilar	*pSilar;

    EEM_CPLX_SET(&lo,-1.0,-1.0);
    EEM_CPLX_SET(&hi, 1.0, 1.0);
    XPPLANE_VSILS_LOOP_BEGIN( pData, silIdx, pPS) {
	if ( TOP_IsOn(pPS->flags,EEMSilarF_Tag1) ) {
	    pSilar = &XPPLANE_WSILS_Get(pData, silIdx);
	    if ( pSilar->loc.r < lo.r )		lo.r = pSilar->loc.r;
	    if ( pSilar->loc.r > hi.r )		hi.r = pSilar->loc.r;
	    if ( pSilar->loc.i < lo.i )		lo.i = pSilar->loc.i;
	    if ( pSilar->loc.i > hi.i )		hi.i = pSilar->loc.i;
	    if ( TOP_IsOn(pPS->flags,EEMSilarF_Conj) ) {
	        if ( -pSilar->loc.i < lo.i )	lo.i = -pSilar->loc.i;
	        if ( -pSilar->loc.i > hi.i )	hi.i = -pSilar->loc.i;
	    }
	}
    } XPPLANE_VSILS_LOOP_END();
    *pLo = lo;
    *pHi = hi;
}

static void
_xpPlaneSilarTag( XPPlaneData *pData, int tagme) {
    XPPLANE_VSILS_LOOP_BEGIN( pData, idx, pPS) {
	if ( idx == tagme ) {
	    pPS->flags |= EEMSilarF_Tag1;
	} else {
	    pPS->flags &= ~EEMSilarF_Tag1;
	}
    } XPPLANE_VSILS_LOOP_END();
}

/**
    Search the currently displayed silars for the one closest to the
    mapped (pixel) location {x y}.  Returns -1 if not found.
    TODO: round-robin.
    TODO: deal with recording conjugate vs non-conj silar
**/
static int
_xpPlaneSilarFindByPt( XPPlaneData *pData, int x, int y) {
    XPPlaneWdg	*pPlane = pData->PntWdg;
    int		dx, dist, mindist = 0.0, minidx = XPPS_None;
    int		maxdistsqr = TOP_SQR(pPlane->vMaxSelDist);

    XPPLANE_VSILS_LOOP_BEGIN( pData, idx, pPS) {
	pPS->flags &= ~EEMSilarF_Tag1;
	dx = pPS->vLoc.x - x;
	if ( TOP_IsOn(pPS->flags,EEMSilarF_VisNorm) ) {
	    dist = TOP_NORMSQR(dx,pPS->vLoc.y - y);
	    if ( dist <= maxdistsqr && (dist < mindist || minidx < 0) ) {
		mindist = dist; minidx = idx;
		pPS->flags |= EEMSilarF_Tag1Norm;
	    }
	}
	if ( TOP_IsOn(pPS->flags,EEMSilarF_VisConj) ) {
	    dist = TOP_NORMSQR(dx,pPS->vLocConj.y - y);
	    if ( dist <= maxdistsqr && (dist < mindist || minidx < 0) ) {
		mindist = dist; minidx = idx;
		pPS->flags |= EEMSilarF_Tag1Conj;
	    }
	}
    } XPPLANE_VSILS_LOOP_END();
    return minidx;
}

int
_xpPlaneSilarFindByRect( XPPlaneData *pData, int x, int y, int w, int h) {
    /* XPPlaneWdg	*pPlane = pData->PntWdg; */
    int		dx, dy, minidx = XPPS_None;
    int		cnt = 0;

    XPPLANE_VSILS_LOOP_BEGIN( pData, idx, pPS) {
	pPS->flags &= ~EEMSilarF_Tag1;
	dx = pPS->vLoc.x - x;
	if ( pPS->power == 0 || dx < 0 || dx > w )
	    continue;
	if ( TOP_IsOn(pPS->flags,EEMSilarF_VisNorm) ) {
	    dy = pPS->vLoc.y - y;
	    if ( dy >= 0 && dy <= h ) {
		pPS->flags |= EEMSilarF_Tag1Norm;
		++cnt;	minidx = idx;
	    }
	}
	if ( TOP_IsOn(pPS->flags,EEMSilarF_VisConj) ) {
	    dy = pPS->vLocConj.y - y;
	    if ( dy >= 0 && dy <= h ) {
		pPS->flags |= EEMSilarF_Tag1Conj;
		if ( minidx != idx ) {
		    ++cnt; minidx = idx;
		}
	    }
	}
    } XPPLANE_VSILS_LOOP_END();
    return cnt >= 2 ? XPPS_Tag1 : minidx;
}

static int
_xpPlaneSilarGetIndex( Tcl_Interp *ip, XPPlaneWdg *pPlane, char *whichStr,
  int *pIdx, XPPlaneSelectType tagCode, char *pConjCode) {
    XPPlaneData		*pData = &pPlane->data;
    char		conjCode = '\0';
    *pIdx = XPPS_None;

    topVecInsureNum( &pData->vSilars, XPPLANE_WSILS_Num(pData));
    if ( *whichStr == '+' || *whichStr == '-' ) {
	conjCode = *whichStr;
	do 	++whichStr;
	while	( isspace(*whichStr) );
    }
    if ( isdigit(*whichStr) ) {
	if ( Tcl_GetInt( ip, whichStr, pIdx) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\nSilarIndex bad int: ``",whichStr,"''",NULL);
	    return TCL_ERROR;
	}
	if ( *pIdx < 0 || *pIdx >= XPPLANE_WSILS_Num(pData) ) {
	    Tcl_AppendResult(ip,"\nSilarIndex out of range: ``",whichStr,"''",
	      NULL);
	    return TCL_ERROR;
	}
	if ( tagCode != XPPST_ExactlyOne )
	    _xpPlaneSilarTag( pData, *pIdx);
    } else if ( *whichStr == 's' ) {
	/* get selected */
	int		cnt = 0;
	XPPlaneSilar	*pPS;
	XPPLANE_WSILS_LOOP_BEGIN( pData, idx, pSilar) {
	    pPS = &XPPLANE_VSILS_Get(pData, idx);
	    /*
	     * It would be nice to allow power!=0 to be selected;
	     * this would allow a pole to become a zero by transitioning
	     * through power=0.  But this isnt currently possible
	     * since it makes silars come back from the dead.
	     */
	    if ( TOP_IsOn(pSilar->flags,EEMSilarF_Sel) && pSilar->power != 0 ) {
		pPS->flags |= EEMSilarF_Tag1;
		*pIdx = idx;	++cnt;
	    } else {
		pPS->flags &= ~EEMSilarF_Tag1;
	    }
	} XPPLANE_WSILS_LOOP_END();
	if ( cnt >= 2 )
	    *pIdx = XPPS_Tag1;
    } else if ( *whichStr == 'a' ) {
	/* get all */
	XPPlaneSilar	*pPS;
	XPPLANE_WSILS_LOOP_BEGIN( pData, idx, pSilar) {
	    pPS = &XPPLANE_VSILS_Get(pData, idx);
	    if ( pSilar->power != 0 ) {
		pPS->flags |= EEMSilarF_Tag1;
	    } else {
		pPS->flags &= ~EEMSilarF_Tag1;
	    }
	} XPPLANE_WSILS_LOOP_END();
	*pIdx = XPPS_Tag1;
    } else if ( *whichStr == '@' ) {
	/* get nearest to xy point */
	int	x, y;
	if ( eemInt2PrsTcl( ip, whichStr+1, &x, &y) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\nSilarIndex bad pt: ``",whichStr,"''",NULL);
	    return TCL_ERROR;
	}
	*pIdx = _xpPlaneSilarFindByPt( pData, x, y);
    } else if ( *whichStr == '<' ) {
	/* get inside xywh rect */
	int	x, y, w, h;
	if ( eemInt4PrsTcl( ip, whichStr+1, &x, &y, &w, &h) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\nSilarIndex bad pt: ``",whichStr,"''",NULL);
	    return TCL_ERROR;
	}
	*pIdx = _xpPlaneSilarFindByRect( pData, x, y, w, h);
    } else {
	Tcl_AppendResult(ip,"\nSilarIndex bad: ``",whichStr,"''",NULL);
	return TCL_ERROR;
    }
    if ( *pIdx == XPPS_None && tagCode != XPPST_TagAtleast ) {
	Tcl_SetResult(ip,"No matching singularity.",TCL_STATIC);
	return TCL_ERROR;
    }
    if ( *pIdx == XPPS_Tag1 && tagCode == XPPST_ExactlyOne ) {
	Tcl_SetResult(ip,"Multiple singularites not allowed.",TCL_STATIC);
	return TCL_ERROR;
    }
    if ( pConjCode )
	*pConjCode = conjCode;
    return TCL_OK;
}

static int
_xpPlaneDataConfig( Tcl_Interp *ip, XPPlaneData *pData) {
    XPPlaneWdg		*pPlane = pData->PntWdg;
    XGCValues		gcValues;
    unsigned long	gcMask;
    XPPlaneDataHandler	*pDH;

    /*
     * config the data handler 
     */
    if ( (pDH = (XPPlaneDataHandler*) xpGetDataHandler( ip,
      pData->dataSrc, "XPPlaneDataHandler", &XpPlaneSimpleDataHandler)) == NULL)
	return TCL_ERROR;
    if ( _xpPlaneDataSetDataHandler( pData, pDH) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nData handler failed.",NULL);
	return TCL_ERROR;
    }
    if ( pData->notifyCmd && pData->notifyCmd[0] == '\0' ) {
	MEM_FREE( pData->notifyCmd);
	pData->notifyCmd = NULL;
    }
    pData->ext.xfaX = pPlane->xAxis.xfa;
    pData->ext.xfaY = pPlane->yAxis.xfa;

    /*
     * config the normGC
     */
    if ( pData->normGC != None )
	Tk_FreeGC( pData->PntWdg->dpy, pData->normGC);
    gcMask = GCFunction | GCForeground | GCBackground;
    gcValues.function = GXxor;
    gcValues.foreground = pData->normColor->pixel ^ pPlane->bgColor->pixel;
    gcValues.background = pPlane->bgColor->pixel;
    gcMask |= GCLineWidth;
    gcValues.line_width = pData->lineWidth;
    gcMask |= GCFont;
    gcValues.font = pData->powerFont->fid;
    pData->normGC = Tk_GetGC( pPlane->tkwin, gcMask, &gcValues);

    /*
     * config the selGC
     */
    if ( pData->selGC != None )
	Tk_FreeGC( pData->PntWdg->dpy, pData->selGC);
    gcMask = GCFunction | GCForeground | GCBackground;
    gcValues.function = GXxor;
    gcValues.foreground = pData->selColor->pixel ^ pPlane->bgColor->pixel;
    gcValues.background = pPlane->bgColor->pixel;
    gcMask |= GCLineWidth;
    gcValues.line_width = pData->lineWidth;
    gcMask |= GCFont;
    gcValues.font = pData->powerFont->fid;
    pData->selGC = Tk_GetGC( pPlane->tkwin, gcMask, &gcValues);


    return TCL_OK;
}

static int
_xpPlaneWdgConfig( Tcl_Interp *ip, XPPlaneWdg *pPlane, 
  int aC, char **aV, int flags) {
    XGCValues		gcValues;
    unsigned long	gcMask;

    pPlane->flags &= ~XPPlaneWdgF_Valid;
    if (Tk_ConfigureWidget( ip, pPlane->tkwin, _XpPlaneConfigSpecs,
	    aC, aV, (char *) pPlane, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * config the background
     */
    Tk_SetBackgroundFromBorder(pPlane->tkwin, pPlane->bgBorder);

    /* 
     * config the annoGC
     */
    if ( pPlane->annoGC != None )
	Tk_FreeGC( pPlane->dpy, pPlane->annoGC);
    gcMask = GCFunction | GCForeground | GCBackground;
    gcValues.function = GXcopy;
    gcValues.foreground = pPlane->foreground->pixel;
    gcValues.background = pPlane->bgColor->pixel;
    gcMask |= GCFont;
    gcValues.font = pPlane->font->fid;
    pPlane->annoGC = Tk_GetGC( pPlane->tkwin, gcMask, &gcValues);

    /*
     * config the axes
     */
    if ( _xpPlaneAxisConfig( ip, &pPlane->xAxis) != TCL_OK )
	return TCL_ERROR;
    if ( _xpPlaneAxisConfig( ip, &pPlane->yAxis) != TCL_OK )
	return TCL_ERROR;
    

    /*
     * config the data
     */
    if ( _xpPlaneDataConfig( ip, &pPlane->data) != TCL_OK )
	return TCL_ERROR;

    pPlane->flags |= XPPlaneWdgF_Valid;
    _xpPlaneIdleCalc( pPlane, XPPlaneWdgF_CalcExpose|XPPlaneWdgF_CalcVars);

    return TCL_OK;
}

static int
_xpPlaneCmds( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    XPPlaneWdg		*pPlane = (XPPlaneWdg*) cld;
    XPPlaneData		*pData = &pPlane->data;
    char		*cmd = aV[1], c, cd, cl;
    int			r = TCL_OK;
    TOPStrLenType	len;
    int			which;
    char		buf[100];

    if ( aC < 2 ) {
	Tcl_AppendResult(ip, "usage: ", aV[0], " configure|axisxf ?options?",
	  NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) pPlane);
    c = *cmd; len = strlen(cmd);
    cd = len >= 6 && c=='s' ? cmd[5] : '\0';
    cl = len >= 6 && c=='l' ? cmd[5] : '\0';
    /*IF*/ if ( c=='c' && strncmp(cmd,"configure",len)==0 ) {
	if (aC <= 2) {
	    r = Tk_ConfigureInfo(ip, pPlane->tkwin, _XpPlaneConfigSpecs,
		    (char *) pPlane, (char *) NULL, 0);
	} else if (aC == 3) {
	    r = Tk_ConfigureInfo(ip, pPlane->tkwin, _XpPlaneConfigSpecs,
		    (char *) pPlane, aV[2], 0);
	} else {
	    r = _xpPlaneWdgConfig(ip, pPlane, aC-2, aV+2, TK_CONFIG_ARGV_ONLY);
	    if ( r != TCL_OK )
		Tcl_AppendResult(ip,aV[0]," ",aV[1],": bad configuration",NULL);
	}
    } else if ( (c=='a' && strncmp(cmd,"axisxf",len)==0)
             || (c=='c' && strncmp(cmd,"calc",len)==0) ) {
       _xpPlaneIdleCalc( pPlane, XPPlaneWdgF_CalcMap);
    } else if ( c=='u' && strncmp(cmd,"update",len)==0 ) {
       _xpPlaneCalcNow( pPlane);
    } else if ( c=='m' && strncmp(cmd,"mappt",len)==0 ) {
	EEMComplex	wLoc;
	int		vX, vY;
	if ( aC != 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," {wX wY}",NULL);
	    goto error;
	}
	if ( eemCplxStrPrsTcl( ip, aV[2], &wLoc) != TCL_OK ) {
	    goto error;
	}
	vX = xpXfaToView( pPlane->xAxis.xfa, wLoc.r);
	vY = xpXfaToView( pPlane->yAxis.xfa, wLoc.i);
	sprintf( buf, "%d %d", vX, vY);
	Tcl_AppendResult(ip, buf, NULL);
    } else if ( c=='i' && strncmp(cmd,"invmappt",len)==0 ) {
	double	wX, wY;
	int	vX, vY;
	if ( aC != 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," vX vY",NULL);
	    goto error;
	}
	if ( eemInt2PrsTcl( ip, aV[2], &vX, &vY) != TCL_OK ) {
	    goto error;
	}
	wX = xpXfaToWin( pPlane->xAxis.xfa, vX);
	wY = xpXfaToWin( pPlane->yAxis.xfa, vY);
	sprintf( buf, "%g %g", wX, wY);
	Tcl_AppendResult(ip, buf, NULL);
    } else if ( cd=='i' && strncmp(cmd,"silarindex",len)==0 ) {
	if ( aC != 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," which",NULL);
	    goto error;
	}
	if ( _xpPlaneSilarGetIndex( ip, pPlane, aV[2], &which,
	  XPPST_TagAtleast, NULL) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad which ``",
	      aV[2],"''",NULL);
	    goto error;
	}
	Tcl_ResetResult(ip);
	if ( which >= 0 ) {
	    sprintf( buf, "%d", which);
	    Tcl_AppendResult( ip, buf, NULL);
	} else if ( which == XPPS_Tag1 ) {
	    XPPLANE_VSILS_LOOP_BEGIN( pData, silIdx, pPS) {
		if ( TOP_IsOn(pPS->flags,EEMSilarF_Tag1) ) {
		    sprintf( buf, "%d ", silIdx);
		    Tcl_AppendResult( ip, buf, NULL);
		}
	    } XPPLANE_VSILS_LOOP_END();
	}
    } else if ( cd=='i' && strncmp(cmd,"silarinsert",len)==0 ) {
	if ( aC < 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," silar", NULL);
	    goto error;
	}
	if ( _xpPlaneDataCmd( ip, &pPlane->data, -1, '\0',
	  aC-2, aV+2) != TCL_OK ){
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": cmd failed",NULL);
	    goto error;
	}
    } else if ( (cd=='m' && strncmp(cmd,"silarmodify",len)==0)
      ||        (cd=='i' && strncmp(cmd,"silarinfo",len)==0) ) {
	char conjCode;
	if ( aC < 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," which ?options?",
	      NULL);
	    goto error;
	}
	if ( _xpPlaneSilarGetIndex( ip, pPlane, aV[2], &which, 
	  XPPST_ExactlyOne, &conjCode) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad which",NULL);
	    goto error;
	}
	if ( _xpPlaneDataCmd( ip, &pPlane->data, which, conjCode, aC-3, aV+3)
	  != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": cmd failed",NULL);
	    goto error;
	}
    } else if ( cd=='d' && strncmp(cmd,"silardelete",len)==0 ) {
	if ( aC < 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," which ?options?",
	      NULL);
	    goto error;
	}
	if ( _xpPlaneSilarGetIndex( ip, pPlane, aV[2], &which,
	  XPPST_TagAny, NULL) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad which",NULL);
	    goto error;
	}
	if ( _xpPlaneDataCmd( ip, &pPlane->data, which, '\0',
	  -1, NULL) != TCL_OK ){
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": cmd failed",NULL);
	    goto error;
	}
    } else if ( cd=='n' && strncmp(cmd,"silarnotify",len)==0 ) {
	XPPlaneSilar	*pPS;
	TOPLogical	onB;
	if ( aC != 3 && aC != 4 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," which ?boolean?",
	      NULL);
	    goto error;
	}
	if ( _xpPlaneSilarGetIndex( ip, pPlane, aV[2], &which,
	  XPPST_ExactlyOne, NULL) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad which",NULL);
	    goto error;
	}
	pPS = &XPPLANE_VSILS_Get(pData,which);
	if ( aC == 3 ) {
	    sprintf( buf, "%d", TOP_IsOn(pPS->flags,EEMSilarF_Tag2) ? 1 : 0);
	    Tcl_SetResult( ip, buf, TCL_VOLATILE);
	} else {
	    if ( Tcl_GetBoolean( ip, aV[3], &onB) != TCL_OK ) {
	        Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad flag",NULL);
		goto error;
	    }
	    if ( onB )		pPS->flags |= EEMSilarF_Tag2;
	    else		 pPS->flags &= ~EEMSilarF_Tag2;
	}
    } else if ( cd=='b' && strncmp(cmd,"silarbb",len)==0 ) {	/*boundingbox*/
	EEMComplex	bblo, bbhi;
	if ( aC != 3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," which", NULL);
	    goto error;
	}
	if ( _xpPlaneSilarGetIndex( ip, pPlane, aV[2], &which,
	  XPPST_TagAny, NULL) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad which",NULL);
	    goto error;
	}
	_xpPlaneSilarBB( pData, &bblo, &bbhi);
	sprintf( buf, "%g %g %g %g", bblo.r, bblo.i, bbhi.r, bbhi.i);
	Tcl_SetResult( ip, buf, TCL_STATIC);
    } else if ( cl=='d' && strncmp(cmd,"locusdraw",len)==0 ) {
	if ( aC != 4 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",cmd," klo khi", NULL);
	    goto error;
	}
	if ( _xpPlaneDataDrawK( ip, &pPlane->data, aC-2, aV+2) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": failed",NULL);
	    goto error;
	}
    } else if ( cl=='m' && strncmp(cmd,"locusmark",len)==0 ) {
	if ( _xpPlaneDataMarkK( ip, &pPlane->data, aC-2, aV+2) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": failed",NULL);
	    goto error;
	}
    } else {
	Tcl_AppendResult(ip, aV[0], ": bad command ``", aV[1], "''", NULL);
	goto error;
    }
    Tk_Release((ClientData) pPlane);
    return TCL_OK;

error:
    Tk_Release((ClientData) pPlane);
    return TCL_ERROR;
}

int
xpPlaneWdgCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    Tk_Window		appwin = (Tk_Window) cld;
    XPPlaneWdg		*pPlane;
    Tk_Window		myWin;

    if (aC < 2) {
	Tcl_AppendResult(ip, "usage: ", aV[0], " pathName ?options?", NULL);
	return TCL_ERROR;
    }

    myWin = Tk_CreateWindowFromPath(ip, appwin, aV[1], (char *) NULL);
    if (myWin == NULL) {
	return TCL_ERROR;
    }

    pPlane = MEM_ALLOC(XPPlaneWdg);
    MEM_CLEAR(pPlane);
    pPlane->tkwin = myWin;
    pPlane->dpy = Tk_Display(pPlane->tkwin);
    pPlane->winPath = memStrSave(Tk_PathName(pPlane->tkwin));
    pPlane->ip = ip;
    pPlane->flags = XPPlaneWdgF_On;
    pPlane->annoGC = None;
    _xpPlaneAxisConstructor( &pPlane->xAxis, pPlane);
    _xpPlaneAxisConstructor( &pPlane->yAxis, pPlane);
    _xpPlaneDataConstructor( &pPlane->data, pPlane);

    Tk_SetClass( pPlane->tkwin, "XPPlaneWidget");
    Tk_CreateEventHandler( pPlane->tkwin, ExposureMask|StructureNotifyMask,
      _xpPlaneEventCB, (ClientData) pPlane);
    if (_xpPlaneWdgConfig( ip, pPlane, aC-2, aV+2, 0) != TCL_OK) {
	goto error;
    }
    Tcl_CreateCommand( ip, pPlane->winPath, _xpPlaneCmds,
      (ClientData) pPlane, (void (*)()) NULL);
    Tcl_SetVar2( ip, pPlane->winPath, "draw", 
      pPlane->winPath, TCL_GLOBAL_ONLY);
    _xpPlaneDataCalcVars( &pPlane->data);
    Tcl_TraceVar2( ip, pPlane->winPath, NULL, TCL_GLOBAL_ONLY|TCL_TRACE_WRITES,
      _xpPlaneDataTraceVars, (ClientData) &pPlane->data);


    ip->result = Tk_PathName(pPlane->tkwin);
    return TCL_OK;

    error:
    MEM_FREE(pPlane->winPath);
    Tk_DestroyWindow(pPlane->tkwin);
    return TCL_ERROR;
}

int
xpPlaneWdgRegisterCmds( Tcl_Interp *ip, Tk_Window appwin) {
    xpAxisRegisterCmds( ip, appwin);
    xpRubberRegisterCmds( ip, appwin);

    Tcl_CreateCommand( ip, "xpplanewidget", xpPlaneWdgCmd, 
      (ClientData)appwin, NULL);
    return TCL_OK;
}
