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
 * X Constants
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file exports routines that return certain key constants
 * related to X version 11.  They are encapsulated here so
 * that globals aren't required and they have clear initialization.
 */

#include "xvals.h"
#include "errtrap.h"

char *xv_pkg_name = "xv";

#define XVAL_DISP	0x01
#define XVAL_SCRN	0x02
#define XVAL_VIS	0x04
#define XVAL_CMAP	0x08
#define XVAL_DRW	0x10
#define XVAL_DEPTH	0x20
#define XVAL_TOPW	0x40

typedef struct Xvalues_defn {
    int set;			/* Mask of fields set */
    Display *disp;		/* Connection to X    */
    int scrn;			/* Screen number      */
    Visual *vis;		/* Visual             */
    Colormap cmap;		/* Primary colormap   */
    Drawable drw;		/* Useful drawable    */
    int depth;			/* Screen depth       */
    Widget topw;		/* Top level shell    */
} Xvalues;

Xvalues values = {
    0, (Display *) 0, 0, (Visual *) 0, (Colormap) 0, (Drawable) 0, 0
};


/*
 * Set/Get routines
 */

void xv_set_disp(disp)
Display *disp;
/* Sets the display - can only be set once */
{
    static int count = 0;

    if (count == 0) {
	values.set |= XVAL_DISP;
	values.disp = disp;
	count++;
    } else {
	errRaise(xv_pkg_name, XV_MULTI_SET, "Display value set twice");
	/*NOTREACHED*/
    }
}

Display *xv_disp()
/* Returns display variable */
{
    if (values.set & XVAL_DISP) {
	return values.disp;
    } else {
	errRaise(xv_pkg_name, XV_NOT_SET, "Display value not set");
	/*NOTREACHED*/
    }
    return (Display *)0;
}



void xv_set_scrn(scrn)
int scrn;
/* Sets the screen number - can only be set once */
{
    static int count = 0;

    if (count == 0) {
	values.set |= XVAL_SCRN;
	values.scrn = scrn;
	count++;
    } else {
	errRaise(xv_pkg_name, XV_MULTI_SET, "Screen number value set twice");
	/*NOTREACHED*/
    }
}

int xv_scrn()
/* Returns Screen Number */
{
    if (values.set & XVAL_SCRN) {
	return values.scrn;
    } else {
	errRaise(xv_pkg_name, XV_NOT_SET, "Screen value not set");
	/*NOTREACHED*/
    }
    return 0;
}



void xv_set_vis(vis)
Visual *vis;
/* Sets the Visual - can only be set once */
{
    static int count = 0;

    if (count == 0) {
	values.set |= XVAL_VIS;
	values.vis = vis;
	count++;
    } else {
	errRaise(xv_pkg_name, XV_MULTI_SET, "Visual value set twice");
	/*NOTREACHED*/
    }
}

Visual *xv_vis()
/* Returns Screen Number */
{
    if (values.set & XVAL_VIS) {
	return values.vis;
    } else {
	errRaise(xv_pkg_name, XV_NOT_SET, "Screen value not set");
	/*NOTREACHED*/
    }
    return (Visual*)0;
}



void xv_set_cmap(cmap)
Colormap cmap;
/* Sets the colormap - can only be set once */
{
    static int count = 0;

    if (count == 0) {
	values.set |= XVAL_CMAP;
	values.cmap = cmap;
	count++;
    } else {
	errRaise(xv_pkg_name, XV_MULTI_SET, "Colormap value set twice");
	/*NOTREACHED*/
    }
}

Colormap xv_cmap()
/* Returns Screen Number */
{
    if (values.set & XVAL_CMAP) {
	return values.cmap;
    } else {
	errRaise(xv_pkg_name, XV_NOT_SET, "Colormap value not set");
	/*NOTREACHED*/
    }
    return (Colormap)0;
}



void xv_set_drw(drw)
Drawable drw;
/* Sets an appropriate drawable - can only be set once */
{
    static int count = 0;

    if (count == 0) {
	values.set |= XVAL_DRW;
	values.drw = drw;
	count++;
    } else {
	errRaise(xv_pkg_name, XV_MULTI_SET, "Drawable value set twice");
    }
}

Drawable xv_drw()
/* Returns Screen Number */
{
    if (values.set & XVAL_DRW) {
	return values.drw;
    } else {
	errRaise(xv_pkg_name, XV_NOT_SET, "Drawable value not set");
	/*NOTREACHED*/
    }
    return (Drawable)0;
}


void xv_set_depth(depth)
int depth;
/* Sets the screen depth - can only be set once */
{
    static int count = 0;

    if (count == 0) {
	values.set |= XVAL_DEPTH;
	values.depth = depth;
	count++;
    } else {
	errRaise(xv_pkg_name, XV_MULTI_SET, "Depth value set twice");
	/*NOTREACHED*/
    }
}

int xv_depth()
/* Returns Screen Number */
{
    if (values.set & XVAL_DEPTH) {
	return values.depth;
    } else {
	errRaise(xv_pkg_name, XV_NOT_SET, "Depth value not set");
	/*NOTREACHED*/
    }
    return 0;
}



void xv_set_topw(widget)
Widget widget;
/* Sets the top-level Xt Shell widget */
{
    static int count = 0;

    if (count == 0) {
	values.set |= XVAL_TOPW;
	values.topw = widget;
	count++;
    } else {
	errRaise(xv_pkg_name, XV_MULTI_SET, "Top level widget set twice");
	/*NOTREACHED*/
    }
}

Widget xv_topw()
/* Returns top level shell widget if set */
{
    if (values.set & XVAL_TOPW) {
	return values.topw;
    } else {
	errRaise(xv_pkg_name, XV_NOT_SET, "Top level widget not set");
	/*NOTREACHED*/
    }
    return (Widget)0;
}
