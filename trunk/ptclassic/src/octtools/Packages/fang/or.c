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
#include "scan.h"
#include "fa_internal.h"

/*LINTLIBRARY*/

typedef struct vertex_sum vertex_sum;

struct vertex_sum {
    fa_coord x,y;
    int delta;	/* cumulative delta of the vertex */
};

#include "or_procs.h"	/* contains or_scan and 
				   or_check_emit */

void fa_or(geo1, geo2, outgeo)
fa_geometry *geo1, *geo2;
fa_geometry *outgeo;
{
    vertex_sum vertex;
    scan_line line;
    fa_vertex *vlist1, *vlist2;
    scan_point *pointer;
    fa_coord current_y;

    int l_delta;

    fa_sort(geo1);
    fa_sort(geo2);

    fa_init_line(&line);
    fa_init(outgeo);

    vlist1 = geo1->head;
    vlist2 = geo2->head;
    vertex = or_get_vertex_sum(&vlist1, &vlist2);

    while ( vertex.delta != 0 ) {
	
	current_y = vertex.y;
	l_delta = 0;
	pointer = line.head;

	while ( vertex.delta != 0 && vertex.y == current_y) {
	    
	    pointer = or_scan(outgeo, pointer, vertex, l_delta);

	    if ( pointer->x == vertex.x ) {
		pointer->delta += vertex.delta;
	    } else {
		fa_insert_point(pointer, vertex.x, vertex.y,
				vertex.delta);
		pointer = pointer->next;
	    }

	    or_check_emit( 
		outgeo, vertex.x, vertex.y,
		l_delta,
		pointer->last->r_depth,
		l_delta + vertex.delta,
		pointer->r_depth = pointer->last->r_depth + pointer->delta
	    );

	    l_delta += vertex.delta;

	    if (pointer->delta == 0) {
		pointer = pointer->last;
		fa_remove_point(pointer->next);
	    }

	    vertex = or_get_vertex_sum(&vlist1, &vlist2);
	}

	if ( l_delta != 0) {
	    fa_error("Unterminated edge at y = %d in or", current_y);
	}

    }

    if ( line.head->next != line.tail ) {
	fa_error("Panic : Non-empty scan line at end of or");
    }

    fa_free_line(line);

    outgeo->status |= (FA_SORTED | FA_MERGED);
}
