defstar {
  name { InverseInt_M }
  domain { SDF }
  desc {
Invert a square integer matrix, producing an integer matrix.
Note: If A is the input Matrix and B is the inverse, then AB
will not necessarily be the identity matrix unless A has an
inverse that can be represented as an integer matrix.
Usually this is not the case!
  }
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
    type { Int_MATRIX_ENV }
  }
  output {
    name { output }
    type { Int_MATRIX_ENV }
  }
  defstate {
    name { rowsCols }
    type { int }
    default { 2 }
    desc { The number of rows and columns of the input square matrix. }
  }
  ccinclude { "Matrix.h" }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const IntMatrix& matrix = *(const IntMatrix *)pkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(pkt.empty()) {
      // if input is empty, return a zero matrix with the given dimensions
      IntMatrix& result = *(new IntMatrix(int(rowsCols),int(rowsCols)));
      result = 0;
      output%0 << result;
    }
    else {
      if((matrix.numRows() != int(rowsCols)) ||
         (matrix.numCols() != int(rowsCols))) {
        Error::abortRun(*this,"Dimension size of IntMatrix input does ",
                              "not match the given state parameters.");
        return;
      }
      IntMatrix& result = *(new IntMatrix(int(rowsCols),int(rowsCols)));
      result = !matrix;        // invert the matrix
      output%0 << result;
    }
  }
}


