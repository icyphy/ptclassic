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

 Programmers: Joseph T. Buck, Craig Reese, Christopher Hylands

       Functions for conversions from PCMMuLaw to Linear and back and to
       write Sun compatible mu-law files.

       Provides functions for compression of 16-bit linear data to PCM
       8-bit mu-law  data and back. The 8-bit mu-law format here is
       8-bit PCM mu-law encoded audio. 
       The 8 bits are: 1 bit for the sign, 3 bits for the exponent,
       and 4 bits for the mantissa.

********************************************************************/

#include "ptdspMuLaw.h"

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

/* Compress 16-bit linear data to 8-bit PCM mu-law data.
   Converts a 16-bit linear data sample (a fixed-point format) into an
   8-bit pulse compression modulated mu-law data sample (a
   floating-point format). 
*/
unsigned char 
Ptdsp_LinearToPCMMuLaw(int sample) {
  int exponent, mantissa, sign;
  unsigned char ulawbyte;

  /* Get the sample into sign-magnitude. */
  /* this code assumes a two's complement representation */
  sign = sample < 0 ? 0x80 : 0;
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

/* Uncompress 8-bit PCM mu-law data to 16-bit linear data.
   Converts an 8-bit pulse compression modulated mu-law data sample (a
   floating-point format) and into a 16-bit linear data sample (a
   fixed-point format). 

   The conversion routine, by Craig Reese at the IDA/Supercomputing
   Research Center, obeys the CCITT Recommendation G.711 and follows
   MIL-STD-188-113,"Interoperability and Performance Standards for
   Analog-to_Digital Conversion Techniques," dated 17 February 1987. 
*/
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


/* Size of a Sun mulaw header */
#define SUNMULAW_HEADER_SIZE 24

/* Write a Sun .au file header.
   This function should be called twice, once before the data has been
   written so that the header can be inserted into the file,.
   Once after the file has all its data so the size can be updated

   fp should be opened with "r+", as this function both reads and writes
   to fp.  See the fopen() man page for details.

   The Sox tool (http://www.spies.com/Sox) says that the format of the
   header is 6 unsigned big-endian 4 byte longs.

   sox-11/au.c says:

   > Any extra bytes (totalling hdr_size - 24) are an
   > "info" field of unspecified nature, usually a string.
   > By convention the header size is a multiple of 4.

   Under Solaris, you can use 'od -t u4 foo.au' to look at the fields.
 */
int
Ptdsp_WriteSunMuLawHeader(FILE *fp) {

    /* The header is 6 unsigned longs: */
    unsigned long magic = 0x2e736e64;	/* Really '.snd' */
    unsigned long hdrsize = SUNMULAW_HEADER_SIZE;
    unsigned long datasize = 0;
    unsigned long encoding = 1;
    unsigned long samplerate = 8000;
    unsigned long channels = 1;

    unsigned long *hdrsizeptr = &hdrsize;
    unsigned long tmpmagic;		/* We read the magic cookie into this*/
    unsigned long *tmpmagicptr = &tmpmagic;

    long currentposition = ftell(fp);
    long filelength;  


    /*  Get the file length */
    if ( fseek(fp, 0L, SEEK_END ) == -1) {
      perror("Ptdsp_WriteSunMuLawHeader: failed to seek to end");
      return 0;
    }
    filelength = ftell(fp);

    if ( fseek(fp, 0L, SEEK_SET ) == -1) {
      perror("Ptdsp_WriteSunMuLawHeader: failed to seek to start");
      return 0;
    }


    if (currentposition == 0 && filelength == 0) {
        /* Nothing has been written yet, write the header.
	   In theory, we could have created a struct here and try
	   writing that, but I don't think the order of struct elements
	   would be guarranteed.
	 */  
        fwrite((const void *)&magic, sizeof(unsigned long), 1, fp);
        fwrite((const void *)&hdrsize, sizeof(unsigned long), 1, fp);
        fwrite((const void *)&datasize, sizeof(unsigned long), 1, fp);
        fwrite((const void *)&encoding, sizeof(unsigned long), 1, fp);
        fwrite((const void *)&samplerate, sizeof(unsigned long), 1, fp);
        fwrite((const void *)&channels, sizeof(unsigned long), 1, fp );

	return 1;
    } else {
        /* Check that the magic cookie is present. */

	if ( fseek(fp, 0L, SEEK_SET ) == -1) {
	    perror("Ptdsp_WriteSunMuLawHeader: failed to seek to start");
	    return 0;
	}

	if ( fread((void *)tmpmagicptr, sizeof(unsigned long), 1, fp) != 1) { 
	    perror("Ptdsp_WriteSunMuLawHeader: failed to read magic cookie");
	    return 0;
	}

	if (*tmpmagicptr == magic) {
	    /* The magic cookie is present, so we can just
	       write the size of the data section.
	     */

	    /* Get the header size, it might not be SUNMULAW_HEADER_SIZE
	       if the file was created with an info field.
	     */  
	    if ( fread((void *)hdrsizeptr, sizeof(unsigned long), 1, fp) != 1){
	      perror("Ptdsp_WriteSunMuLawHeader: failed to read hdrsize");
	      return 0;
	    }

	    /* The Solaris fopen man pagesays that an input
	       may not be folloed by output without a fseek, fsetpos
	       or rewind (or an input operation that encounters an EOF).
	     */  
	    if ( fseek(fp, sizeof(unsigned long) * 2, SEEK_SET ) == -1) {
	        perror("Ptdsp_WriteSunMuLawHeader: ");
		return 0;
	    }

	    /* We are now at the proper location to right the datasize */
	    datasize = filelength - hdrsize;
	    fwrite((const void *)&datasize, sizeof(unsigned long), 1, fp);

	    if ( fseek(fp, currentposition, SEEK_SET ) == -1) {
	        perror("Ptdsp_WriteSunMuLawHeader: ");
		return 0;
	    }
	    return 1;
	} else {
	    /* The file is non-zero in length, but the Sun mu law .au
	       file magic cookie is not present, we should add
	       a header and copy the file, but that should probably
	       go in a function that takes a filename rather than FILE *.
	     */
	    return 0;
	}
    }
}

