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
#include "geo.h"
#include "attach.h"
#include "io.h"
#include "oct_utils.h"

#include "change_record.h"

extern octStatus oct_illegal_op();
extern struct object_desc oct_geo_points_desc;

extern void oct_free_instance_fields();
static octStatus change_record_free(), change_record_delete();
static octStatus change_record_gen_contents(), change_record_gen_first_content();
static struct object_desc *super = &oct_geo_points_desc;

void
oct_change_record_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;

    object_desc->internal_size = sizeof(struct change_record);
    object_desc->external_size = sizeof(struct octChangeRecord);
    object_desc->flags |= HAS_BB_FIELD;
    object_desc->bb_offset = FIELD_OFFSET(struct change_record, bbox);
    object_desc->contain_mask = (octObjectMask) 0;
    object_desc->create_func = oct_illegal_op;
    object_desc->undelete_func = oct_illegal_op;
    object_desc->modify_func = oct_illegal_op;
    object_desc->delete_func = change_record_delete;
    object_desc->free_func = change_record_free;
    object_desc->gen_first_content_func = change_record_gen_first_content;
    object_desc->gen_contents_func = change_record_gen_contents;
    object_desc->user_offset =
      FIELD_OFFSET(struct change_record, user_change_record);
}
/* 
 * Does this operation on this object have a change list registered 
 * for it?
 */
int
oct_must_record(obj, operation)
generic *obj;
int operation;
{
    struct facet *desc = obj->facet;
    int type = obj->flags.type;
    struct change_list *list = (struct change_list *) desc->object_lists[OCT_CHANGE_LIST];
    
    while(list != NIL(struct change_list)) {
	int func_mask = list->user_change_list.functionMask;
	int obj_mask = list->user_change_list.objectMask;

	if (IN_MASK(operation, func_mask) && IN_MASK(type, obj_mask)) {
	    return 1;
	}
	list = (struct change_list *) list->last;
    }
    return 0;
}
    
static octStatus
make_change_record(desc, operation, o_xid, c_xid, old_value, num_points, points, bbox, new_p)
struct facet *desc;
int operation;
int32 o_xid, c_xid;
octObject *old_value;
int num_points;
struct octPoint *points;
struct octBox *bbox;
struct change_record **new_p;
{
    struct octObject rec;
    generic *gnew;
    struct change_record *new;
    int retval;

    rec.type = OCT_CHANGE_RECORD;
    rec.contents.changeRecord.changeType = operation;
    rec.contents.changeRecord.objectExternalId = o_xid;
    rec.contents.changeRecord.contentsExternalId = c_xid;

    retval = (*super->create_func)(desc, &rec, &gnew);

    if (retval < OCT_OK) {
	return retval;
    }

    new = (struct change_record *) gnew;

    new->old_value = old_value;
    new->num_points = num_points;
    new->points = points;
    if (bbox != NIL(struct octBox)) {
	new->bbox = *bbox;
    } else {
	MAKE_BB_INVALID(&new->bbox);
    }
    
    *new_p = new;
    return OCT_OK;
}

octStatus
oct_make_change_record_marker(clid)
octId clid;
{
	octObject cl;
	octObject obj,rec;
	generic *ptr,*gnew;
	struct chain *my_chain;
	struct change_record *new;
	int retval;
	struct facet *desc;
	
	/* get the changelist */
	cl.objectId = clid;
	if( octGetById(&cl) != OCT_OK )
	{
		/* obviously, there's no changeList anymore, for some reason,
		   and the best action is no action! */
		return OCT_OK;
	}

	/* get the internal changelist structure */
	ptr = oct_id_to_ptr(cl.objectId);
	my_chain = ptr->contents;
	if( !my_chain )
		return OCT_OK; /* no real need for a trailing mark (inverted list-wise)*/
	oct_fill_object(&obj,my_chain->next->object);

	if( obj.contents.changeRecord.changeType == OCT_MARKER )
		return OCT_OK; /* Never two in a row */

	/* build the object. Make sure the fields get inititialized right. */
	desc = ptr->facet;
	rec.type = OCT_CHANGE_RECORD;
	rec.contents.changeRecord.changeType = OCT_MARKER;
	rec.contents.changeRecord.objectExternalId = 0;
	rec.contents.changeRecord.contentsExternalId = 0;
	retval = (*super->create_func)(desc,&rec,&gnew);
	if( retval < OCT_OK )
		return retval;
	new = (struct change_record *)gnew;
	new->old_value = 0;
	new->num_points = 0;
	new->points = 0;
	MAKE_BB_INVALID(&new->bbox);

	retval = (int)oct_do_attach(ptr, gnew, 0);
	return retval;
}


octStatus
oct_record_change(obj, type, operation,  c_xid, old_value, num_points, points, bbox)
generic *obj;
unsigned int type;
int operation;
int32 c_xid;			/* The container object, for attach and detach records */
octObject *old_value;		/* The old value of the object, for delete and modify */
int num_points;			/* The number of old points, for put_points*/
struct octPoint *points;	/* The old points, for put_points */
struct octBox *bbox;		/* The old bounding box, for modify, put points and delete */
{
    struct facet *desc = obj->facet;
    int32 o_xid = obj->externalId;
    struct change_list *list;
    struct change_record *new;
    int retval;

    retval = make_change_record(desc, operation, o_xid, c_xid, old_value, num_points, points, bbox, &new);

    if (retval < OCT_OK) {
	return retval;
    }

    list = (struct change_list *) desc->object_lists[OCT_CHANGE_LIST];
    while(list != NIL(struct change_list)) {
	int func_mask = list->user_change_list.functionMask;
	int obj_mask = list->user_change_list.objectMask;

	if (IN_MASK(operation, func_mask) && IN_MASK(type, obj_mask)) {
	    retval = oct_do_attach((generic *) list, (generic *) new, 0);
	    if (retval < OCT_OK) {
		return retval;
	    }
	}
	list = (struct change_list *) list->last;
    }
    
    return OCT_OK;
}

/* Copy away the state of object before it is deleted or modified */

octStatus
oct_do_record_copy(ptr, copy_p)
generic *ptr;
octObject **copy_p;
{
    struct object_desc *obj_desc = &oct_object_descs[ptr->flags.type];
    octObject *copy = ALLOC(octObject, 1);
    if (ptr->flags.type != OCT_INSTANCE) {
	if (obj_desc->copy_fields_func != (int (*)()) 0) {
	    (*obj_desc->copy_fields_func)((char *)ptr + obj_desc->user_offset,
					  (char *) &copy->contents,
					  obj_desc->external_size);
	} else {
	    (void) memcpy((char *) &copy->contents,
			  (char *)ptr + obj_desc->user_offset,
			  (int) obj_desc->external_size);
	}
    } else {
	(void) oct_copy_instance((struct instance *) ptr, &copy->contents.instance);
    }
    
    copy->type = ptr->flags.type;
    /*copy->objectId = oct_null_id;*/
    *copy_p = copy;
    return OCT_OK;
}

octStatus
oct_free_record_copy(copy)
octObject *copy;
{
    struct object_desc *obj_desc = &oct_object_descs[copy->type];

    if (copy->type != OCT_INSTANCE) {
	if (obj_desc->free_fields_func != (int (*)()) 0) {
	    (*obj_desc->free_fields_func)((generic *) ((char *)&copy->contents - obj_desc->user_offset));
	}
    } else {
	oct_free_instance_fields(&copy->contents.instance);
    }

    FREE(copy);
    return OCT_OK;
}

octStatus
oct_do_record_delete(rec)
struct change_record *rec;
{
    return change_record_delete((generic *) rec, 0);
}


static octStatus
change_record_delete(g_rec, by_user)
generic *g_rec;
int by_user;
{
    struct change_record *rec = (struct change_record *) g_rec;

    if (by_user) {
	oct_error("Deletion of change records is not allowed");
	errRaise(OCT_PKG_NAME, OCT_ILL_OP, octErrorString());
    }

    if (rec->old_value != NIL(octObject)) {
	(void) oct_free_record_copy(rec->old_value);
    }

    return (*super->delete_func)(g_rec, by_user);
}

static octStatus
change_record_free(g_rec)
generic *g_rec;
{
    struct change_record *rec = (struct change_record *) g_rec;

    if (rec->old_value != NIL(octObject)) {
	(void) oct_free_record_copy(rec->old_value);
    }

    return (*super->free_func)(g_rec);
}
    
/* 
 * The old values are accessed as though they are attached to the 
 * change_record. Since the old values are actually objects and they 
 * aren't attached using the normal mechanism, we redefine the normal
 * generator functions to allow them to be accessed normally
 */

static octStatus
change_record_gen_first_content(ptr, mask, obj)
generic *ptr;
octObjectMask mask;
octObject *obj;
{
    struct change_record *rec = (struct change_record *) ptr;

    if (rec->old_value == NIL(octObject) || !IN_MASK(rec->old_value->type, mask)) {
	oct_error_string = "No value to generate";
	return OCT_GEN_DONE;
    } else {
	*obj = *rec->old_value;
	/*obj->objectId = oct_null_id; */
	return OCT_OK;
    }
}

/*ARGSUSED*/
static octStatus
nil_gen(gen, obj)
octGenerator *gen;
octObject *obj;
{
    oct_error_string = "No value to generate";
    return OCT_GEN_DONE;
}

/*ARGSUSED*/
static octStatus
free_gen(gen)
octGenerator *gen;
{
    return OCT_OK;
}

static octStatus
change_record_contents_gen(gen, obj)
octGenerator *gen;
octObject *obj;
{
    struct change_record *rec = (struct change_record *) gen->state;

    gen->generator_func = nil_gen;

    if (rec->old_value == NIL(octObject)) {
	oct_error_string = "No value to generate";
	return OCT_GEN_DONE;
    } else {
	*obj = *rec->old_value;
	/*obj->objectId = oct_null_id;*/
	return OCT_OK;
    }
}
     
static octStatus
change_record_gen_contents(ptr, mask, gen)
generic *ptr;
octObjectMask mask;
octGenerator *gen;
{
    struct change_record *rec = (struct change_record *) ptr;

    if (rec->old_value == NIL(octObject) ||
	!IN_MASK(rec->old_value->type, mask)) {
	gen->generator_func = nil_gen;
    } else {
	gen->generator_func = change_record_contents_gen;
    }
    
    gen->free_func = free_gen;
    gen->state = (char *) rec;
    return OCT_OK;
}

