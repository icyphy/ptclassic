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
 * Implementations of the the oct functions for nets.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "io.h"

#include "io_procs.h"

#include "net.h"

extern struct object_desc oct_default_desc;
static struct object_desc *super = &oct_default_desc;

static int net_copy_fields();
static int net_free_fields();
static int net_write_fields();
static int net_read_fields();
static int net_modify();

void
oct_net_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct net);
    object_desc->external_size = sizeof(struct octNet);
    object_desc->contain_mask = (octObjectMask) (OCT_ALL_MASK & ~OCT_FACET_MASK);
    object_desc->flags |= HAS_NAME;
    object_desc->user_offset = FIELD_OFFSET(struct net, user_net);
    object_desc->name_offset = FIELD_OFFSET(struct octNet, name),
    object_desc->copy_fields_func = net_copy_fields;
    object_desc->modify_func = net_modify;
    object_desc->free_fields_func = net_free_fields;
    object_desc->write_fields_func = net_write_fields;
    object_desc->read_fields_func = net_read_fields;
}

/*ARGSUSED*/
static int
net_copy_fields(o_p, c_p, size)
char *o_p, *c_p;
int size;
{
    struct octNet *orig = (struct octNet *) o_p;
    struct octNet *copy = (struct octNet *) c_p;

    *copy = *orig;

    if (copy->name != NIL(char)) {
	copy->name = oct_str_intern(copy->name);
    }
    return OCT_OK;		/* Return OCT_OK because we take the
				   pointer of this func */
}

static int
net_free_fields(ptr)
generic *ptr;
{
    struct net *net = (struct net *) ptr;

    if (net->user_net.name != NIL(char)) {
	oct_str_free(net->user_net.name);
    }
    return OCT_OK;		/* Return OCT_OK because we take the
				   pointer of this func */
}
    
static octStatus
net_modify(ptr, obj)
generic *ptr;
struct octObject *obj;
{
    struct net *net = (struct net *) ptr;
    char *new_name = obj->contents.net.name;

    MODIFY_NAME(net->user_net.name, new_name);
    net->user_net.name = new_name;
    net->user_net.width = obj->contents.net.width;
    
    return OCT_OK;
}

static octStatus
net_read_fields(ptr)
generic *ptr;
{
    struct net *net = (struct net *) ptr;
    
    return (*super->read_fields_func)(ptr) &&
      oct_get_string(&net->user_net.name) &&
      oct_get_32(&net->user_net.width);
}

static octStatus
net_write_fields(ptr)
generic *ptr;
{
    struct net *net = (struct net *) ptr;
    
    return (*super->write_fields_func)(ptr) &&
      oct_put_string(net->user_net.name) &&
      oct_put_32(net->user_net.width);
}
