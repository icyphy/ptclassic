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
#include "tap_int.h"

static char *displayName = NIL(char);
static int displayChromaticism = -1;

/* ---- Forward declaration of static functions --------- */
static struct tapLayer *looksInternLayer();
static void findDisplayBag();
static void parseDisplayBag();
static void parsePattern();
static void parsePatternToStrokes();
static int walkPattern();
static int stepPattern();
static struct bitmap *getBitmap();
static void createDisplayBag();
				

/*LINTLIBRARY*/

void tapSetDisplayType(name, chromaticism)
char *name;
int chromaticism;
{
    TAP_START("tapSetDisplayType");

    if (chromaticism != TAP_BW && chromaticism != TAP_COLOR) {
	errRaise(TAP_PKG_NAME, 0,
			"illegal chromaticism value: %d", chromaticism);
    }

    if (name == NIL(char)) {
	name = (chromaticism == TAP_BW) ?
			    TAP_DEFAULT_BW_NAME : TAP_DEFAULT_COLOR_NAME;
    }

    FREE(displayName);
    displayName = util_strsav(name);
    displayChromaticism = chromaticism;

    tapForEachLayer(tapFreeLayerLooksInfo);

    TAP_END();
}

void tapGetDisplayInfo(layerPtr, prioPtr, nColorPtr, fillPtr, bdrPtr)
octObject *layerPtr;
int *prioPtr, *nColorPtr, *fillPtr, *bdrPtr;
{
    struct tapLayerLooks *looksPtr;
    struct tapLayer *looksInternLayer();

    TAP_START("tapGetDisplayInfo");

    looksPtr = looksInternLayer(layerPtr)->looksPtr;

    if (prioPtr) *prioPtr = looksPtr->priority;
    if (nColorPtr) *nColorPtr = looksPtr->nColors;
    if (fillPtr) *fillPtr = looksPtr->fillPattern.flags;
    if (bdrPtr) *bdrPtr = looksPtr->borderPattern.flags;

    TAP_END();
}

void tapGetDisplayColor(layerPtr, index, redPtr, greenPtr, bluePtr)
octObject *layerPtr;
int index;
unsigned short *redPtr, *greenPtr, *bluePtr;
{
    struct tapLayerLooks *looksPtr;
    struct tapLayer *looksInternLayer();

    TAP_START("tapGetDisplayColor");

    looksPtr = looksInternLayer(layerPtr)->looksPtr;

    if (index < 0 || index >= looksPtr->nColors) {
	errRaise(TAP_PKG_NAME, 0, "color %d not defined for layer %s\n",
		index, layerPtr->contents.layer.name);
    }

    if (redPtr) *redPtr = looksPtr->colorVals[index].red;
    if (greenPtr) *greenPtr = looksPtr->colorVals[index].green;
    if (bluePtr) *bluePtr = looksPtr->colorVals[index].blue;

    TAP_END();
}

void tapGetDisplayPattern(lyrPtr, type, widthPtr, heightPtr, bitPtr)
octObject *lyrPtr;
int type;
int *widthPtr, *heightPtr;
char **bitPtr;
{
    struct tapLayerLooks *looksPtr;
    struct tapLayer *looksInternLayer();

    TAP_START("tapGetDisplayPattern");

    looksPtr = looksInternLayer(lyrPtr)->looksPtr;

    switch (type) {
	case TAP_FILL_PATTERN:
	    if (widthPtr) *widthPtr = looksPtr->fillPattern.width;
	    if (heightPtr) *heightPtr = looksPtr->fillPattern.height;
	    if (bitPtr) *bitPtr = looksPtr->fillPattern.bitArray;
	    break;
	case TAP_BORDER_PATTERN:
	    if (widthPtr) *widthPtr = looksPtr->borderPattern.width;
	    if (heightPtr) *heightPtr = looksPtr->borderPattern.height;
	    if (bitPtr) *bitPtr = looksPtr->borderPattern.bitArray;
	    break;
	default:
	    errRaise(TAP_PKG_NAME, 0, "illegal pattern type %d for layer %s",
		    type, lyrPtr->contents.layer.name);
    }

    TAP_END();
}

static struct tapLayer *looksInternLayer(layerPtr)
octObject *layerPtr;
{
    struct tapLayer *layerS;
    octObject displayBag;
    struct tapLayerLooks *looksPtr;
    void findDisplayBag(), parseDisplayBag();

    layerS = tapInternLayer(layerPtr);

    if (layerS->looksPtr == NIL(struct tapLayerLooks)) {
	findDisplayBag(&layerS->techLayer, &displayBag);
	looksPtr = ALLOC(struct tapLayerLooks, 1);
	looksPtr->strokeCount = -1;
	parseDisplayBag(&displayBag, looksPtr, layerPtr);
	layerS->looksPtr = looksPtr;
    }

    return(layerS);
}

void tapFreeLayerLooksInfo(layerS)
struct tapLayer *layerS;
{
    if (layerS->looksPtr) {
	if (layerS->looksPtr->nColors > 0) {
	    FREE(layerS->looksPtr->colorVals);
	}
	FREE(layerS->looksPtr->fillPattern.bitArray);
	FREE(layerS->looksPtr->borderPattern.bitArray);
	FREE(layerS->looksPtr);
	layerS->looksPtr = NIL(struct tapLayerLooks);
    }
}

static void findDisplayBag(layerPtr, bagPtr)
octObject *layerPtr;
octObject *bagPtr;
{
    octObject bigBag;
    octGenerator gen;
    octStatus status;
    octObject colorSpec;

    if (ohGetByBagName(layerPtr, &bigBag, DISPLAY_BAG_NAME) != OCT_OK) {
	errRaise(TAP_PKG_NAME, 0,
		"no display attributes available for layer %s",
		layerPtr->contents.layer.name);
    }

    /* look for attributes for the named display */
    if (displayName == NIL(char)) {
	errRaise(TAP_PKG_NAME, 0,
		"display not specified before querying layer looks");
    }
    if (ohGetByBagName(&bigBag, bagPtr, displayName) == OCT_OK) return;

    /* look for any display with proper chromaticism */
    OCT_ASSERT(octInitGenContents(&bigBag, OCT_BAG_MASK, &gen));
    while ((status = octGenerate(&gen, bagPtr)) == OCT_OK) {
	colorSpec.type = OCT_PROP;
	colorSpec.contents.prop.name = COLOR_NAME;
	status = octGetByName(bagPtr, &colorSpec);
	if (displayChromaticism == TAP_COLOR) {
	    if (status == OCT_OK) return;
	} else {
	    if (status != OCT_OK) return;
	}
    }
    OCT_ASSERT(status);

    errRaise(TAP_PKG_NAME, 0, "no display attributes available for layer %s",
	    layerPtr->contents.layer.name);
}

static void parseDisplayBag(bagPtr, looksPtr, layerPtr)
octObject *bagPtr;
struct tapLayerLooks *looksPtr;
octObject *layerPtr;
{
    octObject prop;
    int32 *valPtr;
    int i;
    void parsePattern();

    /* get display priority */
    if (ohGetByPropName(bagPtr, &prop, PRIORITY_NAME) == OCT_OK) {
	if (prop.contents.prop.type != OCT_INTEGER) {
	    errRaise(TAP_PKG_NAME, 0,
		    "corrupt technology facet -- bad priority for layer %s",
		    layerPtr->contents.layer.name);
	}
	looksPtr->priority = prop.contents.prop.value.integer;
    } else {
	looksPtr->priority = 0;
    }

    /* get colors */
    if (ohGetByPropName(bagPtr, &prop, COLOR_NAME) == OCT_OK) {
	if (prop.contents.prop.type != OCT_INTEGER_ARRAY ||
		    prop.contents.prop.value.integer_array.length % 3 != 0) {
	    errRaise(TAP_PKG_NAME, 0,
		    "corrupt technology facet -- bad color spec for layer %s",
		    layerPtr->contents.layer.name);
	}
	looksPtr->nColors = prop.contents.prop.value.integer_array.length / 3;
	looksPtr->colorVals = ALLOC(struct RGB, looksPtr->nColors);
	valPtr = prop.contents.prop.value.integer_array.array;
	for (i = 0; i < looksPtr->nColors; i++) {
	    looksPtr->colorVals[i].red = *valPtr++;
	    looksPtr->colorVals[i].green = *valPtr++;
	    looksPtr->colorVals[i].blue = *valPtr++;
	}
    } else {
	looksPtr->nColors = 0;
	looksPtr->colorVals = NIL(struct RGB);
    }

    /* get fill pattern */
    parsePattern(bagPtr, &(looksPtr->fillPattern), TAP_FILL_PATTERN, layerPtr);

    /* get border pattern */
    parsePattern(bagPtr, &(looksPtr->borderPattern), TAP_BORDER_PATTERN,
								    layerPtr);
}

static void parsePattern(bagPtr, patPtr, type, layerPtr)
octObject *bagPtr;
struct pattern *patPtr;
int type;
octObject *layerPtr;
{
    octObject prop;
    double bitArrayLen;

    prop.type = OCT_PROP;
    if (type == TAP_FILL_PATTERN) {
	prop.contents.prop.name = FILL_PATTERN_NAME;
    } else {
	prop.contents.prop.name = BORDER_PATTERN_NAME;
    }

    /* if property not there, empty pattern */
    if (octGetByName(bagPtr, &prop) != OCT_OK) {
	patPtr->flags = TAP_EMPTY;
	patPtr->width = patPtr->height = 1;
	patPtr->bitArray = util_strsav("0");
	return;
    }

    if (prop.contents.prop.type != OCT_STRING) {
	errRaise(TAP_PKG_NAME, 0,
		"corrupt technology facet -- bad pattern for layer %s",
		layerPtr->contents.layer.name);
    }

    /* if property value is zero or more 0s, empty pattern */
    if (strchr(prop.contents.prop.value.string, '1') == NIL(char)) {
	patPtr->flags = TAP_EMPTY;
	patPtr->width = patPtr->height = 1;
	patPtr->bitArray = util_strsav("0");
	return;
    }

    /* if property value is one or more 1s, solid pattern */
    if (strchr(prop.contents.prop.value.string, '0') == NIL(char)) {
	patPtr->flags = TAP_SOLID;
	patPtr->width = patPtr->height = 1;
	patPtr->bitArray = util_strsav("1");
	return;
    }

    /* must be stippled pattern */
    patPtr->flags = TAP_STIPPLED;
    patPtr->bitArray = util_strsav(prop.contents.prop.value.string);

    /* Hack to avoid Purify warnings here.
       patPtr->width = sqrt((double)strlen(patPtr->bitArray));
       was causing trouble
Purify (bsw): write below stack pointer:
  * This is occurring while in:
       parsePattern [line 327, ../../../../src/octtools/Packages/tap/layerlooks.c, pc=0x3415c]
 parseDisplayBag [line 271, ../../../../src/octtools/Packages/tap/layerlooks.c, pc=0x33f2c]
      looksInternLayer [line 166, ../../../../src/octtools/Packages/tap/layerlooks.c, pc=0x33860]
     tapGetDisplayInfo [line 86, ../../../../src/octtools/Packages/tap/layerlooks.c, pc=0x332bc]
     setTechnology [line 1446, /users/ptolemy/src/gnu/src/gcc/libgcc2.c, pc=0xb99cc]
 processCell  [line 813, /users/ptolemy/src/gnu/src/gcc/libgcc2.c, pc=0xb51d8]
  * Writing 4 bytes to 0xeffeef4c.

     */
    bitArrayLen = strlen(patPtr->bitArray);
    if (type == TAP_FILL_PATTERN) {
	patPtr->width = sqrt(bitArrayLen);
	patPtr->height = patPtr->width;
	patPtr->bitArray[patPtr->width * patPtr->height] = '\0';
    } else {
	patPtr->width = strlen(patPtr->bitArray);
	patPtr->height = 1;
    }
}

void tapGetDisplayStrokes(layerPtr, countPtr, strokePtr)
octObject *layerPtr;
int *countPtr;
tapStroke **strokePtr;
{
    struct tapLayerLooks *looksPtr;
    struct tapLayer *looksInternLayer();
    void parsePatternToStrokes();

    TAP_START("tapGetDisplayStrokes");

    looksPtr = looksInternLayer(layerPtr)->looksPtr;
    if (looksPtr->strokeCount < 0) {
	parsePatternToStrokes(looksPtr);
    }

    *countPtr = looksPtr->strokeCount;
    *strokePtr = looksPtr->fillStrokes;

    TAP_END();
}

struct bitmap {
    int height, width;
    char **bits;
};

struct walkPath {
    int x, y;
    int active;
};

struct walker {
    enum dbxNeedsAName { BOTTOM, SIDE } startEdge;
    int deltaX1, deltaY1;		/* first choice direction */
    int deltaX2, deltaY2;		/* second choice direction */
};

static struct walker walkTab[] = {
    {	SIDE,	1,	0,	1,	1	},	/*   0 -  45 deg */
    {	SIDE,	1,	1,	1,	0	},	/*   0 -  45 deg */
    {	BOTTOM,	1,	1,	0,	1	},	/*  45 -  90 deg */
    {	BOTTOM,	0,	1,	1,	1	},	/*  45 -  90 deg */
    {	BOTTOM,	0,	1,	-1,	1	},	/*  90 - 135 deg */
    {	BOTTOM,	-1,	1,	0,	1	},	/*  90 - 135 deg */
    {	SIDE,	-1,	1,	-1,	0	},	/* 135 - 180 deg */
    {	SIDE,	-1,	0,	-1,	1	},	/* 135 - 180 deg */
};

static void parsePatternToStrokes(looksPtr)
struct tapLayerLooks *looksPtr;
{
    int i;
    tapStroke *strokePtr = looksPtr->fillStrokes;
    struct bitmap *bmPtr;
    struct bitmap *getBitmap();

    switch (looksPtr->fillPattern.flags) {
	case TAP_EMPTY:
	    looksPtr->strokeCount = 0;
	    break;
	case TAP_SOLID:
	    looksPtr->strokeCount = 1;
	    looksPtr->fillStrokes[0].angle = 0;
	    looksPtr->fillStrokes[0].spacing = 1;
	    break;
	case TAP_STIPPLED:
	    bmPtr = getBitmap(&looksPtr->fillPattern);
	    for (i = 0; i < sizeof(walkTab)/sizeof(walkTab[0]); i++) {
		if (walkPattern(bmPtr, &walkTab[i], strokePtr)) {
		    if (strokePtr == looksPtr->fillStrokes ||
			    (strokePtr->angle != (strokePtr - 1)->angle &&
			     strokePtr->angle !=
				     looksPtr->fillStrokes->angle + 1800)) {
			strokePtr++;
		    }
		}
	    }
	    looksPtr->strokeCount = strokePtr - looksPtr->fillStrokes;

	    FREE(bmPtr->bits);
	    FREE(bmPtr);
	    break;
	default:
	    errRaise(TAP_PKG_NAME, 0, "internal error -- bad fill pattern flag");
    }
}


#ifndef PI
#define PI	3.14159265358979
#endif

static int walkPattern(bmPtr, walkerPtr, strokePtr)
struct bitmap *bmPtr;
struct walker *walkerPtr;
tapStroke *strokePtr;
{
    struct walkPath *paths, *newPaths, *temp;
    int i, count = 1;		/* Make count = 1 to avoid divide by
				   zero. */
    int w;		/* dimension parallel to startEdge */
    int h;		/* dimension perpendicular to startEdge */
    int stepPattern();

    if (walkerPtr->startEdge == BOTTOM) {
	h = bmPtr->height;
	w = bmPtr->width;
    } else {
	h = bmPtr->width;
	w = bmPtr->height;
    }

    paths = ALLOC(struct walkPath, w);
    newPaths = ALLOC(struct walkPath, w);
    for (i = 0; i < w; i++) {
	if (walkerPtr->startEdge == BOTTOM) {
	    paths[i].x = i;	paths[i].y = 0;
	    paths[i].active = bmPtr->bits[0][i] == '1';
	} else {
	    paths[i].x = 0;	paths[i].y = i;
	    paths[i].active = bmPtr->bits[i][0] == '1';
	}
    }
    for (i = 0; i < h; i++) {
	if ((count = stepPattern(bmPtr, w, paths, newPaths,
				walkerPtr->deltaX1, walkerPtr->deltaY1)) ||
		    (count = stepPattern(bmPtr, w, paths, newPaths,
				walkerPtr->deltaX2, walkerPtr->deltaY2))) {
	    temp = paths;
	    paths = newPaths;
	    newPaths = temp;
	} else {
	    FREE(paths);
	    FREE(newPaths);
	    return(0);
	}
    }
    strokePtr->spacing = (w + count - 1) / count;
    strokePtr->angle = 0.5 + (1800 / PI) *
		    atan2((double) paths[0].y, (double) paths[0].x);

    FREE(paths);
    FREE(newPaths);
    return(1);
}

#define MOD(x, y)	( ((x)%(y) >= 0) ? ((x)%(y)) : ((x)%(y) + (y)))

static int stepPattern(bmPtr, w, paths, newPaths, deltaX, deltaY)
register struct bitmap *bmPtr;
int w;
register struct walkPath *paths, *newPaths;
int deltaX, deltaY;
{
    int i;
    int count = 0;
    int x, y;

    for (i = 0; i < w; i++) {
	newPaths->x = paths->x + deltaX;
	newPaths->y = paths->y + deltaY;
	x = MOD(newPaths->x, bmPtr->width);
	y = MOD(newPaths->y, bmPtr->height);
	newPaths->active = paths->active && bmPtr->bits[y][x] == '1';
	if (newPaths->active) count++;
	paths++;
	newPaths++;
    }

    return(count);
}

static struct bitmap *getBitmap(patternPtr)
struct pattern *patternPtr;
{
    struct bitmap *bmPtr;
    char *tapBM;
    int i;

    bmPtr = ALLOC(struct bitmap, 1);
    bmPtr->width = patternPtr->width;
    bmPtr->height = patternPtr->height;
    bmPtr->bits = ALLOC(char *, bmPtr->height);

    tapBM = patternPtr->bitArray + (bmPtr->height * bmPtr->width);
    for (i = 0; i < bmPtr->height; i++) {
	tapBM -= bmPtr->width;
	bmPtr->bits[i] = tapBM;
    }

    return(bmPtr);
}

void tapSetDisplayInfo(layerPtr, prio, nColors, fill, border)
octObject *layerPtr;
int prio, nColors, fill, border;
{
    octObject displayBag;
    octObject prop;
    int n;
    void createDisplayBag();

    TAP_START("tapSetDisplayInfo");

    createDisplayBag(layerPtr, &displayBag);
    OCT_ASSERT(ohCreateOrModifyPropInt(&displayBag, &prop, PRIORITY_NAME, prio));

    if (nColors <= 0) {
	if (ohGetByPropName(&displayBag, &prop, COLOR_NAME) == OCT_OK) {
	    OCT_ASSERT(octDelete(&prop));
	}
    } else {
	prop.type = OCT_PROP;
	prop.contents.prop.name = COLOR_NAME;
	prop.contents.prop.type = OCT_INTEGER_ARRAY;
	n = 3 * nColors;
	prop.contents.prop.value.integer_array.length = n;
	prop.contents.prop.value.integer_array.array = ALLOC(int32, n);
	while (--n >= 0) {
	    prop.contents.prop.value.integer_array.array[n] = 0;
	}
	OCT_ASSERT(octCreateOrModify(&displayBag, &prop));
	FREE(prop.contents.prop.value.integer_array.array);
    }

    prop.contents.prop.name = FILL_PATTERN_NAME;
    prop.contents.prop.type = OCT_STRING;
    switch (fill) {
	case TAP_EMPTY:
	    prop.contents.prop.value.string = "0";
	    break;
	case TAP_SOLID:
	    prop.contents.prop.value.string = "1";
	    break;
	default:
	    prop.contents.prop.value.string = "0110";
    }
    OCT_ASSERT(octCreateOrModify(&displayBag, &prop));

    prop.contents.prop.name = BORDER_PATTERN_NAME;
    prop.contents.prop.type = OCT_STRING;
    switch (border) {
	case TAP_EMPTY:
	    prop.contents.prop.value.string = "0";
	    break;
	case TAP_SOLID:
	    prop.contents.prop.value.string = "1";
	    break;
	default:
	    prop.contents.prop.value.string = "01";
    }
    OCT_ASSERT(octCreateOrModify(&displayBag, &prop));

    TAP_END();
}

void tapSetDisplayColor(layerPtr, index, red, green, blue)
    octObject *layerPtr;
    int index;
    unsigned short int red, green, blue; 
{
    octObject displayBag;
    octObject prop;
    void createDisplayBag();

    TAP_START("tapSetDisplayColor");

    index = 3 * index;
    if (red > TAP_MAX_INTENSITY) red = TAP_MAX_INTENSITY;
    if (green > TAP_MAX_INTENSITY) green = TAP_MAX_INTENSITY;
    if (blue > TAP_MAX_INTENSITY) blue = TAP_MAX_INTENSITY;

    createDisplayBag(layerPtr, &displayBag);
    if (ohGetByPropName(&displayBag, &prop, COLOR_NAME) != OCT_OK ||
		prop.contents.prop.type != OCT_INTEGER_ARRAY ||
		prop.contents.prop.value.integer_array.length < index + 3) {
	errRaise(TAP_PKG_NAME, 0, "attempt to set tint of undefined color");
    }

    prop.contents.prop.value.integer_array.array[index++] = red;
    prop.contents.prop.value.integer_array.array[index++] = green;
    prop.contents.prop.value.integer_array.array[index++] = blue;

    OCT_ASSERT(octModify(&prop));

    TAP_END();
}

/*ARGSUSED*/
void tapSetDisplayPattern(layerPtr, type, width, height, bitPtr)
octObject *layerPtr;
int type;
int width, height;	/* unused -- should check, I guess */
char *bitPtr;
{
    octObject displayBag;
    octObject prop;
    void createDisplayBag();

    TAP_START("tapSetDisplayPattern");

    createDisplayBag(layerPtr, &displayBag);
    prop.type = OCT_PROP;
    if (type == TAP_FILL_PATTERN) {
	prop.contents.prop.name = FILL_PATTERN_NAME;
    } else {
	prop.contents.prop.name = BORDER_PATTERN_NAME;
    }
    prop.contents.prop.type = OCT_STRING;
    prop.contents.prop.value.string = bitPtr;
    OCT_ASSERT(octCreateOrModify(&displayBag, &prop));

    TAP_END();
}

static void createDisplayBag(layerPtr, bagPtr)
octObject *layerPtr;
octObject *bagPtr;
{
    struct tapLayer *layerS;
    octObject bigBag;

    if (displayName == NIL(char)) {
	errRaise(TAP_PKG_NAME, 0,
		"display name not set before changing display looks");
    }

    layerS = tapInternLayer(layerPtr);
    tapFreeLayerLooksInfo(layerS);

    OCT_ASSERT(ohGetOrCreateBag(&layerS->techLayer, &bigBag, DISPLAY_BAG_NAME));
    OCT_ASSERT(ohGetOrCreateBag(&bigBag, bagPtr, displayName));
}
