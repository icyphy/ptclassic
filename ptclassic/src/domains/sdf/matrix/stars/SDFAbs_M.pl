defstar {
	name { Abs_M }
	domain { SDF }
	desc { 
Return the absolute value of each entry of the floating-point matrix.
        }
	version { @(#)SDFAbs_M.pl	1.2 3/7/96 }
	author { Bilung Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
        location  { SDF matrix library }
	input {
		name { input }
		type { FLOAT_MATRIX_ENV }
	}
	output {
		name { output }
		type { FLOAT_MATRIX_ENV }
	}
	defstate {
		name { numRows }
		type { int }
		default { 2 }
		desc { The number of rows in the input matrices }
	}
	defstate {
		name { numCols }
		type { int }
		default { 2 }
		desc { The number of columns in the input matrices }
	}
        ccinclude { "Matrix.h" }
	go {
          Envelope pkt;
          (input%0).getMessage(pkt);
          const FloatMatrix& matrix = *(const FloatMatrix *)pkt.myData();

          // check for "null" matrix inputs, caused by delays
          if(pkt.empty()) {
            // if input is empty, return a zero matrix with the given dimensions
            FloatMatrix& result = *(new FloatMatrix(int(numRows),int(numCols)));
            result = 0.0;
            output%0 << result;
          }
          else {
            if((matrix.numRows() != int(numRows)) ||
               (matrix.numCols() != int(numCols))) {
              Error::abortRun(*this,"Dimension size of FloatMatrix input does ",
                                    "not match the given state parameters.");
              return;
            }
            FloatMatrix& result = *(new FloatMatrix(int(numRows),int(numCols)));
	    for (int i=0; i<numRows*numCols; i++)
              result.entry(i) = fabs(matrix.entry(i)); // abs the matrix
            output%0 << result;
          }
        }
}


