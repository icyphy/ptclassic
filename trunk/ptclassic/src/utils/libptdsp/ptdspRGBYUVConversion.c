/**CFile***********************************************************************

  FileName    [ ptdspRGBYUVConversion.c ]

  PackageName [ ptdsp ]

  Synopsis    [ Routines for conversion from RGB format images to YUV and back ]

  Author      [ Sun-Inn Shih, Brian L. Evans, and T. J. Klausutis ]

  Copyright   [ 

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
 ]

  Version     [ $Id$ ]

******************************************************************************/

#include "ptdspRGBYUVConversion.h"
 
/* perform rounding in range [0, 255] */
unsigned char quant (double inval) {
  if (inval < 0.5) 
    return ((unsigned char) 0);
  else if (inval > 254.5) 
    return ((unsigned char) 255);
  else 
    return ((unsigned char) (int)(inval + 0.5));
}

/*---------------------------------------------------------------------------*/	
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function*******************************************************************
  Synopsis    [ Converts a RGB format image to YUV format ]
  Description [ Read three double arrays that describe a color image
                in Red-Green-Blue (RGB) format stores the output in
		three double arrats that describe that image in YUV
		format. No downsampling is performed on the U and V
		signals.]
  SideEffects []
  SeeAlso     [ Ptdsp_YUVToRGB ]
******************************************************************************/
void 
Ptdsp_RGBToYUV ( const double* redImg, const double* greenImg, 
		 const double* blueImg, double* YImg, double* UImg,
		 double* VImg, int width, int height, int CCIR_601 ) {
  
  int i, j, temp1, temp2;
  double rvalue, gvalue, bvalue;
  double yvalue, uvalue, vvalue;
  for ( i = 0; i < height; i++ ) {
    temp1 = i*width;
    for ( j = 0; j < width; j++ ) {
      temp2 = j + temp1;
      rvalue = redImg[temp2];
      gvalue = greenImg[temp2];
      bvalue = blueImg[temp2];
      yvalue =  0.299  * rvalue +
	        0.587  * gvalue +
	        0.114  * bvalue;
      uvalue = -0.1687 * rvalue +
	       -0.3313 * gvalue +
	        0.5    * bvalue;
      vvalue =  0.5    * rvalue +
	       -0.4187 * gvalue +
	       -0.0813 * bvalue;
      if ( (int)CCIR_601 ) {
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

/**Function*******************************************************************
  Synopsis    [ Converts a YUV format image to RGB format ]
  Description [ Read three double arrays that describe a color image
                in YUV format and stores the result in  three double
		arrays that describe an image in RGB format. ]
  SideEffects []
  SeeAlso     [ Ptdsp_RGBToYUV ]
******************************************************************************/
void 
Ptdsp_YUVToRGB ( const double* YImg, const double* UImg, 
		 const double* VImg, double* redImg, double* greenImg,
		 double* blueImg, int width, int height, int CCIR_601 ) {
  int i, j, temp1, temp2;
  double rvalue, gvalue, bvalue;
  double yvalue, uvalue, vvalue;
  for (i = 0; i < height; i++) {
    temp1 = i * width;
    for (j = 0; j < width; j++){
      temp2 = j + temp1;
      yvalue = YImg[temp2];
      uvalue = UImg[temp2];
      vvalue = VImg[temp2];
      if ( (int)CCIR_601 ) {
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
