/*******************************************************************
Version identification:
@(#)ptdspfft_rif.c	1.10 04 Oct 1996

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

 Programmer: Joseph T. Buck

       Function definition for Ptdsp_fft_rif.

********************************************************************/

#include <math.h>
#include "ptdspfft_rif.h"

#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr

/* Compute the discrete Fourier transform of a block of data.
   It uses the power-of-two Fast Fourier Transform algorithm.
   It replaces "data" by either its forward discrete Fourier transform,
   if "isign" is 1, or by its inverse discrete Fourier transform,
   if "isign" is -1. 
   "data" is a complex array of length "nn", input as a real array
   indexed from 0..2*nn-1. 
   "nn" MUST be an integer power of 2 (this is not checked for!?).

   This routine is taken from ~gabriel/src/filters/fft/fft.c. Author is
   unsure of the original source. The file contains no copyright
   notice or description. The declaration is changed to the prototype
   form but the function body is unchanged.

   The double array "data" is modified to hold the result of the FFT.
*/
void 
Ptdsp_fft_rif(double *data, int nn, int isign) {
  int	n;
  int	mmax;
  int	m, j, istep, i;
  double wtemp, wr, wpr, wpi, wi, theta;
  double tempr, tempi;
  
  data--;
  n = nn << 1;
  j = 1;

  for (i = 1; i < n; i += 2) {
    if (j > i) {
      SWAP(data[j], data[i]);
      SWAP(data[j+1], data[i+1]);
    }
    m = n >> 1;
    while (m >= 2 && j >m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
  mmax = 2;
  while (n > mmax) {
    istep = 2*mmax;
    theta = -6.28318530717959/(isign*mmax);
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0;
    wi = 0.0;
    for (m = 1; m < mmax; m += 2) {
      for (i = m; i < n; i += istep) {
	j = i + mmax;
	tempr = wr*data[j] - wi*data[j+1];
	tempi = wr*data[j+1] + wi*data[j];
	data[j] = data[i] - tempr;
	data[j+1] = data[i+1] - tempi;
	data[i] += tempr;
	data[i+1] += tempi;
      }
      wr = (wtemp=wr)*wpr - wi*wpi+wr;
      wi = wi*wpr + wtemp*wpi + wi;
    }
    mmax = istep;
  }
}	
