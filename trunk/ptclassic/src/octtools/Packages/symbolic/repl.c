#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
 * 
 */
/*LINTLIBRARY*/
/*
 * Instance Replacement
 *
 * David Harrison
 * University of California, Berkeley
 * 1988, 1990
 *
 * This file contains an implementation for instance replacement
 * that uses a number of heuristics for attempting to recover
 * from deleted, renamed, and additional terminals for the replacement
 * instance.
 *
 * Work to be done:
 *  - Jump terminal replacement?
 */

#include "internal.h"
#include "utility.h"
#include "oh.h"
#include "delete.h"
#include "region.h"
#include "st.h"
#include "inst.h"
#include "terminfo.h"

#include "repl.h"

#define NO_IMPL	-1

#define DOOMED_SEGS	";;; DoomedSegBag ;;;"
#define STRCOPY(s)	strcpy(ALLOC(char, strlen(s)+1), (s))

typedef struct nt_info {
    octObject at;		/* Actual terminal */
    struct octBox bb;		/* Bounding region */
    int l_count;		/* Layer count     */
    char **l_ary;		/* Layer array     */
} ntInfo;

static st_table *new_term_table
  ARGS((octObject *inst));
  /* Make new hash table of actual terminals     */
static void copy_conn
  ARGS((octObject *source, octObject *dest,
	octObjectMask containers, octObjectMask contents));
  /* Copy connections from one object to another */
static void match_by_name
  ARGS((octId fctId, st_table *nt, st_table *ot, octObject *delbag));
  /* Matches actual terminals by name            */
static void fill_term_info
  ARGS((st_table *at_table));
  /* Obtain more detailed information about terminal implementations */
static void trans_term();
  /* Move segments from old terminal to new terminal */
static void pos_match
  ARGS((octId fctId, st_table *new_terms, st_table *old_terms,
	octObject *dead_segs));
  /* Attempt to match terminals by position      */
static enum st_retval del_ntinfo
  ARGS((char *key, char *value, char *arg));
  /* Foreach function for destroying nt_info entries */
static int move_end
  ARGS((octId fctId, octObject *seg, struct octBox *new_impl));
  /* Moves endpoint of segment subject to constraints */



void symReplaceInst(fct, old, new)
octObject *fct;			/* Facet for both */
octObject *old;			/* Old instance */
octObject *new;			/* New instance */
/*
 * This routine replaces the instance `old' with `new'.  The
 * instance `new' will be created and all connections from
 * `old' will be moved to `new'.  The old instance will then
 * be deleted.  Actual terminal correspondence is done first 
 * by name then by position.  The routine will raise errors 
 * and print warnings if there are problems.
 */
{
    octObject delbag;
    st_table *old_terms, *new_terms;

    /* Create bag for dead items */
    delbag.type = OCT_BAG;
    delbag.contents.bag.name = DOOMED_SEGS;
    SYMCK(octCreate(fct, &delbag));
    octMarkTemporary(&delbag);

    /* Create the new instance */
    SYMCK(octCreate(fct, new));

    /* First, instance connections themselves */
    copy_conn(old, new,
	      OCT_ALL_MASK & ~(OCT_FACET_MASK),  /* Except OCT_FACET */
	      OCT_ALL_MASK & ~(OCT_TERM_MASK));  /* Except terminals */

    /* Place terminals of old terminals by name */
    old_terms = new_term_table(old);
    new_terms = new_term_table(new);

    /* Add more detailed info to new terminal table */
    fill_term_info(new_terms);

    /* Try to do name correspondences */
    match_by_name(fct->objectId, new_terms, old_terms, &delbag);

    /* Try direct positional correspondence */
    pos_match(fct->objectId, new_terms, old_terms, &delbag);

    /* Destroy hash tables */
    st_free_table(old_terms);
    (void) st_foreach(new_terms, del_ntinfo, (char *) 0);
    st_free_table(new_terms);

    /* Remove objects */
    symDeleteObjects(&delbag, (symDelItemFunc *) 0);
    SYMCK(octDelete(&delbag));

    /* Remove old instance, unless it has already been deleted by symDeleteObjects(). */
    if ( octGetById( old ) == OCT_OK ) { /* Added by AC Aug 14 1991 */
	SYMCK(octDelete(old));	
    }
}



static st_table *new_term_table(inst)
octObject *inst;		/* Instance for table */
/*
 * This routine creates and returns a hash table that contains
 * all actual terminals of `inst' entered by name.  The value
 * of the items inserted is the octId of the actual terminal
 * object.
 */
{
    octObject at;
    octGenerator gen;
    st_table *ret_table;

    ret_table = st_init_table(strcmp, st_strhash);
    SYMCK(octInitGenContents(inst, OCT_TERM_MASK, &gen));
    while (octGenerate(&gen, &at) == OCT_OK) {
	st_insert(ret_table, at.contents.term.name, (char *) at.objectId);
    }
    return ret_table;
}

static void fill_term_info(at_table)
st_table *at_table;		/* Hash table of terminals */
/*
 * This routine generates through the terminals in `at_table'
 * and stores implementation information with each terminal.
 */
{
    st_generator *gen = st_init_gen(at_table), *lg;
    st_table *layers;
    char *key, *value;
    ntInfo *new_info;
    int i;
    symTermGen tg;
    octObject lyr, geo;

    while (st_gen(gen, &key, &value)) {
	new_info = ALLOC(ntInfo, 1);
	new_info->at.objectId = (octId) value;
	SYMCK(octGetById(&(new_info->at)));
	new_info->l_count = NO_IMPL;

	new_info->bb.lowerLeft.x = new_info->bb.lowerLeft.y = OCT_MAX_COORD;
	new_info->bb.upperRight.x = new_info->bb.upperRight.y = OCT_MIN_COORD;

	layers = st_init_table(strcmp, st_strhash);
	symInitTerm(&(new_info->at), &tg);
	while (symNextTerm(tg, &lyr, &geo)) {
	    if (geo.type == OCT_BOX) {
		LA(geo.contents.box.lowerLeft.x, new_info->bb.lowerLeft.x);
		LA(geo.contents.box.lowerLeft.y, new_info->bb.lowerLeft.y);
		GA(geo.contents.box.upperRight.x, new_info->bb.upperRight.x);
		GA(geo.contents.box.upperRight.y, new_info->bb.upperRight.y);
	    }
	    st_insert(layers, lyr.contents.layer.name, (char *) 0);
	}
	symEndTerm(tg);
	if ((new_info->l_count = st_count(layers)) > 0) {
	    new_info->l_ary = ALLOC(char *, new_info->l_count);
	    lg = st_init_gen(layers);
	    for (i = 0; i < new_info->l_count;  i++) {
		(void) st_gen(lg, (char **) new_info->l_ary+i, (char **) 0);
	    }
	    st_free_gen(lg);
	} else {
	    new_info->l_ary = (char **) 0;
	}
	st_free_table(layers);
	/* Make the new value the new information */
	st_insert(at_table, key, (char *) new_info);
    }
    st_free_gen(gen);
}



/*ARGSUSED*/
static enum st_retval del_ntinfo(key, value, arg)
char *key;			/* Terminal name      */
char *value;			/* (struct nt_info *) */
char *arg;			/* Unused             */
/*
 * Releases all nt_info information from the passed entry.
 */
{
    struct nt_info *info = (struct nt_info *) value;

#ifdef NOTDEF
    int i;

    for (i = 0;  i < info->l_count;  i++) {
	FREE(info->l_ary[i]);
    }
#endif
    if (info->l_count) FREE(info->l_ary);
    FREE(info);
    return ST_DELETE;
}




static void copy_conn(source, dest, containers, contents)
octObject *source;		/* Source object */
octObject *dest;		/* Destination object */
octObjectMask containers;	/* Containing object mask */
octObjectMask contents;		/* Content object mask */
/*
 * This routine copies the attachments from `source' onto `dest'.
 * Only attachments to objects specified in `containers' and `contents'
 * will be copied.
 */
{
    octGenerator gen;
    octObject obj;

    SYMCK(octInitGenContainers(source, containers, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	SYMCK(octAttach(&obj, dest));
    }
    SYMCK(octInitGenContents(source, contents, &gen));
    while (octGenerate(&gen, &obj) == OCT_OK) {
	SYMCK(octAttach(dest, &obj));
    }
}



typedef struct minfo_defn {
    st_table *ot;		/* Old hash table    */
    octId fctId;		/* Redisplay facet   */
    octObject *delbag;		/* For deleted stuff */
} minfo;

/*ARGSUSED*/
static enum st_retval do_match(key, value, arg)
char *key;			/* Terminal name           */
char *value;			/* Terminal info           */
char *arg;			/* Old Terminal Hash Table */
/*
 * This examines each new terminal as explained below.
 */
{
    octObject old_at;
    ntInfo *new_at = (ntInfo *) value;
    minfo *info = (minfo *) arg;

    if (st_delete(info->ot, &(new_at->at.contents.term.name),
		  (char **) &(old_at.objectId))) {
	/* Matched terminals */
	octGetById(&old_at);
	trans_term(info->fctId, new_at, &old_at, info->delbag);
	(void) del_ntinfo((char *) 0, (char *) new_at, (char *) 0);
	return ST_DELETE;
    } else {
	return ST_CONTINUE;
    }
}

static void match_by_name(fctId, nt, ot, delbag)
octId fctId;			/* Facet for new instance      */
st_table *nt;			/* Hash table of new terminals */
st_table *ot;			/* Hash table of old terminals */
octObject *delbag;		/* Bag for deleted items       */
/*
 * This routine generates through the actual terminals in `nt'
 * and attempts to find each one in `ot'.  If found,  all connections
 * between the old and new terminals are copied across.  Then
 * each segment connected to the old terminal is examined and
 * if suitable,  its endpoint is snapped to the center of the
 * new terminal.  If not suitable,  the segment will be ripped
 * back.  The terminal name is then removed from `ot'.
 */
{
    minfo info;

    info.ot = ot;  info.fctId = fctId;  info.delbag = delbag;
    (void) st_foreach(nt, do_match, (char *) &info);
}


static void trans_term(fctId, new_at, old_at, dead_segs)
octId fctId;			/* Facet for both      */
ntInfo *new_at;			/* New actual terminal */
octObject *old_at;		/* Old actual terminal */
octObject *dead_segs;		/* Ripped segment bag  */
/*
 * This routine copies all appropriate connections from `old_at'
 * to `new_at'.  All of the segments attached to `old_at' are
 * then searched.  If a segment is found to be suitable,
 * its endpoint is moved to the center of the `new_at's 
 * implementation.  Otherwise,  the segment is marked for
 * deletion by adding it to the `dead_segs' bag.
 */
{
    octObject seg, seg_lyr;
    octGenerator gen;
    int i, rip_flag;

    (void) copy_conn(old_at, &(new_at->at), OCT_ALL_MASK & ~(OCT_INSTANCE_MASK),
		     OCT_ALL_MASK);

    SYMCK(octInitGenContainers(old_at, OCT_PATH_MASK, &gen));
    while (octGenerate(&gen, &seg) == OCT_OK) {
	SYMCK(octGenFirstContainer(&seg, OCT_LAYER_MASK, &seg_lyr));
	rip_flag = 1;
	if ( new_at->l_count <= 0 ) {
	    octObject inst;
	    ohGetById( &inst, new_at->at.contents.term.instanceId );
	    errRaise( sym_pkg_name, SYM_BADIMPL, 
		     "Bad implementation for\n%s\nof %s\n",
		     ohFormatName( &(new_at->at)), ohFormatName( &inst ));
	}
	for (i = 0;  i < new_at->l_count;  i++) {
	    if (STRCMP(seg_lyr.contents.layer.name, new_at->l_ary[i]) == 0) {
		/* Found to be suitable */
		if (move_end(fctId, &seg, &(new_at->bb))) rip_flag = 0;
		break;
	    }
	}
	if (rip_flag) {
	    SYMCK(octAttach(dead_segs, &seg));
	}
    }
}



#define MAXP	2

static int move_end(fctId, seg, new_impl)
octId fctId;			/* Facet for `seg'             */
octObject *seg;			/* Segment to modify           */
struct octBox *new_impl;	/* New terminal implementation */
/*
 * This routine moves the closest point of `seg' to the center
 * of `new_impl'.  If this is already true,  nothing is done.
 * The routine returns a non-zero status if the endpoint
 * was successfully moved.
 */
{
    int32 np = MAXP;
    struct octPoint pnts[MAXP];
    struct octPoint i_center;
    int min_dist, dist, i, idx;

    i_center.x = (new_impl->lowerLeft.x + new_impl->upperRight.x)/2;
    i_center.y = (new_impl->lowerLeft.y + new_impl->upperRight.y)/2;
    SYMCK(octGetPoints(seg, &np, pnts));

    /* Find closest one to center of new_impl */
    idx = -1;
    min_dist = OCT_MAX_COORD;
    for (i = 0;  i < MAXP;  i++) {
	dist = DIST(pnts[i], i_center);
	if (dist <= min_dist) {
	    min_dist = dist;
	    idx = i;
	}
    }

    if ((pnts[idx].x != i_center.x) || (pnts[idx].y != i_center.y)) {
	/* Move point */
	pnts[idx] = i_center;
	if (octPutPoints(seg, np, pnts) == OCT_OK) {
	    return 1;
	} else {
	    return 0;
	}
    }
    return 1;
}



typedef struct pos_info_defn {
    struct octPoint *pnts;	/* Segment endpoints */
    ntInfo *near_term;		/* Nearest term      */
    struct octPoint near_p;	/* Nearest point     */
    int min_dist;		/* Minimum distance  */
} pos_info;

/*ARGSUSED*/
static enum st_retval find_nearest(key, value, arg)
char *key;			/* Terminal name       */
char *value;			/* (ntInfo *)          */
char *arg;			/* (pos_info *) */
/*
 * This routine compares the distance between the center
 * of the passed terminal and the two segment points
 * passed in `arg'.  If the distance is less than a recorded
 * minimum,  it is recorded.
 */
{
    ntInfo *ti = (ntInfo *) value;
    pos_info *si = (pos_info *) arg;
    struct octPoint cen;
    int dist, i;

    cen.x = (ti->bb.lowerLeft.x + ti->bb.upperRight.x)/2;
    cen.y = (ti->bb.lowerLeft.y + ti->bb.upperRight.y)/2;
    for (i = 0;  i < MAXP;  i++) {
	dist = DIST(cen, si->pnts[i]);
	if (dist <= si->min_dist) {
	    si->near_term = ti;
	    si->near_p = si->pnts[i];
	    si->min_dist = dist;
	}
    }
    return ST_CONTINUE;
}


static void pos_match(fctId, new_terms, old_terms, dead_segs)
octId fctId;			/* Facet for redisplay       */
st_table *new_terms;		/* Unconnected new terminals */
st_table *old_terms;		/* Unconnected old terminals */
octObject *dead_segs;		/* Ripped back segments      */
/*
 * This routine goes through all unconnected old terminals and
 * trys to find the nearest directly corresponding new terminal
 * implementation for the terminal which is not already connected.
 */
{
    octGenerator og;
    st_generator *gen = st_init_gen(old_terms);
    char *key, *value;
    octObject oterm, seg, lyr;
    int32 np = MAXP;
    struct octPoint pnts[MAXP];
    pos_info info;

    while (st_gen(gen, &key, &value)) {
	oterm.objectId = (octId) value;
	SYMCK(octGetById(&oterm));
	/* Generate all segments of old terminal */
	SYMCK(octInitGenContainers(&oterm, OCT_PATH_MASK, &og));
	info.min_dist = OCT_MAX_COORD;
	while (octGenerate(&og, &seg) == OCT_OK) {
	    /* Get points and layer for segment */
	    SYMCK(octGenFirstContainer(&seg, OCT_LAYER_MASK, &lyr));
	    SYMCK(octGetPoints(&seg, &np, pnts));
	    /* Find closest suitable unconnected new terminal implementation */
	    info.pnts = pnts;
	    (void) st_foreach(new_terms, find_nearest, (char *) &info);
	}
	/* The endpoint must be in the terminal implementation */
	if ((info.min_dist < OCT_MAX_COORD) &&
	    (POINT_IN_BOX(&(info.near_p), &(info.near_term->bb)))) {
	    /* Try to transform terminals */
	    trans_term(fctId, info.near_term, &oterm, dead_segs);
	} else {
	    /* Rip back all segments */
	    SYMCK(octInitGenContainers(&oterm, OCT_PATH_MASK, &og));
	    while (octGenerate(&og, &seg) == OCT_OK) {
		SYMCK(octAttach(dead_segs, &seg));
	    }
	}
    }
}

#ifdef PRT
/* Debugging print functions */

int prt_ntinfo(ptr, val)
char *ptr;
char *val;
{
    ntInfo *nt = (ntInfo *) val;
    char *start = ptr;
    int i;

    ptr += prt_str(ptr, "[(nt_info) at = ");
    ptr += prt_oct(ptr, (char *) &(nt->at));
    ptr += prt_str(ptr, ", bb = ");
    ptr += prt_box(ptr, &(nt->bb));
    ptr += prt_str(ptr, ", l_count = ");
    ptr += prt_int(ptr, nt->l_count);
    for (i = 0;  i < nt->l_count;  i++) {
	ptr += prt_str(ptr, ", l_ary[");
	ptr += prt_int(ptr, i);
	ptr += prt_str(ptr, "] = `");
	ptr += prt_str(ptr, nt->l_ary[i]);
	ptr += prt_str(ptr, "'");
    }
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

int prt_posinfo(ptr, val)
char *ptr;
char *val;
{
    pos_info *pi = (pos_info *) val;
    char *start = ptr;

    ptr += prt_str(ptr, "[(pos_info) pnts[0] = ");
    ptr += prt_point(ptr, &(pi->pnts[0]));
    ptr += prt_str(ptr, ", pnts[1] = ");
    ptr += prt_point(ptr, &(pi->pnts[1]));
    ptr += prt_str(ptr, ", near_term = ");
    ptr += prt_ntinfo(ptr, (char *) pi->near_term);
    ptr += prt_str(ptr, ", near_p = ");
    ptr += prt_point(ptr, &(pi->near_p));
    ptr += prt_str(ptr, ", min_dist = ");
    ptr += prt_int(ptr, pi->min_dist);
    ptr += prt_str(ptr, "]");
    return (int) (ptr - start);
}

#endif /* PRT */
