#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/*
 * Symbolic segments
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1990
 *
 * This file contains an improved implementation of path
 * generation in symbolic.  It is an interface over
 * the symbolic policy library.
 */

#include "general.h"
#include "symbolic.h"
#include "oct.h"
#include "commands.h"
#include "list.h"
#include "vemUtil.h"
#include "message.h"
#include "defaults.h"
#include "vemDM.h"
#include "se.h"
#include <math.h>

/*
 * Context for get terminal routine.  Kind of simulates co-routines.
 * see gt() and gt_do() for details.
 */

typedef enum gt_what_defn {
    gt_nothing,			/* Don't need to do anything     */
    gt_breakseg,		/* Break a segment               */
    gt_simpleconn,		/* Put down a simple connector   */
    gt_upgrade			/* Upgrade an existing connector */
} gt_what;

typedef struct gt_context_defn {
    /* Parameters */
    octObject *fct;		/* Facet to look in */
    octObject *lyr;		/* Layer of segment */
    octCoord width;		/* Width of segment */
    Window win;			/* What window (for gravity) */
    struct octPoint *pnt;	/* What point to look at */
    struct octPoint *off_pnt;	/* Other point of segment */
    symConFunc *confun;		/* Connector function */
    octObject *term;		/* Terminal (returned) */

    /* Local context */
    gt_what what;		/* What to do */
    tapLayerListElement elem;	/* New segment description */
    octObject seg;		/* Intersecting segment */
    octObject new_conn;		/* Upgraded connector */
    octObject old_conn;		/* Old connector */
    struct octPoint sp[2];	/* Intersecting segment points */
} gt_context;

static void do_path
  ARGS((octObject *fct, octObject *lyr, octCoord width,
	Window w_first, Window w_last,
	int num, struct octPoint *pnts, symConFunc *confun));

static int get_term
  ARGS((gt_context *ct, octObject *fct, octObject *lyr, octCoord width,
	Window win, struct octPoint *pnt, struct octPoint *off_pnt,
	symConFunc *confun, octObject *term));

static int gt
  ARGS((gt_context *ct));

static void gt_do
  ARGS((gt_context *ct));

static vemStatus handle_layer
  ARGS((vemPoint *spot, lsList cmdList, octObject *fct, octObject *lyr));

static vemStatus ckLines
  ARGS((lsList arg_list));

static int ckNearTerms
  ARGS((octObject *fct, struct octPoint *pnt, tapLayerListElement *elem,
	symConFunc *confun));



vemStatus seCreatePath(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This routine parses a command of the following form:
 *  lines(n) lines(n) ... lines(n) text
 * `text' specifies the layer for the path.  If it is omitted,
 * the layer is determined using `spot'.
 */
{
    octObject target_fct, pathLayer;
    vemStatus rt;

    /* All arguments should be in same buffer */
    if (vuCkBuffer(cmdList, &(target_fct.objectId)) != VEM_OK) return VEM_ARGRESP;
    if (bufWritable(target_fct.objectId) != VEM_OK) {
	vemMsg(MSG_C, "Buffer is not writable\n");
	return VEM_ARGRESP;
    }

    bufMarkActive( target_fct.objectId );

    /* Determine the layer */
    VEM_OCTCKRVAL(octGetById(&target_fct), VEM_CORRUPT);
    if ((rt = handle_layer(spot, cmdList, &target_fct, &pathLayer)) != VEM_OK) {
	return rt;
    }

    return seBasePath(&target_fct, cmdList, &pathLayer, symDefConFunc);
}


vemStatus seBasePath(fct, cmdList, lyr, confun)
octObject *fct;			/* Target facet             */
lsList cmdList;			/* Argument list            */
octObject *lyr;			/* What layer to use        */
symConFunc *confun;		/* What connector function  */
/*
 * This routine performs parsing of the boyd of a create segments
 * command.  It is assumed the layer has already been determined.
 * This is used as the export point for schematic segment creation.
 */
{
    octCoord width;
    struct octPoint *pnts;
    lsGen gen;
    int i;
    vemOneArg *arg;

    /* Make sure all arguments are lines */
    if (ckLines(cmdList) != VEM_OK) {
	vemMsg(MSG_C, "All arguments must be lines\n");
	return VEM_ARGRESP;
    }

    /* Determine path width */
    width = bufPathWidth(fct->objectId, lyr);

    gen = lsStart(cmdList);
    while (lsNext(gen, (Pointer *) &arg, LS_NH) == LS_OK) {
	/* Make a copy of the points -- they may be modified by gravity */
	pnts = VEMARRAYALLOC(struct octPoint, arg->length);
	for (i = 0;  i < arg->length;  i++) {
	    pnts[i] = arg->argData.points[i].thePoint;
	}

	do_path(fct, lyr, width,
		arg->argData.points[0].theWin,
		arg->argData.points[arg->length-1].theWin,
		arg->length,
		pnts,
		confun);
	VEMFREE(pnts);
    }
    lsFinish(gen);
    return VEM_OK;
}





static void do_path(fct, lyr, width, w_first, w_last, num, pnts, confun)
octObject *fct;			/* Facet to modify                     */
octObject *lyr;			/* Layer for path                      */
octCoord width;			/* Path width                          */
Window w_first, w_last;		/* First and last window (for gravity) */
int num;			/* Number of points                    */
struct octPoint *pnts;		/* Points themselves                   */
symConFunc *confun;		/* Connector function                  */
/*
 * This routine creates a symbolic path.  It examines the first and
 * last points to find terminals under them.  This is done in two
 * phases: first the points are examined non-destructively using gt().
 * If both terminals can be found or generated, gt_do() is called
 * to effect the changes.  If both endpoint terminals can be generated,
 * the segment is created.
 */
{
    octObject t1, t2;
    gt_context c1, c2;

    if (get_term(&c1, fct, lyr, width, w_first, pnts, pnts+1, confun, &t1) ) {
	if ( get_term(&c2, fct, lyr, width, w_last, pnts+num-1, pnts+num-2, confun, &t2)) {
	    gt_do(&c1); 
	    gt_do(&c2);
	    symSegments(fct, &t1, &t2, num, pnts, 1, lyr, 1, &width, confun);
	}
    }
}


static int get_term(ct, fct, lyr, width, win, pnt, off_pnt, confun, term)
gt_context *ct;			/* Context               */
octObject *fct;			/* Facet to look in      */
octObject *lyr;			/* Layer of segment      */
octCoord width;			/* Width of segment      */
Window win;			/* What window (gravity) */
struct octPoint *pnt;		/* Point                 */
struct octPoint *off_pnt;	/* Other point of segment */
symConFunc *confun;		/* Connector function    */
octObject *term;		/* Terminal (returned)   */
/*
 * This is a wrapper routine for gt().  It wraps up the passed parameters
 * in `ct' and calls `gt'.
 */
{
    ct->fct = fct;
    ct->lyr = lyr;
    ct->width = width;
    ct->win = win;
    ct->pnt = pnt;
    ct->off_pnt = off_pnt;
    ct->confun = confun;
    ct->term = term;
    return gt(ct);
}


static int gt(ct)
gt_context *ct;			/* Get terminal context */
/*
 * This routine does the non-destructive part of attempting
 * to find a terminal under a given point.  It first tries
 * to find an appropriate actual terminal terminal under
 * the point.  If the terminal can't be connected but can
 * be upgraded, it will be returned.  If that fails, it looks 
 * for an appropriate segment to break.  If that doesn't work,
 * it assumes putting down a simple connector is good enough.
 * Note that this routine doesn't actually break segments
 * upgrade connectors,  or put down new connectors.  That
 * is done by gt_do().  The routine returns a non-zero status
 * if successful, zero if not.
 */
{
    octCoord diff_x, diff_y;
    int result;
    int expert = vemExpert( 0 );
    static char *msg =
"One of the segment endpoints intersects a terminal that is\n\
not the right size or layer.\n\
\n\
Press the Continue button to try create a simple connector\n\
and connect the endpoint to that connector.\n\
Or Cancel the operation and try again.\n";

    /* Build new segment description */
    ct->elem.layer = *(ct->lyr);
    ct->elem.width = ct->width;
    diff_x = (ct->pnt->x - ct->off_pnt->x);
    diff_y = (ct->pnt->y - ct->off_pnt->y);
#ifdef OLDCODE
    if (diff_x > diff_y) {
	ct->elem.direction = TAP_HORIZONTAL;
    } else {
	ct->elem.direction = TAP_VERTICAL;
    }
#endif
    ct->elem.angle = 0;		/* Flag for a manhattan path. */
    if (diff_x == 0 ) {
	ct->elem.direction = TAP_HORIZONTAL;
    } else if ( diff_y == 0 ) {
	ct->elem.direction = TAP_VERTICAL;
    } else {
	/* ct->elem.direction = TAP_WEIRD_DIR; */ /* Even if TAP does not support it yet. */
	ct->elem.direction = TAP_HORIZONTAL; /* Because this is supported . */
	ct->elem.angle = atan( (double)diff_y/ (double)diff_x ) * 180.0 / 3.14159; /* But the angle will be correct. */
    }
	
    result = seSegActual(ct->fct, ct->win, ct->pnt, &(ct->elem),
			 ct->confun, ct->term);
    if (result <= 0) {
	if (result == INAPPROPRIATE) {
	    if (expert) {
		vemMsg(MSG_C, "WARNING: Terminal is not the right size or layer\n");
		vemMsg(MSG_C, "         for segment.  No connection to the terminal\n");
		vemMsg(MSG_C, "         will be made.  Attempting to create a\n");
		vemMsg(MSG_C, "         simple connector instead.\n");
	    } else {
		if (dmConfirm("Inappropriate Terminal", msg, "Continue", "Cancel") != VEM_OK) {
		    return 0;
		}
	    }
	}

	if (seFindSeg(ct->fct, ct->win, ct->pnt, &(ct->elem), ct->confun, &(ct->seg), ct->sp)) {
	    if (ckNearTerms(ct->fct, ct->pnt, &(ct->elem), ct->confun)) {
		ct->what = gt_breakseg;
		return 1;
	    } 
	    return 0; 		/* Failed to meet consequences */
	} 

	{
	    char msg[SYM_MAX_MSG];

	    /* Check to see if we can put down an end-point connector */
	    if ((*ct->confun->func)(1, &(ct->elem), &(ct->new_conn), msg, ct->confun->data)) {
		/* Simple end-point connector */
		ct->what = gt_simpleconn;
		return 1;
	    } else {
		if (strlen(msg)) {
		    vemMsg(MSG_C, "ERROR: Segment not created:\n  %s\n",
			   msg);
		} else {
		    vemMsg(MSG_C,
			   "ERROR: Segment not created:\n  No appropriate connector found.\n");
		}
		return 0;
	    }
	}
    } else if (symContactP(ct->term) && !symSegConnP(ct->term, &(ct->elem))) {
	/* Terminal must be upgraded before returning */
	/* This should work because seSegActual returned it */
	ct->what = gt_upgrade;
	return 1;
    }
    ct->what = gt_nothing;
    return 1;
}


static void gt_do(ct)
gt_context *ct;			/* Get terminal context */
/*
 * Having found that both end-point terminals can be either
 * located or generated, this routine actually carries out
 * the operations specified by `ct'.  The effect is kind of
 * like co-routines.  All of the non-destructive work is
 * done in `gt'.  If both calls to `gt' are `ok', this
 * routine is called to modify the facet appropriately.
 */
{
    octObject nc;

    switch (ct->what) {
    case gt_nothing:
	/* Nothing required */
	break;
    case gt_breakseg:
	/* Break segment */
	seBreakSeg(ct->fct, &(ct->seg), ct->sp, &(ct->elem),
		   ct->pnt, ct->off_pnt, ct->confun, ct->term);
	break;
    case gt_simpleconn:
	symConnector(ct->fct, ct->pnt->x, ct->pnt->y, 1, &(ct->elem),
		     ct->confun, &nc);
	(void) octGenFirstContent(&nc, OCT_TERM_MASK, ct->term);
	break;
    case gt_upgrade:
	ct->old_conn.objectId = ct->term->contents.term.instanceId;
	VEM_OCTCKRET(octGetById(&(ct->old_conn)));
	if (symUpgrade(ct->term, &(ct->elem), ct->confun, &(ct->new_conn))) {
	    ct->new_conn.contents.instance.name = ct->old_conn.contents.instance.name;
	    ct->new_conn.contents.instance.transform = ct->old_conn.contents.instance.transform;
	    symReplaceInst(ct->fct, &(ct->old_conn), &(ct->new_conn));
	    VEM_OCTCKRET(octGenFirstContent(&(ct->new_conn),
					    OCT_TERM_MASK, ct->term));
	}
	break;
    }
}




static vemStatus handle_layer(spot, cmdList, fct, lyr)
vemPoint *spot;			/* Where command was invoked */
lsList cmdList;			/* Argument list             */
octObject *fct;			/* Facet for layer           */
octObject *lyr;			/* Returned layer            */
/*
 * This routine looks for a layer specification at the end of
 * the arg list.  If its not found,  it tries to determine it
 * from `spot'.  Once found,  it is retrieved from the database
 * (or created).  If successful,  it will return in the layer in `lyr'
 * and return a non-zero status.
 */
{
    octObject spot_fct;

    spot_fct.objectId = spot->theFct;
    if (octGetById(&spot_fct) != OCT_OK) {
	vemMsg(MSG_C, "Unable to determine layer:\n  %s\n", octErrorString());
	return VEM_ARGRESP;
    }

    if (vuParseLayer(spot, cmdList, &spot_fct, lyr) != VEM_OK) {
	vemMessage("Unable to determine layer\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }
    lyr->type = OCT_LAYER;
    if (octGetByName(fct, lyr) != OCT_OK) {
	/* Check technology */
	if (tapIsLayerDefined(fct, lyr->contents.layer.name) == OCT_OK) {
	    /* Layer is in technolgy.  Create it. */
	    if (octCreate(fct, lyr) != OCT_OK) {
		(void) sprintf(errMsgArea, "Cannot create layer `%s':\n  %s\n",
			       lyr->contents.layer.name, octErrorString());
		vemMessage(errMsgArea, MSG_DISP);
		return VEM_ARGRESP;
	    }
	} else {
	    (void) sprintf(errMsgArea, "Layer `%s' is not in the technology\n",
			   lyr->contents.layer.name);
	    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
    }
    return VEM_OK;
}



static int ckNearTerms(fct, pnt, elem, confun)
octObject *fct;			/* What facet to check */
struct octPoint *pnt;		/* Segment endpoint    */
tapLayerListElement *elem;	/* Segment description */
symConFunc *confun;		/* Connector function  */
/*
 * This routine checks for actual terminals within `width'
 * of `pnt'.  If found, this means when the segment is
 * broken, the new connector will overlap the actual terminals
 * resulting in possibly non-intuitive connectivity maintenence.
 * This routine warns the user with a dialog if this situation 
 * occurs.  If expert mode is on, only a warning will be produced.
 */
{
    seLwInfo lwi;
    octObject *aterms;
    struct octBox extent;
    int num, half_size, result;
    int expert = vemExpert( 0 );
    static char *message =
"You have asked for a segment that will break another segment.\n\
The resulting connector overlaps an existing actual terminal.\n\
\n\
If you wish to connect the segment to the existing TERMINAL, you\n\
should draw the endpoint directly to that terminal.\n\
\n\
If you wish to connect the segment to the existing SEGMENT, you \n\
should draw the endpoint farther from the existing terminal.\n\
\n\
If you press the Continue button, VEM will connect the segment\n\
endpoint in a way that you might find confusing (but it is correct).\n\
\n\
You can try specifying the segment again by pressing the Cancel button.\n\
\n";

    result = 1;
    half_size = (elem->width/2);
    extent.lowerLeft = extent.upperRight = *pnt;
    extent.lowerLeft.x -= half_size;  extent.upperRight.x += half_size;
    extent.lowerLeft.y -= half_size;  extent.upperRight.y += half_size;
    lwi.elem = elem;
    lwi.confun = confun;
    num = seActualSearch(fct, &extent, seConnectP, (char *) &lwi, &aterms);
    if (num > 0) {
	if (expert) {
	    vemMsg(MSG_C, "WARNING: Segment endpoint connector overlaps existing terminal.\n");
	    vemMsg(MSG_C, "         Resulting connectivity may be non-intuitive.\n");
	} else {
	    if (dmConfirm("Overlapping Terminals", message, "Continue", "Cancel") != VEM_OK) {
		result = 0;
	    }
	}
    } 
    VEMFREE(aterms);
    return result;
}


static vemStatus ckLines(arg_list)
lsList arg_list;
/*
 * Returns VEM_OK if all arguments on `arg_list' are line arguments.
 * Returns VEM_FALSE if not.
 */
{
    vemOneArg *arg;
    lsGen gen;
    int nope = 0;

    gen = lsStart(arg_list);
    while (lsNext(gen, (Pointer *) &arg, LS_NH) == LS_OK) {
	if (arg->argType != VEM_LINE_ARG) {
	    nope = 1;  break;
	}
    }
    lsFinish(gen);
    if (nope) return VEM_FALSE;
    else return VEM_OK;
}
