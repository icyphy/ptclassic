/* 
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
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
