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

int32 oct_null_xid = 0;

xid_table *oct_make_xid_table(size)
int32 size;
{
    if (size == 0) {
	size = 23;
    } else {
	size /= 2;
	if (size <= 0) {
	    size = 1;
	}
    }
    
    return (xid_table *)
      st_init_table_with_params(st_numcmp, st_numhash, (int) size, 5, 2.0, 0);
}

void
oct_free_xid_table(table)
xid_table *table;
{
    st_free_table((st_table *) table);
    return;
}


int32
oct_next_xid(facet, ptr)
struct facet *facet;
generic *ptr; 
{
    int32 xid = facet->next_xid++;
    
    if ( facet->xid_table == NIL( xid_table ) ) {
	/* XXX I did not really expect this to be 0 at this point */
	/* Make a desperate attempt to continue by creating the table */
	/* AC Aug 15 1991 */
	facet->xid_table = oct_make_xid_table(0); 
    }
    (void) st_add_direct((st_table *) facet->xid_table, (char *) xid, (char *) ptr);
    return xid;
}


void
oct_add_xid(facet, xid, ptr)
struct facet *facet;
int32 xid;
generic *ptr;
{
    (void) st_add_direct((st_table *) facet->xid_table, (char *) xid, (char *) ptr);
    return;
}


void
oct_free_xid(facet, xid)
struct facet *facet;
int32 xid;
{
    char *junk, *key = (char *) xid;

    (void) st_delete((st_table *) facet->xid_table, &key, &junk);
    return;
}


generic *
oct_xid_to_ptr(desc, xid)
struct facet *desc;
int32 xid;
{
    char *ptr;
    char *key = (char *) xid;

    if (!st_lookup((st_table *) desc->xid_table, key, &ptr)) {
	return NIL(generic);
    }
    return (struct generic *) ptr;
}


octStatus 
octGetByExternalId(container, xid, object)
octObject *container;
int32 xid;
octObject *object;
{
    generic *ptr;

    ptr = oct_id_to_ptr(container->objectId);

    if (ptr == NIL(generic)) {
	oct_error("octGetByExternalId: The first argument's octId has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    if (ptr->flags.type != OCT_FACET) {
	oct_error("octGetByExternalId: The first argument must be a facet");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    ptr = oct_xid_to_ptr((struct facet *) ptr, xid);

    if (ptr == NIL(generic)) {
	oct_error_string = "No object exists with that external id";
	return OCT_NOT_FOUND;
    }

    oct_fill_object(object, ptr);

    return OCT_OK;
}


void 
octExternalId(object, xid)
octObject *object;
int32 *xid;
{
    generic *ptr = oct_id_to_ptr(object->objectId);

    if (ptr == NIL(generic)) {
	oct_error("octExternalId: The first argument's octId has been corrupted");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }
    *xid = ptr->externalId;
    return;
}
