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
#include "attach.h"
#include "chain.h"
#include "change_record.h"
#include "oct_utils.h"


octStatus octUnDetach(container, object)
struct octObject *container;
struct octObject *object;
{
    generic *cptr, *optr;
    octStatus retval;

    cptr = oct_id_to_ptr(container->objectId);
    if (cptr == NIL(generic)) {
	oct_error("octUnDetach: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
	
    optr = oct_id_to_ptr(object->objectId);
    if (optr == NIL(generic)) {
	oct_error("octUnDetach: The second argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_undetach(cptr, optr);
    if (retval < OCT_OK) {
	oct_prepend_error("octUnDetach: ");
    }
    return retval;
}

int
oct_do_undetach(cptr, optr)
generic *cptr;
generic *optr;
{
    int retval;
    struct facet *desc;
        
    retval = (*oct_object_descs[cptr->flags.type].undetach_func)(cptr, optr);

    if (retval < OCT_OK || retval == OCT_ALREADY_ATTACHED) {
	return retval;
    }

    desc = cptr->facet;
    desc->attach_date = ++desc->time_stamp;

    RECORD_ATTACH(cptr, optr, retval);
    return retval;
}

octStatus octAttach(container, object)
struct octObject *container;
struct octObject *object;
{
    generic *cptr, *optr;
    octStatus retval;

    cptr = oct_id_to_ptr(container->objectId);
    if (cptr == NIL(generic)) {
	oct_error("octAttach: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
	
    optr = oct_id_to_ptr(object->objectId);
    if (optr == NIL(generic)) {
	oct_error("octAttach: The second argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_attach(cptr, optr, 0);
    if (retval < OCT_OK) {
	oct_prepend_error("octAttach: ");
    }
    return retval;
}

int
oct_do_attach(cptr, optr, only_once)
generic *cptr;
generic *optr;
int only_once;
{
    int retval;
    struct facet *desc;
        
    if (cptr->facet != optr->facet) {
	oct_error("can not attach across facets (yet)");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    retval = (*oct_object_descs[cptr->flags.type].attach_func)(cptr, optr, only_once);

    if (retval < OCT_OK || retval == OCT_ALREADY_ATTACHED) {
	return retval;
    }

    desc = cptr->facet;
    desc->attach_date = ++desc->time_stamp;

    RECORD_ATTACH(cptr, optr, retval);
    return retval;
}

octStatus octAttachOnce(container, object)
struct octObject *container;
struct octObject *object;
{
    generic *cptr, *optr;
    octStatus retval;

    cptr = oct_id_to_ptr(container->objectId);
    if (cptr == NIL(generic)) {
	oct_error("octAttachOnce: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
	
    optr = oct_id_to_ptr(object->objectId);
    if (optr == NIL(generic)) {
	oct_error("octAttachOnce: The second argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_attach(cptr, optr, 1);
    
    if (retval < OCT_OK) {
	oct_prepend_error("octAttachOnce: ");
    }
    return retval;
}

octStatus
octUnattach(container, object)
struct octObject *container;
struct octObject *object;
{
    generic *cptr, *optr;
    octStatus retval;
	char buffer[200];
	
    cptr = oct_id_to_ptr(container->objectId);
    if (cptr == NIL(generic)) {
	sprintf(buffer,"octUnAttach: The first argument's id (%d) has been corrupted", (int)container->objectId);
	oct_error(buffer);
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
	
    optr = oct_id_to_ptr(object->objectId);
    if (optr == NIL(generic)) {
	sprintf(buffer,"octUnAttach: The second argument's id (%d) has been corrupted", (int)object->objectId);
	oct_error(buffer);
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_unattach(cptr, optr);
    if (retval < OCT_OK) {
	oct_prepend_error("octUnAttach: ");
    }
    return retval;
}

int
oct_do_unattach(cptr, optr)
generic *cptr;
generic *optr;
{
    struct facet *desc;
    int retval, rc_retval;
    
    retval = (*oct_object_descs[cptr->flags.type].unattach_func)(cptr, optr);
    
    if (retval < OCT_OK) {
	return retval;
    }

    desc = cptr->facet;
    desc->detach_date = ++desc->time_stamp;
    
    RECORD_DETACH(cptr, optr, rc_retval);

    if (rc_retval < OCT_OK) {
	return rc_retval;
    } else {
	return retval;
    }
}

octStatus
octDetach(container, object)
struct octObject *container;
struct octObject *object;
{
    generic *cptr, *optr;
    octStatus retval;
	char buffer[200];
	
    cptr = oct_id_to_ptr(container->objectId);
    if (cptr == NIL(generic)) {
	sprintf(buffer,"octDetach: The first argument's id (%d) has been corrupted", (int)container->objectId);
	oct_error(buffer);
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
	
    optr = oct_id_to_ptr(object->objectId);
    if (optr == NIL(generic)) {
	sprintf(buffer,"octDetach: The second argument's id (%d) has been corrupted", (int)object->objectId);
	oct_error(buffer);
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    retval = oct_do_detach(cptr, optr);
    if (retval < OCT_OK) {
	oct_prepend_error("octDetach: ");
    }
    return retval;
}

int
oct_do_detach(cptr, optr)
generic *cptr;
generic *optr;
{
    struct facet *desc;
    int retval, rc_retval;
    
    retval = (*oct_object_descs[cptr->flags.type].detach_func)(cptr, optr);
    
    if (retval < OCT_OK) {
	return retval;
    }

    desc = cptr->facet;
    desc->detach_date = ++desc->time_stamp;
    
    RECORD_DETACH(cptr, optr, rc_retval);

    if (rc_retval < OCT_OK) {
	return rc_retval;
    } else {
	return retval;
    }
}


octStatus
oct_unattach_default(cptr, optr)
generic *cptr;
generic *optr;
{
    return oct_detach_content(cptr, optr);
}


octStatus
oct_undetach_default(cptr, optr)
generic *cptr;
generic *optr;
{
    /* int ctype = cptr->flags.type;*/
    /* int otype = optr->flags.type;*/

    return oct_attach_content(cptr, optr, 0);
}


octStatus
oct_detach_default(cptr, optr)
generic *cptr;
generic *optr;
{
    return oct_detach_content(cptr, optr);
}


octStatus
oct_attach_default(cptr, optr, only_once)
generic *cptr;
generic *optr;
int only_once;
{
    int ctype = cptr->flags.type;
    int otype = optr->flags.type;

    if (!IN_MASK(otype, oct_object_descs[ctype].contain_mask)) {
	oct_error("Objects of type %s cannot contain objects of type %s",
		  oct_type_names[ctype], oct_type_names[otype]);
	return OCT_ERROR;
    }
    return oct_attach_content(cptr, optr, only_once);
}

octStatus octIsAttached(container, object)
struct octObject *container;
struct octObject *object;
{
    generic *cptr, *optr;
    octStatus retval;

    cptr = oct_id_to_ptr(container->objectId);
    if (cptr == NIL(generic)) {
	oct_error("octIsAttached: The first argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
	
    optr = oct_id_to_ptr(object->objectId);
    if (optr == NIL(generic)) {
	oct_error("octIsAttached: The second argument's id has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (cptr->facet != optr->facet) {
	return OCT_NOT_ATTACHED;
    }
    
    retval = (*oct_object_descs[cptr->flags.type].is_attached_func)(cptr, optr);

    if (retval < OCT_OK) {
	oct_prepend_error("octIsAttached: ");
    }
    return retval;
}

octStatus
oct_is_attached_default(cptr, optr)
generic *cptr;
generic *optr;
{
    struct chain *ptr;
    
    ptr = optr->containers;
    for(;ptr != NIL(struct chain); ptr = ptr->next_chain) {
	if (ptr->container == cptr) {
	    return OCT_OK;
	}
    }
    return OCT_NOT_ATTACHED;
}
