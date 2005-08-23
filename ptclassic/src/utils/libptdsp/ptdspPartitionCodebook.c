/*******************************************************************
Version identification:
@(#)ptdspPartitionCodebook.c	1.8 8/6/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer: Biling Lee and Brian L. Evans

       Function definition for Ptdsp_PartitionCodebook.

********************************************************************/

#include "ptdspPartitionCodebook.h"

/* Find optimum partition for fixed gain and shape codebooks ]
   Find the optimum partition of the training vector for fixed gain
   and shape codebooks, and find the squared error (squared distance)
   of this training vector.
   
   Let X be the input vector and Si be the i_th shape codeword: 
   1. First find shape codeword Si to maximize X'*Si (' means transpose )
   2. Then find the j_th gain codeword gj to minimize (gj-X'*Si)^2 
   3. Squared error (squared distance) is
      ||X||^2+(gj-X'*Si)^2-(X'*Si)^2.

   The values at addresses theindexShape, theindexGain and thedistance
   are set to the index of the shapeCodebook, the index of the
   gainCodebook and the squared error respectively.
*/
void 
Ptdsp_PartitionCodebook(int* theindexShape, int* theindexGain, 
			double* thedistance, const double* trnVector, 
			const double* shapeCodebook, int sizeShapeCodebook,
			int dimension, const double* gainCodebook,
			int sizeGainCodebook) {

  int indexShape = 0;
  int indexGain = 0;
  int distance = 0;
  int dim, index;
  double shapeDistance = 0;

  for ( dim = 0; dim < dimension; dim++ )
    shapeDistance += trnVector[dim] * shapeCodebook[dim];

  for ( index = 1; index < sizeShapeCodebook; index++ ) {
    double sum = 0.0;
    for ( dim = 0; dim < dimension; dim++ )
      sum += trnVector[dim] * shapeCodebook[index*dimension+dim];
    if ( sum > shapeDistance ) {
      shapeDistance = sum;
      indexShape = index;
    }
    sum = 0.0;
  }

  distance = gainCodebook[0] - shapeDistance;
  distance *= distance;
  
  /* Now shapeDistance stores the maximum X'*Si */
  for (index = 1; index < sizeGainCodebook; index++) {
    double sum = gainCodebook[index] - shapeDistance;
    sum *= sum;
    if ( sum < distance ) {
      distance = sum;
      indexGain = index;
    }
  }

  /* Now distance stores the minimum (gj-X'*Si)^2 */
  distance -= shapeDistance * shapeDistance;
  for (dim = 0; dim < dimension; dim++) {
    distance += trnVector[dim] * trnVector[dim];
  }
  
  *theindexShape = indexShape;
  *theindexGain = indexGain;
  *thedistance = distance;
}
