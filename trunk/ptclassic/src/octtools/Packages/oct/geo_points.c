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
#include "geo.h"
#include "geo_points.h"
#include "bb.h"
#include "change_record.h"
#include "io_procs.h"

  /* 
   * This implements shared operations for objects that have points.
   * This is initializing the points fields, reading and writing the 
   * points out, and freeing the points when the object is deleted
   */
  
extern struct object_desc oct_geo_desc;
static struct object_desc *super = &oct_geo_desc;
struct object_desc oct_geo_points_desc;
static int geo_points_create(), geo_points_delete(), geo_points_undelete(), geo_points_free();
static int geo_points_read_fields(), geo_points_write_fields();


void
oct_init_geo_points_desc()
{
    oct_geo_points_desc = *super;
    oct_geo_points_desc.flags |= HAS_POINTS;
    oct_geo_points_desc.create_func = geo_points_create;
    oct_geo_points_desc.delete_func = geo_points_delete;
    oct_geo_points_desc.undelete_func = geo_points_undelete;
    oct_geo_points_desc.free_func = geo_points_free;
    oct_geo_points_desc.read_fields_func = geo_points_read_fields;
    oct_geo_points_desc.write_fields_func = geo_points_write_fields;
}

static int
geo_points_read_fields(obj)
generic *obj;
{
    struct geo_points *gp = (struct geo_points *) obj;
    int val;
    
    val = (*super->read_fields_func)(obj) &&
      oct_get_points(&gp->num_points, &gp->points);

    return val;
}

static int
geo_points_write_fields(obj)
generic *obj;
{
    struct geo_points *gp = (struct geo_points *) obj;
    
    return (*super->write_fields_func)(obj) &&
      oct_put_points(gp->num_points, gp->points);
}

static int
geo_points_create(desc, object, new_p)
struct facet *desc;
struct octObject *object;
generic **new_p;
{
    generic *ptr;
    int val = (*super->create_func)(desc, object, &ptr);
    struct geo_points *new = (struct geo_points *) ptr;

    if (val >= OCT_OK) {
	new->num_points = 0;
	new->points = NIL(struct octPoint);
	    /* create a bogus bounding box */
	MAKE_BB_INVALID(&new->bbox);
	*new_p = ptr;
	return OCT_OK;
    } else {
	return val;
    }
}

static int
geo_points_undelete(desc, object, new_p, old_xid, old_id)
struct facet *desc;
struct octObject *object;
generic **new_p;
int32 old_xid,old_id;
{
    generic *ptr;
    int val = (*super->undelete_func)(desc, object, &ptr, old_xid, old_id);
    struct geo_points *new = (struct geo_points *) ptr;

    if (val >= OCT_OK) {
	new->num_points = 0;
	new->points = NIL(struct octPoint);
	    /* create a bogus bounding box */
	MAKE_BB_INVALID(&new->bbox);
	*new_p = ptr;
	return OCT_OK;
    } else {
	return val;
    }
}

static int
geo_points_delete(gpptr, by_user)
generic *gpptr;
int by_user;
{
    struct geo_points *gp = (struct geo_points *) gpptr;
    int num_points = gp->num_points;
    struct octPoint *points = gp->points;
    struct octBox old_box;
    int val;

    if (num_points != 0) {
	(void) oct_bb_get(gpptr, &old_box);
    }

    val = (*super->delete_func)(gpptr, by_user);
    
    if (val < OCT_OK) {
	return val;
    }
    
    if (NEED_TO_RECORD(gpptr, OCT_PUT_POINTS)) {
	if (num_points != 0) {
	    return oct_record_change(gpptr, gpptr->flags.type, OCT_PUT_POINTS,
				     oct_null_xid, NIL(octObject),
				     num_points, points, &old_box);
	    } else {
	    return oct_record_change(gpptr, gpptr->flags.type, OCT_PUT_POINTS,
				     oct_null_xid, NIL(octObject),
				     num_points, points, NIL(struct octBox));
	    }
    } else {
	FREE(points);
	return OCT_OK;
    }
}

static int
geo_points_free(gpptr)
generic *gpptr;
{
    struct geo_points *gp = (struct geo_points *) gpptr;
    struct octPoint *points = gp->points;
    
    FREE(points);
    return (*super->free_func)(gpptr);
}

