 defstar {
//////// INFO ON STAR.
	name	{ RGBToYUV }
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
Read three GrayImages that describe a color image in RGB format and
output three GrayImages that describe an image in YUV format.
No downsampling is done on the U and V signals.
The inputs and outputs are all GrayImages.
	}
	explanation {
.Id "format conversion, RGB to YUV"
.Id "image format conversion, RGB to YUV"
.Ir "image format, RGB"
.Ir "image format, YUV"
	}

	ccinclude { "GrayImage.h", "Error.h" }

//////// OUTPUTS AND STATES.
	input { name { input1 } type { message } }
	input { name { input2 } type { message } }
	input { name { input3 } type { message } }
	output { name { output1 } type { message } }
	output { name { output2 } type { message } }
	output { name { output3 } type { message } }

	inline method {
		name { quant }
		type { "unsigned char" }
		arglist { "(const float inval)" }
		access { protected }
		code {
			if (inval < 0.5) return ((unsigned char) 0);
			else if (inval > 254.5) return ((unsigned char) 255);
			else return ((unsigned char) int(inval+0.5));
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

		if (redI->fragmented() || redI->processed() ||
				greenI->fragmented() || greenI->processed() ||
				blueI->fragmented() || blueI->processed()) {
			LOG_DEL; delete redI; LOG_DEL; delete greenI;
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
			LOG_DEL; delete redI; LOG_DEL; delete greenI;
			LOG_DEL; delete blueI;
			Error::abortRun(*this,
					"Input image sizes don't match.");
			return;
		}

		unsigned char* rptr = redI->retData();
		unsigned char* gptr = greenI->retData();
		unsigned char* bptr = blueI->retData();

		int i, j, temp1, temp2;
		unsigned char yyy, uuu, vvv;
		for (i = 0; i < height; i++) {
			temp1 = i*width;
			for (j = 0; j < width; j++){
				temp2 = j + temp1;
				yyy = quant(0.298981 * rptr[temp2]
						+ 0.586997 * gptr[temp2]
						+ 0.114022 * bptr[temp2] + 0.5);
				uuu = quant(-0.16863 * rptr[temp2]
						- 0.331075 * gptr[temp2]
						+ 0.499705 * bptr[temp2] + 128.5);
				vvv = quant(0.4998 * rptr[temp2]
						- 0.418506 * gptr[temp2]
						- 0.0812936 * bptr[temp2] + 128.5);
				rptr[temp2] = yyy;
				gptr[temp2] = uuu;
				bptr[temp2] = vvv;
		}	}

// Write whole frame to output here...
		Envelope envpy(*redI);
		Envelope envpu(*greenI);
		Envelope envpv(*blueI);
		output1%0 << envpy;
		output2%0 << envpu;
		output3%0 << envpv;
	} // end go{}
} // end defstar{ Rgb2Yuv }
