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
 * Creating new segments in symbolic
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains code for creating new segments in a symbolic
 * cell.  Connectivity information is updated appropriately.
 *
 * Work to be done:
 *   - Endpoint net merge is not complete yet
 *   - Jump terminal maintence is not considered
 *   - dir_val() isn't written
 *   - No minimum width option
 */

#include "internal.h"
#include "utility.h"

#include "terminfo.h"
#include "net.h"
#include "jump.h"
#include "joined.h"
#include "inst.h"
#include "segments.h"
#include "repl.h"

typedef enum cs_what_defn {
    SIMPLE_RECONNECT, UPGRADE_CONNECTOR, ADD_CONNECTOR
} cs_what;

typedef struct up_conn_info_defn {
    octObject new_conn;		/* New connector  */
    octObject old_conn;		/* Old connector  */
} up_conn_info;

typedef struct add_conn_info_defn {
    octObject new_conn;		/* Connector to add  */
    octObject *i_term;		/* Instance terminal */
    octObject *old_lyr;		/* Old layer         */
    int32 np;			/* Number of points  */
    struct octPoint *pnts;	/* Points themselves */
    octObject seg;		/* New segment       */
    octObject *old_seg;		/* Old segment       */
} add_conn_info;


typedef struct cs_info_defn {
    cs_what do_what;		/* What needs to be done */
    union cs_opts_defn {
	up_conn_info up;	/* UPGRADE_CONNECTOR     */
	add_conn_info add;	/* ADD_CONNECTOR         */
    } cs_opts;
} cs_info;

/*
 * Forward declarations
 */

static void resolve_layers
  ARGS((octObject *fct, int nl, octObject *lyrs));
static int ck_endp
  ARGS((octObject *term, struct octPoint *pnt, octCoord width, octObject *lyr));
static void place_connectors
  ARGS((octObject *fct, int np, struct octPoint pnts[],
	int nl, octObject lyrs[],
	int nw, octCoord wi[], symConFunc *confun,
	octObject cons[]));
static void intern_segments
  ARGS((octObject *fct, int np, struct octPoint pnts[],
	int nl, octObject lyrs[], int nw, octCoord wi[],
	octObject cons[],
	octObject *fn, octObject *ln));
static void end_segment
  ARGS((octObject *fct, octObject *term1, struct octPoint *pnt1,
	octObject *term2, struct octPoint *pnt2, octObject *lyr,
	octCoord width));
static void dir_val
  ARGS((struct octPoint *p1, struct octPoint *p2,
	int *direction, octCoord *angle));
static void get_net
  ARGS((octObject *term, octObject *net));
static void handle_joined
  ARGS((octObject *term, int num, octObject *net));
static int check_conn
  ARGS((octObject *fct, octObject *seg, struct octPoint *sp,
	octObject *term, octObject *new_lyr, octCoord new_width,
	symConFunc *confun, cs_info *what));
static int check_inst
  ARGS((octObject *fct, octObject *seg, octObject *term,
	int pid, struct octPoint *pnts, octObject *old_lyr,
	octObject *new_lyr, octCoord new_width,
	symConFunc *confun, cs_info *what));
static void conn_term
  ARGS((octObject *fct, cs_info *what));
static void inst_term
  ARGS((octObject *fct, cs_info *what));
static int layer_list
  ARGS((octObject *term, octObject *seg, struct octPoint *sp,
	octObject *new_lyr, octCoord new_width, tapLayerListElement **elems));
static int same_connector
  ARGS((octObject *one, octObject *two));
static int term_ok
  ARGS((octObject *term, struct octPoint *pnt, octObject *new_lyr,
	octCoord new_width));
static int seg_dir
  ARGS((struct octPoint *spot, struct octPoint *other));
static int opp_dir
  ARGS((int dir));
static void make_seg
  ARGS((tapLayerListElement *si, struct octPoint *pnt, cs_info *what));
static void do_seg
  ARGS((octObject *t1, octObject *t2, octObject *net, cs_info *what));
  



void symSegments(facet, first, last, np, points, nl, layers, nw, widths, confun)
octObject *facet;		/* Facet to create new segments */
octObject *first, *last;	/* Start and end terminals      */
int np;				/* Number of segment points     */
struct octPoint *points;	/* Array of points              */
int nl;				/* Number of layers             */
octObject layers[];		/* Array of layer objects       */
int nw;				/* Number of widths             */
octCoord widths[];		/* Segment widths               */
symConFunc *confun;		/* Connector creation function  */
/*
 * This routine creates new symbolic segments in `facet' that connect
 * the terminal `first' with the terminal `last'.  All segments
 * are implemented as two-point paths in Oct.  The points for these
 * paths are derived from the array `points' (which must have `np' points,
 * where np > 2).  The first and last points must intersect the
 * implementations of `first' and `last' respectively (this is checked).
 * The layers for the segments are 
 * determined by the array `layers' (which must have `nl' layers, where
 * `nl' > 0).  If there are fewer layers than segments, the routine will 
 * iterate through the layers to create the segments.  Thus, if ten 
 * segments and two layers are provided, the segment layers will alternate 
 * between the two layers.  The widths for the layers are determined by 
 * the array `widths'.  If there are fewer widths than segments,
 * the widths are filled using the same algorithm as that used for layers.
 * `confun' is a function that should return an instance of a connector 
 * given information about joining segments:
 *   int confun(int nLayers, tapLayerListElement *layerList,
 *              octObject *instance, char why[SYM_MAX_MSG], char *data)
 * The structure `tapLayerList' is described in the documentation for
 * the Technology Access Package (TAP).  If the routine succeeds,  it
 * should fill in the fields of `instance' and return a non-zero status.
 * If the routine fails, it should return zero.  Upon failure, the 
 * routine should either fill `why' with a descriptive error message 
 * or provide its own diagnostics.  If `confun' is zero, the routine 
 * will use symDefConFunc as the default value.
 *
 * Implementation notes:
 *   1. Internal points are handled by automatically generating connectors
 *      and connecting the adjoining paths to it.  No outside connections
 *      are made at internal points.
 *   2. Net maintenence is done carefully at the end-points to preserve
 *      nets other than the one(s) automatically created by segment
 *      creation.
 */
{
    octObject *connectors, firstNet, lastNet, cterm;

    if (!confun) confun = symDefConFunc;
    resolve_layers(facet, nl, layers);

    /* Check endpoints */
    if (ck_endp(first, &(points[0]), widths[0], &(layers[0])) &&
	ck_endp(last, &(points[np-1]), widths[(np-2)%nw], &(layers[(np-2)%nl]))) {
	/* Generate internal connectors */
	if (np > 2) {
	    connectors = ALLOC(octObject, np-2);
	    place_connectors(facet, np, points, nl, layers, nw, widths, confun,
			     connectors);
	    
	    /* Generate internal segments and connectivity */
	    intern_segments(facet, np, points, nl, layers, nw, widths,
			    connectors, &firstNet, &lastNet);
	
	    /* Now ending segments */
	    if (!octIdIsNull(connectors[0].objectId)) {
		SYMCK(octGenFirstContent(&connectors[0], OCT_TERM_MASK, &cterm));
		end_segment(facet, first, &(points[0]), &cterm, &(points[1]),
			    &layers[0], widths[0]);
	    }
	    if (!octIdIsNull(connectors[np-3].objectId)) {
		SYMCK(octGenFirstContent(&connectors[np-3], OCT_TERM_MASK,
					 &cterm));
		end_segment(facet, last, &(points[np-1]), &cterm, &(points[np-2]),
			    &layers[(np-2)%nl], widths[(np-2)%nw]);
	    }
	    FREE(connectors);
	} else {
	    /* Only one segment */
	    end_segment(facet, first, &(points[0]), last, &(points[1]),
			&layers[0], widths[0]);
	}
    }
	    
}


static void resolve_layers(fct, nl, lyrs)
octObject *fct;			/* Facet for layers */
int nl;				/* Number of layers */
octObject *lyrs;		/* Array of layers  */
/*
 * This routine goes through `lyrs' and creates any of the layers
 * it finds that are not already in `fct'.
 */
{
    int i;

    for (i = 0;  i < nl;  i++) {
	if (((octGetById(lyrs+i) != OCT_OK) ||
	     (octIsAttached(fct, lyrs+i) != OCT_OK)) &&
	    (octGetByName(fct, lyrs+i) != OCT_OK)) {
	    SYMCK(octCreate(fct, lyrs+i));
	}
    }
}



static void place_connectors(fct, np, pnts, nl, lyrs, nw, wi, confun, cons)
octObject *fct;			/* Facet for creating connectors */
int np;				/* Number of segment points      */
struct octPoint pnts[];		/* Segment point array           */
int nl;				/* Number of segment layers      */
octObject lyrs[];		/* Segment layer array           */
int nw;				/* Number of segment widths      */
octCoord wi[];			/* Segment width array           */
symConFunc *confun;		/* Connector creation function   */
octObject *cons;		/* Returned array of connectors  */
/*
 * This routine creates all internal connectors of a symbolic path.
 * Information about the incoming layers and directions are computed
 * and passed to `confun' for each connector.  The connectors are
 * returned in `cons'.  If a particular connector cannot be generated
 * using `confun', the object id in `cons' will be null.  If `confun'
 * is null, tapGetConnector will be called by default.
 */
{
    int i;
    tapLayerListElement elems[2];

    for (i = 1;  i < np-1;  i++) {
	elems[0].layer = lyrs[(i-1)%nl];
	elems[0].width = wi[(i-1)%nw];
	dir_val(pnts+i-1, pnts+i, &elems[0].direction, &elems[0].angle);
	elems[1].layer = lyrs[i%nl];
	elems[1].width = wi[i%nw];
	dir_val(pnts+i, pnts+i+1, &elems[1].direction, &elems[1].angle);
	cons[i-1].contents.instance.name = OCT_CONTACT_INSTNAME;
	cons[i-1].contents.instance.transform.translation.x = pnts[i].x;
	cons[i-1].contents.instance.transform.translation.y = pnts[i].y;
	symConnector(fct, pnts[i].x, pnts[i].y, 2, elems, confun, cons+i-1);
    }
}



static void intern_segments(fct, np, pnts, nl, lyrs, nw, wi, cons, fn, ln)
octObject *fct;			/* Facet for creating internal segments */
int np;				/* Number of segment points             */
struct octPoint pnts[];		/* Segment point array                  */
int nl;				/* Number of layers                     */
octObject lyrs[];		/* Layer array                          */
int nw;				/* Number of segment widths             */
octCoord wi[];			/* Segment width array                  */
octObject *cons;		/* Connector array (np-2 in length)     */
octObject *fn, *ln;		/* First and last nets created          */
/*
 * This routine creates the internal segments of a new symbolic path.
 * The segments are connected by the connectors in `cons'.  If there
 * is no connector at a given location, no segments are created on
 * either side of it.  New nets are created and attached to the segments
 * appropriately.
 */
{
    int first_not_set_p = 1;
    int new_last_p = 1;
    int i;
    struct octPoint path_points[2];
    octObject new_path, term;

    new_path.type = OCT_PATH;
    fn->objectId = ln->objectId = oct_null_id;
    ln->type = OCT_NET;
    ln->contents.net.name = (char *) 0;
    for (i = 0;  i < np-3;  i++) {
	if (!octIdIsNull(cons[i].objectId)
	 && !octIdIsNull(cons[i+1].objectId)) {
	    /* Create new path */
	    path_points[0] = pnts[i+1];
	    path_points[1] = pnts[i+2];
	    new_path.contents.path.width = wi[(i+1)%nw];
	    SYMCK(octCreate(&lyrs[(i+1)%nl], &new_path));
	    SYMCK(octPutPoints(&new_path, 2, path_points));
	    /* Figure out what to do about nets */
	    if (new_last_p) {
		SYMCK(octCreate(fct, ln));
		if (first_not_set_p) {
		    *fn = *ln;
		    first_not_set_p = 0;
		}
		new_last_p = 0;
	    }
	    /* Now connectivity */
	    SYMCK(octAttach(ln, &new_path));
	    SYMCK(octGenFirstContent(cons+i, OCT_TERM_MASK, &term));
	    SYMCK(octAttach(&new_path, &term));
	    SYMCK(octGenFirstContent(cons+i+1, OCT_TERM_MASK, &term));
	    SYMCK(octAttach(&new_path, &term));
	} else {
	    new_last_p = 1;
	}
    }
}


static int ck_endp(term, pnt, width, lyr)
octObject *term;		/* Connection terminal */
struct octPoint *pnt;		/* Connection point    */
octCoord width;			/* Connection width    */
octObject *lyr;			/* Connection layer    */
/*
 * This routine verifies that `pnt' lies inside the implementation
 * of `term' on layer `lyr' assuming a connection width of `width'.
 * If it does, it returns a non-zero status.  If it doesn't, it returns
 * zero.  Diagnostics are produced by this routine.
 */
{
    symTermGen gen;
    octObject impl_lyr, geo;
    int result = 0;

    if (!octIdIsNull(term->contents.term.instanceId)) {
	symInitTerm(term, &gen);
	while (symNextTerm(gen, &impl_lyr, &geo)) {
	    if (impl_lyr.contents.layer.name &&
		lyr->contents.layer.name &&
		(STRCMP(impl_lyr.contents.layer.name,
			lyr->contents.layer.name) == 0)) {
		result = 1;
		if ((geo.type == OCT_BOX) &&
		    (((geo.contents.box.upperRight.x - geo.contents.box.lowerLeft.x) >= width) ||
		     ((geo.contents.box.upperRight.y - geo.contents.box.lowerLeft.y) >= width))) {
		    symEndTerm(gen);
		    return 1;
		}
	    }
	}
	if (result) {
	    symMsg(SYM_WARN, "segment too wide for endpoint terminal");
	} else {
	    symMsg(SYM_ERR, "segment layer does not match endpoint terminal");
	}
	symEndTerm(gen);
    } else {
	symMsg(SYM_ERR, "segment endpoint terminal is not an actual terminal");
    }
    return result;
}



static void end_segment(fct, term1, pnt1, term2, pnt2, lyr, width)
octObject *fct;			/* Facet to use */
octObject *term1;		/* First terminal */
struct octPoint *pnt1;		/* First point */
octObject *term2;		/* Second terminal */
struct octPoint *pnt2;		/* Second point */
octObject *lyr;			/* Layer to use */
octCoord width;			/* Width of segment */
/*
 * This routine creates an endpoint segment between `term1' and
 * `term2' (located at `pnt1' and `pnt2' respectively) on layer
 * `lyr' with width `width'.  Unlike internal segment creation,
 * all necessary net merging is done for these segments.
 *
 * Notes:
 *   - Routine uses symLocNet to find the nets of the two terminals.
 *   - It will create nets if either terminal has no associated
 *     nets (and handles jump terminal net creation correctly).
 *   - Attaches the terminals to the net if they are not connector
 *     terminals.
 */
{
    octObject net1, net2, path, *final;
    struct octPoint pnts[2];

    /* Segment itself */
    path.type = OCT_PATH;
    path.contents.path.width = width;
    SYMCK(octCreate(lyr, &path));
    pnts[0] = *pnt1; pnts[1] = *pnt2;
    SYMCK(octPutPoints(&path, 2, pnts));
    
    /* Connectivity */
    get_net(term1, &net1);  get_net(term2, &net2);
    SYMCK(octAttach(&path, term1));
    SYMCK(octAttach(&path, term2));
    if (!octIdsEqual(net1.objectId, net2.objectId)) {
	final = symMergeNets(&net1, &net2);
    } else {
	final = &net1;
    }
    SYMCK(octAttach(final, &path));
}


static void get_net(term, net)
octObject *term;		/* Terminal       */
octObject *net;			/* Net (returned) */
/*
 * Finds or creates a net associated with `term'.  Notes:
 *   - It first tries to find an associated net using symLocNet().
 *     If that works, it returns that net.
 *   - If there is no existing net, one is created.  If the
 *     terminal is a jump terminal, a jump net is created.
 *   - If the terminal is a real instance terminal, it is
 *     attached to the net.
 *   - If the terminal is joined with others, all the
 *     other nets attached to the joined terminals are
 *     merged into one and that net is returned.
 */
{
    int num;

    if (!symLocNet(term, net)) {
	if (symJumpTermP(term)) {
	    symCreateJumpNet(term, net);
	} else {
	    symCreateNamedNet(term, net);
	}
	if (symInstanceP(term)) {
	    SYMCK(octAttachOnce(net, term));
	    if ((num = symNumJoined(term)) > 0) {
		/* Terminal is joined to others */
		handle_joined(term, num, net);
	    }
	}
    }
}


static void handle_joined(term, num, net)
octObject *term;		/* Actual terminal            */
int num;			/* Number of joined terminals */
octObject *net;			/* Net (returned)             */
/*
 * This routine examines all of the terminals that are electrically 
 * equivalent to `term' and attempts to find nets connected to
 * them.  These nets are merged into `net'.  If the terminals
 * are instance terminals, they are attached to the net.
 */
{
    octObject *joined, other_net, *merged;
    int last, i;

    joined = ALLOC(octObject, num);
    if ( (last = symListJoined(term, num, joined)) ) {
	for (i = 0;  i < last;  i++) {
	    if (symLocNet(&(joined[i]), &other_net)) {
		merged = symMergeNets(&other_net, net);
		*net = *merged;
	    }
	    if (symInstanceP(&(joined[i]))) {
		SYMCK(octAttachOnce(net, &(joined[i])));
	    }
	}
    }
    FREE(joined);
}



void symSegChange(fct, seg, new_lyr, new_width, confun)
octObject *fct;			/* Facet to work on      */
octObject *seg;			/* Segment to change     */
octObject *new_lyr;		/* New layer             */
octCoord new_width;		/* New segment width     */
symConFunc *confun;		/* Connector function    */
/*
 * This routine changes the layer of a segment to `new_lyr'
 * and its width to `new_width'.  The routine examines the
 * connectors at the endpoints of the segment and changes
 * them as appropriate.  The connector function will default
 * to the standard connector function if `confun' is zero.
 */
{
    octObject old_lyr;
    segTermInfo info[2];
    struct octPoint pnts[2];
    cs_info facts[2];
    int32 np;
    int i;

    if (!confun) confun = symDefConFunc;

    np = 2;
    SYMCK(octGetPoints(seg, &np, pnts));
    SYMCK(octGenFirstContainer(seg, OCT_LAYER_MASK, &old_lyr));
    symSegTerms(seg, pnts, info);

    /* First pass checks to make sure we can do this */
    for (i = 0;  i < 2;  i++) {
	if (!octIdIsNull(info[i].term.objectId)) {
	    if (info[i].flags & SEG_AT_CONNECTOR) {
		if (!check_conn(fct, seg, &(pnts[i]), &(info[i].term),
				new_lyr, new_width, confun, &(facts[i]))) {
		    /* Failed */
		    return;
		}
	    } else {
		if (!check_inst(fct, seg, &(info[i].term), i, pnts,
				&old_lyr, new_lyr, new_width, confun,
				&(facts[i]))) {
		    /* Failed */
		    return;
		}
	    }
	} else {
	    /* Bad segment error */
	    symMsg(SYM_ERR, "Bad segment passed to symSegChange");
	    return;
	}
    }

    /* If we made it here, we can make all necessary changes */
    if (!octIdsEqual(new_lyr->objectId, old_lyr.objectId)) {
	SYMCK(octAttach(new_lyr, seg));
	SYMCK(octDetach(&old_lyr, seg));
    }
    if (new_width != seg->contents.path.width) {
	seg->contents.path.width = new_width;
	SYMCK(octModify(seg));
    }
    for (i = 0;  i < 2;  i++) {
	switch (facts[i].do_what) {
	case UPGRADE_CONNECTOR:
	    conn_term(fct, facts+i);
	    break;
	case ADD_CONNECTOR:
	    inst_term(fct, facts+i);
	    break;
        case  SIMPLE_RECONNECT:
	    /* For completeness*/
	    break;
	}
    }
}



static int check_conn(fct, seg, sp, term, new_lyr, new_width, confun, what)
octObject *fct;			/* Facet for changed connector */
octObject *seg;			/* Segment itself            */
struct octPoint *sp;		/* Segment point             */
octObject *term;		/* Segment endpoint terminal */
octObject *new_lyr;		/* New layer                 */
octCoord new_width;		/* New width of segment      */
symConFunc *confun;		/* Connector function        */
cs_info *what;			/* What to do (returned)     */
/*
 * This routine checks the connector at the end of a segment
 * and sees whether it has to be updated to receive a segment
 * on `new_lyr' of width `new_width'.  If such an upgrade
 * is required, it is noted in `what' and the appropriate
 * connector is returned.  If it can't find an appropriate
 * connector, it returns zero and produces diagnostics.
 */
{
    tapLayerListElement *elems;
    char message[SYM_MAX_MSG];
    int ret, nlyrs;

    nlyrs = layer_list(term, seg, sp, new_lyr, new_width, &elems);
    if ((*confun->func)(nlyrs, elems, &(what->cs_opts.up.new_conn),
			message, confun->data)) {
	what->cs_opts.up.old_conn.objectId = term->contents.term.instanceId;
	SYMCK(octGetById(&(what->cs_opts.up.old_conn)));
	if (same_connector(&what->cs_opts.up.old_conn,
			   &what->cs_opts.up.new_conn)) {
	    what->do_what = SIMPLE_RECONNECT;
	} else {
	    what->do_what = UPGRADE_CONNECTOR;
	    what->cs_opts.up.new_conn.contents.instance.name =
	      what->cs_opts.up.old_conn.contents.instance.name;
	    what->cs_opts.up.new_conn.contents.instance.transform.translation =
	      what->cs_opts.up.old_conn.contents.instance.transform.translation;
	}
	ret = 1;
    } else {
	symMsg(SYM_ERR, "No suitable connector found:\n  %s", message);
	ret = 0;
    }
    if (nlyrs > 0) FREE(elems);
    return ret;
}


static int same_connector(one, two)
octObject *one, *two;		/* Two connector instances */
/* 
 * Compares the two supplied instances to see if they share the
 * same master.  If they do, it returns one.  If they don't it
 * returns zero.  The routine does not require either instance
 * to be instantiated.  That means it is possible for the
 * routine to say the instances are different when they are
 * actually the same.  There is some nasty knowledge embedded
 * in this code but it fails safe.
 */
{
    if (one->contents.instance.master &&
	((one->contents.instance.master[0] == '/') ||
	 (one->contents.instance.master[0] == '~')) &&
	two->contents.instance.master &&
	((two->contents.instance.master[0] == '/') ||
	 (two->contents.instance.master[0] == '~'))) {
	/* Comparable */
	if ((STRCMP(one->contents.instance.master,
		    two->contents.instance.master) == 0) &&
	    (STRCMP(one->contents.instance.view,
		    two->contents.instance.view) == 0) &&
	    (STRCMP(one->contents.instance.facet,
		    two->contents.instance.facet) == 0)) {
	    return 1;
	}
    }
    return 0;
}



static int check_inst(fct, seg, term, pid, pnts, old_lyr, new_lyr,
		      new_width, confun, what)
octObject *fct;			/* Facet for work    */
octObject *seg;			/* Segment itself    */
octObject *term;		/* Instance terminal */
int pid;			/* Which point?      */
struct octPoint *pnts;		/* Segment endpoints */
octObject *old_lyr;		/* Old segment layer */
octObject *new_lyr;		/* New segment layer */
octCoord new_width;		/* New segment width */
symConFunc *confun;		/* Connector function */
cs_info *what;			/* What to do (returned) */
/*
 * This routine determines what to do when you change the layer and/or
 * width of a segment connected to an instance actual terminal.  If 
 * the actual terminal can connect to the segment,  all is well and 
 * no work is required.  If it can't,  a connector and a segment 
 * connecting the terminal to the connector must be generated.  This
 * routine produces the necessary information to carry out this
 * task.  If it determines this isn't possible, it returns zero.
 */
{
    tapLayerListElement elems[2];
    char message[SYM_MAX_MSG];

    if (!term_ok(term, &(pnts[pid]), new_lyr, new_width)) {
	/* Tap entry for existing segment */
	elems[0].layer = *new_lyr;
	elems[0].width = new_width;
	elems[0].direction = seg_dir(&(pnts[pid]), &(pnts[1-pid]));
	/* Tap entry for new segment */
	elems[1].layer = *old_lyr;
	elems[1].width = elems[0].width;
	elems[1].direction = opp_dir(elems[0].direction);
	/* Ready to see if we can do it */
	if ((*confun->func)(2, elems, &(what->cs_opts.add.new_conn),
			    message, confun->data)) {
	    what->do_what = ADD_CONNECTOR;
	    what->cs_opts.add.new_conn.contents.instance.name = "*";
	    what->cs_opts.add.new_conn.contents.instance.transform.translation =
	      pnts[pid];
	    what->cs_opts.add.old_lyr = old_lyr;
	    what->cs_opts.add.i_term = term;
	    what->cs_opts.add.np = 2;
	    make_seg(&(elems[1]), &(pnts[pid]), what);
	    what->cs_opts.add.old_seg = seg;
	} else {
	    symMsg(SYM_INFO, "Cannot get appropriate connector:\n  %s", message);
	    return 0;
	}
    } else {
	what->do_what = SIMPLE_RECONNECT;
    }
    return 1;
}


static void make_seg(si, pnt, what)
tapLayerListElement *si;	/* Segment information  */
struct octPoint *pnt;		/* Start point          */
cs_info *what;			/* What to do           */
/*
 * This routine creates a new segment which is used to connect
 * between an instance actual terminal and a new connector used
 * to connect to a segment on a different layer.  Most of the
 * information for making the new segment is given in `si'.
 */
{
    what->cs_opts.add.pnts = ALLOC(struct octPoint, 2);
    what->cs_opts.add.seg.type = OCT_PATH;
    what->cs_opts.add.seg.contents.path.width = si->width;
    what->cs_opts.add.pnts[0] = *pnt;
    what->cs_opts.add.pnts[1] = *pnt;
    if (si->direction & TAP_LEFT) what->cs_opts.add.pnts[1].x -= 1;
    else if (si->direction & TAP_RIGHT) what->cs_opts.add.pnts[1].x += 1;
    else if (si->direction & TAP_TOP) what->cs_opts.add.pnts[1].y += 1;
    else if (si->direction & TAP_BOTTOM) what->cs_opts.add.pnts[1].y -= 1;
}



static void conn_term(fct, what)
octObject *fct;			/* Facet for changed connector */
cs_info *what;			/* What to do                */
/*
 * This routine handles the swapping of a connector for another
 * connector which is equivalent to the first except it can
 * now appropriately connect to a segment on a different
 * layer or with a different width.  The routine assumes
 * `what' has been filled in properly.
 */
{
    symReplaceInst(fct, &(what->cs_opts.up.old_conn), &(what->cs_opts.up.new_conn));
}


static int layer_list(term, seg, sp, new_lyr, new_width, elems)
octObject *term;		/* Connector terminal    */
octObject *seg;			/* Segment itself        */
struct octPoint *sp;		/* Key segment point     */
octObject *new_lyr;		/* New segment layer     */
octCoord new_width;		/* New segment width     */
tapLayerListElement **elems;	/* Returned element list */
/*
 * This routine examines all segments connected to `term' and
 * fills in their characteristics in `elems'.  All characteristics
 * are computed from the segments directly except for the segment
 * `seg'.  Its layer and width are replaced by `new_lyr' and
 * `new_width' respectively.
 */
{
    octGenerator gen;
    octObject obj;
    struct octPoint pnts[2];
    int32 np = 2;
    int count;
    octCoord dist;

    count = 0;
    SYMCK(octInitGenContainers(term, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) count++;
    *elems = ALLOC(tapLayerListElement, count);

    /* Ready to extract info */
    count = 0;
    SYMCK(octInitGenContainers(term, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	if (octIdsEqual(obj.objectId, seg->objectId)) {
	    (*elems)[count].layer = *new_lyr;
	} else {
	    SYMCK(octGenFirstContainer(&obj, OCT_LAYER_MASK,
				       &((*elems)[count].layer)));
	}
	if (octIdsEqual(obj.objectId, seg->objectId)) {
	    (*elems)[count].width = new_width;
	} else {
	    (*elems)[count].width = obj.contents.path.width;
	}
	SYMCK(octGetPoints(&obj, &np, pnts));
	dist = DISTP(sp, &(pnts[0]));
	if (DISTP(sp, &(pnts[1])) < dist) {
	    (*elems)[count].direction = seg_dir(&(pnts[1]), &(pnts[0]));
	} else {
	    (*elems)[count].direction = seg_dir(&(pnts[0]), &(pnts[1]));
	}
	count++;
    }
    return count;
}



static void inst_term(fct, what)
octObject *fct;			/* Facet for work    */
cs_info *what;			/* What to do        */
/*
 * This routine determines what to do when you change the layer and/or
 * width of a segment connected to an instance actual terminal.  If 
 * the actual terminal can connect to the segment,  all is well and 
 * no work is required.  If it can't,  a connector and a segment 
 * connecting the terminal to the connector must be generated.  The 
 * new segment is then moved from the instance actual terminal to 
 * the connector actual terminal.  The new layer is `new_lyr' and
 * the new width is `new_width'.
 */
{
    octObject cterm, seg_net;

    SYMCK(octCreate(fct, &what->cs_opts.add.new_conn));
    SYMCK(octGenFirstContent(&what->cs_opts.add.new_conn,
			     OCT_TERM_MASK, &cterm));
    if (!symLocNet(what->cs_opts.add.i_term, &seg_net)) {
	/* Create a new net */
	symCreateNamedNet(what->cs_opts.add.i_term, &seg_net);
	symMsg(SYM_INFO, "While upgrading segment: created new net for connector");
    }
    do_seg(what->cs_opts.add.i_term, &cterm, &seg_net, what);
    /* Detach segment from old term and attach to new one */
    SYMCK(octAttach(what->cs_opts.add.old_seg, &cterm));
    SYMCK(octDetach(what->cs_opts.add.old_seg, what->cs_opts.add.i_term));
}



static int term_ok(term, pnt, new_lyr, new_width)
octObject *term;		/* Instance term to check */
struct octPoint *pnt;		/* Segment endpoint       */
octObject *new_lyr;		/* New segment layer      */
octCoord new_width;		/* New segment width      */
/*
 * This routine returns a non-zero status if the implementation
 * of `term' contains a box on `new_lyr' that includes `pnt'
 * and the box is at least as large as `new_width' so that
 * it can accept the newly modified segment.
 */
{
    symTermGen gen;
    octObject lyr, geo;

    /* Initialize generation of terminal implementations */
    symInitTerm(term, &gen);
    while (symNextTerm(gen, &lyr, &geo) == OCT_OK) {
	if ((STRCMP(lyr.contents.layer.name,
		    new_lyr->contents.layer.name) == 0) &&
	    (geo.type == OCT_BOX) &&
	    (POINT_IN_BOX(pnt, &(geo.contents.box))) &&
	    (((geo.contents.box.upperRight.x -
	       geo.contents.box.lowerLeft.x) >= new_width) ||
	     ((geo.contents.box.upperRight.y -
	       geo.contents.box.lowerLeft.y) >= new_width))) {
	    /* It's ok */
	    symEndTerm(gen);
	    return 1;
	}
    }
    symEndTerm(gen);
    return 0;
}



static int seg_dir(spot, other)
struct octPoint *spot;		/* Intersection spot */
struct octPoint *other;	/* Other point       */
/*
 * Returns a tap direction for a segment which starts at
 * `spot' and goes to `other'.  Returns a manhattan direction
 * even if it isn't manhattan.
 */
{
    octCoord dx, dy;

    dx = spot->x - other->x;
    dy = spot->y - other->y;
    if (ABS(dx) > ABS(dy)) {
	/* More horizontal than vertical */
	if (dx < 0) return TAP_RIGHT;
	else return TAP_LEFT;
    } else {
	if (dy < 0) return TAP_TOP;
	else return TAP_BOTTOM;
    }
}

static int opp_dir(dir)
int dir;
/*
 * Returns the opposite direction of the given direction.
 */
{
    int result = 0;

    if (dir & TAP_RIGHT) result |= TAP_LEFT;
    else if (dir & TAP_LEFT) result |= TAP_RIGHT;
    else if (dir & TAP_TOP) result |= TAP_BOTTOM;
    else if (dir & TAP_BOTTOM) result |= TAP_TOP;
    return result;
}

static void dir_val(p1, p2, dir, angle)
struct octPoint *p1, *p2;	/* Segment points       */
int *dir;			/* Direction (returned) */
octCoord *angle;		/* Angle (returned)     */
/*
 * Computes the direction and angle of the line between `p1'
 * and `p2'.  Only manhattan at the moment.
 */
{
    *dir = seg_dir(p1, p2);
    *angle = 0;
}



static void do_seg(t1, t2, net, what)
octObject *t1, *t2;		/* Connection terminals */
octObject *net;			/* Segment net          */
cs_info *what;			/* What to do           */
/*
 * This routine creates a new segment which is used to connect
 * between an instance actual terminal and a new connector used
 * to connect to a segment on a different layer.  Most of the
 * information for making the new segment is given in `si'.
 */
{
    SYMCK(octCreate(what->cs_opts.add.old_lyr, &what->cs_opts.add.seg));
    SYMCK(octPutPoints(&what->cs_opts.add.seg,
		       what->cs_opts.add.np, what->cs_opts.add.pnts));
    /* Now connect the segment */
    SYMCK(octAttach(&what->cs_opts.add.seg, t1));
    SYMCK(octAttach(&what->cs_opts.add.seg, t2));
    SYMCK(octAttach(net, &what->cs_opts.add.seg));
    FREE(what->cs_opts.add.pnts);
}



void symSegDesc(seg, ref, elem)
octObject *seg;			/* Existing symbolic segment */
struct octPoint *ref;		/* Optional reference point  */
tapLayerListElement *elem;	/* Returned description      */
/*
 * This routine returns a description for `seg' suitable for
 * use with connector functions.  If `ref' is non-zero, it
 * will be used as the reference point for comparison (allowing
 * the routine to return more specific directions).  At the moment, 
 * it returns manhattan angle information only.
 *
 * The reference point is unused at the moment.
 */
{
    struct octPoint pnts[2];
    int32 np = 2;
    octCoord diff_x, diff_y;

    SYMCK(octGenFirstContainer(seg, OCT_LAYER_MASK, &(elem->layer)));
    elem->width = seg->contents.path.width;
    SYMCK(octGetPoints(seg, &np, pnts));
    if ((diff_x = pnts[0].x - pnts[1].x) < 0) diff_x = -diff_x;
    if ((diff_y = pnts[0].y - pnts[1].y) < 0) diff_y = -diff_y;
    if (diff_x > diff_y) {
	/* Horizontal */
	elem->direction = TAP_HORIZONTAL;
	elem->angle = 0;
    } else {
	/* Vertical */
	elem->direction = TAP_VERTICAL;
	elem->angle = 0;
    }
}


int symSegConnP(term, elem)
octObject *term;		/* Actual terminal                */
tapLayerListElement *elem;	/* Segment connection description */
/*
 * Returns a non-zero value if `term' has an implementation of the
 * right size on the right layer to accept a connection to the
 * segment described by `elem'.  Does not consider whether the
 * terminal could be upgraded.  Use symUpgrade() if that might
 * be the case.
 */
{
    symTermGen gen;
    octObject t_lyr, t_geo;
    int result = 0;

    symInitTerm(term, &gen);
    while (symNextTerm(gen, &t_lyr, &t_geo)) {
	if ((STRCMP(elem->layer.contents.layer.name,
		    t_lyr.contents.layer.name) == 0) &&
	    (t_geo.type == OCT_BOX) &&
	    (((elem->direction & TAP_HORIZONTAL) &&
	      (elem->width <= (t_geo.contents.box.upperRight.y -
			       t_geo.contents.box.lowerLeft.y))) ||
	     ((elem->direction & TAP_VERTICAL) &&
	      (elem->width <= (t_geo.contents.box.upperRight.x -
			       t_geo.contents.box.lowerLeft.x))))) {
	    /* Its a valid possibility */
	    result = 1;
	    break;
	}
    }
    symEndTerm(gen);
    return result;
}



int symSegBreakP(seg, elem, isect, confun)
octObject *seg;			/* Segment                        */
tapLayerListElement *elem;	/* Segment connection description */
struct octPoint *isect;		/* Optional intersection point    */
symConFunc *confun;		/* Connector function             */
/*
 * This routine returns a non-zero value if the segment described
 * by `elem' can be connected to the existing segment `seg'.  This
 * is true if a connector exists that can connect the two segments.
 * Note this assumes the connection will not be at an endpoint of
 * `seg' but somewhere in the middle.  If the connection is at
 * the endpoint, use symSegConnP with the endpoint terminal.  If
 * the intersection point between the segments is known, it
 * should be provided in `isect'.  If not provided, the routine
 * will use more general specifications for the connector.
 */
{
    tapLayerListElement full_desc[2];
    octObject new_conn;
    char message[SYM_MAX_MSG];

    symSegDesc(seg, isect, &(full_desc[0]));
    full_desc[1] = *elem;
    return (*confun->func)(2, full_desc, &new_conn, message, confun->data);
}
