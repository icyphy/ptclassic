 defstar {
//////// INFO ON STAR.
	name	{ RGBToYUV }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Sun-Inn Shih, Brian L. Evans, and T. J. Klausutis }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF image library }
	desc {
Read three GrayImages that describe a color image in Red-Green-Blue (RGB)
format and output three GrayImages that describe an image in YUV format.
No downsampling is performed on the U and V signals.
	}
	explanation {
The YUV format, which is a linear mapping of the RGB format, is used
in broadcast television to maintain compability between color and
black-and-white televisions [1].
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
.Id "format conversion, RGB to YUV"
.Id "image format conversion, RGB to YUV"
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

	ccinclude { "GrayImage.h", "Error.h" }

//////// OUTPUTS AND STATES.
	input { name { input1 } type { message } }
	input { name { input2 } type { message } }
	input { name { input3 } type { message } }
	output { name { output1 } type { message } }
	output { name { output2 } type { message } }
	output { name { output3 } type { message } }

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
		Envelope envp1, envp2, envp3;
		(input1%0).getMessage(envp1);
		(input2%0).getMessage(envp2);
		(input3%0).getMessage(envp3);
		TYPE_CHECK(envp1, "GrayImage");
		TYPE_CHECK(envp2, "GrayImage");
		TYPE_CHECK(envp3, "GrayImage");

		// Change into YUV format
		GrayImage* redI = (GrayImage*) envp1.writableCopy();
		GrayImage* greenI = (GrayImage*) envp2.writableCopy();
		GrayImage* blueI = (GrayImage*) envp3.writableCopy();

		if ( redI->fragmented() || redI->processed() ||
		     greenI->fragmented() || greenI->processed() ||
		     blueI->fragmented() || blueI->processed()) {
			LOG_DEL; delete redI;
			LOG_DEL; delete greenI;
			LOG_DEL; delete blueI;
			Error::abortRun(*this,
					"Can't handle fragmented or processed inputs.");
			return;
		}
		const int width = redI->retWidth();
		const int height = redI->retHeight();
		if ((greenI->retWidth() != width) ||
		    (greenI->retHeight() != height) ||
		    (blueI->retWidth() != width) ||
		    (blueI->retHeight() != height)) {
			LOG_DEL; delete redI;
			LOG_DEL; delete greenI;
			LOG_DEL; delete blueI;
			Error::abortRun(*this,
					"Input image sizes don't match.");
			return;
		}

		unsigned char* rptr = redI->retData();
		unsigned char* gptr = greenI->retData();
		unsigned char* bptr = blueI->retData();

		int i, j, temp1, temp2;
		double rvalue, gvalue, bvalue;
		double yvalue, uvalue, vvalue;
		for ( i = 0; i < height; i++ ) {
			temp1 = i*width;
			for ( j = 0; j < width; j++ ) {
				temp2 = j + temp1;
				rvalue = rptr[temp2];
				gvalue = gptr[temp2];
				bvalue = bptr[temp2];
				yvalue =  0.299  * rvalue +
					  0.587  * gvalue +
					  0.114  * bvalue;
				uvalue = -0.1687 * rvalue +
					 -0.3313 * gvalue +
					  0.5    * bvalue;
				vvalue =  0.5    * rvalue +
					 -0.4187 * gvalue +
					 -0.0813 * bvalue;
				if ( int(CCIR_601) ) {
				  yvalue = (219.0*yvalue)/255.0 +  16;
				  uvalue = (224.0*uvalue)/255.0 + 128;
				  vvalue = (224.0*vvalue)/255.0 + 128;
				}
				else {
				  uvalue += 128;
				  vvalue += 128;
				}
				rptr[temp2] = quant(yvalue);
				gptr[temp2] = quant(uvalue);
				bptr[temp2] = quant(vvalue);
			}
		}

		// Write whole frame to output here...
		Envelope envpy(*redI);
		Envelope envpu(*greenI);
		Envelope envpv(*blueI);
		output1%0 << envpy;
		output2%0 << envpu;
		output3%0 << envpv;
	} // end go{}
} // end defstar{ Rgb2Yuv }
