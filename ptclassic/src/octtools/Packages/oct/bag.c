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
/*
 * Implementations of the the oct functions for bags.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "io.h"
#include "io_procs.h"

extern struct object_desc oct_default_desc;
static struct object_desc *super = &oct_default_desc;

static int bag_modify();
static int bag_free_fields();
static int bag_write_fields();
static int bag_read_fields();
static int bag_copy_fields();

void
oct_bag_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct bag);
    object_desc->external_size = sizeof(struct octBag);
    object_desc->contain_mask = (octObjectMask) (OCT_ALL_MASK & ~OCT_FACET_MASK);
    object_desc->flags |= HAS_NAME;
    object_desc->user_offset = FIELD_OFFSET(struct bag, user_bag);
    object_desc->name_offset = FIELD_OFFSET(struct octBag, name),
    object_desc->copy_fields_func = bag_copy_fields;
    object_desc->modify_func = bag_modify;
    object_desc->free_fields_func = bag_free_fields;
    object_desc->write_fields_func = bag_write_fields;
    object_desc->read_fields_func = bag_read_fields;
}

/*ARGSUSED*/
static int
bag_copy_fields(o_p, c_p, size)
char *o_p, *c_p;
int size;
{
    struct octBag *orig = (struct octBag *) o_p;
    struct octBag *copy = (struct octBag *) c_p;

    *copy = *orig;

    if (copy->name != NIL(char)) {
	copy->name = oct_str_intern(copy->name);
    }
    return 1;			/* This func must return an int, as we
				   pass a func pointer to it around.  */
}

static int
bag_free_fields(ptr)
generic *ptr;
{
    struct bag *bag = (struct bag *) ptr;

    if (bag->user_bag.name != NIL(char)) {
	oct_str_free(bag->user_bag.name);
    }
    return 1;			/* This func must return an int as we
				   pass a pointer to it around. */
}
    
static octStatus
bag_modify(ptr, obj)
generic *ptr;
struct octObject *obj;
{
    struct bag *bag = (struct bag *) ptr;
    char *new_name = obj->contents.bag.name;

    MODIFY_NAME(bag->user_bag.name, new_name);
    bag->user_bag.name = new_name;
    
    return OCT_OK;
}

static octStatus
bag_read_fields(ptr)
generic *ptr;
{
    struct bag *bag = (struct bag *) ptr;
    
    return (*super->read_fields_func)(ptr) &&
      oct_get_string(&bag->user_bag.name);
}

static octStatus
bag_write_fields(ptr)
generic *ptr;
{
    struct bag *bag = (struct bag *) ptr;
    
    return (*super->write_fields_func)(ptr) &&
      oct_put_string(bag->user_bag.name);
}
