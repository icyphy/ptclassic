#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1994 The Regents of the University of California.
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
#include "io.h"
#include "chain.h"
#include "io_procs.h"
#include "mark.h"
#include "oct_id.h"
#include "oct_utils.h"

#include "default.h"

/* 
 * Here the basic behavior of objects is defined.  Most of the actual
 * objects just modify a few of the operations defined here and 
 * inherit the rest
 */
  
/* if copy_fields_func isn't defined, use memcpy instead */

#define DO_COPY(object_desc, ptr, object)\
if ((object_desc)->copy_fields_func == (int  (*)()) 0) {\
    if ((object_desc)->user_offset != ILLEGAL_OFFSET) {\
	(void) memcpy((char *)(ptr)+(object_desc)->user_offset,\
		      (char *) &(object)->contents,\
		      (int) (object_desc)->external_size);\
    }\
} else {\
     (*(object_desc)->copy_fields_func)((char *) &(object)->contents,\
	                       	       (char *)(ptr)+(object_desc)->user_offset,\
				       (object_desc)->external_size);\
}

#define DO_FIX(object_desc, ptr)\
    if ((object_desc)->fix_fields_func != (int (*)()) 0) {\
	(*(object_desc)->fix_fields_func)(ptr);\
    }

#define DO_FREE(object_desc, ptr)\
    if ((object_desc)->free_fields_func != (int (*)()) 0) {\
	(*(object_desc)->free_fields_func)(ptr);\
    }

struct object_desc oct_default_desc;

extern octStatus oct_bb_default();

octStatus oct_create_default(), oct_uncreate_default(), oct_delete_default(), oct_undelete_default();
octStatus oct_attach_default(), oct_undetach_default(), oct_free_default();
octStatus oct_is_attached_default(), oct_detach_default(), oct_unattach_default();
octStatus oct_modify_default(), oct_unmodify_default(), oct_get_by_default();
octStatus oct_get_container_by_default();
octStatus oct_gen_contents_default(), oct_gen_containers_default();
octStatus oct_gen_contents_offset_default();
octStatus oct_gen_first_content_default(), oct_gen_first_container_default();
static octStatus default_read_fields(), default_write_fields();
octStatus oct_write_object(), oct_read_object();

octStatus oct_init_default_desc()
{
    oct_default_desc.flags = 0;
    oct_default_desc.create_func = oct_create_default;
    oct_default_desc.uncreate_func = oct_uncreate_default;
    oct_default_desc.delete_func = oct_delete_default;
    oct_default_desc.undelete_func = oct_undelete_default;
    oct_default_desc.free_func = oct_free_default;
    oct_default_desc.attach_func = oct_attach_default;
    oct_default_desc.undetach_func = oct_undetach_default;
    oct_default_desc.is_attached_func = oct_is_attached_default;
    oct_default_desc.detach_func = oct_detach_default;
    oct_default_desc.unattach_func = oct_unattach_default;
    oct_default_desc.modify_func = oct_modify_default;
    oct_default_desc.unmodify_func = oct_unmodify_default;
    oct_default_desc.bb_func = oct_bb_default;
    oct_default_desc.get_by_name_func = oct_get_by_default;
    oct_default_desc.get_container_by_name_func = oct_get_container_by_default;
    oct_default_desc.gen_first_content_func = oct_gen_first_content_default;
    oct_default_desc.gen_first_container_func = oct_gen_first_container_default;
    oct_default_desc.gen_contents_func = oct_gen_contents_default;
    oct_default_desc.gen_contents_with_offset_func = oct_gen_contents_offset_default;
    oct_default_desc.gen_containers_func = oct_gen_containers_default;
    oct_default_desc.read_func = oct_read_object;
    oct_default_desc.write_func = oct_write_object;
    oct_default_desc.read_fields_func = default_read_fields;
    oct_default_desc.write_fields_func = default_write_fields;
    oct_default_desc.copy_fields_func = (int (*)()) 0;
    oct_default_desc.fix_fields_func = (int (*)()) 0;
    oct_default_desc.name_offset = ILLEGAL_OFFSET;
    oct_default_desc.bb_offset = ILLEGAL_OFFSET;
    return OCT_OK;
}

octStatus oct_create_default(desc, object, new_p)
struct facet *desc;
struct octObject *object;
generic **new_p;
{
    struct object_desc *object_desc = &oct_object_descs[object->type];
    struct generic *new =
      (struct generic *) malloc((unsigned) object_desc->internal_size);

    new->flags.type = object->type;
    CLEAR_FLAGS(new);
    new->objectId = object->objectId = oct_new_id((generic *)new);
    new->externalId = oct_next_xid(desc, (generic *) new);
    if (new->externalId == oct_null_xid) {
	oct_free_id(new->objectId);
	FREE(new);
        oct_error("oct_create_default: oct_next_xid returned oct_null_xid");
        errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
	/* return OCT_ERROR; */
    }
    new->facet = desc;
    new->containers = new->contents = NIL(struct chain);
    DO_COPY(object_desc, new, object);
    DO_FIX(object_desc, new);
    desc->object_count++;
    oct_attach_generic_list(desc->object_lists[object->type], new);
    *new_p = new;
    return OCT_OK;
}

octStatus oct_undelete_default(desc, object, new_p, old_xid, old_id)
struct facet *desc;
struct octObject *object;
generic **new_p;
int32 old_xid,old_id;
{
    struct object_desc *object_desc = &oct_object_descs[object->type];
    struct generic *new =
      (struct generic *) malloc((unsigned) object_desc->internal_size);

    new->flags.type = object->type;
    CLEAR_FLAGS(new);
	oct_set_id(old_id,(generic *)new);
    new->objectId = object->objectId = old_id;
	oct_add_xid(desc, old_xid, (generic *) new);
    new->externalId = old_xid;
    if (new->externalId == oct_null_xid)
	{
		oct_free_id(new->objectId);
		FREE(new);
        oct_error("oct_create_default: oct_next_xid returned oct_null_xid");
        errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
		/* return OCT_ERROR; */
    }
    new->facet = desc;
    new->containers = new->contents = NIL(struct chain);
    DO_COPY(object_desc, new, object);
    DO_FIX(object_desc, new);
    desc->object_count++;
    oct_attach_generic_list(desc->object_lists[object->type], new);
    *new_p = new;
    return OCT_OK;
}

octStatus
oct_unmodify_default(ptr,new)
generic *ptr;
struct octObject *new;
{
    struct generic *obj = (struct generic *)ptr;
    struct object_desc *object_desc = &oct_object_descs[obj->flags.type];

    DO_FREE(object_desc, ptr);
    DO_COPY(object_desc, ptr, new);
    DO_FIX(object_desc, ptr);
    return OCT_OK;
}

octStatus
oct_modify_default(ptr, new)
generic *ptr;
struct octObject *new;
{
    struct generic *obj = (struct generic *)ptr;
    struct object_desc *object_desc = &oct_object_descs[obj->flags.type];

    DO_FREE(object_desc, ptr);
    DO_COPY(object_desc, ptr, new);
    DO_FIX(object_desc, ptr);
    return OCT_OK;
}

octStatus
oct_free_default(obj)
generic *obj;
{
    struct object_desc *object_desc = &oct_object_descs[obj->flags.type];

    DO_FREE(object_desc, obj);
    oct_detach_all_chains(obj->contents);
    FREE(obj);
    return OCT_OK;
}

/*ARGSUSED*/
octStatus
oct_uncreate_default(obj, by_user)
generic *obj;
int by_user;
{
    struct object_desc *object_desc = &oct_object_descs[obj->flags.type];
    octStatus retval;

    DO_FREE(object_desc, obj);
    
    oct_detach_generic_list(obj->facet->object_lists[obj->flags.type], obj);

    retval = oct_delete_contents(obj);
    
    if (obj->facet->facet_flags.all_loaded) {
	oct_free_id(obj->objectId);
	oct_free_xid(obj->facet, obj->externalId);
    }
    return retval;
}


/*ARGSUSED*/
octStatus oct_delete_default(obj, by_user)
generic *obj;
int by_user;
{
    struct object_desc *object_desc = &oct_object_descs[obj->flags.type];
    octStatus retval;

    DO_FREE(object_desc, obj);
    
    oct_detach_generic_list(obj->facet->object_lists[obj->flags.type], obj);

    retval = oct_delete_contents(obj);
    
    if (obj->facet->facet_flags.all_loaded) {
	oct_free_id(obj->objectId);
	oct_free_xid(obj->facet, obj->externalId);
    }
    return retval;
}


static int
default_read_fields(obj)
generic *obj;
{
    obj->contents = NIL(struct chain);
    return oct_get_props(obj) && oct_get_contents(obj);
}

static int
default_write_fields(obj)
generic *obj;
{
    /* XXX fully getting rid of proplist requires format change */
    return oct_put_props() && oct_put_contents(obj->contents);
}

/*
 * write out the generic fields in a object (the disk id, the type and 
 * the external id) and then the object specific fields
 */

octStatus
oct_write_object(obj)
generic *obj;
{
    int32 id;
    struct object_desc *object_desc = &oct_object_descs[obj->flags.type];

    if (obj->flags.is_temporary) {
	return OCT_OK;
    }

    (void) oct_put_nl();
    
    if (!oct_lookup_mark(obj, &id)) {
   	id = oct_next_dump_id++;
	oct_mark_ptr(obj, id);
    }

    return oct_put_type((int)obj->flags.type) &&
      oct_put_32(id) &&
      oct_put_32(obj->externalId) &&
      (*object_desc->write_fields_func)(obj);
}
    
int
oct_read_object(desc, type, new)
struct facet *desc;
int type;
generic **new;
{
    struct generic *obj;
    int32 id;
    int val;

    GET_ID_AND_PTR(id, obj, type);
    
    if (!oct_get_32(&obj->externalId)) {
	return 0;
    }
    oct_add_xid(desc, obj->externalId, obj);
    
    obj->flags.type = type;
    obj->facet = desc;
    obj->objectId = id;
    
    val = (oct_object_descs[type].read_fields_func)(obj);
    
    if (!val) {
	return 0;
    } else {
	oct_attach_generic_list(desc->object_lists[type], obj);
	if (new != NIL(generic *)) {
	    *new = obj;
	}
	return 1;
    }
}

