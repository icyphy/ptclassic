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
#include "oh.h"
#include "fang.h"
#include "harpoon.h"
#include "tap.h"
#include "th.h"
#include "errtrap.h"
#include "pepint.h"

#define PROPEQ(p,s) (!strcmp((p)->contents.prop.value.string,(s)))

static double coord_size = (-1.0);   /* Caching variable */

double pepBoxCap( box, layer )
    pepBox    *box;
    octObject *layer;
{
    double w,h;
    double area_cap, perim_cap;
    double perim = 0.0;

    double unit_area_cap = thGetAreaCap( layer );
    double unit_perim_cap = thGetPerimCap( layer );

    if( coord_size < 0)  coord_size = thCoordSize();

    w = ohBoxWidth( &(box->bb) ) * coord_size;
    h = ohBoxHeight( &(box->bb) ) * coord_size;

    area_cap  = w * h * unit_area_cap;
    if(box->freeSides & TAP_TOP)      perim += w;
    if(box->freeSides & TAP_BOTTOM)   perim += w;
    if(box->freeSides & TAP_LEFT)     perim += h;
    if(box->freeSides & TAP_RIGHT)    perim += h;
    perim_cap = perim * unit_perim_cap;

    return area_cap + perim_cap;
}

double pepPathCap( path, layer )
    octObject *path;
    octObject *layer;
{
    pepBox   bb;
    double perim = 0.0;
    double area;
    double width, height;
    double area_cap, perim_cap;
    double unit_area_cap = thGetAreaCap( layer );
    double unit_perim_cap = thGetPerimCap( layer );

    if( coord_size < 0)  coord_size = thCoordSize();

    pepPathBB(path, &bb);

    width  = (bb.bb.upperRight.x - bb.bb.lowerLeft.x) * coord_size;
    height = (bb.bb.upperRight.y - bb.bb.lowerLeft.y) * coord_size;

    area = width * height;
    if(bb.freeSides & TAP_TOP)       perim += (width+width);
    if(bb.freeSides & TAP_LEFT)      perim += (height+height);

    area_cap = area * unit_area_cap;
    perim_cap = perim * unit_perim_cap;

    return area_cap + perim_cap;
}


double pepPolygonCap( poly, layer )
    octObject *poly;
    octObject *layer;
{
    double dx,dy;
    double area_cap, perim_cap;
    double area = 0.0;
    double perim = 0.0;
    double unit_area_cap = thGetAreaCap( layer );
    double unit_perim_cap = thGetPerimCap( layer );

    if( coord_size < 0)  coord_size = thCoordSize();
    /* Use stoke's theorem: walk around the perimiter to compute the area */

    {
	int32 num, i;
	struct octPoint p[2];
	struct octPoint *array, *tmparray = 0;

	ohGetPoints( poly, &num, &array );

	if ( array[0].x != array[num-1].x || array[0].y != array[num-1].y ) {
	    tmparray = ALLOC( struct octPoint, num + 1 );
	    for ( i = 0 ; i < num ; i++ ) tmparray[i] = array[i];
	    tmparray[num] = array[0]; /* Close polygon */
	    array = tmparray;
	    num++;
	}

	for ( i = 1; i < num; i++ ) {
	    p[0] = array[i-1];
	    p[1] = array[i];
	    dx = ( p[1].x - p[0].x );
	    dy = ( p[1].y - p[0].y );
	    perim += hypot(dx,dy);
	    area +=  dx * ( p[1].y + p[0].y ) ; /* Stokes */
	}
	area /= 2;		/*  */
	if ( area < 0 ) area = -area; /* Area should be positive. */

	if (tmparray) FREE( tmparray );
    }

    perim *= coord_size;
    area *=  coord_size *  coord_size;

    area_cap = area * unit_area_cap;
    perim_cap = perim * unit_perim_cap;

    return area_cap + perim_cap;
}


double pepCapacitance( obj )
    octObject *obj;
{
    pepBox bb;
    octObject layer;
    double tmpCap;

    switch (obj->type ) {
    case OCT_BOX:
	bb.bb = obj->contents.box;
	bb.freeSides = TAP_TOP | TAP_BOTTOM | TAP_LEFT | TAP_RIGHT;
	octGenFirstContainer( obj, OCT_LAYER_MASK, &layer );
	return pepBoxCap( &bb, &layer );
    case OCT_POLYGON:
	octGenFirstContainer( obj, OCT_LAYER_MASK, &layer );
	return pepPolygonCap( obj, &layer );
    case OCT_PATH:
	octGenFirstContainer( obj, OCT_LAYER_MASK, &layer );
	return pepPathCap( obj, &layer );
    case OCT_BAG:
    case OCT_LAYER:
	{
	    double cap = 0.0;
	    octGenerator gen;
	    octObject other;
	    octInitGenContents( obj, OCT_ALL_MASK, &gen );
	    while ( octGenerate( &gen, &other ) == OCT_OK ) {
		cap += pepCapacitance( &other );
	    }
	    return cap;
	}
    case OCT_NET:
	{
	    double cap = 0.0;
	    octGenerator gen,genTerm;
	    octObject path, layer, term, inst, flag;
	    ohGetOrCreatePropInt(obj,&flag,"PEP_VISITED",0);
	    octInitGenContents( obj, OCT_PATH_MASK, &gen );
	    while ( octGenerate( &gen, &path ) == OCT_OK ) {
		octGenFirstContainer( &path, OCT_LAYER_MASK, &layer );
		if ((tmpCap = pepPathCap( &path, &layer )) == 0) continue;
		cap += tmpCap;
		
		octInitGenContents(&path,OCT_TERM_MASK,&genTerm);
		while(octGenerate(&genTerm,&term)==OCT_OK){
		    /* Skip formal terms */
		    if(octIdIsNull(term.contents.term.instanceId))continue;
		    /* Get instance */
		    OH_ASSERT(ohGetById(&inst,term.contents.term.instanceId));
		    /* If already visited, skip this instance */
		    if(octIsAttached(&inst,&flag)==OCT_OK)continue;
		    cap+=pepCapacitance(&inst);
		    OH_ASSERT(octAttach(&inst,&flag)); /* Mark instance as visited. */
		}
	    }
	    OH_ASSERT(octDelete(&flag));
	    return cap;
	}
    case OCT_INSTANCE:
	{
	    double cap = 0.0;
	    octObject master,prop;
	    octStatus si= ohOpenMaster(&master,obj,"interface","r");
	    if(ohGetByPropName(&master,&prop,"CELLTYPE")==OCT_OK) {
		if (PROPEQ(&prop,"CONTACT")){
		    octStatus sc = ohOpenMaster(&master,obj,"contents","r");
		    octObject layer;
		    octGenerator gen;
		    octInitGenContents( &master,OCT_LAYER_MASK,&gen);
		    while(octGenerate(&gen,&layer)==OCT_OK){
			cap += pepCapacitance(&layer);
		    }
		    if (sc==OCT_ALREADY_OPEN) octFreeFacet(&master);
		}
	    }
	    if (si==OCT_ALREADY_OPEN) octFreeFacet(&master);
	    return cap;
	}
    case OCT_TERM:
	
	
    default:
	errRaise( PEP_PKG_NAME, 1, "Cannot measure cap on %s", ohFormatName( obj ) );
	
    }
    return 0.0;
}
