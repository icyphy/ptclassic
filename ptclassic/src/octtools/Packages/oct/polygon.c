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
 * Implementations of the the oct functions for polygons.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "geo.h"
#include "io.h"

#include "bb.h"
#include "oct_utils.h"

#include "polygon.h"

static octStatus polygon_bb();
static octStatus polygon_read_fields();

extern struct object_desc oct_geo_points_desc;
static struct object_desc *super = &oct_geo_points_desc;

void
oct_polygon_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct polygon);
    object_desc->external_size = 0;
    object_desc->flags |= HAS_BB_FIELD|HAS_BB_FUNC;
    object_desc->contain_mask = (octObjectMask) OCT_PROP_MASK|OCT_BAG_MASK;
    object_desc->user_offset = ILLEGAL_OFFSET;
    object_desc->bb_offset = FIELD_OFFSET(struct polygon, bbox);
    object_desc->bb_func = polygon_bb;
    object_desc->read_fields_func = polygon_read_fields;
}

static octStatus
polygon_bb(pptr, user_box)
generic *pptr;
struct octBox *user_box;
{
    struct octPoint *ptr;
    int32 i;
    struct polygon *poly = (struct polygon *) pptr;
    struct octBox *box = &poly->bbox;

    if (VALID_BB(&poly->bbox)) {
	if (user_box != NIL(struct octBox)) {
	    *user_box = poly->bbox;
	}
	return OCT_OK;
    }
	
    i = poly->num_points;

    if (i < 1) {
	oct_error("The polygon has no points, thus no bounding box");
	return OCT_NO_BB;
    }
    
    ptr = poly->points;
    box->lowerLeft = box->upperRight = *ptr;
    
    for(--i,++ptr; i > 0; i--,ptr++) {
	if (box->lowerLeft.x > ptr->x) {
	    box->lowerLeft.x = ptr->x;
	} else if (box->upperRight.x < ptr->x) {
	    box->upperRight.x = ptr->x;
	}
	if (box->lowerLeft.y > ptr->y) {
	    box->lowerLeft.y = ptr->y;
	} else if (box->upperRight.y < ptr->y) {
	    box->upperRight.y = ptr->y;
	}
    }
    if (user_box != NIL(struct octBox)) {
	*user_box = *box;
    }
    return OCT_OK;
}

static octStatus
polygon_read_fields(pptr)
generic *pptr;
{
    struct polygon *polygon = (struct polygon *) pptr;
    int retval;

    retval = (*super->read_fields_func)(pptr);

    if (retval) {
	MAKE_BB_INVALID(&polygon->bbox); /* force the bb computation */
	(void) oct_bb_get(pptr, NIL(struct octBox));
    }
    return retval;
}
