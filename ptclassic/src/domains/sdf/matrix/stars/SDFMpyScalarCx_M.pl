defstar {
  name { MpyScalarCx_M }
  domain { SDF }
  desc { Multiply a complex input matrix by a scalar input gain value. }
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
    type { COMPLEX_MATRIX_ENV }
  }
  input {
    name { gain }
    type { complex }
    desc { Input gain to be multiplied with the input matrix }
  }
  output {
    name { output }
    type { COMPLEX_MATRIX_ENV }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const ComplexMatrix& matrix = *(const ComplexMatrix *)inpkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(inpkt.empty()) {
      // input empty, just send it back out
      output%0 << inpkt;
    }
    else {
      // valid input matrix

      // do scalar * matrix
      ComplexMatrix& result = *(new ComplexMatrix(matrix.numRows(),matrix.numCols()));
      result = Complex(gain%0) * matrix;
      output%0 << result;
    }
  }
}

