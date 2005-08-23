/*******************************************************************
Version identification:
@(#)ptdspZigZag.c	1.7 8/6/96

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

       Functions for forward/inverse zig-zag scan of a matrix. 

********************************************************************/

#include "ptdspZigZag.h"

/* This function zig-zag scans a matrix representing an image stored
   in inImg and returns the result in outImg. This is useful before
   quantizing a DCT transformed image.
*/
void 
Ptdsp_ZigZagScan (const double * inImg, double * outImg, int width,
		  int height, int bSize) {
  int k, l, indx, row, col;
  double* tmpPtr = outImg;

  /* For each row and col... */
  for (row = 0; row < height; row += bSize) {
    for (col = 0; col < width; col += bSize) {
      /* Do zig-zag scan. */
      indx = 0;
      /* K is length of current (semi)diagonal, L is iteration along
	 diag. */
      for (k = 1; k < bSize; k++) { /* Top triangle */
	for (l = 0; l < k; l++) { /* down */
	  tmpPtr[indx++] = inImg[col + (row+l)*width + (k-l-1)];
	}
	k++; /* NOTE THIS! */
	for (l = 0; l < k; l++) { /* back up */
	  tmpPtr[indx++] = inImg[col + (row+k-l-1)*width + l];
	}
      }

      /* If bSize an odd number, start with diagonal, else one
	 down. */
      if (bSize % 2) { k = bSize; }
      else { k = bSize-1; }

      for(; k > 1; k--) { /* Bottom triangle */
	for (l = 0; l < k; l++) { /* down */
	  tmpPtr[indx++] = inImg[col + (row+bSize-k+l)*width +
				 (bSize-l-1)];
	}
	k--; /* NOTE THIS! */
	for (l = 0; l < k; l++) { /* back up */
	  tmpPtr[indx++] = inImg[col + (row+bSize-l-1)*width +
				 bSize-k+l];
	}
      }

      /* Have to do last element. */
      tmpPtr[indx] = inImg[col + (row + bSize - 1) * width +
				 bSize - 1];
      tmpPtr += bSize*bSize;
    }	
  }
}

/* This function inverse zig-zag scans an image, stored in inImg as a
   double array, and outputs the result in outImg.
*/
void
Ptdsp_ZigZagInverse (const double * inImg, double * outImg,
		     int width, int height,  int bSize) {    

  int k, indx, l, row, col;
  int displace = 0;
  for ( row = 0; row < height; row += bSize) {
    for ( col = 0; col < width; col += bSize) {
      /* Invert the zigzag. */
      /* k is length of current (semi)diagonal; l is iteration on
	 diag. */
      k = 0; indx = displace;
      for (k = 1; k < bSize; k++) { /* Top triangle */
	for (l = 0; l < k; l++) { /* down */
	  outImg[col + (row+l)*width + (k-l-1)] = inImg[indx++];
	}
	k++;						/* NOTE THIS! */
	for (l = 0; l < k; l++) {			/* back up */
	  outImg[col + (row+k-l-1)*width + l] = inImg[indx++];
	}
      }

      /* If bSize an odd number, start with diagonal, else one down. */
      if (bSize % 2) { k = bSize; }
      else { k = bSize-1; }
      
      for (; k > 1; k--) {			/* Bottom triangle */
	for (l = 0; l < k; l++) { /* down */
	  outImg[col + (row+bSize-k+l)*width +
		(bSize-l-1)] = inImg[indx++];
	}
	k--; /* NOTE THIS! */
	for (l = 0; l < k; l++) {		/* back up */
	  outImg[col + (row+bSize-l-1)*width +
		bSize-k+l] = inImg[indx++];
	}
      }

      /* Have to do last element. */
      outImg[col + (row + bSize - 1) * width + bSize - 1] = inImg[indx];
      displace += bSize * bSize;;
    }
  }
}
