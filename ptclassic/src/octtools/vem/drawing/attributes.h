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
 * VEM Attribute Manager Header File
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1988
 *
 * This file contains definitions for the programming interface to
 * the attribute manager.  The attribute manager defines and maintains
 * the display operations for all primitives drawn using X11.  The module
 * itself is implemented in attributes.c.
 */

#ifndef	ATTR_HEADER
#define ATTR_HEADER

#include "ansi.h"
#include "port.h"
#include <X11/Xlib.h>
#include "fill_pat.h"

extern char *attr_pkg_name;

/* Can be typedef'ed to void if supported */
typedef struct attr_dummy_defn {
    int dummy;
} attr_dummy;

typedef attr_dummy *atrHandle;		/* Attributes for drawing */

/* 
 * The following structure is exported to the world as the `current
 * graphics context'.  
 */

#define ATR_DONT	-1	/* Don't fill/draw        */
#define ATR_SOLID	0	/* Always solid draw/fill */

typedef struct atr_context {
    Display *disp;		/* Current display                */
    GC solid_gc;		/* For solid drawing              */
    GC line_gc;			/* For stylized lines             */
    GC stipple_gc;		/* For stipple/dashed drawing     */
    GC clear_gc;		/* For clearing the area          */
    int fill_thres;		/* Solid fill threshold           */
    int line_thres;		/* Solid line threshold           */
    int line_width;		/* Used for optimization          */
} atrContext;

/* ---------------------------- AGC IS EXPORTED HERE ------------------- */
extern atrContext _AGC;

extern int atrInit
  ARGS((Display *disp, unsigned int planes, unsigned int colors));
  /* Initializes the attribute package with information about display */

extern vemStatus atrNew
  ARGS((atrHandle *newattr, XColor *col, flPattern fill, flPattern lstyle,
	int l_width));
  /* Creates an attribute from user level information */
extern vemStatus atrReplace
  ARGS((atrHandle oldattr, XColor *col, flPattern fill, flPattern l_pat,
	int l_width));
  /* Replaces an existing attribute with new information */
extern vemStatus atrSelect
  ARGS((atrHandle *newattr, unsigned long *mask, XColor *col, flPattern fill,
	flPattern l_pat, int l_width));
  /* Creates attributes for VEM selected sets */

extern void atrSet
  ARGS((atrHandle attr));
  /* Sets the current graphics context to the specified attributes */
extern void atrSetXor
  ARGS((atrHandle attr));
  /* Sets attributes but uses xor display function (for arguments) */

extern void atrFont
  ARGS((XFontStruct *font));
  /* Sets the current font in the graphics context                 */
extern void atrClip
  ARGS((int x, int y, int width, int height));
  /* Sets the current clipping region of the context               */

extern vemStatus atrIsPlane
  ARGS((atrHandle attr));
  /* Returns VEM_OK if the attribute is on its own plane */
extern unsigned long atrQBg();
  /* Returns the background pixel used in attributes module */

extern void atrRelease
  ARGS((atrHandle attr));
  /* Releases the resources of an attribute    */

#endif
