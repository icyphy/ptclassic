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

static void addEltToPerimList();
static void convertRegionToPerimList();

/*LINTLIBRARY*/

void harFangToPerimList(fangGeoPtr, perimListPtr, objType)
fa_geometry *fangGeoPtr;	/* pointer to fang geo struct */
harList *perimListPtr;		/* harList to be filled in */
int objType;			/* oct object type of produced geos */
{
    fa_box *fangBoxes;		/* geo converted to boxes */
    fa_region *fangRegions;	/* frame converted to fang regions */

    HAR_START("harFangToPerimList");

    switch (objType) {
	case HAR_FILLED_BOX:
	    fa_to_regions(fangGeoPtr, &fangRegions);
	    fa_fill_regions1(fangRegions);
	    fa_regions_to_boxes(fangRegions, &fangBoxes);
	    fa_free_regions(fangRegions);
	    harBoxesToPerimList(fangBoxes, perimListPtr);
	    fa_free_boxes(fangBoxes);
	    break;
	case HAR_UNFILLED_BOX:
	    fa_to_boxes(fangGeoPtr, &fangBoxes);
	    harBoxesToPerimList(fangBoxes, perimListPtr);
	    fa_free_boxes(fangBoxes);
	    break;
	case HAR_FILLED_POLYGON:
	    fa_to_regions(fangGeoPtr, &fangRegions);
	    fa_fill_regions1(fangRegions);
	    harRegionsToPerimList(fangRegions, perimListPtr);
	    fa_free_regions(fangRegions);
	    break;
	case HAR_UNFILLED_POLYGON:
	    fa_to_regions(fangGeoPtr, &fangRegions);
	    harRegionsToPerimList(fangRegions, perimListPtr);
	    fa_free_regions(fangRegions);
	    break;
	default:
	    errRaise(HAR_PKG_NAME, 0, "illegal oct object type %d", objType);
    }
    HAR_END();
}

void harBoxesToPerimList(fangBoxPtr, perimListPtr)
fa_box *fangBoxPtr;		/* pointer to fang box list */
harList *perimListPtr;		/* harList to be filled in */
{
    void addEltToPerimList();

    HAR_START("harBoxesToPerimList");

    *perimListPtr = HAR_EMPTY_LIST;

    while (fangBoxPtr != NIL(fa_box)) {
	addEltToPerimList(2, perimListPtr);
	harFirstPerimArray(*perimListPtr)[0].x = (octCoord) fangBoxPtr->left;
	harFirstPerimArray(*perimListPtr)[0].y = (octCoord) fangBoxPtr->bottom;
	harFirstPerimArray(*perimListPtr)[1].x = (octCoord) fangBoxPtr->right;
	harFirstPerimArray(*perimListPtr)[1].y = (octCoord) fangBoxPtr->top;

	fangBoxPtr = fangBoxPtr->next;
    }
    HAR_END();
}

void harRegionsToPerimList(fangRegionPtr, perimListPtr)
fa_region *fangRegionPtr;	/* pointer to fang region list */
harList *perimListPtr;		/* harList to be filled in */
{
    fa_region *intRegionPtr;	/* pointer to interior region */
    void addEltToPerimList(), convertRegionToPerimList();

    HAR_START("harRegionsToPerimList");

    *perimListPtr = HAR_EMPTY_LIST;

    while (fangRegionPtr != NIL(fa_region)) {
	addEltToPerimList(0, perimListPtr);
	convertRegionToPerimList(fangRegionPtr, *perimListPtr);

	for (	intRegionPtr = fangRegionPtr->interior_regions;
		intRegionPtr != NIL(fa_region);
		intRegionPtr = intRegionPtr->next) {
	    harRegionsToPerimList(intRegionPtr->interior_regions, perimListPtr);
	}

	fangRegionPtr = fangRegionPtr->next;
    }
    HAR_END();
}

static void addEltToPerimList(nVertices, perimListPtr)
int nVertices;			/* number of vertices in the new element */
harList *perimListPtr;		/* harList to be filled in */
{
    harList newList;

    newList = ALLOC(struct harListItem, 1);
    harRestOfList(newList) = *perimListPtr;
    *perimListPtr = newList;
    newList->type = HAR_POINT_GEO;
    harFirstPerimCount(newList) = nVertices;
    if (nVertices == 0) {
	harFirstPerimArray(newList) = NIL(struct octPoint);
    } else {
	harFirstPerimArray(newList) = ALLOC(struct octPoint, nVertices);
    }
}

#define ADD_POINT(p)							\
	(harFirstPerimArray(list)[harFirstPerimCount(list)].x =		\
						    (octCoord) (p)->x,	\
	harFirstPerimArray(list)[harFirstPerimCount(list)++].y =	\
						    (octCoord) (p)->y)

#define NEED(n)		harFirstPerimArray(list) =			\
			    REALLOC(struct octPoint,			\
				    harFirstPerimArray(list),		\
				    harFirstPerimCount(list) + (n))

static void convertRegionToPerimList(regionPtr, list)
fa_region *regionPtr;		/* pointer to fang region to be converted */
harList list;		/* harList to be filled in */
{
    fa_region *intRegionPtr;	/* pointer to interior region being traversed */
    fa_point *fangPointPtr;	/* pointer to a point in the fang region */

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
}

void harFacetToPerimList(facetPtr, frameGrowth, minEdgeLength, perimListPtr)
octObject *facetPtr;			/* facet containing geometry */
octCoord frameGrowth;			/* distance to grow/merge/shrink */
octCoord minEdgeLength;			/* min. edge with concave endpoint */
harList *perimListPtr;			/* harList to be filled in */
{
    struct octBox boundingBox;		/* facet's bounding box */
    octCoord maxDim;			/* larger of facet's height, width */
    octGenerator layerGen;		/* layer generator */
    octObject layer;			/* layer being added */
    int genStatus;			/* status returned by generator */
    fa_geometry geo1, geo2;		/* temporary geos */
    void fa_smooth();

    HAR_START("harFacetToPerimList");

    /* interpret infinities */
    OCT_ASSERT(octBB(facetPtr, &boundingBox));
    maxDim = MAX(boundingBox.upperRight.x - boundingBox.lowerLeft.x,
		    boundingBox.upperRight.y - boundingBox.lowerLeft.y);
    if (frameGrowth == HAR_INFINITY) frameGrowth = maxDim;
    if (minEdgeLength == HAR_INFINITY) minEdgeLength = maxDim;

    /* collect the geometry */
    fa_init(&geo1);
    OCT_ASSERT(octInitGenContents(facetPtr, OCT_LAYER_MASK, &layerGen));
    while ((genStatus = octGenerate(&layerGen, &layer)) == OCT_OK) {
	harAddContentsToFang(&layer, &geo1, HAR_NO_TRANSFORM);
    }
    OCT_ASSERT(genStatus);

    /* merge and simplify the geometry */
    fa_frame(&geo1, (int) frameGrowth, frameGrowth, &geo2);
    fa_free_geo(&geo1);
    fa_smooth(&geo2, (int) minEdgeLength, &geo1);
    fa_free_geo(&geo2);
    harFangToPerimList(&geo1, perimListPtr, HAR_FILLED_POLYGON);
    fa_free_geo(&geo1);

    HAR_END();
}

#ifdef MOVED_TO_FANG
/* WRONG!!! This belongs in fang (after cleanup) XXX */
static void fa_smooth(inGeoPtr, minEdgeLength, outGeoPtr)
fa_geometry *inGeoPtr;		/* input (unsmoothed) geometry */
int minEdgeLength;		/* minimum edge allowed in smoothed geo */
fa_geometry *outGeoPtr;		/* output (smoothed) geometry */
{
    fa_region *regions;
    void doSmooth();

    fa_to_regions(inGeoPtr, &regions);
    doSmooth(regions, minEdgeLength);
    fa_init(outGeoPtr);
    fa_add_regions(outGeoPtr, regions);
}

#define NEXT(p)	((p)->next == NIL(fa_point) ? regionPtr->head : (p)->next)

static void doSmooth(regionPtr, minEdgeLength)
fa_region *regionPtr;		/* pointer to first region to be smoothed */
int minEdgeLength;		/* minimum edge allowed in smoothed geo */
{
    fa_point *point1, *point2, *point3;
    int killPoints();

    while (regionPtr != NIL(fa_region)) {
	for (point1 = regionPtr->head; point1 != NIL(fa_point); ) {
	    if (regionPtr->count < 3) break;
	    point2 = NEXT(point1);
	    point3 = NEXT(point2);
	    if (killPoints(point1, point2, point3, minEdgeLength)) {
		if (point1->next == point2 && point2->next == point3) {
		    point1->next = point3->next;
		} else {
		    point1->next = NIL(fa_point);
		    regionPtr->head = point3->next;
		}
		FREE(point2);
		FREE(point3);
		regionPtr->count -= 2;
	    } else {
		point1 = point1->next;
	    }
	}

	doSmooth(regionPtr->interior_regions, minEdgeLength);

	regionPtr = regionPtr->next;
    }
}

static int killPoints(point1, point2, point3, lMin)
fa_point *point1, *point2, *point3;
int lMin;
{
    if (	(((point2->x - point1->x) * (point3->y - point2->y) -
		    (point2->y - point1->y) * (point3->x - point2->x)) <= 0) &&
		(	(abs(point1->x - point2->x) < lMin &&
				abs(point1->y - point2->y) < lMin) ||
			(abs(point3->x - point2->x) < lMin &&
				abs(point3->y - point2->y) < lMin))) {
	point1->x = point1->x + point3->x - point2->x;
	point1->y = point1->y + point3->y - point2->y;
	return(1);
    } else {
	return(0);
    }
}

#endif






