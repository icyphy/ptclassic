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
#include "copyright.h"
#ifndef VULCAN_H
#define VULCAN_H

extern char vulcanPkgName[];
#define VULCAN_PKG_NAME	vulcanPkgName

/* type of debugging information to produce */
typedef enum {
    VULCAN_NO_DEBUG,		/* print no debugging geometrical info */
    VULCAN_COUNT_DEBUG		/* per-layer vertex count for frame and terms */
} vulcanDebugType;

/* flag bits */
#define VULCAN_DO_SUBCELLS	0x1	/* update all cells in hierarchy */
#define VULCAN_DESTROY_OLD	0x2	/* kill facet and build from scratch */
#define VULCAN_CLOSE_INPUT	0x4	/* close input facet to save memory */
#define VULCAN_GLOBAL_FRAME     0x8     /* create global frame (layer PLACE) */
#define VULCAN_UNFILL_FRAMES	0x100	/* don't fill in enclosed spaces */
#define VULCAN_BOX_FRAMES	0x200	/* boxes, not polygons, for frames */
#define VULCAN_CUT_BACK_FRAMES	0x400	/* ***EXPERIMENTAL***   don't grow a
					 * frame so it abuts a terminal
					 */
#define VULCAN_DFLT_FLAGS	VULCAN_GLOBAL_FRAME

#define VULCAN_DFLT_MIN_FEATURE	10.0	/* minimum % notch size in frame */

/* message type */
typedef enum {
    VULCAN_PARTIAL,	/* partial message; more to follow */
    VULCAN_DEBUGGING,	/* debug information as requested */
    VULCAN_INFORMATION,	/* advisory -- progress reports, etc.  */
    VULCAN_WARNING,	/* warning -- reasonably correctable problem found */
    VULCAN_SEVERE,	/* severe -- continuing, but results questionable */
    VULCAN_FATAL        /* fatal -- XXX obsolete XXX */
} vulcanMessageType;

extern vulcanMessageType vulcan(
    /* octFacet *inputFacet;	*/
    /* octFacet *outputFacet;	*/
);
extern vulcanMessageType vulcanParameterized(
    /* octFacet *inputFacet;	*/
    /* octFacet *outputFacet;	*/
    /* int flags;		*/
    /* double minFeatureSize;	*/
    /* vulcanDebugType debug;	*/
);
extern void vulcanMessageHandler(
    /* void (*handler)(vulcanMessageType type, char *message);	*/
);

#endif /* VULCAN_H */
