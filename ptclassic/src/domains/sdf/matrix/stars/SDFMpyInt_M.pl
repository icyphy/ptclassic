defstar {
	name { MpyInt_M }
	domain { SDF }
	desc {
Multiply two integer matrices A and B to produce matrix C.
Matrix A has dimensions (numRows,X). Matrix B has dimensions (X,numCols).
Matrix C has dimensions (numRows,numCols). The user need only specify
numRows and numCols. An error will be generated automatically if the
number of columns in A does not match the number of rows in B.
	}
	version { $Id$ }
	author { Mike J. Chen }
	copyright { 1993 The Regents of the University of California }
        location  { SDF matrix library }
	input {
		name { Ainput }
		type { INT_MATRIX_ENV }
	}
	input {
		name { Binput }
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
		desc { The number of rows in Matrix A and Matrix C.}
	}
	defstate {
		name { numCols }
		type { int }
		default { 8 }
		desc { The number of columns in Matrix B and Matrix C}
	}
        ccinclude { "Matrix.h" }
	go {
          // get inputs
          Envelope Apkt;
          (Ainput%0).getMessage(Apkt);
          const IntMatrix& Amatrix = *(const IntMatrix *)Apkt.myData();

          Envelope Bpkt;
          (Binput%0).getMessage(Bpkt);
          const IntMatrix& Bmatrix = *(const IntMatrix *)Bpkt.myData();

          if((Amatrix.numRows() != int(numRows)) ||
             (Bmatrix.numCols() != int(numCols))) {
            Error::abortRun(*this,"Dimension size of IntMatrix inputs do ",
                                  "not match the given state parameters.");
            return;
          }

          // do matrix multiplication
          IntMatrix *result = new IntMatrix(int(numRows),int(numCols));
          // we could write 
          //   *result = Amatrix * Bmatrix;
          // but the following is faster
          multiply(Amatrix,Bmatrix,*result);

          output%0 << *result;
	}
}

