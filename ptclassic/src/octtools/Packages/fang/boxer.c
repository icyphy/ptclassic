#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1994 The Regents of the University of California.
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
#include "port.h"
#include "utility.h"
#include "fang.h"
#include "scan.h"
#include "fa_internal.h"

/*LINTLIBRARY*/
static void put_box();


#include "boxer_procs.h"	/* contains boxer_scan and 
				   boxer_check_emit */

void fa_to_boxes(geo, outbox)
fa_geometry *geo;
fa_box **outbox;
{
    fa_vertex *vertex;
    scan_line line;
    scan_point *pointer;
    fa_coord current_y;
    int delta;

    fa_merge1(geo);

    fa_init_line(&line);
    *outbox = NIL(fa_box);

    vertex = geo->head;

    while ( vertex != NIL(fa_vertex) ) {
	
	current_y = vertex->y;
	pointer = line.head;

	while ( vertex != NIL(fa_vertex) && vertex->y == current_y) {
	    
	    delta = fa_vertex_delta( vertex->type );
	    pointer = boxer_scan(pointer, vertex->x);

	    if ( pointer->x == vertex->x ) {
		pointer->delta += delta;
	    } else {
		fa_insert_point(pointer, vertex->x, vertex->y, delta);
		pointer = pointer->next;
	    }

	    boxer_check_emit( outbox, pointer, vertex );

	    if (pointer->delta == 0) {
		pointer = pointer->last;
		fa_remove_point(pointer->next);
	    }

	    fa_next_vertex(&vertex);

	}

    }

    if ( line.head->next != line.tail ) {
	fa_error("Panic : Non-empty scan line at end of boxer");
    }

    fa_free_line(line);
}
