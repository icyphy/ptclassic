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
 * Internal Definitions for VEM Windows
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1988, 1989
 *
 * This file contains window specific definitions used across several
 * files in this directory.  The globally defined resources are
 * declared in wininternal.c.
 */

#ifndef WIN_INTERNAL
#define WIN_INTERNAL

#include "ansi.h"		/* ANSI definitions       */
#include "port.h"
#include "windows.h"		/* Self declaration       */
#include "tr.h"			/* Transformation package */
#include "da.h"			/* Dynamic arrays         */
#include "titlebar.h"		/* Titlebar management    */

/* Global transformation stack */
extern tr_stack *_wnTranStack;

/* Required height of all graphics window title bars */
extern int _wnTBHeight;


extern void _wnInit();
  /* Initializes the globals above */

/*
 * Windows may have more than one facet associated with them.  If
 * this is the case,  the first is considered to be the `primary'
 * and all the rest are considered `alternates'.  All alternates
 * must have the same scale factor and are drawn before the primary.
 * Below is the information unique to each facet specification.
 */

#define MAX_FACETS	2
#define W_PRI		0	/* Primary facet  */
#define W_MALT		1	/* Main alternate */

#define F_ULYR		0x01	/* Layer array requires update */

typedef struct fct_spec {
    octId winFctId;		/* Facet associated with window  */
    octCoord x_off, y_off;	/* Lower left hand corner in Oct */
    short fctbits;		/* Option bits for facet         */
    dary layers;		/* Dynamic array of tkLayer	 */
} wnFctSpec;

/*
 * The data item of the hash table of windows is given below.  Note
 * Note also that window names (also known as window titles) are 
 * stored as the name of the window itself (XStoreName) and is not 
 * explicitly stored in this structure.
 */

typedef struct intern_win {
    Window mainWin;		/* Overall VEM window                */
    int tw, th;			/* Overall width and height          */
    Window XWin;		/* Geometry display window           */
    int num_fcts;		/* Number of facets                  */
    wnFctSpec fcts[MAX_FACETS];	/* Associated facet information      */
    int width, height;		/* Size in display coordinates       */
    long scaleFactor;		/* If positive,  Oct coordinates per */
				/* display unit.  If negative,       */
				/* display units per Oct coordinate  */
				/* The factor is represented as a    */
				/* fixed point number where the low  */
				/* three bits are the fraction.      */
    bdTable altTable;		/* Alternate binding table           */
    Window TitleBar;		/* Titlebar window                   */
    tbHandle tb;		/* Title bar management handle       */
    wnOpts opts;		/* Window options (see above)        */
} internalWin;

/* Drawing loop globally passed parameters */
extern internalWin _wnWin;		/* Internal version of window data */
extern struct octBox _wnVirtual;	/* Extent in Oct coordinates       */
extern struct octBox _wnPhysical;	/* Extent in display coordinates   */

/*
 * Scale factor computations.  OCTSCALE takes a distance in display
 * coordinates and yields the distance in OCT coordinates.  DISPSCALE
 * does the opposite.  The scale factor has the following layout:
 *     1             28                3
 *    +-+----------------------------+---+
 *    | |                            |   |  32 bits
 *    +-+----------------------------+---+
 * The sign bit is used to distinguish between Oct units per pixel and
 * pixels per Oct unit.  If set,  the scale is the number of display
 * units per Oct coordinate.  If clear,  its the number of Oct coordinates
 * per display unit.  The middle 28 bits represent the integral
 * portion of the factor and the low three bits represent the fraction
 * (eighths of course).  These bits are useful when the scale factor
 * approaches 1 Oct unit per pixel (or vice-versa).  Additional bits
 * of fraction can be added by modifing SCALE_SHIFT.
 *
 * Method suggested by Peter Moore,  the mentor.
 */

#define SCALE_SHIFT	3
#define FRAC_VALUE	(1 << SCALE_SHIFT)

#ifdef OLD
/* Old scaling macros - not robust on all machines */

#define OCTSCALE(x, factor) \
((factor) < 0 ? ((x) << SCALE_SHIFT) / (-(factor)) \
              : ((x) * (factor)) >> SCALE_SHIFT)

#define DISPSCALE(x, factor) \
((factor) > 0 ? ((x) << SCALE_SHIFT) / (factor)    \
              : ((x) * (-(factor))) >> SCALE_SHIFT)
#endif

/* New scaling macros */
extern octCoord _wnSTemp;	/* Scale temporary */

#define OCTSCALE(x, factor) \
((factor) < 0 ? ((x) << SCALE_SHIFT) / (-(factor)) \
              : (((_wnSTemp = (x)) < 0) ? -((_wnSTemp * (-(factor))) >> SCALE_SHIFT ) \
		                        : (_wnSTemp * (factor)) >> SCALE_SHIFT))

#define DISPSCALE(x, factor) \
((factor) > 0 ? ((x) << SCALE_SHIFT) / (factor)    \
              : (((_wnSTemp = (x)) < 0) ? -(((_wnSTemp) * (factor)) >> SCALE_SHIFT) \
		                        : ((_wnSTemp) * (-(factor))) >> SCALE_SHIFT))

/*
 * The following macros translate virtual coordinates into physical
 * ones.   They use the real window information (realWin) and the
 * physical region of the window (physical).  They replace the
 * coordinate in OCT units with the value of the coordinate in
 * display units.  The coordinate is relative to the upper left
 * hand corner of the redisplay region.
 *
 * Under X11,  it is no longer necessary to translate the coordinates
 * to the redisplay region.  Thus,  `phy' is no longer used.  Thus,
 * for all practical purposes, TRANX and TRANY are the same as
 * TR_X and TR_Y.  The separation is presevered just in case we
 * have to go back to the old semantics.
 */

#define	TRANX(win, phy, num, xVal) \
(xVal) = DISPSCALE(xVal - (win).fcts[num].x_off, (win).scaleFactor)

#define TRANY(win, phy, num, yVal) \
(yVal) = (win).height - DISPSCALE(yVal-(win).fcts[num].y_off,(win).scaleFactor)


/*
 * These translate points to the display coordinate system 
 * of window 'theWin' assuming the window is the entire
 * physical region.  
 */

#define TR_X(win, num, val) \
(val) = DISPSCALE((val) - (win).fcts[num].x_off, (win).scaleFactor)

#define TR_Y(win, num, val) \
(val) = (win).height - DISPSCALE((val)-(win).fcts[num].y_off,(win).scaleFactor)

/*
 * The following gets around null dereference on suns
 */

#define ST_EXIST(tbl)	((tbl) ? st_count(tbl) : 0)

/*
 * The following is temporary until tr supports the routine
 * tr_is_identity.
 */

#define tr_is_identity(stk) \
((*stk) && ((*stk)->transform.translation.x == 0) && \
 ((*stk)->transform.translation.y == 0) && \
 ((*stk)->transform.transformType == OCT_NO_TRANSFORM))

/*
 * Internal routines used by routines in this directory
 * but should not be used outside the window module.
 */

#define WN_ACTIVE	0x01
#define WN_INACTIVE	0x02
#define WN_INTR		0x04

#define WN_MAIN		0
#define WN_GRAPH	1
#define WN_TITLE	2

extern void _wnInitTables();
  /* Initializes hash tables used by windows module (table.c) */

extern void _wnEnter
  ARGS((internalWin *realwin, int winstate));
  /* Enters a window structure into one of the tables (table.c) */

extern int _wnFind
  ARGS((Window win, internalWin **realwin, int *winstate, int *wintype));
  /* Finds a window and returns its struct, state, and type (table.c) */

extern void _wnOut
  ARGS((internalWin *realwin));
  /* Removes a window from the tables (table.c) */

extern void _wnChange
  ARGS((internalWin *realwin, int winstate));
  /* Changes the state of a window (table.c) */

extern void _wnForeach
  ARGS((int winstates, enum st_retval (*func)(), char *arg));
  /* Traverses tables calling a function for each entry (table.c) */

extern void _wnQRemove
  ARGS((internalWin *theWin));
  /* Removes requests for a particular window (queue.c) */

extern vemStatus _wnQRedraw
  ARGS((internalWin *theWin, int x, int y, int width, int height));
  /* Queues a redraw of a phyiscal portion of a window (queue.c) */

extern void _wnArrange
  ARGS((internalWin *realWin, int width, int height, int new_offset));
   /* Arranges windows as a graphics window and titlebar (create_modify.c) */

extern void _wnDownBits
  ARGS((tbHandle tb, int dspBits));
   /* Copies titlebar bits into titlebar widget (create_modify.c) */

/* Options to _wnRedraw */
#define DONT_CLR	0x01	/* Don't clear the region    */

extern vemStatus _wnRedraw
  ARGS((internalWin *theWin, struct octBox *virExtent, int n_fct, int opt));
   /* Draws a specified portion of a specified window (event_draw.c) */

extern void _wnObjDisp
  ARGS((octObject *geo, int retLength, struct octPoint *retPoints,
	int trflag, int manFlag, octObject *inst));
   /* Draws a specified object subject to transformation (event_draw.c) */

extern vemStatus _wnUnmap
  ARGS((Window win));
   /* Marks a window as unmapped */

extern vemStatus _wnMakeActive
  ARGS((Window win));
   /* Reactivates an inactive window */

extern void _wnLyrUpdate
  ARGS((internalWin *i_win, int fct_num));
   /* Recomputes the layer list for a window */

extern void argRedraw
  ARGS((internalWin *realWin, struct octBox *virtual, struct octBox *physical));
  /* Redraws argument objects in a region */

extern void objSetRedraw
  ARGS((atrHandle objattr, octId objfct, internalWin *realWin,
	struct octBox *virtual, struct octBox *physical));
  /* Like objRedraw except draws selected objects in a particular set */

#endif
