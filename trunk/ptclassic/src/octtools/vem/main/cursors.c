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
 * VEM Cursor Management
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1989
 *
 * This file contains routines for managing the cursors used
 * inside vem.  Most of the bitmaps for these cursors are defined
 * in the cursors directory.
 */

#include "cursors.h"		/* Self declaration        */
#include <X11/Xutil.h>		/* X utility functions     */
#include "xvals.h"		/* X values                */
#include "vemUtil.h"		/* General VEM utilities   */
#include "defaults.h"		/* Default package         */
#include "windows.h"		/* Window management       */
#include "dds.h"		/* Dialog system           */

/* Console window for vemBusy() */
extern Window consWin;

Cursor vemCursor();

/* Whether or not VEM is busy */
static int busyFlag;

/* Cursor for console window */
#include "vemcon.bitmap"

/* Cursor for normal graphics windows */
#include "vem_cross.bitmap"

/* Hour glass for busy cursor */
#include "stopWatch.bitmap"
#include "stopWatchMask.bitmap"

/* Cursor for grabbing the mouse */
#include "vem_box.bitmap"

/* Cursor table */
static Cursor *vemCursorTable = (Cursor *) 0;




static Cursor makeCursor(disp, drw, w, h, source, mask, hx, hy, fg, bg)
Display *disp;			/* X Connection    */
Drawable drw;			/* Screen Drawable */
int w, h;			/* Size of cursor  */
char *source;			/* Source bits     */
char *mask;			/* Mask bits       */
int hx, hy;			/* Hot spot        */
XColor *fg, *bg;		/* Cursor color    */
/*
 * Creates a new cursor from the supplied information and returns
 * it.  The colors must be fully specified.
 */
{
    Pixmap s, m;
    Cursor result;

    s = XCreateBitmapFromData(disp, drw, source, w, h);
    m = XCreateBitmapFromData(disp, drw, mask, w, h);
    result = XCreatePixmapCursor(disp, s, m, fg, bg, hx, hy);
    XFreePixmap(disp, s);
    XFreePixmap(disp, m);
    return result;
}


static void vemInitCursors()
/*
 * This routine defines all VEM cursors for retrieval using
 * vemCursor.
 */
{
    XColor *consColor, *curColor;
    XColor white, black;
    Drawable drw;

    vemCursorTable = VEMARRAYALLOC(Cursor, END_CURSORS);

    /* Get drawable */
    drw = xv_drw();

    /* Get black and white */
    (void) vuWhite(&white);
    (void) vuBlack(&black);

    /* Console cursor */
    dfGetPixel("console.cursor", &consColor);
    vemCursorTable[CONSOLE_CURSOR] =
      makeCursor(xv_disp(), drw, vemcon_width, vemcon_height,
		 vemcon_bits, vemcon_bits, vemcon_x_hot, vemcon_y_hot,
		 consColor, &white);

    /* Normal Cross Cursor */
    dfGetPixel("cursor.color", &curColor);
    vemCursorTable[CROSS_CURSOR] =
      makeCursor(xv_disp(), drw, vem_cross_width, vem_cross_height,
		 vem_cross_bits, vem_cross_bits,
		 vem_cross_x_hot, vem_cross_y_hot,
		 curColor, &black);

    /* VEM Busy Cursor */
    vemCursorTable[BUSY_CURSOR] =
      makeCursor(xv_disp(), drw, stopWatch_width, stopWatch_height,
		 stopWatch_bits, stopWatchMask_bits,
		 stopWatch_x_hot, stopWatch_y_hot,
		 &black, &white);

    /* VEM Grabbing Cursor */
    vemCursorTable[GRAB_CURSOR] =
      makeCursor(xv_disp(), drw, vem_box_width, vem_box_height,
		 vem_box_bits, vem_box_bits,
		 vem_box_x_hot, vem_box_y_hot,
		 curColor, &black);

    /* Finished */
}



/*
 * VEM Cursor Management
 *
 * The following routines provide a means for any routine anywhere
 * to get a hold of all VEM cursors.  The cursor names are defined
 * in general.h.
 */

Cursor vemCursor(curName)
int curName;			/* One of the #define's in general.h */
{
    if (vemCursorTable == (Cursor *) 0)
      vemInitCursors();
    if ((curName >= 0) && (curName < END_CURSORS)) {
	return vemCursorTable[curName];
    } else return (Cursor) 0;
}




void vemBusy()
/*
 * This routine changes the cursor in all active windows to the
 * 'busy' cursor.  It sets some global state to insure the
 * cursor remains set until 'vemNotBusy' is called.
 */
{
    Cursor busyCursor;

    if (busyFlag) return;
    busyFlag = 1;
    busyCursor = vemCursor(BUSY_CURSOR);
    XDefineCursor(xv_disp(), consWin, busyCursor);
    wnCursor(busyCursor);
    dds_override_cursor(busyCursor);
    XFlush(xv_disp());
}

void vemNotBusy()
/*
 * This routine changes the cursor in all active windows to
 * the 'normal' cursor.  It resets the global state and cleans
 * up the dirty work of 'vemBusy'.
 */
{
    if (!busyFlag) return;
    busyFlag = 0;
    XDefineCursor(xv_disp(), consWin, vemCursor(CONSOLE_CURSOR));
    wnCursor(vemCursor(CROSS_CURSOR));
    dds_override_cursor((Cursor) 0);
    XFlush(xv_disp());
}
