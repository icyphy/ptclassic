defstar {
  name { MpyCx_M }
  domain { SDF }
  desc {
Multiply two complex matrices A and B to produce matrix C.
Matrix A has dimensions (numRows,X). Matrix B has dimensions (X,numCols).
Matrix C has dimensions (numRows,numCols). The user need only specify
numRows and numCols. An error will be generated automatically if the
number of columns in A does not match the number of rows in B.
  }
  version { @(#)SDFMpyCx_M.pl	1.7 10/6/95 }
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
    const ComplexMatrix& Amatrix = *(const ComplexMatrix *)Apkt.myData();

    Envelope Bpkt;
    (Binput%0).getMessage(Bpkt);
    const ComplexMatrix& Bmatrix = *(const ComplexMatrix *)Bpkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(Apkt.empty() || Bpkt.empty()) {
      // if either empty, return a zero matrix with the given dimensions
      ComplexMatrix& result = *(new ComplexMatrix(int(numRows),int(numCols)));
      result = 0.0;
      output%0 << result;
    }
    else {
      // Amatrix and Bmatrix both valid

      if((Amatrix.numRows() != int(numRows)) ||
         (Bmatrix.numCols() != int(numCols))) {
        Error::abortRun(*this,"Dimension size of ComplexMatrix inputs do ",
                              "not match the given state parameters.");
        return;
      }

      // do matrix multiplication
      ComplexMatrix& result = *(new ComplexMatrix(int(numRows),int(numCols)));
      // we could write 
      //   result = Amatrix * Bmatrix;
      // but the following is faster
      multiply(Amatrix,Bmatrix,result);

      output%0 << result;
    }
  }
}

