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
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "fang.h"
#include "harpoon_int.h"

static void addPolygonToFang();

/*LINTLIBRARY*/

void harAddObjectToFang(octObjPtr, fangGeoPtr, transformPtr)
octObject *octObjPtr;			/* pointer to oct geo to be added */
fa_geometry *fangGeoPtr;		/* pointer to fang geo struct */
struct octTransform *transformPtr;	/* transform for geo before adding */
{
    struct octPoint *ptList;	/* list of points for polygon or path */
    int32 npoints;		/* number of points in above list */
    int i;			/* loop index */
    octCoord hWidth;		/* half-width of path */
    octObject boundBoxObj;	/* holds result of octBB() for strange geos */
    void addPolygonToFang();
    
    HAR_START("harAddObjectToFang");

    switch (octObjPtr->type) {
	case OCT_PATH:
	    ohGetPoints(octObjPtr, &npoints, &ptList);
	    if (transformPtr != HAR_NO_TRANSFORM) {
		octTransformPoints(npoints, ptList, transformPtr);
	    }
	    hWidth = (octObjPtr->contents.path.width+1) / 2;
	    for (i = 1; i < npoints; i++) {
		if ( (i == 1 || i == npoints-1) &&
			(ptList[i-1].x == ptList[i].x ||
			    ptList[i-1].y == ptList[i].y) ) {
		    /* put in trimmed segment */
		    if (ptList[i-1].x == ptList[i].x) {
			/* vertical segment */
			fa_add_box(fangGeoPtr,
				(fa_coord) (ptList[i].x - hWidth),
				(fa_coord) MIN(ptList[i-1].y, ptList[i].y),
				(fa_coord) (ptList[i].x + hWidth),
				(fa_coord) MAX(ptList[i-1].y, ptList[i].y));
		    } else {
			/* horizontal segment */
			fa_add_box(fangGeoPtr,
				(fa_coord) MIN(ptList[i-1].x, ptList[i].x),
				(fa_coord) (ptList[i].y - hWidth),
				(fa_coord) MAX(ptList[i-1].x, ptList[i].x),
				(fa_coord) (ptList[i].y + hWidth));
		    }
		    /* put in box around interior joint (if any) */
		    if (i != 1) {
			fa_add_box(fangGeoPtr,
				(fa_coord) (ptList[i-1].x - hWidth),
				(fa_coord) (ptList[i-1].y - hWidth),
				(fa_coord) (ptList[i-1].x + hWidth),
				(fa_coord) (ptList[i-1].y + hWidth));
		    } else if (i != npoints-1) {
			fa_add_box(fangGeoPtr,
				(fa_coord) (ptList[i].x - hWidth),
				(fa_coord) (ptList[i].y - hWidth),
				(fa_coord) (ptList[i].x + hWidth),
				(fa_coord) (ptList[i].y + hWidth));
		    }
		} else {
		    fa_add_box(fangGeoPtr,
			(fa_coord) (MIN(ptList[i-1].x, ptList[i].x) - hWidth),
			(fa_coord) (MIN(ptList[i-1].y, ptList[i].y) - hWidth),
			(fa_coord) (MAX(ptList[i-1].x, ptList[i].x) + hWidth),
			(fa_coord) (MAX(ptList[i-1].y, ptList[i].y) + hWidth));
		}
	    }
	    break;
	case OCT_POLYGON:
	    ohGetPoints(octObjPtr, &npoints, &ptList);
	    if (transformPtr != HAR_NO_TRANSFORM) {
		octTransformPoints(npoints, ptList, transformPtr);
	    }
	    addPolygonToFang(fangGeoPtr, npoints, ptList);
	    break;
	case OCT_TERM:
	case OCT_NET:
	case OCT_PROP:
	case OCT_BAG:
	case OCT_LAYER:
	    /* no geometry -- ignore */
	    break;
	default:
	    boundBoxObj.type = OCT_BOX;
	    OCT_ASSERT(octBB(octObjPtr, &boundBoxObj.contents.box));
	    if (transformPtr != HAR_NO_TRANSFORM) {
		octTransformGeo(&boundBoxObj, transformPtr);
	    }
	    fa_add_box(fangGeoPtr,
		    (fa_coord) boundBoxObj.contents.box.lowerLeft.x,
		    (fa_coord) boundBoxObj.contents.box.lowerLeft.y,
		    (fa_coord) boundBoxObj.contents.box.upperRight.x,
		    (fa_coord) boundBoxObj.contents.box.upperRight.y);
    }
    HAR_END();
}

void harAddContentsToFang(containerPtr, fangGeoPtr, transformPtr)
octObject *containerPtr;		/* pointer to container of geometry */
fa_geometry *fangGeoPtr;		/* pointer to fang geo struct */
struct octTransform *transformPtr;	/* transform for geos before adding */
{
    octGenerator geoGen;	/* geometry generator */
    int genStatus;		/* status returned by generator */
    octObject octGeo;		/* generated oct geometric object */

    HAR_START("harAddContentsToFang");

    /* Generate through the geometries, adding each to fangGeo */
    OCT_ASSERT(octInitGenContents(containerPtr, OCT_GEO_MASK, &geoGen));
    while ((genStatus = octGenerate(&geoGen, &octGeo)) == OCT_OK) {
	harAddObjectToFang(&octGeo, fangGeoPtr, transformPtr);
    }
    OCT_ASSERT(genStatus);
    HAR_END();
}

void harAddFacetToFangList(facetPtr, geoListPtr, transformPtr)
octObject *facetPtr;			/* facet containing geos on layers */
harList *geoListPtr;			/* per-layer list of aggragate geos */
struct octTransform *transformPtr;	/* transform to apply to geos */
{
    octGenerator layerGen;	/* generator for running through the layers */
    octObject layer;		/* layer currently being abstracted */
    int genStatus;		/* return status from generator */
    fa_geometry fangGeo;	/* fang geo struct for a layer */

    HAR_START("harAddFacetToFangList");

    /* Generate through the layers, collecting the geos on each */
    OCT_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &layerGen));
    while ((genStatus = octGenerate(&layerGen, &layer)) == OCT_OK) {
	fa_init(&fangGeo);
	harAddContentsToFang(&layer, &fangGeo, transformPtr);
	if (fa_count(&fangGeo) > 0) {
	    fa_add_geo(harGetNamedGeo(layer.contents.layer.name, geoListPtr),
		       &fangGeo);
	}
	fa_free_geo(&fangGeo);
    }
    OCT_ASSERT(genStatus);
    HAR_END();
}

struct poly_vertex {
    struct poly_vertex *next, *prev;
    fa_point vertPoint;
};

#define IS_DUMB(p)	(( (p)->prev->vertPoint.x == (p)->vertPoint.x &&     \
			   (p)->next->vertPoint.x == (p)->vertPoint.x )   || \
			 ( (p)->prev->vertPoint.y == (p)->vertPoint.y &&     \
			   (p)->next->vertPoint.y == (p)->vertPoint.y ))

static void addPolygonToFang(geo, npoints, oct_points)
fa_geometry *geo;
int npoints;
struct octPoint *oct_points;
{
    struct poly_vertex *fa_points;	/* array of points (also linked) */
    struct poly_vertex *top_point;	/* point with highest Y coordinate */
    struct poly_vertex *current_point;	/* point being considered */
    fa_coord prevX;			/* coordinates of previous vertex */
    fa_coord curX, curY;		/* coordinates of current vertex */
    fa_coord nextX, nextY;		/* coordinates of next vertex */
    int i;				/* loop index */
    fa_region region;			/* fang region created from polygon */

    /* Copy points into doubly-linked list, and expand non-Manhattan edges */
    current_point = fa_points = ALLOC(struct poly_vertex, npoints+1);
    curX = prevX = current_point->vertPoint.x = oct_points[npoints-1].x;
    curY = current_point->vertPoint.y = oct_points[npoints-1].y;
    current_point->next = current_point + 1;
    current_point->prev = current_point - 1;
    for (i = 0; i < npoints; i++) {
	current_point++;
	nextX = (fa_coord) oct_points->x;
	nextY = (fa_coord) oct_points->y;
	if (nextX != curX && nextY != curY) {
	    /* non-Manhattan edge */
	    fa_add_box(geo, MIN(curX,nextX), MIN(curY,nextY),
			    MAX(curX,nextX), MAX(curY,nextY));
	    if (curX == prevX) {
		curY = nextY;
	    } else {
		curX = nextX;
	    }
	}
	current_point->vertPoint.x = curX;
	current_point->vertPoint.y = curY;
	current_point->next = current_point + 1;
	current_point->prev = current_point - 1;
	oct_points++;
	prevX = curX;
	curX = nextX;
	curY = nextY;
    }
    fa_points[0].prev = &fa_points[npoints];
    fa_points[npoints].next = &fa_points[0];
    npoints++;

    /* Delete silly points */
    i = 0;
    while (i < npoints) {
	if (IS_DUMB(current_point)) {
	    current_point->prev->next = current_point->next;
	    current_point->next->prev = current_point->prev;
	    npoints--;
	    if (i > 0) i--;
	    current_point = current_point->prev;
	} else {
	    i++;
	    current_point = current_point->next;
	}
    }

    if (npoints > 0) {

	/* Find topmost point */
	top_point = current_point;
	for (i = 0; i < npoints; i++) {
	    if (current_point->vertPoint.y > top_point->vertPoint.y) {
		top_point = current_point;
	    }
	    current_point = current_point->next;
	}

	/* Determine direction, and put in fa_region links */
	if (top_point->vertPoint.x < top_point->prev->vertPoint.x ||
		    top_point->vertPoint.x > top_point->next->vertPoint.x) {
	    /* direction is correct */
	    for (current_point = top_point; current_point->next != top_point;
					current_point = current_point->next) {
		current_point->vertPoint.next = &current_point->next->vertPoint;
	    }
	    current_point->vertPoint.next = NIL(fa_point);
	} else {
	    /* reverse direction */
	    for (current_point = top_point; current_point->prev != top_point;
					current_point = current_point->prev) {
		current_point->vertPoint.next = &current_point->prev->vertPoint;
	    }
	    current_point->vertPoint.next = NIL(fa_point);
	}

	/* add the region */
	region.count = npoints;
	region.head = &top_point->vertPoint;
	region.tail = &current_point->vertPoint;
	region.interior_regions = NIL(fa_region);
	region.next = NIL(fa_region);
	fa_add_regions(geo, &region);

    }
    free((char *) fa_points);
}
