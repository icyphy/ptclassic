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

#define max(x,y)	((x) > (y) ? (x) : (y))

static void do_merge();
			   
typedef struct vertex_sum vertex_sum;

int fa_debug = 0;

struct vertex_sum {
    fa_coord x,y;
    int delta;	/* cumulative delta of the vertex */
};

#include "merge_procs.h"	/* contains merge_scan and 
				   merge_check_emit */
#include "get_vertex.h"		/* contains get_vertex_sum */

void fa_merge(geo, outgeo)
fa_geometry *geo;
fa_geometry *outgeo;
{
    fa_sort(geo);
    do_merge(geo->head, outgeo, 0);
}
    
void fa_merge1(geo)
fa_geometry *geo;
{
    fa_sort(geo);
    if (geo->status & FA_MERGED) return;
    do_merge(geo->head, geo, 1);
}
    
static void do_merge(vertex_list, outgeo, do_free)
fa_vertex *vertex_list;
fa_geometry *outgeo;
int do_free;
{
    vertex_sum vertex;
    scan_line line;
    scan_point *pointer;
    fa_coord current_y;

    int l_delta;

    fa_init_line(&line);
    fa_init(outgeo);

    vertex = get_vertex_sum(&vertex_list, do_free);

    while ( vertex.delta != 0 ) {
	
	current_y = vertex.y;
	l_delta = 0;
	pointer = line.head;

	while ( vertex.delta != 0 && vertex.y == current_y) {
	    
	    pointer = merge_scan(outgeo, pointer, vertex, l_delta);

	    if ( pointer->x == vertex.x ) {
		pointer->delta += vertex.delta;
	    } else {
		fa_insert_point(pointer, vertex.x, vertex.y,
				vertex.delta);
		pointer = pointer->next;
	    }

	    merge_check_emit( 
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

	    vertex = get_vertex_sum(&vertex_list, do_free);
	}

	if ( l_delta != 0) {
	    fa_error("Unterminated edge at y = %d in merge", current_y);
	}

    }

    if ( line.head->next != line.tail ) {
	fa_error("Panic : Non-empty scan line at end of merge");
    }

    fa_free_line(line);

    outgeo->status |= (FA_MERGED | FA_SORTED);
    outgeo->merge_threshold = max(fa_auto_merge_threshold,
				    outgeo->count * fa_auto_merge_factor);
}
