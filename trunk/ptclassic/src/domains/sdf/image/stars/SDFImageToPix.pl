defstar {
	name { ImageToPix }
	domain { SDF }
	version { $Id$ }
	author { J. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF image library }
	desc {
Accept a black-and-white image from an input packet, and generate the
individual pixels.
	}
	ccinclude { "GrayImage.h" }
	input {
		name { input }
		type { message }
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
	setup {
		np = int(width) * int(height);
		output.setSDFParams(np, np-1);
	}
	go {
		Envelope envp;
		(input%0).getMessage(envp);
		TYPE_CHECK(envp, "GrayImage");
		const GrayImage* imD = (const GrayImage*) envp.myData();
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
