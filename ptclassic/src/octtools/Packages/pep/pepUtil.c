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
#include "port.h"
#include "copyright.h"
#include "utility.h"
#include "oct.h"
#include "region.h"
#include "oh.h"
#include "fang.h"
#include "harpoon.h"
#include "tap.h"
#include "th.h"
#include "errtrap.h"
#include "pepint.h"

#define MAX_NUM_ID 1000

void
  pepPathBB(obj, pbb)
octObject  *obj;
pepBox     *pbb;
/*
 *  Return the bounding box of the object. If it is a path, only the part
 *  not covered by terminals is considered. Free sides are detected.
 */
{
    int32             n;        /*  Point counter */
    struct octPoint  *pts;      /*  Points at the ends of the path */
    octObject   term1, term2;	/*  Terminals connected to the path  */
    octObject   path;
    octBox      bb1,   bb2;	/*  Terms bounding boxes  */
    octGenerator Gener;

    if(ohBB(obj, &(pbb->bb)) != OCT_OK) {
	fprintf(stderr, "WARNING: Object with no valid bounding found. Skip\n");
	pbb->bb.lowerLeft.x = 0;
	pbb->bb.lowerLeft.y = 0;
	pbb->bb.upperRight.x = 0;
	pbb->bb.upperRight.y = 0;
	return;
    }

    switch (obj->type) {
    case OCT_PATH:
        ohGetPoints(obj, &n, &pts);
	if(n != 2) errRaise(PEP_PKG_NAME, 1, "Wrong number of points found in path");

	OH_ASSERT(octInitGenContents(obj, OCT_TERM_MASK, &Gener));
	if((octGenerate(&Gener, &term1) != OCT_OK) ||
	   (octGenerate(&Gener, &term2) != OCT_OK)) {
	    fprintf(stderr,
            "FATAL ERROR: Wrong path implementation in (%ld,%ld)-(%ld,%ld)\n",
		    (long)pbb->bb.lowerLeft.x, (long)pbb->bb.lowerLeft.y,
		    (long)pbb->bb.upperRight.x, (long)pbb->bb.upperRight.y);
	    exit( 1 );
	}
	octFreeGenerator(&Gener);

	if (ohTerminalBB(&term1, &bb1) != OCT_OK) {
	    /* use the bb you have */
	}
	if (ohTerminalBB(&term2, &bb2) != OCT_OK) {
	    /* use the bb you have */
	}
	
	if(pts[0].x == pts[1].x) {
	    /*  Vertical segment  */
	    if(bb1.lowerLeft.y > bb2.lowerLeft.y)
	      SWAP(struct octBox, bb1, bb2);
	    pbb->bb.lowerLeft.y  = bb1.upperRight.y;
	    pbb->bb.upperRight.y = bb2.lowerLeft.y;
	    pbb->bb.upperRight.y = MAX(pbb->bb.lowerLeft.y, pbb->bb.upperRight.y);
	    pbb->freeSides = (TAP_LEFT | TAP_RIGHT);
	} else {
	    /*  Supposedly horizontal segment  */
	    if(bb1.lowerLeft.x > bb2.lowerLeft.x)
	      SWAP(struct octBox, bb1, bb2);
	    pbb->bb.lowerLeft.x  = bb1.upperRight.x;
	    pbb->bb.upperRight.x = bb2.lowerLeft.x;
	    pbb->bb.upperRight.x = MAX(pbb->bb.lowerLeft.x, pbb->bb.upperRight.x);
	    pbb->freeSides = (TAP_TOP | TAP_BOTTOM);
	}
	break;

    case OCT_TERM:
    default:
	pbb->freeSides = TAP_BOTTOM | TAP_TOP | TAP_LEFT | TAP_RIGHT;
	OH_ASSERT(octInitGenContainers( obj, OCT_PATH_MASK, &Gener ));
	while(octGenerate( &Gener, &path ) == OCT_OK ) {
	    ohGetPoints(&path, &n, &pts);
	    if(n != 2) errRaise(PEP_PKG_NAME, 1, "Wrong number of points found in path");
	    if(pts[0].x > pts[1].x)  SWAP(octCoord, pts[0].x, pts[1].x);
	    if(pts[0].y > pts[1].y)  SWAP(octCoord, pts[0].y, pts[1].y);
	    if(pts[1].x > pts[0].x) {     /*   Horizontal segment  */
		if(pts[1].x > pbb->bb.upperRight.x) {
		    pbb->freeSides &= (~ TAP_RIGHT);
		} else if(pts[0].x < pbb->bb.lowerLeft.x) {
		    pbb->freeSides &= (~ TAP_LEFT);
		}
	    } else {                      /*  Vertical segment  */
		if(pts[1].y > pbb->bb.upperRight.y) {
		    pbb->freeSides &= (~ TAP_TOP);
		} else if(pts[0].y < pbb->bb.lowerLeft.y) {
		    pbb->freeSides &= (~ TAP_BOTTOM);
		}
	    }
	}
	octFreeGenerator(&Gener);
    }
    return;
}

octStatus
  pepGenNextLayer(obj, layer, prevLayerName)
octObject *obj;                 /* Object */
octObject *layer;               /* Returned layer */
char      *prevLayerName;       /* Previous layer name */
/*
 *  This function generates one of the layers associated with the
 *  given object. If prevLayerName is "" or NULL, the first layer
 *  is returned and the others are put into a list.
 *  Otherwise a layer with name prevLayerName is searched, and
 *  the next one in the list is returned.
 *  This function returns OCT_OK is a valid layer is found, OCT_NOT_FOUND
 *  otherwise.
 */
{
    octBox       implBB;	/*  Terminal implementation box  */
    int          i;		/*  Layer count  */
    static   octObject    facet;
    static char **layer_array;	/*  Array of layer names  */
    static int    layer_count;	/*  Size of the array */
  
    if((prevLayerName == NULL) || (strcmp(prevLayerName, "") == 0)) {
    
	octGetFacet (obj, &facet);
    
	switch (obj->type) {
      
	case OCT_TERM:
	    regFindLyrImpl(obj, &implBB, &layer_count, &layer_array);
	    if(layer_count < 1)
	      errRaise(PEP_PKG_NAME, 1, "Wrong terminal implementation");    
      
	    for(i=0 ; i<layer_count ; i++) {
		OH_ASSERT(ohGetOrCreateLayer(&facet, layer, layer_array[i]));
		if(strcmp(thGetLayerClass(layer), "CONTACT") != 0) {
		    return OCT_OK;
		}
	    }
	    return OCT_NOT_FOUND;
      
	case OCT_BOX:
	case OCT_PATH:
	    /*  There can be one layer only  */
	    OH_ASSERT(octGenFirstContainer(obj, OCT_LAYER_MASK, layer));
	    layer_count = 1;
	    return OCT_OK;
      
	case OCT_INSTANCE:
	    {
		static char *static_layer_array[20];
		octObject master;
		octGenerator gen;

		ohOpenMaster( &master, obj, "interface", "r" );
		octInitGenContents( &master, OCT_LAYER_MASK, &gen );
		layer_count = 0;
		layer_array = static_layer_array;
		while ( octGenerate( &gen, layer ) == OCT_OK ) {
		    if ( ohCountContents( layer, OCT_GEO_MASK ) ) {
			layer_array[layer_count++] = layer->contents.layer.name;
		    }
		}
		octCloseFacet( &master );
	    }

	    if ( layer_count ) {
		ohGetOrCreateLayer( &facet, layer, layer_array[0] );
		return OCT_OK;
	    } else {
		return OCT_NOT_FOUND;
	    }
	default:
	    errRaise(PEP_PKG_NAME, 1, "Unknown layer for the given object");
	}
    }

    for(i=1 ; i<layer_count ; i++) {
	if(strcmp(prevLayerName, layer->contents.layer.name) == 0) {
	    break;
	}
    }
    for(++i ; i<layer_count ; i++) {
	OH_ASSERT(ohGetOrCreateLayer(&facet, layer, layer_array[i]));
	if(strcmp(thGetLayerClass(layer), "CONTACT") != 0) {
	    return OCT_OK;
	}
    }

    return  OCT_NOT_FOUND;
}

int
  pepDisjointBoxes(bb1, bb2, margin)
pepBox  *bb1;
pepBox  *bb2;
octCoord margin;
{
    if(bb1->bb.lowerLeft.x > (bb2->bb.upperRight.x+margin))  return 0;
    if(bb2->bb.lowerLeft.x > (bb1->bb.upperRight.x+margin))  return 0;
    if(bb1->bb.lowerLeft.y > (bb2->bb.upperRight.y+margin))  return 0;
    if(bb2->bb.lowerLeft.y > (bb1->bb.upperRight.y+margin))  return 0;
    return 1;
}


/* finds center of term */

octPoint
  pepCenterTerm(term, coord)
octObject *term;
double coord;
{
    octPoint center;
    octBox bb;

    ohTerminalBB(term, &bb);
    center.x = (octCoord)((bb.lowerLeft.x + bb.upperRight.x)/(2*coord)) ;
    center.y = (octCoord)((bb.lowerLeft.y + bb.upperRight.y)/(2*coord)) ;

    return center;
}


/* checks if obj is in array */

octStatus
  pepIsInArray(obj, idArray)
octObject *obj;
octId *idArray;
{
    int i;
    
    for (i = 0; i < MAX_NUM_ID; i++) {
	if (octIdIsNull(idArray[i])) /* end of series */
	  return OCT_NOT_FOUND; 
	if (octIdsEqual(obj->objectId, idArray[i])) 
	  return OCT_OK;
    }
    return OCT_NOT_FOUND;

}
