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

#include "create.h"
#include "facet_info.h"
#include "inst.h"
#include "master.h"

#include "term_update.h"

/* Forward reference, could be static?*/
octStatus oct_update_formals
	ARGS((struct facet *contents, struct facet *interface));

static octStatus check_contents();
static octStatus check_masters();
static void create_instance_terminals
	ARGS((struct facet *desc, struct term *term));
static void create_interface_terminals
	ARGS((struct facet *desc, struct term *term));
static void delete_instance_terminals
	ARGS((struct facet *desc, struct term *term));
static void delete_interface_terminals
	ARGS((struct facet *desc, struct term *term));
static void modify_instance_terminals
	ARGS((struct term *term));
static void modify_interface_terminals
	ARGS((struct term *term));


/*ARGSUSED*/
static enum st_retval
do_check_masters(c_master, junk1, c_retval)
char *c_master;
char *junk1;
char *c_retval;
{
    struct master *master = (struct master *) c_master;
    int *retval_p = (int *) c_retval;
    struct octFacet dummy;
    struct facet *master_facet;
    int retval;
    struct instance *instance = master->instances;

    if (instance == NIL(struct instance)) {
	return ST_CONTINUE;
    }
    
    dummy.cell = master->cell;
    dummy.view = master->view;
    dummy.facet = master->facet;
    dummy.version = master->version;
    dummy.mode = NIL(char);
    
    retval = oct_get_facet_info(&dummy, &master_facet, instance->facet);
    if (retval < OCT_OK) {
	master->master = NIL(struct facet);
	retval = oct_add_to_error_bag((generic *) instance);
	*retval_p = retval;
	if (retval != OCT_INCONSISTENT) {
	    return ST_STOP;
	}
    } else {
	master->master = master_facet;
	DLL_ATTACH(master_facet->instantiations, master, struct master,
		   next, last);
	if (master->master_bb_date != master_facet->bb_date) {
	    instance = master->instances;
	    /* XXX removed and added the line below:
	       instance->facet->facet_flags.bb_invalid_instances = 1;
	       */ /*XXX - either this or keep bounding boxes on disk */
	    instance->facet->facet_flags.bb_valid = 0;
	    (void) oct_invalidate_all_instance_bbs(instance->facet);
	    while (instance != NIL(struct instance)) {
		instance->flags.bb_valid = 0;
		instance = instance->last_inst;
	    }
	}
	master->master_bb_date = master_facet->bb_date;

	/* check the contents facet */
	check_contents(master_facet);
	    
	if (master->master_formal_date != master_facet->formal_date) {
	    instance = master->instances;
	    while (instance != NIL(struct instance)) {
		retval = oct_update_terminals(master->master, instance);
		if (retval < OCT_OK) {
		    *retval_p = retval;
		    if (retval != OCT_INCONSISTENT) {
			return ST_STOP;
		    }
		}
		instance = instance->last_inst;
	    }
	}
	master->master_formal_date = master_facet->formal_date;
    }
    return ST_CONTINUE;
}

static octStatus
check_contents(desc)
struct facet *desc;
{
    struct octFacet dummy;
    struct facet *contents;
    int retval;

    /* XXX strcmp -> ptr compare */
    if (desc->user_facet.facet == oct_contents_facet_name) {
	return OCT_OK;
    }

    dummy.cell = desc->user_facet.cell;
    dummy.view = desc->user_facet.view;
    dummy.facet = oct_contents_facet_name;
    dummy.version = desc->user_facet.version;

    retval = oct_get_facet_info(&dummy, &contents, desc);
    if (retval < OCT_OK) {
	desc->contents_facet = NIL(struct facet);
	return OCT_OK;
    } else {
	/*
  	 * XXX there used to be a check if retval is OCT_ALREADY_OPEN
 	 * but oct_get_facet_info will always return OCT_ALREADY_OPEN
	 * or an error at this point in the execution.  If this were
	 * a more generally used function, then the check against
	 * OCT_ALREADY_OPEN would be to make sure the interface is only
	 * on the list once.
	 */
        desc->contents_facet = contents;
/* XXX should really make sure that the interface is not already on the
 * list, but since there are a small number of interfaces on for each
 * contents, the penalty of doing or not doing it is minimal */
	DLL_ATTACH(contents->interfaces, desc, struct facet,
		   next_interface, last_interface);
	if (desc->contents_date != contents->formal_date) {
	    return oct_update_formals(contents, desc);
	}
    }
    return OCT_OK;
}

static struct term *find_interface_formal();

static octStatus
delete_formal(interface, term)
struct facet *interface;
struct term *term;
{
    int retval;
    
    if (oct_only_trivial_attachements((generic *)term, (generic *) interface)) {
	retval = oct_do_delete((generic *) term, 0);
    } else {
	retval = oct_add_to_error_bag((generic *) term);
	if (retval == OCT_INCONSISTENT) {
	    retval = OCT_OK;
	}
    }
    return retval;
}

octStatus
oct_update_formals(contents, interface)
struct facet *contents;
struct facet *interface;
{
    struct term *term, *next;
    int retval, final_val = OCT_OK;

    term = (struct term *) contents->object_lists[OCT_FORMAL];
    
    while (term != NIL(struct term)) {
	if (term->user_term.instanceId == oct_null_id) {
	    struct term *interface_term = find_interface_formal(interface, term);
	    if (interface_term == NIL(struct term)) {
		/* a new terminal from the contents */
		retval = oct_add_interface_formal(interface, term, &interface_term);
		if (retval < OCT_OK) {
		    return retval;
		}
	    } else {
		char *old_name = term->user_term.name;
		term->user_term.name = interface_term->user_term.name;
		MODIFY_NAME(old_name, term->user_term.name);
	    }
	    interface_term->flags.is_marked = 1;
	}
	term = (struct term *) term->last;
    }

    term = (struct term *) interface->object_lists[OCT_FORMAL];
    
    while (term != NIL(struct term)) {
	next = (struct term *) term->last;
	if (!term->flags.is_marked) {
	    /* The corresponding formal has been deleted */
	    retval = delete_formal(interface, term);
	    if (retval != OCT_OK) {
		final_val = retval;
	    }
	} else {
	    term->flags.is_marked = 0;
	}
	term = next;
    }
    
    return final_val;
}

/*
 * Check all the masters in the facet to see if the corresponding masters
 * have changed. Check to see it its contents facet has changed.
 * Called by oct_do_open after everything has been read in.
 *
 * desc->facet_desc must be valid at this time (used by oct_get_facet_info)
 */

octStatus
oct_check_masters_and_contents(desc)
struct facet *desc;
{
    int retval, final_val = OCT_OK;

    /*
     * if this is an interface facet, make sure it is consistent with
     * the corresponding contents facet
     */
    if ((retval = check_contents(desc)) != OCT_OK) {
	final_val = retval;
    }

    /* 
     * make sure the instances of the facet are up to date
     */
    if ((retval = check_masters(desc)) != OCT_OK) {
	final_val = retval;
    }
    
    return final_val;
}

    
static octStatus
check_masters(desc)
struct facet *desc;
{
    int retval = OCT_OK;

    (void) st_foreach((st_table *) desc->masters, do_check_masters, (char *) &retval);
    
    return retval;
}

void
oct_create_terminal_copies(desc, new)
struct facet *desc;
struct term *new;
{
    create_instance_terminals(desc, new);
    create_interface_terminals(desc, new);
}

static void create_instance_terminals(desc, term)
struct facet *desc;
struct term *term;
{
    struct master *master = desc->instantiations;

    while(master != NIL(struct master)) {
	struct instance *instance = master->instances;
	while (instance != NIL(struct instance)) {
	    (void) oct_add_actual(instance, term);
	    instance = instance->last_inst;
	}
	master->master_formal_date = master->master->formal_date;
	master = master->last;
    }
}

static void create_interface_terminals(desc, term)
struct facet *desc;
struct term *term;
{
    struct facet *interface = desc->interfaces;

    while(interface != NIL(struct facet)) {
	/* see if the interface is in memory - not just in the tables */
	if (interface->facet_flags.all_loaded) {
	    (void) oct_add_interface_formal(interface, term, NIL(struct term *));
	    interface->contents_date = desc->formal_date;
	}
	interface = interface->last_interface;
    }
}

static struct term *find_actual(inst, formal)
struct instance *inst;
struct term *formal;
{
    struct chain *ptr = inst->contents;
    int32 xid = formal->externalId;
    
    while (ptr != NIL(struct chain)) {
	if (ptr->object->flags.type == OCT_TERM) {
	    struct term *actual = (struct term *) ptr->object;
	    if (actual->user_term.instanceId == inst->objectId &&
		actual->user_term.formalExternalId == xid) {
		return actual;
	    }
	}
	ptr = ptr->last;
    }
    
    return NIL(struct term);
}

static struct term *find_interface_formal(facet, contents_formal)
struct facet *facet;
struct term *contents_formal;
{
    int32 xid = contents_formal->externalId;
    
    struct term *formal = (struct term *) facet->object_lists[OCT_FORMAL];
    while (formal != NIL(struct term)) {
	if (formal->user_term.formalExternalId == xid) {
	    return formal;
	}
	formal = (struct term *) formal->last;
    }
    return NIL(struct term);
}

void
oct_delete_terminal_copies(desc, new)
struct facet *desc;
struct term *new;
{
    delete_instance_terminals(desc, new);
    delete_interface_terminals(desc, new);
}

static void delete_instance_terminals(desc, term)
struct facet *desc;
struct term *term;
{
    struct master *master = desc->instantiations;
    
    while (master != NIL(struct master)) {
	struct instance *instance = master->instances;
	
	while (instance != NIL(struct instance)) {
	    struct term *actual = find_actual(instance, term);

	    if (actual == NIL(struct term)) {
	        instance = instance->last_inst;
		continue;
	    }

	    (void) oct_delete_actual(instance, actual);

	    instance = instance->last_inst;
	}
	master->master_formal_date = master->master->formal_date;
	master = master->last;
    }
}
    

static void delete_interface_terminals(desc, term)
struct facet *desc;
struct term *term;
{
    struct facet *interface = desc->interfaces;
    
    while (interface != NIL(struct facet)) {
	struct term *interface_formal = find_interface_formal(interface, term);
	
	if (interface_formal == NIL(struct term)) {
	    continue;
	}
	
	(void) delete_formal(interface, interface_formal);
	/* clean up instances of the interface */
	(void) delete_instance_terminals(interface, interface_formal);
	
	interface->contents_date = desc->formal_date;
	interface = interface->last_interface;
    }
}
    
void
oct_modify_terminal_copies(new)
struct term *new;
{
    modify_instance_terminals(new);
    modify_interface_terminals(new);
}

static void modify_interface_terminals(term)
struct term *term;
{
    struct facet *contents = term->facet;
    struct facet *interface = contents->interfaces;
    
    while (interface != NIL(struct facet)) {
	char *old_name;
	struct term *formal = find_interface_formal(interface, term);
	
	if (term == NIL(struct term)) {
	    continue;
	}
	
	old_name = formal->user_term.name;
	formal->user_term.name = term->user_term.name;
	MODIFY_NAME(old_name, formal->user_term.name);

	/* clean up instances of the interface */
	(void) modify_instance_terminals(formal);
	
	interface->contents_date = contents->formal_date;
	interface = interface->last_interface;
    }
}
    
static void modify_instance_terminals(term)
struct term *term;
{
    struct master *master = term->facet->instantiations;
    
    while (master != NIL(struct master)) {
	struct instance *instance = master->instances;
	
	while (instance != NIL(struct instance)) {
	    char *old_name;
	    struct term *actual = find_actual(instance, term);
	    
	    if (term == NIL(struct term)) {
		continue;
	    }
	    
	    old_name = actual->user_term.name;
	    actual->user_term.name = term->user_term.name;
	    MODIFY_NAME(old_name, actual->user_term.name);
	    
	    instance = instance->last_inst;
	}
	master->master_formal_date = master->master->formal_date;
	master = master->last;
    }
}
