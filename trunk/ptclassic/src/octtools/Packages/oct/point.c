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
 * Implementations of the the oct functions for boxes.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "geo.h"
#include "io.h"

#include "io_procs.h"

#include "point.h"

static octStatus point_read_fields(), point_write_fields(), point_bb();

extern struct object_desc oct_geo_desc;
static struct object_desc *super = &oct_geo_desc;

void
oct_point_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct point);
    object_desc->external_size = sizeof(struct octPoint);
    object_desc->flags |= HAS_BB_FUNC;
    object_desc->contain_mask = (octObjectMask) OCT_PROP_MASK|OCT_BAG_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct point, user_point);
    object_desc->write_fields_func = point_write_fields;
    object_desc->read_fields_func = point_read_fields;
    object_desc->bb_func = point_bb;
}
  
static octStatus
point_bb(lptr, bbox)
generic *lptr;
struct octBox *bbox;
{
    struct point *point = (struct point *) lptr;

    if (bbox == NIL(struct octBox)) {
	return OCT_OK;
    }
    
    bbox->lowerLeft = point->user_point;
    bbox->upperRight = point->user_point;
    return OCT_OK;
}

static octStatus point_read_fields(ptr)
generic *ptr;
{
    struct point *point = (struct point *) ptr;
    
    return (*super->read_fields_func)(ptr) &&
      oct_get_point(&point->user_point);
}

static octStatus
point_write_fields(ptr)
generic *ptr;
{
    struct point *point = (struct point *) ptr;
    
    return (*super->write_fields_func)(ptr) &&
      oct_put_point(point->user_point);
}
