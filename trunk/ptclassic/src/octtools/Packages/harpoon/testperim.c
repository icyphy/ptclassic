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
#include "harpoon.h"

char *optProgName;
void usage
	ARGS((char *name));
void demo
	ARGS((octObject *facetPtr, int edgeIncr, octObject *outFacetPtr));
void buildFrame
	ARGS((octObject *layerPtr, int nPoints, struct octPoint *pointList));

int
main(argc, argv)
int argc;
char *argv[];
{
    octObject inFacet;
    octObject outFacet;

    optProgName = argv[0];
    if (argc != 3) usage(argv[0]);

    ohUnpackDefaults(&inFacet, "r", "test:test:contents");
    if (ohUnpackFacetName(&inFacet, argv[2]) != OCT_OK) usage(argv[0]);
    if (octOpenFacet(&inFacet) < OCT_OK) {
	octError("couldn't open input facet");
	exit(1);
    }

    outFacet = inFacet;
    outFacet.contents.facet.view = "single";
    outFacet.contents.facet.mode = "w";
    if (octOpenFacet(&outFacet) < OCT_OK) {
	octError("couldn't open output facet");
	exit(1);
    }
    demo(&inFacet, atoi(argv[1]), &outFacet);
    if (octCloseFacet(&outFacet) < OCT_OK) {
	octError("couldn't close output facet");
	exit(1);
    }

    return 0;
}

void usage(name)
char *name;
{
    (void) fprintf(stderr, "usage: %s minedge cell:view\n", name);
    exit(1);
}

void demo(facetPtr, edgeIncr, outFacetPtr)
octObject *facetPtr;		/* the facet whose frame you want */
int edgeIncr;			/* edge length increment in smoothed frame */
octObject *outFacetPtr;		/* the facet to put the frame into */
{
    harList polyList;		/* frame converted to a polygon list */
    int nPoints;		/* number of points in polygon */
    struct octPoint *pointList;	/* vector of points in polygon */
    int edgeLength;
    octObject layer;
    void doProps();

    doProps(facetPtr, outFacetPtr);

    layer.type = OCT_LAYER;
    layer.contents.layer.name = "MET1";
    if (octCreate(outFacetPtr, &layer) != OCT_OK) {
	octError("creating layer");
	exit(1);
    }

    for (edgeLength = 0; edgeLength <= 10 * edgeIncr; edgeLength += edgeIncr) {
	/* convert facet to list of polygons */
	harFacetToPerimList(facetPtr, HAR_INFINITY, edgeLength, &polyList);

	/* check that there is exactly one frame */
	if (polyList == HAR_EMPTY_LIST) {
	    (void) fprintf(stderr, "No geometry in facet\n");
	    exit(1);
	}
	if (harRestOfList(polyList) != HAR_EMPTY_LIST) {
	    (void) fprintf(stderr, "More than one geometry in facet's frame\n");
	    exit(1);
	}

	nPoints = harFirstPerimCount(polyList);
	pointList = harFirstPerimArray(polyList);

	(void) printf("Number of points in frame:  %d\n", nPoints);
	buildFrame(&layer, nPoints, pointList);

	harFreeList(&polyList);
    }
}

void doProps(inFacetPtr, outFacetPtr)
octObject *inFacetPtr;
octObject *outFacetPtr;
{
    octObject prop;

    prop.type = OCT_PROP;

    prop.contents.prop.name = "TECHNOLOGY";
    if (octGetByName(inFacetPtr, &prop) == OCT_OK) {
	if (octCreate(outFacetPtr, &prop) != OCT_OK) {
	    octError("creating TECHNOLOGY property");
	    exit(1);
	}
    }

    prop.contents.prop.name = "VIEWTYPE";
    if (octGetByName(inFacetPtr, &prop) == OCT_OK) {
	if (octCreate(outFacetPtr, &prop) != OCT_OK) {
	    octError("creating VIEWTYPE property");
	    exit(1);
	}
    }
}

void buildFrame(layerPtr, nPoints, pointList)
octObject *layerPtr;		/* the layer to put the frame into */
int nPoints;			/* number of points in polygon */
struct octPoint *pointList;	/* vector of points in polygon */
{
    octObject polygon;

    polygon.type = OCT_POLYGON;
    if (octCreate(layerPtr, &polygon) != OCT_OK) {
	octError("creating polygon");
	exit(1);
    }
    if (octPutPoints(&polygon, nPoints, pointList) != OCT_OK) {
	octError("putting points on polygon");
	exit(1);
    }
}
