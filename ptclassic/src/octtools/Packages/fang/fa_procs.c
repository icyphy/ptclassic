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
#include "errtrap.h"
#include "fang.h"
#include "fa_internal.h"

/*LINTLIBRARY*/

char faPkgName[] = "fang";

#define update_bb(bb, x, y) {\
    if (bb.left > x) bb.left = x;\
    if (bb.right < x) bb.right = x;\
    if (bb.top < y) bb.top = y;\
    if (bb.bottom > y) bb.bottom = y;\
}

	/* an array for determining the delta of a vertex by its' 
	 * type. 
	 */

int fa_delta_array[] = {
      0,	/* NO_VERTEX */
     -1,	/* UPPER_RIGHT_NEG */
     -1,	/* UPPER_LEFT_POS */
     -1,	/* LOWER_RIGHT_POS */
     -1,	/* LOWER_LEFT_NEG */
      1,	/* UPPER_RIGHT_POS */
      1,	/* LOWER_LEFT_POS */
      1,	/* UPPER_LEFT_NEG */
      1,	/* LOWER_RIGHT_NEG */
     -2,	/* LEFT_DIAGONAL */
      2 	/* RIGHT_DIAGONAL */
};
    
    /* a table mapping a vertex type into its inverse type */

fa_vertex_type fa_inverse_type[] = {
    NO_VERTEX,  	/* NO_VERTEX */
    UPPER_RIGHT_POS,  	/* UPPER_RIGHT_NEG */
    UPPER_LEFT_NEG,  	/* UPPER_LEFT_POS */
    LOWER_RIGHT_NEG,  	/* LOWER_RIGHT_POS */
    LOWER_LEFT_POS,  	/* LOWER_LEFT_NEG */
    UPPER_RIGHT_NEG,  	/* UPPER_RIGHT_POS */
    LOWER_LEFT_NEG,  	/* LOWER_LEFT_POS */
    UPPER_LEFT_POS,  	/* UPPER_LEFT_NEG */
    LOWER_RIGHT_POS,  	/* LOWER_RIGHT_NEG */
    RIGHT_DIAGONAL,  	/* LEFT_DIAGONAL */
    LEFT_DIAGONAL  	/* RIGHT_DIAGONAL */
};

int fa_auto_merge_threshold = FA_INFINITY;
double fa_auto_merge_factor = 0.0;
    
    
void fa_debug_func(x,y)
int x, y;
{
	(void) printf("x and y are %d,%d", x, y);
}

void fa_put_vertex(outgeo, x, y, type)
fa_geometry *outgeo;
fa_coord x,y;
fa_vertex_type type;
{
    fa_vertex *ptr;

    outgeo->status &= ~ (FA_SORTED | FA_MERGED);

    if (x == FA_INFINITY || y == FA_INFINITY ||
        x == -FA_INFINITY || y == -FA_INFINITY ) {
	fa_debug_func(x,y);
    }

    ptr = ALLOC(fa_vertex, 1);

    ptr->x = x;
    ptr->y = y;
    ptr->type = type;

    if (outgeo->tail == NIL(fa_vertex) ) {
	outgeo->tail =  outgeo->head = ptr;
    } else {
	outgeo->tail->next = ptr;
	outgeo->tail = ptr;
    }

    ptr->next = NIL(fa_vertex);

    outgeo->count++;

    update_bb(outgeo->bb, x, y);
}

void fa_add_geo(geo1, geo2)
fa_geometry *geo1;
fa_geometry *geo2;
{
    fa_vertex *ptr;

    for(ptr=geo2->head; ptr != NIL(fa_vertex); ptr = ptr->next) {
	fa_put_vertex(geo1, ptr->x, ptr->y, ptr->type);
    }

    if (geo1->count >= geo1->merge_threshold) {
	(void) fa_merge1(geo1);
    }
}
    
void fa_add_box(geo, left, bottom, right, top)
fa_geometry *geo;
fa_coord left, bottom, right, top;
{
    fa_put_vertex(geo, right, top, UPPER_RIGHT_POS);
    fa_put_vertex(geo, right, bottom, LOWER_RIGHT_POS);
    fa_put_vertex(geo, left, top, UPPER_LEFT_POS);
    fa_put_vertex(geo, left, bottom, LOWER_LEFT_POS);

    if (geo->count >= geo->merge_threshold) {
	(void) fa_merge1(geo);
    }
}

void fa_init(geo)
fa_geometry *geo;
{
    
    geo->status = 0;
    geo->count = 0;
    geo->merge_threshold = fa_auto_merge_threshold;
    geo->tail = geo->head = NIL(fa_vertex);

	/* set the bounding box so that any point will
	   over write them */

    geo->bb.left = geo->bb.bottom = FA_INFINITY;
    geo->bb.right = geo->bb.top = -FA_INFINITY;

}

void fa_free_boxes(box)
fa_box *box;
{
    fa_box *next;

    while (box != NIL(fa_box)) {
	next = box->next;
	FREE(box);
	box = next;
    }
}

void fa_free_geo(geo)
fa_geometry *geo;
{
    fa_vertex *ptr, *next;

    for (ptr = geo->head; ptr != NIL(fa_vertex); ptr = next) {
	next = ptr->next;
	FREE(ptr);
	geo->count--;
    }

    if (geo->count != 0) {
	geo->count = 0;
    }
    geo->status = 0;
    geo->tail = geo->head = NIL(fa_vertex);
}

void fa_copy(geo, outgeo)
fa_geometry *geo;
fa_geometry *outgeo;
{
    fa_vertex *ptr, *copy;

    fa_init(outgeo);

    *outgeo = *geo;

    if ( (ptr = geo->head) == NIL(fa_vertex) ) {
	outgeo->tail = outgeo->head = NIL(fa_vertex);
	return;
    }

    outgeo->head = copy = ALLOC(fa_vertex, 1);
    *copy = *ptr;
    
    while ( ptr->next != NIL(fa_vertex) ) {
	ptr = ptr->next;
	copy->next = ALLOC(fa_vertex, 1);
	copy = copy->next;
	*copy = *ptr;
    }

    copy->next = NIL(fa_vertex);
    outgeo->tail = copy;
}

void fa_scale(geo, scale_factor, outgeo)
fa_geometry *geo;
double scale_factor;
fa_geometry *outgeo;
{
    fa_copy(geo, outgeo);
    fa_scale1(outgeo, scale_factor);
}

void fa_scale1(geo, scale_factor)
fa_geometry *geo;
double scale_factor;
{
    fa_vertex *ptr;

    for (ptr = geo->head; ptr != NIL(fa_vertex); ptr = ptr->next) {
	ptr->x *= scale_factor;
	ptr->y *= scale_factor;
    }

    geo->bb.top *= scale_factor;
    geo->bb.left *= scale_factor;
    geo->bb.right *= scale_factor;
    geo->bb.bottom *= scale_factor;
    
    geo->status &= ~(FA_SORTED|FA_MERGED);
}

int fa_count(geo)
fa_geometry *geo;
{
    return geo->count;
}

void fa_bb(geo, bbox)
fa_geometry *geo;
fa_box *bbox;
{
    if (geo->count <= 0) {
	fa_error("no geometry for fa_bb");
    }
    *bbox = geo->bb;
}

char *fa_print_type(type)
fa_vertex_type type;
{
    switch (type) {

	case UPPER_RIGHT_NEG : 
	    return("UPPER_RIGHT_NEG");

	case UPPER_LEFT_POS : 
	    return("UPPER_LEFT_POS");

	case LOWER_RIGHT_POS :
	    return("LOWER_RIGHT_POS");

	case LOWER_LEFT_NEG : 
	    return("LOWER_LEFT_NEG");

	case NO_VERTEX : 
	    return("NO_VERTEX");

	case UPPER_RIGHT_POS  :
	    return("UPPER_RIGHT_POS ");

	case LOWER_LEFT_POS :
	    return("LOWER_LEFT_POS");

	case UPPER_LEFT_NEG :
	    return("UPPER_LEFT_NEG");

	case LOWER_RIGHT_NEG :
	    return("LOWER_RIGHT_NEG");

	case LEFT_DIAGONAL :
	    return("LEFT_DIAGONAL");

	case RIGHT_DIAGONAL :
	    return("RIGHT_DIAGONAL");

	default :
	    return("God_only_knows");
    }
}


char fa_err_string[1000];

/*VARARGS1*/
void fa_error(string, a, b, c, d, e, f, h, i)
char *string;
int a, b, c, d, e, f, h, i;
{
    (void) sprintf(fa_err_string, string, a, b, c, d, e, f, h, i);
    errRaise(FA_PKG_NAME, 0, fa_err_string);
}

void fa_set_auto_merge(threshold, factor)
int threshold;
double factor;
{
    if (threshold == 0) {
	fa_auto_merge_threshold = FA_INFINITY;
	fa_auto_merge_factor = 0.0;
    } else {
	fa_auto_merge_threshold = threshold;
	fa_auto_merge_factor = factor;
    }
}
