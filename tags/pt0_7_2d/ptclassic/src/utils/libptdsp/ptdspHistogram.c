/*******************************************************************
Version identification:
@(#)ptdspHistogram.c	1.5 8/6/96

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

 Programmer: Bilung Lee

       Function definition for Ptdsp_Histogram

********************************************************************/

#include <string.h>
#include "ptdspNearestNeighbor.h"

/* Generates histogram of an image.
   This function takes integer array "matrix" which represents an
   image and computes the histogram of that image. This is useful for
   contrast enhancement. 
   
   The return histogram is stored in the integer array hist.
*/
void
Ptdsp_Histogram (const int* matrix, int size, int* hist, int min, int max) {
  /* Initialize the histogram buffer to zero */
  int i;
  int histlen = max - min + 1;

  /* Reset the elements of the histogram buffer to zero */
  memset(hist, 0, histlen*sizeof(int));

  /* Compute the histogram */
  for (i = 0; i < size; i++) {
    /* If the value is larger than max, then is counted into max */
    /* If the value is smaller than min, then is counted into min */
    int index = *matrix++;
    if (index < min) index = min;
    else if (index > max) index = max;
    hist[index-min]++;
  }
}
