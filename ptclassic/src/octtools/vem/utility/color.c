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
/*
 * VEM Color Utilities
 *
 * David Harrison
 * University of California,  Berkeley
 * 1989
 *
 * This file contains some general purpose utilities for
 * dealing with color.
 */

#include "vemUtil.h"
#include "xvals.h"

unsigned long vuWhite(clr)
XColor *clr;			/* Returned color structure */
/*
 * Returns the white pixel for the display.  If `clr' is
 * non-zero,  it will return the full color structure
 * for the white pixel.
 */
{
    static XColor white;
    static int init = 0;

    if (!init) {
	white.pixel = WhitePixel(xv_disp(), xv_scrn());
	XQueryColor(xv_disp(), xv_cmap(), &white);
    }
    if (clr) *clr = white;
    return white.pixel;
}


unsigned long vuBlack(clr)
XColor *clr;			/* Returned color structure */
/*
 * Returns the black pixel for the display.  If `clr' is
 * non-zero,  it will return the full color structure
 * for the black pixel.
 */
{
    static XColor black;
    static int init = 0;

    if (!init) {
	black.pixel = BlackPixel(xv_disp(), xv_scrn());
	XQueryColor(xv_disp(), xv_cmap(), &black);
    }
    if (clr) *clr = black;
    return black.pixel;
}
