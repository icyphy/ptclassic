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
#include "ansi.h"
#include "tap_int.h"

/*LINTLIBRARY*/

static void incorporateWidth
	ARGS((struct connLayer *layerDescPtr, octCoord width, octCoord
	      angle, int direction));
static int initConnectors
	ARGS((struct tapTech *techPtr));
static int tryConnector
	ARGS((struct tapConnector *connPtr, int layerCount, struct connLayer *neededLayers, octObject *instPtr));
static int tabulateConnector
	ARGS((struct tapTech *techPtr, struct tapConnector *connPtr, octObject *instancePtr, octObject *palettePtr));
static void interpretConnector
	ARGS((struct tapTech *techPtr, octObject *instancePtr, octObject *palettePtr));
static int addImplBoxToConn
	ARGS((struct connLayer *layerDescs, octObject *implBoxPtr,
	      octObject *techFacetPtr));
static void cacheConnectorInfo
	ARGS((struct tapTech *techPtr, octObject *instancePtr,
	      octObject *palettePtr, double area, struct connLayer *layerDescs));


static void sortConnectors
	ARGS((struct tapTech *techPtr));
static int connCompare
	ARGS((struct tapConnector *a, struct tapConnector *b));

int tapOpenPalette(objPtr, name, masterPtr, mode)
octObject *objPtr;
char *name;
octObject *masterPtr;
char *mode;
{
    struct tapTech *techPtr;
    octObject bag, instance, bigBag;
    char *dir, *fullName, *nameTail;
    octStatus stat;

    TAP_START("tapOpenPalette");

    /* get the technology facet for this layer */
    techPtr = tapInternTech(objPtr);

    /* find existing palette instance */
    OCT_ASSERT(ohGetOrCreateBag(&techPtr->facet, &bag, PALETTE_BAG_NAME));
    if (ohGetByInstName(&bag, &instance, name) == OCT_OK) {
	if ( strcmp( name, "connector" ) == 0 ) {
	    if ( ohOpenMaster(masterPtr, &instance, "interface", mode) >= OCT_OK ) {
		if (ohGetByBagName(masterPtr,&bigBag,"SORTED_CONNECTORS")==OCT_OK) {
		    return 1;	/*  */
		} 
	    }
	}
	OCT_ASSERT(ohOpenMaster(masterPtr, &instance, "contents", mode));
	TAP_END();
	return 1;
    }

    /* XXX -- for backward compatibility, check old format */
    printf( "Warning: Tap.views is obsolete\n" );
    fullName = ALLOC(char, strlen(name) + strlen("-palette") + 1);
    (void) sprintf(fullName, "%s-palette", name);
    stat = ohGetByInstName(&techPtr->facet, &instance, fullName);
    FREE(fullName);
    if (stat == OCT_OK) {
	OCT_ASSERT(ohOpenMaster(masterPtr, &instance, "contents", mode));
	TAP_END();
	return(1);
    }

    /* palette doesn't exist (yet) */
    if (mode[0] == 'r') return(0);
    dir = tapGetDirectory(objPtr);
    fullName = ALLOC(char, strlen(dir) + strlen("tap.views") +
			    strlen(name) + strlen("-palette") + 3);
    (void) sprintf(fullName, "%s/tap.views/", dir);
    FREE(dir);
    nameTail = fullName + strlen(fullName);
    (void) sprintf(nameTail, "%s-palette", name);
    OCT_ASSERT(ohOpenFacet(masterPtr, fullName,
			techPtr->facet.contents.facet.view, "contents", mode));
    FREE(fullName);

    OCT_ASSERT(ohCreateInst(&bag, &instance, name, nameTail,
				techPtr->facet.contents.facet.view));
    OCT_ASSERT(octAttach(&techPtr->facet, &instance));

    TAP_END();
    return(1);
}

void tapListPalettes(objPtr, numPtr, arrayPtr)
octObject *objPtr;
int *numPtr;
char ***arrayPtr;
{
    struct tapTech *techPtr;
    octObject bag, instance;
    octGenerator gen;
    octStatus stat;

    TAP_START("tapListPalettes");

    /* get the palette bag */
    techPtr = tapInternTech(objPtr);
    OCT_ASSERT(ohGetOrCreateBag(&techPtr->facet, &bag, PALETTE_BAG_NAME));

    /* get the names */
    OCT_ASSERT(octInitGenContents(&bag, OCT_INSTANCE_MASK, &gen));
    *arrayPtr = NIL(char *);
    *numPtr = 0;
    while ((stat = octGenerate(&gen, &instance)) == OCT_OK) {
	*arrayPtr = REALLOC(char *, *arrayPtr, ++*numPtr);
	(*arrayPtr)[*numPtr - 1] = instance.contents.instance.name;
    }
    OCT_ASSERT(stat);

    TAP_END();
}


static int (*userConnectorGenerator)() = 0;


void
tapSetUserConnectorGenerator(funct)
int (*funct)();
{
    userConnectorGenerator = funct;
    return;
}

int tapGetConnector(nLayers, layerList, instPtr)
int nLayers;
tapLayerListElement layerList[];
octObject *instPtr;
{
    struct tapTech *techPtr;
    struct tapLayer *layerS;
    struct connLayer *neededLayers;
    struct tapConnector *connPtr;
    struct tapConnector *selectedConnPtr = 0;
    int    largerConn;
    int i, success;
    int initConnectors(), tryConnector();
    octObject selectedInstance;
    int actualLayersInConn = 0;
    void incorporateWidth();

    TAP_START("tapGetConnector");

    if (nLayers < 1) {
	errRaise(TAP_PKG_NAME, 0,
		    "Need at least one layer joining at connector");
    }

    /* get technology with connector list */
    techPtr = tapInternTech(&layerList[0].layer);
    if (techPtr->connList == NIL(struct tapConnector)) {
	if ( ! initConnectors(techPtr)) return(0);
    }

    neededLayers = ALLOC(struct connLayer, techPtr->layerCount);
    for (i = 0; i < techPtr->layerCount; i++) {
	neededLayers[i].widthArray[0] =
		neededLayers[i].widthArray[1] =
		neededLayers[i].widthArray[2] =
		neededLayers[i].widthArray[3] = -1;
	neededLayers[i].isUsed = 0;
    }

    /* build table of what layer sizes we need */
    for (i = 0; i < nLayers; i++) {
	layerS = tapInternLayer(&layerList[i].layer);
	if (layerS->isConnector) {
	    continue;
	}
	if (layerS->techPtr != techPtr) {
	    errRaise(TAP_PKG_NAME, 0,
		     "layers ``%s'' and ``%s'' joining at connector have different technologies",
		     layerList[0].layer.contents.layer.name,
		     layerList[i].layer.contents.layer.name);
	}
	incorporateWidth(&neededLayers[layerS->layerIndex], layerList[i].width,
				layerList[i].direction, layerList[i].angle);
    }

    /*
     * Find first adequate connector -- it is the best since they're sorted.
     * Check all connectors of the same size, once you find one that works. Choose the
     * one with fewer layers. 
     */
    success = 0;
    for (connPtr = techPtr->connList; connPtr; connPtr = connPtr->next) {
	if( (largerConn = tryConnector(connPtr, techPtr->layerCount,
				       neededLayers, instPtr)) ) {
	    if ((largerConn<0) && userConnectorGenerator) {
		break;
	    }
	    
	    if ( selectedConnPtr ) {
		if ( connPtr->area > selectedConnPtr->area )  break;	/* Connector is larger than the one we have found */
		if ( connPtr->actualLayers >= actualLayersInConn ) continue;
		/* printf( "Selecting a connector with fewer layers\n" ); */
	    } 
	    selectedConnPtr = connPtr;
	    selectedInstance = *instPtr;
	    actualLayersInConn = connPtr->actualLayers;
	    success = 1;
	}
    }

    if ( success && connPtr != selectedConnPtr ) {
	*instPtr = selectedInstance;
    }


    FREE(neededLayers);
    TAP_END();

    if (!success && userConnectorGenerator) {
	success = (*userConnectorGenerator)(nLayers, layerList, instPtr);
    }
    
    return(success);
}


/*ARGSUSED*/
static void incorporateWidth(layerDescPtr, width, direction, angle)
struct connLayer *layerDescPtr;
octCoord width, angle;
int direction;
{
    layerDescPtr->isUsed = 1;

    if (direction & TAP_LEFT) {
	if (width > layerDescPtr->widthArray[0]) {
	    layerDescPtr->widthArray[0] = width;
	}
    }

    if (direction & TAP_TOP) {
	if (width > layerDescPtr->widthArray[1]) {
	    layerDescPtr->widthArray[1] = width;
	}
    }

    if (direction & TAP_RIGHT) {
	if (width > layerDescPtr->widthArray[2]) {
	    layerDescPtr->widthArray[2] = width;
	}
    }

    if (direction & TAP_BOTTOM) {
	if (width > layerDescPtr->widthArray[3]) {
	    layerDescPtr->widthArray[3] = width;
	}
    }

    if (direction & TAP_WEIRD_DIR) {
	errRaise(TAP_PKG_NAME, 0,
		    "nonmanhattan connectors not supported (yet?)");
    }
}

static int tryConnector(connPtr, layerCount, neededLayers, instPtr)
struct tapConnector *connPtr;
int layerCount;
struct connLayer *neededLayers;
octObject *instPtr;
/*  Return 0 if no connectors were found, 1 if one was found with the exact size
 *  and (-1) if a larger one was found.  */
{
    static octTransformType transTab[] = {
			    OCT_NO_TRANSFORM,	OCT_ROT90,
			    OCT_ROT180,		OCT_ROT270 };
    register int rotation, direction, layerIndex;
    int     larger;
    register octCoord *widthArray;

    for (rotation = 0; rotation < 4; rotation++) {
	larger = 0;
	for (layerIndex = 0; layerIndex < layerCount; layerIndex++) {
	    if (neededLayers[layerIndex].isUsed) {
		if ( ! connPtr->layerDescList[layerIndex].isUsed) {
		    goto badConnector;
		}
		widthArray = connPtr->layerDescList[layerIndex].widthArray;
		for (direction = 0; direction < 4; direction++) {
		    if (widthArray[direction] <
			neededLayers[layerIndex].widthArray[(direction+rotation) % 4]) {
			goto badConnector;
		    } else if(widthArray[direction] >
			      neededLayers[layerIndex].widthArray[(direction+rotation) % 4]) {
			larger = (neededLayers[layerIndex].widthArray[(direction+rotation) %4] > 0);
		    }
		}
	    }
	}
	/* found one */
	instPtr->type = OCT_INSTANCE;
	instPtr->contents.instance.master = connPtr->instance.contents.instance.master;
	instPtr->contents.instance.view = connPtr->instance.contents.instance.view;
	instPtr->contents.instance.facet = connPtr->instance.contents.instance.facet;
	instPtr->contents.instance.version = connPtr->instance.contents.instance.version;
	instPtr->contents.instance.transform.transformType = transTab[rotation];
	return ((larger) ? (-1) : 1);
	
badConnector: ;
    }

    return(0);
}

void tapFreeConnTable(techPtr)
struct tapTech *techPtr;
{
    struct tapConnector *connPtr, *tempConnPtr;

    for (connPtr = techPtr->connList; connPtr; ) {
	tempConnPtr = connPtr;
	connPtr = tempConnPtr->next;
	FREE(tempConnPtr->layerDescList);
	FREE(tempConnPtr->instance.contents.instance.master);
	FREE(tempConnPtr->instance.contents.instance.view);
	FREE(tempConnPtr->instance.contents.instance.version);
	OCT_ASSERT(octCloseFacet(&tempConnPtr->master));
	FREE(tempConnPtr);
    }
}

static int initConnectors(techPtr)
struct tapTech *techPtr;
{
    octObject palette;
    octGenerator instGen;
    octStatus genStatus;
    octObject instance;
    extern struct tapConnector *fastTapInitPalette();
    struct tapConnector *connPtr;
    struct tapConnector *connList = NIL(struct tapConnector);

    if ( ! tapOpenPalette(&techPtr->facet, TAP_CONNECTOR_PALETTE_NAME,
							    &palette, "r")) {
	return(0);
    }

    connList = fastTapInitPalette( techPtr, &palette );
    if (  connList == NIL(struct tapConnector) ) {
	OCT_ASSERT(octInitGenContents(&palette, OCT_INSTANCE_MASK, &instGen));
	while ((genStatus = octGenerate(&instGen, &instance)) == OCT_OK) {
	    connPtr = ALLOC(struct tapConnector, 1);

	    if (tabulateConnector(techPtr, connPtr, &instance, &palette)) {
		connPtr->next = connList;
		connList = connPtr;
	    } else {
		FREE(connPtr);
	    }
	}
	OCT_ASSERT(genStatus);
    }

    techPtr->connList = connList;
    sortConnectors(techPtr);

    OCT_ASSERT(octCloseFacet(&palette));
    return(1);
}

static int tabulateConnector(techPtr, connPtr, instancePtr, palettePtr)
struct tapTech *techPtr;
struct tapConnector *connPtr;
octObject *instancePtr;
octObject *palettePtr;
{
    octObject bag;
    octObject prop;
    octObject layer;
    octGenerator gen;
    octStatus stat;
    int i;
    void interpretConnector();

    if (ohGetByBagName(instancePtr, &bag, CONN_BAG_NAME) != OCT_OK) {
	interpretConnector(techPtr, instancePtr, palettePtr);
	if (octGetByName(instancePtr, &bag) != OCT_OK) {
	    /* oh, well.  we couldn't get any info */
	    return(0);
	}
    }
    if (ohGetByPropName(&bag, &prop, CONN_AREA_PROP_NAME) != OCT_OK ||
				    prop.contents.prop.type != OCT_REAL)  {
	return(0);
    }
    connPtr->area = prop.contents.prop.value.real;

    connPtr->instance.contents.instance.name = "";
    connPtr->instance.contents.instance.master =
		    util_strsav(instancePtr->contents.instance.master);
    connPtr->instance.contents.instance.view =
		    util_strsav(instancePtr->contents.instance.view);
    connPtr->instance.contents.instance.facet =
		    util_strsav(instancePtr->contents.instance.facet);
    connPtr->instance.contents.instance.version =
		    util_strsav(instancePtr->contents.instance.version);
    connPtr->instance.contents.instance.transform =
		    instancePtr->contents.instance.transform;
    connPtr->nLayers = 0;

    /* allocate new connector structure */
    connPtr->layerDescList = ALLOC(struct connLayer, techPtr->layerCount);
    for (i = 0; i < techPtr->layerCount; i++) {
	connPtr->layerDescList[i].widthArray[0] =
		connPtr->layerDescList[i].widthArray[1] =
		connPtr->layerDescList[i].widthArray[2] =
		connPtr->layerDescList[i].widthArray[3] = -1;
	connPtr->layerDescList[i].isUsed = 0;
    }

    OCT_ASSERT(octInitGenContents(&bag, OCT_PROP_MASK, &gen));
    while ((stat = octGenerate(&gen, &prop)) == OCT_OK) {
	if (strcmp(prop.contents.prop.name, CONN_LAYER_PROP_NAME) == 0) {
	    if (prop.contents.prop.type != OCT_INTEGER_ARRAY ||
		    prop.contents.prop.value.integer_array.length != 4 ||
		    octGenFirstContainer(&prop, OCT_LAYER_MASK, &layer) != OCT_OK) {
		errRaise(TAP_PKG_NAME, 0,
"corrupt %s palette -- bogus ``%s'' property for %s connector",
			TAP_CONNECTOR_PALETTE_NAME, CONN_LAYER_PROP_NAME,
			instancePtr->contents.instance.master);
	    }

	    if (tapInternLayer(&layer)->isConnector) {
		continue;
	    }

	    i = tapInternLayer(&layer)->layerIndex;
	    connPtr->layerDescList[i].widthArray[0] =
			prop.contents.prop.value.integer_array.array[0];
	    connPtr->layerDescList[i].widthArray[1] =
			prop.contents.prop.value.integer_array.array[1];
	    connPtr->layerDescList[i].widthArray[2] =
			prop.contents.prop.value.integer_array.array[2];
	    connPtr->layerDescList[i].widthArray[3] =
			prop.contents.prop.value.integer_array.array[3];
	    connPtr->layerDescList[i].isUsed = 1;
	    connPtr->nLayers++;
	}
    }
    OCT_ASSERT(stat);
    return(1);
}

static void interpretConnector(techPtr, instancePtr, palettePtr)
struct tapTech *techPtr;
octObject *instancePtr;
octObject *palettePtr;
{
    octObject master;
    octGenerator implGen;
    struct octBox bBox;
    octObject implGeo;
    octStatus genStatus;
    octObject term;
    struct connLayer *layerDescs;
    double area;
    int i;
    void cacheConnectorInfo();

    if (ohOpenMaster(&master, instancePtr, "contents", "r") < OCT_OK) {
	/* should be an error, but what a pain... */
	return;
    }
    if (octGenFirstContent(&master, OCT_TERM_MASK, &term) != OCT_OK ||
					    octBB(&master, &bBox) != OCT_OK) {
	/* bogus connector */
	return;
    }

    area = (double) (bBox.upperRight.x - bBox.lowerLeft.x) *
		    (double) (bBox.upperRight.y - bBox.lowerLeft.y);

    layerDescs = ALLOC(struct connLayer, techPtr->layerCount);
    for (i = 0; i < techPtr->layerCount; i++) {
	layerDescs[i].widthArray[0] =
		layerDescs[i].widthArray[1] =
		layerDescs[i].widthArray[2] =
		layerDescs[i].widthArray[3] = -1;
	layerDescs[i].isUsed = 0;
    }

    /* examine terminal implementation geometries */
    OCT_ASSERT(octInitGenContents(&term, OCT_BOX_MASK, &implGen));
    while ((genStatus = octGenerate(&implGen, &implGeo)) == OCT_OK) {
	if ( ! addImplBoxToConn(layerDescs, &implGeo, &techPtr->facet)) {
	    FREE(layerDescs);
	    OCT_ASSERT(octFreeGenerator(&implGen));
	    return;
	}
    }
    OCT_ASSERT(genStatus);

    OCT_ASSERT(octCloseFacet(&master));

    cacheConnectorInfo(techPtr, instancePtr, palettePtr, area, layerDescs);
    FREE(layerDescs);
}

static int addImplBoxToConn(layerDescs, implBoxPtr, techFacetPtr)
struct connLayer *layerDescs;	/* layer width array to be added to */
octObject *implBoxPtr;		/* implementing box on connector terminal */
octObject *techFacetPtr;	/* pointer to desired technology facet */
{
    octObject layer;
    struct connLayer *layerDescPtr;
    void incorporateWidth();
    struct octPoint *llPtr = &implBoxPtr->contents.box.lowerLeft,
			*urPtr = &implBoxPtr->contents.box.upperRight;

    if (octGenFirstContainer(implBoxPtr, OCT_LAYER_MASK, &layer) != OCT_OK) {
	/* bogus implementation box */
	return(0);
    }
    if (octGetByName(techFacetPtr, &layer) != OCT_OK) {
	/* layer in connector doesn't exist in desired technology */
	return(0);
    }

    if (tapInternLayer(&layer)->isConnector) {
	/* just ignore it */
	return(1);
    }

    layerDescPtr = &layerDescs[tapInternLayer(&layer)->layerIndex];
    if (llPtr->x + urPtr->x >= 0) {
	/* allowed to connect from the right */
	incorporateWidth(layerDescPtr,
		2 * MIN(-llPtr->y, urPtr->y), TAP_RIGHT, (octCoord) 0);
    }
    if (llPtr->x + urPtr->x <= 0) {
	/* allowed to connect from the left */
	incorporateWidth(layerDescPtr,
		2 * MIN(-llPtr->y, urPtr->y), TAP_LEFT, (octCoord) 0);
    }
    if (llPtr->y + urPtr->y >= 0) {
	/* allowed to connect from the top */
	incorporateWidth(layerDescPtr,
		2 * MIN(-llPtr->x, urPtr->x), TAP_TOP, (octCoord) 0);
    }
    if (llPtr->y + urPtr->y <= 0) {
	/* allowed to connect from the bottom */
	incorporateWidth(layerDescPtr,
		2 * MIN(-llPtr->x, urPtr->x), TAP_BOTTOM, (octCoord) 0);
    }

    return(1);
}

static void cacheConnectorInfo(techPtr, instancePtr, palettePtr, area, layerDescs)
struct tapTech *techPtr;
octObject *instancePtr;
octObject *palettePtr;
double area;
struct connLayer *layerDescs;
{
    octObject bag;
    octObject prop;
    octObject layer;
    int i;

    OCT_ASSERT(ohCreateBag(instancePtr, &bag, CONN_BAG_NAME));
    OCT_ASSERT(ohCreatePropReal(&bag, &prop, CONN_AREA_PROP_NAME, area));

    prop.contents.prop.name = CONN_LAYER_PROP_NAME;
    prop.contents.prop.type = OCT_INTEGER_ARRAY;
    prop.contents.prop.value.integer_array.length = 4;
    for (i = 0; i < techPtr->layerCount; i++) {
	if (layerDescs[i].isUsed) {
	    prop.contents.prop.value.integer_array.array =
						    layerDescs[i].widthArray;
	    layer = techPtr->layerList[i].techLayer;
	    OCT_ASSERT(octCreate(&bag, &prop));
	    OCT_ASSERT(octGetOrCreate(palettePtr, &layer));
	    OCT_ASSERT(octAttach(&layer, &prop));
	}
    }
}

#define TYPE	struct tapConnector
#define NEXT	next
#define SORT	doInternalSort
#define SORT1	doSort
#include "lsort.h"

static void sortConnectors(techPtr)
struct tapTech *techPtr;
{
    int connCompare();

    techPtr->connList = doSort(techPtr->connList, connCompare);
}

static int connCompare(a, b)
struct tapConnector *a, *b;
{
    if (a->nLayers < b->nLayers) return(-1);
    if (a->nLayers > b->nLayers) return(1);
    if (a->area < b->area) return(-1);
    if (a->area > b->area) return(1);
    return(0);
}
