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
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "fang.h"
#include "tr.h"
#include "harpoon_int.h"

/*LINTLIBRARY*/

static void convertRegionToPolygon();

void harFangToContents(fangGeoPtr, containerPtr, objType)
fa_geometry *fangGeoPtr;	/* pointer to fang geo struct */
octObject *containerPtr;	/* pointer to object to contain geos */
int objType;			/* oct object type of produced geos */
{
    fa_box *fangBoxes;		/* geo converted to boxes */
    fa_region *fangRegions;	/* frame converted to fang regions */

    HAR_START("harFangToContents");

    switch (objType) {
	case HAR_FILLED_BOX:
	    fa_to_regions(fangGeoPtr, &fangRegions);
	    fa_fill_regions1(fangRegions);
	    fa_regions_to_boxes(fangRegions, &fangBoxes);
	    fa_free_regions(fangRegions);
	    harBoxesToContents(fangBoxes, containerPtr);
	    fa_free_boxes(fangBoxes);
	    break;
	case HAR_UNFILLED_BOX:
	    fa_to_boxes(fangGeoPtr, &fangBoxes);
	    harBoxesToContents(fangBoxes, containerPtr);
	    fa_free_boxes(fangBoxes);
	    break;
	case HAR_FILLED_POLYGON:
	    fa_to_regions(fangGeoPtr, &fangRegions);
	    fa_fill_regions1(fangRegions);
	    harRegionsToContents(fangRegions, containerPtr);
	    fa_free_regions(fangRegions);
	    break;
	case HAR_UNFILLED_POLYGON:
	    fa_to_regions(fangGeoPtr, &fangRegions);
	    harRegionsToContents(fangRegions, containerPtr);
	    fa_free_regions(fangRegions);
	    break;
	default:
	    errRaise(HAR_PKG_NAME, 0, "illegal oct object type %d", objType);
    }
    HAR_END();
}

void harBoxesToContents(fangBoxPtr, containerPtr)
fa_box *fangBoxPtr;		/* pointer to fang box list */
octObject *containerPtr;	/* pointer to object to contain oct boxes */
{
    octObject box;		/* current box converted to oct */

    HAR_START("harBoxesToContents");

    while (fangBoxPtr != NIL(fa_box)) {
	OCT_ASSERT(ohCreateBox(containerPtr, &box,
		    (octCoord) fangBoxPtr->left, (octCoord) fangBoxPtr->bottom,
		    (octCoord) fangBoxPtr->right, (octCoord) fangBoxPtr->top));
	fangBoxPtr = fangBoxPtr->next;
    }

    HAR_END();
}

void harRegionsToContents(fangRegionPtr, containerPtr)
fa_region *fangRegionPtr;	/* pointer to fang region list */
octObject *containerPtr;	/* pointer to object to contain oct polygons */
{
    octObject octPoly;		/* the oct polygon from a region */
    fa_region *intRegionPtr;	/* pointer to interior region */
    void convertRegionToPolygon();

    HAR_START("harRegionsToContents");

    octPoly.type = OCT_POLYGON;

    while (fangRegionPtr != NIL(fa_region)) {
	OCT_ASSERT(octCreate(containerPtr, &octPoly));
	convertRegionToPolygon(fangRegionPtr, &octPoly);

	for (	intRegionPtr = fangRegionPtr->interior_regions;
		intRegionPtr != NIL(fa_region);
		intRegionPtr = intRegionPtr->next) {
	    harRegionsToContents(intRegionPtr->interior_regions, containerPtr);
	}

	fangRegionPtr = fangRegionPtr->next;
    }
    HAR_END();
}

#define ADD_POINT(p)	(pointList[pointIdx].x = (octCoord) (p)->x, \
			pointList[pointIdx++].y = (octCoord) (p)->y)

#define NEED(n)								\
    if (pointIdx + (n) > maxPoints) {					\
	maxPoints = pointIdx + (n);					\
	pointList = REALLOC(struct octPoint, pointList, maxPoints);	\
    }

static void convertRegionToPolygon(regionPtr, polygonPtr)
fa_region *regionPtr;		/* pointer to fang region to be converted */
octObject *polygonPtr;		/* pointer to oct polygon object */
{
    static int maxPoints = 0;	/* number of points we have storage for now */
    static struct octPoint *pointList = NIL(struct octPoint);
				/* storage for point list */
    fa_region *intRegionPtr;	/* pointer to interior region being traversed */
    fa_point *fangPointPtr;	/* pointer to a point in the fang region */
    int pointIdx;		/* index into pointList */

    pointIdx = 0;

    NEED(regionPtr->count);
    for (	fangPointPtr = regionPtr->head;
		fangPointPtr != NIL(fa_point);
		fangPointPtr = fangPointPtr->next) {
	ADD_POINT(fangPointPtr);
    }

    for (	intRegionPtr = regionPtr->interior_regions;
		intRegionPtr != NIL(fa_region);
		intRegionPtr = intRegionPtr->next) {
	NEED(intRegionPtr->count + 2);
	ADD_POINT(regionPtr->head);
	for (		fangPointPtr = intRegionPtr->head;
			fangPointPtr != NIL(fa_point);
			fangPointPtr = fangPointPtr->next) {
	    ADD_POINT(fangPointPtr);
	}
	ADD_POINT(intRegionPtr->head);
    }

    OCT_ASSERT(octPutPoints(polygonPtr, pointIdx, pointList));
}

void harFangListToFacet(geoListPtr, facetPtr, objType)
harList *geoListPtr;		/* pointer to per-layer list of aggragate geos */
octObject *facetPtr;		/* facet containing geos on layers */
int objType;			/* oct object type of produced geos */
{
    octObject octLayer;		/* current layer */

    HAR_START("harFangListToFacet");

    octLayer.type = OCT_LAYER;
    while (*geoListPtr != HAR_EMPTY_LIST) {
	octLayer.contents.layer.name = harFirstName(*geoListPtr);
	if (fa_count(harFirstGeo(*geoListPtr)) > 0) {
	    OCT_ASSERT(octGetOrCreate(facetPtr, &octLayer));
	    harFangToContents(harFirstGeo(*geoListPtr), &octLayer, objType);
	}
	geoListPtr = &harRestOfList(*geoListPtr);
    }
    HAR_END();
}

static fa_vertex_type vertTypeTable[][11] = {
		/*	OCT_NO_TRANSFORM	*/
    {	NO_VERTEX,		UPPER_RIGHT_NEG,	UPPER_LEFT_POS,
	LOWER_RIGHT_POS,	LOWER_LEFT_NEG,		UPPER_RIGHT_POS,
	LOWER_LEFT_POS,		UPPER_LEFT_NEG,		LOWER_RIGHT_NEG,
	LEFT_DIAGONAL,		RIGHT_DIAGONAL		},
		/*	OCT_MIRROR_X		*/
    {	NO_VERTEX,		UPPER_LEFT_NEG,		UPPER_RIGHT_POS,
	LOWER_LEFT_POS,		LOWER_RIGHT_NEG,	UPPER_LEFT_POS,
	LOWER_RIGHT_POS,	UPPER_RIGHT_NEG,	LOWER_LEFT_NEG,
	RIGHT_DIAGONAL,		LEFT_DIAGONAL		},
		/*	OCT_MIRROR_Y		*/
    {	NO_VERTEX,		LOWER_RIGHT_NEG,	LOWER_LEFT_POS,
	UPPER_RIGHT_POS,	UPPER_LEFT_NEG,		LOWER_RIGHT_POS,
	UPPER_LEFT_POS,		LOWER_LEFT_NEG,		UPPER_RIGHT_NEG,
	RIGHT_DIAGONAL,		LEFT_DIAGONAL		},
		/*	OCT_ROT90		*/
    {	NO_VERTEX,		UPPER_LEFT_NEG,		LOWER_LEFT_POS,
	UPPER_RIGHT_POS,	LOWER_RIGHT_NEG,	UPPER_LEFT_POS,
	LOWER_RIGHT_POS,	LOWER_LEFT_NEG,		UPPER_RIGHT_NEG,
	RIGHT_DIAGONAL,		LEFT_DIAGONAL		},
		/*	OCT_ROT180		*/
    {	NO_VERTEX,		LOWER_LEFT_NEG,		LOWER_RIGHT_POS,
	UPPER_LEFT_POS,		UPPER_RIGHT_NEG,	LOWER_LEFT_POS,
	UPPER_RIGHT_POS,	LOWER_RIGHT_NEG,	UPPER_LEFT_NEG,
	LEFT_DIAGONAL,		RIGHT_DIAGONAL		},
		/*	OCT_ROT270		*/
    {	NO_VERTEX,		LOWER_RIGHT_NEG,	UPPER_RIGHT_POS,
	LOWER_LEFT_POS,		UPPER_LEFT_NEG,		LOWER_RIGHT_POS,
	UPPER_LEFT_POS,		UPPER_RIGHT_NEG,	LOWER_LEFT_NEG,
	RIGHT_DIAGONAL,		LEFT_DIAGONAL		},
		/*	OCT_MX_ROT90		*/
    {	NO_VERTEX,		LOWER_LEFT_NEG,		UPPER_LEFT_POS,
	LOWER_RIGHT_POS,	UPPER_RIGHT_NEG,	LOWER_LEFT_POS,
	UPPER_RIGHT_POS,	UPPER_LEFT_NEG,		LOWER_RIGHT_NEG,
	LEFT_DIAGONAL,		RIGHT_DIAGONAL		},
		/*	OCT_MY_ROT90		*/
    {	NO_VERTEX,		UPPER_RIGHT_NEG,	LOWER_RIGHT_POS,
	UPPER_LEFT_POS,		LOWER_LEFT_NEG,		UPPER_RIGHT_POS,
	LOWER_LEFT_POS,		LOWER_RIGHT_NEG,	UPPER_LEFT_NEG,
	LEFT_DIAGONAL,		RIGHT_DIAGONAL		},
};

void harAddTransformedFang(inGeoPtr, transformPtr, outGeoPtr)
fa_geometry *inGeoPtr;
register struct octTransform *transformPtr;
fa_geometry *outGeoPtr;
{
    fa_vertex_type *vertTypePtr;
    register fa_vertex *fangVertPtr;
    struct octPoint coord;
    octTransformType trType;
    int mustTransformP = 0;

    HAR_START("harAddTransformedFang");

    if (transformPtr == HAR_NO_TRANSFORM) {
	fa_add_geo(outGeoPtr, inGeoPtr);
	HAR_END();
	return;
    }

    trType = transformPtr->transformType;
    switch (trType) {
	case OCT_NO_TRANSFORM:
	    mustTransformP = 0;
	    break;
	case OCT_MIRROR_X:
	case OCT_MIRROR_Y:
	case OCT_ROT90:
	case OCT_ROT180:
	case OCT_ROT270:
	case OCT_MX_ROT90:
	case OCT_MY_ROT90:
	    mustTransformP = 1;
	    break;
	case OCT_FULL_TRANSFORM:
	    {
		double angle, scale;
		int mirrorP;

		/* see if it's really manhattan, maybe with scaling */
		if (!tr_oct_is_manhattan(transformPtr)) {
		    errRaise(HAR_PKG_NAME, 0,
				"non-manhattan transform illegal in harpoon");
		}
		tr_oct_get_angle(transformPtr, &angle, &mirrorP, &scale);
		if (angle < -135 || angle > 135) {
		    trType = (mirrorP ? OCT_MIRROR_X : OCT_ROT180);
		} else if (angle < -45) {
		    trType = (mirrorP ? OCT_MX_ROT90 : OCT_ROT270);
		} else if (angle < 45) {
		    trType = (mirrorP ? OCT_MIRROR_Y : OCT_NO_TRANSFORM);
		} else {
		    trType = (mirrorP ? OCT_MY_ROT90 : OCT_ROT90);
		}
		mustTransformP = (trType != OCT_NO_TRANSFORM || scale != 1.0);
	    }
	    break;
	default:
	    errRaise(HAR_PKG_NAME, 0,
			    "unknown transform type: %d", (int) trType);
    }

    /* do the job */
    if (mustTransformP) {
	vertTypePtr = vertTypeTable[(int) trType];
	for (fangVertPtr = inGeoPtr->head; fangVertPtr != NIL(fa_vertex);
			    fangVertPtr = fangVertPtr->next) {
	    coord.x = (octCoord) fangVertPtr->x;
	    coord.y = (octCoord) fangVertPtr->y;
	    octTransformPoints(1, &coord, transformPtr);
	    fa_put_vertex(outGeoPtr,
			(fa_coord) coord.x, (fa_coord) coord.y,
			vertTypePtr[(int) fangVertPtr->type]);
	}
    } else {
	for (fangVertPtr = inGeoPtr->head; fangVertPtr != NIL(fa_vertex);
			    fangVertPtr = fangVertPtr->next) {
	    fa_put_vertex(outGeoPtr,
		    fangVertPtr->x + (fa_coord) transformPtr->translation.x,
		    fangVertPtr->y + (fa_coord) transformPtr->translation.y,
		    fangVertPtr->type);
	}
    }

    /* keep the fang geos reasonably compact */
    if (outGeoPtr->count >= outGeoPtr->merge_threshold) {
	fa_merge1(outGeoPtr);
    }
    HAR_END();
}
