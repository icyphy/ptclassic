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
 * Geometric Updates
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This package provides an interface above Oct change lists for
 * examining geometric changes.  It automatically collapses the
 * events down to a minimum for quick analysis.
 */

#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "st.h"
#include "oct.h"
#include "errtrap.h"

#include "gu.h"

char *gu_pkg_name = "gu";
#define GU_BAG_NAME	";;; Geometric Updates ;;;"

/*
 * Collapsing change records requires that a small database of
 * records be constructed.  This database is implemented as
 * hash table of gu_item structures indexed by external object Id.
 * The gu_item structures are allocated in blocks to reduce
 * allocation/deallocation overhead and to simplify change
 * generation.
 */

typedef struct gu_change_defn {
    gu_op op;			/* Update type                  */
    unsigned bad_parent:1;	/* Set if wrong or no parent    */
    unsigned invalid_record:1;	/* Set if record_id has no size */
    unsigned bad_type:1;	/* Set if wrong type or no type */
    int32 externalId;		/* Changed item id              */
    octId record_id;		/* Change record id             */
    octId facet_id;		/* Facet id of change records   */
} gu_ch;

#define TBL_SIZE			211
#define TBL_DENSITY			ST_DEFAULT_MAX_DENSITY
#define TBL_GROW_FACTOR			4.0
#define TBL_DEFAULT_REORDER_FLAG	1
#define TBL_INIT_POOL_SIZE		200

#define BLOCK_SIZE	TBL_SIZE/2
#define LAST_BLOCK(blk)	((blk)->last)
struct blk_defn {
    gu_ch pool[BLOCK_SIZE];	/* Top of this block          */
    int last;			/* Current last item in block */
    struct blk_defn *next;	/* Linkage if we run out      */
};

static void new_block(blk)
struct blk_defn **blk;
/*
 * Makes a new block from an old one.  If `*blk' is zero,
 * returns an entirely new block.
 */
{
    struct blk_defn *new = ALLOC(struct blk_defn, 1);

    new->last = 0;
    new->next = *blk;
    *blk = new;
}
   
#define NEW_ITEM(blk) \
((*blk)->last < BLOCK_SIZE ? \
 ((*blk)->pool + (*blk)->last++) : \
 ((void) new_block(blk), (*blk)->pool + (*blk)->last++))

/*
 * Hidden structure definitions
 */

typedef struct gu_marker_defn {
    octObject change_list;	/* Change list for monitoring changes */
    octId parent_id;		/* Id of containing object            */
} gu_mrk;

typedef struct gu_gen_defn {
    struct blk_defn *blk;	/* Item block            */
    int current;		/* Current spot in block */
} gu_fullgen;



gu_marker guMark(container, mask)
octObject *container;		/* Object container    */
octObjectMask mask;		/* Interesting objects */
/*
 * This routine creates a new marker for examining changes to geometric
 * objects attached to `container'.  Changes will be monitored from the 
 * time the marker is created.  Only changes to geometric objects whose 
 * type is in `mask' will be monitored.  A marker created using this 
 * routine should be removed using guUnmark().
 */
{
    gu_mrk *result = ALLOC(gu_mrk, 1);
    octObject facet, gu_bag;

    octGetFacet(container, &facet);

    /* Create a bag for the change list */
    gu_bag.type = OCT_BAG;
    gu_bag.contents.bag.name = GU_BAG_NAME;
    if (octGetOrCreate(&facet, &gu_bag) < OCT_OK) {
	errRaise(gu_pkg_name, GU_NO_BAG,
		 "Cannot create bag for change list: %s",
		 octErrorString());
	/*NOTREACHED*/
    }
    octMarkTemporary(&gu_bag);

    /* Make the change list */
    result->change_list.type = OCT_CHANGE_LIST;
    result->change_list.contents.changeList.objectMask =
      mask & (OCT_INSTANCE_MASK|OCT_GEO_MASK);
    result->change_list.contents.changeList.functionMask =
      OCT_DELETE_MASK | OCT_MODIFY_MASK | OCT_ATTACH_CONTENT_MASK |
	OCT_DETACH_CONTENT_MASK | OCT_PUT_POINTS_MASK;
    if (octCreate(&gu_bag, &(result->change_list)) < OCT_OK) {
	errRaise(gu_pkg_name, GU_NO_LIST,
		 "Cannot create change list: %s", octErrorString());
	/*NOTREACHED*/
    }
    result->parent_id = container->objectId;

    return (gu_marker) result;
}
    


void guUnmark(v_mark)
gu_marker v_mark;	/* Free marker */
/*
 * Frees resources associated with `mark'.  The change list may
 * no longer exist (i.e. facet has been closed).  We don't consider
 * this a fatal error anymore.
 */
{
    gu_mrk *mark = (gu_mrk *) v_mark;

    if (octGetById(&(mark->change_list)) == OCT_OK) {
	(void) octDelete(&(mark->change_list));
    }
    FREE(mark);
}



static int tbl_query(tbl, blk, xid, rec)
st_table *tbl;
struct blk_defn **blk;
int32 xid;
gu_ch **rec;
/*
 * If `xid' is already in `tbl',  `rec' is set to point to the record
 * and the routine will return a non-zero value.  If `xid' is not in
 * the table,  a new record is allocated,  added to the table,  and returned
 * in `rec';  the routine itself returns zero.
 */
{
    char **slot;

    if (st_find_or_add(tbl, (char *) xid, &slot)) {
	/* Found the old one */
	*rec = (gu_ch *) *slot;
	return 1;
    } else {
	/* Allocate a new one */
	*rec = NEW_ITEM(blk);
	(*rec)->externalId = xid;
	*slot = (char *) *rec;
	return 0;
    }
}


static int deep_type(change)
octObject *change;		/* Modify or delete record */
/*
 * This routine determines the type of an item by generating
 * the changed object off of the change record.  If there is
 * no object under the change record,  it will return OCT_UNDEFINED_OBJECT.
 */
{
    octObject old_object;

    if (octGenFirstContent(change, OCT_ALL_MASK, &old_object) == OCT_OK) {
	return old_object.type;
    } else {
	return OCT_UNDEFINED_OBJECT;
    }
}



/* A most needed short cut */
#define CR(field)	change.contents.changeRecord.field

gu_gen guStart(v_mark)
gu_marker v_mark;
/*
 * Begins a generation of changes to geometric objects meeting the
 * criteria specified to guMark() at mark creation time.  The
 * routine returns a handle that should be used in calls to
 * guNext() to generate the changes.  Only changes up to the time
 * guStart() is called will be generated by guNext().  After generating
 * through the changes,  the caller should call guEnd() to reclaim
 * the necessary resources.
 */
{
    gu_mrk *mark = (gu_mrk *) v_mark;
    gu_fullgen *gen = ALLOC(gu_fullgen, 1);
    st_table *ch_rec_table = NIL(st_table);
    octGenerator oct_gen;
    octObject fct, change, ch_obj, sub_obj, parent;
    octObjectMask mask;
    gu_ch *info;
    int cr_count = 0;		/* Count change records in this list */

    gen->blk = (struct blk_defn *) 0;
    new_block(&(gen->blk));
    gen->current = 0;

    parent.objectId = mark->parent_id;
    if (octGetById(&parent) < OCT_OK) {
	errRaise(gu_pkg_name, GU_BAD_ID,
		 "Can't get id: %s", octErrorString());
	/*NOTREACHED*/
    }
    octGetFacet(&parent, &fct);
    if (octInitGenContents(&(mark->change_list), OCT_CHANGE_RECORD_MASK, &oct_gen) < OCT_OK) {
	errRaise(gu_pkg_name, GU_BAD_CL,
		 "Can't generate records off change list: %s",
		 octErrorString());
	/*NOTREACHED*/
    }

    mask = mark->change_list.contents.changeList.objectMask;
    while (octGenerate(&oct_gen, &change) == OCT_OK) {
	/* Create table if needed */
	cr_count++;
	if ( ch_rec_table == 0 ) {
	    ch_rec_table = st_init_table_with_params
	      (st_numcmp, st_numhash, TBL_SIZE, TBL_DENSITY, TBL_GROW_FACTOR, TBL_DEFAULT_REORDER_FLAG);
	}

	ch_obj.objectId = oct_null_id;
	if (!octIdIsNull(CR(objectExternalId))) {
	    (void) octGetByExternalId(&fct, CR(objectExternalId), &ch_obj);
	}
	sub_obj.objectId = oct_null_id;
	if (!octIdIsNull(CR(contentsExternalId))) {
	    (void) octGetByExternalId(&fct, CR(contentsExternalId), &sub_obj);
	}
	switch (CR(changeType)) {
	case OCT_ATTACH_CONTENT:
	    /* Insertion */
	    if (!octIdIsNull(ch_obj.objectId) &&
		 octIdsEqual(ch_obj.objectId, mark->parent_id) &&
		((octIdIsNull(sub_obj.objectId) || ((1 << sub_obj.type) & mask)))) {
		if (tbl_query(ch_rec_table, &(gen->blk), CR(contentsExternalId), &info)) {
		    switch (info->op) {
		    case GU_NOOP:
		    case GU_UPDATE:
			info->op = GU_ADD;
			break;
		    case GU_REMOVE:
			info->op = GU_UPDATE;
			break;
		    default:
			errRaise(gu_pkg_name, GU_BAD_OP,
				 "Invalid op for OCT_ATTACH_CONTENT record");
			/*NOTREACHED*/
		    }
		} else {
		    /* Not in table */
		    info->op = GU_ADD;
		    info->bad_parent = info->bad_type = 1;
		    info->invalid_record = 1;
		    info->record_id = change.objectId;
		    info->facet_id = fct.objectId;
		}
		/* Try to validate record if invalid */
		if (info->bad_parent) {
		    info->bad_parent = octIdIsNull(ch_obj.objectId) ||
		      !octIdsEqual(ch_obj.objectId, mark->parent_id);
		}
		if (info->bad_type) {
		    info->bad_type = octIdIsNull(sub_obj.objectId) ||
		      (((1 << sub_obj.type) & mask) == 0);
		}
	    } /* Ignore record if demonstrably bad */
	    break;
	case OCT_DETACH_CONTENT:
	    /* Deletion */
	    if (!octIdIsNull(ch_obj.objectId) && octIdsEqual(ch_obj.objectId, mark->parent_id) &&
		((octIdIsNull(sub_obj.objectId) || ((1 << sub_obj.type) & mask)))) {
		if (tbl_query(ch_rec_table, &(gen->blk), CR(contentsExternalId), &info)) {
		    switch (info->op) {
		    case GU_ADD:
			info->op = GU_NOOP;
			break;
		    case GU_NOOP:
		    case GU_UPDATE:
			info->op = GU_REMOVE;
			break;
		    default:
			errRaise(gu_pkg_name, GU_BAD_OP,
				 "Invalid op for OCT_DETACH_CONTENT");
			/*NOTREACHED*/
		    }
		} else {
		    /* Not in table */
		    info->op = GU_REMOVE;
		    info->bad_parent = info->bad_type = 1;
		    info->invalid_record = 1;
		    info->record_id = change.objectId;
		    info->facet_id = fct.objectId;
		}
		/* Try to validate record if invalid */
		if (info->bad_parent) {
		    info->bad_parent = octIdIsNull(ch_obj.objectId) ||
		      !octIdsEqual(ch_obj.objectId, mark->parent_id);
		}
		if (info->bad_type) {
		    info->bad_type = octIdIsNull(sub_obj.objectId) ||
		      (((1 << sub_obj.type) & mask) == 0);
		}
	    } /* Ignore record if either item is invalid */
	    break;
	case OCT_MODIFY:
	case OCT_PUT_POINTS:
	    /* Delete and insert */
	    if (!(!octIdIsNull(ch_obj.objectId) && (((1 << ch_obj.type) & mask) == 0))) {
		if (tbl_query(ch_rec_table, &(gen->blk), CR(objectExternalId), &info)) {
		    if (info->invalid_record) {
			info->record_id = change.objectId;
			info->invalid_record = 0;
		    }
		} else {
		    info->op = GU_UPDATE;
		    info->bad_parent = info->bad_type = 1;
		    info->invalid_record = 0;
		    info->record_id = change.objectId;
		    info->facet_id = fct.objectId;
		}
		if (info->bad_parent) {
		    info->bad_parent = octIdIsNull(ch_obj.objectId) ||
		      (octIsAttached(&parent, &ch_obj) != OCT_OK);
		}
		if (info->bad_type) {
		    info->bad_type =
		      (octIdIsNull(ch_obj.objectId) ||
		       (((1 << ch_obj.type) & mask) == 0)) &&
			 (((1 << deep_type(&change)) & mask) == 0);
		}
	    }
	    break;
	case OCT_DELETE:
	    if (tbl_query(ch_rec_table, &(gen->blk), CR(objectExternalId), &info)) {
		switch (info->op) {
		case GU_ADD:
		    info->op = GU_NOOP;
		    break;
		case GU_UPDATE:
		case GU_REMOVE:
		    info->op = GU_REMOVE;
		    /* INTENTIONAL FALL-THROUGH */
		case GU_NOOP:
		    if (info->invalid_record) {
			info->record_id = change.objectId;
			info->invalid_record = 0;
		    }
		    break;
		default:
		    errRaise(gu_pkg_name, GU_BAD_OP, "Invalid op for OCT_DELETE");
		    /*NOTREACHED*/
		}
	    } else {
		/* Not in table */
		info->op = GU_REMOVE;
		info->record_id = change.objectId;
		info->invalid_record = 0;
		info->bad_parent = info->bad_type = 1;
		info->facet_id = fct.objectId;
	    }
	    if (info->bad_type) {
		info->bad_type = (((1 << deep_type(&change)) & mask) == 0);
	    }
	    break;
	default:
	    errRaise(gu_pkg_name, GU_BAD_REC, "Bad change list record");
	    /*NOTREACHED*/
	}
    }
    /* printf( "There are %d change records in this list\n", cr_count ); */
    if ( ch_rec_table )  st_free_table(ch_rec_table); /* Free table if needed. */
    return (gu_gen) gen;
}



#define GEN ((gu_fullgen *) v_gen)

gu_change guNext(v_gen)
register gu_gen v_gen;	/* Generator state */
/*
 * This routine generates the next change in the sequence of
 * changes initialized by guStart().  An opaque change record
 * is returned by the function.  It can be examined by
 * using the functions guOp(), guXid(), guOldSize(),
 * guOldObject(), and guOldPoints().  When there are no
 * more changes,  the routine returns zero. The generator state
 * is not automatically reclaimed.  Thus,  the user should
 * call guEnd() after generating through the changes.
 */
{
    register gu_ch *item;
    struct blk_defn *old;

    if (GEN->blk == (struct blk_defn *) 0) return (gu_change) 0;
    while (GEN->current < LAST_BLOCK(GEN->blk)) {
	item = GEN->blk->pool + (GEN->current)++;
	if ((item->op != GU_NOOP) && (!item->bad_parent) && (!item->bad_type)) {
	    return (gu_change) item;
	}
    }
    old = GEN->blk;
    GEN->blk = GEN->blk->next;
    FREE(old);
    GEN->current = 0;
    return guNext(v_gen);
}

#undef GEN



void guEnd(v_gen)
gu_gen v_gen;
/*
 * Frees up all resources associated with the change generator `gen'.
 */
{
    gu_fullgen *gen = (gu_fullgen *) v_gen;
    struct blk_defn *old;

    while (gen->blk) {
	old = gen->blk;
	gen->blk = gen->blk->next;
	FREE(old);
    }
    FREE(gen);
}



#define CHANGE	((gu_ch *) v_change)

gu_op guOp(v_change)
gu_change v_change;
/*
 * Returns the type of `change'.
 */
{
    return CHANGE->op;
}

int32 guXid(v_change)
gu_change v_change;
/*
 * Returns the external id of the changed item.
 */
{
    return CHANGE->externalId;
}

octObject *guFacet(v_change)
gu_change v_change;
/*
 * Returns the facet of the change record.  If the facet has
 * ceased to exist for some reason,  the routine returns zero.
 * The object returned is static and should be copied if the
 * caller wishes to preserve its contents across multiple
 * calls to guFacet().
 */
{
    static octObject fct;

    fct.objectId = CHANGE->facet_id;
    if (octGetById(&fct) < OCT_OK) {
	return (octObject *) 0;
    } else {
	return &fct;
    }
}

struct octBox *guOldSize(v_change)
gu_change v_change;
/*
 * Returns the old size of the item.  This is a multi-stage
 * process.  First,  it tries to get the change record
 * associated with the change.  If successful, it obtains
 * that record and returns the bounding box of the *change record*
 * which should be the saved size of the item.  If this bounding
 * box does not exist,  zero is returned.  If there is no
 * associated change record,  this means the change has no
 * saved size.  In this case,  we are forced to assume the
 * old size is the current size of the object (if any).  We
 * return that size or zero if we can't find any size. The box itself
 * is static and should be copied if the caller wishes
 * to preserve it's contents.
 */
{
    static struct octBox bb;
    octObject obj;
    octObject *fct;

    if (!(CHANGE->invalid_record) && !octIdIsNull(CHANGE->record_id)) {
	obj.objectId = CHANGE->record_id;
	if (octGetById(&obj) == OCT_OK) {
	    if (octBB(&obj, &bb) == OCT_OK) {
		return &bb;
	    } else {
		return (struct octBox *) 0;
	    }
	} else {
	    errRaise(gu_pkg_name, GU_BAD_ID,
		     "Can't get change record by id: %s",
		     octErrorString());
	}
    } else {
	if ((fct = guFacet(v_change)) &&
	    (octGetByExternalId(fct, CHANGE->externalId, &obj) >= OCT_OK) &&
	    (octBB(&obj, &bb) >= OCT_OK)) {
	    return &bb;
	} else {
	    return (struct octBox *) 0;
	}
    }
    /*NOTREACHED*/
    return (struct octBox *) 0;
}

/*ARGSUSED*/
octObject *guOldObject(v_change)
gu_change v_change;
/*
 * Returns the old object associated with a record (if any).
 * Initially unimplemented.
 */
{
    errRaise(gu_pkg_name, GU_NOT_IMPL, "guOldObject() not implemented");
    /*NOTREACHED*/
    return (octObject*) 0;
}

/*ARGSUSED*/
int guOldPoints(v_change, pnts)
gu_change v_change;
struct octPoint **pnts;
/*
 * Returns the old points associated with an object (if any).
 * Initially unimplemented.
 */
{
    errRaise(gu_pkg_name, GU_NOT_IMPL, "guOldPoints() not implemented");
    /*NOTREACHED*/
    return 0;
}
