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

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <math.h>
#include <X11/Xlib.h>

#include "topStd.h"
#include "xpXfa.h"

void
xpXfaDrawCircle2( Display *dpy, Window win, GC gc, 
  XPXfa *xmap, XPXfa *ymap, XPXfaSrcType x0, XPXfaSrcType y0, XPXfaSrcType r) {
    int numsteps = 400;
    double phi,delta = 2*M_PI / numsteps;
    double c1, s1, x, y;
    int i;
    XPoint pts[500];

    i = 0;
    for ( phi=0; phi < 2*M_PI; phi += delta) {
	c1 = cos(phi);
	s1 = sin(phi);
	x = c1 * r + x0;
	y = s1 * r + y0;
        pts[i].x = xpXfaToView(xmap,x);
        pts[i].y = xpXfaToView(ymap,y);
	i++;
    }
    pts[i++] = pts[0];
    XDrawLines( dpy, win, gc,
      pts, i, CoordModeOrigin);
}


/**
    Most servers dont handle values far outside of the clip region
    very well (they slow down to a crawl).  This can occur when
    we transform a circle that is not currently visisble or only
    partially visible.  Thus we need to do clipping in the window space,
    before going to viewport space.
**/
void
xpXfaDrawCircle( Display *dpy, Window win, GC gc, 
  XPXfa *xmap, XPXfa *ymap, XPXfaSrcType x, XPXfaSrcType y, XPXfaSrcType r) {
    int x1, x2, y1, y2;

    /* check for window-port completely outside of circle */
    if ( x+r < xmap->loWin || x-r > xmap->hiWin
      || y+r < ymap->loWin || y-r > ymap->hiWin ) {
	return;
    }
    /* check for window-port completely inside of circle */
    if ( x-r < xmap->loWin && x+r > xmap->hiWin
      && y-r < ymap->loWin && y+r > ymap->hiWin ) {
	return;
    }

    if ( XPXFA_Mode(xmap) != XPXfaMode_Lin 
      || XPXFA_Mode(ymap) != XPXfaMode_Lin ) {
	xpXfaDrawCircle2( dpy, win, gc, xmap, ymap, x, y, r);
	return;
    }
    x1 = xpXfaToView(xmap,x-r);
    y1 = xpXfaToView(ymap,y-r);
    x2 = xpXfaToView(xmap,x+r);
    y2 = xpXfaToView(ymap,y+r);
    if ( x1 > x2 ) 	TOP_SWAPTYPE(int,x1,x2);
    if ( y1 > y2 ) 	TOP_SWAPTYPE(int,y1,y2);
    XDrawArc( dpy, win, gc,
      x1, y1, 
      (unsigned)(x2-x1), (unsigned)(y2-y1),
      0, 360*64);
}
