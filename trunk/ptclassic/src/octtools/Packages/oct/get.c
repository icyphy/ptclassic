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
#include "create.h"
#include "modify.h"
#include "oct_utils.h"
#include "get.h"

/* Forward references (Should these be static?)*/
int oct_do_get_by_name
	ARGS((generic *cptr, octObject *object, generic **optr_p));
int oct_do_get_container_by_name
	ARGS((generic *optr, octObject *container, generic **cptr_p));

  /* implement the various oct `get' funcitons  */

octStatus
octGetByName(container, object)
octObject *container;
octObject *object;
{
    generic *cptr, *optr;
    int retval;

    cptr = oct_id_to_ptr(container->objectId);

    if (cptr == NIL(generic)) {
	oct_error("OctGetByName: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_get_by_name(cptr, object, &optr);

    if (retval >= OCT_OK) {
	oct_fill_object(object, optr);
    } else if (retval == OCT_NOT_FOUND) {
    /* Print object name for debugging purposes */
    /*	int type = object->type;
	int object_offset = oct_object_descs[type].name_offset;
	char *target = *(char **)((char *)&object->contents + object_offset);
	fprintf(stderr, "Object: %s\n", target); */
	oct_error_string = "octGetByName: No object with given name was found";
    } else {
	oct_prepend_error("octGetByName: ");
    }

    return retval;
}



octStatus
octGetContainerByName(container, object)
octObject *container;
octObject *object;
{
    generic *cptr, *optr;
    int retval;

    cptr = oct_id_to_ptr(container->objectId);

    if (cptr == NIL(generic)) {
	oct_error("OctGetContainerByName: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_get_container_by_name(cptr, object, &optr);

    if (retval < OCT_OK) {
	oct_prepend_error("octGetContainerByName: ");
    } else {
	oct_fill_object(object, optr);
    }

    return retval;
}

int
oct_do_get_by_name(cptr, object, optr_p)
generic *cptr;
octObject *object;
generic **optr_p;
{
    int retval;
    int otype, ctype;
    
    ctype = cptr->flags.type;
    
    if (ctype <= OCT_UNDEFINED_OBJECT || ctype > OCT_MAX_TYPE) {
	oct_error("Container is of an undefined type");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    otype = object->type;

    if (otype <= OCT_UNDEFINED_OBJECT || otype > OCT_MAX_TYPE) {
	oct_error("Object is of an undefined type");
	errRaise(OCT_PKG_NAME, OCT_ERROR, "Object is of an undefined type");
    }
    
    if (!IN_MASK(otype, oct_object_descs[ctype].contain_mask)) {
	oct_error("Objects of type %s cannot contain objects of type %s",
		  oct_type_names[ctype], oct_type_names[otype]);
	return OCT_ERROR;
    }

    if (!oct_object_descs[otype].flags&HAS_NAME) {
	oct_error("Objects of type %s cannot be retrieved by name",
		  oct_type_names[otype]);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
	
    retval = (*oct_object_descs[ctype].get_by_name_func)(cptr,object,optr_p); 

    return retval;
}

int
oct_do_get_container_by_name(optr, container, cptr_p)
generic *optr;
octObject *container;
generic **cptr_p;
{
    int retval;
    int otype, ctype;
    
    ctype = container->type;
    
    if (ctype <= OCT_UNDEFINED_OBJECT || ctype > OCT_MAX_TYPE) {
	oct_error("Container is of an undefined type");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    otype = optr->flags.type;

    if (otype <= OCT_UNDEFINED_OBJECT || otype > OCT_MAX_TYPE) {
	oct_error("Object is of an undefined type");
	errRaise(OCT_PKG_NAME, OCT_ERROR, "Object is of an undefined type");
    }
    
    if (!IN_MASK(otype, oct_object_descs[ctype].contain_mask)) {
	oct_error("Objects of type %s cannot contain objects of type %s",
		  oct_type_names[ctype], oct_type_names[otype]);
	return OCT_ERROR;
    }

    if (!oct_object_descs[ctype].flags&HAS_NAME) {
	oct_error("Objects of type %s cannot be retrieved by name",
		  oct_type_names[ctype]);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
	
    retval =
  (*oct_object_descs[ctype].get_container_by_name_func) (optr,container,cptr_p); 

    return retval;
}

int
octGetOrCreate(container, object)
octObject *container;
octObject *object;
{
    generic *cptr, *optr;
    int retval;

    cptr = oct_id_to_ptr(container->objectId);

    if (cptr == NIL(generic)) {
	oct_error("OctGetOrCreate: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_get_by_name(cptr, object, &optr);

    if (retval < OCT_OK && retval != OCT_NOT_FOUND) {
	oct_prepend_error("octGetOrCreate: ");
	return retval;
    }
    
    switch (retval) {
    case OCT_OK:
	oct_fill_object(object, optr);
	return retval;
    case OCT_NOT_FOUND:
	retval = oct_do_create_and_attach(cptr, object, &optr);
	if (retval < OCT_OK) {
	    oct_prepend_error("octGetOrCreate: ");
	}
	return retval;
    default:
	oct_error("Panic: octGetOrCreate: Unknown return value from getbyname");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    /*NOTREACHED*/
    return OCT_ERROR;
}


/* XXX hack for oct_add_to_error_bag */
int
oct_get_or_create(cptr, object)
generic *cptr;
octObject *object;
{
    generic *optr;
    int retval;

    retval = oct_do_get_by_name(cptr, object, &optr);

    if (retval < OCT_OK && retval != OCT_NOT_FOUND) {
	oct_prepend_error("oct_get_or_create: ");
	return retval;
    }
    
    switch (retval) {
    case OCT_OK:
	oct_fill_object(object, optr);
	return retval;
    case OCT_NOT_FOUND:
	retval = oct_do_create_and_attach(cptr, object, &optr);
	if (retval < OCT_OK) {
	    oct_prepend_error("oct_get_or_create: ");
	}
	return retval;
    default :
	oct_error("Panic: oct_get_or_create: Unknown return value from getbyname");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    /*NOTREACHED*/
    return OCT_ERROR;
}


octStatus
octCreateOrModify(container, object)
octObject *container;
octObject *object;
{
    generic *cptr, *optr;
    octStatus retval;

    cptr = oct_id_to_ptr(container->objectId);

    if (cptr == NIL(generic)) {
	oct_error("OctGetOrCreate: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_get_by_name(cptr, object, &optr);

    if (retval < OCT_OK && retval != OCT_NOT_FOUND) {
	oct_prepend_error("octCreateOrModify: ");
	return retval;
    }

    switch (retval) {
    case OCT_OK:
	object->objectId = optr->objectId;
	return oct_do_modify(optr, object);
	
    case OCT_NOT_FOUND: 
	return oct_do_create_and_attach(cptr, object, &optr);
    default :
	oct_error("Panic: octGetOrModify: Unknown return value from getbyname");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    /*NOTREACHED*/
    return OCT_ERROR;
}

octStatus
octGetById(object)
octObject *object;
{
    generic *ptr;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error("OctGetById: No object with the given ID exists");
	return OCT_NOT_FOUND;
    }
    if (ptr->flags.type == OCT_UNDEFINED_OBJECT ||
	ptr->flags.type > OCT_MAX_TYPE) {
	oct_error("OctGetById: Argument is of an undefined type");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    oct_fill_object(object, ptr);
    
    return OCT_OK;
}

void
octGetFacet(object, user_facet)
octObject *object;
octObject *user_facet;
{
    generic *ptr;
    struct facet *facet;

    ptr = oct_id_to_ptr(object->objectId);
    
    if (ptr == NIL(generic)) {
	oct_error("OctGetFacet: The argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
    
    if (ptr->flags.type == OCT_UNDEFINED_OBJECT ||
	ptr->flags.type > OCT_MAX_TYPE) {
	oct_error("OctGetFacet: Argument is of an undefined type");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    facet = ptr->facet;
    
    user_facet->contents.facet = facet->user_facet;
    user_facet->objectId = facet->objectId;
    user_facet->type = OCT_FACET;
    
    return;
}
