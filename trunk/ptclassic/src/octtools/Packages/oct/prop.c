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

#include "io_procs.h"
#include "mark.h"
#include "oct_utils.h"

#include "prop.h"

static int prop_write_fields(), prop_read_fields(), prop_free_fields();
static int prop_copy_fields();

extern struct object_desc oct_default_desc;
static struct object_desc *super = &oct_default_desc;

void
oct_prop_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct prop);
    object_desc->external_size = sizeof(struct octProp);
    object_desc->flags |= HAS_NAME;
    object_desc->contain_mask = (octObjectMask) OCT_PROP_MASK|OCT_BAG_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct prop, user_prop);
    object_desc->name_offset = FIELD_OFFSET(struct octProp, name);
    object_desc->write_fields_func = prop_write_fields;
    object_desc->read_fields_func = prop_read_fields;
    object_desc->free_fields_func = prop_free_fields;
    object_desc->copy_fields_func = prop_copy_fields;
}

/*ARGSUSED*/
static octStatus
prop_copy_fields(orig_p, copy_p, size)
char *orig_p, *copy_p;
int size;
{
    struct octProp *copy = (struct octProp *) copy_p;
    struct octProp *orig = (struct octProp *) orig_p;

    *copy = *orig;

    copy->name = oct_str_intern(copy->name);

    switch (copy->type) {
    case OCT_STRING:
	copy->value.string = oct_str_intern(copy->value.string);
	break;
    case OCT_STRANGER:
	{
	    int32 length = copy->value.stranger.length;
	    char *tmp, *contents = copy->value.stranger.contents;
	    
	    if (length != 0) {
		tmp = ALLOC(char, length);
		(void) memcpy(tmp, contents, length);
		copy->value.stranger.contents = tmp;
	    } else {
		copy->value.stranger.contents = NIL(char);
	    }
	}
	break;
    case OCT_REAL_ARRAY:
	{
	    int32 length = copy->value.real_array.length;
	    double *tmp, *array = copy->value.real_array.array;
	    
	    if (length != 0) {
		tmp = ALLOC(double, length);
		(void) memcpy((char *) tmp,
			      (char *)array,
			      length*sizeof(double));
		copy->value.real_array.array = tmp;
	    } else {
		copy->value.real_array.array = NIL(double);
	    }
	}
	break;
    case OCT_INTEGER_ARRAY:
	{
	    int32 length = copy->value.integer_array.length;
	    int32 *tmp, *array = copy->value.integer_array.array;
	    
	    if (length != 0) {
		tmp = ALLOC(int32, length);
		(void) memcpy((char *) tmp,
			      (char *)array,
			      length*sizeof(int32));
		copy->value.integer_array.array = tmp;
	    } else {
		copy->value.integer_array.array = NIL(int32);
	    }
	}
	break;
    case OCT_INTEGER:
    case OCT_REAL:
    case OCT_NULL:
    case OCT_ID:
	break;
    default :
	oct_error("Unknown property type, %d", copy->type);
	return OCT_ERROR;
    }
    
    return OCT_OK;
}

static octStatus
prop_free_fields(pptr)
generic *pptr;
{
    struct prop *prop = (struct prop *) pptr;

    oct_str_free(prop->user_prop.name);
    
    switch (prop->user_prop.type) {
    case OCT_STRING :
      oct_str_free(prop->user_prop.value.string);
      break;
    case OCT_STRANGER :
      FREE(prop->user_prop.value.stranger.contents);
      break;
    case OCT_REAL_ARRAY :
      FREE(prop->user_prop.value.real_array.array);
      break;
    case OCT_INTEGER_ARRAY :
      FREE(prop->user_prop.value.integer_array.array);
      break;
    case OCT_NULL :
    case OCT_INTEGER :
    case OCT_REAL :
    case OCT_ID :
      break;
    }

    return OCT_OK;
}

static octStatus
prop_write_fields(ptr)
generic *ptr;
{
    struct prop *prop = (struct prop *) ptr;
    int val;
    
    val = (*super->write_fields_func)(ptr) &&
      oct_put_string(prop->user_prop.name) &&
	oct_put_byte((int)prop->user_prop.type);

    if (!val) {
	return 0;
    }
    
    switch (prop->user_prop.type) {
    case OCT_INTEGER:
	if (!oct_put_32(prop->user_prop.value.integer)) {
	    return 0;
	}
	break;
    case OCT_REAL:
	if (!oct_put_double(prop->user_prop.value.real)) {
	    return 0;
	}
	break;
    case OCT_STRING:
	if (!oct_put_string(prop->user_prop.value.string)) {
	    return 0;
	}
	break;
    case OCT_ID:
	{
	    generic *optr = oct_id_to_ptr(prop->user_prop.value.id);
	    octId id;
	    
	    if (optr == NIL(generic)) {
		if (!oct_put_32((octId)0)) {
		    return 0;
		}
	    } else {
		PTR_TO_DUMP_ID(optr, id);
		if (!oct_put_32(id)) {
		    return 0;
		}
	    }
	    break;
	}
    case OCT_STRANGER:
	if (!oct_put_bytes(prop->user_prop.value.stranger.length,
			   prop->user_prop.value.stranger.contents)) {
			       return 0;
			   }
	break;
    case OCT_REAL_ARRAY:
	{
	    int32 length = prop->user_prop.value.real_array.length;
	    double *ptr = prop->user_prop.value.real_array.array;
	    
	    if (!oct_put_32(length)) {
		return 0;
	    }
	    for(;length > 0;length--,ptr++) {
		if (!oct_put_double(*ptr)) {
		    return 0;
		}
	    }
	}
	break;
    case OCT_INTEGER_ARRAY:
	{
	    int32 length = prop->user_prop.value.integer_array.length;
	    int32 *ptr = prop->user_prop.value.integer_array.array;
	    
	    if (!oct_put_32(length)) {
		return 0;
	    }
	    for(;length > 0;length--,ptr++) {
		if (!oct_put_32(*ptr)) {
		    return 0;
		}
	    }
	}
	break;
    case OCT_NULL :
	break;
    default :
	oct_error("Corrupted property type");
	return 0;
    }
    
    return 1;
}

static octStatus
prop_read_fields(ptr)
generic *ptr;
{
    int val, type;
    struct prop *prop = (struct prop *) ptr;
    
    val = (*super->read_fields_func)(ptr) &&
      oct_get_string(&prop->user_prop.name) &&
	oct_get_byte(&type);
    
    if (!val) {
	return 0;
    }
    
    prop->user_prop.type = (octPropType) type;
    
    switch ((octId) type) {
    case OCT_INTEGER:
	if (!oct_get_32(&prop->user_prop.value.integer)) {
	    return 0;
	}
	break;
    case OCT_REAL:
	if (!oct_get_double(&prop->user_prop.value.real)) {
	    return 0;
	}
	break;
    case OCT_STRING:
	if (!oct_get_string(&prop->user_prop.value.string)) {
	    return 0;
	}
	break;
    case OCT_ID:
	{
	    octId id;
	    if (!oct_get_32(&id)) {
		return 0;
	    }
	    prop->user_prop.value.id = id + oct_id_offset;
	}
    case OCT_STRANGER:
	if (!oct_get_bytes(&prop->user_prop.value.stranger.length,
			   &prop->user_prop.value.stranger.contents)) {
			       return 0;
			   }
	break;
    case OCT_REAL_ARRAY:
	{
	    int32 length;
	    double *ptr;
	    
	    if (!oct_get_32(&length)) {
		return 0;
	    }
	    prop->user_prop.value.real_array.length = length;
	    if (length == 0) {
		prop->user_prop.value.real_array.array = NIL(double);
	    } else {
		ptr = ALLOC(double, length);
		prop->user_prop.value.real_array.array = ptr;
		if (ptr == NIL(double)) {
		    return 0;
		}
		for(;length > 0;length--,ptr++) {
		    if (!oct_get_double(ptr)) {
			return 0;
		    }
		}
	    }
	}
	break;
    case OCT_INTEGER_ARRAY:
	{
	    int32 length;
	    int32 *ptr;
	    
	    if (!oct_get_32(&length)) {
		return 0;
	    }
	    prop->user_prop.value.integer_array.length = length;
	    if (length == 0) {
		prop->user_prop.value.integer_array.array = NIL(int32);
	    } else {
		ptr = ALLOC(int32, length);
		prop->user_prop.value.integer_array.array = ptr;
		if (ptr == NIL(int32)) {
		    return 0;
		}
		for(;length > 0;length--,ptr++) {
		    if (!oct_get_32(ptr)) {
			return 0;
		    }
		}
	    }
	}
	break;
    case OCT_NULL :
	break;
    default :
	oct_error("Corrupted property type");
	return 0;
    }
    
    return 1;
}

