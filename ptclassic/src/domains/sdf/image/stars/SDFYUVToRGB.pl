 defstar {
//////// INFO ON STAR.
	name	{ Yuv2Rgb }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Sun-Inn Shih }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Read three packets that describe a color image in YUV format, and output
three packets that describe an image in RGB format.
The inputs and outputs are packets of type GrayImage.
	}
	explanation {
.Id "format conversion, YUV to RGB"
.Id "image format conversion, YUV to RGB"
.Ir "image format, RGB"
.Ir "image format, YUV"
	}

	ccinclude { "GrayImage.h", "Error.h" }

//////// OUTPUTS AND STATES.
	input {
		name { input1 }
		type { packet }
	}
	input {
		name { input2 }
		type { packet }
	}
	input {
		name { input3 }
		type { packet }
	}
	output {
		name { output1 }
		type { packet }
	}
	output {
		name { output2 }
		type { packet }
	}
	output {
		name { output3 }
		type { packet }
	}

	inline method {
		name { quant }
		type { "unsigned char" }
		arglist { "(float inval)" }
		access { protected }
		code {
			if (inval < 0.5) return ((unsigned char) 0);
			else if (inval > 254.5) return ((unsigned char) 255);
			else return ((unsigned char) int(inval+0.5));
		}
	} // end quant()

	go {
// Read inputs.
		Packet pkt1, pkt2, pkt3;
		(input1%0).getPacket(pkt1);
		(input2%0).getPacket(pkt2);
		(input3%0).getPacket(pkt3);
		TYPE_CHECK(pkt1, "GrayImage");
		TYPE_CHECK(pkt2, "GrayImage");
		TYPE_CHECK(pkt3, "GrayImage");

// Change into RGB format
		GrayImage* redI = (GrayImage*) pkt1.writableCopy();
		GrayImage* greenI = (GrayImage*) pkt2.writableCopy();
		GrayImage* blueI = (GrayImage*) pkt3.writableCopy();
		if (redI->fragmented() || redI->processed() ||
				greenI->fragmented() || greenI->processed() ||
				blueI->fragmented() || blueI->processed()) {
			delete redI; delete greenI; delete blueI;
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
			delete redI; delete greenI; delete blueI;
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
						+ 1.4026 * (bptr[temp2]-128) + 0.5);
				ggg = quant(rptr[temp2]
						- 0.3444 * (gptr[temp2]-128)
						- 0.7144 * (bptr[temp2]-128) + 0.5);
				bbb = quant(rptr[temp2]
						+ 1.773 * (gptr[temp2]-128) + 0.5);
				rptr[temp2] = rrr;
				gptr[temp2] = ggg;
				bptr[temp2] = bbb;
		}	}

// Write whole frame to output here...
		Packet pkty(*redI);
		Packet pktu(*greenI);
		Packet pktv(*blueI);
		output1%0 << pkty;
		output2%0 << pktu;
		output3%0 << pktv;
	} // end go{}
} // end defstar{ Yuv2Rgb }
