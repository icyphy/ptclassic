/*
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

 Programmer: Joseph T. Buck and Craig Reese

*/

/**CFile***********************************************************************

  FileName    [ ptdspMuLaw.c ]

  PackageName [ ptdsp ]

  Synopsis    [ Conversions from PCMMuLaw to Linear and back ]

  Description [ Provides functions for compression of 16-bit linear
                data to PCM 8-bit mu-law data and back. The 8-bit mu-law
		format here is 8-bit PCM mu-law encoded audio. The 8
		bits are: 1 bit for the sign, 3 bits for the exponent,
		and 4 bits for the mantissa. ]

  Copyright   [ 

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions. ]

******************************************************************************/

#include "ptdspMuLaw.h"

/*---------------------------------------------------------------------------*/
/* Definition of constants and statics                                       */
/*---------------------------------------------------------------------------*/

const int BIAS16 = 0x84;
const int CLIP16 = 32635;		/* 2^15 - BIAS16 - 1 */

static int sample_to_exponent_table[256] =
	{0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
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

static int exponent_to_sample_table[8] =
	{ 0, 132, 396, 924, 1980, 4092, 8316, 16764 };

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function*******************************************************************
  Synopsis    [ Compress 16-bit linear data to 8-bit PCM mu-law data ]
  Description [ Converts a 16-bit linear data sample (a fixed-point format)
		into an 8-bit pulse compression modulated mu-law data
	 	sample (a floating-point format). ]
  SideEffects []
  SeeAlso     [ Ptdsp_PCMMuLawToLinear ]
******************************************************************************/
unsigned 
char Ptdsp_LinearToPCMMuLaw(int sample) {
  int exponent, mantissa, sign;
  unsigned char ulawbyte;

  /* Get the sample into sign-magnitude. */
  /* this code assumes a two's complement representation */
  sign = (sample >> 8) & 0x80;
  if ( sign ) sample = -sample;
  if ( sample > CLIP16 ) sample = CLIP16;

  /* Convert from 16-bit linear to mu-law. */
  /* The exponent is three bits; the mantissa is four bits */
  sample += BIAS16;
  exponent = sample_to_exponent_table[( sample >> 7 ) & 0xFF];
  mantissa = ( sample >> ( exponent + 3 ) ) & 0x0F;
  ulawbyte = ~ ( sign | ( exponent << 4 ) | mantissa );

  return ulawbyte;
}

/**Function*******************************************************************
  Synopsis    [ Uncompress 8-bit PCM mu-law data to 16-bit linear data ]
  Description [ Converts an 8-bit pulse compression modulated mu-law data
	 	sample (a floating-point format) and into a 16-bit
		linear data sample (a fixed-point format).  The conversion
		routine, by Craig Reese at the IDA/Supercomputing Research
		Center, obeys the CCITT Recommendation G.711 and follows
		MIL-STD-188-113,"Interoperability and Performance Standards
		for Analog-to_Digital Conversion Techniques," dated
		17 February 1987. ]
  SideEffects []
  SeeAlso     [ Ptdsp_LinearToPCMMuLaw ]
******************************************************************************/
int
Ptdsp_PCMMuLawToLinear(unsigned char ulawbyte) {
  int sign, exponent, mantissa, sample;
  ulawbyte = ~ ulawbyte;
  sign = ( ulawbyte & 0x80 );
  exponent = ( ulawbyte >> 4 ) & 0x07;
  mantissa = ulawbyte & 0x0F;
  sample = exponent_to_sample_table[exponent] + ( mantissa << (exponent + 3) );
  if ( sign != 0 ) sample = -sample;
  return sample;
}
