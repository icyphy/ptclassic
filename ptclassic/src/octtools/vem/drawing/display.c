#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/*LINTLIBRARY*/
/*
 * VEM Graphics Display Routines
 *
 * David Harrison
 * University of California, 1985, 1987, 1989
 *
 * This file contains the implementation of the display routines for
 * drawing OCT graphics primitives.  The displayer routines are designed
 * to repaint a region of a specified VEM graphics window.  The general
 * procedure is:
 *
 *    - When the application first starts,  it should call atrInit to
 *      inform the displayer of the display capabilities and attributes.
 *    - Create new layer attributes using atrNew.  These attributes
 *      are used to set the colors and styles used to display primtives
 *      in this module.
 *    - Set up the display region by calling dspInit.
 *    - For each layer,  call atrSet to inform the displayer
 *      of the drawing attributes for the layer (these are the handles
 *      returned by atrNew).
 *    - For each geometry on the layer,  call the appropriate displayer
 *      routine for the type (dspBox, dspNonManBox, dspLabel, dspCircle,
 *      dspOctagon, or dspPolygon).
 *    - Other primitives are provided for drawing other kinds of graphics
 *      items in a window (dspBound, dspPoint, dspLine, and dspRegion).
 *    - Finish the sequence with dspEnd.
 *
 * Display sequences cannot be overlapped (i.e.  no nested calls to
 * dspInit/dspEnd).
 */

#include "display.h"		/* Self declaration          */
#include <math.h>		/* Math library functions    */
#include "defaults.h"		/* Default information pack. */
#include "polymod.h"		/* Polygon modifications     */
#include "message.h"		/* Message handling system   */
#include "list.h"		/* List package              */
#include "attributes.h"		/* Attribute handler         */
#include "da.h"			/* Dynamic array package     */
#include "lel.h"		/* Label evaluation          */
#include "errtrap.h"		/* Error handling            */

char *dsp_pkg_name = "display";

/* Rounding */
static double rd_temp;
#define RD(val) (((rd_temp = (val)) < 0.0) ? \
		   ((int) (rd_temp-0.5)) : ((int) (rd_temp+0.5)))

/* State information maintained about the display */

typedef struct dsp_info_defn {
    Window dsp_drawable;	/* Window to draw into       */
    XRectangle dsp_area;	/* Size of drawing region    */
    lsList labelFonts;		/* Label fonts (unordered)   */
    XFontStruct *boundFont;	/* Inst and terminal names   */
#ifndef NOPOLYMOD
    struct octBox curWinBB;	/* Clipping box for polygons */
#endif
} dsp_info;

static int dspFlag = 0;		/* Whether info has been filled */
static dsp_info dspInfo;	/* global display information   */

/* General static area for drawing boxes */

static XPoint pointSpace[5] = {
{ 0, 0 } , { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
};

/* For lint */

#define UI	unsigned int
#define US	unsigned short

/* Forward declarations */

static void initInfo();
static void putPoint();



/*
 * Circles and arcs where the inner radius is less than the outer radius
 * and the inner radius is not zero must be drawn manually (there is
 * no primitive in X for such entities.  These figures are drawn using
 * a pre-computed circle of radius 10000 with points at 5 degree increments.  
 * Since the y-axis in the X coordinate system increases in the downward 
 * direction,  the values for the circle are computed by going clockwise 
 * from the x-axis.
 */

#define ARCPNTS 72		/* Number of points of pre-computed circle */
#define BASEANG 50		/* Increment angle of pre-computed circle  */
#define BASERAD	10000		/* Radius of pre-computed circle	   */

#define CIRCIDX(x)	((x) >= ARCPNTS ? (x)-ARCPNTS : (x))

static int circleX[ARCPNTS] = {
    10000, 9961, 9848, 9659, 9396, 9063, 8660, 8191,
    7660, 7071, 6427, 5735, 5000, 4226, 3420, 2588,
    1736, 871, 0, -871, -1736, -2588, -3420, -4226,
    -4999, -5735, -6427, -7071, -7660, -8191, -8660, -9063,
    -9396, -9659, -9848, -9961, -10000, -9961, -9848, -9659,
    -9396, -9063, -8660, -8191, -7660, -7071, -6427, -5735,
    -5000, -4226, -3420, -2588, -1736, -871, 0, 871,
    1736, 2588, 3420, 4226, 4999, 5735, 6427, 7071,
    7660, 8191, 8660, 9063, 9396, 9659, 9848, 9961
};

static int circleY[ARCPNTS] = {
    0, -871, -1736, -2588, -3420, -4226, -4999, -5735,
    -6427, -7071, -7660, -8191, -8660, -9063, -9396, -9659,
    -9848, -9961, -10000, -9961, -9848, -9659, -9396, -9063,
    -8660, -8191, -7660, -7071, -6427, -5735, -5000, -4226,
    -3420, -2588, -1736, -871, 0, 871, 1736, 2588,
    3420, 4226, 4999, 5735, 6427, 7071, 7660, 8191,
    8660, 9063, 9396, 9659, 9848, 9961, 10000, 9961,
    9848, 9659, 9396, 9063, 8660, 8191, 7660, 7071,
    6427, 5735, 5000, 4226, 3420, 2588, 1736, 871
};

/* Final array of points for drawing */
static struct octPoint finalPoints[ARCPNTS*2+4];

/* Constant for creating octagon */
#define TAN45OVER2	0.41421356237458981

/* Apparent legal range X coordinates */

#define	SHORTMIN	-5000
#define SHORTMAX	5000

/*
 * All coordinates to X must be within reasonable boundaries.  The
 * following macro clamps a value to that range.
 */

#ifndef NOPOLYMOD
#define CLAMP(x)	x
#else
#define CLAMP(x) \
((x < SHORTMIN) ? SHORTMIN : ((x > SHORTMAX) ? SHORTMAX : x))
#endif

#ifdef MULTI_TEST

#define Q_SIZE 10000
static XRectangle stip_box_queue[Q_SIZE];
static int top_stip_box_queue = 0;

static XRectangle solid_box_queue[Q_SIZE];
static int top_solid_box_queue = 0;

#define BITS_PER_BYTE	8
#define ROOT_TWO	5
#define VEC_SIZE	Q_SIZE/(sizeof(unsigned long)*BITS_PER_BYTE)+1
static unsigned long stip_vec[VEC_SIZE];
static unsigned long solid_vec[VEC_SIZE];

#define Q_STIP_BOX(_x, _y, _w, _h) \
stip_box_queue[top_stip_box_queue].x = (short) _x; \
stip_box_queue[top_stip_box_queue].y = (short) _y; \
stip_box_queue[top_stip_box_queue].width = (unsigned short) _w; \
stip_box_queue[top_stip_box_queue].height = (unsigned short) _h; \
if (++top_stip_box_queue >= Q_SIZE) dspFlushBoxQueue(1);

#define Q_SOLID_BOX(_x, _y, _w, _h) \
solid_box_queue[top_solid_box_queue].x = (short) _x; \
solid_box_queue[top_solid_box_queue].y = (short) _y; \
solid_box_queue[top_solid_box_queue].width = (unsigned short) _w; \
solid_box_queue[top_solid_box_queue].height = (unsigned short) _h; \
if (++top_solid_box_queue >= Q_SIZE) dspFlushBoxQueue(2);


void dspFlushBoxQueue(val)
int val;
/*
 * Flushes multiple boxes out of queue
 */
{
    switch (val) {
    case 0:
    case 1:
	if (top_stip_box_queue > 0) {
	    XFillRectangles(_AGC.disp, dspInfo.dsp_drawable, _AGC.stipple_gc,
			    stip_box_queue, top_stip_box_queue);
	    stip_vec[top_stip_box_queue >> ROOT_TWO] |=
	      1 << (top_stip_box_queue & (1 << ROOT_TWO)-1);
	    top_stip_box_queue = 0;
	}
	if (val) break;
    case 2:
	if (top_solid_box_queue > 0) {
	    XFillRectangles(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			    solid_box_queue, top_solid_box_queue);
	    solid_vec[top_solid_box_queue >> ROOT_TWO] |=
	      1 << (top_solid_box_queue & (1 << ROOT_TWO)-1);
	    top_solid_box_queue = 0;
	}
	if (val) break;
    }
}

static void dump_top(vec, num)
unsigned long vec[];
int num;
{
    int i, cnt, j;

    cnt = 0;
    for (i = VEC_SIZE-1;  i >= 0;  i--) {
	if (vec[i]) {
	    for (j = ((1 << ROOT_TWO)-1);  j >= 0;  j--) {
		if (vec[i] & (1 << j)) {
		    printf(" %d", j + (i * (1 << ROOT_TWO)));
		    if (++cnt >= num) {
			return;
		    }
		}
	    }
	}
    }
}

static void dump_bot(vec, num)
unsigned long vec[];
int num;
{
    int i, cnt, j;

    cnt = 0;
    for (i = 0;  i < VEC_SIZE;  i++) {
	if (vec[i]) {
	    for (j = 0;  j < (1 << ROOT_TWO);  j++) {
		if (vec[i] & (1 << j)) {
		    printf(" %d", j + (i * (1 << ROOT_TWO)));
		    if (++cnt >= num) {
			return;
		    }
		}
	    }
	}
    }
}

void dspDumpStats()
{
    int i;

    printf("Top five stippled:");
    dump_top(stip_vec, 5);
    printf("\nBottom five stippled:");
    dump_bot(stip_vec, 5);
    for (i = 0;  i < VEC_SIZE;  i++) stip_vec[i] = 0;

    printf("\nTop five solid:");
    dump_top(solid_vec, 5);
    printf("\nBottom five solid:");
    dump_bot(solid_vec, 5);
    printf("\n");
    for (i = 0;  i < VEC_SIZE;  i++) solid_vec[i] = 0;
}

#endif



void dspInit(win, x, y, width, height)
Window win;			/* VEM graphics window 		 */
int x, y, width, height;	/* Region in display coordinates */
/*
 * Initializes the display of a region of a window.  This routine
 * sets up a transparent child window of the supplied window
 * for clipping and clears the region.  This routine should be
 * the first one called when drawing begins.  The window is mapped.
 * If there is alreay a display sequence in progress,  the routine
 * will return VEM_INUSE. VEM_RESOURCE is returned if the allocation of the
 * transparent window fails.
 */
{
    if (!dspFlag) {
	/* Initialize basic display information */
	initInfo();
	dspFlag = 1;
    }
    if (dspInfo.dsp_drawable != (Window) 0) {
	/* No recursive display sequences allowed */
	errRaise(dsp_pkg_name, VEM_INUSE,
		 "in dspInit: recursive display attempted");
    }
    dspInfo.dsp_drawable = win;
    dspInfo.dsp_area.x = x;
    dspInfo.dsp_area.y = y;
    dspInfo.dsp_area.width = width;
    dspInfo.dsp_area.height = height;
    atrClip(x, y, width, height);
#ifndef NOPOLYMOD
    dspInfo.curWinBB.lowerLeft.x = x;
    dspInfo.curWinBB.lowerLeft.y = y;
    dspInfo.curWinBB.upperRight.x = x+width;
    dspInfo.curWinBB.upperRight.y = y+height;
#endif
}


static void initInfo()
/*
 * Initializes global information about a display for use in other
 * parts of the module.  This information is stored in the global
 * structure `dspInfo'.
 */
{
    atrHandle bgAttr;

    dfGetFontList("label.fonts", &(dspInfo.labelFonts));
    dfGetFont("boundbox.font", &(dspInfo.boundFont));

    dfGetAttr("Background", &bgAttr);
    atrSet(bgAttr);
}



void dspClear(opt)
int opt;
/*
 * This routine clears the display region of a display sequence.
 * If opt is CLEAR_ALL,  the plane_mask argument in the graphics
 * context is ignored.  New precondition:  the attribute should
 * be set appropriately (e.g. the background or default attribute).
 */
{
    if (opt & CLEAR_ALL) {
	XClearArea(_AGC.disp, dspInfo.dsp_drawable,
		   dspInfo.dsp_area.x, dspInfo.dsp_area.y,
		   dspInfo.dsp_area.width, dspInfo.dsp_area.height,
		   False);
    } else {
	XFillRectangle(_AGC.disp, dspInfo.dsp_drawable, _AGC.clear_gc,
		       dspInfo.dsp_area.x, dspInfo.dsp_area.y,
		       (UI) dspInfo.dsp_area.width,
		       (UI) dspInfo.dsp_area.height);
    }
}


/*
 * Basic Display Routines
 *
 * VEM is currently capable of displaying five (purely graphic) OCT objects:
 * Boxes, Paths, Polygons, Circles, and Labels.  These items are drawn into
 * and clipped to the boundaries of the window specified in dspInit.
 * The appearence of the items is determined by _AGC (attribute graphics
 * context) which is set by the attributes module routine atrSet.
 *
 * There are a couple of technical notes which are important here:
 *   0.  Versions of VEM written for X10 required coordinates to
 *       be specified relative to the clipping area defined by
 *       dspInt.  In this version,  coordinates should be specified
 *       with respect to the overall window specified in dspInit.
 *   1.  Previous versions of VEM supplied versions of XStippleFill
 *       and XDrawStippled if your server or library didn't support
 *       these calls.  This version no longer supplies these routines.
 *   2.  Large,  non-manhattan polygons cause some servers severe
 *       problems.  To get around these problems,  a polygon massaging
 *       package has been written (see polymod.c).  This package attempts
 *       to change manhattan polygons into boxes and clip non-manhattan
 *       polygons to the window.  If you think you have a server that
 *       doesn't need this kind of help,  use the flag NOPOLYMOD.
 *   3.  Once a geometry becomes so small that drawing it in its stipple
 *       pattern has no meaning,  the display routines draw the object
 *       using solid fill.
 *   4.  If anything fails while using the primitives,  a full VEM
 *       assertion is done which will bring the program to a grinding
 *       halt and await user instructions on how to continue.
 */


void dspBox(oneBox)
register struct octBox *oneBox;		/* PHYSICAL extent of box */
/*
 * Draws a standard filled box.  Uses the global space `pointSpace'
 * for efficiency.  Notes:
 *  - Input coordinates should actually specify the lower-lef
 *    and upper right coordinates (even though the Y coordinate
 *    increases downward.
 *  - Coordinates are clamped to a specified range to prevent
 *    library overflow.
 *  - A minimum sized box is always displayed (features never
 *    disappear).
 */
{
    register unsigned int width, height;

    /* Clamp coordinates */
    oneBox->lowerLeft.x = VEMMAX(oneBox->lowerLeft.x, SHORTMIN);
    oneBox->upperRight.x = VEMMIN(oneBox->upperRight.x, SHORTMAX);
    oneBox->lowerLeft.y = VEMMIN(oneBox->lowerLeft.y, SHORTMAX);
    oneBox->upperRight.y = VEMMAX(oneBox->upperRight.y, SHORTMIN);

    /* Filled box portion */
    width = VEMMAX(oneBox->upperRight.x - oneBox->lowerLeft.x, 1);
    height = VEMMAX(oneBox->lowerLeft.y - oneBox->upperRight.y, 1);
    if (_AGC.fill_thres >= ATR_SOLID) {
	if ((width <= _AGC.fill_thres) || (height <= _AGC.fill_thres)) {
	    /* Solid fill */
#ifdef MULTI_TEST
	    Q_SOLID_BOX(oneBox->lowerLeft.x, oneBox->upperRight.y, width, height);
#else
	    XFillRectangle(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			   (int) oneBox->lowerLeft.x, (int) oneBox->upperRight.y,
			   width, height);
#endif
	} else {
	    /* Stipple fill */
#ifdef MULTI_TEST
	    Q_STIP_BOX(oneBox->lowerLeft.x, oneBox->upperRight.y, width, height);
#else
	    XFillRectangle(_AGC.disp, dspInfo.dsp_drawable, _AGC.stipple_gc,
			   (int) oneBox->lowerLeft.x, (int) oneBox->upperRight.y,
			   width, height);
#endif
	}
    }

    /* Outline portion */
    if (_AGC.line_thres > ATR_SOLID) {
	/* Use line style outline */
	XDrawRectangle(_AGC.disp, dspInfo.dsp_drawable, _AGC.line_gc,
		       (int) oneBox->lowerLeft.x, (int) oneBox->upperRight.y,
		       width, height);
    } else if (_AGC.line_thres == ATR_SOLID) {
	XDrawRectangle(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
		       (int) oneBox->lowerLeft.x, (int) oneBox->upperRight.y,
		       width, height);
    }
}


void dspNonManBox(x1, y1, x2, y2, size)
octCoord x1, y1;		/* Starting coordinate     */
octCoord x2, y2;		/* Ending coordinate       */
int size;			/* Half width of box       */
/*
 * This routine computes the coordinates of a non-manhattan box
 * on the line (x1,y1) to (x2,y2) with width size*2.  This box is 
 * displayed using dspPolygon.  This should  only be called if 
 * the size is non-zero (otherwise it can be drawn as a line).
 */
{
    struct octPoint points[5];
    double distRatio;
    struct octPoint temporary;

    /* Compute point 'size' units along line between (x1,y1) and (x2,y2) */
    distRatio = ((double) size) / 
      sqrt( VEMSQUARE((double) (x2-x1)) + VEMSQUARE((double) (y2-y1)) );
    temporary.x = RD(distRatio * ((double) (x2-x1)));
    temporary.y = RD(distRatio * ((double) (y2-y1)));

    /* Generate points by inverting temporary and adding to x1, y1 */

    points[0].x = x1 - temporary.y;
    points[0].y = y1 + temporary.x;
    points[1].x = x1 + temporary.y;
    points[1].y = y1 - temporary.x;

    /* Generate new temporary relative to x2, y2 */

    temporary.x = RD(distRatio * ((double) (x1-x2)));
    temporary.y = RD(distRatio * ((double) (y1-y2)));

    /* Generate final points relative to x2 and y2. */

    points[2].x = x2 - temporary.y;
    points[2].y = y2 + temporary.x;
    points[3].x = x2 + temporary.y;
    points[3].y = y2 - temporary.x;

    /* Make sure there is some area to fill */

    if (points[0].x == points[1].x) points[1].x += 1;
    if (points[2].x == points[3].x) points[2].x += 1;
    points[4] = points[0];

    /* Draw the box */

    if ((size*2 < _AGC.fill_thres) ||
	(VEMABS(x1-x2) < _AGC.fill_thres) ||
	(VEMABS(y1-y2) < _AGC.fill_thres))
      {
	  /* Too small to fill */
	  dspPolygon(4, points, 0, Convex);
      }
    else
      {
	  /* Use the stipple pattern */
	  dspPolygon(4, points, 1, Convex);
      }
}


#define AVGPOINTS	20

void dspPolygon(numPoints, polyPoints, stipple, shape)
int numPoints;			/* Number of points        */
struct octPoint *polyPoints;	/* Points for poly         */
int stipple;			/* If set, stipple polygon */
int shape;			/* Hints about polygon     */
/*
 * Draws a polygon from the specified coordinate points.  These
 * points must all be PHYSICAL coordinates.  The polygon
 * will be filled if the attribute information specifies it
 * should be filled.  There must be at least three points.
 * PRECONDITION:  the array of points must be allocated with
 * enough space to close the polygon if necessary (at least
 * numPoints+1 spaces).  Implementation notes:
 *   - Normally,  polygons are processed to attempt to make them
 *     easier for the server to handle.  This is done in
 *     polymod.c.
 *   - The space for the polygon points is a dynamic array
 *     maintained by this procedure.
 * The shape of the polygon is as described for XFillPolygon:
 * Complex, Nonconvex, Convex.
 */
{
    static dary points = (dary) 0; /* Dynamic array of XPoint */
    int idx;
#ifndef NOPOLYMOD
    int dontDraw;
#endif

    if (numPoints < 3) return;

    /* Close it if it isn't already closed */
    if ((polyPoints[0].x != polyPoints[numPoints-1].x) ||
	(polyPoints[0].y != polyPoints[numPoints-1].y)) {
	polyPoints[numPoints] = polyPoints[0];
	numPoints++;
    }

#ifndef NOPOLYMOD
    dontDraw = 0;
    if (_AGC.fill_thres >= ATR_SOLID) {
	if (polyProc(&numPoints, &polyPoints,
		     &(dspInfo.curWinBB), stipple) == VEM_OK) {
	    dontDraw = 1;
	}
    }
#endif

    if (!points) {
	points = daAlloc(XPoint, AVGPOINTS);
    }
    for (idx = 0;  idx < numPoints;  idx++) {
	daSet(XPoint, points, idx)->x = CLAMP(polyPoints[idx].x);
	daSet(XPoint, points, idx)->y = CLAMP(polyPoints[idx].y);
    }
	
    /* Fill the polygon */
#ifndef NOPOLYMOD
if (!dontDraw) {
#endif
    if (_AGC.fill_thres >= ATR_SOLID) {
	if (stipple) {
	    /* Stipple it */
	    XFillPolygon(_AGC.disp, dspInfo.dsp_drawable, _AGC.stipple_gc,
			 daData(XPoint, points), numPoints, shape,
			 CoordModeOrigin);
	} else {
	    /* Solid fill it */
	    XFillPolygon(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			 daData(XPoint, points), numPoints, shape,
			 CoordModeOrigin);
	}
    }

    /* Draw the polygon */
    if (_AGC.line_thres > ATR_SOLID) {
	/* Stipple line drawing */
	XDrawLines(_AGC.disp, dspInfo.dsp_drawable, _AGC.line_gc,
		   daData(XPoint, points), numPoints, CoordModeOrigin);
    } else if (_AGC.line_thres == ATR_SOLID) {
	XDrawLines(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
		   daData(XPoint, points), numPoints, CoordModeOrigin);
    }

#ifndef NOPOLYMOD
}
#endif


}



#define	TENTHS_TO_64THS(val)	((val)*32/5)
static void dspGenCircle();

void dspCircle(oneCircle)
struct octCircle *oneCircle;	/* Circle specification */
/*
 * Draws an arbitrary circle or arc.  The circle specification should
 * be in display coordinates.  Angles are specified in tenths of degrees
 * counter-clockwise from the x-axis.  For the cases where the inner radius
 * equals the outer radius,  XDrawArc() is used.  When the inner radius
 * is zero,  XFillArc() is used.  For all other cases,  the general approach 
 * is to scale a precomputed circle to given radius.  If starting or ending 
 * angle does not fall on a precomputed point,  do a linear interpolation to 
 * find  it.  The arc is drawn as a solid shape using the dspPolygon()
 * routine.
 */
{
    octCoord sa, ea;
    XArc arc;

    /* Start by normalizing angles */
    sa = oneCircle->startingAngle % 3600;
    ea = oneCircle->endingAngle % 3600;
    while (ea <= sa) ea += 3600;

    if ((oneCircle->innerRadius > 0) &&
	(oneCircle->innerRadius < oneCircle->outerRadius)) {
	/* General case */
	dspGenCircle(oneCircle);
    } else {
	/* Circle or pie slice */
	arc.x = oneCircle->center.x - oneCircle->outerRadius;
	arc.y = oneCircle->center.y - oneCircle->outerRadius;
	arc.width = arc.height =
	  oneCircle->outerRadius + oneCircle->outerRadius;
	arc.angle1 = TENTHS_TO_64THS(sa);
	arc.angle2 = TENTHS_TO_64THS(ea) - arc.angle1;
	if (oneCircle->innerRadius == 0) {
	    /* Pie slice */
	    if (_AGC.fill_thres >= ATR_SOLID) {
		if (oneCircle->outerRadius*2 <= _AGC.fill_thres) {
		    /* Solid fill */
		    XFillArc(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			     arc.x, arc.y, arc.width, arc.height,
			     arc.angle1, arc.angle2);
		} else {
		    /* Stipple fill */
		    XFillArc(_AGC.disp, dspInfo.dsp_drawable, _AGC.stipple_gc,
			     arc.x, arc.y, arc.width, arc.height,
			     arc.angle1, arc.angle2);
		}
	    }
	    if (_AGC.line_thres > ATR_SOLID) {
		/* Use stylized line outline */
		XDrawArc(_AGC.disp, dspInfo.dsp_drawable, _AGC.line_gc,
			 arc.x, arc.y, arc.width, arc.height,
			 arc.angle1, arc.angle2);
	    } else if (_AGC.line_thres == ATR_SOLID) {
		XDrawArc(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			 arc.x, arc.y, arc.width, arc.height,
			 arc.angle1, arc.angle2);
	    }
	} else {
	    /* Outline only */
	    if ((_AGC.fill_thres >= ATR_SOLID) || (_AGC.line_thres == ATR_SOLID)) {
		/* Solid draw */
		XDrawArc(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			 arc.x, arc.y, arc.width, arc.height,
			 arc.angle1, arc.angle2);
	    } else {
		/* Stipple draw */
		XDrawArc(_AGC.disp, dspInfo.dsp_drawable, _AGC.line_gc,
			 arc.x, arc.y, arc.width, arc.height,
			 arc.angle1, arc.angle2);
	    }
	}
    }
}


static void dspGenCircle(oneCircle)
struct octCircle *oneCircle;	/* Circle specification (ALL physical) */
/*
 * Draws a circle where the inner radius doesn't equal the outer
 * radius (donut shapes).  The routine computes a suitable polygon
 * by scaling a precomputed circle appropriately and uses dspPolygon()
 * to actually render the circle.
 */
{
    struct octPoint spot, nextSpot;
    int startIndex, endIndex, idx, finalIndex, modValue;
    int startAngle, endAngle;

    /* Make angles decent */
    startAngle = oneCircle->startingAngle % 3600;
    endAngle = oneCircle->endingAngle % 3600;
    while (endAngle <= startAngle) {
	endAngle += 3600;
    }

    /* Figure out starting point */

    startIndex = startAngle / BASEANG;
    spot.x = circleX[startIndex] * oneCircle->outerRadius / BASERAD;
    spot.y = circleY[startIndex] * oneCircle->outerRadius / BASERAD;
    if ((modValue = startAngle % BASEANG) != 0) {
	/* Interpolation is required */
	nextSpot.x = circleX[(startIndex+1) % ARCPNTS]
	  * oneCircle->outerRadius / BASERAD;
	nextSpot.y = circleY[(startIndex+1) % ARCPNTS]
	  * oneCircle->outerRadius / BASERAD;
	spot.x += modValue * (nextSpot.x - spot.x) / BASEANG;
	spot.y += modValue * (nextSpot.y - spot.y) / BASEANG;
    }

    /* Put starting point on outer radius into the final array */

    finalIndex = 0;
    putPoint(&finalIndex, spot.x, spot.y);
    endIndex = (endAngle / BASEANG);

    /* Until reaching ending angle,  compute outer radius points */
    /* removing any duplicates */

    for (idx = startIndex+1; idx < endIndex; idx++) {
	putPoint(&finalIndex,
		 circleX[CIRCIDX(idx)] * oneCircle->outerRadius / BASERAD,
		 circleY[CIRCIDX(idx)] * oneCircle->outerRadius / BASERAD);
    }

    /* Do final outer radius point: */

    endIndex = endIndex % ARCPNTS;
    spot.x = circleX[endIndex] * oneCircle->outerRadius / BASERAD;
    spot.y = circleY[endIndex] * oneCircle->outerRadius / BASERAD;
    if ((modValue = endAngle % BASEANG) != 0) {
	/* Interpolation required */
	nextSpot.x =
	  circleX[CIRCIDX(endIndex+1)] * oneCircle->outerRadius / BASERAD;
	nextSpot.y =
	  circleY[CIRCIDX(endIndex+1)] * oneCircle->outerRadius / BASERAD;
	putPoint(&finalIndex,
		 spot.x + modValue * (nextSpot.x - spot.x) / BASEANG,
		 spot.y + modValue * (nextSpot.y - spot.y) / BASEANG);
    } else {
	putPoint(&finalIndex, spot.x, spot.y);
    }

    /* Starting inner radius point */
    spot.x = circleX[endIndex] * oneCircle->innerRadius / BASERAD;
    spot.y = circleY[endIndex] * oneCircle->innerRadius / BASERAD;
    /* Note:  modValue is still valid from ending radius point computation */
    if (modValue != 0) {
	nextSpot.x =
	  circleX[CIRCIDX(endIndex+1)] * oneCircle->innerRadius / BASERAD;
	nextSpot.y =
	  circleY[CIRCIDX(endIndex+1)] * oneCircle->innerRadius / BASERAD;
	putPoint(&finalIndex,
		 spot.x + modValue * (nextSpot.x - spot.x) / BASEANG,
		 spot.y + modValue * (nextSpot.y - spot.y) / BASEANG);
    } else {
	putPoint(&finalIndex, spot.x, spot.y);
    }

    /* Compute inner radius points */

    endIndex = endAngle / BASEANG;
    for (idx = endIndex-1; idx > startIndex; idx--) {
	putPoint(&finalIndex,
		 circleX[CIRCIDX(idx)]*oneCircle->innerRadius / BASERAD,
		 circleY[CIRCIDX(idx)]*oneCircle->innerRadius / BASERAD);
    }
    
    /* Do final inner radius point: */
    
    spot.x = circleX[startIndex] * oneCircle->innerRadius / BASERAD;
    spot.y = circleY[startIndex] * oneCircle->innerRadius / BASERAD;
    if ((modValue = startAngle % BASEANG) != 0) {
	/* Interpolation required */
	nextSpot.x = circleX[(startIndex+1) % ARCPNTS]
	  * oneCircle->innerRadius / BASERAD;
	nextSpot.y = circleY[(startIndex+1) % ARCPNTS]
	  * oneCircle->innerRadius / BASERAD;
	putPoint(&finalIndex,
		 spot.x + modValue * (nextSpot.x - spot.x) / BASEANG,
		 spot.y + modValue * (nextSpot.y - spot.y) / BASEANG);
    } else {
	putPoint(&finalIndex, spot.x, spot.y);
    }

    /*
     * Finally,  add a final closing point if needed, add center to
     * all the points,  and draw the curve.
     */

    if (oneCircle->outerRadius > oneCircle->innerRadius) {
	putPoint(&finalIndex, (octCoord) finalPoints[0].x,
		 (octCoord) finalPoints[0].y);
    }
    for (idx = 0;  idx < finalIndex;  idx++) {
	finalPoints[idx].x += oneCircle->center.x;
	finalPoints[idx].y += oneCircle->center.y;
    }

    /* Display */
    if (_AGC.fill_thres >= ATR_SOLID) {
	if ((oneCircle->outerRadius-oneCircle->innerRadius) * 2 <= _AGC.fill_thres) {
	    /* Solid fill */
	    dspPolygon(finalIndex, finalPoints, 0, Nonconvex);
	} else {
	    /* Stipple fill */
	    dspPolygon(finalIndex, finalPoints, 1, Nonconvex);
	}
    }
}

static void putPoint(index, xVal, yVal)
int *index;			/* Array index (possibly modifed) */
octCoord xVal, yVal;		/* New X and Y coordinates        */
/*
 * This routine puts a new coordinate into the final array of
 * coordinates stored in the global variable 'finalPoints'.  The
 * new coordinate is added at position 'index' iff it is different
 * from the point at index-1.  If the point is added,  index is
 * incremented.
 */
{
    if ((index == 0) ||
	(finalPoints[*index-1].x != xVal) ||
	(finalPoints[*index-1].y != yVal)) {

	    finalPoints[*index].x = xVal;
	    finalPoints[*index].y = yVal;
	    (*index)++;
	}
}




void dspOctagon(oneCircle)
struct octCircle *oneCircle;	/* Octagon specification */
/*
 * This routine is used to draw end types of non-manhattan paths.
 * It draws an octagon centered at the center of the given circle
 * and with the specified radius.  The implementation is transformed
 * into a polygon and displayed using dspPolygon.
 */
{
    struct octPoint octagon[9];
    int x[4], y[4], step;
    int index;

    step = oneCircle->outerRadius -
      ((int) (((double) oneCircle->outerRadius) * TAN45OVER2));
    x[0] = oneCircle->center.x - oneCircle->outerRadius;
    x[3] = oneCircle->center.x + oneCircle->outerRadius;
    x[1] = x[0] + step;
    x[2] = x[3] - step;
    y[0] = oneCircle->center.y - oneCircle->outerRadius;
    y[3] = oneCircle->center.y + oneCircle->outerRadius;
    y[1] = y[0] + step;
    y[2] = y[3] - step;
    for (index = 0;  index < 4;  index++) {
	octagon[index].x = octagon[7 - index].x = x[index];
	if (9-index > 7)
	  octagon[1-index].y = y[index];
	else
	  octagon[9-index].y = y[index];
	octagon[index+2].y = y[index];
    }
    /* Close it */
    octagon[8] = octagon[0];
    /* Ready to display */
    if ((oneCircle->outerRadius << 1) > _AGC.fill_thres) {
	dspPolygon(9, octagon, 1, Convex);
    } else {
	dspPolygon(9, octagon, 0, Convex);
    }
}



/*
 * New Label Display
 *
 * The new Oct label is more complex than the old one.  The following
 * attempts to display the full-fledged label in all its glory.
 */

typedef struct lbl_desc {
    int pixels;			/* Line length in pixels      */
    dary line;			/* Line itself (type is char) */
} lblDesc;

#define LABEL_LINES	10
#define LABEL_LENGTH	40

/* Maintains space for decomposing labels into lines */
static dary lblInfo = (dary) 0;	/* Type is (lblDesc) */

static void prep_line(count)
int count;
/*
 * Sets up the `count' line in the global dary `lblInfo'.
 */
{
    if (daLen(lblInfo) <= count) {
	lblDesc init;
	
	init.pixels = 0;
	init.line = daAlloc(char, LABEL_LENGTH);
	*daSet(lblDesc, lblInfo, count) = init;
    } else {
	daReset(daGet(lblDesc, lblInfo, count)->line);
    }
}

int decompLbl(lbl_str, font)
STR lbl_str;			/* Label string */
XFontStruct *font;		/* Chosen font  */
/*
 * This routine discovers line breaks in `lbl_str'.  The decomposed
 * label is written into the dynamic array of lines `lblInfo'.  This
 * array also contains line length information.  The routine returns
 * the number of lines found in the label (which may be zero).
 */
{
    char *idx;
    int count = 0;
    dary local_line;

    if (!lblInfo) lblInfo = daAlloc(lblDesc, LABEL_LINES);
    count = 0;
    prep_line(count);
    if (lbl_str) {
	for (idx = lbl_str;  *idx;  idx++) {
	    if (*idx == '\n') {
		/* Line break */
		local_line = daGet(lblDesc, lblInfo, count)->line;
		daGet(lblDesc, lblInfo, count)->pixels =
		  XTextWidth(font, daData(char, local_line),
			     daLen(local_line));
		prep_line(++count);
	    } else {
		local_line = daGet(lblDesc, lblInfo, count)->line;
		*daSetLast(char, local_line) = *idx;
	    }
	}
	/* Final null termination */
	local_line = daGet(lblDesc, lblInfo, count)->line;
	daGet(lblDesc, lblInfo, count)->pixels =
	  XTextWidth(font, daData(char, local_line), daLen(local_line));
    } else {
	return 0;
    }
    return count+1;
}




#define FONT_BIG	5000
#define LABEL_FACET	"interface"

static void labelSize( oneLabel, font, l, h, w )
    octObject* oneLabel;
    XFontStruct* font;
    int* l;			/* Lines */
    int* h;			/* Height */
    int* w;			/* Width */
{
    *l = decompLbl(oneLabel->contents.label.label, font);
    *h = oneLabel->contents.label.region.lowerLeft.y -
      oneLabel->contents.label.region.upperRight.y;
    *w = oneLabel->contents.label.region.upperRight.x -
      oneLabel->contents.label.region.lowerLeft.x;
}

static int checkLabelFont( oneLabel, font, checkHeight, l, h, w, mh, mw )
    octObject*   oneLabel;
    XFontStruct* font;
    int checkHeight;
    int* l;			/* Lines */
    int* h;			/* reg height */
    int* w;			/* reg width */
    int* mh;			/* max height */
    int* mw;			/* max width */
{
    int idx;

    if ( checkHeight ) {
	if ((font->ascent + font->descent) > oneLabel->contents.label.textHeight) {
	    return 0;		/* Font too tall */
	}
    }
    
    labelSize( oneLabel, font, l, h, w );
    *mh = *l * (font->ascent + font->descent);
    *mw = 0;
    for (idx = 0;  idx < *l;  idx++) {
	if (daGet(lblDesc, lblInfo, idx)->pixels > *mw) {
	    *mw = daGet(lblDesc, lblInfo, idx)->pixels;
	}
    }

    if ( *mh > *h) {
	return 0;		/* Font is too tall */
    }
    if ( *mw > *w) {
	return 0;		/* Font is too wide. */
    }
    return 1;			/* Font is good. */
}


void dspLabel(oneLabel, inst, opt)
octObject *oneLabel;		/* Label to display       */
octObject *inst;		/* Instance for hierarchy */
int opt;			/* Options                */
/*
 * Draws a label.  First the label is evaluated using lelEvalLabel
 * to expand any inline expressions.  If the label is in a hierarchy
 * of cells,  `inst' should specify the instance whose master
 * contains the label.  After the label is evaluated, an appropriate 
 * font is chosen based on the text height specified in the label.  
 * Then the text is broken into lines (with pixel widths).  Then 
 * the starting position is computed and each line is rendered 
 * according to the specified justifications.  The color of the label 
 * is set by the current attribute.  If the text cannot be rendered,  
 * the bounding box outline is drawn instead.   If `opt' contains DSP_BOX,  
 * the reference box is always drawn.
 */
{
    int idx;
    int lines;		/* Finding font */
    int maxwidth, maxheight;	/* Size of text */
    int regwidth, regheight;	/* Region size  */
    int found = 0;
    int x = 0, y = 0, margin = 0;
    XFontStruct *font, *lastFont = (XFontStruct *)NULL;
    lsGen gen;
    lsStatus stat;
    int apply_boundbox;

    dfGetInt("label.boundbox",  &apply_boundbox);

    /* Evaluate label - ignore return for safety */
    (void) lelEvalLabel(inst, oneLabel, LABEL_FACET);

    /* First find the font closest in height */
    gen = lsStart(dspInfo.labelFonts);
    while ((stat = lsNext(gen, (lsGeneric *) &font, LS_NH)) == LS_OK) {
	lastFont = font;
	/* First check text height */
	if ( checkLabelFont( oneLabel, font, 1, 
			    &lines,&regheight,&regwidth,&maxheight,&maxwidth)) {
	    found = 1;
	    break;
	}
    }
    lsFinish(gen);
    if ( ! found ) {
	/* Check again, this time ignoring text height, just the label region. */
	found =  checkLabelFont( oneLabel, lastFont, 0,
				&lines,&regheight,&regwidth,&maxheight,&maxwidth);
    }

    /* draw the text anyway, so figure out a size */
    if (apply_boundbox == 0 && ! found ) {
	labelSize( oneLabel, lastFont, &lines, &regheight, &regwidth );
	maxheight = lines * (lastFont->ascent + lastFont->descent);
	maxwidth = regwidth;
	found = 1;
    }

    if (apply_boundbox == 0 || found ) {
	atrFont(lastFont);
	/* Compute left margin */
	switch (oneLabel->contents.label.horizJust) {
	case OCT_JUST_LEFT:
	    margin = oneLabel->contents.label.region.lowerLeft.x;
	    break;
	case OCT_JUST_CENTER:
	    margin = oneLabel->contents.label.region.lowerLeft.x +
	      (regwidth - maxwidth)/2;
	    break;
	case OCT_JUST_RIGHT:
	    margin = oneLabel->contents.label.region.lowerLeft.x +
	      (regwidth - maxwidth);
	    break;
	}
	/* Compute starting vertical position */
	switch (oneLabel->contents.label.vertJust) {
	case OCT_JUST_BOTTOM:
	    y = oneLabel->contents.label.region.upperRight.y +
	      (regheight - maxheight);
	    break;
	case OCT_JUST_CENTER:
	    y = oneLabel->contents.label.region.upperRight.y +
	      (regheight - maxheight)/2;
	    break;
	case OCT_JUST_TOP:
	    y = oneLabel->contents.label.region.upperRight.y;
	    break;
	}

	/* Begin drawing sequence */
	y += lastFont->ascent;
	for (idx = 0;  idx < lines;  idx++) {
	    switch (oneLabel->contents.label.lineJust) {
	    case OCT_JUST_LEFT:
		x = margin;
		break;
	    case OCT_JUST_CENTER:
		x = margin + (maxwidth - daGet(lblDesc, lblInfo, idx)->pixels)/2;
		break;
	    case OCT_JUST_RIGHT:
		x = margin + (maxwidth - daGet(lblDesc, lblInfo, idx)->pixels);
		break;
	    }
	    XDrawString(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc, x, y,
			daData(char, daGet(lblDesc, lblInfo, idx)->line),
			daLen(daGet(lblDesc, lblInfo, idx)->line));
	    y += lastFont->ascent + lastFont->descent;
	}
    } else {
	opt |= DSP_BOX;
    }
    if (apply_boundbox == 1) {
	if ((opt & DSP_BOX) || (_AGC.line_thres >= ATR_SOLID)) {
	    dspRegion(dspInfo.dsp_drawable, &(oneLabel->contents.label.region));
	}
    }
}




void dspBound(box, name, location, thres, min)
struct octBox *box;		/* Box specification (physical) */
STR name;			/* Labeling text                */
int location;			/* Options                      */
double thres;			/* Bounding box threshold       */
int min;			/* Abstraction threshold        */
/*
 * This routine draws the outline of the supplied box with
 * labeling text drawn inside it.  The box and labeling text
 * is drawn using the attributes currently in effect (those set
 * using atrSet).  The justification of the text is given by
 * `location' which may be DSP_LL, DSP_UR, DSP_CEN.  The routine 
 * uses `thres' to determine whether the text should be drawn 
 * given the box size.  Also, if the box size (in pixels) is 
 * smaller than the threshold `min',  no text is drawn and the 
 * bounding box will be drawn as a small filled area.  This is 
 * used to display abstractions of small terminals.
 */
{
    int textSize, boxWidth, boxHeight;

    boxWidth = box->upperRight.x - box->lowerLeft.x;
    boxHeight = box->lowerLeft.y - box->upperRight.y;
    if ((boxWidth > 0) && (boxHeight > 0)) {
	if ((boxWidth > min) && (boxHeight > min)) {
	    /* Draw the box as an outline */
	    if (_AGC.line_thres > ATR_SOLID) {
		XDrawRectangle(_AGC.disp, dspInfo.dsp_drawable, _AGC.line_gc,
			       box->lowerLeft.x, box->upperRight.y,
			       (UI) box->upperRight.x - box->lowerLeft.x,
			       (UI) box->lowerLeft.y - box->upperRight.y);
	    } else {
		XDrawRectangle(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			       box->lowerLeft.x, box->upperRight.y,
			       (UI) (box->upperRight.x - box->lowerLeft.x),
			       (UI) (box->lowerLeft.y - box->upperRight.y));
	    }
	} else {
	    /* Draw the box as a filled region */
	    XFillRectangle(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			   box->lowerLeft.x, box->upperRight.y,
			   (UI) (box->upperRight.x - box->lowerLeft.x),
			   (UI) (box->lowerLeft.y - box->upperRight.y));
	}
    }

    /* Draw the name */
    atrFont(dspInfo.boundFont);
    textSize = XTextWidth(dspInfo.boundFont, name, strlen(name));
    if ((textSize > 0) && (boxWidth > 0) &&
	((((double) textSize) / ((double) boxWidth)) < thres))
      {
	  switch (location) {
	  case 0:
	      /* Draw in lower left corner */
	      XDrawString(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			  box->lowerLeft.x + 1,
			  box->lowerLeft.y - dspInfo.boundFont->descent,
			  name, STRLEN(name));
	      break;
	  case DSP_UR:
	      /* Draw in upper right corner */
	      XDrawString(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			  box->upperRight.x - textSize - 1,
			  box->upperRight.y + dspInfo.boundFont->ascent + 1,
			  name, STRLEN(name));
	      break;
	  case DSP_CEN:
	      /* Draw in center */
	      XDrawString(_AGC.disp, dspInfo.dsp_drawable, _AGC.solid_gc,
			  (int) (((box->lowerLeft.x + box->upperRight.x) / 2) -
				 (textSize/2)),
			  (int) ((box->lowerLeft.y + box->upperRight.y)/2 +
				 (dspInfo.boundFont->ascent)/2 -
				 (dspInfo.boundFont->descent)/2),
			  name, STRLEN(name));
	      break;
	  }
      }
}



#define HALF_X	2
#define SIZE	5

void dspPoint(theWin, thePoint, opt)
Window theWin;			/* Window to draw into    */
struct octPoint *thePoint;	/* Position to draw point */
int opt;			/* Options                */
/*
 * This routine draws a marker at the supplied point.  This is used
 * to display the VEM_POINT_ARG argument type and for a dotted grid.
 * If `theWin' is supplied,  the marker will be drawn in that window
 * at the physical location given by `thePoint'.  If `theWin' is
 * not supplied,  the marker will be drawn in the current clipping
 * region set by dspInit.  There are two marker types.  The default
 * marker type (no options are specified) is a small filled box the
 * size of the brush for the current layer.  If `opt' is DSP_XMARK,
 * a 5x5 X will be drawn instead.  The first type is normally used
 * for dotted grids while the second is normally used for the point
 * argument type.
 */
{

    if (opt & DSP_XMARK) {
#ifdef NOTDEF
	XSegment segs[2];
#endif
	XFillRectangle(_AGC.disp, theWin ? theWin : dspInfo.dsp_drawable,
		       _AGC.solid_gc, thePoint->x - HALF_X,
		       thePoint->y - HALF_X, SIZE, SIZE);
#ifdef NOTDEF
	/* Draws small X */
	segs[0].x1 = thePoint->x - HALF_X;
	segs[0].y1 = thePoint->y - HALF_X;
	segs[0].x2 = thePoint->x + HALF_X;
	segs[0].y2 = thePoint->y + HALF_X;
	segs[1].x1 = segs[0].x1;
	segs[1].y1 = segs[0].y2;
	segs[1].x2 = segs[0].x2;
	segs[1].y2 = segs[0].y1;
	XDrawSegments(_AGC.disp, theWin ? theWin : dspInfo.dsp_drawable,
		      _AGC.solid_gc, segs, 2);
#endif
    } else {
	XDrawPoint(_AGC.disp, theWin ? theWin : dspInfo.dsp_drawable,
		   _AGC.solid_gc, thePoint->x, thePoint->y);
    }
}



/*
 * Line clipping - only rarely used (hopefully).  Taken from
 * Neumann and Sproull (algorithm by Cohen and Sutherland).
 * Chosen because it quickly determines whether clipping is
 * required.
 */

#define T_MIN	-10
#define T_MAX	4000

#define LEFT_CODE	0x01
#define RIGHT_CODE	0x02
#define BOTTOM_CODE	0x04
#define TOP_CODE	0x08

#define DO_CODE(pnt, rtn) \
    rtn = 0;   	       	       	       	       	 \
    if ((pnt).x < T_MIN) rtn = LEFT_CODE;	 \
    else if ((pnt).x > T_MAX) rtn = RIGHT_CODE;  \
    if ((pnt).y < T_MIN) rtn |= BOTTOM_CODE;     \
    else if ((pnt).y > T_MAX) rtn |= TOP_CODE

void dspLine(theWin, point1, point2)
Window theWin;			/* X Window to draw in */
struct octPoint *point1;	/* First point         */
struct octPoint *point2;	/* Second point        */
/*
 * This routine draws a line from point1 to point2.  It is used to
 * draw the VEM_LINE_ARG type and for drawing grid lines.  If theWin
 * is zero,  the current dspInfo.dsp_drawable will be used to draw the line.
 * To prevent nastyness,  we try to clip the line to the maximum X
 * coordinate space.
 */
{
    int code1, code2, cd, x = 0, y = 0;
    XRectangle rect;		/* In case its manhattan */

    /* Copy into drawing vector */
    pointSpace[0].x = point1->x;  pointSpace[0].y = point1->y;
    pointSpace[1].x = point2->x;  pointSpace[1].y = point2->y;

    /* Check to see if we clip */
    DO_CODE(pointSpace[0], code1);
    DO_CODE(pointSpace[1], code2);

    /* Begin line clipper */
    while (code1 || code2) {
	/* See if entirely off screen */
	if (code1 & code2) return;
	cd = (code1 ? code1 : code2);
	if (cd & LEFT_CODE) {	/* Crosses left edge */
	    y = pointSpace[0].y + (pointSpace[1].y - pointSpace[0].y) *
	      (T_MIN - pointSpace[0].x) / (pointSpace[1].x - pointSpace[0].x);
	    x = T_MIN;
	} else if (cd & RIGHT_CODE) { /* Crosses right edge */
	    y = pointSpace[0].y + (pointSpace[1].y - pointSpace[0].y) *
	      (T_MAX - pointSpace[0].x) / (pointSpace[1].x - pointSpace[0].x);
	    x = T_MAX;
	} else if (cd & BOTTOM_CODE) { /* Crosses bottom edge */
	    x = pointSpace[0].x + (pointSpace[1].x - pointSpace[0].x) *
	      (T_MIN - pointSpace[0].y) / (pointSpace[1].y - pointSpace[0].y);
	    y = T_MIN;
	} else if (cd & TOP_CODE) { /* Crosses top edge */
	    x = pointSpace[0].x + (pointSpace[1].x - pointSpace[0].x) *
	      (T_MAX - pointSpace[0].y) / (pointSpace[1].y - pointSpace[0].y);
	    y = T_MAX;
	}
	if (cd == code1) {
	    pointSpace[0].x = x;  pointSpace[0].y = y;
	    DO_CODE(pointSpace[0], code1);
	} else {
	    pointSpace[1].x = x;  pointSpace[1].y = y;
	    DO_CODE(pointSpace[1], code2);
	}
    }

    /* If we reach here,  we are ready to show it */
    if (_AGC.line_thres != ATR_SOLID) {
	/* Use line style outline */
	XDrawLines(_AGC.disp, (theWin ? theWin : dspInfo.dsp_drawable),
		   _AGC.line_gc, pointSpace, 2, CoordModeOrigin);
    } else {
	/* Optimization - draw it as a box if manhattan */
	if (pointSpace[0].x == pointSpace[1].x) {
	    /* Vertical */
	    rect.width = _AGC.line_width;
	    if (rect.width <= 0) rect.width = 1;
	    rect.x = pointSpace[0].x - (_AGC.line_width>>1);
	    if (pointSpace[0].y < pointSpace[1].y) {
		rect.y = pointSpace[0].y;
		rect.height = pointSpace[1].y - pointSpace[0].y + 1;
	    } else {
		rect.y = pointSpace[1].y;
		rect.height = pointSpace[0].y - pointSpace[1].y + 1;
	    }
	    XFillRectangle(_AGC.disp, (theWin ? theWin : dspInfo.dsp_drawable),
			   _AGC.solid_gc, rect.x, rect.y, rect.width, rect.height);
	} else if (pointSpace[0].y == pointSpace[1].y) {
	    /* Horizontal */
	    rect.height = _AGC.line_width;
	    if (rect.height <= 0) rect.height = 1;
	    rect.y = pointSpace[0].y - (_AGC.line_width>>1);
	    if (pointSpace[0].x < pointSpace[1].x) {
		rect.x = pointSpace[0].x;
		rect.width = pointSpace[1].x - pointSpace[0].x + 1;
	    } else {
		rect.x = pointSpace[1].x;
		rect.width = pointSpace[0].x - pointSpace[1].x + 1;
	    }
	    XFillRectangle(_AGC.disp, (theWin ? theWin : dspInfo.dsp_drawable),
			   _AGC.solid_gc, rect.x, rect.y, rect.width, rect.height);
	} else {
	    /* Non-manhattan */
	    XDrawLines(_AGC.disp, (theWin ? theWin : dspInfo.dsp_drawable),
		       _AGC.solid_gc, pointSpace, 2, CoordModeOrigin);
	}
    }
}



void dspRegion(theWin, oneBox)
Window theWin;			/* Window to draw in */
struct octBox *oneBox;		/* Box to draw       */
/*
 * This routine draws the outline of the specified rectangle in
 * the specified window.  It is used to display the VEM_BOX_ARG
 * argument type and outlines around normal boxes (if any).
 * If the window is not provided, the current clipping box will be used.
 */
{
    /* Clamp (if required) */
    if (oneBox->lowerLeft.x < SHORTMIN) oneBox->lowerLeft.x = SHORTMIN;
    if (oneBox->upperRight.x > SHORTMAX) oneBox->upperRight.x = SHORTMAX;
    if (oneBox->lowerLeft.y < SHORTMIN) oneBox->lowerLeft.y = SHORTMIN;
    if (oneBox->upperRight.y > SHORTMAX) oneBox->upperRight.y = SHORTMAX;

    /* Draw outline around box - always draws something */
    if (_AGC.line_thres > ATR_SOLID) {
	XDrawRectangle(_AGC.disp, (theWin ? theWin : dspInfo.dsp_drawable),
		       _AGC.line_gc,
		       oneBox->lowerLeft.x, oneBox->upperRight.y,
		       (UI) (oneBox->upperRight.x - oneBox->lowerLeft.x),
		       (UI) (oneBox->lowerLeft.y - oneBox->upperRight.y));
    } else {
	XDrawRectangle(_AGC.disp, (theWin ? theWin : dspInfo.dsp_drawable),
		       _AGC.solid_gc,
		       oneBox->lowerLeft.x, oneBox->upperRight.y,
		       (UI) (oneBox->upperRight.x - oneBox->lowerLeft.x),
		       (UI) (oneBox->lowerLeft.y - oneBox->upperRight.y));
    }
}




void dspEnd()
/*
 * Finish a display sequence.  Resets any clipping region set by
 * dspInit.
 */
{
#ifdef MULTI_TEST
#ifdef MULTI_STATS
    dspDumpStats();
#endif
    dspFlushBoxQueue(0);
#endif
    dspInfo.dsp_drawable = (Window) 0;
    atrClip(0, 0, 0, 0);
}

