/**CFile***********************************************************************

  FileName    [ ptdspRunLength.c ]

  PackageName [ ptdsp ]

  Synopsis    [ required ]

  Description [ optional ]

  Author      [ Paul Haskell ]

  Copyright   [ 

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
 ]

  Version     [ $Id$ ]

******************************************************************************/

#include <math.h>
#include <malloc.h>
#include "ptdspRunLength.h"

const float StartOfBlock = 524288.0;
const float StartOfRun = 1048576.0;

int larger (const float fl, const float in) {
  return (fabs((double)fl) >= (double)in); 
}

/*---------------------------------------------------------------------------*/	
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function*******************************************************************
  Synopsis    [ Run length encodes a DCT image ]
  Description [ This function takes a float array which represents a
                DCT image, inserts "start of block" markers,
		run-length encodes it, and outputs the modified image. </p>
		For the run-length encoding, all values with absolute
		value less than "thresh" are set to 0.0, to help
		improve compression. ]
  SideEffects []
  SeeAlso     [ Ptdsp_RunLengthInverse ]
******************************************************************************/
/*
  Runlengths are coded with a "start of run" symbol and then an
  (integer) run-length.
*/
void 
Ptdsp_RunLengthEncode ( const double * inImagePtr, int arraySize, int bSize,
			int HiPri, double thresh, double **outDc,
			double **outAc, int *indxDc, int *indxAc) { 
  /* Initialize. */
  int blocks = arraySize / (bSize*bSize);
  int i, blk, zeroRunLen;
  
  /* The biggest runlen blowup we can have is the string "01010101...".
     This gives a blowup of 50%, so with StartOfBlock and StartOfRun
     markers, 1.70 should be ok. */
  if (*outDc != 0) free(*outDc);
  if (*outAc != 0) free(*outAc);
  *outDc = ( double* ) malloc(((int)(1.70*arraySize + 1)) * sizeof(double));
  *outAc = ( double* ) malloc(((int)(1.70*arraySize + 1)) * sizeof(double));
  *indxDc = 0; *indxAc = 0;
  for ( blk = 0; blk < blocks; blk++) {
    /* High priority coefficients. */
    for(i = 0; i < HiPri; i++) {
      (*outDc)[(*indxDc)++] = *inImagePtr++;
    }
    
    /* Low priority coefficients--start with block header. */
    (*outAc)[(*indxAc)++] = StartOfBlock;
    (*outAc)[(*indxAc)++] = (double)blk;
	
    zeroRunLen = 0;
    for(; i < bSize*bSize; i++) {
      if(zeroRunLen) {
	if (larger(*inImagePtr, thresh)) {
	  (*outAc)[(*indxAc)++] = (double)zeroRunLen;
	  zeroRunLen = 0;
	  (*outAc)[(*indxAc)++] = *inImagePtr;
	} else {
	  zeroRunLen++;
	}
      } else {
	if (larger(*inImagePtr, thresh)) {
	  (*outAc)[(*indxAc)++] = *inImagePtr;
	} else {
	  (*outAc)[(*indxAc)++] = StartOfRun;
	  zeroRunLen++;
	}
      }
      inImagePtr++;
    }
    /* Handle zero-runs that last till end of the block. */
    if(zeroRunLen) {
      (*outAc)[(*indxAc)++] = (double)zeroRunLen;
    }
  }
}

