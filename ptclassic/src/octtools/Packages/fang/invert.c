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

/*LINTLIBRARY*/

void fa_invert(geo, bbox, outgeo)
fa_geometry *geo;
fa_box *bbox;
fa_geometry *outgeo;
{
    fa_vertex *ptr, *loc;

    fa_sort(geo);

    fa_init(outgeo);
    fa_copy(geo, outgeo);

    if (bbox->left > outgeo->bb.left 
	    || bbox->bottom > outgeo->bb.bottom 
	    || bbox->right < outgeo->bb.right 
	    || bbox->top < outgeo->bb.top) {
	fa_error("Bounding box for inversion is smaller than the bounding box for the geometry");
    }

    outgeo->bb = *bbox;

    if (outgeo->head == NIL(fa_vertex)) {
	/* empty geometry */
	fa_add_box(outgeo, bbox->left, bbox->bottom, bbox->right, bbox->top);
	return;
    }

    for(ptr = outgeo->head; ptr != NIL(fa_vertex); ptr = ptr->next) {
	ptr->type = fa_inverse_type[(int) ptr->type];
    }

    /* add the lower edge of the bounding box. 
       Be careful to keep things properly sorted in the case
       where part of the original geometry touches the bbox */


    ptr = ALLOC(fa_vertex, 1);
    ptr->x = bbox->left;
    ptr->y = bbox->bottom;
    ptr->type = LOWER_LEFT_POS;
    ptr->next = outgeo->head; 
    outgeo->head = ptr;
    if (ptr->next == NIL(fa_vertex)) {
	outgeo->tail = ptr;
    }

    loc = outgeo->head;
    while (loc->next != NIL(fa_vertex) && loc->next->y == bbox->bottom) {
	loc = loc->next;
    }

    ptr = ALLOC(fa_vertex, 1);
    ptr->x = bbox->right;
    ptr->y = bbox->bottom;
    ptr->type = LOWER_RIGHT_POS;
    ptr->next = loc->next; 
    loc->next = ptr;
    if (loc == outgeo->tail) {
	outgeo->tail = ptr;
    }

	/* add the upper edge */

    loc = outgeo->head;
    while (loc->next != NIL(fa_vertex) && loc->next->y < bbox->top) {
	loc = loc->next;
    }

    ptr = ALLOC(fa_vertex, 1);
    ptr->x = bbox->left;
    ptr->y = bbox->top;
    ptr->type = UPPER_LEFT_POS;
    ptr->next = loc->next; 
    loc->next = ptr;
    if (loc == outgeo->tail) {
	outgeo->tail = ptr;
    }

    outgeo->count += 3;

    fa_put_vertex(outgeo, bbox->right, bbox->top, UPPER_RIGHT_POS);

    if (fa_debug) {
	(void) fprintf(stderr,"Invert, on output : \n");
	fa_dump_vertices(outgeo->head);
    }
}
