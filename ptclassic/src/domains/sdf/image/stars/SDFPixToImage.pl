defstar {
	name { PixToImage }
	domain { SDF }
	version { $Id$ }
	author { J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { SDF image library }
	desc {
Accept integer values, and assemble them into an image.  Warning: this
star produces large sample rate changes, use the loop scheduler!
}
	ccinclude { "GrayImage.h" }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { message }
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
		int np, id;
	}
	start {
		np = int(width)*int(height);
		input.setSDFParams(np,np-1);
		id = 0;
	}
	go {
		LOG_NEW; GrayImage* imgData = new GrayImage(width, height, id);
		id++;
		unsigned char* d = imgData->retData();
		for (int i = np-1; i >= 0; i--) {
			int data = input%i;
			if (data < 0) data = 0;
			else if (data > 255) data = 255;
			*d++ = data;
		}
		Envelope envp(*imgData);
		output%0 << envp;
	}
}
