defstar {
  name { MpyScalarInt_M }
  domain { SDF }
  desc { Multiply a integer input matrix by a scalar input gain value. }
  version { @(#)SDFMpyScalarInt_M.pl	1.3 10/6/95 }
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
    type { INT_MATRIX_ENV }
  }
  input {
    name { gain }
    type { int }
    desc { Input gain to be multiplied with the input matrix }
  }
  output {
    name { output }
    type { INT_MATRIX_ENV }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const IntMatrix& matrix = *(const IntMatrix *)inpkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(inpkt.empty()) {
      // input empty, just send it back out
      output%0 << inpkt;
    }
    else {
      // valid input matrix

      // do scalar * matrix
      IntMatrix& result = *(new IntMatrix(matrix.numRows(),matrix.numCols()));
      result = int(gain%0) * matrix;
      output%0 << result;
    }
  }
}

