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
 * VEM OCT Window Management Include File
 *
 * David Harrison
 * University of California, 1985, 1988, 1989
 *
 * This file contains type definitions used by the VEM OCT window
 * management routines.  
 */

#ifndef WINDOWS_HEADER
#define WINDOWS_HEADER

extern char *wn_pkg_name;

#include "ansi.h"
#include <X11/Xlib.h>
#include "general.h"
#include "st.h"
#include "oct.h"
#include "bindings.h"
#include "commands.h"

#define VEM_GRIDLINES	0x0001	/* Grid lines drawn (if visible)             */
#define VEM_TITLEDISP	0x0002	/* Display window title                      */
#define VEM_EXPAND	0x0004	/* Expand instances in this window           */
#define VEM_REMOTERUN	0x0008	/* Remote application running in this window */
#define VEM_SHOWDIFF	0x0010	/* Show difference coordinates               */
#define VEM_SHOWABS	0x0020	/* Show absolute coordinates                 */
#define VEM_SHOWBITS	0x0040	/* Show status bits                          */
#define VEM_SHOWEDIT	0x0080	/* Show edit cell name                       */
#define VEM_SHOWCON	0x0100	/* Show context cell name                    */
#define VEM_SHOWBB	0x0200	/* Show instance bounding boxes              */
#define VEM_SHOWRPC     0x0400  /* Show RPC application name                 */
#define VEM_NODRAW	0x0800	/* Make window inactive                      */
#define VEM_MANLINE	0x1000	/* Manhattan line arguments                  */
#define VEM_GRAVITY	0x2000	/* Argument gravity for symbolic/schematic   */
#define VEM_SHOWAT	0x4000  /* Show actual terminals                     */
#define VEM_DOTGRID	0x8000	/* Display dotted grid                       */
#define VEM_EXPERT     0x10000	/* Expert mode of operation (AC)	     */

#define VEM_WNALL      0x1FFFF	/* All possible bits */

/*
 * Options structure exported by the windows module
 */

typedef struct win_options {
    int disp_options;		/* Display option bits (window.h)    */
    int lambda;			/* Oct units per lambda              */
    int snap;			/* Input coordinates snapped to this */
    /* Grid related options */
    int grid_base;		/* Logarithmic base of grid lines    */
    int grid_minbase;		/* Minimum base of logarithmic grid  */
    int grid_majorunits;	/* Major grid line frequency         */
    int grid_minorunits;	/* Minor grid line frequency         */
    int grid_diff;		/* Difference before grid not displayed */
    /* Layer display */
    st_table *off_layers;	/* Hash table of layers that are off */
    /* Display related options */
    double bb_thres;		/* Bounding box name threshold       */
    int bb_min;			/* Abstraction level of bounding boxes */
    int con_min;		/* Abstraction level of contacts     */
    int solid_thres;		/* Objects drawn solid at this level */
    char *interface;		/* Interface facet to display        */
    int inst_prio;		/* Instance priority level           */
} wnOpts;

/*
 * create_modify.c
 *
 * Creating and modifying Oct windows
 */

extern vemStatus wnGrab
  ARGS((Window win, octObject *winFacet, struct octBox *winExtent,
	unsigned long brdr, unsigned long bkgd));
  /* Takes over ownership of a previously created window */

extern vemStatus wnDup
  ARGS((Window win, Window oldWin, struct octBox *newExtent,
	unsigned long brdr, unsigned long bkgd));
  /* Grab a window by duplicating the attributes of another */

extern vemStatus wnRelease
  ARGS((Window win));
  /* Releases control of a previously created window     */

extern vemStatus wnBufRelease
  ARGS((octId fctId, vemStatus (*delFunc)()));
  /* Releases all windows looking at a particular buffer */

extern vemStatus wnSwitchBuf
  ARGS((octId oldFctId, octId newFctId));
  /* Replaces all occurances of one buffer with another */

extern vemStatus wnNewBuf
  ARGS((Window win, octId fctId, struct octBox *extent));
  /* Changes the buffer of a given window */

extern vemStatus wnAltBuf
  ARGS((Window win, octId altFct, struct octPoint *altPnt,
	struct octPoint *mainPnt));
  /* Specifies a buffer as a background of a given window */

extern vemStatus wnSwapBuf
  ARGS((Window win));
  /* Makes the main buffer the background and vice-versa */

/*
 * event_draw.c
 *
 * Handles window events and draws geometry in windows
 */

extern vemStatus wnFilter
  ARGS((XEvent *evt));
  /* Handles graphic window events */

/*
 * queue.c
 *
 * Queues up redraw requests that are eventually
 * serviced by event_draw.c
 */

extern vemStatus wnQWindow
  ARGS((Window win));
  /* Queues up a full redraw of a specified window */

extern vemStatus wnQRegion
  ARGS((octId theFctId, struct octBox *theExtent));
  /* Queues up a redraw of a virtual OCT region (all windows)      */

#ifndef OLD_DISPLAY
extern vemStatus wnQRedisplay
  ARGS((octId theFctId, struct octBox *theExtent));
  /* Queues up a redraw of a virtual OCT region (all windows)      */
#endif

extern vemStatus wnQSelect
  ARGS((vemObjects *objset, struct octBox *extent));
  /* Queues up selection information if its on its own plane       */

extern vemStatus wnForceRedraw
  ARGS((Window win, int x, int y, int width, int height));
  /* Queues up physical region of window regardless of window state */

extern vemStatus wnRefresh();
  /* Refreshes all VEM graphics windows */

extern vemStatus wnQuickFlush();
  /* Collapses queued redraw events and carries out the redraws    */
extern vemStatus wnFlush();
  /* Expansive call that handles also change lists    */

/*
 * trans.c
 *
 * Handles coordinate transformations including zoom and pan
 */

extern vemStatus wnPan
  ARGS((Window win, struct octPoint *panPoint));
  /* Pans window to virtual point    */

extern vemStatus wnZoom
  ARGS((Window win, struct octBox *octExtent));
  /* Changes extent of window to new virtual region */

extern vemStatus wnAssignScale
  ARGS((Window destination, Window source));
  /* Makes two windows the same scale */

#define	VEM_NOSNAP	0x01	/* Do not snap point to grid */
#define VEM_SETPNT	0x02	/* Make this a key point     */
#define VEM_DPNT	0x04	/* Update relative coordinate*/

extern vemStatus wnPoint
  ARGS((Window win, int x, int y, octId *retFct,
	struct octPoint *newPoint, int options));
  /* Transforms physical point into virtual point */

extern vemStatus wnEq
  ARGS((Window win, struct octPoint *p1, struct octPoint *p2,
	int tol, long *dist));
  /* Compares OCT points with a pixel tolerance  */

extern vemStatus wnGravSize
  ARGS((Window win, struct octPoint *pnt, struct octBox *bb));
  /* Computes gravity region for a window */

/*
 * options.c
 *
 * Handles window options
 */

extern vemStatus wnGetInfo
  ARGS((Window win, wnOpts *opts, octId *mainFct, octId *altFct,
	bdTable *altbind, struct octBox *outExtent));
  /* Return window information */

extern int vemExpert
  ARGS(( vemPoint* spot ));
  /* Return status of expert moe flag in window options */

extern vemStatus wnSetInfo
  ARGS((Window win, wnOpts *newopts, bdTable *altbind));
  /* Set various window parameters */

extern vemStatus wnCursor
  ARGS((Cursor theCursor));
  /* Changes the cursor in all active windows */

extern vemStatus wnZeroAltTbl
  ARGS((bdTable theTbl));
  /* Zeros the alternate bindings of windows with a certain table */

extern Window wnGrabWindow
  ARGS((Window win));
  /* Returns the top-level window of a given window */

/* Turning off windows */
extern vemStatus wnMakeInactive
  ARGS((Window win));
  /* Makes a window inactive */

extern vemStatus wnSetRPCName
  ARGS((Window win, STR name));
  /* Sets the RPC name field for a window */


/*
 * layers.c
 *
 * Window layer table maintenence
 */

extern void wnLyrChange
  ARGS((octId fct_id));
   /* Causes updates to window layer tables */

#endif
