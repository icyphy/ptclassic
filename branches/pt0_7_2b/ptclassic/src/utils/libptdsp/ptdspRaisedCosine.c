/*******************************************************************
Version identification:
@(#)ptdspRaisedCosine.c	1.7 04 Oct 1996

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

 Programmer: Joseph Buck and Brian Evans

       Functions for raised cosine and square-root raised cosine
       frequency response.

********************************************************************/

#include <math.h>
#include "ptdspRaisedCosine.h"

/* A raised cosine frequency response.
   The code is written with integer arguments so that it can reliably
   detect the 0/0 condition, avoiding roundoff problems. 
*/
double 
Ptdsp_RaisedCosine(int t, int T, double excess) {
  const double DELTA = 1.0e-7;
  double den, s, x;
  if (t == 0) return 1.0;
  x = (double)t/(double)T;
  s = sin(M_PI * x) / (M_PI * x);
  x *= excess;
  den = 1.0 - 4 * x * x;
  if (den > -DELTA && den < DELTA) return s * M_PI/4.0;
  return s * cos (M_PI * x) / den;
}

/* A square-root raised cosine frequency response.
   The code is written with integer arguments so that it can reliably
   detect the 0/0 condition, avoiding roundoff problems. 
*/
double
Ptdsp_SqrtRaisedCosine(int t, int T, double excess) {
  double den, oneminus, oneplus, x;
  double sqrtT = sqrt((double)T);
  
  if (t == 0) {
    return ((4*excess/M_PI) + 1 - excess)/sqrtT;
  }

  x = ((double) t) / ((double) T);
  if (excess == 0.0) {
    return sqrtT*sin(M_PI * x)/(M_PI * t);
  }
  
  oneplus = (1.0 + excess)*M_PI/T;
  oneminus = (1.0 - excess)*M_PI/T;
  /* Check to see whether we will get divide by zero */
  den = t*t*16*excess*excess - T*T;
  if (den == 0) {
    return (T * sqrtT/(8 * excess * M_PI * t)) *
      (oneplus * sin(oneplus * t) -
       (oneminus * T/(4 * excess * t)) * cos(oneminus * t) +
       (T/(4 * excess * t * t)) * sin(oneminus * t) );
  }
  return (4 * excess / (M_PI*sqrtT)) *
    (cos(oneplus * t) + sin(oneminus * t)/(x * 4 * excess)) /
    (1.0 - 16 * excess * excess * x * x);
}
