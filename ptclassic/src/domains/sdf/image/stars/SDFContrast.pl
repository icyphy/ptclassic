defstar {
    name {Contrast}
    domain {SDF}
    desc {
Enhance the contrast in input image by histogram modification.
Input image should be in a int matrix.
The possible contrast type are : Uniform, Hyperbolic.
    }
    version { $Id$ }
    author { Bilung Lee }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
	desc {Minimun value in input matrix.}
    }
    ccinclude {"Matrix.h"}
    protected {
        int max, min;

        int *hist, *cum_hist;
    }
    code {
	extern "C" {
	    extern int strcasecmp(const char*,const char*);
	}
    }
    ccinclude {"Matrix.h"}
    setup {
        max = int(maxInMatrix);
        min = int(minInMatrix);
    }
    go {
        int i, j;
        double value;

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
          LOG_NEW; int *matrix = new int[height*width];

          for (i=0; i<height*width; i++) {
	    matrix[i] = inMatrix.entry(i);
          }

	  /* Allocate space for histogram and compute it. */
	  hist = (int *)calloc(max-min+1, sizeof(int));
	  histogram(hist,matrix,width*height,min,max);

	  /* Allocate space for cumulative histogram and compute it. */
	  cum_hist = (int *)calloc(max-min+1, sizeof(int));
	  for (i=0; i<(max-min+1); i++) {
	    cum_hist[i] = 0;
	    for (j=0; j<=i; j++) {
	      cum_hist[i] += hist[j];
	    }
	  }

          IntMatrix& result = *(new IntMatrix(inMatrix));
          const char *ct = contrastType;
          if ( strcasecmp( ct, "Uniform")==0 ) { 
 	    /* Use uniform histogram modification transfer function. */
	    for (i=0; i<width*height; i++) {
	      value = (max-min)*cum_hist[matrix[i]]/(width*height)+min;
	      result.entry(i) = (int) value;
	    }    
          } else if ( strcasecmp( ct, "Hyperbolic")==0 ) {
	    /* Use Hyperbolic histogram modification transfer function. */
	    for (i=0; i<width*height; i++) {
	      value = (pow(max,1/3.0)-pow(min,1/3.0))*cum_hist[matrix[i]];
              value /= width*height;
	      value += pow(min,1/3.0);
	      result.entry(i) = (int) pow(value,3);
	    }
          } else {
	    Error::abortRun(*this, ": Unknown contrast type.");
	    return;
	  }
          
          output%0 << result;
          LOG_DEL;  delete [] matrix;
          free((char *)hist);
          free((char *)cum_hist);
	}  // end of valid input matrix.
        
    } 

    // Procedure to compute the histogram.
    method {
        name {histogram}
        access { private }
        arglist {"(int *hist,int *matrix,int size,int min,int max)"}
        type {void}
        code {
          int i;
          int index;

          /* Initialize to zero. */
          for (i=0; i<(max-min+1); i++) {
            hist[i] = 0;
          }  

          /* Compute the histogram. */
          for (i=0; i<size; i++) {
            /* If the value is larger than max, then is counted into max. */
            index = (matrix[i]>max) ? max : matrix[i]; 
            /* If the value is smaller than min, then is counted into min. */
            index = (matrix[i]<min) ? min : matrix[i]; 

            hist[index-min]++;
          }

        }
    }  //end of method histogram.
}
