defstar {
	name		{ DPCMImageInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
If the "past" input is not a GrayImage or has size 0, pass the "diff"
directly to the "output". Otherwise, add the "past" to the
"diff" (with leak factor "alpha") and send the result to "output".
	}
	explanation {
.Id "image decompression"
.Id "image DPCM, inverse"
.Id "DPCM, inverse"
.Id "inverse DPCM"
.Id "decompression, image"
	}
	seealso { DPCMImage }

	hinclude { "GrayImage.h", "Error.h" }

//////// I/O AND STATES.
	input { name { diff } type { message } }
	input { name { past } type { message } }
	output { name { output } type { message } }

	defstate {
		name { alpha }
		type { float }
		default { 1.0 }
		desc { Leak value to aid error recovery. }
	}

	protected { float leak; }
	setup { leak = float(double(alpha)); }

	inline method {
		name { quant }
		type { "unsigned char" }
		access { protected }
		arglist { "(const float dif, const float prv)" }
		code {
			return ((unsigned char) (dif + 0.5 + leak * (prv-128.0)));
	}	}

	go {
// Read data from input.
		Envelope diffEnvp, pastEnvp;
		(diff%0).getMessage(diffEnvp);
		(past%0).getMessage(pastEnvp);
		TYPE_CHECK(diffEnvp, "GrayImage");
		GrayImage* inImage = (GrayImage*) diffEnvp.writableCopy();

// Resynchronize if 'past' of wrong type.
		if(!pastEnvp.typeCheck("GrayImage")) {
			Envelope tmp(*inImage); output%0 << tmp;
			return;
		}
		const GrayImage* pastImage =
				(const GrayImage*) pastEnvp.myData();

// Resynchronize because past.size() = 0
		if (!pastImage->retSize()) {
			Envelope tmp(*inImage); output%0 << tmp;
			return;
		}

		unsigned char* dif = inImage->retData();
		unsigned const char* prev = pastImage->constData();
		for(int travel = inImage->retWidth() * inImage->retHeight() - 1;
				travel >= 0; travel--) {
			dif[travel] = quant(dif[travel], prev[travel]);
		}

// Send the outputs on their way.
		Envelope outEnvp(*inImage); output%0 << outEnvp;
	}
} // end defstar { DpcmInv }
