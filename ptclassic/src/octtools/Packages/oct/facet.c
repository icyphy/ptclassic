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
#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "attach.h"
#include "bb.h"
#include "master.h"
#include "oct_utils.h"

#include "facet.h"

extern struct object_desc oct_default_desc;
static struct object_desc *super = &oct_default_desc;

static octStatus facet_detach();
static octStatus facet_unattach();
static octStatus facet_bb();

void
oct_facet_desc_set(object_desc)
struct object_desc *object_desc;
{

    *object_desc = *super;

    object_desc->internal_size = sizeof(struct facet);
    object_desc->external_size = sizeof(struct octFacet);
    object_desc->flags = HAS_BB_FUNC;
    object_desc->contain_mask = (octObjectMask) (OCT_ALL_MASK & ~OCT_FACET_MASK);
    object_desc->user_offset = FIELD_OFFSET(struct facet, user_facet);
    object_desc->bb_offset = ILLEGAL_OFFSET;
    object_desc->name_offset = ILLEGAL_OFFSET;
    object_desc->create_func = oct_illegal_op;
    object_desc->undelete_func = oct_illegal_op;
    object_desc->delete_func = oct_illegal_op;
    object_desc->detach_func = facet_detach;
    object_desc->unattach_func = facet_unattach;
    object_desc->modify_func = oct_illegal_op;
    object_desc->bb_func = facet_bb;
    object_desc->gen_containers_func = oct_illegal_op;
    object_desc->read_func = oct_illegal_op;
    object_desc->write_func = oct_illegal_op;

    object_desc->free_func = oct_illegal_op;
    object_desc->free_fields_func = oct_illegal_op;
}

#define ADD_BOX(bbox, new) {						      \
    if ((bbox)->lowerLeft.x > (new)->lowerLeft.x) {			      \
	(bbox)->lowerLeft.x = (new)->lowerLeft.x;			      \
    }									      \
    if ((bbox)->lowerLeft.y > (new)->lowerLeft.y) {			      \
	(bbox)->lowerLeft.y = (new)->lowerLeft.y;			      \
    }									      \
    if ((bbox)->upperRight.x < (new)->upperRight.x) {			      \
	(bbox)->upperRight.x = (new)->upperRight.x;			      \
    }									      \
    if ((bbox)->upperRight.y < (new)->upperRight.y) {			      \
	(bbox)->upperRight.y = (new)->upperRight.y;			      \
    }									      \
}									      \

/* 
 * Get the bounding box of the facet, recomputing if necessary.  This 
 * is kept partially upto date incrementally.  See bb.c and master.c 
 * for further details
 */
static octStatus
facet_bb(ptr, box)
generic *ptr;
struct octBox *box;
{
    struct facet *facet = (struct facet *) ptr;
    struct octBox temp_box, bbox;
    generic *optr;
    int i, first = 1;
    
    if (facet->facet_flags.bb_valid) {
	if (facet->facet_flags.bb_invalid_instances) {
	    /* 
	     * The master of some instance in the facet has an invalid 
	     * bounding box, thus rendering the bounding box of the 
	     * instance unknown.  Find the invalid master and force a 
	     * re-evaluation so we can get an accurate bounding box.
	     */
	    int retval = oct_validate_all_instance_bbs(facet);
	    if (retval != OCT_OK) {
		return retval;
	    }
	    facet->facet_flags.bb_invalid_instances = 0;
	    /* 
	     * The instance validation may have reset the bb_valid 
	     * flag.  The easiest way to recheck is a recursive call
	     */
	    return facet_bb(ptr, box);
	}
	if (box != NIL(struct octBox)) {
	    *box = facet->bbox;
	}
	return OCT_OK;
    } else if (!facet->facet_flags.bb_init) {
	oct_error(
	    "The facet does not have any geometry to define a bounding box");
        return OCT_NO_BB;
    }

    /* The bounding box is invalid, recompute it */
    
    facet->facet_flags.bb_init = facet->facet_flags.bb_valid =
      facet->facet_flags.bb_invalid_instances = 0;

    for (i = 0; i <= OCT_MAX_TYPE; i++) {
	if (IN_MASK(i,OCT_GEO_MASK|OCT_INSTANCE_MASK)) {
	    optr = facet->object_lists[i];
	    while (optr != NIL(generic)) {
		(void) oct_bb_get(optr, &temp_box);
		if (VALID_BB(&temp_box)) {
		    if (first) {
			bbox = temp_box;
			first = 0;
		    } else {
			ADD_BOX(&bbox, &temp_box);
		    }
		}
		optr = optr->last;
	    }
	}
    }
    
    if (first) {
	oct_error(
	    "The facet does not have any geometry to define a bounding box");
        return OCT_NO_BB;
    }

    if (box != NIL(struct octBox)) {
	*box = bbox;
    }

    /* let oct_bb_add do all the other work, such as instance bb update */
    
    return oct_bb_add(facet, &bbox);
}


static octStatus
facet_detach(cptr, optr)
generic *cptr;
generic *optr;
{
    if (optr->flags.type == OCT_FORMAL) {
	oct_error("facet_detach: formal terminals may not be detached from the facet");
	errRaise(OCT_PKG_NAME, OCT_ILL_OP, octErrorString());
    }
    return oct_detach_default(cptr, optr);
}

static octStatus
facet_unattach(cptr, optr)
generic *cptr;
generic *optr;
{
    return oct_detach_default(cptr, optr);
}
