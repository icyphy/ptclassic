defstar {
	name		{ Dpcm }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ SDF image library }
	desc {
If the "past" input is not a GrayImage or has size 0, pass the "input"
directly to the "output".  Otherwise, subtract the "past" from the
"input" (with leak factor "alpha") and send the result to "output".
	}
	explanation {
.Id "image DPCM"
.Id "image compression"
.Id "DPCM, image"
.Id "compression, image"
}

	hinclude { "GrayImage.h", "Error.h" }


//////// I/O AND STATES.
	input { name { input } type { packet } }
	input { name { past } type { packet } }
	output { name { output } type { packet } }

	defstate {
		name { alpha }
		type { float }
		default { 1.0 }
		desc { Leak value for error-recovery. }
	}

	protected { float leak; }
	start { leak = float(double(alpha)); }

// This star code uses "wraparound" to represent negative values
// using unsigned char's. Suppose unsigned char a = 3 and unsigned
// char b = 4. Then unsigned char (a-b) == 255. BUT, (a-b) + b == 3,
// which is what we need. Note that the quant() function does not
// do thresholding for negative floating-point values.
// The '128' is so this star's output can be compressed with the DCT.
// Without the '128', the values +1 and -1 are represented as 1 and 255.
// The DCT can't compress very well then.
	inline method {
		name { quant }
		access { protected }
		type { "unsigned char" }
		arglist { "(float inp, float pst)" }
		code {
			return((unsigned char) (inp + 0.5 + leak * (128.0 - pst)));
	}	}

	method {
		name { inputsOk }
		type { "int" }
		access { private }
		arglist { "(const GrayImage& one, const GrayImage& two)" }
		code {
			int retval = !one.fragmented();
			retval &= !one.processed();
			retval &= !two.fragmented();
			retval &= !two.processed();
			retval &= (one.retWidth() == two.retWidth());
			retval &= (one.retHeight() == two.retHeight());
			return(retval);
	}	}

	go {
// Read data from input.
		Packet curPkt, pastPkt;
		(input%0).getPacket(curPkt);
		(past%0).getPacket(pastPkt);
		TYPE_CHECK(curPkt, "GrayImage");
		GrayImage* inImage = (GrayImage*) curPkt.writableCopy();

// Resynchronize because of non-GrayImage input.
		if (!pastPkt.typeCheck("GrayImage")) {
			Packet tmp(*inImage); output%0 << tmp;
			return;
		}
		const GrayImage* pastImage =
				(const GrayImage*) pastPkt.myData();

// Resynchronize because of size=0 input.
		if (!pastImage->retSize()) {
			Packet tmp(*inImage); output%0 << tmp;
			return;
		}

		if(!inputsOk(*inImage, *pastImage)) {
			delete inImage;
			Error::abortRun(*this, "Problem with input images.");
			return;
		}

// Handle real valid inputs...
		unsigned char* dif = inImage->retData();
		unsigned const char* prv = pastImage->constData();
		for(int travel = inImage->retWidth() * inImage->retHeight() - 1;
				travel >= 0; travel--) {
			dif[travel] = quant(dif[travel], prv[travel]);
		}

		Packet outPkt(*inImage); output%0 << outPkt;
	}
} // end defstar { Dpcm }
