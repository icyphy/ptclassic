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
 * OCT Region Package
 *
 * David Harrison
 * University of California, 1985, 1988
 *
 * This package generates all OCT objects of a circuit view in a specified
 * region.  The new implementation of this package uses k-d trees
 * (with k=4) to speed region searches on spaces with large numbers
 * of objects.
 *
 * It is important to note this is an independent package which does not
 * depend on VEM in any way.  It should be compiled and linked into a
 * separate library so that others can use it if they so desire.
 */

#include "copyright.h"
#include "port.h"		/* Portability package           */
#include "oct.h"		/* OCT data management package   */
#include "gu.h"			/* Geometric changes package     */
#include "tr.h"			/* Transformation package        */
#include "region.h"		/* Self-declaration              */
#include "reg_internal.h"	/* Internal definitions          */
#include "kd.h"			/* K-d tree implementation       */
#include "st.h"			/* Hash table package            */
#include "errtrap.h"		/* Error handling package        */

/* For use by memory allocation macros */
char *_reg_mem;

/* General forward declarations */
static kd_tree make_kd_tree();	/* Creates new k-d tree         */
static void init_request();	/* Initialize region request    */
static void change_update();	/* Examines change list         */
static int reg_size();		/* Determines sizes of objects  */
static int aterm_size();	/* Size of actual terminal      */
static int fterm_size();	/* Size of formal terminal      */

/* Forward generator declarations */

static regStatus regKDGen();	/* K-d generator      */
#ifdef ID_TEST
static regStatus regKDIDGen();	/* Modified generator */
#endif
static void regKDEnd();		/* End k-d generator  */

static regStatus regLinGen();	/* Linear generator      */
static void regLinEnd();	/* End linear generation */



/*
 * Internally,  the new region package maintains a hash table of
 * region requests keyed by the parent object id and the generation
 * mask.  When a new request is encountered,  a count of objects
 * is made.  If the count is above KD_THRESHOLD,  a k-d tree is
 * allocated for the objects.  Otherwise,  it is left as a simple
 * linear search.
 *
 * Terminals are a special case.  Since the size of the terminal 
 * depends on the items attached to it,  it is not practical to
 * monitor the change list appropriately to keep a k-d representation
 * of the terminals up to date.  Thus,  all terminal requests are left
 * as linear searches.
 */

#define KD_THRESHOLD		kd_threshold
#define DEFAULT_THRESHOLD	30
static int kd_threshold = 	DEFAULT_THRESHOLD;

static st_table *reg_requests = (st_table *) 0;

typedef struct reg_key_defn {
    octId parent;		/* Id of parent object */
    octObjectMask mask;		/* Requested mask      */
} reg_key;

#define RC(val)		((reg_key *) (val))

static int reg_cmp(key1, key2)
register char *key1, *key2;
/*
 * This function is called by the hash table package to compare
 * the keys in the hash table.
 */
{
    return ((RC(key1)->parent == RC(key2)->parent) ?
	    RC(key1)->mask - RC(key2)->mask :
	    RC(key1)->parent - RC(key2)->parent);
}

static unsigned reg_hash(key, modulus)
register char *key;
register int modulus;
/*
 * Hashing function for a region query key.
 */
{
    return (RC(key)->parent ^ RC(key)->mask) % modulus;
}

typedef struct reg_val_defn {
    int obj_count;		/* Number of objects        */
    kd_tree tr;			/* Tree - if any            */
    gu_marker changes;		/* Changes since last query */
} reg_val;



/*
 * K-D generation is actuall a pointer to the following:
 */

typedef struct reg_kd {
    octObject *gen_facet;	/* Generator facet        */
    kd_gen gen_state;		/* Actual generator state */
} regKD;

/*
 * Linear generation is actually a pointer to the following:
 */

typedef struct reg_linear {
    octGenerator oct_gen;	/* Oct linear generator */
    struct octBox extent;	/* Extent of region     */
    int *req_count;		/* Request count        */
    int local_count;		/* Local object count   */
} regLinear;

/*
 * 'pointAry' is space used for getting path and polygon points for
 * the normal case.  If there are more than NORMPOINTS points,  special
 * case code reallocates the array to the new maximum plus the maximum
 * divided by 8 (see regNext).  The arrangement always makes sure there
 * is at least one extra point allocated at the end in case the polygon
 * must be closed.
 */

#define NORMPOINTS	10
static int pointSize = 0;
static struct octPoint *pointAry = NIL(struct octPoint);



void regInit(parentObj, someExtent, someMask, generator)
octObject *parentObj;		/* OCT object to generate objects from */
struct octBox *someExtent;	/* Rectangular region in OCT coords    */
octObjectMask someMask;		/* OCT Mask of objects to look for     */
regGenerator *generator;	/* Returned generator                  */
/*
 * This routine returns a generator handle which can be used to
 * retrieve the objects of type `someMask' that intersect `someExtent'.
 * The routine supports all geometric objects (OCT_INSTANCE, OCT_POLYGON,
 * OCT_BOX, OCT_CIRCLE, OCT_PATH, OCT_LABEL, OCT_POINT, and OCT_EDGE),
 * and one non-geometric object: OCT_TERM.  The size of a formal terminal
 * is defined as the union of the sizes of all geometry attached to
 * it.  As a special case,  if a terminal has an actual terminal attached
 * to it,  its size will be determined by the size of all items attached
 * to the interface formal terminal corresponding to the actual terminal
 * in the instance master.  The region specified should be relative
 * to the facet containing `parentObj'.
 */
{
    reg_key key;
    reg_val *val;
    regKD *newKD;
    regLinear *newLinear;
    kd_box region;

    REG_PRE("regInit");
    if (!reg_requests) {
	/* Make new hash table */
	reg_requests = st_init_table(reg_cmp, (int (*)()) reg_hash);
    }
    /* Look it up */
    key.parent = parentObj->objectId;
    key.mask = someMask;
    if (!st_lookup(reg_requests, (char *) &key, (char **) &val)) {
	init_request(parentObj, someMask, &val);
    }

    /* Generator depends on what kind of situation we have */
    if (val->tr) {
	/* K-d tree - see if it can be downgraded to linear */
	if (val->obj_count < KD_THRESHOLD) {
	    /* Downgrade to linear */
	    if (val->tr) {
		kd_destroy(val->tr, (void (*)()) 0);
		val->tr = (kd_tree) 0;
	    }
	    if (val->changes) {
		guUnmark(val->changes);
		val->changes = (gu_marker) 0;
	    }
	    regInit(parentObj, someExtent, someMask, generator);
	} else {
	    /* Start up generator */
	    change_update(parentObj, someMask, val);
	    newKD = REG_ALLOC(regKD);
	    newKD->gen_facet = REG_ALLOC(octObject);
	    octGetFacet(parentObj, newKD->gen_facet);
	    region[KD_LEFT] = someExtent->lowerLeft.x;
	    region[KD_BOTTOM] = someExtent->lowerLeft.y;
	    region[KD_RIGHT] = someExtent->upperRight.x;
	    region[KD_TOP] = someExtent->upperRight.y;
	    newKD->gen_state = kd_start(val->tr, region);
	    generator->state = (reg_generic) newKD;
#ifdef ID_TEST
	    if (someMask & OCT_INSTANCE_MASK) {
		generator->gen_func = regKDIDGen;
	    } else {
		generator->gen_func = regKDGen;
	    }
#else
	    generator->gen_func = regKDGen;
#endif
	    generator->end_func = regKDEnd;
	}
    } else {
	/* Linear - see if it can be upgraded to kd-tree */
	if ((val->obj_count >= KD_THRESHOLD) && !(someMask & OCT_TERM_MASK)) {
	    /* Upgrade to kd-tree */
	    val->tr = make_kd_tree(parentObj, someMask);
	    val->obj_count = kd_count(val->tr);
	    regInit(parentObj, someExtent, someMask, generator);
	} else {
	    /* Set up linear generation */
	    newLinear = REG_ALLOC(regLinear);
	    REG_OA(octInitGenContents(parentObj, someMask, &(newLinear->oct_gen)));
	    newLinear->extent = *someExtent;
	    newLinear->req_count = &(val->obj_count);
	    newLinear->local_count = 0;
	    generator->state = (reg_generic) newLinear;
	    generator->gen_func = regLinGen;
	    generator->end_func = regLinEnd;
	}
    }
    REG_POST;
    return;
}



#ifdef PRT
void prt_req(obj, mask)
octObject *obj;
octObjectMask mask;
/*
 * For debugging only
 */
{
    octObject fct;
    int type;

    octGetFacet(obj, &fct);
    printf("* %s:\n", vuDispObject(&fct));
    octGetById(obj);
    printf("  > %s\n", vuDispObject(obj));
    type = 0;
    printf("  >");
    while (mask) {
	if (mask & 0x01) {
	    switch (type) {
	    case OCT_FACET:
		printf(" OCT_FACET");
		break;
	    case OCT_TERM:
		printf(" OCT_TERM");
		break;
	    case OCT_NET:
		printf(" OCT_NET");
		break;
	    case OCT_INSTANCE:
		printf(" OCT_INSTANCE");
		break;
	    case OCT_POLYGON:
		printf(" OCT_POLYGON");
		break;
	    case OCT_BOX:
		printf(" OCT_BOX");
		break;
	    case OCT_CIRCLE:
		printf(" OCT_CIRCLE");
		break;
	    case OCT_PATH:
		printf(" OCT_PATH");
		break;
	    case OCT_LABEL:
		printf(" OCT_LABEL");
		break;
	    case OCT_PROP:
		printf(" OCT_PROP");
		break;
	    case OCT_BAG:
		printf(" OCT_BAG");
		break;
	    case OCT_LAYER:
		printf(" OCT_LAYER");
		break;
	    case OCT_POINT:
		printf(" OCT_POINT");
		break;
	    case OCT_EDGE:
		printf(" OCT_EDGE");
		break;
	    case OCT_FORMAL:
		printf(" OCT_FORMAL");
		break;
	    case OCT_MASTER:
		printf(" OCT_MASTER");
		break;
	    case OCT_CHANGE_LIST:
		printf(" OCT_CHANGE_LIST");
		break;
	    case OCT_CHANGE_RECORD:
		printf(" OCT_CHANGE_RECORD");
		break;
	    default:
		printf(" UNKNOWN_TYPE");
		break;
	    }
	}
	mask = mask >> 1;
	type += 1;
    }
    printf("\n");
}
#endif

static void init_request(obj, mask, val)
octObject *obj;			/* Parent object           */
octObjectMask mask;		/* Region query mask       */
reg_val **val;		/* Returned request record */
/*
 * This routine places a new request in the region package
 * request table.  A quick count of objects is made to
 * determine if it is necessary to build a k-d tree for
 * the request.  This will be used by regInit to build
 * a tree if there are a sufficient number of objects.
 */
{
    octObject thing;
    octGenerator gen;
    reg_key *new_entry;
    register int i;
    octStatus gen_ret = OCT_OK;

#ifdef PRT
prt_req(obj, mask);
#endif
    REG_OA(octInitGenContents(obj, mask, &gen));
    for (i = 0;  i <= KD_THRESHOLD;  i++) {
	if ((gen_ret = octGenerate(&gen, &thing)) != OCT_OK) break;
    }
    if (gen_ret < OCT_OK) {
	(void) reg_fault(REG_OCT, "error generating items");
	/* NOTREACHED */
    } else if (gen_ret == OCT_OK) {
	REG_OA(octFreeGenerator(&gen));
    }
    *val = REG_ALLOC(reg_val);
    (*val)->obj_count = i;
    (*val)->tr = (kd_tree) 0;
    (*val)->changes = (gu_marker) 0;
    new_entry = REG_ALLOC(reg_key);
    new_entry->parent = obj->objectId;
    new_entry->mask = mask;
    (void) st_insert(reg_requests, (char *) new_entry, (char *) *val);
}


/*
 * Change list operations
 *
 * The following routines handle change lists for the region package.
 * Things that change are automatically inserted or deleted from
 * the k-d tree.  Note that terminals are not supported.
 */

#define TO_KD_BOX(size, box) \
  (size[KD_LEFT] = box->lowerLeft.x,	\
   size[KD_BOTTOM] = box->lowerLeft.y,	\
   size[KD_RIGHT] = box->upperRight.x,	\
   size[KD_TOP] = box->upperRight.y,	\
   size)

static void change_apply(tree, change)
kd_tree tree;			/* Geometric data structure  */
gu_change change;		/* Generated database change */
/*
 * Updates `tree' to reflect the change specified by `change'.
 * This might involve adding an object to the tree,  changing
 * its position, or removing it from the tree.  Lots of checking
 * here because things are delicate.
 */
{
    octObject obj;
    register struct octBox *old_bb_p, *new_bb_p;
    struct octBox new_bb;
    int32 xid;
    kd_box size;

    obj.objectId = oct_null_id;
    old_bb_p = guOldSize(change);
    xid = guXid(change);
    new_bb_p = (struct octBox *) 0;
    if (octGetByExternalId(guFacet(change), xid, &obj) == OCT_OK) {
	if (octBB(&obj, &new_bb) == OCT_OK) {
	    new_bb_p = &new_bb;
	} else {
	    (void) reg_fault(REG_BDB, "Invalid bounding box");
	    /*NOTREACHED*/
	}
    }
    switch (guOp(change)) {
    case GU_ADD:
	if (new_bb_p) {
	    kd_insert(tree, (kd_generic) xid, TO_KD_BOX(size, new_bb_p));
	} else {
	    (void) reg_fault(REG_BDB, "Invalid bounding box on insert");
	}
	break;
    case GU_UPDATE:
	if (old_bb_p) {
	    if (kd_delete(tree, (kd_generic) xid,
			  TO_KD_BOX(size, old_bb_p)) != KD_OK) {
		(void) reg_fault(REG_KDE, "Can't delete updated item");
		/*NOTREACHED*/
	    }
	} else {
	    (void) reg_fault(REG_BDB, "Invalid old bounding box on update");
	}
	if (new_bb_p) {
	    kd_insert(tree, (kd_generic) xid, TO_KD_BOX(size, new_bb_p));
	} else {
	    (void) reg_fault(REG_BDB, "Invalid new bounding box on update");
	}
	break;
    case GU_REMOVE:
	if (old_bb_p) {
	    if (kd_delete(tree, (kd_generic) xid,
			  TO_KD_BOX(size, old_bb_p)) != KD_OK) {
		(void) reg_fault(REG_KDE, "Can't delete updated item");
		/*NOTREACHED*/
	    }
	} else {
	    (void) reg_fault(REG_BDB, "Invalid bounding box on delete");
	    /*NOTREACHED*/
	}
	break;
    default:
	(void) reg_fault(REG_BDB, "Bad operator to change apply");
	/*NOTREACHED*/
    }
}



static void change_update(parent, mask, val)
octObject *parent;		/* Parent object         */
octObjectMask mask;		/* Interesting objects   */
reg_val *val;			/* Region request record */
/*
 * This routine examines the changes associated with the change
 * list in `val' and updates the k-d tree in `val' accordingly.
 * If there are changes,  a new change list replaces the old
 * so that new changes can be incorporated next time the routine
 * is called.
 */
{
    gu_gen gen;
    gu_change change;
    int count;

    if (val->changes) {
	/* Examine list */
	count = 0;
	gen = guStart(val->changes);
	while ( (change = guNext(gen)) ) {
	    change_apply(val->tr, change);
	    count++;
	}
	guEnd(gen);
	if (count > 0) {
	    guUnmark(val->changes);
	    val->changes = guMark(parent, mask);
	}
    } else {
	val->changes = guMark(parent, mask);
    }
}



/* 
 * Size determination functions
 */

static int reg_size(obj, size)
register octObject *obj;	/* Object to examine */
register kd_box size;		/* Returned size     */
/*
 * Returns the size of `obj' in `size'.  Normally,  this
 * is a straight oct bounding box call.  However,  if it
 * is a terminal,  the bounding box is determined by
 * examining the objects attached to the terminal.
 */
{
    static struct octBox bb;
    register flag;

    if (!octIdIsNull(obj->objectId)) {
	if (obj->type != OCT_TERM) {
	    /* Normal object */
	    flag = (octBB(obj, &bb) == OCT_OK);
	} else if (!octIdIsNull(obj->contents.term.instanceId)) {
	    /* Actual Terminal */
	    flag = aterm_size(obj, &bb);
	} else {
	    /* Formal Terminal */
	    flag = fterm_size(obj, &bb);
	}
	if (flag) {
	    size[0] = bb.lowerLeft.x;
	    size[1] = bb.lowerLeft.y;
	    size[2] = bb.upperRight.x;
	    size[3] = bb.upperRight.y;
	}
    } else {
	flag = 0;
    }
    return flag;
}

static int aterm_size(obj, size)
register octObject *obj;	/* Object to examine */
register struct octBox *size;	/* Returned size     */
/*
 * This routine finds the size of an actual terminal by determining
 * the size of the interface formal terminal and transforming it
 * appropriately.
 */
{
    struct octObject inst, master, formal;
    register octCoord temp;

    /* Go to instance */
    inst.objectId = obj->contents.term.instanceId;
    if (octGetById(&inst) < OCT_OK) return 0;

    /* Jump to interface master */
    master.type = OCT_FACET;
    master.contents.facet.facet = "interface";
    master.contents.facet.mode = "r";
    if (octOpenMaster(&inst, &master) < OCT_OK) return 0;

    /* Find corresponding terminal */
#ifdef NO_OCT_EXTERNAL_ID_BUG
    if (octGetByExternalId(&master, obj->contents.term.formalExternalId,
			   &formal) < OCT_OK) return 0;
#else
    formal.type = OCT_TERM;
    formal.contents.term.name = obj->contents.term.name;
    if (octGetByName(&master, &formal) != OCT_OK) return 0;
#endif

    /* Get size of resulting formal terminal */
    if (fterm_size(&formal, size)) {
	/* Inverse transform it */
	tr_oct_transform(&(inst.contents.instance.transform),
			 &(size->lowerLeft.x), &(size->lowerLeft.y));
	tr_oct_transform(&(inst.contents.instance.transform),
			 &(size->upperRight.x), &(size->upperRight.y));
	/* Normalize it */
	if (size->lowerLeft.x > size->upperRight.x) {
	    temp = size->lowerLeft.x;
	    size->lowerLeft.x = size->upperRight.x;
	    size->upperRight.x = temp;
	}
	if (size->lowerLeft.y > size->upperRight.y) {
	    temp = size->lowerLeft.y;
	    size->lowerLeft.y = size->upperRight.y;
	    size->upperRight.y = temp;
	}
	return 1;
    } else {
	return 0;
    }
}


static int fterm_size(obj, bb)
register octObject *obj;	/* Object to examine */
register struct octBox *bb;	/* Returned size     */
/*
 * This routine returns the size of a formal terminal (relative
 * to the coordinate system of the facet of the formal terminal).
 * This size is defined as the union of all of the sizes of
 * objects attached to the terminal.
 */
{
    octGenerator gen;
    octObject sub_object;
    struct octBox sub_bb;
    register int flag, count;

    bb->lowerLeft.x = REGMAXINT;
    bb->lowerLeft.y = REGMAXINT;
    bb->upperRight.x = REGMININT;
    bb->upperRight.y = REGMININT;
    if (octInitGenContents(obj, OCT_ALL_MASK, &gen) < OCT_OK) return 0;
    count = 0;
    while (octGenerate(&gen, &sub_object) == OCT_OK) {
	if (sub_object.type != OCT_TERM) {
	    flag = (octBB(&sub_object, &sub_bb) == OCT_OK);
	} else if (octIdIsNull(sub_object.contents.term.instanceId)) {
	    flag = fterm_size(&sub_object, &sub_bb);
	} else {
	    flag = aterm_size(&sub_object, &sub_bb);
	}
	if (flag) {
	    if (sub_bb.lowerLeft.x < bb->lowerLeft.x)
	      bb->lowerLeft.x = sub_bb.lowerLeft.x;
	    if (sub_bb.lowerLeft.y < bb->lowerLeft.y)
	      bb->lowerLeft.y = sub_bb.lowerLeft.y;
	    if (sub_bb.upperRight.x > bb->upperRight.x)
	      bb->upperRight.x = sub_bb.upperRight.x;
	    if (sub_bb.upperRight.y > bb->upperRight.y)
	      bb->upperRight.y = sub_bb.upperRight.y;
	    count++;
	}
    }
    return count;
}


static int reg_genitem(arg, val, size)
kd_generic arg;			/* Generator state */
kd_generic *val;		/* Returned item   */
kd_box size;			/* Returned size   */
/*
 * This routine is passed to kd_build to generate all
 * of the items that are to be added to the k-d tree
 * representation.
 */
{
    octObject obj;

    if (octGenerate((octGenerator *) arg, &obj) == OCT_OK) {
	if (reg_size(&obj, size)) {
	    octExternalId(&obj, (int32 *) val);
	} else {
	    (void) reg_fault(REG_OCT, "Can't get size of generated object");
	}
    } else {
	return 0;
    }
    return 1;
}

static kd_tree make_kd_tree(obj, mask)
octObject *obj;			/* Parent object   */
octObjectMask mask;		/* Mask of objects */
/*
 * This routine returns a newly constructed k-d tree that
 * contains all objects under `obj' whose type is in `mask'.
 */
{
    octGenerator gen;

    REG_OA(octInitGenContents(obj, mask, &gen));
    return kd_build(reg_genitem, (kd_generic) &gen);
}


void regGetPoints(obj, n, points)
struct octObject *obj;		/* Path or polygon object    */
int *n;				/* Number of points          */
struct octPoint **points;	/* Points of polygon or path */
/*
 * This routine gets the points associated with a path or polygon
 * object.  The returned data is owned by this routine and should
 * not be freed.  The routine guarantees that the returned area
 * is at least n+1 spaces long in case the polygon must be closed.
 * It is a precondition that the object is a path
 * or polgon and the routine does not check to make sure this is true.
 * The routine returns a non-zero value if there were no errors.
 */
{
    int32 numPoints;

    REG_PRE("regGetPoints");
    /* Look at points array and allocate to minimum size (if needed) */

    if (pointSize == 0) {
	pointSize = NORMPOINTS;
	pointAry = REG_ARYALLOC(struct octPoint, pointSize);
    }

    numPoints = pointSize-1;
    switch (octGetPoints(obj, &numPoints, pointAry)) {
    case OCT_OK:
	break;
    case OCT_TOO_SMALL:
	if (pointSize > 0) {
	    /* 
	     * The new array size is the returned size plus
	     * 12.5% (numPoints/8).
	     */
	    REG_FREE(pointAry);
	}
	pointSize = numPoints + (numPoints >> 3);
	pointAry = REG_ARYALLOC(struct octPoint, pointSize);
	numPoints = pointSize;
	if (octGetPoints(obj, &numPoints, pointAry) != OCT_OK) {
	    (void) reg_fault(REG_OCT, "can't get object points");
	    /* NOTREACHED */
	}
	break;
    default:
	(void) reg_fault(REG_OCT, "can't get object points");
    }
    *n = numPoints;
    *points = pointAry;
    REG_POST;
}


#define KDGEN(gen)	((regKD *) ((gen)->state))

static regStatus regKDGen(gen, object, np, pnts, size)
regGenerator *gen;		/* Generator       */
octObject *object;		/* Returned object */
int *np;			/* Returned number of points */
struct octPoint **pnts;		/* Returned points           */
register struct octBox *size;	/* Returned size             */
/*
 * This routine generates the next item using the k-d tree generator.
 * If the object is a path or polygon,  the number of points and
 * the points themselves will be returned in `np' and `pnts' (if non-zero).
 * Also,  if `size' is non-zero,  the bounding box of the item will
 * be returned.  If there are no more items,  the routine will return
 * REG_NOMORE.  I don't push an error handler here for efficiency sake.
 */
{
    static kd_box box;
    int32 xid;

    if (size) {
	if (kd_next(KDGEN(gen)->gen_state , (kd_generic *) &xid,
		    box) == KD_OK) {
	    size->lowerLeft.x = (octCoord) box[0];
	    size->lowerLeft.y = (octCoord) box[1];
	    size->upperRight.x = (octCoord) box[2];
	    size->upperRight.y = (octCoord) box[3];
	} else {
	    return REG_NOMORE;
	}
    } else if (kd_next(KDGEN(gen)->gen_state, (kd_generic *) &xid,
		       (int *) 0) != KD_OK) {
	return REG_NOMORE;
    }
    if (octGetByExternalId(KDGEN(gen)->gen_facet, xid, object) != OCT_OK) {
	return reg_error(REG_FAIL, "Unable to retrieve object:\n  %s\n",
			 octErrorString());
    }
    if (((object->type == OCT_PATH) || (object->type == OCT_POLYGON)) &&
	(np != (int *) 0) && (pnts != (struct octPoint **) 0)) {
	regGetPoints(object, np, pnts);
    }
    return REG_OK;
}

#ifdef ID_TEST


static regStatus regKDIDGen(gen, object, np, pnts, size)
regGenerator *gen;		/* Generator       */
octObject *object;		/* Returned object */
int *np;			/* Returned number of points */
struct octPoint **pnts;		/* Returned points           */
register struct octBox *size;	/* Returned size             */
/*
 * This is a special purpose version of regKDGen that just fills
 * in the object id.  It is a test to see if just generating
 * ids for instances is a better solution.
 */
{
    static kd_box box;

    if (size) {
	if (kd_next((kd_gen) gen->state, (kd_generic *) &(object->objectId),
		    box) == KD_OK) {
	    size->lowerLeft.x = (octCoord) box[0];
	    size->lowerLeft.y = (octCoord) box[1];
	    size->upperRight.x = (octCoord) box[2];
	    size->upperRight.y = (octCoord) box[3];
	} else {
	    return REG_NOMORE;
	}
    } else if (kd_next((kd_gen) gen->state, (kd_generic *) &(object->objectId),
		       0) != KD_OK) {
	return REG_NOMORE;
    }
    return REG_OK;
}
#endif

static void regKDEnd(gen)
regGenerator *gen;		/* Generator */
/*
 * This routine cleans up the k-d generator.  Basically,  this
 * calls kd_finish with the state of the generator.
 */
{
    REG_PRE("regKDEnd");
    REG_FREE(KDGEN(gen)->gen_facet);
    kd_finish(KDGEN(gen)->gen_state);
    REG_FREE(gen->state);
    REG_POST;
}



static regStatus regLinGen(gen, object, np, pnts, size)
regGenerator *gen;		/* Generator       */
register octObject *object;	/* Returned object */
int *np;			/* Returned number of points */
struct octPoint **pnts;		/* Returned points           */
register struct octBox *size;	/* Returned size             */
/*
 * This routine generates the next item in a region search using
 * linear lookup.  The next object in the search is generated and
 * its size is found using reg_size().  The size is compared against
 * the supplied region and it is returned if it intersects it.
 * If the object is a polygon or path and `np' and `pnts' are non-zero,
 * the routine will return the number of points and the points themselves
 * in those slots.  If `size' is non-zero,  the bounding box of the
 * item is returned.  Again,  no error handler is pushed for efficiency
 * sake.
 */
{
    register regLinear *current = (regLinear *) gen->state;
    register octStatus stat;
    kd_box rtn_size;

    for (;;) {
	stat = octGenerate(&(current->oct_gen), object);
	if (stat == OCT_OK) {
	    current->local_count += 1;
	    if (reg_size(object, rtn_size)) {
		if (REGBOXKDINTR(current->extent, rtn_size)) {
		    if (size) {
			size->lowerLeft.x = rtn_size[KD_LEFT];
			size->lowerLeft.y = rtn_size[KD_BOTTOM];
			size->upperRight.x = rtn_size[KD_RIGHT];
			size->upperRight.y = rtn_size[KD_TOP];
		    }
		    if (((object->type == OCT_PATH) ||
			 (object->type == OCT_POLYGON)) &&
			(np != (int *) 0) && (pnts != (struct octPoint **) 0)) {
			regGetPoints(object, np, pnts);
		    }
		    return REG_OK;
		}
	    }
	} else if (stat == OCT_GEN_DONE) {
	    *(current->req_count) = current->local_count;
	    return REG_NOMORE;
	} else {
	    return REG_FAIL;
	}
    }
}



static void regLinEnd(gen)
regGenerator *gen;		/* Generator */
/*
 * This routine cleans up the linear generation of items.
 * The Oct generator is explicitly freed if necessary.
 */
{
    register regLinear *current = (regLinear *) gen->state;
    octObject obj;

    REG_PRE("regLinEnd");
    if (octGenerate(&(current->oct_gen), &obj) == OCT_OK) {
	(void) octFreeGenerator(&(current->oct_gen));
    }
    REG_FREE(current);
    REG_POST;
}

