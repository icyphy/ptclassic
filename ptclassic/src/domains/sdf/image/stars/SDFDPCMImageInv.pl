defstar {
	name		{ DpcmInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ SDF image library }
	desc {
If the 'past' input is not a GrayImage or has size 0, pass the 'diff'
directly to the 'output'. Otherwise, add the 'past' to the
'diff' (with leak factor 'alpha') and send the result to 'output'.
	}
	seealso { Dpcm }

	hinclude { "GrayImage.h", "Error.h" }

//////// I/O AND STATES.
	input { name { diff } type { packet } }
	input { name { past } type { packet } }
	output { name { output } type { packet } }

	defstate {
		name { alpha }
		type { float }
		default { 1.0 }
		desc { Leak value to aid error recovery. }
	}

	protected { float leak; }
	start { leak = float(double(alpha)); }

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
		Packet diffPkt, pastPkt;
		(diff%0).getPacket(diffPkt);
		(past%0).getPacket(pastPkt);
		TYPE_CHECK(diffPkt, "GrayImage");
		GrayImage* inImage = (GrayImage*) diffPkt.writableCopy();

// Resynchronize if 'past' of wrong type.
		if(!pastPkt.typeCheck("GrayImage")) {
			Packet tmp(*inImage); output%0 << tmp;
			return;
		}
		const GrayImage* pastImage =
				(const GrayImage*) pastPkt.myData();

// Resynchronize because past.size() = 0
		if (!pastImage->retSize()) {
			Packet tmp(*inImage); output%0 << tmp;
			return;
		}

		unsigned char* dif = inImage->retData();
		unsigned const char* prev = pastImage->constData();
		for(int travel = inImage->retWidth() * inImage->retHeight() - 1;
				travel >= 0; travel--) {
			dif[travel] = quant(dif[travel], prev[travel]);
		}

// Send the outputs on their way.
		Packet outPkt(*inImage); output%0 << outPkt;
	}
} // end defstar { DpcmInv }
