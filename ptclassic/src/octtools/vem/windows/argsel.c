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
 * Argument Selection Module
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1988
 *
 * This module supports routines for displaying the basic VEM argument
 * types:  points, lines, and boxes.  It replaces an earlier effort
 * which was part of the main window module.
 *
 * The module is designed to operate on a set of vemOneArg structures.
 * The programmer passes such a structure to the module allocated
 * but un-initialized.  The module then exports routines for
 * changing the argument type,  adding points, deleting points,
 * inserting points, and changing points in the structure.  As
 * the programmer calls these routines,  the update of the
 * on-screen representation of the argument is done immediately.
 * This is how line and box stretching is done.
 *
 * Although the module is in its own file,  it is very tightly
 * tied with the main window module.  It makes heavy use of
 * internal window structures and mapping functions.
 */

#include "wininternal.h"	/* Internal Window Definitions */
#include "display.h"		/* Display primitives          */
#include "list.h"		/* List management             */

/*
 * Initial point size for point arrays
 */
#define INITPNTSIZE	10

/*
 * This list maintains a collection of argument structures which
 * are currently displayed on the screen.  Argument structures
 * are added to the list using argOn and removed from the list
 * by argOff.  Only persistent arguments (those that must survive
 * exposure events and editing operations) need be added to this list.
 */
static lsList argList = (lsList) 0;

/*
 * Structure for passing update information to the large number
 * of st_foreach functions given at the end of the module.
 */
#define MAX_OLDPNTS	5
#define MAX_NEWPNTS	3

struct update_args {
    int num_old, num_new;	/* Number of each         */
    int offset;			/* Where new points begin */
    octId fct;			/* Facet for points       */
    struct octPoint oldpnts[MAX_OLDPNTS];	/* Old points */
    struct octPoint newpnts[MAX_NEWPNTS];	/* New points */
};

/*
 * Forward declarations
 */

static enum st_retval sweepDraw();
static enum st_retval pntDraw();
static enum st_retval boxDraw();
static enum st_retval lineDraw();

static enum st_retval pntUpdate();
static enum st_retval box1Update();
static enum st_retval box2Update();
static enum st_retval lineUpdate();

static void man_line_update();
static void norm_line_update();
static void resize();
static int man_line_p();



static int man_line_p(win)
Window win;			/* Window to check */
/*
 * This routine returns a non-zero value if the window argument
 * has manhattan line snapping turned on.  This is implemented
 * with explicit knowledge of the window data structures.
 */
{
    internalWin *theWin;

    if (_wnFind(win, &theWin, (int *) 0, (int *) 0)) {
	if (theWin->opts.disp_options & VEM_MANLINE) return 1;
    }
    return 0;
}



void argOn(arg)
vemOneArg *arg;			/* Argument to control */
/*
 * Turns on the selection of the specified argument.  It
 * will be displayed immediately in all appropriate windows.
 * It will remain displayed until the programmer turns off
 * selection using argOff().
 */
{
    if (!argList) {
	argList = lsCreate();
    }

    /* Go through all windows looking for stuff to draw */
    _wnForeach(WN_ACTIVE|WN_INTR, sweepDraw, (char *) arg);

    /* Add to list of active arguments */
    lsNewBegin(argList, (lsGeneric) arg, LS_NH);
}


void argOff(arg)
vemOneArg *arg;			/* Argument to release */
/*
 * Turns off selection of the specified argument.  All
 * on-screen selection will be turned off immediately in
 * all appropriate windows.
 */
{
    lsGen theGen;
    vemOneArg *theArg;

    if (!argList) {
	argList = lsCreate();
    }

    /* Go through all windows looking for stuff to undraw */
    _wnForeach(WN_ACTIVE | WN_INTR, sweepDraw, (char *) arg);

    /* Remove from the list of items */
    theGen = lsStart(argList);
    while (lsNext(theGen, (lsGeneric *) &theArg, LS_NH) == VEM_OK) {
	if (theArg == arg) {
	    lsDelBefore(theGen, (lsGeneric *) &theArg);
	    break;
	}
    }
    lsFinish(theGen);
}



void argType(arg, type)
vemOneArg *arg;			/* Argument to change */
int type;			/* New type           */
/*
 * Changes the type of the item.  The change is effected
 * on the screen at once.  For example,  this is used
 * when a point argument is upgraded to a box.  The point
 * will be undrawn and a box drawn.
 */
{
    /* First,  undraw the item entirely */
    _wnForeach(WN_ACTIVE | WN_INTR, sweepDraw, (char *) arg);

    /* Change the type */
    if (arg->argType == VEM_BOX_ARG) {
	if (arg->length < 0) {
	    arg->length = -(arg->length) * 2 - 1;
	} else {
	    arg->length *= 2;
	}
    }
    if (type == VEM_BOX_ARG) {
	if (arg->length % 2) {
	    /* Odd length */
	    arg->length = -(arg->length/2) - 1;
	} else {
	    arg->length /= 2;
	}
    }
    arg->argType = type;

    /* Draw it all again */
    _wnForeach(WN_ACTIVE | WN_INTR, sweepDraw, (char *) arg);
}



void argAddPoint(arg, pnt)
vemOneArg *arg;			/* Argument to modify */
vemPoint *pnt;			/* Point to add       */
/*
 * This routine appends `pnt' to the argument `arg'.  The
 * on screen representation of the item is updated appropriately.
 * Notes:
 *  - Adding a point to a line argument of length zero does
 *    not draw anything on the screen.
 *  - Adding a point to a box argument has no effect until
 *    an opposite corner point is also added.
 */
{
    switch (arg->argType) {
    case VEM_POINT_ARG:
	if (arg->length >= arg->alloc_size) {
	    resize(arg);
	}
	arg->argData.points[arg->length] = *pnt;
	arg->length += 1;
	_wnForeach(WN_ACTIVE | WN_INTR, pntDraw, (char *) pnt);
	break;
    case VEM_BOX_ARG:
	if (VEMABS(arg->length)*2 >= arg->alloc_size) {
	    resize(arg);
	}
	if (arg->length < 0) {
	    /* Adding a matching pair */
	    arg->length = -(arg->length);
	    arg->argData.points[arg->length*2-1] = *pnt;

	    _wnForeach(WN_ACTIVE | WN_INTR, boxDraw,
		       (char *) &(arg->argData.points[arg->length*2-2]));
	} else {
	    /* Adding a non-match point */
	    arg->argData.points[arg->length*2] = *pnt;
	    arg->length = -(arg->length + 1);
	}
	break;
    case VEM_LINE_ARG:
	if (arg->length+1 >= arg->alloc_size) {
	    resize(arg);
	}
	arg->argData.points[arg->length] = *pnt;
	arg->length += 1;
	if (arg->length > 1) {
	    if (man_line_p(arg->argData.points[arg->length].theWin)) {
		int dx, dy;

		dx = VEMABS(arg->argData.points[arg->length-1].thePoint.x -
			    arg->argData.points[arg->length-2].thePoint.x);
		dy = VEMABS(arg->argData.points[arg->length-1].thePoint.y -
			    arg->argData.points[arg->length-2].thePoint.y);
		if (dx > dy) {
		    arg->argData.points[arg->length-1].y =
		      arg->argData.points[arg->length-2].y;
		} else {
		    arg->argData.points[arg->length-1].x =
		      arg->argData.points[arg->length-2].x;
		}
	    }
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
		       (char *) &(arg->argData.points[arg->length-2]));
	}
	break;
    default:
	vemFail("Type conflict", __FILE__, __LINE__);
    }
}



void argChgPoint(arg, idx, pnt)
vemOneArg *arg;			/* Argument to update */
int idx;			/* Index into arg     */
vemPoint *pnt;			/* New value of point */
/*
 * This routine changes the `idx'th point in the argument `arg'
 * to `pnt'.  The change is immediately reflected on the screen.
 * Thus,  changing points in a box or line argument will cause
 * the appropriate stretching to occur.  
 */
{
    struct update_args up_args;
    enum st_retval (*func)();

    up_args.fct = arg->argData.points[idx].theFct;
    switch (arg->argType) {
    case VEM_POINT_ARG:
	up_args.oldpnts[0] = arg->argData.points[idx].thePoint;
	arg->argData.points[idx] = *pnt;
	up_args.newpnts[0] = arg->argData.points[idx].thePoint;
	_wnForeach(WN_ACTIVE | WN_INTR, pntUpdate, (char *) &up_args);
	break;
    case VEM_BOX_ARG:
	if (idx & 1) {
	    /* Odd offset - last point of box */
	    func = box1Update;
	    up_args.oldpnts[0] = arg->argData.points[idx-1].thePoint;
	    up_args.oldpnts[1] = arg->argData.points[idx].thePoint;
	} else {
	    /* Even offset - first point of box */
	    func = box2Update;
	    up_args.oldpnts[0] = arg->argData.points[idx].thePoint;
	    up_args.oldpnts[1] = arg->argData.points[idx+1].thePoint;
	}
	up_args.newpnts[0] = pnt->thePoint;
	arg->argData.points[idx] = *pnt;
	_wnForeach(WN_ACTIVE | WN_INTR, func, (char *) &up_args);
	break;
    case VEM_LINE_ARG:
	if (man_line_p(arg->argData.points[idx].theWin)) {
	    man_line_update(arg, idx, pnt);
	} else {
	    norm_line_update(arg, idx, pnt);
	}
	break;
    default:
	vemFail("Type conflict", __FILE__, __LINE__);
    }
}


typedef enum lt	{ HORIZONTAL = 0, VERTICAL = 1, NO_TYPE = 2 } linetype;

/* Returns the type of the segment after `idx' given type of first arg */
#define LT(type_1, idx) \
(((type_1) != NO_TYPE) ? (linetype) (((idx) & 0x01) ^ (int) (type_1)) : (type_1))

static linetype first_type(arg)
vemOneArg *arg;
/*
 * Returns the type of the first segment of a manhattan line.
 */
{
    int idx;
    octCoord dx, dy;
    linetype result;

    result = HORIZONTAL;
    for (idx = 0;  idx < arg->length-1;  idx++) {
	dx = VEMABS(arg->argData.points[idx].thePoint.x -
		    arg->argData.points[idx+1].thePoint.x);
	dy = VEMABS(arg->argData.points[idx].thePoint.y -
		    arg->argData.points[idx+1].thePoint.y);
	if ((dx > 0) || (dy > 0)) {
	    if (dx > dy) {
		return result;
	    } else {
		return (linetype) (((int) result) ^ 0x01);
	    }
	}
	result = (linetype) (((int) result) ^ 0x01);
    }
    return NO_TYPE;
}


static void man_line_update(arg, idx, pnt)
vemOneArg *arg;			/* Argument to update */
int idx;			/* Index into arg     */
vemPoint *pnt;			/* New value of point */
/*
 * This routine updates point `idx' of `arg' in a manhattan fashion.
 * This may imply moving more than one point.  Assumes everything
 * is already manhattan.
 */
{
    struct update_args up_args;
    linetype argType;
    int do_update = 1;

    argType = first_type(arg);
    up_args.fct = arg->argData.points[idx].theFct;
    if ((argType == NO_TYPE) || (arg->length < 3)) {
	/* Special case - hard manhattan stretch */
	octCoord dx, dy;

	up_args.num_old = 2;
	up_args.oldpnts[0] = arg->argData.points[idx-1].thePoint;
	up_args.oldpnts[1] = arg->argData.points[idx].thePoint;
	arg->argData.points[idx] = *pnt;
	dx = VEMABS(arg->argData.points[idx].thePoint.x -
		    arg->argData.points[idx-1].thePoint.x);
	dy = VEMABS(arg->argData.points[idx].thePoint.y -
		    arg->argData.points[idx-1].thePoint.y);
	if (dx > dy) {
	    /* Snap horizontal */
	    arg->argData.points[idx].thePoint.y =
	      arg->argData.points[idx-1].thePoint.y;
	} else {
	    /* Snap vertical */
	    arg->argData.points[idx].thePoint.x =
	      arg->argData.points[idx-1].thePoint.x;
	}
	up_args.num_new = 1;  up_args.offset = 1;
	up_args.newpnts[0] = arg->argData.points[idx].thePoint;
    } else if (idx == 0) {
	/* First point */
	up_args.num_old = 3;
	up_args.oldpnts[0] = arg->argData.points[0].thePoint;
	up_args.oldpnts[1] = arg->argData.points[1].thePoint;
	up_args.oldpnts[2] = arg->argData.points[2].thePoint;
	up_args.num_new = 2;  up_args.offset = 0;
	up_args.newpnts[0] = pnt->thePoint;
	if (LT(argType, 0) == HORIZONTAL) {
	    arg->argData.points[1].thePoint.y = pnt->thePoint.y;
	} else /* VERTICAL */ {
	    arg->argData.points[1].thePoint.x = pnt->thePoint.x;
	}
	arg->argData.points[0] = *pnt;
	up_args.newpnts[1] = arg->argData.points[1].thePoint;
    } else  if (idx == arg->length-1) {
	/* Last point - opposite of first */
	up_args.num_old = 3;
	up_args.oldpnts[0] = arg->argData.points[arg->length-3].thePoint;
	up_args.oldpnts[1] = arg->argData.points[arg->length-2].thePoint;
	up_args.oldpnts[2] = arg->argData.points[arg->length-1].thePoint;
	if (LT(argType, arg->length-2) == HORIZONTAL) {
	    arg->argData.points[arg->length-2].thePoint.y = pnt->thePoint.y;
	} else /* VERTICAL */ {
	    arg->argData.points[arg->length-2].thePoint.x = pnt->thePoint.x;
	}		    
	arg->argData.points[arg->length-1] = *pnt;
	up_args.num_new = 2;  up_args.offset = 1;
	up_args.newpnts[0] = arg->argData.points[arg->length-2].thePoint;
	up_args.newpnts[1] = arg->argData.points[arg->length-1].thePoint;
    } else {
	/* Middle point */
	if (idx > 1) {
	    if (idx < arg->length-2) {
		/* Total freedom */
		up_args.num_old = 5;
		up_args.oldpnts[0] = arg->argData.points[idx-2].thePoint;
		up_args.oldpnts[1] = arg->argData.points[idx-1].thePoint;
		up_args.oldpnts[2] = arg->argData.points[idx].thePoint;
		up_args.oldpnts[3] = arg->argData.points[idx+1].thePoint;
		up_args.oldpnts[4] = arg->argData.points[idx+2].thePoint;
		if (LT(argType, idx-1) == HORIZONTAL) {
		    arg->argData.points[idx-1].thePoint.y = pnt->thePoint.y;
		    arg->argData.points[idx+1].thePoint.x = pnt->thePoint.x;
		} else /* VERTICAL */ {
		    arg->argData.points[idx-1].thePoint.x = pnt->thePoint.x;
		    arg->argData.points[idx+1].thePoint.y = pnt->thePoint.y;
		}
		arg->argData.points[idx] = *pnt;
		up_args.num_new = 3;  up_args.offset = 1;
		up_args.newpnts[0] = arg->argData.points[idx-1].thePoint;
		up_args.newpnts[1] = arg->argData.points[idx].thePoint;
		up_args.newpnts[2] = arg->argData.points[idx+1].thePoint;
	    } else {
		/* Freedom before point */
		up_args.num_old = 4;
		up_args.oldpnts[0] = arg->argData.points[idx-2].thePoint;
		up_args.oldpnts[1] = arg->argData.points[idx-1].thePoint;
		up_args.oldpnts[2] = arg->argData.points[idx].thePoint;
		up_args.oldpnts[3] = arg->argData.points[idx+1].thePoint;
		arg->argData.points[idx] = *pnt;
		if (LT(argType, idx-1) == HORIZONTAL) {
		    arg->argData.points[idx-1].thePoint.y =
		      arg->argData.points[idx].thePoint.y;
		    arg->argData.points[idx].thePoint.x =
		      arg->argData.points[idx+1].thePoint.x;
		} else /* VERTICAL */ {
		    /* Snap vertical */
		    arg->argData.points[idx-1].thePoint.x =
		      arg->argData.points[idx].thePoint.x;
		    arg->argData.points[idx].thePoint.y =
		      arg->argData.points[idx+1].thePoint.y;
		}
		up_args.num_new = 2;  up_args.offset = 1;
		up_args.newpnts[0] = arg->argData.points[idx-1].thePoint;
		up_args.newpnts[1] = arg->argData.points[idx].thePoint;
	    }
	} else if (arg->length > 3) {
	    /* Freedom after point */
	    up_args.num_old = 4;
	    up_args.oldpnts[0] = arg->argData.points[idx-1].thePoint;
	    up_args.oldpnts[1] = arg->argData.points[idx].thePoint;
	    up_args.oldpnts[2] = arg->argData.points[idx+1].thePoint;
	    up_args.oldpnts[3] = arg->argData.points[idx+2].thePoint;
	    if (LT(argType, idx) == HORIZONTAL) {
		/* Snap horizontal */
		arg->argData.points[idx] = *pnt;
		arg->argData.points[idx+1].thePoint.y =
		  arg->argData.points[idx].thePoint.y;
		arg->argData.points[idx].thePoint.x =
		  arg->argData.points[idx-1].thePoint.x;
	    } else {
		/* Snap vertical */
		arg->argData.points[idx] = *pnt;
		arg->argData.points[idx+1].thePoint.x =
		  arg->argData.points[idx].thePoint.x;
		arg->argData.points[idx].thePoint.y =
		  arg->argData.points[idx-1].thePoint.y;
	    }
	    up_args.num_new = 2;  up_args.offset = 1;
	    up_args.newpnts[0] = arg->argData.points[idx].thePoint;
	    up_args.newpnts[1] = arg->argData.points[idx+1].thePoint;
	} else {
	    /* No freedom whatsoever */
	    do_update = 0;
	}
    }
    if (do_update) {
	_wnForeach(WN_ACTIVE | WN_INTR, lineUpdate, (char *) &up_args);
    }
}



static void norm_line_update(arg, idx, pnt)
vemOneArg *arg;			/* Argument to update */
int idx;			/* Index into arg     */
vemPoint *pnt;			/* New value of point */
/*
 * This routine updates point `idx' of `arg' with no manhattan
 * restrictions.  This always guarantees that only one point changes.
 */
{
    struct update_args up_args;

    up_args.fct = arg->argData.points[idx].theFct;
    if (idx == 0) {
	/* First point */
	up_args.num_old = 2;  up_args.offset = 0;
	up_args.oldpnts[0] = arg->argData.points[0].thePoint;
	up_args.oldpnts[1] = arg->argData.points[1].thePoint;
    } else if (idx == arg->length-1) {
	/* Last point */
	up_args.num_old = 2;  up_args.offset = 1;
	up_args.oldpnts[0] = arg->argData.points[idx-1].thePoint;
	up_args.oldpnts[1] = arg->argData.points[idx].thePoint;
    } else {
	/* Middle point */
	up_args.num_old = 3;  up_args.offset = 1;
	up_args.oldpnts[0] = arg->argData.points[idx-1].thePoint;
	up_args.oldpnts[1] = arg->argData.points[idx].thePoint;
	up_args.oldpnts[2] = arg->argData.points[idx+1].thePoint;
    }
    up_args.num_new = 1;
    arg->argData.points[idx] = *pnt;
    up_args.newpnts[0] = arg->argData.points[idx].thePoint;
    _wnForeach(WN_ACTIVE | WN_INTR, lineUpdate, (char *) &up_args);
}



vemStatus argDelPoint(arg, idx)
vemOneArg *arg;			/* Argument to update       */
int idx;			/* Index of point to remove */
/*
 * This routine removes a point from the argument `arg'.  The
 * change will be reflected on the display immediately. Some
 * notes:
 *  - Removing a point from a box deletes both points of the box.
 *  - Removing a point from a line argument which results in
 *    only one remaining point causes all traces of the
 *    line to disappear but the point remains intact.
 *  - Removing the last point of any argument turns off all
 *    on-screen selection and returns the status code VEM_NOMORE.
 *    This normally implies the deletion of the argument from
 *    the argument list.
 *  - Removing a point from a manhattan line is not supported
 *    and funny things might occur if attempted.
 */
{
    vemPoint pntspace[2];
    int j;

    if (arg->length <= 0) return VEM_NOMORE;
    switch (arg->argType) {
    case VEM_POINT_ARG:
	_wnForeach(WN_ACTIVE | WN_INTR, pntDraw,
		   (char *) &(arg->argData.points[idx]));
	arg->length -= 1;
	for (j = idx;  j < arg->length;  j++) {
	    arg->argData.points[j] = arg->argData.points[j+1];
	}
	break;
    case VEM_BOX_ARG:
	if (idx & 1) {
	    /* Odd offset - last point of box */
	    _wnForeach(WN_ACTIVE | WN_INTR, boxDraw,
		       (char *) &(arg->argData.points[idx-1]));
	    arg->length--;
	    for (j = idx-1;  j < arg->length*2;  j++) {
		arg->argData.points[j] = arg->argData.points[j+2];
	    }
	} else {
	    /* Even offset - first point of box */
	    _wnForeach(WN_ACTIVE | WN_INTR, boxDraw,
		       (char *) &(arg->argData.points[idx]));
	    arg->length--;
	    for (j = idx;  j < arg->length*2;  j++) {
		arg->argData.points[j] = arg->argData.points[j+2];
	    }
	}
	break;
    case VEM_LINE_ARG:
	/* Don't know what to do about manhattan lines yet */
	if (idx == 0) {
	    /* First point */
	    if (arg->length > 1) {
		_wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
			   (char *) &(arg->argData.points[0]));
	    }
	    arg->length -= 1;
	    for (j = 0;  j < arg->length;  j++) {
		arg->argData.points[j] = arg->argData.points[j+1];
	    }
	} else if (idx == arg->length-1) {
	    /* Last point */
	    if (arg->length > 1) {
		_wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
			   (char *) &(arg->argData.points[idx-1]));
	    }
	    arg->length -= 1;
	} else {
	    /* Middle point */
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
		       (char *) &(arg->argData.points[idx-1]));
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
		       (char *) &(arg->argData.points[idx]));
	    pntspace[0] = arg->argData.points[idx-1];
	    pntspace[1] = arg->argData.points[idx+1];
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw, (char *) pntspace);
	    arg->length -= 1;
	    for (j = idx;  j <= arg->length;  j++) {
		arg->argData.points[j] = arg->argData.points[j+1];
	    }
	}
	break;
    default:
	vemFail("Type conflict", __FILE__, __LINE__);
    }
    if (arg->length > 0) {
	return VEM_OK;
    } else {
	return VEM_NOMORE;
    }
}



void argInsPoint(arg, idx, pnt)
vemOneArg *arg;			/* Argument to modify  */
int idx;			/* Index of prev point */
vemPoint *pnt;			/* Point to insert     */
/*
 * This routine inserts `pnt' into `arg' at the point at
 * position `idx'.  The necessary screen changes are done
 * immediately.  Some notes:
 *  - Inserting a point into a box argument causes a zero
 *    sized box to be inserted at the indicated spot (two
 *    points are actually inserted).  The programmer should
 *    then change the second one to effect the insertion
 *    of a box.
 *  - For boxes,  the insertion location, `idx', specifies
 *    the box location (not the point offset).
 *  - Insertion into manhattan lines is not supported.  Funny
 *    things may occur if attempted.
 */
{
    vemPoint copyPnt;
    int j;

    copyPnt = *pnt;
    switch (arg->argType) {
    case VEM_POINT_ARG:
	if (arg->length >= arg->alloc_size) {
	    resize(arg);
	}
	for (j = arg->length;  j > idx;  j--) {
	    arg->argData.points[j] = arg->argData.points[j-1];
	}
	arg->argData.points[idx] = copyPnt;
	arg->length += 1;
	_wnForeach(WN_ACTIVE | WN_INTR, pntDraw, (char *) pnt);
	break;
    case VEM_BOX_ARG:
	if (VEMABS(arg->length)*2 >= arg->alloc_size) {
	    resize(arg);
	}
	if (arg->length < 0) {
	    vemFail("Bad Box State", __FILE__, __LINE__);
	}
	/* Make room */
	arg->length += 2;
	for (j = arg->length;  j > idx*2+1;  j--) {
	    arg->argData.points[j] = arg->argData.points[j-2];
	}
	arg->argData.points[idx*2] = copyPnt;
	arg->argData.points[idx*2+1] = copyPnt;
	_wnForeach(WN_ACTIVE | WN_INTR, boxDraw,
		   (char *) &(arg->argData.points[idx*2]));
	break;
    case VEM_LINE_ARG:
	/* Lots of manhattan processing required */
	if (arg->length+1 >= arg->alloc_size) {
	    resize(arg);
	}
	if (idx == 0) {
	    /* Insert first point */
	    arg->length += 1;
	    for (j = arg->length;  j > 0;  j--) {
		arg->argData.points[j] = arg->argData.points[j-1];
	    }
	    arg->argData.points[0] = copyPnt;
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
		       (char *) &(arg->argData.points[0]));
	} else if (idx == arg->length-1) {
	    /* Insert last point */
	    arg->argData.points[arg->length] = copyPnt;
	    arg->length += 1;
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
		       (char *) &(arg->argData.points[idx]));
	} else {
	    /* Middle point */
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
		       (char *) &(arg->argData.points[idx-1]));
	    arg->length += 1;
	    for (j = arg->length;  j > idx;  j--) {
		arg->argData.points[j] = arg->argData.points[j-1];
	    }
	    arg->argData.points[idx] = copyPnt;
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
		       (char *) &(arg->argData.points[idx-1]));
	    _wnForeach(WN_ACTIVE | WN_INTR, lineDraw,
		       (char *) &(arg->argData.points[idx]));
	}
	break;
    default:
	vemFail("Type conflict", __FILE__, __LINE__);
    }
}


static void resize(arg)
vemOneArg *arg;			/* Argument to modify */
/*
 * This routine allocates more space for points when the
 * current space is exhausted.  It also does the initial
 * allocation for the points (if necessary).
 */
{
    if (arg->alloc_size == 0) {
	arg->alloc_size = INITPNTSIZE;
	arg->argData.points = VEMARRAYALLOC(vemPoint, INITPNTSIZE);
    } else {
	arg->alloc_size += arg->alloc_size;
	arg->argData.points = VEMREALLOC(vemPoint, arg->argData.points,
					 arg->alloc_size);
    }
}



/*
 * Hash Table Functions
 *
 * The following macros are used to translate coordinates to physical
 * display coordinates assuming the internal window structure is
 * available in `realWin'.
 */


#define TO_DSPX(xVal) (DISPSCALE((xVal) - realWin->fcts[W_PRI].x_off, realWin->scaleFactor))

#define TO_DSPY(yVal) \
  realWin->height - DISPSCALE((yVal) - realWin->fcts[W_PRI].y_off, realWin->scaleFactor)


/*ARGSUSED*/
static enum st_retval sweepDraw(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the hash table of
 * active windows to draw (or undraw) a complete argument
 * list item.  IMPORTANT:  note the routine assumes the
 * appropriate display attribute has been set with atrSet().
 */
{
    internalWin *realWin = (internalWin *) value;
    vemOneArg *thearg = (vemOneArg *) arg;
    int j;

    switch (thearg->argType) {
    case VEM_POINT_ARG:
	{
	    struct octPoint pnt;
	    
	    for (j = 0;  j < thearg->length;  j++) {
		if (thearg->argData.points[j].theFct ==
		    realWin->fcts[W_PRI].winFctId) {
		    pnt = thearg->argData.points[j].thePoint;
		    pnt.x = TO_DSPX(pnt.x);
		    pnt.y = TO_DSPY(pnt.y);
		    dspPoint(realWin->XWin, &pnt, DSP_XMARK);
		}
	    }
	}
	break;
    case VEM_BOX_ARG:
	{
	    struct octBox box;
	    int len, temp;

	    if (thearg->length < 0) {
		len = (-thearg->length) * 2 - 2;
	    } else {
		len = thearg->length * 2;
	    }
	    for (j = 0;  j < len;  j += 2) {
		if ((thearg->argData.points[j].theFct == realWin->fcts[W_PRI].winFctId)&&
		    (thearg->argData.points[j+1].theFct == realWin->fcts[W_PRI].winFctId))
		  {
		      box.lowerLeft = thearg->argData.points[j].thePoint;
		      box.upperRight = thearg->argData.points[j+1].thePoint;
		      VEMBOXNORM(box, temp);
		      box.lowerLeft.x = TO_DSPX(box.lowerLeft.x);
		      box.upperRight.x = TO_DSPX(box.upperRight.x);
		      box.lowerLeft.y = TO_DSPY(box.lowerLeft.y);
		      box.upperRight.y = TO_DSPY(box.upperRight.y);
		      dspRegion(realWin->XWin, &box);
		  }
	    }
	}
	break;
    case VEM_LINE_ARG:
	{
	    struct octPoint pnt1, pnt2;

	    for (j = 0;  j < thearg->length-1;  j++) {
		if ((thearg->argData.points[j].theFct == realWin->fcts[W_PRI].winFctId)&&
		    (thearg->argData.points[j+1].theFct == realWin->fcts[W_PRI].winFctId))
		  {
		      pnt1 = thearg->argData.points[j].thePoint;
		      pnt2 = thearg->argData.points[j+1].thePoint;
		      pnt1.x = TO_DSPX(pnt1.x);
		      pnt2.x = TO_DSPX(pnt2.x);
		      pnt1.y = TO_DSPY(pnt1.y);
		      pnt2.y = TO_DSPY(pnt2.y);
		      dspLine(realWin->XWin, &pnt1, &pnt2);
		  }
	    }
	}
	break;
    }
    return ST_CONTINUE;
}


/*ARGSUSED*/
static enum st_retval pntDraw(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the hash table of
 * active windows to draw (or undraw) a particular point
 * passed as an argument. IMPORTANT:  note the routine assumes the
 * appropriate display attribute has been set with atrSet().
 * Since all of these functions are GXxor,  the distinction
 * between drawing and undrawing is unimportant.
 */
{
    internalWin *realWin = (internalWin *) value;
    vemPoint *pnts = (vemPoint *) arg;
    struct octPoint onepnt;

    if (pnts->theFct == realWin->fcts[W_PRI].winFctId) {
	onepnt = pnts->thePoint;
	onepnt.x = TO_DSPX(onepnt.x);
	onepnt.y = TO_DSPY(onepnt.y);
	dspPoint(realWin->XWin, &onepnt, DSP_XMARK);
    }
    return ST_CONTINUE;
}


/*ARGSUSED*/
static enum st_retval boxDraw(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the hash table of
 * active windows to draw (or undraw) a particular box
 * passed as an argument. IMPORTANT:  note the routine assumes the
 * appropriate display attribute has been set with atrSet().
 * Since all of these functions are GXxor,  the distinction
 * between drawing and undrawing is unimportant.
 */
{
    internalWin *realWin = (internalWin *) value;
    vemPoint *pnts = (vemPoint *) arg;
    struct octBox box;
    int temp;

    if ((pnts[0].theFct == realWin->fcts[W_PRI].winFctId)&&
	(pnts[1].theFct == realWin->fcts[W_PRI].winFctId))
      {
	  box.lowerLeft = pnts[0].thePoint;
	  box.upperRight = pnts[1].thePoint;
	  VEMBOXNORM(box, temp);
	  box.lowerLeft.x = TO_DSPX(box.lowerLeft.x);
	  box.upperRight.x = TO_DSPX(box.upperRight.x);
	  box.lowerLeft.y = TO_DSPY(box.lowerLeft.y);
	  box.upperRight.y = TO_DSPY(box.upperRight.y);
	  dspRegion(realWin->XWin, &box);
      }
    return ST_CONTINUE;
}


/*ARGSUSED*/
static enum st_retval lineDraw(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the hash table of
 * active windows to draw (or undraw) a particular line
 * passed as an argument. IMPORTANT:  note the routine assumes the
 * appropriate display attribute has been set with atrSet().
 * Since all of these functions are GXxor,  the distinction
 * between drawing and undrawing is unimportant.
 */
{
    internalWin *realWin = (internalWin *) value;
    vemPoint *pnts = (vemPoint *) arg;
    struct octPoint p1, p2;

    if ((pnts[0].theFct == realWin->fcts[W_PRI].winFctId) &&
	(pnts[1].theFct == realWin->fcts[W_PRI].winFctId))
      {
	  p1 = pnts[0].thePoint;
	  p2 = pnts[1].thePoint;
	  p1.x = TO_DSPX(p1.x);
	  p2.x = TO_DSPX(p2.x);
	  p1.y = TO_DSPY(p1.y);
	  p2.y = TO_DSPY(p2.y);
	  dspLine(realWin->XWin, &p1, &p2);
      }
    return ST_CONTINUE;
}




/*ARGSUSED*/
static enum st_retval pntUpdate(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the hash table of
 * active windows to update a point passed as an argument.
 * The function both undraws and draws the point.  IMPORTANT:  
 * note the routine assumes the appropriate display attribute 
 * has been set with atrSet(). Since all of these functions are 
 * GXxor,  the distinction between drawing and undrawing is
 * unimportant.
 */
{
    internalWin *realWin = (internalWin *) value;
    register struct update_args *up_args = (struct update_args *) arg;
    struct octPoint onepnt;

    if (up_args->fct == realWin->fcts[W_PRI].winFctId) {
	onepnt.x = TO_DSPX(up_args->oldpnts[0].x);
	onepnt.y = TO_DSPY(up_args->oldpnts[0].y);
	dspPoint(realWin->XWin, &onepnt, DSP_XMARK);
	onepnt.x = TO_DSPX(up_args->newpnts[0].x);
	onepnt.y = TO_DSPY(up_args->newpnts[0].y);
	dspPoint(realWin->XWin, &onepnt, DSP_XMARK);
    }
    return ST_CONTINUE;
}



/*ARGSUSED*/
static enum st_retval box1Update(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the hash table of
 * active windows to update a particular box on the screen which is
 * passed as an argument. This one assumes the last point of the
 * box is updated.  IMPORTANT:  note the routine assumes the
 * appropriate display attribute has been set with atrSet().
 * Since all of these functions are GXxor,  the distinction
 * between drawing and undrawing is unimportant.
 */
{
    internalWin *realWin = (internalWin *) value;
    register struct update_args *up_args = (struct update_args *) arg;
    struct octBox box;
    int temp;

    if (up_args->fct == realWin->fcts[W_PRI].winFctId) {
	box.lowerLeft = up_args->oldpnts[0];
	box.upperRight = up_args->oldpnts[1];
	VEMBOXNORM(box, temp);
	box.lowerLeft.x = TO_DSPX(box.lowerLeft.x);
	box.upperRight.x = TO_DSPX(box.upperRight.x);
	box.lowerLeft.y = TO_DSPY(box.lowerLeft.y);
	box.upperRight.y = TO_DSPY(box.upperRight.y);
	dspRegion(realWin->XWin, &box);
	/* Turn it on in its new place - Second point modified */
	box.lowerLeft = up_args->oldpnts[0];
	box.upperRight = up_args->newpnts[0];
	VEMBOXNORM(box, temp);
	box.lowerLeft.x = TO_DSPX(box.lowerLeft.x);
	box.upperRight.x = TO_DSPX(box.upperRight.x);
	box.lowerLeft.y = TO_DSPY(box.lowerLeft.y);
	box.upperRight.y = TO_DSPY(box.upperRight.y);
	dspRegion(realWin->XWin, &box);
    }
    return ST_CONTINUE;
}


/*ARGSUSED*/
static enum st_retval box2Update(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the hash table of
 * active windows to update a particular box on the screen which is
 * passed as an argument.  This routine assumes the first point
 * of the box is modified.  IMPORTANT:  note the routine assumes the
 * appropriate display attribute has been set with atrSet().
 * Since all of these functions are GXxor,  the distinction
 * between drawing and undrawing is unimportant.
 */
{
    internalWin *realWin = (internalWin *) value;
    register struct update_args *up_args = (struct update_args *) arg;
    struct octBox box;
    int temp;

    if (up_args->fct == realWin->fcts[W_PRI].winFctId) {
	box.lowerLeft = up_args->oldpnts[0];
	box.upperRight = up_args->oldpnts[1];
	VEMBOXNORM(box, temp);
	box.lowerLeft.x = TO_DSPX(box.lowerLeft.x);
	box.upperRight.x = TO_DSPX(box.upperRight.x);
	box.lowerLeft.y = TO_DSPY(box.lowerLeft.y);
	box.upperRight.y = TO_DSPY(box.upperRight.y);
	dspRegion(realWin->XWin, &box);
	/* Turn it on in its new place - First point is modified */
	box.lowerLeft = up_args->newpnts[0];
	box.upperRight = up_args->oldpnts[1];
	VEMBOXNORM(box, temp);
	box.lowerLeft.x = TO_DSPX(box.lowerLeft.x);
	box.upperRight.x = TO_DSPX(box.upperRight.x);
	box.lowerLeft.y = TO_DSPY(box.lowerLeft.y);
	box.upperRight.y = TO_DSPY(box.upperRight.y);
	dspRegion(realWin->XWin, &box);
    }
    return ST_CONTINUE;
}

/*ARGSUSED*/
static enum st_retval lineUpdate(key, value, arg)
char *key, *value, *arg;
/*
 * This function is passed to st_foreach on the hash table of
 * active windows to update a line on the argument list.
 * The old and new points for the line are given by `arg'
 * which is acutally a pointer to struct update_args.
 * IMPORTANT:  note the routine assumes the
 * appropriate display attribute has been set with atrSet().
 * Since all of these functions are GXxor,  the distinction
 * between drawing and undrawing is unimportant.
 */
{
    internalWin *realWin = (internalWin *) value;
    register struct update_args *real_arg = (struct update_args *) arg;
    struct octPoint draw_ary[MAX_OLDPNTS];
    register int i;

    if (real_arg->fct == realWin->fcts[W_PRI].winFctId) {
	/* Convert all points */
	for (i = 0;  i < real_arg->num_old;  i++) {
	    draw_ary[i].x = TO_DSPX(real_arg->oldpnts[i].x);
	    draw_ary[i].y = TO_DSPY(real_arg->oldpnts[i].y);
	}
	/* Undraw old */
	for (i = 0;  i < real_arg->num_old-1;  i++) {
	    dspLine(realWin->XWin, &(draw_ary[i]), &(draw_ary[i+1]));
	}
	/* Replace old with new */
	for (i = 0;  i < real_arg->num_new;  i++) {
	    draw_ary[i + real_arg->offset].x = TO_DSPX(real_arg->newpnts[i].x);
	    draw_ary[i + real_arg->offset].y = TO_DSPY(real_arg->newpnts[i].y);
	}
	/* Draw the new */
	for (i = 0;  i < real_arg->num_old-1;  i++) {
	    dspLine(realWin->XWin, &(draw_ary[i]), &(draw_ary[i+1]));
	}
    }
    return ST_CONTINUE;
}


/*ARGSUSED*/
void argRedraw(realWin, virtual, physical)
internalWin *realWin;		/* Internal window structure */
struct octBox *virtual;		/* Virtual region redrawn    */
struct octBox *physical;	/* Physical region redrawn   */
/*
 * This routine is called by the window module to update selection
 * in a particular region of a window.  Assumes both the selection
 * attribute and the drawing region have been set up properly
 * before this routine is called.
 */
{
    lsGen theGen;
    vemOneArg *anArg;
    struct octBox bb;
    int j, len, temp;

    if (!argList) {
	argList = lsCreate();
    }

    theGen = lsStart(argList);
    while (lsNext(theGen, (lsGeneric *) &anArg, LS_NH) == VEM_OK) {
	switch (anArg->argType) {
	case VEM_POINT_ARG:
	    for (j = 0;  j < anArg->length;  j++) {
		if (anArg->argData.points[j].theFct == realWin->fcts[W_PRI].winFctId) {
		    bb.lowerLeft = anArg->argData.points[j].thePoint;
		    bb.upperRight = bb.lowerLeft;
		    if (VEMBOXINTERSECT(*virtual, bb)) {
			TRANX(*realWin, *physical, W_PRI,  bb.lowerLeft.x);
			TRANY(*realWin, *physical, W_PRI,  bb.lowerLeft.y);
			dspPoint((Window) 0, &(bb.lowerLeft), DSP_XMARK);
		    }
		}
	    }
	    break;
	case VEM_BOX_ARG:
	    if (anArg->length < 0) {
		len = (-anArg->length) * 2 - 1;
	    } else {
		len = anArg->length * 2;
	    }
	    for (j = 0;  j < len;  j += 2) {
		if ((anArg->argData.points[j].theFct == realWin->fcts[W_PRI].winFctId)&&
		    (anArg->argData.points[j+1].theFct == realWin->fcts[W_PRI].winFctId))
		  {
		      bb.lowerLeft = anArg->argData.points[j].thePoint;
		      bb.upperRight = anArg->argData.points[j+1].thePoint;
		      VEMBOXNORM(bb, temp);
		      if (VEMBOXINTERSECT(*virtual, bb)) {
			  TRANX(*realWin, *physical, W_PRI,  bb.lowerLeft.x);
			  TRANY(*realWin, *physical, W_PRI,  bb.lowerLeft.y);
			  TRANX(*realWin, *physical, W_PRI,  bb.upperRight.x);
			  TRANY(*realWin, *physical, W_PRI,  bb.upperRight.y);
			  dspRegion((Window) 0, &bb);
		      }
		  }
	    }
	    break;
	case VEM_LINE_ARG:
	    for (j = 0;  j < anArg->length-1;  j++) {
		if ((anArg->argData.points[j].theFct == realWin->fcts[W_PRI].winFctId)&&
		    (anArg->argData.points[j+1].theFct == realWin->fcts[W_PRI].winFctId))
		  {
		      bb.lowerLeft = anArg->argData.points[j].thePoint;
		      bb.upperRight = anArg->argData.points[j+1].thePoint;
		      VEMBOXNORM(bb, temp);
		      if (VEMBOXINTERSECT(*virtual, bb)) {
			  bb.lowerLeft = anArg->argData.points[j].thePoint;
			  bb.upperRight = anArg->argData.points[j+1].thePoint;
			  TRANX(*realWin, *physical, W_PRI,  bb.lowerLeft.x);
			  TRANY(*realWin, *physical, W_PRI,  bb.lowerLeft.y);
			  TRANX(*realWin, *physical, W_PRI,  bb.upperRight.x);
			  TRANY(*realWin, *physical, W_PRI,  bb.upperRight.y);
			  dspLine((Window) 0, &(bb.lowerLeft), &(bb.upperRight));
		      }
		  }
	    }
	    break;
	}
    }
    lsFinish(theGen);
}
