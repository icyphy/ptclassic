defstar {
	name { MxToImage }
	domain { DE }
	version { @(#)DEMxToImage.pl	1.1 2/22/96 }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
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
	protected {
		int np, id;
	}
	setup {

	}
	go {
	  if (input.dataNew && frameId.dataNew) {
	    completionTime = arrivalTime;

	    Envelope pkt;
	    input.get().getMessage(pkt);
	    const FloatMatrix& matrix = *(const FloatMatrix*)pkt.myData();

	    int width = matrix.numCols();
	    int height = matrix.numRows();

	    int id = int(frameId.get());
	    LOG_NEW; GrayImage* imgData = new GrayImage(width, height, id);
	    unsigned char* d = imgData->retData();
	    
	    int	np = int(width)*int(height);
	    for (int i = 0; i < np; i++) {
	      if (matrix.entry(i) < 0)
		*d++ = 0;
	      else if(matrix.entry(i) > 255)
		*d++ = 255;
	      else *d++ = int(matrix.entry(i)); // convert to int
	    }
	    Envelope envp(*imgData);
	    output.put(completionTime) << envp;
	  }
	}
}
