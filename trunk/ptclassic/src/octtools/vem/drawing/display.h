/* Version Identification:
 * $Id$
 */
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
 * VEM Graphics Display Header File
 *
 * David Harrison
 * University of California, 1985
 *
 * This file contains definitions for using the graphics window display
 * routines implemented in display.c
 */

#ifndef DISPHEADER
#define DISPHEADER

#include "ansi.h"
#include "general.h"
#include <X11/Xlib.h>
#include "oct.h"

extern char *dsp_pkg_name;

/*
 * Initialization
 */

extern void dspInit
  ARGS((Window win, int x, int y, int width, int height));
  /* Specify region of window to repaint */

/* 
 * Basic Oct drawing primitives
 */

extern void dspBox
  ARGS((struct octBox *oneBox));
  /* Draw Box */

extern void dspNonManBox
  ARGS((octCoord x1, octCoord y1, octCoord x2, octCoord y2, int size));
  /* Draws non-manhattan box for paths */

extern void dspPolygon
  ARGS((int numPoints, struct octPoint *polyPoints, int stipple, int shape));
  /* Draw polygon */

extern void dspCircle
  ARGS((struct octCircle *oneCircle));
  /* Draw arc or circle */

#define DSP_BOX		0x01	/* Always draw reference box */

extern void dspLabel
  ARGS((octObject *oneLabel, octObject *inst, int opt));
  /* Draw a label */

/*
 * Non-oct specific graphic primitives
 */

#define CLEAR_ALL	0x01	/* Clear everything disregarding _AGC */
extern void dspClear
  ARGS((int opt));
  /* Clears the region specified in dspInit() */

extern void dspOctagon
  ARGS((struct octCircle *oneCircle));
  /* Draw octagon for path endpoints */

#define DSP_LL		0x00	/* Label in lower left     */
#define DSP_UR		0x01	/* Label in upper right    */
#define DSP_CEN		0x02	/* Label in center         */
#define DSP_XMARK	0x04	/* Use a cross type marker */

extern void dspBound
  ARGS((struct octBox *box, STR name, int location, double thres, int min));
  /* Displays labeled bounding box */

extern void dspPoint
  ARGS((Window theWin, struct octPoint *thePoint, int opt));
  /* Draws a marker */

extern void dspLine
  ARGS((Window theWin, struct octPoint *point1, struct octPoint *point2));
  /* Draws an arbitrary line */

extern void dspRegion
  ARGS((Window theWin, struct octBox *oneBox));
  /* Draws an arbitrary box outline */

extern void dspEnd();
  /* Finish display sequence */

#endif
