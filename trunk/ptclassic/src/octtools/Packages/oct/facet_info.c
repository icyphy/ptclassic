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
#include "fsys.h"
#include "io.h"
#include "mark.h"
#include "oct_utils.h"
#include "oct_files.h"
#include "facet_info.h"

int
octGetFacetInfo(obj, info)
struct octObject *obj;
struct octFacetInfo *info;
{
    struct facet *desc;
    struct octFacet *user_facet;
    int retval;

    octBegin();   /* this is safe, same as in octOpenFacet */

    if (obj->type != OCT_FACET) {
	oct_error("octGetFacetInfo: The first argument must be a facet");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    user_facet = &obj->contents.facet;
    
    if (user_facet->cell == NIL(char) || user_facet->view == NIL(char) ||
	    user_facet->facet == NIL(char)) {
	oct_error("octGetFacetInfo: Incompletely specified facet");
	return OCT_ERROR;
    }

    
    retval = oct_get_facet_info(user_facet, &desc, NIL(struct facet));
    
    if (retval < OCT_OK) {
	oct_prepend_error("octGetFacetInfo: ");
	return retval;
    }
    
    info->timeStamp = desc->time_stamp;
    info->formalDate = desc->formal_date;
    info->createDate = desc->create_date;
    info->deleteDate = desc->delete_date;
    info->modifyDate = desc->modify_date;
    info->attachDate = desc->attach_date;
    info->detachDate = desc->detach_date;
    info->bbDate = desc->bb_date;
    info->bbox = desc->bbox;
    info->numObjects = desc->object_count;
    info->fullName = fsys_full_name(desc->file_desc);
    
    return OCT_OK;
}

extern void oct_full_init_facet();

/* 
 * Get the facet-info header for a facet, pulling it in off disk if 
 * necessary.  The header contains the facetinfo structure as well as 
 * the bounding box and formal terminals.  This is the minimum 
 * information required to instantiate a cell or create an interface facet
 */
octStatus
oct_get_facet_info(user_facet, desc_ptr, parent)
struct octFacet *user_facet;
struct facet **desc_ptr;
struct facet *parent;
{
    int retval;
    struct facet *desc;
    char *file_desc = NIL(char);
    char *file_handle;
    
    retval = oct_make_key(user_facet, &file_desc, parent, FSYS_CHILD|FSYS_SIBLING);

    if (retval < OCT_OK) {
	return retval;
    }

    if (!(oct_lookup_facet(file_desc, &desc) && desc->facet_flags.info_loaded)) {

#if defined(MCC_DMS)
	/*
	 * Because the DMS uses oct, resolve will call DMS which will
	 * call oct, causing the key just generated to be overwritten.
	 * Therefore, we must save it here and then free it when we
	 * dont' need it any more.
	 */
	{
	    char *ldesc = file_desc;
	    retval = oct_save_key(ldesc, &file_desc);
	    if (retval != OCT_OK) {
		return retval;
	    }
	}
#endif	
	
	retval = oct_resolve(user_facet, OCT_READ, file_desc);
	if (retval == OCT_NEW_FACET) {
#if defined(MCC_DMS)
	    fsys_free_key(file_desc);
#endif	    
	    oct_error("octGetFacetInfo: can't open %s", file_desc);
	    return OCT_NOT_FOUND;
	}

	desc = ALLOC(struct facet, 1);
	user_facet->mode = "r";		/* oct_full_init_facet needs 'mode' */
	oct_full_init_facet(user_facet, desc);

	retval = oct_save_key(file_desc, &desc->file_desc);
#if defined(MCC_DMS)
	fsys_free_key(file_desc);
#endif	
	if (retval != OCT_OK) {
	    return retval;
	}
	
	if (oct_open_facet_stream(file_desc, "r", &file_handle) != OCT_OK) {
	    return OCT_ERROR;
	}
	
	if (!oct_restore_info(file_handle, desc)) {
	    fsys_close(file_handle);
	    oct_prepend_error("octGetFacetInfo: ");
	    return OCT_ERROR;
	}
	
	fsys_close(file_handle);
	desc->facet_flags.info_loaded = 1;
	oct_mark_facet(desc);

	*desc_ptr = desc;
	return OCT_OLD_FACET;
    } else {
	/* facet is already open */
	*desc_ptr = desc;
	return OCT_ALREADY_OPEN;
    }
}

