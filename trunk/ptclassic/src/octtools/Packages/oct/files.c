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
#include <sys/file.h>
#include "internal.h"
#include "io_internal.h"
#include "fsys.h"
#include "oct_files.h"
#include "oct_utils.h"

octStatus
oct_make_key(user_facet, file_desc, parent, location)
struct octFacet *user_facet;
char **file_desc;
struct facet *parent;
int location;
{
    char *parent_desc =
      (parent == NIL(struct facet) ? NIL(char) : parent->file_desc);

    if (!fsys_make_key(user_facet, file_desc, parent_desc, location)) {
	oct_error(fsys_error_message);
	return OCT_ERROR;
    }

    return OCT_OK;
}
       
octStatus
oct_save_key(file_desc, copy_desc)
char *file_desc;
char **copy_desc;
{
    if (!fsys_save_key(file_desc, copy_desc)) {
	oct_error(fsys_error_message);
	return OCT_ERROR;
    }

    return OCT_OK;
}
       
octStatus
oct_resolve(user_facet, mode, desc)
struct octFacet *user_facet;
int mode;
char *desc;
{
    int umode = 0, fmode;
    
    switch (mode&~OCT_REVERT) {
    case OCT_READ:
	umode = FSYS_READABLE;
	break;
    case OCT_OVER_WRITE:
	umode = FSYS_WRITABLE;
	break;
    case OCT_APPEND:
	umode = FSYS_READABLE|FSYS_WRITABLE;
	break;
    default:
	oct_error("Illegal facet mode (%d)", mode);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    fmode = fsys_resolve(desc);
    
    if (!(fmode&FSYS_EXISTS)) {
	if (fmode&FSYS_CREATABLE) {
	    if (umode&FSYS_WRITABLE) {
		return OCT_NEW_FACET;
	    } else {
		oct_error("`%s' doesn't exist",
			  oct_facet_name(user_facet));
		return OCT_NO_EXIST;
	    }
	} else {
	    oct_error("Cannot create facet: %s", fsys_error_message);
	    return OCT_NO_PERM;
	}
    }
    
    if ((fmode&umode) != umode) {
	oct_error("`%s' : permission denied", oct_facet_name(user_facet));
	return OCT_NO_PERM;
    }
    
    return OCT_OLD_FACET;
}

octStatus
oct_open_facet_stream(desc, mode, file_handle)
char *desc;
char *mode;
char **file_handle;
{
    IOFILE *ptr, *fsys_open();
    IOFILE **file_desc = (IOFILE **) file_handle;

    ptr = fsys_open(desc, mode);
    if (ptr == NULL) {
	oct_error(fsys_error_message);
	return OCT_ERROR;
    }
    
    *file_desc = ptr;
    return OCT_OK;
}

octStatus
oct_commit(desc)
char *desc;
{
    if (!fsys_commit(desc)){
	oct_error("Cannot commit facet: %s", fsys_error_message);
	return OCT_ERROR;
    }
    return OCT_OK;
}

octStatus
oct_lock(facet)
struct facet *facet;
{
    if (facet->facet_flags.locked) {
	return OCT_OK;
    }
	
    if (!fsys_lock(facet->file_desc)){
	oct_error("Cannot lock facet: %s", fsys_error_message);
	return OCT_ERROR;
    }

    facet->facet_flags.locked = 1;
    return OCT_OK;
}

octStatus
oct_unlock(facet)
struct facet *facet;
{
    if (!(facet->facet_flags.locked)) {
	return OCT_OK;
    }
    
    if (!fsys_unlock(facet->file_desc)){
	oct_error("Cannot unlock facet: %s", fsys_error_message);
	return OCT_ERROR;
    }
    facet->facet_flags.locked = 0;
    return OCT_OK;
}
