/* Version Identification:
 * $Id$
 */
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
static vertex_sum get_vertex_sum(ptr, do_free)
fa_vertex **ptr;
int do_free;
{
    vertex_sum sum;

    sum.delta = 0;

    while (sum.delta == 0 && *ptr !=  NIL(fa_vertex)) {
	sum.x = (*ptr)->x;
	sum.y = (*ptr)->y;
	sum.delta = fa_vertex_delta((*ptr)->type);

	if (do_free) {
	    fa_vertex *next = (*ptr)->next;
	    FREE(*ptr);
	    *ptr = next;
	} else {
	    *ptr = (*ptr)->next;
	}

	while( *ptr != NIL(fa_vertex) 
	       && (*ptr)->x == sum.x 
	       && (*ptr)->y == sum.y) {
	    sum.delta += fa_vertex_delta((*ptr)->type);
	    
	    if (do_free) {
		fa_vertex *next = (*ptr)->next;
		FREE(*ptr);
		*ptr = next;
	    } else {
		*ptr = (*ptr)->next;
	    }
	}
    }

    return (sum);
}
