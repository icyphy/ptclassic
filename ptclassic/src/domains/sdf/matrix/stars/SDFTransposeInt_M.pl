defstar {
  name { TransposeInt_M }
  domain { SDF }
  desc { Transpose an integer matrix read as a single particle. }
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
    name { numRows }
    type { int }
    default { 2 }
    desc { The number of rows in the input matrix. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 2 }
    desc { The number of columns in the input matrix.  }
  }
  ccinclude { "Matrix.h" }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const IntMatrix& matrix = *(const IntMatrix *)pkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(pkt.empty()) {
      // input empty, send out a zero matrix with the dimensions transposed
      IntMatrix& result = *(new IntMatrix(int(numCols),int(numRows)));
      result = 0;
      output%0 << result;
    }
    else {
      // valid input matrix

      if((matrix.numRows() != int(numRows)) ||
         (matrix.numCols() != int(numCols))) {
        Error::abortRun(*this,"Dimension size of IntMatrix input does ",
                              "not match the given state parameters.");
        return;
      }
      IntMatrix& result = *(new IntMatrix(int(numCols),int(numRows)));
      result = ~matrix;           // equivalent to matrix.transpose()
      output%0 << result;
    }
  }
}
