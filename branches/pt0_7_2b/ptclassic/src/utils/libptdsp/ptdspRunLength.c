/*******************************************************************
Version identification:
@(#)ptdspRunLength.c	1.8 04 Oct 1996

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

       Routines for run length encoding and decoding.

********************************************************************/

#include <math.h>
#include <malloc.h>
#include "ptdspRunLength.h"

static const double StartOfBlock = 524288.0;
static const double StartOfRun = 1048576.0;

#define DBL_MAX_VALUE(fl,in) (fabs(fl) >= (in))

/* Run length encode a floating-point matrix in subblocks.
   In each subblock of size "bSize x bSize", this function sends the
   first "HiPri" elements in the "outDc" array.  For the remaining
   elements, inserts start-of-block markers, run-length encodes it,
   and outputs the encoded elements in the "outAc" array.

   For the run-length encoding, all values with absolute value less
   than "thresh" are set to 0.0 to help improve compression.
   Run lengths are coded with a "start of run" symbol and then an
   (integer) run-length.

   This technique is especially useful when applied to images
   subblocks have been transformated by the Discrete Cosine Transform.

   The routine reallocates the double arrays at *outDc and *outAc.
   The integer values at addresses "indxDc" and "indxAc" are set to
   zero.
*/
void 
Ptdsp_RunLengthEncode ( const double* inImagePtr, int arraySize, int bSize,
			int HiPri, double thresh, double** outDc,
			double** outAc, int* indxDc, int* indxAc) { 
  /* Initialize */
  int blk;
  int blocks = arraySize / (bSize*bSize);
 
  /* The biggest run length blowup we can have is the string "01010101...".
     This gives a blowup of 50%, so with StartOfBlock and StartOfRun
     markers, 1.70 should be ok.
   */
  if (*outDc != 0) free(*outDc);
  if (*outAc != 0) free(*outAc);
  *outDc = ( double* ) malloc(((int)(1.70*arraySize + 1)) * sizeof(double));
  *outAc = ( double* ) malloc(((int)(1.70*arraySize + 1)) * sizeof(double));
  *indxDc = 0;
  *indxAc = 0;
  for ( blk = 0; blk < blocks; blk++) {
    int i;
    int zeroRunLen = 0;

    /* High priority coefficients. */
    for(i = 0; i < HiPri; i++) {
      (*outDc)[(*indxDc)++] = *inImagePtr++;
    }
    
    /* Low priority coefficients--start with block header. */
    (*outAc)[(*indxAc)++] = StartOfBlock;
    (*outAc)[(*indxAc)++] = (double)blk;
	
    for(; i < bSize*bSize; i++) {
      if(zeroRunLen) {
	if (DBL_MAX_VALUE(*inImagePtr, thresh)) {
	  (*outAc)[(*indxAc)++] = (double)zeroRunLen;
	  zeroRunLen = 0;
	  (*outAc)[(*indxAc)++] = *inImagePtr;
	}
	else {
	  zeroRunLen++;
	}
      } else {
	if (DBL_MAX_VALUE(*inImagePtr, thresh)) {
	  (*outAc)[(*indxAc)++] = *inImagePtr;
	}
	else {
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

/* Inverts the run length encoding of a floating-point matrix.
   This function reads 2 double arrays, representing two coded
   images (one high priority and one low-priority), inverts the
   run-length encoding, and outputs the resulting image.

   Protection is built in to avoid crashing even if some of the coded
   input data is affected by loss. 

   This is particular useful when applied to images for which a
   Discrete Cosine Transform has been applied to each of the subblocks.

   The double array "outPtr" is modified to store the resulting image. 
*/
void 
Ptdsp_RunLengthInverse (const double* hiImage, const double* loImage,
			double* outPtr, int origSize, 
			int bSize, int loSize, int HiPri) {
  /* Initialize. */
  int i, j;

  /* Do DC image first. */
  i = 0;
  for(j = 0; j < origSize; j += bSize * bSize) {
    int k;
    for(k = 0; k < HiPri; k++) {
      outPtr[j + k] = hiImage[i++];
    }	
  }

  /* While still low priority input data left... */
  i = 0;
  while (i < loSize) {
    int blk;

    /* Process each block, which starts with "StartOfBlock". */
    while ((i < loSize) && (loImage[i] != StartOfBlock)) {
      i++;
    }	
    if (i < loSize-2) {
      i++;
      blk = (int)loImage[i++];
      blk *= bSize*bSize;
      if ((blk >= 0) && (blk < origSize)) {
	int k = 0;
	blk += HiPri;
	while ((i < loSize) && (k < bSize*bSize - HiPri)
	       && (loImage[i] != StartOfBlock)) {
	  if (loImage[i] == StartOfRun) {
	    i++;
	    if (i < loSize) {
	      int l;
	      int runLen = (int)loImage[i++];
	      for( l = 0; l < runLen; l++ ) {
		outPtr[blk+k] = 0.0;
		k++;
	      }
	    }
	  }
	  else {
	    outPtr[blk+k] = loImage[i++];
	    k++;
	  }
	}
      } /* if (blk OK) */
    }
  } /* end while (indx < loSize) */
}
