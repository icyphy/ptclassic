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
 * Implementations of the the oct functions for layers.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "io.h"
#include "io_procs.h"

#include "layer.h"

extern struct object_desc oct_default_desc;
static struct object_desc *super = &oct_default_desc;

static int layer_copy_fields();
static int layer_free_fields();
static int layer_write_fields();
static int layer_read_fields();
static int layer_modify();

void
oct_layer_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct layer);
    object_desc->external_size = sizeof(struct octLayer);
    object_desc->flags |= HAS_NAME;
    object_desc->contain_mask =
      (octObjectMask) OCT_BAG_MASK|OCT_GEO_MASK|OCT_PROP_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct layer, user_layer);
    object_desc->name_offset = FIELD_OFFSET(struct octLayer, name),
    object_desc->copy_fields_func = layer_copy_fields;
    object_desc->modify_func = layer_modify;
    object_desc->free_fields_func = layer_free_fields;
    object_desc->write_fields_func = layer_write_fields;
    object_desc->read_fields_func = layer_read_fields;
}

/*ARGSUSED*/
static int
layer_copy_fields(o_p, c_p, size)
char *o_p, *c_p;
int size;
{
    struct octLayer *orig = (struct octLayer *) o_p;
    struct octLayer *copy = (struct octLayer *) c_p;

    *copy = *orig;

    if (copy->name != NIL(char)) {
	copy->name = oct_str_intern(copy->name);
    }
    return OCT_OK;		/* Return OCT_OK because we take the
				   pointer of this func */
}

static int
layer_free_fields(ptr)
generic *ptr;
{
    struct layer *layer = (struct layer *) ptr;

    if (layer->user_layer.name != NIL(char)) {
	oct_str_free(layer->user_layer.name);
    }
    return OCT_OK;		/* Return OCT_OK because we take the
				   pointer of this func */
}
    
static octStatus
layer_modify(ptr, obj)
generic *ptr;
struct octObject *obj;
{
    struct layer *layer = (struct layer *) ptr;
    char *new_name = obj->contents.layer.name;

    MODIFY_NAME(layer->user_layer.name, new_name);
    layer->user_layer.name = new_name;
    
    return OCT_OK;
}

static octStatus
layer_read_fields(ptr)
generic *ptr;
{
    struct layer *layer = (struct layer *) ptr;
    
    return (*super->read_fields_func)(ptr) &&
      oct_get_string(&layer->user_layer.name);
}

static octStatus
layer_write_fields(ptr)
generic *ptr;
{
    struct layer *layer = (struct layer *) ptr;
    
    return (*super->write_fields_func)(ptr) &&
      oct_put_string(layer->user_layer.name);
}
