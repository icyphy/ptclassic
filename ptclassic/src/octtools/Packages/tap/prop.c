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

/*LINTLIBRARY*/

static int doGetLyrProp();
static char* pTypeName();

tapPropDescription tapKnownProperties[] = {
	/* Properties not associated with layers */
    {{ "LAMBDA",	OCT_INTEGER },	    0,	"size of lambda in octCoords" },
    {{ "COORD-SIZE",	OCT_REAL },	    0,	"size of octCoord in meters" },

	/* Properties associated with 1 layer */
    {{ "LAYER-CLASS",	OCT_STRING },	    1,	"type of layer; one of `ROUTING', `CONTACT', `ANNOTATION', or `OTHER'" },
    {{ "MAX-CURRENT-DENSITY",	OCT_REAL }, 1,	"Amperes/meter of width" },
    {{ "SHEET-RESISTANCE",	OCT_REAL }, 1,	"ohms/square" },
    {{ "CONTACT-RESISTANCE",	OCT_REAL }, 1,	"ohms/meter^2" },
    {{ "AREA-CAP-TO-GROUND",	OCT_REAL }, 1, 	"Farads/meter^2" },
    {{ "PERIM-CAP-TO-GROUND",	OCT_REAL }, 1,	"Farads/meter of perimeter" },

	/* Properties associated with 2 layers */
    {{ "AREA-CAP-TO-LAYER",	OCT_REAL }, 2,	"Farads/meter^2" },
    {{ "PERIM-CAP-TO-LAYER",	OCT_REAL }, 2,	"Farads/meter of perimeter" },
    {{ 0,			OCT_NULL }, 0,	0 }
};

int tapGetProp(objPtr, propPtr)
octObject *objPtr;
struct octProp *propPtr;
{
    octObject bag;
    octObject myProp;
    octStatus stat;
    char *pTypeName();

    TAP_START("tapGetProp");

    OCT_ASSERT(ohGetOrCreateBag(&tapInternTech(objPtr)->facet, &bag,
						    TECH_PROP_BAG_NAME));
    stat = ohGetByPropName(&bag, &myProp, propPtr->name);
    if (stat == OCT_NOT_FOUND) {
	TAP_END();
	return(0);
    }
    OCT_ASSERT(stat);

    if (myProp.contents.prop.type != propPtr->type) {
	errRaise(TAP_PKG_NAME, 0,
"probably corrupt technology facet:  property `%s'\n\
\twas found with type %s; expected type %s",
		propPtr->name, pTypeName(myProp.contents.prop.type),
		pTypeName(propPtr->type));
    }

    propPtr->value = myProp.contents.prop.value;

    TAP_END();
    return(stat);
}

int tapGetLyrProp(nLayers, layerList, propPtr)
int nLayers;
tapLayerListElement *layerList;
struct octProp *propPtr;
{
    octObject myProp;
    octStatus stat;
    char *pTypeName();

    TAP_START("tapGetLyrProp");

    myProp.type = OCT_PROP;
    myProp.contents.prop.name = propPtr->name;

    if (nLayers == 1) {
	stat = octGetByName(&tapInternLayer(&layerList[0].layer)->techLayer,
			    &myProp);
	if (stat == OCT_NOT_FOUND) {
	    TAP_END();
	    return(0);
	}
	OCT_ASSERT(stat);
    } else {
	if (!doGetLyrProp(nLayers, layerList, &myProp)) {
	    TAP_END();
	    return(0);
	}
    }

    if (myProp.contents.prop.type != propPtr->type) {
	errRaise(TAP_PKG_NAME, 0,
"probably corrupt technology facet:  layer property `%s'\n\
\ton %d layers including %s was found with type %s; expected type %s",
		nLayers, layerList[0].layer.contents.layer.name,
		propPtr->name, pTypeName(myProp.contents.prop.type),
		pTypeName(propPtr->type));
    }

    propPtr->value = myProp.contents.prop.value;

    TAP_END();
    return(1);
}

static int doGetLyrProp(nLayers, layerList, propPtr)
    int nLayers;
    tapLayerListElement *layerList;
    octObject *propPtr;		/* Returned value. */
{
    static int arraySize = 0;
    static octObject **array = NIL(octObject *);
    struct tapLayer *layerPtr;
    struct tapTech *techPtr;
    int i;
    octObject myProp;
    char *propName;
    octGenerator gen;
    octStatus stat;

    if (nLayers < 1) {
	errRaise(TAP_PKG_NAME, 0, "need at least one layer: %d is not enough",
		 nLayers);
    }

    myProp = *propPtr;		/* Make a copy of the argument */

    if (nLayers > arraySize) {
	arraySize = nLayers;
	array = REALLOC(octObject *, array, arraySize);
    }

    techPtr = tapInternTech(&layerList[0].layer);
    for (i = 0; i < nLayers; i++) {
	layerPtr = tapInternLayer(&layerList[i].layer);
	if (layerPtr->techPtr != techPtr) {
	    errRaise(TAP_PKG_NAME, 0,
		     "%d layers don't all have the same technology", nLayers);
	}
	array[i] = &layerPtr->techLayer;
    }

    propName = myProp.contents.prop.name;
    OCT_ASSERT(octInitGenContents(array[0], OCT_PROP_MASK, &gen));
    while ((stat = octGenerate(&gen, &myProp)) == OCT_OK) {
	if (strcmp(propName, myProp.contents.prop.name)) continue; /* Wrong prop. */
	for (i = 1; i < nLayers; i++) {
	    if (octIsAttached(array[i], &myProp) != OCT_OK) break; /* Not attached to all layers */
	}
	if (i == nLayers) {
	    OCT_ASSERT(octFreeGenerator(&gen));	/* Found the correct prop. */
	    *propPtr = myProp;
	    return(1);
	}
    }
    OCT_ASSERT(stat);

    return(0);			/* Not found. */
}

void tapSetProp(objPtr, propPtr)
octObject *objPtr;
struct octProp *propPtr;
{
    octObject bag;
    octObject myProp;

    TAP_START("tapSetProp");

    OCT_ASSERT(ohGetOrCreateBag(&tapInternTech(objPtr)->facet, &bag,
						    TECH_PROP_BAG_NAME));
    myProp.type = OCT_PROP;
    myProp.contents.prop = *propPtr;
    OCT_ASSERT(octCreateOrModify(&bag, &myProp));

    TAP_END();
}

void tapSetLyrProp(nLayers, layerList, propPtr)
int nLayers;
tapLayerListElement *layerList;
struct octProp *propPtr;
{
    octObject myProp;
    int i;
    int doGetLyrProp();

    TAP_START("tapSetLyrProp");

    myProp.type = OCT_PROP;
    myProp.contents.prop = *propPtr;

    if (doGetLyrProp(nLayers, layerList, &myProp)) {
	myProp.contents.prop = *propPtr; /* Reset the value of the prop. */
	OCT_ASSERT(octModify(&myProp));
	return;
    }


    if ( nLayers > 1 ) {
	OCT_ASSERT(octCreate(&tapInternLayer(&layerList[0].layer)->techLayer, &myProp));
	for (i = 1; i < nLayers; i++) {
	    OCT_ASSERT(octAttach(&tapInternLayer(&layerList[i].layer)->techLayer, &myProp));
	}
    } else {
	OCT_ASSERT(octCreateOrModify(&tapInternLayer(&layerList[0].layer)->techLayer,
				     &myProp));
    }

    TAP_END();
}

static char *pTypeName(type)
octPropType type;
{
    switch (type) {
	case OCT_NULL:		return("OCT_NULL");
	case OCT_INTEGER:	return("OCT_INTEGER");
	case OCT_REAL:		return("OCT_REAL");
	case OCT_STRING:	return("OCT_STRING");
	case OCT_ID:		return("OCT_ID");
	case OCT_STRANGER:	return("OCT_STRANGER");
	case OCT_REAL_ARRAY:	return("OCT_REAL_ARRAY");
	case OCT_INTEGER_ARRAY:	return("OCT_INTEGER_ARRAY");
	default:		return("**unknown type**");
    }
}
