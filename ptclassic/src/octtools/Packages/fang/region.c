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

typedef struct region_region region_region;
typedef struct region_point region_point;

struct region_region {
    fa_region region;
    region_point *head, *tail;
};

struct region_point {
    fa_coord x,y;
    int delta;
    region_region *region;
    region_point *next, *last;
};

typedef struct region_line region_line;

struct region_line {
    region_point *head, *tail;
};

#include "region_procs.h"	/* contains region_scan and 
				   region_check_emit */

void fa_to_regions(geo, outregion)
fa_geometry *geo;
fa_region **outregion;
{
    fa_vertex *vertex;
    region_line line;
    region_point *pointer;
    fa_coord current_y;
    int delta;

    fa_merge1(geo);

    *outregion = NIL(fa_region);
    
    region_init_line(&line, geo);

    vertex = geo->head;

    while ( vertex != NIL(fa_vertex) ) {
	
	current_y = vertex->y;
	pointer = line.head;

	while ( vertex != NIL(fa_vertex) && vertex->y == current_y) {
	    
	    delta = fa_vertex_delta( vertex->type );
	    pointer = region_scan(pointer, vertex->x);

	    if ( pointer->x == vertex->x ) {
		pointer->delta += delta;
	    } else {
		region_insert_point(pointer, vertex->x, vertex->y, delta);
		pointer = pointer->next;
	    }

	    region_check_emit( outregion, pointer, vertex );

	    if (pointer->delta == 0) {
		pointer = pointer->last;
		region_remove_point(pointer->next);
	    }

	    fa_next_vertex(&vertex);

	}

    }

    if ( line.head->next != line.tail ) {
	fa_error("Panic : Non-empty scan line at end of fa_to_region");
    }

    region_free_line(line);
}
