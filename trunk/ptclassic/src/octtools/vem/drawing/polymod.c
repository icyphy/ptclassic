#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
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
*/
/*LINTLIBRARY*/
/*
 * VEM Polygon Library
 *
 * David Harrison
 * University of California,  1986, 1989
 *
 * This file contains a routine for clipping polygons to a bounding
 * box.  The approach is a modified version of the Liang-Barsky 
 * Polygon clipping algorithm [CACM, Vol 26 (Nov, 1983)]. This routine 
 * is necessary since some devices (notably the GPX),  may go into
 * the twilight zone attempting to clip very large polygons (especially
 * non-manhattan polygons).
 *
 * Also,  X is not noted for its ability to draw complex polygons
 * quickly.  This library also has a routine to turn manhattan polygons
 * into boxes.
 */

#include "port.h"
#include "polymod.h"		/* Self declaration  */
#include "attributes.h"		/* Attribute handler */
#include "message.h"		/* Message stuff     */
#include "display.h"		/* Display code      */
#include "vemUtil.h"



/* Modified Liang-Barsky Polygon Clipping [CACM, Vol 26 (Nov, 1983)] */

/*
 * Should fill in comments from the original version here.
 */

#define INFINITY	(1.0e+30)
#define NEARZERO	(1.0e-30)	/* 1/INFINITY */

struct varray {
    int length;
    int count;
    struct octPoint *array;
};

#define varray_init(varray) ((varray)->count = 0)

#define varray_add(varray, xv,yv)\
    ((varray)->count < (varray)->length ?\
         (varray)->array[(varray)->count].x = xv,\
     	     (varray)->array[(varray)->count++].y = yv\
         : varray_grow((varray), xv,yv))

#define varray_length(varray) (varray)->count

#define varray_array(varray) (varray)->array

/* varray_grow must return an int, since we use it in a ':' statement
 * and with cfront, both args of : must have the same type
 */
static int varray_grow(varray, x, y)
struct varray *varray;
int x, y;
{
    if (varray->length == 0) {
	varray->length = 50;
	varray->array = (struct octPoint *)
	  malloc((unsigned) varray->length*sizeof(struct octPoint));
    } else {
	varray->length *= 2;
	varray->array = (struct octPoint *)
	  realloc((char *) varray->array,
		  (unsigned) varray->length*sizeof(struct octPoint));
    }
    varray_add(varray, x,y);
    return 0;
}

#define add(x,y) varray_add(&out, x, y)

static void polyClip(len, points, bb, outlen, outpoints)
int len;			/* Incoming length */
register struct octPoint *points;/* Incoming points */
struct octBox *bb;		/* Bounding box    */
int *outlen;			/* Output length   */
struct octPoint **outpoints;	/* Output points   */
{
    struct octPoint *ptr;
    static struct varray out;
    double deltax, deltay;
    double tinx,tiny,  toutx,touty,  tin1, tin2,  tout;
    int wx1,wy1, wx2,wy2;
    int x1,y1, x2,y2;
    int xin,xout,  yin,yout;

    varray_init(&out);

    wx1 = bb->lowerLeft.x, wy1 = bb->lowerLeft.y;
    wx2 = bb->upperRight.x, wy2 = bb->upperRight.y;
    
    x1 = points[len-1].x;
    y1 = points[len-1].y;
    
    for (ptr = points; ptr < points + len; ptr++) {
	
	x2 = ptr->x;
	y2 = ptr->y;
	
	deltax = x2-x1;
	if (deltax == 0) { /* bump off of the vertical */
	    deltax = (x1 > wx1) ? -NEARZERO : NEARZERO ;
	}
	deltay = y2-y1;
	if (deltay == 0) { /* bump off of the horizontal */
	    deltay = (y1 > wy1) ? -NEARZERO : NEARZERO ;
	}
	
	if (deltax > 0) {		/*  points to right */
	    xin = wx1;
	    xout = wx2;
	} else {
	    xin = wx2;
	    xout = wx1;
	}
	if (deltay > 0) {		/*  points up */
	    yin = wy1;
	    yout = wy2;
	} else {
	    yin = wy2;
	    yout = wy1;
	}
	
	tinx = (xin - x1)/deltax;
	tiny = (yin - y1)/deltay;
	
	if (tinx < tiny) {	/* hits x first */
	    tin1 = tinx;
	    tin2 = tiny;
	}
	else			/* hits y first */
	  {
	      tin1 = tiny;
	      tin2 = tinx;
	  }
	
	if (1 >= tin1) {
	    if (0 < tin1) {
		add(xin,yin);
            }
	    if (1 >= tin2) {
		toutx = (xout - x1)/deltax;
		touty = (yout - y1)/deltay;
		
		tout = (toutx < touty) ? toutx : touty ;
		
		if (0 < tin2 || 0 < tout) {
		    if (tin2 <= tout) {
			if (0 < tin2) {
			    if (tinx > tiny) {
				add(xin, y1+ (int)(tinx*deltay));
			    } else {
				add(x1 + (int)(tiny*deltax), yin);
			    }
			}
			if (1 > tout) {
			    if (toutx < touty) {
				add(xout, y1 + (int)(toutx*deltay));
			    } else {
				add(x1 + (int)(touty*deltax), yout);
			    }
			} else {
			    add(x2,y2);
			}
		    } else {
			if (tinx > tiny) {
			    add(xin, yout);
			} else {
			    add(xout, yin);
			}
		    }
		}
            }
	}
	x1 = x2;
	y1 = y2;
    }
    *outlen = varray_length(&out);
    *outpoints = varray_array(&out);
}


/*
 * Polygon Fracturing
 *
 * The following declarations and functions provide a means to break
 * manhattan polygons into boxes and display them using dspBox (see
 * display.c).  The general approach is a scan-line like approach
 * inspired by P. Moore's fang package.
 */


/* List of X coordinates for sorting */
static int XCoordAlloc = 0;
static int *XCoords = (int *) 0;

static int intcmp(a, b)
int *a, *b;
{
    return *a - *b;
}

/* List of X edges to construct boxes */

typedef struct vem_edge {
    int x1, x2;
    int y;
} VemEdge;

static int edgeAlloc = 0;
static VemEdge *edges = (VemEdge *) 0;

static int edgecmp(a, b)
VemEdge *a, *b;
{
    if (a->y < b->y) return -1;
    else if (a->y > b->y) return 1;
    else if (a->x2 < b->x1) return -1;
    else if (a->x1 > b->x2) return 1;
    else return 0;
}


#define INITPOINTS	20

static void fracPoly(num, pnts)
int num;			/* Number of points */
struct octPoint *pnts;		/* Point list       */
/*
 * This routine breaks a manhattan polygon into boxes and
 * uses dspBox to display them.  The algorithm used is of the
 * scan-line type.  All of the X edges are extracted,  sorted and
 * paired together to form boxes.
 */
{
    struct octBox theBox;
    int index, count;
    int startX, endX, uniqX, idx, edgeY;
    int edgeidx, regidx, found;
    
    if (XCoordAlloc == 0) {
	XCoordAlloc = INITPOINTS;
	XCoords = VEMARRAYALLOC(int, XCoordAlloc);
    }
    if (XCoordAlloc < num) {
	XCoordAlloc = num + (num/10);
	XCoords = VEMREALLOC(int, XCoords, XCoordAlloc);
    }

    /* Sort the X coordinates */
    for (index = 0;  index < num;  index++) {
	XCoords[index] = pnts[index].x;
    }
    qsort((char *) XCoords, num, sizeof(int), intcmp);

    /* Remove the duplicates from the sorted list (see vemUtil.c) */
    uniqX = uniq((char *) XCoords, num, sizeof(int), intcmp);

    /* Build list of X edges */
    if (edgeAlloc == 0) {
	edgeAlloc = INITPOINTS;
	edges = VEMARRAYALLOC(VemEdge, edgeAlloc);
    }
    count = 0;
    for (index = 0;  index < num-1;  index++) {
	if (pnts[index].y == pnts[index+1].y) {
	    /* Its an X edge */
	    edgeY = pnts[index].y;
	    /* Test against sorted X to generate edges */
	    if (pnts[index].x < pnts[index+1].x) {
		startX = pnts[index].x;
		endX = pnts[index+1].x;
	    } else {
		startX = pnts[index+1].x;
		endX = pnts[index].x;
	    }
	    for (idx = 0;  idx < uniqX;  idx++) {
		if (XCoords[idx] == startX) break;
	    }
	    while (XCoords[idx] < endX) {
		if (count >= edgeAlloc) {
		    /* More space required */
		    edgeAlloc *= 2;
		    edges = VEMREALLOC(VemEdge, edges, edgeAlloc);
		}
		edges[count].y = edgeY;
		edges[count].x1 = XCoords[idx];
		edges[count].x2 = XCoords[idx+1];
		idx++;
		count++;
	    }
	}
    }

    /* Sort the list of edges in by Y */
    qsort((char *) edges, count, sizeof(VemEdge), edgecmp);

    /* Produce list of boxes */
    regidx = 0;
    for (edgeidx = 0;  edgeidx < count;  edgeidx++) {
	found = 0;
	for (index = 0;  index < regidx;  index++) {
	    if ((edges[edgeidx].x1 == edges[index].x1) &&
		(edges[edgeidx].x2 == edges[index].x2))
	      {
		  /* Found a registered edge */
		  theBox.lowerLeft.x = edges[edgeidx].x1;
		  theBox.upperRight.x = edges[edgeidx].x2;
		  theBox.lowerLeft.y = edges[edgeidx].y;
		  theBox.upperRight.y = edges[index].y;
		  dspBox(&theBox);
		  edges[index] = edges[--regidx];
		  found = 1;
		  break;
	      }
	}
	if (!found) {
	    /* Didn't find it - register it */
	    edges[regidx++] = edges[edgeidx];
	}
    }
    for (index = 0;  index < regidx;  index++) {
	vemMsg(MSG_A, "vemFracPoly: unmatched edge: y=%d, x1=%d, x2=%d\n",
	       edges[index].y, edges[index].x1, edges[index].x2);
    }
}




vemStatus polyProc(numPoints, polyPoints, theBB, stip)
int *numPoints;			/* Number of points in polygon */
struct octPoint **polyPoints;	/* Points of polygon           */
struct octBox *theBB;		/* Clipping area               */
int stip;			/* Whether or not to stipple   */
/*
 * This routine processes a polygon to obtain a reasonable drawing
 * implementation for it.  If the polygon is non-manhattan,  the
 * polygon will be manually clipped to 'theBB' and retured for
 * display using XDrawTiled.  If the polygon is manhattan,  it
 * will be fractured into boxes and displayed using dspBox.
 */
{
    struct octBox theBox, polyBB;
    register struct octPoint *localPnts = *polyPoints;
    int index, manFlag;

    /* First,  close the polygon if required */
    if (memcmp((char *) &(localPnts[0]), (char *) &(localPnts[*numPoints-1]),
	     sizeof(struct octPoint)))
      {
	  localPnts[*numPoints] = localPnts[0];
	  (*numPoints)++;
      }
    /* Now,  check to see if its manhattan */
    manFlag = 1;
    for (index = 0;  index < *numPoints-1;  index++) {
	if ((localPnts[index].x != localPnts[index+1].x) &&
	    (localPnts[index].y != localPnts[index+1].y))
	  {
	      manFlag = 0;
	      break;
	  }
    }
    if (manFlag) {
	int old_value;

	/* Make sure drawn boxes will be stippled - this is a hack */
	old_value = _AGC.fill_thres;
	if (stip) {
	    _AGC.fill_thres = 0;
	} else {
	    _AGC.fill_thres = ATR_SOLID;
	}

	/* If its a box,  draw it that way */
	if (*numPoints == 5) {
	    /* Draw as a box */
	    if (localPnts[0].x == localPnts[1].x) {
		if (localPnts[0].y < localPnts[1].y) {
		    theBox.lowerLeft.y = localPnts[1].y;
		    theBox.upperRight.y = localPnts[0].y;
		} else {
		    theBox.lowerLeft.y = localPnts[0].y;
		    theBox.upperRight.y = localPnts[1].y;
		}
		if (localPnts[1].x < localPnts[2].x) {
		    theBox.lowerLeft.x = localPnts[1].x;
		    theBox.upperRight.x = localPnts[2].x;
		} else {
		    theBox.lowerLeft.x = localPnts[2].x;
		    theBox.upperRight.x = localPnts[1].x;
		}
	    } else {
		if (localPnts[0].x < localPnts[1].x) {
		    theBox.lowerLeft.x = localPnts[0].x;
		    theBox.upperRight.x = localPnts[1].x;
		} else {
		    theBox.lowerLeft.x = localPnts[1].x;
		    theBox.upperRight.x = localPnts[0].x;
		}
		if (localPnts[1].y < localPnts[2].y) {
		    theBox.lowerLeft.y = localPnts[2].y;
		    theBox.upperRight.y = localPnts[1].y;
		} else {
		    theBox.lowerLeft.y = localPnts[1].y;
		    theBox.upperRight.y = localPnts[2].y;
		}
	    }
	    dspBox(&theBox);
	} else {
	    /* Fracture into boxes and display them */
	    fracPoly(*numPoints, localPnts);
	}
	_AGC.fill_thres = old_value;
	return VEM_OK;
    } else {
	/* Its non-manhattan - clip against boundary if necessary */
    
	/* Figure out bounding box for polygon */
	polyBB.lowerLeft.x = VEMMAXINT;
	polyBB.upperRight.x = VEMMININT;
	polyBB.upperRight.y = VEMMININT;
	polyBB.lowerLeft.y = VEMMAXINT;
	for (index = 0;  index < *numPoints;  index++) {
	    if (localPnts[index].x < polyBB.lowerLeft.x)
	      polyBB.lowerLeft.x = localPnts[index].x;
	    if (localPnts[index].x > polyBB.upperRight.x)
	      polyBB.upperRight.x = localPnts[index].x;
	    if (localPnts[index].y < polyBB.lowerLeft.y)
	      polyBB.lowerLeft.y = localPnts[index].y;
	    if (localPnts[index].y > polyBB.upperRight.y)
	      polyBB.upperRight.y = localPnts[index].y;
	}

	/* Does polygon need to be clipped? */
	if (!VEMBOXINSIDE(polyBB, *theBB)) {
	    /* Must be clipped */
	    polyClip(*numPoints, localPnts, theBB, numPoints, polyPoints);
	    if (*numPoints < 3) {
		/* Clipped to nothing */
		return VEM_OK;
	    } else {
		/* Got a polygon to draw */
		return VEM_FALSE;
	    }
	} else {
	    /* No clipping */
	    return VEM_FALSE;
	}
    }
}
