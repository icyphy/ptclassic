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
#include "sort.h"
/* forward declaration */
static void or_check_emit();

static scan_point *or_scan(outgeo, pointer, vertex, l_delta)
fa_geometry *outgeo;
scan_point *pointer;
vertex_sum vertex;
int l_delta;
{
    register scan_point *ptr;
    register int x = vertex.x;
    
    if (l_delta == 0) {

	    /* if there are no changes to propagate, go
	     * directly to the correct position
	     */

	ptr = pointer;
	while ( ptr->next->x <= x ) {
	    ptr = ptr->next;
	}

	return(ptr);
    }

	/* step through the list, updating each point as we go 
	 * and checking to see if any new vertices need to be 
	 * emitted because of the change.
	 */

    for (ptr = pointer->next; ptr->x < x; ptr = ptr->next) {

	or_check_emit(
	    outgeo, ptr->x, vertex.y,
	    l_delta,
	    ptr->last->r_depth,
	    l_delta,
	    ptr->r_depth = ptr->last->r_depth + ptr->delta
	);

    }

    if (ptr->x != x) {
	ptr = ptr->last;
    }

    return (ptr);
}

static void or_check_emit(geo, x, y, l_delta, l_new, r_delta, r_new)
fa_geometry *geo;
fa_coord x,y;
int l_delta, l_new;
int r_delta, r_new;
{
    int l_old = l_new - l_delta,
	r_old = r_new - r_delta;

    if ( l_delta != 0 ) {

	if ( l_delta > 0 ) {

	    if ( l_old <= 0 && l_new > 0 ) {
		if ( r_new <= 0 ) {
		    fa_put_vertex(geo, x, y, LOWER_RIGHT_POS);
		} else if ( r_old > 0 ) {
		    fa_put_vertex(geo, x, y, UPPER_RIGHT_NEG);
		} 
	    }

	} else {	/* l_delta < 0 */

	    if ( l_new <= 0 && l_old > 0 ) {
		if ( r_old <= 0 ) {
		    fa_put_vertex(geo, x, y, UPPER_RIGHT_POS);
		} else if ( r_new > 0 ) {
		    fa_put_vertex(geo, x, y, LOWER_RIGHT_NEG);
		}
	    }

	}

    }

    if ( r_delta != 0 ) {

	if ( r_delta > 0 ) {

	    if ( r_old <= 0 && r_new > 0 ) {
		if ( l_new <= 0 ) {
		    fa_put_vertex(geo, x, y, LOWER_LEFT_POS);
		} else if ( l_old > 0 ) {
		    fa_put_vertex(geo, x, y, UPPER_LEFT_NEG);
		} 
	    }

	} else {	/* r_delta < 0 */

	    if ( r_new <= 0 && r_old > 0 ) {
		if ( l_old <= 0 ) {
		    fa_put_vertex(geo, x, y, UPPER_LEFT_POS);
		} else if ( l_new > 0 ) {
		    fa_put_vertex(geo, x, y, LOWER_LEFT_NEG);
		}
	    }

	}

    }
    
}

static fa_vertex *or_next_vertex(ptr1, ptr2)
fa_vertex **ptr1, **ptr2;
{
    fa_vertex *ptr;

    if (*ptr1 != NIL(fa_vertex) && *ptr2 != NIL(fa_vertex)) {
	if (fa_compar_vertex(*ptr1, *ptr2) < 0) {
	    ptr = *ptr1;
	    *ptr1 = (*ptr1)->next;
	    return(ptr);
	} else {
	    ptr = *ptr2;
	    *ptr2 = (*ptr2)->next;
	    return(ptr);
	}
    }

    if (*ptr1 != NIL(fa_vertex)) {
	ptr = *ptr1;
	*ptr1 = (*ptr1)->next;
	return(ptr);
    }

    if (*ptr2 != NIL(fa_vertex)) {
	ptr = *ptr2;
	*ptr2 = (*ptr2)->next;
	return(ptr);
    }

    return(NIL(fa_vertex));
}

static vertex_sum or_get_vertex_sum(ptr1, ptr2)
fa_vertex **ptr1, **ptr2;
{
    static fa_vertex *ptr = NIL(fa_vertex);
    vertex_sum sum;

    sum.delta = 0;

    if (ptr == NIL(fa_vertex)) {
	ptr = or_next_vertex(ptr1,ptr2);
    }

    while (sum.delta == 0 && ptr !=  NIL(fa_vertex)) {
	sum.x = ptr->x;
	sum.y = ptr->y;
	sum.delta = fa_vertex_delta(ptr->type);

	ptr = or_next_vertex(ptr1,ptr2);

	while( ptr != NIL(fa_vertex) && ptr->x == sum.x 
	       && ptr->y == sum.y) {
	    sum.delta += fa_vertex_delta(ptr->type);
	    ptr = or_next_vertex(ptr1,ptr2);
	}
    }
    return(sum);
}
