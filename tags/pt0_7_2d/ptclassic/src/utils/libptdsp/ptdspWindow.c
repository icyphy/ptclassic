/*******************************************************************
Version identification:
@(#)ptdspWindow.c	1.13 04 Oct 1996

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

 Programmer: Kennard White and Brian Evans

       Contains function for specifying standard window functions and
       for generating them .

********************************************************************/

#include <string.h>
#include <math.h>
#include "ptdspcephes.h"
#include "ptdspWindow.h"

/* Returns the window identification number of a window name.
   The window name is passed in as a char string, and this function
   returns the window identification number as defined in ptdspWindow.h. 
   The possible windows are Rectangle, Bartlett, Hanning, Hamming,
   Blackman, SteepBlackman, and Kaiser.
*/
int 
Ptdsp_WindowNumber(const char* wn) {
  int winType = PTDSP_WINDOW_TYPE_NULL;
  if ( strcasecmp(wn, "Rectangle")==0 ) {
    winType = PTDSP_WINDOW_TYPE_RECTANGLE;
  } else if ( strcasecmp(wn, "Bartlett")==0 ) {
    winType = PTDSP_WINDOW_TYPE_BARTLETT;
  } else if ( strcasecmp(wn, "Hanning")==0 ) {
    winType = PTDSP_WINDOW_TYPE_HANNING;
  } else if ( strcasecmp(wn, "Hamming")==0 ) {
    winType = PTDSP_WINDOW_TYPE_HAMMING;
  } else if ( strcasecmp(wn, "Blackman")==0 ) {
    winType = PTDSP_WINDOW_TYPE_BLACKMAN;
  } else if ( strcasecmp(wn, "SteepBlackman")==0 ) {
    winType = PTDSP_WINDOW_TYPE_STEEPBLACKMAN;
  } else if ( strcasecmp(wn, "Kaiser")==0 ) {
    winType = PTDSP_WINDOW_TYPE_KAISER;
  }
  return winType;
}	

/* Produces a standard windowing function.
   The possible windows are Rectangle, Bartlett, Hanning, Hamming,
   Blackman, SteepBlackman, and Kaiser, as defined by winType,
   according to what ptdsp_WindowNumber returns. 
   "realLen" specifies the length of the window. 
   The parameters argument is accessed only for the Kaiser window.
   Returns 1  if there is an error.

   The double array "window" holds the return window.
*/
int 
Ptdsp_Window(double* window, int realLen, int winType, double* parameters) {
  double alpha = 0.0;
  double freq1 = 0.0;
  double freq2 = 0.0;
  double norm = 1.0;
  double scale0 = 0.0;
  double scale1 = 0.0;
  double scale2 = 0.0;
  
  double base_w = M_PI/(realLen-1);
  double sin_base_w = sin(base_w);
  double sin_2base_w = sin(2*base_w);
  double d = -(sin_base_w/sin_2base_w) * (sin_base_w/sin_2base_w);
  
  int i = 0;

  /* 
   *  Window defs taken from Jackson, Digital Filters and Signal
   *  Processing, Second Ed, chap 7.
   */
  switch ( winType ) {
  case PTDSP_WINDOW_TYPE_RECTANGLE:
    scale0 = 1;
    break;
  case PTDSP_WINDOW_TYPE_BARTLETT:
    scale0 = 0;		scale1 = 2.0/realLen;
    break;
  case PTDSP_WINDOW_TYPE_HANNING:
    scale0 = .5;	scale1 = -.5;	freq1 = 2*base_w;
    break;
  case PTDSP_WINDOW_TYPE_HAMMING:
    scale0 = .54;	scale1 = -.46;	freq1 = 2*base_w;
    break;
  case PTDSP_WINDOW_TYPE_BLACKMAN:
    /* This is a special case of SteepBlackman */
    d = -.16;		/* scale0 = .42, scale1 = -.5, scale2=.08 */
    /* FALLTHROUGH */
  case PTDSP_WINDOW_TYPE_STEEPBLACKMAN:
    /* See Jackson 2ed, eqns 7.3.6 through 7.3.10 */
    scale0 = (d+1)/2;	scale1 = -.5;	freq1 = 2*base_w;
    scale2 = -d/2;	freq2 = 4*base_w;
    break;
  case PTDSP_WINDOW_TYPE_KAISER:
    alpha = ((double)(realLen-1))/2.0;
    norm = fabs(Ptdsp_i0(parameters[0]));
    break;
  default:
    return 0;
  }
  
  switch ( winType ) {
  case PTDSP_WINDOW_TYPE_BARTLETT:
    for (i = 0; i < realLen/2; i++) {
      window[i] = scale1 * (i+1);
    }
    for ( ; i < realLen; i++) {
      window[i] = scale1 * (realLen-(i+1));
    }
    break;
  case PTDSP_WINDOW_TYPE_KAISER:
    for (i = 0; i < realLen; i++){
      double squared = pow(((i - alpha)/alpha), 2);
      double entry = parameters[0] * sqrt(1-squared);
      window[i] = Ptdsp_i0(entry)/norm;
    }
    break;
  default:
    for (i = 0; i < realLen; i++) {
      window[i] = scale0 + scale1 * cos(freq1*i) +
	scale2 * cos(freq2*i);
    }
  }
  
  return 1;
}
