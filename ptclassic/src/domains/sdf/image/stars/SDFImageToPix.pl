defstar {
	name { ImageToPix }
	domain { SDF }
	version { $Id$ }
	author { J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { SDF image library }
	desc {
Accept a black-and-white image from an input packet, and generate the
individual pixels.
	}
	ccinclude { "GrayImage.h" }
	input {
		name { input }
		type { packet }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { width }
		type { int }
		default { 100 }
		desc { width of image }
	}
	defstate {
		name { height }
		type { int }
		default { 100 }
		desc { height of image }
	}
	protected {
		int np;
	}
	start {
		np = int(width) * int(height);
		output.setSDFParams(np, np-1);
	}
	go {
		Packet pkt;
		(input%0).getPacket(pkt);
		TYPE_CHECK(pkt, "GrayImage");
		const GrayImage* imD = (const GrayImage*) pkt.myData();
		if (imD->retWidth() != int(width) ||
			imD->retHeight() != int(height)) {
			Error::abortRun(*this, "Got image with wrong size");
			return;
		}
		if (imD->fragmented() || imD->processed()) {
			Error::abortRun(*this,
					"Can't process fragmented or processed images.");
			return;
		}
		unsigned const char* p = imD->constData();
		// 0 is oldest.
		for (int i = np-1; i >= 0; i--)
			output%i << int(*p++);
	}
}
