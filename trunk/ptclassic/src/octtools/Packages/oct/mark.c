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
#include "st.h"
#include "mark.h"

extern int fsys_compare_key(), fsys_hash_key();


/* 
 * The `mark' functions are used to assign a unique sequential id 
 * number to each object in a facet as it is written out to disk.  
 */

static st_table *mark_table = (st_table *) 0;

void
oct_init_marks()
{
    mark_table = st_init_table(st_ptrcmp, st_ptrhash);
    return;
}

void
oct_mark_ptr(ptr, id)
generic *ptr;
octId id;
{
    (void) st_insert(mark_table, (char *)ptr, (char *)id);
    return;
}

int
oct_lookup_mark(ptr, id)
generic *ptr;
octId *id;
{
    char *junk;
    int retval = st_lookup(mark_table, (char *) ptr, &junk);
    *id = (octId) junk;
    return retval;
}

void
oct_clear_marks()
{
    st_free_table(mark_table);
}

/* 
 * The facet functions manage a hash table of in memory facets, keyed 
 * by their file_desc as assigned by the fsys layer
 */

static st_table *facet_table;

void
oct_init_facet_table()
{
    facet_table = st_init_table(fsys_compare_key, fsys_hash_key);
    return;
}

void
oct_free_facet_table()
{
    st_free_table(facet_table);
    return;
}

void
oct_mark_facet(desc)
    struct facet *desc;
{
    (void) st_insert(facet_table, desc->file_desc, (char *)desc);
    return;
}

int
oct_unmark_facet(desc)
    struct facet *desc;
{
    char *junk = desc->file_desc;
    return st_delete(facet_table, &junk, (char **)&desc);
}

st_generator *oct_init_gen_facet_table()
{
    return st_init_gen(facet_table);
}

int
oct_lookup_facet(file_desc, desc)
    char *file_desc;
    struct facet **desc;
{
    char *junk;
    int retval = st_lookup(facet_table, file_desc, &junk);
    *desc = (struct facet *) junk;
    return retval;
}

#ifdef DEBUG
#ifndef lint
/* 
 * for debugging only
 */
/*ARGSUSED*/
static enum st_retval
do_dump_facet_table(key, value, junk)
char *key, *value, *junk;
{
    struct facet *facet = (struct facet *) value;

    fsys_print_key(stderr, key);
    (void) fprintf(stderr, "\n\tid:%d ref:%d %s:%s:%s:%s\n",
	    facet->objectId,facet->ref_count,
	    facet->user_facet.cell, facet->user_facet.view,
	    facet->user_facet.facet,
	    (facet->user_facet.version == (char *) 0 ? "" :
	     facet->user_facet.version));
    return ST_CONTINUE;
}

static
dump_facet_table()
{
    (void) st_foreach(facet_table, do_dump_facet_table, (char *) 0);
}
#endif /* lint */
#endif /* DEBUG */
