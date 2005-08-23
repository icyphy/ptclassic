/*******************************************************************
Version identification:
@(#)ptdspRGBYUVConversion.c	1.7 04 Oct 1996

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

 Programmer: Sun-Inn Shih, Brian L. Evans, and T. J. Klausutis

       Routines for conversion from RGB format images to YUV and back.

********************************************************************/

#include "ptdspRGBYUVConversion.h"
 
/* Map a double into an integer in the range [0,255] by rounding. */
static unsigned char
quant(double inval) {
  if (inval < 0.5) 
    return ((unsigned char) 0);
  else if (inval > 254.5) 
    return ((unsigned char) 255);
  else 
    return ((unsigned char) (int)(inval + 0.5));
}

/* Converts a RGB format image to YUV format.
   Read three double arrays that describe a color image in
   Red-Green-Blue (RGB) format stores the output in three double
   arrays that describe that image in YUV format. No downsampling is
   performed on the U and V signals.
   
   The contents of the double arrays YImg, UImg and VImg are modified
   to store the return Y, U and V images, respectively.
*/
void 
Ptdsp_RGBToYUV ( const double* redImg, const double* greenImg, 
		 const double* blueImg, double* YImg, double* UImg,
		 double* VImg, int width, int height, int CCIR_601 ) {
  
  int i;
  for ( i = 0; i < height; i++ ) {
    int temp1 = i*width;
    int j;
    for ( j = 0; j < width; j++ ) {
      int temp2 = j + temp1;
      double rvalue = redImg[temp2];
      double gvalue = greenImg[temp2];
      double bvalue = blueImg[temp2];
      double yvalue =	 0.299  * rvalue +
			 0.587  * gvalue +
			 0.114  * bvalue;
      double uvalue =	-0.1687 * rvalue +
			-0.3313 * gvalue +
			 0.5    * bvalue;
      double vvalue =	 0.5    * rvalue +
			-0.4187 * gvalue +
			-0.0813 * bvalue;
      if ( CCIR_601 ) {
	yvalue = (219.0*yvalue)/255.0 +  16;
	uvalue = (224.0*uvalue)/255.0 + 128;
	vvalue = (224.0*vvalue)/255.0 + 128;
      }
      else {
	uvalue += 128;
	vvalue += 128;
      }
      YImg[temp2] = quant(yvalue);
      UImg[temp2] = quant(uvalue);
      VImg[temp2] = quant(vvalue);
    }
  }
}

/* Converts a YUV format image to RGB format.
   Read three double arrays that describe a color image in YUV format
   and stores the result in  three double arrays that describe an
   image in RGB format. 
   
   The contents of double arrays redImg, greenImg and blueImg are
   modified to store the return red, green and blue image
   respectively. 
*/
void 
Ptdsp_YUVToRGB ( const double* YImg, const double* UImg, 
		 const double* VImg, double* redImg, double* greenImg,
		 double* blueImg, int width, int height, int CCIR_601 ) {
  int i;
  for (i = 0; i < height; i++) {
    int temp1 = i * width;
    int j;
    for (j = 0; j < width; j++){
      double rvalue, gvalue, bvalue;
      int temp2 = j + temp1;
      double yvalue = YImg[temp2];
      double uvalue = UImg[temp2];
      double vvalue = VImg[temp2];
      if ( CCIR_601 ) {
	yvalue = 255.0*(yvalue -  16)/219.0;
	uvalue = 255.0*(uvalue - 128)/224.0;
	vvalue = 255.0*(vvalue - 128)/224.0;
      }
      else {
	uvalue -= 128;
	vvalue -= 128;
      }
      rvalue = yvalue + 1.4020*vvalue;
      gvalue = yvalue - 0.3441*uvalue - 0.7141*vvalue;
      bvalue = yvalue + 1.7720*uvalue;
      redImg[temp2] = quant(rvalue);
      greenImg[temp2] = quant(gvalue);
      blueImg[temp2] = quant(bvalue);
    }
  }
}
