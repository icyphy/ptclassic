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
/*       termCap.c
 *
 *       Author: Enrico Malavasi
 *       Ver. 1.00  -  Last revised: September 1991
 *
 *       Exported function: pepTermCap(). It returns the capacitance
 *       between a terminal (and whatever is connected to it) and ground
 *       Capacitance is given in Farad.
 */
#include "port.h"
#include "copyright.h"
#include "utility.h"
#include "oct.h"
#include "errtrap.h"
#include "region.h"
#include "oh.h"
#include "fang.h"
#include "harpoon.h"
#include "tap.h"
#include "th.h"
#include "pepint.h"        /*  Includes pep.h too */

double
  pepTermCap( term )
octObject  *term;
/*
 *  Return the capacitance-to-ground associated to the given terminal.
 *  Hierarchy is followed until the physical view is reached.
 *  The lowest layer attached to the terminal is expanded. If it is on an
 *  ACTIVE class layer, it is clipped by any geometry on the GATE-LAYER layer
 */
{
    octObject  ttt;
    octObject  fterm;
    octObject  layer, layer0;
    octObject  geo;
    octGenerator Gener;
    octBox     gbb;
    octBox     tbb;
    pepBox     box;
    char      *prevLayerName = NULL;
    double     hgt, hgt0;

    if(octGenFirstContent(term, OCT_TERM_MASK, &ttt) == OCT_OK) {
	ohFindFormal(&fterm, &ttt);
	return pepTermCap( &fterm );
    }

    hgt0 = 1;   /*  It is in meters !! */
    while(pepGenNextLayer(term, &layer, prevLayerName) == OCT_OK) {
	prevLayerName = layer.contents.layer.name;
	if(! strcmp( thGetLayerClass(&layer), "CONTACT"))   continue;
	if((hgt=thGetLayerPropReal(&layer, "HEIGHT")) < hgt0) {
	    hgt0 = hgt;
	    layer0 = layer;
	}
    }
    if(hgt0 > 0.9)   return ((double) 0.0);  /*  No layers found except contacts */
    
    ohBB( term, &tbb );
    box.bb = tbb;
    box.freeSides = TAP_TOP | TAP_BOTTOM | TAP_LEFT | TAP_RIGHT;

    /*  Expand the terminal to the geometry it is embedded in  */
    octInitGenContents(&layer0, OCT_GEO_MASK, &Gener);
    while(octGenerate(&Gener, &geo) == OCT_OK) {
	ohBB( &geo, &gbb );
	if((gbb.lowerLeft.y <= tbb.lowerLeft.y) && (gbb.upperRight.y >= tbb.upperRight.y) &&
	   (gbb.lowerLeft.x <= tbb.lowerLeft.x) && (gbb.upperRight.x >= tbb.upperRight.x)) {
	    box.bb.lowerLeft.x = MIN(box.bb.lowerLeft.x, gbb.lowerLeft.x);
	    box.bb.lowerLeft.y = MIN(box.bb.lowerLeft.y, gbb.lowerLeft.y);
	    box.bb.upperRight.x = MAX(box.bb.upperRight.x, gbb.upperRight.x);
	    box.bb.upperRight.y = MAX(box.bb.upperRight.y, gbb.upperRight.y);
	}
    }

    if(! strcmp(thGetLayerClass( &layer0 ), "ACTIVE" )) {
	octObject facet;
	octObject shield;
	char     *shieldName = NULL;
	octGetFacet(term, &facet);
	ERR_IGNORE( shieldName = thGetPropString( "GATE-LAYER" ));
	if((shieldName != NULL) && (ohGetByLayerName(&facet, &shield, shieldName) == OCT_OK)) {
	    
	    octInitGenContents(&shield, OCT_GEO_MASK, &Gener);
	    while(octGenerate(&Gener, &geo) == OCT_OK) {
		ohBB( &geo, &gbb );
		if((gbb.lowerLeft.y <= box.bb.upperRight.y) && (gbb.upperRight.y >= box.bb.lowerLeft.y)) {
		    if((gbb.lowerLeft.x >= tbb.upperRight.x) && (gbb.lowerLeft.x <= box.bb.upperRight.x)) {
			box.freeSides &= (~ TAP_RIGHT);
			box.bb.upperRight.x = gbb.lowerLeft.x;
		    }
		    if((gbb.upperRight.x <= tbb.lowerLeft.x) && (gbb.upperRight.x >= box.bb.lowerLeft.x)) {
			box.freeSides &= (~ TAP_LEFT);
			box.bb.lowerLeft.x = gbb.upperRight.x;
		    }
		}
		if((gbb.lowerLeft.x <= box.bb.upperRight.x) && (gbb.upperRight.x >= box.bb.lowerLeft.x)) {
		    if((gbb.lowerLeft.y >= tbb.upperRight.y) && (gbb.lowerLeft.y <= box.bb.upperRight.y)) {
			box.freeSides &= (~ TAP_TOP);
			box.bb.upperRight.y = gbb.lowerLeft.y;
		    }
		    if((gbb.upperRight.y <= tbb.lowerLeft.y) && (gbb.upperRight.y >= box.bb.lowerLeft.y)) {
			box.freeSides &= (~ TAP_BOTTOM);
			box.bb.lowerLeft.y = gbb.upperRight.y;
		    }
		}
	    }
	}
    }
    if((box.bb.lowerLeft.x >= box.bb.upperRight.x) || (box.bb.lowerLeft.y >= box.bb.upperRight.y))
      return  ((double) 0.0);
    return   pepBoxCap( &box, &layer0 );
}
