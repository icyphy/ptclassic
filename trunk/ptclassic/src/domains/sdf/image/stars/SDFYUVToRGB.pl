 defstar {
//////// INFO ON STAR.
	name	{ YUVToRGB }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Sun-Inn Shih }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Read three GrayImages that describe a color image in YUV format and
output three GrayImages that describe an image in RGB format.
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
.Id "format conversion, YUV to RGB"
.Id "image format conversion, YUV to RGB"
.Ir "image format, red-green-blue (RGB)"
.Ir "image format, luminance-chrominance (YUV)"
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

	inline method {		// perform rounding in range [0, 255]
		name { quant }
		type { "unsigned char" }
		arglist { "(float inval)" }
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
		unsigned char rrr, ggg, bbb;
		for (i = 0; i < height; i++) {
			temp1 = i * width;
			for (j = 0; j < width; j++){
				temp2 = j + temp1;
				rrr = quant(rptr[temp2]
						+ 1.4026 * (bptr[temp2]-127.5));
				ggg = quant(rptr[temp2]
						- 0.3444 * (gptr[temp2]-127.5)
						- 0.7144 * (bptr[temp2]-127.5));
				bbb = quant(rptr[temp2]
						+ 1.773 * (gptr[temp2]-127.5));
				rptr[temp2] = rrr;
				gptr[temp2] = ggg;
				bptr[temp2] = bbb;
		}	}

// Write whole frame to output here...
		Envelope envpy(*redI);
		Envelope envpu(*greenI);
		Envelope envpv(*blueI);
		output1%0 << envpy;
		output2%0 << envpu;
		output3%0 << envpv;
	} // end go{}
} // end defstar{ Yuv2Rgb }
