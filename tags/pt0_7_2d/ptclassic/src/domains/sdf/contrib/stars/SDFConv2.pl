defstar {
    name { Conv2 }
    domain { SDF }
    desc {
This is linear 2-D convolution.
    }
    author { Biao Lu }
    acknowledge { Brian L. Evans }
    location  { SDF contrib library }
    version { @(#)SDFConv2.pl	1.1	03/24/98 }
    copyright {
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
    input {
	name { inputa }
	type { FLOAT_MATRIX_ENV }
	desc {
The output matrix from the output equation.
        }
    }
    input {
	name { inputb }
	type { FLOAT_MATRIX_ENV }
	desc { 
The second input is cloning template, which does not have the 
same size as the first input. 
        }
    }
  
    output {
	name { output }
	type { FLOAT_MATRIX_ENV }
	desc { 
Output matrix is the convolution of input a and input b. 
        }
    }
 
    ccinclude { "Matrix.h" }
    go {
	// get inputs
	    Envelope Apkt;
	(inputa%0).getMessage(Apkt);
	const FloatMatrix& Amatrix = *(const FloatMatrix *)Apkt.myData();
	
	Envelope Bpkt;
	(inputb%0).getMessage(Bpkt);
	const FloatMatrix& Bmatrix = *(const FloatMatrix *)Bpkt.myData();
 
        // check empty
	if (Apkt.empty() || Bpkt.empty()) {
	    output %0 << double(0);
	}
	else {
	    //valid input matrix;
	    int heighta = Amatrix.numRows();
	    int widtha  = Amatrix.numCols();
	    
	    int heightb = Bmatrix.numRows();
	    int widthb  = Bmatrix.numCols();
	    
	    FloatMatrix& result = *(new FloatMatrix(heighta,widtha));
	    int n1, n2;
	    for (n2 = 0; n2 < heighta; n2++) {
		for (n1 = 0; n1 < widtha; n1++) {
		    int k2;
		    
		    //Convolve matrix with cloning template;
		    double fx = 0.0;
		    for (k2 = 0; k2 < heightb; k2++) {
			for (int k1 = 0; k1 < widthb; k1++) {
			    int m2 = n2 - (k2 - 1);
			    int m1 = n1 - (k1 - 1);
			    if ((0 <= m2 && m2 < heighta)) {
				if ((0 <= m1 && m1 < widtha)) {
				    fx += Bmatrix[k2][k1] * Amatrix[m2][m1];
				}
			    }   
			}
		    }
		    result[n2][n1] = fx;
		}
	    }
	    output%0 << result;
	}
    }
}
