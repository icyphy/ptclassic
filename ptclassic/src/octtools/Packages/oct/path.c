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
/*
 * Implementations of the the oct functions for paths.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "geo.h"
#include "io.h"

#include "bb.h"
#include "io_procs.h"
#include "oct_utils.h"

#include "path.h"

static octStatus path_read_fields(), path_write_fields(), path_bb();

extern struct object_desc oct_geo_points_desc;
static struct object_desc *super = &oct_geo_points_desc;

void
oct_path_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    
    object_desc->internal_size = sizeof(struct path);
    object_desc->external_size = sizeof(struct octPath);
    object_desc->flags |= HAS_BB_FIELD|HAS_BB_FUNC;
    object_desc->contain_mask =
      (octObjectMask) OCT_PROP_MASK|OCT_BAG_MASK|OCT_TERM_MASK|OCT_FORMAL_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct path, user_path);
    object_desc->bb_offset = FIELD_OFFSET(struct path, bbox);
    object_desc->bb_func = path_bb;
    object_desc->write_fields_func = path_write_fields;
    object_desc->read_fields_func = path_read_fields;
}
  

/*
 * add to the bounding box `box` the non-manhattan box with center line
 * (pt1, pt2) and width `width`
 */

static void
add_segment(box, pt1, pt2, width)
struct octBox *box;
struct octPoint *pt1, *pt2;
int width;
{
    octCoord dir_x, dir_y;
    octCoord low_x, low_y;
    octCoord high_x, high_y;
    octCoord x_delt, y_delt;
    extern double hypot();
    double length;

    dir_x = pt2->x - pt1->x;
    dir_y = pt2->y - pt1->y;
    dir_x = ABS(dir_x);
    dir_y = ABS(dir_y);
    
    length = hypot((double) dir_x, (double) dir_y);

    if (length == 0.0) {
	low_x = high_x = pt1->x;
	low_y = high_y = pt1->y;
    } else {
	x_delt = width*(dir_y/length)/2;
	y_delt = width*(dir_x/length)/2;

	low_x = MIN(pt1->x, pt2->x) - x_delt;
	low_y = MIN(pt1->y, pt2->y) - y_delt;
	high_x = MAX(pt1->x, pt2->x) + x_delt;
	high_y = MAX(pt1->y, pt2->y) + y_delt;
    }
    
    if (box->lowerLeft.x > low_x) box->lowerLeft.x = low_x;
    if (box->lowerLeft.y > low_y) box->lowerLeft.y = low_y;
    if (box->upperRight.x < high_x) box->upperRight.x = high_x;
    if (box->upperRight.y < high_y) box->upperRight.y = high_y;
}
    
static octStatus
path_bb(pptr, user_box)
generic *pptr;
struct octBox *user_box;
{
    int32 i;
    struct path *path = (struct path *) pptr;
    struct octBox *box = &path->bbox;
    struct octPoint *ptr, *points = path->points;
    int width = path->user_path.width;

    if (VALID_BB(&path->bbox)) {
	if (user_box != NIL(struct octBox)) {
	    *user_box = path->bbox;
	}
	return OCT_OK;
    }
	
    switch (path->num_points) {
    case 0:
	   /* create a bogus bounding box */
	oct_error("The path has no points, thus no bounding box");
	return OCT_NO_BB;
    case 1:
	box->lowerLeft.x = points->x - width/2;
	box->lowerLeft.y = points->y - width/2;
	box->upperRight.x = points->x + width/2;
	box->upperRight.y = points->y + width/2;
	break;
    case 2:
	box->lowerLeft = box->upperRight = *points;
	add_segment(box, points, points + 1, width);
	break;
    default:
	box->lowerLeft = box->upperRight = points[1];
	ptr = &points[2];

	for(i = path->num_points - 3; i > 0; i--, ptr++) {
	    if (box->lowerLeft.x > ptr->x) {
		box->lowerLeft.x = ptr->x;
	    } else if (box->upperRight.x < ptr->x) {
		box->upperRight.x = ptr->x;
	    }
	    if (box->lowerLeft.y > ptr->y) {
		box->lowerLeft.y = ptr->y;
	    } else if (box->upperRight.y < ptr->y) {
		box->upperRight.y = ptr->y;
	    }
	}
	box->lowerLeft.x -= path->user_path.width/2;
	box->lowerLeft.y -= path->user_path.width/2;
	box->upperRight.x += path->user_path.width/2;
	box->upperRight.y += path->user_path.width/2;
	
	add_segment(box, &points[0], &points[1], width);
	add_segment(box, &points[path->num_points-2],
		    &points[path->num_points-1], width);
		    
	break;
    }
    if (user_box != NIL(struct octBox)) {
	*user_box = *box;
    }
    return OCT_OK;
}

static octStatus
path_read_fields(pptr)
generic *pptr;
{
    struct path *path = (struct path *) pptr;
    int retval;

    retval = (*super->read_fields_func)(pptr) &&
      oct_get_32(&path->user_path.width);

    if (retval) {
	MAKE_BB_INVALID(&path->bbox); /* force the bb computation */
	(void) oct_bb_get(pptr, NIL(struct octBox));
    }
    return retval;
}

static octStatus
path_write_fields(pptr)
generic *pptr;
{
    struct path *path = (struct path *) pptr;
    
    return (*super->write_fields_func)(pptr) &&
      oct_put_32(path->user_path.width);
}

