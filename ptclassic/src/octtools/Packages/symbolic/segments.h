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
 * Creating new segments in symbolic
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains code for creating new segments in a symbolic
 * cell.  Connectivity information is updated appropriately.  This
 * file contains definitions for using the segment functions.
 */

#ifndef _SEGMENTS_H_
#define _SEGMENTS_H_

#include "ansi.h"
#include "contact.h"

extern void symSegments
  ARGS((octObject *facet, octObject *first, octObject *last,
	int np, struct octPoint *points,
	int nl, octObject layers[],
	int nw, octCoord widths[],
	symConFunc *confun));
  /* Creates a series of new symbolic segments */

extern void symSegChange
  ARGS((octObject *fct, octObject *seg, octObject *new_lyr,
	octCoord new_width, symConFunc *confun));
  /* Changes the layer or width of an existing segment */

extern void symSegDesc
  ARGS((octObject *seg, struct octPoint *ref, tapLayerListElement *elem));
  /* Returns a tap style description of a segment */

extern int symSegConnP
  ARGS((octObject *term, tapLayerListElement *elem));
  /* Returns non-zero if terminal is appropriate for segment description */

extern int symSegBreakP
  ARGS((octObject *seg, tapLayerListElement *elem,
	struct octPoint *isect, symConFunc *confun));
  /* Returns non-zero if `seg' can be broken by `elem' */

#endif /* _SEGMENTS_H_ */
