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
/*
 * VEM Buffer Management
 * Common routines in buffer module
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This file contains implementations for common routines
 * used in all the files that implement the buffer module.
 * Globally defined resources are also declared here.
 */

#include "bufinternal.h"	/* Internal view of buffer module */

/* Global hash table of all buffers */
st_table *_bufTable = (st_table *) 0;
st_table *_activeBufTable = (st_table *) 0; /* Table of buggers that may be active. */

void bufMarkActive( id )
    octId id;
{
    intern_buffer* buf = _bufFindBuffer( id );
    if ( !_activeBufTable ) {
	_activeBufTable = st_init_table( st_numcmp, st_numhash );
    }
    st_insert( _activeBufTable, (char*)id, (char*)buf );
}


intern_buffer *_bufFindBuffer(id)
octId id;			/* Facet object Id */
/*
 * This routine translates a facet id into an internal
 * buffer structure.  If the facet was opened with
 * bufOpen() or bufOpenMaster(),  this will be a simple
 * table lookup.  Otherwise,  the buffer is opened
 * using bufOpen.
 */
{
    intern_buffer *buf;
    octObject fct;

    if (!st_lookup(_bufTable, (char *) id, (char **) &buf)) {
	/* Didn't find it in the table -- try to open it */
	fct.objectId = id;
	if (octGetById(&fct) < OCT_OK) return (intern_buffer *) 0;
	if (bufOpen(&fct, BUFSILENT|BUFMUSTEXIST) < VEM_OK) {
	    if ( !octIdsEqual( fct.objectId, id )  ) { /* Avoid calling recursively with same arg. */
		return _bufFindBuffer(fct.objectId);
	    }
	} 
	return (intern_buffer *) 0;
    }
    return buf;
}
