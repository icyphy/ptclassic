defstar {
  name { Conjugate_M }
  domain { SDF }
  desc { Conjugate a matrix. }
  version { @(#)SDFConjugate_M.pl	1.8 3/7/96 }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1993-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  input {
    name { input }
    type { COMPLEX_MATRIX_ENV }
  }
  output {
    name { output }
    type { COMPLEX_MATRIX_ENV }
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
    const ComplexMatrix& matrix = *(const ComplexMatrix *)pkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(pkt.empty()) {
      // if input is empty, return a zero matrix with the given dimensions
      ComplexMatrix& result = *(new ComplexMatrix(int(numRows),int(numCols)));
      result = 0.0;
      output%0 << result;
    }
    else {
      if((matrix.numRows() != int(numRows)) ||
         (matrix.numCols() != int(numCols))) {
        Error::abortRun(*this,"Dimension size of ComplexMatrix input does ",
                              "not match the given state parameters.");
        return;
      }
      ComplexMatrix& result = *(new ComplexMatrix(int(numCols),int(numRows)));
      result = matrix.conjugate();
      output%0 << result;
    }
  }
}
