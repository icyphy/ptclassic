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
/*LINTLIBRARY*/
/*
 * Symbolic Instances
 *
 * David Harrison 
 * University of California, Berkeley
 * 1990
 *
 * This file contains routines for creating and manipulating symbolic
 * instances.  It includes routines for creating new instances,
 * reconnecting existing instances, and replacing an instance
 * with a new master.
 */

#include "internal.h"

#include "utility.h"
#include "contact.h"
#include "jump.h"
#include "terminfo.h"
#include "da.h"
#include "st.h"
#include "region.h"
#include "net.h"

#include "inst.h"

#define FREEDOM_THRESHOLD	3

#define SYM_DEAD_RECONNECT	";;; Deleted Objects ;;;"

#define INIT_LYRS	5

#define NONE	0
#define HORI	1
#define VERT	2

#define LEFT_CODE	0x01
#define RIGHT_CODE	0x02
#define BOTTOM_CODE	0x04
#define TOP_CODE	0x08

#define DO_CODE(pnt, rtn) \
    rtn = 0;   	       	       	       	       	 	      \
    if ((pnt)->x < box->lowerLeft.x) rtn = LEFT_CODE;	      \
    else if ((pnt)->x > box->upperRight.x) rtn = RIGHT_CODE;  \
    if ((pnt)->y < box->lowerLeft.y) rtn |= BOTTOM_CODE;      \
    else if ((pnt)->y > box->upperRight.y) rtn |= TOP_CODE

/*
 * Forward declarations
 */

static void do_inst
  ARGS((octObject *inst));
static void check_term
  ARGS((octObject *fct, octObject *term, symConFunc *confun, octObject *dead));
static void check_inst
  ARGS((octObject *fct, octObject *at, struct octBox *size,
	dary impls, octObject *inst, symConFunc *confun));
static void match_term
  ARGS((octObject *fct, octObject *t_one, octObject *t_two,
	dary common, symConFunc *confun));
static void check_seg
  ARGS((octObject *at, struct octBox *impl, octObject *lyr,
	octObject *seg, int np, struct octPoint *pnts,
	octObject *dead));
static void min_connect
  ARGS((octObject *fct, octObject *term_1, octObject *term_2,
	octObject *lyr, struct octBox *box_1, struct octBox *box_2,
	symConFunc *confun));
static void max_connect
  ARGS((octObject *fct, octObject *term_1, octObject *term_2,
	octObject *lyr, struct octBox *box_1, struct octBox *box_2,
	symConFunc *confun));
static void no_intersect
  ARGS((octObject *at, struct octBox *impl, octObject *lyr,
	octObject *seg, int np, struct octPoint *pnts,
	segTermInfo info[2]));
static int line_cross_box
  ARGS((struct octBox *box, struct octPoint *p1, struct octPoint *p2,
	struct octPoint *pmod1, struct octPoint *pmod2));



/*ARGSUSED*/
static int def_alt_func(t1, t2, num, lyrs, data)
octObject *t1, *t2;		/* Terminals              */
int num;			/* Number of alternatives */
octObject lyrs[];		/* Alternative layers     */
char *data;			/* User data              */
/*
 * This is the default alternative implementation function.  It
 * is called when two terminals are to be connected automatically
 * but there are several possible implementation layers for
 * the connection.  This implementation always returns the first
 * item.
 */
{
    return 0;
}

static symLyrAltFunc curAltFunc = { def_alt_func, (char *) 0 };

void symSetLyrAltFunc(new, old)
symLyrAltFunc *new;		/* New function */
symLyrAltFunc *old;		/* Old function */
/*
 * This routine sets the current layer alternative function
 * in the symbolic policy library.  This function is called
 * when two terminals are to be connected automatically but
 * there are several possible implementation layers for
 * the connection.  The function is expected to choose
 * the appropriate implementation layer:
 *   int func(t1, t2, num, lyrs, data)
 *   octObject *t1, *t2;
 *   int num;
 *   octObject lyrs[];
 *   char *data;
 * The two actual terminals are passed in `t1' and `t2'.  The
 * list of possible layers is passed in `num', and `lyrs'.
 * The routine should return a value between 0 and `num'
 * representing the choice.  A value outside that range
 * will cause an error. 
 */
{
    if (old) {
	*old = curAltFunc;
    }
    curAltFunc = *new;
}



int symInstanceP(obj)
octObject *obj;			/* Instance or terminal */
/*
 * Returns a non-zero status if `obj' is a real instance (or
 * a real instance terminal).  A quick check of the instances
 * bag is made.  If that is inconclusive,  it asks whether
 * the thing is a connector.  If not, it is an instance.
 */
{
    octGenerator gen;
    octObject inst, bag;
    int found = 0;

    switch (obj->type) {
    case OCT_TERM:
	/* Terminal */
	if (!octIdIsNull(inst.objectId = obj->contents.term.instanceId)) {
	    SYMCK(octGetById(&inst));
	} else {
	    symMsg(SYM_ERR, "Formal terminal passed to symInstanceP");
	    return 0;
	}
	break;
    case OCT_INSTANCE:
	/* Instance itself */
	inst = *obj;
	break;
    default:
	symMsg(SYM_ERR, "Bad object type to symInstanceP");
	return 0;
    }
    /* Bag check */
    SYMCK(octInitGenContainers(&inst, OCT_BAG_MASK, &gen));
    while (octGenerate(&gen, &bag) == OCT_OK) {
	if (bag.contents.bag.name && (strcmp(bag.contents.bag.name,
					     OCT_INSTANCE_NAME) == 0)) found++;
    }
    if (found > 1) {
	errRaise(sym_pkg_name, SYM_BADBAG,
		 "Too many connections to instances bag");
	/*NOTREACHED*/
    }
    if (found) return found;
    else return !symContactP(&inst);
}


static octObject *symInstanceBag(obj)
octObject *obj;
/*
 * Returns the instances bag for the facet containing `obj'.
 * Some caching is done to improve performance.  Returns zero
 * if there is no instances bag in the facet.
 */
{
    static octId old_id = oct_null_id;
    static octObject InstBag;
    octObject fct;

    if (obj->type != OCT_FACET) {
	octGetFacet(obj, &fct);
    } else {
	fct = *obj;
    }
    if (!octIdsEqual(fct.objectId, old_id) || (octGetById(&InstBag) != OCT_OK)) {
	InstBag.type = OCT_BAG;
	InstBag.objectId = oct_null_id;
	InstBag.contents.bag.name = OCT_INSTANCE_NAME;
	(void) octGetByName(&fct, &InstBag);
	old_id = fct.objectId;
    }
    if (!octIdIsNull(InstBag.objectId)) {
	return &InstBag;
    } else {
	return (octObject *) 0;
    }
}



static void symAddToInstanceBag(inst)
octObject *inst;
/*
 * Adds `inst' to the connectors bag of its parent facet.
 * If there is no connectors bag, one is created.
 */
{
    octObject *bag, newbag, fct;

    if (!(bag = symInstanceBag(inst))) {
	newbag.objectId = oct_null_id;
	newbag.type = OCT_BAG;
	newbag.contents.bag.name = OCT_INSTANCE_NAME;
	octGetFacet(inst, &fct);
	SYMCK(octCreate(&fct, &newbag));
	symAddToInstanceBag(inst);
    } else {
	SYMCK(octAttachOnce(bag, inst));
    }
}



void symInstance(fct, inst)
octObject *fct;			/* What facet to create the instance in */
octObject *inst;		/* Instance record itself               */
/*
 * This routine creates a new symbolic instance.  The instance may
 * be either a connector or a real instance.  Connectors will be
 * connected to the connectors bag.  Instances will be connected
 * to the instances bag.  Jump terminals will be connected to
 * the jump terminals bag.  No other net maintenence is done (see
 * symReconnect() for terminal net maintenence.
 */
{

    if (octCreate(fct, inst) == OCT_OK) {
	if (symContactP(inst)) {
	    /* Just add to connectors bag */
	    symAddToConnectorsBag(inst);
	} else {
	    do_inst(inst);
	}
    } else {
	/* Can't create instance error */
	symMsg(SYM_ERR, "Cannot create instance:  %s\n",
	       octErrorString());
    }
}



static void do_inst(inst)
octObject *inst;		/* Real instance */
/*
 * This routine performs the necessary maintenence on a real symbolic
 * instance.  This includes connecting the instance to the instances
 * bag and connecting jump terminals to the jump terminal bag.  Connections 
 * to the instance are made using symSegments() or symReconnect().
 */
{
    octGenerator gen;
    octObject term;

    symAddToInstanceBag(inst);
    SYMCK(octInitGenContents(inst, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &term) == OCT_OK) {
	if (symJumpTermDeepP(&term)) {
	    symAddToJTBag(&term);
	}
    }
}



void symReconnect(inst, confun, del_func)
octObject *inst;		/* Instance to connect */
symConFunc *confun;		/* Connector function  */
symDelItemFunc *del_func;	/* Delete function     */
/*
 * This routine examines the connections to the terminals of `inst'
 * for correctness.  If any inappropriate segments are connected
 * to any of the terminals, the connection will be removed and
 * the segment will be deleted (if it is not connected to anything
 * else).  If any appropriate segments are over the terminals
 * they will be connected to the instance.  If an appropriate
 * terminal of another instance intersects a terminal, a segment
 * between them will be created.  In general, this routine is
 * used to cause connectivity updates to an instance and insure
 * proper connectivity.  It must be used carefully since it
 * is very agressive at removing bad connections.
 */
{
    octGenerator gen;
    octObject fct, term, dead;

    dead.type = OCT_BAG;
    dead.contents.bag.name = SYM_DEAD_RECONNECT;
    octGetFacet(inst, &fct);
    SYMCK(octCreate(&fct, &dead));
    octMarkTemporary(&dead);
    SYMCK(octInitGenContents(inst, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &term) == OCT_OK) {
	check_term(&fct, &term, confun, &dead);
    }
    symDeleteObjects(&dead, del_func);
    SYMCK(octDelete(&dead));

    /*
     * XXX - Remove redundant connectors
     */

}


typedef struct one_impl_defn {
    octObject lyr;		/* Layer of implementation */
    octObject geo;		/* Implementation geo      */
} termImpl;

static void check_term(fct, term, confun, dead)
octObject *fct;			/* Facet to modify    */
octObject *term;		/* Actual terminal    */
symConFunc *confun;		/* Connector function */
octObject *dead;		/* Bag to connect deleted things to */
/*
 * This routine examines the segments and instances
 * that intersect the implementation region of the terminal
 * `term'.  Segments are checked with check_seg(), instances
 * with check_inst().
 */
{
    symTermGen gen;
    struct octBox overall;
    octObject obj, dest_lyr;
    regGenerator reg_gen;
    int np, i;
    struct octPoint *pnts;
    dary all_impl;
    termImpl impl, *p_impl;
    
    /* First gather information about terminal implementation */
    all_impl = daAlloc(termImpl, INIT_LYRS);
    overall.lowerLeft.x = overall.lowerLeft.y = OCT_MAX_COORD;
    overall.upperRight.x = overall.upperRight.y = OCT_MIN_COORD;
    symInitTerm(term, &gen);
    while (symNextTerm(gen, &impl.lyr, &impl.geo)) {
	if (impl.geo.type == OCT_BOX) {
	    LA(impl.geo.contents.box.lowerLeft.x, overall.lowerLeft.x);
	    LA(impl.geo.contents.box.lowerLeft.y, overall.lowerLeft.y);
	    GA(impl.geo.contents.box.upperRight.x, overall.upperRight.x);
	    GA(impl.geo.contents.box.upperRight.y, overall.upperRight.y);
	    *daSetLast(termImpl, all_impl) = impl;
	}
    }
    symEndTerm(gen);

    /* Then, intersecting instances (if any) */
    if ((overall.lowerLeft.x <= overall.upperRight.x) &&
	(overall.lowerLeft.y <= overall.upperRight.y)) {
	regInit(fct, &overall, OCT_INSTANCE_MASK, &reg_gen);
	while (regNext(&reg_gen, &obj, &np, &pnts,
		       (struct octBox *) 0) == REG_OK) {
	    if (!octIdsEqual(obj.objectId, term->contents.term.instanceId)) {
		check_inst(fct, term, &overall, all_impl, &obj, confun);
	    }
	}
	regEnd(&reg_gen);
    }

    /* Now intersecting segments (if any) */
    for (i = 0;  i < daLen(all_impl);  i++) {
	p_impl = daGet(termImpl, all_impl, i);
	if (p_impl->geo.type == OCT_BOX) {
	    /* Find associated layer */
	    dest_lyr = p_impl->lyr;
	    if (octGetByName(fct, &dest_lyr) == OCT_OK) {
		regInit(&dest_lyr, &(p_impl->geo.contents.box), OCT_PATH_MASK, &reg_gen);
		while (regNext(&reg_gen, &obj, &np, &pnts, (struct octBox *) 0) == REG_OK) {
		    check_seg(term, &(p_impl->geo.contents.box), &dest_lyr, &obj, np, pnts, dead);
		}
		regEnd(&reg_gen);
	    }
	}
    }
    daFree(all_impl, (void (*)()) 0);
}



typedef struct match_defn {
    octObject lyr;
    octObject geo1;
    octObject geo2;
} match;

static void check_inst(fct, at, size, impls, inst, confun)
octObject *fct;			/* Facet to modify                 */
octObject *at;			/* Actual terminal of an instance  */
struct octBox *size;		/* Actual terminal size            */
dary impls;			/* Actual terminal implementations */
octObject *inst;		/* Intersecting instance           */
symConFunc *confun;		/* Connector function              */
/*
 * This routine checks the connectivity of `at' considering `inst'
 * intersects its implementation (whose size is `size').  It checks
 * to see if an appropriate terminal implementation of `inst' 
 * intersects `size' on the right layer.  If so, it checks to
 * make sure the terminals are both on the same net.  If they
 * aren't, theu are connected using three segments on a shared
 * layer between the implementations.
 */
{
    octObject one_lyr, geo, term;
    octGenerator gen;
    symTermGen t_gen;
    int i;
    dary common;
    match one;

    /* Generate terminals of instance to find intersections */
    common = daAlloc(match, INIT_LYRS);
    SYMCK(octInitGenContents(inst, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &term) == OCT_OK) {
	daReset(common);
	symInitTerm(&term, &t_gen);
	while (symNextTerm(t_gen, &one_lyr, &geo)) {
	    if (geo.type == OCT_BOX) {
		/* Only box implementations checked */
		for (i = 0;  i < daLen(impls);  i++) {
		    if (STRCMP(daGet(termImpl, impls, i)->lyr.contents.layer.name,
			       one_lyr.contents.layer.name) == 0) {
			/* Layer names match */
			if (SYMBOXINTERSECT(daGet(termImpl, impls, i)->geo.contents.box,
					    geo.contents.box)) {
			    one.lyr = daGet(termImpl, impls, i)->lyr;
			    one.geo1 = daGet(termImpl, impls, i)->geo;
			    one.geo2 = geo;
			    *daSetLast(match, common) = one;
			}
		    }
		    
		}
	    }
	}
	symEndTerm(t_gen);
	/* Now actually match them */
	if (daLen(common) > 0) match_term(fct, at, &term, common, confun);
    }
    daFree(common, (void (*)()) 0);
}


static void match_term(fct, t_one, t_two, common, confun)
octObject *fct;			/* Facet to modify        */
octObject *t_one;		/* First actual terminal  */
octObject *t_two;		/* Second actual terminal */
dary common;			/* Possible connection alternatives */
symConFunc *confun;		/* Connector function     */
/*
 * This routine is called when two instance terminals are found
 * that intersect on common layers.  If these terminals are not
 * already connected, they will be connected using segments from
 * one of the common layers.  The routine measures the degree
 * of freedom of the terminals.  If one or both are free, a minimum
 * connection is made between them.  If neither is free, it
 * will connect them using multiple segments to increase
 * freedom.  If there is more than one common
 * layer, a user-settable function will be called to determine
 * an appropriate layer.
 */
{
    octObject net_1, net_2, *lyrs;
    int i, idx;
    match *found;

    if (symLocNet(t_one, &net_1) && symLocNet(t_two, &net_2) &&
	(octIdsEqual(net_1.objectId, net_2.objectId))) {
	/* Terminals already connected */
	return;
    }
    if (daLen(common) > 1) {
	/* Use symaltLyrFunc to determine which one */
	lyrs = ALLOC(octObject, daLen(common));
	for (i = 0;  i < daLen(common);  i++) {
	    lyrs[i] = daGet(match, common, i)->lyr;
	}
	idx = (*curAltFunc.lyr_alt_func)(t_one, t_two, daLen(common),
					 lyrs, curAltFunc.data);
	if ((idx < 0) || (idx >= daLen(common))) {
	    /* Nasty error -- routine returned out of bounds */
	    errRaise(sym_pkg_name, SYM_BADFUNC, "Index bounds out of range");
	    /*NOTREACHED*/
	}
    } else {
	idx = 0;
    }
    found = daGet(match, common, idx);
#ifdef OLDCODE
    if (symDirChanges(t_one, 1) || symDirChanges(t_two, 1)) {
	min_connect(fct, t_one, t_two, &(found->lyr),
		    &(found->geo1.contents.box), &(found->geo2.contents.box),
		    confun);
    } else {
	max_connect(fct, t_one, t_two, &(found->lyr),
		    &(found->geo1.contents.box), &(found->geo2.contents.box),
		    confun);
    }
#else
    /* Always go with the minimal connection. */
    min_connect(fct, t_one, t_two, &(found->lyr),
		&(found->geo1.contents.box), &(found->geo2.contents.box),
		confun);
    
#endif
}



static int find_dir(seg)
octObject *seg;			/* Segment */
/*
 * Finds the direction (horizontal or vertical) for the segment.
 */
{
    struct octPoint pnts[2];
    octCoord diff_x, diff_y;
    int32 np;

    np = 2;
    if (octGetPoints(seg, &np, pnts) == OCT_OK) {
	diff_x = ABS(pnts[0].x - pnts[1].x);
	diff_y = ABS(pnts[0].y - pnts[1].y);
	if (diff_x > diff_y) return HORI;
	else if (diff_x < diff_y) return VERT;
	else return NONE;
    } else {
	return NONE;
    }
}

static int look(term, visited, prev, num_dir)
octObject *term;		/* Actual terminal    */
st_table *visited;		/* Already visited    */
int prev;			/* Previous direction */
int num_dir;			/* Number of directions */
{
    octGenerator gen, up;
    octObject seg, new_term;
    int seg_dir;

    if (num_dir > 0) {
	(void) st_insert(visited, (char *) term->objectId, (char *) 0);
	SYMCK(octInitGenContainers(term, OCT_PATH_MASK, &gen));
	while (octGenerate(&gen, &seg) == OCT_OK) {
	    SYMCK(octInitGenContents(&seg, OCT_TERM_MASK, &up));
	    while (octGenerate(&up, &new_term) == OCT_OK) {
		if (!octIdsEqual(new_term.objectId, term->objectId) &&
		    !st_is_member(visited, (char *) new_term.objectId)) {
		    seg_dir = find_dir(&seg);
		    if (look(&new_term, visited, seg_dir,
				  (((seg_dir != prev) && (prev != NONE)) ?
				   num_dir-1 : num_dir))) {
			return 1;
		    }
		      
		}
	    }
	}
	return 0;
    } else {
	return 1;
    }
}

int symDirChanges(term, num)
octObject *term;		/* Actual terminal   */
int num;			/* Direction changes */
/*
 * This routine traces segments from term and returns one if
 * there are at least `num' changes in directions.  It does
 * not search through non-connector terminals.  It stops
 * the search as soon as the required number of changes
 * is found.
 */
{
    st_table *visited;
    int result;

    visited = st_init_table(st_numcmp, st_numhash);
    result = look(term, visited, NONE, num);
    st_free_table(visited);
    return result;
}



static void min_connect(fct, term_1, term_2, lyr, box_1, box_2, confun)
octObject *fct;			/* Facet to modify        */
octObject *term_1;		/* First actual terminal  */
octObject *term_2;		/* Second actual terminal */
octObject *lyr;			/* Chosen layer           */
struct octBox *box_1;		/* First box              */
struct octBox *box_2;		/* Second box             */
symConFunc *confun;		/* Connector function     */
/*
 * This routine is called when two terminals are found to
 * be compatible.  It connects them with the largest possible
 * manhattan path.
 */
{
    struct octPoint pnts[2];
    octCoord width, diff_x, diff_y;
    struct octBox intersection;	/* Intersection of the two boxes. */
    int vertical_segment_flag;

    intersection.lowerLeft.x = MAX( box_1->lowerLeft.x, box_2->lowerLeft.x );
    intersection.lowerLeft.y = MAX( box_1->lowerLeft.y, box_2->lowerLeft.y );
    intersection.upperRight.x = MIN( box_1->upperRight.x, box_2->upperRight.x );
    intersection.upperRight.y = MIN( box_1->upperRight.y, box_2->upperRight.y );

    diff_x = intersection.upperRight.x - intersection.lowerLeft.x;
    diff_y = intersection.upperRight.y - intersection.lowerLeft.y;

    if ( diff_x < 0 || diff_y < 0 ) {
	errRaise( sym_pkg_name, 1, "terminals do not intersect" );
    }

    vertical_segment_flag = ( diff_x > diff_y );
    width = MAX( diff_x, diff_y );

    /* Compute segment points */
    if ( vertical_segment_flag ) {
	pnts[0].y = (box_1->lowerLeft.y + box_1->upperRight.y)/2;
	pnts[0].x = (intersection.lowerLeft.x + intersection.upperRight.x)/2;
	pnts[1].y = (box_2->lowerLeft.y + box_2->upperRight.y)/2;
	pnts[1].x = pnts[0].x;
    } else {
	pnts[0].x = (box_1->lowerLeft.x + box_1->upperRight.x)/2;
	pnts[0].y = (intersection.lowerLeft.y + intersection.upperRight.y)/2;
	pnts[1].x = (box_2->lowerLeft.x + box_2->upperRight.x)/2;
	pnts[1].y = pnts[0].y;
    }

    /* Make segment */
    symSegments(fct, term_1, term_2, 2, pnts, 1, lyr, 1, &width, confun);
}

#ifdef NEED_OLD_MIN_CONNECT
static void old_min_connect(fct, term_1, term_2, lyr, box_1, box_2, confun)
octObject *fct;			/* Facet to modify        */
octObject *term_1;		/* First actual terminal  */
octObject *term_2;		/* Second actual terminal */
octObject *lyr;			/* Chosen layer           */
struct octBox *box_1;		/* First box              */
struct octBox *box_2;		/* Second box             */
symConFunc *confun;		/* Connector function     */
/*
 * This routine is called when two terminals are found to
 * be compatible.  It connects them simply using a single
 * segment from the center of one to the center of the
 * other.
 */
{
    struct octPoint pnts[2];
    octCoord width, diff_x, diff_y, w1, w2;

    /* Compute segment points */
    pnts[0].x = (box_1->lowerLeft.x + box_1->upperRight.x)/2;
    pnts[0].y = (box_1->lowerLeft.y + box_1->upperRight.y)/2;
    pnts[1].x = (box_2->lowerLeft.x + box_2->upperRight.x)/2;
    pnts[1].y = (box_2->lowerLeft.y + box_2->upperRight.y)/2;

    /* Determine width */
    if ((diff_x = pnts[0].x - pnts[1].x) < 0) diff_x = -diff_x;
    if ((diff_y = pnts[0].y - pnts[1].y) < 0) diff_y = -diff_y;
    if (diff_x > diff_y) {
	w1 = box_1->upperRight.y - box_1->lowerLeft.y;
	w2 = box_2->upperRight.y - box_2->lowerLeft.y;
    } else {
	w1 = box_1->upperRight.x - box_1->lowerLeft.x;
	w2 = box_2->upperRight.x - box_2->lowerLeft.x;
    }
    if (w1 < w2) width = w1; else width = w2;

    /* Make segment */
    symSegments(fct, term_1, term_2, 2, pnts, 1, lyr, 1, &width, confun);
}
#endif /* NEED_OLD_MIN_CONNECT*/


#ifdef NEED_MAX_CONNECT 
static void max_connect(fct, term_1, term_2, lyr, box_1, box_2, confun)
octObject *fct;			/* Facet to modify        */
octObject *term_1;		/* First actual terminal  */
octObject *term_2;		/* Second actual terminal */
octObject *lyr;			/* Chosen layer           */
struct octBox *box_1;		/* First box              */
struct octBox *box_2;		/* Second box             */
symConFunc *confun;		/* Connector function     */
/*
 * This routine is called when two terminals are found to
 * be compatible.  It connects them using an L-shaped
 * connection to preserve (or introduce) degrees of
 * freedom.
 */
{
    struct octPoint pnts[4];
    octCoord width, diff_x, diff_y, w1, w2;

    /* Compute segment points */
    pnts[0].x = (box_1->lowerLeft.x + box_1->upperRight.x)/2;
    pnts[0].y = (box_1->lowerLeft.y + box_1->upperRight.y)/2;
    pnts[3].x = (box_2->lowerLeft.x + box_2->upperRight.x)/2;
    pnts[3].y = (box_2->lowerLeft.y + box_2->upperRight.y)/2;

    /* Determine width and juking direction */
    if ((diff_x = pnts[0].x - pnts[3].x) < 0) diff_x = -diff_x;
    if ((diff_y = pnts[0].y - pnts[3].y) < 0) diff_y = -diff_y;
    if (diff_x < diff_y) {
	/* Horizontal */
	pnts[3].y -= 1;
	pnts[1].x = pnts[2].x = (pnts[0].x + pnts[3].x)/2;
	pnts[1].y = pnts[0].y;
	pnts[2].y = pnts[3].y;
	w1 = box_1->upperRight.y - box_1->lowerLeft.y;
	w2 = box_2->upperRight.y - box_2->lowerLeft.y;
    } else {
	/* Vertical */
	pnts[3].x -= 1;
	pnts[1].y = pnts[2].y = (pnts[0].y + pnts[3].y)/2;
	pnts[1].x = pnts[0].x;
	pnts[2].x = pnts[3].x;
	w1 = box_1->upperRight.x - box_1->lowerLeft.x;
	w2 = box_2->upperRight.x - box_2->lowerLeft.x;
    }
    if (w1 < w2) width = w1; else width = w2;

    /* Make segment */
    symSegments(fct, term_1, term_2, 4, pnts, 1, lyr, 1, &width, confun);
}
#endif /*NEED_MAX_CONNECT*/



static void check_seg(at, impl, lyr, seg, np, pnts, dead)
octObject *at;			/* Actual terminal of an instance         */
struct octBox *impl;		/* Implementation area                    */
octObject *lyr;			/* Layer of object in instance facet      */
octObject *seg;			/* Intersecting segment                   */
int np;				/* Number of segment points (should be 2) */
struct octPoint *pnts;		/* Array of points                        */
octObject *dead;		/* Items to be deleted                    */
/*
 * This routine checks the connectivity of `seg'.  `seg' was found to
 * intersect an implementation of `at' on layer `lyr' of size `impl'.
 * The routine determines whether or not `seg' is already connected
 * to `at' and if not, connects the segment appropriately.  This may
 * require breaking the segment into two segments if none of the 
 * endpoints intersect the terminal implementation.  The routine
 * will remove bad segments (those with too many points or those
 * that aren't connected to a terminal on both ends).
 */
{
    octObject seg_net, term_net;
    int num_intersect, i;
    segTermInfo info[2];

    if (np != 2) {
	symMsg(SYM_WARN, "Segment found with more or less than 2 points -- deleting");
	SYMCK(octAttachOnce(dead, seg));
    } else {
	/* Gather information about intersecting segment endpoints */
	symSegTerms(seg, pnts, info);
	if (octIdIsNull(info[0].term.objectId) || octIdIsNull(info[1].term.objectId)) {
	    symMsg(SYM_WARN, "Segment found without appropriate endpoint connections -- deleting");
	    SYMCK(octAttachOnce(dead, seg));
	} else {
	    /* Check the number of points that intersect implementation region */
	    num_intersect = 0;
	    for (i = 0;  i < np;  i++) {
		if (POINT_IN_BOX(pnts+i, impl)) num_intersect++;
	    }
	    if (num_intersect == 0) {
		no_intersect(at, impl, lyr, seg, np, pnts, info);
	    } else {
		int error = 0;
		char* errmsg = 0;
		if (!symLocNet(seg, &seg_net) )  {
		    errmsg = "No net on segment";
		    error++;
		} 
		if ( !symLocNet(at, &term_net) ) {
		    errmsg = "No net on terminal";
		    error++;
		}
		if ( !octIdsEqual(seg_net.objectId, term_net.objectId) ) {
		    errmsg = "Nets are not the same";
		    error++;
		}
		if ( error ) {
		    symMsg(SYM_WARN, "Segment intersecting instance terminal not connected appropriately -- deleting");
		    SYMCK(octAttach(dead, seg));
		}
	    }
	}
    }
}



static void no_intersect(at, impl, lyr, seg, np, pnts, info)
octObject *at;			/* Actual terminal of an instance         */
struct octBox *impl;		/* Implementation area                    */
octObject *lyr;			/* Layer of object in instance facet      */
octObject *seg;			/* Intersecting segment                   */
int np;				/* Number of segment points (should be 2) */
struct octPoint *pnts;		/* Array of points                        */
segTermInfo info[2];		/* Segment endpoint information           */
/*
 * `seg' intersects `at' but neither endpoint intersects the implementation
 * `impl'.  This generally means the segment must be broken into two segments
 * and the middle endpoints must be connected to the actual terminal.  The
 * segment may not actually intersect the box due to the overagressive
 * bounding box of non-manhattan segments.  If this is the case, no
 * action is taken.
 */
{
    struct octPoint bp1, bp2, mid, sp[2];
    octObject new_seg, net, term_net, *final;

    if (!symLocNet(seg, &net)) {
	/* Can't find net for segment -- make one */
	symMsg(SYM_WARN, "Found segment without a net -- creating one");
	symCreateNamedNet(seg, &net);
	SYMCK(octAttachOnce(&net, seg));
    }
    /*
     * Only break the segment if it is on a different net or the
     * freedom of the segment to move is less than a threshold
     * and the segment actually intersects the implementation.
     */
    term_net.objectId = oct_null_id;
    if ((!symLocNet(at, &term_net) ||
	 !octIdsEqual(net.objectId, term_net.objectId) ||
	 !symDirChanges(&(info[0].term), FREEDOM_THRESHOLD)) &&
	(line_cross_box(impl, &(pnts[0]), &(pnts[1]), &bp1, &bp2))) {
	/* Intersects implementation and should be broken */
	mid.x = (bp1.x + bp2.x)/2;
	mid.y = (bp1.y + bp2.y)/2;

	/* Create new segment from mid to second point */
	new_seg.type = OCT_PATH;
	new_seg.contents.path.width = seg->contents.path.width;
	SYMCK(octCreate(lyr, &new_seg));
	sp[0] = mid;  sp[1] = pnts[1];
	SYMCK(octPutPoints(&new_seg, 2, sp));
	
	SYMCK(octAttachOnce(&net, &new_seg));
	SYMCK(octAttach(&new_seg, at));
	SYMCK(octAttach(&new_seg, &(info[1].term)));
	
	/* Modify first segment */
	SYMCK(octDetach(seg, &(info[1].term)));
	sp[0] = pnts[0];  sp[1] = mid;
	SYMCK(octPutPoints(seg, 2, sp));
	SYMCK(octAttach(seg, at));

	/* Now merge the nets */
	if (!octIdIsNull(term_net.objectId) ) {
	    final = symMergeNets(&term_net, &net);
	} else {
	    final = &net;
	    if (symInstanceP(at)) SYMCK(octAttach(final, at));
	}
    }
}



static int line_cross_box(box, p1, p2, pmod1, pmod2)
register struct octBox *box;			/* Region to clip to */
struct octPoint *p1, *p2;			/* Line to clip      */
register struct octPoint *pmod1, *pmod2;	/* Clipped line      */
/*
 * This routine checks to see if the line from `p1' to `p2'
 * intersects `box'.  If it does,  the routine will return
 * a non-zero value and the clipped result will be returned in `pmod1'
 * and `pmod2'.  If it does not intersect the box,  the routine
 * will return zero and `pmod1' and `pmod2' will be
 * undefined.  The clipping algorithm used is Cohen and
 * Sutherland (taken from Newmann and Sproull).
 */
{
    register int x = 0, y = 0;
    int c1, c2, cd;

    *pmod1 = *p1;     *pmod2 = *p2;
    DO_CODE(p1, c1);  DO_CODE(p2, c2);

    /* Begin line clipper */
    while (c1 || c2) {
	/* See if entirely off screen */
	if (c1 & c2) return 0;
	cd = (c1 ? c1 : c2);
	if (cd & LEFT_CODE) {	/* Crosses left edge */
	    y = pmod1->y + (pmod2->y - pmod1->y) *
	      (box->lowerLeft.x - pmod1->x) / (pmod2->x - pmod1->x);
	    x = box->lowerLeft.x;
	} else if (cd & RIGHT_CODE) { /* Crosses right edge */
	    y = pmod1->y + (pmod2->y - pmod1->y) *
	      (box->upperRight.x - pmod1->x) / (pmod2->x - pmod1->x);
	    x = box->upperRight.x;
	} else if (cd & BOTTOM_CODE) { /* Crosses bottom edge */
	    x = pmod1->x + (pmod2->x - pmod1->x) *
	      (box->lowerLeft.y - pmod1->y) / (pmod2->y - pmod1->y);
	    y = box->lowerLeft.y;
	} else if (cd & TOP_CODE) { /* Crosses top edge */
	    x = pmod1->x + (pmod2->x - pmod1->x) *
	      (box->upperRight.y - pmod1->y) / (pmod2->y - pmod1->y);
	    y = box->upperRight.y;
	}
	if (cd == c1) {
	    pmod1->x = x;  pmod1->y = y;
	    DO_CODE(pmod1, c1);
	} else {
	    pmod2->x = x;  pmod2->y = y;
	    DO_CODE(pmod2, c2);
	}
    }
    return 1;
}
