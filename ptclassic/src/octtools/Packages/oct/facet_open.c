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
#include "fsys.h"

#include "bb.h"
#include "io.h"
#include "inst.h"
#include "facet_info.h"
#include "oct_files.h"
#include "mark.h"
#include "master.h"
#include "oct_utils.h"
#include "term_update.h"

#define MASTER 0
#define FACET 1

#undef octOpenMaster

/* Forward declaration of static functions. */
static octStatus inherit_formals();


octStatus 
octOpenFacet(obj)
struct octObject *obj;
{
    struct facet *desc;
    struct octFacet *user_facet;
    octStatus retval;
    int mode = 0;

    octBegin(); /* XXX: just in case, guarenteed to be safe */
    
    if (obj->type != OCT_FACET) {
	oct_error("octOpenFacet: The argument must be an octFacet object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    user_facet = &obj->contents.facet;

    /*XXX temporary compatibility hack */
    switch (user_facet->mode[0]) {
    case 'r':
	mode = OCT_READ;
	break;
    case 'w':
	mode = OCT_OVER_WRITE;
	break;
    case 'a':
	mode = OCT_APPEND;
	break;
    default:
	oct_error("octOpenFacet: Unknown mode for facet: (%s)", user_facet->mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    switch (user_facet->mode[1]) {
    case '!':
	mode |= OCT_REVERT;
	break;
    case '\0':
	break;
    default:
	oct_error("octOpenFacet: Unknown mode for facet: (%s)", user_facet->mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
     
    if (user_facet->cell == NIL(char) || user_facet->view == NIL(char) ||
	    user_facet->facet == NIL(char)) {
	oct_error("octOpenFacet: Incompletely specified facet");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    retval = oct_do_open(user_facet, &desc, mode, 1, NIL(struct facet), 0);

    if (retval < OCT_OK) {
	oct_prepend_error("octOpenFacet: ");
	return retval;
    }

    obj->objectId = desc->objectId;
    return retval;
}


octStatus 
octOpenMaster(iobj, fobj)
struct octObject *iobj;
struct octObject *fobj;
{
    struct octFacet *user_facet = &fobj->contents.facet;
    struct instance *inst;
    struct master *master;
    struct facet *master_desc;
    struct octFacet dummy;
    int mode = 0;
    octStatus retval;

    inst = (struct instance *) oct_id_to_ptr(iobj->objectId);

    if (inst == NIL(struct instance)) {
	oct_error("octOpenMaster: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (inst->flags.type != OCT_INSTANCE) {
	oct_error("octOpenMaster: The first argument must an octInstance object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    if (fobj->type != OCT_FACET) {
	oct_error("octOpenMaster: The second argument must an octFacet object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }


    /*XXX temporary compatibility hack */
    switch (user_facet->mode[0]) {
    case 'r':
	mode = OCT_READ;
	break;
    case 'w':
	mode = OCT_OVER_WRITE;
	break;
    case 'a':
	mode = OCT_APPEND;
	break;
    default:
	oct_error("octOpenMaster: Unknown mode for facet: (%s)", user_facet->mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    switch (user_facet->mode[1]) {
    case '!':
	mode |= OCT_REVERT;
	break;
    case '\0':
	break;
    default:
	oct_error("octOpenMaster: Unknown mode for facet: (%s)", user_facet->mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    master = inst->master;

    if (master->master) {
	master_desc = master->master;
	retval = oct_do_open_with_desc(master_desc, mode, 0, MASTER);
	if (retval < OCT_OK) {
	    oct_prepend_error("octOpenMaster: ");
	    return retval;
	}
    } else {
	/* 
	 * For some reason, we couldn't open the master earlier,
	 * lets make one more try.
	 */
	dummy.cell = master->cell;
	dummy.view = master->view;
	/* moore's version has user_facet->facet */
	dummy.facet = master->facet;
	dummy.version = master->version;
	dummy.mode = user_facet->mode;

	retval = oct_do_open(&dummy, &master_desc, mode, 0, inst->facet, FSYS_CHILD|FSYS_SIBLING);

	if (retval < OCT_OK) {
	    oct_prepend_error("octOpenMaster: ");
	    return retval;
	} else if (retval == OCT_NEW_FACET) {
	    oct_error("octOpenMaster: Cannot find master for instance %s",
		      oct_facet_name(&dummy));
	    return OCT_ERROR;
	}
    }

    *user_facet = master_desc->user_facet; 
    fobj->objectId = master_desc->objectId;
    
    return retval;
}


/*
 * open a facet relative to another one, allows for the changing
 * of cell, view, and/or facet.  location is either OCT_CHILD,
 * OCT_SIBLING or both.  OCT_CHILD opens at the next level
 * down in the hierarchy (view level), SIBLING opens at the
 * current level (cell level).
 */
octStatus 
octOpenRelative(robj, fobj, location)
struct octObject *robj;
struct octObject *fobj;
int location;
{
    struct octFacet *user_facet = &fobj->contents.facet;
    struct facet *rfacet, *facet_desc;
    struct octFacet dummy;
    int mode = 0;
    octStatus retval;

    rfacet = (struct facet *) oct_id_to_ptr(robj->objectId);

    if (rfacet == NIL(struct facet)) {
	oct_error("octOpenRelative: The id of the relative facet has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    /* XXX INSTANCE eventually for octOpenMaster type operations */
    if (rfacet->flags.type != OCT_FACET) {
	oct_error("octOpenRelative: The relative facet must be of type OCT_FACET");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    if (fobj->type != OCT_FACET) {
	oct_error("octOpenRelative: The facet to the be opened must be of type OCT_FACET");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    /*XXX temporary compatibility hack */
    switch (user_facet->mode[0]) {
    case 'r':
	mode = OCT_READ;
	break;
    case 'w':
	mode = OCT_OVER_WRITE;
	break;
    case 'a':
	mode = OCT_APPEND;
	break;
    default:
	oct_error("octOpenRelative: unknown mode for facet: (%s)", user_facet->mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    switch (user_facet->mode[1]) {
    case '!':
	mode |= OCT_REVERT;
	break;
    case '\0':
	break;
    default:
	oct_error("octOpenRelative: unknown mode for facet: (%s)", user_facet->mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    /* open the new facet */
    dummy = *user_facet;

    retval = oct_do_open(&dummy, &facet_desc, mode, 1, rfacet, location);

    if (retval < OCT_OK) {
	oct_prepend_error("octOpenRelative: ");
	return retval;
    }

    *user_facet = facet_desc->user_facet; 
    fobj->objectId = facet_desc->objectId;
    
    return retval;
}

void oct_full_init_facet();

octStatus 
oct_do_open(user_facet, odesc, mode, create, parent, location)
struct octFacet *user_facet;
struct facet **odesc;
int mode, create;
struct facet *parent;
int location;
{
    struct facet *desc;
    char *file_desc;
    int need_to_free_desc = 0, retval;

    retval = oct_make_key(user_facet, &file_desc, parent, location);
    
    if (retval < OCT_OK) {
	return retval;
    }
    
    if (!oct_lookup_facet(file_desc, &desc)) {
	desc = ALLOC(struct facet, 1);
	oct_full_init_facet(user_facet, desc);
	need_to_free_desc = 1;
	
	retval = oct_save_key(file_desc, &desc->file_desc);
	if (retval != OCT_OK) {
	    FREE(desc);
	    return retval;
	}
    }

    /* 
     * At this point we have either existing facet or a properly 
     * initialized new one (include the file_desc from fsys).
     * oct_do_open_with_desc does the rest.
     */
    retval = oct_do_open_with_desc(desc, mode, create, ((parent == 0) ? FACET : MASTER));

    if (retval < OCT_OK) {
	if (need_to_free_desc) {
	    FREE(desc);
	}
    } else {
	*odesc = desc;
    }

    return retval;
}

static void oct_init_facet_in_place();

/* 
 * Do whatever is necessary to get all of desc into memory. Desc must 
 * at least be intialized and have a valid file_desc field 
 */
octStatus 
oct_do_open_with_desc(desc, mode, create, type)
struct facet *desc;
int mode, create, type;
{
    octStatus resolve_val, retval, final_val;
    char *file_handle;
    int revert = mode & OCT_REVERT;
    int full_facet_in_memory = 0, just_info_in_memory = 0, not_in_memory = 0;

    if (desc->facet_flags.all_loaded) {
	full_facet_in_memory = 1;
    } else if (desc->facet_flags.info_loaded) {
	just_info_in_memory = 1;
    } else {
	not_in_memory = 1;
    }
    
    if (full_facet_in_memory && !revert) {
	/* we already have what we need */
	if (mode == OCT_OVER_WRITE) {
	    oct_error(
	       "Cannot open for over-write a facet that is already in memory");
	    return OCT_ERROR;
	}

	if (mode == OCT_APPEND) {
	    resolve_val = oct_resolve(&desc->user_facet, mode, desc->file_desc);
	    if (resolve_val == OCT_NO_PERM) {
		return OCT_NO_PERM;
	    }
	    if ((resolve_val == OCT_ERROR) ||
		(resolve_val == OCT_NO_EXIST)) {
		errRaise(OCT_PKG_NAME, OCT_ERROR, "Oct internal error: this should not happen (oct_resolve returned %d)\n", resolve_val);
	    }
	}
	/* update mode and reference count */
	desc->mode |= mode;
	desc->user_facet.mode = oct_determine_mode(desc);
	desc->ref_count++;
	if (desc->facet_flags.old_facet) {
	    desc->facet_flags.old_facet = 0;
	    return OCT_OLD_FACET;
	}
	return OCT_ALREADY_OPEN;
    }

    if (full_facet_in_memory) {
	/* 
	 * We are reverting to the disk copy, so clear out the in 
	 * memory copy
	 */
	(void) oct_free(desc);
	oct_free_info(desc);
	oct_init_facet_in_place(desc);
        full_facet_in_memory = 0;	/* force new octId to be created */
    } else if (just_info_in_memory) {
	/* 
	 * We have just the facet-info, free that copy and read the 
	 * full facet from disk (we don't try to do incremental reads, 
	 * it isnt worth the effort, so far.
	 */
	oct_free_info(desc);
	oct_init_facet_in_place(desc);
    }
    
    resolve_val = oct_resolve(&desc->user_facet, mode, desc->file_desc);

    if (resolve_val < OCT_OK) {
	return resolve_val;
    }
	
    if (resolve_val == OCT_NEW_FACET) {
	if (!create) {
	    return OCT_NEW_FACET;
	}

	retval = inherit_formals(desc, type);
	if (retval != OCT_OK) {
	    return retval;
	}
	final_val = OCT_NEW_FACET;

    } else if (mode == OCT_OVER_WRITE) {
	    /* we are over-writing the old copy */
	retval = inherit_formals(desc, type);
	if (retval != OCT_OK) {
	    return retval;
	}
	final_val = OCT_NEW_FACET;
    } else {
	if (oct_open_facet_stream(desc->file_desc, "r", &file_handle) != OCT_OK) {
	    return OCT_ERROR;
	}

	if (!oct_restore(file_handle, desc)) {
	    fsys_close(file_handle);
	    oct_error("The disk representation is corrupted");
	    return OCT_ERROR;
	}
	fsys_close(file_handle);

	final_val = OCT_OLD_FACET;

	retval = oct_check_masters_and_contents(desc);

	if (retval != OCT_OK) {
	    if (retval == OCT_INCONSISTENT) {
		final_val = retval;
	    } else {
		return retval;
	    }
	}
	(void) oct_bb_get((generic *) desc, NIL(struct octBox));
    }

    desc->facet_flags.all_loaded = desc->facet_flags.info_loaded = 1;

    if (not_in_memory) {
	oct_mark_facet(desc);
    }

    if (!full_facet_in_memory) {
	desc->objectId = oct_new_id((generic *)desc);
    }

    desc->mode |= mode;
    desc->user_facet.mode = oct_determine_mode(desc);
    desc->ref_count++;
    return final_val;
}


/* 
 * Open the corresponding contents facet for desc and copy the formal 
 * terminals over.
 *
 * desc->facet_desc must be valid at this time (used by oct_get_facet_info)
 */
static octStatus
inherit_formals(desc, type)
struct facet *desc;
int type;
{
    int retval;

    /* XXX strcmp -> ptr compare */
    if (desc->user_facet.facet != oct_contents_facet_name) {
	struct octFacet contents_user_facet;
	struct facet *contents_desc;
	struct term *ptr;
	
	contents_user_facet.cell = desc->user_facet.cell;
	contents_user_facet.view = desc->user_facet.view;
	contents_user_facet.facet = "contents";
	contents_user_facet.version = desc->user_facet.version;
	
	retval = oct_get_facet_info(&contents_user_facet,
                    &contents_desc, ((type == MASTER) ? desc : 0));
	
	if (retval == OCT_NEW_FACET) {
	    desc->contents_facet = NIL(struct facet);
	    errRaise(OCT_PKG_NAME, OCT_ERROR, "Oct internal error: this should not happen");
	} else if (retval < OCT_OK) {
	    oct_prepend_error("Opening the corresponding contents facet: ");
	    return retval;
	}

	desc->contents_facet = contents_desc;
	DLL_ATTACH(contents_desc->interfaces, desc, struct facet,
		   next_interface, last_interface);

	ptr = (struct term *) contents_desc->object_lists[OCT_FORMAL];
	while (ptr != NIL(struct term)) {
	    retval = oct_add_interface_formal(desc, ptr, NIL(struct term *));
	    if (retval != OCT_OK) {
		return retval;
	    }
	    ptr = (struct term *) ptr->last;
	}
	desc->contents_date = contents_desc->formal_date;
    } else {
	desc->contents_facet = desc;
    }
    
    return OCT_OK;
}


/* 
 * Reset all the fields in desc except for the interfaces and 
 * instances lists. oct_free_info should have already been called.
 */ 
static void
oct_init_facet_in_place(desc)
struct facet *desc;
{
    int i;

    desc->flags.type = OCT_FACET;
    desc->contents = desc->containers = NIL(struct chain);
    desc->generators = NIL(struct generator);
    desc->search_path = NIL(char *);
    desc->facet = desc;
    desc->object_count = 0;
    desc->temp_count = 0;
    desc->function_mask = (octFunctionMask) 0;
    desc->object_mask = (octObjectMask) 0;
    desc->num_formals = 0;
    desc->bbox.lowerLeft.x = 0;
    desc->bbox.lowerLeft.y = 0;
    desc->bbox.upperRight.x = 0;
    desc->bbox.upperRight.y = 0;
    CLEAR_FLAGS(desc);
    CLEAR_FACET_FLAGS(desc);
    desc->next_xid = 1;
    desc->xid_table = oct_make_xid_table((int32)0);
    desc->externalId = oct_next_xid(desc, (generic *) desc);
    desc->masters = oct_make_master_table();
    desc->bb_date = desc->formal_date = desc->contents_date =
        desc->create_date = desc->delete_date = desc->modify_date =
	desc->attach_date = desc->detach_date = desc->time_stamp = 0;

    for(i=0; i<=OCT_MAX_TYPE; i++) {
	desc->object_lists[i] = (generic *) 0;
    }
    return;
}

/* 
 * Reset everything in the facet, should only be used on a new facet, 
 * not one that is already in memory.
 */
void
oct_full_init_facet(user_facet, desc)
struct octFacet *user_facet;
struct facet *desc;
{
    oct_init_facet_in_place(desc);
    desc->user_facet.cell = oct_str_intern(user_facet->cell);
    desc->user_facet.view = oct_str_intern(user_facet->view);
    desc->user_facet.facet = oct_str_intern(user_facet->facet);
    desc->user_facet.version = oct_str_intern(user_facet->version);
    desc->user_facet.mode = oct_str_intern(user_facet->mode);
    desc->instantiations = NIL(struct master);
    desc->interfaces = NIL(struct facet);
    desc->contents_facet = NIL(struct facet); 				/* XXX Added by AC Aug 5 1991. */
    desc->next_interface = desc->last_interface = NIL(struct facet);
    desc->mode = 0;
    desc->ref_count = 0;
}


/*
 * real version of oct copy facet, uses the new octWriteFacet
 */
octStatus
octCopyFacet(new, old)
struct octObject *new;
struct octObject *old;
{
    octStatus retval;
    char *mode = old->contents.facet.mode;

    old->contents.facet.mode = "r";
    retval = octOpenFacet(old);
    if (retval < OCT_OK) {
	oct_prepend_error("octCopyFacet: ");
	return OCT_ERROR;
    }

    if (retval == OCT_NEW_FACET) {
	oct_error("octCopyFacet: can not copy a non-existent facet");
	(void) octCloseFacet(old);
	old->contents.facet.mode = mode;
	return OCT_ERROR;
    }

    if (octWriteFacet(new, old) < OCT_OK) {
	oct_prepend_error("octCopyFacet: ");
	(void) octCloseFacet(old);
	old->contents.facet.mode = mode;
	return OCT_ERROR;
    }

    retval = octFreeFacet(old);
    old->contents.facet.mode = mode;

    return retval;
}


/* debugging function */
#ifdef NEED_PRINTINTERFACES
static void printInterfaces( fct )
    struct facet *fct;
{
    struct facet *i = 0;
    if ( fct->contents_facet ) {
	fct = fct->contents_facet;
	printf( "Start from contents facet\n" );
    } 
    
    if ( fct->interfaces ) {
	i = fct->interfaces;
	printf( "Start from interfaces\n" ) ;
    } else if ( fct->next_interface ) {
	i = fct->next_interface;
	printf( "Start from next_interface\n" ) ;
    }
    while ( i && i != fct ) {
	printf( "%s:%s:%s\n", i->user_facet.cell, i->user_facet.view, i->user_facet.facet );
	i = i->next_interface;
    }
    printf( "\n" );
}
#endif

octStatus 
compatOctOpenMaster(iobj, fobj)
struct octObject *iobj;
struct octObject *fobj;
{
    struct octFacet *user_facet = &fobj->contents.facet;
    struct instance *inst;
    struct master *master;
    struct facet *master_desc;
    struct octFacet dummy;
    int mode = 0;
    octStatus retval;

    inst = (struct instance *) oct_id_to_ptr(iobj->objectId);

    if (inst == NIL(struct instance)) {
	oct_error("octOpenMaster: The arguments id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (inst->flags.type != OCT_INSTANCE) {
	oct_error("octOpenMaster: The first argument must an octInstance object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    if (fobj->type != OCT_FACET) {
	oct_error("octOpenMaster: The second argument must an octFacet object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }


    /*XXX temporary compatibility hack */
    switch (user_facet->mode[0]) {
    case 'r':
	mode = OCT_READ;
	break;
    case 'w':
	mode = OCT_OVER_WRITE;
	break;
    case 'a':
	mode = OCT_APPEND;
	break;
    default:
	oct_error("octOpenMaster: Unknown mode for facet: (%s)", user_facet->mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    switch (user_facet->mode[1]) {
    case '!':
	mode |= OCT_REVERT;
	break;
    case '\0':
	break;
    default:
	oct_error("octOpenMaster: Unknown mode for facet: (%s)", user_facet->mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    master = inst->master;

    if (master->master && !strcmp(master->facet, user_facet->facet)) {
	master_desc = master->master;
	retval = oct_do_open_with_desc(master_desc, mode, 0, MASTER);
	if (retval < OCT_OK) {
	    oct_prepend_error("octOpenMaster: ");
	    return retval;
	}
    } else {
	/* 
	 * For some reason, we couldn't open the master earlier,
	 * lets make one more try - maybe a different facet of the
	 * view .
	 */
	/* try other facets on the interfaces list... */
	struct facet *fct = master->master;

	if (fct) {
	    /* First check the contents facet (if not already the contents facet) */
	    /* Code added on April 25 1991 by AC */
	    if ( strcmp(fct->user_facet.facet,oct_contents_facet_name) && fct->contents_facet ) {
		master_desc = fct->contents_facet;
		if (!strcmp(master_desc->user_facet.facet, user_facet->facet)) {
		    retval = oct_do_open_with_desc(master_desc, mode, 0, MASTER);
		    if (retval < OCT_OK) {
			oct_prepend_error("octOpenMaster: ");
			return retval;
		    }
		    *user_facet = master_desc->user_facet; 
		    fobj->objectId = master_desc->objectId;
		    return retval;
		}
	    }

	    /* printInterfaces( fct ); */
	    master_desc = fct->interfaces;
	    /* This used to be an infinite loop. */
	    while (master_desc) {
		if (!strcmp(master_desc->user_facet.facet, user_facet->facet)) {
		    retval = oct_do_open_with_desc(master_desc, mode, 0, MASTER);
		    if (retval < OCT_OK) {
			oct_prepend_error("octOpenMaster: ");
			return retval;
		    }
		    *user_facet = master_desc->user_facet; 
		    fobj->objectId = master_desc->objectId;
		    return retval;
		}
		
		master_desc = master_desc->next_interface;
		if ( master_desc == fct->interfaces ) { /* We are back at the beginning */
		    /* Break infinite loop. (AC Jan 3 91) */
		    master_desc = NIL(struct facet);
		}
	    }
	}

	dummy.cell = master->cell;
	dummy.view = master->view;
	dummy.facet = user_facet->facet;
	dummy.version = master->version;
	dummy.mode = user_facet->mode;

	retval = oct_do_open(&dummy, &master_desc, mode, 0, inst->facet, FSYS_CHILD|FSYS_SIBLING);

	if (retval < OCT_OK) {
	    oct_prepend_error("octOpenMaster: ");
	    return retval;
	} else if (retval == OCT_NEW_FACET) {
	    oct_error("octOpenMaster: Cannot find master for instance %s",
		      oct_facet_name(&dummy));
	    return OCT_ERROR;
	}
    }

    *user_facet = master_desc->user_facet; 
    fobj->objectId = master_desc->objectId;
    
    return retval;
}
