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

#include "change_record.h"
#include "oct_utils.h"

#include "modify.h"

#undef octModify

/* Forward references (Should these be static?) */
int oct_do_unmodify
	ARGS((generic *ptr, octObject *object));

octStatus
octModify(object)
octObject *object;
{
    generic *ptr;
    octStatus retval;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error("octModify: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_modify(ptr, object);
    if (retval < OCT_OK) {
	oct_prepend_error("octModify: ");
    }
    return retval;
}

octStatus
oct_do_modify(ptr, object)
generic *ptr;
octObject *object;
{
    octObject *copy;
    int do_mark = 0, retval;
    struct facet *desc;
    struct octBox bbox, *bptr = NIL(struct octBox);
    struct object_desc *object_desc = &oct_object_descs[ptr->flags.type];
    
    if ((ptr->flags.type == OCT_FORMAL) && (object->type == OCT_TERM)) {
	object->type = OCT_FORMAL;
    }

    if (ptr->flags.type != object->type) {
	oct_error("octModify: can not modify the type of an object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    desc = ptr->facet;
    if (NEED_TO_RECORD(ptr, OCT_MODIFY)) {
	int retval = oct_do_record_copy(ptr, &copy);
	if (retval != OCT_OK) {
	    return retval;
	}
	if (object_desc->flags&HAS_BB) {
	    (void) (*object_desc->bb_func)(ptr, &bbox);
	    bptr = &bbox;
	}
	do_mark = 1;
    }
    
    retval = (*object_desc->modify_func)(ptr,object);

    if (retval < OCT_OK) {
	if (do_mark) {
	    (void) oct_free_record_copy(copy);
	}
	return retval;
    }

    desc->modify_date = ++desc->time_stamp;
    
    if (do_mark) {
	int val = oct_record_change(ptr, ptr->flags.type, OCT_MODIFY, oct_null_xid, copy, 0, NIL(struct octPoint), bptr);
	if (val != OCT_OK) {
	    return val;
	}
    }
    
    return retval;
}

octStatus
octUnModify(object)
octObject *object;
{
    generic *ptr;
    octStatus retval;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error("octUnModify: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_unmodify(ptr, object);
    if (retval < OCT_OK) {
	oct_prepend_error("octUnModify: ");
    }
    return retval;
}

int
oct_do_unmodify(ptr, object)
generic *ptr;
octObject *object;
{
    octObject *copy;
    int do_mark = 0, retval;
    struct facet *desc;
    struct octBox bbox, *bptr = NIL(struct octBox);
    struct object_desc *object_desc = &oct_object_descs[ptr->flags.type];
    
    if ((ptr->flags.type == OCT_FORMAL) && (object->type == OCT_TERM)) {
	object->type = OCT_FORMAL;
    }

    if (ptr->flags.type != object->type) {
	oct_error("octUnModify: can not modify the type of an object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    desc = ptr->facet;
    if (NEED_TO_RECORD(ptr, OCT_MODIFY)) {
	int retval = oct_do_record_copy(ptr, &copy);
	if (retval != OCT_OK) {
	    return retval;
	}
	if (object_desc->flags&HAS_BB) {
	    (void) (*object_desc->bb_func)(ptr, &bbox);
	    bptr = &bbox;
	}
	do_mark = 1;
    }
    
    retval = (*object_desc->unmodify_func)(ptr,object);

    if (retval < OCT_OK) {
	if (do_mark) {
	    (void) oct_free_record_copy(copy);
	}
	return retval;
    }

    desc->modify_date = ++desc->time_stamp;
    
    if (do_mark) {
	int val = oct_record_change(ptr, ptr->flags.type, OCT_MODIFY, oct_null_xid, copy, 0, NIL(struct octPoint), bptr);
	if (val != OCT_OK) {
	    return val;
	}
    }
    
    return retval;
}


octStatus
compatOctModify(object)
octObject *object;
{
    generic *ptr;
    octStatus retval;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error("octModify: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    /* compatibility */
    if (ptr->flags.type == OCT_INSTANCE) {
	object->contents.instance.facet = strsave("contents");
    }

    if (ptr->flags.type == OCT_NET) {
	object->contents.net.width = 1;
    }

    if (ptr->flags.type == OCT_TERM) {
	object->contents.term.width = 1;
    }

    retval = oct_do_modify(ptr, object);
    if (retval < OCT_OK) {
	oct_prepend_error("octModify: ");
    }

    return retval;
}
