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
#include "port.h"
#include "utility.h"
#include "fang.h"
#include "fa_internal.h"

/*LINTLIBRARY*/

static char *print_etype();
static int vertex_type();

#define print_vtype(type) \
    ( type == FA_CONCAVE ? "FA_CONCAVE" : "FA_CONVEX" )

void fa_dump_edge(edge)
fa_edge *edge;
{
    (void) fprintf(stderr, "edge (%d(%s),%d(%s)) at %d (%s)\n", edge->start,
		print_vtype(edge->type.low_corner),
		edge->end,
		print_vtype(edge->type.high_corner),
		edge->center,
		print_etype((int) edge->type.edge));
}

static char *
print_etype(type)
int type;
{
    switch (type) {
	case FA_LEFT : return ("FA_LEFT"); break;
	case FA_RIGHT : return ("FA_RIGHT"); break;
	case FA_UP : return ("FA_UP"); break;
	case FA_DOWN : return ("FA_DOWN"); break;
    }

    return("GOD KNOWS");
}

void fa_dump_edges(list)
fa_edge *list;
{
    while (list != NIL(fa_edge)) {
	fa_dump_edge(list);
	list = list->next;
    }
}

void fa_free_edges(list)
fa_edge *list;
{
    fa_edge *next;

    while (list != NIL(fa_edge)) {
	next = list->next;
	FREE(list);
	list = next;
    }
}

#define DO_TO_EDGES(geo, coord1, coord2, plus_type, neg_type)\
head = NIL(fa_edge);\
for( ptr = (geo)->head; ptr != NIL(fa_vertex); ) {\
\
    if (head == NIL(fa_edge)) {\
	head = new = ALLOC(fa_edge, 1);\
    } else {\
	new->next = ALLOC(fa_edge, 1);\
	new = new->next;\
    }\
    \
    if ( (next = ptr->next) == NIL(fa_vertex) ) {\
	fa_error("Odd number of vertices in geometry");\
    }\
\
    if ( next->coord1 != ptr->coord1 ) {\
	fa_error("Odd number of vertices in scanline");\
    }\
\
    if (fa_vertex_delta(ptr->type) > 0) {\
	new->type.edge = plus_type;\
    } else {\
	new->type.edge = neg_type;\
    }\
\
    new->type.low_corner = vertex_type(ptr->type);\
    new->type.high_corner = vertex_type(next->type);\
    new->center = ptr->coord1;\
    new->start = ptr->coord2;\
    new->end = next->coord2;\
    new->next = NIL(fa_edge);\
\
    ptr = next->next;\
}

void fa_to_edges(geo, horiz_edges, vert_edges)
fa_geometry *geo;
fa_edge **horiz_edges, **vert_edges;
{
    fa_geometry rev_geo;
    register fa_vertex *ptr, *next;
    fa_edge *new=NULL, *head;
    
    fa_merge1(geo);

    DO_TO_EDGES(geo, y, x, FA_UP, FA_DOWN);
    *horiz_edges = head;
    
    fa_copy(geo, &rev_geo);
    fa_rev_sort(&rev_geo);
    
    DO_TO_EDGES(&rev_geo, x, y, FA_RIGHT, FA_LEFT);
    *vert_edges = head;
    
    fa_free_geo(&rev_geo);
}
    
static int vertex_type(type)
fa_vertex_type type;
{
    switch(type) {
	case UPPER_LEFT_POS :
	case LOWER_RIGHT_POS :
	case UPPER_RIGHT_POS :
	case LOWER_LEFT_POS :
	    return(FA_CONVEX);

	case LOWER_LEFT_NEG :
	case UPPER_RIGHT_NEG :
	case UPPER_LEFT_NEG :
	case LOWER_RIGHT_NEG :
        case NO_VERTEX:
	case LEFT_DIAGONAL: 
	case RIGHT_DIAGONAL: 
	    return(FA_CONCAVE);
	}
    
    return(FA_CONCAVE);
}
