defstar {
	name { ImageToMx }
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
Accept a black-and-white image from an input image packet, and copy
the individual pixels to a FloatMatrix.  Note that even thought the
GrayImage input contains all integer values, we convert to a FloatMatrix
to allow easier manipulation.
	}
	ccinclude { "GrayImage.h" }
	ccinclude { "Matrix.h" }
	input {
		name { input }
		type { message }
	}
	output {
		name { dataOutput }
		type { FLOAT_MATRIX_ENV }
	}
	output {
		name { frameIdOutput }
		type { int }
	}
	output {
		name { widthOutput }
		type { int }
	}
	output {
		name { heightOutput }
		type { int }
	}
	output {
		name { sizeOutput }
		type { int }
	}
	defstate {
		name { width }
		type { int }
		default { 100 }
		desc { width of image = the number of columns in the matrix }
	}
	defstate {
		name { height }
		type { int }
		default { 100 }
		desc { height of image = the number of rows in the matrix }
	}
	protected {
		int np;
	}
	setup {
		np = int(width) * int(height);
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
                FloatMatrix& matrix = *(new FloatMatrix(int(height),int(width)));
		for (int i = 0; i < np; i++)
			matrix.entry(i) = double(int(*p++));
                dataOutput%0 << matrix;
	        frameIdOutput%0 << imD->retId();
	        widthOutput%0 << imD->retWidth();
	        heightOutput%0 << imD->retHeight();
	        sizeOutput%0 << imD->retSize();
	}
}
