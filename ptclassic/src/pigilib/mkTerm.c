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
#include "mkTerm.h"
#include "mkIcon.h"

/* define paths to each possible terminal position */
/* relative to the terminal box position */
struct octPoint luPoints[] = {
    {0, 0}, {10, 0}, {25, -15}
};
struct octPoint lcPoints[] = {
    {0, 0}, {25, 0}
};
struct octPoint llPoints[] = {
    {0, 0}, {10, 0}, {25, 15}
};
struct octPoint ruPoints[] = {
    {0, 0}, {-10, 0}, {-25, -15}
};
struct octPoint rcPoints[] = {
    {0, 0}, {-25, 0}
};
struct octPoint rlPoints[] = {
    {0, 0}, {-15, 0}, {-25, 15}
};

Shape leftPathShapesTable[] = {
    { OCT_PATH, luPoints, sizeof(luPoints)/sizeof(struct octPoint) },
    { OCT_PATH, lcPoints, sizeof(lcPoints)/sizeof(struct octPoint) },
    { OCT_PATH, llPoints, sizeof(llPoints)/sizeof(struct octPoint) },
};
Shape rightPathShapesTable[] = {
    { OCT_PATH, ruPoints, sizeof(ruPoints)/sizeof(struct octPoint) },
    { OCT_PATH, rcPoints, sizeof(rcPoints)/sizeof(struct octPoint) },
    { OCT_PATH, rlPoints, sizeof(rlPoints)/sizeof(struct octPoint) },
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

/* FIXME: packetColor should become messageColor!!! */

static octObject *facetPtr, wiringLayer;
static octObject floatColorLayer, intColorLayer, complexColorLayer,
		 anytypeColorLayer, packetColorLayer;

/* terminalPath 12/21/91 - by Edward A. Lee
*/
static Shape*
terminalPath(left,position,translation,totalNumber)
boolean left;
int position;
struct octPoint *translation;
int totalNumber;
{
    translation->x = (left) ? -75 : 75;
    translation->y = -(position - 2)*25;
    if(totalNumber >= 4) {
	if(left && position == 0) return &leftPathShapesTable[0];
	if(position == 0) return &rightPathShapesTable[0];
	if(left && position == totalNumber-1) return &leftPathShapesTable[2];
	if(position == totalNumber-1) return &rightPathShapesTable[2];
    }
    if(left) return &leftPathShapesTable[1];
    else return &rightPathShapesTable[1];
}

/* MkTermInit 7/28/89 8/6/88 8/27/88
Call this first.
*/
boolean
MkTermInit(CurrentFacetPtr)
octObject *CurrentFacetPtr;
{
    facetPtr = CurrentFacetPtr;
    CK_OCT(ohGetOrCreateLayer(facetPtr, &floatColorLayer, "floatColor"));
    CK_OCT(ohGetOrCreateLayer(facetPtr, &intColorLayer, "intColor")); 
    CK_OCT(ohGetOrCreateLayer(facetPtr, &complexColorLayer, "complexColor"));
    CK_OCT(ohGetOrCreateLayer(facetPtr, &anytypeColorLayer, "anytypeColor")); 
    CK_OCT(ohGetOrCreateLayer(facetPtr, &packetColorLayer, "packetColor")); 
    CK_OCT(ohGetOrCreateLayer(facetPtr, &wiringLayer, "WIRING"));
    return(TRUE);
}

/*
Caveats: Assumes that Shape has <= SHAPE_MAX points
*/
boolean
PutShape(containPtr, objPtr, shapePtr, translatePtr)
octObject *containPtr, *objPtr;
Shape *shapePtr;
struct octPoint *translatePtr;
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
MkTerm(name, input, type, multiple, position, totalNumber)
char *name;
boolean input;
char *type;
boolean multiple;
int position;
int totalNumber;
{
    struct octPoint translation;
    Shape *path;
    octObject dummy, box, term;
    struct octPoint arrowTranslate;
    octObject *layerPtr;

    if (strcmp(type, "float") == 0 || strcmp(type, "FLOAT") == 0) {
	layerPtr = &floatColorLayer;
    } else if (strcmp(type, "int") == 0 || strcmp(type, "INT") == 0 ) {
	layerPtr = &intColorLayer;
    } else if (strcmp(type, "complex") == 0 || strcmp(type, "COMPLEX") == 0) {
	layerPtr = &complexColorLayer;
    } else if (strcmp(type, "anytype") == 0 || strcmp(type, "ANYTYPE") == 0) {
	layerPtr = &anytypeColorLayer;
    } else if (strcmp(type, "packet") == 0 || strcmp(type, "PACKET") == 0) {
	layerPtr = &packetColorLayer;
    } else if (strcmp(type, "message") == 0 || strcmp(type, "MESSAGE") == 0) {
	layerPtr = &packetColorLayer;
    } else if (strcmp(type, "fix") == 0 || strcmp(type, "FIX") == 0) {
	    /* temporary, should get own color */
	layerPtr = &floatColorLayer;
    } else {
	/* print error message, unknown datatype */
	ErrAdd("Unknown datatype for terminal");
	return FALSE;
    }
    path = terminalPath(input,position,&translation,totalNumber);
    ERR_IF1(!PutShape(layerPtr, &dummy, path, &translation));
    ERR_IF1(!PutShape(&wiringLayer, &box, &boxShape, &translation));
    CK_OCT(ohGetByTermName(facetPtr, &term, name));
    CK_OCT(octAttach(&term, &box));
    ERR_IF1(!PutShape(layerPtr, &dummy, &arrowShape, &translation));
    if (multiple) {
	if (input) {
	    /* multiple input: add arrow to the right */
	    arrowTranslate.x = translation.x + (octCoord) 5;
	} else {
	    /* multiple output: add arrow to the left */
	    arrowTranslate.x = translation.x - (octCoord) 5;
        }
        arrowTranslate.y = translation.y;
        ERR_IF1(!PutShape(layerPtr, &dummy, &arrowShape, &arrowTranslate));
    }
    if (input) {
	(void) ohCreatePropStr(&term, &dummy, "input", "");
    } else {
	(void) ohCreatePropStr(&term, &dummy, "output", "");
    }
    return(TRUE);
}
