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
 * Move Objects in Symbolic
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1989, 1990
 *
 * Move a set of objects without perturbing connectivity (drags
 * connection end-points).
 */

#include "internal.h"
#include "terminfo.h"
#include "move.h"

#define DRAG_SET_BAG	";;; Drag Set Bag ;;;"

/* Forward declarations */
static void find_drag_set();
static void drag_items();
static void drag_inst();
static void do_term();



void symMoveObjects(set, transform, move_func)
octObject *set;			/* Set of objects to drag     */
struct octTransform *transform;	/* Transformation for objects */
symMoveItemFunc *move_func;	/* Move item callback         */
/*
 * This routine moves the objects in `set' to a new location
 * computed by applying `transformation' to all objects in
 * the set.  Unlike symMoveObjects(), this routine does not
 * change connectivity.  Instead, segment endpoints are moved
 * with the instances.  `move_func' is a function that is
 * called when the actual move of an object is performed.
 * The function will be called twice consecutively for each
 * item (once before the move and once after).  See symMoveItemFunc
 * for the argument form.  `item' will be item moved.  `after_p'
 * will be non-zero if this is the call after the move.  `data'
 * is the same as passed to this function.
 *
 * Implementation notes:
 *  - Builds set of pure instances (and other geometry) to move.
 *  - Moves the instances and connected segments.
 */
{
    octObject fct, real_set;

    octGetFacet(set, &fct);

    /* Build actual instance/geometry set */
    real_set.type = OCT_BAG;  real_set.contents.bag.name = DRAG_SET_BAG;
    SYMCK(octCreate(&fct, &real_set));
    find_drag_set(set, &real_set);

    /* Now do actual move */
    drag_items(&real_set, transform, move_func);
    SYMCK(octDelete(&real_set));
}



static void find_drag_set(in, out)
octObject *in;			/* Orginal set of items */
octObject *out;			/* Output set of items  */
/*
 * This routine examines `in' and returns `out'.  All instances
 * in `in' are also attached to `out'.  Paths attached to `in'
 * have the instances on both ends of the path added to `out'.
 * Other kinds of geometric objects are copied from `in' to
 * `out'.  The result is a bag with just instances and other
 * geometry.
 */
{
    octGenerator gen, path_gen;
    octObject obj, term, inst;

    SYMCK(octInitGenContents(in, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	if (obj.type == OCT_PATH) {
	    /* Add endpoint instances - if not already on it */
	    SYMCK(octInitGenContents(&obj, OCT_TERM_MASK, &path_gen));
	    while (octGenerate(&path_gen, &term) == OCT_OK) {
		inst.objectId = term.contents.term.instanceId;
		if (!octIdIsNull(inst.objectId)) {
		    SYMCK(octGetById(&inst));
		    SYMCK(octAttachOnce(out, &inst));
		}
	    }
	} else {
	    /* Add verbatim */
	    SYMCK(octAttachOnce(out, &obj));
	}
    }
}



static void drag_items(set, transform, move_func)
octObject *set;			/* Set of instances and labels */
struct octTransform *transform;	/* Transformation for objects  */
symMoveItemFunc *move_func;	/* Move item callback          */
/*
 * This routine drags the objects in `set' through `transform'.
 * It assumes the set has no paths.  Instances that are moved
 * will cause paths attached to their end-point connectors
 * to be moved appropriately.  Other geometries are simply
 * moved through the transform and no connectivity is updated.
 */
{
    octGenerator gen;
    octObject obj;

    SYMCK(octInitGenContents(set, OCT_ALL_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	if (obj.type == OCT_INSTANCE) {
	    drag_inst(&obj, transform, move_func);
	} else {
	    if (move_func) (*move_func->func)(&obj, 0, move_func->data);
	    SYMCK(octTransformAndModifyGeo(&obj, transform));
	    if (move_func) (*move_func->func)(&obj, 1, move_func->data);
	}
    }
}



static void drag_inst(inst, tran, move_func)
octObject *inst;		/* Instance to drag     */
struct octTransform *tran;	/* Transformation       */
symMoveItemFunc *move_func;	/* Move item callback   */
/*
 * This routine drags the indicated instance through `tran'.
 * Paths attached to the terminal are also moved.
 */
{
    octGenerator gen;
    octObject inst_term;

    /* First move the path endpoints */
    SYMCK(octInitGenContents(inst, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &inst_term) == OCT_OK) {
	do_term(&inst_term, tran, move_func);
    }
    /* Move the instance */
    if (move_func) (*move_func->func)(inst, 0, move_func->data);
    SYMCK(octTransformAndModifyGeo(inst, tran));
    if (move_func) (*move_func->func)(inst, 1, move_func->data);
}



static void do_term(i_term, tran, move_func)
octObject *i_term;		/* Instance terminal */
struct octTransform *tran;	/* Transformation    */
symMoveItemFunc *move_func;	/* Move callback     */
/*
 * This routine moves the endpoints of all segments connected
 * to `i_term' a distance given by `dx' and `dy'.  The necessary
 * redraw regions are queued for redisplay.
 */
{
    octObject path, move_point;
    octGenerator gen;
    struct octPoint pnts[2];
    segTermInfo info[2];
    int32 np = 2;

    move_point.type = OCT_POINT;
    SYMCK(octInitGenContainers(i_term, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &path) == OCT_OK) {
	/* Call before modification */
	if (move_func) (*move_func->func)(&path, 0, move_func->data);
	/* Get points */
	SYMCK(octGetPoints(&path, &np, pnts));
	/* Try to match endpoint to terminal */
	symSegTerms(&path, pnts, info);
	/* Apply dx, dy to point corresponding to i_term */
	if (octIdsEqual(info[0].term.objectId, i_term->objectId)) {
	    move_point.contents.point = pnts[0];
	    octTransformGeo(&move_point, tran);
	    pnts[0] = move_point.contents.point;
	} else if (octIdsEqual(info[1].term.objectId, i_term->objectId)) {
	    move_point.contents.point = pnts[1];
	    octTransformGeo(&move_point, tran);
	    pnts[1] = move_point.contents.point;
	}
	SYMCK(octPutPoints(&path, np, pnts));
	/* Call after function */
	if (move_func) (*move_func->func)(&path, 1, move_func->data);
    }
}
