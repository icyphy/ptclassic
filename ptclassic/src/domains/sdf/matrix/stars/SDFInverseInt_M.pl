defstar {
  name { InverseInt_M }
  domain { SDF }
  desc {
Invert a square matrix using the Matrix class function.
The input matrix has dimensions (rowsCols,rowsCols).
The output matrix has dimensions (rowsCols,rowsCols).
NOTE: If A is the input Matrix and !A is the inverse, then
      A * !A will not necessarily be the identity matrix unless
      A has an inverse which can be represented by as an IntMatrix.
      Many times this is not the case!
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
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
    default { 8 }
    desc { The number of rows/columns of the input square matrix. }
  }
  ccinclude { "Matrix.h" }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const IntMatrix& matrix = *(const IntMatrix *)pkt.myData();

    if((matrix.numRows() != int(rowsCols)) ||
       (matrix.numCols() != int(rowsCols))) {
      Error::abortRun(*this,"Dimension size of IntMatrix input does ",
                            "not match the given state parameters.");
      return;
    }
    IntMatrix *result = new IntMatrix(int(rowsCols),int(rowsCols));
    *result = !matrix;        // invert the matrix

    output%0 << *result;
  }
}


