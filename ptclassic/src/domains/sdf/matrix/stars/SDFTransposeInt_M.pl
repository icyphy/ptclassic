defstar {
  name { TransposeInt_M }
  domain { SDF }
  desc { Transpose an integer matrix read as a single particle. }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
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
    default { 8 }
    desc { The number of rows in the input matrix. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 8 }
    desc { The number of columns in the input matrix.  }
  }
  ccinclude { "Matrix.h" }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const IntMatrix& matrix = *(const IntMatrix *)pkt.myData();
    if((matrix.numRows() != int(numRows)) ||
       (matrix.numCols() != int(numCols))) {
      Error::abortRun(*this,"Dimension size of IntMatrix input does ",
                            "not match the given state parameters.");
      return;
    }
    IntMatrix *result = new IntMatrix(int(numCols),int(numRows));
    *result = ~matrix;           // equivalent to matrix.transpose()
    output%0 << *result;
  }
}
