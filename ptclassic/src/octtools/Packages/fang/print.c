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
#include "fa_internal.h"

/*LINTLIBRARY*/
static void do_print_region();

void fa_dump_vertices(vertices)
fa_vertex *vertices;
{
    int count = 0;

    while (vertices != NIL(fa_vertex)) {

	(void) fprintf(stderr, "vertex at (%d,%d) of type %s\n", vertices->x,
		vertices->y, fa_print_type(vertices->type));
	vertices = vertices->next;
	count++;
    }

    (void) fprintf(stderr, "%d vertices in all\n\n", count);
    (void) fflush(stderr);
}

void fa_dump_geo(geo)
fa_geometry *geo;
{
    (void) fprintf(stderr, "\tFANG GEOMETRY ----- %ssorted, %smerged\n",
		(geo->status & FA_SORTED) ? "" : "un",
		(geo->status & FA_MERGED) ? "" : "un");
    fa_dump_vertices(geo->head);
}

void fa_dump_boxes(boxes)
fa_box *boxes;
{
    fa_print_boxes(boxes, 1, stderr);
}

void fa_print_boxes(boxes, scale, stream)
fa_box *boxes;
int scale;
FILE *stream;
{
    int l,b,r,t;

    while (boxes != NIL(fa_box)) {

	l = boxes->left*scale;
	r = boxes->right*scale;
	b = boxes->bottom*scale;
	t = boxes->top*scale;

	(void) fprintf(stream,"B %d %d %d %d;\n", r - l, t - b, (r+l)>>1, 
		    (t+b)>>1);
	boxes = boxes->next;
    }
}

void fa_dump_regions(list)
fa_region *list;
{
    fa_print_regions(list, stderr);
}

void fa_print_regions(ptr, stream)
fa_region *ptr;
FILE *stream;
{
    
    int i = 0;

    while (ptr != NIL(fa_region)) {
	(void) fprintf(stream, "Region #%d: \n", ++i);
	fa_print_region(ptr, stream);
	ptr = ptr->next;
    }

    (void) fprintf(stream, "%d regions in all\n", i);
}

void fa_dump_region(region)
fa_region *region;
{
    fa_print_region(region, stderr);
}

void fa_print_region(region, stream)
fa_region *region;
FILE *stream;
{
    do_print_region(region, stream, 0);
}

static void print_indent(indent, stream)
int indent;
FILE *stream;
{
    while (indent-- > 0) {
	(void) fprintf(stream, "    ");
    }
}

static void do_print_region(region, stream, indent)
fa_region *region;
FILE *stream;
int indent;
{
    fa_point *ptr = region->head;
    int i = 0;

    print_indent(indent, stream);
    
    while (ptr != NIL(fa_point)) {
	(void) fprintf(stream, "(%d, %d) ", ptr->x, ptr->y);
	if (++i % 3 == 0) {
	    (void) fprintf(stream, "\n");
	    print_indent(indent, stream);
	}
	ptr = ptr->next;
    }
    
    if (i%3 != 0) {
	(void) fprintf(stream, "\n");
	print_indent(indent, stream);
    }
    
    if (i != region->count) {
	(void) fprintf(stream, "Point count mismatch: claimed %d, found %d\n",
		region->count, i);
    } else {
	(void) fprintf(stream, "%d points in all\n", i);
    }

    if (region->interior_regions != NIL(fa_region)) {
	fa_region *iregion;
	
	print_indent(indent, stream);
	(void) fprintf(stream, "Interior regions: \n");
	iregion = region->interior_regions;
	while(iregion != NIL(fa_region)) {
	    do_print_region(iregion, stream, indent + 1);
	    iregion = iregion->next;
	}
    }
}


/*
 * print.c
 *
 * Copyright -C- 1987	Brian D. N. Lee
 * University of California, Berkeley
 *
 * Extra fang diagnostic routines.
 */

int fa_print_vertices(vertices, stream)
fa_vertex *vertices;
FILE *stream;
{
    int count = 0;

    while (vertices != 0) {

	fprintf(stream, "vertex at (%d,%d) of type %s\n", vertices->x,
		vertices->y, fa_print_type(vertices->type));
	vertices = vertices->next;
	count++;
    }

    fprintf(stream, "%d vertices in all\n\n", count);
    (void) fflush(stream);
    return 0;
}


int fa_print_geo(geo, stream)
fa_geometry *geo;
FILE *stream;
{
    fprintf(stream, "\tFANG GEOMETRY ----- %ssorted, %smerged\n",
		(geo->status & FA_SORTED) ? "" : "un",
		(geo->status & FA_MERGED) ? "" : "un");
    fa_print_vertices(geo->head, stream);
    return 0;
}

