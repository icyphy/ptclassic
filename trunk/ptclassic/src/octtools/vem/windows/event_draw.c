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
 * VEM Oct Management Routines
 * Event handling and drawing code
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This module contains code for handling X events on windows
 * and drawing the geometric information in the graphics portion
 * of these windows.
 *
 * The geometric portion of Oct windows are called Viewports.
 * Viewports are windows which look at a selectable region of an OCT
 * facet.  The facet itself may be much larger (in terms of coordinate
 * extent) then any possible viewport window.  Such windows require
 * transformation and scaling and support operations like pan and zoom.
 * These windows are used to view OCT circuit views (layout, schematics, etc).
 *
 * For OCT style windows,  normal OCT unit
 * to display unit scaling is done using the simple scheme of mapping
 * a fixed point number of OCT units to display units (or vice-versa).
 * Instance transformation is carried out using the transformation
 * package "tr" written by Peter Moore.
 *
 * Graphics windows respond to a large number of events.  The wnFilter
 * routine handles ExposeRegion, UnmapWindow, EnterWindow, LeaveWindow,
 * and MouseMoved events in these windows.  Note that KeyPressed
 * and ButtonPressed events are handled as argument entry events in
 * the argument processing code in vem.c.
 */

#define SUPRESS_SMALL_INSTANCES

#include "wininternal.h"	/* Internal Window definitions */
#include "display.h"		/* Display primitives          */
#include "tech.h"		/* Technology definitions      */
#include "message.h"		/* Message handling            */
#include "region.h"		/* Region package              */
#include "buffer.h"		/* Buffer handling             */
#include "objsel.h"		/* Object selection            */
#include "argsel.h"		/* Argument selection          */
#include "defaults.h"		/* Default handling package    */
#include "tr.h"			/* Transformation package      */
#include "da.h"			/* Dynamic array package       */

/* Non package functions */
extern vemStatus vemIntrCheck();

#define GRID_PRIORITY	0	/* Hard-coded grid priority */
#define TOPLEVEL	0x01	/* Option to wnViewDisp     */

/* Globally passed drawing loop information */

internalWin _wnWin;		/* Internal version of window data */
struct octBox _wnVirtual;	/* Extent in Oct coordinates       */
struct octBox _wnPhysical;	/* Extent in display coordinates   */
static int wnFctNum;		/* What facet number               */
static tkLayer wnLyr;		/* What layer are we drawing       */
static int wnLastLayer;		/* If set,  we are drawing the last layer */

/* Forward declarations for lint */

static void wnInvNonMan();
  /* Inverts the transformation of a non-manhattan box */
static void wnNonManBox();
  /* Rotation/mirroring of a non-manhattan box */
static void wnPathDisp();
  /* Path primitive */
static void wnGridDisp();
  /* Draws a grid in a specified window */

static vemStatus wnViewDisp();
  /* Draws a specified region of graphics window */
static vemStatus wnInstDisp();
  /* Draws instances at a given level */
static vemStatus wnTermDisp();
  /* Draws terminals */

static void wnBadMaster();
  /* Registers a bad master */
static void wnBadList();
  /* Lists bad masters */



/*
 * Global drawing parameters
 */

static int draw_param_init = 0;

/* Attributes of grid lines */
static atrHandle zeroAttr;
static atrHandle majLineAttr;
static atrHandle majDotAttr;
static atrHandle minLineAttr;
static atrHandle minDotAttr;

/* Attributes of terminals and instance bounding boxes */
static atrHandle formalAttr;
static atrHandle actualAttr;
static atrHandle instAttr;

/* Selection attribute */
static atrHandle selAttr;



static void param_init()
/*
 * Initializes the drawing parameters from defaults given
 * in the defaults package.
 */
{
    /* Define the grid line attributes */
    dfGetAttr("grid.zero", &zeroAttr);
    dfGetAttr("grid.major.line", &majLineAttr);
    dfGetAttr("grid.major.dot", &majDotAttr);
    dfGetAttr("grid.minor.line", &minLineAttr);
    dfGetAttr("grid.minor.dot", &minDotAttr);

    /* Define formal and actual terminal attributes */
    dfGetAttr("formal", &formalAttr);
    dfGetAttr("actual", &actualAttr);

    /* Instance bounding box attribute */
    dfGetAttr("inst", &instAttr);

    /* Primary selection attribute */
    dfGetAttr("select", &selAttr);

    draw_param_init = 1;
}



vemStatus wnFilter(evt)
XEvent *evt;			/* Event to process */
/*
 * This routine handles certain events associated with VEM graphics
 * windows.  If the event was handled properly,  it will return VEM_OK.
 * If the event is not a VEM graphics window event or the routine cannot
 * handle the type of the event,  the routine returns VEM_FALSE.  If
 * some terrible error occurred while processing the event,  it
 * will return VEM_CORRUPT.
 */
{
    Window root_rtn, child_rtn;
    internalWin *theWin;
    struct octPoint thePoint;
    octId fctId;
    int draw_flag = 1;
    int winstate, wintype;
    int root_x, root_y, win_x, win_y;
    unsigned int win_w, win_h, win_bdr, win_d;
    unsigned int mask;

    /* See if its our window */
    if (!_wnFind(evt->xany.window, &theWin, &winstate, &wintype)) {
	return VEM_FALSE;
    }
    if (winstate == WN_INACTIVE) {
	_wnChange(theWin, winstate = WN_ACTIVE);
    }
    draw_flag = (winstate == WN_ACTIVE);
    switch (evt->type) {
    case Expose:
	if ((wintype == WN_GRAPH) && draw_flag) {
	    (void) _wnQRedraw(theWin, evt->xexpose.x, evt->xexpose.y,
			      evt->xexpose.width, evt->xexpose.height);
	} else if ((wintype == WN_TITLE) && (evt->xexpose.count == 0)) {
	    tbRedraw(theWin->tb);
	}
	break;
    case GraphicsExpose:
    case NoExpose:
	/* We should not get these but note them anyway */
	vemMsg(MSG_A, "warning: wnFilter received an unsolicited GraphicsExpose or NoExpose event.\n");
	return VEM_FALSE;
    case MotionNotify:
	/* PointerMotionHint reduces spurious events - but requires XQueryPointer */
	if (XQueryPointer(evt->xmotion.display, evt->xmotion.window,
			  &root_rtn, &child_rtn, &root_x, &root_y,
			  &win_x, &win_y, &mask) &&
	    (theWin->opts.disp_options & VEM_TITLEDISP) &&
	    ((theWin->opts.disp_options & VEM_SHOWDIFF) ||
	     (theWin->opts.disp_options & VEM_SHOWABS)))
	  {
	      (void) wnPoint(evt->xany.window, win_x, win_y, &fctId, &thePoint, 0);
	  }
	break;
    case ConfigureNotify:
	/* Change in window size */
	if ((evt->xany.window == theWin->mainWin) &&
	    ((theWin->tw != evt->xconfigure.width) ||
	     (theWin->th != evt->xconfigure.height))) {
	    _wnArrange(theWin, evt->xconfigure.width, evt->xconfigure.height, 1);
	}
	break;
    case MapNotify:
	/* Get geometry information */
	if ((evt->xany.window == theWin->mainWin) &&
	    (XGetGeometry(evt->xmap.display, evt->xmap.window,
			  &root_rtn, &win_x, &win_y, &win_w, &win_h,
			  &win_bdr, &win_d)) &&
	    ((theWin->tw != win_w) || (theWin->th != win_h))) {
	    _wnArrange(theWin, win_w, win_h, 1);
	}
	break;
    case UnmapNotify:
	if (evt->xany.window == theWin->mainWin) {
	    (void) _wnUnmap(evt->xany.window);
	}
	break;
    case CirculateNotify:
	vemMsg(MSG_L, "wnFilter: received CirculateNotify event.\n");
	break;
    case DestroyNotify:
	vemMsg(MSG_L, "wnFilter: received CirculateNotify event.\n");
	break;
    case GravityNotify:
	vemMsg(MSG_L, "wnFilter: received GravityNotify event.\n");
	break;
    case ReparentNotify:
	vemMsg(MSG_L, "wnFilter: received ReparentNotify event.\n");
	break;
    default:
	return VEM_FALSE;
    }
    return VEM_OK;
}


#ifdef NOTDEF
/* For debugging */
static dary ll_list;
static void ll_set(ary, str)
dary ary;
char *str;
{
  printf("llset: %s\n", str);
  ll_list = ary;
}
static void ll_print()
{
    int foo, len;
    len = daLen(ll_list);
    printf("%d:", len);
    for (foo = 0;  foo < len;  foo++) {
	printf(" %lx", (long) *daGet(tkLayer, ll_list, foo));
    }
    printf("\n");
}
#endif



vemStatus _wnRedraw(theWin, virExtent, n_fct, opt)
internalWin *theWin;		/* Window to draw into       */
struct octBox *virExtent;	/* Extent in Oct coordinates */
int n_fct;			/* Number of facet           */
int opt;			/* Options                   */
/*
 * This routine repaints the specified OCT region of the specified
 * facet for the given window.  If the region does not intersect the
 * current display region of the window, the request is ignored.  If
 * the window is not mapped,  the routine ignores redraw requests on
 * the window.  There is one option: DONT_CLR.  If the facet number
 * is greater than zero,  the draw is done using alternate colors.
 * If DONT_CLR is specified,  no clear is done before the redraw.
 */
{
    octObject facet;		/* Facet to redraw */
    int grid_not_done;		/* Grid hasn't been drawn */
    int lyr_num;		/* What layer to draw     */
    int intr_flag = 0;		/* Interrupted            */
    jmp_buf env;		/* Error enviroment       */

    /*
     * Display loop error handling -- causes window to become
     * disabled.
     */
    if (setjmp(env)) {
	/* Error detected - disable window and return */
	(void) wnMakeInactive(theWin->mainWin);
	return VEM_CORRUPT;
    } else {
	/* No error -- push context */
	vemPushErrContext(env);
    }

    if (!draw_param_init) param_init();
    if (!_wnTranStack) _wnTranStack = tr_create();
    /*
     * Due to integer rounding problems,  the OCT region passed in may
     * be too small if the scaleFactor is negative (many display units
     * per OCT coordinate.  To compensate,  we add a small pad to
     * the incoming OCT region.
     */
    _wnWin = *theWin;
    _wnVirtual = *virExtent;
    wnFctNum = n_fct;
    if (_wnWin.scaleFactor < 0) {
	_wnVirtual.lowerLeft.x -= 1;
	_wnVirtual.lowerLeft.y -= 1;
	_wnVirtual.upperRight.x += 1;
	_wnVirtual.upperRight.y += 1;
    }

    /* Translate the supplied OCT region to a physical region */
    _wnPhysical.lowerLeft.x =
      DISPSCALE(_wnVirtual.lowerLeft.x - _wnWin.fcts[wnFctNum].x_off,
		_wnWin.scaleFactor);
    _wnPhysical.lowerLeft.y = 1+ _wnWin.height -
      DISPSCALE(_wnVirtual.lowerLeft.y - _wnWin.fcts[wnFctNum].y_off,
		_wnWin.scaleFactor);
    _wnPhysical.upperRight.x =
      DISPSCALE(_wnVirtual.upperRight.x - _wnWin.fcts[wnFctNum].x_off,
		_wnWin.scaleFactor) + 1;
    _wnPhysical.upperRight.y = _wnWin.height -
      DISPSCALE(_wnVirtual.upperRight.y - _wnWin.fcts[wnFctNum].y_off,
		_wnWin.scaleFactor) - 1;

    /* Clip the physical region to the physical region of the window */

    _wnPhysical.lowerLeft.x = VEMMAX(_wnPhysical.lowerLeft.x, 0);
    _wnPhysical.upperRight.x = VEMMIN(_wnPhysical.upperRight.x, _wnWin.width);
    _wnPhysical.lowerLeft.y = VEMMIN(_wnPhysical.lowerLeft.y, _wnWin.height);
    _wnPhysical.upperRight.y = VEMMAX(_wnPhysical.upperRight.y, 0);

    /* If the region is exactly zero,  we make it a bit bigger */
    if (_wnPhysical.lowerLeft.x == _wnPhysical.upperRight.x) {
	(_wnPhysical.lowerLeft.x)--;  (_wnPhysical.upperRight.x)++;
    }
    if (_wnPhysical.lowerLeft.y == _wnPhysical.upperRight.y) {
	(_wnPhysical.lowerLeft.y)--;  (_wnPhysical.upperRight.y)++;
    }

    /* If the region is zero sized,  no redraw is done */
    if ((_wnPhysical.lowerLeft.x > _wnPhysical.upperRight.x) ||
	(_wnPhysical.lowerLeft.y < _wnPhysical.upperRight.y)) {
	vemPopErrContext();
	return VEM_OK;
    }

    /*
     * Translate the clipped physical region into a suitable virtual region.
     * To insure against integer rounding problems,  a small fudge factor
     * is added to make the region slightly larger.
     */
    _wnVirtual.lowerLeft.x = _wnWin.fcts[wnFctNum].x_off +
      OCTSCALE(_wnPhysical.lowerLeft.x, _wnWin.scaleFactor) - 1;
    _wnVirtual.lowerLeft.y = _wnWin.fcts[wnFctNum].y_off +
      OCTSCALE(_wnWin.height - _wnPhysical.lowerLeft.y, _wnWin.scaleFactor) - 1;
    _wnVirtual.upperRight.x = _wnWin.fcts[wnFctNum].x_off +
      OCTSCALE(_wnPhysical.upperRight.x, _wnWin.scaleFactor) + 1;
    _wnVirtual.upperRight.y = _wnWin.fcts[wnFctNum].y_off +
      OCTSCALE(_wnWin.height - _wnPhysical.upperRight.y, _wnWin.scaleFactor)+ 1;

    /*
     * Initialize the display sequence.  Note the same physical
     * region is used for all calls (including recursive calls)
     * to wnViewDisp.  Thus,  this is passed down globally as
     * _wnPhysical.
     */

    dspInit(_wnWin.XWin,
	    (int) _wnPhysical.lowerLeft.x, (int) _wnPhysical.upperRight.y,
	    (int) (_wnPhysical.upperRight.x-_wnPhysical.lowerLeft.x),
	    (int) (_wnPhysical.lowerLeft.y-_wnPhysical.upperRight.y));

    if (!(opt & DONT_CLR)) {
	dspClear(CLEAR_ALL);
    }

    facet.objectId = _wnWin.fcts[wnFctNum].winFctId;
    if (octGetById(&facet) != OCT_OK) {
	errRaise("vem", 0, "Can't get facet for redraw:\n  %s\n",
		 octErrorString());
	/*NOTREACHED*/
    }

    /*
     * Begin priority redisplay layer traversal
     */
    if (_wnWin.fcts[wnFctNum].fctbits & F_ULYR) {
	_wnLyrUpdate(theWin, wnFctNum);
    }
    grid_not_done = 1;
    wnLastLayer = 0;
    for (lyr_num = 0; lyr_num < daLen(_wnWin.fcts[wnFctNum].layers); lyr_num++) {

	wnLyr = *daGet(tkLayer, _wnWin.fcts[wnFctNum].layers, lyr_num);

	/* Check for grid display */
	if ((wnFctNum == 0) && grid_not_done &&
	    (wnLyr->priority > GRID_PRIORITY)) {
	    wnGridDisp();
	    grid_not_done = 0;
	}

	if (lyr_num == daLen(_wnWin.fcts[wnFctNum].layers)-1) wnLastLayer = 1;

	/*
	 * Set up instance transformation.  wnViewDisp pops the current
	 * transformation before it returns.
	 */

	tr_push(_wnTranStack);
	tr_identity(_wnTranStack);

	if (wnViewDisp(&facet, (octObject *) 0, 0) == VEM_INTERRUPT) {
	    intr_flag = 1;
	    break;
	}
    }

    if (!intr_flag) {
	if (grid_not_done) {
	    wnGridDisp();
	}

	/* Object selection update */
	if (!intr_flag) objRedraw();

	/* Argument selection update */
	atrSetXor(selAttr);
	argRedraw(theWin, &_wnVirtual, &_wnPhysical);
    }

    /* End the display sequence */
    dspEnd();

    /* Bad master messages */
    wnBadList(&facet);

    /* Error context pop */
    vemPopErrContext();

    return VEM_OK;
}





static vemStatus wnViewDisp(facet, inst, level)
octObject *facet;		/* Facet to draw from      */
octObject *inst;		/* Derived instance        */
int level;			/* Level in the heirarchy  */
/*
 * This routine repaints a specified region of a VEM viewport window.
 * The redraw window is given by the global _wnWin.  The facet of
 * this window is given by the global wnFctNum.  The region in Oct
 * space is given in the global _wnVirtual.  The physical region of the
 * window is given in the global _wnPhysical.  Only geometry on the
 * layer given by the global wnLyr is drawn. This is a recursive routine
 * which calls on itself to draw instances of the supplied facet.  To
 * transform points properly,  the routine uses a global transformation stack
 * ("_wnTranStack") supplied by the "tr" package written by P. Moore.
 * The routine uses the region package (region.c) to return geometries
 * in the supplied virtual region. If this is a recursive call,  an
 * inverse transform is carried out on the virtual region to yield a
 * box relative to the master.  Once a geometry is obtained,  it is
 * first transformed into virtual space and then scaled into display
 * units.  The routine pops off the current transformation from
 * "_wnTranStack" just before returning.  The 'level' parameter
 * indicates the level in the heirarchy of the display.  If it
 * is greater than zero,  alternate colors will be used.  Note the routine
 * may return VEM_INTERRUPT which implies that the stack should be
 * unwound to stop the current display sequence.  Evaluating label
 * expressions properly requires that the instance opened to examine
 * this cell be provided in the parameter `inst'.  At the top most 
 * level,  it can be specified as zero.
 */
{
    XEvent evt;
    struct octObject theLayer;	/* OCT layer object                  */
    struct octObject geo;	/* Geometry to draw          	     */
    struct octBox invVirtual;	/* Inverse transform of 'virtual'    */
    regGenerator theGen;	/* For generating geometry           */
    struct octPoint *retPoints;	/* Points for polygon or path        */
    struct octPoint aPoint;	/* Point for retPoints		     */
    int retLength = 0;		/* Length of path or polygon         */
    int manFlag = 0;		/* Manhattan transformation          */
    int trflag = 0;		/* Whether it's worth it to transform */
    int lyrDraw = 0;		/* Whether layer should be drawn     */
    octCoord temp;		/* Temporary for box normalization   */

    /*
     * By inverse transforming the virtual region,  we get an absolute
     * region with respect to `facet'.   This will be used for
     * generating the proper geometries.  If the transform is non-manhattan,
     * we have to work a little harder by transforming the whole box and
     * then computing its bounding box.
     */

    manFlag = tr_is_manhattan(_wnTranStack);
    if (manFlag) {
	invVirtual = _wnVirtual;
	tr_inverse_transform(_wnTranStack, &invVirtual.lowerLeft.x,
			     &invVirtual.lowerLeft.y);
	tr_inverse_transform(_wnTranStack, &invVirtual.upperRight.x,
			     &invVirtual.upperRight.y);
	VEMBOXNORM(invVirtual, temp);
    } else {
	/* Non manhattan */
	wnInvNonMan(&_wnVirtual, &invVirtual);
    }

    /*
     * First,  we draw the instances for this facet.
     */

    if (wnInstDisp(facet, &invVirtual, level) == VEM_INTERRUPT) {
	tr_pop(_wnTranStack);
	return VEM_INTERRUPT;
    }

    /*
     * Interruptable redisplay check.  Here we see if the window
     * has been interrupted or iconified.  If interrupted,
     * we make the window inactive.  If iconified,  we simply
     * make the window unmapped.
     */
    if (vemIntrCheck(&evt) != VEM_OK) {
	if (evt.type == KeyPress) {
	    /* Interrupted Window */
	    (void) wnMakeInactive(evt.xany.window);
	    if ((_wnWin.mainWin == evt.xany.window) ||
	        (_wnWin.TitleBar == evt.xany.window) ||
		(_wnWin.XWin == evt.xany.window)) {
		tr_pop(_wnTranStack);
		return VEM_INTERRUPT;
	    }
	} else {
	    /* Iconified Window */
	    (void) _wnUnmap(evt.xany.window);
	    if ((_wnWin.mainWin == evt.xany.window) ||
		(_wnWin.TitleBar == evt.xany.window) ||
		(_wnWin.XWin == evt.xany.window)) {
		tr_pop(_wnTranStack);
		return VEM_INTERRUPT;
	    }
	}
    }

    /*
     * Now draw geometry on the passed layer wnLyr.
     */

    theLayer.type = OCT_LAYER;
    if (bufLyrId(facet->objectId, wnLyr, &(theLayer.objectId)) == VEM_OK) {
	/* Check to see if this layer should be drawn */
	lyrDraw = (_wnWin.opts.off_layers ?
		   (VEM_OCTCK(octGetById(&theLayer)) ?
		    !st_is_member(_wnWin.opts.off_layers,
				  theLayer.contents.layer.name) : 0)
		   : 1);

	if (lyrDraw) {
	    regInit(&theLayer, &invVirtual, OCT_GEO_MASK, &theGen);

	    /* Set up drawing attributes */
	    atrSet(tkAttr(wnLyr, wnFctNum+level));

	    /* Note I don't expect any terminals */
	    trflag = !tr_is_identity(_wnTranStack);
	    retPoints = &aPoint;
	    while (regNext(&theGen, &geo, &retLength, &retPoints,
			   (struct octBox *) 0) == REG_OK)
	      {
		 _wnObjDisp(&geo, retLength, retPoints, trflag, manFlag, inst);
	      }
	    regEnd(&theGen);
	}
    }
    /* Handle terminals - if any */
    if (wnLastLayer && (level < 2) &&
	((level == 1) ? _wnWin.opts.disp_options & VEM_SHOWAT : 1)) {
	(void) wnTermDisp(facet, &invVirtual, level);
    }

    /* Pop transformation and return */
    tr_pop(_wnTranStack);
    return VEM_OK;
}



/* Macro to update result bounding box */

#define UPBB \
    tr_inverse_transform(_wnTranStack, &onePoint.x, &onePoint.y); \
    if (onePoint.x < invreg->lowerLeft.x)  \
      invreg->lowerLeft.x = onePoint.x;    \
    if (onePoint.x > invreg->upperRight.x) \
      invreg->upperRight.x = onePoint.x;   \
    if (onePoint.y < invreg->lowerLeft.y)  \
      invreg->lowerLeft.y = onePoint.y;    \
    if (onePoint.y > invreg->upperRight.y) \
      invreg->upperRight.y = onePoint.y


static void wnInvNonMan(realreg, invreg)
struct octBox *realreg;		/* Real region to display                     */
struct octBox *invreg;		/* Inverted region in master coordinate space */
/*
 * This routine is used to transform the given display region (realreg)
 * into the coordinate space of a master when the transformation is
 * non-manhattan.  The result is placed in 'invreg'.  The approach is
 * to invert the transformation at all four corners and return the bounding
 * box of the result.
 */
{
    struct octPoint onePoint;

    /* Non manhattan */
    invreg->lowerLeft.x = invreg->lowerLeft.y = VEMMAXINT;
    invreg->upperRight.x = invreg->upperRight.y = VEMMININT;
    onePoint = realreg->lowerLeft;
    UPBB;
    onePoint = realreg->upperRight;
    UPBB;
    onePoint.x = realreg->lowerLeft.x;
    onePoint.y = realreg->upperRight.y;
    UPBB;
    onePoint.x = realreg->upperRight.x;
    onePoint.y = realreg->lowerLeft.y;
    UPBB;
}



void _wnObjDisp(geo, retLength, retPoints, trflag, manFlag, inst)
register octObject *geo;	/* OCT object to draw            */
int retLength;			/* Length of path or polygon     */
register struct octPoint *retPoints;	/* Points for polygon or path    */
int trflag;			/* If non-zero, transform else dont */
int manFlag;			/* Manhattan flag                */
octObject *inst;		/* Parent instance (if any)      */
/*
 * This routine draws a geometric OCT object using the low level
 * routines found in display.c.  In addition to the parameters
 * passed,  the routine uses the current transformation stack
 * '_wnTranStack' and assumes a display sequence and layer has been set up
 * using dspInit() and atrSet().  This routine is used by
 * the main drawing routine and the selection drawing routines
 * in objsel.c.  Note that tr_is_identity is not part of the
 * offical tr package and is included at the end of this file.
 * The routine now uses the globals _wnWin and _wnPhysical as well.
 * The `inst' parameter specifies the instance whose master contains
 * the object displayed.  It is used for displaying computable
 * labels appropriately.  It may be zero if the cell is not
 * part of a hierarchy.
 */
{
    octCoord temp;
    register int index;
    double angle;

    switch (geo->type) {
    case OCT_BOX:
	if (manFlag) {
	    if (trflag) {
		tr_transform(_wnTranStack,
			     &(geo->contents.box.lowerLeft.x),
			     &(geo->contents.box.lowerLeft.y));
		tr_transform(_wnTranStack,
			     &(geo->contents.box.upperRight.x),
			     &(geo->contents.box.upperRight.y));
		VEMBOXNORM(geo->contents.box, temp);
	    }
	    TRANX(_wnWin, _wnPhysical, wnFctNum, geo->contents.box.lowerLeft.x);
	    TRANX(_wnWin, _wnPhysical, wnFctNum, geo->contents.box.upperRight.x);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, geo->contents.box.lowerLeft.y);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, geo->contents.box.upperRight.y);
	    dspBox(&(geo->contents.box));
	} else {
	    /* Non-manhattan transformation - change into polygon */
	    wnNonManBox(geo, trflag);
	}
	break;
    case OCT_PATH:
#ifdef OLD_PATH
	for (index = 0;  index < retLength;  index++) {
	    if (trflag) {
		tr_transform(_wnTranStack,
			     &(retPoints[index].x),
			     &(retPoints[index].y));
	    }
	    TRANX(_wnWin, _wnPhysical, wnFctNum, retPoints[index].x);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, retPoints[index].y);
	}
	tr_scaler_transform(_wnTranStack, &(geo->contents.path.width));
	geo->contents.path.width =
	  DISPSCALE(geo->contents.path.width, _wnWin.scaleFactor);
	dspPath(&(geo->contents.path), retLength, retPoints);
#else
	wnPathDisp(&(geo->contents.path), retLength, retPoints, trflag);
#endif
	break;
    case OCT_POLYGON:
	for (index = 0;  index < retLength;  index++) {
	    if (trflag) {
		tr_transform(_wnTranStack,
			     &(retPoints[index].x),
			     &(retPoints[index].y));
	    }
	    TRANX(_wnWin, _wnPhysical, wnFctNum, retPoints[index].x);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, retPoints[index].y);
	}
	/* Always stipple - could be a bad assumption */
	dspPolygon(retLength, retPoints, 1, Nonconvex);
	break;
    case OCT_CIRCLE:
	if (trflag) {
	    tr_transform(_wnTranStack,
			 &(geo->contents.circle.center.x),
			 &(geo->contents.circle.center.y));
	    /* Transform angles */
	    angle = ((double) geo->contents.circle.startingAngle) / 10.0;
	    tr_angle_transform(_wnTranStack, &angle);
	    geo->contents.circle.startingAngle = (octCoord) (angle * 10.0);
	    angle = ((double) geo->contents.circle.endingAngle) / 10.0;
	    tr_angle_transform(_wnTranStack, &angle);
	    geo->contents.circle.endingAngle = (octCoord) (angle * 10.0);
	    /* Transform angles */
	    tr_scaler_transform(_wnTranStack,&geo->contents.circle.innerRadius);
	    tr_scaler_transform(_wnTranStack,&geo->contents.circle.outerRadius);
	}
	TRANX(_wnWin, _wnPhysical, wnFctNum, geo->contents.circle.center.x);
	TRANY(_wnWin, _wnPhysical, wnFctNum, geo->contents.circle.center.y);
	geo->contents.circle.innerRadius =
	  DISPSCALE(geo->contents.circle.innerRadius, _wnWin.scaleFactor);
	geo->contents.circle.outerRadius =
	  DISPSCALE(geo->contents.circle.outerRadius, _wnWin.scaleFactor);
	dspCircle(&(geo->contents.circle));
	break;
    case OCT_LABEL:
	if (trflag) {
	    struct octTransform transform;
	    tr_get_oct_transform(_wnTranStack, &transform);
	    octTransformGeo(geo, &transform);
	}
	TRANX(_wnWin, _wnPhysical, wnFctNum, geo->contents.label.region.lowerLeft.x);
	TRANX(_wnWin, _wnPhysical, wnFctNum, geo->contents.label.region.upperRight.x);
	TRANY(_wnWin, _wnPhysical, wnFctNum, geo->contents.label.region.lowerLeft.y);
	TRANY(_wnWin, _wnPhysical, wnFctNum, geo->contents.label.region.upperRight.y);
	geo->contents.label.textHeight =
	  DISPSCALE(geo->contents.label.textHeight, _wnWin.scaleFactor);
	dspLabel(geo, inst, 0);
	break;
    default:
	vemMsg(MSG_L, "_wnRedraw: unknown object type: %d\n",
	       geo->type);
	break;
    }
}




#define PREVNONMAN	0x01	/* Previous segment is non-manhattan */
#define NOWNOTMAN	0x02	/* Current segment is non-manhattan  */

#define HALF(x)		(((x) < 0) ? -((-x) >> 1) : x >> 1)

static void wnPathDisp(onePath, numPoints, pathPoints, trflag)
struct octPath *onePath;	/* Virtual width of path   */
int numPoints;			/* Number of points        */
struct octPoint *pathPoints;	/* Points for path         */
int trflag;			/* Transformation required */
/*
 * Draws a path with a width specified in the width field of
 * 'onePath' and the points supplied.  These must ALL be
 * virtual coordinates.  Paths are drawn as boxes of the given
 * width between the points.  For manhattan intersections,
 * a square corner type is drawn.  For non-manhattan,  a rounded
 * corner type is drawn.  Manhattan paths have a single pixel line
 * drawn down the center of the segments.  Paths are drawn at
 * this level to prevent information loss when converting the
 * width to physical coordinates.
 */
{
    int index, widthOverTwo = HALF(onePath->width);
    short prevManFlag = 1, nowManFlag = 1;
    struct octBox oneBox;
    struct octCircle oneCircle;
    struct octPoint p1, p2;
    octCoord temp;

    if (numPoints < 2) return;
    /* Set up rounded corner type circle */
    oneCircle.startingAngle = 0;
    oneCircle.endingAngle = 0;
    oneCircle.innerRadius = 0;
    oneCircle.outerRadius = widthOverTwo;
    if (trflag) {
	tr_scaler_transform(_wnTranStack, &(oneCircle.outerRadius));
    }
    oneCircle.outerRadius = DISPSCALE(oneCircle.outerRadius,
				      _wnWin.scaleFactor);
    /* Main Loop */
    for (index = 0;  index < numPoints-1;  index++) {
	prevManFlag = nowManFlag;  nowManFlag = 1;
	if (pathPoints[index].x == pathPoints[index+1].x) {
	    /* Vertical box */
	    if (widthOverTwo > 0) {
		oneBox.upperRight.x = pathPoints[index].x + widthOverTwo;
		oneBox.lowerLeft.x = pathPoints[index].x - widthOverTwo;
		if (pathPoints[index].y < pathPoints[index+1].y) {
		    /* Extent if previous is manhattan and not the start */
		    if (prevManFlag && index) {
			oneBox.lowerLeft.y = pathPoints[index].y - widthOverTwo;
		    } else {
			oneBox.lowerLeft.y = pathPoints[index].y;
		    }
		    oneBox.upperRight.y = pathPoints[index+1].y;
		} else {
		    /* Extend downward if previous is manhattan and not start */
		    if (prevManFlag && index) {
			oneBox.upperRight.y = pathPoints[index].y + widthOverTwo;
		    } else {
			oneBox.upperRight.y = pathPoints[index].y;
		    }
		    oneBox.lowerLeft.y = pathPoints[index+1].y;
		}
		/* Transform appropriately */
		if (trflag) {
		    tr_transform(_wnTranStack,
				 &(oneBox.lowerLeft.x), &(oneBox.lowerLeft.y));
		    tr_transform(_wnTranStack,
				 &(oneBox.upperRight.x), &(oneBox.upperRight.y));
		    VEMBOXNORM(oneBox, temp);
		}
		TRANX(_wnWin, _wnPhysical, wnFctNum, oneBox.lowerLeft.x);
		TRANX(_wnWin, _wnPhysical, wnFctNum, oneBox.upperRight.x);
		TRANY(_wnWin, _wnPhysical, wnFctNum, oneBox.lowerLeft.y);
		TRANY(_wnWin, _wnPhysical, wnFctNum, oneBox.upperRight.y);
		dspBox(&oneBox);
	    }
	    /* Draw manhattan path indicator line */
	    p1 = pathPoints[index];
	    p2 = pathPoints[index+1];
	    if (trflag) {
		tr_transform(_wnTranStack, &(p1.x), &(p1.y));
		tr_transform(_wnTranStack, &(p2.x), &(p2.y));
	    }
	    TRANX(_wnWin, _wnPhysical, wnFctNum, p1.x);
	    TRANX(_wnWin, _wnPhysical, wnFctNum, p2.x);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, p1.y);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, p2.y);
	    dspLine((Window) 0, &(p1), &(p2));
	} else if (pathPoints[index].y == pathPoints[index+1].y) {
	    /* Horizontal box */
	    if (widthOverTwo > 0) {
		oneBox.upperRight.y = pathPoints[index].y + widthOverTwo;
		oneBox.lowerLeft.y = pathPoints[index].y - widthOverTwo;
		if (pathPoints[index].x < pathPoints[index+1].x) {
		    /* Extend to the left previous is manhattan and not start */
		    if (prevManFlag && index) {
			oneBox.lowerLeft.x = pathPoints[index].x - widthOverTwo;
		    } else {
			oneBox.lowerLeft.x = pathPoints[index].x;
		    }
		    oneBox.upperRight.x = pathPoints[index+1].x;
		} else {
		    /* Extend to right if previous is manhattan and not start */
		    if (prevManFlag && index) {
			oneBox.upperRight.x = pathPoints[index].x + widthOverTwo;
		    } else {
			oneBox.upperRight.x = pathPoints[index].x;
		    }
		    oneBox.lowerLeft.x = pathPoints[index+1].x;
		}
		/* Transform appropriately */
		if (trflag) {
		    tr_transform(_wnTranStack,
				 &(oneBox.lowerLeft.x), &(oneBox.lowerLeft.y));
		    tr_transform(_wnTranStack,
				 &(oneBox.upperRight.x), &(oneBox.upperRight.y));
		    VEMBOXNORM(oneBox, temp);
		}
		TRANX(_wnWin, _wnPhysical, wnFctNum, oneBox.lowerLeft.x);
		TRANX(_wnWin, _wnPhysical, wnFctNum, oneBox.upperRight.x);
		TRANY(_wnWin, _wnPhysical, wnFctNum, oneBox.lowerLeft.y);
		TRANY(_wnWin, _wnPhysical, wnFctNum, oneBox.upperRight.y);
		dspBox(&oneBox);
	    }
	    /* Draw manhattan path indicator line */
	    p1 = pathPoints[index];
	    p2 = pathPoints[index+1];
	    if (trflag) {
		tr_transform(_wnTranStack, &(p1.x), &(p1.y));
		tr_transform(_wnTranStack, &(p2.x), &(p2.y));
	    }
	    TRANX(_wnWin, _wnPhysical, wnFctNum, p1.x);
	    TRANX(_wnWin, _wnPhysical, wnFctNum, p2.x);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, p1.y);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, p2.y);
	    dspLine((Window) 0, &(p1), &(p2));
	} else {
	    /* Non-manhattan */
	    p1 = pathPoints[index];
	    p2 = pathPoints[index+1];
	    if (trflag) {
		tr_transform(_wnTranStack, &(p1.x), &(p1.y));
		tr_transform(_wnTranStack, &(p2.x), &(p2.y));
	    }
	    TRANX(_wnWin, _wnPhysical, wnFctNum, p1.x);
	    TRANX(_wnWin, _wnPhysical, wnFctNum, p2.x);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, p1.y);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, p2.y);
	    temp = widthOverTwo;
	    if (trflag) tr_scaler_transform(_wnTranStack, &temp);
	    temp = DISPSCALE(temp, _wnWin.scaleFactor);
	    if (temp > 1) {
		if ((p1.x != p2.x) || (p1.y != p2.y)) {
		    dspNonManBox(p1.x, p1.y, p2.x, p2.y, temp);
		} else {
		    /* XXX - should draw as a line along perp. not implemented */
		}
	    } else {
		/* Too small to fill,  draw line */
		dspLine((Window) 0, &p1, &p2);
	    }
	    nowManFlag = 0;
	}
	if ((!prevManFlag) || (!nowManFlag) || ((index == 0) && (!nowManFlag))) {
	    /* Non-manhattan corner type (octagon is drawn) */
	    if (widthOverTwo > 0) {
		oneCircle.center = pathPoints[index];
		if (trflag) {
		    tr_transform(_wnTranStack,
				 &oneCircle.center.x, &oneCircle.center.y);
		}
		TRANX(_wnWin, _wnPhysical, wnFctNum, oneCircle.center.x);
		TRANY(_wnWin, _wnPhysical, wnFctNum, oneCircle.center.y);
		dspOctagon(&oneCircle);
	    }
	}
    }
    if (!nowManFlag) {
	/* Draw final corner type */
	if (widthOverTwo > 0) {
	    oneCircle.center = pathPoints[numPoints-1];
	    if (trflag) {
		tr_transform(_wnTranStack,
			     &oneCircle.center.x, &oneCircle.center.y);
	    }
	    TRANX(_wnWin, _wnPhysical, wnFctNum, oneCircle.center.x);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, oneCircle.center.y);
	    dspOctagon(&oneCircle);
	}
    }
}



static void wnNonManBox(geo, trflag)
octObject *geo;			/* Possibly non-manhattan box */
int trflag;			/* Transformation flag        */
/*
 * This routine is called when we encounter a box which must be
 * transformed by rotation through a possibly non-manhattan angle.
 * It basically packages the box as a five point polygon and
 * throws it back to _wnObjDisp.
 */
{
    struct octPoint boxPoints[5];

    boxPoints[0] = geo->contents.box.lowerLeft;
    boxPoints[1].x = geo->contents.box.lowerLeft.x;
    boxPoints[1].y = geo->contents.box.upperRight.y;
    boxPoints[2] = geo->contents.box.upperRight;
    boxPoints[3].x = geo->contents.box.upperRight.x;
    boxPoints[3].y = geo->contents.box.lowerLeft.y;
    boxPoints[4] = boxPoints[0];
    geo->type = OCT_POLYGON;
    _wnObjDisp(geo, 5, boxPoints, trflag, 0, (octObject *) 0);
}


static void wnGridDisp()
/*
 * This routine draws a grid in the window specified by
 * _wnWin in the region given by _wnVirtual.  The physical
 * part of the screen is given in _wnPhysical. It always draws
 * the zero grid line. If the grid option of the window is on,
 * it draws the major and minor grid lines as well.  If either
 * of the grid lines are too close together they are not drawn
 * (the default value grid.pixels determines what is too close).
 */
{
    struct octPoint p1, p2;
    int minorIncr, majorIncr, thres, index, base;

    thres = _wnWin.opts.grid_diff;
    if (_wnWin.opts.disp_options & VEM_GRIDLINES) {
	base = _wnWin.opts.grid_base;
	minorIncr = _wnWin.opts.grid_minorunits;
	majorIncr = _wnWin.opts.grid_majorunits;
	if (base > 0) {
	    int minspace, basepower, multfactor, idx, minpower;

	    minspace = OCTSCALE(thres, _wnWin.scaleFactor);
	    if (minspace > 0) {
		basepower = (int) (log((double) minspace) / log((double) base));
	    } else {
		basepower = 0;
	    }
	    minpower = _wnWin.opts.grid_minbase;
	    basepower = VEMMAX(basepower, minpower);
	    multfactor = 1;
	    for (idx = 0;  idx < basepower;  idx++) {
		multfactor *= base;
	    }
	    minorIncr *= multfactor;
	    majorIncr *= multfactor;
	}
	/* Draw the minor grid lines */
	if (DISPSCALE(minorIncr, _wnWin.scaleFactor) >= thres) {
	    if (_wnWin.opts.disp_options & VEM_DOTGRID) {
		atrSet(minDotAttr);
		for (p1.x = (_wnVirtual.lowerLeft.x/minorIncr) * minorIncr;
		     p1.x <= _wnVirtual.upperRight.x;
		     p1.x += minorIncr)
		  for (p1.y = (_wnVirtual.upperRight.y/minorIncr)*minorIncr+minorIncr;
		       p1.y >= _wnVirtual.lowerLeft.y;
		       p1.y -= minorIncr)
		    {
			p2 = p1;
			TRANX(_wnWin, _wnPhysical, wnFctNum, p2.x);
			TRANY(_wnWin, _wnPhysical, wnFctNum, p2.y);
			dspPoint((Window) 0, &p2, 0);
		    }
	    } else {
		/* First, lines parallel to the x-axis */
		atrSet(minLineAttr);
		p1.x = 0;
		p2.x = _wnPhysical.upperRight.x;
		for (index=(_wnVirtual.upperRight.y/minorIncr)*minorIncr + minorIncr;
		     index >= _wnVirtual.lowerLeft.y;
		     index -= minorIncr)
		  {
		      p1.y = index;
		      TRANY(_wnWin, _wnPhysical, wnFctNum, p1.y);  p2.y = p1.y;
		      dspLine((Window) 0, &p1, &p2);
		  }
		/* Next, grid lines parallel to the y-axis */
		p1.y = 0;
		p2.y = _wnPhysical.lowerLeft.y;
		for (index = (_wnVirtual.lowerLeft.x / minorIncr) * minorIncr;
		     index <= _wnVirtual.upperRight.x;
		     index += minorIncr)
		  {
		      p1.x = index;
		      TRANX(_wnWin, _wnPhysical, wnFctNum, p1.x);  p2.x = p1.x;
		      dspLine((Window) 0, &p1, &p2);
		  }
	    }
	}

	/* Draw the major grid lines */
	if (DISPSCALE(majorIncr, _wnWin.scaleFactor) >= thres) {
	    /*
	     * Major grid lines large enough to draw.  First,
	     * grid lines parallel to the x-axis
	     */
	    if (_wnWin.opts.disp_options & VEM_DOTGRID) {
		atrSet(majDotAttr);
		for (p1.x = (_wnVirtual.lowerLeft.x/majorIncr) * majorIncr;
		     p1.x <= _wnVirtual.upperRight.x;
		     p1.x += majorIncr)
		  for (p1.y = (_wnVirtual.upperRight.y/majorIncr)*majorIncr+majorIncr;
		       p1.y >= _wnVirtual.lowerLeft.y;
		       p1.y -= majorIncr)
		    {
			p2 = p1;
			TRANX(_wnWin, _wnPhysical, wnFctNum, p2.x);
			TRANY(_wnWin, _wnPhysical, wnFctNum, p2.y);
			dspPoint((Window) 0, &p2, 0);
		    }
	    } else {
		atrSet(majLineAttr);
		p1.x = 0;
		p2.x = _wnPhysical.upperRight.x;
		for (index = (_wnVirtual.upperRight.y / majorIncr) * majorIncr + majorIncr;
		     index >= _wnVirtual.lowerLeft.y;
		     index -= majorIncr)
		  {
		      p1.y = index;
		      TRANY(_wnWin, _wnPhysical, wnFctNum, p1.y);  p2.y = p1.y;
		      dspLine((Window) 0, &p1, &p2);
		  }
		/* Next, lines parallel to the y-axis */
		p1.y = 0;
		p2.y = _wnPhysical.lowerLeft.y;
		for (index = (_wnVirtual.lowerLeft.x / majorIncr) * majorIncr;
		     index <= _wnVirtual.upperRight.x;
		     index += majorIncr)
		  {
		      p1.x = index;
		      TRANX(_wnWin, _wnPhysical, wnFctNum, p1.x);  p2.x = p1.x;
		      dspLine((Window) 0, &p1, &p2);
		  }
	    }
	}
    }

    /* Draw the zero grid line */
    atrSet(zeroAttr);
    if ((_wnVirtual.lowerLeft.x <= 0) && (_wnVirtual.upperRight.x >=0)) {
	/* Draw Y axis */
	p1.x = 0;
	TRANX(_wnWin, _wnPhysical, wnFctNum, p1.x); p2.x = p1.x;
	p1.y = 0;
	p2.y = _wnPhysical.lowerLeft.y;
	dspLine((Window) 0, &p1, &p2);
    }
    if ((_wnVirtual.lowerLeft.y <= 0) && (_wnVirtual.upperRight.y >= 0)) {
	/* Draw the X axis */
	p1.y = 0;
	TRANY(_wnWin, _wnPhysical, wnFctNum, p1.y);  p2.y = p1.y;
	p1.x = 0;
	p2.x = _wnPhysical.upperRight.x;
	dspLine((Window) 0, &p1, &p2);
    }
}


/*
 * Contact abstraction code
 *
 * It has been found that most symbolic cells have large numbers of contact
 * instances.  For these cells,  VEM was spending most of its time drawing
 * these contacts.  To speed up this operation,  we define a contact
 * abstraction which draws much faster.  An instance is a contact if it
 * is connected to the "CONNECTORS" bag.  In all cases,  the bounding box
 * of the instance is used for drawing.   The current drawing abstraction
 * is a single box on the highest priority layer in the contact master.
 */

#define CONBAGNAME	"CONNECTORS"

/*
 * The following tests for a contact.  If it is,  'result' will be
 * set to one.  Otherwise it will be set to zero.  'inst' is octObject *.
 * It assumes the window record is stored in 'realWin' and the bounding
 * box of the instance is in 'BB'.    First,  if the
 * contact size is larger than the contact abstraction threshold,  it
 * is immediately rejected.  Then,  if it is not on the connectors
 * bag,  it is rejected.
 */
#define ISCONTACT(inst, bag, result) \
{ 								\
  int checkSize;						\
								\
  checkSize = BB.upperRight.x - BB.lowerLeft.x;			\
  if (checkSize < BB.upperRight.y - BB.lowerLeft.y) {		\
     checkSize = BB.upperRight.y - BB.lowerLeft.y;		\
  }								\
  result = 0;							\
  if (DISPSCALE(checkSize, _wnWin.scaleFactor) < _wnWin.opts.con_min) { \
    if (((bag)->objectId != oct_null_id) && (octIsAttached((bag), (inst)) == OCT_OK)) {		\
       result = 1; 						\
    } 								\
  }								\
}



#define I_BADMAS	0x01	/* Bad master          */
#ifdef SUPRESS_SMALL_INSTANCES
#define I_ABSBOX	0x02	/* Abstract inst by bb */
#endif

static vemStatus wnInstDisp(theFacet, invVirtual, level)
octObject *theFacet;		/* Facet to draw from                     */
struct octBox *invVirtual;	/* Inverse transform of virtual region    */
int level;			/* Level in the heirarchy                 */
/*
 * This routine displays instances.  Instances are displayed by calling
 * wnViewDisp recursively with the proper transform pushed on the
 * transformation stack.  The expand mode is also handled here.  If
 * an instance is unexpanded,  its interface is shown.  If it has no
 * interface,  its bounding box is displayed.  If it is expanded,  the
 * contents of the master is shown.  Note the routine may return
 * VEM_INTERRUPT which implies unwinding the stack to stop the current
 * display sequence.  As in all other routines,  the globals _wnWin,
 * _wnVirtual, _wnPhysical, wnFctNum, and wnLyr are used as parameters.
 */
{
    int retLength;		/* Length of path or polygon         */
    struct octObject master;	/* Master of instance                */
    struct octPoint *retPoints;	/* Points for polygon or path        */
    struct octObject conbag;	/* Connector bag                     */
    struct octObject inst;      /* Instance record                   */
    struct octBox BB;	        /* General purpose bounding box      */
    regGenerator theGen;	/* For generating geometry           */
    octCoord temp;		/* Temporary for box normalization   */
    int flags;			/* Various status flags              */
    vemStatus ret_code = VEM_OK;/* Return from routine               */
#ifdef SUPRESS_SMALL_INSTANCES
    int inst_size;		/* Small instance size check         */
#else
    octObject newbox;		/* Contact abstraction box           */
    int isContact;		/* Contact flag                      */
    octGenerator gen;		/* For connectors bag search         */
#endif

    flags = 0;
    regInit(theFacet, invVirtual, OCT_INSTANCE_MASK, &theGen);
    conbag.objectId = oct_null_id;
    while (regNext(&theGen, &inst, &retLength, &retPoints, &BB) == REG_OK) {
#ifndef SUPRESS_SMALL_INSTANCES
	/*
	 * We now check to see if this is a contact and draw it
	 * quickly if it meets the criteria.
	 */
	if (conbag.objectId == oct_null_id) {
	    VEM_OCTCKRVAL(octInitGenContainers(&inst, OCT_BAG_MASK, &gen),
			  VEM_CORRUPT);
	    while (octGenerate(&gen, &conbag) == OCT_OK) {
		if (STRCOMP(conbag.contents.bag.name, CONBAGNAME) == 0) {
		    octFreeGenerator(&gen);
		    break;
		} else {
		    conbag.objectId = oct_null_id;
		}
	    }
	}
	ISCONTACT(&inst, &conbag, isContact);

	if (isContact) {
	    if (wnLastLayer) {
		/* Open a buffer to the master */
		if (_wnWin.opts.disp_options & VEM_EXPAND)
		  master.contents.facet.facet = "contents";
		else master.contents.facet.facet = _wnWin.opts.interface;
		flags = 0;
		if (bufOpenMaster(&inst, &master, BUFREADONLY) != VEM_OK) {
		    flags |= I_BADMAS;
		}
		atrSet(tkAttr(bufHighLayer(master.objectId), 1));
		newbox.type = OCT_BOX;
		newbox.contents.box = BB;
		_wnObjDisp(&newbox, 0, (struct octPoint *) 0, level, 1,
			   (octObject *) 0);
	    }
	} else { /* Its a real instance */
#else
	    /* Check for too small */
	    inst_size = BB.upperRight.x - BB.lowerLeft.x;
	    if (inst_size < BB.upperRight.y - BB.lowerLeft.y) {
		inst_size = BB.upperRight.y - BB.lowerLeft.y;
	    }
	    if (DISPSCALE(inst_size, _wnWin.scaleFactor) >= _wnWin.opts.con_min) {
#endif
	    /* Open a buffer to the master  */
	    if (_wnWin.opts.disp_options & VEM_EXPAND) {
		master.contents.facet.facet = "contents";
	    } else if (strcmp(inst.contents.instance.facet, "contents") == 0) {
		master.contents.facet.facet = _wnWin.opts.interface;
	    } else {
		master.contents.facet.facet = inst.contents.instance.facet;
	    }
	    flags = 0;
	    if (bufOpenMaster(&inst, &master, BUFREADONLY) == VEM_OK) {
		/*
		 * We must determine the proper instance
		 * transformation.  This is done by pushing the
		 * current transformation and operating upon it.
		 * It will be popped off by the recursive call.
		 */
		tr_push(_wnTranStack);

		/* First the translation */
		tr_translate(_wnTranStack,
			     inst.contents.instance.transform.translation.x,
			     inst.contents.instance.transform.translation.y,
			     0);

		/* Then the rotation */
		if (inst.contents.instance.transform.transformType
		    == OCT_FULL_TRANSFORM)
		  {
		      tr_rotate_matrix(_wnTranStack,
				       inst.contents.instance.transform.generalTransform,0);
		  } else {
		      tr_do_op(_wnTranStack,
			       inst.contents.instance.transform.transformType,
			       0);
		  }

		/* The recursive call: */
		if (wnViewDisp(&master, &inst, level+1) == VEM_INTERRUPT) {
		    ret_code = VEM_INTERRUPT;
		    break;
		}
	    } else {
		flags |= I_BADMAS;
	    }
#ifdef SUPRESS_SMALL_INSTANCES
	} else {
	    flags |= I_ABSBOX;
	}
#else
	}
#endif
#ifdef SUPRESS_SMALL_INSTANCES
	if (wnLastLayer && ((_wnWin.opts.disp_options & VEM_SHOWBB) ||
			    (flags & I_ABSBOX))) {
#else
	if (wnLastLayer && (_wnWin.opts.disp_options & VEM_SHOWBB)) {
#endif
	    /* Display bounding box */
	    tr_transform(_wnTranStack, &BB.lowerLeft.x,
			 &BB.lowerLeft.y);
	    tr_transform(_wnTranStack, &BB.upperRight.x,
			 &BB.upperRight.y);
	    VEMBOXNORM(BB, temp);
	    TRANX(_wnWin, _wnPhysical, wnFctNum, BB.lowerLeft.x);
	    TRANX(_wnWin, _wnPhysical, wnFctNum, BB.upperRight.x);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, BB.lowerLeft.y);
	    TRANY(_wnWin, _wnPhysical, wnFctNum, BB.upperRight.y);
	    atrSet(instAttr);
	    if ((inst.contents.instance.name) &&
		(strlen(inst.contents.instance.name)))
	      {
		  dspBound(&BB, inst.contents.instance.name, 2,
			   _wnWin.opts.bb_thres,
			   _wnWin.opts.bb_min);
	      } else {
		  dspBound(&BB, inst.contents.instance.master, 2,
			   _wnWin.opts.bb_thres,
			   _wnWin.opts.bb_min);
	      }
	}
	if (wnLastLayer & (flags & I_BADMAS)) {
	    wnBadMaster(&inst, &master);
	}
    }
    regEnd(&theGen);
    return ret_code;
}



static vemStatus wnTermDisp(theFacet, invVirtual, level)
octObject *theFacet;		/* Facet to draw from                     */
struct octBox *invVirtual;	/* Inverse transform of virtual region    */
int level;			/* Level in the heirarchy                 */
/*
 * This routine displays terminals.  The routine draws terminals
 * using the vemBoundDisp routine to outline the implementation(s)
 * of the terminal.
 */
{
    int retLength;		/* Length of path or polygon         */
    struct octPoint *retPoints;	/* Points for polygon or path        */
    struct octObject term;      /* Terminal record                   */
    struct octBox BB;	        /* General purpose bounding box      */
    regGenerator theGen;	/* For generating geometry           */
    octCoord temp;		/* Temporary for box normalization   */

    /* initialize region generator */
    regInit(theFacet, invVirtual, OCT_TERM_MASK, &theGen);

    if (level == 0)
      atrSet(formalAttr);
    else
      atrSet(actualAttr);

    while (regNext(&theGen, &term, &retLength, &retPoints, &BB) == REG_OK) {
	/* Terminal: transform bounding box and display */
	tr_transform(_wnTranStack, &BB.lowerLeft.x,
		     &BB.lowerLeft.y);
	tr_transform(_wnTranStack, &BB.upperRight.x,
		     &BB.upperRight.y);
	VEMBOXNORM(BB, temp);
	TRANX(_wnWin, _wnPhysical, wnFctNum, BB.lowerLeft.x);
	TRANX(_wnWin, _wnPhysical, wnFctNum, BB.upperRight.x);
	TRANY(_wnWin, _wnPhysical, wnFctNum, BB.lowerLeft.y);
	TRANY(_wnWin, _wnPhysical, wnFctNum, BB.upperRight.y);
	dspBound(&BB, term.contents.term.name, level,
		 _wnWin.opts.bb_thres, _wnWin.opts.bb_min);
    }
    regEnd(&theGen);
    return VEM_OK;
}



/*
 * Bad master errors
 */

static st_table *bad_masters = (st_table *) 0;

/* bad_masters is an st_table of the following: */
typedef struct _bm_data {
    char *master_key;		/* Name of master for master matching */
    int num;			/* Number of times referenced         */
} bm_data;

static void wnBadMaster(inst, master)
octObject *inst;		/* Instance        */
octObject *master;		/* Specifies facet */
/*
 * This routine adds a master to a hash table of bad masters for
 * a given display sequence. wnBadList() outputs the error list and 
 * resets the system.
 */
{
    char name[VEMMAXSTR*4];
    bm_data *table;

    if (!bad_masters) {
	bad_masters = st_init_table(strcmp, st_strhash);
    }
    /* Compose master name */
    (void) sprintf(name, "%s:%s:%s",
		   inst->contents.instance.master,
		   inst->contents.instance.view,
		   master->contents.facet.facet);
    /* Examine current table */
    if (st_lookup(bad_masters, name, (char **) &table)) {
	/* Found it */
	table->num += 1;
    } else {
	/* Didn't find it */
	table = VEMALLOC(bm_data);
	table->master_key = VEMSTRCOPY(name);
	table->num = 1;
	(void) st_insert(bad_masters, table->master_key, (char *) table);
    }
}

/*ARGSUSED*/
enum st_retval bm_func(key, value, arg)
char *key;
char *value;
char *arg;
/*
 * Prints out bad master information for a certain cell.  Side
 * effect: frees the allocated string in the record and the record
 * itself.
 */
{
    bm_data *data = (bm_data *) value;

    if (data->num > 1) {
	vemMsg(MSG_A, "  %d instances of `%s'\n", data->num, data->master_key);
    } else {
	vemMsg(MSG_A, "  Instance of `%s'\n", data->master_key);
    }
    VEMFREE(data->master_key);
    VEMFREE(data);
    return ST_CONTINUE;
}

static void wnBadList(fct)
octObject *fct;			/* Inconsistent facet */
/*
 * Sends error messages to the console window and log file
 * if any bad masters were found while redrawing a window.
 * Its major side effect:  it clears the table built
 * by wnBadMaster.
 */
{
    if (bad_masters) {
	vemMsg(MSG_A, "Bad masters were found in %s:%s:%s:\n",
	       fct->contents.facet.cell,
	       fct->contents.facet.view,
	       fct->contents.facet.facet);
	st_foreach(bad_masters, bm_func, (char *) 0);
	st_free_table(bad_masters);
	bad_masters = (st_table *) 0;
    }
}
