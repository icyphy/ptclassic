 defstar {
//////// INFO ON STAR.
	name	{ YUVToRGB }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Sun-Inn Shih, Brian L. Evans, and T. J. Klausutis }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Read three float matrices that describe a color image in YUV format and
output three float matrices that describe an image in RGB format.
	}
	explanation {
The YUV format, which is an affine mapping (linear mapping plus offset)
of the RGB format, is used in broadcast television to maintain compability
between color and black-and-white televisions [1].
Y is the luminance (intensity) of the image, and U and V represent the
chrominance (hue and saturation).
The YUV format is based on how the eyes perceive color.
The U axis has yellow at its minimum point and blue at its maximum point,
whereas the V axis has turquoise at its minimum point and red at its
maximum point.
In this implementation, each of the RGB values and each of the YUV values
are integer values in the range from 0 to 255 (inclusive).
.pp
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
.Id "format conversion, YUV to RGB"
.Id "image format conversion, YUV to RGB"
.Ir "image format, red-green-blue (RGB)"
.Ir "image format, luminance-chrominance (YUV)"
.Id "CCIR 601 standard"
.Id "MPEG"
.Id "H.261"
.Id "Pratt, W."
.UH REFERENCES
.ip [1]
W. Pratt, \fIDigital Image Processing\fR,
Wiley & Sons: New York.  1991.  2nd ed.
	}

	ccinclude { "Matrix.h", "Error.h" }

//////// OUTPUTS AND STATES.
	input { name { input1 } type { FLOAT_MATRIX_ENV } }
	input { name { input2 } type { FLOAT_MATRIX_ENV } }
	input { name { input3 } type { FLOAT_MATRIX_ENV } }
	output { name { output1 } type { FLOAT_MATRIX_ENV } }
	output { name { output2 } type { FLOAT_MATRIX_ENV } }
	output { name { output3 } type { FLOAT_MATRIX_ENV } }

        defstate {
                name    { CCIR_601 }
                type    { int }
                default { "YES" }
                desc    {
TRUE means that the YUV color space is the shifted-and-scaled YUV space
defined by the CCIR 601 standard, which is used in the MPEG and H.261
standards; otherwise, the usual YUV space is meant. }
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
                Envelope ypkt,upkt,vpkt;
                (input1%0).getMessage(ypkt);
                (input2%0).getMessage(upkt);
                (input3%0).getMessage(vpkt);

		// Change into RGB format
                const FloatMatrix& yI = *(const FloatMatrix*)ypkt.myData();
                const FloatMatrix& uI = *(const FloatMatrix*)upkt.myData();
                const FloatMatrix& vI = *(const FloatMatrix*)vpkt.myData();

		int height = yI.numRows();
		int width  = yI.numCols();

		if ((uI.numCols() != width) ||
		    (uI.numRows() != height) ||
		    (vI.numCols() != width) ||
		    (vI.numRows() != height)) {
			Error::abortRun(*this,
					"Input image sizes don't match.");
			return;
		}

		LOG_NEW;
		FloatMatrix& rI = *(new FloatMatrix(yI));
		LOG_NEW;
		FloatMatrix& gI = *(new FloatMatrix(uI));
		LOG_NEW;
		FloatMatrix& bI = *(new FloatMatrix(vI));

		int i, j, temp1, temp2;
		double rvalue, gvalue, bvalue;
		double yvalue, uvalue, vvalue;
		for (i = 0; i < height; i++) {
			temp1 = i * width;
			for (j = 0; j < width; j++){
				temp2 = j + temp1;
				yvalue = rI.entry(temp2);
				uvalue = gI.entry(temp2);
				vvalue = bI.entry(temp2);
				if ( int(CCIR_601) ) {
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
				rI.entry(temp2) = quant(rvalue);
				gI.entry(temp2) = quant(gvalue);
				bI.entry(temp2) = quant(bvalue);
			}
		}

		// Write whole frame to output here...
		output1%0 << rI;
		output2%0 << gI;
		output3%0 << bI;
	} // end go{}
} // end defstar{ Yuv2Rgb }
