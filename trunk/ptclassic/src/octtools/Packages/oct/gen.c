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
#include "oct_utils.h"  

/*
 * Routines to handle generation of objects attached to other objects.
 * 
 * A generator consists of a generate function, a free function and a 
 * state variable.  The standard contents and containers generate 
 * functions use a pointer to one link in the attachement chain as 
 * part of the state of the generator.  If the object associated with 
 * that chain is deleted, then that pointer could be left dangling. To 
 * avoid this, on deletions, generators are searched to see if the
 * object was on the list.  This assumes that deletes are done less
 * frequently than generators and that when a delete it done there
 * are only a few active generators.
 */
  
static char *gen_done_string = "Nothing more to generate";

octStatus
octInitGenContents(object, mask, generator)
octObject *object;
octObjectMask mask;
octGenerator *generator;
{
    generic *ptr;
    octStatus retval;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error(
	    "octInitGenContents: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval =
	  (*oct_object_descs[ptr->flags.type].gen_contents_func)(ptr, mask,
								   generator);

    if (retval < OCT_OK) {
	oct_prepend_error("octInitGenContents: ");
    }
    return retval;
}

octStatus
octInitGenContentsWithOffset(object, mask, generator, offset)
octObject *object;
octObjectMask mask;
octGenerator *generator;
octObject *offset;
{
    generic *ptr, *offset_ptr;
    octStatus retval;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error(
	    "octInitGenContentsOffset: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
    
    offset_ptr = oct_id_to_ptr(offset->objectId);
    if (offset_ptr == NIL(generic)) {
	oct_prepend_error("octInitGenContents: ");
	return( OCT_NOT_FOUND );
    }

    retval = (*oct_object_descs[ptr->flags.type].gen_contents_with_offset_func)(ptr, offset_ptr, mask, generator);

    if (retval < OCT_OK) {
	oct_prepend_error("octInitGenContents: ");
    }
    return retval;
}

octStatus
octInitGenContainers(object, mask, generator)
octObject *object;
octObjectMask mask;
octGenerator *generator;
{
    generic *ptr;
    octStatus retval;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error(
	   "octInitGenContainers: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval =
      (*oct_object_descs[ptr->flags.type].gen_containers_func)(ptr, mask,
								   generator);

    if (retval < OCT_OK) {
	oct_prepend_error("octInitGenContainers: ");
    }
    return retval;
}

octStatus octFreeGenerator(gen)
octGenerator *gen;
{
    return (*gen->free_func)(gen);
}

octStatus
octGenFirstContainer(object, mask, container)
octObject *object;
octObjectMask mask;
octObject *container;
{
    generic *ptr;
    octStatus retval;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error(
	    "octGenFirstContainer: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval =
  (*oct_object_descs[ptr->flags.type].gen_first_container_func)(ptr, mask, container);

    if (retval < OCT_OK) {
	oct_prepend_error("octGenFirstContainer: ");
    }
    return retval;
}

octStatus
octGenFirstContent(object, mask, content)
octObject *object;
octObjectMask mask;
octObject *content;
{
    generic *ptr;
    octStatus retval;

    ptr = oct_id_to_ptr(object->objectId);
    if (ptr == NIL(generic)) {
	oct_error(
	    "octGenFirstContent: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval =
  (*oct_object_descs[ptr->flags.type].gen_first_content_func)(ptr, mask, content);

    if (retval < OCT_OK) {
	oct_prepend_error("octGenFirstContent: ");
    }
    return retval;
}

/* 
 * the follow 2 functions are wrapper functions that allow user 
 * defined generators to be used identically as oct generators.
 * They hide the internal struct of octGenerator from the user-generator 
 * as well handling some of the messy end cases such as: 
 * octFreeGenerator being called twice, octGenerate being called after 
 * OCT_GEN_DONE had been returned.
 */
struct user_state {
    octStatus (*user_gen)();
    octStatus (*user_free)();
    char *user_state;
};

static octStatus
gen_user_gen_func(gen, obj)
octGenerator *gen;
octObject *obj;
{
    struct user_state *my_state = (struct user_state *) gen->state;

    if (my_state == NIL(struct user_state)) {
	return OCT_GEN_DONE;
    } else {
	int retval = (*my_state->user_gen)(&my_state->user_state, obj);
	if (retval == OCT_GEN_DONE) {
	    (void) octFreeGenerator(gen);
	}
	return retval;
    }
}

static octStatus
gen_user_free_func(gen)
octGenerator *gen;
{
    struct user_state *my_state = (struct user_state *) gen->state;

    if (my_state == NIL(struct user_state)) { 
	return OCT_OK;
    } else {
	int retval = (*my_state->user_free)(my_state->user_state);
	FREE(my_state);
	gen->state = NIL(char);
	return retval;
    }
}

octStatus
octInitUserGen(user_state, gen_func, free_func, gen)
char *user_state;
octStatus (*gen_func)();
octStatus (*free_func)();
octGenerator *gen;
{
    struct user_state *my_state = ALLOC(struct user_state, 1);

    my_state->user_gen = gen_func;
    my_state->user_free = free_func;
    my_state->user_state = user_state;
    
    gen->state = (char *)my_state;
    gen->generator_func = gen_user_gen_func;
    gen->free_func = gen_user_free_func;
    return OCT_OK;
}

octStatus
oct_gen_first_content_default(ptr, mask, obj)
generic *ptr;
octObjectMask mask;
octObject *obj;
{
    struct chain *chain;
    
    if (mask & OCT_TERM_MASK) {
	mask |= OCT_FORMAL_MASK;
    }
    
    chain = ptr->contents;
    while (chain != NIL(struct chain)) {
	if (IN_MASK(chain->object->flags.type, mask)) {
	    oct_fill_object(obj, chain->object);
	    return OCT_OK;
	}
	chain = chain->last;
    }
    
    oct_error_string = gen_done_string;
    return OCT_GEN_DONE;
}

octStatus
oct_gen_first_container_default(ptr, mask, obj)
generic *ptr;
octObjectMask mask;
octObject *obj;
{
    struct chain *head;

    if (mask & OCT_TERM_MASK) {
	mask |= OCT_FORMAL_MASK;
    }
    
    head = ptr->containers;

    while (head != NIL(struct chain)) {
	if (IN_MASK(head->container->flags.type, mask)) {
	    oct_fill_object(obj, head->container);
	    return OCT_OK;
	}
	head = head->next_chain;
    }
    
    oct_error_string = gen_done_string;
    return OCT_GEN_DONE;
}

static octStatus
contents_free_func(gen)
octGenerator *gen;
{
    struct generator *generator = (struct generator *) gen->state;

    if (generator == NIL(struct generator)) {
	return OCT_OK;
    }
    
    DLL_DETACH(generator->object->facet->generators,generator,
	       struct generator,next_gen,last_gen);
    FREE(generator);
    gen->state = NIL(char);
    return OCT_OK;
}


/*
 * When link current pointed to by generator->next is about to be deleted,
 * contents_fix is used to move generator->next one forward.
 */
static int
contents_fix(generator)
struct generator *generator;
{
    if (generator->next != NIL(struct chain)) {
	generator->next = generator->next->last;
    }
    return 1;
}
    
static octStatus
contents_gen(gen, obj)
octGenerator *gen;
octObject *obj;
{
    struct generator *generator = (struct generator *) gen->state;
    struct chain *next;
    octObjectMask mask;

    if (generator == NIL(struct generator)) {
	oct_error_string = gen_done_string;
	return OCT_GEN_DONE;
    }
    
    next = generator->next;
    mask = generator->mask;

    while (next != NIL(struct chain) && !IN_MASK(next->object->flags.type, mask)) {
	next = next->last;
    }
    
    if (next == NIL(struct chain)) {
	/*
	 * we're back at the start, so we've
	 * generated everything on this chain.
	 */
	DLL_DETACH(generator->object->facet->generators,generator,
		   struct generator,next_gen,last_gen);
	FREE(generator);
	gen->state = NIL(char);
	oct_error_string = gen_done_string;
	return OCT_GEN_DONE;
    }
    
    generator->next = next->last;
    
    oct_fill_object(obj, next->object);
    return OCT_OK;
}
     
static octStatus
containers_free_func(gen)
octGenerator *gen;
{
    struct generator *generator = (struct generator *) gen->state;

    if (generator == NIL(struct generator)) {
	return OCT_OK;
    }
    
    DLL_DETACH(generator->object->facet->generators,generator,
	       struct generator,next_gen,last_gen);
    FREE(generator);
    gen->state = NIL(char);
    return OCT_OK;
}

/*
 * When the link current pointed to by generator->next is about to be deleted,
 * containers_fix is used to move generator->next one link forward forward
 */
static int
containers_fix(generator)
struct generator *generator;
{
    if (generator->next != NIL(struct chain)) {
	generator->next = generator->next->next_chain;
    }
    return 1;
}
    
static octStatus
containers_gen(gen, obj)
octGenerator *gen;
octObject *obj;
{
    struct generator *generator = (struct generator *) gen->state;
    struct chain *next;
    octObjectMask mask;

    if (generator == NIL(struct generator)) {
	oct_error_string = gen_done_string;
	return OCT_GEN_DONE;
    }
    
    next = generator->next;
    mask = generator->mask;
    
    while (next != NIL(struct chain) &&
	   !IN_MASK(next->container->flags.type, mask)) {
	next = next->next_chain;
    }

    if (next == NIL(struct chain)) {
	DLL_DETACH(generator->object->facet->generators,generator,
		   struct generator,next_gen,last_gen);
	FREE(generator);
	gen->state = NIL(char);
	oct_error_string = gen_done_string;
	return OCT_GEN_DONE;
    }
	
    generator->next = next->next_chain;
	
    oct_fill_object(obj, next->container);
    return OCT_OK;
}

octStatus
oct_gen_contents_default(ptr, mask, gen)
generic *ptr;
octObjectMask mask;
octGenerator *gen;
{
    struct generator *generator;
    struct chain *next;

    if (mask & OCT_TERM_MASK) {
	mask |= OCT_FORMAL_MASK;
    }
    
    if (ptr->contents != NIL(struct chain)) {
	next = ptr->contents;
    } else {
	/* nothing to generate */
	next = NIL(struct chain);
    }

    if (next != NIL(struct chain)) {
	
	generator = ALLOC(struct generator, 1);
	generator->next = next;
	generator->mask = mask;
	generator->fix_func = contents_fix;
	generator->object = ptr;
	gen->state = (char *)generator;
	
	DLL_ATTACH(ptr->facet->generators,generator,
		   struct generator,next_gen,last_gen);
    } else {
	gen->state = NIL(char);
    }
	
    
    gen->generator_func = contents_gen;
    gen->free_func = contents_free_func;
    return OCT_OK;
}

octStatus
oct_gen_contents_offset_default(ptr, offset_ptr, mask, gen)
generic *ptr;
generic *offset_ptr;
octObjectMask mask;
octGenerator *gen;
{
    struct generator *generator;
    struct chain *next;

    if (mask & OCT_TERM_MASK) {
	mask |= OCT_FORMAL_MASK;
    }
    
    for(next = offset_ptr->containers; next != NIL(struct chain) && next->container != ptr; next = next->next_chain) ;
    	
    gen->generator_func = contents_gen;
    gen->free_func = contents_free_func;

    if (next != NIL(struct chain)) {
	generator = ALLOC(struct generator, 1);
	generator->next = next;
	generator->mask = mask;
	generator->fix_func = contents_fix;
	generator->object = ptr;
	gen->state = (char *)generator;
	
	DLL_ATTACH(ptr->facet->generators,generator,
		   struct generator,next_gen,last_gen);
    } else {
	gen->state = NIL(char);
	return OCT_NOT_FOUND;
    }
    
    return OCT_OK;
}

octStatus
oct_gen_containers_default(ptr, mask, gen)
generic *ptr;
octObjectMask mask;
octGenerator *gen;
{
    struct generator *generator;

    if (mask & OCT_TERM_MASK) {
	mask |= OCT_FORMAL_MASK;
    }
    
    if (ptr->containers != NIL(struct chain)) {
	
	generator = ALLOC(struct generator, 1);
	
	generator->mask = mask;
	generator->next = ptr->containers;
	generator->fix_func = containers_fix;
	generator->object = ptr;
	
	gen->state = (char *) generator;
	
	DLL_ATTACH(ptr->facet->generators,generator,
		   struct generator,next_gen,last_gen);
	
    } else {
	gen->state = NIL(char);
    }

    gen->generator_func = containers_gen;
    gen->free_func = containers_free_func;
    
    return OCT_OK;
}
    

/* The code for the backward generator is here */

static octStatus 
backgen(state,obj)
struct chain  **state;
octObject *obj;
{
	struct chain *chain = *state;
	struct chain *next;
	char *gen_done_string = "Nothing more to generate";
	
	if( chain == NIL(struct chain) )
	{
		oct_error_string = gen_done_string;
		return OCT_GEN_DONE;
	}
	next = chain;

	if( next->next->last == NIL(struct chain) )
	{
		oct_error_string = gen_done_string;
		return OCT_GEN_DONE;
	}
	
	*state = next->next;
	oct_fill_object(obj,next->object);
	
	/* not really necc. -- the traverser can check --
	   if( obj->contents.changeRecord.changeType == OCT_MARK )
	   {
	   oct_error_string = gen_done_string;
	   return OCT_GEN_DONE;
	   }
	   */
	return OCT_OK;
}

static octStatus
backfree(state)
struct chain *state;
{
	return OCT_OK;
}

octStatus
octInitBackwardsGenContents(object, generator)
octObject *object;
octGenerator *generator;
{
	generic *ptr;
	struct chain *my_chain;
	ptr = oct_id_to_ptr(object->objectId);
	my_chain = ptr->contents;
	if( my_chain )
		my_chain = my_chain->next; /* get it to pt to the last guy in the list */
	return octInitUserGen((char*)my_chain, backgen, backfree, generator);
}


/* SPECIAL GENERATORS */
/* These functions are in the RPC library and they provide
 * snapshot generators. Objects can be attached and detached
 * to 'object' with no effect on the generation sequence.
 * The RPC provides this functionality for efficiency reason,
 * that is to reduce the number of round trips between
 * the client and the RPC server.
 *
 * NOTICE: 
 * These functions are just stubs provided for compatibility
 * with the RPC library. 
 * They should be implemented so as to cache the generation
 * sequence.
 */

octStatus
octInitGenContentsSpecial(object, mask, generator)
    octObject *object;
    octObjectMask mask;
    octGenerator *generator;
{
    return octInitGenContents(object, mask, generator);
}

octStatus
octInitGenContainersSpecial(object, mask, generator)
    octObject *object;
    octObjectMask mask;
    octGenerator *generator;
{
    return octInitGenContainers(object, mask, generator);
}
