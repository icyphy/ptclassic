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

static int figure_vertex();

/*LINTLIBRARY*/

static void do_point(geo, last, ptr, next)
fa_geometry *geo;
fa_point *last, *ptr, *next;
{
    fa_vertex_type type;

    switch (figure_vertex(last, ptr, next, &type)) {
    case 1:
	fa_put_vertex(geo, ptr->x, ptr->y, type);
	break;
    case 0:
	break;
    default:
	fa_error("fa_add_regions: Non-manhattan edge in region");
    }
}

static int
figure_vertex(last, pnt, next, type)
fa_point *last, *pnt, *next;
fa_vertex_type *type;
{
    if (last->x == pnt->x) {
	if (last->y < pnt->y) {
	    if (next->x < pnt->x) {
		*type = UPPER_RIGHT_POS;
		return 1;
	    } else if (next->x > pnt->x) {
		*type = UPPER_LEFT_NEG;
		return 1;
	    } else {
		return 0;
	    }
	} else if (last->y > pnt->y) {
	    if (next->x < pnt->x) {
		*type = LOWER_RIGHT_NEG;
		return 1;
	    } else if (next->x > pnt->x) {
		*type = LOWER_LEFT_POS;
		return 1;
	    } else {
		return 0;
	    }
	} else {
	    return 0;
	}
    } else if (last->y == pnt->y) {
	if (last->x < pnt->x) {
	    if (next->y < pnt->y) {
		*type = UPPER_RIGHT_NEG;
		return 1;
	    } else if (next->y > pnt->y) {
		*type = LOWER_RIGHT_POS;
		return 1;
	    } else {
		return 0;
	    }
	} else if (last->x > pnt->x) {
	    if (next->y < pnt->y) {
		*type = UPPER_LEFT_POS;
		return 1;
	    } else if (next->y > pnt->y) {
		*type = LOWER_LEFT_NEG;
		return 1;
	    } else {
		return 0;
	    }
	} else {
	    return 0;
	}
    } else {
	return -1;
    }
}






void fa_add_regions(geo, region)
fa_geometry *geo;
fa_region *region;
{
    fa_point *ptr, *last;
    fa_region *iregion;
    void do_point();

    while (region != NIL(fa_region)) {

	if (region->count < 4) {
	    fa_error("fa_add_regions: Degenerate region given");
	}

	do_point(geo, region->tail, region->head, region->head->next);

	last = region->head;
	ptr = region->head->next;
	
	while (ptr->next != NIL(fa_point)) {
	    do_point(geo, last, ptr, ptr->next);
	    last = ptr;
	    ptr = ptr->next;
	}
	do_point(geo, last, ptr, region->head);

	iregion = region->interior_regions;
	
	while (iregion != NIL(fa_region)) {
	    fa_add_regions(geo, iregion);
	    iregion = iregion->next;
	}

    region = region->next;
    }
  
    if (geo->count >= geo->merge_threshold) {
	fa_merge1(geo);
    }
}


void fa_free_regions(region)
fa_region *region;
{
    fa_point *ptr, *next;
    fa_region *next_region;

    while (region != NIL(fa_region)) {
	ptr = region->head;

	while (ptr != NIL(fa_point)) {
	    next = ptr->next;
	    FREE(ptr);
	    ptr = next;
	}

	fa_free_regions(region->interior_regions);

	next_region = region->next;
	FREE(region);
	region = next_region;
    }
}

void fa_regions_to_boxes(region, outbox)
fa_region *region;
fa_box **outbox;
{
    fa_geometry geo;
    
    fa_init(&geo);

    fa_add_regions(&geo, region);
    fa_to_boxes(&geo, outbox);
    
    fa_free_geo(&geo);
}

void fa_fill_regions1(region)
fa_region *region;
{
    while (region != NIL(fa_region)) {
	fa_free_regions(region->interior_regions);
	region->interior_regions = NIL(fa_region);
	region = region->next;
    }
}

void fa_fill(geo, outgeo)
fa_geometry *geo;
fa_geometry *outgeo;
{
    fa_region *region;

    fa_to_regions(geo, &region);
    fa_fill_regions1(region);
    fa_init(outgeo);
    fa_add_regions(outgeo, region);
    fa_free_regions(region);
}
