/*******************************************************************
Version identification:
$Id$

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

 Programmer: Paul Haskell

       Routines for run length encoding and decoding.

********************************************************************/

#include <math.h>
#include <malloc.h>
#include "ptdspRunLength.h"

const double StartOfBlock = 524288.0;
const double StartOfRun = 1048576.0;

#define larger(fl,in) (fabs(fl) >= (in))

/* Run length encodes a DCT image.
   This function takes a double array which represents a DCT image,
   inserts "start of block" markers, run-length encodes it, and
   outputs the modified image.

   For the run-length encoding, all values with absolute value less
   than "thresh" are set to 0.0, to help improve compression.
   Runlengths are coded with a "start of run" symbol and then an
   (integer) run-length. 

   The integer values at addresses indxDc and indxAc and the double
   arrays at address outDc and outAc are set. 
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

/* Inverts run length encoding on a DCT image.
   This function reads 2 double array, representing two coded DCT
   images (one high priority and one low-priority), inverts the
   run-length encoding, and outputs the resulting image.

   Protection is built in to avoid crashing even if some of the coded
   input data is affected by loss. 

   The double array outPtr is modified to store the resulting image. 
*/
void 
Ptdsp_RunLengthInverse (const double * hiImage, const double * loImage,
			double * outPtr, int origSize, 
			int bSize,  int loSize, int HiPri) {
  /* Initialize. */
  int i, j, k, l, blk, runLen;

  /* Do DC image first. */
  i = 0;
  for(j = 0; j < origSize; j += bSize * bSize) {
    for(k = 0; k < HiPri; k++) {
      outPtr[j + k] = hiImage[i++];
    }	
  }

  /* While still low priority input data left... */
  i = 0;
  while (i < loSize) {
    
    /* Process each block, which starts with "StartOfBlock". */
    while ((i < loSize) && (loImage[i] != StartOfBlock)) {
      i++;
    }	
    if (i < loSize-2) {
      i++;
      blk = (int)loImage[i++];
      blk *= bSize*bSize;
      if ((blk >= 0) && (blk < origSize)) {
	blk += HiPri;
	k = 0;
	while ((i < loSize) && (k < bSize*bSize - HiPri)
	       && (loImage[i] != StartOfBlock)) {
	  if (loImage[i] == StartOfRun) {
	    i++;
	    if (i < loSize) {
	      runLen = (int)loImage[i++];
	      for( l = 0; l < runLen; l++ ) {
		outPtr[blk+k] = 0.0;
		k++;
	      }
	    }
	  } else {
	    outPtr[blk+k] = loImage[i++];
	    k++;
	  }
	}
      } /* if (blk OK) */
    }
  } /* end while (indx < loSize) */
}
