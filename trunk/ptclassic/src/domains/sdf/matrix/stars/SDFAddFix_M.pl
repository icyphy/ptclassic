defstar {
	name { AddFix_M }
	domain { SDF }
	desc {
Does a matrix addition of two input Fix Matricies A and B to 
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
		type { FIX_MATRIX_ENV }
	}
	input {
		name { Binput }
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
          const FixMatrix& Amatrix = *(const FixMatrix *)Apkt.myData();

          Envelope Bpkt;
          (Binput%0).getMessage(Bpkt);
          const FixMatrix& Bmatrix = *(const FixMatrix *)Bpkt.myData();

          // just check that A's dimensions match the state info.
          // the operator + on matricies will check that A matches B
          if((Amatrix.numRows() != int(numRows)) ||
             (Amatrix.numCols() != int(numCols))) {
            Error::abortRun(*this,"Dimension size of FixMatrix inputs do ",
                                  "not match the given state parameters.");
            return;
          }

          // do matrix addition
          FixMatrix *result = new FixMatrix(int(numRows),int(numCols));
          *result = Amatrix + Bmatrix;
          output%0 << *result;
	}
}

