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

/*
  The original routines to build the connector palette 
  are too slow, because they require opening many facets.
  This file implements a faster procedure to get connectors.
  The interface facet of the connector-palette is opened.
  This facet has been preprocessed with XXXX.
  All information is avaiable immediately.
*/
/*LINTLIBRARY*/


static int getInteger( obj, name )
    octObject* obj;
    char* name;
{
    octObject prop;
    if ( ohGetByPropName( obj, &prop, name ) != OCT_OK || prop.contents.prop.type != OCT_INTEGER ) {
	errRaise( TAP_PKG_NAME, 1 , "Cannot find int prop %s in %s.\n\tCorrupted fast palette.",
		 name, ohFormatName( obj ));
    }
    return prop.contents.prop.value.integer;
}

static char* getString( obj, name )
    octObject* obj;
    char* name;
{
    octObject prop;
    if ( ohGetByPropName( obj, &prop, name ) != OCT_OK || prop.contents.prop.type != OCT_STRING ) {
	errRaise( TAP_PKG_NAME, 1 , "Cannot find string prop %s in %s.\n\tCorrupted fast palette.",
		 name, ohFormatName( obj ));
    }
    return util_strsav( prop.contents.prop.value.string );
}

static int fastTabulateConnector(techPtr, connPtr, topBag, palettePtr)
    struct tapTech *techPtr;
    struct tapConnector *connPtr;
    octObject *topBag;
    octObject *palettePtr;
{
    octObject prop;
    octObject layer;
    octObject layerbag;
    octGenerator gen;
    octStatus stat;
    int i;

    connPtr->area = getInteger( topBag, "AREA" );
    connPtr->actualLayers = getInteger( topBag, "ACTUALLAYERS" );
    connPtr->instance.contents.instance.name = "";
    connPtr->instance.contents.instance.master = getString(topBag, "INSTANCEMASTER" );
    connPtr->instance.contents.instance.view =   getString(topBag, "INSTANCEVIEW" );
    connPtr->instance.contents.instance.facet =  getString(topBag, "INSTANCEFACET" );
    connPtr->instance.contents.instance.version =getString(topBag, "INSTANCEVERSION" );
    connPtr->instance.contents.instance.transform.translation.x = 0;
    connPtr->instance.contents.instance.transform.translation.y = 0;
    connPtr->instance.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
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

    OCT_ASSERT(octInitGenContents(topBag, OCT_BAG_MASK, &gen));
    while ((stat = octGenerate(&gen, &layerbag)) == OCT_OK) {
	OH_ASSERT( ohGetByPropName( &layerbag, &prop, "WIDTHS" ));
	if (prop.contents.prop.type != OCT_INTEGER_ARRAY || prop.contents.prop.value.integer_array.length != 4 ) {
	    errRaise(TAP_PKG_NAME, 0, "corrupt %s fast palette", TAP_CONNECTOR_PALETTE_NAME );
	}
	ohGetByLayerName( &techPtr->facet, &layer, layerbag.contents.prop.name );
	if (tapInternLayer(&layer)->isConnector) {
	    continue;
	}

	i = tapInternLayer(&layer)->layerIndex;
	connPtr->layerDescList[i].widthArray[0] = prop.contents.prop.value.integer_array.array[0];
	connPtr->layerDescList[i].widthArray[1] = prop.contents.prop.value.integer_array.array[1];
	connPtr->layerDescList[i].widthArray[2] = prop.contents.prop.value.integer_array.array[2];
	connPtr->layerDescList[i].widthArray[3] = prop.contents.prop.value.integer_array.array[3];
	connPtr->layerDescList[i].isUsed = 1;
	connPtr->nLayers++;
    }
    OCT_ASSERT(stat);
    return(1);
}


struct tapConnector* fastTapInitPalette( techPtr, palette )
    struct tapTech *techPtr;
    octObject* palette;
{
    octGenerator gen;
    octObject bag, bigBag;
    octStatus genStatus;
    struct tapConnector *connPtr;
    struct tapConnector *connList = NIL(struct tapConnector);

    if ( strcmp( palette->contents.facet.facet, "interface" ) != 0 ) {
	return 0;
    }
    if ( ohGetByBagName( palette, &bigBag, "SORTED_CONNECTORS" ) != OCT_OK ) {
	return 0;
    }
    /* printf( "Using fast palette\n" ); */
    OCT_ASSERT(octInitGenContents( &bigBag, OCT_BAG_MASK, &gen));

    while ((genStatus = octGenerate(&gen, &bag)) == OCT_OK) {
	connPtr = ALLOC(struct tapConnector, 1);

	if (fastTabulateConnector(techPtr, connPtr, &bag, &palette)) {
	    connPtr->next = connList;
	    connList = connPtr;
	} else {
	    FREE(connPtr);
	}
    }
    OCT_ASSERT(genStatus);

    return connList;
}
