defstar {
  name { TransposeFix_M }
  domain { SDF }
  desc { Transpose a fixed-point matrix read as a single particle. }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
    name { input }
    type { FIX_MATRIX_ENV }
  }
  output {
    name { output }
    type { FIX_MATRIX_ENV }
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
    const FixMatrix& matrix = *(const FixMatrix *)pkt.myData();
    if((matrix.numRows() != int(numRows)) ||
       (matrix.numCols() != int(numCols))) {
      Error::abortRun(*this,"Dimension size of FixMatrix input does ",
                            "not match the given state parameters.");
      return;
    }
    FixMatrix *result = new FixMatrix(int(numCols),int(numRows));
    *result = ~matrix;             // equivalent to matrix.transpose()
    output%0 << *result;
  }
}
