defstar {
  name { SampleMean }
  domain { SDF }
  desc { Find the average amplitude of the components of the input matrix. }
  version { $Id$ }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
    type { float }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const FloatMatrix& matrix = *(const FloatMatrix *)inpkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(inpkt.empty()) {
      // input empty, just send a zero out
      output%0 << 0;
    }
    else {
      // valid input matrix
      int dimension = matrix.numRows()*matrix.numCols();
      double result = 0;
      for (int i=0; i<dimension; i++) 
	result += matrix.entry(i);
      result /= dimension;
      output%0 << result;
    }
  }
}

