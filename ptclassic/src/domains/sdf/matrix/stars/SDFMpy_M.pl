defstar {
  name { Mpy_M }
  domain { SDF }
  desc {
Multiply two floating-point matrices A and B to produce matrix C.
Matrix A has dimensions (numRows,X). Matrix B has dimensions (X,numCols).
Matrix C has dimensions (numRows,numCols). The user need only specify
numRows and numCols. An error will be generated automatically if the
number of columns in A does not match the number of rows in B.
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
  	name { Ainput }
  	type { FLOAT_MATRIX_ENV }
  }
  input {
  	name { Binput }
  	type { FLOAT_MATRIX_ENV }
  }
  output {
  	name { output }
  	type { FLOAT_MATRIX_ENV }
  }
  defstate {
  	name { numRows }
  	type { int }
  	default { 2 }
  	desc { The number of rows in Matrix A and Matrix C.}
  }
  defstate {
  	name { numCols }
  	type { int }
  	default { 2 }
  	desc { The number of columns in Matrix B and Matrix C}
  }
  ccinclude { "Matrix.h" }
  go {
    // get inputs
    Envelope Apkt;
    (Ainput%0).getMessage(Apkt);
    const FloatMatrix& Amatrix = *(const FloatMatrix *)Apkt.myData();

    Envelope Bpkt;
    (Binput%0).getMessage(Bpkt);
    const FloatMatrix& Bmatrix = *(const FloatMatrix *)Bpkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(Apkt.empty() || Bpkt.empty()) {
      // if either empty, return a zero matrix with the given dimensions
      FloatMatrix& result = *(new FloatMatrix(int(numRows),int(numCols)));
      result = 0.0;
      output%0 << result;
    }
    else {
      // Amatrix and Bmatrix both valid

      if((Amatrix.numRows() != int(numRows)) ||
         (Bmatrix.numCols() != int(numCols))) {
        Error::abortRun(*this,"Dimension size of FloatMatrix inputs do ",
                              "not match the given state parameters.");
        return;
      }

      // do matrix multiplication
      FloatMatrix& result = *(new FloatMatrix(int(numRows),int(numCols)));
      // we could write 
      //   result = Amatrix * Bmatrix;
      // but the following is faster
      multiply(Amatrix,Bmatrix,result);

      output%0 << result;
    }
  }
}
