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
/*
 * Implementations of the the oct functions for terms.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "io.h"

#include "change_record.h"
#include "io_procs.h"
#include "mark.h"
#include "oct_utils.h"
#include "term_update.h"

#include "term.h"

/* Forward reference, should this be static? */
int oct_do_term_undelete
	ARGS((struct facet *desc, struct octObject *term, generic **new_p, int32 xid, int32 old_xid, int32 old_id));

extern struct object_desc oct_default_desc;
static struct object_desc *super = &oct_default_desc;

static int term_create(), term_uncreate(), term_delete(), term_undelete(), term_modify();
static int term_free_fields(), term_write_fields(), term_read_fields();
static int term_copy_fields();

void
oct_formal_desc_set(object_desc)
struct object_desc *object_desc;
{
    oct_term_desc_set(object_desc);
}

void
oct_term_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct term);
    object_desc->external_size = sizeof(struct octTerm);
    object_desc->flags |= HAS_NAME;
    object_desc->contain_mask =
      OCT_GEO_MASK|OCT_INSTANCE_MASK|OCT_PROP_MASK|
	OCT_BAG_MASK|OCT_NET_MASK|OCT_TERM_MASK|OCT_FORMAL_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct term, user_term);
    object_desc->name_offset = FIELD_OFFSET(struct octTerm, name);
    object_desc->create_func = term_create;
    object_desc->delete_func = term_delete;
    object_desc->undelete_func = term_undelete;
    object_desc->uncreate_func = term_uncreate;
    object_desc->modify_func = term_modify;
    object_desc->unmodify_func = term_modify;
    object_desc->free_fields_func = term_free_fields;
    object_desc->read_fields_func = term_read_fields;
    object_desc->copy_fields_func = term_copy_fields;
    object_desc->write_fields_func = term_write_fields;
}
  
static octStatus
term_create(desc, term, new_p)
struct facet *desc;
struct octObject *term;
generic **new_p;
{
    struct octTerm *user_term = &term->contents.term;

    if (user_term->instanceId != oct_null_id) {
	oct_error(
	    "Only formal terminals (instanceId == 0) can be explicitly created");
	return OCT_ERROR;
    }

    /* XXX strcmp -> ptr compare */
    if (desc->user_facet.facet != oct_contents_facet_name) {
	oct_error("terminals may be only created in the contents facet");
	return OCT_ERROR;
    }

    return oct_do_term_create(desc, term, new_p, (int32) 0);
}

static octStatus
term_undelete(desc, term, new_p, old_xid, old_id)
struct facet *desc;
struct octObject *term;
generic **new_p;
int32 old_xid,old_id;
{
    /*struct octTerm *user_term = &term->contents.term;*/

    if (strcmp(desc->user_facet.facet, "contents") != 0) {
	oct_error("terminals may be only created in the contents facet");
	return OCT_ERROR;
    }

    return oct_do_term_undelete(desc, term, new_p, (int32) 0, old_xid, old_id);
}

octStatus
oct_do_term_create(desc, term, new_p, xid)
struct facet *desc;
struct octObject *term;
generic **new_p;
int32 xid;	/* force the xid to be this */
{
    int retval;
    struct term *new;
    
    if (term->contents.term.instanceId == oct_null_id)
	{
		/*
		 *formals and actuals are treated as different
		 * types internally
		 */
		
		term->type = OCT_FORMAL;
		retval = (*super->create_func)(desc, term, new_p);
		term->type = OCT_TERM;
		
		if (retval < OCT_OK)
		{
			return retval;
		}
	
		new = (struct term *)*new_p;

		/* assign the xid to the negative space */
		oct_free_xid(desc, new->externalId);
		if (xid == 0)
		{
			new->externalId = 0 - new->externalId;
		}
		else
		{
			new->externalId = xid;
		}
		oct_add_xid(desc, new->externalId, (generic *) new);
		
		new->user_term.formalExternalId = new->externalId;

		/* create will increment timestamp after we return */
		desc->formal_date = desc->time_stamp + 1;
		desc->num_formals++;
		oct_create_terminal_copies(desc, new);
    }
	else
	{
		retval = (*super->create_func)(desc, term, new_p);
		if (retval < OCT_OK)
		{
			return retval;
		}
		return RECORD_CHANGE(*new_p, OCT_CREATE, oct_null_xid);
    }
	return OCT_OK;
}

int
oct_do_term_undelete(desc, term, new_p, xid, old_xid, old_id)
struct facet *desc;
struct octObject *term;
generic **new_p;
int32 xid,old_xid,old_id;	/* force the xid to be this */
{
    int retval;
    struct term *new;
    
    if (term->contents.term.instanceId == oct_null_id)
	{
		/*
		 *formals and actuals are treated as different
		 * types internally
		 */
		
		term->type = OCT_FORMAL;
		retval = (*super->undelete_func)(desc, term, new_p, old_xid, old_id);
		term->type = OCT_TERM;
		
		if (retval < OCT_OK)
		{
			return retval;
		}
		
		new = (struct term *)*new_p;
		
		new->user_term.formalExternalId = new->externalId;
		
		/* create will increment timestamp after we return */
		desc->formal_date = desc->time_stamp + 1;
		desc->num_formals++;
		oct_create_terminal_copies(desc, new);
	}
	else
	{
		retval = (*super->undelete_func)(desc, term, new_p, old_xid, old_id);
		if (retval < OCT_OK)
		{
			return retval;
		}
		/* there is no need to report a change here, because no automatic
		   additions occur during UNDELETEs. The REPORT call is in the
		   create routine, because no such report is made by the calling
		   routines, automatically inserting actuals on instances. */
    }
	return OCT_OK;
}

/*ARGSUSED*/
static int
term_copy_fields(o_p, c_p, size)
char *o_p, *c_p;
int size;
{
    struct octTerm *orig = (struct octTerm *) o_p;
    struct octTerm *copy = (struct octTerm *) c_p;

    *copy = *orig;

    if (copy->name != NIL(char)) {
	copy->name = oct_str_intern(copy->name);
    }
    return OCT_OK;		/* Return OCTOK because we take the
				   pointer of this func and pass it
				   around as a func returning an int */
				   
}

static int
term_free_fields(nptr)
generic *nptr;
{
    struct term *term = (struct term *) nptr;

    if (term->user_term.name != NIL(char)) {
	oct_str_free(term->user_term.name);
    }
    return OCT_OK;		/* Return OCTOK because we take the
				   pointer of this func and pass it
				   around as a func returning an int */
}

static octStatus
term_read_fields(ptr)
generic *ptr;
{
    struct term *term = (struct term *) ptr;
    int val;
    
    val = (*super->read_fields_func)(ptr) &&
      oct_get_string(&term->user_term.name) &&
      oct_get_32(&term->user_term.instanceId) &&
      oct_get_32(&term->user_term.formalExternalId) &&
      oct_get_32(&term->user_term.width);

    if (!val) {
	return val;
    }
    
    term->user_term.instanceId += oct_id_offset;
    return val;
}
 
static octStatus
term_write_fields(ptr)
generic *ptr;
{
    struct term *term = (struct term *) ptr;
    octId inst_id = 0;

    if (term->user_term.instanceId != 0 &&
	!oct_lookup_mark(oct_id_to_ptr(term->user_term.instanceId),&inst_id)) {
	oct_error("Panic: can't find instance for terminal");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    return (*super->write_fields_func)(ptr) &&
      oct_put_string(term->user_term.name) &&
      oct_put_32(inst_id) &&
      oct_put_32(term->user_term.formalExternalId) &&
      oct_put_32(term->user_term.width);
}


static octStatus
term_delete(nptr, by_user)
generic *nptr;
int by_user;
{
    struct term *term = (struct term *) nptr;
    int retval, formal = (term->user_term.instanceId == oct_null_id);
    
    if (by_user && !formal) {
	oct_error("actual terminals can not be deleted");
	errRaise(OCT_PKG_NAME, OCT_ILL_OP, octErrorString());
    }
    
    if (formal) {
	term->flags.type = OCT_FORMAL;
    }
    retval = (*super->delete_func)((generic *) term, by_user);
    
    if (retval < OCT_OK) {
	return retval;
    }

    if (formal) {
	/* octDelete will increment time_stamp */
	term->facet->formal_date = term->facet->time_stamp + 1;
	term->facet->num_formals--;
	oct_delete_terminal_copies(term->facet, term);
    }
    
    return retval;
}

static octStatus
term_uncreate(nptr, by_user)
generic *nptr;
int by_user;
{
    struct term *term = (struct term *) nptr;
    int retval, formal = (term->user_term.instanceId == oct_null_id);
    
    if (formal) {
	term->flags.type = OCT_FORMAL;
    }
    retval = (*super->uncreate_func)((generic *) term, by_user);
    
    if (retval < OCT_OK) {
	return retval;
    }

    if (formal) {
	/* octDelete will increment time_stamp */
	term->facet->formal_date = term->facet->time_stamp + 1;
	term->facet->num_formals--;
	oct_delete_terminal_copies(term->facet, term);
    }
    
    return retval;
}

static octStatus
term_modify(nptr, obj)
generic *nptr;
struct octObject *obj; 
{
    struct term *term = (struct term *)nptr;
    char *name = obj->contents.term.name;

    if (term->user_term.instanceId != oct_null_id) {
	oct_error("actual terminals can not be modified");
	return OCT_ERROR;
    }

    if (obj->contents.term.instanceId != oct_null_id) {
	oct_error("Formal terminals can not be changed into actual terminals");
	return OCT_ERROR;
    }

    MODIFY_NAME(term->user_term.name, name);

    term->facet->formal_date = term->facet->time_stamp + 1;
    term->user_term = obj->contents.term;
    term->user_term.name = name;
    oct_modify_terminal_copies(term);
    
    return OCT_OK;
}

