/*******************************************************************
Version identification:
@(#)ptdspNearestNeighbor.c	1.9 8/6/96

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

 Programmer: Biling Lee and Brian Evans

       Function definition for Ptdsp_NearestNeighbor.

********************************************************************/

#include "ptdspNearestNeighbor.h"

/* Find nearest neighbor in codebook and return squared error.
   Find the index of nearest neighbor in the codebook for the training
   vector, and find the squared error (squared distance) between the
   training vector and its nearest neighbor. 

   Let X=input vector and Yi=i_th codeword.  
   Find the nearest neighbor codeword Yi to maximize X'*Yi-Ai (' means
   transpose), where Ai=||Yi||^2/2 and should have already been stored
   in the array halfCodewordPower(numCodewords).

   The values at addresses theindex and thedistance are set to the
   index and the squared error respectively.
*/
void 
Ptdsp_NearestNeighbor(int* theindex, double* thedistance,
		      const double* trnVector, const double* codebook,
		      const double* halfCodewordPower, int sizeCodebook,
		      int dimension) {
  int index = 0;
  int distance = 0;
  int i, j;
  for (j = 0; j < dimension; j++ ) {
    distance += trnVector[j]*codebook[j];
  }
  distance -= halfCodewordPower[0];

  for (i = 1; i < sizeCodebook; i++ ) {
    double sum = 0.0;
    for (j = 0; j < dimension; j++ )
      sum += trnVector[j]*codebook[i*dimension+j];
    sum -= halfCodewordPower[i];
    if ( sum > distance ) {
      distance = sum;
      index = i;
    }
  }
  
  /*
    Distance we compute here is the X'*Yi-||Yi||^2/2, but actually the 
    squared distance is ||X-Yi||^2 = ||X||^2+||Yi||^2-2*X'*Yi. So, we
    need to calculate the true squared distance before return. 
    */
  distance *= -2;
  for (i = 0; i < dimension; i++) {
    distance += trnVector[i] * trnVector[i];
  }
  
  /* Return the compute values of codebook index and distance */
  *theindex = index;
  *thedistance = distance;
}	
