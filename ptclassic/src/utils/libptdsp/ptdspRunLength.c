
/*
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

Programmer: Paul Haskell
Version: $Id$
*/

#include <math.h>
#include <malloc.h>
#include "ptdspRunLength.h"

const float StartOfBlock = 524288.0;
const float StartOfRun = 1048576.0;

/* return non-zero if fl >= in */
int larger (const float fl, const float in) {
  return (fabs((double)fl) >= ((double)in));
}

/*
  This function takes a Float Matrix which represents a DCT image, 
  inserts "start of block" markers, run-length encodes it, 
  and outputs the modified image. 
  For the run-length encoding, all values with absolute value 
  less than "thresh" are set to 0.0, to help improve compression.

  Runlengths are coded with a "start of run" symbol and then an
  (integer) run-length.
*/

void Ptdsp_RunLengthEncode ( const Ptdsp_FloatMatrix_t inImage, int BlockSize,
			    int HiPri, float thresh, float **outDc,
			    float **outAc, int *indxDc, int *indxAc) { 
  /* Initialize. */
  int bSize = (int)BlockSize;
  int size = inImage.numCols * inImage.numRows;
  int blocks = size / (bSize*bSize);
  int i, blk, zeroRunLen;
  
  /* Temporary storage for one block. */
  float* tmpFloatPtr = ( float* ) malloc(size * sizeof(float));
  float* tmpPtr = tmpFloatPtr;
  for ( i = 0; i < size; i++) {
    tmpPtr[i] = (float)Ptdsp_FloatMatrixEntry(inImage,i);
  }
  
  /* The biggest runlen blowup we can have is the string "01010101...".
     This gives a blowup of 50%, so with StartOfBlock and StartOfRun
     markers, 1.70 should be ok. */
  if (*outDc != 0) free(*outDc);
  if (*outAc != 0) free(*outAc);
  *outDc = ( float* ) malloc(((int)(1.70*size + 1)) * sizeof(float));
  *outAc = ( float* ) malloc(((int)(1.70*size + 1)) * sizeof(float));
  *indxDc = 0; *indxAc = 0;
  for ( blk = 0; blk < blocks; blk++) {
    /* High priority coefficients. */
    for(i = 0; i < HiPri; i++) {
      (*outDc)[(*indxDc)++] = *tmpPtr++;
    }
    
    /* Low priority coefficients--start with block header. */
    (*outAc)[(*indxAc)++] = StartOfBlock;
    (*outAc)[(*indxAc)++] = (float)blk;
	
    zeroRunLen = 0;
    for(; i < bSize*bSize; i++) {
      if(zeroRunLen) {
	if (larger(*tmpPtr, thresh)) {
	  (*outAc)[(*indxAc)++] = (float)zeroRunLen;
	  zeroRunLen = 0;
	  (*outAc)[(*indxAc)++] = *tmpPtr;
	} else {
	  zeroRunLen++;
	}
      } else {
	if (larger(*tmpPtr, thresh)) {
	  (*outAc)[(*indxAc)++] = *tmpPtr;
	} else {
	  (*outAc)[(*indxAc)++] = StartOfRun;
	  zeroRunLen++;
	}
      }
      tmpPtr++;
    }
    /* Handle zero-runs that last till end of the block. */
    if(zeroRunLen) {
      (*outAc)[(*indxAc)++] = (float)zeroRunLen;
    }
  }
  
  /* Delete tmpFloatPtr and not tmpPtr since tmpPtr has been
     incremented */
  free(tmpFloatPtr);
}

