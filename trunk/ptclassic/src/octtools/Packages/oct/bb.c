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
#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "bb.h"
#include "master.h"
#include "oct_utils.h"

/* 
 * bb.c : implements the various functions that keep the facet 
 * bounding box up to date, as well as the functions to access the 
 * bounding box of the various objects that have them.  The facet bb 
 * is kept upto date in a lazy fashion: when it is invalidated because 
 * of the deletion of an object on the perimeter of the bounding box, 
 * the bounding box is not immediately recomputed.  Instead, it is 
 * recomputed only when the bb is actually needed. This avoids an N^2 
 * recomputation cost when deleting or modifying a lot of objects.  
 * This makes keeping the instance bounding boxes upto date more 
 * difficult. See master.c for more details
 */
  
octStatus
octBB(object, box)
octObject *object;
struct octBox *box;
{
    generic *optr;
    int retval;

    optr = oct_id_to_ptr(object->objectId);
    if (optr == NIL(generic)) {
	oct_error("octBB: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_bb_get(optr, box);

    if (retval < OCT_OK && retval != OCT_NO_BB) {
	oct_prepend_error("octBB: ");
    }
    return retval;
}

octStatus oct_bb_get(object, box)
generic *object;
struct octBox *box;
{
    struct object_desc *object_desc = &oct_object_descs[object->flags.type];

    return (*object_desc->bb_func)(object, box);
}

octStatus oct_bb_default(object, box)
generic *object;
struct octBox *box;
{
    struct object_desc *object_desc = &oct_object_descs[object->flags.type];

    if (object_desc->flags&HAS_BB_FIELD) {
	*box = *(struct octBox *) ((char *)object + object_desc->bb_offset);
	if (!VALID_BB(box)) {
	    oct_error(
	       "The object does not currently have a valid bounding box");
	    return OCT_NO_BB;
	} else {
	    return OCT_OK;
	}
    } else {
	/* object does not have a bb */
	return OCT_NO_BB;
    }
}

/*
 * This is messy, it is basically a combination of the the delete and 
 * add code, with the additional optimization of only forcing a facet 
 * bounding box recalculation if the old bb touched the facet 
 * perimeter AND the new bb doesn't.
 */
octStatus
oct_bb_modify(object, old, new)
generic *object;
struct octBox *old, *new;
{
    
    struct facet *desc = object->facet;
    struct octBox *bbox = &desc->bbox;
    int bb_changed = 0;
    
    if (desc->facet_flags.bb_valid) {
	if (VALID_BB(old)) {
	    if (!VALID_BB(new)) {
		return oct_bb_delete(desc, old);
	    } else if ((bbox->lowerLeft.x == old->lowerLeft.x &&
		     old->lowerLeft.x < new->lowerLeft.x) ||
		    (bbox->lowerLeft.y == old->lowerLeft.y &&
		     old->lowerLeft.y < new->lowerLeft.y) ||
		    (bbox->upperRight.x == old->upperRight.x &&
		     old->upperRight.x > new->upperRight.x) ||
		    (bbox->upperRight.y == old->upperRight.y &&
		     old->upperRight.y > new->upperRight.y)) {
		 desc->facet_flags.bb_valid = 0;
		 return oct_invalidate_all_instance_bbs(desc);
	    } else {
		if (bbox->lowerLeft.x > new->lowerLeft.x) {
		    bbox->lowerLeft.x = new->lowerLeft.x;
		    bb_changed = 1;
		}
		if (bbox->lowerLeft.y > new->lowerLeft.y) {
		    bbox->lowerLeft.y = new->lowerLeft.y;
		    bb_changed = 1;
		}
		if (bbox->upperRight.x < new->upperRight.x) {
		    bbox->upperRight.x = new->upperRight.x;
		    bb_changed = 1;
		}
		if (bbox->upperRight.y < new->upperRight.y) {
		    bbox->upperRight.y = new->upperRight.y;
		    bb_changed = 1;
		}
		if (bb_changed) {
		    desc->bb_date = desc->time_stamp;
		    return oct_update_all_instance_bbs(desc);
		} 
	    }
	} else if (VALID_BB(new)) {
	    return oct_bb_add(desc, new);
	}
    }

    return OCT_OK;
}

/* 
 * If the deleted objects bounding box touches the bounding box of the 
 * facet, then the entire facet must be marked as invalid.  (the 
 * deleted object might have been the only thing keeping the facet bb
 * that large)
 */

octStatus
oct_bb_delete(desc, old)
struct facet *desc;
struct octBox *old;
{
    struct octBox *bbox = &desc->bbox;

    if (desc->facet_flags.bb_valid && VALID_BB(old)) {
	if ((bbox->lowerLeft.x == old->lowerLeft.x) ||
	    (bbox->lowerLeft.y == old->lowerLeft.y) ||
	    (bbox->upperRight.x == old->upperRight.x) ||
	    (bbox->upperRight.y == old->upperRight.y)) {
		desc->facet_flags.bb_valid = 0;
		return oct_invalidate_all_instance_bbs(desc);
	    }
    }
    return OCT_OK;
}

octStatus
oct_bb_add(desc, box)
struct facet *desc;
struct octBox *box;
{
    struct octBox *bbox = &desc->bbox;
    int bb_changed = 0;

    if (!VALID_BB(box)) return OCT_OK;
    
    if (!desc->facet_flags.bb_init) {
	*bbox = *box;
	desc->facet_flags.bb_init = desc->facet_flags.bb_valid = 1;
	bb_changed = 1;
    } else if (desc->facet_flags.bb_valid) {
	if (bbox->lowerLeft.x > box->lowerLeft.x) {
	    bbox->lowerLeft.x = box->lowerLeft.x;
	    bb_changed = 1;
	}
	if (bbox->lowerLeft.y > box->lowerLeft.y) {
	    bbox->lowerLeft.y = box->lowerLeft.y;
	    bb_changed = 1;
	}
	if (bbox->upperRight.x < box->upperRight.x) {
	    bbox->upperRight.x = box->upperRight.x;
	    bb_changed = 1;
	}
	if (bbox->upperRight.y < box->upperRight.y) {
	    bbox->upperRight.y = box->upperRight.y;
	    bb_changed = 1;
	}
    }
    
    if (bb_changed) {
	desc->bb_date = desc->time_stamp;
	return oct_update_all_instance_bbs(desc);
    }
    
    return OCT_OK;
}

