/* Version Identification:
 * $Id$
 */
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
#ifndef PEPINT_H
#define PEPINT_H

#include "pep.h"


/*  pepUtilities   */
OCT_EXTERN void pepPathBB
 ARGS((octObject *path, pepBox *bb));

OCT_EXTERN octStatus pepGenNextLayer
 ARGS((octObject *obj, octObject *layer, char *prevLayer));

OCT_EXTERN int  pepDisjointBoxes
  ARGS((pepBox  *bb1, pepBox  *bb2, octCoord margin));

#define   SWAP(t,a,b)  {t x; x = a; a = b; b = x; }

OCT_EXTERN double pepBoxCap
  ARGS((pepBox    *box, octObject *layer));

OCT_EXTERN octPoint pepCenterTerm
 ARGS((octObject *term, double coord));

OCT_EXTERN octStatus pepIsInArray
 ARGS((octObject *obj, octId *idArray));


/*  Graph extraction and reduction */

OCT_EXTERN octStatus pepGraphExtract
ARGS((octObject   *net));

OCT_EXTERN octStatus pepReduce
ARGS((octObject  *net));

OCT_EXTERN octStatus pepInitGenerator
 ARGS((octObject *net));

OCT_EXTERN double pepGenerateRes
 ARGS((octObject  *net, octObject *term1, octObject *term2));

OCT_EXTERN octStatus pepFreeGraph
 ARGS((octObject *net));


/*  Models  */

OCT_EXTERN double pepModParallelLines
 ARGS(( octCoord   dist , octCoord  width , octObject *layer));

OCT_EXTERN double pepModParallel
  ARGS((octCoord   dist,  octCoord   w1, octCoord w2, octCoord width, octObject *layer));

OCT_EXTERN double pepModCrossing
  ARGS((octCoord   w1, octCoord w2, octObject *layer1, octObject *layer2));

OCT_EXTERN double pepModParallelDiffLayers
  ARGS((octCoord   spac, octCoord   w1, octCoord w2, octCoord side, octObject *layer1, octObject *layer2));

OCT_EXTERN double pepModOverlapArea
 ARGS(( octCoord area , octObject *layer1 , octObject *layer2));

OCT_EXTERN double pepModFringe
  ARGS((octCoord wdt, octCoord dist, octObject *layer1, octObject *layer2));

#endif
