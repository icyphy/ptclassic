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

    XPSeqWdg

    Tk multiple complex sequence widget (for displaying graphs)


********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <tk.h>

#include "topStd.h"
#include "topMisc.h"
#include "topMem.h"
#include "topDlList.h"
#include "topVec.h"
#include "topTcl.h"

#include "eemDflt.h"

#include "xpDefaults.h"
#include "xpXfa.h"
#include "xpRubber.h"
#include "xpAxis.h"
#include "xpSeq.h"

static char _XpSeqWdgSubPackage[] = "XP.SeqWdg";
#define SPKG _XpSeqWdgSubPackage

typedef struct _XPSeqAxis XPSeqAxis;
typedef struct _XPSeqData XPSeqData;
typedef struct _XPSeqWdg XPSeqWdg;

/*************************************************************************
 *			XPSeqAxis
 *
 *	Used to keep track of our associated axis widgets.  There are
 *	a fixed number of possible axis within a given widget, see
 *	{XPSEQ_MAX_AXIS}.  We cache info here about the C interface
 *	to the axis widget (as opposed to the tcl interface).
 *	By convention:
 *	  axisTbl[0] -> domain (bottom)
 *	  axisTbl[1] -> range (left)
 *	  axisTbl[2] -> 2nd range (right)
 *************************************************************************/
#define XPSEQ_MAX_AXIS 3
struct _XPSeqAxis {
    XPSeqWdg*		PntWdg;
    int			index;		/* numerical index of this axis */
    int			flags;
    char*		name;		/* nickname for axis */
    char*		path;		/* full pathname of axis widget */
    XPAxisHandle	axisHandle;	/* token (ptr) for axis */
    XPXfa*		xfa;		/* mapping func */
    double		wLo;		/* min val of lo clip */
    double		wHi;		/* max val of hi clip */

    int			numData;	/* number of SeqData using this axis */
    double		minVal;		/* min data value on this axis */
    double		maxVal;		/* max data value on this axis */
};


static Tk_ConfigSpec _XpSeqAxisConfigSpecs[] = {
    {TK_CONFIG_STRING, "-name", "name", "Name",
	NULL, Tk_Offset(XPSeqAxis, name), 0},
    {TK_CONFIG_STRING, "-path", "path", "Path",
	NULL, Tk_Offset(XPSeqAxis, path), 0},

    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*************************************************************************
 *			XPSeqData
 *
 *	Used to track a given sequence (data set).
 *************************************************************************/

struct _XPSeqData {
    XPSeqWdg*		PntWdg;		/* at creation */
    Tk_Uid		name;		/* at creation */
    Tk_Uid		fullName;	/* creation: wdgName.seqName */
    int			flags;		/* same flags as in SeqWdg */
    int			index;		/* numerical index of this dataseq */
					/* WARNING: changes on add/del */
    char*		dataSrc;	/* resource -datasrc */
    char*		dataSpecOrig;	/* copy of ext.dataSpec */
    char*		axisName;	/* resource -axis */
    XPSeqAxis*		axisPtr;	/* from axisName */
    XPXfa*		XF;		/* from axisPtr */

    /* tcl interface */
    double		wInsValue;	/* for seqaddy */
    double		wInsDelta;	/* for seqaddy */

    /* visuals of line */
    XColor*		fgColor;	/* resource -foreground */
    GC			drawGC;
    char*		lineStyle;	/* resource -linestyle */
    int			lineWidth;	/* resource -linewidth */
    TOPLogical		lineXorB;	/* resource -linexor */
    char*		markStyle;	/* resource -markstyle */
    int			markWidth;	/* resource -markwidth */

    /* legend for line */
    char*		legendLabel;	/* resource -legendlabel */
    Pixmap		legendPixmap;
    Tk_Window		legendPixmapWin;	/* same as wdg.tkwin */
    XRectangle		legendBB;
    XPoint		legendLinePt;
    XPoint		legendLblPt;
    int			legendLblSkew;

    /* extension interface: data handler */
    XPSeqDataExt	ext;
    XPSeqDataHandler	*dh;
};

#define DEF_SEQDATA_FOREGROUND_COLOR	"red"
#define DEF_SEQDATA_FOREGROUND_MONO	BLACK
#define DEF_SEQDATA_LINE_STYLE		"solid"
#define DEF_SEQDATA_LINE_WIDTH		"0"
#define DEF_SEQDATA_LINE_XOR		"0"	/* sparc MIT X buggy */
#define DEF_SEQDATA_MARK_STYLE		"none"
#define DEF_SEQDATA_MARK_WIDTH		"5"
#define DEF_SEQDATA_AXIS		"y"
#define DEF_SEQDATA_RESOL		"2"

static Tk_ConfigSpec _XpSeqDataConfigSpecs[] = {
    {TK_CONFIG_STRING, "-datasrc", "datasrc", "DataSrc",
	NULL, Tk_Offset(XPSeqData, dataSrc), 0},
    {TK_CONFIG_STRING, "-dataspec", "dataspec", "DataSpec",
	NULL, Tk_Offset(XPSeqData, ext.dataSpec), 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_SEQDATA_FOREGROUND_COLOR, Tk_Offset(XPSeqData, fgColor),
	  TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_SEQDATA_FOREGROUND_MONO, Tk_Offset(XPSeqData, fgColor),
	  TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_STRING, "-linestyle", "lineStyle", "LineStyle",
	DEF_SEQDATA_LINE_STYLE, Tk_Offset(XPSeqData, lineStyle), 0},
    {TK_CONFIG_INT, "-linewidth", "lineWidth", "LineWidth",
	DEF_SEQDATA_LINE_WIDTH, Tk_Offset(XPSeqData, lineWidth), 0},
    {TK_CONFIG_BOOLEAN, "-linexor", "lineXor", "LineXor",
	DEF_SEQDATA_LINE_XOR, Tk_Offset(XPSeqData, lineXorB), 0},
    {TK_CONFIG_STRING, "-markstyle", "markStyle", "MarkStyle",
	DEF_SEQDATA_MARK_STYLE, Tk_Offset(XPSeqData, markStyle), 0},
    {TK_CONFIG_INT, "-markwidth", "markWidth", "MarkWidth",
	DEF_SEQDATA_MARK_WIDTH, Tk_Offset(XPSeqData, markWidth), 0},
    {TK_CONFIG_INT, "-resolution", "resolution", "Resolution",
	DEF_SEQDATA_RESOL, Tk_Offset(XPSeqData, ext.vResol), 0},

    {TK_CONFIG_STRING, "-axis", "axis", "Axis",
	DEF_SEQDATA_AXIS, Tk_Offset(XPSeqData, axisName), 0},
    {TK_CONFIG_STRING, "-legendlabel", "legendLabel", "LegendLabel",
	NULL, Tk_Offset(XPSeqData, legendLabel), 0},

    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*************************************************************************
 *			XPSeqWdg
 *
 *	This is the the sequence (graph) widget itself.  Contains
 *	a set of seqAxis and seqData, as well as state info about
 *	whats happening to the widget.
 *************************************************************************/

struct _XPSeqWdg {
    /* state & control */
    int			SN;
    char*		Tag;
    Tcl_Interp*		ip;
    Tk_Window		tkwin;
    Display*		dpy;		/* dpy of tkwin */
    TOPMask		flags;	
    DLListHandle	DataList;	/* of XPSeqData */
    XPSeqAxis		axisTbl[XPSEQ_MAX_AXIS];

    /* visuals & layout */
    Tk_3DBorder         bgBorder;	/* resource -background */
    XColor*		bgColor;	/* resource -background */
    XColor*		fgColor;	/* resource -foreground */
    int			borderWidth;	/* resource -borderWidth */
    int			relief;		/* resource -relief */
    XFontStruct*	legendFont;	/* resource -legendFont */
    GC			legendGC;	/* font & colors */
    int			legendLineLen;	/* resource -legendlinelen */
};


#define XPSeqWdgF_On		TOP_BIT(0)
#define XPSeqWdgF_CalcExpose	TOP_BIT(1)
#define XPSeqWdgF_CalcXf	TOP_BIT(2)
#define XPSeqWdgF_CalcData	TOP_BIT(3)
#define XPSeqWdgF_CalcLegend	TOP_BIT(4)
#define XPSeqWdgF_CalcZoom	TOP_BIT(5)
#define XPSeqWdgF_CalcAR	TOP_BIT(6)
#define XPSeqWdgF_Calc		( XPSeqWdgF_CalcExpose \
				  | XPSeqWdgF_CalcZoom \
				  | XPSeqWdgF_CalcXf|XPSeqWdgF_CalcData \
				  | XPSeqWdgF_CalcLegend \
				  | XPSeqWdgF_CalcAR )
#define XPSeqWdgF_IdleCalc	TOP_BIT(7)
#define XPSeqWdgF_IgnoreTrace	TOP_BIT(8)

#define DEF_SEQ_BG_COLOR	DEF_GRAPH_BACKGROUND
#define DEF_SEQ_BG_MONO		WHITE
#define DEF_SEQ_FG_COLOR	DEF_GRAPH_FOREGROUND
#define DEF_SEQ_FG_MONO		BLACK
#define DEF_SEQ_BORDER_WIDTH	"0"
#define DEF_SEQ_RELIEF		"flat"
#define DEF_SEQ_LEDGENDFONT	"fixed"
#define DEF_SEQ_LEDGENDLINELEN	"20"

static Tk_ConfigSpec _XpSeqWdgConfigSpecs[] = {
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_SEQ_BG_COLOR, Tk_Offset(XPSeqWdg, bgBorder), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, NULL, NULL, NULL,
	NULL, Tk_Offset(XPSeqWdg, bgColor), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_SEQ_BG_MONO, Tk_Offset(XPSeqWdg, bgBorder), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, NULL, NULL, NULL,
	NULL, Tk_Offset(XPSeqWdg, bgColor), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_SEQ_FG_COLOR, Tk_Offset(XPSeqWdg, fgColor), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_SEQ_FG_MONO, Tk_Offset(XPSeqWdg, fgColor), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},

    {TK_CONFIG_INT, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_SEQ_BORDER_WIDTH, Tk_Offset(XPSeqWdg, borderWidth), 0},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_SEQ_RELIEF, Tk_Offset(XPSeqWdg, relief), 0},
    {TK_CONFIG_FONT, "-legendfont", "legendFont", "Font",
	DEF_SEQ_LEDGENDFONT, Tk_Offset(XPSeqWdg, legendFont), 0},
    {TK_CONFIG_INT, "-legendlinelen", "legendLineLen", "LegendLineLen",
	DEF_SEQ_LEDGENDLINELEN, Tk_Offset(XPSeqWdg, legendLineLen), 0},

    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};



static void _xpSeqDataSetTraceVars( XPSeqData *pSeqData);

/*********************************************************************
 *
 *	Legend drawing
 *
 *********************************************************************/

#define LGD_L2L 10	/* legend line to label spacing */
#define LGD_PAD 2	/* internal border to line or label spacing (X&Y) */

static int
_xpSeqDataLegendPixmapDestroy( XPSeqData *pSeqData) {
    XPSeqWdg		*pSeqWdg = pSeqData->PntWdg;
    Pixmap		pix = pSeqData->legendPixmap; 

    if ( pix != None ) {
	if ( Tk_UndefinePixmap( pSeqWdg->ip, Tk_NameOfPixmap(pSeqWdg->dpy,pix),
	  pSeqData->legendPixmapWin) != TCL_OK ) {
	    return TCL_ERROR;
	}
	Tk_FreePixmap( pSeqWdg->dpy, pix);
	pSeqData->legendPixmap = None;
    }
    return TCL_OK;
}

static int
_xpSeqDataLegendCalc( XPSeqData *pSeqData) {
    XPSeqWdg		*pSeqWdg = pSeqData->PntWdg;
    char		*lbl;
    unsigned		bw, markY, fontY, wantX, wantY;
    int			dummy;
    XCharStruct		lblBB;

    _xpSeqDataLegendPixmapDestroy( pSeqData);
    lbl = pSeqData->legendLabel ? pSeqData->legendLabel : pSeqData->name;
    XTextExtents( pSeqWdg->legendFont, lbl, (int)strlen(lbl),
      &dummy, &dummy, &dummy, &lblBB);

    bw = LGD_PAD;
    wantX = pSeqWdg->legendLineLen + LGD_L2L + lblBB.width + 2*bw;
    fontY = pSeqWdg->legendFont->ascent + pSeqWdg->legendFont->descent + 2*bw;
    markY = 2 * pSeqData->markWidth + pSeqData->lineWidth + 2*bw;
    wantY = TOP_MAX(fontY,markY);

    pSeqData->legendBB.x = 0;
    pSeqData->legendBB.y = 0;
    pSeqData->legendBB.width = wantX;
    pSeqData->legendBB.height = wantY;

    pSeqData->legendLinePt.x = bw + pSeqWdg->legendLineLen/2;
    pSeqData->legendLinePt.y = wantY/2;
    pSeqData->legendLblPt.x = bw + pSeqWdg->legendLineLen + LGD_L2L;
    pSeqData->legendLblPt.y = wantY/2;
    pSeqData->legendLblSkew = (lblBB.ascent - lblBB.descent)/2;

    if ( Tk_DefinePixmap( pSeqWdg->ip, pSeqData->fullName, pSeqWdg->tkwin,
      None, None, NULL, wantX, wantY, 0, NULL) != TCL_OK )
	return TCL_ERROR;
    pSeqData->legendPixmapWin = pSeqWdg->tkwin;
#if TK_MAJOR_VERSION == 4
    /* TK4.0b2 has Tk_GetPixmap */
    if ( (pSeqData->legendPixmap = Tk_GetPixmap( pSeqWdg->ip, pSeqWdg->tkwin,
      wantX, wantY, 0)) == None )
	return TCL_ERROR;
#else
    if ( (pSeqData->legendPixmap = Tk_GetPixmap( pSeqWdg->ip, pSeqWdg->tkwin,
      pSeqData->fullName)) == None )
	return TCL_ERROR;
#endif
    return TCL_OK;
}

static void
_xpSeqDataLegendExpose( XPSeqData *pSeqData) {
    XPSeqWdg		*pSeqWdg = pSeqData->PntWdg;
    Tk_Window		tkw = pSeqWdg->tkwin;
    Display		*dpy = Tk_Display( tkw);
    Drawable		win = pSeqData->legendPixmap;
    GC			gc = pSeqData->drawGC;
    int			tx, ty,	lx1, lx2, ly1, ly2, bx, by;
    unsigned		r;
    char		*s, *lbl;

    /* background */
    XSetForeground( dpy, pSeqWdg->legendGC, pSeqWdg->bgColor->pixel); 
    XFillRectangle( dpy, win, pSeqWdg->legendGC, 
      pSeqData->legendBB.x, pSeqData->legendBB.y, 
      (unsigned)pSeqData->legendBB.width, (unsigned)pSeqData->legendBB.height);

    /* text label */
    tx = pSeqData->legendLblPt.x;
    ty = pSeqData->legendLblPt.y + pSeqData->legendLblSkew;
    lbl = pSeqData->legendLabel ? pSeqData->legendLabel : pSeqData->name;
    XSetForeground( dpy, pSeqWdg->legendGC, pSeqWdg->fgColor->pixel); 
    XDrawString( dpy, win, pSeqWdg->legendGC, tx, ty, lbl, (int)strlen(lbl));

    /* line-style */
    lx1 = pSeqData->legendLinePt.x - pSeqWdg->legendLineLen/2;
    lx2 = pSeqData->legendLinePt.x + pSeqWdg->legendLineLen/2;
    ly1 = ly2 = pSeqData->legendLinePt.y;
    s = pSeqData->lineStyle;
    /*IF*/ if ( strcasecmp( s, "none")==0 ) {
	;
    } else if ( strcasecmp( s, "solid")==0 ) {
	XDrawLine( dpy, win, gc, lx1, ly1, lx2, ly2);
    } else if ( strcasecmp( s, "fence")==0 ) {
	XDrawLine( dpy, win, gc, lx1, ly1, lx2, ly2);
    }

    /* mark-styles */
    bx = pSeqData->legendLinePt.x;
    by = pSeqData->legendLinePt.y;
    s = pSeqData->markStyle;
    r = pSeqData->markWidth;
    /*IF*/ if ( strcasecmp( s, "none")==0 ) {
	;
    } else if ( strcasecmp( s, "box")==0 ) {
    	XDrawRectangle( dpy, win, gc, bx-(int)r, by-(int)r, r*2, r*2);
    } else if ( strcasecmp( s, "circle")==0 ) {
    	XDrawArc( dpy, win, gc, bx-(int)r, by-(int)r, r*2, r*2, 0, 360*64);
    } else if ( strcasecmp( s, "dot")==0 ) {
	XFillArc( dpy, win, gc, bx-(int)r, by-(int)r, r*2, r*2, 0, 360*64);
    } else if ( strcasecmp( s, "cross")==0 ) {
	XDrawLine( dpy, win, gc, bx-(int)r, by-(int)r, bx+(int)r, by+(int)r);
	XDrawLine( dpy, win, gc, bx-(int)r, by+(int)r, bx+(int)r, by-(int)r);
    }
}

static void
_xpSeqLegendExpose( XPSeqWdg *pSeqWdg) {
    DLLIST_LOOP_FORW_BEGIN( pSeqWdg->DataList, XPSeqData*, pSeqData) {
	if ( TOP_IsOn(pSeqData->flags,XPSeqWdgF_CalcLegend) ) {
	    _xpSeqDataLegendCalc( pSeqData);
	    _xpSeqDataLegendExpose( pSeqData);
	    pSeqData->flags &= ~XPSeqWdgF_CalcLegend;
	}
    } DLLIST_LOOP_FORW_END();
    Tcl_SetVar2( pSeqWdg->ip, Tk_PathName(pSeqWdg->tkwin), "legends",
      "changed", TCL_GLOBAL_ONLY);
}

/*********************************************************************
 *
 *	Expose & idle calc
 *
 *********************************************************************/

static void
_xpSeqDataExpose( XPSeqData *pSeqData) {
    XPSeqWdg		*pSeqWdg = pSeqData->PntWdg;
    Display		*dpy = Tk_Display( pSeqWdg->tkwin);
    Window		win = Tk_WindowId( pSeqWdg->tkwin);
    GC			gc = pSeqData->drawGC;
    TOPVector		*ptsvec = & pSeqData->ext.vPtsVec;
    int			n;
    unsigned		r;
    char		*s;

    n = TOPVEC_Num( ptsvec);
    TOP_PDBG((SPKG,"%s: expose %s %d %s%d %s%d", pSeqWdg->Tag, pSeqData->name,
      n, pSeqData->lineStyle, pSeqData->lineWidth,
      pSeqData->markStyle, pSeqData->markWidth));

    if ( n == 0 )
	return;

    /* line-styles */
    s = pSeqData->lineStyle;
    if ( strcasecmp( s, "none")==0 ) {
	;
    } else if ( strcasecmp( s, "solid")==0 ) {
	if ( TOPXServerFixB ) {
	    TOPVEC_SUBIDX_LOOP_FORW_BEGIN( ptsvec, 0, n-1, i, XPoint, pXPt) {
	        XDrawLine( dpy, win, gc, 
	          pXPt->x+1, pXPt->y, pXPt[1].x, pXPt[1].y);
	    } TOPVEC_LOOP_FORW_END();
	} else {
	    XDrawLines( dpy, win, gc,
	      TOPVEC_GETARRAY( ptsvec, XPoint), n, CoordModeOrigin);
	}
    } else if ( strcasecmp( s, "fence")==0 ) {
	TOPVEC_LOOP_FORW_BEGIN( ptsvec, XPoint, pXPt) {
	    XDrawLine( dpy, win, gc, 
	      pXPt->x, pSeqData->ext.vZero, pXPt->x, pXPt->y);
	} TOPVEC_LOOP_FORW_END();
    } else {
	topWarnMsg("%s: invalid line style: %s", pSeqWdg->Tag, s);
	pSeqData->lineStyle = Tk_GetUid("none");
    }

    /* mark-styles */
    s = pSeqData->markStyle;
    r = pSeqData->markWidth;
    if ( strcasecmp( s, "none")==0 ) {
	;
    } else if ( strcasecmp( s, "box")==0 ) {
	TOPVEC_LOOP_FORW_BEGIN( ptsvec, XPoint, pXPt) {
	    XDrawRectangle( dpy, win, gc, pXPt->x-(int)r, pXPt->y-(int)r,
	      r*2, r*2);
	} TOPVEC_LOOP_FORW_END();
    } else if ( strcasecmp( s, "circle")==0 ) {
	TOPVEC_LOOP_FORW_BEGIN( ptsvec, XPoint, pXPt) {
	    XDrawArc( dpy, win, gc, 
	      pXPt->x-(int)r, pXPt->y-(int)r, r*2, r*2, 0, 360*64);
	} TOPVEC_LOOP_FORW_END();
    } else if ( strcasecmp( s, "dot")==0 ) {
	TOPVEC_LOOP_FORW_BEGIN( ptsvec, XPoint, pXPt) {
	    XFillArc( dpy, win, gc, 
	      pXPt->x-(int)r, pXPt->y-(int)r, r*2, r*2, 0, 360*64);
	} TOPVEC_LOOP_FORW_END();
    } else if ( strcasecmp( s, "cross")==0 ) {
	TOPVEC_LOOP_FORW_BEGIN( ptsvec, XPoint, pXPt) {
	    XDrawLine( dpy, win, gc, 
	      pXPt->x-(int)r, pXPt->y-(int)r, pXPt->x+(int)r, pXPt->y+(int)r);
	    XDrawLine( dpy, win, gc, 
	      pXPt->x-(int)r, pXPt->y+(int)r, pXPt->x+(int)r, pXPt->y-(int)r);
	} TOPVEC_LOOP_FORW_END();
    } else {
	topWarnMsg("%s: invalid mark style: %s", pSeqWdg->Tag, s);
	pSeqData->markStyle = Tk_GetUid("none");
    }
}

static void
_xpSeqWdgExpose( XPSeqWdg *pSeqWdg) {
    Tk_Window		tkw = pSeqWdg->tkwin;
    int			i;
    XPSeqAxis		*pSeqAxis;

    TOP_PDBG((SPKG,"%s: expose", pSeqWdg->Tag));

    if ( tkw == NULL || !Tk_IsMapped(tkw) )
	return;

    XClearWindow( Tk_Display( tkw), Tk_WindowId( tkw));
    DLLIST_LOOP_FORW_BEGIN( pSeqWdg->DataList, XPSeqData*, pSeqData) {
	_xpSeqDataExpose( pSeqData);
    } DLLIST_LOOP_FORW_END();

    for ( i=0; i < XPSEQ_MAX_AXIS; i++) {
	pSeqAxis = & pSeqWdg->axisTbl[i];
	if ( pSeqAxis->numData == 0 || pSeqAxis->axisHandle == NULL ) continue;
	xpAxisExposeDrawWin( pSeqAxis->axisHandle);
    }
}

/**
    One or more of the axis/transforms changed or was replaced.  Go through
    the axis and refresh stuff.  Then go through data list and
    call the config on affected sequences.
**/
static void
_xpSeqWdgCalcXf( XPSeqWdg *pSeqWdg) {
    int			i, domWidth;
    XPSeqAxis		*pSeqAxis, *pDomAxis, *pRngAxis;
    TOPLogical		domainChangeB, rangeChangeB, doAutoInB, doExposeB;
    TOPMask		zoomMode;

    for ( i=0; i < XPSEQ_MAX_AXIS; i++) {
	pSeqAxis = & pSeqWdg->axisTbl[i];
	pSeqAxis->numData = 0;
    }
    pDomAxis = & pSeqWdg->axisTbl[0];
    domWidth = pDomAxis->xfa->hiView - pDomAxis->xfa->loView;
    domainChangeB = TOP_IsOn(pDomAxis->flags, XPSeqWdgF_CalcXf);
    doAutoInB = TOP_IsOn(pDomAxis->flags,XPSeqWdgF_CalcAR);
    doExposeB = domainChangeB;
    DLLIST_LOOP_FORW_BEGIN( pSeqWdg->DataList, XPSeqData*, pSeqData) {
	if ( pSeqData->axisPtr == NULL || pSeqData->dh == NULL )
	    continue;
	if ( domainChangeB || pSeqData->ext.configReqB ) {
	    pSeqData->ext.xfaX = pDomAxis->xfa;
	    pSeqData->ext.wDomainLo = pDomAxis->wLo;
	    pSeqData->ext.wDomainHi = pDomAxis->wHi;
	    pSeqData->ext.vWidth = TOP_ABS(domWidth);
	    if ( pSeqData->dh && pSeqData->dh->config ) {
		(*(pSeqData->dh->config))( &pSeqData->ext);
	    }
	}
	pRngAxis = pSeqData->axisPtr;
	rangeChangeB =  pSeqData->ext.configReqB || pSeqData->ext.mapReqB;
	if ( rangeChangeB || TOP_IsOn(pRngAxis->flags,XPSeqWdgF_CalcAR) )
	    doAutoInB = TOP_TRUE;
	if ( rangeChangeB || domainChangeB
	  || TOP_IsOn(pRngAxis->flags, XPSeqWdgF_CalcXf) ) {
	    doExposeB = TOP_TRUE;
	    pSeqData->ext.xfaX = pDomAxis->xfa;
	    pSeqData->ext.xfaY = pRngAxis->xfa;
	    if ( pSeqData->dh && pSeqData->dh->map ) {
		(*(pSeqData->dh->map))( &pSeqData->ext);
	    }
	}
	pSeqData->ext.configReqB = TOP_FALSE;
	pSeqData->ext.mapReqB = TOP_FALSE;

	/* min-max on range */
	if ( pRngAxis->numData == 0 || pSeqData->ext.minVal < pRngAxis->minVal )
	    pRngAxis->minVal = pSeqData->ext.minVal;
	if ( pRngAxis->numData == 0 || pSeqData->ext.maxVal > pRngAxis->maxVal )
	    pRngAxis->maxVal = pSeqData->ext.maxVal;
	++(pRngAxis->numData);

	/* min-max on domain */
	if ( pDomAxis->numData == 0 || pSeqData->ext.minDom < pDomAxis->minVal )
	    pDomAxis->minVal = pSeqData->ext.minDom;
	if ( pDomAxis->numData == 0 || pSeqData->ext.maxDom > pDomAxis->maxVal )
	    pDomAxis->maxVal = pSeqData->ext.maxDom;
	++(pDomAxis->numData);
    } DLLIST_LOOP_FORW_END();

    zoomMode = XPAxisZM_Out;
    if ( doAutoInB ) zoomMode |= XPAxisZM_In;
    for ( i=0; i < XPSEQ_MAX_AXIS; i++) {
	pSeqAxis = & pSeqWdg->axisTbl[i];
	pSeqAxis->flags &= ~(XPSeqWdgF_CalcXf|XPSeqWdgF_CalcAR);
	if ( pSeqAxis->numData > 0 ) {
	    xpAxisInformRange( pSeqAxis->axisHandle, 
	      pSeqAxis->minVal, pSeqAxis->maxVal, zoomMode);
	}
    }

    if ( doExposeB )
        pSeqWdg->flags |= XPSeqWdgF_CalcExpose;
}

/**
    Tell all axis to zoom full.  If we return TRUE, then there
    is a recalc pending on one of the axis.  When that recalc
    is completed, the axis will tell us to recalc.
**/
static TOPLogical
_xpSeqWdgCalcZoom( XPSeqWdg *pSeqWdg) {
    int		i;
    XPSeqAxis	*pSeqAxis;
    TOPLogical	pendB = TOP_FALSE;

    for ( i=0; i < XPSEQ_MAX_AXIS; i++) {
	pSeqAxis = & pSeqWdg->axisTbl[i];
	if ( pSeqAxis->numData > 0 ) {
	    xpAxisZoomFull( pSeqAxis->axisHandle);
	    pendB |= xpAxisXformPendingB( pSeqAxis->axisHandle);
	}
    }
    return pendB;
}

static void
_xpSeqWdgIdleCalcCB( ClientData cld) {
    XPSeqWdg *pSeqWdg = (XPSeqWdg *) cld;
    
    pSeqWdg->flags &= ~XPSeqWdgF_IdleCalc;

    TOP_PDBG((SPKG,"%s: idle calc %d", pSeqWdg->Tag, pSeqWdg->flags));
    if ( TOP_IsOn( pSeqWdg->flags, XPSeqWdgF_CalcXf|XPSeqWdgF_CalcData) ) {
	pSeqWdg->flags &= ~(XPSeqWdgF_CalcXf|XPSeqWdgF_CalcData);
        _xpSeqWdgCalcXf( pSeqWdg);
    }
    if ( TOP_IsOn( pSeqWdg->flags, XPSeqWdgF_CalcZoom) ) {
	pSeqWdg->flags &= ~XPSeqWdgF_CalcZoom;
        if ( _xpSeqWdgCalcZoom( pSeqWdg) )
	    return;
    }
    if ( TOP_IsOn( pSeqWdg->flags, XPSeqWdgF_CalcExpose) ) {
	pSeqWdg->flags &= ~XPSeqWdgF_CalcExpose;
        _xpSeqWdgExpose( pSeqWdg);
    }
    if ( TOP_IsOn( pSeqWdg->flags, XPSeqWdgF_CalcLegend) ) {
	pSeqWdg->flags &= ~XPSeqWdgF_CalcLegend;
	_xpSeqLegendExpose( pSeqWdg);
    }
}

static void
_xpSeqWdgIdleCalc( XPSeqWdg *pSeqWdg, int calcMask) {
    pSeqWdg->flags |= (calcMask & XPSeqWdgF_Calc);
    if ( TOP_IsOn( pSeqWdg->flags, XPSeqWdgF_IdleCalc)
      || pSeqWdg->tkwin == NULL 
      || !Tk_IsMapped(pSeqWdg->tkwin) ) {
	return;
    }
    pSeqWdg->flags |= XPSeqWdgF_IdleCalc;
    Tk_DoWhenIdle( _xpSeqWdgIdleCalcCB, (ClientData) pSeqWdg);
}

static void
_xpSeqWdgIdleCalcUndo( XPSeqWdg *pSeqWdg) {
    if ( TOP_IsOn(pSeqWdg->flags, XPSeqWdgF_IdleCalc) ) {
        Tk_CancelIdleCall( _xpSeqWdgIdleCalcCB, (ClientData) pSeqWdg);
	pSeqWdg->flags &= ~XPSeqWdgF_IdleCalc;
    }
}

/*********************************************************************
 *
 *	Tk muck for SeqAxis
 *
 *********************************************************************/

static void
_xpSeqAxisDestructor( XPSeqAxis *pSeqAxis) {
    MEM_FREE(pSeqAxis->name);
    pSeqAxis->name = NULL;
    MEM_FREE(pSeqAxis->path);
    pSeqAxis->path = NULL;
    pSeqAxis->axisHandle = NULL;
}

/**
    This retrieves the "index" in the tcl sense: the index of the
    a particular axis within the seq widget.
**/
static int
_xpSeqAxisGetIndex( Tcl_Interp *ip, XPSeqWdg *pSeqWdg, char *axisIdx,
  XPSeqAxis **ppSeqAxis) {
    int		idxNum, i;
    XPSeqAxis	*pSeqAxis;

    *ppSeqAxis = NULL;

    if ( axisIdx == NULL ) {
	Tcl_AppendResult( ip, "missing axis index", NULL);
	return TCL_ERROR;
    }
    /*IF*/ if ( isdigit(*axisIdx) ) {
	if ( Tcl_GetInt( ip, axisIdx, &idxNum) != TCL_OK
	  || idxNum < 0 || idxNum >= XPSEQ_MAX_AXIS ) {
	    Tcl_AppendResult( ip, ": bad axis index", NULL);
	    return TCL_ERROR;
	}
	*ppSeqAxis = & pSeqWdg->axisTbl[idxNum];	return TCL_OK;
    } else if ( strcasecmp(axisIdx,"x")==0 
      ||        strcasecmp(axisIdx,"domain")==0
      ||        strcasecmp(axisIdx,"bottom")==0 ) {
	*ppSeqAxis = & pSeqWdg->axisTbl[0];		return TCL_OK;
    } else if ( strcasecmp(axisIdx,"y")==0
      ||        strcasecmp(axisIdx,"range")==0
      ||        strcasecmp(axisIdx,"left")==0 ) {
	*ppSeqAxis = & pSeqWdg->axisTbl[1];		return TCL_OK;
    } else if ( strcasecmp(axisIdx,"right")==0 ) {
	*ppSeqAxis = & pSeqWdg->axisTbl[2];		return TCL_OK;
    } else {
	for ( i=0; i < XPSEQ_MAX_AXIS; i++) {
	    pSeqAxis = & pSeqWdg->axisTbl[i];
	    if ( (pSeqAxis->name && strcasecmp(axisIdx, pSeqAxis->name)==0)
	      || (pSeqAxis->path && strcasecmp(axisIdx, pSeqAxis->path)==0) ) {
		*ppSeqAxis = pSeqAxis;
		return TCL_OK;
	    }
	}
    }

    Tcl_AppendResult( ip, "malformed axis index: ", axisIdx, NULL);
    return TCL_ERROR;
}

static void
_xpSeqAxisConfigXf( XPSeqAxis *pSeqAxis, TOPMask calcFlags) {
    XPSeqWdg	*pSeqWdg = pSeqAxis->PntWdg;

    if ( pSeqAxis->axisHandle == NULL )
	return;
    xpAxisGetDomain( pSeqAxis->axisHandle, &pSeqAxis->wLo, &pSeqAxis->wHi);
    pSeqAxis->flags |= XPSeqWdgF_CalcXf|calcFlags;
    _xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcXf);
}

static int
_xpSeqAxisConfig( Tcl_Interp *ip, XPSeqAxis *pSeqAxis, 
  int aC, char **aV, int flags) {
    XPSeqWdg	*pSeqWdg = pSeqAxis->PntWdg;

    if (Tk_ConfigureWidget( ip, pSeqWdg->tkwin, _XpSeqAxisConfigSpecs,
	    aC, aV, (char *) pSeqAxis, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    if ( pSeqAxis->path && pSeqAxis->path[0] == '\0' ) {
	MEM_FREE( pSeqAxis->path);
	pSeqAxis->path = NULL;
    }
    if ( pSeqAxis->path == NULL ) {
	pSeqAxis->axisHandle = NULL;
	pSeqAxis->xfa = NULL;
    } else {
        if ( xpAxisGetHandle( pSeqWdg->ip, pSeqAxis->path, 
	  &pSeqAxis->axisHandle) != TCL_OK ) {
	    Tcl_AppendResult( ip, "\n%s: lookup failed for axis %s %s",
	      pSeqWdg->Tag, pSeqAxis->name, pSeqAxis->path);
	    return TCL_ERROR;
	}
	pSeqAxis->xfa = xpAxisGetXfa( pSeqAxis->axisHandle);
    }

    _xpSeqAxisConfigXf( pSeqAxis, XPSeqWdgF_CalcAR);
    return TCL_OK;
}

/*********************************************************************
 *
 *	Tk muck for SeqData
 *
 *********************************************************************/

#define EXT_OFFSET              Tk_Offset(XPSeqData,ext)
#define SEQ_EXT_TO_DATA(pExt) \
    TOP_Cast(XPSeqData*, TOP_CharPtr(pExt)-EXT_OFFSET)


void
xpSeqDataUpdate( XPSeqDataExt *pExt) {
    XPSeqData		*pSeqData = SEQ_EXT_TO_DATA(pExt);
    XPSeqWdg		*pSeqWdg = pSeqData->PntWdg;

    pSeqData->ext.mapReqB = TOP_TRUE;
    _xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcData);
}

#ifdef notdef
static void
_xpSeqDataLegendEventCB( ClientData cld, XEvent *pEvent) {
    XPSeqData	*pSeqData = (XPSeqData *) cld;
    XPSeqWdg	*pSeqWdg = pSeqData->PntWdg;

    /*IF*/ if ((pEvent->type == Expose) && (pEvent->xexpose.count == 0)) {
	_xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcLegend);
    } else if ( pEvent->type == ConfigureNotify || pEvent->type == MapNotify) {
	_xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcLegend);
    }
}
#endif

static int
_xpSeqDataSetDataHandler( XPSeqData *pSeqData, XPSeqDataHandler *newDH) {
    pSeqData->ext.graphTag = pSeqData->PntWdg->Tag;
    if ( newDH == pSeqData->dh ) {
	if ( pSeqData->ext.dataSpec == NULL && pSeqData->dataSpecOrig == NULL )
	    return TCL_OK;
	if ( pSeqData->ext.dataSpec != NULL && pSeqData->dataSpecOrig != NULL
          && strcmp( pSeqData->ext.dataSpec, pSeqData->dataSpecOrig)==0 )
	    return TCL_OK;
    }
    if ( pSeqData->dh && pSeqData->dh->destructor ) {
	(*(pSeqData->dh->destructor))( &pSeqData->ext);
    }
    pSeqData->dh = NULL;
    MEM_FREE( pSeqData->dataSpecOrig);
    pSeqData->dataSpecOrig = NULL;
    if ( newDH ) {
	if ( newDH->constructor ) {
	    if ( (*(newDH->constructor))( &pSeqData->ext) == TOP_FALSE )
		return TCL_ERROR;
	}
	pSeqData->dh = newDH;
	pSeqData->dataSpecOrig = memStrSave(pSeqData->ext.dataSpec);
    }
    return TCL_OK;
}

static void
_xpSeqDataPrsStyle( char *style, int *pWidth) {
    int		len, width;
    char	*wp;

    if ( style == NULL )
	return;
    len = strlen(style);
    if ( ! isdigit(style[len-1]) )
	return;
    for ( wp=style+len-1; wp > style && isdigit(*(wp-1)); wp--)
	;
    width = atoi(wp);
    if ( width >= 0 )
	*pWidth = width;
    *wp = '\0';
}

static int
_xpSeqDataConfig( Tcl_Interp *ip, XPSeqData *pSeqData) {
    XPSeqWdg		*pSeqWdg = pSeqData->PntWdg;
    XGCValues		gcValues;
    unsigned long	gcMask;
    XPSeqDataHandler	*pDH;

    /* axis */
    if (pSeqData->axisPtr != NULL)
	--(pSeqData->axisPtr->numData);
    if ( pSeqData->axisName == NULL )
	pSeqData->axisName = memStrSave("y");
    if ( _xpSeqAxisGetIndex( ip, pSeqWdg, pSeqData->axisName, 
      &pSeqData->axisPtr) != TCL_OK 
      || pSeqData->axisPtr->index == 0 )
	return TCL_ERROR;
    if (pSeqData->axisPtr != NULL)
	++(pSeqData->axisPtr->numData);

    /* data handler */
    if ( (pDH = (XPSeqDataHandler*) xpGetDataHandler( ip,
      pSeqData->dataSrc, "XPSeqDataHandler", &XpSeqSimpleDataHandler)) == NULL )
	return TCL_ERROR;
    if ( _xpSeqDataSetDataHandler( pSeqData, pDH) != TCL_OK ) {
	Tcl_AppendResult(ip,"\nData handler failed.",NULL);
	return TCL_ERROR;
    }

    /* visuals */
    if ( pSeqData->drawGC != None ) {
	Tk_FreeGC( pSeqData->PntWdg->dpy, pSeqData->drawGC);
	pSeqData->drawGC = None;
    }
    gcMask = GCFunction|GCForeground;
    _xpSeqDataPrsStyle( pSeqData->lineStyle, &pSeqData->lineWidth);
    _xpSeqDataPrsStyle( pSeqData->markStyle, &pSeqData->markWidth);
    if ( pSeqData->lineXorB ) {
        gcValues.function = GXxor;
        gcValues.foreground= pSeqData->fgColor->pixel ^ pSeqWdg->bgColor->pixel;
    } else {
        gcValues.function = GXcopy;
        gcValues.foreground = pSeqData->fgColor->pixel;
    }
    gcMask |= GCLineWidth;
    gcValues.line_width = pSeqData->lineWidth;
    pSeqData->drawGC = Tk_GetGC( pSeqWdg->tkwin, gcMask, &gcValues);
    if ( pSeqData->ext.vResol <= 0 )
	pSeqData->ext.vResol = 1;

#ifdef notdef
    /* legend */
    if ( pSeqData->legendWin != NULL ) {
    	Tk_DeleteEventHandler( pSeqData->legendWin,
	  ExposureMask|StructureNotifyMask,
      	  _xpSeqDataLegendEventCB, (ClientData) pSeqData);
	pSeqData->legendWin = NULL;
    }
    if ( pSeqData->legendPath != NULL ) {
	if ( (pSeqData->legendWin = Tk_NameToWindow( ip, 
	  pSeqData->legendPath, pSeqWdg->tkwin)) == NULL ) {
	    Tcl_AppendResult(ip,"\nNo such legend window: ``",
	      pSeqData->legendWin,"''",NULL);
	    return TCL_ERROR;
	}
        Tk_CreateEventHandler( pSeqData->legendWin,
	  ExposureMask|StructureNotifyMask,
	  _xpSeqDataLegendEventCB, (ClientData) pSeqData);
    }
#endif

    pSeqData->ext.configReqB = TOP_TRUE;
    pSeqData->flags |= XPSeqWdgF_CalcLegend;
    _xpSeqWdgIdleCalc( pSeqWdg, 
      XPSeqWdgF_CalcData|XPSeqWdgF_CalcExpose|XPSeqWdgF_CalcLegend);
    return TCL_OK;
}

static void
_xpSeqDataSetTraceVars( XPSeqData *pSeqData) {
    XPSeqWdg	*pSeqWdg = pSeqData->PntWdg;
    Tcl_Interp	*ip = pSeqWdg->ip;
    char	*n1 = pSeqData->fullName;
    char	buf[100];

    pSeqData->flags |= XPSeqWdgF_IgnoreTrace;

    Tcl_SetVar2(ip, n1, "color", Tk_NameOfColor(pSeqData->fgColor),
      TCL_GLOBAL_ONLY);

    Tcl_SetVar2(ip, n1, "linestyle", pSeqData->lineStyle, TCL_GLOBAL_ONLY);
    sprintf( buf, "%d", pSeqData->lineWidth);
    Tcl_SetVar2(ip, n1, "linewidth", buf, TCL_GLOBAL_ONLY);
    Tcl_SetVar2(ip, n1, "linexor", pSeqData->lineXorB ? "1" : "0",
      TCL_GLOBAL_ONLY);

    Tcl_SetVar2(ip, n1, "markstyle", pSeqData->markStyle, TCL_GLOBAL_ONLY);
    sprintf( buf, "%d", pSeqData->markWidth);
    Tcl_SetVar2(ip, n1, "markwidth", buf, TCL_GLOBAL_ONLY);

    pSeqData->flags &= ~XPSeqWdgF_IgnoreTrace;
}

static char*
_xpSeqDataVarsTraceCB( ClientData cld, Tcl_Interp *ip,
  char *n1, char *n2, int f) {
    XPSeqData	*pSeqData = (XPSeqData*) cld;
    XPSeqWdg	*pSeqWdg = pSeqData->PntWdg;
    char	*val;
    XColor	*pColor;
    int		wid;
    TOPLogical	b;

    if ( TOP_IsOn(pSeqData->flags,XPSeqWdgF_IgnoreTrace) )
	return NULL;
    if ( (val = Tcl_GetVar2( ip, n1, n2, f&TCL_GLOBAL_ONLY)) == NULL ) {
	TOP_PDBG((SPKG,"%s: Couldn't get trace var: ``%s''",pSeqData->name,n2));
	return NULL;
    }

    /*IF*/ if ( strcmp(n2,"color")==0 ) {
	if ( (pColor = Tk_GetColor( ip, pSeqWdg->tkwin, Tk_GetUid(val)))
	  == NULL )
	    goto error;
	if ( pColor == pSeqData->fgColor ) {
	     Tk_FreeColor( pColor);
	     return NULL;
	}
	if ( pSeqData->fgColor != NULL )
	    Tk_FreeColor( pSeqData->fgColor);
	pSeqData->fgColor = pColor;
    } else if ( strcmp(n2,"linestyle")==0 ) {
	if ( strcmp(val, pSeqData->lineStyle)==0 )	return NULL;
	MEM_FREE(pSeqData->lineStyle);
	pSeqData->lineStyle = memStrSave(val);
    } else if ( strcmp(n2,"linewidth")==0 ) {
	wid = atoi(val);
	if ( wid == pSeqData->lineWidth )		return NULL;
	pSeqData->lineWidth = wid;
    } else if ( strcmp(n2,"linexor")==0 ) {
	if ( Tcl_GetBoolean( ip, val, &b) != TCL_OK )	goto error;
	if ( b == pSeqData->lineXorB )			return NULL;
	pSeqData->lineXorB = b;
    } else if ( strcmp(n2,"markstyle")==0 ) {
	if ( strcmp(val, pSeqData->markStyle)==0 )	return NULL;
	MEM_FREE(pSeqData->markStyle);
	pSeqData->markStyle = memStrSave(val);
    } else if ( strcmp(n2,"markwidth")==0 ) {
	wid = atoi(val);
	if ( wid == pSeqData->markWidth )		return NULL;
	pSeqData->markWidth = wid;
    } else {
	TOP_PDBG((SPKG,"%s: Unknown trace var: ``%s''", pSeqData->name, n2));
	return NULL;
    }
    if ( _xpSeqDataConfig( ip, pSeqData) != TCL_OK ) {
	return "Sequence window variable set to bad value (config)";
    }
    return NULL;

error:
    return "Sequence window variable set to bad value (parse)";
}

static int
_xpSeqDataCreate( Tcl_Interp *ip, XPSeqWdg *pSeqWdg, char *seqName,
  int aC, char **aV) {
    XPSeqData	*pSeqData;
    char	*fn;
    char	buf[200];
    int		idx = 0;
    static int	sn = 0;

    ++sn;
    pSeqData = (XPSeqData*) dlListCreateAtTail( pSeqWdg->DataList);
    MEM_CLEAR(pSeqData);
    /* renumber all the sequences */
    DLLIST_LOOP_FORW_BEGIN( pSeqWdg->DataList, XPSeqData*, pSeqData) {
	pSeqData->index = idx++;
    } DLLIST_LOOP_FORW_END();

    if ( seqName == NULL || seqName[0]=='\0' ) {
	sprintf(buf, "seq%d", sn);
	pSeqData->name = Tk_GetUid(buf);
    } else {
	pSeqData->name = Tk_GetUid(seqName);
    }
    fn = memStrSaveFmt("%s.%s", Tk_PathName(pSeqWdg->tkwin), pSeqData->name);
    pSeqData->fullName = Tk_GetUid(fn);
    memFree(fn);

    pSeqData->PntWdg = pSeqWdg;
    pSeqData->drawGC = None;
    pSeqData->legendPixmap = None;
    if (Tk_ConfigureWidget( ip, pSeqWdg->tkwin, _XpSeqDataConfigSpecs,
	    aC, aV, (char *) pSeqData, 0) != TCL_OK) {
	dlListElemDestroy( pSeqWdg->DataList, pSeqData);
	return TCL_ERROR;
    }
    if ( _xpSeqDataConfig( ip, pSeqData) != TCL_OK ) {
	dlListElemDestroy( pSeqWdg->DataList, pSeqData);
	return TCL_ERROR;
    }
    if ( _xpSeqDataLegendCalc( pSeqData) != TCL_OK ) /* do now so can ref */
	return TCL_ERROR;
    topVecConstructor( &pSeqData->ext.vPtsVec, sizeof(XPoint));

    _xpSeqDataSetTraceVars( pSeqData);	/* force it once right now */
    Tcl_TraceVar2(ip, pSeqData->fullName, NULL,
      TCL_GLOBAL_ONLY|TCL_TRACE_WRITES,
      _xpSeqDataVarsTraceCB, (ClientData) pSeqData);

    _xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcData|XPSeqWdgF_CalcZoom);
    return TCL_OK;
}

static void
_xpSeqDataDestructor( XPSeqData *pSeqData) {
    XPSeqWdg	*pSeqWdg = pSeqData->PntWdg;
    Tcl_Interp	*ip = pSeqWdg->ip;

    /* name & fullName are Tk_Uid's */

    Tcl_UntraceVar2(ip, pSeqData->fullName, NULL,
      TCL_GLOBAL_ONLY|TCL_TRACE_WRITES,
      _xpSeqDataVarsTraceCB, (ClientData) pSeqData);
    if ( Tcl_UnsetVar2(ip, pSeqData->fullName, NULL, TCL_GLOBAL_ONLY|TCL_LEAVE_ERR_MSG) != TCL_OK ) {
	topTclBgError( ip, NULL);
    }
    

    /* kill data handler*/
    /* dataSpecOrig is part of datahandler */
    _xpSeqDataSetDataHandler( pSeqData, NULL);
    MEM_FREE(pSeqData->dataSrc);
    pSeqData->dataSrc = NULL;
    topVecDestructor( &pSeqData->ext.vPtsVec);

    MEM_FREE(pSeqData->legendLabel);
    pSeqData->legendLabel = NULL;
    _xpSeqDataLegendPixmapDestroy( pSeqData);

    if ( pSeqData->fgColor != NULL ) {
	Tk_FreeColor( pSeqData->fgColor);
	pSeqData->fgColor = NULL;
    }
    if ( pSeqData->drawGC != None ) {
	Tk_FreeGC( pSeqData->PntWdg->dpy, pSeqData->drawGC);
	pSeqData->drawGC = None;
    }
    MEM_FREE(pSeqData->lineStyle);
    pSeqData->lineStyle = NULL;
    MEM_FREE(pSeqData->markStyle);
    pSeqData->markStyle = NULL;

    if (pSeqData->axisPtr != NULL) {
	--(pSeqData->axisPtr->numData);
    }
}

/**
    This retrieves the "index" in the tcl sense: the index of the
    a particular data set within the seq widget.
**/
static int
_xpSeqDataGetIndex( Tcl_Interp *ip, XPSeqWdg *pSeqWdg, char *seqIdx,
  XPSeqData **ppSeqData) {
    int		totNum, idxNum;

    *ppSeqData = NULL;

    if ( seqIdx == NULL ) {
	Tcl_AppendResult( ip, "missing sequence index", NULL);
	return TCL_ERROR;
    }
    /*IF*/ if ( isdigit(*seqIdx) ) {
	if ( Tcl_GetInt( ip, seqIdx, &idxNum) != TCL_OK ) {
	    Tcl_AppendResult( ip, ": bad sequence index", NULL);
	    return TCL_ERROR;
	}
	if ( (totNum = dlListSize( pSeqWdg->DataList)) == 0 ) {
    	    *ppSeqData = NULL;
	    return TCL_OK;
	}
	if ( idxNum < 0 )	idxNum = 0;
	if ( idxNum >= totNum )	idxNum = totNum-1;
	*ppSeqData = (XPSeqData*) dlListGetNth( pSeqWdg->DataList, idxNum);
	return TCL_OK;
    } else {
	DLLIST_LOOP_FORW_BEGIN( pSeqWdg->DataList, XPSeqData*, pSeqData) {
	    if ( pSeqData->name && strcasecmp(seqIdx, pSeqData->name)==0 ) {
		*ppSeqData = pSeqData;
		return TCL_OK;
	    }
	} DLLIST_LOOP_FORW_END();
    }

    Tcl_AppendResult( ip, "malformed sequence index: ", seqIdx, NULL);
    return TCL_ERROR;
}

/**
    This gets the numerical index of {idxStr} within {pSeqData}.
    The forms are:
	#idx	integer index
	@x	point closest to view coord x,y
	loc	point closest to domain loc {loc}.
**/
static int
_xpSeqDataGetPtIdx( Tcl_Interp *ip, XPSeqData *pSeqData, char *idxStr,
  int *pPtIdx) {
    int		c, vx;
    double	wVal;

    if ( pSeqData->dh == NULL || pSeqData->dh->getIndex == NULL ) {
	Tcl_AppendResult( ip, "getIndex cmd not supported by data handler",
	  NULL);
	return TCL_ERROR;
    }
    c = idxStr[0];
    /*IF*/ if ( c=='#' ) {
	return Tcl_GetInt( ip, idxStr+1, pPtIdx);
    } else if ( c=='@' ) {
	if ( Tcl_GetInt( ip, idxStr+1, &vx) != TCL_OK )
	    return TCL_ERROR;
	wVal = xpXfaToWin( pSeqData->ext.xfaX, vx);
	*pPtIdx = (*(pSeqData->dh->getIndex))( &pSeqData->ext, wVal);
    } else {
	if ( Tcl_GetDouble( ip, idxStr, &wVal) != TCL_OK )
	    return TCL_ERROR;
	*pPtIdx = (*(pSeqData->dh->getIndex))( &pSeqData->ext, wVal);
    }
    return TCL_OK;
}

static int
_xpSeqDataAddXYPts( Tcl_Interp *ip, XPSeqData *pSeqData, 
  TOPLogical xyModeB, int aC, char **aV) {
    XPSeqWdg		*pSeqWdg = pSeqData->PntWdg;
    int			i, r;
    TOPVector		vec;
    TOPDblPoint2	*pPt;

    if ( pSeqData->dh == NULL || pSeqData->dh->insertPts == NULL ) {
	Tcl_AppendResult( ip, "InsertPts cmd not supported by data handler",
	  NULL);
	return TCL_ERROR;
    }
    topVecConstructor( &vec, sizeof(TOPDblPoint2));
    for (i=0; i < aC; i++) {
	pPt = & TOPVEC_Append( &vec, TOPDblPoint2);
	if ( xyModeB ) {
	    if ( topPrsDblTcl( ip, aV[i], &pPt->x) )
		goto error;
	    if ( ++i >= aC ) {
		Tcl_AppendResult( ip, "Should be even number of arguments");
		goto error;
	    }
	} else {
	    pPt->x = pSeqData->wInsValue;
	    pSeqData->wInsValue += pSeqData->wInsDelta;
	}
	if ( topPrsDblTcl( ip, aV[i], &pPt->y) )
	    goto error;
    }
    r = (*(pSeqData->dh->insertPts))( &pSeqData->ext, 
      TOPVEC_Array(&vec, TOPDblPoint2), TOPVEC_Num(&vec));
    if ( ! r ) {
	Tcl_AppendResult( ip, "Data Handler insert failed.", NULL);
	goto error;
    }
    if ( pSeqData->ext.configReqB )
	_xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcXf);
    topVecDestructor( &vec);
    return TCL_OK;

error:
    topVecDestructor( &vec);
    return TCL_ERROR;
}

/**
    This is a helper function for _xpSeqWdgCmds, NOT a stand-alone
    tcl command implementation.  We can assume aC >= 2, and
    that aV[1] starts with "seq".
**/
static int
_xpSeqDataCmds( Tcl_Interp *ip, XPSeqWdg *pSeqWdg, int aC, char **aV) {
    char	*cmd, buf[100];
    int		c, result = TCL_OK, ptIdx;
    TOPStrLenType len;
    XPSeqData	*pSeqData;
    TOPDblPoint2	ptVal;

    cmd = aV[1]; len = strlen(cmd);
    if ( aC < 2 || len < 4 ) {
    	Tcl_AppendResult(ip, "usage (sequence commands): ",
	  aV[0], " seqcreate|seqconfigure|seqdestroy|seqaddxy|seqaddy|seqxform",
	  " ?options?", NULL);
	return TCL_ERROR;
    }
    cmd+=3; len-=3; c = cmd[0];

    pSeqData = NULL;
    if ( aC >= 3 ) {
	_xpSeqDataGetIndex( ip, pSeqWdg, aV[2], &pSeqData);
    }

    /*IF*/ if ( c=='c' && len>2 && strncmp(cmd, "create",len)==0 ) {
	Tcl_ResetResult(ip); /* reset error from GetIndex */
	if ( aC <= 2 ) {
	    Tcl_AppendResult(ip, "usage: ", aV[0], " ", aV[1], 
	      "seqName ?options?", NULL);
	    return TCL_ERROR;
	}
	if ( pSeqData != NULL ) {
	    Tcl_AppendResult(ip,aV[0]," ",aV[1],": sequence already exists: ``",
	      aV[2],"''",NULL);
	    return TCL_ERROR;
	}
	if ( _xpSeqDataCreate(ip, pSeqWdg, aV[2], aC-3, aV+3) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": create failed",NULL);
	    return TCL_ERROR;
	}
    } else if ( c=='i' && strncmp(cmd, "index",len)==0 ) {
	if ( pSeqData == NULL ) {
	    Tcl_SetResult(ip,"none",TCL_STATIC);
	} else {
	    sprintf(buf,"%d",pSeqData->index);
	    Tcl_SetResult(ip,buf,TCL_VOLATILE);
	}
    } else if ( c=='c' && len>2 && strncmp(cmd, "configure",len)==0 ) {
	if ( pSeqData == NULL ) {
	    Tcl_AppendResult(ip, "\nusage: ", aV[0], " ", aV[1], 
	      "seqIdx ?options?", NULL);
	    return TCL_ERROR;
	}
	if (aC <= 3) {
	    result = Tk_ConfigureInfo(ip, pSeqWdg->tkwin, _XpSeqDataConfigSpecs,
		    (char *) pSeqData, (char *) NULL, 0);
	} else if (aC == 4) {
	    result = Tk_ConfigureInfo(ip, pSeqWdg->tkwin, _XpSeqDataConfigSpecs,
		    (char *) pSeqData, aV[3], 0);
	} else {
	    if (Tk_ConfigureWidget( ip, pSeqWdg->tkwin, _XpSeqDataConfigSpecs,
	      aC-3, aV+3, (char *) pSeqData, TK_CONFIG_ARGV_ONLY) != TCL_OK) {
		return TCL_ERROR;
	    }
	    if ( _xpSeqDataConfig(ip, pSeqData) != TCL_OK )
		return TCL_ERROR;
	    _xpSeqDataSetTraceVars( pSeqData);
	}
    } else if ( (c=='d' && strncmp(cmd,"destroy",len)==0)
             || (c=='d' && strncmp(cmd,"delete",len)==0) ) {
	if ( aC != 3 || pSeqData == NULL ) {
	    Tcl_AppendResult(ip,"\nusage: ",aV[0]," ",aV[1]," seqIdx",NULL);
	    return TCL_ERROR;
	}
	_xpSeqDataDestructor( pSeqData);
	dlListElemDestroy( pSeqWdg->DataList, pSeqData);
        _xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcExpose|XPSeqWdgF_CalcLegend);
    } else if ( c=='p' && strcmp(cmd,"ptindex")==0 ) {
	if ( aC!=4 || pSeqData == NULL ) {
	    Tcl_AppendResult(ip,"\nusage: ",aV[0]," ",aV[1], 
	      " seqIdx pointIdx", NULL);
	    return TCL_ERROR;
	}
	    if ( _xpSeqDataGetPtIdx( ip, pSeqData, aV[3], &ptIdx) != TCL_OK ) {
		Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad point index", NULL);
		return TCL_ERROR;
	    }
	    sprintf( buf, "%d", ptIdx);
	    Tcl_SetResult(ip, buf, TCL_VOLATILE);
	} else if ( c=='v' && strcmp(cmd, "value")==0 ) {
	    if ( aC < 4 || pSeqData == NULL ) {
		Tcl_AppendResult(ip,"\nusage: ",aV[0]," ",aV[1], 
		  " seqIdx pointIdx ?value? ", NULL);
		return TCL_ERROR;
	    }
	    if ( _xpSeqDataGetPtIdx( ip, pSeqData, aV[3], &ptIdx) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad point index", NULL);
	    return TCL_ERROR;
	}
	if ( aC == 4 ) {
	    if ( pSeqData->dh == NULL || pSeqData->dh->getPts == NULL ) {
	        Tcl_AppendResult(ip,aV[0]," ",aV[1],
		  ": getPts not supported by data handler", NULL);
	        return TCL_ERROR;
	    }
	    if ( !(*(pSeqData->dh->getPts))( &pSeqData->ext, ptIdx, &ptVal, 1)){
	        Tcl_AppendResult(ip,aV[0]," ",aV[1],": DH getPts failed",NULL);
	        return TCL_ERROR;
	    }
	    eemDfltFmtTcl( ip, ptVal.y, TOP_FALSE);
	} else {
	    if ( pSeqData->dh == NULL || pSeqData->dh->setPts == NULL ) {
	        Tcl_AppendResult(ip,aV[0]," ",aV[1],
		  ": setPts not supported by data handler", NULL);
	        return TCL_ERROR;
	    }
	    if ( Tcl_GetDouble(ip, aV[4], &ptVal.y) != TCL_OK ) {
	        Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": bad point value", NULL);
	        return TCL_ERROR;
	    }
	    if (!(*(pSeqData->dh->setPts))( &pSeqData->ext, ptIdx, &ptVal, 1)){
	        Tcl_AppendResult(ip,aV[0]," ",aV[1],": DH setPts failed",NULL);
	        return TCL_ERROR;
	    }
    	    pSeqData->ext.mapReqB = TOP_TRUE;
            _xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcData);
	}
    } else if ( c=='a' && strcmp(cmd, "addy")==0 ) {
	if ( aC < 4 || pSeqData == NULL ) {
	    Tcl_AppendResult(ip,"\nusage: ",aV[0]," ",aV[1], 
	      " seqIdx xy-point ... ", NULL);
	    return TCL_ERROR;
	}
	result = _xpSeqDataAddXYPts( ip, pSeqData, TOP_FALSE, aC-3, aV+3);
    } else if ( c=='x' && strncmp(cmd,"xform",len)==0 ) {
	if ( aC != 3 || pSeqData == NULL ) {
	    Tcl_AppendResult(ip,"\nusage: ",aV[0]," ",aV[1]," seqIdx", NULL);
	    return TCL_ERROR;
	}
	pSeqData->ext.configReqB = TOP_TRUE;
        _xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcXf);
    } else {
	Tcl_AppendResult(ip, aV[0], ": bad sequence command: ", aV[1], NULL);
	return TCL_ERROR;
    }
    return result;
}


/*********************************************************************
 *
 *	Tk muck for SeqWdg
 *
 *********************************************************************/

static int
_xpSeqGetPtIndex( Tcl_Interp *ip, XPSeqWdg *pSeqWdg, char *idxStr,
  XPSeqData **ppSeqData, int *pPtIdx) {
    char	*ix = idxStr;
    char	c = idxStr[0];
    int		xval, yval, dist, mindist = TOP_NORMSQR(20,20), minxval = 0;
    XPSeqData	*pSeqData = NULL;
    double	wVal;

    /*IF*/ if ( c=='@' ) {
	xval = strtol( ix+1, &ix, 10);
	if ( *ix != ',' ) {
	    Tcl_AppendResult(ip,"malformed ptidx: ``",idxStr,"''",NULL);
	    return TCL_ERROR;
	}
	yval = strtol( ix+1, &ix, 10);
	DLLIST_LOOP_FORW_BEGIN( pSeqWdg->DataList, XPSeqData*, pSD) {
	    TOPVEC_IDX_LOOP_FORW_BEGIN( &pSD->ext.vPtsVec, vIdx, XPoint, pXPt) {
		dist = TOP_NORMSQR(xval-pXPt->x,yval-pXPt->y);
		if ( dist < mindist ) {
		    pSeqData = pSD;
		    minxval = pXPt->x;
		    mindist = dist;
		}
	    } TOPVEC_LOOP_FORW_END();
	} DLLIST_LOOP_FORW_END();

    } else {
	Tcl_AppendResult(ip,"bad ptindex string: ``",idxStr,"''",NULL);
	return TCL_ERROR;
    }
    *ppSeqData = pSeqData;
    if ( pSeqData == NULL )
	return TCL_OK;
    if ( pSeqData->dh == NULL || pSeqData->dh->getIndex == NULL ) {
	Tcl_AppendResult( ip, "DH getIndex cmd not supported", NULL);
	return TCL_ERROR;
    }
    wVal = xpXfaToWin( pSeqData->ext.xfaX, minxval);
    *pPtIdx = (*(pSeqData->dh->getIndex))( &pSeqData->ext, wVal);
    return TCL_OK;
}

static void
_xpSeqDataDestructorCB( DLListHandle dataList, TOPPtr elem, TOPPtr userData) {
    XPSeqData	*pSeqData = (XPSeqData*) elem;
    _xpSeqDataDestructor( pSeqData);
}

static void
_xpSeqWdgDestroyCB( ClientData cld) {
    XPSeqWdg	*pSeqWdg = (XPSeqWdg *) cld;
    int		i;

    Tcl_DeleteCommand( pSeqWdg->ip, pSeqWdg->Tag);

    if (pSeqWdg->DataList != NULL) {
	dlListDestroy( pSeqWdg->DataList, _xpSeqDataDestructorCB, NULL);
	pSeqWdg->DataList = NULL;
    }
    for (i=0; i < XPSEQ_MAX_AXIS; i++) {
        _xpSeqAxisDestructor( &pSeqWdg->axisTbl[i]);
    }

    /*
     * even though we dont use this var, we unset it so that tk UI procs
     * can trace off if it
     */
    Tcl_UnsetVar2( pSeqWdg->ip, pSeqWdg->Tag, NULL,
      TCL_GLOBAL_ONLY);

    if (pSeqWdg->bgColor != NULL) {
	Tk_FreeColor(pSeqWdg->bgColor);
	pSeqWdg->bgColor = NULL;
    }
    if (pSeqWdg->fgColor != NULL) {
	Tk_FreeColor(pSeqWdg->fgColor);
	pSeqWdg->fgColor = NULL;
    }
    if (pSeqWdg->bgBorder != NULL) {
	Tk_Free3DBorder(pSeqWdg->bgBorder);
	pSeqWdg->bgBorder = NULL;
    }
    if (pSeqWdg->legendFont != NULL) {
	Tk_FreeFontStruct(pSeqWdg->legendFont);
	pSeqWdg->legendFont = NULL;
    }
    if (pSeqWdg->legendGC != None) {
	Tk_FreeGC( pSeqWdg->dpy, pSeqWdg->legendGC);
	pSeqWdg->legendGC = None;
    }
    MEM_FREE(pSeqWdg->Tag);
    pSeqWdg->Tag = NULL;

    MEM_FREE(pSeqWdg);
}


static void
_xpSeqWdgEventCB( ClientData cld, XEvent *pEvent) {
    XPSeqWdg *pSeqWdg = (XPSeqWdg *) cld;

    /*IF*/ if ((pEvent->type == Expose) && (pEvent->xexpose.count == 0)) {
	_xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcExpose);
    } else if ( pEvent->type == DestroyNotify ) {
	Tcl_DeleteCommand( pSeqWdg->ip, Tk_PathName(pSeqWdg->tkwin));
	pSeqWdg->tkwin = NULL;
	_xpSeqWdgIdleCalcUndo( pSeqWdg);
	Tk_EventuallyFree( (ClientData) pSeqWdg, _xpSeqWdgDestroyCB);
    } else if ( pEvent->type == ConfigureNotify || pEvent->type == MapNotify) {
	TOP_PDBG((SPKG,"%s: config", pSeqWdg->Tag));
	/* XXX
	_xpSeqWdgGeomCheck( pSeqWdg);
	_xpSeqWdgIdleCalc( pSeqWdg, 0);
	*/
    }
}


/**
    Update resources.  We dont know what will change,
    so we must recompute everything.  Some things we do immediately,
    and other things we defer using the IdleCalc.  For resources which
    we dont expect to change again soon, we handle now.
**/
static int
_xpSeqWdgConfig( Tcl_Interp *ip, XPSeqWdg *pSeqWdg, 
  int aC, char **aV, int flags) {
    XGCValues		gcValues;
    unsigned long	gcMask;

    if (Tk_ConfigureWidget( ip, pSeqWdg->tkwin, _XpSeqWdgConfigSpecs,
	    aC, aV, (char *) pSeqWdg, flags) != TCL_OK) {
	return TCL_ERROR;
    }
    Tk_SetBackgroundFromBorder(pSeqWdg->tkwin, pSeqWdg->bgBorder);

    if ( pSeqWdg->legendGC != None )
	Tk_FreeGC( pSeqWdg->dpy, pSeqWdg->legendGC);
    gcValues.background = pSeqWdg->bgColor->pixel;
    gcValues.foreground = pSeqWdg->fgColor->pixel;
    gcValues.font = pSeqWdg->legendFont->fid;
    gcMask = GCForeground|GCBackground|GCFont;
    pSeqWdg->legendGC = Tk_GetGC(pSeqWdg->tkwin, gcMask, &gcValues);

    _xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcExpose|XPSeqWdgF_CalcLegend);

    return TCL_OK;
}

static int
_xpSeqWdgCmds( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    XPSeqWdg	*pSeqWdg = (XPSeqWdg*) cld;
    int		result = TCL_OK;
    TOPStrLenType	len;
    char	c, *cmd;
    XPSeqData	*pSeqData;
    int		ptIdx;
    XPSeqAxis	*pSeqAxis = NULL;
    char	*sV[10], buf1[20], buf2[20], *ainfo;
    int		i;

    if (aC < 2) {
	Tcl_AppendResult(ip, "wrong # args: should be \"",
		aV[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) pSeqWdg);

    cmd = aV[1]; len = strlen(cmd); c = cmd[0];
    /*IF*/ if ( c=='c' && strncmp(cmd, "configure",len)==0 ) {
	if (aC <= 2) {
	    result = Tk_ConfigureInfo(ip, pSeqWdg->tkwin, _XpSeqWdgConfigSpecs,
		    (char *) pSeqWdg, (char *) NULL, 0);
	} else if (aC == 3) {
	    result = Tk_ConfigureInfo(ip, pSeqWdg->tkwin, _XpSeqWdgConfigSpecs,
		    (char *) pSeqWdg, aV[2], 0);
	} else {
	    result = _xpSeqWdgConfig(ip, pSeqWdg, aC-2, aV+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ( c=='p' && strncmp(cmd,"ptindex",len)==0 ) {
	if ( aC!=3 ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",aV[1]," @x,y",NULL);
	    return TCL_ERROR;
	}
	if ( _xpSeqGetPtIndex( ip, pSeqWdg, aV[2], &pSeqData, &ptIdx)!=TCL_OK ){
	    Tcl_AppendResult(ip,"\n",aV[0]," ",aV[1],": failed",NULL);
	    return TCL_ERROR;
	}
	Tcl_ResetResult(ip);
	if ( pSeqData != NULL ) {
	    sprintf( ip->result, "%d %d", pSeqData->index, ptIdx);
	}
    } else if ( c=='a' && len>=5 && strncmp(cmd,"axislist",len)==0 ) {
        for ( i=0; i < XPSEQ_MAX_AXIS; i++) {
	    pSeqAxis = & pSeqWdg->axisTbl[i];
	    if ( pSeqAxis->path == NULL || pSeqAxis->path[0] == '\0' )
		continue;
	    sprintf( buf1, "%d", i);
	    sV[0] = buf1;
	    sV[1] = pSeqAxis->name ? pSeqAxis->name : "";
	    sV[2] = pSeqAxis->path ? pSeqAxis->path : "";
	    sprintf( buf2, "%d", pSeqAxis->numData);
	    sV[3] = buf2;
	    ainfo = Tcl_Merge( 4, sV);
	    Tcl_AppendElement(ip, ainfo);
	    MEM_FREE( ainfo);
	}
    } else if ( c=='a' && len>=5 && strncmp(cmd,"axisconfigure",len)==0 ) {
	if ( aC < 3
	  || _xpSeqAxisGetIndex( ip, pSeqWdg, aV[2], &pSeqAxis) != TCL_OK ) {
	    Tcl_AppendResult(ip, "usage: ", aV[0], " ", aV[1], 
	      " axisIdx ?options?", NULL);
	    goto error;
	}
	if (aC <= 3) {
	    result = Tk_ConfigureInfo(ip, pSeqWdg->tkwin, _XpSeqAxisConfigSpecs,
		    (char *) pSeqAxis, (char *) NULL, 0);
	} else if (aC == 4) {
	    result = Tk_ConfigureInfo(ip, pSeqWdg->tkwin, _XpSeqAxisConfigSpecs,
		    (char *) pSeqAxis, aV[3], 0);
	} else {
	    result = _xpSeqAxisConfig(ip, pSeqAxis, aC-3, aV+3,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ( c=='a' && len>=5 && strncmp(cmd,"axisxf",len)==0 ) {
	int	geomB = 0;
	/* axis transform changed ... re-evaluate */
	if ( (aC != 3 && aC != 4)
	  || _xpSeqAxisGetIndex( ip, pSeqWdg, aV[2], &pSeqAxis) != TCL_OK 
	  || (aC==4 && Tcl_GetInt(ip, aV[3], &geomB)!=TCL_OK) ) {
	    Tcl_AppendResult(ip,"usage: ",aV[0]," ",cmd,
	      " axisIdx ?geomB?",NULL);
	    goto error;
	}
	_xpSeqAxisConfigXf( pSeqAxis, geomB ? 0 : XPSeqWdgF_CalcAR);
    } else if ( c=='f' && len>=1 && strncmp(cmd,"full",len)==0 ) {
        _xpSeqWdgIdleCalc( pSeqWdg, XPSeqWdgF_CalcZoom);
    } else if ( c=='s' && len>=3 && strncmp(cmd,"seq",3)==0 ) {
	result = _xpSeqDataCmds( ip, pSeqWdg, aC, aV);
    } else {
	Tcl_AppendResult(ip, aV[0], ": bad command: ",cmd,NULL);
	goto error;
    }
    Tk_Release((ClientData) pSeqWdg);
    return result;

    error:
    Tk_Release((ClientData) pSeqWdg);
    return TCL_ERROR;
}



int
xpSeqWdgCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    Tk_Window		appwin = (Tk_Window) cld;
    XPSeqWdg		*pSeqWdg;
    Tk_Window		myWin;
    int			i;

    if (aC < 2) {
	Tcl_AppendResult(ip, "wrong # args: should be \"",
	  aV[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    myWin = Tk_CreateWindowFromPath(ip, appwin, aV[1], (char *) NULL);
    if (myWin == NULL) {
	return TCL_ERROR;
    }

    pSeqWdg = MEM_ALLOC(XPSeqWdg);
    MEM_CLEAR(pSeqWdg);
    pSeqWdg->tkwin = myWin;
    pSeqWdg->dpy = Tk_Display(pSeqWdg->tkwin);
    pSeqWdg->Tag = memStrSave(Tk_PathName(pSeqWdg->tkwin));
    pSeqWdg->ip = ip;
    pSeqWdg->flags = XPSeqWdgF_On;

    Tk_SetClass( pSeqWdg->tkwin, "XPSeqWidget");
    Tk_CreateEventHandler( pSeqWdg->tkwin, ExposureMask|StructureNotifyMask,
	    _xpSeqWdgEventCB, (ClientData) pSeqWdg);
    Tcl_CreateCommand( ip, pSeqWdg->Tag, _xpSeqWdgCmds,
    	    (ClientData) pSeqWdg, (void (*)()) NULL);
    if (_xpSeqWdgConfig( ip, pSeqWdg, aC-2, aV+2, 0) != TCL_OK) {
	goto error;
    }

    /*
     * Init seq_data and seq_axis
     */
    pSeqWdg->DataList = dlListCreate( "XPSeqData", sizeof(XPSeqData));
    /* axistbl already zeroed by MEM_CLEAR of seqwdg */
    for ( i=0; i < XPSEQ_MAX_AXIS; i++) {
	pSeqWdg->axisTbl[i].PntWdg = pSeqWdg;
	pSeqWdg->axisTbl[i].index = i;
    }

    ip->result = Tk_PathName(pSeqWdg->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(pSeqWdg->tkwin);
    return TCL_ERROR;
}

#ifdef notdef
XPSeqWdg*
xpGetSeqWdgByName( Tcl_Interp *ip, char *name) {
}

XPSeqData*
xpGetSeqDataByName( XPSeqWdg *pSeqWdg, char *name) {
}
#endif

void
xpSeqWdgRegisterCmds( Tcl_Interp *ip, Tk_Window appwin) {
    /* force our hacked version to be linked */
#ifdef notdef
    tkBitmap_Pixmap1();
    tkButton_Pixmap1();
    tkMenubutton_Pixmap1();
    tkMenu_Pixmap1();
#endif

    xpAxisRegisterCmds( ip, appwin);
    xpRubberRegisterCmds( ip, appwin);
    Tcl_CreateCommand( ip, "xpseqwidget", 
      xpSeqWdgCmd, (ClientData)appwin, NULL);
}
