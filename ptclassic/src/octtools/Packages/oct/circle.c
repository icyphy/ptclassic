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
 * Implementations of the the oct functions for circles.
 */

#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "geo.h"
#include "io.h"

#include "oct_utils.h"
#include "io_procs.h"

#include "circle.h"

static octStatus circle_read_fields(), circle_write_fields();
static octStatus circle_bb();

extern struct object_desc oct_geo_desc;
static struct object_desc *super = &oct_geo_desc;

void
oct_circle_desc_set(object_desc)
struct object_desc *object_desc;
{
    *object_desc = *super;
    object_desc->internal_size = sizeof(struct circle);
    object_desc->external_size = sizeof(struct octCircle);
    object_desc->flags |= HAS_BB_FUNC|HAS_BB_FIELD;
    object_desc->contain_mask = (octObjectMask) OCT_PROP_MASK|OCT_BAG_MASK;
    object_desc->user_offset = FIELD_OFFSET(struct circle, user_circle);
    object_desc->bb_offset = FIELD_OFFSET(struct circle, bbox);
    object_desc->bb_func = circle_bb;
    object_desc->write_fields_func = circle_write_fields;
    object_desc->read_fields_func = circle_read_fields;
}

#define BB_FIX(box)\
    if (box->lowerLeft.x > box->upperRight.x) {\
	int _tmp;\
	_tmp = box->lowerLeft.x;\
	box->lowerLeft.x = box->upperRight.x;\
	box->upperRight.x = _tmp;\
    }\
    if (box->lowerLeft.y > box->upperRight.y) {\
	int _tmp;\
	_tmp = box->lowerLeft.y;\
	box->lowerLeft.y = box->upperRight.y;\
	box->upperRight.y = _tmp;\
    }

#define BB_ADD(box, xv, yv)\
    if ((box)->lowerLeft.x > (xv)) {\
	(box)->lowerLeft.x = (xv);\
    } else if ((box)->upperRight.x < (xv)) {\
	(box)->upperRight.x = (xv);\
    }\
    if ((box)->lowerLeft.y > (yv)) {\
	(box)->lowerLeft.y = (yv);\
    } else if ((box)->upperRight.y < (yv)) {\
	(box)->upperRight.y = (yv);\
    }

/* a mod that works for a < 0 */

#define MOD(a,b) ((a) >= 0 ? (a)%(b) : ((-(a))%(b) == 0 ? 0 :(b) - (-(a))%(b)))

#define TO_RADS (3.14159265358979/1800)

static octStatus
circle_bb(cptr, box)
generic *cptr;
struct octBox *box;
{
    struct circle *circle = (struct circle *) cptr;
    struct octCircle *ucircle = &circle->user_circle;
    struct octBox *bb = &circle->bbox; 
    int start = ucircle->startingAngle;
    int end = ucircle->endingAngle;
    int diff = MOD(end-start, 3600);

    if (VALID_BB(&circle->bbox)) {
	if (box != NIL(struct octBox)) {
	    *box = circle->bbox;
	}
	return OCT_OK;
    }
	
    if (diff == 0) {
	if (start != end) {
	    /* full circle */
	    bb->lowerLeft.x = ucircle->center.x - ucircle->outerRadius;
	    bb->lowerLeft.y = ucircle->center.y - ucircle->outerRadius;
	    bb->upperRight.x = ucircle->center.x + ucircle->outerRadius;
	    bb->upperRight.y = ucircle->center.y + ucircle->outerRadius;
	} else {
	    /* zero width wedge */
	    double sin_s = sin(TO_RADS*start);
	    double cos_s = cos(TO_RADS*start);

	    bb->lowerLeft.x = ucircle->center.x +
	      ucircle->innerRadius*cos_s;
	    bb->lowerLeft.y = ucircle->center.y +
	      ucircle->innerRadius*sin_s;
	    bb->upperRight.x = ucircle->center.x +
	      ucircle->outerRadius*cos_s;
	    bb->upperRight.y = ucircle->center.y +
	      ucircle->outerRadius*sin_s;
	    BB_FIX(bb);
	}
    } else {
	/* The complicated pie-shape case */
	double sin_s = sin(TO_RADS*start),
	       cos_s = cos(TO_RADS*start),
	       sin_e = sin(TO_RADS*end),
	       cos_e = cos(TO_RADS*end);
	int32 x,y;

	    /*
	     * There are only eight possible points on an arc
	     * that could touch the minimum bounding box.
	     * The inner and outer radius at the starting and ending
	     * angle are always possible. Depending on the starting and
	     * ending angles, (center.x +- radius, center.y +- radius)
	     * are the other four.
	     */

	if (ucircle->innerRadius == 0) {
	    bb->lowerLeft = bb->upperRight = ucircle->center;
	} else {
	    bb->lowerLeft.x = bb->upperRight.x =
	      ucircle->center.x + cos_s*ucircle->innerRadius;
	    bb->lowerLeft.y = bb->upperRight.y =
	      ucircle->center.y + sin_s*ucircle->innerRadius;

	    x = ucircle->center.x + ucircle->innerRadius*cos_e;
	    y = ucircle->center.y + ucircle->innerRadius*sin_e;
	    BB_ADD(bb, x, y);
	}

	x = ucircle->center.x + ucircle->outerRadius*cos_s;
	y = ucircle->center.y + ucircle->outerRadius*sin_s;
	BB_ADD(bb, x, y);

	x = ucircle->center.x + ucircle->outerRadius*cos_e;
	y = ucircle->center.y + ucircle->outerRadius*sin_e;
	BB_ADD(bb, x, y);

	start = MOD(start, 3600);

	x = ucircle->center.x;
	y = ucircle->center.y;

	/*
	 * We simple check to see if the axis is closer to the start
	 * (measured in the proper direction) than the end. If so,
	 * that axis is inside the circle, and we add that point.
	 *
	 * since now 0 <= start < 3600, we can use this simple macro to
	 * compute the distance to the axis.
	 */

#define DIST(axis, start) (start > axis ? 3600 - (start - axis) : axis - start)

	if (diff > DIST(0, start)) {
	    BB_ADD(bb, x + ucircle->outerRadius, y);
	}
	if (diff > DIST(900, start)) {
	    BB_ADD(bb, x, y + ucircle->outerRadius);
	}
	if (diff > DIST(1800, start)) {
	    BB_ADD(bb, x - ucircle->outerRadius, y);
	}
	if (diff > DIST(2700, start)) {
	    BB_ADD(bb, x, y - ucircle->outerRadius);
	}
    }

    if (box != NIL(struct octBox)) {
	*box = circle->bbox;
    }
    return OCT_OK;
}

static octStatus 
circle_read_fields(ptr)
generic *ptr;
{
    struct circle *circle = (struct circle *) ptr;
    int val;

    val = (*super->read_fields_func)(ptr) &&
            oct_get_32(&circle->user_circle.startingAngle) &&
	    oct_get_32(&circle->user_circle.endingAngle) &&
	    oct_get_32(&circle->user_circle.innerRadius) &&
	    oct_get_32(&circle->user_circle.outerRadius) &&
	    oct_get_32(&circle->user_circle.center.x) &&
	    oct_get_32(&circle->user_circle.center.y);
    if (val) {
	MAKE_BB_INVALID(&circle->bbox);	/* force a bb calculation */
	if (circle_bb(ptr, NIL(struct octBox)) != OCT_OK) {
	    oct_prepend_error("bad return from circle_bb");
	    errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
	}
    }
    return val;
}

static octStatus
circle_write_fields(ptr)
generic *ptr;
{
    struct circle *circle = (struct circle *) ptr;
    
    return (*super->write_fields_func)(ptr) && 
            oct_put_32(circle->user_circle.startingAngle) &&
	    oct_put_32(circle->user_circle.endingAngle) &&
	    oct_put_32(circle->user_circle.innerRadius) &&
	    oct_put_32(circle->user_circle.outerRadius) &&
	    oct_put_32(circle->user_circle.center.x) &&
	    oct_put_32(circle->user_circle.center.y);
}
