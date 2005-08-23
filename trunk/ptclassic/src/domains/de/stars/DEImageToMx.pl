defstar {
	name { ImageToMx }
	domain { DE }
	version { @(#)DEImageToMx.pl	1.2 3/7/96 }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
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
	go {
	  completionTime = arrivalTime;

	  Envelope envp;
	  input.get().getMessage(envp);
	  TYPE_CHECK(envp, "GrayImage");
	  const GrayImage* imD = (const GrayImage*) envp.myData();

	  if (imD->fragmented() || imD->processed()) {
	    Error::abortRun(*this,
			    "Can't process fragmented or processed images.");
	    return;
	  }

	  int width = imD->retWidth();
	  int height = imD->retHeight();
	  unsigned const char* p = imD->constData();
	  FloatMatrix& matrix = *(new FloatMatrix(int(height),int(width)));

	  int np = int(width) * int(height);
	  for (int i = 0; i < np; i++)
	    matrix.entry(i) = double(int(*p++));
	  dataOutput.put(completionTime) << matrix;
	  frameIdOutput.put(completionTime) << imD->retId();
	}
}
