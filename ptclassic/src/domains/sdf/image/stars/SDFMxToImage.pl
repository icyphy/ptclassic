defstar {
	name { MxToImage }
	domain { SDF }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF image library }
	desc {
Accept FloatMatrix input and creates a GrayImage output.  Note that the
double values of the FloatMatrix are converted to the integer values
of the GrayImage representation.
	}
	ccinclude { "GrayImage.h" }
	ccinclude { "Matrix.h" }
	input {
		name { input }
		type { FLOAT_MATRIX_ENV }
	}
	input {
		name { frameId }
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
	setup {
		np = int(width)*int(height);
	}
	go {
		id = int(frameId%0);
		LOG_NEW; GrayImage* imgData = new GrayImage(width, height, id);
		unsigned char* d = imgData->retData();

                Envelope pkt;
                (input%0).getMessage(pkt);
                const FloatMatrix& matrix = *(const FloatMatrix*)pkt.myData();
		for (int i = 0; i < np; i++) {
			if (matrix.entry(i) < 0)
				*d++ = 0;
			else if(matrix.entry(i) > 255)
				*d++ = 255;
			else *d++ = int(matrix.entry(i)); // convert to int
		}
		Envelope envp(*imgData);
		output%0 << envp;
	}
}
