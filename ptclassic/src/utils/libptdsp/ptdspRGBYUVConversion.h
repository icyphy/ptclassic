#ifndef _ptdspRGBYUVConversion_h
#define _ptdspRGBYUVConversion_h 1

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

Programmer: Sun-Inn Shih, Brian L. Evans, and T. J. Klausutis
Version: @(#)ptdspRGBYUVConversion.h	1.2 7/23/96
*/

#ifdef __cplusplus
extern "C" {
#endif

extern void Ptdsp_RGBToYUV (const double* redImg, const double* greenImg,
		const double* blueImg, double* YImg, double* UImg,
		double* VImg, int width, int height, int CCIR_601 );
extern void Ptdsp_YUVToRGB (const double* YImg, const double* UImg, 
		const double* VImg, double* redImg, double* greenImg,
		double* blueImg, int width, int height, int CCIR_601 );

#ifdef __cplusplus
}
#endif

#endif
