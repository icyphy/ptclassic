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
#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "tr.h"

#include "oct_utils.h"

static int get_points();

/* play around with justifications in labels */
#define V_TO_H(x)	(x)
#define H_TO_V(x)	(x)
#define MIRROR_V(x)	(OCT_JUST_BOTTOM + OCT_JUST_TOP - (x))
#define MIRROR_H(x)	(OCT_JUST_LEFT + OCT_JUST_RIGHT - (x))

void
octTransformGeo(object, transform)
octObject *object;
struct octTransform *transform;
{
    switch (object->type) {
    case OCT_BOX : 
	{
	    struct octBox *box = &object->contents.box;
	    octCoord x,y;

	    if (!tr_oct_is_manhattan(transform)) {
	        oct_error("Cannot transform box: Transform is non-manhattan");
		errRaise(OCT_PKG_NAME, OCT_ILL_OP, octErrorString());
	    }
	    tr_oct_transform(transform, &box->lowerLeft.x, &box->lowerLeft.y);
	    tr_oct_transform(transform, &box->upperRight.x,
			     &box->upperRight.y);
	    x = MIN(box->lowerLeft.x, box->upperRight.x);
	    y = MIN(box->lowerLeft.y, box->upperRight.y);
	    box->upperRight.x = MAX(box->lowerLeft.x, box->upperRight.x);
	    box->upperRight.y = MAX(box->lowerLeft.y, box->upperRight.y);
	    box->lowerLeft.x = x;
	    box->lowerLeft.y = y;
	    break;
	}
    case OCT_CIRCLE :
	{
	    struct octCircle *circle = &object->contents.circle;
	    if (circle->startingAngle + 3600 != circle->endingAngle) {
		double rot_angle;
		double junk;
		int mirrored;

		tr_oct_get_angle(transform, &rot_angle, &mirrored, &junk);
		if (mirrored) {
		    int tmp = circle->startingAngle;
		    circle->startingAngle = -circle->endingAngle;
		    circle->endingAngle = -tmp;
		}
		circle->startingAngle += 10*rot_angle;
		circle->endingAngle += 10*rot_angle;
	    }
	    tr_oct_transform(transform, &circle->center.x, &circle->center.y);
	    tr_oct_scaler_transform(transform, &circle->innerRadius);
	    tr_oct_scaler_transform(transform, &circle->outerRadius);
	    break;
	}
    case OCT_LABEL :
	{
	    struct octLabel *label = &object->contents.label;
	    octCoord x,y;
	    double rot_angle, scale;
	    int mirrored, temp;

	    if (!tr_oct_is_manhattan(transform)) {
	        oct_error("Cannot transform label: Transform is non-manhattan");
		errRaise(OCT_PKG_NAME, OCT_ILL_OP, octErrorString());
	    }
	    tr_oct_transform(transform, &label->region.lowerLeft.x,
			     &label->region.lowerLeft.y);
	    tr_oct_transform(transform, &label->region.upperRight.x,
			     &label->region.upperRight.y);
	    x = MIN(label->region.lowerLeft.x, label->region.upperRight.x);
	    y = MIN(label->region.lowerLeft.y, label->region.upperRight.y);
	    label->region.upperRight.x =
		    MAX(label->region.lowerLeft.x, label->region.upperRight.x);
	    label->region.upperRight.y =
		    MAX(label->region.lowerLeft.y, label->region.upperRight.y);
	    label->region.lowerLeft.x = x;
	    label->region.lowerLeft.y = y;

	    tr_oct_get_angle(transform, &rot_angle, &mirrored, &scale);
	    label->textHeight *= scale;
	    if (mirrored) {
		label->vertJust = MIRROR_V(label->vertJust);
	    }
	    /* we already know rot_angle will be close to a multiple of 90 */
	    if (rot_angle > 225) {
		/* 270 degrees --  y = -x; x = y */
		temp = label->vertJust;
		label->vertJust = MIRROR_V(H_TO_V(label->horizJust));
		label->horizJust = V_TO_H(temp);
	    } else if (rot_angle > 135) {
		/* 180 degrees */
		label->vertJust = MIRROR_V(label->vertJust);
		label->horizJust = MIRROR_H(label->horizJust);
	    } else if (rot_angle > 45) {
		/* 90 degrees --  y = x; x = -y */
		temp = label->vertJust;
		label->vertJust = H_TO_V(label->horizJust);
		label->horizJust = MIRROR_H(V_TO_H(temp));
	    }
	    break;
	}
    case OCT_POINT :
	{
	    struct octPoint *point = &object->contents.point;
	    tr_oct_transform(transform, &point->x, &point->y);
	    break;
	}
    case OCT_EDGE :
	{
	    struct octEdge *edge = &object->contents.edge;
	    tr_oct_transform(transform, &edge->start.x, &edge->start.y);
	    tr_oct_transform(transform, &edge->end.x, &edge->end.y);
	    break;
	}
    case OCT_PATH :
	{
	    struct octPath *path = &object->contents.path;
	    tr_oct_scaler_transform(transform, &path->width);
	    break;
	}
    case OCT_INSTANCE:
	{
	    struct octInstance *instance = &object->contents.instance;
	    tr_stack *stack = tr_create();
	    
	    tr_add_transform(stack, transform, 0);
	    tr_add_transform(stack, &instance->transform, 0);
	    tr_get_oct_transform(stack, &instance->transform);
	    tr_free(stack);
	    break;
	}
    case OCT_NET:
    case OCT_LAYER:
    case OCT_TERM:
    case OCT_PROP:
    case OCT_BAG:
    case OCT_CHANGE_LIST:
    case OCT_CHANGE_RECORD:
    case OCT_POLYGON :
	break;
    default :
	oct_error("Unknown object type: %d", object->type);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    return;
}

octStatus
octTransformAndModifyGeo(object, transform)
struct octObject *object;
struct octTransform *transform;
{
    int retval, num_points, i;
    struct octPoint *points, *ptr;;

    octTransformGeo(object, transform);
    
    switch(object->type) {
    case OCT_PATH:
	if ((retval = octModify(object)) != OCT_OK) {
	    return retval;
	}
	/* FALL THROUGH */
    case OCT_POLYGON:
	if ((retval = get_points(object, &num_points, &points)) != OCT_OK) {
	    oct_prepend_error(
    "octTransformAndModifyObject: Getting the points on a path or polygon");
	    return retval;
	}
	for (ptr = points, i = 0; i < num_points; i++, ptr++) {
	    tr_oct_transform(transform, &ptr->x, &ptr->y);
	}
	return octPutPoints(object, num_points, points);
    case OCT_BOX:
    case OCT_LABEL:
    case OCT_POINT:
    case OCT_EDGE:
    case OCT_CIRCLE:
    case OCT_INSTANCE:
	return octModify(object);
    case OCT_NET:
    case OCT_LAYER:
    case OCT_TERM:
    case OCT_BAG:
    case OCT_CHANGE_LIST:
    case OCT_CHANGE_RECORD:
    case OCT_PROP:
	return OCT_OK; 
    default:
	oct_error("Unknown object type: %d", object->type);
	return OCT_ERROR;
    }
}


void
octTransformPoints(num_pts, pts, xform)
int num_pts;
struct octPoint *pts;
struct octTransform *xform;
{
    struct octPoint *ptr = pts;

    while (num_pts-- > 0) {
	tr_oct_transform(xform, &ptr->x, &ptr->y);
	ptr++;
    }
    return;
}

#define SCALE(val) ((val) *= scale)

void
octScaleGeo(object, scale)
struct octObject *object;
double scale;
{
    switch(object->type) {
    case OCT_PATH:
	SCALE(object->contents.path.width);
	break;
	/* FALL THROUGH */
    case OCT_POLYGON:
	break;
    case OCT_BOX:
	SCALE(object->contents.box.lowerLeft.x);
	SCALE(object->contents.box.lowerLeft.y);
	SCALE(object->contents.box.upperRight.x);
	SCALE(object->contents.box.upperRight.y);
	break;
    case OCT_LABEL:
	SCALE(object->contents.label.region.lowerLeft.x);
	SCALE(object->contents.label.region.lowerLeft.y);
	SCALE(object->contents.label.region.upperRight.x);
	SCALE(object->contents.label.region.upperRight.y);
	SCALE(object->contents.label.textHeight);
	break;
    case OCT_POINT:
	SCALE(object->contents.point.x);
	SCALE(object->contents.point.y);
	break;
    case OCT_EDGE:
	SCALE(object->contents.edge.start.x);
	SCALE(object->contents.edge.start.y);
	SCALE(object->contents.edge.end.x);
	SCALE(object->contents.edge.end.y);
	break;
    case OCT_CIRCLE:
	SCALE(object->contents.circle.center.x);
	SCALE(object->contents.circle.center.y);
	SCALE(object->contents.circle.innerRadius);
	SCALE(object->contents.circle.outerRadius);
	break;
    case OCT_INSTANCE: 
	SCALE(object->contents.instance.transform.translation.x);
	SCALE(object->contents.instance.transform.translation.y);
	if (object->contents.instance.transform.transformType == OCT_FULL_TRANSFORM) {
	    SCALE(object->contents.instance.transform.generalTransform[0][0]);
	    SCALE(object->contents.instance.transform.generalTransform[0][1]);
	    SCALE(object->contents.instance.transform.generalTransform[1][0]);
	    SCALE(object->contents.instance.transform.generalTransform[1][1]);
        } else {
	    struct octInstance *instance = &object->contents.instance;
	    tr_stack *stack = tr_create();

	    tr_add_transform(stack, &instance->transform, 0);
            tr_scale(stack, scale, 0);
	    tr_get_oct_transform(stack, &instance->transform);
	    tr_free(stack);
        }
	break;
    case OCT_NET:
    case OCT_LAYER:
    case OCT_TERM:
    case OCT_BAG:
    case OCT_CHANGE_LIST:
    case OCT_CHANGE_RECORD:
    case OCT_PROP:
	break;
    default:
	oct_error("Unknown object type: %d", object->type);
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }
    return;
}

octStatus
octScaleAndModifyGeo(object, scale)
struct octObject *object;
double scale;
{
    int retval, num_points, i;
    struct octPoint *points;

    octScaleGeo(object, scale);
    
    switch(object->type) {
    case OCT_PATH:
	if ((retval = octModify(object)) != OCT_OK) {
	    return retval;
	}
	/* FALL THROUGH */
    case OCT_POLYGON:
	if ((retval = get_points(object, &num_points, &points)) != OCT_OK) {
	    oct_prepend_error(
       	       "octScaleAndModifyObject: Getting the points on a path or polygon");
	    return retval;
	}
	for (i = 0; i < num_points; i++) {
	    SCALE(points[i].x);
	    SCALE(points[i].y);
	}
	return octPutPoints(object, num_points, points);
    case OCT_BOX:
    case OCT_LABEL:
    case OCT_EDGE:
    case OCT_POINT:
    case OCT_CIRCLE:
    case OCT_INSTANCE:
	return octModify(object);
    case OCT_NET:
    case OCT_LAYER:
    case OCT_TERM:
    case OCT_BAG:
    case OCT_CHANGE_LIST:
    case OCT_CHANGE_RECORD:
    case OCT_PROP:
	return OCT_OK; 
    default:
	oct_error("Unknown object type: %d", object->type);
	return OCT_ERROR;
    }
}

static int pt_arr_size = 0;
static struct octPoint *pt_arr;

static int get_points(obj, num, arr)
octObject *obj;
int *num;
struct octPoint **arr;
{
    int status;
    int32 num_points;

    if (pt_arr_size == 0) {
	pt_arr_size = 50;
	pt_arr = ALLOC(struct octPoint, pt_arr_size);
    }
    
    num_points = pt_arr_size;
    status = octGetPoints(obj, &num_points, pt_arr);
    if (status != OCT_OK) {
	if (status != OCT_TOO_SMALL) {
	    return status;
	} else {
	    pt_arr_size = num_points;
	    pt_arr = (struct octPoint *)
	      realloc((char *) pt_arr,
		      (unsigned) num_points*sizeof(struct octPoint));
	    if ((status = octGetPoints(obj, &num_points, pt_arr)) != OCT_OK) {
		return status;
	    }
	}
    }
    *num = num_points;
    *arr = pt_arr;
    
    return OCT_OK;
}
