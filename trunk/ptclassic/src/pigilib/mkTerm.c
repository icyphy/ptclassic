/* mkTerm.c  edg
Version identification:
$Id$
Functions that create terminals and thier symbols in a icon facet.
Functions here use err.c for error handling.
*/

/*
Copyright (c) 1989 The Regents of the University of California.
			All Rights Reserved.
*/

/* Includes */
#include <stdio.h>
#include "local.h"
#include "rpc.h"
#include "err.h"
#include "octMacros.h"
#include "oh.h"

#define SHAPE_MAX 100  /* max number of points in a Shape */

/* Position of terminals around icon box.  This is exported. */
enum Position_e {
    l3u = 0, l2u, l1u, l0, l1d, l2d, l3d,
    r3u, r2u, r1u, r0, r1d, r2d, r3d
};
#define POSITION_MAX 14  /* max number of positions */

struct Shape_s {
    int type;  /* OCT_PATH, OCT_POLYGON, OCT_BOX */
    struct octPoint *points;
    int points_n;
};
typedef struct Shape_s Shape;

/* define paths to each possible terminal position */
struct octPoint l3uPoints[] = {
    {-75, 75}, {-65, 75}, {-50, 45}
};
struct octPoint l2uPoints[] = {
    {-75, 50}, {-65, 50}, {-50, 35}
};
struct octPoint l1uPoints[] = {
    {-75, 25}, {-50, 25}
};
struct octPoint l0Points[] = {
    {-75, 0}, {-50, 0}
};
struct octPoint l1dPoints[] = {
    {-75, -25}, {-50, -25}
};
struct octPoint l2dPoints[] = {
    {-75, -50}, {-65, -50}, {-50, -35}
};
struct octPoint l3dPoints[] = {
    {-75, -75}, {-65, -75}, {-50, -45}
};
struct octPoint r3uPoints[] = {
    {75, 75}, {65, 75}, {50, 45}
};
struct octPoint r2uPoints[] = {
    {75, 50}, {65, 50}, {50, 35}
};
struct octPoint r1uPoints[] = {
    {75, 25}, {50, 25}
};
struct octPoint r0Points[] = {
    {75, 0}, {50, 0}
};
struct octPoint r1dPoints[] = {
    {75, -25}, {50, -25}
};
struct octPoint r2dPoints[] = {
    {75, -50}, {65, -50}, {50, -35}
};
struct octPoint r3dPoints[] = {
    {75, -75}, {65, -75}, {50, -45}
};

Shape pathShapes[] = {
    { OCT_PATH, l3uPoints, sizeof(l3uPoints)/sizeof(struct octPoint) },
    { OCT_PATH, l2uPoints, sizeof(l2uPoints)/sizeof(struct octPoint) },
    { OCT_PATH, l1uPoints, sizeof(l1uPoints)/sizeof(struct octPoint) },
    { OCT_PATH, l0Points, sizeof(l0Points)/sizeof(struct octPoint) },
    { OCT_PATH, l1dPoints, sizeof(l1dPoints)/sizeof(struct octPoint) },
    { OCT_PATH, l2dPoints, sizeof(l2dPoints)/sizeof(struct octPoint) },
    { OCT_PATH, l3dPoints, sizeof(l3dPoints)/sizeof(struct octPoint) },
    { OCT_PATH, r3uPoints, sizeof(r3uPoints)/sizeof(struct octPoint) },
    { OCT_PATH, r2uPoints, sizeof(r2uPoints)/sizeof(struct octPoint) },
    { OCT_PATH, r1uPoints, sizeof(r1uPoints)/sizeof(struct octPoint) },
    { OCT_PATH, r0Points, sizeof(r0Points)/sizeof(struct octPoint) },
    { OCT_PATH, r1dPoints, sizeof(r1dPoints)/sizeof(struct octPoint) },
    { OCT_PATH, r2dPoints, sizeof(r2dPoints)/sizeof(struct octPoint) },
    { OCT_PATH, r3dPoints, sizeof(r3dPoints)/sizeof(struct octPoint) }
};

/* arrowShape is used to mark terminal as input/output and multiple/single */
struct octPoint arrowPoints[] = {
    {0, 0}, {-5, 5}, {5, 0}, {-5, -5}
};

Shape arrowShape = {
    OCT_POLYGON, arrowPoints, sizeof(arrowPoints)/sizeof(struct octPoint)
};
/* end of arrowShape */

/* boxShape defines terminal implementation */
struct octPoint boxPoints[] = {
    {-1, -1},
    {1, 1}
};
Shape boxShape = {
    OCT_BOX, boxPoints, 2
};
/* end of boxShape */

/* boxTranslates maps a Position_e to coords for boxShape */
struct octPoint boxTranslates[] = {
    {-75, 75}, {-75, 50}, {-75, 25}, {-75, 0}, {-75, -25}, {-75, -50},
    {-75, -75},
    {75, 75}, {75, 50}, {75, 25}, {75, 0}, {75, -25}, {75, -50},
    {75, -75}
};


static octObject *facetPtr, wiringLayer, blueOutlineLayer, blueSolidLayer;

/* MkTermInit 7/28/89 8/6/88 8/27/88
Call this first.
*/
boolean
MkTermInit(CurrentFacetPtr)
octObject *CurrentFacetPtr;
{
    facetPtr = CurrentFacetPtr;
    CK_OCT(GetOrCreateLayer(facetPtr, &blueOutlineLayer, "blueOutline"));
    CK_OCT(GetOrCreateLayer(facetPtr, &blueSolidLayer, "blueSolid"));
    CK_OCT(GetOrCreateLayer(facetPtr, &wiringLayer, "WIRING"));
    return(TRUE);
}

/*
Caveats: Assumes that Shape has <= SHAPE_MAX points
*/
static boolean
PutShape(containPtr, objPtr, shapePtr, translatePtr)
octObject *containPtr, *objPtr;
struct octPoint *translatePtr;
Shape *shapePtr;
{
    struct octPoint buf[SHAPE_MAX], *dest, *src;
    octCoord tx, ty;

    tx = translatePtr->x;
    ty = translatePtr->y;
    objPtr->type = shapePtr->type;

    switch (shapePtr->type) {
    case OCT_PATH:
	objPtr->contents.path.width = (octCoord) 0;
    case OCT_POLYGON:
	CK_OCT(octCreate(containPtr, objPtr));
	src = shapePtr->points;
	dest = buf;
	while (dest < (buf + shapePtr->points_n)) {
	    dest->x = src->x + tx;
	    dest->y = src->y + ty;
	    dest++;
	    src++;
	}
	(void) octPutPoints(objPtr, shapePtr->points_n, buf);
	break;
    case OCT_BOX:
	objPtr->contents.box.lowerLeft.x = shapePtr->points[0].x + tx;
	objPtr->contents.box.lowerLeft.y = shapePtr->points[0].y + ty;
	objPtr->contents.box.upperRight.x = shapePtr->points[1].x + tx;
	objPtr->contents.box.upperRight.y = shapePtr->points[1].y + ty;
	(void) octCreate(containPtr, objPtr);
	break;
    default:
	return(FALSE);
    }
    return(TRUE);
}

/* 8/24/89 8/6/88
Finds a terminal and makes its implementation and other related geometry.
Caveats: Assumes that inputs are always on the left and outputs are
    on the right.
Updates: 8/24/89 = change to conform to OCT2.0
*/
boolean
MkTerm(name, input, multiple, position)
char *name;
boolean input, multiple;
enum Position_e position;
{
    static struct octPoint noTranslate = {0, 0};
    octObject dummy, box, term;
    struct octPoint arrowTranslate;

    ERR_IF1(!PutShape(&blueOutlineLayer, &dummy, &pathShapes[(int) position],
	&noTranslate));
    ERR_IF1(!PutShape(&wiringLayer, &box, &boxShape,
	&boxTranslates[(int) position]));
    CK_OCT(ohGetByTermName(facetPtr, &term, name));
    CK_OCT(octAttach(&term, &box));
    ERR_IF1(!PutShape(&blueSolidLayer, &dummy, &arrowShape, 
	&boxTranslates[(int) position]));
    if (multiple) {
/* this is obsolete now (eeg 8/27/89)
	(void) CreatePropStr(&term, &dummy, "multiple", "");
*/
	if (input) {
	    /* multiple input: add arrow to the right */
	    arrowTranslate.x = boxTranslates[(int) position].x + (octCoord) 5;
	    arrowTranslate.y = boxTranslates[(int) position].y;
	    ERR_IF1(!PutShape(&blueSolidLayer, &dummy, &arrowShape,
		&arrowTranslate));
	} else {
	    /* multiple output: add arrow to the left */
	    arrowTranslate.x = boxTranslates[(int) position].x - (octCoord) 5;
	    arrowTranslate.y = boxTranslates[(int) position].y;
	    ERR_IF1(!PutShape(&blueSolidLayer, &dummy, &arrowShape,
		&arrowTranslate));
	}
    } else {
/* this is obsolete now (eeg 8/27/89)
	(void) CreatePropStr(&term, &dummy, "single", "");
*/
    }
    if (input) {
	(void) CreatePropStr(&term, &dummy, "input", "");
    } else {
	(void) CreatePropStr(&term, &dummy, "output", "");
    }
    return(TRUE);
}
