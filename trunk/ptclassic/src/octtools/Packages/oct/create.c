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
#include "attach.h"
#include "bb.h"
#include "change_record.h"
#include "create.h"
#include "oct_utils.h"
#include "ref.h"

#undef octCreate

/* Perhaps this could be static?*/
int oct_do_uncreate
	ARGS((generic *ptr, int by_user));

octStatus
octCreate(container, object)
octObject *container;
octObject *object;
{
    generic *cptr, *new;
    octStatus retval;
    
    cptr = oct_id_to_ptr(container->objectId);
    
    if (cptr == NIL(generic)) {
	oct_error("octCreate: The first object's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_create_and_attach(cptr, object, &new);
    
    if (retval < OCT_OK) {
	oct_prepend_error("octCreate: ");
    }
    
    return retval;
}


int
oct_do_create(cptr, object, optr_p)
generic *cptr;
struct octObject *object;
generic **optr_p;
{
    struct object_desc *object_desc;
    struct facet *desc;
    int retval;
    
    desc = cptr->facet;

    if (object->type <= OCT_UNDEFINED_OBJECT || object->type > OCT_MAX_TYPE) {
	oct_error("Bad type field in the object to be created");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
	/*NOTREACHED*/
    }
    
    object_desc = &oct_object_descs[object->type];

    retval = (*object_desc->create_func)(desc, object, optr_p);

    if (retval < OCT_OK) {
	return retval;
    }

    /* 
     * XXX - this belongs with geo.c, but the present structure forces 
     * it to be done here instead
     */
    
    if (object_desc->flags&HAS_BB) {
	struct octBox bbox;
	
	if (object_desc->flags&HAS_BB_FUNC) {
	    if (object_desc->flags&HAS_BB_FIELD) {
		MAKE_BB_INVALID((struct octBox *)((char *)*optr_p + object_desc->bb_offset));
	    }
	    if ((*object_desc->bb_func)(*optr_p, &bbox) == OCT_OK) {
		(void) oct_bb_add(desc, &bbox);
	    }		
	} else {
	    bbox = *(struct octBox *) ((char *)*optr_p + object_desc->bb_offset);
	    (void) oct_bb_add(desc, &bbox);
	}
    }
    
    desc->create_date = ++desc->time_stamp;
    if( object->type != OCT_INSTANCE ) /* the instance will report the addition itself*/
		return RECORD_CHANGE(*optr_p, OCT_CREATE, oct_null_xid);
	else
		return OCT_OK;
}


/*
 * ARGH!!!! XXXX!!!! this has to be made public so vem/undo can use it
 * looks like undo really belongs in Oct...
 */
int
oct_do_undelete(container, object, old_xid, old_id)
octObject *container;
octObject *object;
int32 old_xid,old_id;
{
    struct object_desc *object_desc;
    struct facet *desc;
    int retval;
    generic *cptr;
    generic *optr_p;

    if ((cptr = oct_id_to_ptr(container->objectId)) == NIL(generic)) {
	oct_error("Corrupted container object in oct_do_undelete");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
	/*NOTREACHED*/
    }
    
    if (object->type <= OCT_UNDEFINED_OBJECT || object->type > OCT_MAX_TYPE) {
	oct_error("Bad type field in the object to be undeleted");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
	/*NOTREACHED*/
    }
    
    object_desc = &oct_object_descs[object->type];

    desc = cptr->facet;

    retval = (*object_desc->undelete_func)(desc, object, &optr_p, old_xid, old_id);

    if (retval < OCT_OK) {
	return retval;
    }

    /* 
     * XXX - this belongs with geo.c, but the present structure forces 
     * it to be done here instead
     */
    
    if (object_desc->flags&HAS_BB) {
	struct octBox bbox;
	
	if (object_desc->flags&HAS_BB_FUNC) {
	    if (object_desc->flags&HAS_BB_FIELD) {
		MAKE_BB_INVALID((struct octBox *)((char *) optr_p + object_desc->bb_offset));
	    }
	    if ((*object_desc->bb_func)(optr_p, &bbox) == OCT_OK) {
		(void) oct_bb_add(desc, &bbox);
	    }		
	} else {
	    bbox = *(struct octBox *) ((char *) optr_p + object_desc->bb_offset);
	    (void) oct_bb_add(desc, &bbox);
	}
    }
    
    desc->create_date = ++desc->time_stamp;

	/* the undelete-instance routine  doesn't do automatic actual creation.
	   So there's no need for the routine to report the creation itself, because
	   it doesn't have to immediately follow it with a bunch of creates and attaches.
	   So we do it here for everybody.  */
	
	RECORD_CHANGE(optr_p, OCT_CREATE, oct_null_xid);
	return OCT_OK;
}

int    
oct_do_create_and_attach(cptr, object, optr_p)
generic *cptr;
octObject *object;
generic **optr_p;
{
    int retval;

    retval = oct_do_create(cptr, object, optr_p);

    if (retval < OCT_OK) {
	return retval;
    }

    retval = oct_do_attach(cptr, *optr_p, 0);
    
    return retval;
}


octStatus
octDelete(object)
octObject *object;
{
    generic *ptr;
    octStatus retval;
      
    ptr = oct_id_to_ptr(object->objectId);
    
    if (ptr == NIL(generic)) {
	oct_error("octDelete: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_delete(ptr, 1);
    if (retval < OCT_OK) {
	oct_prepend_error("octDelete: ");
    }
    return retval;
}


int
oct_do_delete(ptr, by_user)
generic *ptr;
int by_user;
{
    int retval, record;
    struct object_desc *object_desc = &oct_object_descs[ptr->flags.type];
    struct facet *desc;
    octObject *copy;
    struct octBox bbox, *bptr = NIL(struct octBox);

    if (!oct_fix_generators(NIL(struct chain), ptr)) {
	return OCT_ERROR;
    }
    
    desc = ptr->facet;

    record = NEED_TO_RECORD(ptr, OCT_DELETE);
    
    if (record) {
	int val = oct_do_record_copy(ptr, &copy);
	if (val != OCT_OK) {
	    return val;
	}
	copy->objectId = ptr->objectId; /* save that Id!!! */
	if (object_desc->flags&HAS_BB) {
	    (void) (*object_desc->bb_func)(ptr, &bbox);
	    bptr = &bbox;
	}
    }

    retval = (object_desc->delete_func)(ptr, by_user);

    if (retval < OCT_OK) {
	if (record) {
	    (void) oct_free_record_copy(copy);
	}
	return retval;
    } 
    
    desc->object_count--;
    
    desc->delete_date = ++desc->time_stamp;
    
    if (record) {
	int val = oct_record_change(ptr, ptr->flags.type, OCT_DELETE, oct_null_xid, copy, 0, NIL(struct octPoint), bptr);
	if (val != OCT_OK) {
	    return val;
	}
    }
    FREE(ptr);
    return retval;
}

octStatus
octUnCreate(object)
octObject *object;
{
    generic *ptr;
    octStatus retval;
      
    ptr = oct_id_to_ptr(object->objectId);
    
    if (ptr == NIL(generic)) {
	oct_error("octUnCreate: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_uncreate(ptr, 1);
    if (retval < OCT_OK) {
	oct_prepend_error("octUnCreate: ");
    }
    return retval;
}


int
oct_do_uncreate(ptr, by_user)
generic *ptr;
int by_user;
{
    int retval, record;
    struct object_desc *object_desc = &oct_object_descs[ptr->flags.type];
    struct facet *desc;
    octObject *copy;
    struct octBox bbox, *bptr = NIL(struct octBox);

    if (!oct_fix_generators(NIL(struct chain), ptr))
	{
		return OCT_ERROR;
    }
    
    desc = ptr->facet;

    record = NEED_TO_RECORD(ptr, OCT_DELETE);
    
    if (record)
	{
		int val = oct_do_record_copy(ptr, &copy);
		if (val != OCT_OK)
		{
			return val;
		}
		copy->objectId = ptr->objectId; /* save that Id!!! */
		if (object_desc->flags&HAS_BB)
		{
			(void) (*object_desc->bb_func)(ptr, &bbox);
			bptr = &bbox;
		}
    }

    retval = (object_desc->uncreate_func)(ptr, by_user);

    if (retval < OCT_OK)
	{
		if (record)
		{
			(void) oct_free_record_copy(copy);
		}
		return retval;
    } 
    
    desc->object_count--;
    
    desc->delete_date = ++desc->time_stamp;
    
    if (record)
	{
		int val = oct_record_change(ptr, ptr->flags.type, OCT_DELETE, oct_null_xid, copy, 0, NIL(struct octPoint), bptr);
		if (val != OCT_OK)
		{
			return val;
		}
    }
    FREE(ptr);
    return retval;
}


octStatus
compatOctCreate(container, object)
octObject *container;
octObject *object;
{
    generic *cptr, *new;
    int retval;
    
    cptr = oct_id_to_ptr(container->objectId);
    
    if (cptr == NIL(generic))
	{
		oct_error("octCreate: The first object's id has been corrupted");
		errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    /* compatibility */
    if (object->type == OCT_INSTANCE)
	{
		object->contents.instance.facet = strsave("contents");
    }

    if (object->type == OCT_NET)
	{
		object->contents.net.width = 1;
    }

    if (object->type == OCT_TERM)
	{
		object->contents.term.width = 1;
    }

    retval = oct_do_create_and_attach(cptr, object, &new);
    
    if (retval < OCT_OK)
	{
		oct_prepend_error("octCreate: ");
    }
    
    return retval;
}
