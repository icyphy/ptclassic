/* Version Identification:
 * $Id$
 */
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
/*ARGSUSED*/
static void region_init_line(line, geo)
region_line *line;
fa_geometry *geo;
{
    line->head = ALLOC(region_point, 1);
    line->tail = ALLOC(region_point, 1);
    line->head->x = -FA_INFINITY;
    line->tail->x = FA_INFINITY;
    line->head->region = line->tail->region = NIL(region_region);
    line->head->y = line->tail->y = -FA_INFINITY;
    line->head->delta = line->tail->delta = 0;

    line->tail->next = NIL(region_point);
    line->tail->last = line->head;
    line->head->next = line->tail;
    line->head->last = NIL(region_point);
}

static void region_insert_point(pointer, x, y, delta)
region_point *pointer;
fa_coord x;
fa_coord y;
int delta;
{
    register region_point *ptr;

    ptr = ALLOC(region_point, 1);

    pointer->next->last = ptr;
    ptr->next = pointer->next;
    ptr->last = pointer;
    pointer->next = ptr;

    ptr->x = x;
    ptr->y = y;
    ptr->delta = delta;
    ptr->region = NIL(region_region);

}


static void region_free_line(line)
region_line line;
{
    FREE(line.head);
    FREE(line.tail);
}

static void region_remove_point(pointer)
region_point *pointer;
{
    pointer->last->next = pointer->next;
    pointer->next->last = pointer->last;
    FREE(pointer);
}

static region_point *region_scan(pointer, x)
register region_point *pointer;
register fa_coord x;
{

    while ( pointer->next->x <= x ) {
	    pointer = pointer->next;
    }

    return(pointer);
}

static void new_region(head_pointer, tail_pointer, tail_y)
region_point *head_pointer;
region_point *tail_pointer;
fa_coord tail_y;
{
    region_region *ptr = ALLOC(region_region, 1);
    
    ptr->region.head = ALLOC(fa_point, 1);
    ptr->region.head->x = head_pointer->x;
    ptr->region.head->y = head_pointer->y;
    
    ptr->region.tail = ALLOC(fa_point, 1);
    ptr->region.tail->x = tail_pointer->x;
    ptr->region.tail->y = tail_y;

    ptr->region.head->next = ptr->region.tail;
    ptr->region.tail->next = NIL(fa_point);
    
    ptr->head = head_pointer;
    ptr->tail = tail_pointer;

    head_pointer->region = tail_pointer->region = ptr;
    
    ptr->region.next = ptr->region.interior_regions = NIL(fa_region);
    ptr->region.count = 2;
}

static void push_head(region, pointer, y)
region_region *region;
region_point *pointer;
fa_coord y;
{
    fa_point *new = ALLOC(fa_point, 1);

    new->x = pointer->x;
    new->y = y;
    
    new->next = region->region.head;
    region->region.head = new;
    
    region->head = pointer;
    region->region.count++;
}

static void push_tail(region, pointer, y)
region_region *region;
region_point *pointer;
fa_coord y;
{
    fa_point *new = ALLOC(fa_point, 1);

    new->next = NIL(fa_point);
    new->x = pointer->x;
    new->y = y;
    
    region->region.tail->next = new;
    region->region.tail = new;
    
    region->tail = pointer;
    region->region.count++;
}

static void merge_region(head_region, tail_region)
region_region *head_region, *tail_region;
{
    struct fa_region *ptr;
    
        /* splice tail_regions points on to the tail of head_region */

    head_region->region.tail->next = tail_region->region.head;
    head_region->region.tail = tail_region->region.tail;
    head_region->tail = tail_region->tail;
    head_region->tail->region = head_region;
    head_region->region.count += tail_region->region.count;
    
        /* merge tail_regions interior polygon list onto head_region's */
    
    ptr = head_region->region.interior_regions;
    if (ptr != NIL(fa_region)) {
	while (ptr->next != NIL(fa_region)) {
	    ptr = ptr->next;
	}
	ptr->next = tail_region->region.interior_regions;
    } else {
	head_region->region.interior_regions =
	  tail_region->region.interior_regions;
    }
    
    FREE(tail_region);
}

/* add iregion to the interior_regions list of cregion */

#define PUSH_INTERIOR(cregion, iregion)\
{\
    (iregion)->next = (cregion)->interior_regions;\
    (cregion)->interior_regions = (iregion);\
}

/* add region to the list of complete regions, outregion */

#define PUSH_OUTREGION(outregion, region)\
{\
    region->next = *outregion;\
    *outregion = region;\
}

#define PHASE(region) (region->head->x < region->tail->x)

static void emit_region(outregion, region, pointer)
fa_region **outregion;
region_region *region;
region_point *pointer;
{
    struct fa_region *new = ALLOC(fa_region, 1);
    struct fa_region *ptr, *next;
    int new_phase;

    *new = region->region;
    new_phase = PHASE(new);
    
    /*
     * We can not determine if `region' is an interior region of a surrounding
     * region until the surrounding region is completed.  So it will be
     * temporarily attached to the interior_region list of the active
     * (uncompleted) region on its left to await further processing.
     *
     *  This further processing consists of a newly completed region comparing
     * its `phase' (whether the region has a positive or negative orientation)
     * with the phase of all regions that have been temporarily placed on its
     * interior_region list.  If the phases are opposite, then the region must
     * contain that sub-region. If the phases are the same, then the sub-region
     * is not an interior region of that region and is attached to next active
     * region on the left for processing when THAT region is completed.
     */

    if (pointer->last->region != NIL(region_region) &&
	    pointer->next->region != NIL(region_region)) {
	ptr = new->interior_regions;
	new->interior_regions = NIL(fa_region);
	while (ptr != NIL(fa_region)) {
	    next = ptr->next;
	    if (PHASE(ptr) == new_phase) {
		PUSH_INTERIOR(&pointer->last->region->region, ptr);
	    } else {
		PUSH_INTERIOR(new, ptr);
	    }
	    ptr = next;
	}
	PUSH_INTERIOR(&pointer->last->region->region, new);
    } else {
	/* 
         * if there isn't an active region on both sides, there is no chance
	 * that `region' will be an interior region, so `region' is immediately
	 * output after processing its interior_region list
	 */
	ptr = new->interior_regions;
	new->interior_regions = NIL(fa_region);
	while (ptr != NIL(fa_region)) {
	    next = ptr->next;
	    if (PHASE(ptr) == new_phase) {
		PUSH_OUTREGION(outregion, ptr);
	    } else {
		PUSH_INTERIOR(new, ptr);
	    }
	    ptr = next;
	}
	PUSH_OUTREGION(outregion, new);
    }
    FREE(region);
}

static void region_check_emit(outregion, pointer, vertex)
fa_region **outregion;
region_point *pointer;
fa_vertex *vertex;
{
    static region_region *carry_region = NIL(region_region);
    
    switch (vertex->type) {

    case LOWER_RIGHT_POS : /* see also LEFT_DIAGONAL */
	if (carry_region == NIL(region_region)) {
	    new_region(pointer->last, pointer, pointer->y);
	} else {
	    pointer->region = carry_region;
	    push_tail(carry_region, pointer, pointer->y);
	    carry_region = NIL(region_region);
	}
	break;
    case UPPER_RIGHT_NEG :
	if (carry_region == NIL(region_region)) {
	    pointer->last->region = pointer->region;
	    push_head(pointer->region, pointer, vertex->y);
	    push_head(pointer->region, pointer->last, vertex->y);
	} else {
	    push_head(pointer->region, pointer, vertex->y);
	    if (pointer->region == carry_region) {
		emit_region(outregion, carry_region, pointer);
	    } else {
		merge_region(carry_region, pointer->region);
	    }
	    carry_region = NIL(region_region);	
	}
	break;
    case UPPER_LEFT_NEG :
	push_tail(pointer->region, pointer, vertex->y);
	carry_region = pointer->region;
	break;
    case UPPER_LEFT_POS : /* see also LEFT_DIAGONAL */
	push_head(pointer->region, pointer, vertex->y);
	carry_region = pointer->region;
	break;
    case LOWER_RIGHT_NEG :
	if (carry_region == NIL(region_region)) {
	    new_region(pointer, pointer->last, pointer->last->y);
	} else {
	    pointer->region = carry_region;
	    push_head(pointer->region, pointer, pointer->y);
	    carry_region = NIL(region_region);
	}
	break;
    case RIGHT_DIAGONAL:
	pointer->y = vertex->y;
	/* FALL THROUGH */
    case UPPER_RIGHT_POS :
	if (carry_region == NIL(region_region)) {
	    push_tail(pointer->region, pointer, vertex->y);
	    push_tail(pointer->region, pointer->last, vertex->y);
	    pointer->last->region = pointer->region;
	} else {
	    push_tail(pointer->region, pointer, vertex->y);
	    if (pointer->region == carry_region) {
		emit_region(outregion, carry_region, pointer);
	    } else {
		merge_region(pointer->region, carry_region);
	    }
	    carry_region = NIL(region_region);
	}
	break;
    case LEFT_DIAGONAL :
	    /* a carefully ordered mix of LOWER_RIGHT_POS and UPPER_LEFT_POS */
	push_head(pointer->region, pointer, vertex->y);
	if (carry_region == NIL(region_region)) {
	    carry_region = pointer->region;
	    new_region(pointer->last, pointer, vertex->y);
	} else {
	    region_region *tmp;
	    
	    tmp = carry_region;
	    carry_region = pointer->region;
	    pointer->region = tmp;
	    
	    push_tail(pointer->region, pointer, vertex->y);
	}
	break;
      case NO_VERTEX:
      case LOWER_LEFT_NEG:
      case LOWER_LEFT_POS:
	break;
      default:
	break;
    }
}

