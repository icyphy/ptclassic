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

Programmer: J. T. Buck
Version: $Id$
*/

#include "ptdspMuLaw.h"

const int BIAS16 = 0x84;
const int CLIP16 = 32635;		/* 2^15 - BIAS16 - 1 */

static int exp_lut[256] = {0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
			   4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
			   5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
			   5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
			   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};

/* Compress 16-bit linear data to 8-bit mu-law data */
unsigned char Ptdsp_Linear16ToMuLaw8(int sample) {
  int exponent, mantissa, sign;
  unsigned char ulawbyte;

  /* Get the sample into sign-magnitude. */
  /* this code assumes a two's complement representation */
  sign = (sample >> 8) & 0x80;
  if ( sign ) sample = -sample;
  if ( sample > CLIP16 ) sample = CLIP16;

  /* Convert from 16-bit linear to mu-law. */
  /* The exponent is four bits; the mantissa is four bits */
  sample += BIAS16;
  exponent = exp_lut[( sample >> 7 ) & 0xFF];
  mantissa = ( sample >> ( exponent + 3 ) ) & 0x0F;
  ulawbyte = ~ ( sign | ( exponent << 4 ) | mantissa );

  return ulawbyte;
}

/* Compress 8-bit linear data to 8-bit mu-law data */
/* Convert the data to 16 bits and call the 16-bit converter */
unsigned char Ptdsp_Linear8ToMuLaw8(int sample) {
  return Ptdsp_Linear8ToMuLaw8((sample & 0xFF) << 8);
}
