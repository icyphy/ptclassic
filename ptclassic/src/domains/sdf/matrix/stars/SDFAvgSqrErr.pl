defstar {
  name { AvgSqrErr }
  domain { SDF }
  desc { 
Find the average squared error between two input sequences of matrices. 
  }
  version { @(#)SDFAvgSqrErr.pl	1.6 12/14/97 }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  input {
    name { input1 }
    type { FLOAT_MATRIX_ENV }
  }
  input {
    name { input2 }
    type { FLOAT_MATRIX_ENV }
  }
  output {
    name { output }
    type { float }
  }
  defstate {
    name { numRows }
    type { int }
    default { 1 }
    desc { The number of rows for input matrix. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 2 }
    desc { The number of columns for input matrix. }
  }
  defstate {
    name { numInputs }
    type { int }
    default { 8 }
    desc { The number of input matrices to average. }
  }
  ccinclude { "Matrix.h" }
  setup {
    input1.setSDFParams(int(numInputs),int(numInputs)-1);
    input2.setSDFParams(int(numInputs),int(numInputs)-1);
  }
  go {
    Envelope inpkt1;
    Envelope inpkt2;
    FloatMatrix& tempmatrix = *(new FloatMatrix(int(numRows), int(numCols)));
    double sqrErr = 0.0;
    int maxi = int(numRows) * int(numCols);
    int i;
    for (i = 0; i < int(numInputs); i++) {
      // get input
      (input1%(int(numInputs)-1-i)).getMessage(inpkt1);
      (input2%(int(numInputs)-1-i)).getMessage(inpkt2);
 
      // check for "null" matrix inputs, caused by delays
      // if input1 is empty, treat it as a zero matrix
      const FloatMatrix& matrix1 = *(const FloatMatrix *)inpkt1.myData();
      int empty1 = inpkt1.empty();
      if ( ! empty1 ) {
        if ((matrix1.numRows() != int(numRows)) ||
            (matrix1.numCols() != int(numCols))) {
	  delete &tempmatrix;
          Error::abortRun(*this,
		  "Input1 matrix doesn't match the specified dimension");
          return;
        }
      }

      // if input2 is empty, treat it as a zero matrix
      const FloatMatrix& matrix2 = *(const FloatMatrix *)inpkt2.myData();
      int empty2 = inpkt2.empty();
      if ( ! empty2 ) {
        if ((matrix2.numRows() != int(numRows)) ||
            (matrix2.numCols() != int(numCols))) {
	  delete &tempmatrix;
          Error::abortRun(*this,
			"Input2 matrix doesn't match the specified dimension");
          return;
        }
      }

      // If at least one matrix is not empty, then adjust squared error
      if ( !empty1 || !empty2 ) {
        if ( empty1 && !empty2 ) {
          for (i = 0; i < maxi; i++) {
	    sqrErr += matrix2.entry(i) * matrix2.entry(i);
	  }
        }
        else if ( !empty1 && empty2 ) {
          for (i = 0; i < maxi; i++) {
	    sqrErr += matrix1.entry(i) * matrix1.entry(i);
	  }
        }
        else {
          tempmatrix = matrix1;
	  tempmatrix -= matrix2;
          for (i = 0; i < maxi; i++) {
	    sqrErr += tempmatrix.entry(i) * tempmatrix.entry(i);
	  }
        }
      }
    }
    delete &tempmatrix;
    sqrErr /= int(numInputs);
    output%0 << sqrErr;
  }    // end of go method
}
