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
#include "desrule.h"
#include "tap_int.h"

#define MIN_RULE	0
#define MAX_RULE	1

#define WIDTH		0
#define SPACING		1
#define OVERLAP		2
#define UNDERLAP	3	/* like overlap, but layers in reverse order */

static int minWidthRule(), maxWidthRule();
static int minSpacingRule(), minOverlapRule();
octCoord tapEvaluateDesignRule();

/* ---- Forward declaration of static functions --------- */
static struct tapLayer *desInternLayer();
static void initDesignRules();
static octCoord ruleDistance();
static int ruleType();
static void checkEdgesParallel();



static int (*ruleEvaluators[2][4]) () = {
	/* WIDTH */	/* SPACING */	/* OVERLAP */	/* UNDERLAP */
	{minWidthRule,	minSpacingRule,	minOverlapRule,	((int (*)())0)},
	{maxWidthRule,	((int (*)())0),	((int (*)())0),	((int (*)())0)}
};

/*LINTLIBRARY*/

octCoord tapGetMinWidth(layer)
octObject *layer;
{
    octCoord width;
    struct tapLayer *desInternLayer();

    TAP_START("tapGetMinWidth");
    width = desInternLayer(layer)->desRulePtr->minWidth;
    TAP_END();
    return(width);
}

void tapSetMinWidth(layer, width)
octObject *layer;
octCoord width;
{
    octObject facet, bag, rule, edge1, edge2;
    struct tapLayer *tlayer, *desInternLayer();

    TAP_START("tapSetMinWidth");
    tlayer = desInternLayer(layer);
    facet = tlayer->techPtr->facet;

    OCT_ASSERT(ohGetOrCreateBag(&facet, &bag, TAP_RULE_BAG_NAME));
    OCT_ASSERT(ohCreateBag(&bag, &rule, TAP_MIN_RULE_NAME));
    edge1.type = OCT_EDGE;
    edge1.contents.edge.start.x = 0;
    edge1.contents.edge.start.y = width;
    edge1.contents.edge.end.x = 0;
    edge1.contents.edge.end.y = 0;
    edge2.type = OCT_EDGE;
    edge2.contents.edge.start.x = width;
    edge2.contents.edge.start.y = 0;
    edge2.contents.edge.end.x = width;
    edge2.contents.edge.end.y = width;
    OCT_ASSERT(octCreate(&tlayer->techLayer, &edge1));
    OCT_ASSERT(octAttach(&rule, &edge1));
    OCT_ASSERT(octCreate(&tlayer->techLayer, &edge2));
    OCT_ASSERT(octAttach(&rule, &edge2));

    (void) tapEvaluateDesignRule(&rule, NIL(char));
    TAP_END();
    return;
}

octCoord tapGetMaxWidth(layer)
octObject *layer;
{
    octCoord width;
    struct tapLayer *desInternLayer();

    TAP_START("tapGetMaxWidth");
    width = desInternLayer(layer)->desRulePtr->maxWidth;
    TAP_END();
    return(width);
}

octCoord tapGetMinSpacing(layer1, layer2)
octObject *layer1, *layer2;
{
    struct tapLayer *layerS1, *layerS2;
    struct tapLayer *desInternLayer();

    TAP_START("tapGetMinSpacing");

    layerS1 = desInternLayer(layer1);
    layerS2 = desInternLayer(layer2);
    if (layerS1->techPtr != layerS2->techPtr) {
	errRaise(TAP_PKG_NAME, 0, "layers %s and %s not in same technology",
		layer1->contents.layer.name, layer2->contents.layer.name);
    }

    TAP_END();

    if (layerS1->desRulePtr->minSpacing == NIL(octCoord)) {
	return(TAP_NO_RULE);
    } else {
	return(layerS1->desRulePtr->minSpacing[ layerS2->layerIndex ]);
    }
}

octCoord tapGetMinOverlap(layer1, layer2)
octObject *layer1, *layer2;
{
    struct tapLayer *layerS1, *layerS2;
    struct tapLayer *desInternLayer();

    TAP_START("tapGetMinOverlap");

    layerS1 = desInternLayer(layer1);
    layerS2 = desInternLayer(layer2);
    if (layerS1->techPtr != layerS2->techPtr) {
	errRaise(TAP_PKG_NAME, 0, "layers %s and %s not in same technology",
		layer1->contents.layer.name, layer2->contents.layer.name);
    }

    TAP_END();

    if (layerS1->desRulePtr->minOverlap == NIL(octCoord)) {
	return(TAP_NO_RULE);
    } else {
	return(layerS1->desRulePtr->minOverlap[ layerS2->layerIndex ]);
    }
}

static struct tapLayer *desInternLayer(layer)
octObject *layer;
{
    struct tapLayer *layerS;
    void initDesignRules();

    layerS = tapInternLayer(layer);

    if (layerS->desRulePtr == NIL(struct tapDesignRules)) {
	initDesignRules(layerS->techPtr);

	if (layerS->desRulePtr == NIL(struct tapDesignRules)) {
	    errRaise(TAP_PKG_NAME, 0,
		    "layer %s disappeared from technology facet",
		    layer->contents.layer.name);
	}
    }

    return(layerS);
}

void
tapFreeDesRuleInfo(layerS)
struct tapLayer *layerS;
{
    if (layerS->desRulePtr) {
	FREE(layerS->desRulePtr->minSpacing);
	FREE(layerS->desRulePtr->minOverlap);
	FREE(layerS->desRulePtr);
	layerS->desRulePtr = NIL(struct tapDesignRules);
    }
}

static void initDesignRules(techPtr)
struct tapTech *techPtr;
{
    octObject ruleBag;
    octObject rule;
    octGenerator gen;
    octStatus status;
    struct tapLayer *layerS;
    int i, j;

    /* allocate design rule structures for all layers in tech facet */
    for (i = 0; i < techPtr->layerCount; i++) {
	layerS = &techPtr->layerList[i];
	layerS->desRulePtr = ALLOC(struct tapDesignRules, 1);
	layerS->desRulePtr->minWidth = TAP_NO_RULE;
	layerS->desRulePtr->maxWidth = TAP_NO_RULE;
	layerS->desRulePtr->minSpacing = ALLOC(octCoord, techPtr->layerCount);
	layerS->desRulePtr->minOverlap = ALLOC(octCoord, techPtr->layerCount);
	for (j = 0; j < techPtr->layerCount; j++) {
	    layerS->desRulePtr->minSpacing[j] = TAP_NO_RULE;
	    layerS->desRulePtr->minOverlap[j] = TAP_NO_RULE;
	}
    }

    /* get rule bag, and evaluate each rule */
    if (ohGetByBagName(&techPtr->facet, &ruleBag, TAP_RULE_BAG_NAME) < OCT_OK) {
	/* no design rules */
	return;
    }
    OCT_ASSERT(octInitGenContents(&ruleBag, OCT_BAG_MASK, &gen));
    while ((status = octGenerate(&gen, &rule)) == OCT_OK) {
	(void) tapEvaluateDesignRule(&rule, NIL(char));
    }
    OCT_ASSERT(status);
}

static char *ruleSenseName[] =
    {	"minimum",	"maximum" };
static char *ruleTypeName[] =
    {	"width",	"spacing",	"overlap",	"overlap" };
static char *ruleTypePreposition[] =
    {	" ",		" to ",		" over ",	" under " };

octCoord tapEvaluateDesignRule(rule, printString)
octObject *rule;
char *printString;
{
    octObject edge1, edge2;
    octObject layer1, layer2;
    octGenerator gen;
    int ruleSense, rType;
    octCoord rDistance;
    int (*fnPtr) ();
    octCoord ruleDistance();
    void checkEdgesParallel();
    struct tapLayer *tapInternLayer();

    TAP_START("tapEvaluateDesignRule");

    /* Determine rule type by name of bag */
    if (strcmp(rule->contents.bag.name, TAP_MIN_RULE_NAME) == 0) {
	ruleSense = MIN_RULE;
    } else if (strcmp(rule->contents.bag.name, TAP_MAX_RULE_NAME) == 0) {
	ruleSense = MAX_RULE;
    } else {
	/* something we don't understand -- ignore it */
	if (printString) {
	    (void) strcpy(printString, "unknown rule type");
	}
	TAP_END();
	return(TAP_NO_RULE);
    }

    /* Get the edges and their layers */
    OCT_ASSERT(octInitGenContents(rule, OCT_EDGE_MASK, &gen));
    if (octGenerate(&gen, &edge1) != OCT_OK ||
				    octGenerate(&gen, &edge2) != OCT_OK) {
	/* something we don't understand -- ignore it */
	if (printString) {
	    (void) strcpy(printString, "unknown rule type");
	}
	TAP_END();
	return(TAP_NO_RULE);
    }
    OCT_ASSERT(octFreeGenerator(&gen));
    if (octGenFirstContainer(&edge1, OCT_LAYER_MASK, &layer1) != OCT_OK ||
	    octGenFirstContainer(&edge2, OCT_LAYER_MASK, &layer2) != OCT_OK) {
	errRaise(TAP_PKG_NAME, 0, "rule edge in technology cell not on layer");
    }
    
    checkEdgesParallel(&edge1, &edge2);

    rDistance = ruleDistance(&edge1, &edge2);
    rType = ruleType(&edge1, &edge2);

    if (rType == UNDERLAP) {
	octObject temp;

	temp = layer1;
	layer1 = layer2;
	layer2 = temp;
	rType = OVERLAP;
    }

    if (printString) {
	(void) sprintf(printString, "%s %s rule, %s",
		    ruleSenseName[ruleSense],
		    ruleTypeName[rType],
		    layer1.contents.layer.name);
	if (!octIdsEqual(layer1.objectId, layer2.objectId)) {
	    (void) strcat(printString, ruleTypePreposition[rType]);
	    (void) strcat(printString, layer2.contents.layer.name);
	}
    } else {
	fnPtr = ruleEvaluators[ruleSense][rType];
	if (fnPtr == 0) {
	    errRaise(TAP_PKG_NAME, 0, "unknown rule type: %s %s",
		    ruleSenseName[ruleSense], ruleTypeName[rType]);
	}
	(*fnPtr) (rDistance, tapInternLayer(&layer1), tapInternLayer(&layer2));
    }

    TAP_END();
    return(rDistance);
}

/*
 *	Check that the edges pointed to by edge1 and edge2 are parallel
 * and Manhattan.  Anything else is an error (this way, these routines
 * will be updated if anyone non-Manhattan rules that they should
 * understand at all).
 */
static void checkEdgesParallel(edge1, edge2)
octObject *edge1, *edge2;
{
    if (edge1->contents.edge.start.x == edge1->contents.edge.end.x) {
	if (edge2->contents.edge.start.x != edge2->contents.edge.end.x) {
	    errRaise(TAP_PKG_NAME, 0,
			"non-parallel rule edges in technology cell");
	}
    } else if (edge1->contents.edge.start.y == edge1->contents.edge.end.y) {
	if (edge2->contents.edge.start.y != edge2->contents.edge.end.y) {
	    errRaise(TAP_PKG_NAME, 0,
			"non-parallel rule edges in technology cell");
	}
    } else {
	errRaise(TAP_PKG_NAME, 0,
			"non-Manhattan rule edges in technology cell");
    }
}

#define SUBTRACT_POINTS(res, sub, min)	\
		( (res).x = (sub).x - (min).x, (res).y = (sub).y - (min).y )
#define dotProd(v1, v2)		( (v1).x*(v2).x + (v1).y*(v2).y )
#define crossProd(v1, v2)	( (v1).x*(v2).y - (v1).y*(v2).x )

/*
 *	This function figures out whether the rule is a width, spacing,
 * overlap, or underlap rule.  The two edges are known to be parallel,
 * so all we have to check is whether the vectors v1 and v2 representing
 * the directed edges are parallel or antiparallel (check v1 dot v2),
 * and whether the second is to the left or right of the first (check
 * v1 cross v3, where v3 is the vector from the tail of v1 to the tail
 * of v2).  This routine doesn't require Manhattan edges.
 */
static int ruleType(edge1, edge2)
octObject *edge1, *edge2;
{
    struct octPoint v1, v2, v3;

    SUBTRACT_POINTS(v1, edge1->contents.edge.end, edge1->contents.edge.start);
    SUBTRACT_POINTS(v2, edge2->contents.edge.end, edge2->contents.edge.start);
    SUBTRACT_POINTS(v3, edge2->contents.edge.start, edge1->contents.edge.start);

    if (dotProd(v1, v2) < 0) {
	/* width or spacing */
	if (crossProd(v1, v3) > 0) {
	    return(WIDTH);
	} else {
	    return(SPACING);		/* zero-value rule must be spacing */
	}
    } else {
	/* overlap or underlap */
	if (crossProd(v1, v3) < 0) {
	    return(UNDERLAP);
	} else {
	    return(OVERLAP);
	}
    }
}

/*
 *	Return the distance between the edges pointed to by the arguments.
 * This is very easy because we already checked that these edges are
 * parallel and Manhattan.
 */
static octCoord ruleDistance(edge1, edge2)
octObject *edge1, *edge2;
{
    if (edge1->contents.edge.start.x == edge1->contents.edge.end.x) {
	return(abs(edge1->contents.edge.start.x-edge2->contents.edge.start.x));
    } else {
	return(abs(edge1->contents.edge.start.y-edge2->contents.edge.start.y));
    }
}

static int minWidthRule(distance, layerS1, layerS2)
octCoord distance;
struct tapLayer *layerS1, *layerS2;
{
    if (layerS1 != layerS2) {
	errRaise(TAP_PKG_NAME, 0,
		"technology cell describes a two-layer width rule");
    }
    if (layerS1->desRulePtr->minWidth == TAP_NO_RULE ||
				layerS1->desRulePtr->minWidth < distance) {
	layerS1->desRulePtr->minWidth = distance;
    }
    return 1;			/* We pass this func around as a func.
				   ptr to a func that returns an int */
}

static int maxWidthRule(distance, layerS1, layerS2)
octCoord distance;
struct tapLayer *layerS1, *layerS2;
{
    if (layerS1 != layerS2) {
	errRaise(TAP_PKG_NAME, 0,
		"technology cell describes a two-layer width rule");
    }
    if (layerS1->desRulePtr->maxWidth == TAP_NO_RULE ||
				layerS1->desRulePtr->maxWidth > distance) {
	layerS1->desRulePtr->maxWidth = distance;
    }
    return 1;			/* We pass this func around as a func.
				   ptr to a func that returns an int */
}

static int minSpacingRule(distance, layerS1, layerS2)
octCoord distance;
struct tapLayer *layerS1, *layerS2;
{
    if (layerS1->desRulePtr->minSpacing[layerS2->layerIndex] == TAP_NO_RULE ||
	    layerS1->desRulePtr->minSpacing[layerS2->layerIndex] < distance) {
	layerS1->desRulePtr->minSpacing[layerS2->layerIndex] = distance;
	layerS2->desRulePtr->minSpacing[layerS1->layerIndex] = distance;
    }
    return 1;			/* We pass this func around as a func.
				   ptr to a func that returns an int */
}

static int minOverlapRule(distance, layerS1, layerS2)
octCoord distance;
struct tapLayer *layerS1, *layerS2;
{
    if (layerS1->desRulePtr->minOverlap[layerS2->layerIndex] == TAP_NO_RULE ||
	    layerS1->desRulePtr->minOverlap[layerS2->layerIndex] < distance) {
	layerS1->desRulePtr->minOverlap[layerS2->layerIndex] = distance;
    }
    return 1;			/* We pass this func around as a func.
				   ptr to a func that returns an int */
}
