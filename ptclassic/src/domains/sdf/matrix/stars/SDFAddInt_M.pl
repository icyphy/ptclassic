defstar {
	name { AddInt_M }
	domain { SDF }
	desc {
Does a matrix addition of two input Int Matricies A and B to 
produce matrix C.
Matrix A has dimensions (numRows,numCols).
Matrix B has dimensions (numRows,numCols).
Matrix C has dimensions (numRows,numCols).
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
		desc { The number of rows in Matrix A and B}
	}
	defstate {
		name { numCols }
		type { int }
		default { 8 }
		desc { The number of columns in Matrix A and B }
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

          // just check that A's dimensions match the state info.
          // the operator + on matricies will check that A matches B
          if((Amatrix.numRows() != int(numRows)) ||
             (Amatrix.numCols() != int(numCols))) {
            Error::abortRun(*this,"Dimension size of IntMatrix inputs do ",
                                  "not match the given state parameters.");
            return;
          }

          // do matrix addition
          IntMatrix *result = new IntMatrix(int(numRows),int(numCols));
          *result = Amatrix + Bmatrix;
          output%0 << *result;
	}
}

