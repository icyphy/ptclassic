 defstar {
//////// INFO ON STAR.
	name	{ RGBToYUV }
	domain	{ SDF }
	version	{ @(#)SDFRGBToYUV.pl	1.25 06 Oct 1996 }
	author	{ Sun-Inn Shih, Brian L. Evans, and T. J. Klausutis }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF image library }
	desc {
Read three float matrices that describe a color image in Red-Green-Blue (RGB)
format and output three float matrices that describe an image in YUV format.
No downsampling is performed on the U and V signals.
	}
	htmldoc {
The YUV format, which is a linear mapping of the RGB format, is used
in broadcast television to maintain compatibility between color and
black-and-white televisions [1].
Y is the luminance (intensity) of the image, and U and V represent the
chrominance (hue and saturation).
The YUV format is based on how the eyes perceive color.
The U axis has yellow at its minimum point and blue at its maximum point,
whereas the V axis has turquoise at its minimum point and red at its
maximum point.
In this implementation, each of the RGB values and each of the YUV values
are integer values in the range from 0 to 255 (inclusive).
<p>
This star supports two YUV formats--- the usual one [1] and
the CCIR 601 standard.
The usual YUV definition shifts the U and V values so that they fall in
the range from 0 to 255 (inclusive).
The CCIR 601 standard scales and then shifts the Y, U, and V values so that
Y falls in the range from 16 to 235 (inclusive) and U and V falls
in the range from 16 to 240 (inclusive).
The CCIR 601 standard was developed so that more of the YUV space maps into
the RGB space.
All RGB values, however, map into both YUV standards, so RGB to YUV conversion
followed by YUV to RGB conversion is nearly lossless.
The CCIR 601 standard is used in the MPEG and H.261 compression standards.
<a name="format conversion, RGB to YUV"></a>
<a name="image format conversion, RGB to YUV"></a>
<a name="image format, red-green-blue (RGB)"></a>
<a name="image format, luminance-chrominance (YUV)"></a>
<a name="CCIR 601 standard"></a>
<a name="MPEG"></a>
<a name="H.261"></a>
<a name="Pratt, W."></a>
<h3>References</h3>
<p>[1]  
W. Pratt, <i>Digital Image Processing</i>,
Wiley & Sons: New York.  1991.  2nd ed.
	}

	ccinclude { "Matrix.h", "Error.h", "ptdspRGBYUVConversion.h" }

//////// OUTPUTS AND STATES.
	input { name { input1 } type { FLOAT_MATRIX_ENV } }
	input { name { input2 } type { FLOAT_MATRIX_ENV } }
	input { name { input3 } type { FLOAT_MATRIX_ENV } }
	output { name { output1 } type { FLOAT_MATRIX_ENV } }
	output { name { output2 } type { FLOAT_MATRIX_ENV } }
	output { name { output3 } type { FLOAT_MATRIX_ENV } }

	defstate {
		name	{ CCIR_601 }
		type	{ int }
		default { "YES" }
		desc	{
TRUE means that the RGB space will be converted to shifted-and-scaled YUV
color space defined by the CCIR 601 standard, which is used in the MPEG
and H.261 standards; otherwise, the RGB space will be converted according
to the usual YUV definition.
		}
	}

	inline method {		// perform rounding in range [0, 255]
		name { quant }
		type { "unsigned char" }
		arglist { "(double inval)" }
		access { protected }
		code {
			if (inval < 0.5) return ((unsigned char) 0);
			else if (inval > 254.5) return ((unsigned char) 255);
			else return ((unsigned char) int(inval + 0.5));
		}
	} // end quant()

	go {
		// Read inputs.
                Envelope rpkt,gpkt,bpkt;
                (input1%0).getMessage(rpkt);
                (input2%0).getMessage(gpkt);
                (input3%0).getMessage(bpkt);

		// Change into YUV format
                const FloatMatrix& redI = *(const FloatMatrix*)rpkt.myData();
                const FloatMatrix& greenI = *(const FloatMatrix*)gpkt.myData();
                const FloatMatrix& blueI = *(const FloatMatrix*)bpkt.myData();

		int height = redI.numRows();
		int width  = redI.numCols();

		if ((greenI.numCols() != width) ||
		    (greenI.numRows() != height) ||
		    (blueI.numCols() != width) ||
		    (blueI.numRows() != height)) {
			Error::abortRun(*this,
					"Input image sizes don't match.");
			return;
		}

		// FIXME
		// Sets redImg to the vector representing the FloatMatrix redI, 
		// greenImg to greenI, and blueImg to blueI.
		// This only works because in the underlying implementation of 
		// FloatMatrix, redI[0] for example, which returns the 1st row of the 
		// matrix, also returns the entire vector representing the matrix. 
		// A method should be added to the FloatMatrix class to do this instead
		// of relying on this current operation
		const double * redImg = redI[0];
		const double * greenImg = greenI[0];
		const double * blueImg = blueI[0];

		int size = height * width;
		double * YImg = new double[size];
		double * UImg = new double[size];
		double * VImg = new double[size];
		
		Ptdsp_RGBToYUV (redImg, greenImg, blueImg, YImg, UImg,
				VImg, width, height, CCIR_601);

		FloatMatrix& yI = *(new FloatMatrix(height, width));
		FloatMatrix& uI = *(new FloatMatrix(height, width));
		FloatMatrix& vI = *(new FloatMatrix(height, width));

		for (int i = 0; i < size; i++) {
		  yI.entry(i) = YImg[i];
		  uI.entry(i) = UImg[i];
		  vI.entry(i) = VImg[i];
		}
		
		delete [] YImg;
		delete [] UImg;
		delete [] VImg;

		// Write whole frame to output here...
		output1%0 << yI;
		output2%0 << uI;
		output3%0 << vI;
	} // end go{}
} // end defstar{ Rgb2Yuv }


