defstar {
	name		{ Dpcm }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
For the first input image, copy the image unchanged to the output.
For all other inputs, subtract the past frame from the new frame
and send the difference to the output.
	}

	hinclude { "GrayImage.h", "Error.h" }

code {
// This star code uses "wraparound" to represent negative values
// using unsigned char's.  Suppose unsigned char a = 3 and unsigned
// char b = 4.  Then unsigned char (a-b) == 255.  BUT, (a-b) + b == 3,
// which is what we need.  Note that the quant() function does not
// do thresholding for negative floating-point values.
}

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
		desc { Leak value for error-recovery. }
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
		name { getInput }
		type { "const GrayImage*" }
		access { protected }
		arglist { "(Packet& inPkt)" }
		code {
			(input%0).getPacket(inPkt);
			if (badType(*this,inPkt,"GrayImage")) {
				return NULL;
			}
			return( (const GrayImage*) inPkt.myData() );
		}
	} // end getInput()

	method { // Don't get rid of negative values--use "wraparound".
		name { quant }
		access { protected }
		type { "unsigned char" }
		arglist { "(float val)" }
		code {
			return((unsigned char) (val + 0.5));
		}
	}

	method {
		name	{ doFirstImage }
		access	{ protected }
		type	{ "void" }
		arglist	{ "(const GrayImage* inImage)" }
		code {
			width  = inImage->retWidth();
			height = inImage->retHeight();
			firstTime = 0;

// write first input unchanged.
			Packet pkt( *inImage );
			output%0 << pkt;
		}
	} // end doFirstImage()

	go {
// Read data from input.
		Packet pastPkt = storPkt;
		const GrayImage* inImage = getInput(storPkt);

// Initialize if this is the first input image.
		if (firstTime) {
			doFirstImage(inImage);
			return;
		}

// If we reached here, we already have read one image.
// Put the difference into the past image (which we won't need after
// this go{} call), and then send the past image to the output.
// Call clone(int) rather than clone() to avoid copying data.
		GrayImage* diffImage = (GrayImage*) inImage->clone(1);
		unsigned char* dif = diffImage->retData();
		unsigned const char* cur = inImage->constData();
		unsigned const char* prv =
			((const GrayImage*) pastPkt.myData())->constData();

		for(int travel = 0; travel < width*height; travel++) {
			dif[travel] = quant(cur[travel] - leak*float(prv[travel]));
		}

// Send the outputs on their way, and save new input for next time.
		Packet outPkt(*diffImage);
		output%0 << outPkt;
// pastPkt goes out of scope now, decrementing its contents' reference
// count.
	} // end go{}
} // end defstar { Dpcm }
