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
/*      models.c
 *
 *      Models used by the PEP package for parasitic extraction.
 *      Author: Enrico Malavasi
 *      Ver. 1.00   -   Last revised:  August 1991
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
#include "pepint.h"

static double localCoordSize = (-1);

#define   FARAD_TO_FEMTO    1.0e15
#define   FEMTO_TO_FARAD    1.0e-15
#define   METER_TO_MICRON   1.0e6
#define   MICRON_TO_METER   1.0e-6

/*  ------------------------------------------------------------
 *  AUTOMATICALLY GENERATED MODELS
 *  They work with um, fF.
 *  ------------------------------------------------------------ */
static double
  capmodPAR1(s, w1, w2)      /*  Parallel lines on the same layer  */
double s; /* separation */ 
double w1; /* line1 width */ 
double w2; /* line2 width */ 
{
    double s12 = 0.0;
    double sw1;
    double sw2;
    double cap; /* capacitance */ 

    if((s > 5) || (w1 > 10) || (w2 > 10))        return((double) (-1));
    if((s < 1) || (w1 < 1) || (w2 < 1))          return((double) (-1));

    if(s12 != 0) s12 = 1/s;
    else {
        return((double)(-1));
    }
    sw1 = 1/(s+w1);
    sw2 = 1/(s+w2);
    cap = 
    - 0.0048
    - 0.023 * sw2 
    - 0.023 * sw1 
    + 0.106 * s12 
    - 0.032 * s12  * s12 
    ;
return(cap);
}

static double
  capmodPAR2(s, w1, w2)
double s; /* separation */ 
double w1; /* line1 width */ 
double w2; /* line2 width */ 
{
    double s12 = 0.0;
    double sw1;
    double sw2;
    double cap; /* capacitance */ 

    if((s12 > 5) || (w1 > 10) || (w2 > 10))
        return((double)(-1));
    if((s12 < 1) || (w1 < 1) || (w2 < 1))
        return((double)(-1));

    if(s12 != 0) s12 = 1/s;
    else {
        return((double)(-1));
    }
    sw1 = 1/(s+w1);
    sw2 = 1/(s+w2);
    cap = 
    - 0.0066
    - 0.011 * sw2 
    - 0.011 * sw1 
    + 0.115 * s12 
    - 0.037 * s12  * s12 
    ;
return(cap);
}

static double
  capmodCROSS(w1, w2)
double w1; /* line1 width */ 
double w2; /* line2 width */ 
{

    double cap; /* capacitance */ 

    if((w1 > 10)||(w2 > 10)) return((double)(-1));
    if((w1 < 1)||(w2 < 1)) return((double)(-1));

    cap = 
    0.16
    + 0.071 * w2 
    + 0.093 * w1 
    + 0.048 * w1  * w2 
    ;
return(cap);
}

/* ------------------------------------------------------------ */


/* Silicon Dielectric constant (Farad/meter) */
#define     PEP_OX_DIELECTRIC     34.5e-12

double
pepModParallelLines(dist, width, layer)
    octCoord   dist;
    octCoord   width;
    octObject *layer;
/*
 *  Capacitance between two paths on the same layer, at distance dist,
 *  whose borders are faced for a length <width>.
 */ 
{
    double  pepCap;
    double  D, W;
    double  FRINGE_FACTOR = 4.0; /* Fringe effect is larger than area effect. */

    if( localCoordSize < 0)  localCoordSize = thCoordSize();
    D = dist  * localCoordSize;
    W = width * localCoordSize;
    /* LINEAR MODEL */
    pepCap  = PEP_OX_DIELECTRIC * thGetThickness(layer) * W / D;
    pepCap  *= FRINGE_FACTOR;

    return  pepCap;
}
double
pepModParallel(dist, w1, w2, side, layer)
    octCoord   dist;
    octCoord   w1, w2;
    octCoord   side;
    octObject *layer;
/*  Return the capacitance between parallel segments of widths
 *  w1, w2 on the same layer layer.
 */
{
    double  pepCap;
    double  D, W1, W2;
    double  Side;
    
    if( localCoordSize < 0)  localCoordSize = thCoordSize();
    D = dist  * localCoordSize;
    W1 = w1 * localCoordSize;
    W2 = w2 * localCoordSize;
    Side = side * localCoordSize;
    pepCap = capmodPAR1(D*1.0e6, W1*METER_TO_MICRON, W2*METER_TO_MICRON);
    if(pepCap >= 0.0)  return (Side * METER_TO_MICRON * pepCap * FEMTO_TO_FARAD);

/*    return pepModParallelLines(dist, side, layer); */
    return ((double) 0.0);
}
double
pepModCrossing(w1, w2, layer1, layer2)
    octCoord   w1, w2;
    octObject *layer1;
    octObject *layer2;
/*  Return the capacitance between crossing segments of widths
 *  w1, w2 on different layers.
 */
{
    double  pepCap;
    double  W1, W2;
    
    if( localCoordSize < 0)  localCoordSize = thCoordSize();
    W1 = w1 * localCoordSize;
    W2 = w2 * localCoordSize;
    pepCap = capmodCROSS(W1*METER_TO_MICRON, W2*METER_TO_MICRON);
    if(pepCap >= 0.0)  return (pepCap * FEMTO_TO_FARAD);

    return ((double) 0.0);
}


double
  pepModOverlapArea(area, layer1, layer2)
octCoord area;           /*  X size of overlapepd area */
octObject *layer1;     /*  Lower layer  */
octObject *layer2;     /*  Upper layer  */
/*
 *  Capacitive model for overlapped areas - NO fringe effect considered
 */ 
{
  double  pepCap;
  double  A;

  if( localCoordSize < 0)  localCoordSize = thCoordSize();
  A = area * localCoordSize * localCoordSize;
  pepCap = thGetLayerLayerPropReal(layer1, layer2, "AREA-CAP-TO-LAYER") * A;

  return(pepCap);
}

static double
  pepUnitFringe(layer1, layer2)
octObject *layer1;
octObject *layer2;
{
    return thGetLayerLayerPropReal(layer1, layer2, "AREA-CAP-TO-LAYER") * 1.0e-6;
    /* !!!!!!!!!!!!!!!!!!!!!!!  GUESS !!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
}
double
pepModParallelDiffLayers(spac, w1, w2, side, layer1, layer2)
    octCoord   spac;    /* If <0 there is an overlap = -s */
    octCoord   w1, w2;
    octCoord   side;
    octObject *layer1;
    octObject *layer2;
/*  Return the capacitance between crossing segments of widths
 *  w1, w2 on different layers.
 */
{
    double  pepCap;
    double  W1, W2;
    double  Spac;
    double  Side;
    
    if( localCoordSize < 0)  localCoordSize = thCoordSize();
    Spac = spac * localCoordSize;
    W1 = w1 * localCoordSize;
    W2 = w2 * localCoordSize;
    Side = side * localCoordSize;
    if(spac > 0) {
	pepCap = capmodPAR2(Spac*METER_TO_MICRON, W1*METER_TO_MICRON, W2*METER_TO_MICRON);
	if(pepCap >= 0.0)  return (Side * METER_TO_MICRON * pepCap * FEMTO_TO_FARAD);
	return ((double) 0.0);
    }
    return (pepModOverlapArea(-spac * side, layer1, layer2) + Side * pepUnitFringe(layer1, layer2));
}
double
  pepModFringe(wdt, dist, layer1, layer2)
octCoord   wdt;
octCoord   dist;
octObject *layer1;      /*  Lower layer  */
octObject *layer2;      /*  Upper layer  */
/*
 *  Return the fringe capacitance between two infinitely extended rectangles
 *  whose borders are faced for a length <wdt>, at distance <dist>.
 *  If (dist > 0), dist is the distance separating two non-overlapping areas.
 *  Otherwise, (- dist) is the width of the overlapped region.
 */
{
    double  pepCap;
    double  W, D;

    if( localCoordSize < 0)  localCoordSize = thCoordSize();
    D = dist  * localCoordSize;
    W = wdt * localCoordSize;

    pepCap  = pepUnitFringe(layer1, layer2) * W;

    if(dist >= 0) {   /*  Non overlapped lines */
	pepCap  /= (1 + 0.18e6 * D);
    }

    return   pepCap;
}
