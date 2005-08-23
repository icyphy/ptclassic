defstar {
  name { SubCx_M }
  domain { SDF }
  desc { Subtract complex matrix B from A. }
  version { @(#)SDFSubCx_M.pl	1.9 3/7/96 }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  input {
          name { Ainput }
          type { COMPLEX_MATRIX_ENV }
  }
  input {
          name { Binput }
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
          desc { The number of rows in Matrix A and B}
  }
  defstate {
          name { numCols }
          type { int }
          default { 2 }
          desc { The number of columns in Matrix A and B }
  }
  ccinclude { "Matrix.h" }
  go {
    // get inputs
    Envelope Apkt;
    (Ainput%0).getMessage(Apkt);
    const ComplexMatrix& Amatrix = *(const ComplexMatrix *)Apkt.myData();
  
    Envelope Bpkt;
    (Binput%0).getMessage(Bpkt);
    const ComplexMatrix& Bmatrix = *(const ComplexMatrix *)Bpkt.myData();
  
    // check for "null" matrix inputs, caused by delays
    if(Apkt.empty() && Bpkt.empty()) {
      // both empty, return a zero matrix with the given dimensions
      ComplexMatrix& result = *(new ComplexMatrix(int(numRows),int(numCols)));
      result = 0.0;
      output%0 << result;
    }
    else if(Apkt.empty()) {
      // Amatrix is empty but Bmatrix is not, return -Bmatrix
      ComplexMatrix& result = *(new ComplexMatrix(int(numRows),int(numCols)));
      result = -Bmatrix;
      output%0 << result;
    }
    else if(Bpkt.empty()) {
      // Bmatrix is empty but Amatrix is not, return Amatrix
      // NOTE: must resend the Envelope and not an input matrix to keep
      // the reference count mechanism working
      output%0 << Apkt;
    }
    else {
      // Amatrix and Bmatrix both valid

      // just check that A's dimensions match the state info.
      // the operator - on matrices will check that A matches B
      if((Amatrix.numRows() != int(numRows)) ||
         (Amatrix.numCols() != int(numCols))) {
        Error::abortRun(*this,"Dimension size of ComplexMatrix inputs do ",
                              "not match the given state parameters.");
        return;
      }
  
      // do matrix subtraction
      ComplexMatrix& result = *(new ComplexMatrix(int(numRows),int(numCols)));
      result = Amatrix - Bmatrix;
      output%0 << result;
    }
  }
}

