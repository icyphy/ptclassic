defstar {
  name { Hermitian_M }
  domain { SDF }
  desc {
Does a Hermitian Transpose (conjugate transpose) on the input matrix,
using the Matrix class function.
The input matrix has dimensions (numRows,numCols).
The output matrix has dimensions (numCols,numRows).
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
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
    const ComplexMatrix *matrix = (const ComplexMatrix *)pkt.myData();
    if((matrix->numRows() != int(numRows)) ||
       (matrix->numCols() != int(numCols))) {
      Error::abortRun(*this,"Dimension size of ComplexMatrix input does ",
                            "not match the given state parameters.");
      return;
    }
    ComplexMatrix *result = new ComplexMatrix(int(numCols),int(numRows));
    *result = matrix->hermitian();
    output%0 << *result;
  }
}
