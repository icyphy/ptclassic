/* mkTerm.c  edg
Version identification:
$Id$
Functions that create terminals and thier symbols in a icon facet.
Functions here use err.c for error handling.
*/

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

/* Includes */
#include "local.h"
#include <stdio.h>
#include "rpc.h"
#include "err.h"
#include "octMacros.h"
#include "oh.h"
#include "mkTerm.h"
#include "mkIcon.h"

/* 
 * Define paths for stems from the icon body to each possible
 * terminal position, relative to the terminal box position.
 * The terminal box is what oct uses as the actual terminal.
 * The "l" means "left", and there are upper, center,
 * and lower paths.  The center paths are just straight lines,
 * while the lower and upper paths bend, for better visual appearance.
 */
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

/*
 * The arrowShape is used to mark a terminal as input or output
 * and multiple or single porthole. Multiple portholes use two arrows.
 * The points are given relative to the crotch of the arrow, where
 * the terminal box goes for single portholes.
 */
struct octPoint arrowPoints[] = {
    {0, 0}, {-5, 5}, {5, 0}, {-5, -5}
};

Shape arrowShape = {
    OCT_POLYGON, arrowPoints, sizeof(arrowPoints)/sizeof(struct octPoint)
};

/* 
 * The largeArrow shape was added by Mike Chen for MatrixMessage portholes
 */
struct octPoint largeArrowPoints[] = {
    {0, 0}, {-5, 8}, {5, 0}, {-5, -8}
};

Shape largeArrowShape = {
    OCT_POLYGON, largeArrowPoints, sizeof(largeArrowPoints)/sizeof(struct octPoint)
};

/* end of arrowShapes */

/* 
 * The boxShape defines the terminal itself.  The arrows and paths
 * are just decorations around the terminal.
 */
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
		 anytypeColorLayer, packetColorLayer, fixColorLayer,
		 labelLayer;

/* 
 * terminalPath 12/21/91 - by Edward A. Lee
 * For a given portole (left or right and given number (position))
 * return a path shape to use for the stem and translation
 * (relative to the center of the icon) for that path shape. 
 * This routine also needs to know the total number of portholes,
 * in order to select a visulally appealing path shape.
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

/*
 * MkTermLabel 10/1/94 by Edward A. Lee
 * If there is more than one terminal total, create a label.
 */
static boolean
MkTermLabel(name, input, position, totalNumber)
	char *name;
	boolean input;
	int position;
	int totalNumber;
{
    octObject label;
    int llx, lly, urx, ury, len;
    
    static struct octBox labelBox;

    if (totalNumber == 1) return (TRUE);
    
    /* Compute position of the box, relative to the center of the icon */
    /* We actually specify the x and y coordinates of the lower left   */
    /* and upper right corners of the bounding box.                    */
    /* The size of the bounding box is determined by visual appeal.    */
    len = strlen(name);
    if (input)  {
    	urx = -55;
    	llx = -8*len - 55;
    } else  {
    	urx = 8*len + 55;
    	llx = 55;
    }
    /* The y coordinates do not depend on whether its input. */
    ury = -(position - 2)*25 + 20;
    lly = -(position - 2)*25 + 5;

    labelBox.lowerLeft.x = (octCoord) llx;
    labelBox.lowerLeft.y = (octCoord) lly;
    labelBox.upperRight.x = (octCoord) urx;
    labelBox.upperRight.y = (octCoord) ury;

    if (input)  {
    	CK_OCT(ohCreateLabel(&labelLayer, &label, name, labelBox, (octCoord) 15,
	    OCT_JUST_BOTTOM, OCT_JUST_RIGHT, OCT_JUST_RIGHT));
    } else  {
    	CK_OCT(ohCreateLabel(&labelLayer, &label, name, labelBox, (octCoord) 15,
	    OCT_JUST_BOTTOM, OCT_JUST_LEFT, OCT_JUST_LEFT));
    }
    return (TRUE);
}


/*
 * MkTermInit 7/28/89 8/6/88 8/27/88
 * Call this first.  This sets the available layers.
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
    CK_OCT(ohGetOrCreateLayer(facetPtr, &fixColorLayer, "fixColor")); 
    CK_OCT(ohGetOrCreateLayer(facetPtr, &wiringLayer, "WIRING"));
    CK_OCT(ohGetOrCreateLayer(facetPtr, &labelLayer, "label"));
    return(TRUE);
}

/*
 * PutShape.
 * Draw a shape.
 * Caveats: Assumes that Shape has <= SHAPE_MAX points.
 * Thickness argument handling added 11/11/93 by Mike Chen.
 */
boolean
PutShape (containPtr, objPtr, shapePtr, translatePtr, thick)
octObject *containPtr, *objPtr;
Shape *shapePtr;
struct octPoint *translatePtr;
boolean thick;
{
    struct octPoint buf[SHAPE_MAX], *dest, *src;
    octCoord tx, ty;

    tx = translatePtr->x;
    ty = translatePtr->y;
    objPtr->type = shapePtr->type;

    switch (shapePtr->type) {
    case OCT_PATH:
        if(thick) objPtr->contents.path.width = (octCoord) 5;
        else objPtr->contents.path.width = (octCoord) 0;
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

/*
 * MkTerm 8/24/89 8/6/88
 * Create a terminal, arrow, and stem  for a porthole.
 * Always puts inputs on the left and outputs on the right.
 */
boolean
MkTerm(name, input, type, multiple, position, totalNumber)
	char	*name;
	boolean	input;
	char 	*type;
	boolean	multiple;
	int	position;
	int	totalNumber;
{
    struct octPoint translation;
    Shape *path;
    octObject dummy, box, term;
    struct octPoint arrowTranslate;
    octObject *layerPtr;
    boolean thick = FALSE;

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
	layerPtr = &fixColorLayer;
    } else if (strcmp(type, "COMPLEX_MATRIX_ENV") == 0 || strcmp(type, "COMPLEX_MATRIX") == 0) {
	layerPtr = &complexColorLayer;
        thick = TRUE;
    } else if (strcmp(type, "FIX_MATRIX_ENV") == 0 || strcmp(type, "FIX_MATRIX") == 0) {
	layerPtr = &fixColorLayer;
        thick = TRUE;
    } else if (strcmp(type, "FLOAT_MATRIX_ENV") == 0 || strcmp(type, "FLOAT_MATRIX") == 0) {
	layerPtr = &floatColorLayer;
        thick = TRUE;
    } else if (strcmp(type, "INT_MATRIX_ENV") == 0 || strcmp(type, "INT_MATRIX") == 0) {
	layerPtr = &intColorLayer;
        thick = TRUE;
    } else {
	/* print error message, unknown datatype */
	ErrAdd("Unknown datatype for terminal");
	return FALSE;
    }
    path = terminalPath(input,position,&translation,totalNumber);
    ERR_IF1(!PutShape(layerPtr, &dummy, path, &translation, thick));
    ERR_IF1(!MkTermLabel(name, input, position, totalNumber));
    ERR_IF1(!PutShape(&wiringLayer, &box, &boxShape, &translation, thick));
    CK_OCT(ohGetByTermName(facetPtr, &term, name));
    CK_OCT(octAttach(&term, &box));
    if(thick) {
      ERR_IF1(!PutShape(layerPtr,&dummy,&largeArrowShape,&translation,thick));
    }
    else {
      ERR_IF1(!PutShape(layerPtr, &dummy, &arrowShape, &translation, thick));
    }
    if (multiple) {
	if (input) {
	    /* multiple input: add arrow to the right */
	    arrowTranslate.x = translation.x + (octCoord) 5;
	} else {
	    /* multiple output: add arrow to the left */
	    arrowTranslate.x = translation.x - (octCoord) 5;
        }
        arrowTranslate.y = translation.y;
        if(thick) {
          ERR_IF1(!PutShape(layerPtr,&dummy,&largeArrowShape,&arrowTranslate,thick));
        }
        else {
          ERR_IF1(!PutShape(layerPtr, &dummy, &arrowShape, &arrowTranslate, thick));
        }
    }
    if (input) {
	(void) ohCreatePropStr(&term, &dummy, "input", "");
    } else {
	(void) ohCreatePropStr(&term, &dummy, "output", "");
    }
    return(TRUE);
}
