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
 * Fill Pattern Manipulation Header
 *
 * David Harrison
 * University of California, 1985, 1987, 1989
 *
 * This file contains declarations for use of the fill pattern manipulation
 * package implemented in fillPat.c.  Other files which should be included
 * before this one:
 */

#ifndef FILLPAT
#define FILLPAT

#include "ansi.h"
#include "general.h"
#include <X11/Xlib.h>

/* Can be typedef'ed to void if supported */
typedef struct fill_dummy_defn {
    int dummy;
} fill_dummy;

typedef fill_dummy *flPattern;	/* Internal representation of a fill pattern */

extern flPattern flNewPattern
  ARGS((int width, int height, STR dataStr));
  /* Create a new fill pattern from external form */

extern void flSize
  ARGS((flPattern pat, int *width, int *height));
  /* Returns the current size of a fill pattern */

extern vemStatus flIsSolid
  ARGS((flPattern pat));
  /* Returns VEM_OK if the pattern is a solid fill,  VEM_FALSE otherwise */

extern void flXBitmap
  ARGS((Display *disp, Visual *vis, flPattern pat, int *width, int *height,
	char **data));
  /* Returns pattern in format suitable for making X bitmaps and pixmaps */

extern void flXLineStyle
  ARGS((flPattern pat, int *offset, int *len, char **list));
  /* Returns pattern in format suitable for using as X line style */

extern void flFreePattern
  ARGS((flPattern fillPat));
  /* Releases the resources associated with a pattern created by vemNewPattern */

extern flPattern flCopyPattern
  ARGS((flPattern pat));
  /* Returns a copy of `pat' */

extern flPattern flChangeSize
  ARGS((flPattern pat, int new_width, int new_height));
  /* Sizes a pattern to a specified size using clipping and replication */

extern vemStatus flEqual
  ARGS((flPattern pat1, flPattern pat2));
  /* Compares two fill patterns to see if they are equal */

#endif
