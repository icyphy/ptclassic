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
#include "bb.h"

  /*
   * This implements operations that are shared between all geometric objects.
   * This is mainly bounding box maintenence
   */
  
static int geo_modify(), geo_unmodify(), geo_delete(), geo_uncreate();
extern struct object_desc oct_default_desc;
static struct object_desc *super = &oct_default_desc;
struct object_desc oct_geo_desc;

void
oct_init_geo_desc()
{
    oct_geo_desc = *super;
    oct_geo_desc.modify_func = geo_modify;
    oct_geo_desc.unmodify_func = geo_unmodify;
/* moved to create.c    
    oct_geo_desc.create_func = geo_create;
*/    
    oct_geo_desc.delete_func = geo_delete;
    oct_geo_desc.uncreate_func = geo_uncreate;
}

#ifdef notdef
/*
 * the bounding box update had to be moved out of here into create.c,
 * since the object has not been properly initialized
 * yet.
 */

static int geo_create(desc, object, new_p)
struct facet *desc;
struct octObject *object;
generic **new_p;
{
    struct octBox bbox;
    struct object_desc *obj_desc = &oct_object_descs[object->type];
    int val = (*super->create_func)(desc, object, new_p);
    
    if (val < OCT_OK) {
	return val;
    }

       /* make sure the bounding box is initialized */
    
    if (obj_desc->flags&HAS_BB_FUNC) {
	(*obj_desc->bb_func)(*new_p, &bbox);
    } else {
	bbox = *(struct octBox *) ((char *)*new_p + obj_desc->bb_offset);
    }

    (void) oct_bb_add(desc, &bbox);
    
    return val;
}
#endif

static int geo_delete(obj, by_user)
struct generic *obj;
int by_user;
{
    struct octBox bbox;
    struct facet *desc = obj->facet;
    int val;

    if (!by_user) {
	return (*super->delete_func)(obj, by_user);
    }
    
    (void) oct_bb_get(obj, &bbox);

    val = (*super->delete_func)(obj, by_user);
    
    if (val < OCT_OK) {
	return val;
    }
    
    (void) oct_bb_delete(desc, &bbox);
    
    return val;
}

static int geo_uncreate(obj, by_user)
struct generic *obj;
int by_user;
{
    struct octBox bbox;
    struct facet *desc = obj->facet;
    int val;

    if (!by_user) {
	return (*super->uncreate_func)(obj, by_user);
    }
    
    (void) oct_bb_get(obj, &bbox);

    val = (*super->uncreate_func)(obj, by_user);
    
    if (val < OCT_OK) {
	return val;
    }
    
    (void) oct_bb_delete(desc, &bbox);
    
    return val;
}

static int
geo_modify(ptr, new)
generic *ptr;
struct octObject *new;
{
    struct geo *obj = (struct geo *)ptr;
    struct octBox old_bb, new_bb;
    struct object_desc *desc = &oct_object_descs[obj->flags.type];
    int retval;

        /*
	 * the order is important here.  If the object has
	 * both a BB_FUNC and a BB_FIELD, we use the field to
	 * find the value before modification and call the
	 * function afterward to force an update of bounding
	 * box.
	 */
    
    if (desc->flags&HAS_BB_FIELD) {
	old_bb = *(struct octBox *) ((char *)ptr + desc->bb_offset);
    } else {
	(*desc->bb_func)(ptr, &old_bb);
    }

    retval = (*super->modify_func)(ptr, new);

    if (retval < OCT_OK) {
	return retval;
    }
    
    if (desc->flags&HAS_BB_FUNC) {
	if (desc->flags&HAS_BB_FIELD) {
	    MAKE_BB_INVALID((struct octBox *)((char *)ptr + desc->bb_offset));
	}
	(*desc->bb_func)(ptr, &new_bb);
    } else {
	new_bb = *(struct octBox *) ((char *)ptr + desc->bb_offset);
    }

    (void) oct_bb_modify(ptr, &old_bb, &new_bb);
    return OCT_OK;
}

static int
geo_unmodify(ptr, new)
generic *ptr;
struct octObject *new;
{
    struct geo *obj = (struct geo *)ptr;
    struct octBox old_bb, new_bb;
    struct object_desc *desc = &oct_object_descs[obj->flags.type];
    int retval;

        /*
	 * the order is important here.  If the object has
	 * both a BB_FUNC and a BB_FIELD, we use the field to
	 * find the value before modification and call the
	 * function afterward to force an update of bounding
	 * box.
	 */
    
    if (desc->flags&HAS_BB_FIELD) {
	old_bb = *(struct octBox *) ((char *)ptr + desc->bb_offset);
    } else {
	(*desc->bb_func)(ptr, &old_bb);
    }

    retval = (*super->unmodify_func)(ptr, new);

    if (retval < OCT_OK) {
	return retval;
    }
    
    if (desc->flags&HAS_BB_FUNC) {
	if (desc->flags&HAS_BB_FIELD) {
	    MAKE_BB_INVALID((struct octBox *)((char *)ptr + desc->bb_offset));
	}
	(*desc->bb_func)(ptr, &new_bb);
    } else {
	new_bb = *(struct octBox *) ((char *)ptr + desc->bb_offset);
    }

    (void) oct_bb_modify(ptr, &old_bb, &new_bb);
    return OCT_OK;
}

