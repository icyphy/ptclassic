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
#include <sys/file.h>
#include "internal.h"

#include "change_list.h"
#include "bb.h"
#include "io.h"
#include "mark.h"
#include "oct_files.h"
#include "oct_utils.h"


extern void fsys_close();	/* Used here and there. */

static octStatus oct_flush_facet();
static octStatus oct_free_facet();


octStatus 
octFlushFacet(obj)
struct octObject *obj;
{
    struct facet *facet;
    octStatus retval;

    facet = (struct facet *) oct_id_to_ptr(obj->objectId);

    if (facet == NIL(struct facet)) {
	oct_error("octFlushFacet: The first argument's objectId is invalid");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (facet->flags.type != OCT_FACET) {
	oct_error("octFlushFacet: The argument must be an octFacet object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    retval = oct_flush_facet(facet);
    if (retval != OCT_OK) {
	oct_prepend_error("octFlushFacet: ");
    }
    return retval;
}


static octStatus
oct_flush_facet(facet)
struct facet *facet;
{
    struct octBox junk_box;
    char *file_handle;
    int retval;
    
    if (!(facet->mode&OCT_OVER_WRITE)) {
	oct_error("Writing the facet: The facet is read-only");
	return OCT_ERROR;
    }
    
        /* make sure the bounding box is valid */
    (void) oct_bb_get((struct generic *) facet, &junk_box);
    
    if (!oct_filter_records(facet)) {
	return OCT_ERROR;
    }
    
    retval = oct_open_facet_stream(facet->file_desc, "w", &file_handle);
    
    if (retval < OCT_OK) {
	return retval;
    }
    
    if ((retval = oct_dump(facet, file_handle)) < OCT_OK) {
	fsys_close(file_handle);
	return retval;
    }
    
    fsys_close(file_handle);
    return oct_commit(facet->file_desc);
}


octStatus 
octCloseFacet(obj)
struct octObject *obj;
{
    struct facet *facet;
    octStatus status;

    facet = (struct facet *) oct_id_to_ptr(obj->objectId);

    if (facet == NIL(struct facet)) {
	oct_error("octCloseFacet: The first argument's objectId is invalid");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (facet->flags.type != OCT_FACET) {
	oct_error("octCloseFacet: The argument must be an octFacet object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    if (facet->mode&OCT_OVER_WRITE) {
	status = oct_flush_facet(facet);
	if (status != OCT_OK) {
	    oct_prepend_error("octCloseFacet:");
	    return status;
	}
    }
    
    status = oct_free_facet(facet);
    if (status != OCT_OK) {
	oct_prepend_error("octCloseFacet: ");
    }
    return status;
}


octStatus 
octFreeFacet(obj)
struct octObject *obj;
{   
    struct facet *facet;
    int retval;

    facet = (struct facet *) oct_id_to_ptr(obj->objectId);

    if (facet == NIL(struct facet)) {
	oct_error("octFreeFacet: The first argument's objectId is invalid");
#if defined(TDMS)
	fprintf(stderr, "octFreeFacet passed invalid facet: %d\n",
		obj->objectId);
	return OCT_OK;
#else
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
#endif
    }

    if (facet->flags.type != OCT_FACET) {
	oct_error("octFreeFacet: The argument must be an octFacet object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
	return OCT_ERROR;
    }

    retval = oct_free_facet(facet);
    if (retval != OCT_OK) {
	oct_prepend_error("octFreeFacet: ");
    }
    return retval;
}


static octStatus
oct_free_facet(desc)
struct facet *desc;
{
    if (--desc->ref_count > 0) {
	return OCT_OK;
    }
    
    if (desc->mode&OCT_OVER_WRITE) {
	int retval = oct_unlock(desc);
        if (retval != OCT_OK) {
	    oct_prepend_error("Unlocking the facet: ");
	    return retval;
	}
    }

    return oct_free(desc);
}


octStatus
octWriteFacet(new, old)
octObject *new, *old;
{
    struct facet *ofacet, *dummy;
    struct octFacet *nfacet;
    char *desc;
#if defined(MCC_DMS)
    char *ldesc;
#endif
    char *file_handle;
    int retval;
    octBox junk_box;

    ofacet = (struct facet *) oct_id_to_ptr(old->objectId);

    if (ofacet == NIL(struct facet)) {
	oct_error("octWriteFacet: The id of the old facet has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (ofacet->flags.type != OCT_FACET) {
	oct_error("octWriteFacet: The old facet must be of type OCT_FACET");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    
    if (new->type != OCT_FACET) {
	oct_error("octWriteFacet: The facet to the be written must be of type OCT_FACET");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    nfacet = &new->contents.facet;

    retval = oct_make_key(nfacet, &desc, NIL(struct facet), 0);
    if (retval < 0) {
	oct_prepend_error("octWriteFacet: ");
	return retval;
    }

#if defined(MCC_DMS)
    /*
     * Because the DMS uses oct, resolve will call DMS which will
     * call oct, causing the key just generated to be overwritten.
     * Therefore, we must save it here and then free it when we
     * dont' need it any more.
     */
    ldesc = desc;
    retval = oct_save_key(ldesc, &desc);
    if (retval != OCT_OK) {
	return retval;
    }
#endif
    
    retval = oct_resolve(nfacet, OCT_OVER_WRITE, desc);
    if (retval < 0) {
	oct_prepend_error("octWriteFacet: ");
	return retval;
    }

    if (oct_lookup_facet(desc, &dummy)) {
	if (dummy->facet_flags.all_loaded) {
#if defined(MCC_DMS)
	    fsys_free_key(desc);
#endif	    
	    errRaise(OCT_PKG_NAME, OCT_ERROR, "octWriteFacet: can not write to an in-memory facet");
	}
    }

    /* make sure the bounding box is valid */
    (void) oct_bb_get((struct generic *) ofacet, &junk_box);

    if (!oct_filter_records(ofacet)) {
#if defined(MCC_DMS)
	fsys_free_key(desc);
#endif	
	return OCT_ERROR;
    }

    if (oct_open_facet_stream(desc, "w", &file_handle) < OCT_OK) {
	oct_prepend_error("octWriteFacet: ");
#if defined(MCC_DMS)
	fsys_free_key(desc);
#endif	
	return retval;
    }

#if defined(MCC_DMS)
    fsys_free_key(desc);
#endif    

    if ((retval = oct_dump(ofacet, file_handle)) < OCT_OK) {
	fsys_close(file_handle);
	return retval;
    }

    fsys_close(file_handle);

    /* XXX need to handle info structure */

    /* is this commit correct if the thing was already loaded? */
    return oct_commit(desc);
}


void
octFullName(fobj, name)
octObject *fobj;
char **name;
{
    struct facet *facet;
    extern char *fsys_full_name();

    facet = (struct facet *) oct_id_to_ptr(fobj->objectId);

    if (facet == NIL(struct facet)) {
	oct_error("octFullName: The id of the facet has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
    *name = strsave(fsys_full_name(facet->file_desc));
    return;
}
