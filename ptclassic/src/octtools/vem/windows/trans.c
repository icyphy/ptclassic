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
 * VEM Oct Window Management Routines
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This module contains code for coordinate transformations
 * and basic window operations like zoom and pan.
 */

#include "wininternal.h"	/* Internal window definitions */
#include "defaults.h"		/* Default handling            */



vemStatus wnPan(win, panPoint)
Window win;			/* VEM window 			          */
struct octPoint *panPoint;	/* New center of window in OCT coordinates */
/*
 * This routine changes the center of the specified window without changing
 * its extent.  The window must have been grabbed using wnGrab and the
 * coordinates should be expressed in terms of the facet associated with
 * the window.  The routine DOES NOT redraw the window.  This must be
 * done explicitly using wnRedraw.
 */
{
    internalWin *theWin;
    int x_diff, y_diff, fct_num;

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	/* Got him.  Now,  we want [x,y] to be the new center */
	x_diff =
	  (panPoint->x - OCTSCALE(theWin->width >> 1, theWin->scaleFactor)) -
	    theWin->fcts[W_PRI].x_off;
	y_diff =
	  (panPoint->y - OCTSCALE(theWin->height >> 1, theWin->scaleFactor)) -
	    theWin->fcts[W_PRI].y_off;
	for (fct_num = 0;  fct_num < theWin->num_fcts;  fct_num++) {
	    theWin->fcts[fct_num].x_off += x_diff;
	    theWin->fcts[fct_num].y_off += y_diff;
	}
	return VEM_OK;
    } else return VEM_CANTFIND;
}


vemStatus wnAssignScale(destination, source)
Window destination;		/* Target window */
Window source;			/* Source window */
/*
 * This routine makes the scale of 'destination' be the same
 * as that of 'source'.  It then pans the 'destination' window
 * so that the center of the window remains unaffected.  Note:
 * the routine DOES NOT redraw either window.
 */
{
    internalWin *srcWin, *destWin;
    struct octPoint curCenter;

    /* Find both windows */
    if (_wnFind(source, &srcWin, (int *) 0, (int *) 0) &&
	_wnFind(destination, &destWin, (int *) 0, (int *) 0))
      {
	  /* Compute the current center of the destination window */
	  curCenter.x = destWin->fcts[W_PRI].x_off +
	    OCTSCALE(destWin->width >> 1, destWin->scaleFactor);
	  curCenter.y = destWin->fcts[W_PRI].y_off +
	    OCTSCALE(destWin->height >> 1, destWin->scaleFactor);

	  /* Set the scales the same */
	  destWin->scaleFactor = srcWin->scaleFactor;

	  /* Pan the destination so center remains the same */
	  return wnPan(destination, &curCenter);
      }
    else return VEM_CANTFIND;
}



vemStatus wnZoom(win, octExtent)
Window win;			/* VEM window 				   */
struct octBox *octExtent;	/* New extent of window in OCT coordinates */
/*
 * This routine changes the extent of the specified window.  The window
 * must have been grabbed using wnGrab and the coordinates should be
 * expressed in terms of the facet associated with the window.  The
 * routine DOES NOT redraw the window.
 */
{
    internalWin *theWin;
    int octHeight, octWidth;
    int x_diff, y_diff, fct_num;
    double scaleX, scaleY, newScale;

    octHeight = octExtent->upperRight.y - octExtent->lowerLeft.y;
    octWidth = octExtent->upperRight.x - octExtent->lowerLeft.x;
    if ((octHeight == 0) || (octWidth == 0))
      return VEM_CORRUPT;

    /* Get the real information about the window */

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	/*
	 * First,  the scale factor.  We compute both possible
	 * scale factors and choose the larger.  This will always guarantee
	 * the entire OCT region will appear in the window.  However,
	 * more information may appear as well.
	 */

	scaleX = ((double) octWidth)  / ((double) theWin->width);
	scaleY = ((double) octHeight) / ((double) theWin->height);
	newScale = VEMMAX(scaleX, scaleY);

	/*
	 * Now put it in the standard fixed point scale factor form.
	 */

	theWin->scaleFactor = 0;
	if (newScale < 1.0) {
	    theWin->scaleFactor = - (long) (((double) FRAC_VALUE)/newScale+0.5);
	} else {
	    theWin->scaleFactor = (long)(((double) FRAC_VALUE) * newScale + 0.5);
	}

	/*
	 * Next,  the offset.  The center of the original specified
	 * region must be the center of the newly computed region.
	 */

	x_diff = (((octWidth >> 1) + octExtent->lowerLeft.x) -
		  (OCTSCALE(theWin->width, theWin->scaleFactor) >> 1)) -
		    theWin->fcts[W_PRI].x_off;
	y_diff = (((octHeight >> 1) + octExtent->lowerLeft.y) -
		  (OCTSCALE(theWin->height, theWin->scaleFactor) >> 1)) -
		    theWin->fcts[W_PRI].y_off;
	for (fct_num = 0;  fct_num < theWin->num_fcts;  fct_num++) {
	    theWin->fcts[fct_num].x_off += x_diff;
	    theWin->fcts[fct_num].y_off += y_diff;
	}
	return VEM_OK;
    } else {
	return VEM_CANTFIND;
    }
}


#define XNEG	0x01
#define YNEG	0x02

vemStatus wnPoint(win, x, y, retFct, newPoint, options)
Window win;			/* VEM window 			   */
int x, y;			/* Display coordinates in window   */
octId *retFct;			/* Facet of point (RETURN)         */
struct octPoint *newPoint;	/* Transformed point in OCT coords (RETURN) */
int options;			/* Option word (VEM_NOSNAP,VEM_SETPNT,VEM_DPNT)*/
/*
 * Transforms a given point into OCT coordinates.  Returns the buffer of
 * and transformed point of the supplied point.  Returns VEM_CANTFIND
 * if its not a VEM OCT window.  The options are described below:
 *   VEM_NOSNAP:	Point is not snapped to vem.snap.
 *   VEM_SETPNT:	Relative coordinates in titlebar relative to this point
 *   VEM_DPNT:		Update relative coordinates in titlebar else not
 */
{
    internalWin *theWin;
    int signFlags;		/* nothing, XNEG, YNEG, or both */
    int halfGrid, roundSize, temp;
    int octGridUnits;

    /* Find out real information about window */

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	if (retFct) *retFct = theWin->fcts[W_PRI].winFctId;
	/*
	 * Translate point to correct window (only if titlebar is on)
	 * Shouldn't be necessary anymore.
	 */
#ifdef NOTDEF
	if (theWin->opts.disp_options & VEM_TITLEDISP) {
	    y -= _wnTBHeight;
	}
#endif
	/*
	 * We must snap the point to the current grid.  Since OCTSCALE
	 * reliabily underestimates distances (due to integer truncation),
	 * we do this by adding half the physical distance between
	 * grid lines.  Since y increases downward in X,  we subtract
	 * the fudge factor from it.
	 */
	roundSize = DISPSCALE(1, theWin->scaleFactor) / 2;
	x += roundSize;
	y -= roundSize;
	newPoint->x = theWin->fcts[W_PRI].x_off +
	  OCTSCALE(x, theWin->scaleFactor);
	newPoint->y = theWin->fcts[W_PRI].y_off +
	  OCTSCALE(theWin->height - y, theWin->scaleFactor);

	if (!(options & VEM_NOSNAP)) {
	    /* Always snap to small grid */
	    octGridUnits = theWin->opts.snap;
	    /* Snap point to grid line */
	    signFlags = 0;
	    halfGrid = octGridUnits >> 1;
	    if (newPoint->x < 0) {
		newPoint->x = -newPoint->x;
		signFlags |= XNEG;
	    }
	    if (newPoint->y < 0) {
		newPoint->y = -newPoint->y;
		signFlags |= YNEG;
	    }
	    temp = newPoint->x / octGridUnits * octGridUnits;
	    if ((newPoint->x % octGridUnits) > halfGrid)
	      newPoint->x = temp + octGridUnits;
	    else newPoint->x = temp;
	    if (signFlags & XNEG) newPoint->x = -newPoint->x;

	    temp = newPoint->y / octGridUnits * octGridUnits;
	    if ((newPoint->y % octGridUnits) > halfGrid)
	      newPoint->y = temp + octGridUnits;
	    else newPoint->y = temp;
	    if (signFlags & YNEG) newPoint->y = -newPoint->y;
	}
	tbPoint(theWin->tb, newPoint, (options & VEM_DPNT));
	if (options & VEM_SETPNT) {
	    tbSpot(theWin->tb, newPoint);
	}
	return VEM_OK;
    } else {
	return VEM_CANTFIND;
    }
}



vemStatus wnEq(win, p1, p2, tol, dist)
Window win;			/* Reference window  */
struct octPoint *p1, *p2;	/* Comparison points */
int tol;			/* Pixel tolerance   */
long *dist;			/* Representative distance */
/*
 * This routine compares `p1' and `p2' and returns VEM_OK if
 * they are within `tol' pixels of each other in window `win'.
 * `dist' is filled with a representative distance between
 * the points suitable for comparison but should not be
 * construed as the actual distance between the points.
 */
{
    internalWin *theWin;
    long dx, dy;

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	dx = VEMABS(p1->x - p2->x);  dy = VEMABS(p1->y - p2->y);
	if (DISPSCALE(VEMMAX(dx, dy), theWin->scaleFactor) < tol) {
	    if (dist) {
		*dist = dx + dy;
	    }
	    return VEM_OK;
	} else {
	    return VEM_FALSE;
	}
    } else {
	return VEM_CANTFIND;
    }
}



vemStatus wnGravSize(win, pnt, bb)
Window win;			/* Oct graphics window        */
struct octPoint *pnt;		/* Point in window            */
struct octBox *bb;		/* Gravity region (returned)  */
/*
 * This routine checks to see if gravity is turned on for the window
 * `win'.  If it is,  it computes the gravity region,  places it in
 * `bb',  and returns VEM_OK.  If gravity is turned off,  the routine
 * returns VEM_FALSE.  The routine may return VEM_CANTFIND if the
 * window is not under the control of this package.
 */
{
    internalWin *theWin;
    static int grav_size = -1;
    octCoord half_size;

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	if (theWin->opts.disp_options & VEM_GRAVITY) {
	    if (grav_size < 0) {
		dfGetInt("gravity", &grav_size);
	    }
	    half_size = OCTSCALE(grav_size, theWin->scaleFactor)/2;
	    bb->lowerLeft.x = pnt->x - half_size;
	    bb->lowerLeft.y = pnt->y - half_size;
	    bb->upperRight.x = pnt->x + half_size;
	    bb->upperRight.y = pnt->y + half_size;
	    return VEM_OK;
	} else {
	    return VEM_FALSE;
	}
    } else {
	return VEM_CANTFIND;
    }
}
