 defstar {
//////// INFO ON STAR.
	name	{ YUVToRGB }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Sun-Inn Shih, Brian L. Evans, and T. J. Klausutis }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Read three GrayImages that describe a color image in YUV format and
output three GrayImages that describe an image in RGB format.
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

	ccinclude { "GrayImage.h", "Error.h" }

//////// OUTPUTS AND STATES.
	input { name { input1 } type { message } }
	input { name { input2 } type { message } }
	input { name { input3 } type { message } }
	output { name { output1 } type { message } }
	output { name { output2 } type { message } }
	output { name { output3 } type { message } }

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
		Envelope envp1, envp2, envp3;
		(input1%0).getMessage(envp1);
		(input2%0).getMessage(envp2);
		(input3%0).getMessage(envp3);
		TYPE_CHECK(envp1, "GrayImage");
		TYPE_CHECK(envp2, "GrayImage");
		TYPE_CHECK(envp3, "GrayImage");

		// Change into RGB format
		GrayImage* redI = (GrayImage*) envp1.writableCopy();
		GrayImage* greenI = (GrayImage*) envp2.writableCopy();
		GrayImage* blueI = (GrayImage*) envp3.writableCopy();
		if (redI->fragmented() || redI->processed() ||
		    greenI->fragmented() || greenI->processed() ||
		    blueI->fragmented() || blueI->processed()) {
			LOG_DEL; delete redI; LOG_DEL; delete greenI;
			LOG_DEL; delete blueI;
			Error::abortRun(*this,
					"Need unfragmented and unprocessed inputs.");
			return;
		}
		const int width = redI->retWidth();
		const int height = redI->retHeight();
		if ((greenI->retWidth() != width) ||
		    (greenI->retHeight() != height) ||
		    (blueI->retWidth() != width) ||
		    (blueI->retHeight() != height)) {
			LOG_DEL; delete redI; LOG_DEL; delete greenI;
			LOG_DEL; delete blueI;
			Error::abortRun(*this,
					"Not all input images are same size.");
			return;
		}

		unsigned char* rptr = redI->retData();
		unsigned char* gptr = greenI->retData();
		unsigned char* bptr = blueI->retData();

		int i, j, temp1, temp2;
		double rvalue, gvalue, bvalue;
		double yvalue, uvalue, vvalue;
		for (i = 0; i < height; i++) {
			temp1 = i * width;
			for (j = 0; j < width; j++){
				temp2 = j + temp1;
				yvalue = rptr[temp2];
				uvalue = gptr[temp2];
				vvalue = bptr[temp2];
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
				rptr[temp2] = quant(rvalue);
				gptr[temp2] = quant(gvalue);
				bptr[temp2] = quant(bvalue);
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
} // end defstar{ Yuv2Rgb }
