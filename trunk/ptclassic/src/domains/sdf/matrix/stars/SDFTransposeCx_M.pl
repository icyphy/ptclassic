defstar {
  name { TransposeCx_M }
  domain { SDF }
  desc { Transpose a complex matrix read as a single particle. }
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
      // input empty, send out a zero matrix with the dimensions transposed
      ComplexMatrix& result = *(new ComplexMatrix(int(numCols),int(numRows)));
      result = Complex(0.0,0.0);
      output%0 << result;
    }
    else {
      // valid input matrix

      if((matrix.numRows() != int(numRows)) ||
         (matrix.numCols() != int(numCols))) {
        Error::abortRun(*this,"Dimension size of ComplexMatrix input does ",
                              "not match the given state parameters.");
        return;
      }
      ComplexMatrix& result = *(new ComplexMatrix(int(numCols),int(numRows)));
      result = ~matrix;           // equivalent to matrix.transpose()
      output%0 << result;
    }
  }
}
