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
#include "io.h"
#include "geo.h"
#include "change_list.h"
#include "change_record.h"

extern struct object_desc oct_default_desc;

/* static octStatus change_list_read_fields(), change_list_write_fields(); */
static octStatus change_list_delete();
static octStatus change_list_create();
static octStatus change_list_modify();
static struct object_desc *super = &oct_default_desc;


void
oct_change_list_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;

    object_desc->internal_size = sizeof(struct change_list);
    object_desc->external_size = sizeof(struct octChangeList);
    object_desc->contain_mask = (octObjectMask) OCT_PROP_MASK|OCT_CHANGE_RECORD_MASK|OCT_BAG_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct change_list, user_change_list);
    object_desc->delete_func = change_list_delete;
    object_desc->create_func = change_list_create;
    object_desc->modify_func = change_list_modify;
}

static int
change_list_create(desc, object, new_p)
struct facet *desc;
struct octObject *object;
generic **new_p;
{
    int val = (*super->create_func)(desc, object, new_p);
    struct change_list *new = (struct change_list *) *new_p;

    if (val >= OCT_OK) {
	new->user_change_list.objectMask &= ~(OCT_CHANGE_RECORD_MASK | OCT_CHANGE_LIST_MASK); 
	new->flags.is_temporary = 1;
	return OCT_OK;
    } else {
	return val;
    }
}
/* 
 * When a change-list is deleted, we delete any change change_records 
 * that are left dangling.
 */

/*ARGSUSED*/
static octStatus
change_list_delete(cptr, by_user)
generic *cptr;
int by_user;
{
    struct change_list *change_list = (struct change_list *)cptr;
    struct chain *next, *ptr;
    int retval;

    if (by_user) {
	ptr = change_list->contents;

	while (ptr != NIL(struct chain)) {
	    next = ptr->last;
	    if (ptr->object->flags.type == OCT_CHANGE_RECORD) {
		if (ptr->object->containers->next_chain == NIL(struct chain)) {
			/* 
			 * garbage collect the records */
		    retval = oct_do_record_delete((struct change_record *) ptr->object);
		    if (retval < OCT_OK) {
			return retval;
		    }
		}
	    }
	    ptr = next;
	}
    }
    return (*super->delete_func)(cptr, by_user);
}

/* a no-op until we save change-lists to disk */

/*ARGSUSED*/
int
oct_filter_records(desc)
struct facet *desc;
{
    return 1;
}


static octStatus
change_list_modify(ptr, obj)
generic *ptr;
struct octObject *obj;
{
    struct change_list *cl = (struct change_list *) ptr;
    cl->user_change_list = obj->contents.changeList;
    return OCT_OK;
}

