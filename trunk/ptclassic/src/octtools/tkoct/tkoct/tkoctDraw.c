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

    tkoctDraw.c :: Primitive X11 drawing for GED (graphics editor) widget

    This file contains the core drawing functions of the ged widget.

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"

#include <tk.h>
#include <X11/Xutil.h>

#include "oct.h"
#include "oh.h"
#include "errtrap.h"
#include "tr.h"
#include "tap.h"

#include "topStd.h"
#include "topMsg.h"
#include "topMem.h"
#include "topVec.h"
#include "topTcl.h"

#include "toct.h"
#include "tkh.h"

#include "xpDefaults.h"
#include "tkoctInt.h"
#include "tkoct.h"

#define SPKG _XpGedSubPackage

static char _XpGedDrawPackage[] = "xp.ged.draw";
#define SPKG_DRAW _XpGedDrawPackage

static Pixmap
_xpGedCvtTapFillStr( XPGed *pGed, int fillW, int fillH, char *fillBits) {
    Display	*dpy = Tk_Display(pGed->tkwin);
    XImage*	im;
    Pixmap	pm;
    unsigned	dataBytes, row, col;
    int		c;
    char	*data, *s;


    dataBytes = ((fillW+7)/8) * fillH;
    data = (char*) memMalloc(dataBytes);
    im = XCreateImage( dpy, Tk_Visual(pGed->tkwin), 1, XYPixmap, 0,
      data, (unsigned)fillW, (unsigned)fillH, 8, 0);
    s = fillBits;
    for (row=0; row < fillH; row++) {
        for (col=0; col < fillW; col++) {
	    while ( (c=*s)==' ' || c=='\n' || c=='\t' )	s++;
	    if ( c == '\0' ) {
		memFree(data);
		XDestroyImage(im);
		return None;
	    }

	    XPutPixel(im, (int)col, (int)row, 
	      c!='0' ? (unsigned)~0L : (unsigned)0L);
	}
    }
    pm = XCreateBitmapFromData( dpy, Tk_WindowId(pGed->tkwin), data,
      (unsigned)fillW, (unsigned)fillH);
    XDestroyImage(im);
    memFree(data);
    return pm;
}

static GC
_xpGedAllocLayerGC( XPGed *pGed, XPGedLayer *pLI) {
    octObject		layer;
    int			pri, nColors, fillFlags, borderFlags;
    XColor		fgc;
    XGCValues		gcValues;
    unsigned long	gcMask = 0;
    static TOPLogical	tapInitB = TOP_FALSE;

    if ( tapInitB == TOP_FALSE ) {
    	tapSetDisplayType(NULL, TAP_COLOR);
	tapInitB = TOP_TRUE;
    }
    OH_FAIL(ohGetById(&layer,pLI->layerId),"get layer by id",NULL);
    tapGetDisplayInfo( &layer, &pri, &nColors, &fillFlags, &borderFlags);

    pLI->prio = pri;

    if ( nColors >= 1 ) {
        tapGetDisplayColor( &layer, 0, &fgc.red, &fgc.green, &fgc.blue);
#if TK_MAJOR_VERSION == 4
	/* tk4.0b2/changes says:
	12/6/94 (cleanup) Eliminated "interp" argument to Tk_GetColorByValue,
	since it is no longer needed.
	*** POTENTIAL INCOMPATIBILITY ***
	*/
	pLI->fgColor = Tk_GetColorByValue( pGed->tkwin, None, &fgc);
#else
	pLI->fgColor = Tk_GetColorByValue( pGed->ip, pGed->tkwin, None,
	  &fgc);
#endif
	gcMask |= GCFunction | GCForeground;
	gcValues.function = GXcopy;
	gcValues.foreground = pLI->fgColor->pixel;
    }

    gcMask |= GCFillRule;
    gcValues.fill_rule = WindingRule;
    if ( fillFlags == TAP_SOLID ) {
	pLI->flags |= XPGedLayerF_Fill;
    } else if ( fillFlags == TAP_STIPPLED ) {
	int	fillW, fillH;
	char	*fillBits;
	tapGetDisplayPattern(&layer, TAP_FILL_PATTERN, 
	  &fillW, &fillH, &fillBits);
	if ( fillW==1 && fillH==1 ) {
	    if ( *fillBits=='0' ) {
		/* dont fill */
	    } else {
		/* fill solid */
		pLI->flags |= XPGedLayerF_Fill;
	    }
	} else {
	    pLI->fillPixmap = _xpGedCvtTapFillStr(pGed,fillW, fillH, fillBits);
	    if ( pLI->fillPixmap == None ) {
		fprintf(stderr,"Couldn't alloc pixmap for layer.\n");
	    } else {
		gcMask |= GCStipple | GCFillStyle;
	        gcValues.fill_style = FillStippled;
		gcValues.stipple = pLI->fillPixmap;
		pLI->flags |= XPGedLayerF_Fill;
	    }
	}
    }
    TOP_PDBG((SPKG,"layer %s: flags %x fill %x",
      layer.contents.layer.name, pLI->flags, pLI->fillPixmap));
    pLI->gc = Tk_GetGC( pGed->tkwin, gcMask, &gcValues);
    return pLI->gc;
}


/**
    Dealing with layers is kind of complicated.  Notes:
    1.  The same layer will appear with different octIds in different
	facets.  Thus we must collapse layers by name, not octIds.
**/
static XPGedLayer*
_xpGedGetLayerInfo( XPGed *pGed, octObject *pLayer) {
    Tcl_HashEntry	*pLayerEnt;
    TOPLogical		newB;
    XPGedLayer		*pLI;

    pLayerEnt = Tcl_CreateHashEntry( &pGed->layerIdHash, 
      (char*)(pLayer->objectId), &newB);
    if ( newB ) {
	pLI = MEM_ALLOC(XPGedLayer);
	MEM_CLEAR(pLI);
	pLI->layerId = pLayer->objectId;
	_xpGedAllocLayerGC( pGed, pLI);
	Tcl_SetHashValue(pLayerEnt, (ClientData)pLI);
    } else {
	pLI = (XPGedLayer*) Tcl_GetHashValue(pLayerEnt);
    }
    return pLI;
}

static TOPLogical _XpGedFIHashB = TOP_FALSE;
static Tcl_HashTable _XpGedFacetInfoHash;

int _xpGedLayerListSortCB(const void *a, const void *b) {
    XPGedLayer	*pLA = *((XPGedLayer**) a);
    XPGedLayer	*pLB = *((XPGedLayer**) b);
    return pLA->prio - pLB->prio;
}

static XPGedFacet*
_xpGedGetFacetInfo( XPGed *pGed, octObject *pFacet) {
    TOPLogical		newB;
    XPGedFacet		*pFI;
    XPGedLayer		*pLI;
    Tcl_HashEntry	*facetEnt;

    if ( _XpGedFIHashB == TOP_FALSE ) {
	Tcl_InitHashTable( &_XpGedFacetInfoHash, TCL_ONE_WORD_KEYS);
	_XpGedFIHashB = TOP_TRUE;
    }
    facetEnt = Tcl_CreateHashEntry( &_XpGedFacetInfoHash, 
      (char*)(pFacet->objectId), &newB);
    if ( newB ) {
	pFI = MEM_ALLOC(XPGedFacet);
	MEM_CLEAR(pFI);
	Tcl_SetHashValue(facetEnt,(ClientData)pFI);
	topPtrVecConstructor( &pFI->layerList);
	TOCT_LOOP_CONTENTS_BEGIN(pFacet,OCT_LAYER_MASK,layer) {
	    if ( (pLI = _xpGedGetLayerInfo( pGed, &layer)) == NULL ) {
		fprintf(stderr,"Skipping layer %lx\n", layer.objectId);
		continue;
	    }
	    TOPVEC_Append( &pFI->layerList, XPGedLayer*) = pLI;
	} TOCT_LOOP_CONTENTS_END();
	topVecSort( &pFI->layerList, _xpGedLayerListSortCB);
    } else {
	pFI = (XPGedFacet*) Tcl_GetHashValue(facetEnt);
    }

    return pFI;
}

/**
    Transformations

    We need to map OCT units to pixels.  There are several approaches that
    could be taken:
    -	Use the xpXfa() package.  This is not suitable because (1) it
	assumes we are coming from a floating-point space while OCT 
	units are integers, (2) it uses high-resolution doubles,
	and (3) provides logrithmic scaling which we dont need.
    -	Use the OCT tr package.  This is not suitable because the only
	support it has for scaling is through a full matrix, which also
	supports rotation (twice the complexity).
    -	Use VEM stype OCTSCALE() and DIPSCALE() macros.  These implement
	the scaling factor with a bizarre fixed-point format.  These have
	the advantage of doing all math using integers, but is prob. not
	worthwhile on typical RISC chips which can do floating point
	at least at fast (if not faster) than integer.

    Based on this, we implement our own trivial set of mapping funcs.
    These funcs compute the x&y transforms independently, use float
    (not int or double) for the scaling factors, and map ints (OCT coords)
    to ints (X11 pixels).  The "origin" in both coordinate systems is the
    relative to the upper left corner.
    Typically, map.pixOfs = (0,height) and mapY.oct2pix
    is negative, which moves the pixel origin (and thus the
    oct origin) to the lower left.
**/

#define XPGed_ScaleToPix(pMap,oct)	( (oct)*((pMap)->oct2pix) )
#define XPGed_ScaleToPixX(pGed,octX)	XPGed_ScaleToPix(&(pGed)->mapX,octX)
#define XPGed_ScaleToPixY(pGed,octY)	XPGed_ScaleToPix(&(pGed)->mapY,octY)

#define XPGed_ScaleToOct(pMap,pix)	( (pix)*((pMap)->pix2oct) )
#define XPGed_ScaleToOctX(pGed,pixX)	XPGed_ScaleToOct(&(pGed)->mapX,pixX)
#define XPGed_ScaleToOctY(pGed,pixY)	XPGed_ScaleToOct(&(pGed)->mapY,pixY)

#define XPGed_MapToPix(pMap,oct) \
    ( ((oct)-((pMap)->octOfs))*((pMap)->oct2pix) + (pMap)->pixOfs )
#define XPGed_MapToPixX(pGed,octX)	XPGed_MapToPix(&(pGed)->mapX,octX)
#define XPGed_MapToPixY(pGed,octY)	XPGed_MapToPix(&(pGed)->mapY,octY)

#define XPGed_MapToOct(pMap,pix) \
    ( ((pix)-((pMap)->pixOfs))*((pMap)->pix2oct) + (pMap)->octOfs )
#define XPGed_MapToOctX(pGed,pixX)	XPGed_MapToOct(&(pGed)->mapX,pixX)
#define XPGed_MapToOctY(pGed,pixY)	XPGed_MapToOct(&(pGed)->mapY,pixY)

/**
    In theory, the scale might scale and X and Y axes differently.
    In practice, this doesnt happen.
**/
void
_xpGedScaleOctPt( XPGed *pGed, octPoint *pPt) {
    tr_scaler_transform( pGed->tstk, &pPt->x);
    tr_scaler_transform( pGed->tstk, &pPt->y);
    pPt->x = XPGed_ScaleToPixX(pGed, pPt->x);
    pPt->y = XPGed_ScaleToPixY(pGed, pPt->y);
    pPt->x = TOP_ABS(pPt->x);
    pPt->y = TOP_ABS(pPt->y);
}

void
_xpGedScalePixPt( XPGed *pGed, octPoint *pPt) {
    pPt->x = XPGed_ScaleToOctX(pGed, pPt->x);
    pPt->y = XPGed_ScaleToOctY(pGed, pPt->y);
    pPt->x = TOP_ABS(pPt->x);
    pPt->y = TOP_ABS(pPt->y);
}

/**
    Using the current geometry and mapping of {pGed}, determine the
    oct-coordinate region covered by the window.
**/
static void
_gedGetWinPort( XPGed *pGed, octBox *pBox) {
    pBox->lowerLeft.x = pGed->mapX.octOfs;
    pBox->lowerLeft.y = pGed->mapY.octOfs;
    pBox->upperRight.x = XPGed_MapToOctX(pGed, Tk_Width(pGed->tkwin));
    pBox->upperRight.y = XPGed_MapToOctY(pGed, 0);
}

void
_xpGedMapOctPt( XPGed *pGed, octPoint *pPt) {
    tr_transform( pGed->tstk, &pPt->x, &pPt->y);
    pPt->x = XPGed_MapToPixX(pGed,pPt->x);
    pPt->y = XPGed_MapToPixY(pGed,pPt->y);
}

void
_xpGedMapPixPt( XPGed *pGed, octPoint *pPt) {
    pPt->x = XPGed_MapToOctX(pGed,pPt->x);
    pPt->y = XPGed_MapToOctY(pGed,pPt->y);
}

#define XPGed_SwapLoFirst(type,a,b) {		\
    if ( (a) > (b) )	TOP_SWAPTYPE(type,a,b);	\
}

/**
    Transform the box {pBox} into pixel units.  On return,
    lowerLeft refers to the lower left corner on the screen,
    and similarly for upperRight.  Thus 
      lowerLeft.x < upperRight.x && lowerleft.y > upperRight.y.
**/
void
_xpGedMapOctBox( XPGed *pGed, octBox *pBox) {
    tr_transform( pGed->tstk, &pBox->lowerLeft.x, &pBox->lowerLeft.y);
    tr_transform( pGed->tstk, &pBox->upperRight.x, &pBox->upperRight.y);
    pBox->lowerLeft.x = XPGed_MapToPixX(pGed,pBox->lowerLeft.x);
    pBox->lowerLeft.y = XPGed_MapToPixY(pGed,pBox->lowerLeft.y);
    pBox->upperRight.x = XPGed_MapToPixX(pGed,pBox->upperRight.x);
    pBox->upperRight.y = XPGed_MapToPixY(pGed,pBox->upperRight.y);
    XPGed_SwapLoFirst(int,pBox->lowerLeft.x,pBox->upperRight.x);
    XPGed_SwapLoFirst(int,pBox->upperRight.y,pBox->lowerLeft.y);
}


void
_xpGedDrawOctBox( XPGed *pGed, octBox *pBox, GC gc, TOPLogical fillB) {
    Display	*dpy = Tk_Display(pGed->tkwin);
    Window	win = Tk_WindowId(pGed->tkwin);
    unsigned	dx, dy;

    _xpGedMapOctBox( pGed, pBox);
    dx = pBox->upperRight.x - pBox->lowerLeft.x;
    dy = pBox->lowerLeft.y - pBox->upperRight.y;
    TOP_PDBG((SPKG_DRAW,"%s: draw box %dx%d of %dx%d", pGed->winPath,
      pBox->lowerLeft.x, pBox->upperRight.y, dx, dy));
    if ( fillB ) {
	XFillRectangle( dpy, win, gc,
	  pBox->lowerLeft.x, pBox->upperRight.y, dx, dy);
    } else {
	XDrawRectangle( dpy, win, gc,
	  pBox->lowerLeft.x, pBox->upperRight.y, dx, dy);
    }
}

void
_xpGedMapOctPts( XPGed *pGed, octPoint *pPts_in, int numPts_in,
  XPoint *pPts_out) {
    int		numPts = numPts_in;
    octPoint	*pPt = pPts_in;
    XPoint	*pXPt = pPts_out;
    for ( ; numPts>0; numPts--, pPt++, pXPt++) {
        tr_transform( pGed->tstk, &pPt->x, &pPt->y);
	pXPt->x = XPGed_MapToPixX(pGed,pPt->x);
	pXPt->y = XPGed_MapToPixY(pGed,pPt->y);
    }
}

static void
_xpGedDrawOctCircle( XPGed *pGed, struct octCircle *pCirc, GC gc) {
    Display	*dpy = Tk_Display(pGed->tkwin);
    Window	win = Tk_WindowId(pGed->tkwin);
    int		ang1 = (pCirc->startingAngle << 6) / 10;
    int		ang2 = (pCirc->endingAngle << 6) / 10 - ang1;
    unsigned	mid_radOct;
    octPoint	mRad;

    _xpGedMapOctPt( pGed, &pCirc->center);
    if ( pCirc->innerRadius <= 1 ) {
	/* its a filled circle */
	mRad.x = mRad.y = pCirc->outerRadius;
        _xpGedScaleOctPt( pGed, &mRad);
        XFillArc( dpy, win, gc, pCirc->center.x-mRad.x, pCirc->center.y-mRad.y,
          2*(unsigned)mRad.x, 2*(unsigned)mRad.y, ang1, ang2);
    } else {
        mid_radOct = (pCirc->innerRadius + pCirc->outerRadius) / 2;
    	mRad.x = mRad.y = mid_radOct;
        _xpGedScaleOctPt( pGed, &mRad);
        XDrawArc( dpy, win, gc, pCirc->center.x-mRad.x, pCirc->center.y-mRad.y,
          2*(unsigned)mRad.x, 2*(unsigned)mRad.y, ang1, ang2);
    }
}


#define XPGed_LblJfyH2V(x)	(x)
#define XPGed_LblJfyV2H(x)	(x)
#define XPGed_LblJfyMirrorV(x)	(OCT_JUST_BOTTOM + OCT_JUST_TOP - (x))
#define XPGed_LblJfyMirrorH(x)	(OCT_JUST_LEFT   + OCT_JUST_RIGHT - (x))


static void
_xpGedMapOctLabel( XPGed *pGed, struct octLabel *pLbl) {
    double	rotAngle;
    TOPLogical	mirrorB;
    double	scale;
    unsigned	temp;
    int		th;

    th = XPGed_ScaleToPixY(pGed, pLbl->textHeight);
    pLbl->textHeight = TOP_ABS(th);
    _xpGedMapOctBox( pGed, &pLbl->region);

    tr_get_angle( pGed->tstk, &rotAngle, &mirrorB, &scale);
    if ( mirrorB ) {
	pLbl->vertJust = XPGed_LblJfyMirrorV(pLbl->vertJust);
    }
    /* below should be a switch, but use comparisons instead */
    /*IF*/ if ( rotAngle > 225 ) {
	/* 270 degs: y = -x, x = y */
	temp = pLbl->vertJust;
	pLbl->vertJust = XPGed_LblJfyMirrorV(XPGed_LblJfyH2V(pLbl->horizJust));
	pLbl->horizJust = XPGed_LblJfyV2H(temp);
    } else if ( rotAngle > 135 ) {
	/* 180 degs */
	pLbl->vertJust = XPGed_LblJfyMirrorV(pLbl->vertJust);
	pLbl->horizJust = XPGed_LblJfyMirrorH(pLbl->horizJust);
    } else if ( rotAngle > 45 ) {
	/* 90 degs: y = x; x = -y */
	temp = pLbl->vertJust;
	pLbl->vertJust = XPGed_LblJfyH2V(pLbl->horizJust);
	pLbl->horizJust = XPGed_LblJfyMirrorH(XPGed_LblJfyV2H(temp));
    }
}


#ifdef notdef
static XPGedLabel*
_xpGedGetLblInfo( XPGed *pGed, octObject *pLblObj) {
    Tcl_HashValue	lblEnt;
    TOPLogical		newB;

    lblEnt = Tcl_CreateHashEntry( pGed->lblHash, (char*)(pLblObj->objectId), 
      &newB);
    if ( newB ) {
	pLblObj = MEM_ALLOC(XPGedLabel);
	pLbl
	Tcl_SetHashValue(lblEnt, (ClientData)pLblInfo);
    } else {
	pLblInfo = Tcl_GetHashValue(lblEnt);
    }
}
#endif


static XTextItem*
_xpGedCvtOctLabel( XPGed *pGed, struct octLabel *pLbl, int *pNumItems) {
    static TOPVector	items;
    static TOPLogical	itemsInitB = TOP_FALSE;
    XTextItem		*pTItem;
    char		*s, *s_end;

    if ( itemsInitB == TOP_FALSE ) {
	topVecConstructor( &items, sizeof(XTextItem));
	itemsInitB = TOP_TRUE;
    }
    topVecSetNum( &items, 0);
    for ( s=pLbl->label; *s != '\0'; ) {
	pTItem = &TOPVEC_Append( &items, XTextItem);
	if ( (s_end = strchr(s,'\n')) == NULL )
	    s_end = s + strlen(s);
	pTItem->chars = s;
	pTItem->nchars = s_end - s;
	pTItem->delta = 0;
	if ( *(s = s_end) == '\n' )
	    ++s;
    }
    *pNumItems = TOPVEC_Num( &items);
    return TOPVEC_Array( &items, XTextItem);
}

/**
    Return the overall size of {items} into {pTextW} and {pTextH}.
    We also return the size of each line into the "delta" field
    of each TextItem; this is bogus!
**/
static void
_xpGedCalcLblSize( XTextItem *items, int numItems, XFontStruct *pFont, 
  int *pTextW, int *pTextH) {
    int		i;
    int		textH = 0, textW = 0, tw;

    for (i=0; i < numItems; i++) {
    	textH += pFont->ascent + pFont->descent;
    	tw = XTextWidth( pFont, items[i].chars, items[i].nchars);
	if ( tw > textW )
	    textW = tw;
	items[i].delta = tw;
    }
    *pTextH = textH;
    *pTextW = textW;
}

/**
    Oct Labels are tricky objects.  The text {pLbl->label}
    is multiline with newline seperators between lines.
    All lines are of the same height {pLbl->textHeight}.
    The area and shape of the object is determined by the
    chosen font (thus the text height), and gives a textBB.
    The individual lines are justified within the textBB according
    to {pLbl->lineJust}.  The textBB is anchored with
    {pLbl->region} by {pLbl->vertJust} and {pLbl->horzJust}.
**/
static void
_xpGedDrawOctLabel( XPGed *pGed, struct octLabel *pLbl, GC gc) {
    int		th, textW, textH, textX, textY, delta;
    XFontStruct	*pFont;
    Display	*dpy = Tk_Display(pGed->tkwin);
    Window	win = Tk_WindowId(pGed->tkwin);
    XTextItem	*items;
    int		i, numItems;

    _xpGedMapOctLabel( pGed, pLbl);
    th = pLbl->textHeight;
    if ( th < XPGED_MIN_FONT_HEIGHT )
	th = XPGED_MIN_FONT_HEIGHT;
    if ( (pFont = tkhGetFontByHeight( pGed->labelFontList, th)) == NULL ) {
	topWarnMsg("No font available to draw label ``%s''",
	  pLbl->label);
	return;
    }
    items = _xpGedCvtOctLabel( pGed, pLbl, &numItems);
    _xpGedCalcLblSize( items, numItems, pFont, &textW, &textH);

    switch ( pLbl->vertJust ) {
    case OCT_JUST_BOTTOM:
	textY = pLbl->region.lowerLeft.y - textH + pFont->ascent;
	break;
    case OCT_JUST_CENTER:
	textY = (pLbl->region.lowerLeft.y + pLbl->region.upperRight.y)/2
	  - textH/2 + pFont->ascent - pFont->descent;
	break;
    case OCT_JUST_TOP:
	textY = pLbl->region.upperRight.y - pFont->descent;
	break;
    default: textY = 0;
    }

    switch ( pLbl->horizJust ) {
    case OCT_JUST_LEFT:
	textX = pLbl->region.lowerLeft.x;
	break;
    case OCT_JUST_CENTER:
	textX = (pLbl->region.lowerLeft.x + pLbl->region.upperRight.x)/2
	  - textW/2;
	break;
    case OCT_JUST_RIGHT:
	textX = pLbl->region.upperRight.x - textW;
	break;
    default: textX = 0;
    }

    for (i=0; i < numItems; i++) {
	switch ( pLbl->lineJust ) {
	case OCT_JUST_LEFT:
	    delta = 0;
	    break;
	case OCT_JUST_CENTER:
	    delta = (textW - items[i].delta)/2;
	    break;
	case OCT_JUST_RIGHT:
	    delta = textW - items[i].delta;
	    break;
	default: delta = 0;
	}
	items[i].delta = delta;
	items[i].font = pFont->fid;
        XDrawText( dpy, win, gc, textX, textY, &items[i], 1);
	textY += pFont->ascent + pFont->descent;
    }
}


void
_xpGedDrawFacet( XPGed *pGed, octObject *pFacet, int level) {
    Display	*dpy = Tk_Display(pGed->tkwin);
    Window	win = Tk_WindowId(pGed->tkwin);
    GC		gc = pGed->normGC;
    octBox	instBB;
    int		numPts;
#define XPGED_NUMPTS 200
    octPoint	pts_oct[XPGED_NUMPTS];
    XPoint	pts_x[XPGED_NUMPTS];
    TOPLogical	quickDrawB = 0;
    octObject	master, layer;
    XPGedFacet	*pFI;

    pFI = _xpGedGetFacetInfo( pGed, pFacet);

    if ( level == 0 ) {
	octBox	winBB;
	_gedGetWinPort( pGed, &winBB);
	TOP_PDBG((SPKG_DRAW,"%s: draw winport %dx%d -> %dx%d\n",
	  pGed->winPath, winBB.lowerLeft.x, winBB.lowerLeft.y,
	  winBB.upperRight.x, winBB.upperRight.y));
	TOCT_LOOP_CONTENTS_BEGIN(pFacet,OCT_INSTANCE_MASK,inst) {
	    octBB(&inst,&instBB);
	    if ( TOCT_DisjointB(&winBB, &instBB) )
		continue;
	    TOP_PDBG((SPKG_DRAW,"%s: draw inst %x: %dx%d\n",
	      pGed->winPath, inst.objectId,
	      instBB.lowerLeft.x, instBB.upperRight.y));
	    if ( quickDrawB ) {
		_xpGedDrawOctBox( pGed, &instBB, gc, TOP_FALSE);
		break;
	    } else {
		if ( toctGetMasterByInst( &inst, NULL, &master) != TCL_OK ) {
		    fprintf(stderr,"%s: skipping instance %lx\n",
		      pGed->winPath, inst.objectId);
		    continue;
		}
		tr_push(pGed->tstk);
		tr_add_transform(pGed->tstk, &inst.contents.instance.transform,
		  0);
		_xpGedDrawFacet( pGed, &master, level+1);
		tr_pop(pGed->tstk);
	    }
	} TOCT_LOOP_CONTENTS_END();
    }

    TOPPTRVEC_LOOP_FORW_BEGIN(&pFI->layerList,XPGedLayer*,pLI) {
	  if ( (gc = pLI->gc) == None )
	    continue;
	OH_FAIL(ohGetById(&layer,pLI->layerId),"get layer by id",NULL);
	TOP_PDBG((SPKG_DRAW,"%s: draw layer", pGed->winPath));
        TOCT_LOOP_CONTENTS_BEGIN(&layer,OCT_GEO_MASK,obj) {
	    switch ( obj.type ) {
	    case OCT_POLYGON:
	    case OCT_PATH:
		TOP_PDBG((SPKG_DRAW,"%s: draw path %x", pGed->winPath,
		  obj.objectId));
		numPts = XPGED_NUMPTS;
		if ( octGetPoints( &obj, (long*)&numPts, pts_oct) != OCT_OK ) {
		    fprintf(stderr,"%s: skip path %lx\n", pGed->winPath,
		      (long)obj.objectId);
		    continue;	/* XXX: FIX ME */
		}
		_xpGedMapOctPts( pGed, pts_oct, numPts, pts_x);
		if ( obj.type == OCT_POLYGON 
		  && TOP_OnB(pLI->flags,XPGedLayerF_Fill) ) {
		    XFillPolygon( dpy, win, gc, pts_x, numPts, Complex,
		      CoordModeOrigin);
		} else {
		    XDrawLines( dpy, win, gc, pts_x, numPts, CoordModeOrigin);
		}
		break;
	    case OCT_BOX:
		_xpGedDrawOctBox( pGed, &obj.contents.box, gc, 
		  pLI->flags&XPGedLayerF_Fill);
		break;
	    case OCT_CIRCLE:
		_xpGedDrawOctCircle( pGed, &obj.contents.circle, gc);
		break;
	    case OCT_LABEL:
		TOP_PDBG((SPKG_DRAW,"%s: draw label ``%s'' %d%d%d",
		  pGed->winPath, obj.contents.label.label,
		  obj.contents.label.vertJust,obj.contents.label.horizJust,
		  obj.contents.label.lineJust));
		_xpGedDrawOctLabel( pGed, &obj.contents.label, gc);
		break;
	    }
        } TOCT_LOOP_CONTENTS_END();
    } TOPPTRVEC_LOOP_FORW_END();
}

void
_xpGedExpose( XPGed *pGed) {
    Display	*dpy = Tk_Display(pGed->tkwin);
    Window	win = Tk_WindowId(pGed->tkwin);
    octObject	facet;

    XClearWindow( dpy, win);
    OH_FAIL(ohGetById(&facet,pGed->facetId),"get facet obj",&facet);
    if ( pGed->tstk == NULL )
	pGed->tstk = tr_create();
    _xpGedDrawFacet( pGed, &facet, 0);
}
