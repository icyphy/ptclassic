/**************************************************************************
SCCS Version identification:
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
    tkoctInt.h :: Internal decl's for ged (graphics editor)

    Author: Kennard White
**************************************************************************/


typedef struct _XPGedLayer XPGedLayer;
struct _XPGedLayer {
    octId		layerId;
    TOPMask		flags;
    int			prio;
    XColor*		fgColor;	/* foreground color */
    Pixmap		fillPixmap;	/* stippling fill pixmap */
    GC			gc;		/* base GC */
};
#define XPGedLayerF_Fill	TOP_BIT(0)

struct _XPGedLabelSeg {
    char		*text;
    int			textLen;
};

typedef struct _XPGedFacet XPGedFacet;
struct _XPGedFacet {
    TOPPtrVector	layerList;
};




typedef struct _XPGedMap XPGedMap;
struct _XPGedMap {
    int			octOfs;
    int			pixOfs;
    float		oct2pix;	/* pixUnit/octUnit */
    float		pix2oct;	/* recip of oct2pix */
};

typedef struct _XPGed XPGed;
struct _XPGed {
    /* state info */
    TOPMask		flags;
    Tk_Uid		facetUid;	/* resource -facet */
    Tk_Uid		oldFacetUid;	/* copy of facetUid */
    octId		facetId;	/* octId of facetUid */

    /* the global mapping */
    double		mapScale;	/* resource -scale (pixUnit/octUnit)*/
    XPGedMap		mapX;
    XPGedMap		mapY;
    tr_stack*		tstk;		/* transform stack */

    /* "scan" widget command */
    int			scanTriant;
    float		scanOct2PixX;
    float		scanOct2PixY;
    int			scanOctOfsX;
    int			scanOctOfsY;
    int			scanX;
    int			scanY;

    /* our links back to tcl/tk */
    Tcl_Interp*		ip;		/* our interpreter */
    Tk_Window		tkwin;		/* our window */
    Display*		dpy;		/* dpy of tkwin */
    char*		winPath;	/* strsave of Tk_PathName(tkwin) */

    /* global visuals */
    Cursor		cursor;		/* resource -cursor */
    Tk_3DBorder		bgBorder;	/* resource -background */
    XColor*		bgColor;	/* resource -background */
    XColor*		foreground;	/* resource -foreground */
    XFontStruct*	font;		/* resource -font */

    Tk_Uid		labelFonts;	/* resource -labelfonts */
    Tk_Uid		oldLabelFonts;	/* prev. val of labelFonts */
    TKHFontList*	labelFontList;	/* tkhGetFontList() of labelFonts */

    /* derived visuals */
    GC			normGC;

    Tcl_HashTable	layerIdHash;	/* by object Id */
};

/*
 * XPGed.flags
 */
#define XPGedF_CalcExpose	TOP_BIT(0)
#define XPGedF_CalcMap	TOP_BIT(1)
#define XPGedF_CalcVars	TOP_BIT(2)
#define XPGedF_Calc	(XPGedF_CalcExpose|XPGedF_CalcMap \
				  |XPGedF_CalcVars)
#define XPGedF_IdleCalc	TOP_BIT(8)
#define XPGedF_On		TOP_BIT(9)
#define XPGedF_Valid	TOP_BIT(10)	/* req resources are valid */
#define XPGedF_IgnoreVars	TOP_BIT(11)

/*
 * misc. values
 */
#define XPGED_MIN_FONT_HEIGHT	5

extern char _XpGedSubPackage[];

extern void	_xpGedExpose( XPGed *pGed);

extern void _xpGedMapOctPt( XPGed *pGed, octPoint *pPt);
extern void _xpGedMapPixPt( XPGed *pGed, octPoint *pPt);
extern void _xpGedScaleOctPt( XPGed *pGed, octPoint *pPt);
extern void _xpGedScalePixPt( XPGed *pGed, octPoint *pPt);
