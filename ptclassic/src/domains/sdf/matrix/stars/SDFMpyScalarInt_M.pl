defstar {
  name { MpyScalarInt_M }
  domain { SDF }
  desc { Multiply a integer input matrix by a scalar input gain value. }
  version { $Id$ }
  author { Bilung Lee }
  copyright { 1993 The Regents of the University of California }
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

