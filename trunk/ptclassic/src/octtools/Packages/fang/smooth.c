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

/* This used to be in harpoon */

static int killPoints(point1, point2, point3, lMin)
    fa_point *point1, *point2, *point3;
    int lMin;
/* Returns 1 if point2 is killed. 0 otherwise */
{
    if (	(((point2->x - point1->x) * (point3->y - point2->y) -
		  (point2->y - point1->y) * (point3->x - point2->x)) <= 0) &&
	(	(abs(point1->x - point2->x) < lMin &&
		 abs(point1->y - point2->y) < lMin) ||
	 (abs(point3->x - point2->x) < lMin &&
	  abs(point3->y - point2->y) < lMin))) {
	point1->x = point1->x + point3->x - point2->x;
	point1->y = point1->y + point3->y - point2->y;
	return 1;		/* Point killed */
    } else {
	return 0;		/* No change */
    }
}


#define NEXT(p)	((p)->next == NIL(fa_point) ? regionPtr->head : (p)->next)

static void doSmooth(regionPtr, minEdgeLength)
    fa_region *regionPtr;		/* pointer to first region to be smoothed */
    int minEdgeLength;			/* minimum edge allowed in smoothed geo */
{
    fa_point *point1, *point2, *point3;

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


void fa_smooth(inGeoPtr, minEdgeLength, outGeoPtr)
    fa_geometry *inGeoPtr;		/* input (unsmoothed) geometry */
    int minEdgeLength;			/* minimum edge allowed in smoothed geo */
    fa_geometry *outGeoPtr;		/* output (smoothed) geometry */
{
    fa_region *regions;

    fa_to_regions(inGeoPtr, &regions);
    doSmooth(regions, minEdgeLength);
    fa_init(outGeoPtr);
    fa_add_regions(outGeoPtr, regions);
}

