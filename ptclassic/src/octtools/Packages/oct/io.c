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
#include "bb.h"
#include "chain.h"
#include "default.h"
#include "io_internal.h"
#include "io_procs.h"
#include "master.h"
#include "mark.h"
#include "translate.h"
#include "oct_utils.h"

#include "io.h"

/* Forward references */
int oct_get_terms
	ARGS((struct facet *desc));
int oct_put_terms
	ARGS((struct facet *desc));
int oct_get_rest_of_terms
	ARGS((struct facet *desc));
int oct_put_rest_of_terms
	ARGS((struct facet *desc));

  /* The top level io operations: dump, restore and free */

IOFILE *oct_facet_file;		/* the file pointer for the file we are reading or writing */

int oct_binary_format = 1;
static int binary_output_format = 1;
int32 oct_next_dump_id;

extern void oct_mark_ptr();

#define VERSION_NUMBER 2

#define BB_INIT 1
#define BB_VALID 2

int
oct_dump(desc, file)
struct facet *desc;
char *file;
{
    int val, i;
    int bb_flags = 0;
    generic *ptr;

    oct_facet_file = (IOFILE *) file;
    
    oct_init_marks();
    oct_next_dump_id = 0;
    oct_binary_format = binary_output_format;

    if (desc->facet_flags.bb_init) bb_flags |= BB_INIT;
    if (desc->facet_flags.bb_valid) bb_flags |= BB_VALID;
    
    val =   oct_put_version(VERSION_NUMBER|binary_output_format) &&
    	    oct_put_type(OCT_FACET) &&
	    oct_put_32(oct_next_dump_id++) &&
	    oct_put_byte(bb_flags) &&
	    oct_put_box(desc->bbox) &&
	    oct_put_32((int32) (desc->object_count - desc->temp_count)) &&
	    oct_put_32((int32) desc->num_formals) &&
	    oct_put_32(desc->next_xid) &&
	    oct_put_terms(desc) &&
	    oct_put_32(desc->time_stamp) &&
	    oct_put_32(desc->bb_date) &&
	    oct_put_32(desc->formal_date) &&
	    oct_put_32(desc->contents_date) &&
	    oct_put_32(desc->create_date) &&
	    oct_put_32(desc->delete_date) &&
	    oct_put_32(desc->modify_date) &&
	    oct_put_32(desc->attach_date) &&
	    oct_put_32(desc->detach_date) &&
	    oct_put_rest_of_terms(desc) &&
	    /* XXX really getting rid of proplist requires a format change */
	    oct_put_props() &&
	    oct_put_contents(desc->contents) &&
	    oct_dump_instances(desc);

    if (!val) {
	return OCT_ERROR;
    }

    for(i = 2; i <= OCT_MAX_TYPE; i++) {

	/*XXX*/

	/* 
	 * The formals are dumped in oct_put_terms, instances are 
	 * written when the master-object is written, and change lists 
	 * and records are not written to disk yet.
	 */
	if (i == OCT_FORMAL || i == OCT_INSTANCE || i == OCT_CHANGE_LIST || i == OCT_CHANGE_RECORD) {
	    continue;
	}
	
	ptr = desc->object_lists[i];
	while(ptr != NIL(generic)) {
	    if (!oct_write_object(ptr)) {
		return OCT_ERROR;
	    }
	    ptr = ptr->last;
	}
    }


    if (!oct_put_nl() || !oct_put_type(OCT_UNDEFINED_OBJECT)) {
	return OCT_ERROR;
    }
    
    oct_clear_marks();
    return OCT_OK;
}
octId oct_id_offset;

/* 
 * Restore only the first part of the facet: the timestamps, bounding 
 * box and formal terminals
 */
int
oct_restore_info(file, desc)
char *file;
struct facet *desc;
{
    octId id;
    int bb_flags;
    int val, version_number, type;
    extern octId oct_next_id();

    oct_facet_file = (IOFILE *) file;
    
    oct_id_offset = oct_next_id() - 1;

    if (!oct_get_version(&version_number) || version_number < 0) {
	oct_error("Corrupt facet");
	return 0;
    }
	
    oct_binary_format = version_number % 2;
    version_number -= oct_binary_format;
    
    if (version_number > VERSION_NUMBER) {
	oct_error("Panic: oct version mismatch. Either oct is out of date or possible corrupt facet");
	return 0;
    }


    if (version_number != VERSION_NUMBER) {
	return oct_translate(version_number, desc);
    }
    
    val = oct_get_type(&type) &&
	   oct_get_32(&id) &&
	   oct_get_byte(&bb_flags) &&
	   oct_get_box(&desc->bbox) &&
	   oct_get_32(&desc->object_count) &&
	   oct_get_32(&desc->num_formals) &&
	   oct_get_32(&desc->next_xid) &&
	   oct_get_terms(desc) &&
	   oct_get_32(&desc->time_stamp) &&
	   oct_get_32(&desc->bb_date) &&
	   oct_get_32(&desc->formal_date) &&
	   oct_get_32(&desc->contents_date) &&
	   oct_get_32(&desc->create_date) &&
	   oct_get_32(&desc->delete_date) &&
	   oct_get_32(&desc->modify_date) &&
	   oct_get_32(&desc->attach_date) &&
	   oct_get_32(&desc->detach_date);

    if (!val || id != oct_null_id || type != OCT_FACET) {
	oct_error("The facet is corrupted");
	return 0;
    }

    if (bb_flags&BB_VALID) desc->facet_flags.bb_valid = 1;
    if (bb_flags&BB_INIT) desc->facet_flags.bb_init = 1;
    
    return 1;
}

int
oct_restore(file, desc)
char *file;
struct facet *desc;
{
    int type;

    if (!oct_restore_info(file, desc)) {
	return 0;
    }

    if (desc->facet_flags.all_loaded) {
	    /* translation may have loaded the entire facet at this point */
	return 1;
    }
    
    /*
     *desc->xid_table = oct_make_xid_table(desc->object_count);
     * done is oct_init_in_place
     */
    desc->object_count = 0;

    desc->contents = NIL(struct chain);
    if (!(oct_get_rest_of_terms(desc)
	    && oct_get_props((generic *) desc)
	    && oct_get_contents((generic *) desc)
	    && oct_restore_instances(desc, &type))) {
	oct_error("The facet is corrupted");
	return 0;
    }

    while (type != OCT_UNDEFINED_OBJECT) {
	if (!oct_read_object(desc,type,NIL(generic *))) {
	    return 0;
	}
	desc->object_count++; /* recompute, to keep sane */
	if (!oct_get_type(&type)) {
	    return 0;
	}
    }
    
    return 1;
}

int
oct_put_terms(desc)
struct facet *desc;
{
    struct term *term;
    
    term = (struct term *) desc->object_lists[OCT_FORMAL];
    while (term != NIL(struct term)) {
	if (!term->flags.is_temporary) {
	    if (!(oct_put_string(term->user_term.name) &&
		  oct_put_32(term->user_term.formalExternalId) &&
		  oct_put_32(term->user_term.width) &&
		  oct_put_32(term->externalId))) {
		return 0;
	    }
	} else {
	    desc->formal_date = ++desc->time_stamp;
	}
	term = (struct term *) term->last;
    }
    return 1;
}

int    
oct_get_terms(desc)
struct facet *desc;
{
    struct term *term;
    int i;

    for (i = desc->num_formals; i > 0; i--) {
	term = ALLOC(struct term, 1);
	term->flags.type = OCT_FORMAL;
	term->contents = term->containers = NIL(struct chain);
	CLEAR_FLAGS(term);
	term->facet = desc;
	term->user_term.instanceId = oct_null_id;
	
	if (! (oct_get_string(&term->user_term.name) &&
	       oct_get_32(&term->user_term.formalExternalId) &&
	       oct_get_32(&term->user_term.width) &&
	       oct_get_32(&term->externalId))) {
	    return 0;
	}
	oct_attach_generic_list(desc->object_lists[OCT_FORMAL], term);
    }
    return 1;
}
    
/* 
 * The attachements to the formals are not needed with the facet_info, 
 * so there are stored later in the file and only read in during a 
 * full restore
 */
int
oct_get_rest_of_terms(desc)
struct facet *desc;
{
    struct term *term;
    int val;

    term = (struct term *) desc->object_lists[OCT_FORMAL];
    while (term != NIL(struct term)) {
	term->objectId = oct_new_id((generic *) term);
	oct_add_xid(desc, term->externalId, (generic *) term);
	((generic *) term)->contents = NIL(struct chain);
	val = oct_get_props((generic *) term) && oct_get_contents((generic *) term);
	if (!val) {
	    return 0;
	}
	term = (struct term *) term->last;
	desc->object_count++;
    }
    return 1;
}
int    
oct_put_rest_of_terms(desc)
struct facet *desc;
{
    struct term *term;
    int val;

    term = (struct term *) desc->object_lists[OCT_FORMAL];
    while (term != NIL(struct term)) {
	if (!term->flags.is_temporary) {
	    oct_mark_ptr((generic *) term, oct_next_dump_id++);
	    val = oct_put_props() && oct_put_contents(term->contents);
	    if (!val) {
		return 0;
	    }
	}
	term = (struct term *) term->last;
    }
    return 1;
}
    
octStatus
oct_free(desc)
struct facet *desc;
{
    int retval, i;
    generic *ptr, *next;
    struct octBox bb;
    struct ff flags;
    
    retval = oct_bb_get((generic *) desc, &bb);

    if (retval < OCT_OK && retval != OCT_NO_BB) {
	oct_error("updating bounding box on free");
	return retval;
    }
    
    flags = desc->facet_flags;
    
    for(i=2; i <= OCT_MAX_TYPE; i++) {

	if (i == OCT_FORMAL) {	/* formals are take care of in oct_free_info */
	    continue;
	}
	
	ptr = desc->object_lists[i];
	while(ptr != NIL(generic)) {
	    next = ptr->last;
	    oct_free_id(ptr->objectId);
	    (*oct_object_descs[ptr->flags.type].free_func)(ptr);
	    ptr = next;
	}
    }
    
    oct_detach_all_chains(desc->contents);
    desc->contents = NIL(struct chain);

    oct_free_xid_table(desc->xid_table);
    desc->xid_table = NIL(st_table);

    (void) oct_free_master_table(desc->masters);
    desc->masters = NIL(st_table);
    
    oct_free_id(desc->objectId);
    desc->objectId = oct_null_id;

    desc->facet_flags = flags;
    desc->bbox = bb;
    desc->facet_flags.all_loaded = 0;

    if (retval == OCT_NO_BB) {
	retval = OCT_OK;
    }
    return retval;
}

void
oct_free_info(desc)
struct facet *desc;
{
    generic *ptr, *next;
    
    /* 
     * The cell, view, facet, and version names and also the 
     * file_desc are never freed, since the info currently is never deleted,
     * and those fields will not change
     */
    ptr = desc->object_lists[OCT_FORMAL];
    
    while (ptr != NIL(generic)) {
	next = ptr->last;
	(*oct_object_descs[OCT_TERM].free_func)(ptr);
	ptr = next;
    }
    return;
}

void
octSetBinaryOutput()
{
    binary_output_format = 1;
    return;
}

void
octSetAsciiOutput()
{
    binary_output_format = 0;
    return;
}

