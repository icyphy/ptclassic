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
#include "chain.h"
#include "change_record.h"
#include "io_procs.h"
#include "mark.h"
#include "oct_utils.h"
#include "ref.h"

/* 
 * This file contains the functions that maintains the attachement 
 * chains in oct.  Every attachment is represented by a chain 
 * structure, which points to the object and its container. In 
 * addition the chain structure is threaded on a singlely linked list 
 * (through the next_chain field) off of the `containers' pointer of the
 * object and is also threaded on a circular doubly linked list 
 * (through the next and last fields) off of the `contents' pointer of the 
 * container.  As a final touch, the circular doubly linked list is terminated
 * in the last direction by a nil link (if `head' is the start of the list,
 * head->last->next == head, but head->next->last = nil).  This allows all
 * elements contained by an object to be processed (in the order they were
 * attached) in a loop like:
 * 
 * 	for (ptr = object->contents; ptr != NIL(struct chain); ptr = ptr->last) {
 *		process_object(ptr->object);
 * 	}
 * 	
 * The structure is only singly linked in the containers direction 
 * based on an assuption that an object will have few containers and 
 * an (probably ill-advised) attempt to save memory.
 */
  
static char *not_found_message = "No object with that name was found";

octStatus
oct_attach_chain(container, chain, object, only_once)
generic *container;
struct chain **chain;
generic *object;
int only_once;
{
    struct chain *link, *ptr;

    /* is object already attached to container? */

    if (only_once) {
	ptr = object->containers;
	for(;ptr != NIL(struct chain); ptr = ptr->next_chain) {
	    if (ptr->container == container) {
		return OCT_ALREADY_ATTACHED;
	    }
	}
    }
    
    link = ALLOC(struct chain, 1);
    /* initialize the object */
    link->next = NIL(struct chain);
    link->last = NIL(struct chain);
    link->next_chain = NIL(struct chain);
    link->object = NIL(generic);
    link->container = NIL(generic);

    link->object = object;
    link->next_chain = object->containers;
    object->containers = link;
    DLL_ATTACH(*chain, link, struct chain, next, last);
    link->container = container;
    return OCT_OK;
}

octStatus
oct_detach_chain(chain, container, object)
struct chain **chain;
generic *container;
generic *object;
{
    struct chain *ptr, *last;

    ptr = object->containers;
    
    last = NIL(struct chain);

        /* Find the link by which `object' is attached to `container' */
    
    while (ptr != NIL(struct chain) && ptr->container != container) {
	last = ptr;
	ptr = ptr->next_chain;
    }

    if (ptr == NIL(struct chain)) {
	return OCT_NOT_ATTACHED;
    }

        /* detach the link from the object's list of containers */
    
    if (last == NIL(struct chain)) {
	object->containers = ptr->next_chain;
    } else {
	last->next_chain = ptr->next_chain;
    }

    if (!oct_fix_generators(ptr, object)) {
	return OCT_ERROR;
    }
    
       /* detach the link from the container's list of objects */

    DLL_DETACH(*chain, ptr, struct chain, next, last);
    
    FREE(ptr);
    return OCT_OK;
}

/*
 * disconnect chain with no regard for the cross links, only usable when
 * freeing the entire facet! (leaves lots of dangling ptrs)
 */

void
oct_detach_all_chains(chain)
struct chain *chain;
{
    struct chain *ptr, *tmp;

    ptr = chain;

    if (ptr != NIL(struct chain)) {
	while (ptr != NIL(struct chain)) {
	    tmp = ptr->last;
	    FREE(ptr);
	    ptr = tmp;
	}
    }
    return;
}


octStatus
oct_delete_contents(object)
generic *object;
{
    struct chain *ptr, *tmp, *last, *cptr;
    int retval;
    /*
     * First disconnect the object from all containers, and free the chain
     * links used.  Free a structure will change the contents of the structure
     * in some mallocs, so care must be taken to save the next field
     * before freeing
     */
    
    ptr = object->containers;

    while (ptr != NIL(struct chain)) {
	RECORD_DETACH(ptr->container, object, retval);
	if (retval != OCT_OK) {
	    return retval;
	}

	tmp = ptr->next_chain;
	DLL_DETACH(ptr->container->contents, ptr, struct chain, next, last);
	FREE(ptr);
	ptr = tmp;
    }

    /* Now untangle `object` from all the objects it contains */

    ptr = object->contents;
    while (ptr != NIL(struct chain)) {
	
	/*
	 * ptr is singly linked on next_chain, so we must do a linear
	 * search to unlink it from ptr->object.
	 */
	
	last = NIL(struct chain);
	cptr = ptr->object->containers;
	while (cptr != NIL(struct chain) && cptr != ptr) {
	    last = cptr;
	    cptr = cptr->next_chain;
	}
	if (cptr == NIL(struct chain)) {
	    oct_error("oct_delete_contents: panic: can't find cross link");
	    errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
	}
	if (last == NIL(struct chain)) {
	    ptr->object->containers = ptr->next_chain;
	} else {
	    last->next_chain = ptr->next_chain;
	}
	tmp = ptr->last;
	RECORD_DETACH(object, ptr->object, retval);
	if (retval < OCT_OK) {
	    return retval;
	}
	FREE(ptr);
	ptr = tmp;
    }

    return OCT_OK;
}


/*
 * Search the contents of the object pointed to
 * by `cptr' for an object with the same name as `object'
 */

octStatus
oct_get_by_default(cptr, object, optr_p)
generic *cptr;
octObject *object;
generic **optr_p;
{
    struct chain *chain;
    int type = object->type;
    int object_offset = oct_object_descs[type].name_offset;
    int internal_offset = oct_object_descs[type].user_offset + object_offset;
    int mask;
    char *target = *(char **)((char *)&object->contents + object_offset);
    char *name;
				  
    chain = cptr->contents;

    mask = 1 << type;
    if (type == OCT_TERM) {
	mask |= OCT_FORMAL_MASK;
    }
    
    while (chain != NIL(struct chain)) {
	if (IN_MASK(chain->object->flags.type, mask)) {
	    name = *(char **)((char *)chain->object + internal_offset);
	    if (STR_EQ(target, name)) {
		*optr_p = chain->object;
		return OCT_OK;
	    }
	}
	chain = chain->last;
    }
    oct_error_string = not_found_message;
    return OCT_NOT_FOUND;
}


    /*
     * Search the containers of the object pointed to
     * by `cptr' for an object with the same name as `object'
     */

octStatus
oct_get_container_by_default(optr, container, cptr_p)
generic *optr;
octObject *container;
generic **cptr_p;
{
    struct chain *head;
    int type = container->type;
    int object_offset = oct_object_descs[type].name_offset;
    int internal_offset = oct_object_descs[type].user_offset + object_offset;
    int mask;
    char *target = *(char **)((char *)&container->contents + object_offset);
    char *name;

    mask = 1 << type;
    if (type == OCT_TERM) {
	mask |= OCT_FORMAL_MASK;
    }
    
    head = optr->containers;

    while (head != NIL(struct chain)) {
	if (IN_MASK(head->container->flags.type, mask)) {
	    name = *(char **)((char *)head->container + internal_offset);
	    if (STR_EQ(target, name)) {
		*cptr_p = head->container;
		return OCT_OK;
	    }
	}
	head = head->next_chain;
    }
    
    oct_error_string = not_found_message;
    return OCT_NOT_FOUND;
}

/* 
 * Encode the attachement chains to disk.  The format is 
 * pairs of `type_of_the_content' `disk_id', terminated by a type of 0.
 * The type is neccessary so oct_get_chain can allocate the
 * contained object if it hasn't been created yet.
 */
int
oct_put_chain(chain)
struct chain *chain;
{
    octId id;
    struct chain *ptr;
    
    ptr = chain;
    while (ptr != NIL(struct chain)) {
	if (!ptr->object->flags.is_temporary) {
	    if (!oct_put_byte((int)ptr->object->flags.type)) {
		return 0;
	    }
	    PTR_TO_DUMP_ID(ptr->object, id);
	    if (!oct_put_32(id)) {
		return 0;
	    }
	}
	ptr = ptr->last;
    }
    
    if (!oct_put_byte(0)) {
	return 0;
    }
    return 1;
}

/* read the attachement chains for container from disk and rebuild them  */

int
oct_get_chain(container, chain_ptr)
generic *container;
struct chain **chain_ptr;
{
    int type;
    generic *ptr;
    octId id;
    int retval;
    
    if (!oct_get_byte(&type)) {
	return 0;
    }

    /* *chain_ptr = NIL(struct chain); */
    
    while (type != 0) {
	
	GET_ID_AND_PTR(id, ptr, type);
	
	retval = oct_attach_chain(container,chain_ptr, ptr, 0);
	if (retval < OCT_OK) {
	    return 0;
	}
	
	if (!oct_get_byte(&type)) {
	    return 0;
	}
	
    }
    return 1;
}
