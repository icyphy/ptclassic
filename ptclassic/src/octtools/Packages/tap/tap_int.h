/* Version Identification:
 * $Id$
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
#ifndef TAP_INT_H
#define TAP_INT_H
#include "copyright.h"
#include "tap.h"

extern int tapPassedStatus;
extern int tapDepth;
extern char *tapRoutine;
extern void tapErrHandler();

#define TAP_START(name)		if (tapDepth++ == 0) {			\
				    tapRoutine = name;			\
				    errPushHandler(tapErrHandler);	\
				}
#define TAP_END()		if (--tapDepth == 0) {			\
				    errPopHandler();			\
				}

#undef OCT_ASSERT
#define OCT_ASSERT(exp)		if ((tapPassedStatus = (exp)) < OCT_OK) {\
				    errRaise("oct", tapPassedStatus,	\
						octErrorString());	\
				}


#define TAP_CELL_NAME		"%s/%s/tap.views"
#define TAP_DIR_NAME		"%s/%s"
#define DEFAULT_TAP_NAME	"."
#define TAP_FACET_NAME		"contents"

#define TECH_PROP_NAME		"TECHNOLOGY"
#define VIEW_PROP_NAME		"VIEWTYPE"

#define TECH_PROP_BAG_NAME	"TECHNOLOGY-PROPERTIES"

#define VIA_BAG_NAME		"VIA-LAYERS"

struct tapTechName {
    struct tapTechName *next;
    char *name;
    char *view;
    struct tapTech *techPtr;
};

struct tapTech {
    struct tapTech *next;
    octObject facet;
    int layerCount;
    struct tapLayer *layerList;
    struct tapConnector *connList;
};

extern struct tapTech *tapInternTech();
extern void tapForEachTech(), tapForEachLayer();

struct tapLayer {
    struct tapTech *techPtr;
    octObject techLayer;
    int layerIndex;
    struct tapDesignRules *desRulePtr;
    struct tapLayerLooks *looksPtr;
    int isConnector;
};

extern struct tapLayer *tapInternLayer();

    /* design rule stuff */
#define TAP_RULE_BAG_NAME	"DESIGN-RULES"
#define TAP_MIN_RULE_NAME	"MINIMUM"
#define TAP_MAX_RULE_NAME	"MAXIMUM"

struct tapDesignRules {
    octCoord minWidth, maxWidth;
    octCoord *minSpacing, *minOverlap;
};


    /* layer looks stuff */
#define DISPLAY_BAG_NAME	"DISPLAY-LOOKS"
#define PRIORITY_NAME		"PRIORITY"
#define COLOR_NAME		"RGB-VALS"
#define FILL_PATTERN_NAME	"FILL-PATTERN"
#define BORDER_PATTERN_NAME	"BORDER-PATTERN"

struct RGB {
    unsigned short red, green, blue;
};

struct pattern {
    int flags;
    int width, height;
    char *bitArray;
};

#define MAX_STROKES		8

struct tapLayerLooks {
    int priority;
    int nColors;
    struct RGB *colorVals;
    struct pattern borderPattern;
    struct pattern fillPattern;
    int strokeCount;
    struct tapStroke fillStrokes[MAX_STROKES];
};

extern void tapFreeLayerLooksInfo();

    /* palette stuff */
#define PALETTE_BAG_NAME	"PALETTES"


    /* connector table stuff */
struct tapConnector {
    struct tapConnector *next;
    octObject instance;
    octObject master;
    int nLayers;
    int actualLayers;		/* AC. Actual layers in the connector */
    float area;
    struct connLayer *layerDescList;
};

struct connLayer {
    int isUsed;				/* is this layer used at all */
    octCoord widthArray[4];		/* from (angle/900) direction */
};

#define CONN_BAG_NAME		"MANHATTAN-CONNECTOR"
#define CONN_AREA_PROP_NAME	"AREA"
#define CONN_LAYER_PROP_NAME	"LAYER-WIDTHS"

void tapFreeConnTable();
#endif
