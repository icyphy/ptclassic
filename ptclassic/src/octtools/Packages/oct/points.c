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
#include "geo.h"

#include "bb.h"
#include "change_record.h"
#include "oct_utils.h"

octStatus
octGetPoints(object, num_points, points)
octObject *object;
int32 *num_points;
struct octPoint *points;
{
    generic *ptr;
    struct geo_points *gp;
    int i;
    int32 flags, num_pts;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error("octGetPoints: The object's id has been corrupted");
	return OCT_CORRUPTED_OBJECT;
	/* errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString()); */
    }

    flags = oct_object_descs[ptr->flags.type].flags;
    if (!(flags&HAS_POINTS)) {
	oct_error("octGetPoints: You can not get points from a %s",
		  oct_type_names[ptr->flags.type]);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    gp = (struct geo_points *) ptr;

    num_pts = gp->num_points;

    if (points != NIL(struct octPoint)) {
	struct octPoint *pt_ptr = gp->points;
	if (*num_points < num_pts) {
	    *num_points = num_pts;
	    oct_error(
		   "octGetPoints: the array is too small to hold the points");
	    return OCT_TOO_SMALL;
	}
	for (i = num_pts; i > 0; i--) {
	    *(points++) = *(pt_ptr++);
	}
    }
    *num_points = num_pts;
    
    return OCT_OK;
}

octStatus
octPutPoints(object, num_points, points)
octObject *object;
int32 num_points;
struct octPoint *points;
{
    generic *ptr;
    struct geo_points *gp;
    struct octBox old_box, new_box;
    struct object_desc *object_desc;
    int32 flags, old_num_points;
    int i, record;
    struct octPoint *pt_ptr, *head, *old_points;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error("octPutPoints: The object's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    object_desc = &oct_object_descs[ptr->flags.type];
    flags = object_desc->flags;

    if (!flags&HAS_POINTS) {
	oct_error("octPutPoints: You can not put points on a %s",
		  oct_type_names[ptr->flags.type]);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    record = NEED_TO_RECORD(ptr, OCT_PUT_POINTS);
    
    gp = (struct geo_points *) ptr;
    
    if (num_points < 0) {
	oct_error("octPutPoints: Negative number of points not allowed");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    old_num_points = gp->num_points;
    old_points = gp->points;
   
    if (old_num_points != 0) {
	(void) oct_bb_get(ptr, &old_box);
    }
    
    if (points == NIL(struct octPoint)) {
	oct_error("octPutPoints: The point array is null");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    pt_ptr = head = ALLOC(struct octPoint, num_points);

    for (i = num_points; i > 0; i--) {
	*(pt_ptr++) = *(points++);
    }

    gp->points = head;
    gp->num_points = num_points;

        /* Force a recomputation the bounding box */
    MAKE_BB_INVALID(&gp->bbox);
    (*object_desc->bb_func)(ptr, &new_box);
    
    if (old_num_points != 0) {
	if (num_points != 0) {
	    (void) oct_bb_modify(ptr, &old_box, &new_box);
	} else {
	    (void) oct_bb_delete(ptr->facet, &old_box);
	}
    } else if (num_points != 0) {
	(void) oct_bb_add(ptr->facet, &new_box);
    }
    
    if (record) {
	if (old_num_points != 0) {
	    return oct_record_change(ptr, ptr->flags.type, OCT_PUT_POINTS,
				     oct_null_xid, NIL(octObject),
				     old_num_points, old_points, &old_box);
	} else {
	    return oct_record_change(ptr, ptr->flags.type, OCT_PUT_POINTS,
				     oct_null_xid, NIL(octObject),
				     old_num_points, old_points, NIL(struct octBox));
	}
    } else if (old_num_points > 0) {
	FREE(old_points);
    }
    return OCT_OK;
}

