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
#include <ctype.h>
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "st.h"
#include "desrule.h"
#include "tap_int.h"

int tapPassedStatus;
char tapPkgName[] = "tap";
int tapDepth = 0;
char *tapRoutine;

/* Linked list of technology/view names */
    /* A linked list is fine for the number of technologies we expect */
static struct tapTechName *nameList = NIL(struct tapTechName);

/* Linked list of all technologies (walk this to flush info) */
static struct tapTech *techList = NIL(struct tapTech);

/* Hash tables from user's facet or layer to tap facet or layer */
static st_table *techHash = NIL(st_table);
static st_table *layerHash = NIL(st_table);

/*LINTLIBRARY*/

static char *rootDirName = NIL(char);

/* Forward declaration of static functions. */

static struct tapTech *initTechFacetFromName();
static struct tapTech *initTechFacet();
static void initLayers();




/* -------------------------------------------------------------------------- */

char *tapRootDirectory(newName)
char *newName;
{
    TAP_START("tapRootDirectory");

    if (newName) {
	tapFlushCache();
	FREE(rootDirName);
	rootDirName = util_strsav(newName);
    } else if (rootDirName == NIL(char)) {
	rootDirName = util_strsav(tapGetDefaultRoot());
    }

    TAP_END();
    return(rootDirName);
}

/* useful for debugging */
#ifndef CADROOT
#define CADROOT "$OCTTOOLS"
#endif

char *tapGetDefaultRoot()
{
    /* boy, I wish I could depend on ANSI string concatenation */
    static char *defaultRoot = NIL(char);
    static char *prefix = CADROOT;
    static char *suffix = "/tech";

    TAP_START("tapGetDefaultRoot");

    if (defaultRoot == NIL(char)) {
	defaultRoot = ALLOC(char, strlen(prefix) + strlen(suffix) + 1);
	(void) strcat(strcpy(defaultRoot, prefix), suffix);
    }

    TAP_END();
    return(defaultRoot);
}

char *tapGetDirectory(objPtr)
octObject *objPtr;
{
    struct tapTech *techPtr;
    char *dirStr, *endPtr;

    TAP_START("tapGetDirectory");

    techPtr = tapInternTech(objPtr);
    dirStr = util_strsav(techPtr->facet.contents.facet.cell);
    endPtr = strrchr(dirStr, '/');
    if (endPtr == NIL(char)) {
	FREE(dirStr);
	errRaise(TAP_PKG_NAME, 0,
"can't determine technology directory:  ``%s'' contains own technology",
		techPtr->facet.contents.facet.cell);
    }
    *endPtr = '\0';

    TAP_END();
    return(dirStr);
}

int tapIsLayerDefined(objPtr, name)
octObject *objPtr;
char *name;
{
    struct tapTech *techPtr;
    octObject layer;
    octStatus status;

    TAP_START("tapIsLayerDefined");

    techPtr = tapInternTech(objPtr);

    status = ohGetByLayerName(&techPtr->facet, &layer, name);
    if (status != OCT_NOT_FOUND) OCT_ASSERT(status);

    TAP_END();
    return(status == OCT_OK);
}

octId tapGetFacetIdFromObj(objPtr)
octObject *objPtr;
{
    octId id;

    TAP_START("tapGetFacetIdFromObj");

    id = tapInternTech(objPtr)->facet.objectId;

    TAP_END();
    return(id);
}

#define TECH	1
#define VIEW	2

struct tapTech *tapInternTech(objPtr)
octObject *objPtr;
{
    octObject facet;
    struct tapTech *techPtr;
    char *dummyPtr;
    char *techName;
    char *viewName;
    octObject prop;
    int propsMissing = 0;
    char *pkgName, *message;
    int code;
    struct tapTech *initTechFacet(), *initTechFacetFromName();
    extern int octIdCmp(), octIdHash();
    octId *id;

    /* get the object's facet */
    octGetFacet(objPtr, &facet);

    if (techHash == NIL(st_table)) {
	techHash = st_init_table(octIdCmp, octIdHash);
    }

    /* maybe facet is in the hash table already */
    if (st_lookup(techHash, (char *) &facet.objectId, &dummyPtr)) {
	return((struct tapTech *) dummyPtr);
    }

    /* get the technology name */
    if (ohGetByPropName(&facet, &prop, TECH_PROP_NAME) == OCT_OK) {
	if (prop.contents.prop.type == OCT_STRING) {
	    techName = prop.contents.prop.value.string;
	    if (strcmp(techName, TAP_TECH_PROP_VAL) == 0) {
		/* this IS a tap facet -- open for us and put into hash */
		facet.contents.facet.mode = "r";
		if (octOpenFacet(&facet) < OCT_OK) {
		    errRaise(TAP_PKG_NAME, 0,
			"can't open technology facet: %s\n", octErrorString());
		}
		return(initTechFacet(&facet));
	    }
	} else {
	    errRaise(TAP_PKG_NAME, 0, "cell ``%s'' has invalid %s property",
		    facet.contents.facet.cell, TECH_PROP_NAME);
	}
    } else {
	techName = DEFAULT_TAP_NAME;
	propsMissing |= TECH;
    }

    /* get the view name */
    if (ohGetByPropName(&facet, &prop, VIEW_PROP_NAME) == OCT_OK) {
	if (prop.contents.prop.type == OCT_STRING) {
	    viewName = prop.contents.prop.value.string;
	} else {
	    errRaise(TAP_PKG_NAME, 0, "cell ``%s'' has invalid %s property",
		    facet.contents.facet.cell, VIEW_PROP_NAME);
	}
    } else {
	viewName = facet.contents.facet.view;
	propsMissing |= VIEW;
    }

    ERR_IGNORE(techPtr = initTechFacetFromName(techName, viewName));
    if (errStatus(&pkgName, &code, &message)) {
	switch (propsMissing) {
	    case TECH|VIEW:
		errRaise(TAP_PKG_NAME, 0,
			"cell ``%s'' has no %s or %s property",
			facet.contents.facet.cell,
			TECH_PROP_NAME, VIEW_PROP_NAME);
	    case TECH:
		errRaise(TAP_PKG_NAME, 0, "cell ``%s'' has no %s property",
			facet.contents.facet.cell, TECH_PROP_NAME);
	    case VIEW:
		errRaise(TAP_PKG_NAME, 0, "cell ``%s'' has no %s property",
			facet.contents.facet.cell, VIEW_PROP_NAME);
	    default:
		errRaise(pkgName, code, message);
	}
    }

    id = ALLOC(octId, 1);
    *id = facet.objectId;
    (void) st_insert(techHash, (char *) id, (char *) techPtr);

    TAP_END();
    return(techPtr);
}

static struct tapTech *initTechFacetFromName(name, view)
char *name;
char *view;
{
    struct tapTechName *namePtr;
    octObject facet;
    struct tapTech *techPtr;
    char *dummyPtr;
    struct tapTech *initTechFacet();

    /* look for technology in list of ones we've seen */
    for (namePtr = nameList; namePtr != NIL(struct tapTechName);
						namePtr = namePtr->next) {
	if (strcmp(name, namePtr->name) == 0 &&
			    strcmp(view, namePtr->view) == 0) {
	    return(namePtr->techPtr);
	}
    }

    /* either initialize new technology or find it (with another name) */
    tapOpenTechFacet(name, view, &facet, "r");
    if (st_lookup(techHash, (char *) &facet.objectId, &dummyPtr)) {
	techPtr = (struct tapTech *) dummyPtr;
	OCT_ASSERT(octCloseFacet(&facet));
    } else {
	techPtr = initTechFacet(&facet);
    }

    /* new technology (name) -- add new structure to the list */
    namePtr = ALLOC(struct tapTechName, 1);
    namePtr->name = util_strsav(name);
    namePtr->view = util_strsav(view);
    namePtr->techPtr = techPtr;
    namePtr->next = nameList;
    nameList = namePtr;

    return(techPtr);
}

void tapOpenTechFacet(name, view, facetPtr, mode)
char *name;
char *view;
octObject *facetPtr;
char *mode;
{
    char *cellStr;
    char *viewStr;
    char *cPtr;

    TAP_START("tapOpenTechFacet");

    /* assure that tech directory is initialized */
    (void) tapRootDirectory(NIL(char));

    /* create technology cell name */
    cellStr = ALLOC(char, strlen(TAP_CELL_NAME) + strlen(rootDirName) +
				strlen(name) + 1);
    (void) sprintf(cellStr, TAP_CELL_NAME, rootDirName, name);

    /* create technology view name */
    viewStr = util_strsav(view);
    for (cPtr = viewStr; *cPtr; cPtr++) {
	if (isupper(*cPtr)) *cPtr = tolower(*cPtr);
    }

    /* open technology facet */
    facetPtr->type = OCT_FACET;
    facetPtr->contents.facet.cell = cellStr;
    facetPtr->contents.facet.view = viewStr;
    facetPtr->contents.facet.facet = TAP_FACET_NAME;
    facetPtr->contents.facet.version = OCT_CURRENT_VERSION;
    facetPtr->contents.facet.mode = mode;
    
    if ( octOpenFacet(facetPtr) < OCT_OK ) {
	errRaise( TAP_PKG_NAME, 1, "Cannot open %s\nEither technology \"%s\" does not exist, or viewtype \"%s\" is wrong.", cellStr, name, viewStr );
    }

    /* get oct's internal strings, and free our own */
    FREE(cellStr);
    FREE(viewStr);
    OCT_ASSERT(octGetById(facetPtr));

    if (mode[0] == 'w') {
	octObject prop;

	OCT_ASSERT(ohCreateOrModifyPropStr(facetPtr, &prop,
					TECH_PROP_NAME, TAP_TECH_PROP_VAL));
    }

    TAP_END();
    return;
}

static struct tapTech *initTechFacet(facetPtr)
octObject *facetPtr;
{
    struct tapTech *techPtr;
    void initLayers();
    octId *id;

    techPtr = ALLOC(struct tapTech, 1);
    techPtr->facet = *facetPtr;
    initLayers(techPtr);
    techPtr->connList = NIL(struct tapConnector);
    id = ALLOC(octId, 1);
    *id = facetPtr->objectId;
    (void) st_insert(techHash, (char *) id, (char *) techPtr);
    techPtr->next = techList;
    techList = techPtr;
    return(techPtr);
}

static void initLayers(techPtr)
struct tapTech *techPtr;
{
    octObject bag, layer;
    octGenerator gen;
    octStatus status;
    int i;
    struct tapLayer *layerS;
    extern int octIdCmp(), octIdHash();

    bag.type = OCT_BAG;
    bag.contents.bag.name = VIA_BAG_NAME;
    if (octGetByName(&(techPtr->facet), &bag) != OCT_OK) {
	bag.objectId = oct_null_id;
    }

    /* create layers, which are inserted in the hash table */
    OCT_ASSERT(octInitGenContents(&(techPtr->facet), OCT_LAYER_MASK, &gen));
    techPtr->layerCount = 0;
    techPtr->layerList = ALLOC(struct tapLayer, 1);
    while ((status = octGenerate(&gen, &layer)) == OCT_OK) {
	techPtr->layerList = REALLOC(struct tapLayer, techPtr->layerList,
						techPtr->layerCount + 1);
	layerS = &techPtr->layerList[techPtr->layerCount];

	layerS->techPtr = techPtr;
	layerS->techLayer = layer;
	layerS->layerIndex = techPtr->layerCount++;
	layerS->desRulePtr = NIL(struct tapDesignRules);
	layerS->looksPtr = NIL(struct tapLayerLooks);
	if (!octIdIsNull(bag.objectId) && (octIsAttached(&bag, &layer) == OCT_OK)) {
	    layerS->isConnector = 1;
	} else {
	    layerS->isConnector = 0;
	}
    }
    OCT_ASSERT(status);

    /* put the layers into the hash table AFTER all realloc'ing is done */
    if (layerHash == NIL(st_table)) {
	layerHash = st_init_table(octIdCmp, octIdHash);
    }
    for (i = 0; i < techPtr->layerCount; i++) {
        octId *id = ALLOC(octId, 1);
        *id = techPtr->layerList[i].techLayer.objectId,
	(void) st_insert(layerHash, (char *) id, (char *) &techPtr->layerList[i]);
    }
}

void tapFlushCache()
{
    struct tapTechName *namePtr;
    struct tapTech *techPtr;
    int i;

    TAP_START("tapFlushCache");

    /* free technology name list */
    while (nameList) {
	namePtr = nameList;
	nameList = nameList->next;
	FREE(namePtr->name);
	FREE(namePtr->view);
	FREE(namePtr);
    }

    /* free technology structure list */
    while (techList) {
	techPtr = techList;
	techList = techList->next;
	OCT_ASSERT(octCloseFacet(&techPtr->facet));
	for (i = 0; i < techPtr->layerCount; i++) {
	    tapFreeDesRuleInfo(&techPtr->layerList[i]);
	    tapFreeLayerLooksInfo(&techPtr->layerList[i]);
	}
	FREE(techPtr->layerList);
	tapFreeConnTable(techPtr);
	FREE(techPtr);
    }

    /* free tech facet hash table */
    if (techHash != NIL(st_table)) {
	st_free_table(techHash);
	techHash = NIL(st_table);
    }

    /* free layer hash table */
    if (layerHash != NIL(st_table)) {
	st_free_table(layerHash);
	layerHash = NIL(st_table);
    }

    TAP_END();
}

struct tapLayer *tapInternLayer(layerPtr)
octObject *layerPtr;
{
    struct tapTech *techPtr;
    octObject protoLayer;
    char *dummyPtr;
    int tryCount;

    for (tryCount = 0; tryCount < 2; tryCount++) {
	/* maybe layer is in the hash table already */
	if (layerHash != NIL(st_table) &&
		st_lookup(layerHash, (char *) &layerPtr->objectId, &dummyPtr)) {
	    return((struct tapLayer *) dummyPtr);
	}

	/* get the technology structure for this layer */
	if (layerPtr->type != OCT_LAYER) {
	    errRaise(TAP_PKG_NAME, 0, "object is not a layer");
	}
	techPtr = tapInternTech(layerPtr);	/* this'll init layerHash */

	/* find prototype layer */
	protoLayer.type = OCT_LAYER;
	protoLayer.contents.layer.name = layerPtr->contents.layer.name;
	if (octGetByName(&techPtr->facet, &protoLayer) != OCT_OK) {
	    errRaise(TAP_PKG_NAME, 0,
			"layer ``%s'' does not exist in technology",
			layerPtr->contents.layer.name);
	}
	if (st_lookup(layerHash, (char *) &protoLayer.objectId, &dummyPtr)) {
	    /* we found it -- reinsert with new key */
            octId *id = ALLOC(octId, 1);
            *id = layerPtr->objectId;
	    (void) st_insert(layerHash, (char *) id, dummyPtr);
	    return ((struct tapLayer *) dummyPtr);
	} else {
	    /* the layer isn't in layerHash -- might be new */
	    /* try flushing and re-interning once */
	    /*   XXX -- what if the user is holding a pointer we're freeing? */
	    tapFlushCache();
	}
    }

    /* we couldn't get things to work */
    errRaise(TAP_PKG_NAME, 0,
		"internal error -- layer ``%s'' not put into table",
		protoLayer.contents.layer.name);
    /*NOTREACHED*/
    return((struct tapLayer *) NULL);
}

void tapForEachTech(fnPtr)
void (*fnPtr)();
{
    struct tapTech *techPtr;

    for (techPtr = techList; techPtr; techPtr = techPtr->next) {
	(*fnPtr)(techPtr);
    }
}

void tapForEachLayer(fnPtr)
void (*fnPtr)();
{
    struct tapTech *techPtr;
    int i;

    for (techPtr = techList; techPtr; techPtr = techPtr->next) {
	for (i = 0; i < techPtr->layerCount; i++) {
	    (*fnPtr)(&techPtr->layerList[i]);
	}
    }
}

/*ARGSUSED*/
void tapErrHandler(pkgName, code, message)
char *pkgName;
int code;
char *message;
{
    tapDepth = 0;
    errPopHandler();
    if (pkgName == TAP_PKG_NAME) pkgName = "internal";
    errPass("Unexpected %s error in function %s:\n\n%s\n\n",
	    pkgName, tapRoutine, message);
}
