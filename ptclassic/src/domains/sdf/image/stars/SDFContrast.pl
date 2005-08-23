defstar {
    name {Contrast}
    domain {SDF}
    desc {
Enhance the contrast in input image by histogram modification.
Input image should be in an integer matrix.
The possible contrast type are : Uniform, Hyperbolic.
    }
    version { @(#)SDFContrast.pl	1.5 7/15/96 }
    author { Bilung Lee }
    acknowledge { Brian L. Evans }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { SDF image library }
    input {
        name {input}
        type {INT_MATRIX_ENV}
    }
    output {
        name {output}
        type {INT_MATRIX_ENV}
    }
    defstate {
	name {contrastType}
	type {string}
	default {"Uniform"}
	desc {Type of the histogram modification to be used.}
    }
    defstate {
	name {maxInMatrix}
	type {int}
	default {"255"}
	desc {Maximun value in input matrix.}
    }
    defstate {
	name {minInMatrix}
	type {int}
	default {"0"}
	desc {Minimum value in input matrix.}
    }
    ccinclude { <string.h>, "Matrix.h", "ptdspHistogram.h" }
    protected {
        int max, min;
        int *histBuf, *cumulativeHistBuf;
    }
    constructor {
	histBuf = 0;
	cumulativeHistBuf= 0;
    }
    setup {
        max = int(maxInMatrix);
        min = int(minInMatrix);
	if ( min > max ) {
	   Error::abortRun(*this,
			   "the minInMatrix parameter must be less than "
			   "the maxInMatrix parameter");
	   return;
	}
        const char *ct = contrastType;
        if ( (strcasecmp(ct, "Uniform") != 0) &&
	     (strcasecmp(ct, "Hyperbolic") != 0) ) {
	   Error::abortRun(*this,
			   "Invalid contrastType: must be either Uniform "
			   "or Hyperbolic.");
	   return;
	}
	delete [] histBuf;
	histBuf = new int[max - min + 1];
	delete [] cumulativeHistBuf;
	cumulativeHistBuf = new int[max - min + 1];
    }
    go {
        Envelope pkt;
        (input%0).getMessage(pkt);
        const IntMatrix& inMatrix = *(const IntMatrix*)pkt.myData();

        if(pkt.empty()) {
          // input empty, send out a zero matrix
          output%0 << int(0);
        } else {
          // valid input matrix
          int height = inMatrix.numRows();
          int width  = inMatrix.numCols();


	  // FIXME
	  // Sets matrix to the vector representing the FloatMatrix inMatrix 
	  // This only works because in the underlying implementation of 
	  // FloatMatrix, inMatrix[0] for example, which returns the 1st row of the 
	  // matrix, also returns the entire vector representing the matrix. 
	  // A method should be added to the FloatMatrix class to do this instead
	  // of relying on this current operation
	  const int * matrix = inMatrix[0];

	  // Compute the histBuf
	  Ptdsp_Histogram(matrix, width*height, histBuf, min, max);

	  // Compute cumulative histogram
	  int histlen = max - min + 1;
	  cumulativeHistBuf[0] = histBuf[0];
	  for (int k = 1; k < histlen; k++) {
	    cumulativeHistBuf[k] = cumulativeHistBuf[k-1] + histBuf[k];
	  }

          IntMatrix& result = *(new IntMatrix(inMatrix));
          const char *ct = contrastType;
          if ( strcasecmp(ct, "Uniform") == 0 ) { 
 	    // Use uniform histogram modification transfer function
	    int wh = width*height;
	    double scale = double(max - min) / double(wh);
	    double value = 0.0;
	    for (int i = 0; i < wh; i++) {
	      value = scale * cumulativeHistBuf[matrix[i]] + min;
	      result.entry(i) = int(value);
	    }    
          }
	  // The setup method flags invalid contrast types, so
	  // else if ( strcasecmp(ct, "Hyperbolic") == 0 )
	  // simply becomes as else statement
	  else {
	    // Use Hyperbolic histogram modification transfer function
	    int wh = width*height;
	    double min1_3 = pow(min, 1/3.0);
	    double max1_3 = pow(max, 1/3.0);
	    double scale = (max1_3 - min1_3) / double(wh);
	    double value = 0.0;
	    for (int i = 0; i < wh; i++) {
	      value = scale * cumulativeHistBuf[matrix[i]] + min1_3;
	      result.entry(i) = int(value * value * value);
	    }
          }

          output%0 << result;
	}  // end of valid input matrix.
        
    } 

    destructor {
	delete [] histBuf;
	delete [] cumulativeHistBuf;
    }
}
