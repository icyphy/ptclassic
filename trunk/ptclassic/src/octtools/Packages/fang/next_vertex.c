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
#include "port.h"
#include "utility.h"
#include "fang.h"
#include "fa_internal.h"

    /* get the next vertex from the vertex list, merging
       abutting vertices in either LEFT_DIAGONAL or RIGHT_DIAGONAL
     */
void fa_next_vertex(result)
fa_vertex **result;
{
    register fa_vertex *vertex, *next;
    static fa_vertex merged_vertex;
    int delta;

    if (*result == NIL(fa_vertex) ) {
	fa_error("Panic : Unexpected NIL vertex in next_vertex");
    }

    *result = vertex = (*result)->next;

    if (*result == NIL(fa_vertex) ) {
	return;
    }

    next = (*result)->next;

    if (next != NIL(fa_vertex)) {

	if (vertex->x == next->x && vertex->y == next->y) {

	    merged_vertex = *vertex;

	    delta = fa_vertex_delta(vertex->type) +
			fa_vertex_delta(next->type);

	    if (delta == 2) {
		merged_vertex.type = RIGHT_DIAGONAL;
	    } else if (delta == -2) {
		merged_vertex.type = LEFT_DIAGONAL;
	    } else {

		fa_error("Panic : Improper overlapping vertices, (x,y) = (%d,%d)",
			    vertex->x, vertex->y);
	    }

		/* eat both vertices and splice in the merged one */

	    merged_vertex.next = (*result)->next->next;
	    *result = &merged_vertex;

	    return;
	}
    }
}
