defstar {
	name		{ DpcmInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
For the first input image, copy the image unchanged to the output.
For all other inputs, add the past frame to the new difference frame
and send the result to the output.

The 'alpha' parameter should be set to the same value as the 'alpha'
parameter in the corresponding DPCM star.  If alpha is 0, no
differencing is done.  If alpha is 1, standard DPCM is done.  For
alpha values between 0 and 1, "leaky DPCM" is done, which helps correct
for errors in the transmission of the difference frames.
	}

	hinclude { "GrayImage.h", "Error.h" }

//////// I/O AND STATES.
	input {
		name { input }
		type { packet }
	}
	output {
		name { output }
		type { packet }
	}
	defstate {
		name { alpha }
		type { float }
		default { 1.0 }
		desc { Leak value to aid error recovery. }
	}

////// CODE.
	protected {
		Packet storPkt;
		int firstTime; // True if we have not received any inputs yet.
		int width, height;
		float leak;
	}

	start { firstTime = 1; leak = float(double(alpha)); }

	method {
		name { quant }
		type { "unsigned char" }
		access { protected }
		arglist { "(float val)" }
		code {
			return ((unsigned char) (val + 0.5));
		}
	}

	method {
		name { getInput }
		type { "GrayImage*" }
		access { protected }
		code {
			Packet pkt;
			(input%0).getPacket(pkt);
			if (!StrStr(pkt.dataType(), "GrayI")) {
				Error::abortRun(*this, pkt.typeError("GrayI"));
				return((GrayImage*) NULL);
			}
			return( (GrayImage*) pkt.writableCopy() );
		}
	} // end getInput()

	method {
		name	{ doFirstImage }
		access	{ protected }
		type	{ "void" }
		arglist	{ "(GrayImage* inImage)" }
		code {
			width  = inImage->retWidth();
			height = inImage->retHeight();
			firstTime = 0;

			Packet pkt(*inImage);
			storPkt = pkt;		// store for use next time.
			output%0 << pkt;	// write first input unchanged.
		}
	} // end doFirstImage()

	go {
// Read data from input.
		GrayImage* inImage = getInput();

// Initialize if this is the first input image.
		if (firstTime) {
			doFirstImage(inImage);
			return;
		}

// If we reached here, we already have read one image.
// Put the difference into the past image (which we won't need after
// this go{} call), and then send the past image to the output.
		unsigned char *cur = inImage->retData();
		unsigned char *prev =
				((GrayImage*) storPkt.myData())->retData();
		for(int travel = 0; travel < width*height; travel++) {
			cur[travel] = quant(cur[travel] + leak*float(prev[travel]));
		}

// Send the outputs on their way.
		Packet outPkt(*inImage);
		storPkt = outPkt; // Forcing out storPkt's old contents.
		output%0 << outPkt;
	} // end go{}
} // end defstar { DpcmInv }
