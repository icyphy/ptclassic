defstar {
  name { GainInt_M }
  domain { SDF }
  desc { Multiply an integer matrix by an integer scalar gain value. }
  version { $Id$ }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  input {
    name { input }
    type { INT_MATRIX_ENV }
  }
  output {
    name { output }
    type { INT_MATRIX_ENV }
  }
  defstate {
    name { gain }
    type { int }
    default { "1" }
    desc { Gain to be multiplied with the input matrix }
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
      result = int(gain) * matrix;
      output%0 << result;
    }
  }
}

