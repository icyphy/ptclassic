defstar {
  name { InverseFix_M }
  domain { SDF }
  desc { Invert a square fixed-point matrix. }
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
    name { rowsCols }
    type { int }
    default { 2 }
    desc { The number of rows/columns of the input square matrix. }
  }
  ccinclude { "Matrix.h" }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const FixMatrix& matrix = *(const FixMatrix *)pkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(pkt.empty()) {
      // if input is empty, return a zero matrix with the given dimensions
      FixMatrix& result = *(new FixMatrix(int(rowsCols),int(rowsCols)));
      result = 0.0;
      output%0 << result;
    }
    else {
      if((matrix.numRows() != int(rowsCols)) ||
         (matrix.numCols() != int(rowsCols))) {
        Error::abortRun(*this,"Dimension size of FixMatrix input does ",
                              "not match the given state parameters.");
        return;
      }
      FixMatrix& result = *(new FixMatrix(int(rowsCols),int(rowsCols)));
      result = !matrix;         // invert the matrix
      output%0 << result;
    }
  }
}


