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
/*

  tap_quantizeWidth()
  tap_quantizeWidthDOWN()
  tap_currentDensity()
  tap_cDensityInit()
  tap_getWidthFromPeakCurrent()

To compute the width of a terminal do the following:

   width = tap_getWidthFromPeakCurrent( peakCurrent, layer )

*/
#include "copyright.h"
#include "port.h"
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "utility.h" 

#define OCT_LAMBDA 20
#define DEFAULT_MAX_CURRENT_DENSITY 0.1e-4 /* in amperes per lambda */

int	layer_width[3];
int	layer_spacing[3];
char	*layer_nameTable[3] = {"POLY", "MET1", "MET2"};
char	*alter_nameTable[3] = {"MB", "MB", "MC"};
char	*TECH_DIR;
 
int
  tap_quantizeWidth(w)
int  w;	       /* original width (in lambdas) */
/*  This function returns the given wire width quantized according to a
 *  built-in look-up table - all units are in lambdas */
{
    int	m;			/* w MOD OCT_LAMBDA*/
    int	nw;			/* new width in lambda */
    
    /* The width is in "OCT" units: It must be rounded to the nearest LAMBDA */
  
    m = w % OCT_LAMBDA;
    if (m != 0) {
	w += OCT_LAMBDA - m;	/* round up to next OCT_LAMBDA */
    }

  /*
   * Quantize: Quantization sequence is (in lambda) :
   * 1,2,3,4,5,6,7,8,9,10,12,14,17,20,25,30,35,.... (up by 5)
   */
    nw = w / OCT_LAMBDA;		/* Work in lambda  */

    if (ABS(nw) < 1) return 1;

    if (nw > 10) {
	if ( nw <= 14 ) {
	    if (nw % 2 != 0)  nw = ((nw / 2) + 1) * 2;
	} else if ( nw <= 20) { 
	    if ((nw-14) % 3 != 0)   nw = 14 + (((nw-14) / 3) + 1) * 3;
	} else if (nw < 30) {
	    if (nw % 5 != 0)   nw = ((nw / 5) + 1) * 5;
	} else {
	    nw = nw + 5 - nw%5;
	}
    }
    return nw;	/* return lambdas */
}


int
  tap_quantizeWidthDOWN(w)
int	w;			/* original width (in lambdas) */
{
    int	m;			/* w MOD OCT_LAMBDA*/
    int	nw;			/* new width in lambda */
    
    /* The width is in "OCT" units: It must be rounded to the nearest LAMBDA */
    
    m = w % OCT_LAMBDA;
    if (m != 0) {
	w -= m;	/* round down to next OCT_LAMBDA */
    }
    
    /*
     * Quantize: Quantization sequence is (in lambda) :
     * 1,2,3,4,5,6,7,8,9,10,12,14,17,20,25,30,35,.... (up by 5)
     */
    nw = w / OCT_LAMBDA;		/* Work in lambda  */
    if (nw > 10) {
	if ( nw <= 14 ) {
	    if (nw % 2 != 0)  nw = (nw / 2) * 2;
	} else if ( nw <= 20) { 
	    if ((nw-14) % 3 != 0)   nw = 14 + (((nw-14) / 3)) * 3;
	} else {
	    if (nw % 5 != 0)   nw = (nw / 5) * 5;
	}
    }
    return nw;	/* return LAMBDAS */
}



static double curr_denTable[3] = {0.0, 0.0, 0.0};

int tap_cDensityInit( facet )
    octObject* facet;
{
    static int init = 0;
    char *tech;
    octObject prop;

    if ( !init && facet != 0 ) {
	int i;
	octObject layer;
	double coordSize = 1.e-6;
	tapLayerListElement layerList[1];

	OH_ASSERT (ohGetByPropName(facet, &prop, "TECHNOLOGY"));
	tech = util_strsav(prop.contents.prop.value.string);

	if( curr_denTable[1] == 0.0 )  {
	    if ( tapGetProp( facet, TAP_COORD_SIZE ) ) {
		coordSize = TAP_COORD_SIZE->value.real;
	    }
	    for ( i = 0; i < 3 ; i++ ) {
		layer.type = OCT_LAYER;
		if (!strcasecmp(tech, "super")) {
		    layer.contents.layer.name = alter_nameTable[i];
		} else {
		    layer.contents.layer.name = layer_nameTable[i];
		}
		octGetOrCreate( facet, &layer );
		layerList[0].layer = layer;
		if ( tapGetLyrProp( 1, layerList, TAP_MAX_CURRENT_DENSITY ) ) {
		    curr_denTable[i] = TAP_MAX_CURRENT_DENSITY->value.real * coordSize;
		} else {
		    curr_denTable[i] = 0.0015;
		}
		/* printf("%s current density %lf (amps/oct unit)\n", layer_nameTable[i], curr_denTable[i] ); */
	    }
	}
	init = 1;
    }
    return init;
}

double
  tap_currentDensity( layer )
octObject *layer;
/*
 * returns the maximum current density for a particular layer. If the layer
 * is not known, then some default is returned. 
 */
{
    int  layerNum;

    if (!layer) return DEFAULT_MAX_CURRENT_DENSITY;

    if( ! tap_cDensityInit( NULL ) ) {
	fprintf(stderr, "ERROR: before using the current density utilities the \n");
	fprintf(stderr, "cDensity package should be initialized with \"tap_cDensityInit\"\n");
	exit(-1);
    }
    if(layer->contents.layer.name && !strcmp(layer->contents.layer.name,"POLY"))
      layerNum = 0;
    else if(layer->contents.layer.name && !strcmp( layer->contents.layer.name,
						  "MET1"))   layerNum = 1;
    else if(layer->contents.layer.name && !strcmp( layer->contents.layer.name, 
						  "MET2" ) ) layerNum = 2;
    else {
	return DEFAULT_MAX_CURRENT_DENSITY;
    }
    return curr_denTable[layerNum];
}

int
  tap_getWidthFromPeakCurrent( peakCurrent, layer )
double       peakCurrent;   /*  Peak current in amperes */
octObject   *layer;         /*  Layer of the terminal/wire  */
/*  Return the width of a wire necessary to sustain the given
 *  peak current. The width is returned in LAMBDAs  */
{
    return  tap_quantizeWidth( (int) (ABS(peakCurrent) / tap_currentDensity( layer )));
}
